#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/jiffies.h>
#include <asm/delay.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <linux/interrupt.h>
#include <sound/initval.h>
#include <sound/tlv.h>
#include <asm/div64.h>
#include <mach/regs-irq.h>
#include <plat/gpio-cfg.h>
#include <mach/map.h>
#include <linux/gpio.h>

#include <linux/irq.h>

//#include <plat/s5pc100_pmic.h>
/* For accessing S3C6410 GPIO register to drive AK4671 PDN pin(GPE1)
 * Ideally ALSA "Codec driver" implmentation should not have any
 * machine specific details in this file. However this is an
 * exception. */
//#include <asm/arch/regs-gpio.h>
#include <asm/io.h>
/****/
#include "rt5625.h"

#include <linux/regulator/consumer.h>
static  struct regulator *speaker_5v_regulator;


#define RT5625_VERSION "0.04"

//#define CONFIG_SND_DEBUG

#ifdef CONFIG_SND_DEBUG
#define s3cdbg(fmt, args...) printk( "rt5625:"fmt"\n" , ## args)
#else
#define s3cdbg(fmt, args...) 
#endif

struct rt5625_priv {
	unsigned int stereo_sysclk;
	unsigned int voice_sysclk;
};

struct rt5625_init_reg {
	char name[30];
	u16 reg_value;
	u8 reg_index;
};

struct aec_config *rt5625_aec_cfg;

static struct snd_pcm_substream *mSubStream = NULL;

static struct rt5625_init_reg rt5625_init_list[] = {
	{"HP Output Volume", 0x9090, RT5625_HP_OUT_VOL},
	{"SPK Output Volume", 0x8080, RT5625_SPK_OUT_VOL},
	{"DAC Mic Route", 0xee03, RT5625_DAC_AND_MIC_CTRL},
	{"Output Mixer Control", 0x0748, RT5625_OUTPUT_MIXER_CTRL},
	{"Mic Control", 0x0500, RT5625_MIC_CTRL},
	//{"Voice DAC Volume", 0x6008, RT5625_VOICE_DAC_OUT_VOL},
	{"Voice DAC Volume", 0x7008, RT5625_VOICE_DAC_OUT_VOL},
	//{"ADC Rec Mixer", 0x3f3f, RT5625_ADC_REC_MIXER},
	{"ADC Rec Mixer", 0x7f7f, RT5625_ADC_REC_MIXER},
	{"General Control", 0x0c0a, RT5625_GEN_CTRL_REG1}
};

#define RT5625_INIT_REG_NUM ARRAY_SIZE(rt5625_init_list)

/*
 *	bit[0]  for linein playback switch
 *	bit[1] phone
 *	bit[2] mic1
 *	bit[3] mic2
 *	bit[4] vopcm
 *	
 */
 #define HPL_MIXER 0x80
#define HPR_MIXER 0x82
static unsigned int reg80 = 0, reg82 = 0;

/*
 *	bit[0][1] use for aec control
 *	bit[2][3] for ADCR func
 *	bit[4] for SPKL pga
 *	bit[5] for SPKR pga
 *	bit[6] for hpl pga
 *	bit[7] for hpr pga
 *	bit[8] for aec mode
 */
 #define VIRTUAL_REG_FOR_MISC_FUNC 0x84
static unsigned int reg84 = 0;
 static unsigned int PlaybackStreamNum = 0;
 static unsigned int CaptureStreamNum = 0;
 
static const u16 rt5625_reg[] = {
	0x59b4, 0x8080, 0x8080, 0x8080,		/*reg00-reg06*/
	0xc800, 0xe808, 0x1010, 0x0808,		/*reg08-reg0e*/
	0xe0ef, 0xcbcb, 0x7f7f, 0x0000,		/*reg10-reg16*/
	0xe010, 0x0000, 0x8008, 0x2007,		/*reg18-reg1e*/
	0x0000, 0x0000, 0x00c0, 0xef00,		/*reg20-reg26*/
	0x0000, 0x0000, 0x0000, 0x0000,		/*reg28-reg2e*/
	0x0000, 0x0000, 0x0000, 0x0000,		/*reg30-reg36*/
	0x0000, 0x0000, 0x0000, 0x0000, 		/*reg38-reg3e*/
	0x0c0a, 0x0000, 0x0000, 0x0000,		/*reg40-reg46*/
	0x0029, 0x0000, 0xbe3e, 0x3e3e,		/*reg48-reg4e*/
	0x0000, 0x0000, 0x803a, 0x0000,		/*reg50-reg56*/
	0x0000, 0x0009, 0x0000, 0x3000,		/*reg58-reg5e*/
	0x3075, 0x1010, 0x3110, 0x0000,		/*reg60-reg66*/
	0x0553, 0x0000, 0x0000, 0x0000,		/*reg68-reg6e*/
	0x0000, 0x0000, 0x0000, 0x0000,		/*reg70-reg76*/
	0x0000, 0x0000, 0x0000, 0x0000,               /*reg76-reg7e*/
};


Voice_DSP_Reg VODSP_AEC_Init_Value[]=
{
	{0x232C, 0x0025},
	{0x230B, 0x0001},
	{0x2308, 0x007F},
	{0x23F8, 0x4003},
	{0x2301, 0x0002},
	{0x2328, 0x0001},
	{0x2304, 0x00FA},
	{0x2305, 0x0100},
	{0x2306, 0x4000},
	{0x230D, 0x0400},
	{0x230E, 0x0100},
	{0x2312, 0x00B1},
	{0x2314, 0xC000},
	{0x2316, 0x0041},
	{0x2317, 0x2000},
	{0x2318, 0x0C00},
	{0x231D, 0x00A0},
	{0x231F, 0x5800},
	{0x2330, 0x0008},
	{0x2335, 0x0005},
	{0x2336, 0x0001},
	{0x2337, 0x5800},
	{0x233A, 0x0300},
	{0x233B, 0x0030},
	{0x2341, 0x0008},
	{0x2343, 0x0800},		
	{0x23A7, 0x0200},
	{0x22CE, 0x0400},
	{0x22D3, 0x0C00},
	{0x22D4, 0x1800},
	{0x230C, 0x0000},	//to enable VODSP AEC function
};


#define SET_VODSP_REG_INIT_NUM	ARRAY_SIZE(VODSP_AEC_Init_Value)
static struct snd_soc_device*rt5625_socdev;

extern int smdkc100_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params);

static inline unsigned int rt5625_read_reg_cache(struct snd_soc_codec *codec, 
	unsigned int reg)
{
	u16 *cache = codec->reg_cache;

	if (reg > 0x7e)
		return 0;
	return cache[reg / 2];
}


static unsigned int rt5625_read_hw_reg(struct snd_soc_codec *codec, unsigned int reg) 
{
	u8 data[2] = {0};
	unsigned int value = 0x0;
	
	data[0] = reg;
	if (codec->hw_write(codec->control_data, data, 1) == 1)
	{
		i2c_master_recv(codec->control_data, data, 2);//wangkai changed from codec->hw_read
		value = (data[0] << 8) | data[1];
//		s3cdbg(KERN_DEBUG "%s read reg%x = %x\n", __func__, reg, value);
		return value;
	}
	else
	{
		printk(KERN_DEBUG "%s failed\n", __func__);
		return -EIO;
	}
}


static unsigned int rt5625_read(struct snd_soc_codec *codec, unsigned int reg)
{
	if ((reg == 0x80)
		|| (reg == 0x82)
		|| (reg == 0x84))
		return (reg == 0x80) ? reg80 : ((reg == 0x82) ? reg82 : reg84);
	
		return rt5625_read_hw_reg(codec, reg);
}


static inline void rt5625_write_reg_cache(struct snd_soc_codec *codec,
	unsigned int reg, unsigned int value)
{
	u16 *cache = codec->reg_cache;
	if (reg > 0x7E)
		return;
	cache[reg / 2] = value;
}

static int rt5625_write(struct snd_soc_codec *codec, unsigned int reg,
	unsigned int value)
{
	u8 data[3];
	unsigned int *regvalue = NULL;

	data[0] = reg;
	data[1] = (value & 0xff00) >> 8;
	data[2] = value & 0x00ff;
	if ((reg == 0x80)
		|| (reg == 0x82)
		|| (reg == 0x84))
	{		
		regvalue = ((reg == 0x80) ? &reg80 : ((reg == 0x82) ? &reg82 : &reg84));
		*regvalue = value;
		//s3cdbg(KERN_INFO "rt5625_write ok, reg = %x, value = %x\n", reg, value);
		return 0;
	}
	rt5625_write_reg_cache(codec, reg, value);
	if (codec->hw_write(codec->control_data, data, 3) == 3)
	{
		//s3cdbg(KERN_INFO "rt5625_write ok, reg = %x, value = %x\n", reg, value);
		return 0;
	}
	else 
	{
		printk(KERN_ERR "rt5625_write fail\n");
		return -EIO;
	}
}

#define rt5625_write_mask(c, reg, value, mask) snd_soc_update_bits(c, reg, mask, value)

#define rt5625_reset(c) rt5625_write(c, RT5625_RESET, 0)

static unsigned int rt5625_read_index(struct snd_soc_codec *codec, unsigned int reg_index)
{
	rt5625_write(codec, 0x6a, reg_index);
	return rt5625_read(codec, 0x6c);
}

/*read/write dsp reg*/
static int rt5625_wait_vodsp_i2c_done(struct snd_soc_codec *codec)
{
	unsigned int checkcount = 0, vodsp_data;

	vodsp_data = rt5625_read(codec, RT5625_VODSP_REG_CMD);
	while(vodsp_data & VODSP_BUSY)
	{
		if(checkcount > 10)
			return -EBUSY;
		vodsp_data = rt5625_read(codec, RT5625_VODSP_REG_CMD);
		checkcount ++;		
	}
	return 0;
}
static int rt5625_write_vodsp_reg(struct snd_soc_codec *codec, unsigned int vodspreg, unsigned int value)
{
	int ret = 0;

	if(ret = rt5625_wait_vodsp_i2c_done(codec))
		return ret;

	rt5625_write(codec, RT5625_VODSP_REG_ADDR, vodspreg);
	rt5625_write(codec, RT5625_VODSP_REG_DATA, value);
	rt5625_write(codec, RT5625_VODSP_REG_CMD, VODSP_WRITE_ENABLE | VODSP_CMD_MW);
	mdelay(10);
	return ret;
	
}

static unsigned int rt5625_read_vodsp_reg(struct snd_soc_codec *codec, unsigned int vodspreg)
{
	int ret = 0;
	unsigned int nDataH, nDataL;
	unsigned int value;

	if(ret = rt5625_wait_vodsp_i2c_done(codec))
		return ret;
	
	rt5625_write(codec, RT5625_VODSP_REG_ADDR, vodspreg);
	rt5625_write(codec, RT5625_VODSP_REG_CMD, VODSP_READ_ENABLE | VODSP_CMD_MR);

	if (ret = rt5625_wait_vodsp_i2c_done(codec))
		return ret;
	rt5625_write(codec, RT5625_VODSP_REG_ADDR, 0x26);
	rt5625_write(codec, RT5625_VODSP_REG_CMD, VODSP_READ_ENABLE | VODSP_CMD_RR);

	if(ret = rt5625_wait_vodsp_i2c_done(codec))
		return ret;
	nDataH = rt5625_read(codec, RT5625_VODSP_REG_DATA);
	rt5625_write(codec, RT5625_VODSP_REG_ADDR, 0x25);
	rt5625_write(codec, RT5625_VODSP_REG_CMD, VODSP_READ_ENABLE | VODSP_CMD_RR);

	if(ret = rt5625_wait_vodsp_i2c_done(codec))
		return ret;
	nDataL = rt5625_read(codec, RT5625_VODSP_REG_DATA);
	value = ((nDataH & 0xff) << 8) |(nDataL & 0xff);
	s3cdbg(KERN_INFO "%s vodspreg=0x%x, value=0x%x\n", __func__, vodspreg, value);
	return value;
}

static int rt5625_reg_init(struct snd_soc_codec *codec)
{
	int i;

	for (i = 0; i < RT5625_INIT_REG_NUM; i++)
		rt5625_write(codec, rt5625_init_list[i].reg_index, rt5625_init_list[i].reg_value);

	return 0;
}



static const char *rt5625_spk_out_sel[] = {"Class AB", "Class D"}; 					/*1*/
static const char *rt5625_spk_l_source_sel[] = {"LPRN", "LPRP", "LPLN", "MM"};		/*2*/	
static const char *rt5625_spkmux_source_sel[] = {"VMID", "HP Mixer", 
							"SPK Mixer", "Mono Mixer"};               					/*3*/
static const char *rt5625_hplmux_source_sel[] = {"VMID","HPL Mixer"};				/*4*/
static const char *rt5625_hprmux_source_sel[] = {"VMID","HPR Mixer"};				/*5*/
static const char *rt5625_auxmux_source_sel[] = {"VMID", "HP Mixer", 
							"SPK Mixer", "Mono Mixer"};							/*6*/
static const char *rt5625_spkamp_ratio_sel[] = {"2.25 Vdd", "2.00 Vdd",
					"1.75 Vdd", "1.50 Vdd", "1.25 Vdd", "1.00 Vdd"};				/*7*/
static const char *rt5625_mic1_boost_sel[] = {"Bypass", "+20db", "+30db", "+40db"};	/*8*/
static const char *rt5625_mic2_boost_sel[] = {"Bypass", "+20db", "+30db", "+40db"};	/*9*/



static const struct soc_enum rt5625_enum[] = {

SOC_ENUM_SINGLE(RT5625_OUTPUT_MIXER_CTRL, 13, 2, rt5625_spk_out_sel),		/*1*/
SOC_ENUM_SINGLE(RT5625_OUTPUT_MIXER_CTRL, 14, 4, rt5625_spk_l_source_sel),	/*2*/
SOC_ENUM_SINGLE(RT5625_OUTPUT_MIXER_CTRL, 10, 4, rt5625_spkmux_source_sel),/*3*/
SOC_ENUM_SINGLE(RT5625_OUTPUT_MIXER_CTRL, 9, 2, rt5625_hplmux_source_sel),	/*4*/
SOC_ENUM_SINGLE(RT5625_OUTPUT_MIXER_CTRL, 8, 2, rt5625_hprmux_source_sel),/*5*/
SOC_ENUM_SINGLE(RT5625_OUTPUT_MIXER_CTRL, 6, 4, rt5625_auxmux_source_sel),/*6*/
SOC_ENUM_SINGLE(RT5625_GEN_CTRL_REG1, 1, 6, rt5625_spkamp_ratio_sel),		/*7*/
SOC_ENUM_SINGLE(RT5625_MIC_CTRL, 10, 4,  rt5625_mic1_boost_sel),			/*8*/
SOC_ENUM_SINGLE(RT5625_MIC_CTRL, 8, 4, rt5625_mic2_boost_sel)
};

////////////////////////
//For headphone IRQ
////////////////////////

//AUDIO PATH relative
typedef enum {
	AUDIO_OUT_PATH_HEADPHONE=0,
	AUDIO_OUT_PATH_SPEAKER,
	AUDIO_OUT_PATH_UNKNOWN
}audio_out_path_type;

typedef enum{
	SPEAKER_OFF = 0,
	SPEAKER_ON = 1,
}speaker_state;
audio_out_path_type gAudioOutPath = AUDIO_OUT_PATH_UNKNOWN;
speaker_state	 gSpeakerState = SPEAKER_OFF;
static int playing_music_count = 0 ;
static struct work_struct hp_short_wq;
static irqreturn_t rt5625_hp_hotplug_detect(int irqno, void *dev_id);


static u32 get_headphone_status()
{
	return gpio_get_value(S5PV210_GPH0(6));
}

