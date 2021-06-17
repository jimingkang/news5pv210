#!/bin/sh
sudo dd iflag=dsync oflag=dsync if=210.bin of=/dev/mmcblk0 seek=1
