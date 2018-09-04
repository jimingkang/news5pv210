#include <main.h>
#include <graphic/color.h>
#include <font.h>

int randomInt(int min, int max)
{
	return min + rand() % (max - min + 1);
}

int tester_framebuffer_font(int argc, char * argv[])
{
	u32_t x, y;
	u32_t fc, bc;
	u32_t count = 0;

	while(1)
	{
		s5pv210_screen_swap();
		surface_fill(s5pv210_screen_surface(), 0, 0, BLEND_MODE_REPLACE);

		x = randomInt(0, 799 - 16);
		y = randomInt(0, 479 - 8);
		fc = randomInt(0, 0x00ffffff);
		bc = (~fc) & 0x00ffffff;

		lcd_print(x, y, fc, bc, "LCD Print [count = %d]", count++);

		s5pv210_screen_flush();
		mdelay(1000);
	}
	return 0;
}
