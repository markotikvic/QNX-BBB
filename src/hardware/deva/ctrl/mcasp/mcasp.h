/*
 * $QNXLicenseC:
 * Copyright 2007-2008, QNX Software Systems.
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


#ifndef __MCASP_H
#define __MCASP_H


/*
 *  Definitions for mcasp interface
 */
struct mcasp_card;
struct mcasp_subchn;
struct ahclk;

#define  HW_CONTEXT_T struct mcasp_card
#define  PCM_SUBCHN_CONTEXT_T struct mcasp_subchn
#include <audio_driver.h>
#include "variant.h"
#include <hw/i2c.h>

typedef volatile uint32_t	vuint32_t; 
typedef volatile uint16_t	vuint16_t; 
typedef volatile uint8_t	vuint8_t ;

#define MAX_HIGH_CLK_FREQS                8
#define MAX_CAP_SUBCHN_COUNT              3
#define NUM_CAPTURE_DMA_FRAGS             3
#define DEFAULT_CAPTURE_DMA_FRAG_SIZE     (1*1024)

typedef struct McASP
{
    vuint32_t PID;                      /* 0x000 */
    vuint32_t PWRDEMU;                  /* 0x004 */
#define PWRDEMU_FREE		(1<<0)	
    vuint32_t resv_a[2];
    vuint32_t PFUNC;                    /* 0x010 */
#define PFUNC_MCASP			(0)			/* Pins function as MCASP pins */	
    vuint32_t PDIR;                     /* 0x014 */
#define PDIR_ALL_INPUT		(0)			/* All pins are inputs */
#define PDIR_AFSR_OUT       (1<<31)
#define PDIR_AHCLKR_OUT     (1<<30)
#define PDIR_ACLKR_OUT      (1<<29)
#define PDIR_AFSX_OUT       (1<<28)
#define PDIR_AHCLKX_OUT     (1<<27)
#define PDIR_ACLKX_OUT      (1<<26)
#define PDIR_AMUTE_OUT      (1<<25)
#define PDIR_AXR_OUT(x)	    (1<<(x & 0x0f))		/* Set AXR[x] as output */
    vuint32_t PDOUT;                    /* 0x018 */
    vuint32_t PIN;                      /* 0x01C */
    vuint32_t resv_b[9];
    vuint32_t GBLCTL;                   /* 0x044 */
    vuint32_t AMUTE;                    /* 0x048 */
    vuint32_t DLBCTL;                   /* 0x04C */
#define DLBCTL_SYNC         (1<<2)
#define DLBCTL_ORD          (1<<1)
#define DLBCTL_DLBEN        (1<<0)
    vuint32_t DITCTL;                   /* 0x050 */
#define DITCTL_DITEN        (1<<0)
	vuint32_t resv_c[3];
    vuint32_t RGBLCTL;                  /* 0x060 */
#define RGBLCTL_RFRST		(1<<4)	
#define RGBLCTL_RSMRST		(1<<3)	
#define RGBLCTL_RSRCLR		(1<<2)	
#define RGBLCTL_RHCLKRST	(1<<1)	
#define RGBLCTL_RCLKRST		(1<<0)	
    vuint32_t RMASK;                    /* 0x064 */
    vuint32_t RFMT;                     /* 0x068 */
#define RFMT_RDATDLY_1BIT	(1<<16)		/* 1-bit sync delay */
#define RFMT_RDATDLY_2BIT	(2<<16)		/* 2-bit sync delay */
#define RFMT_RRVRS_MSB		(1<<15)
#define RFMT_RSSZ_16BIT		(0x7<<4)
#define RFMT_RSSZ_32BIT		(0xf<<4)
#define RFMT_RBUSEL			(1<<3)
#define RFMT_RROT_8BIT		(1<<1)
#define RFMT_RROT_16BIT		(1<<2)
#define RFMT_RROT_24BIT		(0x3<<1)
    vuint32_t AFSRCTL;                  /* 0x06C */
#define AFSRCTL_RMOD_8CH    (8<<7)      /* 8-slot TDM-LJ */
#define AFSRCTL_RMOD_6CH    (6<<7)      /* 6-slot I2S TDM */
#define AFSRCTL_RMOD_4CH    (4<<7)      /* 4-slot I2S TDM */
#define AFSRCTL_RMOD_2CH	(2<<7)		/* 2-slot i2s mode */
#define AFSRCTL_FRWID		(1<<4)		/* Frame sync = Singel word */
#define AFSRCTL_FSRM		(1<<1)		/* Frame sync internally generated */
#define AFSRCTL_FSRP		(1<<0)		/* Frame sync on falling edge */
    vuint32_t ACLKRCTL;                 /* 0x070 */
#define ACLKRCTL_CLKRP		(1<<7)		/* Sample data on falling edge of Bitstream clock */	
#define ACLKRCTL_CLKRM		(1<<5)		/* Bit Clock Master */
    vuint32_t AHCLKRCTL;                /* 0x074 */
#define AHCLKRCTL_HCLKRM    (1<<15)     /* Internal high freq clock source */
    vuint32_t RTDM;                     /* 0x078 */
#define	TDMS(x)				(1<<x)		/* Enable TDM time slot x */	
    vuint32_t RINTCTL;                  /* 0x07C */
#define RINTCTL_ROVRN           (1<<0)
    vuint32_t RSTAT;                    /* 0x080 */
#define RSTAT_RDATA			(1<<5)
#define RSTAT_ROVRN			(1<<0)
    vuint32_t RSLOT;                    /* 0x084 */
    vuint32_t RCLKCHK;                  /* 0x088 */
    vuint32_t REVTCTL;                  /* 0x08C */
    vuint32_t resv_d[4];
    vuint32_t XGBLCTL;                  /* 0x0A0 */
#define XGBLCTL_XFRST		(1<<12)	
#define XGBLCTL_XSMRST		(1<<11)	
#define XGBLCTL_XSRCLR		(1<<10)	
#define XGBLCTL_XHCLKRST	(1<<9)	
#define XGBLCTL_XCLKRST		(1<<8)	
    vuint32_t XMASK;                    /* 0x0A4 */
    vuint32_t XFMT;                     /* 0x0A8 */
#define XFMT_XDATDLY_1BIT	(1<<16)		/* 1-bit sync delay */
#define XFMT_XDATDLY_2BIT	(2<<16)		/* 2-bit sync delay */
#define XFMT_XRVRS_MSB		(1<<15)		/* Transmit MSB first */
#define XFMT_XSSZ_16BIT		(0x7<<4)
#define XFMT_XSSZ_24BIT		(0xb<<4)
#define XFMT_XSSZ_32BIT		(0xf<<4)	/* 32-bit slot size */
#define XFMT_XBUSEL			(1<<3)		/* Perpheral configuration bus select */
#define XFMT_XROT_8BIT		(1<<1)
#define XFMT_XROT_16BIT		(1<<2)		/* Rotate 16 bits to the right */
#define XFMT_XROT_24BIT		(0x3<<1)
    vuint32_t AFSXCTL;                  /* 0x0AC */
#define AFSXCTL_XMOD_384_DIT    (384<<7)      /* 384-slot S/PDIF */
#define AFSXCTL_XMOD_16CH   (16<<7)     /* 16-slot TDM */
#define AFSXCTL_XMOD_8CH    (8<<7)      /* 8-slot TDM-LJ */
#define AFSXCTL_XMOD_6CH    (6<<7)      /* 6-slot I2S TDM */
#define AFSXCTL_XMOD_4CH    (4<<7)      /* 4-slot I2S TDM */
#define AFSXCTL_XMOD_2CH	(2<<7)		/* 2-slot i2s mode */
#define AFSXCTL_XMOD_1CH	(1<<7)		/* 1-slot i2s mode */
#define AFSXCTL_FXWID		(1<<4)		/* Frame sync = Singel word */
#define AFSXCTL_FSXM		(1<<1)		/* Frame sync Master */
#define AFSXCTL_FSXP		(1<<0)		/* Frame sync on falling edge */
    vuint32_t ACLKXCTL;                 /* 0x0B0 */
#define ACLKXCTL_CLKXP		(1<<7)		/* Shift data out on falling edge of Bitstream clock */	
#define ACLKXCTL_CLKXM		(1<<5)		/* Bit Clock Master */
#define ACLKXCTL_ASYNC		(1<<6)		/* Trasmit and receive clocks operate aysnchronously */
    vuint32_t AHCLKXCTL;                /* 0x0B4 */
#define AHCLKXCTL_HCLKXM    (1<<15)     /* Internal high freq clock source */
    vuint32_t XTDM;                     /* 0x0B8 */
    vuint32_t XINTCTL;                  /* 0x0BC */
#define XINTCTL_XUNDRN      (1<<0)
#define XINTCTL_XSTAFRM 	(1<<7)    
	vuint32_t XSTAT;                    /* 0x0C0 */
#define XSTAT_XDATA			(1<<5)
#define XSTAT_XUNDRN		(1<<0)
#define XSTAT_XSTAFRM		(1<<6)
    vuint32_t XSLOT;                    /* 0x0C4 */
    vuint32_t XCLKCHK;                  /* 0x0C8 */
    vuint32_t XEVTCTL;                  /* 0x0CC */
    vuint32_t resv_e[12];
    vuint32_t DITCSRA0;                 /* 0x100 */
    vuint32_t DITCSRA1;                 /* 0x104 */
    vuint32_t DITCSRA2;                 /* 0x108 */
    vuint32_t DITCSRA3;                 /* 0x10C */
    vuint32_t DITCSRA4;                 /* 0x110 */
    vuint32_t DITCSRA5;                 /* 0x114 */
    vuint32_t DITCSRB0;                 /* 0x118 */
    vuint32_t DITCSRB1;                 /* 0x11C */
    vuint32_t DITCSRB2;                 /* 0x120 */
    vuint32_t DITCSRB3;                 /* 0x124 */
    vuint32_t DITCSRB4;                 /* 0x128 */
    vuint32_t DITCSRB5;                 /* 0x12C */
    vuint32_t DITUDRA0;                 /* 0x130 */
    vuint32_t DITUDRA1;                 /* 0x134 */
    vuint32_t DITUDRA2;                 /* 0x138 */
    vuint32_t DITUDRA3;                 /* 0x13C */
    vuint32_t DITUDRA4;                 /* 0x140 */
    vuint32_t DITUDRA5;                 /* 0x144 */
    vuint32_t DITUDRB0;                 /* 0x148 */
    vuint32_t DITUDRB1;                 /* 0x14C */
    vuint32_t DITUDRB2;                 /* 0x150 */
    vuint32_t DITUDRB3;                 /* 0x154 */
    vuint32_t DITUDRB4;                 /* 0x158 */
    vuint32_t DITUDRB5;                 /* 0x15C */
    vuint32_t resv_f[8];
    vuint32_t SRCTL[16];                /* 0x180 */
#define SRCTL_XMIT		(1<<0)			/* Serializer is Transmitter */
#define SRCTL_RECV		(1<<1)			/* Serializer is Receiver */
    vuint32_t resv_g[16];
    vuint32_t XBUF0;                    /* 0x200 */
    vuint32_t XBUF1;                    /* 0x204 */
    vuint32_t XBUF2;                    /* 0x208 */
    vuint32_t XBUF3;                    /* 0x20C */
    vuint32_t XBUF4;                    /* 0x210 */
    vuint32_t XBUF5;                    /* 0x214 */
    vuint32_t XBUF6;                    /* 0x218 */
    vuint32_t XBUF7;                    /* 0x21C */
    vuint32_t XBUF8;                    /* 0x220 */
    vuint32_t XBUF9;                    /* 0x224 */
    vuint32_t XBUF10;                   /* 0x228 */
    vuint32_t XBUF11;                   /* 0x22C */
    vuint32_t XBUF12;                   /* 0x230 */
    vuint32_t XBUF13;                   /* 0x234 */
    vuint32_t XBUF14;                   /* 0x238 */
    vuint32_t XBUF15;                   /* 0x23C */
    vuint32_t resv_h[16];
    vuint32_t RBUF0;                    /* 0x280 */
    vuint32_t RBUF1;                    /* 0x284 */
    vuint32_t RBUF2;                    /* 0x288 */
    vuint32_t RBUF3;                    /* 0x28C */
    vuint32_t RBUF4;                    /* 0x290 */
    vuint32_t RBUF5;                    /* 0x294 */
    vuint32_t RBUF6;                    /* 0x298 */
    vuint32_t RBUF7;                    /* 0x29C */
    vuint32_t RBUF8;                    /* 0x2A0 */
    vuint32_t RBUF9;                    /* 0x2A4 */
    vuint32_t RBUF10;                   /* 0x2A8 */
    vuint32_t RBUF11;                   /* 0x2AC */
    vuint32_t RBUF12;                   /* 0x2B0 */
    vuint32_t RBUF13;                   /* 0x2B4 */
    vuint32_t RBUF14;                   /* 0x2B8 */
    vuint32_t RBUF15;                   /* 0x2BC */
    vuint32_t resv_i[848];
    vuint32_t WFIFOCTL;			/* 0x1000 */
#define WFIFOCTL_WENA			(1<<16)
#define WFIFOCTL_WNUMEVT(x)		(x<<8)
#define WFIFOCTL_WNUMDMA(x)		(x<<0)
    vuint32_t WFIFOSTS;			/* 0x1004 */
#define WFIFOSTS_WLVL			(0xff)
    vuint32_t RFIFOCTL;			/* 0x1008 */
#define RFIFOCTL_RENA			(1<<16)
#define RFIFOCTL_RNUMEVT(x)		(x<<8)
#define RFIFOCTL_RNUMDMA(x)		(x<<0)
    vuint32_t RFIFOSTS;			/* 0x100C */
#define RFIFOSTS_RLVL			(0xff)
}mcasp_t;

