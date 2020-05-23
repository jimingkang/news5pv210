# start.S 文件的拷贝
cp /root/winshare/s5pv210/uboot/u-boot-2013.10/arch/arm/cpu/armv7/start.S ./arch/arm/cpu/armv7/ -f

# crt0.S
cp /root/winshare/s5pv210/uboot/u-boot-2013.10/arch/arm/lib/crt0.S ./arch/arm/lib/ -f

# arch/arm/lib/board.c
cp /root/winshare/s5pv210/uboot/u-boot-2013.10/arch/arm/lib/board.c ./arch/arm/lib/ -f


# board/samsung/goni/lowlevel_init.S
#cp /root/winshare/s5pv210/uboot/u-boot-2013.10/board/samsung/goni/lowlevel_init.S ./board/samsung/goni/ -f

# u-boot.lds
cp /root/winshare/s5pv210/uboot/u-boot-2013.10/arch/arm/cpu/u-boot.lds ./arch/arm/cpu/ -f

# board/samsung/goni/Makefile
#cp /root/winshare/s5pv210/uboot/u-boot-2013.10/board/samsung/goni/Makefile ./board/samsung/goni/ -f

# board/samsung/goni
cp /root/winshare/s5pv210/uboot/u-boot-2013.10/board/samsung/goni ./board/samsung/ -rf

# include/s5pc110.h
cp /root/winshare/s5pv210/uboot/u-boot-2013.10/include/s5pc110.h ./include/ -f

# include/confits/s5p_goni.h
cp /root/winshare/s5pv210/uboot/u-boot-2013.10/include/configs/s5p_goni.h ./include/configs/ -f

cp /root/winshare/s5pv210/uboot/u-boot-2013.10/include/movi.h ./include/ -f

cp /root/winshare/s5pv210/uboot/u-boot-2013.10/arch/arm/cpu/armv7/s5p-common/cpu_info.c ./arch/arm/cpu/armv7/s5p-common/ -f

cp /root/winshare/s5pv210/uboot/u-boot-2013.10/arch/arm/include/asm/arch-s5pc1xx/cpu.h ./arch/arm/include/asm/arch-s5pc1xx/ -f

cp /root/winshare/s5pv210/uboot/u-boot-2013.10/arch/arm/cpu/armv7/s5pc1xx/clock.c ./arch/arm/cpu/armv7/s5pc1xx/ -f

# sd/mmc driver
cp /root/winshare/s5pv210/uboot/u-boot-2013.10/drivers/mmc/mmc.c ./drivers/mmc -f
cp /root/winshare/s5pv210/uboot/u-boot-2013.10/drivers/mmc/s3c_hsmmc.c ./drivers/mmc -f
cp /root/winshare/s5pv210/uboot/u-boot-2013.10/include/mmc.h ./include/ -f
cp /root/winshare/s5pv210/uboot/u-boot-2013.10/include/s3c_hsmmc.h ./include/ -f
cp /root/winshare/s5pv210/uboot/u-boot-2013.10/common/cmd_mmc.c ./common/ -f
cp /root/winshare/s5pv210/uboot/u-boot-2013.10/drivers/mmc/Makefile ./drivers/mmc -f

// network
cp /root/winshare/s5pv210/uboot/u-boot-2013.10/net/eth.c ./net/ -f


#echo $#
if [$# -eq 0]; then
make distclean
make s5p_goni_config
fi

make

