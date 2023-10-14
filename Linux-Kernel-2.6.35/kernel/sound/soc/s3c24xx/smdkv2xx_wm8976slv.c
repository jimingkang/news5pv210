/*
* s3c6410_wm8976.c  --  SoC audio for s3c6410 with WM8976
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */

#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/io.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>

#include <asm/mach-types.h>
#include <plat/regs-iis.h>
#include <mach/map.h>
#include <asm/hardware/scoop.h>

#include <asm/gpio.h> 
#include <plat/gpio-cfg.h> 
#include <mach/regs-gpio.h>

#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <asm/io.h>
#include <mach/regs-clock.h>
#include <sound/soc.h>

#include "../codecs/wm8976.h"
#include "s3c-dma.h"
#include "s3c64xx-i2s-v4.h"
#include "s3c-dma-wrapper.h"


static int set_epll_rate(unsigned long rate)
{
	struct clk *fout_epll;

	fout_epll = clk_get(NULL, "fout_epll");
	if (IS_ERR(fout_epll)) {
		printk(KERN_ERR "%s: failed to get fout_epll\n", __func__);
		return -ENOENT;
	}

	if (rate == clk_get_rate(fout_epll))
		goto out;

	clk_disable(fout_epll);
	clk_set_rate(fout_epll, rate);
	clk_enable(fout_epll);

out:
	clk_put(fout_epll);

	return 0;
}

#define I2S_NUM 0

/* define the scenarios */
#define SMT_AUDIO_OFF			0
#define SMT_GSM_CALL_AUDIO_HANDSET	1
#define SMT_GSM_CALL_AUDIO_HEADSET	2
#define SMT_GSM_CALL_AUDIO_BLUETOOTH	3
#define SMT_STEREO_TO_SPEAKERS		4
#define SMT_STEREO_TO_HEADPHONES	5
#define SMT_CAPTURE_HANDSET		6
#define SMT_CAPTURE_HEADSET		7
#define SMT_CAPTURE_BLUETOOTH		8
#define SMT_ENABLE_SPEAKERS_AND_HANDSET	9