static void set_audio_out_path(void)
{
	struct snd_soc_device *socdev = rt5625_socdev;
	struct snd_soc_codec *codec = socdev->card->codec;

	u32 headphone_pin_stat = get_headphone_status();
	
	if (headphone_pin_stat == AUDIO_OUT_PATH_UNKNOWN)
		return;

	gAudioOutPath = headphone_pin_stat;


	if (!PlaybackStreamNum && !CaptureStreamNum)
		return;
		
	if (headphone_pin_stat == AUDIO_OUT_PATH_SPEAKER)
	{
		//Mute headphone right/left channel
		rt5625_write_mask(codec,RT5625_HP_OUT_VOL, RT_L_MUTE|RT_R_MUTE, RT_L_MUTE|RT_R_MUTE);

		//On speaker right/left channel
		rt5625_write_mask(codec,RT5625_SPK_OUT_VOL, 0, RT_L_MUTE|RT_R_MUTE);

		//On voice DAC volume out to speaker mixer control
		//rt5625_write_mask(codec,RT5625_VOICE_DAC_OUT_VOL,0,0x4000);

		//rt5625_write_mask(codec,RT5625_OUTPUT_MIXER_CTRL,SPKOUT_SEL_CLASS_AB|SPK_CLASS_AB_W_AMP|SPKOUT_INPUT_SEL_MASK,SPKOUT_SEL_CLASS_AB|SPK_CLASS_AB_W_AMP|SPKOUT_INPUT_SEL_SPKMIXER);

		//set DAC vol. to +6dB for spk 
		rt5625_write(codec, RT5625_STEREO_DAC_VOL,0x0808);

		//power on SPK out R/L volume
		rt5625_write(codec, RT5625_PWR_MANAG_ADD3, (rt5625_read_reg_cache(codec,RT5625_PWR_MANAG_ADD3) | 0x3000));

		//power on speaker apm
		rt5625_write(codec, RT5625_PWR_MANAG_ADD1, (rt5625_read_reg_cache(codec,RT5625_PWR_MANAG_ADD1) | 0x0400));	             		

		//rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD1, PWR_HP_OUT_AMP|PWR_HP_OUT_ENH_AMP,PWR_HP_OUT_AMP|PWR_HP_OUT_ENH_AMP);
		
		regulator_enable(speaker_5v_regulator);
		gSpeakerState = SPEAKER_ON;
	}
	else
	{
		if (gSpeakerState == SPEAKER_ON)
		{
			regulator_disable(speaker_5v_regulator);
			gSpeakerState = SPEAKER_OFF;
		}
		
		//Mute speaker right/left channel
		rt5625_write_mask(codec,RT5625_SPK_OUT_VOL, RT_L_MUTE|RT_R_MUTE, RT_L_MUTE|RT_R_MUTE);

		//On HP right/left channel
		rt5625_write_mask(codec,RT5625_HP_OUT_VOL, 0, RT_L_MUTE|RT_R_MUTE);

		//On voice DAC volume out to headphone mixer control
		//rt5625_write_mask(codec,RT5625_VOICE_DAC_OUT_VOL,0,0x8000);

		//set DAC vol. to 0dB for hp 
		rt5625_write(codec, RT5625_STEREO_DAC_VOL,0x1010);

		//power on HP out R/L volume
		rt5625_write(codec, RT5625_PWR_MANAG_ADD3, (rt5625_read_reg_cache(codec,RT5625_PWR_MANAG_ADD3) | 0x0C00));
			
		rt5625_write(codec, RT5625_PWR_MANAG_ADD2, (rt5625_read_reg_cache(codec,RT5625_PWR_MANAG_ADD2) | 0x0030));
	}
#if 0
	//Mute DAC to HP,Speaker,Mono Mixer
	rt5625_write_mask(codec,RT5625_DAC_AND_MIC_CTRL,
                                         M_DAC_TO_HPL_MIXER|M_DAC_TO_HPR_MIXER|M_DAC_TO_SPK_MIXER|M_DAC_TO_MONO_MIXER,
                                         M_DAC_TO_HPL_MIXER|M_DAC_TO_HPR_MIXER|M_DAC_TO_SPK_MIXER|M_DAC_TO_MONO_MIXER);
#endif
}

static irqreturn_t rt5625_hp_detect_wq(void)
{
	s3cdbg("%s",__FUNCTION__);	
	set_audio_out_path();
}
	
static irqreturn_t rt5625_hp_hotplug_detect(int irqno, void *dev_id)
{
	s3cdbg("%s",__FUNCTION__);	
	
	schedule_work(&hp_short_wq); 
	
	return IRQ_HANDLED;
}
static irqreturn_t rt5625_mic_detect(int irqno, void *dev_id)
{
	s3cdbg("%s",__FUNCTION__);	
	return IRQ_HANDLED;
}

//static int rt5625_dump_dsp_reg(struct snd_soc_codec *codec);
static int init_vodsp_aec(struct snd_soc_codec *codec)
{
	int i;
	int ret = 0;
	s3cdbg( "enter %s\n", __func__);	
	/*enable LDO power and set output voltage to 1.2V*/
	rt5625_write_mask(codec, RT5625_LDO_CTRL,LDO_ENABLE|LDO_OUT_VOL_CTRL_1_20V,LDO_ENABLE|LDO_OUT_VOL_CTRL_MASK);
	mdelay(20);
	/*enable power of VODSP I2C interface*/ 
	rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD3,PWR_VODSP_INTERFACE|PWR_I2C_FOR_VODSP,PWR_VODSP_INTERFACE|PWR_I2C_FOR_VODSP);
	mdelay(1);
	rt5625_write_mask(codec, RT5625_VODSP_CTL,0,VODSP_NO_RST_MODE_ENA);	/*Reset VODSP*/
	mdelay(1);
	rt5625_write_mask(codec, RT5625_VODSP_CTL,VODSP_NO_RST_MODE_ENA,VODSP_NO_RST_MODE_ENA);	/*set VODSP to non-reset status*/		
	mdelay(20);
	/*initize AEC paramter*/
	for(i = 0; i < SET_VODSP_REG_INIT_NUM; i++)
	{
		ret = rt5625_write_vodsp_reg(codec, VODSP_AEC_Init_Value[i].VoiceDSPIndex,VODSP_AEC_Init_Value[i].VoiceDSPValue);
		if(ret)
			return -EIO;
	}		
	schedule_timeout_uninterruptible(msecs_to_jiffies(100));
	//set VODSP to pown down mode	
	rt5625_write_mask(codec, RT5625_VODSP_CTL,0,VODSP_NO_PD_MODE_ENA);
	rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD3,0,PWR_VODSP_INTERFACE|PWR_I2C_FOR_VODSP);	
//	rt5625_dump_dsp_reg(codec);
	return 0;
}

static int rt5625_set_aec_sysclk(struct snd_soc_codec *codec, unsigned int mode);
static int aec_additional_config_for_analog_path(struct snd_soc_codec *codec, unsigned int mode)
{
	s3cdbg( "enter %s\n", __func__);	
	if (mode != VODSP_AEC_DISABLE) {
		rt5625_set_aec_sysclk(codec, mode);
		snd_soc_dapm_stream_event(codec, "Playback", SND_SOC_DAPM_STREAM_START);
		snd_soc_dapm_stream_event(codec, "Capture", SND_SOC_DAPM_STREAM_START);
		
	} else {
		snd_soc_dapm_stream_event(codec, "Playback", SND_SOC_DAPM_STREAM_STOP);
		snd_soc_dapm_stream_event(codec, "Capture", SND_SOC_DAPM_STREAM_STOP);
		rt5625_set_aec_sysclk(codec, mode);
	}
}
static int set_vodsp_aec_path(struct snd_soc_codec *codec, unsigned int mode)
{
	s3cdbg( "enter %s\n", __func__);	
		if (rt5625_aec_cfg->is_analog_path)
		{
			aec_additional_config_for_analog_path(codec, mode);
		}
		
		switch(mode)
		{
			/*Far End signal is from Voice interface and Near End signal is from MIC1/MIC2)*/
			case PCM_IN_PCM_OUT:

				 /*	1.Far End setting(Far end device PCM out-->VODAC PCM IN--->VODSP_RXDP )*/
				 
				/*****************************************************************************
				  *	a.Enable RxDP power and select RxDP source from "Voice to Stereo Digital path"		
				  *	b.Voice PCM out from VoDSP TxDP(VODSP TXDP--->VODAC PCM out-->Far End devie PCM out)
				  ******************************************************************************/
				rt5625_write_mask(codec, RT5625_VODSP_PDM_CTL,VODSP_RXDP_PWR|VODSP_RXDP_S_SEL_VOICE|VOICE_PCM_S_SEL_AEC_TXDP
															,VODSP_RXDP_PWR|VODSP_RXDP_S_SEL_MASK|VOICE_PCM_S_SEL_MASK);
				 /*	2.Near end setting*/
				/***********************************************************************************	 
				  *	a.ADCR function select PDM Slave interface(Mic-->ADCR-->PDM interface)
				  *	b.Voice DAC Source Select VODSP_TxDC
				  ************************************************************************************/
				rt5625_write_mask(codec, RT5625_DAC_ADC_VODAC_FUN_SEL,ADCR_FUNC_SEL_PDM|VODAC_SOUR_SEL_VODSP_TXDC
																,ADCR_FUNC_SEL_MASK|VODAC_SOUR_SEL_MASK);

				/*3.setting VODSP LRCK to 8k*/
				rt5625_write_mask(codec, RT5625_VODSP_CTL,VODSP_LRCK_SEL_8K,VODSP_LRCK_SEL_MASK);						

				break;
			
			/*Far End signal is from Analog input and Near End signal is from MIC1/MIC2)*/
			case ANALOG_IN_ANALOG_OUT:	
				/*	1.Far End setting(Far end device-->Analog in-->ADC_L-->VODSP_RXDP)   */
				/************************************************************************	
				  *	a.Enable RxDP power and select RxDP source from ADC_L 
				  ************************************************************************/
				rt5625_write_mask(codec, RT5625_VODSP_PDM_CTL,VODSP_RXDP_PWR|VODSP_RXDP_S_SEL_ADCL,
															 VODSP_RXDP_PWR|VODSP_RXDP_S_SEL_MASK);

				/*2.Near end setting*/
					/*************************************************************************
					  *a.VoDSP TxDP--->VODAC--->analog out-->to Far end analog input
					  *b.ADCR function select PDM Slave interface(Mic-->ADCR-->PDM interface)
					  *************************************************************************/
				rt5625_write_mask(codec, RT5625_DAC_ADC_VODAC_FUN_SEL,ADCR_FUNC_SEL_PDM|VODAC_SOUR_SEL_VODSP_TXDP
																	,ADCR_FUNC_SEL_MASK|VODAC_SOUR_SEL_MASK);
				/*3.setting VODSP LRCK to 16k*/
				rt5625_write_mask(codec, RT5625_VODSP_CTL,VODSP_LRCK_SEL_16K,VODSP_LRCK_SEL_MASK);	
			
				break;

			/*Far End signal is from Playback and Near End signal is from MIC1/MIC2)*/
			case DAC_IN_ADC_OUT:	
				/***********************************************************************
				  *	1.Far End setting(Playback-->SRC1-->VODSP_RXDP)
				  *		a.enable SRC1 and VoDSP_RXDP source select SRC1
				  *	2.Near End setting(VoDSP_TXDP-->SRC2-->Stereo Record)
				  *		a.enable SRC2 and select Record source from SRC2
				  **********************************************************************/
				rt5625_write_mask(codec, RT5625_VODSP_PDM_CTL,VODSP_SRC1_PWR|VODSP_SRC2_PWR|VODSP_RXDP_PWR|VODSP_RXDP_S_SEL_SRC1|REC_S_SEL_SRC2,
															 VODSP_SRC1_PWR|VODSP_SRC2_PWR|VODSP_RXDP_PWR|VODSP_RXDP_S_SEL_MASK|REC_S_SEL_MASK);					

				break;

			case VODSP_AEC_DISABLE:
			default:
			
				/*set stereo DAC&Voice DAC&Stereo ADC function select to default*/ 
				rt5625_write(codec, RT5625_DAC_ADC_VODAC_FUN_SEL,0);

		
				/*set VODSP&PDM Control to default*/ 
				rt5625_write(codec, RT5625_VODSP_PDM_CTL,0);	
			
				break;
		}		

	return 0;
}

static int enable_vodsp_aec(struct snd_soc_codec *codec, unsigned int VodspAEC_En, unsigned int AEC_mode)
{
	int  ret = 0;
	s3cdbg( "enter %s\n", __func__);	
	
	if (VodspAEC_En != 0)
	{	
		//select input/output of VODSP AEC
		set_vodsp_aec_path(codec, AEC_mode);		
		//enable power of VODSP I2C interface & VODSP interface
		rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD3,PWR_VODSP_INTERFACE|PWR_I2C_FOR_VODSP,PWR_VODSP_INTERFACE|PWR_I2C_FOR_VODSP);
		//enable power of VODSP I2S interface 
		rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD1,PWR_I2S_INTERFACE,PWR_I2S_INTERFACE);	
		//set VODSP to active			
		rt5625_write_mask(codec, RT5625_VODSP_CTL,VODSP_NO_PD_MODE_ENA,VODSP_NO_PD_MODE_ENA);	
		mdelay(50);	
	}
	else
	{
		//set VODSP AEC to power down mode			
		rt5625_write_mask(codec, RT5625_VODSP_CTL,0,VODSP_NO_PD_MODE_ENA);
		//disable power of VODSP I2C interface & VODSP interface
		rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD3,0,PWR_VODSP_INTERFACE|PWR_I2C_FOR_VODSP);
		//disable VODSP AEC path
		rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD1,0,PWR_I2S_INTERFACE);	
		set_vodsp_aec_path(codec, VODSP_AEC_DISABLE);				
	}

	return ret;
}



static int rt5625_get_aec_mode(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	int mode;
	s3cdbg( "enter %s\n", __func__);	
	mode = codec->read(codec, 0x84);
	ucontrol->value.integer.value[0] = (mode & 0x0100) >> 8;
	return 0;
}

static int rt5625_set_aec_mode(struct snd_kcontrol *kcontrol,
	struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_codec *codec = snd_kcontrol_chip(kcontrol);
	int mode;
	s3cdbg( "enter %s\n", __func__);	
	mode = codec->read(codec, 0x84);
	if ((mode & 0x0100) == (ucontrol->value.integer.value[0] << 8))
		return 0;

	mode &= ~(0x0100);
	mode |= ucontrol->value.integer.value[0] << 8;
	codec->write(codec, 0x84, mode);
	enable_vodsp_aec(codec, ucontrol->value.integer.value[0], rt5625_aec_cfg->aec_mode);
	
}
static const struct snd_kcontrol_new rt5625_snd_controls[] = {

SOC_ENUM("SPK Amp Type", rt5625_enum[0]),
SOC_ENUM("Left SPK Source", rt5625_enum[1]),
SOC_ENUM("SPK Amp Ratio", rt5625_enum[6]),
SOC_ENUM("Mic1 Boost", rt5625_enum[7]),
SOC_ENUM("Mic2 Boost", rt5625_enum[8]),
SOC_DOUBLE("PCM Playback Volume", RT5625_STEREO_DAC_VOL, 8, 0, 63, 1),
SOC_DOUBLE("LineIn Playback Volume", RT5625_LINE_IN_VOL, 8, 0, 31, 1),
SOC_SINGLE("Phone Playback Volume", RT5625_PHONEIN_VOL, 8, 31, 1),
SOC_SINGLE("Mic1 Playback Volume", RT5625_MIC_VOL, 8, 31, 1),
SOC_SINGLE("Mic2 Playback Volume", RT5625_MIC_VOL, 0, 31, 1),
SOC_DOUBLE("PCM Capture Volume", RT5625_ADC_REC_GAIN, 8, 0, 31, 1),
SOC_DOUBLE("SPKOUT Playback Volume", RT5625_SPK_OUT_VOL, 8, 0, 31, 1),
SOC_DOUBLE("SPKOUT Playback Switch", RT5625_SPK_OUT_VOL, 15, 7, 1, 1),
SOC_DOUBLE("HPOUT Playback Volume", RT5625_HP_OUT_VOL, 8, 0, 31, 1),
SOC_DOUBLE("HPOUT Playback Switch", RT5625_HP_OUT_VOL, 15, 7, 1, 1),
SOC_DOUBLE("AUXOUT Playback Volume", RT5625_AUX_OUT_VOL, 8, 0, 31, 1),
SOC_DOUBLE("AUXOUT Playback Switch", RT5625_AUX_OUT_VOL, 15, 7, 1, 1),
SOC_DOUBLE("ADC Record Gain", RT5625_ADC_REC_GAIN, 8, 0, 31, 0),
SOC_SINGLE_EXT("Echo Cancelation", VIRTUAL_REG_FOR_MISC_FUNC, 8, 1, 0, rt5625_get_aec_mode, rt5625_set_aec_mode)
};