#define SET_DRAE0(x) ( (x <= 31) ? (mcasp_card->edma3->DRAE0 |= (1 << x)) : (mcasp_card->edma3->DRAEH0 |= (1 << (x - 32))))
#define SET_DRAE1(x) ( (x <= 31) ? (mcasp_card->edma3->DRAE1 |= (1 << x)) : (mcasp_card->edma3->DRAEH1 |= (1 << (x - 32))))
#define SET_DRAE2(x) ( (x <= 31) ? (mcasp_card->edma3->DRAE2 |= (1 << x)) : (mcasp_card->edma3->DRAEH2 |= (1 << (x - 32))))
#define SET_DRAE3(x) ( (x <= 31) ? (mcasp_card->edma3->DRAE3 |= (1 << x)) : (mcasp_card->edma3->DRAEH3 |= (1 << (x - 32))))
#define CLR_DRAE0(x) ( (x <= 31) ? (mcasp_card->edma3->DRAE0 &= ~(1 << x)) : (mcasp_card->edma3->DRAEH0 &= ~(1 << (x - 32))))
#define CLR_DRAE1(x) ( (x <= 31) ? (mcasp_card->edma3->DRAE1 &= ~(1 << x)) : (mcasp_card->edma3->DRAEH1 &= ~(1 << (x - 32))))
#define CLR_DRAE2(x) ( (x <= 31) ? (mcasp_card->edma3->DRAE2 &= ~(1 << x)) : (mcasp_card->edma3->DRAEH2 &= ~(1 << (x - 32))))
#define CLR_DRAE3(x) ( (x <= 31) ? (mcasp_card->edma3->DRAE3 &= ~(1 << x)) : (mcasp_card->edma3->DRAEH3 &= ~(1 << (x - 32))))

