
/***********************************************
���ƣ����۹�
���ڣ�2012-2-13
˵��������logo
***********************************************/

#ifndef __FBSCALE_H__
#define __FBSCALE_H__

#include <xboot.h>
#include <fb/fb.h>

enum bitmap_scale_method
{
	BITMAP_SCALE_METHOD_FASTEST,
	BITMAP_SCALE_METHOD_BEST,
};

bool_t bitmap_create_scaled(struct bitmap ** dst, u32_t w, u32_t h, struct bitmap * src, enum bitmap_scale_method method);

#endif /* __FBSCALE_H__ */
