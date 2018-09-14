#!/bin/sh
#
# Description		: Build Android Script.
# Authors		: http://www.9tripod.com
# Version		: 0.01
# Notes			: None
#
QT_KERNEL_CONFIG=x210_jffs_defconfig

CPU_NUM=$(cat /proc/cpuinfo |grep processor|wc -l)
CPU_NUM=$((CPU_NUM+1))

# make distclean
make ${QT_KERNEL_CONFIG} || return 1
make -j${CPU_NUM} || return 1

cp arch/arm/boot/zImage ~lqm/tftpboot

echo "" >&2
echo "^_^ qt kernel path: arch/arm/boot/zImage" >&2