static int smdk6410_scenario = 0;
static int set_scenario_endpoints(struct snd_soc_codec *codec, int scenario)
{
        switch (smdk6410_scenario) {
        case SMT_AUDIO_OFF:
                snd_soc_dapm_disable_pin(codec, "Audio Out1");
                snd_soc_dapm_disable_pin(codec, "Audio Out2");
                snd_soc_dapm_disable_pin(codec, "GSM Line Out");
                snd_soc_dapm_disable_pin(codec, "GSM Line In");
                snd_soc_dapm_disable_pin(codec, "Headset Mic");
                snd_soc_dapm_disable_pin(codec, "Call Mic");
                break;
        case SMT_ENABLE_SPEAKERS_AND_HANDSET:
                snd_soc_dapm_enable_pin(codec, "Audio Out1");
                snd_soc_dapm_enable_pin(codec, "Audio Out2");
                snd_soc_dapm_enable_pin(codec, "GSM Line Out");
                snd_soc_dapm_enable_pin(codec, "GSM Line In");
                snd_soc_dapm_disable_pin(codec, "Headset Mic");
                snd_soc_dapm_enable_pin(codec, "Call Mic");
                break;
        case SMT_GSM_CALL_AUDIO_HANDSET:
                snd_soc_dapm_enable_pin(codec, "Audio Out1");
                snd_soc_dapm_enable_pin(codec, "GSM Line Out");
                snd_soc_dapm_enable_pin(codec, "GSM Line In");
                snd_soc_dapm_disable_pin(codec, "Headset Mic");
                snd_soc_dapm_enable_pin(codec, "Call Mic");
                break;
        case SMT_GSM_CALL_AUDIO_HEADSET:
                snd_soc_dapm_enable_pin(codec, "Audio Out1");
                snd_soc_dapm_enable_pin(codec, "GSM Line Out");
                snd_soc_dapm_enable_pin(codec, "GSM Line In");
                snd_soc_dapm_enable_pin(codec, "Headset Mic");
                snd_soc_dapm_disable_pin(codec, "Call Mic");
                break;
        case SMT_GSM_CALL_AUDIO_BLUETOOTH:
		snd_soc_dapm_disable_pin(codec, "Audio Out2");
                snd_soc_dapm_disable_pin(codec, "Audio Out1");
                snd_soc_dapm_enable_pin(codec, "GSM Line Out");
                snd_soc_dapm_enable_pin(codec, "GSM Line In");
                snd_soc_dapm_disable_pin(codec, "Headset Mic");
                snd_soc_dapm_disable_pin(codec, "Call Mic");
                break;
        case SMT_STEREO_TO_SPEAKERS:
                snd_soc_dapm_enable_pin(codec, "Audio Out2");
                snd_soc_dapm_disable_pin(codec, "Audio Out1");
                snd_soc_dapm_disable_pin(codec, "GSM Line Out");
                snd_soc_dapm_disable_pin(codec, "GSM Line In");
                snd_soc_dapm_disable_pin(codec, "Headset Mic");
                snd_soc_dapm_disable_pin(codec, "Call Mic");
                break;
        case SMT_STEREO_TO_HEADPHONES:
                snd_soc_dapm_disable_pin(codec, "Audio Out2");
                snd_soc_dapm_enable_pin(codec, "Audio Out1");
                snd_soc_dapm_disable_pin(codec, "GSM Line Out");
                snd_soc_dapm_disable_pin(codec, "GSM Line In");
                snd_soc_dapm_disable_pin(codec, "Headset Mic");
                snd_soc_dapm_disable_pin(codec, "Call Mic");
                break;
        case SMT_CAPTURE_HANDSET:
                snd_soc_dapm_disable_pin(codec, "Audio Out1");
                snd_soc_dapm_disable_pin(codec, "GSM Line Out");
                snd_soc_dapm_disable_pin(codec, "GSM Line In");
                snd_soc_dapm_disable_pin(codec, "Headset Mic");
                snd_soc_dapm_enable_pin(codec, "Call Mic");

                break;
        case SMT_CAPTURE_HEADSET:
                snd_soc_dapm_disable_pin(codec, "Audio Out1");
                snd_soc_dapm_disable_pin(codec, "GSM Line Out");
                snd_soc_dapm_disable_pin(codec, "GSM Line In");
                snd_soc_dapm_enable_pin(codec, "Headset Mic");
                snd_soc_dapm_disable_pin(codec, "Call Mic");
                break;
        case SMT_CAPTURE_BLUETOOTH:
                snd_soc_dapm_disable_pin(codec, "Audio Out2");
                snd_soc_dapm_disable_pin(codec, "Audio Out1");
                snd_soc_dapm_disable_pin(codec, "GSM Line Out");
                snd_soc_dapm_disable_pin(codec, "GSM Line In");
                snd_soc_dapm_disable_pin(codec, "Headset Mic");
                snd_soc_dapm_disable_pin(codec, "Call Mic");
                break;
        default:
                snd_soc_dapm_disable_pin(codec, "Audio Out2");
                snd_soc_dapm_disable_pin(codec, "Audio Out1");
                snd_soc_dapm_disable_pin(codec, "GSM Line Out");
                snd_soc_dapm_disable_pin(codec, "GSM Line In");
                snd_soc_dapm_disable_pin(codec, "Headset Mic");
                snd_soc_dapm_disable_pin(codec, "Call Mic");
        }

        snd_soc_dapm_sync(codec);

        return 0;
}
static int smdk6410_get_scenario(struct snd_kcontrol *kcontrol,
        struct snd_ctl_elem_value *ucontrol)
{
        ucontrol->value.integer.value[0] = smdk6410_scenario;
        return 0;
}
static int smdk6410_set_scenario(struct snd_kcontrol *kcontrol,
        struct snd_ctl_elem_value *ucontrol)
{
        struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);

        if (smdk6410_scenario == ucontrol->value.integer.value[0])
                return 0;

        smdk6410_scenario = ucontrol->value.integer.value[0];
        set_scenario_endpoints(codec, smdk6410_scenario);
        return 1;
}

