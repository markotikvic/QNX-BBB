/*
 * $QNXLicenseC:
 * Copyright 2014, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */

struct tda19988_context;
#define MIXER_CONTEXT_T struct tda19988_context

#include <mcasp.h>

#define HDMI_INFOFRAME_TYPE_AUDIO  0x84
#define HDMI_AUDIO_INFOFRAME_SIZE  10
#define CEA_CHN_ALLOCATION_STEREO  0x0

#define SF_48000     (1<<1)
#define SS_16BIT     (1<<1)
#define CN_2         (1<<4)	 /* Value = Channel count - 1 */
#define NO_COPYRIGHT (1<<2)  /* 0 = copyright, 1 = not copyright */

#define REG_CURRPAGE 0xFF

/* Page 00h: General Control */
#define PAGE_GENERAL_CTRL 0x00
#define REG_VERSION_LSB          0x00     /* read-only */
#define REG_VERSION_MSB          0x02     /* read-only */
#define REG_SOFTRESET            0x0a     /* write-only */
#define     SOFTRESET_AUDIO          (1 << 0)
#define     SOFTRESET_I2C_MASTER     (1 << 1)
#define REG_FEAT_POWERDOWN       0x0e
#define     FEAT_POWERDOWN_SPDIF     (1 << 3)
#define REG_ENA_ACLK             0x16
#define REG_ENA_AP               0x1e
#define REG_MUX_AP               0x26
#define MUX_AP_SELECT_I2S        0x64
#define MUX_AP_SELECT_SPDIF      0x40
#define REG_TBG_CNTRL_1          0xcb
#define     TBG_CNTRL_1_H_TGL        (1 << 0)
#define     TBG_CNTRL_1_V_TGL        (1 << 1)
#define     TBG_CNTRL_1_TGL_EN       (1 << 2)
#define     TBG_CNTRL_1_X_EXT        (1 << 3)
#define     TBG_CNTRL_1_H_EXT        (1 << 4)
#define     TBG_CNTRL_1_V_EXT        (1 << 5)
#define     TBG_CNTRL_1_DWIN_DIS     (1 << 6)
#define REG_I2S_FORMAT           0xfc
#define     I2S_FORMAT(x)            (((x) & 3) << 0)
#define REG_AIP_CLKSEL           0xfd     /* write-only */
#define     AIP_CLKSEL_AIP_SPDIF     (0 << 3)
#define     AIP_CLKSEL_AIP_I2S       (1 << 3)
#define     AIP_CLKSEL_FS_ACLK       (0 << 0)
#define     AIP_CLKSEL_FS_MCLK       (1 << 0)
#define     AIP_CLKSEL_FS_FS64SPDIF  (2 << 0)

#define PAGE_PLL          0x02
#define REG_PLL_SERIAL_2         0x1
#define     PL2_SRL_NOSC(x)          (((x) & 0x03) << 0)
#define     PL2_SRL_PR(x)            (((x) & 0x0F) << 4)
#define REG_AUDIO_DIV            0x0e
#define     AUDIO_DIV_SERCLK_1       0
#define     AUDIO_DIV_SERCLK_2       1
#define     AUDIO_DIV_SERCLK_4       2
#define     AUDIO_DIV_SERCLK_8       3
#define     AUDIO_DIV_SERCLK_16      4
#define     AUDIO_DIV_SERCLK_32      5
#define REG_SEL_CLK              0x11
#define     SEL_CLK_SEL_CLK1         (1 << 0)
#define     SEL_CLK_SEL_VRF_CLK(x)   (((x) & 3) << 1)
#define     SEL_CLK_ENA_SC_CLK       (1 << 3)
#define REG_ANA_GENERAL          0x12

/* Page 10h: information frames and packets */
#define PAGE_INFOFRAME    0x10
#define REG_IF1_HB0              0x20
#define REG_IF2_HB0              0x40
#define REG_IF3_HB0              0x60
#define REG_IF4_HB0              0x80
#define REG_IF5_HB0              0xa0