#define SET_EER(x) ( (x <= 31) ? (mcasp_card->edma3->EESR = (1 << x)) : (mcasp_card->edma3->EESRH = (1 << (x - 32))))
#define CLR_EER(x) ( (x <= 31) ? (mcasp_card->edma3->EECR = (1 << x)) : (mcasp_card->edma3->EECRH = (1 << (x - 32))))

typedef struct EDMA3
{
   vuint32_t PID;			/* 0x000 */
   vuint32_t CCCFG;			/* 0x004 */
   uint8_t res_a[504];
   vuint32_t QCHAMP0;		/* 0x200 */
   vuint32_t QCHAMP1;		/* 0x204 */
   vuint32_t QCHAMP2;		/* 0x208 */
   vuint32_t QCHAMP3;		/* 0x20c */
   vuint32_t QCHAMP4;		/* 0x210 */
   vuint32_t QCHAMP5;		/* 0x214 */
   vuint32_t QCHAMP6;		/* 0x218 */
   vuint32_t QCHAMP7;		/* 0x21c */
   uint8_t res_c[32];
   vuint32_t DMAQNUM0;		/* 0x240 */
   vuint32_t DMAQNUM1;		/* 0x244 */
   vuint32_t DMAQNUM2;		/* 0x248 */
   vuint32_t DMAQNUM3;		/* 0x24c */
   vuint32_t DMAQNUM4;		/* 0x250 */
   vuint32_t DMAQNUM5;		/* 0x254 */
   vuint32_t DMAQNUM6;		/* 0x258 */
   vuint32_t DMAQNUM7;		/* 0x25c */
   vuint32_t QDMAQNUM;		/* 0x260 */
   uint8_t res_d[32];
   vuint32_t QPRI;			/* 0x284 */
   uint8_t res_e[120];
   vuint32_t EMR;			/* 0x300 */
   vuint32_t EMRH;			/* 0x304 */
   vuint32_t EMCR;			/* 0x308 */
   vuint32_t EMCRH;			/* 0x30c */
   vuint32_t QEMR;			/* 0x310 */
   vuint32_t QEMCR;			/* 0x314 */
   vuint32_t CCERR;			/* 0x318 */
   vuint32_t CCERRCLR;		/* 0x31c */
   vuint32_t EEVAL;			/* 0x320 */
   uint8_t res_f[28];
   vuint32_t DRAE0;			/* 0x340 */
   vuint32_t DRAEH0;		/* 0x344 */
   vuint32_t DRAE1;			/* 0x348 */
   vuint32_t DRAEH1;		/* 0x34c */
   vuint32_t DRAE2;			/* 0x350 */
   vuint32_t DRAEH2;		/* 0x354 */
   vuint32_t DRAE3;			/* 0x358 */
   vuint32_t DRAEH3;		/* 0x35c */
   uint8_t res_g[32];
   vuint32_t QRAE0;			/* 0x380 */
   vuint32_t QRAE1;			/* 0x384 */
   vuint32_t QRAE2;			/* 0x388 */
   vuint32_t QRAE3;			/* 0x38c */
   uint8_t res_h[112];
   vuint32_t Q0E[16];		/* 0x400 */
   vuint32_t Q1E[16];		/* 0x440 */
   uint8_t res_i[384];
   vuint32_t QSAT0;			/* 0x600 */
   vuint32_t QSAT1;			/* 0x604 */
   uint8_t res_j[24];
   vuint32_t QWMTHRA;		/* 0x620 */
   uint8_t res_k[28];
   vuint32_t CCSTAT;		/* 0x640 */
   uint8_t res_l[6588];

   vuint32_t ER;			/* 0x2000 */
   vuint32_t ERH;			/* 0x2004 */
   vuint32_t ECR;			/* 0x2008 */
   vuint32_t ECRH;			/* 0x200c */
   vuint32_t ESR;			/* 0x2010 */
   vuint32_t ESRH;			/* 0x2014 */
   vuint32_t CER;			/* 0x2018 */
   vuint32_t CERH;			/* 0x201c */
   vuint32_t EER;			/* 0x2020 */
   vuint32_t EERH;			/* 0x2024 */
   vuint32_t EECR;			/* 0x2028 */
   vuint32_t EECRH;			/* 0x202c */
   vuint32_t EESR;			/* 0x2030 */
   vuint32_t EESRH;			/* 0x2034 */
   vuint32_t SER;			/* 0x2038 */
   vuint32_t SERH;			/* 0x203c */
   vuint32_t SECR;			/* 0x2040 */
   vuint32_t SECRH;			/* 0x2044 */
   vuint32_t RES_A;			/* 0x2048 */
   vuint32_t RES_B;			/* 0x204c */
   vuint32_t IER;			/* 0x2050 */
   vuint32_t IERH;			/* 0x2054 */
   vuint32_t IECR;			/* 0x2058 */
   vuint32_t IECRH;			/* 0x205c */
   vuint32_t IESR;			/* 0x2060 */
   vuint32_t IESRH;			/* 0x2064 */
   vuint32_t IPR;			/* 0x2068 */
   vuint32_t IPRH;			/* 0x206c */
   vuint32_t ICR;			/* 0x2070 */
   vuint32_t ICRH;			/* 0x2074 */
   vuint32_t IEVAL;			/* 0x2078 */
   vuint32_t RES_C;			/* 0x207c */
   vuint32_t QER;			/* 0x2080 */
   vuint32_t QEER;			/* 0x2084 */
   vuint32_t QEECR;			/* 0x2088 */
   vuint32_t QEESR;			/* 0x208c */
   vuint32_t QSER;			/* 0x2090 */
   vuint32_t QSECR;			/* 0x2094 */
   uint8_t res_m[8040];

   struct PaRAM
   {
      vuint32_t OPT;		/* 0x00 */
#define OPT_PRIVID(x)		(x<<24)
#define OPT_ITCCHEN			(1<<23)
#define OPT_TCCHEN			(1<<22)
#define OPT_ITCINTEN		(1<<21)
#define OPT_TCINTEN			(1<<20)
#define OPT_TCC(x)			(x<<12)
#define OPT_TCCMODE			(1<<11)
#define OPT_FWID(x)			(x<<8)
#define OPT_STATIC			(1<<3)
#define OPT_SYNCDIM			(1<<2)
#define OPT_DAM				(1<<1)
#define OPT_SAM				(1<<0)
      vuint32_t SRC;		/* 0x04 */
      vuint16_t ACNT;		/* 0x08 */
      vuint16_t BCNT;		/* 0x0a */
      vuint32_t DST;		/* 0x0c */
      vuint16_t SRCBIDX;	/* 0x10 */
      vuint16_t DSTBIDX;	/* 0x12 */
      vuint16_t LINK;		/* 0x14 */
      vuint16_t BCNTRLD;	/* 0x16 */
      vuint16_t SRCCIDX;	/* 0x18 */
      vuint16_t DSTCIDX;	/* 0x1a */
      vuint16_t CCNT;		/* 0x1c */
      vuint16_t RES;		/* 0x1e */
   } PaRAM[64 + 64];
}edma3_t;