static int s3c6410_hifi_hw_params(struct snd_pcm_substream *substream,	struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;
	struct snd_soc_dai *codec_dai = rtd->dai->codec_dai;
	unsigned int rclk, psr = 1;
	int bfs, rfs, ret;

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_U24:
	case SNDRV_PCM_FORMAT_S24:
		bfs = 48;
		break;
	case SNDRV_PCM_FORMAT_U16_LE:
	case SNDRV_PCM_FORMAT_S16_LE:
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
	case 16000:
	case 22050:
	case 24000:
	case 32000:
	case 44100:
	case 48000:
	case 88200:
	case 96000:
		if (bfs == 48)
			rfs = 384;
		else
			rfs = 256;
		break;
	case 64000:
		rfs = 384;
		break;
	case 8000:
	case 11025:
	case 12000:
		if (bfs == 48)
			rfs = 768;
		else
			rfs = 512;
		break;
	default:
		return -EINVAL;
	}

	rclk = params_rate(params) * rfs;

	switch (rclk) {
	case 4096000:
	case 5644800:
	case 6144000:
	case 8467200:
	case 9216000:
		psr = 8;
		break;
	case 8192000:
	case 11289600:
	case 12288000:
	case 16934400:
	case 18432000:
		psr = 4;
		break;
	case 22579200:
	case 24576000:
	case 33868800:
	case 36864000:
		psr = 2;
		break;
	case 67737600:
	case 73728000:
		psr = 1;
		break;
	default:
		printk(KERN_ERR "Not yet supported!\n");
		return -EINVAL;
	}

	set_epll_rate(rclk * psr);

	ret = snd_soc_dai_set_sysclk(cpu_dai, S3C64XX_CLKSRC_CDCLK, 0, SND_SOC_CLOCK_OUT);
	if (ret < 0)
		return ret;

	/* We use MUX for basic ops in SoC-Master mode */
	ret = snd_soc_dai_set_sysclk(cpu_dai, S3C64XX_CLKSRC_MUX, 0, SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_clkdiv(cpu_dai, S3C_I2SV2_DIV_PRESCALER, psr-1);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_clkdiv(cpu_dai, S3C_I2SV2_DIV_BCLK, bfs);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_clkdiv(cpu_dai, S3C_I2SV2_DIV_RCLK, rfs);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_fmt(codec_dai, (SND_SOC_DAIFMT_CBS_CFS |SND_SOC_DAIFMT_I2S |SND_SOC_DAIFMT_NB_NF));
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_fmt(cpu_dai, (SND_SOC_DAIFMT_CBS_CFS|SND_SOC_DAIFMT_I2S|SND_SOC_DAIFMT_NB_NF));
	if (ret < 0)
		return ret;

	return 0;
}

/* machine dapm widgets */
static const struct snd_soc_dapm_widget s3c6410_dapm_widgets[] = {
	SND_SOC_DAPM_LINE("Audio Out1", NULL),
	SND_SOC_DAPM_LINE("Audio Out2", NULL),
	SND_SOC_DAPM_LINE("GSM Line Out", NULL),
	SND_SOC_DAPM_LINE("GSM Line In", NULL),
	SND_SOC_DAPM_MIC("Headset Mic", NULL),
	SND_SOC_DAPM_MIC("Call Mic", NULL),
};
static const char *smt_scenarios[] = {
	"Off",
	"GSM Handset",
	"GSM Headset",
	"GSM Bluetooth",
	"Speakers",
	"Headphones",
	"Capture Handset",
	"Capture Headset",
	"Capture Bluetooth"
};

static const struct soc_enum smdk_scenario_enum[] = {
	SOC_ENUM_SINGLE_EXT(ARRAY_SIZE(smt_scenarios), smt_scenarios),
};

static const struct snd_kcontrol_new wm8976_s3c6410_controls[] = {
	SOC_ENUM_EXT("SMT Mode", smdk_scenario_enum[0], smdk6410_get_scenario, smdk6410_set_scenario),
};