static int rt5625_add_controls(struct snd_soc_codec *codec)
{
	int err, i;

	for (i = 0; i < ARRAY_SIZE(rt5625_snd_controls); i++){
		err = snd_ctl_add(codec->card, 
				snd_soc_cnew(&rt5625_snd_controls[i],
						codec, NULL));
		if (err < 0)
			return err;
	}
	return 0;
}

static void hp_depop_mode2(struct snd_soc_codec *codec)
{
	s3cdbg( "enter %s\n", __func__);	
        rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD1, PWR_MAIN_BIAS, PWR_MAIN_BIAS);
        rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD2, PWR_MIXER_VREF, PWR_MIXER_VREF);
        rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD1, PWR_SOFTGEN_EN, PWR_SOFTGEN_EN);
        rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD3, PWR_HP_R_OUT_VOL|PWR_HP_L_OUT_VOL,
                PWR_HP_R_OUT_VOL|PWR_HP_L_OUT_VOL);
        rt5625_write_mask(codec, RT5625_MISC_CTRL, HP_DEPOP_MODE2_EN, HP_DEPOP_MODE2_EN);
        schedule_timeout_uninterruptible(msecs_to_jiffies(300));

        rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD1, PWR_HP_OUT_AMP|PWR_HP_OUT_ENH_AMP,
                PWR_HP_OUT_AMP|PWR_HP_OUT_ENH_AMP);
        rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD1, 0, HP_DEPOP_MODE2_EN);

}


#if USE_DAPM_CTRL
/*
 * _DAPM_ Controls
 */
 /*Left ADC Rec mixer*/
static const struct snd_kcontrol_new rt5625_left_adc_rec_mixer_controls[] = {
SOC_DAPM_SINGLE("Mic1 Capture Switch", RT5625_ADC_REC_MIXER, 14, 1, 1),
SOC_DAPM_SINGLE("Mic2 Capture Switch", RT5625_ADC_REC_MIXER, 13, 1, 1),
SOC_DAPM_SINGLE("LineIn Capture Switch", RT5625_ADC_REC_MIXER, 12, 1, 1),
SOC_DAPM_SINGLE("Phone Capture Switch", RT5625_ADC_REC_MIXER, 11, 1, 1),
SOC_DAPM_SINGLE("HP Mixer Capture Switch", RT5625_ADC_REC_MIXER, 10, 1, 1),
SOC_DAPM_SINGLE("MoNo Mixer Capture Switch", RT5625_ADC_REC_MIXER, 8, 1, 1),
SOC_DAPM_SINGLE("SPK Mixer Capture Switch", RT5625_ADC_REC_MIXER, 9, 1, 1),

};

/*Left ADC Rec mixer*/
static const struct snd_kcontrol_new rt5625_right_adc_rec_mixer_controls[] = {
SOC_DAPM_SINGLE("Mic1 Capture Switch", RT5625_ADC_REC_MIXER, 6, 1, 1),
SOC_DAPM_SINGLE("Mic2 Capture Switch", RT5625_ADC_REC_MIXER, 5, 1, 1),
SOC_DAPM_SINGLE("LineIn Capture Switch", RT5625_ADC_REC_MIXER, 4, 1, 1),
SOC_DAPM_SINGLE("Phone Capture Switch", RT5625_ADC_REC_MIXER, 3, 1, 1),
SOC_DAPM_SINGLE("HP Mixer Capture Switch", RT5625_ADC_REC_MIXER, 2, 1, 1),
SOC_DAPM_SINGLE("MoNo Mixer Capture Switch", RT5625_ADC_REC_MIXER, 0, 1, 1),
SOC_DAPM_SINGLE("SPK Mixer Capture Switch", RT5625_ADC_REC_MIXER, 1, 1, 1),
};

static const struct snd_kcontrol_new rt5625_left_hp_mixer_controls[] = {
SOC_DAPM_SINGLE("ADC Playback Switch", RT5625_ADC_REC_GAIN, 15, 1, 1),
SOC_DAPM_SINGLE("LineIn Playback Switch", HPL_MIXER, 0, 1, 0),
SOC_DAPM_SINGLE("Phone Playback Switch", HPL_MIXER, 1, 1, 0),
SOC_DAPM_SINGLE("Mic1 Playback Switch", HPL_MIXER, 2, 1, 0),
SOC_DAPM_SINGLE("Mic2 Playback Switch", HPL_MIXER, 3, 1, 0),
SOC_DAPM_SINGLE("HIFI DAC Playback Switch", RT5625_DAC_AND_MIC_CTRL, 3, 1, 1),
SOC_DAPM_SINGLE("Voice DAC Playback Switch", HPL_MIXER, 4, 1, 0),
};

static const struct snd_kcontrol_new rt5625_right_hp_mixer_controls[] = {
SOC_DAPM_SINGLE("ADC Playback Switch", RT5625_ADC_REC_GAIN, 7, 1, 1),
SOC_DAPM_SINGLE("LineIn Playback Switch", HPR_MIXER, 0, 1, 0),
SOC_DAPM_SINGLE("Phone Playback Switch", HPR_MIXER, 1, 1, 0),
SOC_DAPM_SINGLE("Mic1 Playback Switch", HPR_MIXER, 2, 1, 0),
SOC_DAPM_SINGLE("Mic2 Playback Switch", HPR_MIXER, 3, 1, 0),
SOC_DAPM_SINGLE("HIFI DAC Playback Switch", RT5625_DAC_AND_MIC_CTRL, 2, 1, 1),
SOC_DAPM_SINGLE("Voice DAC Playback Switch", HPR_MIXER, 4, 1, 0),
};

static const struct snd_kcontrol_new rt5625_mono_mixer_controls[] = {
SOC_DAPM_SINGLE("ADCL Playback Switch", RT5625_ADC_REC_GAIN, 14, 1, 1),
SOC_DAPM_SINGLE("ADCR Playback Switch", RT5625_ADC_REC_GAIN, 6, 1, 1),
SOC_DAPM_SINGLE("Line Mixer Playback Switch", RT5625_LINE_IN_VOL, 13, 1, 1),
SOC_DAPM_SINGLE("Mic1 Playback Switch", RT5625_DAC_AND_MIC_CTRL, 13, 1, 1),
SOC_DAPM_SINGLE("Mic2 Playback Switch", RT5625_DAC_AND_MIC_CTRL, 9, 1, 1),
SOC_DAPM_SINGLE("DAC Mixer Playback Switch", RT5625_DAC_AND_MIC_CTRL, 0, 1, 1),
SOC_DAPM_SINGLE("Voice DAC Playback Switch", RT5625_VOICE_DAC_OUT_VOL, 13, 1, 1),
};

static const struct snd_kcontrol_new rt5625_spk_mixer_controls[] = {
SOC_DAPM_SINGLE("Line Mixer Playback Switch", RT5625_LINE_IN_VOL, 14, 1, 1),	
SOC_DAPM_SINGLE("Phone Playback Switch", RT5625_PHONEIN_VOL, 14, 1, 1),
SOC_DAPM_SINGLE("Mic1 Playback Switch", RT5625_DAC_AND_MIC_CTRL, 14, 1, 1),
SOC_DAPM_SINGLE("Mic2 Playback Switch", RT5625_DAC_AND_MIC_CTRL, 10, 1, 1),
SOC_DAPM_SINGLE("DAC Mixer Playback Switch", RT5625_DAC_AND_MIC_CTRL, 1, 1, 1),
SOC_DAPM_SINGLE("Voice DAC Playback Switch", RT5625_VOICE_DAC_OUT_VOL, 14, 1, 1),
};

static int mixer_event(struct snd_soc_dapm_widget *w, struct snd_kcontrol *k, int event)
{
	struct snd_soc_codec *codec = w->codec;
	unsigned int l, r;

	s3cdbg(KERN_INFO "enter %s\n", __func__);

	l= rt5625_read(codec, HPL_MIXER);
	r = rt5625_read(codec, HPR_MIXER);
	
	if ((l & 0x1) || (r & 0x1))
		rt5625_write_mask(codec, 0x0a, 0x0000, 0x8000);
	else
		rt5625_write_mask(codec, 0x0a, 0x8000, 0x8000);

	if ((l & 0x2) || (r & 0x2))
		rt5625_write_mask(codec, 0x08, 0x0000, 0x8000);
	else
		rt5625_write_mask(codec, 0x08, 0x8000, 0x8000);

	if ((l & 0x4) || (r & 0x4))
		rt5625_write_mask(codec, 0x10, 0x0000, 0x8000);
	else
		rt5625_write_mask(codec, 0x10, 0x8000, 0x8000);

	if ((l & 0x8) || (r & 0x8))
		rt5625_write_mask(codec, 0x10, 0x0000, 0x0800);
	else
		rt5625_write_mask(codec, 0x10, 0x0800, 0x0800);

	if ((l & 0x10) || (r & 0x10))
		rt5625_write_mask(codec, 0x18, 0x0000, 0x8000);
	else
		rt5625_write_mask(codec, 0x18, 0x8000, 0x8000);

	return 0;
}


/*
 *	bit[0][1] use for aec control
 *	bit[2][3] for ADCR func
 *	bit[4] for SPKL pga
 *	bit[5] for SPKR pga
 *	bit[6] for hpl pga
 *	bit[7] for hpr pga
 */
static int spk_pga_event(struct snd_soc_dapm_widget *w, struct snd_kcontrol *k, int event)
 {
	struct snd_soc_codec *codec = w->codec;
	int reg;
	
	s3cdbg(KERN_DEBUG "enter %s\n", __func__);
	reg = rt5625_read(codec, VIRTUAL_REG_FOR_MISC_FUNC) & (0x3 << 4);
	if ((reg >> 4) != 0x3 && reg != 0)
		return 0;

	switch (event)
	{
		case SND_SOC_DAPM_POST_PMU:
			s3cdbg(KERN_INFO "after virtual spk power up!\n");
			rt5625_write_mask(codec, 0x3e, 0x3000, 0x3000);
			rt5625_write_mask(codec, 0x02, 0x0000, 0x8080);
			rt5625_write_mask(codec, 0x3a, 0x0400, 0x0400);                  //power on spk amp
			break;
		case SND_SOC_DAPM_POST_PMD:
			s3cdbg(KERN_INFO "aftet virtual spk power down!\n");
			rt5625_write_mask(codec, 0x3a, 0x0000, 0x0400);//power off spk amp
			rt5625_write_mask(codec, 0x02, 0x8080, 0x8080);
			rt5625_write_mask(codec, 0x3e, 0x0000, 0x3000);                 
			break;
		default:
			return 0;
	}
	return 0;
}




static int hp_pga_event(struct snd_soc_dapm_widget *w, struct snd_kcontrol *k, int event)
{
	struct snd_soc_codec *codec = w->codec;
	int reg;

	s3cdbg(KERN_DEBUG "enter %s\n", __func__);
	reg = rt5625_read(codec, VIRTUAL_REG_FOR_MISC_FUNC) & (0x3 << 6);
	if ((reg >> 6) != 0x3 && reg != 0)
		return 0;
	
	switch (event)
	{
		case SND_SOC_DAPM_POST_PMD:
			s3cdbg(KERN_INFO "aftet virtual hp power down!\n");
			rt5625_write_mask(codec, 0x04, 0x8080, 0x8080);
			rt5625_write_mask(codec, 0x3e, 0x0000, 0x0600);
			rt5625_write_mask(codec, 0x3a, 0x0000, 0x0030);
			break;
		case SND_SOC_DAPM_POST_PMU:	
			s3cdbg(KERN_INFO "after virtual hp power up!\n");
			hp_depop_mode2(codec);
			rt5625_write_mask(codec ,0x04, 0x0000, 0x8080);
			break;
		default:
			return 0;
	}	
	return 0;
}



static int aux_pga_event(struct snd_soc_dapm_widget *w, struct snd_kcontrol *k, int event)
{	s3cdbg( "enter %s\n", __func__);	
	return 0;
}

/*SPKOUT Mux*/
static const struct snd_kcontrol_new rt5625_spkout_mux_out_controls = 
SOC_DAPM_ENUM("Route", rt5625_enum[2]);

/*HPLOUT MUX*/
static const struct snd_kcontrol_new rt5625_hplout_mux_out_controls = 
SOC_DAPM_ENUM("Route", rt5625_enum[3]);

/*HPROUT MUX*/
static const struct snd_kcontrol_new rt5625_hprout_mux_out_controls = 
SOC_DAPM_ENUM("Route", rt5625_enum[4]);
/*AUXOUT MUX*/
static const struct snd_kcontrol_new rt5625_auxout_mux_out_controls = 
SOC_DAPM_ENUM("Route", rt5625_enum[5]);

