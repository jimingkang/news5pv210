/*
 * smdkc100_wm8580.c
 *
 * Copyright (C) 2009, Samsung Elect. Ltd. - Jaswinder Singh <jassisinghbrar@gmail.com>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
//#include "s5pc11x-i2s.h"
//#include "s5pc11x-pcm.h"

#include <asm/io.h>
#include <asm/gpio.h> 
#include <plat/gpio-cfg.h> 
#include <plat/map-base.h>
//#include <plat/regs-clock.h>

//#include "../codecs/wm8580.h"
#include "../codecs/rt5625.h"

//--------------------------------
#include "s3c-dma.h"
#include "s3c64xx-i2s.h"

#include <mach/map.h>
#include <mach/regs-clock.h>


//#define CONFIG_SND_DEBUG
#define I2S_NUM 0

#ifdef CONFIG_SND_DEBUG
#define s3cdbg(x...) printk(x)
#else
#define s3cdbg(x...)
#endif

#define wait_stable(utime_out)                                  \
        do {                                                    \
                if (!utime_out)                                 \
                        utime_out = 1000;                       \
                utime_out = loops_per_jiffy / HZ * utime_out;   \
                while (--utime_out) {                           \
                        cpu_relax();                            \
                }                                               \
        } while (0);




/* SMDKC100 has 12MHZ Osc attached to WM8580 */
//#define SMDKC100_WM8580_OSC_FREQ (12000000)

#define PLAY_51       0
#define PLAY_STEREO   1
#define PLAY_OFF      2

#define REC_MIC    0
#define REC_LINE   1
#define REC_OFF    2

extern struct s5p_pcm_pdata s3c_pcm_pdat;
extern struct s5p_i2s_pdata s3c_i2s_pdat;
extern struct snd_soc_platform s3c_dma_wrapper;
#define  XCLKOUT_FOR_MCLK			24000000


#define SRC_CLK	(*s3c_i2s_pdat.p_rate)

static int lowpower = 0;
static int smdkc100_play_opt;
static int smdkc100_rec_opt;


//extern unsigned int rt5625_record_status;

static int set_epll_rate(unsigned long rate)
{
	struct clk *fout_epll;
	unsigned int wait_utime = 100;

	fout_epll = clk_get(NULL, "fout_epll");
	if (IS_ERR(fout_epll)) {
		printk(KERN_ERR "%s: failed to get fout_epll\n", __func__);
		return -ENOENT;
	}

	if (rate == clk_get_rate(fout_epll))
		goto out;

	clk_disable(fout_epll);
	wait_stable(wait_utime);

	clk_set_rate(fout_epll, rate);
	wait_stable(wait_utime);

	clk_enable(fout_epll);

out:
	clk_put(fout_epll);

	return 0;
}