typedef struct mcasp_subchn {
	ado_pcm_subchn_t   *pcm_subchn;
	ado_pcm_config_t   *pcm_config;
	int32_t            pcm_offset;      /* holds offset of data populated in PCM subchannel buffer */
	uint8_t            go;              /* indicates if trigger GO has been issue by client for data transfer */
	void               *strm;			/* pointer back to parent stream structure */
} mcasp_subchn_t;

#define SERIALIZER_ENABLED 1

typedef struct mcasp_play_strm {
	ado_pcm_cap_t      pcm_caps;
	ado_pcm_hw_t       pcm_funcs;
	uint32_t           pcm_cur_frag;		/* Used to index the fragment to be scheduled next */
	uint32_t           dma_idx;	
	int32_t            irq;
	int                voices;
	mcasp_subchn_t     subchn;
	uint32_t           sample_rate;
	uint32_t           sample_rate_min;
	uint32_t           sample_rate_max;
	uint8_t            protocol;
	uint8_t            bit_delay;
	uint8_t            fsync_pol;
	uint8_t            clk_pol;
#define NUMBER_OF_SERIALIZER 6
	uint8_t            serializer[NUMBER_OF_SERIALIZER];
	uint8_t            serializer_cnt;
	void               *reconstitute_buffer;
}mcasp_play_strm_t;

