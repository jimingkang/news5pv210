#include <main.h>
#include <gui.h>

int randomInt(int min, int max)
{
	return min + rand() % (max - min + 1);
}

int tester_framebuffer_gui(int argc, char * argv[])
{
	u32_t x, y, w, h, r;
	u32_t x1, y1;
	u32_t stangle, endangle;
	u32_t c;

	s5pv210_screen_swap();
	s5pv210_screen_flush();

	while(1)
	{
		x = randomInt(0, GUI_LCM_XMAX - 1);
		y = randomInt(0, GUI_LCM_YMAX - 1);
		w = randomInt(1, GUI_LCM_XMAX);
		h = randomInt(1, GUI_LCM_YMAX);
		c = randomInt(0, 0x00ffffff);
		GUI_Rectangle(x, y, w, h, c);
		mdelay(200);

		x = randomInt(100, (GUI_LCM_XMAX - 1) / 2);
		y = randomInt(100, (GUI_LCM_YMAX - 1) / 2);
		r = randomInt(10, 100);
		c = randomInt(0, 0x00ffffff);
		GUI_Circle(x, y, r, c);
		mdelay(200);

		x = randomInt(100, (GUI_LCM_XMAX - 1) / 2);
		y = randomInt(100, (GUI_LCM_YMAX - 1) / 2);
		r = randomInt(10, 100);
		c = randomInt(0, 0x00ffffff);
		GUI_CircleFill(x, y, r, c);
		mdelay(200);

		x = randomInt(100, (GUI_LCM_XMAX - 1) / 2);
		y = randomInt(100, (GUI_LCM_YMAX - 1) / 2);
		x1 = randomInt(100, (GUI_LCM_XMAX - 1) / 2);
		y1 = randomInt(100, (GUI_LCM_YMAX - 1) / 2);
		w = randomInt(10, 30);
		c = randomInt(0, 0x00ffffff);
		GUI_LineWith(x, y, x1, y1, w, c);
		mdelay(200);

		x = randomInt(100, (GUI_LCM_XMAX - 1) / 2);
		y = randomInt(100, (GUI_LCM_YMAX - 1) / 2);
		x1 = randomInt(100, (GUI_LCM_XMAX - 1) / 2);
		y1 = randomInt(100, (GUI_LCM_YMAX - 1) / 2);
		c = randomInt(0, 0x00ffffff);
		GUI_EllipseFill(x, x1, y, y1, c);
		mdelay(200);

		x = randomInt(100, (GUI_LCM_XMAX - 1) / 2);
		y = randomInt(100, (GUI_LCM_YMAX - 1) / 2);
		r = randomInt(10, 50);
		stangle = randomInt(0, 359);
		endangle = randomInt(0, 359);
		c = randomInt(0, 0x00ffffff);
		GUI_Arc(x, x1, r, stangle, endangle, c);
		mdelay(200);
	}

	return 0;
}
