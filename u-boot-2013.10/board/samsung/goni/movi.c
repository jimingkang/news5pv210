#include <common.h>
#include <s5pc110.h>

#include <movi.h>
#include <asm/io.h>
//#include <regs.h>
#include <mmc.h>




typedef u32(*copy_sd_mmc_to_mem)
(u32 channel, u32 start_block, u16 block_size, u32 *trg, u32 init);

void movi_bl2_copy(void)
{
	ulong ch;

	ch = *(volatile u32 *)(0xD0037488);
	copy_sd_mmc_to_mem copy_bl2 =
	    (copy_sd_mmc_to_mem) (*(u32 *) (0xD0037F98));

	u32 ret;
	if (ch == 0xEB000000) {
		ret = copy_bl2(0, MOVI_BL2_POS, MOVI_BL2_BLKCNT,
			CFG_PHY_UBOOT_BASE, 0);
	}
	else if (ch == 0xEB200000) {
		ret = copy_bl2(2, MOVI_BL2_POS, MOVI_BL2_BLKCNT,
			CFG_PHY_UBOOT_BASE, 0);
	}
	else
		return;

	if (ret == 0)
		while (1)
			;
	else
		return;
}