static
int smdkc100_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;
	struct snd_soc_dai *codec_dai = rtd->dai->codec_dai;
	unsigned int rclk, psr=1;
	int bfs, rfs, ret;
	unsigned long pll1_out_rate;

	printk("enter into %s, master\n",__func__);

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_U24:
	case SNDRV_PCM_FORMAT_S24:
		bfs = 48;
		break;
	case SNDRV_PCM_FORMAT_U16_LE:
	case SNDRV_PCM_FORMAT_S16_LE:
		printk("data format:16bits\n");
		bfs = 32;
		break;
	default:
		return -EINVAL;
	}

	/* The Fvco for WM8580 PLLs must fall within [90,100]MHz.
	 * This criterion can't be met if we request PLL output
	 * as {8000x256, 64000x256, 11025x256}Hz.
	 * As a wayout, we rather change rfs to a minimum value that
	 * results in (params_rate(params) * rfs), and itself, acceptable
	 * to both - the CODEC and the CPU.
	 */
	switch (params_rate(params)) {
	case 8000:
	case 11025:
	case 12000:
	case 16000:
	case 22050:
	case 24000:
	case 32000:
	case 44100:
	case 48000:
	case 88200:
	case 96000:
	case 64000:
		rfs = 512;
		break;
	default:
		return -EINVAL;
	}


	rclk = params_rate(params) * rfs;


	switch(rclk)
	{
		case 4096000:
			psr = 6;
			break;
		case 5644800:
			psr = 4;
			break;
		case 8192000:
			psr = 3;
			break;
		case 11289600:
			psr = 2;
			break;
		case 22579200:
		case 24576000:
			psr = 1;
			break;
		default:
			psr = 1;
			break;
	}

	pll1_out_rate = rclk*psr;
		
	ret = snd_soc_dai_set_pll(codec_dai,RT5625_PLL1_FROM_MCLK,XCLKOUT_FOR_MCLK,pll1_out_rate);
	if (ret < 0)
		return ret;
	
	ret = snd_soc_dai_set_sysclk(codec_dai, RT5625_MCLK,
					pll1_out_rate, SND_SOC_CLOCK_OUT);
	if (ret < 0)
		return ret;
	
	ret = snd_soc_dai_set_sysclk(cpu_dai, S3C64XX_CLKSRC_CDCLK,
					0, SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;

	/* We use PCLK for basic ops in SoC-Slave mode */
	ret = snd_soc_dai_set_sysclk(cpu_dai, S3C64XX_CLKSRC_PCLK,
					0, SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;
	
	//ret = snd_soc_dai_set_clkdiv(cpu_dai, S3C_I2SV2_DIV_PRESCALER, psr-1);
	//if (ret < 0)
		//return ret;

	ret = snd_soc_dai_set_clkdiv(cpu_dai, S3C_I2SV2_DIV_BCLK, bfs);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_clkdiv(cpu_dai, S3C_I2SV2_DIV_RCLK, rfs);
	if (ret < 0)
		return ret;

#if 1
	ret = snd_soc_dai_set_fmt(codec_dai, (SND_SOC_DAIFMT_CBS_CFS |SND_SOC_DAIFMT_I2S |SND_SOC_DAIFMT_NB_NF));
	if (ret < 0)
		return ret;
	ret = snd_soc_dai_set_fmt(cpu_dai, (SND_SOC_DAIFMT_CBM_CFM|SND_SOC_DAIFMT_I2S|SND_SOC_DAIFMT_NB_NF));
	if (ret < 0)
		return ret;
#endif

	return 0;
}

EXPORT_SYMBOL(smdkc100_hw_params);
/*
 * RT5625 DAI operations.
 */
static struct snd_soc_ops smdkc100_ops = {
	.hw_params = smdkc100_hw_params,
};

static void smdkc100_ext_control(struct snd_soc_codec *codec)
{
	printk("%s\n", __func__);
	/* set up jack connection */
	if(smdkc100_play_opt == PLAY_51){
		snd_soc_dapm_enable_pin(codec, "Front-L/R");
		snd_soc_dapm_enable_pin(codec, "Center/Sub");
		snd_soc_dapm_enable_pin(codec, "Rear-L/R");
	}else if(smdkc100_play_opt == PLAY_STEREO){
		printk("PLAY_STEREO\n");
		snd_soc_dapm_enable_pin(codec, "Front-L/R");
		snd_soc_dapm_disable_pin(codec, "Center/Sub");
		snd_soc_dapm_disable_pin(codec, "Rear-L/R");
	}else{
		snd_soc_dapm_disable_pin(codec, "Front-L/R");
		snd_soc_dapm_disable_pin(codec, "Center/Sub");
		snd_soc_dapm_disable_pin(codec, "Rear-L/R");
	}

	if(smdkc100_rec_opt == REC_MIC){ 
		printk(" %s:: ON MIC path...\n",__func__);
		snd_soc_dapm_enable_pin(codec, "MicIn");
		snd_soc_dapm_disable_pin(codec, "LineIn");
	}else if(smdkc100_rec_opt == REC_LINE){
		printk("%s:: ON LINE-IN path...\n",__func__);
		snd_soc_dapm_disable_pin(codec, "MicIn");
		snd_soc_dapm_enable_pin(codec, "LineIn");
	}else{
		snd_soc_dapm_disable_pin(codec, "MicIn");
		snd_soc_dapm_disable_pin(codec, "LineIn");
	}

	/* signal a DAPM event */
	snd_soc_dapm_sync(codec);
}

static int smdkc100_get_pt(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	s3cdbg("%s\n", __func__);
	ucontrol->value.integer.value[0] = smdkc100_play_opt;
	return 0;
}

static int smdkc100_set_pt(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	s3cdbg("%s\n", __func__);
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);

	if(smdkc100_play_opt == ucontrol->value.integer.value[0])
		return 0;

	smdkc100_play_opt = ucontrol->value.integer.value[0];
	smdkc100_ext_control(codec);
	return 1;
}

