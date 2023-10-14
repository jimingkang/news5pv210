/* linux/drivers/media/video/ov2655_2m.h
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *	         http://www.samsung.com/
 *
 * Driver for OV2655 (UXGA camera) from Samsung Electronics
 * 1/4" 2.0Mp CMOS Image Sensor SoC with an Embedded Image Processor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __OV2655_2M_H__
#define __OV2655_2M_H__

struct ov2655_reg {
	unsigned char addr;
	unsigned char val;
};

struct ov2655_regset_type {
	unsigned char *regset;
	int len;
};

/*
 * Macro
 */
#define REGSET_LENGTH(x)	(sizeof(x)/sizeof(ov2655_reg))

/*
 * User defined commands
 */
/* S/W defined features for tune */
#define REG_DELAY	0x0000	/* in ms */
#define REG_CMD		0xFFFF	/* Followed by command */
#define REG_ID		0x8899	/* Followed by command */


/* Following order should not be changed */
enum image_size_ov2655 {
	/* This SoC supports upto UXGA (1600*1200) */
	QQVGA,	/* 160*120 */
	QCIF,	/* 176*144 */
	QVGA,	/* 320*240 */
	CIF,	/* 352*288 */
	VGA,	/* 640*480 */
	SVGA,	/* 800*600 */
	HD720P,	/* 1280*720 */
	SXGA,	/* 1280*1024 */
	UXGA,	/* 1600*1200 */
};

/*
 * Following values describe controls of camera
 * in user aspect and must be match with index of ov2655_regset[]
 * These values indicates each controls and should be used
 * to control each control
 */
enum ov2655_control {
	OV2655_INIT,
	OV2655_EV,
	OV2655_AWB,
	OV2655_MWB,
	OV2655_EFFECT,
	OV2655_CONTRAST,
	OV2655_SATURATION,
	OV2655_SHARPNESS,
};

#define OV2655_REGSET(x)	{	\
	.regset = x,			\
	.len = sizeof(x)/sizeof(ov2655_reg),}


/*
 * User tuned register setting values
 */
