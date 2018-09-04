#include <main.h>
#include <math.h>
#include <stdio.h>
#include <graphic/color.h>
#include <graphic/surface.h>
#include <qrencode.h>
#include "split.h"

static int casesensitive = 1;
static int eightbit = 0;
static int version = 1;
static int size = 4;
static int margin = 4;
static int structured = 0;
static int micro = 0;
static QRecLevel level = QR_ECLEVEL_L;
static QRencodeMode hint = QR_MODE_8;

static void draw_QRcode(QRcode *qrcode, struct surface_t * screen, int ox, int oy)
{
	int x, y, width;
	unsigned char *p;
	struct rect_t rect;
	int size = 4;

	ox += margin * size;
	oy += margin * size;
	width = qrcode->width;
	p = qrcode->data;
	for (y = 0; y < width; y++)
	{
		for (x = 0; x < width; x++)
		{
			rect.x = ox + x * size;
			rect.y = oy + y * size;
			rect.w = size;
			rect.h = size;

			surface_fill(screen, &rect, (*p & 1) ? 0 : 0xffffff, BLEND_MODE_REPLACE);
			p++;
		}
	}
}

void draw_singleQRcode(QRinput *stream, int mask, struct surface_t * screen)
{
	u32_t c;
	QRcode *qrcode;
	int width;

	QRinput_setVersionAndErrorCorrectionLevel(stream, version, level);
	if(micro) {
		qrcode = QRcode_encodeMaskMQR(stream, mask);
	} else {
		qrcode = QRcode_encodeMask(stream, mask);
	}
	if(qrcode == NULL) {
		width = (11 + margin * 2) * size;
	} else {
		version = qrcode->version;
		width = (qrcode->width + margin * 2) * size;
	}

	c = surface_map_color(screen, get_named_color("white"));
	surface_fill(screen, &screen->clip, c, BLEND_MODE_REPLACE);
	if(qrcode) {
		draw_QRcode(qrcode, screen, 0, 0);
	}
	QRcode_free(qrcode);
}

void view_simple(unsigned char *str, int length, struct surface_t * screen)
{
	QRinput *input;
	int ret;

	if(micro) {
		input = QRinput_newMQR(version, level);
	} else {
		input = QRinput_new2(version, level);
	}
	if(input == NULL) {
	}
	if(eightbit) {
		ret = QRinput_append(input, QR_MODE_8, length, str);
	} else {
		ret = Split_splitStringToQRinput((char *)str, input, hint, casesensitive);
	}
	if(ret < 0) {
	}

	draw_singleQRcode(input, -1, screen);

	QRinput_free(input);
}

int tester_framebuffer_qrencode(int argc, char * argv[])
{
	char buf[256];
	static int i = 0;
	struct surface_t * screen = s5pv210_screen_surface();

	while(1)
	{
		sprintf(buf, "copyright to 9tripod.com. every 5s will refresh. test val = %d", i++);
		view_simple((unsigned char *)buf, strlen(buf), screen);

		mdelay(5000);
	}
	return 0;
}