typedef struct mcasp_cap_strm {
	ado_pcm_t          *pcm;
	ado_pcm_cap_t      pcm_caps;
	ado_pcm_hw_t       pcm_funcs;
	uint32_t           dma_idx;	
	int32_t            irq;
	int                voices;
	uint32_t           nsubchn;  /* number of supported subchannels */
	mcasp_subchn_t     subchn[MAX_CAP_SUBCHN_COUNT];
	uint8_t            serializer;
}mcasp_cap_strm_t;

typedef struct mcasp_cap_interfaces {
	mcasp_cap_strm_t   cap_strm[NUMBER_OF_SERIALIZER];
	uint8_t            serializer_cnt;
	uint8_t            active;	/* Capture DMA is active */
	uint32_t           pcm_cur_frag;		/* Used to index the fragment to be scheduled next */
	uint32_t           pcm_completed_frag;	/* Used to index fragment just completed by the DMA */
	uint32_t           sample_rate;
	uint32_t           sample_rate_min;
	uint32_t           sample_rate_max;
	uint8_t            protocol;
	uint8_t            bit_delay;
	uint8_t            fsync_pol;
	uint8_t            clk_pol;
	/* DMA buffer info for multi-subchn and/or multi-serializer devices */
	ado_pcm_dmabuf_t   *dmabuf;
	volatile int32_t   frag_size;          /* Fragment/block size for DMA buffer */
}mcasp_cap_interface_t;