/* example machine audio_mapnections */
static const struct snd_soc_dapm_route audio_map[] = {

	/* Connections to the ... */
	{"Audio Out1", NULL, "LOUT1"},
	{"Audio Out1", NULL, "ROUT1"},
        {"Audio Out2", NULL, "LOUT2"},
        {"Audio Out2", NULL, "ROUT2"},
	/* Connections to the GSM Module */
	{"GSM Line Out", NULL, "OUT4"},
	{"GSM Line Out", NULL, "OUT4"},
	{"MICP", NULL, "GSM Line In"},
	{"MICN", NULL, "GSM Line In"},

	/* Connections to Headset */
	//{"L2", NULL, "Mic Bias"},
	//{"Mic Bias", NULL, "Headset Mic"},

	/* Call Mic */
	{"MICP", NULL, "Mic Bias"},
	{"MICN", NULL, "Mic Bias"},
	{"Mic Bias", NULL, "Call Mic"},
};

static int s3C6410_wm8976_init(struct snd_soc_codec *codec)
{
	int i, err;

	/* Add s3c6410 specific widgets */
	for(i = 0; i < ARRAY_SIZE(s3c6410_dapm_widgets); i++) 
	{
		snd_soc_dapm_new_control(codec, &s3c6410_dapm_widgets[i]);
	}
	/* set endpoints to default mode */
	set_scenario_endpoints(codec, SMT_AUDIO_OFF);

	/* add s3c6410 specific controls */
	for (i = 0; i < ARRAY_SIZE(wm8976_s3c6410_controls); i++)
	{
		err = snd_ctl_add(codec->card, snd_soc_cnew(&wm8976_s3c6410_controls[i], codec, NULL));
		if (err < 0)
			return err;
	}
	smdk6410_scenario = SMT_ENABLE_SPEAKERS_AND_HANDSET;
	set_scenario_endpoints(codec, SMT_ENABLE_SPEAKERS_AND_HANDSET);
	
	/* set up s3c6410 specific audio paths */
	snd_soc_dapm_add_routes(codec, audio_map, ARRAY_SIZE(audio_map));

	return 0;
}

static struct snd_soc_ops s3C6410_hifi_ops = {
	.hw_params = s3c6410_hifi_hw_params,
};

static struct snd_soc_dai_link s3c6410_dai[] = {
	{
		.name = "WM8976",
		.stream_name = "WM8976 HiFi",
		.cpu_dai = &s3c64xx_i2s_v4_dai[I2S_NUM],
		.codec_dai = &wm8976_dai,
		.init = s3C6410_wm8976_init,
		.ops = &s3C6410_hifi_ops,
	},
};

static struct snd_soc_card s3c6410 = {
	.name = "smdkc1000",
	.platform = &s3c_dma_wrapper,
	.dai_link = s3c6410_dai,
	.num_links = ARRAY_SIZE(s3c6410_dai),
};

static struct wm8976_setup_data s3c6410_wm8976_setup = {
	.i2c_bus = 2,
	.i2c_address = 0x1A,
};

static struct snd_soc_device s3c6410_snd_devdata = {
	.card = &s3c6410,
	.codec_dev = &soc_codec_dev_wm8976,
	.codec_data = &s3c6410_wm8976_setup,
};

static struct platform_device *s3c6410_snd_device;
static int __init s3c6410_init(void)
{
	int ret;

	s3c6410_snd_device = platform_device_alloc("soc-audio", 1);
	if (!s3c6410_snd_device)
		return -ENOMEM;

	platform_set_drvdata(s3c6410_snd_device, &s3c6410_snd_devdata);
	s3c6410_snd_devdata.dev = &s3c6410_snd_device->dev;
	ret = platform_device_add(s3c6410_snd_device);

	if (ret)
		platform_device_put(s3c6410_snd_device);
	
	return ret;
}

static void __exit s3c6410_exit(void)
{
	platform_device_unregister(s3c6410_snd_device);
}

module_init(s3c6410_init);
module_exit(s3c6410_exit);

MODULE_AUTHOR("Jiang jianjun");
MODULE_DESCRIPTION("ALSA SoC WM8976 S3C6410");
MODULE_LICENSE("GPL");