static unsigned short ov2655_init_reg[][2] = {
	{0x3012, 0x80},
	
	{0x0000, 0x0a}, //10ms
	
	{0x308c,0x80}, 
	{0x308d,0x0e}, 
	{0x360b,0x00},
	{0x30b0,0xff}, 
	{0x30b1,0xff}, 
	{0x30b2,0x04}, 
	
	{0x300e,0x34},
	{0x300f,0xa6},
	{0x3010,0x81},
	{0x3082,0x01},
	{0x30f4,0x01},  
	{0x3090,0x43}, //add 3.6
	{0x3091,0xc0},
	{0x30ac,0x42},
	
	{0x30d1,0x08},
	{0x30a8,0x55},
	{0x3015,0x02}, 
	{0x3093,0x00},
	{0x307e,0xe5}, 
	{0x3079,0x00},
	{0x30aa,0x42},
	{0x3017,0x40}, 
	{0x30f3,0x83},
	{0x306a,0x0c}, 
	{0x306d,0x00},
	{0x336a,0x3c},
	{0x3076,0x6a}, 
	{0x30d9,0x95},
	{0x3016,0x82}, 
	{0x3601,0x30},
	{0x304e,0x88},
	{0x30f1,0x82},
	
	{0x306f,0x14},  
	{0x3012,0x10},  //resolution
	{0x3011,0x01},			//change 3.6  0x01->0x02

	{0x302a,0x02},
	{0x302b,0xe6},
	{0x3028,0x07},
	{0x3029,0x93},
	{0x3391,0x06}, 
	{0x3394,0x38},
	{0x3395,0x38},
	
	{0x3013,0xf7},  
	{0x3018,0x80}, 
	{0x3019,0x70}, 
	{0x301a,0xd4}, 
	{0x301c,0x07}, 
	{0x301d,0x08}, 
	{0x3070,0x5d}, 
	{0x3072,0x4d},
	
	{0x30af,0x00},
	{0x3048,0x1f},
	{0x3049,0x4e},
	{0x304a,0x20},
	{0x304f,0x20},
	{0x304b,0x02},
	{0x304c,0x00},
	{0x304d,0x02},
	{0x304f,0x20},
	{0x30a3,0x10},
	{0x3014,0x84}, 
	{0x3071,0x00},
	{0x3073,0x00},
	{0x304d,0x42},
	{0x304a,0x40}, 
	{0x304f,0x40}, 
	{0x3095,0x07},
	{0x3096,0x16},
	{0x3097,0x1d},
	{0x3020,0x01}, 
	{0x3021,0x18},
	{0x3022,0x00},
	{0x3023,0x06},
	{0x3024,0x06},
	{0x3025,0x58},
	{0x3026,0x04},
	{0x3027,0xbc},
	{0x3088,0x03},
	{0x3089,0x20},
	{0x308a,0x02},
	{0x308b,0x58},
	{0x3316,0x64},
	{0x3317,0x25},
	{0x3318,0x80},
	{0x3319,0x08},
	{0x331a,0x64},
	{0x331b,0x4b},
	{0x331c,0x00},
	{0x331d,0x38},
	{0x3100,0x00},
	{0x3320,0xfa},   
	{0x3321,0x11},   
	{0x3322,0x92},   
	{0x3323,0x01},   
	{0x3324,0x97},   
	{0x3325,0x02},   
	{0x3326,0xff},   
	{0x3327,0x0c},   
	{0x3328,0x10},   
	{0x3329,0x10},   
	{0x332a,0x58},   
	{0x332b,0x56},  
	{0x332c,0xbe},   
	{0x332d,0xe1},  
	{0x332e,0x3a}, 
	{0x332f,0x36},   
	{0x3330,0x4d},   
	{0x3331,0x44},   
	{0x3332,0xf8},   
	{0x3333,0x0a},   
	{0x3334,0xf0},   
	{0x3335,0xf0},   
	{0x3336,0xf0},   
	{0x3337,0x40},   
	{0x3338,0x40},   
	{0x3339,0x40},   
	{0x333a,0x00},   
	{0x333b,0x00},   
	{0x3380,0x28},
	{0x3381,0x48}, 
	{0x3382,0x10},
	{0x3383,0x22},  
	{0x3384,0xc0},  
	{0x3385,0xe2},  
	{0x3386,0xe2},  
	{0x3387,0xf2},  
	{0x3388,0x10},  
	{0x3389,0x98},
	{0x338a,0x00},  
	{0x3340,0x04},  
	{0x3341,0x07},  
	{0x3342,0x19},
	{0x3343,0x34},
	{0x3344,0x4a},  
	{0x3345,0x5a},  
	{0x3346,0x67},  
	{0x3347,0x71},  
	{0x3348,0x7c},  
	{0x3349,0x8c},
	{0x334a,0x9b},
	{0x334b,0xa9},
	{0x334c,0xc0},
	{0x334d,0xd5},
	{0x334e,0xe8},
	{0x334f,0x20},
	{0x3090,0x03},
	{0x307c,0x10},
	{0x3350,0x33},
	{0x3351,0x28},
	{0x3352,0x00},
	{0x3353,0x16},
	{0x3354,0x00},
	{0x3355,0x85},
	{0x3356,0x35},
	{0x3357,0x28},
	{0x3358,0x00},
	{0x3359,0x13},
	{0x335a,0x00},
	{0x335b,0x85},
	{0x335c,0x34},
	{0x335d,0x28},
	{0x335e,0x00},
	{0x335f,0x13},
	{0x3360,0x00},
	{0x3361,0x85},
	{0x3363,0x70},
	{0x3364,0x7f},
	{0x3365,0x00},
	{0x3366,0x00},
	{0x3362,0x90}, 
	{0x3301,0xff},
	{0x338B,0x11},
	{0x338c,0x10},
	{0x338d,0x40},
	{0x3370,0xd0},
	{0x3371,0x00},
	{0x3372,0x00},
	{0x3373,0x30},
	{0x3374,0x10},
	{0x3375,0x10},
	{0x3376,0x05},
	{0x3377,0x00},
	{0x3378,0x04},
	{0x3379,0x40},
	{0x3069,0x86},  
	{0x3087,0x02},
	{0x3300,0xfc},
	{0x3302,0x11},
	{0x3400,0x02},    // format
	{0x3606,0x20},
	{0x3601,0x30},
	{0x30f3,0x83},
	{0x304e,0x88},
	{0x3015,0x02}, 
	{0x302d,0x00}, 
	{0x302e,0x00},
	{0x3306,0x00},
	{0x363b,0x01},
	{0x363c,0xf2},
	{0x3086,0x0f},
	{0x3086,0x00},
	{0x30a1,0x41},
	{0x30a3,0x80},
	{0x30a8,0x56}, 
	{0x30aa,0x72}, 
	{0x30af,0x10}, 
	{0x30b2,0x2f}, 
	{0x30d9,0x8c},
};

#define OV2655_INIT_REGS	\
	(sizeof(ov2655_init_reg) / sizeof(ov2655_init_reg[0]))

/*
 * EV bias
 */

static const struct ov2655_reg ov2655_ev_m6[] = {
};

static const struct ov2655_reg ov2655_ev_m5[] = {
};

static const struct ov2655_reg ov2655_ev_m4[] = {
};

static const struct ov2655_reg ov2655_ev_m3[] = {
};

static const struct ov2655_reg ov2655_ev_m2[] = {
};

static const struct ov2655_reg ov2655_ev_m1[] = {
};

static const struct ov2655_reg ov2655_ev_default[] = {
};

static const struct ov2655_reg ov2655_ev_p1[] = {
};

static const struct ov2655_reg ov2655_ev_p2[] = {
};

static const struct ov2655_reg ov2655_ev_p3[] = {
};

static const struct ov2655_reg ov2655_ev_p4[] = {
};

static const struct ov2655_reg ov2655_ev_p5[] = {
};

static const struct ov2655_reg ov2655_ev_p6[] = {
};