/* Page 11h: audio settings and content info packets */
#define PAGE_AUDIO        0x11
#define REG_AIP_CNTRL_0          0x00
#define     AIP_CNTRL_0_RST_FIFO     (1 << 0)
#define     AIP_CNTRL_0_SWAP         (1 << 1)
#define     AIP_CNTRL_0_LAYOUT       (1 << 2)
#define     AIP_CNTRL_0_ACR_MAN      (1 << 5)
#define     AIP_CNTRL_0_RST_CTS      (1 << 6)
#define REG_CA_I2S               0x01
#define     CA_I2S_CA_I2S(x)         (((x) & 31) << 0)
#define     CA_I2S_HBR_CHSTAT        (1 << 6)
#define REG_LATENCY_RD           0x04
#define REG_ACR_CTS_0            0x05
#define REG_ACR_CTS_1            0x06
#define REG_ACR_CTS_2            0x07
#define REG_ACR_N_0              0x08
#define REG_ACR_N_1              0x09
#define REG_ACR_N_2              0x0a
#define REG_CTS_N                0x0c
#define     CTS_N_K(x)               (((x) & 7) << 0)
#define     CTS_N_M(x)               (((x) & 3) << 4)
#define REG_ENC_CNTRL            0x0d
#define     ENC_CNTRL_RST_ENC        (1 << 0)
#define     ENC_CNTRL_RST_SEL        (1 << 1)
#define     ENC_CNTRL_CTL_CODE(x)    (((x) & 3) << 2)
#define REG_DIP_FLAGS            0x0e
#define     DIP_FLAGS_ACR            (1 << 0)
#define     DIP_FLAGS_GC             (1 << 1)
#define REG_DIP_IF_FLAGS         0x0f
#define     DIP_IF_FLAGS_IF1         (1 << 1)
#define     DIP_IF_FLAGS_IF2         (1 << 2)
#define     DIP_IF_FLAGS_IF3         (1 << 3)
#define     DIP_IF_FLAGS_IF4         (1 << 4)
#define     DIP_IF_FLAGS_IF5         (1 << 5)
#define REG_CH_STAT_B            0x14

/* Page 12h: HDCP and OTP */
#define PAGE_HDCP         0x12
#define REG_TX3                   0x9a
#define REG_TX4                   0x9b
#define     TX4_PD_RAM                (1 << 1)
#define REG_TX33                  0xb8
#define     TX33_HDMI                 (1 << 1)

static int32_t pcm_devices[1] = {
	0
};

static snd_mixer_voice_t stereo_voices[2] = {
    {SND_MIXER_VOICE_LEFT, 0},
    {SND_MIXER_VOICE_RIGHT, 0}
};

typedef struct tda19988_context
{
	ado_mixer_t *mixer;
	HW_CONTEXT_T *hwc;
	int fd;
	int i2c_addr;
} tda19988_context_t;

/* read one 8 bit register from an 8 bit I2C address space */
static int tda19988_read_reg(tda19988_context_t *tda19988, uint32_t reg, uint8_t* val)
{
	int r;
	struct
	{
		i2c_send_t      hdr;
		uint8_t         reg;
		uint8_t         val;
	} msg;
	struct
	{
		i2c_send_t      hdr;
		uint8_t         val;
	} msg_r;

	msg.hdr.slave.addr = tda19988->i2c_addr;
	msg.hdr.slave.fmt  = I2C_ADDRFMT_7BIT;
	msg.hdr.len        = 1;
	msg.hdr.stop       = 1;
	msg.reg            = reg;

	r = devctl(tda19988->fd, DCMD_I2C_SEND, &msg, sizeof(msg), NULL);
	if (r)
	{
		ado_error("TDA19988: I2C read failed (DCMD_I2C_SEND) (%s)", strerror(errno));
		return errno;
	}

	msg_r.hdr.slave.addr = tda19988->i2c_addr;
	msg_r.hdr.slave.fmt  = I2C_ADDRFMT_7BIT;
	msg_r.hdr.len        = 1;
	msg_r.hdr.stop       = 1;
	msg_r.val            = 0;

	r = devctl(tda19988->fd, DCMD_I2C_RECV, &msg_r, sizeof(msg_r), NULL);
	if (r)
	{
		ado_error("TDA19988: I2C read failed (DCMD_I2C_RECV) (%s)", strerror(errno));
		return errno;
	}

	*val=msg_r.val;

	return (EOK);
}

