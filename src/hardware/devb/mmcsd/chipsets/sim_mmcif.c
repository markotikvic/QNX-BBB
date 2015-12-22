/*
 * $QNXLicenseC:
 * Copyright 2013, QNX Software Systems.
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

#include	<sim_mmc.h>

#ifdef MMCSD_VENDOR_RENESAS

#include 	<arm/renesas_reg.h>
#include	<sim_mmcif.h>
#include 	<hw/renesas_dma.h>
#define	mmcif_CARD_STABLE	(mmcif_PSTATE_CD | mmcif_PSTATE_CSS | mmcif_PSTATE_CI)

/* MMCIF */

#define CONFIG_SH_MMCIF_FREQ	104000000

#define MMC_SWITCH_MODE_WRITE_BYTE	0x03 /* Set target byte to value */
#define EXT_CSD_BUS_WIDTH	183	/* R/W */
#define EXT_CSD_BUS_WIDTH_1	0	/* Card is in 1 bit mode */
#define EXT_CSD_BUS_WIDTH_4	1	/* Card is in 4 bit mode */
#define EXT_CSD_BUS_WIDTH_8	2	/* Card is in 8 bit mode */

struct mmcif_dma_config {
	 uint32_t	tx_addr;
	 uint32_t	rx_addr;
	 uint8_t	tx_mid_rid;
	 uint8_t	rx_mid_rid;
};

const uint16_t SYS_DMAC_DMARS_MMCIF_TX[MMCIIF_CHANNELS]={
	SYS_DMAC_DMARS_MMCIF0_TXI,
	SYS_DMAC_DMARS_MMCIF1_TXI
};
const uint16_t SYS_DMAC_DMARS_MMCIF_RX[MMCIIF_CHANNELS]={
	SYS_DMAC_DMARS_MMCIF0_RXI,
	SYS_DMAC_DMARS_MMCIF1_RXI
};

struct mmcif_dma_config mmcif_dma_slave[MMCIIF_CHANNELS] = 
{
	{0xee200034, 0xee200034, 0xd1, 0xd2},
	{0xee220034, 0xee220034, 0xe1, 0xe2}
};

static unsigned long mmcif_read(unsigned long base, int reg) {
	return in32(base + reg);
}

static void mmcif_write(unsigned long base, int reg, uint32_t val) {
	out32(base + reg, val);
}

static void mmcif_bitset(unsigned long base, unsigned int reg, uint32_t val) {
	out32(base + reg, val | in32(base + reg));
}

static void mmcif_bitclr(unsigned long base, unsigned int reg, uint32_t val) {
	out32(base + reg, ~val & in32(base + reg));
}

static void mmcif_sync_reset(SIM_HBA *hba) {
	SIM_MMC_EXT *ext;
	mmcif_ext_t *mmcif;
	uintptr_t base;
	uint32_t tmp;

	ext = (SIM_MMC_EXT *) hba->ext;
	mmcif = (mmcif_ext_t *) ext->handle;
	base = mmcif->base;

	tmp = 0x010f0000 & mmcif_read(base, CE_CLK_CTRL);

	mmcif_write(base, CE_VERSION, SOFT_RST_ON);
	mmcif_write(base, CE_VERSION, SOFT_RST_OFF);
	mmcif_bitset(base, CE_CLK_CTRL, tmp | SRSPTO_256 | SRBSYTO_29 | SRWDTO_29
			| SCCSTO_29);


	if (mmcif->use_dma == 1)
	{
		if(ext->dir == MMC_DIR_IN)
		mmcif_bitclr(mmcif->base, CE_BUF_ACC, BUF_ACC_DMAREN);
		else
		mmcif_bitclr(mmcif->base, CE_BUF_ACC, BUF_ACC_DMAWEN);
	}


	/* byte swap on */
	mmcif_bitset(base, CE_BUF_ACC, BUF_ACC_ATYP);
}

static int mmcif_error_manage(SIM_HBA *hba) {
	SIM_MMC_EXT *ext;
	mmcif_ext_t *mmcif;
	uintptr_t base;
	uint32_t state1, state2;
	int ret;

	ext = (SIM_MMC_EXT *) hba->ext;
	mmcif = (mmcif_ext_t *) ext->handle;
	base = mmcif->base;

	state1 = mmcif_read(base, CE_HOST_STS1);
	state2 = mmcif_read(base, CE_HOST_STS2);

	if (state1 & STS1_CMDSEQ) {
		mmcif_bitset(base, CE_CMD_CTRL, CMD_CTRL_BREAK);
		mmcif_bitset(base, CE_CMD_CTRL, ~CMD_CTRL_BREAK);
		while (1) {
			if (!(mmcif_read(base, CE_HOST_STS1) & STS1_CMDSEQ))
				break;
			nanospin_ns(1000);
		}
		mmcif_sync_reset(hba);
		return -1;
	}

	if (state2 & STS2_CRC_ERR) {
		ret = -1;
	} else if (state2 & STS2_TIMEOUT_ERR) {
		ret = -1;
	} else {
		ret = 0;
	}
	return ret;
}

