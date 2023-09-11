## 原作者：liukun321 咕唧咕唧

版权声明：本文为博主原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接和本声明。
本文链接：https://blog.csdn.net/liukun321/article/details/8558425
这里只是搬运方便记录更新

# 编译u-boot
```shell
make distclean
make ARCH=arm CROSS_COMPILE=/opt/FriendlyARM/toolschain/4.5.1/bin/arm-none-linux-gnueabi- tiny210_config
make ARCH=arm CROSS_COMPILE=/opt/FriendlyARM/toolschain/4.5.1/bin/arm-none-linux-gnueabi- all spl
```
由于我的系统下装有两套交叉工具链，所以没有把 /opt/FriendlyARM/toolschain/4.5.1/bin/ 添加到环境变量，在使用工具链时要指明路径。

## 1.sd启动
 
将u-boot镜像写入SD卡
将SD卡通过读卡器接上电脑（或直接插入笔记本卡槽），通过"cat /proc/partitions"找出SD卡对应的设备，我的设备节点是/dev/sdb.

执行下面的命令
```shell
sudo dd iflag=dsync oflag=dsync if=tiny210-uboot.bin of=/dev/sdb seek=1
```
## 2.nand启动
通过SD卡启动的u-boot for tiny210 将u-boot镜像写入nandflash
开发板终端下执行下面的命令
```shell
[FriendlyLEG-TINY210]# tftp 21000000 tiny210-uboot.bin
[FriendlyLEG-TINY210]# nand erase.chip
[FriendlyLEG-TINY210]# nand write 21000000 0 3c1f4 
```