/* Order of this array should be following the querymenu data */
static const unsigned char *ov2655_regs_ev_bias[] = {
	(unsigned char *)ov2655_ev_m6, (unsigned char *)ov2655_ev_m5,
	(unsigned char *)ov2655_ev_m4, (unsigned char *)ov2655_ev_m3,
	(unsigned char *)ov2655_ev_m2, (unsigned char *)ov2655_ev_m1,
	(unsigned char *)ov2655_ev_default, (unsigned char *)ov2655_ev_p1,
	(unsigned char *)ov2655_ev_p2, (unsigned char *)ov2655_ev_p3,
	(unsigned char *)ov2655_ev_p4, (unsigned char *)ov2655_ev_p5,
	(unsigned char *)ov2655_ev_p6,
};

/*
 * Auto White Balance configure
 */
static const struct ov2655_reg ov2655_awb_off[] = {
};

static const struct ov2655_reg ov2655_awb_on[] = {
};

static const unsigned char *ov2655_regs_awb_enable[] = {
	(unsigned char *)ov2655_awb_off,
	(unsigned char *)ov2655_awb_on,
};

/*
 * Manual White Balance (presets)
 */
static const struct ov2655_reg ov2655_wb_tungsten[] = {

};

static const struct ov2655_reg ov2655_wb_fluorescent[] = {

};

static const struct ov2655_reg ov2655_wb_sunny[] = {

};

static const struct ov2655_reg ov2655_wb_cloudy[] = {

};

/* Order of this array should be following the querymenu data */
static const unsigned char *ov2655_regs_wb_preset[] = {
	(unsigned char *)ov2655_wb_sunny,
	(unsigned char *)ov2655_wb_cloudy,
	(unsigned char *)ov2655_wb_tungsten,
	(unsigned char *)ov2655_wb_fluorescent,
};

/*
 * Color Effect (COLORFX)
 */
static const struct ov2655_reg ov2655_color_normal[] = {
};

static const struct ov2655_reg ov2655_color_monochrome[] = {
};

static const struct ov2655_reg ov2655_color_sepia[] = {
};

static const struct ov2655_reg ov2655_color_aqua[] = {
};

static const struct ov2655_reg ov2655_color_negative[] = {
};

static const struct ov2655_reg ov2655_color_sketch[] = {
};

/* Order of this array should be following the querymenu data */
static const unsigned char *ov2655_regs_color_effect[] = {
	(unsigned char *)ov2655_color_normal,
	(unsigned char *)ov2655_color_monochrome,
	(unsigned char *)ov2655_color_sepia,
	(unsigned char *)ov2655_color_aqua,
	(unsigned char *)ov2655_color_sketch,
	(unsigned char *)ov2655_color_negative,
};

/*
 * Contrast bias
 */
static const struct ov2655_reg ov2655_contrast_m2[] = {
};

static const struct ov2655_reg ov2655_contrast_m1[] = {
};

static const struct ov2655_reg ov2655_contrast_default[] = {
};

static const struct ov2655_reg ov2655_contrast_p1[] = {
};

static const struct ov2655_reg ov2655_contrast_p2[] = {
};

static const unsigned char *ov2655_regs_contrast_bias[] = {
	(unsigned char *)ov2655_contrast_m2,
	(unsigned char *)ov2655_contrast_m1,
	(unsigned char *)ov2655_contrast_default,
	(unsigned char *)ov2655_contrast_p1,
	(unsigned char *)ov2655_contrast_p2,
};

/*
 * Saturation bias
 */
static const struct ov2655_reg ov2655_saturation_m2[] = {
};

static const struct ov2655_reg ov2655_saturation_m1[] = {
};

static const struct ov2655_reg ov2655_saturation_default[] = {
};

static const struct ov2655_reg ov2655_saturation_p1[] = {
};

static const struct ov2655_reg ov2655_saturation_p2[] = {
};

static const unsigned char *ov2655_regs_saturation_bias[] = {
	(unsigned char *)ov2655_saturation_m2,
	(unsigned char *)ov2655_saturation_m1,
	(unsigned char *)ov2655_saturation_default,
	(unsigned char *)ov2655_saturation_p1,
	(unsigned char *)ov2655_saturation_p2,
};

/*
 * Sharpness bias
 */
static const struct ov2655_reg ov2655_sharpness_m2[] = {
};

static const struct ov2655_reg ov2655_sharpness_m1[] = {
};

static const struct ov2655_reg ov2655_sharpness_default[] = {
};

static const struct ov2655_reg ov2655_sharpness_p1[] = {
};

static const struct ov2655_reg ov2655_sharpness_p2[] = {
};

static const unsigned char *ov2655_regs_sharpness_bias[] = {
	(unsigned char *)ov2655_sharpness_m2,
	(unsigned char *)ov2655_sharpness_m1,
	(unsigned char *)ov2655_sharpness_default,
	(unsigned char *)ov2655_sharpness_p1,
	(unsigned char *)ov2655_sharpness_p2,
};

#endif