static int dma_init(mmcif_ext_t *mmcif)
{
	dma_channel_query_t play_chinfo, cap_chinfo;
	
	if (get_dmafuncs (&mmcif->dmafuncs, sizeof (mmcif->dmafuncs)) == -1)
	{
    slogf (_SLOGC_SIM_MMC, _SLOG_ERROR, "sim_mmcif.c: init_dma: failed to get DMA lib functions");
    return MMC_FAILURE;
	}

	mmcif->dmafuncs.init();
	if ((mmcif->write_chn.dma_chn  = mmcif->dmafuncs.channel_attach (NULL, NULL,0,
	SYS_DMAC_DMARS_MMCIF_TX[mmcif->channel], 0,DMA_ATTACH_ANY_CHANNEL))== NULL){
    slogf (_SLOGC_SIM_MMC, _SLOG_ERROR, "sim_mmcif.c: Unable to attach to TX DMA Channel");
    return MMC_FAILURE;
	}
	if ((mmcif->read_chn.dma_chn  = mmcif->dmafuncs.channel_attach (NULL, NULL,0,
	SYS_DMAC_DMARS_MMCIF_RX[mmcif->channel], 1,DMA_ATTACH_ANY_CHANNEL))== NULL){
    slogf (_SLOGC_SIM_MMC, _SLOG_ERROR, "sim_mmcif.c: Unable to attach to RX DMA Channel");
    return MMC_FAILURE;
	}
	mmcif->dmafuncs.query_channel (mmcif->write_chn.dma_chn, &play_chinfo);
	mmcif->dmafuncs.query_channel (mmcif->read_chn.dma_chn, &cap_chinfo);	
	return (MMC_SUCCESS);
}

static int _mmcif_detect(SIM_HBA *hba) {
	return (MMC_SUCCESS);
}

static int _mmcif_interrupt(SIM_HBA *hba, int irq, int resp_type,
		uint32_t *resp) {
	SIM_MMC_EXT *ext;
	mmcif_ext_t *mmcif;
	uintptr_t base;
	int intr = 0;
	unsigned long state = 0;
	ext = (SIM_MMC_EXT *) hba->ext;
	mmcif = (mmcif_ext_t *) ext->handle;
	base = mmcif->base;
	state = mmcif_read(base, CE_INT);
	state &= mmcif_read(base, CE_INT_MASK);

	/* Respons End */
	if (state & (INT_CRSPE | INT_RBSYE)) {
		if (state & INT_RSPTO) {
			// Response Timeout ?
			intr |= MMC_ERR_CMD_TO | MMC_INTR_ERROR | MMC_INTR_COMMAND;
			mmcif_bitclr(base, CE_INT, INT_RSPTO); //Clear Flag Response Timeout
		} else {
			intr |= MMC_INTR_COMMAND;
			mmcif_write(base, CE_INT, ~(INT_CRSPE | INT_RBSYE));
			//mmcif_bitclr(base, CE_INT_MASK, MASK_MCRSPE);
			if ((mmcif_read(base, CE_CMD_SET) & CMD_SET_RBSY) == 0) {
				if (state & (INT_RIDXERR | INT_RSPERR))
					intr |= MMC_INTR_ERROR;
				if (resp_type & (MMC_RSP_136 | MMC_RSP_PRESENT)) {
					resp[0] = mmcif_read(base, CE_RESP0);
					resp[1] = mmcif_read(base, CE_RESP1);
					resp[2] = mmcif_read(base, CE_RESP2);
					resp[3] = mmcif_read(base, CE_RESP3);
				} else {
					resp[0] = mmcif_read(base, CE_RESP0);
					resp[1] = 0;
					resp[2] = 0;
					resp[3] = 0;
				}
			}
			mmcif->ResponseEnd = 1;
		}
	}
	if (state & (INT_CMD12DRE | INT_CMD12RBE)) {
		mmcif_write(base, CE_INT, ~(INT_CMD12DRE | INT_CMD12RBE | INT_CMD12CRE
				| INT_BUFRE));
	}
	if (state & INT_ERR_STS) {
		delay(20);
		mmcif_write(base, CE_INT, ~INT_ERR_STS);
		intr |= MMC_ERR_CMD_TO | MMC_INTR_ERROR | MMC_INTR_COMMAND;
		mmcif_error_manage(hba);
	}
	/* SD_BUF Read/Write Enable */
	//if (mmcif->DMAmode == 0)//for PIO
	{
		if (state & (INT_BUFREN)) {
			intr |= MMC_INTR_RBRDY;
		}
		if (state & (INT_BUFWEN)) {
			intr |= MMC_INTR_WBRDY;
		}
	}
	/* Access End */
	if (state & (INT_BUFRE | INT_DTRANE)) {
		mmcif_write(base, CE_INT, ~(INT_BUFRE | INT_DTRANE));
		/* data done */
		intr |= MMC_INTR_DATA;
		if (mmcif->use_dma == 1)
		{
			if(ext->hccap & MMC_HCCAP_DMA)
			{
				if(ext->dir == MMC_DIR_IN)
				{
					mmcif->dmafuncs.xfer_complete (mmcif->read_chn.dma_chn);
				}
				else
				{
					mmcif->dmafuncs.xfer_complete (mmcif->write_chn.dma_chn);
				}	
			}
		}
	}

	return intr;
}