static const struct snd_soc_dapm_widget rt5625_dapm_widgets[] = {
SND_SOC_DAPM_INPUT("Left LineIn"),
SND_SOC_DAPM_INPUT("Right LineIn"),
SND_SOC_DAPM_INPUT("Phone"),
SND_SOC_DAPM_INPUT("Mic1"),
SND_SOC_DAPM_INPUT("Mic2"),

SND_SOC_DAPM_PGA("Mic1 Boost", RT5625_PWR_MANAG_ADD3, 1, 0, NULL, 0),
SND_SOC_DAPM_PGA("Mic2 Boost", RT5625_PWR_MANAG_ADD3, 0, 0, NULL, 0),

SND_SOC_DAPM_DAC("Left DAC", "Left HiFi Playback DAC", RT5625_PWR_MANAG_ADD2, 9, 0),
SND_SOC_DAPM_DAC("Right DAC", "Right HiFi Playback DAC", RT5625_PWR_MANAG_ADD2, 8, 0),
SND_SOC_DAPM_DAC("Voice DAC", "Voice Playback DAC", RT5625_PWR_MANAG_ADD2, 10, 0),

SND_SOC_DAPM_PGA("Left LineIn PGA", RT5625_PWR_MANAG_ADD3, 7, 0, NULL, 0),
SND_SOC_DAPM_PGA("Right LineIn PGA", RT5625_PWR_MANAG_ADD3, 6, 0, NULL, 0),
SND_SOC_DAPM_PGA("Phone PGA", RT5625_PWR_MANAG_ADD3, 5, 0, NULL, 0),
SND_SOC_DAPM_PGA("Mic1 PGA", RT5625_PWR_MANAG_ADD3, 3, 0, NULL, 0),
SND_SOC_DAPM_PGA("Mic2 PGA", RT5625_PWR_MANAG_ADD3, 2, 0, NULL, 0),
SND_SOC_DAPM_PGA("Left DAC PGA", RT5625_PWR_MANAG_ADD1, 15, 0, NULL, 0),
SND_SOC_DAPM_PGA("Right DAC PGA", RT5625_PWR_MANAG_ADD1, 14, 0, NULL, 0),
SND_SOC_DAPM_PGA("VoDAC PGA", RT5625_PWR_MANAG_ADD1, 7, 0, NULL, 0),
SND_SOC_DAPM_MIXER("Left Rec Mixer", RT5625_PWR_MANAG_ADD2, 1, 0,
	&rt5625_left_adc_rec_mixer_controls[0], ARRAY_SIZE(rt5625_left_adc_rec_mixer_controls)),
SND_SOC_DAPM_MIXER("Right Rec Mixer", RT5625_PWR_MANAG_ADD2, 0, 0,
	&rt5625_right_adc_rec_mixer_controls[0], ARRAY_SIZE(rt5625_right_adc_rec_mixer_controls)),
SND_SOC_DAPM_MIXER_E("Left HP Mixer", RT5625_PWR_MANAG_ADD2, 5, 0,
	&rt5625_left_hp_mixer_controls[0], ARRAY_SIZE(rt5625_left_hp_mixer_controls),
	mixer_event, SND_SOC_DAPM_POST_REG),
SND_SOC_DAPM_MIXER_E("Right HP Mixer", RT5625_PWR_MANAG_ADD2, 4, 0,
	&rt5625_right_hp_mixer_controls[0], ARRAY_SIZE(rt5625_right_hp_mixer_controls),
	mixer_event, SND_SOC_DAPM_POST_REG),
SND_SOC_DAPM_MIXER("MoNo Mixer", RT5625_PWR_MANAG_ADD2, 2, 0, 
	&rt5625_mono_mixer_controls[0], ARRAY_SIZE(rt5625_mono_mixer_controls)),
SND_SOC_DAPM_MIXER("SPK Mixer", RT5625_PWR_MANAG_ADD2, 3, 0,
	&rt5625_spk_mixer_controls[0], ARRAY_SIZE(rt5625_spk_mixer_controls)),
	
/*hpl mixer --> hp mixer-->spkout mux, hpr mixer-->hp mixer -->spkout mux
   hpl mixer-->hp mixer-->Auxout Mux, hpr muxer-->hp mixer-->auxout mux*/	
SND_SOC_DAPM_MIXER("HP Mixer", SND_SOC_NOPM, 0, 0, NULL, 0),
SND_SOC_DAPM_MIXER("DAC Mixer", SND_SOC_NOPM, 0, 0, NULL, 0),
SND_SOC_DAPM_MIXER("Line Mixer", SND_SOC_NOPM, 0, 0, NULL, 0),

SND_SOC_DAPM_MUX("SPKOUT Mux", SND_SOC_NOPM, 0, 0, &rt5625_spkout_mux_out_controls),
SND_SOC_DAPM_MUX("HPLOUT Mux", SND_SOC_NOPM, 0, 0, &rt5625_hplout_mux_out_controls),
SND_SOC_DAPM_MUX("HPROUT Mux", SND_SOC_NOPM, 0, 0, &rt5625_hprout_mux_out_controls),
SND_SOC_DAPM_MUX("AUXOUT Mux", SND_SOC_NOPM, 0, 0, &rt5625_auxout_mux_out_controls),

SND_SOC_DAPM_PGA_E("SPKL Out PGA", VIRTUAL_REG_FOR_MISC_FUNC, 4, 0, NULL, 0,
				spk_pga_event, SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_POST_PMD),
SND_SOC_DAPM_PGA_E("SPKR Out PGA", VIRTUAL_REG_FOR_MISC_FUNC, 5, 0, NULL, 0,
				spk_pga_event, SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_POST_PMD),
SND_SOC_DAPM_PGA_E("HPL Out PGA",VIRTUAL_REG_FOR_MISC_FUNC, 6, 0, NULL, 0, 
				hp_pga_event, SND_SOC_DAPM_POST_PMD | SND_SOC_DAPM_POST_PMU),
SND_SOC_DAPM_PGA_E("HPR Out PGA",VIRTUAL_REG_FOR_MISC_FUNC, 7, 0, NULL, 0, 
				hp_pga_event, SND_SOC_DAPM_POST_PMD | SND_SOC_DAPM_POST_PMU),
SND_SOC_DAPM_PGA_E("AUX Out PGA",RT5625_PWR_MANAG_ADD3, 14, 0, NULL, 0, 
				aux_pga_event, SND_SOC_DAPM_PRE_PMD | SND_SOC_DAPM_POST_PMU),
				

SND_SOC_DAPM_ADC("Left ADC", "Left ADC HiFi Capture", RT5625_PWR_MANAG_ADD2, 7, 0),
SND_SOC_DAPM_ADC("Right ADC", "Right ADC HiFi Capture", RT5625_PWR_MANAG_ADD2, 6, 0),
SND_SOC_DAPM_OUTPUT("SPKL"),
SND_SOC_DAPM_OUTPUT("SPKR"),
SND_SOC_DAPM_OUTPUT("HPL"),
SND_SOC_DAPM_OUTPUT("HPR"),
SND_SOC_DAPM_OUTPUT("AUX"),
SND_SOC_DAPM_MICBIAS("Mic1 Bias", RT5625_PWR_MANAG_ADD1, 3, 0),
SND_SOC_DAPM_MICBIAS("Mic2 Bias", RT5625_PWR_MANAG_ADD1, 2, 0),
};

static const struct snd_soc_dapm_route audio_map[] = {
		/*Input PGA*/

		{"Left LineIn PGA", NULL, "Left LineIn"},
		{"Right LineIn PGA", NULL, "Right LineIn"},
		{"Phone PGA", NULL, "Phone"},
		{"Mic1 Boost", NULL, "Mic1"},
		{"Mic2 Boost", NULL, "Mic2"},
		{"Mic1 PGA", NULL, "Mic1 Boost"},
		{"Mic2 PGA", NULL, "Mic2 Boost"},
		{"Left DAC PGA", NULL, "Left DAC"},
		{"Right DAC PGA", NULL, "Right DAC"},
		{"VoDAC PGA", NULL, "Voice DAC"},
		
		/*Left ADC mixer*/
		{"Left Rec Mixer", "LineIn Capture Switch", "Left LineIn"},
		{"Left Rec Mixer", "Phone Capture Switch", "Phone"},
		{"Left Rec Mixer", "Mic1 Capture Switch", "Mic1"},
		{"Left Rec Mixer", "Mic2 Capture Switch", "Mic2"},
		{"Left Rec Mixer", "HP Mixer Capture Switch", "Left HP Mixer"},
		{"Left Rec Mixer", "SPK Mixer Capture Switch", "SPK Mixer"},
		{"Left Rec Mixer", "MoNo Mixer Capture Switch", "MoNo Mixer"},

		/*Right ADC Mixer*/
		{"Right Rec Mixer", "LineIn Capture Switch", "Right LineIn"},
		{"Right Rec Mixer", "Phone Capture Switch", "Phone"},
		{"Right Rec Mixer", "Mic1 Capture Switch", "Mic1"},
		{"Right Rec Mixer", "Mic2 Capture Switch", "Mic2"},
		{"Right Rec Mixer", "HP Mixer Capture Switch", "Right HP Mixer"},
		{"Right Rec Mixer", "SPK Mixer Capture Switch", "SPK Mixer"},
		{"Right Rec Mixer", "MoNo Mixer Capture Switch", "MoNo Mixer"},
		
		/*HPL mixer*/
		{"Left HP Mixer", "ADC Playback Switch", "Left Rec Mixer"},
		{"Left HP Mixer", "LineIn Playback Switch", "Left LineIn PGA"},
		{"Left HP Mixer", "Phone Playback Switch", "Phone PGA"},
		{"Left HP Mixer", "Mic1 Playback Switch", "Mic1 PGA"},
		{"Left HP Mixer", "Mic2 Playback Switch", "Mic2 PGA"},
		{"Left HP Mixer", "HIFI DAC Playback Switch", "Left DAC PGA"},
		{"Left HP Mixer", "Voice DAC Playback Switch", "VoDAC PGA"},
		
		/*HPR mixer*/
		{"Right HP Mixer", "ADC Playback Switch", "Right Rec Mixer"},
		{"Right HP Mixer", "LineIn Playback Switch", "Right LineIn PGA"},	
		{"Right HP Mixer", "HIFI DAC Playback Switch", "Right DAC PGA"},
		{"Right HP Mixer", "Phone Playback Switch", "Phone PGA"},
		{"Right HP Mixer", "Mic1 Playback Switch", "Mic1 PGA"},
		{"Right HP Mixer", "Mic2 Playback Switch", "Mic2 PGA"},
		{"Right HP Mixer", "Voice DAC Playback Switch", "VoDAC PGA"},

		/*DAC Mixer*/
		{"DAC Mixer", NULL, "Left DAC PGA"},
		{"DAC Mixer", NULL, "Right DAC PGA"},

		/*line mixer*/
		{"Line Mixer", NULL, "Left LineIn PGA"},
		{"Line Mixer", NULL, "Right LineIn PGA"},

		/*spk mixer*/
		{"SPK Mixer", "Line Mixer Playback Switch", "Line Mixer"},
		{"SPK Mixer", "Phone Playback Switch", "Phone PGA"},
		{"SPK Mixer", "Mic1 Playback Switch", "Mic1 PGA"},
		{"SPK Mixer", "Mic2 Playback Switch", "Mic2 PGA"},
		{"SPK Mixer", "DAC Mixer Playback Switch", "DAC Mixer"},
		{"SPK Mixer", "Voice DAC Playback Switch", "VoDAC PGA"},

		/*mono mixer*/
		{"MoNo Mixer", "Line Mixer Playback Switch", "Line Mixer"},
		{"MoNo Mixer", "ADCL Playback Switch","Left Rec Mixer"},
		{"MoNo Mixer", "ADCR Playback Switch","Right Rec Mixer"},
		{"MoNo Mixer", "Mic1 Playback Switch", "Mic1 PGA"},
		{"MoNo Mixer", "Mic2 Playback Switch", "Mic2 PGA"},
		{"MoNo Mixer", "DAC Mixer Playback Switch", "DAC Mixer"},
		{"MoNo Mixer", "Voice DAC Playback Switch", "VoDAC PGA"},
		
		/*hp mixer*/
		{"HP Mixer", NULL, "Left HP Mixer"},
		{"HP Mixer", NULL, "Right HP Mixer"},

		/*spkout mux*/
	//	{"SPKOUT Mux", "HP Mixer", "HP Mixer"},
	//	{"SPKOUT Mux", "SPK Mixer", "SPK Mixer"},
	//	{"SPKOUT Mux", "Mono Mixer", "MoNo Mixer"},
		{"SPKOUT Mux", NULL, "HP Mixer"},
		{"SPKOUT Mux", NULL, "SPK Mixer"},
		{"SPKOUT Mux", NULL, "MoNo Mixer"},
		
		/*hpl out mux*/
		{"HPLOUT Mux", "HPL Mixer", "Left HP Mixer"},
		
		/*hpr out mux*/
		{"HPROUT Mux", "HPR Mixer", "Right HP Mixer"},

		/*aux out mux*/
		{"AUXOUT Mux", "HP Mixer", "HP Mixer"},
		{"AUXOUT Mux", "SPK Mixer", "SPK Mixer"},
		{"AUXOUT Mux", "Mono Mixer", "MoNo Mixer"},

		/*spkl out pga*/
		{"SPKL Out PGA", NULL, "SPKOUT Mux"},
		
		
		/*spkr out pga*/
		{"SPKR Out PGA", NULL, "SPKOUT Mux"},
		
		/*hpl out pga*/
		{"HPL Out PGA", NULL, "HPLOUT Mux"},

		/*hpr out pga*/
		{"HPR Out PGA", NULL, "HPROUT Mux"},

		/*aux out pga*/
		{"AUX Out PGA", NULL, "AUXOUT Mux"}, 
		
		/*left adc*/
		{"Left ADC", NULL, "Left Rec Mixer"},
		
		/*right adc*/
		{"Right ADC", NULL, "Right Rec Mixer"},
		
		/*output*/
		{"SPKL", NULL, "SPKL Out PGA"},
		{"SPKR", NULL, "SPKR Out PGA"},
		{"HPL", NULL, "HPL Out PGA"},
		{"HPR", NULL, "HPR Out PGA"},
		{"AUX", NULL, "AUX Out PGA"},
};


static int rt5625_add_widgets(struct snd_soc_codec *codec)
{
	snd_soc_dapm_new_controls(codec, rt5625_dapm_widgets, 
				ARRAY_SIZE(rt5625_dapm_widgets));
	snd_soc_dapm_add_routes(codec, audio_map, ARRAY_SIZE(audio_map));
	snd_soc_dapm_new_widgets(codec);
}

#endif
#if !USE_DAPM_CTRL

static enum OUTPUT_DEVICE_MASK
{
	SPK_OUTPUT_MASK = 1,
	HP_OUTPUT_MASK,
};



static int rt5625_set_path_from_dac_to_output(struct snd_soc_codec *codec, int enable, int sink)
{	s3cdbg( "enter %s\n", __func__);	
	switch (sink)
	{
		case SPK_OUTPUT_MASK:
			if (enable)
			{	
				rt5625_write_mask(codec, 0x10, 0x000c, 0x000c);          /*unmute dac-->hpmixer*/
				rt5625_write_mask(codec, 0x1c, 0x0400, 0x0400);  	 /*choose mux to hpmixer-->spk  */		
				rt5625_write_mask(codec, 0x3c, 0x0300, 0x0300);  	/*power up dac lr */
				rt5625_write_mask(codec, 0x3c, 0x0030, 0x0030);  	/*power up hp mixer lr*/
				rt5625_write_mask(codec, 0x3c, 0x3000, 0x3000);      /*power up spk lr*/
				
			}
			else
				rt5625_write_mask(codec, 0x02, 0x8080, 0x8080);      /*mute spk*/			
			break;
		case HP_OUTPUT_MASK:
			if (enable)
			{
				rt5625_write_mask(codec, 0x10, 0x000c, 0x000c);          /*unmute dac-->hpmixer*/
				rt5625_write_mask(codec, 0x1c, 0x0300, 0x0300);          /*hpmixer to hp out*/
				rt5625_write_mask(codec, 0x3c, 0x0300, 0x0300);  	/*power up dac lr */
				rt5625_write_mask(codec, 0x3c, 0x0030, 0x0030);  	/*power up hp mixer lr*/
				hp_depop_mode2(codec);
			}
			else
			{
				rt5625_write_mask(codec, 0x3a, 0x0000, 0x0300);                     /*power off hp amp*/
				rt5625_write_mask(codec, 0x04, 0x8080, 0x8080);
			}
			break;
		default:
		return 0;
	}
	return 0;
}


#endif

#if !USE_DAPM_CTRL
rt5625_pcm_hw_prepare(struct snd_pcm_substream *substream, struct snd_soc_dai *codec_dai)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	int stream = substream->stream;

	s3cdbg( "enter %s\n", __func__);	
	switch (stream)
	{
		case SNDRV_PCM_STREAM_PLAYBACK:	
			rt5625_write_mask(codec, 0x3a, 0xc000, 0xc000);
			rt5625_write_mask(codec, 0x3c, 0x0330, 0x0330);        /*power daclr*/
			hp_depop_mode2(codec);
			rt5625_write_mask(codec, 0x3c, 0x3000, 0x3000);       /*power spklr volume*/
			rt5625_write_mask(codec, 0x04, 0x0000, 0x8080);        /*unmute hp*/
			rt5625_write_mask(codec, 0x02, 0x0000, 0x8080);        /*unmute spk*/
			rt5625_write_mask(codec, 0x3a, 0x0400, 0x0400);        /*power on classabd amp*/	
			break;
		case SNDRV_PCM_STREAM_CAPTURE:			
			rt5625_write_mask(codec, 0x3e, 0x0002, 0x0002);        /*power mic1 boost*/
			rt5625_write_mask(codec, 0x3c, 0x0003, 0x0003);        /*power adc rec mixer lr*/
			rt5625_write_mask(codec, 0x3c, 0x00c0, 0x00c0);         /*power adc lr*/
			break;
		default:
			return 0;
	}
	return 0;	
}
#endif
struct _pll_div{
	u32 pll_in;
	u32 pll_out;
	u16 regvalue;
};


/**************************************************************
  *	watch out!
  *	our codec support you to select different source as pll input, but if you 
  *	use both of the I2S audio interface and pcm interface instantially. 
  *	The two DAI must have the same pll setting params, so you have to offer
  *	the same pll input, and set our codec's sysclk the same one, we suggest 
  *	24576000.
  **************************************************************/
static const struct _pll_div codec_master_pll1_div[] = {
		
	{  2048000,  24576000,	0x2ea0},
	{  3686400,  24576000,	0xee27},	
	{ 12000000,  24576000,	0x2915},   
	{ 13000000,  24576000,	0x772e},
	{ 13100000,	 24576000,	0x0d20},	
	{18432000, 24576000, 0x0290},
	{12288000, 24576000, 0x0490},
	{7872000, 24576000, 0x6519},
	{6144000, 24576000, 0x0a90},
	{4096000, 24576000, 0x1090},	
	{2048000,  22579200,	0x2ba0},//for 44.1kHz
	{3686400,  22579200,	0x4821},	
	{12000000,22579200,	0x4227},   
	{13000000,22579200,	0x4428},
	{4096000, 22579200, 0x4021},	
};

