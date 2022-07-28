#include "string.h"
#include "arp.h"

u8 sendbuf[1024];
#define TFTP_START_ADDR 0x20008000
u16 curblock = 1;

#define HON(n) ((((u16)((n) & 0xff)) << 8) | (((n) & 0xff00) >> 8))

u16 checksum(u8 *ptr, int len)
{
    u32 sum = 0;
    u16 *p = (u16 *)ptr;

    while (len > 1)
    {
        sum += *p++;
        len -= 2;
    }
    
    if(len == 1)
        sum += *(u8 *)p;
    
    while(sum>>16)
        sum = (sum&0xffff) + (sum>>16);
    
    return (u16)((~sum)&0xffff);
}

void tftp_send_request(const char *filename)
{
    u8 *ptftp = &sendbuf[200];
    u32 tftp_len = 0;
    UDP_HDR *udphdr;
    u8 *iphdr;
    
    ptftp[0] = 0x00;
    ptftp[1] = 0x01;
    tftp_len += 2 ;
    
    sprintf(&ptftp[tftp_len],"%s",filename);
    tftp_len += strlen(filename);
    ptftp[tftp_len] = '\0';
    tftp_len += 1;
    
    sprintf(&ptftp[tftp_len],"%s","octet");
    tftp_len += strlen("octet");
    ptftp[tftp_len] = '\0';
    tftp_len += 1;
    
    
    
    udphdr = (UDP_HDR *)(ptftp-sizeof(UDP_HDR));
    iphdr =  (u8*)(ptftp-sizeof(UDP_HDR)+ sizeof(ETH_HDR));
    
    /*UDP帧头信息*/
    udphdr->sport = HON(48915);
    udphdr->dport = HON(69);
    udphdr->len = HON(tftp_len+sizeof(UDP_HDR)-sizeof(IP_HDR));
    udphdr->udpchksum = 0x00;
    
    /*IP帧头信息*/
    udphdr->iphdr.vhl = 0x45;
    udphdr->iphdr.tos = 0x00;
    udphdr->iphdr.len = HON(tftp_len+sizeof(UDP_HDR)-sizeof(ETH_HDR));
    udphdr->iphdr.ipid = HON(0x00);
    udphdr->iphdr.ipoffset = HON(0x4000);
    udphdr->iphdr.ttl = 0xff;
    udphdr->iphdr.proto = 17;
    memcpy(udphdr->iphdr.srcipaddr,ip_addr,4);
    memcpy(udphdr->iphdr.destipaddr,host_ip_addr,4);
    udphdr->iphdr.ipchksum = 0;
    udphdr->iphdr.ipchksum = checksum(iphdr,20);
    
    /*以太网帧头信息*/
    memcpy(udphdr->iphdr.ethhdr.s_mac,mac_addr,6);
    memcpy(udphdr->iphdr.ethhdr.d_mac,host_mac_addr,6);
    udphdr->iphdr.ethhdr.type = HON(PROTO_IP);
    
    dm9000_tx((u32 *)udphdr,sizeof(UDP_HDR)+tftp_len);
}

void tftp_send_ack(u16 blocknum,u16 serverport)
{
    u8 *ptftp = &sendbuf[200];
    u32 tftp_len = 0;
    UDP_HDR *udphdr;
    u8 *iphdr;
    
    ptftp[0] = 0x00;
    ptftp[1] = 0x04;
    tftp_len += 2 ;
    
    ptftp[2] = (blocknum&0xff00)>>8;
    ptftp[3] = (blocknum&0xff);
    tftp_len += 2 ;
    
    udphdr = (UDP_HDR*)(ptftp-sizeof(UDP_HDR));
    iphdr =  (u8*)(ptftp-sizeof(UDP_HDR)+ sizeof(ETH_HDR));
    
    /*UDP帧头信息*/
    udphdr->sport = HON(48915);
    udphdr->dport = serverport;  
    udphdr->len = HON(tftp_len+sizeof(UDP_HDR)-sizeof(IP_HDR));
    udphdr->udpchksum = 0x00;
    
    /*IP帧头信息*/
    udphdr->iphdr.vhl = 0x45;
    udphdr->iphdr.tos = 0x00;
    udphdr->iphdr.len = HON(tftp_len+sizeof(UDP_HDR)-sizeof(ETH_HDR));
    udphdr->iphdr.ipid = HON(0x00);
    udphdr->iphdr.ipoffset = HON(0x4000);
    udphdr->iphdr.ttl = 0xff;
    udphdr->iphdr.proto = 17;
    memcpy(udphdr->iphdr.srcipaddr,ip_addr,4);
    memcpy(udphdr->iphdr.destipaddr,host_ip_addr,4);
    udphdr->iphdr.ipchksum = 0;
    udphdr->iphdr.ipchksum = checksum(iphdr,20);
    
     /*以太网帧头信息*/
    memcpy(udphdr->iphdr.ethhdr.s_mac,mac_addr,6);
    memcpy(udphdr->iphdr.ethhdr.d_mac,host_mac_addr,6);
    udphdr->iphdr.ethhdr.type = HON(PROTO_IP);
    
    dm9000_tx((u8 *)udphdr,sizeof(UDP_HDR)+tftp_len);
}


void tftp_process(u8 *buf, u16 len, u16 port)
{
     u32 i;
     u32 tftp_len;
     u8* tftp_download_addr = (u8*)TFTP_START_ADDR;
//     serverport = port;
     
     TFTP_PAK *ptftp = (TFTP_PAK*)(buf + sizeof(UDP_HDR));
     
     tftp_len = len - sizeof(UDP_HDR);

     if(HON(ptftp->opcode) == 3)
     {
		/**
		*由于服务器端(pc)处理tftp包速度响应快，重复多次包，客户端才响应
		*为避免重发包多次响应，引入变量curblock
		**/	     	
     	 if (HON(ptftp->blocknum) == curblock)
     	 {
     	 				tftp_download_addr = (u8 *)(TFTP_START_ADDR+(curblock-1)*512);
     	 	
              for (i = 0;i<(tftp_len-4);i++)
              {
                  *(tftp_download_addr++) = *(ptftp->data+i);
              }	
         
              tftp_send_ack(HON(ptftp->blocknum),port);
              
              curblock++;
         
         	  /*数据包小于512byte，说明为最后一个包*/	     
              if ((tftp_len-4)<512)
              	{
                  curblock = 1;
                  printf("\rDownload Completely!\r\n");
                }
         }
    }
}