/*
 * setup DMA transfer
 */

static int _mmcif_setup_dma(SIM_HBA *hba, paddr_t paddr, int len, int dir)
{

	SIM_MMC_EXT *ext=(SIM_MMC_EXT *)hba->ext;
	mmcif_ext_t *mmcif=(mmcif_ext_t *)ext->handle;
	uint16_t blkcnt = len / mmcif->blksz;
	dma_transfer_t tinfo;
	dma_addr_t saddr, daddr;
	mmcif->use_dma = 1;

	if (blkcnt == 0) return 0;

	/* only valid for multi-block transfer	*/
	if (blkcnt > 1)
	{
		mmcif_write(mmcif->base, CE_BLOCK_SET, (mmcif_read(mmcif->base, CE_BLOCK_SET) & 0x0000FFFF) | (blkcnt<<16));
	}

	if(dir == MMC_DIR_IN)
	{
		mmcif_bitset(mmcif->base, CE_BUF_ACC, BUF_ACC_DMAREN);
	}
	else
	{
		mmcif_bitset(mmcif->base, CE_BUF_ACC, BUF_ACC_DMAWEN);
	}

	memset (&tinfo, 0, sizeof (tinfo));
	if(dir == MMC_DIR_IN)
	{			
		saddr.paddr = mmcif_dma_slave[mmcif->channel].rx_addr;
		tinfo.src_addrs = &saddr;
		tinfo.xfer_unit_size = 4;
		tinfo.xfer_bytes =len;
		daddr.paddr = paddr;
		tinfo.dst_addrs = &daddr;
		tinfo.dst_flags = 0;
		tinfo.src_flags = DMA_ADDR_FLAG_NO_INCREMENT;
		mmcif->dmafuncs.setup_xfer (mmcif->read_chn.dma_chn, &tinfo);
		mmcif->dmafuncs.xfer_start (mmcif->read_chn.dma_chn);
	}
	else
	{
		saddr.paddr = paddr;
		tinfo.src_addrs = &saddr;
		tinfo.xfer_unit_size = 4;
		tinfo.xfer_bytes =len;
		daddr.paddr = mmcif_dma_slave[mmcif->channel].tx_addr;
		tinfo.dst_addrs = &daddr;
		tinfo.dst_flags = DMA_ADDR_FLAG_NO_INCREMENT;
		tinfo.src_flags = 0;
		mmcif->dmafuncs.setup_xfer (mmcif->write_chn.dma_chn, &tinfo);
		mmcif->dmafuncs.xfer_start (mmcif->write_chn.dma_chn);
	}
	/* we don't need the DMA interrupt */
	return (len);
}


/*
 * setup PIO transfer
 */
static int _mmcif_setup_pio(SIM_HBA *hba, int len, int dir) {

	SIM_MMC_EXT *ext;
	mmcif_ext_t *mmcif;
	uintptr_t base;
	uint16_t blkcnt;
	ext = (SIM_MMC_EXT *) hba->ext;
	mmcif = (mmcif_ext_t *) ext->handle;
	base = mmcif->base;
	blkcnt = len / mmcif->blksz;

	mmcif->use_dma = 0;

	if (blkcnt == 0)
		return 0;

	/* only valid for multi-block transfer	*/
	if (blkcnt > 1) {
		mmcif_write(base, CE_BLOCK_SET, (mmcif_read(base, CE_BLOCK_SET)
				& 0x0000FFFF) | (blkcnt << 16));
	}
	return (blkcnt * mmcif->blksz);
}

static int _mmcif_dma_done(SIM_HBA *hba, int dir)
{
	SIM_MMC_EXT *ext;
	mmcif_ext_t *mmcif;

	ext = (SIM_MMC_EXT *)hba->ext;
	mmcif= (mmcif_ext_t *)ext->handle;

	if(dir == MMC_DIR_IN)
	{
		mmcif->dmafuncs.xfer_complete (mmcif->read_chn.dma_chn);
	}
	else
	{
		mmcif->dmafuncs.xfer_complete (mmcif->write_chn.dma_chn);
	}	
	if(dir == MMC_DIR_IN)
		mmcif_bitclr(mmcif->base, CE_BUF_ACC, BUF_ACC_DMAREN);
	else
		mmcif_bitclr(mmcif->base, CE_BUF_ACC, BUF_ACC_DMAWEN);

	mmcif->use_dma = 0;

	return MMC_SUCCESS;
}


