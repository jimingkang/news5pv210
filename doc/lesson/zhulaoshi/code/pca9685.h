#ifndef __MPU6050_H_
#define __MPU6050_H_
//定义MPU6050硬件地址
#define PCA9685_ADDRESS 	0X40//接地为0X68 接高电平为0X69

//定义PCA9685寄存器地址
#define MODE1   0x00
#define MODE2   0x01
#define SUBADR1   0x02
#define SUBADR2   0x03
#define SUBADR3   0x04
#define PRESCALE   0xFE
#define LED0_ON_L   0x06
#define LED0_ON_H   0x07
#define LED0_OFF_L   0x08
#define LED0_OFF_H   0x09
#define ALL_LED_ON_L   0xFA
#define ALL_LED_ON_H   0xFB
#define ALL_LED_OFF_L   0xFC
#define ALL_LED_OFF_H   0xFD
 
#define STP_CHA_L   2047
#define STP_CHA_H   4095
 
#define STP_CHB_L   1
#define STP_CHB_H   2047
 
#define STP_CHC_L   1023
#define STP_CHC_H   3071
 
#define STP_CHD_L   3071
#define STP_CHD_H   1023

#endif