static const struct _pll_div codec_bclk_pll1_div[] = {

	{  1536000,	 24576000,	0x3ea0 },	//0x3ea0
	{  3072000,	 24576000,	0x1ea0},
};

static const struct _pll_div codec_vbclk_pll1_div[] = {

	{  1536000,	 24576000,	0x3ea0},	
	{  3072000,	 24576000,	0x1ea0},
};


struct _coeff_div_stereo {
	unsigned int mclk;
	unsigned int rate;
	unsigned int reg60;
	unsigned int reg62;
};

struct _coeff_div_voice {
	unsigned int mclk;
	unsigned int rate;
	unsigned int reg64;
};

static const struct _coeff_div_stereo coeff_div_stereo[] = {
		/*bclk is config to 32fs, if codec is choose to be slave mode , input bclk should be 32*fs */
		{24576000, 48000, 0x3174, 0x1010},      
		{24576000, 48000, 0x3075, 0x1010},  
		//{24576000, 48000, 16000,0x30B7, 0x1022}, 
		//{24576000, 48000, 16000,0x31B4, 0x1022}, 
		//{24576000, 48000, 8000,0x30B9, 0x1024}, 
		//{24576000, 48000, 8000,0x31B6, 0x1024}, 
		{22579200, 44100, 0x3075, 0x1010},		//for 44100Hz sampling rate , both DAC and ADC
		{22579200, 44100, 0x3174, 0x1010}, 
		//{22579200, 44100, 22050, 0x3077, 0x1012},
		//{22579200, 44100, 22050, 0x3174, 0x1012}, 
		//{22579200, 44100, 11025, 0x3079, 0x1014},
		//{22579200, 44100, 11025, 0x3176, 0x1014}, 
		{0, 0, 0, 0},
};

static const struct _coeff_div_voice coeff_div_voice[] = {
		/*bclk is config to 32fs, if codec is choose to be slave mode , input bclk should be 32*fs */
		{24576000, 16000, 0x2622}, 
		{24576000, 8000, 0x2824},
		{0, 0, 0},
};

static int get_coeff(unsigned int mclk, unsigned int rate, int mode)
{
	int i;

	//return 1;//wangkai
	
	s3cdbg( "enter %s\n", __func__);	
	s3cdbg("get_coeff mclk = %d, rate = %d, mode=%d\n", mclk, rate, mode);
	if (!mode)
	{
		for (i = 0; i < ARRAY_SIZE(coeff_div_stereo); i++) {
			if ((coeff_div_stereo[i].rate == rate) && (coeff_div_stereo[i].mclk == mclk))
				return i;
		}
	}
	else {
		for (i = 0; i< ARRAY_SIZE(coeff_div_voice); i++) {
			if ((coeff_div_voice[i].rate == rate) && (coeff_div_voice[i].mclk == mclk))
				return i;
		}
	}


	printk(KERN_ERR "can't find a matched mclk and rate in %s\n", 
				(mode ? "coeff_div_voice[]" : "coeff_div_audio[]"));
	return -EINVAL;
}


static int startCodecPlayback(struct snd_soc_codec *codec)
{
	int ret = 0;
	s3cdbg("%s",__FUNCTION__);		

	//power on I2S and VoDSP Digital interface, DAC Vref and main bias power
	rt5625_write(codec, RT5625_PWR_MANAG_ADD1, (rt5625_read_reg_cache(codec,RT5625_PWR_MANAG_ADD1) | 0x0803));

	//power on PLL1 and Vref for all analog circuit
	rt5625_write(codec, RT5625_PWR_MANAG_ADD2, (rt5625_read_reg_cache(codec,RT5625_PWR_MANAG_ADD2) | 0xA000));
	
	rt5625_write(codec, RT5625_PWR_MANAG_ADD1, (rt5625_read_reg_cache(codec,RT5625_PWR_MANAG_ADD1) | 0xc000));

	//power daclr
	rt5625_write(codec, RT5625_PWR_MANAG_ADD2, (rt5625_read_reg_cache(codec,RT5625_PWR_MANAG_ADD2) | 0x0300));

	rt5625_write_mask(codec, RT5625_PD_CTRL_STAT, 0,0xff00);
	
	set_audio_out_path(); 
	return ret;
}

static int stopCodecPlayback(struct snd_soc_codec *codec)
{
	int ret = 0;
	s3cdbg("%s",__FUNCTION__);		
	unsigned int tmp;

	if (gSpeakerState == SPEAKER_ON)
	{
		regulator_disable(speaker_5v_regulator);
		gSpeakerState = SPEAKER_OFF;
	}
	
	if (!CaptureStreamNum && !PlaybackStreamNum)
	{
		//power off I2S and VoDSP Digital interface, DAC Vref and main power bias	
		rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD1, 0x0, 0x0803);
	 	//power off PLL1 and Vref for all analog circuit
		rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD2, 0x0, 0xA000);

		rt5625_write_mask(codec, RT5625_PD_CTRL_STAT, 0xff00,0xff00);
	}
	
	
	 rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD1,0x0, 0xc000);	             
	 //power off  hp/spk mixer
	 rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD2, 0x0, 0x0330);

	 //power off spk amp
	  rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD1, 0x0, 0x0400);

	 //power off spk/hp out L/R volume
	 tmp = tmp= rt5625_read(codec, RT5625_PWR_MANAG_ADD3);
	 tmp &=~(0xf<<10);  
 	 rt5625_write(codec, RT5625_PWR_MANAG_ADD3, tmp);

	 tmp= rt5625_read(codec, RT5625_PD_CTRL_STAT);
	 tmp |= (1<<15)|(1<<14)|(1<<9);								// power off speaker amplifier and hp out and aumux
	 rt5625_write(codec, RT5625_PD_CTRL_STAT, tmp);
	
	return ret;
}

static int startCodecCapture(struct snd_soc_codec *codec)
{
	unsigned int tmp;
	printk("%s\n",__FUNCTION__);

	 rt5625_write_mask(codec, RT5625_PD_CTRL_STAT, 0,0xff00);

	 tmp= rt5625_read(codec, RT5625_DAC_ADC_VODAC_FUN_SEL);
 	 tmp &= ~0x3f; 								//0x2e, L/R ADC selected as stereo adc
 	 rt5625_write(codec, RT5625_DAC_ADC_VODAC_FUN_SEL, tmp);

	 tmp= rt5625_read(codec, RT5625_PWR_MANAG_ADD1);
	 tmp |=((0x1<<11)|(0x1<<5)|(0x1<<3)|(0x1<<1));  
 	 rt5625_write(codec, RT5625_PWR_MANAG_ADD1, tmp);

	 tmp= rt5625_read(codec, RT5625_PWR_MANAG_ADD2);
	 tmp |=((0x1<<15)|(PWR_MIXER_VREF)|(0x3<<6)|(0x3<<0));  
 	 rt5625_write(codec, RT5625_PWR_MANAG_ADD2, tmp);

	 tmp= rt5625_read(codec, RT5625_PWR_MANAG_ADD3);
	 tmp |=((0x1<<3)|(0x1<<1));  
 	 rt5625_write(codec, RT5625_PWR_MANAG_ADD3, tmp);

/*recording route from MIC1 to stereo IIS*/
	
	 rt5625_write(codec, RT5625_MIC_VOL, 0x0000); //0x0e, use mic1, single ended input

	 tmp= rt5625_read(codec, RT5625_MIC_CTRL);
	 //tmp |=0x2<<10;		//0x22, mic boot 30db, micbias=0.9AVDD
	  tmp &=~(1<<5); 
	  tmp |=0x3<<10;  		//0x22, mic boot 40db, micbias=0.9AVDD
 	 rt5625_write(codec, RT5625_MIC_CTRL, tmp);  

	 tmp = rt5625_read(codec,RT5625_DMIC_CTRL);
	 tmp |=0x4<<0;		//DMIC boost control (ADC digital boost), +24dB
 	 rt5625_write(codec, RT5625_DMIC_CTRL, tmp); 

 	 tmp= rt5625_read(codec, RT5625_ADC_REC_MIXER);
	 tmp &= ~(0x1<<6|0x1<<14);					//0x14, select inpurt for ADC
	 rt5625_write(codec, RT5625_ADC_REC_MIXER, tmp);

 	 

	 tmp= rt5625_read(codec, RT5625_VODSP_PDM_CTL);
	 tmp &= ~(0x3<<4);							//0x1a, IIS source select ADC L/R
	 rt5625_write(codec, RT5625_VODSP_PDM_CTL, tmp);
	
	return 0;
}


static int stopCodecCapture(struct snd_soc_codec *codec)
{
	unsigned int tmp;
	printk("%s\n",__FUNCTION__);

	if (!CaptureStreamNum && !PlaybackStreamNum)
	{
		//power off I2S and VoDSP Digital interface, DAC Vref and main power bias	
		rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD1, 0x0, 0x0803);
		//power off PLL1 and VRef for all analog circuit
		rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD2, 0x0, 0xA000);

		rt5625_write_mask(codec, RT5625_PD_CTRL_STAT, 0xff00,0xff00);
	}
	
	 tmp= rt5625_read(codec, RT5625_PWR_MANAG_ADD1);
	 tmp &= ~((0x1<<11)|(0x1<<5)|(0x1<<3));  
 	 rt5625_write(codec, RT5625_PWR_MANAG_ADD1, tmp);

	 tmp= rt5625_read(codec, RT5625_PWR_MANAG_ADD2);
	 tmp &=~((0x3<<6)|(0x3<<0));  
 	 rt5625_write(codec, RT5625_PWR_MANAG_ADD2, tmp);

	 tmp= rt5625_read(codec, RT5625_PWR_MANAG_ADD3);
	 tmp &=~((0x1<<3)|(0x1<<1));  
 	 rt5625_write(codec, RT5625_PWR_MANAG_ADD3, tmp);

	 tmp= rt5625_read(codec, RT5625_MIC_CTRL);
	 tmp &= ~(0x3<<10);  
 	 rt5625_write(codec, RT5625_MIC_CTRL, tmp); 

	 tmp= rt5625_read(codec, RT5625_PD_CTRL_STAT);
	 tmp |= (1<<8);								// power off stereo adc and input mux
	 rt5625_write(codec, RT5625_PD_CTRL_STAT, tmp);
	
	return 0;
}
static struct work_struct playback_start_wq;
static struct work_struct playback_stop_wq;
static struct work_struct capture_start_wq;
static struct work_struct capture_stop_wq;

static void rt5625_playback_start_wq(void)
{
	struct snd_soc_device *socdev = rt5625_socdev;
	struct snd_soc_codec *codec = socdev->card->codec;
	
	startCodecPlayback(codec);
}

static void rt5625_playback_stop_wq(void)
{
	struct snd_soc_device *socdev = rt5625_socdev;
	struct snd_soc_codec *codec = socdev->card->codec;
	
	stopCodecPlayback(codec);
}

static void rt5625_capture_start_wq(void)
{
	struct snd_soc_device *socdev = rt5625_socdev;
	struct snd_soc_codec *codec = socdev->card->codec;
	
	startCodecCapture(codec);
}

static void rt5625_capture_stop_wq(void)
{
	struct snd_soc_device *socdev = rt5625_socdev;
	struct snd_soc_codec *codec = socdev->card->codec;
	
	stopCodecCapture(codec);
}

//unsigned int rt5625_record_status=RT5625_IDLE;

/**
 * rt5625_config_record_params - config all the registers of rt5625 necessary for recording
 *some settings may not be necessary, because they have been done
 *in previous probe function.
 *However, they are set again here to err on the safe side.
 */
static int rt5625_config_record_params(struct snd_soc_codec *codec)
{
	 unsigned int tmp;
	 ///printk("%s\n",__FUNCTION__);

	//rt5625_record_status=RT5625_CAPTURE;
#if 0
	//reset
	 rt5625_write(codec, 0x00, 0x0000); 
	 udelay(100);

/*set AP as master*/
	
 	 tmp= rt5625_read(codec, RT5625_MAIN_SDP_CTRL);
 	 tmp |= 0x1<<15;
 	 rt5625_write(codec, RT5625_MAIN_SDP_CTRL, tmp);
  	 tmp= rt5625_read(codec, RT5625_MAIN_SDP_CTRL);
	 //printk("rt5625 0x34 = %x\n", tmp);
	 

/*power on all the necessary blocks*/

	 tmp= rt5625_read(codec, RT5625_PD_CTRL_STAT);
	 tmp &=~(1<<8);								//0x26, power on stereo adc and input mux
	 rt5625_write(codec, RT5625_PD_CTRL_STAT, 0x0000);

	 tmp= rt5625_read(codec, 0x3a);
	 tmp |=(0x1<<11|0x3<<4|0xa);  
 	 rt5625_write(codec, 0x3a, 0xffff);

	 rt5625_write(codec, 0x3c, 0xffff);
	 
	 tmp= rt5625_read(codec, 0x3e);
	 tmp |=0xf;  
 	 rt5625_write(codec, 0x3e, 0xffff);
/*recording route from MIC1 to stereo IIS*/
	
	 rt5625_write(codec, RT5625_MIC_VOL, 0x0000); //0x0e, use mic1, single ended input

 	 rt5625_write(codec, RT5625_MIC_CTRL, 0x0A00); //0x22, mic boot 30db, micbias=0.9AVDD

 	 tmp= rt5625_read(codec, RT5625_ADC_REC_MIXER);
	 tmp &= ~(0x1<<6|0x1<<14);					//0x14, select inpurt for ADC
	 rt5625_write(codec, RT5625_ADC_REC_MIXER, tmp);

 	 tmp= rt5625_read(codec, RT5625_DAC_ADC_VODAC_FUN_SEL);
 	 tmp &= ~0x3f; 								//0x2e, L/R ADC selected as stereo adc
 	 rt5625_write(codec, RT5625_DAC_ADC_VODAC_FUN_SEL, tmp);

	 tmp= rt5625_read(codec, RT5625_VODSP_PDM_CTL);
	 tmp &= ~(0x3<<4);							//0x1a, IIS source select
	 rt5625_write(codec, RT5625_VODSP_PDM_CTL, tmp);

/* playback route from DAC to HP*/

#if 1
	 tmp= rt5625_read(codec, RT5625_DAC_AND_MIC_CTRL); 
	 tmp |=(0x1<<15|0x1<<11);					//0x10, mute mic1,2 input to HP mixer
 	 tmp &=~(0x1<<3|0x1<<2);					      //0x10,  enable DAC L/R output to HP mixer
	 rt5625_write(codec, RT5625_DAC_AND_MIC_CTRL, tmp);

	 tmp= rt5625_read(codec, RT5625_ADC_REC_GAIN);
	 tmp |=(0x1<<7|0x1<<15);					 //0x12, mute ADC output to HP mixer
	 rt5625_write(codec, RT5625_ADC_REC_GAIN, tmp); 

 	 tmp= rt5625_read(codec, RT5625_OUTPUT_MIXER_CTRL); 
	 tmp |=(0x1<<9|0x1<<8);						//0x1c, HP L/R mixer to HP amp
	 tmp &=~0x3;									//disable dac direct to HP amp
	 tmp |=(0x1<<10);							//0x1c, SPKVolume Input Select
	 tmp &=~(0x7<<13);							//0x1c, CLASS ABD select,SPK Output Type Select
	 rt5625_write(codec, RT5625_OUTPUT_MIXER_CTRL, tmp);

 	 rt5625_write(codec, RT5625_HP_OUT_VOL, 0x0000); //0x04, HP volume and mute setting
  	 rt5625_write(codec, RT5625_SPK_OUT_VOL, 0x0000); //0x02, SPK volume and mute setting
 #endif
#else

	 printk("%s\n",__FUNCTION__);

	 tmp= rt5625_read(codec, RT5625_PD_CTRL_STAT);
	 tmp &=~(1<<8);								//0x26, power on stereo adc and input mux
	 rt5625_write(codec, RT5625_PD_CTRL_STAT, tmp);

	 tmp= rt5625_read(codec, RT5625_DAC_ADC_VODAC_FUN_SEL);
 	 tmp &= ~0x3f; 								//0x2e, L/R ADC selected as stereo adc
 	 rt5625_write(codec, RT5625_DAC_ADC_VODAC_FUN_SEL, tmp);

	 tmp= rt5625_read(codec, RT5625_PWR_MANAG_ADD1);
	 tmp |=((0x1<<11)|(0x1<<5)|(0x1<<3)|(0x1<<1));  
 	 rt5625_write(codec, RT5625_PWR_MANAG_ADD1, tmp);

	 tmp= rt5625_read(codec, RT5625_PWR_MANAG_ADD2);
	 tmp |=((0x3<<6)|(0x3<<0));  
 	 rt5625_write(codec, RT5625_PWR_MANAG_ADD2, tmp);

	 tmp= rt5625_read(codec, RT5625_PWR_MANAG_ADD3);
	 tmp |=((0x1<<3)|(0x1<<1));  
 	 rt5625_write(codec, RT5625_PWR_MANAG_ADD3, tmp);

/*recording route from MIC1 to stereo IIS*/
	
	 rt5625_write(codec, RT5625_MIC_VOL, 0x0000); //0x0e, use mic1, single ended input

	 tmp= rt5625_read(codec, RT5625_MIC_CTRL);
	 tmp |=0x2<<10;  
 	 rt5625_write(codec, RT5625_MIC_CTRL, tmp);  //0x22, mic boot 30db, micbias=0.9AVDD

 	 tmp= rt5625_read(codec, RT5625_ADC_REC_MIXER);
	 tmp &= ~(0x1<<6|0x1<<14);					//0x14, select inpurt for ADC
	 rt5625_write(codec, RT5625_ADC_REC_MIXER, tmp);

 	 

	 tmp= rt5625_read(codec, RT5625_VODSP_PDM_CTL);
	 tmp &= ~(0x3<<4);							//0x1a, IIS source select ADC L/R
	 rt5625_write(codec, RT5625_VODSP_PDM_CTL, tmp);

/* playback route from DAC to HP*/

#if 1
	 tmp= rt5625_read(codec, RT5625_DAC_AND_MIC_CTRL); 
	 tmp |=(0x1<<15|0x1<<11);					//0x10, mute mic1,2 input to HP mixer
 	 tmp &=~(0x1<<3|0x1<<2);					      //0x10,  enable DAC L/R output to HP mixer
	 rt5625_write(codec, RT5625_DAC_AND_MIC_CTRL, tmp);

	 tmp= rt5625_read(codec, RT5625_ADC_REC_GAIN);
	 tmp |=(0x1<<7|0x1<<15);					 //0x12, mute ADC output to HP mixer
	 rt5625_write(codec, RT5625_ADC_REC_GAIN, tmp); 

 	 tmp= rt5625_read(codec, RT5625_OUTPUT_MIXER_CTRL); 
	 tmp |=(0x1<<9|0x1<<8);						//0x1c, HP L/R mixer to HP amp
	 tmp &=~0x3;									//disable dac direct to HP amp
	 tmp |=(0x1<<10);							//0x1c, SPKVolume Input Select
	 tmp &=~(0x7<<13);							//0x1c, CLASS ABD select,SPK Output Type Select
	 rt5625_write(codec, RT5625_OUTPUT_MIXER_CTRL, tmp);

 	 rt5625_write(codec, RT5625_HP_OUT_VOL, 0x0000); //0x04, HP volume and mute setting
  	 rt5625_write(codec, RT5625_SPK_OUT_VOL, 0x0000); //0x02, SPK volume and mute setting
 #endif

 	printk("RT5625_PWR_MANAG_ADD1:%x\n",rt5625_read(codec, RT5625_PWR_MANAG_ADD1));
	printk("RT5625_PWR_MANAG_ADD2:%x\n",rt5625_read(codec, RT5625_PWR_MANAG_ADD2));
 	printk("RT5625_PWR_MANAG_ADD3:%x\n",rt5625_read(codec, RT5625_PWR_MANAG_ADD3));
	printk("RT5625_GEN_CTRL_REG1:%x\n",rt5625_read(codec, RT5625_GEN_CTRL_REG1));
	printk("RT5625_GEN_CTRL_REG2:%x\n",rt5625_read(codec, RT5625_GEN_CTRL_REG2));
 	printk("RT5625_MAIN_SDP_CTRL:%x\n",rt5625_read(codec, RT5625_MAIN_SDP_CTRL));
#endif
  	 return 0;
} 