static int _mmcif_pio_done(SIM_HBA *hba, char *buf, int len, int dir) {
	long timeout1, timeout2;
	uint32_t cnt;
	uint32_t *pbuf = (uint32_t *) buf;
	SIM_MMC_EXT *ext = (SIM_MMC_EXT *) hba->ext;
	mmcif_ext_t *mmcif = (mmcif_ext_t *) ext->handle;
	uintptr_t base = mmcif->base;

	if (len < mmcif->blksz)
		return 0;

	timeout1 = 1000000;
	timeout2 = 1000000;

	if (dir == MMC_DIR_IN) {
		while (timeout2 > 0) {
			timeout2--;
			if (mmcif_read(base, CE_INT) & (INT_BUFREN)) {
				mmcif_write(base, CE_INT, ~(INT_BUFREN));
				break;
			}
		}
		if (timeout2 == 0) {
			return 0;
		}
		while (mmcif->ResponseEnd == 0) {
			nanospin_ns(10);
			timeout1--;
			if (timeout1 < 1) {
				return 0;
			}
		}
		for (cnt = 0; cnt < mmcif->blksz; cnt += 4) {
			*pbuf++ = mmcif_read(base, CE_DATA);
		}
	} else {
		while (timeout1 > 0) {
			timeout1--;
			if (mmcif_read(base, CE_INT) & (INT_BUFWEN)) {
				mmcif_write(base, CE_INT, ~(INT_BUFWEN));
				break;
			}
		}
		if (timeout1 == 0) {
			return 0;
		}

		for (cnt = 0; cnt < mmcif->blksz; cnt += 4) {
			mmcif_write(base, CE_DATA, *pbuf++);
		}
	}
	return mmcif->blksz;
}

static unsigned long mmcif_set_cmd(mmc_cmd_t *cmd, unsigned long opc, int data,
		uint8_t bus_w) {
	unsigned long tmp = 0;

	/* Response Type check */
	switch (cmd->resp) {
	case MMC_RSP_NONE:
		tmp |= CMD_SET_RTYP_NO;
		break;
	case MMC_RSP_R1:
	case MMC_RSP_R1b:
	case MMC_RSP_R3:
		tmp |= CMD_SET_RTYP_6B;
		break;
	case MMC_RSP_R2:
		tmp |= CMD_SET_RTYP_17B;
		break;
	default:
		printf("MMC: Not support type response.\n");
		break;
	}

	switch (opc) {
	/* RBSY */
	case SD_SET_BUS_WIDTH:
	case MMC_STOP_TRANSMISSION:
		tmp |= CMD_SET_RBSY;
		break;
	}

	/* WDAT / DATW */
	if (data) {
		tmp |= CMD_SET_WDAT;
		if (bus_w == 4)
			tmp |= CMD_SET_DATW_4;
		if (bus_w == 8)
			tmp |= CMD_SET_DATW_8;
	}

	/* DWEN */
	if ((opc == MMC_WRITE_BLOCK) || (opc == MMC_WRITE_MULTIPLE_BLOCK))
		tmp |= CMD_SET_DWEN;

	/* CMLTE/CMD12EN */
	if ((opc == MMC_READ_MULTIPLE_BLOCK) || (opc == MMC_WRITE_MULTIPLE_BLOCK))
		tmp |= CMD_SET_CMLTE | CMD_SET_CMD12EN;

	/* RIDXC[1:0] check bits */
	if (opc == MMC_SEND_OP_COND || opc == MMC_ALL_SEND_CID || opc
			== MMC_SEND_CSD || opc == MMC_SEND_CID)
		tmp |= CMD_SET_RIDXC_BITS;
	/* RCRC7C[1:0] check bits */
	if (opc == MMC_SEND_OP_COND)
		tmp |= CMD_SET_CRC7C_BITS;
	/* RCRC7C[1:0] internal CRC7 */
	if (opc == MMC_ALL_SEND_CID || opc == MMC_SEND_CSD || opc == MMC_SEND_CID)
		tmp |= CMD_SET_CRC7C_INTERNAL;

	return opc = ((opc << 24) | tmp);
}

static int _mmcif_command(SIM_HBA *hba, mmc_cmd_t *cmd) {
	SIM_MMC_EXT *ext;
	mmcif_ext_t *mmcif;
	uintptr_t base;
	uint16_t command;
	uint32_t mask, opc;
	uint8_t bus_w;
	int ret = MMC_SUCCESS;

	command = 0;
	ext = (SIM_MMC_EXT *) hba->ext;
	mmcif = (mmcif_ext_t *) ext->handle;
	base = mmcif->base;
	command = cmd->opcode;
	bus_w = ext->bus_width;

	if (mmcif_read(base, CE_HOST_STS1) & STS1_CMDSEQ) {
		while (1) {
			if (!(mmcif_read(base, CE_HOST_STS1) & STS1_CMDSEQ))
				break;
			nanospin_ns(1000);
		}
	}

	switch (command) {
	/* respons busy check */
	case SD_SET_BUS_WIDTH:
	case MMC_STOP_TRANSMISSION:
		mask = MASK_MRBSYE;
		break;
	default:
		mask = MASK_MCRSPE;
		break;
	}

	mask |= MASK_MCMDVIO | MASK_MBUFVIO | MASK_MWDATERR | MASK_MRDATERR
			| MASK_MRIDXERR | MASK_MRSPERR | MASK_MCCSTO | MASK_MCRCSTO
			| MASK_MWDATTO | MASK_MRDATTO | MASK_MRBSYTO | MASK_MRSPTO;

	opc = mmcif_set_cmd(cmd, command, (cmd->eflags & MMC_CMD_DATA) ? 1 : 0,
			bus_w);

	mmcif_write(base, CE_INT, 0xD80430C0);
	mmcif_write(base, CE_INT_MASK, 0x00F3CF1F);
	/* set arg */
	mmcif_write(base, CE_ARG, cmd->argument);
	/* set cmd */
	mmcif_write(base, CE_CMD_SET, opc);

	return ret;
}