typedef void (*select_clk_mux_t)(HW_CONTEXT_T *);

struct ahclk {
	unsigned io;
	unsigned num_freqs;
	uint32_t freqs[MAX_HIGH_CLK_FREQS];
	select_clk_mux_t mux_select;
	unsigned freq_idx;
	unsigned afs_ctl;
};

struct mcasp_card {
	ado_pcm_t  			*pcm;
	ado_card_t			*card;
	ado_mutex_t			hw_lock;
	ado_mixer_t			*mixer;
	uint32_t			mcasp_idx;
	uint32_t			mcasp_baseaddr;
	uint32_t			mcasp_dmax;
	uint32_t			codec_i2c_addr;
	uint8_t				i2c_dev;
	uint8_t				sample_size;
	void				*pllss;
	mcasp_t 	        *mcasp;
	edma3_t				*edma3;
	mcasp_play_strm_t	play_strm;
	mcasp_cap_interface_t cap_aif;	
	int					clk_mode;
#define					MASTER		0
#define					SLAVE		1
	uint32_t			mclk;
	struct ahclk		ahclkx;
	struct ahclk		ahclkr;
	uint8_t             async_clks;
	uint8_t             fs_active_width;
	uint32_t			slot_size;
	uint8_t 			slot_num;
#define                 FS_WORD     0
#define                 FS_BIT      1
	uint8_t             loopback;
};
typedef struct mcasp_card mcasp_card_t;

#define INPUT  0
#define OUTPUT 1

#define PROTOCOL_TDM_I2S   0
#define PROTOCOL_TDM_LJ    1
#define PROTOCOL_SPDIF     2
#define PROTOCOL_TDM_PCM   3

int setup_mux_select_funcs(HW_CONTEXT_T *mcasp_card);
int codec_mixer ( ado_card_t *card , HW_CONTEXT_T *hwc );
uint32_t num_open_capture_subchn(mcasp_cap_strm_t *strm);
#endif	

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/deva/ctrl/mcasp/mcasp.h $ $Rev: 760918 $")
#endif