static int rt5625_startup(struct snd_pcm_substream *substream)
{
	s3cdbg( "enter %s\n", __func__);
	
	return 0;
}

static int rt5625_trigger(struct snd_pcm_substream *substream, 
	int cmd, struct snd_soc_dai *dai)
{
    int    ret = 0;
    struct snd_soc_codec  *codec = rt5625_socdev->card->codec;

    s3cdbg("Enter %s,substream=%d  cmd=%d\n", __FUNCTION__, substream->stream,cmd);  
    switch (cmd) 
    {
     case SNDRV_PCM_TRIGGER_START:
     case SNDRV_PCM_TRIGGER_RESUME:
     case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
	 	 mSubStream = substream;
     		if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
     		{
     			CaptureStreamNum++;
			printk("Record start\n");
			schedule_work(&capture_start_wq);
     		}
     		else
     		{
			PlaybackStreamNum++;
			schedule_work(&playback_start_wq);
     		}
     		break;

     case SNDRV_PCM_TRIGGER_STOP:
     case SNDRV_PCM_TRIGGER_SUSPEND:
     case SNDRV_PCM_TRIGGER_PAUSE_PUSH:

	  	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE){
			//rt5625_record_status=RT5625_IDLE;
			CaptureStreamNum--;
		     	printk("Record Completed.\n");
			schedule_work(&capture_stop_wq);
	  	}
	 	else
	 	{
	 		PlaybackStreamNum--;
	 		schedule_work(&playback_stop_wq);
		 	printk("Playback Completed.\n");
	 	}
		mSubStream = NULL;

		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int rt5625_codec_set_dai_pll(struct snd_soc_dai *codec_dai, 
		int pll_id, unsigned int freq_in, unsigned int freq_out)
{
	int i;
	int ret = -EINVAL;
	struct snd_soc_codec *codec = codec_dai->codec;

	pr_debug( "enter %s,pll_id:%d,freq_in:%ld,freq_out:%ld\n", __func__,pll_id,freq_in,freq_out);

	if (pll_id < RT5625_PLL1_FROM_MCLK || pll_id > RT5625_PLL1_FROM_VBCLK)
		return -EINVAL;

	if (!freq_in || !freq_out)
		return 0;

	if (RT5625_PLL1_FROM_MCLK == pll_id)
	{
		for (i = 0; i < ARRAY_SIZE(codec_master_pll1_div); i ++)
		{
			if (((freq_in == codec_master_pll1_div[i].pll_in) ||(freq_in/2 == codec_master_pll1_div[i].pll_in)) && (freq_out == codec_master_pll1_div[i].pll_out))
			{
				if (freq_in == codec_master_pll1_div[i].pll_in)
					rt5625_write(codec, RT5625_GEN_CTRL_REG2, 0x0000); 		//select MCLK as the source clock of PLL1                 			 /*PLL source from MCLK*/
				else
					rt5625_write(codec, RT5625_GEN_CTRL_REG2, 0x0001); 		//select MCLK as the source clock of PLL1                 			 /*PLL source from MCLK and pre-scale mclk*/
				rt5625_write(codec, RT5625_PLL_CTRL, codec_master_pll1_div[i].regvalue);   /*set pll code*/
				rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD2, 0x8000, 0x8000);	// power on  pll1
				ret = 0;
			}
		}
	}
	else if (RT5625_PLL1_FROM_BCLK == pll_id)
	{
		for (i = 0; i < ARRAY_SIZE(codec_bclk_pll1_div); i ++)
		{
			if ((freq_in == codec_bclk_pll1_div[i].pll_in) && (freq_out == codec_bclk_pll1_div[i].pll_out))
			{
				rt5625_write(codec, RT5625_GEN_CTRL_REG2, 0x2000);    					/*PLL source from BCLK*/
				rt5625_write(codec, RT5625_PLL_CTRL, codec_bclk_pll1_div[i].regvalue);   /*set pll1 code*/
				rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD2, 0x8000, 0x8000);       	 /*enable pll1 power*/
				ret = 0;
			}
		}
	}
	else if (RT5625_PLL1_FROM_VBCLK == pll_id)
	{
		for (i = 0; i < ARRAY_SIZE(codec_vbclk_pll1_div); i ++)
		{
			if ((freq_in == codec_vbclk_pll1_div[i].pll_in) && (freq_out == codec_vbclk_pll1_div[i].pll_out))
			{
				rt5625_write(codec, RT5625_GEN_CTRL_REG2, 0x2000);    					/*PLL source from BCLK*/
				rt5625_write(codec, RT5625_PLL_CTRL, codec_vbclk_pll1_div[i].regvalue);   /*set pll1 code*/
				rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD2, 0x8000, 0x8000);       	 /*enable pll1 power*/
				ret = 0;
			}
		}
	}
	
	rt5625_write_mask(codec, RT5625_GEN_CTRL_REG1, 0x8000, 0x8000);		//select PLL1 output as the source of Main SYSCLK
	return ret;
}

static int rt5625_set_aec_sysclk(struct snd_soc_codec *codec, unsigned int mode)
{
	unsigned int pll_in;
	int i;
	s3cdbg( "enter %s\n", __func__);	
	pll_in = rt5625_aec_cfg->pll_input;

	if (pll_in < 2000000 || pll_in > 40000000) {
		printk(KERN_ERR "your pll input isn't matched, not a proper one\n");
		return 0;
	}

	if (mode != VODSP_AEC_DISABLE) {
		for (i = 0; i < ARRAY_SIZE(codec_bclk_pll1_div); i ++)
		{
			if ((pll_in == codec_bclk_pll1_div[i].pll_in) && (24576000 == codec_bclk_pll1_div[i].pll_out))
			{
				rt5625_write(codec, RT5625_GEN_CTRL_REG2, 0x2000);    					/*PLL source from BCLK*/
				rt5625_write(codec, RT5625_PLL_CTRL, codec_bclk_pll1_div[i].regvalue);   /*set pll1 code*/
				rt5625_write_mask(codec, RT5625_PWR_MANAG_ADD2, 0x8000, 0x8000);       	 /*enable pll1 power*/
	
			}
		}
		rt5625_write_mask(codec, 0x3a, 0x8000, 0x8000);     //enable pll
		rt5625_write_mask(codec, 0x3a, 0x0001, 0x0001);    //enable dac ref
		rt5625_write_mask(codec, 0x42, 0x0000, 0x3000);   //pll1 source from mclk
		rt5625_write_mask(codec, 0x40, 0x8000, 0x8000);    //main sysclk from pll1
		
	} else {
		rt5625_write_mask(codec, 0x3a, 0x0000, 0x8000);     //disable pll
		rt5625_write_mask(codec, 0x3a, 0x0000, 0x0001);    //disable dac ref
	}
	return 0;
	
}
static int rt5625_hifi_codec_set_dai_sysclk(struct snd_soc_dai *codec_dai, 
		int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	struct rt5625_priv * rt5625 = codec->drvdata;
	s3cdbg( "enter %s\n", __func__);
	
	switch (freq) {
	case 22579200:
	case 24576000:
		if (clk_id == RT5625_MCLK) {
		rt5625->stereo_sysclk = freq;
		return 0;
		} else if (clk_id == RT5625_PCMCLK) {
			rt5625->voice_sysclk = freq;
			return 0;
		}
		return 0;
	default:
		printk(KERN_ERR "unsupported sysclk freq %u for audio i2s\n", freq);
		return -EINVAL;;
	}
	
}

static int rt5625_voice_codec_set_dai_sysclk(struct snd_soc_dai *codec_dai, 
		int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	struct rt5625_priv * rt5625 = codec->drvdata;
	
	s3cdbg( "enter %s\n", __func__);	
	if ((freq >= (256 * 8000)) && (freq <= (512 * 48000))) 
		{
		rt5625->voice_sysclk = freq;
		return 0;
	}			

	printk(KERN_ERR "unsupported sysclk freq %u for voice pcm\n", freq);
	return -EINVAL;
}


static int rt5625_hifi_pcm_hw_params(struct snd_pcm_substream *substream, 
			struct snd_pcm_hw_params *params,
			struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_device *socdev = rtd->socdev;
	struct snd_soc_codec *codec = socdev ->card->codec;
	struct rt5625_priv *rt5625 = codec->drvdata;
	struct snd_soc_dapm_widget *w;
//	int stream = substream->stream;
	unsigned int iface = rt5625_read(codec, RT5625_MAIN_SDP_CTRL) & 0xfff3;
	int rate = params_rate(params); //to fix
	int coeff = get_coeff(rt5625->stereo_sysclk, rate, 0);
	unsigned int tmp;
	
	pr_debug("enter %s\n", __func__);

	list_for_each_entry(w, &codec->dapm_widgets, list)
	{
		if (!w->sname)
			continue;
		if (!strcmp(w->name, "Right ADC"))
			strcpy(w->sname, "Right ADC HiFi Capture");
	}
	
	iface &= ~(0xc);
	switch (params_format(params))
	{
		case SNDRV_PCM_FORMAT_S16_LE:
			break;
		case SNDRV_PCM_FORMAT_S20_3LE:
			iface |= 0x0004;
		case SNDRV_PCM_FORMAT_S24_LE:
			iface |= 0x0008;
		case SNDRV_PCM_FORMAT_S8:
			iface |= 0x000c;
	}


	rt5625_write(codec, RT5625_MAIN_SDP_CTRL, iface);
	rt5625_write_mask(codec, 0x3a, 0x0801, 0x0801);   /*power i2s and dac ref*/
	if (coeff >= 0) {
		pr_debug("coeff == %d,reg60:%08x\n", coeff,coeff_div_stereo[coeff].reg60);
		rt5625_write(codec, RT5625_STEREO_DAC_CLK_CTRL1, coeff_div_stereo[coeff].reg60);
		rt5625_write(codec, RT5625_STEREO_DAC_CLK_CTRL2, coeff_div_stereo[coeff].reg62);
	}
	else
		return coeff;

	return 0;
}

static int rt5625_voice_pcm_hw_params(struct snd_pcm_substream *substream, 
			struct snd_pcm_hw_params *params,
			struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_device *socdev = rtd->socdev;
	struct snd_soc_codec *codec = socdev ->card->codec;
	struct rt5625_priv *rt5625 = codec->drvdata;
	struct snd_soc_dapm_widget *w;
	unsigned int iface = rt5625_read(codec, RT5625_EXTEND_SDP_CTRL) & 0xfff3;
	int rate = params_rate(params);
	int coeff = get_coeff(rt5625->voice_sysclk, rate, 1);

	s3cdbg(KERN_DEBUG "enter %s\n", __func__);

	list_for_each_entry(w, &codec->dapm_widgets, list)
	{
		if (!w->sname)
			continue;
		if (!strcmp(w->name, "Right ADC"))
			strcpy(w->sname, "Right ADC Voice Capture");
	}
	
	switch (params_format(params))
	{
		case SNDRV_PCM_FORMAT_S16_LE:
			break;
		case SNDRV_PCM_FORMAT_S20_3LE:
			iface |= 0x0004;
		case SNDRV_PCM_FORMAT_S24_LE:
			iface |= 0x0008;
		case SNDRV_PCM_FORMAT_S8:
			iface |= 0x000c;
	}
	rt5625_write_mask(codec, 0x3a, 0x0801, 0x0801);   /*power i2s and dac ref*/
	rt5625_write(codec, RT5625_EXTEND_SDP_CTRL, iface);
	if (coeff >= 0)
		rt5625_write(codec, RT5625_VOICE_DAC_PCMCLK_CTRL1, coeff_div_voice[coeff].reg64);
	else
		return coeff;
	return 0;
}