static int smdkc100_get_cs(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	s3cdbg("%s\n", __func__);
	ucontrol->value.integer.value[0] = smdkc100_rec_opt;
	return 0;
}

static int smdkc100_set_cs(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{	
	s3cdbg("%s\n", __func__);
	struct snd_soc_codec *codec =  snd_kcontrol_chip(kcontrol);

	if(smdkc100_rec_opt == ucontrol->value.integer.value[0])
		return 0;

	smdkc100_rec_opt = ucontrol->value.integer.value[0];
	smdkc100_ext_control(codec);
	return 1;
}

/* smdkc100 machine dapm widgets */
static const struct snd_soc_dapm_widget rt5625_dapm_widgets[] = {
	SND_SOC_DAPM_HP("Front-L/R", NULL),
	SND_SOC_DAPM_HP("Center/Sub", NULL),
	SND_SOC_DAPM_HP("Rear-L/R", NULL),
	SND_SOC_DAPM_MIC("MicIn", NULL),
	SND_SOC_DAPM_LINE("LineIn", NULL),
};

/* smdk machine audio map (connections to the codec pins) */
static const struct snd_soc_dapm_route audio_map[] = {
	/* Front Left/Right are fed VOUT1L/R */
	{"Front-L/R", NULL, "VOUT1L"},
	{"Front-L/R", NULL, "VOUT1R"},

	/* Center/Sub are fed VOUT2L/R */
	{"Center/Sub", NULL, "VOUT2L"},
	{"Center/Sub", NULL, "VOUT2R"},

	/* Rear Left/Right are fed VOUT3L/R */
	{"Rear-L/R", NULL, "VOUT3L"},
	{"Rear-L/R", NULL, "VOUT3R"},

	/* MicIn feeds AINL */
	{"AINL", NULL, "MicIn"},

	/* LineIn feeds AINL/R */
	{"AINL", NULL, "LineIn"},
	{"AINR", NULL, "LineIn"},
};

static const char *play_function[] = {
	[PLAY_51]     = "5.1 Chan",
	[PLAY_STEREO] = "Stereo",
	[PLAY_OFF]    = "Off",
};

static const char *rec_function[] = {
	[REC_MIC] = "Mic",
	[REC_LINE] = "Line",
	[REC_OFF] = "Off",
};

static const struct soc_enum smdkc100_enum[] = {
	SOC_ENUM_SINGLE_EXT(3, play_function),
	SOC_ENUM_SINGLE_EXT(3, rec_function),
};

static const struct snd_kcontrol_new rt5625_smdkc100_controls[] = {
	SOC_ENUM_EXT("Playback Target", smdkc100_enum[0], smdkc100_get_pt,
		smdkc100_set_pt),
	SOC_ENUM_EXT("Capture Source", smdkc100_enum[1], smdkc100_get_cs,
		smdkc100_set_cs),
};

static int smdkc100_rt5625_init(struct snd_soc_codec *codec)
{
	printk("enter into %s\n",__func__);
	int i, err;
	

	/* Add smdkc100 specific controls */
	for (i = 0; i < ARRAY_SIZE(rt5625_smdkc100_controls); i++) {
		err = snd_ctl_add(codec->card,
			snd_soc_cnew(&rt5625_smdkc100_controls[i], codec, NULL));
		if (err < 0)
			return err;
	}

	/* Add smdkc100 specific widgets */
	snd_soc_dapm_new_controls(codec, rt5625_dapm_widgets,
				  ARRAY_SIZE(rt5625_dapm_widgets));

	/* Set up smdkc100 specific audio path audio_map */
	snd_soc_dapm_add_routes(codec, audio_map, ARRAY_SIZE(audio_map));

	/* No jack detect - mark all jacks as enabled */
	for (i = 0; i < ARRAY_SIZE(rt5625_dapm_widgets); i++)
		snd_soc_dapm_enable_pin(codec, rt5625_dapm_widgets[i].name);

	/* Setup Default Route */
	smdkc100_play_opt = PLAY_STEREO;
	smdkc100_rec_opt = REC_LINE;
	//smdkc100_rec_opt = REC_MIC;
	smdkc100_ext_control(codec);

	return 0;
}

static struct snd_soc_dai_link smdkc100_dai[] = {
{
	.name = "RT5625 HiFi",
	.stream_name = "HiFi Playback",
	.cpu_dai = &s3c64xx_i2s_v4_dai[I2S_NUM],
	.codec_dai = &rt5625_dai[0],
	.init = smdkc100_rt5625_init,
	.ops = &smdkc100_ops,
},
{
	.name = "RT5625 HiFi",
	.stream_name = "HiFi Capture",
	.cpu_dai = &s3c64xx_i2s_v4_dai[I2S_NUM],
	.codec_dai = &rt5625_dai[0],
	.init = smdkc100_rt5625_init,
	.ops = &smdkc100_ops,
},
};

static struct snd_soc_card smdkc100 = {
	.name = "smdkc100",
	.platform = &s3c24xx_soc_platform,
	.dai_link = smdkc100_dai,
	.num_links = ARRAY_SIZE(smdkc100_dai),
};

static struct rt5625_setup_data smdkc100_rt5625_setup = {
	.i2c_address = 0x3e >> 1,
};

static struct snd_soc_device smdkc100_snd_devdata = {
	.card = &smdkc100,
	.codec_dev = &soc_codec_dev_rt5625,
	.codec_data = &smdkc100_rt5625_setup,
};

static struct platform_device *smdkc100_snd_device;

static int __init smdkc100_audio_init(void)
{
	int ret;
	u32 div;
	u32 reg;
	u32 val;
#include <mach/map.h>
#define S3C_VA_AUDSS	S3C_ADDR(0x01600000)	/* Audio SubSystem */
#include <mach/regs-audss.h>

	printk("enter into %s\n",__func__);

#if 1
	div = 4;
	ret = set_epll_rate(XCLKOUT_FOR_MCLK* div);
	if (ret < 0)
		return ret;
#endif

#if 1	
	val = readl(S5P_CLKSRC_AUDSS);
	val &= ~(0x1<<0);
	val |= 0x1<<0;
	//val &= ~(0x3<<2);
	//val |= 0x1<<2;
	writel(val, S5P_CLKSRC_AUDSS);		//select XXTI as the source for audio bus clock
	 printk("S5P_CLKSRC_AUDSS,val:%08x\n",val);
	 
	val = readl(S5P_CLKGATE_AUDSS);
	val |= (0x7f<<0);					//enable all modules for audio subsystem except for I2SCLK
	writel(val, S5P_CLKGATE_AUDSS);
	printk("S5P_CLKGATE_AUDSS,val:%08x\n",val);
#endif	
	/* Select the XUSBXTI as the source clock of XCLK_OUT(XUSBXTI -> 24MHz) */
	reg = __raw_readl(S5P_CLK_OUT);
	reg &= ~S5P_CLKOUT_CLKSEL_MASK;
	reg |= S5P_CLKOUT_CLKSEL_XUSBXTI;
	reg &= ~S5P_CLKOUT_DIV_MASK;
	reg |= 0x0000 << S5P_CLKOUT_DIV_SHIFT;	/* DIVVAL = 1, Ratio = 2 = DIVVAL + 1, codec's MCLK can use 2.048MHz to 40MHz when PLL1 enabled, select 12MHz*/
	__raw_writel(reg, S5P_CLK_OUT);


	/* CLKOUT is prior to CLK_OUT of SYSCON. XXTI & XUSBXTI work in sleep mode */
	reg = __raw_readl(S5P_OTHERS);
	reg &= ~(0x0003 << 8);
	reg |= 0x3 << 8;	/* Clock from SYSCON */
	__raw_writel(reg, S5P_OTHERS);

	smdkc100_snd_device = platform_device_alloc("soc-audio", 0);
	if (!smdkc100_snd_device)
		return -ENOMEM;
	
	platform_set_drvdata(smdkc100_snd_device, &smdkc100_snd_devdata);

	
	smdkc100_snd_devdata.dev = &smdkc100_snd_device->dev;
	ret = platform_device_add(smdkc100_snd_device);

	if (ret)
		platform_device_put(smdkc100_snd_device);

	return ret;
}

static void __exit smdkc100_audio_exit(void)
{
	s3cdbg("%s\n", __func__);
	platform_device_unregister(smdkc100_snd_device);
}

module_init(smdkc100_audio_init);
module_exit(smdkc100_audio_exit);

module_param (lowpower, int, 0444);

/* Module information */
MODULE_DESCRIPTION("ALSA SoC SMDKC100 RT5625 master");
MODULE_LICENSE("GPL");