static int _mmcif_cfg_bus(SIM_HBA *hba, int width, int mmc) {

	SIM_MMC_EXT *ext;
	mmcif_ext_t *mmcif;
	uintptr_t base;
	uint32_t opc, cmdarg;

	ext = (SIM_MMC_EXT *) hba->ext;
	mmcif = (mmcif_ext_t *) ext->handle;
	base = mmcif->base;

	/* Because mmcsd.c does not support MMC transfer in 8 bits. So we support that here */
	opc = (SD_SET_BUS_WIDTH << 24);
	opc |= CMD_SET_RTYP_6B;

	cmdarg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) | (EXT_CSD_BUS_WIDTH << 16)
			| (EXT_CSD_BUS_WIDTH_8 << 8);

	mmcif_write(base, CE_INT, 0xD80430C0);
	/* set arg */
	mmcif_write(base, CE_ARG, cmdarg);
	/* set cmd */
	mmcif_write(base, CE_CMD_SET, opc);
	delay(2);
	mmcif_write(base, CE_INT, 0xD80430C0);
	ext->bus_width = 8;//width;

	return (MMC_SUCCESS);
}

static int _mmcif_clock(SIM_HBA *hba, int *clock, int high_speed) {
	SIM_MMC_EXT *ext;
	mmcif_ext_t *mmcif;
	uintptr_t base;
	uint16_t clkdiv;
	uint32_t clk;
	int i;

	ext = (SIM_MMC_EXT *) hba->ext;
	mmcif = (mmcif_ext_t *) ext->handle;
	base = mmcif->base;
	clk = mmcif->clock;

	if (*clock > clk || *clock < (clk / 512))
		return (MMC_FAILURE);

	clkdiv = 0x9;
	i = CONFIG_MMCIF_FREQ >> (0x9 + 1);
	for (; clkdiv && *clock >= (i << 1); clkdiv--)
		i <<= 1;
	*clock = i << 1;

	mmcif_bitclr(base, CE_CLK_CTRL, 0xf << 16);
	mmcif_bitset(base, CE_CLK_CTRL, clkdiv << 16);

	return (MMC_SUCCESS);
}

static int _mmcif_block_size(SIM_HBA *hba, int blksz) {
	SIM_MMC_EXT *ext;
	mmcif_ext_t *mmcif;
	uintptr_t base;
	ext = (SIM_MMC_EXT *) hba->ext;
	mmcif = (mmcif_ext_t *) ext->handle;
	base = mmcif->base;
	if (blksz > 512) {
		return (MMC_FAILURE);
	}
	mmcif->blksz = blksz;

	mmcif_write(base, CE_BLOCK_SET, (mmcif_read(base, CE_BLOCK_SET)
			& 0xFFFF0000) | blksz);

	return (MMC_SUCCESS);
}

/*
 * Reset host controller and card
 * The clock should be enabled and set to minimum (<400KHz)
 */
static int _mmcif_powerup(SIM_HBA *hba) {
	SIM_MMC_EXT *ext;
	mmcif_ext_t *mmcif;

	ext = (SIM_MMC_EXT *) hba->ext;
	mmcif = (mmcif_ext_t *) ext->handle;

	/*Enable MMCIF operates */
	mmcif_clock_enable(mmcif, mmcif->channel);

	/* Reset the MMCIF module */

	return (MMC_SUCCESS);
}

static int _mmcif_powerdown(SIM_HBA *hba) {
	return (MMC_SUCCESS);
}

static int _mmcif_shutdown(SIM_HBA *hba) {
	SIM_MMC_EXT *ext;
	mmcif_ext_t *mmcif;

	ext = (SIM_MMC_EXT *) hba->ext;
	mmcif = (mmcif_ext_t *) ext->handle;

	_mmcif_powerdown(hba);
	mmcif->dmafuncs.channel_release (mmcif->read_chn.dma_chn);
	mmcif->dmafuncs.channel_release (mmcif->write_chn.dma_chn);
	munmap_device_io((uintptr_t) mmcif->mmcif_up, 4);
	munmap_device_io(mmcif->base, 0x1000);

	free(mmcif);
	return (MMC_SUCCESS);
}