static int rt5625_hifi_codec_set_dai_fmt(struct snd_soc_dai *codec_dai, unsigned int fmt)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	//struct rt5625_priv *rt5625 = codec->drvdata;
	u16 iface = 0;


	pr_debug("enter into %s\n",__func__);	
	 iface = rt5625_read(codec, RT5625_MAIN_SDP_CTRL);

	/*set master/slave interface*/
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK)
	{
		case SND_SOC_DAIFMT_CBS_CFS:
			pr_debug("audio codec rt5625 iis worked as master mode\n");
			iface &= ~MAIN_I2S_MODE_SEL;		//codec worked as master
			break;
		case SND_SOC_DAIFMT_CBM_CFM:
			pr_debug("audio codec rt5625 iis worked as slave mode\n");
			iface |= MAIN_I2S_MODE_SEL;		//codec worked as slave
			break;
		default:
			printk("unsupport iis mode for rt5625\n");
			return -EINVAL;
	}

	iface &= ~MAIN_I2S_SADLRCK_CTRL; 		//ADC and DAC use the same sample rate 

	/*interface format*/
	iface &= ~(0x3);
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK)
	{
		case SND_SOC_DAIFMT_I2S:
			iface |= 0x0000;
			break;
		case SND_SOC_DAIFMT_LEFT_J:
			iface |= 0x0001;
			break;
		case SND_SOC_DAIFMT_DSP_A:
			iface |= 0x0002;
			break;
		case SND_SOC_DAIFMT_DSP_B:
			iface |= 0x0003;
			break;
		default:
			printk("unsupport interface format for rt5625\n");
			return -EINVAL;			
	}

	/*clock inversion*/
	switch (fmt & SND_SOC_DAIFMT_INV_MASK)
	{
		case SND_SOC_DAIFMT_NB_NF:
			iface |= 0x0000;
			break;
		case SND_SOC_DAIFMT_IB_NF:
			iface |= 0x0080;
			break;
		default:
			printk("unsupport clock mode for rt5625\n");
			return -EINVAL;
	}

	rt5625_write(codec, RT5625_MAIN_SDP_CTRL, iface);
	return 0;
}

static int rt5625_voice_codec_set_dai_fmt(struct snd_soc_dai *codec_dai, unsigned int fmt)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	//struct rt5625_priv *rt5625 = codec->drvdata;
	int iface;

	s3cdbg(KERN_DEBUG "enter %s\n", __func__);
	
	/*set slave/master mode*/
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK)
	{
		case SND_SOC_DAIFMT_CBM_CFM:
			iface = 0x0000;
			break;
		case SND_SOC_DAIFMT_CBS_CFS:
			iface = 0x4000;
			break;
		default:
			return -EINVAL;			
	}

	switch(fmt & SND_SOC_DAIFMT_FORMAT_MASK)
	{
		case SND_SOC_DAIFMT_I2S:
			iface |= 0x0000;
			break;
		case SND_SOC_DAIFMT_LEFT_J:
			iface |= 0x0001;
			break;
		case SND_SOC_DAIFMT_DSP_A:
			iface |= 0x0002;
			break;
		case SND_SOC_DAIFMT_DSP_B:
			iface |= 0x0003;
			break;
		default:
			return -EINVAL;		
	}

	/*clock inversion*/
	switch (fmt & SND_SOC_DAIFMT_INV_MASK)
	{
		case SND_SOC_DAIFMT_NB_NF:
			iface |= 0x0000;
			break;
		case SND_SOC_DAIFMT_IB_NF:
			iface |= 0x0080;
			break;
		default:
			return -EINVAL;			
	}

	iface |= 0x8000;      /*enable vopcm*/
	rt5625_write(codec, RT5625_EXTEND_SDP_CTRL, iface);	
	return 0;
}


static int rt5625_hifi_codec_mute(struct snd_soc_dai *dai, int mute)
{
	struct snd_soc_codec *codec = dai->codec;
	s3cdbg( "enter %s\n", __func__);		
	if (mute) 
		rt5625_write_mask(codec, RT5625_STEREO_DAC_VOL, 0x8080, 0x8080);
	else
		rt5625_write_mask(codec, RT5625_STEREO_DAC_VOL, 0x0000, 0x8080);
	return 0;
}

static int rt5625_voice_codec_mute(struct snd_soc_dai *dai, int mute)
{
	struct snd_soc_codec *codec = dai->codec;
	s3cdbg( "enter %s\n", __func__);	
	if (mute)
		rt5625_write_mask(codec, RT5625_VOICE_DAC_OUT_VOL, 0x1000, 0x1000);
	else 
		rt5625_write_mask(codec, RT5625_VOICE_DAC_OUT_VOL, 0x0000, 0x1000);
	return 0;
}


#if USE_DAPM_CTRL
static int rt5625_set_bias_level(struct snd_soc_codec *codec, 
			enum snd_soc_bias_level level)
{	s3cdbg( "enter %s\n", __func__);	
	switch(level) {
	case SND_SOC_BIAS_ON:
	case SND_SOC_BIAS_PREPARE:
		break;
	case SND_SOC_BIAS_STANDBY:
	case SND_SOC_BIAS_OFF:
#if 0
		rt5625_write_mask(codec, 0x3a, 0x0000, 0x0800);    /*disable main i2s*/
		rt5625_write_mask(codec, 0x3a, 0x0000, 0x0001);	/*disable dac ref*/
		rt5625_write_mask(codec, 0x3c, 0x0000, 0xc000);  /*off pll*/
#else
		rt5625_write_mask(codec, 0x04, 0x8080, 0x8080);        /*mute hp*/
		rt5625_write_mask(codec, 0x02, 0x8080, 0x8080);        /*mute spk*/
		rt5625_write(codec, 0x3e, 0x0000);					//power off all bit
		rt5625_write(codec, 0x3a, 0x0000);					//power off all bit
		rt5625_write(codec, 0x3c, 0x0000);					//power off all bit
#endif
		break;
	}
	codec->bias_level = level;
	return 0;
}
#else
static int rt5625_set_bias_level(struct snd_soc_codec *codec, 
			enum snd_soc_bias_level level)
{	s3cdbg( "enter %s\n", __func__);	
	switch(level) {
	case SND_SOC_BIAS_ON:
	case SND_SOC_BIAS_PREPARE:
		break;
	case SND_SOC_BIAS_STANDBY:
		break;
	case SND_SOC_BIAS_OFF:
#if 0		
		rt5625_write_mask(codec, 0x3a, 0x0000, 0x0800);    /*enable main i2s*/
		rt5625_write_mask(codec, 0x3a, 0x0000, 0x0001);	/*enable dac ref*/
		rt5625_write(codec, 0x3a, 0x0002);
		rt5625_write(codec, 0x3c, 0x2000);
		rt5625_write(codec, 0x3e, 0x0000);
#else
		rt5625_write_mask(codec, 0x04, 0x8080, 0x8080);        /*mute hp*/
		rt5625_write_mask(codec, 0x02, 0x8080, 0x8080);        /*mute spk*/
		rt5625_write(codec, 0x3e, 0x0000);					//power off all bit
		rt5625_write(codec, 0x3a, 0x0000);					//power off all bit
		rt5625_write(codec, 0x3c, 0x0000);					//power off all bit
#endif
		break;
	}
	codec->bias_level = level;
	return 0;
}
#endif

static int rt5625_hifi_pcm_shutdown(struct snd_pcm_substream *substream,
		struct snd_soc_dai *codec_dai)
{	s3cdbg( "enter %s\n", __func__);	
//	struct snd_soc_codec *codec = codec_dai->codec;

	#if !USE_DAPM_CTRL
	rt5625_write_mask(codec, 0x3a, 0x0000, 0x0300);
	rt5625_write_mask(codec, 0x02, 0x8080, 0x8080);
	rt5625_write_mask(codec, 0x04, 0x8080, 0x8080);
	rt5625_write(codec, 0x3a, 0x0002);
	rt5625_write(codec, 0x3c, 0x2000);
	rt5625_write(codec, 0x3e, 0x0000);
	#endif

	return 0;
}

static int rt5625_voice_pcm_shutdown(struct snd_pcm_substream *substream, 
		struct snd_soc_dai *codec_dai)
{
	struct snd_soc_codec *codec = codec_dai->codec;
	s3cdbg( "enter %s\n", __func__);	
	rt5625_write_mask(codec, 0x2e, 0x0000, 0x0030);
	
	#if !USE_DAPM_CTRL
	rt5625_write_mask(codec, 0x3a, 0x0000, 0x0300);
	rt5625_write_mask(codec, 0x02, 0x8080, 0x8080);
	rt5625_write_mask(codec, 0x04, 0x8080, 0x8080);
	rt5625_write(codec, 0x3a, 0x0002);
	rt5625_write(codec, 0x3c, 0x2000);
	rt5625_write(codec, 0x3e, 0x0000);
	#endif
	
	return 0;
}

#define RT5625_STEREO_RATES (SNDRV_PCM_RATE_48000|SNDRV_PCM_RATE_44100)
#define RT5626_VOICE_RATES (SNDRV_PCM_RATE_44100 |SNDRV_PCM_RATE_16000 | SNDRV_PCM_RATE_8000)

#define RT5625_FORMATS (SNDRV_PCM_FMTBIT_S16_LE |\
			SNDRV_PCM_FMTBIT_S20_3LE |\
			SNDRV_PCM_FMTBIT_S24_LE |\
			SNDRV_PCM_FMTBIT_S8)

//wangkai for 2.6.32
static struct snd_soc_dai_ops rt5625_dai_ops = { 
	//.startup  = rt5625_startup,
	.hw_params = rt5625_hifi_pcm_hw_params,
#if !USE_DAPM_CTRL	
	.prepare = rt5625_pcm_hw_prepare,
#endif
	//.digital_mute = rt5625_hifi_codec_mute,	
	.set_fmt = rt5625_hifi_codec_set_dai_fmt,
	.set_sysclk = rt5625_hifi_codec_set_dai_sysclk,
	.set_pll = rt5625_codec_set_dai_pll,
	.shutdown = rt5625_hifi_pcm_shutdown,
	.trigger = rt5625_trigger, 
};
struct snd_soc_dai_ops rt5625_dai_voice_ops ={	
	.hw_params = rt5625_voice_pcm_hw_params,	
	//.digital_mute = rt5625_voice_codec_mute,	
	.set_fmt = rt5625_voice_codec_set_dai_fmt,	
	.set_sysclk = rt5625_voice_codec_set_dai_sysclk,	
	.set_pll = rt5625_codec_set_dai_pll,	
	.shutdown = rt5625_voice_pcm_shutdown,
};

struct snd_soc_dai rt5625_dai[] = {
	/*hifi codec dai*/
	{
		.name = "RT5625 HiFi",
		.id = 1,
		.playback = {
			.stream_name = "HiFi Playback",
			.channels_min = 1,
			.channels_max = 2,
			.rates =  RT5625_STEREO_RATES,
			.formats = RT5625_FORMATS,
		},
		.capture = {
			.stream_name = "HiFi Capture",
			.channels_min = 1,
			.channels_max = 2,
			.rates = RT5626_VOICE_RATES,//RT5625_STEREO_RATES,//jmq.
			.formats = RT5625_FORMATS,
		},
		.ops = &rt5625_dai_ops,
	},

	/*voice codec dai*/
	{
		.name = "RT5625 Voice",
		.id = 1,
		.playback = {
			.stream_name = "Voice Playback",
			.channels_min = 1,
			.channels_max =1,
			.rates = RT5626_VOICE_RATES,
			.formats = RT5625_FORMATS,
		},
		.capture = {
			.stream_name = "Voice Capture",
			.channels_min = 1,
			.channels_max = 1,
			.rates = RT5626_VOICE_RATES,
			.formats = RT5625_FORMATS,
		},

		.ops = &rt5625_dai_voice_ops,
	},
};

EXPORT_SYMBOL_GPL(rt5625_dai);


static void rt5625_work(struct work_struct *work)
{	//s3cdbg( "enter %s\n", __func__);	
	struct snd_soc_codec *codec =
		 container_of(work, struct snd_soc_codec, delayed_work.work);
	rt5625_set_bias_level(codec, codec->bias_level);
}

#if defined(CONFIG_SND_HWDEP)
#if REALTEK_HWDEP
#define RT_CE_CODEC_HWDEP_NAME "rt56xx hwdep "


static int rt56xx_hwdep_open(struct snd_hwdep *hw, struct file *file)
{
	s3cdbg("enter %s\n", __func__);
	return 0;
}

static int rt56xx_hwdep_release(struct snd_hwdep *hw, struct file *file)
{
	s3cdbg("enter %s\n", __func__);
	return 0;
}


static int rt56xx_hwdep_ioctl_common(struct snd_hwdep *hw, struct file *file, unsigned int cmd, unsigned long arg)
{
	struct rt56xx_cmd rt56xx;
	struct rt56xx_cmd __user *_rt56xx = arg;
	struct rt56xx_reg_state *buf;
	struct rt56xx_reg_state *p;
	struct snd_soc_codec *codec = hw->private_data;
	s3cdbg( "enter %s\n", __func__);	
	if (copy_from_user(&rt56xx, _rt56xx, sizeof(rt56xx)))
		return -EFAULT;
	buf = kmalloc(sizeof(*buf) * rt56xx.number, GFP_KERNEL);
	if (buf == NULL)
		return -ENOMEM;
	if (copy_from_user(buf, rt56xx.buf, sizeof(*buf) * rt56xx.number)) {
		goto err;
	}
	switch (cmd) {
		case RT_READ_CODEC_REG_IOCTL:
			for (p = buf; p < buf + rt56xx.number; p++)
			{
				p->reg_value = codec->read(codec, p->reg_index);
			}
			if (copy_to_user(rt56xx.buf, buf, sizeof(*buf) * rt56xx.number))
				goto err;
				
			break;
		case RT_WRITE_CODEC_REG_IOCTL:
			for (p = buf; p < buf + rt56xx.number; p++)
				codec->write(codec, p->reg_index, p->reg_value);
			break;
	}

	kfree(buf);
	return 0;

err:
	kfree(buf);
	return -EFAULT;
	
}

static int rt56xx_codec_dump_reg(struct snd_hwdep *hw, struct file *file, unsigned long arg)
{
	struct rt56xx_cmd rt56xx;
	struct rt56xx_cmd __user *_rt56xx = arg;
	struct rt56xx_reg_state *buf;
	struct snd_soc_codec *codec = hw->private_data;
	int number = codec->reg_cache_size;
	int i;

	s3cdbg(KERN_DEBUG "enter %s, number = %d\n", __func__, number);	
	if (copy_from_user(&rt56xx, _rt56xx, sizeof(rt56xx)))
		return -EFAULT;
	
	buf = kmalloc(sizeof(*buf) * number, GFP_KERNEL);
	if (buf == NULL)
		return -ENOMEM;

	for (i = 0; i < number; i++)
	{
		buf[i].reg_index = i << 1;
		buf[i].reg_value = codec->read(codec, buf[i].reg_index);
	}
	if (copy_to_user(rt56xx.buf, buf, sizeof(*buf) * i))
		goto err;
	rt56xx.number = number;
	if (copy_to_user(_rt56xx, &rt56xx, sizeof(rt56xx)))
		goto err;
	kfree(buf);
	return 0;
err:
	kfree(buf);
	return -EFAULT;
	
}

static int rt56xx_hwdep_ioctl(struct snd_hwdep *hw, struct file *file, unsigned int cmd, unsigned long arg)
{	s3cdbg( "enter %s\n", __func__);	
	if (cmd == RT_READ_ALL_CODEC_REG_IOCTL)
	{
		return rt56xx_codec_dump_reg(hw, file, arg);
	}
	else
	{
		return rt56xx_hwdep_ioctl_common(hw, file, cmd, arg);
	}
}

static int realtek_ce_init_hwdep(struct snd_soc_codec *codec)
{
	struct snd_hwdep *hw;
	struct snd_card *card = codec->card;
	int err;
	s3cdbg( "enter %s\n", __func__);	
	if ((err = snd_hwdep_new(card, RT_CE_CODEC_HWDEP_NAME, 0, &hw)) < 0)
		return err;
	
	strcpy(hw->name, RT_CE_CODEC_HWDEP_NAME);
	hw->private_data = codec;
	hw->ops.open = rt56xx_hwdep_open;
	hw->ops.release = rt56xx_hwdep_release;
	hw->ops.ioctl = rt56xx_hwdep_ioctl;
	return 0;
}

#endif
#endif