static int tda19988_write_reg(tda19988_context_t *tda19988, uint8_t reg, uint8_t val, uint8_t mask)
{
	int r;
	uint8_t read_val = 0;
	struct
	{
		i2c_send_t      hdr;
		uint8_t         reg;
		uint8_t         val;
	} msg;

	/* If the mask is 0 or 0xFF, don't bother reading */
	if (mask != 0 && mask != 0xFF)
	{
		tda19988_read_reg(tda19988, reg, &read_val);
		val = ((read_val & ~mask) | (val & mask));
	}

	msg.hdr.slave.addr = tda19988->i2c_addr;
	msg.hdr.slave.fmt  = I2C_ADDRFMT_7BIT;
	msg.hdr.len        = 2;
	msg.hdr.stop       = 1;
	msg.reg            = reg;
	msg.val            = val;

	r = devctl(tda19988->fd, DCMD_I2C_SEND, &msg, sizeof(msg), NULL);
	if (r)
	{
		ado_error("TDA19988: I2C write failed (%s)", strerror(errno));
		return errno;
	}

	return EOK;
}

static int tda19988_write_buf(MIXER_CONTEXT_T * tda19988, uint8_t regaddr, uint8_t *data, int data_len)
{
	struct {
		i2c_sendrecv_t hdr;
		uint8_t        bytes[1];
	} *msg;

	if ((msg = ado_calloc (1, sizeof(*msg) + data_len)) == NULL)
	{
		ado_error("%s: Failed to alloc i2c message buffer - %s",  __FUNCTION__, strerror(errno));
		return -1;
	}

	msg->hdr.slave.addr = tda19988->i2c_addr;
	msg->hdr.slave.fmt = I2C_ADDRFMT_7BIT;
	msg->hdr.stop = 1;

	msg->bytes[0] = regaddr;
	memcpy(&msg->bytes[1], data, data_len);
	msg->hdr.send_len = data_len + 1;		/* +1 for regaddr */
	msg->hdr.recv_len = 1;

	if (devctl(tda19988->fd, DCMD_I2C_SENDRECV, msg, sizeof (*msg) + data_len, NULL))
	{
		ado_error("Failed to write to TDA998: %s\n", strerror(errno));
	}

	ado_debug(DB_LVL_MIXER, "TDA998 Codec write slave_addr = 0x%x, reg=%x data= %d", tda19988->i2c_addr, regaddr, data[0]);

	ado_free(msg);

    return EOK;
}

static uint8_t tda19988_cksum(uint8_t *buf, size_t bytes)
{
	int sum = 0;

	while (bytes--)
	{
		sum += *buf++;
	}
	return (0x100 - sum);
}

static void
tda19988_setup_audio_infoframe(tda19988_context_t *tda19988)
{
	struct
	{
		uint8_t type;
		uint8_t version;
		uint8_t len;
		uint8_t dbytes[HDMI_AUDIO_INFOFRAME_SIZE];
	}audio_infoframe;

	/* Disable transmission of the Audio InfoFrame */
	tda19988_write_reg(tda19988, REG_CURRPAGE, PAGE_AUDIO, 0xff);
	tda19988_write_reg(tda19988, REG_DIP_IF_FLAGS, 0x0, DIP_IF_FLAGS_IF4);

	memset(&audio_infoframe, 0, sizeof(audio_infoframe));
	audio_infoframe.type = HDMI_INFOFRAME_TYPE_AUDIO;
	audio_infoframe.version = 1;
	audio_infoframe.len = HDMI_AUDIO_INFOFRAME_SIZE;
	/* Note: All dbytes values left a 0 will tell the sink to look at the channel status bytes for the value */
	audio_infoframe.dbytes[1] = 1;	/* Channel count - 1 */
	audio_infoframe.dbytes[4] = CEA_CHN_ALLOCATION_STEREO;
	audio_infoframe.dbytes[0] = tda19988_cksum((uint8_t*) &audio_infoframe, sizeof(audio_infoframe));

	/* Update Audio InfoFrame */
	tda19988_write_reg(tda19988, REG_CURRPAGE, PAGE_INFOFRAME, 0xff);
	tda19988_write_buf(tda19988, REG_IF4_HB0, (uint8_t *)&audio_infoframe, sizeof(audio_infoframe));

	/* Enable transmission of the Audio InfoFrame */
	tda19988_write_reg(tda19988, REG_CURRPAGE, PAGE_AUDIO, 0xff);
	tda19988_write_reg(tda19988, REG_DIP_IF_FLAGS, DIP_IF_FLAGS_IF4, DIP_IF_FLAGS_IF4);
}