int mmcif_init(SIM_HBA *hba) {
	CONFIG_INFO *cfg;
	SIM_MMC_EXT *ext;
	mmcif_ext_t *mmcif;
	uintptr_t base;
	uint32_t iobase, irq;

	ext = (SIM_MMC_EXT *) hba->ext;
	cfg = (CONFIG_INFO *) &hba->cfg;
	ThreadCtl(_NTO_TCTL_IO, 0);
	hba->verbosity = 0;
	if ((mmcif = calloc(1, sizeof(mmcif_ext_t))) == NULL)
		return (MMC_FAILURE);

	mmcif->CardInsertion = 1;
//	mmcif->channel = ext->channel;
    // set channel to 0 for now
    mmcif->channel = 0;
    
	switch (mmcif->channel) {
		case 0:
			iobase = MMCIF0_BASE;
			irq = SPI_MMC0;
			break;
		case 1:
			iobase = MMCIF1_BASE;
			irq = SPI_MMC1;
			break;
		default:
//			slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "mmcif_init: channel %d is not supported", ext->channel);
			return (MMC_FAILURE);
	}
	if (!cfg->NumIOPorts) {
		cfg->IOPort_Base[0] = iobase;
		cfg->IOPort_Length[0] = 0x1000;
		cfg->NumIOPorts = 1;
	}
	if (!cfg->NumIRQs) {
		cfg->IRQRegisters[0] = irq;
		cfg->NumIRQs = 1;
	}
	if (!cfg->NumDMAs) {
		cfg->DMALst[0] = irq;
		cfg->NumDMAs = 1;
	}
	
	/* Reset */
	mmcif_reset(mmcif->channel);
	
	mmcif->mmc_pbase = cfg->IOPort_Base[0];
	if ((base = mmap_device_io(0x1000, mmcif->mmc_pbase))
			== (uintptr_t) MAP_FAILED) {
		slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "eMMC: mmap_device_io failed");
		goto fail;
	}
	if ((mmcif_port_init(mmcif->channel)) == MMC_FAILURE)
		goto fail;

	if ((mmcif_clock_enable(mmcif, mmcif->channel)) == MMC_FAILURE)
		goto fail;

	/* Reset the MMCIF module */
	mmcif_bitset(base, CE_VERSION, SOFT_RST_ON);
	usleep(1000);
	mmcif_bitclr(base, CE_VERSION, SOFT_RST_ON);

	mmcif_bitset(base, CE_CLK_CTRL, 0x9 << 16);
	mmcif_bitset(base, CE_CLK_CTRL, CLK_ENABLE);

	ext->hccap |= MMC_HCCAP_ACMD12 | MMC_HCCAP_BW4 | MMC_HCCAP_BW8
			| MMC_HCCAP_CD_INTR;	
	ext->hccap |= MMC_HCCAP_DMA;
	mmcif->use_dma = 0;
	//Enables the SD host interface to receive SDIO interrupt from the SDIO card.
	mmcif->clock 	= PCLK;
	mmcif->base 	= base;
	mmcif->hba 		= hba;
	ext->handle 	= mmcif;
	ext->clock 		= mmcif->clock;
	ext->detect 	= _mmcif_detect;
	ext->powerup 	= _mmcif_powerup;
	ext->powerdown 	= _mmcif_powerdown;
	ext->cfg_bus 	= _mmcif_cfg_bus;
	ext->set_clock 	= _mmcif_clock;
	ext->set_blksz 	= _mmcif_block_size;
	ext->interrupt 	= _mmcif_interrupt;
	ext->command 	= _mmcif_command;
	ext->setup_pio 	= _mmcif_setup_pio;
	ext->pio_done 	= _mmcif_pio_done;
	ext->shutdown 	= _mmcif_shutdown;
	ext->setup_dma 	= _mmcif_setup_dma;
	ext->dma_done 	= _mmcif_dma_done;
	if(ext->hccap & MMC_HCCAP_DMA)
	{
		if(dma_init(mmcif)==MMC_FAILURE)
		goto fail;
	}

	mmcif_sync_reset(hba);
	mmcif_write(base, CE_INT_MASK, MASK_ALL);

	if (!cfg->Description[0])
		strncpy((char *) cfg->Description, "RCarH2 Lager MMCIF",
				sizeof(cfg->Description));

	mmcif->CardInsertion = 1;

	//printf("mmcif_read(base, CE_INT_MASK)=%x\n", mmcif_read(base, CE_INT_MASK));
	return (MMC_SUCCESS);
fail: 
	free(mmcif);
	slogf(_SLOGC_SIM_MMC, _SLOG_INFO, " exiting sim_mmcif with failure");
	return (MMC_FAILURE);

}

/* MMCIF  PORT*/