static int rt5625_init(struct snd_soc_device *socdev)
{
	struct snd_soc_codec *codec = socdev->card->codec;
	int ret = 0;
	int err = 0;

	s3cdbg( "enter %s\n", __func__);	
	codec->name = "RT5625";
	codec->owner = THIS_MODULE;
	codec->read = rt5625_read;
	codec->write = rt5625_write;
	codec->set_bias_level = rt5625_set_bias_level;
	codec->dai= rt5625_dai;
	codec->num_dai = 2;
	codec->reg_cache_size = ARRAY_SIZE(rt5625_reg);
	codec->reg_cache = kmemdup(rt5625_reg, sizeof(rt5625_reg), GFP_KERNEL);
	if (codec->reg_cache == NULL)
		return -ENOMEM;

#if 1//Merged from ZhangMing, for PVT board
	/*output GPC0_0 to high level since the pull up register is not present, othrer place to be checked*/
	err = gpio_request(S5PV210_GPC0(0), "GPC0");
	if (err)
	    printk(KERN_ERR "#### failed to request GPC0-0 for ");

	gpio_direction_output(S5PV210_GPC0(0), 1);
	
	s3c_gpio_setpull(S5PV210_GPC0(0), S3C_GPIO_PULL_NONE);

	gpio_free(S5PV210_GPC0(0));

	rt5625_reset(codec);
#endif	
	
	err = gpio_request(S5PV210_GPC0(0), "GPC0");
	if (err)
	    printk(KERN_ERR "#### failed to request GPC0-2 ");
	err = gpio_request(S5PV210_GPH0(6), "GPH0");
	if (err)
	    printk(KERN_ERR "#### failed to request GPH0-6 ");
	s3c_gpio_setpull(S5PV210_GPH0(6), S3C_GPIO_PULL_NONE);

	s3c_gpio_cfgpin(S5PV210_GPC0(0), S3C_GPIO_OUTPUT); 
	gpio_direction_output(S5PV210_GPC0(0), 1);
	mdelay(2);
	gpio_direction_output(S5PV210_GPC0(0), 0);
	mdelay(2);
	gpio_direction_output(S5PV210_GPC0(0), 1);
	//gpio_free(S5PV210_GPC0(0));

	s3c_gpio_cfgpin(S5PV210_GPC0(0), S3C_GPIO_INPUT); 
	s3c_gpio_setpull(S5PV210_GPC0(0), S3C_GPIO_PULL_NONE);
#if 1
	s3c_gpio_cfgpin(S5PV210_GPH0(6), S3C_GPIO_SPECIAL(0xf)); //Eint6
	s3c_gpio_setpull(S5PV210_GPH0(6), S3C_GPIO_PULL_NONE);
	set_irq_type(IRQ_EINT6,IRQF_TRIGGER_RISING |IRQF_TRIGGER_FALLING);
	INIT_WORK(&hp_short_wq, rt5625_hp_detect_wq);
	//enable_irq(IRQ_EINT6);
	
	ret = request_irq(IRQ_EINT6, rt5625_hp_hotplug_detect, IRQF_SHARED,
                                               "rt5625 HP Hotplug", (void *) codec);
	
	if (ret < 0) 
	{
		printk("fail to claim Headphone Hotplug irq , ret = %d\n", ret);
		return -EIO;
	}
	else
		printk("request irq sucussfully\n");
#endif

	ret = snd_soc_new_pcms(socdev, SNDRV_DEFAULT_IDX1, SNDRV_DEFAULT_STR1);
	if (ret < 0 )
	{
		printk(KERN_ERR "rt5625:  failed to create pcms\n");
		goto pcm_err;
	}

	//rt5625_write(codec, RT5625_PD_CTRL_STAT, 0);
	rt5625_write(codec, RT5625_PD_CTRL_STAT, 0xff00);
	//rt5625_write(codec, RT5625_PWR_MANAG_ADD1, PWR_MAIN_BIAS);
	//rt5625_write(codec, RT5625_PWR_MANAG_ADD2, PWR_MIXER_VREF);
	rt5625_reg_init(codec);
	//init_vodsp_aec(codec);
	rt5625_set_bias_level(codec, SND_SOC_BIAS_PREPARE);
	codec->bias_level = SND_SOC_BIAS_STANDBY;
	schedule_delayed_work(&codec->delayed_work, msecs_to_jiffies(2000));
	rt5625_add_controls(codec);
#if USE_DAPM_CTRL
	rt5625_add_widgets(codec);
#endif

#if defined(CONFIG_SND_HWDEP)
#if REALTEK_HWDEP
	 realtek_ce_init_hwdep(codec);
#endif
#endif

#if 0
	ret = snd_soc_init_card(socdev);
	if (ret < 0)
	{
		printk(KERN_ERR "rt5625: failed to register card\n");
		goto card_err;
	}
#endif
	INIT_WORK(&playback_start_wq, rt5625_playback_start_wq);
	INIT_WORK(&playback_stop_wq, rt5625_playback_stop_wq);
	INIT_WORK(&capture_start_wq, rt5625_capture_start_wq);
	INIT_WORK(&capture_stop_wq, rt5625_capture_stop_wq);
	
	s3cdbg(KERN_DEBUG "rt5625: initial ok\n");
	return ret;

	card_err:
		snd_soc_free_pcms(socdev);
		snd_soc_dapm_free(socdev);
	
	pcm_err:
		kfree(codec->reg_cache);
		return ret;
	
	
}


static int rt5625_i2c_probe(struct i2c_client *i2c, const struct i2c_device_id *id)
{
	struct snd_soc_device *socdev = rt5625_socdev;
	struct snd_soc_codec *codec = socdev->card->codec;
	int ret;
	s3cdbg( "enter %s\n", __func__);	
	i2c_set_clientdata(i2c, codec);
	codec->control_data = i2c;


	ret = rt5625_init(socdev);
	if (ret < 0)
		pr_err("failed to initialise rt5625\n");

	return ret;
}

static int rt5625_i2c_remove(struct i2c_client *client)
{
	struct snd_soc_codec *codec = i2c_get_clientdata(client);
	kfree(codec->reg_cache);
	return 0;
}

static const struct i2c_device_id rt5625_i2c_id[] = {
		{"rt5625", 0},
		{}
};
MODULE_DEVICE_TABLE(i2c, rt5625_i2c_id);
static struct i2c_driver rt5625_i2c_driver = {
	.driver = {
		.name = "RT5625 I2C Codec",
		.owner = THIS_MODULE,
	},
	.probe =    rt5625_i2c_probe,
	.remove =   rt5625_i2c_remove,
	.id_table = rt5625_i2c_id,
};


static int rt5625_add_i2c_device(struct platform_device *pdev,
				 const struct rt5625_setup_data *setup)
{
	struct i2c_board_info info;
	struct i2c_adapter *adapter;
	struct i2c_client *client;
	int ret;
	s3cdbg( "enter %s\n", __func__);	
	ret = i2c_add_driver(&rt5625_i2c_driver);
	if (ret != 0) {
		dev_err(&pdev->dev, "can't add i2c driver\n");
		return ret;
	}

	memset(&info, 0, sizeof(struct i2c_board_info));
	info.addr = setup->i2c_address;
	strlcpy(info.type, "rt5625", I2C_NAME_SIZE);

	adapter = i2c_get_adapter(setup->i2c_bus);
	if (!adapter) {
		dev_err(&pdev->dev, "can't get i2c adapter %d\n",
			setup->i2c_bus);
		goto err_driver;
	}

	client = i2c_new_device(adapter, &info);
	i2c_put_adapter(adapter);
	if (!client) {
		dev_err(&pdev->dev, "can't add i2c device at 0x%x\n",
			(unsigned int)info.addr);
		goto err_driver;
	}

	return 0;

err_driver:
	i2c_del_driver(&rt5625_i2c_driver);
	return -ENODEV;
}

static ssize_t rt5625_dsp_reg_show(struct device *dev, 
	struct device_attribute *attr, char *buf)
{
	struct snd_soc_device *socdev = dev_get_drvdata(dev);
	struct snd_soc_codec *codec = socdev->card->codec;
	int count = 0;
	int dsp_value;
	int i;
	s3cdbg( "enter %s\n", __func__);	
	count += sprintf(buf, "%s dsp registers\n", codec ->name);
	for (i = 0; i < SET_VODSP_REG_INIT_NUM; i ++) {
		count += sprintf(buf + count, "0x%4x:", VODSP_AEC_Init_Value[i].VoiceDSPIndex);
		if (count > PAGE_SIZE - 1)
			break;
		dsp_value = rt5625_read_vodsp_reg(codec, VODSP_AEC_Init_Value[i].VoiceDSPIndex);
		count += snprintf(buf + count, PAGE_SIZE - count, "0x%4x", dsp_value);

		if (count >= PAGE_SIZE - 1)
			break;
		
		count += snprintf(buf + count, PAGE_SIZE - count, "\n");
		if (count >= PAGE_SIZE - 1)
			break;
	}

	if (count >= PAGE_SIZE)
		count = PAGE_SIZE - 1;

	return count;

}

static DEVICE_ATTR(dsp_reg, 0444, rt5625_dsp_reg_show, NULL);

static ssize_t rt5625_index_reg_show(struct device *dev, 
	struct device_attribute *attr, char *buf)
{
	struct snd_soc_device *socdev = dev_get_drvdata(dev);
	struct snd_soc_codec *codec = socdev->card->codec;
	int count = 0;
	int value;
	int i; 
		s3cdbg( "enter %s\n", __func__);	
	count += sprintf(buf, "%s index register\n", codec->name);

	for (i = 0; i < 60; i++) {
		count += sprintf(buf + count, "index-%2x", i);
		if (count >= PAGE_SIZE - 1)
			break;
		value = rt5625_read_index(codec, i);
		count += snprintf(buf + count, PAGE_SIZE - count, "0x%4x", value);

		if (count >= PAGE_SIZE - 1)
			break;

		count += snprintf(buf + count, PAGE_SIZE - count, "\n");
		if (count >= PAGE_SIZE - 1)
			break;
	}

	if (count >= PAGE_SIZE)
			count = PAGE_SIZE - 1;
		
	return count;
	
}

static DEVICE_ATTR(index_reg, 0444, rt5625_index_reg_show, NULL);

static int rt5625_probe(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct rt5625_setup_data *setup = socdev->codec_data;
	struct snd_soc_codec *codec;
	struct rt5625_priv *rt5625;
	int ret;
	s3cdbg( "enter %s\n", __func__);	
	printk("Enter rt5625_probe, socdev=%x\n", socdev);
	codec = kzalloc(sizeof(struct snd_soc_codec), GFP_KERNEL);
	if (codec == NULL)
		return -ENOMEM;

	rt5625 = kzalloc(sizeof(struct rt5625_priv), GFP_KERNEL);
	if (rt5625 == NULL) {
		kfree(codec);
		return -ENOMEM;
	}

	rt5625_aec_cfg = setup->cfg;
	codec->drvdata = rt5625;
	socdev->card->codec = codec;
	mutex_init(&codec->mutex);
	INIT_LIST_HEAD(&codec->dapm_widgets);
	INIT_LIST_HEAD(&codec->dapm_paths);
	rt5625_socdev = socdev;
	INIT_DELAYED_WORK(&codec->delayed_work, rt5625_work);
	ret = device_create_file(&pdev->dev, &dev_attr_dsp_reg);
	if (ret < 0)
		printk(KERN_WARNING "asoc: failed to add dsp_reg sysfs files\n");
	ret = device_create_file(&pdev->dev, &dev_attr_index_reg);
	if (ret < 0)
		printk(KERN_WARNING "asoc: failed to add index_reg sysfs files\n");
	
	ret = -ENODEV;
	if (setup->i2c_address) {
		codec->hw_write = (hw_write_t)i2c_master_send;
		//codec->hw_read = (hw_read_t)i2c_master_recv; //wangkai
		ret = rt5625_add_i2c_device(pdev, setup);
	}

	if (ret != 0) {
		kfree(codec->drvdata);
		kfree(codec);
	}

	/* VDD_5V regulator on: GPG16 */
	speaker_5v_regulator = regulator_get(NULL, "vdd_5v_speaker");
	if (IS_ERR(speaker_5v_regulator)) {
		printk(KERN_ERR "failed to get resource %s\n", "vdd_5v_speaker");
		return PTR_ERR(speaker_5v_regulator);
	}
	
	return ret;
}

static int run_delayed_work(struct delayed_work *dwork)
{
	int ret;
	s3cdbg( "enter %s\n", __func__);	
	/* cancel any work waiting to be queued. */
	ret = cancel_delayed_work(dwork);

	/* if there was any work waiting then we run it now and
	 * wait for it's completion */
	if (ret) {
		schedule_delayed_work(dwork, 0);
		flush_scheduled_work();
	}
	return ret;
}


static int rt5625_remove(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;
	s3cdbg( "enter %s\n", __func__);	
	if (codec->control_data)
		rt5625_set_bias_level(codec, SND_SOC_BIAS_OFF);
	run_delayed_work(&codec->delayed_work);
	snd_soc_free_pcms(socdev);
	snd_soc_dapm_free(socdev);
	device_remove_file(&pdev->dev, &dev_attr_dsp_reg);
	device_remove_file(&pdev->dev, &dev_attr_index_reg);
	i2c_unregister_device(codec->control_data);
	i2c_del_driver(&rt5625_i2c_driver);
	kfree(codec->drvdata);
	kfree(codec);

	if (gSpeakerState == SPEAKER_ON)
		regulator_disable(speaker_5v_regulator);
	regulator_put(speaker_5v_regulator);	
	return 0;
}


static int rt5625_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;
	s3cdbg( "enter %s\n", __func__);	
	rt5625_set_bias_level(codec, SND_SOC_BIAS_OFF);

	rt5625_reset(codec);

	disable_irq(IRQ_EINT6);
	gpio_direction_output(S5PV210_GPH0(6), 1);
	s3c_gpio_setpull(S5PV210_GPH0(6), S3C_GPIO_PULL_NONE);

	//gpio_direction_input(S5PV210_GPH0(6));
	//s3c_gpio_setpull(S5PV210_GPH0(6), S3C_GPIO_PULL_NONE);
	return 0;
}

static int rt5625_resume(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;
	int i;
	u8 data[3];
	u16 *cache = codec->reg_cache;
	s3cdbg( "enter %s\n", __func__);	
	/* Sync reg_cache with the hardware */
	for (i = 0; i < ARRAY_SIZE(rt5625_reg); i++) {
		if (i == RT5625_RESET)
			continue;
		data[0] = i << 1;
		data[1] = (0xff00 & cache[i]) >> 8;
		data[2] = 0x00ff & cache[i];
		codec->hw_write(codec->control_data, data, 3);
	}

	rt5625_set_bias_level(codec, SND_SOC_BIAS_STANDBY);

	/* charge rt5625 caps */
	if (codec->suspend_bias_level == SND_SOC_BIAS_ON) {
		rt5625_set_bias_level(codec, SND_SOC_BIAS_PREPARE);
		codec->bias_level = SND_SOC_BIAS_ON;
		schedule_delayed_work(&codec->delayed_work,
					msecs_to_jiffies(1000));
	}

	
	s3c_gpio_cfgpin(S5PV210_GPH0(6), S3C_GPIO_SPECIAL(0xf)); //Eint6
	s3c_gpio_setpull(S5PV210_GPH0(6), S3C_GPIO_PULL_NONE);
	enable_irq(IRQ_EINT6);//cynthia 20100907 for hp detect.
	return 0;
}


struct snd_soc_codec_device soc_codec_dev_rt5625 = {
	.probe = 	rt5625_probe,
	.remove = 	rt5625_remove,
	.suspend = 	rt5625_suspend,
	.resume =	rt5625_resume,
};

EXPORT_SYMBOL_GPL(soc_codec_dev_rt5625);



static int __init rt5625_modinit(void)
{
	return snd_soc_register_dais(rt5625_dai, ARRAY_SIZE(rt5625_dai));
}

static void __exit rt5625_exit(void)
{
	snd_soc_unregister_dais(rt5625_dai, ARRAY_SIZE(rt5625_dai));
}


module_init(rt5625_modinit);
module_exit(rt5625_exit);

MODULE_LICENSE("GPL");