static void tda19988_audio_mute(tda19988_context_t *tda19988, bool on)
{

	if (on)
	{
		/* Un-map audio input before muting */
		tda19988_write_reg(tda19988, REG_CURRPAGE, PAGE_GENERAL_CTRL, 0xff);
		tda19988_write_reg(tda19988, REG_SOFTRESET, SOFTRESET_AUDIO, 0x0);
		tda19988_write_reg(tda19988, REG_SOFTRESET, 0x0, 0x0);

		/* Put Audio FIFO into reset to mute */
		tda19988_write_reg(tda19988, REG_CURRPAGE, PAGE_AUDIO, 0xff);
		tda19988_write_reg(tda19988, REG_AIP_CNTRL_0, AIP_CNTRL_0_RST_FIFO, AIP_CNTRL_0_RST_FIFO);
	}
	else
	{
		/* Take Audio FIFO out of reset to unmute */
		tda19988_write_reg(tda19988, REG_CURRPAGE, PAGE_AUDIO, 0xff);
		tda19988_write_reg(tda19988, REG_AIP_CNTRL_0, 0x0, AIP_CNTRL_0_RST_FIFO);
	}
}

static int32_t
tda19988_mute_control(MIXER_CONTEXT_T * tda19988, ado_mixer_delement_t * element, uint8_t set, uint32_t * val,
						void *instance_data)
{
	int altered = 0;
	uint8_t mute = 0;

	/* Get current mute state */
	tda19988_write_reg(tda19988, REG_CURRPAGE, PAGE_AUDIO, 0xff);
	tda19988_read_reg(tda19988, REG_AIP_CNTRL_0, &mute);
	mute = (mute & AIP_CNTRL_0_RST_FIFO) ? (SND_MIXER_CHN_MASK_FRONT_LEFT | SND_MIXER_CHN_MASK_FRONT_RIGHT) : 0;

	if (set)
	{
		altered = val[0] != mute;
		if (altered)
		{
			if (val[0] & (SND_MIXER_CHN_MASK_FRONT_LEFT | SND_MIXER_CHN_MASK_FRONT_RIGHT))
				tda19988_audio_mute(tda19988, true);
			else
				tda19988_audio_mute(tda19988, false);
		}
	}
	else
	{
		val[0] = mute;
	}

	return (altered);
}

static ado_mixer_destroy_t tda19988_destroy;
static int tda19988_destroy(MIXER_CONTEXT_T * tda19988)
{
	ado_debug(DB_LVL_MIXER, "destroying TDA19988 Codec");

	/* Mute Audio */
	tda19988_audio_mute(tda19988, true);

	/* Disable transmission of the Audio InfoFrame */
	tda19988_write_reg(tda19988, REG_CURRPAGE, PAGE_AUDIO, 0xff);
	tda19988_write_reg(tda19988, REG_DIP_IF_FLAGS, 0x0, DIP_IF_FLAGS_IF4);
	/* Put CTS generator into reset */
	tda19988_write_reg(tda19988, REG_AIP_CNTRL_0, AIP_CNTRL_0_RST_CTS, AIP_CNTRL_0_RST_CTS);

	/* Disable audio ports */
	tda19988_write_reg(tda19988, REG_CURRPAGE, PAGE_GENERAL_CTRL, 0xff);
	tda19988_write_reg(tda19988, REG_ENA_AP, 0x3, 0x0);
	tda19988_write_reg(tda19988, REG_ENA_ACLK, 0x1, 0x0);

	close(tda19988->fd);
	ado_free(tda19988);
	return (0);
}