int mmcif_port_init(int channel) {
	uintptr_t PFC_PMMR_val;
	uintptr_t PFC_IPSR9_val;
	uintptr_t PFC_IPSR10_val;
	uintptr_t PFC_IPSR11_val;
	uintptr_t PFC_GPSR3_val;
	uint32_t ui32Tmp1 = 0;
	
	PFC_PMMR_val    = mmap_device_io(4 ,PFC_PMMR);
	PFC_IPSR9_val   = mmap_device_io(4 ,PFC_IPSR9);
	PFC_IPSR10_val   = mmap_device_io(4 ,PFC_IPSR10);
	PFC_IPSR11_val   = mmap_device_io(4 ,PFC_IPSR11);
    PFC_GPSR3_val   = mmap_device_io(4 ,PFC_GPSR3);
	//CPLD_REG2_val  = mmap_device_io(1 , CPLD_REG2);
	
    if(	(PFC_PMMR_val==MAP_DEVICE_FAILED)||
		(PFC_IPSR9_val==MAP_DEVICE_FAILED) ||
		(PFC_IPSR10_val==MAP_DEVICE_FAILED) ||
		(PFC_IPSR11_val==MAP_DEVICE_FAILED) ||
		(PFC_GPSR3_val==MAP_DEVICE_FAILED))
    {
		slogf (_SLOGC_SIM_MMC, _SLOG_ERROR, "mmcif_port_init: GPIO map failed");
        return MMC_FAILURE;
    }
	
	switch(channel)
	{
		case 0:			
			/* GP3[6]    1:IP9[11:8]
			   GP3[7]    1:IP9[15:12]
			   GP3[16]   1:IP10[6:4]
			   GP3[17]   1:IP10[10:7]
			   GP3[18]   1:IP10[14:11]
			   GP3[19]   1:IP10[18:15]
			   GP3[20]   1:IP10[22:19]
			   GP3[21]   1:IP10[25:23] 
			   GP3[22]   1:IP10[29:26] 
			   GP3[23]   1:IP11[3:0] */
			ui32Tmp1  =  in32(PFC_GPSR3_val) & (0xFF00FF3F);
			ui32Tmp1 |=  (0x00FF00C0);
			out32(PFC_PMMR_val, ~ui32Tmp1);
			out32(PFC_GPSR3_val, ui32Tmp1);
			
			/* IP9[11:8]  	0001:MMC0_D6
			   IP9[15:12]  	0001:MMC0_D7 */
			ui32Tmp1  =  in32(PFC_IPSR9_val) & (0xFFFF00FF);
			ui32Tmp1 |=  (0x00001100);
			out32(PFC_PMMR_val, ~ui32Tmp1);
			out32(PFC_IPSR9_val, ui32Tmp1);		
			
			/* IP10[6:4]  		001:MMC0_CLK
			   IP10[10:7]  	   0001:MMC0_CMD
			   IP10[14:11]     0001:MMC0_D0
			   IP10[18:15]     0001:MMC0_D1
			   IP10[22:19] 	   0001:MMC0_D2
			   IP10[25:23]      001:MMC0_D3 
			   IP10[29:26]     0001:MMC0_D4 */
			ui32Tmp1  =  in32(PFC_IPSR10_val) & (0xC000000F);
			ui32Tmp1 |=  (1 << 4) | (1 << 7) | (1 << 11) | (1 << 15) | (1 << 19) | (1 << 23) | (1 << 26);
			out32(PFC_PMMR_val, ~ui32Tmp1);
			out32(PFC_IPSR10_val, ui32Tmp1);
			
			/* IP11[3:0]  0001:MMC0_D5 */
			ui32Tmp1  =  in32(PFC_IPSR11_val) & (0xFFFFFFF0);
			ui32Tmp1 |=  (1 << 0);
			out32(PFC_PMMR_val, ~ui32Tmp1);
			out32(PFC_IPSR11_val, ui32Tmp1);
			break;	
		case 1:
			/* GP3[14]   1:IP9[31:28]
			   GP3[15]   1:IP10[3:0]
			   GP3[24]   1:IP11[4]
			   GP3[25]   1:IP11[6:5]
			   GP3[26]   1:IP11[8:7]
			   GP3[27]   1:IP11[10:9]
			   GP3[28]   1:IP11[12:11]
			   GP3[29]   1:IP11[14:13]
			   GP3[30]   1:IP11[17:15]
			   GP3[31]   1:IP11[21:18] */
			ui32Tmp1 =  in32(PFC_GPSR3_val) & (0x00FF3FFF);
			ui32Tmp1 |=  (0xFF00C000);
			out32(PFC_PMMR_val, ~ui32Tmp1);
			out32(PFC_GPSR3_val, ui32Tmp1);

			/* IP9[31:28]  0001:MMC1_D6 */
			ui32Tmp1  =  in32(PFC_IPSR9_val) & (0x0FFFFFFF);
			ui32Tmp1 |=  (1 << 28);
			out32(PFC_PMMR_val, ~ui32Tmp1);
			out32(PFC_IPSR9_val, ui32Tmp1);
			
			/* IP10[3:0]  0001:MMC1_D7 */
			ui32Tmp1  =  in32(PFC_IPSR10_val) & (0xFFFFFFF0);
			ui32Tmp1 |=  (1 << 0);
			out32(PFC_PMMR_val, ~ui32Tmp1);
			out32(PFC_IPSR10_val, ui32Tmp1);
			
			/* IP11[4]  		   1:MMC1_CLK
			   IP11[6:5]  		  01:MMC1_CMD
			   IP11[8:7]  		  01:MMC1_D0
			   IP11[10:9]  		  01:MMC1_D1
			   IP11[12:11]  	  01:MMC1_D2
			   IP11[14:13]  	  01:MMC1_D3
			   IP11[17:15]  	 001:MMC1_D4 
			   IP11[21:18] 		0001:MMC1_D5 */
			ui32Tmp1  =  in32(PFC_IPSR11_val) & (0xFFC0000F);
			ui32Tmp1 |=  (1 << 4) | (1 << 5) | (1 << 7) | (1 << 9) | (1 << 11) | (1 << 13) | (1 << 15) | ( 1 << 18);
			out32(PFC_PMMR_val, ~ui32Tmp1);
			out32(PFC_IPSR11_val, ui32Tmp1);
			
			break;			
		default:
			slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "mmcif_port_init: channel %d is not supported", channel);
			return (MMC_FAILURE);
	}	
	
	if ((munmap_device_io(PFC_PMMR_val, 4) == -1) ||
		(munmap_device_io(PFC_IPSR9_val, 4) == -1)||
		(munmap_device_io(PFC_IPSR10_val, 4) == -1)||
		(munmap_device_io(PFC_IPSR11_val, 4) == -1)||
		(munmap_device_io(PFC_GPSR3_val, 4) == -1))
    {
		slogf (_SLOGC_SIM_MMC, _SLOG_ERROR, "mmcif_port_init: PFC GPIO unmap failed");
    }

	return (MMC_SUCCESS);
}

