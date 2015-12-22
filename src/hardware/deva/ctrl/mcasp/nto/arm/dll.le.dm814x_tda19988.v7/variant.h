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


#define VARIANT_DM814X_TDA19988

#define MCLK                 24576000 /* External Oscillator enabled via GPIO1_27 */
#define AHCLKX_FREQ          24576000
#define AHCLKR_FREQ          AHCLKX_FREQ
#define PLAYBACK_SAMPLE_RATE 48000
#define CAPTURE_SAMPLE_RATE  PLAYBACK_SAMPLE_RATE

#define SAMPLE_SIZE          2 // in bytes
#define PROTOCOL             PROTOCOL_TDM_I2S
#define TDM_SLOT_SIZE        32
#define TDM_NSLOTS           2
#define RX_VOICES            0
#define TX_VOICES            2
#define AHCLKX_DIR           INPUT  /* External Oscillator is connected to this pin */
#define AHCLKR_DIR           INPUT  /* AHCLKR is an input */

#define CLOCK_MODE           MASTER
#define ASYNC_CLKS           0
#define FS_ACTIVE_WIDTH      FS_WORD

#define RX_TDM_SLOT_START    0
#define TX_TDM_SLOT_START    0

#define RX_SERIALIZER        0
#define TX_SERIALIZER        2

#define CODEC_SLAVE_ADDR1    0x70

/* Board Specific Mapping */
#define EDMA3CC_ADDR         0x49000000
#define MCASP5_BASEADDR      0x4A1AE000
#define MCASP4_BASEADDR      0x4A1A8000
#define MCASP3_BASEADDR      0x4A1A2000
#define MCASP2_BASEADDR      0x48050000
#define MCASP1_BASEADDR      0x4803C000
#define MCASP0_BASEADDR      0x48038000
#define MCASP_SIZE           0x2C0

#define MCASP0_dMAX          (0x46000000)
#define MCASP1_dMAX          (0x46400000)
#define MCASP2_dMAX          (0x46800000)
#define MCASP3_dMAX          (MCASP3_BASEADDR + 0x3000)
#define MCASP4_dMAX          (MCASP4_BASEADDR + 0x3000)
#define MCASP5_dMAX          (MCASP5_BASEADDR + 0x3000)

#define MCASP0_DMA_PLAY      8
#define MCASP0_DMA_CAP       9
#define MCASP1_DMA_PLAY      10
#define MCASP1_DMA_CAP       11
#define MCASP2_DMA_PLAY      12
#define MCASP2_DMA_CAP       13
#define MCASP3_DMA_PLAY      56
#define MCASP3_DMA_CAP       57
#define MCASP4_DMA_PLAY      62
#define MCASP4_DMA_CAP       63
#define MCASP5_DMA_PLAY      4
#define MCASP5_DMA_CAP       5

#define MCASP0_TX_IRQ        80
#define MCASP0_RX_IRQ        81
#define MCASP1_TX_IRQ        82
#define MCASP1_RX_IRQ        83
#define MCASP2_TX_IRQ        84
#define MCASP2_RX_IRQ        85
#define MCASP3_TX_IRQ        105
#define MCASP3_RX_IRQ        106
#define MCASP4_TX_IRQ        108
#define MCASP4_RX_IRQ        109
#define MCASP5_TX_IRQ        110
#define MCASP5_RX_IRQ        111

#define DMA_RELOAD           64

#define EDMA_BASE_IRQ        0x200

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/deva/ctrl/mcasp/nto/arm/dll.le.dm814x_tda19988.v7/variant.h $ $Rev: 761911 $")
#endif