static ado_mixer_reset_t reset_mixer;
static  int32_t
reset_mixer (MIXER_CONTEXT_T *tda19988)
{
	uint8_t buf[10], cts_n, adiv;
	uint32_t n, cts;

	/* Mute Audio */
	tda19988_audio_mute(tda19988, true);

	/* We need to turn HDMI HDCP on to get audio flowing */
	tda19988_write_reg(tda19988, REG_CURRPAGE, PAGE_GENERAL_CTRL, 0xff);
	tda19988_write_reg(tda19988, REG_TBG_CNTRL_1, 0x0, TBG_CNTRL_1_DWIN_DIS);
	tda19988_write_reg(tda19988, REG_CURRPAGE, PAGE_AUDIO, 0xff);
	tda19988_write_reg(tda19988, REG_ENC_CNTRL, ENC_CNTRL_CTL_CODE(1), ENC_CNTRL_CTL_CODE(3));
	tda19988_write_reg(tda19988, REG_CURRPAGE, PAGE_HDCP, 0xff);
	tda19988_write_reg(tda19988, REG_TX33, TX33_HDMI, TX33_HDMI);

	/* Enable audio ports */
	tda19988_write_reg(tda19988, REG_CURRPAGE, PAGE_GENERAL_CTRL, 0xff);
	tda19988_write_reg(tda19988, REG_ENA_AP, 0x3, 0xff);
	tda19988_write_reg(tda19988, REG_ENA_ACLK, 0x1, 0xff);
	/* Set audio input source */
	tda19988_write_reg(tda19988, REG_MUX_AP, MUX_AP_SELECT_I2S, 0xff);
	tda19988_write_reg(tda19988, REG_AIP_CLKSEL, AIP_CLKSEL_AIP_I2S | AIP_CLKSEL_FS_ACLK, 0xff);
	tda19988_write_reg(tda19988, REG_I2S_FORMAT, 12 , 0xff);

	/* Put CTS generator into reset */
	tda19988_write_reg(tda19988, REG_CURRPAGE, PAGE_AUDIO, 0xff);
	tda19988_write_reg(tda19988, REG_AIP_CNTRL_0, AIP_CNTRL_0_RST_CTS, AIP_CNTRL_0_RST_CTS);
	tda19988_write_reg(tda19988, REG_CA_I2S, 0 , 0xff);
	tda19988_write_reg(tda19988, REG_AIP_CNTRL_0, 0, AIP_CNTRL_0_LAYOUT | AIP_CNTRL_0_ACR_MAN);	/* auto CTS */
	cts_n = CTS_N_M(3) | CTS_N_K(3);
	tda19988_write_reg(tda19988, REG_CTS_N, cts_n, 0xff);

	tda19988_write_reg(tda19988, REG_CURRPAGE, PAGE_PLL, 0xff);
	/* Determine the Pixel clock frequency programmed by the graphics driver */
	tda19988_read_reg(tda19988, REG_PLL_SERIAL_2, &adiv);
	cts = (148500000 / (adiv * 1000));

	/* Pixel clock greater then 100Mhz needs a larger divider */
	if (cts < 100000)
		adiv = AUDIO_DIV_SERCLK_8;
	else
		adiv = AUDIO_DIV_SERCLK_16;

	tda19988_write_reg(tda19988, REG_AUDIO_DIV, adiv, 0xff);

	/*
	 * This is the approximate value of N, which happens to be
	 * the recommended values for non-coherent clocks.
	 */
	n = 128 * 48000 / 1000;

	/* Write the CTS and N values */
	memset(buf, 0x0, sizeof(buf));
	buf[0] = cts & 0xff;
	buf[1] = (cts >> 8) & 0xff;
	buf[2] = (cts >> 16) & 0xff;
	buf[3] = n;
	buf[4] = (n >> 8);
	buf[5] = (n >> 16);
	tda19988_write_reg(tda19988, REG_CURRPAGE, PAGE_AUDIO, 0xff);
	tda19988_write_buf(tda19988, REG_ACR_CTS_0, buf, 6);

	/* Take CTS generator out of reset */
	tda19988_write_reg(tda19988, REG_AIP_CNTRL_0, 0x0, AIP_CNTRL_0_RST_CTS);

	/* Write the channel status bits
	 * Note: Bytes 2 and 4 are swapped
	 */
	memset(buf, 0x0, sizeof(buf));
	buf[0] = NO_COPYRIGHT;  /* Bits 0-7   */
	buf[1] = 0x00;          /* Bits 8-15  */
	buf[2] = SF_48000;      /* Bits 24-31 */
	buf[3] = SS_16BIT;      /* Bits 32-39 */
	buf[4] = CN_2;          /* Bits 16-23 */
	tda19988_write_buf(tda19988, REG_CH_STAT_B, buf, 5);

	tda19988_setup_audio_infoframe(tda19988);

	tda19988_audio_mute(tda19988, false);

	return (0);
}