int mmcif_clock_enable(mmcif_ext_t *mmcif, int channel) {
	uint32_t 	tmp, mmcckcr;
	uintptr_t	pmmcckcr;

	mmcif->mmcif_up = (uintptr_t) mmap_device_io(4, SMSTPCR3);

	if (mmcif->mmcif_up == (uintptr_t) MAP_FAILED) {
		slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "mmcif_clock_enable: mmap_device_io failed");
		return (MMC_FAILURE);
	}
	tmp = in32(mmcif->mmcif_up);
	switch (channel) {
		case 0:
			//Enable MMC0 operates
			tmp &= (~(1 << 15));
			out32(mmcif->mmcif_up, tmp);
			mmcckcr = MMC0CKCR;
			break;
		case 1:
			//Enable MMC1 operates
			tmp &= (~(1 << 5));
			out32(mmcif->mmcif_up, tmp);
			mmcckcr = MMC1CKCR;
			break;
		default:
			slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "mmcif_clock_enable: channel %d is not supported", channel);
			return (MMC_FAILURE);
	}			
	pmmcckcr = (uintptr_t) mmap_device_io(4, mmcckcr);
	if (pmmcckcr == (uintptr_t) MAP_FAILED) {
		slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "mmcif_clock_enable: mmap_device_io failed");
		return (MMC_FAILURE);
	}

	out32(pmmcckcr, 0x00000007);//in32(pmmcckcr) & ~(1 << 8));
	munmap_device_io(pmmcckcr, 4);
	
	return (MMC_SUCCESS);
}


int mmcif_reset(int channel) {
	uintptr_t mmc_reset, clr_reset;
	
	mmc_reset = mmap_device_io(4, SRCR3);
	clr_reset = mmap_device_io(4, SRSTCLR3);

	if ((mmc_reset == (uintptr_t) MAP_FAILED) || 
		(clr_reset == (uintptr_t) MAP_FAILED)) {
		slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "mmcif_reset: mmap_device_io failed");
		return (MMC_FAILURE);
	}
	switch (channel) {
		case 0:
			//Reset
			out32(mmc_reset, in32(mmc_reset) | (1 << 15));
			delay(100);
			out32(mmc_reset, in32(mmc_reset) & ~(1 << 15));
			out32(clr_reset, in32(clr_reset) | (1 << 15));
			delay(100);	
			break;
		case 1:
			//Reset
			out32(mmc_reset, in32(mmc_reset) | (1 << 5));
			delay(100);
			out32(mmc_reset, in32(mmc_reset) & ~(1 << 5));
			out32(clr_reset, in32(clr_reset) | (1 << 5));
			delay(100);			
			break;
		default:
			return (MMC_FAILURE);
	}
	munmap_device_io(mmc_reset, 4);
	munmap_device_io(clr_reset, 4);
	
	return (MMC_SUCCESS);
}

#endif



#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/devb/mmcsd/chipsets/sim_mmcif.c $ $Rev: 748860 $")
#endif