static int
build_mixer (MIXER_CONTEXT_T * hwc, ado_mixer_t * mixer)
{
	int     error = 0;
	ado_mixer_delement_t *pre_elem, *elem = NULL, *mute = NULL;

	if (!error && (elem = ado_mixer_element_pcm1 (mixer, SND_MIXER_ELEMENT_PLAYBACK,
				SND_MIXER_ETYPE_PLAYBACK1, 1, &pcm_devices[0])) == NULL)
		error++;
	pre_elem = elem;

    if(!error && (mute = ado_mixer_element_sw2(mixer, "Mute", tda19988_mute_control, NULL, NULL)) == NULL)
        error++;

    if(!error && ado_mixer_element_route_add(mixer, pre_elem, mute) != 0)
        error++;
	pre_elem = mute;

	if (!error &&
		(elem =
			ado_mixer_element_io (mixer, SND_MIXER_PCM_OUT, SND_MIXER_ETYPE_OUTPUT, 0, 2, stereo_voices)) == NULL)
		error++;
	if (!error && ado_mixer_element_route_add (mixer, pre_elem, elem) != 0)
		error++;

	if (!error && ado_mixer_playback_group_create (mixer, SND_MIXER_PCM_OUT, SND_MIXER_CHN_MASK_STEREO, NULL, mute) == NULL)
		error++;
	return (0);
}

int
codec_mixer (ado_card_t * card, HW_CONTEXT_T * hwc)
{
	char i2c_port[20];
	tda19988_context_t *tda19988 = NULL;
	int32_t status;

	ado_debug (DB_LVL_MIXER, "Initializing Mixer");

	if((tda19988 = (tda19988_context_t *) ado_calloc(1, sizeof (tda19988_context_t))) == NULL)
	{
		ado_error("%s: no memory %s", __FUNCTION__, strerror(errno));
		return (-1);
	}

	if ((status = ado_mixer_create (card, "TDA19988", &hwc->mixer, tda19988)) != EOK)
		return (status);

	tda19988->mixer = hwc->mixer;
	tda19988->hwc = hwc;

	/* determine which I2C device is being used */
	if(hwc->i2c_dev == 0xff)
	{
		ado_debug(DB_LVL_MIXER, "No i2c device selected (will assume TDA19988 is connected to i2c0)");
		hwc->i2c_dev = 0;
	}

	sprintf(i2c_port, "/dev/i2c%d", hwc->i2c_dev);
	if((tda19988->fd = open(i2c_port, O_RDWR)) < 0)
	{
		ado_error("TDA998: could not open i2c%d device %s", hwc->i2c_dev, strerror(errno));
		ado_free(tda19988);
		return (-1);
	}

	tda19988->i2c_addr = CODEC_SLAVE_ADDR1;

	if (build_mixer (tda19988, hwc->mixer))
		return (-1);

	if (reset_mixer (tda19988))
		return (-1);

	ado_mixer_set_reset_func (hwc->mixer, reset_mixer);
	ado_mixer_set_destroy_func(hwc->mixer, tda19988_destroy);

	return (0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/deva/ctrl/mcasp/nto/arm/dll.le.dm814x_tda19988.v7/mixer.c $ $Rev: 765324 $")
#endif
