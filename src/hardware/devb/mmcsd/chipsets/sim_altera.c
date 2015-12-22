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

#include 	<sim_mmc.h>
#include 	<sim_bs.h>
#include 	<mmcsd.h>

#define MMCSD_VENDOR_ALT_SOCFPGA
#ifdef MMCSD_VENDOR_ALT_SOCFPGA
//#define MMC_DEBUG

#include	<sim_altera.h>
#include	<sys/syspage.h>
#include	<hw/sysinfo.h>

int mmc_dma_in_prog;
int socfpga_data_completed(SIM_HBA *hba);

//
// convert virtual to physical address
//
static paddr_t vtophys(void *addr)
{
	off64_t offset;
	if (mem_offset64(addr, NOFD, 1, &offset, 0) == -1)
		return -1;
	return offset;
}

/*****************************************************************************************
 * Function:  socfpga_alloc_desc_ring
 * Description: Alloc descriptor ring for 
 *
 * SIM_HBA *hba,
 *
 * return: 	MMC_SUCCESS
 * 			MMC_FAILURE
 * */
static inline int
socfpga_alloc_desc_ring(SIM_HBA *hba)
{
	SIM_MMC_EXT		*ext;
	socfpga_ext_t	*socfpga;

	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;

	// alloc descr ring
	if ((socfpga->dma_buf_desc = mmap(NULL, sizeof(mmc_dma_desc_t) * socfpga->dma_num_descs,
			PROT_READ | PROT_WRITE | PROT_NOCACHE,
			/* MAP_PRIVATE |*/ MAP_ANON | MAP_PHYS, NOFD, 0)) == MAP_FAILED) {
		slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "%s(): mmap failed: %d", __func__, errno);
		return MMC_FAILURE;
	}

	if ((socfpga->dma_buf_desc_phys = vtophys((void *) socfpga->dma_buf_desc)) == -1) {
		slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "%s(): vtophys error: %d", __func__, errno);
		return MMC_FAILURE;
	}

	return MMC_SUCCESS;
}


/*****************************************************************************************
 * Function:  socfpga_init_desc_ring
 * Description: Read and analize interrupt status, reset pending interrupts
 *
 * SIM_HBA *hba,

 * return: 	MMC_SUCCESS
 * 			MMC_FAILURE
 * */

static inline int
socfpga_init_desc_ring(SIM_HBA *hba)
{
	int i;
	mmc_dma_desc_t *bd;

	SIM_MMC_EXT		*ext;
	socfpga_ext_t	*socfpga;

	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;

	// init descr ring
	for (i = 0; i < socfpga->dma_num_descs; i++) {
		bd = socfpga->dma_buf_desc + i;
		memset((void *)bd, 0, sizeof(*bd));
		bd->des0.own_bit = 0;
		bd->des0.second_address_chained = 1;
		bd->des1.buff1_size = 0;
		bd->des2 = 0;
		/* Chained mode usage */
		if (i == (socfpga->dma_num_descs - 1)) {
			/* The latest element has to point to the head. */
			bd->des3 = (uint32_t) socfpga->dma_buf_desc_phys;
		} else {
			/* Point to the next element in the ring */
			bd->des3 = (uint32_t) vtophys((void *) &socfpga->dma_buf_desc[i + 1]);
		}
	}

	return MMC_SUCCESS;
}

/*****************************************************************************************
 * Function:  socfpga_reset_desc_ring
 * Description: Reset description ring
 *
 * SIM_HBA *hba,

 * return: 	MMC_SUCCESS
 * 			MMC_FAILURE
 * */

static inline int
socfpga_clr_desc_ring(SIM_HBA *hba)
{
	int i;
	mmc_dma_desc_t *bd;

	SIM_MMC_EXT		*ext;
	socfpga_ext_t	*socfpga;

	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;

	// init descr ring
	for (i = 0; i < socfpga->dma_num_descs; i++) {
		bd = socfpga->dma_buf_desc + i;
		bd->des0.own_bit = 0;
		bd->des0.dis_int_on_completion =
		bd->des0.last_segment =
		bd->des0.first_segment = 0;
		bd->des1.buff1_size = 0;
		bd->des2 = 0;
	}

	return MMC_SUCCESS;
}
/*****************************************************************************************
 * Function:  socfpga_interrupt
 * Description: Read and analize interrupt status, reset pending interrupts
 *
 * SIM_HBA *hba,
 * int irq,
 * int resp_type  - value which describe kind of response expected
 * uint32_t *resp - buffer for responce data
 *
 * return: interrupt status bitmask
 * */

static int socfpga_interrupt(SIM_HBA *hba, int irq, int resp_type, uint32_t *resp)
{
	SIM_MMC_EXT		*ext;
	socfpga_ext_t		*socfpga;
	uint32_t		sts, idsts , err, iderr;
	int				intr = 0;
	uintptr_t		base;

	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;
	base  = socfpga->mmc_base;

	sts = in32(base + ALT_SDMMC_MINTSTS_OFST);				// read Masked Interrupt Status Register
	idsts = in32(base + ALT_SDMMC_IDSTS_OFST);			// read Masked DMA Interrupt Status Register


	err = (ALT_SDMMC_MINTSTS_RESP_SET_MSK |
		   ALT_SDMMC_MINTSTS_RESPCRCERR_SET_MSK |
		   ALT_SDMMC_MINTSTS_DATACRCERR_SET_MSK	|
		   ALT_SDMMC_MINTSTS_FIFOOVUNERR_SET_MSK |
		   ALT_SDMMC_MINTSTS_STRERR_SET_MSK |
		   ALT_SDMMC_MINTSTS_EBE_SET_MSK);
	iderr = ALT_SDMMC_IDSTS_FBE_SET_MSK |
			ALT_SDMMC_IDSTS_DU_SET_MSK |
			ALT_SDMMC_IDSTS_CES_SET_MSK |
			ALT_SDMMC_IDSTS_AIS_SET_MSK |
			ALT_SDMMC_IDSTS_EB_SET_MSK;

	if (sts & err || idsts & iderr)
	{	// Any errors ?
		out32(base + ALT_SDMMC_IDSTS_OFST, idsts);
		out32(base + ALT_SDMMC_RINTSTS_OFST, sts);
#ifdef MMCSD_DEBUG
		slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "Socfpga interrupt error = %x", sts);
#endif
		intr |= MMC_INTR_ERROR;
		if (sts & ALT_SDMMC_MINTSTS_RESPTO_SET_MSK) {
			out32(base + ALT_SDMMC_RINTSTS_OFST, ALT_SDMMC_MINTSTS_RESPTO_SET_MSK);
			intr = MMC_ERR_DATA_TO;
		}
		if (sts & ALT_SDMMC_MINTSTS_RESPTO_SET_MSK) {
			out32(base + ALT_SDMMC_RINTSTS_OFST, ALT_SDMMC_MINTSTS_RESPTO_SET_MSK);
			intr = MMC_ERR_DATA_END;
		}
		if (sts & ALT_SDMMC_MINTSTS_EBE_SET_MSK) {
			out32(base + ALT_SDMMC_RINTSTS_OFST, ALT_SDMMC_MINTSTS_EBE_SET_MSK);
			intr = MMC_ERR_DATA_CRC;
		}
	}

	if (sts & ALT_SDMMC_MINTSTS_HLWERR_SET_MSK)
	{
		// Hardware locked write error (HLE)
		// attempt to write to hardware registers while start_cmd is 1
		out32(base + ALT_SDMMC_RINTSTS_OFST, ALT_SDMMC_MINTSTS_HLWERR_SET_MSK);
		intr = MMC_INTR_NONE;

		#ifdef PRINT_DEBUG_LOG
			printf("     Socfpga interrupt error ALT_SDMMC_MINTSTS_HLWERR_SET_MSK\n");
		#endif
	}

	if (sts & ALT_SDMMC_MINTSTS_CD_SET_MSK)
	{
		// Card insertion detected
		out32(base + ALT_SDMMC_RINTSTS_OFST, ALT_SDMMC_MINTSTS_CD_SET_MSK);
		intr |= MMC_INTR_CARDINS;

		#ifdef PRINT_DEBUG_LOG
			printf("     Socfpga interrupt  ALT_SDMMC_MINTSTS_CD_SET_MSK\n");
		#endif
	}

	if (sts & ALT_SDMMC_MINTSTS_CMD_DONE_SET_MSK)
	{
		// Command Done
		out32(base + ALT_SDMMC_RINTSTS_OFST, ALT_SDMMC_MINTSTS_CMD_DONE_SET_MSK);
		intr |= MMC_INTR_COMMAND;

		if (resp)
		{
			if (resp_type & MMC_RSP_136)
			{
				resp[3] = in32(base + ALT_SDMMC_RESP3_OFST);
				resp[2] = in32(base + ALT_SDMMC_RESP2_OFST);
				resp[1] = in32(base + ALT_SDMMC_RESP1_OFST);
				resp[0] = in32(base + ALT_SDMMC_RESP0_OFST);
			}
			else
			if (resp_type & MMC_RSP_PRESENT)
			{
				resp[0] = in32(base + ALT_SDMMC_RESP0_OFST);
			}
		}

			// Busy check?
		if (resp_type & MMC_RSP_BUSY)
		{
			int	 i=500000000;
			while (--i > 0)
			{
				if (!ALT_SDMMC_STAT_DATA_BUSY_GET(in32( base + ALT_SDMMC_STAT_OFST ) ))
					break;
				// nanospin_ns(100);
			}

			if (i == 0)
			{
				intr |= MMC_ERR_CMD_TO | MMC_INTR_ERROR;	// Command Timeout Rrror
					slogf(_SLOGC_SIM_MMC, _SLOG_ERROR, "SOCFPGA busy check time out, pstate = %x", in32(base + ALT_SDMMC_STAT_OFST));
			}
		}

	}

	if (sts & ALT_SDMMC_MINTSTS_ACD_SET_MSK)
	{
		out32(base + ALT_SDMMC_RINTSTS_OFST, ALT_SDMMC_RINTSTS_ACD_SET_MSK);
	}

	if(mmc_dma_in_prog == 0)
	{
		out32(base + ALT_SDMMC_IDSTS_OFST, ALT_SDMMC_IDSTS_RI_SET_MSK | ALT_SDMMC_IDSTS_TI_SET_MSK);
	}

	if((ext->hccap & MMC_HCCAP_DMA) && mmc_dma_in_prog )
	{
		if (ALT_SDMMC_IDSTS_FSM_GET(idsts) == 0)
			mmc_dma_in_prog = 0;

		if (idsts & ALT_SDMMC_IDSTS_RI_SET_MSK)
		{
			out32(base + ALT_SDMMC_IDSTS_OFST, ALT_SDMMC_IDSTS_RI_SET_MSK);
			intr |= MMC_INTR_DATA;							// Data complete
			intr |= MMC_INTR_RBRDY;							// Receive FIFO Data Request
		}

		if (idsts & ALT_SDMMC_IDSTS_TI_SET_MSK)
		{
			out32(base + ALT_SDMMC_IDSTS_OFST, ALT_SDMMC_IDSTS_TI_SET_MSK);
			intr |= MMC_INTR_DATA;							// Data complete
			intr |= MMC_INTR_WBRDY;							// Transmit FIFO Data Request
		}
	}
	else
	{
		if (sts & ALT_SDMMC_MINTSTS_DT_SET_MSK)
		{
			out32(base + ALT_SDMMC_RINTSTS_OFST, ALT_SDMMC_MINTSTS_DT_SET_MSK);
			intr |= MMC_INTR_DATA;							// Data complete
		}

		if (sts & ALT_SDMMC_MINTSTS_RXFIFODR_SET_MSK)
		{
			out32(base + ALT_SDMMC_RINTSTS_OFST, ALT_SDMMC_MINTSTS_RXFIFODR_SET_MSK);
			intr |= MMC_INTR_RBRDY;							// Receive FIFO Data Request
		}

		if (sts & ALT_SDMMC_MINTSTS_DTTXFIFODR_SET_MSK)
		{
			out32(base + ALT_SDMMC_RINTSTS_OFST, ALT_SDMMC_MINTSTS_DTTXFIFODR_SET_MSK);
			intr |= MMC_INTR_WBRDY;							// Transmit FIFO Data Request
		}
	}

	return intr;
}


/*****************************************************************************************
 * Function: socfpga_setup_pio
 * Description: Setup controller for pio operations.
 *              Prepage socfpga->dcmd, socfpga->dmask and len values according to pending operation
 *
 * SIM_HBA *hba,
 * int len     - number of bytes to be transfered
 * int dir     - transfer direction:  	MMC_DIR_OUT
 * 										MMC_DIR_IN
 *
 * return: amount of data, which can be transfered. This amout may differ from the input "len" argument due hardware specific limitations
 *
 * */

static int socfpga_setup_pio(SIM_HBA *hba, int len, int dir)
{
	SIM_MMC_EXT		*ext;
	socfpga_ext_t		*socfpga;
	uintptr_t		base;
	int				nblk, timeout = 20000000;

	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;
	base  = socfpga->mmc_base;

	nblk = len / socfpga->blksz;		// number of block to transfer
	len  = nblk * socfpga->blksz;		// update len value: it is must be a multiple of Block Size

#ifdef MMCSD_DEBUG
	slogf (_SLOGC_SIM_MMC, _SLOG_INFO, "Socfpga MMCSD setup pio:");
#endif

	// reset FIFO
	out32(base + ALT_SDMMC_CTL_OFST, ALT_SDMMC_CTL_FIFO_RST_SET_MSK | ALT_SDMMC_CTL_DMA_RST_SET_MSK);

	/* hw clears the bit when it's done */
	while (in32(base + ALT_SDMMC_CTL_OFST) & (ALT_SDMMC_CTL_CTLLER_RST_SET_MSK | ALT_SDMMC_CTL_FIFO_RST_SET_MSK	| ALT_SDMMC_CTL_DMA_RST_SET_MSK))
	{
		if (timeout == 0)
		{
			perror("Reset host timeout error\n");
			return (MMC_FAILURE);
		}
		timeout--;
		// usleep(1);
	}

	socfpga->dcmd = ALT_SDMMC_CMD_DATA_EXPECTED_SET_MSK;     // Data transfer expected (read/write)

	out32(base + ALT_SDMMC_CARDTHRCTL_OFST, ALT_SDMMC_CARDTHRCTL_CARDRDTHREN_SET_MSK | ALT_SDMMC_CARDTHRCTL_CARDRDTHRESHOLD_SET(0x80));

	out32(base + ALT_SDMMC_BYTCNT_OFST, ALT_SDMMC_BYTCNT_BYTE_COUNT_SET(len));

	if(len > 0 && nblk > 1)
	{
		socfpga->dcmd |= ALT_SDMMC_CMD_SEND_AUTO_STOP_SET_MSK;		// Send stop command at end of data transfer
	}


	socfpga->dmask = ALT_SDMMC_INTMSK_RE_SET_MSK   |			// Response error
			         ALT_SDMMC_INTMSK_DTO_SET_MSK  |			// Data transfer over
			         ALT_SDMMC_INTMSK_RCRC_SET_MSK |			// Response CRC error
			         ALT_SDMMC_INTMSK_DCRC_SET_MSK |			// Data CRC error
			         ALT_SDMMC_INTMSK_RTO_SET_MSK  |			// Response timeout
			         ALT_SDMMC_INTMSK_HTO_SET_MSK  |			// Data starvation by host timeout
			         ALT_SDMMC_INTMSK_FRUN_SET_MSK |			// FIFO underrun/overrun error
			         ALT_SDMMC_INTMSK_EBE_SET_MSK;				// End-bit error

	if (dir == MMC_DIR_OUT)
	{
		socfpga->dcmd |= ALT_SDMMC_CMD_RD_WR_SET_MSK;
		socfpga->dmask |= ALT_SDMMC_INTMSK_TXDR_SET_MSK | 			// Transmit FIFO data request (TXDR)
				          ALT_SDMMC_INTMSK_HLE_SET_MSK;  		 	// Hardware locked write error (HLE)
	}
	else
	{
		socfpga->dmask |= ALT_SDMMC_INTMSK_RXDR_SET_MSK | 			// Receive FIFO data request (RXDR)
						  ALT_SDMMC_INTMSK_SBE_SET_MSK;				// Start-bit error (SBE)
	}

	return (len);
}


/*****************************************************************************************
 * Function: socfpga_pio_done
 * Description: Read/ Write data from/to FIFO
 *
 * SIM_HBA *hba,
 * char *buf,
 * int len   - len of data
 * int dir   - transfer direction
 *
 * return: amount of data transfered. It can be less then "len"
 * */

static int socfpga_pio_done(SIM_HBA *hba, char *buf, int len, int dir)
{
	SIM_MMC_EXT		*ext;
	socfpga_ext_t		*socfpga;
	uintptr_t		base;
	int				nbytes, cnt;
	uint32_t		*pbuf = (uint32_t *)buf;
	uint32_t	status;
	uint32_t items_in_fifo,dw_cnt,b_cnt;

	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;
	base  = socfpga->mmc_base;

	cnt = nbytes = len < MMCHS_FIFO_SIZE ? len : MMCHS_FIFO_SIZE;

	if (dir == MMC_DIR_IN)
	{
		items_in_fifo = ALT_SDMMC_STAT_FIFO_COUNT_GET(in32(base + ALT_SDMMC_STAT_OFST));
		if (items_in_fifo == 0)
		{
			// no data in FIFO yet
			#ifdef PRINT_DEBUG_LOG
				printf("   No data in FIFO to read  : STATUS %08X\n",in32(base + ALT_SDMMC_STAT_OFST));
			#endif
			return 0;
		}

		dw_cnt = nbytes >> 2;					// number of "full" FIFO items to read
		b_cnt  = nbytes & 3;					// number of "additional" bytes, which will be read in last dword

		if (dw_cnt > items_in_fifo)
		{
			dw_cnt = items_in_fifo;    			// limit amount of data to real value in fifo
			b_cnt = 0;							// and no "additional" bytes
		}
		else
		if ((dw_cnt == items_in_fifo) && (b_cnt > 0))
		{
			b_cnt = 0;							// FIFO has exact number of "full" dwords but no "additional" bytes
		}


		cnt = (dw_cnt * 4) + b_cnt;

		// reading "full" dwords
		while (dw_cnt > 0)
		{
			*pbuf++ = in32(base + ALT_SDMMC_DATA_OFST);
			dw_cnt--;
		}

		if (b_cnt > 0)
		{
			// reading "additional" bytes
			buf = (char *)pbuf;
			status = in32(base + ALT_SDMMC_DATA_OFST);
			while (b_cnt > 0)
			{
				*buf++ = status;
				status = status >> 8;
				b_cnt--;
			}
		}

		#ifdef PRINT_DEBUG_LOG
			printf("   data:  cnt %08X\n         ",cnt);
			uint8_t * pbkp = (uint8_t *)buf;
			int n;
			for (n=0;n<cnt;n++)
			{
				printf("%02X",pbkp[n]);
				if ((n & 0x1F) == 0x1F)
					printf("\n         ");
			}
			printf("\n");

			status = in32(base + ALT_SDMMC_STAT_OFST);
			printf("   MMC_DIR_IN  : STATUS(after) %08X  FIFO_COUNT=%08X\n",status,ALT_SDMMC_STAT_FIFO_COUNT_GET(status));
		#endif
	}
	else
	{
		#ifdef PRINT_DEBUG_LOG
			printf(" MMC_DIR_OUT: nbytes %08X data:\n         ",nbytes);
			for (n=0;n<nbytes;n++)
			{
				printf("%02X",pbkp[n]);
				if ((n & 0x1F) == 0x1F)
					printf("\n         ");
			}
			printf("\n");
		#endif

		// get amount of free section in FIFO
		items_in_fifo = (MMCHS_FIFO_SIZE >> 2) - ALT_SDMMC_STAT_FIFO_COUNT_GET(in32(base + ALT_SDMMC_STAT_OFST));
		if (items_in_fifo == 0)
			return 0; 					// no space in FIFO, try next time

		dw_cnt = (nbytes+3) >> 2;					// number of "full" FIFO items to write

		if (dw_cnt > items_in_fifo)
			dw_cnt = items_in_fifo;

		cnt = dw_cnt << 2;
		if (cnt > nbytes)			// it's possible if nbytes is not aligned to 4
			cnt = nbytes;

		// writing data
		for (; dw_cnt > 0; dw_cnt--)
			out32(base + ALT_SDMMC_DATA_OFST, *pbuf++);
	}

	return cnt;

}

/*****************************************************************************************
 * Function: socfpga_setup_dma
 * Description: Initialize DMA transfer operation
 *
 * SIM_HBA *hba,
 * paddr_t paddr  - pointer to data
 * int len        - size
 * int dir        - direction:  MMC_DIR_IN
 *
 *
 * return: size of
 * */

static int socfpga_setup_dma(SIM_HBA *hba, paddr_t paddr, int len, int dir)
{

	SIM_MMC_EXT		*ext;
	socfpga_ext_t	*socfpga;
	uintptr_t		base;
	mmc_dma_desc_t 	*bd;
	uint32_t		timeout = 20000000;

	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;
	base  = socfpga->mmc_base;

	while (ALT_SDMMC_STAT_FIFO_COUNT_GET(in32(base + ALT_SDMMC_STAT_OFST)))
	{
		if (timeout == 0)
		{
			perror("Reset host timeout error\n");
			return (-1);
		}
		timeout--;
		// usleep(1);			// UNDO point2
	}

	timeout = 20000000;
	socfpga_clr_desc_ring(hba);
	mmc_dma_in_prog = 0;

	//
	// Initialize DMA channel
	//
	out32(base + ALT_SDMMC_BMOD_OFST, in32(base + ALT_SDMMC_BMOD_OFST) | ALT_SDMMC_BMOD_SWR_SET_MSK);

	while (in32(base + ALT_SDMMC_CTL_OFST) & ALT_SDMMC_BMOD_SWR_SET_MSK)
	{
		if (timeout == 0)
		{
			perror("Reset host timeout error\n");
			return (-1);
		}
		timeout--;
		// usleep(1);			// UNDO point3
	}

	len = socfpga_setup_pio(hba, len, dir);

	if (len > 0) {
		bd = socfpga->dma_buf_desc;
		bd->des0.first_segment = 1;
		socfpga->dma_cur_pos = 0;
		//ext->dma_continue(hba, paddr, len, dir);
		socfpga->dsize = len;
		mmc_dma_in_prog = 1;
		out32(base + ALT_SDMMC_DBADDR_OFST, socfpga->dma_buf_desc_phys);

		//Set watermark
		if (dir == MMC_DIR_IN)
		{
			out32(base + ALT_SDMMC_FIFOTH_OFST,	ALT_SDMMC_FIFOTH_RX_WMARK_SET(MMCHS_DMA_SEGMENT_SIZE));
		}
		else
		{
			out32(base + ALT_SDMMC_FIFOTH_OFST,	ALT_SDMMC_FIFOTH_TX_WMARK_SET(MMCHS_DMA_SEGMENT_SIZE)
					| ALT_SDMMC_FIFOTH_DW_DMA_MULT_TRANSACTION_SIZE_SET(ALT_SDMMC_FIFOTH_DW_DMA_MULT_TRANSACTION_SIZE_E_RXMSIZEK1));
		}
		out32(base + ALT_SDMMC_BMOD_OFST, (in32(base + ALT_SDMMC_BMOD_OFST) & ALT_SDMMC_BMOD_PBL_CLR_MSK)
				| ALT_SDMMC_BMOD_PBL_SET(ALT_SDMMC_BMOD_PBL_E_TRANS16));
		out32(base + ALT_SDMMC_IDINTEN_OFST, ALT_SDMMC_IDINTEN_NI_SET_MSK | ALT_SDMMC_IDINTEN_TI_SET_MSK | ALT_SDMMC_IDINTEN_RI_SET_MSK
				| ALT_SDMMC_IDINTEN_FBE_SET_MSK | ALT_SDMMC_IDINTEN_DU_SET_MSK  | ALT_SDMMC_IDINTEN_CES_SET_MSK  | ALT_SDMMC_IDINTEN_AI_SET_MSK);
		out32(base + ALT_SDMMC_CTL_OFST, in32(base + ALT_SDMMC_CTL_OFST) | ALT_SDMMC_CTL_USE_INTERNAL_DMAC_SET_MSK);
		out32(base + ALT_SDMMC_BMOD_OFST, in32(base + ALT_SDMMC_BMOD_OFST) | ALT_SDMMC_BMOD_DE_SET_MSK);
	}

	return (len);

}


/*****************************************************************************************
 * Function: socfpga_dma_continue
 * Description: Initialize DMA transfer descriptors
 *
 * SIM_HBA *hba,
 * paddr_t paddr  - pointer to data
 * int len        - size
 * int dir        - direction:  MMC_DIR_IN
 *
 *
 * return: size of
 * */

int socfpga_dma_continue(SIM_HBA *hba, paddr_t paddr, int len, int dir)
{

	SIM_MMC_EXT		*ext;
	socfpga_ext_t	*socfpga;
	mmc_dma_desc_t 	*bd;

	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;
	bd = socfpga->dma_buf_desc + socfpga->dma_cur_pos;

	mmc_dma_desc_t 	*prev_bd = bd;


	int init_len = len;
	paddr_t tmp_paddr = paddr;

	if (len > 0) {
		while (len > 0 && bd->des0.own_bit == 0) {
			int set_len = len > MMCHS_DMA_SEGMENT_SIZE ? MMCHS_DMA_SEGMENT_SIZE : len;
			bd->des0.own_bit = 1;
			bd->des0.dis_int_on_completion = 1;//socfpga->dma_cur_pos % 4;
			bd->des0.last_segment = 0;
			bd->des1.buff1_size = set_len;
			bd->des2 = tmp_paddr;
#ifdef PRINT_DEBUG_LOG
			printf("socfpga_setup_dma_add: des_adrdr %08X des2_paddr %08X des1_len %08X\n", (uint32_t) vtophys((void *)bd), (uint32_t) tmp_paddr, set_len);
#endif
			tmp_paddr += set_len;
			len -= set_len;
			if (++socfpga->dma_cur_pos >= MMCHS_DMA_DESC_NUM)
				socfpga->dma_cur_pos = 0;
			prev_bd = bd;
			bd = socfpga->dma_buf_desc + socfpga->dma_cur_pos;
		}

		if(len <= 0)
			prev_bd->des0.last_segment = 1;
		prev_bd->des0.dis_int_on_completion = 0;
	}

	return (init_len - len);

}
/*****************************************************************************************
 * Function: socfpga_dma_done
 * Description:
 *
 * SIM_HBA *hba
 * int dir
 *
 * return:	MMC_SUCCESS
 * 			MMC_FAILURE
 * */


static int socfpga_dma_done(SIM_HBA *hba, int dir)
{

	SIM_MMC_EXT		*ext;
	socfpga_ext_t		*socfpga;
	int				ret = MMC_SUCCESS;
	uintptr_t		base;

	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;
	base  = socfpga->mmc_base;

#ifdef PRINT_DEBUG_LOG
	uint32_t status;
	status = in32(base + ALT_SDMMC_STAT_OFST);
	printf("   MMC_DIR_IN  : STATUS(after) %08X  FIFO_COUNT=%08X\n",status,ALT_SDMMC_STAT_FIFO_COUNT_GET(status));
#endif

	out32(base + ALT_SDMMC_IDINTEN_OFST, 0);
	out32(base + ALT_SDMMC_BMOD_OFST, in32(base + ALT_SDMMC_BMOD_OFST) & ALT_SDMMC_BMOD_DE_CLR_MSK & ALT_SDMMC_BMOD_FB_CLR_MSK);
	out32(base + ALT_SDMMC_CTL_OFST, in32(base + ALT_SDMMC_CTL_OFST) & ALT_SDMMC_CTL_USE_INTERNAL_DMAC_CLR_MSK);
	socfpga_data_completed(hba);

	mmc_dma_in_prog = 0;


#ifdef PRINT_DEBUG_LOG
	uint8_t		*dst;
	int n;
	dst = (uint8_t *)mmap(NULL, 4096,
			PROT_READ | PROT_WRITE,
			MAP_PHYS|MAP_SHARED, NOFD, socfpga->dma_buf_desc[0].des2);

	printf("socfpga_dma_done_data: des_adrdr %08X des2_paddr %08X\n", (uint32_t) dst, (uint32_t) socfpga->dma_buf_desc[0].des2);
	printf("          ");
	for (n=0; n < MMCHS_FIFO_SIZE; n++)
	{
		printf("%02X",dst[n]);
		if ((n & 0x1F) == 0x1F)
			printf("\n         ");
	}
	printf("\n");

	status = in32(base + ALT_SDMMC_STAT_OFST);
	printf("   MMC_DIR_IN  : STATUS(after) %08X  FIFO_COUNT=%08X\n",status,ALT_SDMMC_STAT_FIFO_COUNT_GET(status));


	munmap(dst, 4096);
#endif
	return ret;
}

/*****************************************************************************************
 * Function: socfpga_command
 * Description: Driver is about to sending a command to the card.
 *              Preparing command and argument registers. Starting a command.
 *
 * SIM_HBA *hba
 * mmc_cmd_t *cmd  - pointer to structure which holds opcode, argument, flags and answer type
 *
 * return: MMC_SUCCESS
 *  	   MMC_FAILURE - something goes wrong
 * */

static int socfpga_command(SIM_HBA *hba, mmc_cmd_t *cmd)
{
	SIM_MMC_EXT		*ext;
	socfpga_ext_t	*socfpga;
	uintptr_t		base;
	uint32_t		command;
//	uint32_t		imask;

	if(hba == NULL || cmd == NULL)
		return (MMC_FAILURE);

	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;
	base  = socfpga->mmc_base;

#ifdef PRINT_DEBUG_LOG
	uint32_t		status;
	extern	char *mmc_cmds[];

	printf("\n					socfpga_command: cmd-> opcode 0x%02X(%d %s) argument %08X eflags %04X  resp %02X \n",cmd->opcode,cmd->opcode,mmc_cmds[cmd->opcode],cmd->argument, cmd->eflags, cmd->resp);

	printf("					   eflags:");
	if (cmd->eflags & MMC_CMD_INIT )
		printf("MMC_CMD_INIT ");
	if (cmd->eflags & MMC_CMD_PPL )
		printf("MMC_CMD_PPL ");
	if (cmd->eflags & MMC_CMD_DATA )
		printf("MMC_CMD_DATA ");
	if (cmd->eflags & MMC_CMD_DATA_MULTI )
		printf("MMC_CMD_DATA_MULTI ");
	if (cmd->eflags & MMC_CMD_DATA_IN )
		printf("MMC_CMD_DATA_IN ");
	if (cmd->eflags & MMC_CMD_DATA_DMA )
		printf("MMC_CMD_DATA_DMA ");
	if (cmd->eflags & MMC_CMD_INTR )
		printf("MMC_CMD_INTR ");

	printf("   resp:");
	if (cmd->resp & MMC_RSP_PRESENT )
		printf("MMC_RSP_PRESENT ");
	if (cmd->resp & MMC_RSP_136 )
		printf("MMC_RSP_136 ");
	if (cmd->resp & MMC_RSP_CRC )
		printf("MMC_RSP_CRC ");
	if (cmd->resp & MMC_RSP_BUSY )
		printf("MMC_RSP_BUSY ");
	if (cmd->resp & MMC_RSP_OPCODE )
		printf("MMC_RSP_OPCODE ");
	printf("\n");


	status = in32(base + ALT_SDMMC_STAT_OFST);
	printf("		  STATUS %08X  FIFO_COUNT=%08X  DATA_BUSY %d  DATA3_STATUS %d  RESP_INDEX %d  DATA_STATE_MC_BUSY %d\n",
			                status,
			                       ALT_SDMMC_STAT_FIFO_COUNT_GET(status),
			                                        ALT_SDMMC_STAT_DATA_BUSY_GET(status),
					                                              ALT_SDMMC_STAT_DATA_3_STAT_GET(status),
					                                                               ALT_SDMMC_STAT_RESPONSE_INDEX_GET(status),
					                                                                              ALT_SDMMC_STAT_DATA_STATE_MC_BUSY_GET(status));



#endif


	// specify bits which are common for all operations
	command = ALT_SDMMC_CMD_CARD_NUMBER_SET(0) |					// specify card number
			  ALT_SDMMC_CMD_CMD_INDEX_SET(cmd->opcode)	|			// command index - opcode
			  ALT_SDMMC_CMD_USE_HOLD_REG_SET_MSK | 					// CMD and DATA sent to card through the HOLD Register
			  ALT_SDMMC_CMD_WAIT_PRVDATA_COMPLETE_SET_MSK; 			// wait a completion for previous command

	switch (cmd->opcode)
	{
	case MMC_GO_IDLE_STATE:											// MMC_RSP_NONE      MMC_CMD_INIT | MMC_CMD_INTR
		command |= ALT_SDMMC_CMD_SEND_INITIALIZATION_SET_MSK;

		break;
	case MMC_STOP_TRANSMISSION:
		command |= ALT_SDMMC_CMD_STOP_ABT_CMD_SET_MSK;				// 1 \96 Stop or abort command intended to stop current data transfer in progress.
		command &= ALT_SDMMC_CMD_WAIT_PRVDATA_COMPLETE_CLR_MSK;		// don't wait a completion of previous command (its related to data transfer in progress)
		break;

	case MMC_PROGRAM_CSD:
	case MMC_SEND_WRITE_PROT:
	case MMC_LOCK_UNLOCK:
	case MMC_SEND_STATUS:
	case 22:

		command &= ALT_SDMMC_CMD_WAIT_PRVDATA_COMPLETE_CLR_MSK;		// don't wait a completion of previous command (its related to data transfer in progress)
		break;
	}

	if (cmd->eflags & MMC_CMD_DATA)
	{
		// data transfer will be initiated by current commands
//		imask 	= socfpga->dmask;			// use interrupts specified in socfpga_setup_pio routine
		command |= socfpga->dcmd;			// update "command" with next bits:   ALT_SDMMC_CMD_RD_WR_SET_MSK    ALT_SDMMC_CMD_SEND_AUTO_STOP_SET_MSK
	}
	else
	{
//		imask = ALT_SDMMC_INTMSK_CD_SET_MSK |     // allow  Card detect (CD) interrupt
//				ALT_SDMMC_INTMSK_RE_SET_MSK |     //        Response error (RE) interrupt
//				ALT_SDMMC_INTMSK_CMD_SET_MSK;	  //	    Command done (CD) interrupt
	}


	if (cmd->resp != MMC_RSP_NONE)
	{
		// we expect some special response
		command |= ALT_SDMMC_CMD_RESPONSE_EXPECT_SET_MSK;		// specify that response is expected
		if (cmd->resp == MMC_RSP_R2)						    // (MMC_RSP_PRESENT | MMC_RSP_136 | MMC_RSP_CRC)
			command |= ALT_SDMMC_CMD_RESPONSE_LEN_SET_MSK;      // 1 \96 Long response expected from card

		if (cmd->resp & MMC_RSP_CRC )
			command |= ALT_SDMMC_CMD_CHECK_RESPONSE_CRC_SET_MSK;
	}

	if (command & ALT_SDMMC_CMD_WAIT_PRVDATA_COMPLETE_SET_MSK)
	{
		while (in32(base + ALT_SDMMC_STAT_OFST) & ALT_SDMMC_STAT_DATA_BUSY_SET_MSK);
	}

	// Interrupts enable
	out32(base + ALT_SDMMC_CTL_OFST, in32(base + ALT_SDMMC_CTL_OFST) | ALT_SDMMC_CTL_INT_EN_SET(ALT_SDMMC_CTL_INT_EN_E_END));

	// Reset all possible interrupts
	out32(base + ALT_SDMMC_RINTSTS_OFST, 0xFFFFFFFF);

	// ???? Unmask all possible interrupts
	out32(base + ALT_SDMMC_INTMSK_OFST, 0xFFFFFFFF);

	// Setup the Argument Register and send CMD
	out32(base + ALT_SDMMC_CMDARG_OFST, ALT_SDMMC_CMDARG_CMD_ARG_SET(cmd->argument));


#ifdef PRINT_DEBUG_LOG

printf("					   CMD %08X    CTRL %08X\n 					",command, in32(base + ALT_SDMMC_CTL_OFST));


if (command & ALT_SDMMC_CMD_SEND_INITIALIZATION_SET_MSK)
	printf("send_initialization(1) ");
else
	printf("send_initialization(0) ");
if (command & ALT_SDMMC_CMD_STOP_ABT_CMD_SET_MSK)
	printf("stop_abort_cmd(1) ");
else
	printf("stop_abort_cmd(0) ");
if (command & ALT_SDMMC_CMD_WAIT_PRVDATA_COMPLETE_SET_MSK)
	printf("wait_prvdata_compl(1) ");
else
	printf("wait_prvdata_compl(0) ");
if (command & ALT_SDMMC_CMD_SEND_AUTO_STOP_SET_MSK)
	printf("send_auto_stop(1)\n");
else
	printf("send_auto_stop(0)\n");
if (command & ALT_SDMMC_CMD_TFR_MOD_SET_MSK)
	printf("					transfer_mode(1-stream) ");
else
	printf("					transfer_mode(0-block) ");

if (command & ALT_SDMMC_CMD_RD_WR_SET_MSK)
	printf("read/write(1-write) ");
else
	printf("read/write(0-read) ");
if (command & ALT_SDMMC_CMD_DATA_EXPECTED_SET_MSK)
	printf("data_expected(1-yes) ");
else
	printf("data_expected(0-no) ");
if (command & ALT_SDMMC_CMD_CHECK_RESPONSE_CRC_SET_MSK)
	printf("check_response_crc(1) ");
else
	printf("check_response_crc(0) ");

if (command & ALT_SDMMC_CMD_RESPONSE_LEN_SET_MSK)
	printf("response_ length(1-long) ");
else
	printf("response_ length(0-short) ");
if (command & ALT_SDMMC_CMD_RESPONSE_EXPECT_SET_MSK)
	printf("response_expect(1-yes)\n");
else
	printf("response_expect(0-no)\n");

#endif


	// prepare command register values
	out32(base + ALT_SDMMC_CMD_OFST, command);

	// and start process.
	out32(base + ALT_SDMMC_CMD_OFST, command | ALT_SDMMC_CMD_START_CMD_SET_MSK);

	return (MMC_SUCCESS);
}


/*****************************************************************************************
 * Function:
 * Description: Signal from CAM that all data related to current transfer is completed and we have to disable unnecessary interrupts
 *
 *
 * return:
 * */

int socfpga_data_completed(SIM_HBA *hba)
{
	if(hba == NULL)
		return (MMC_FAILURE);

	SIM_MMC_EXT *ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga_ext_t *socfpga = (socfpga_ext_t *)ext->handle;
	uintptr_t base  = socfpga->mmc_base;

	out32(base + ALT_SDMMC_INTMSK_OFST, in32(base + ALT_SDMMC_INTMSK_OFST) & ALT_SDMMC_INTMSK_TXDR_CLR_MSK & ALT_SDMMC_INTMSK_RXDR_CLR_MSK );
	return (MMC_SUCCESS);
}



/*****************************************************************************************
 * Function: socfpga_cfg_bus
 * Description: Setup card bus width
 *
 * SIM_HBA *hba,
 * int width  - card width value: 8 bit, 4 bit or 1 bit.
 * int mmc    - ????
 *
 * return: MMC_SUCCESS
 * */

static int socfpga_cfg_bus(SIM_HBA *hba, int width, int mmc)
{
	SIM_MMC_EXT		*ext;
	socfpga_ext_t		*socfpga;
	uintptr_t		base;
	uint32_t		ctype;

	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;
	base  = socfpga->mmc_base;

	ctype = in32(base + ALT_SDMMC_CTYPE_OFST);
	if(width==8)
	{
		out32(base + ALT_SDMMC_CTYPE_OFST, ctype | ALT_SDMMC_CTYPE_CARD_WIDTH1_SET_MSK );
	}
	else
	{
		ctype &= ALT_SDMMC_CTYPE_CARD_WIDTH1_CLR_MSK;
		if (width == 4)
			ctype |= ALT_SDMMC_CTYPE_CARD_WIDTH2_SET_MSK;
		else
			ctype &= ALT_SDMMC_CTYPE_CARD_WIDTH2_CLR_MSK;
		out32(base + ALT_SDMMC_CTYPE_OFST, ctype);
	}

	return (MMC_SUCCESS);
}





static void socfpga_send_clock_update_cmd(SIM_HBA *hba)
{
	SIM_MMC_EXT		*ext;
	socfpga_ext_t	*socfpga;
	uintptr_t		base;

	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;
	base  = socfpga->mmc_base;

	out32(base + ALT_SDMMC_CMD_OFST, in32(base + ALT_SDMMC_CMD_OFST) |
				                         ALT_SDMMC_CMD_UPDATE_CLK_REGS_ONLY_SET_MSK  |   // 1 \96 Do not send commands, just update clock register value into card clock domain
				                         ALT_SDMMC_CMD_WAIT_PRVDATA_COMPLETE_SET_MSK |   // 1 \96 Wait for previous data transfer completion before sending command
				                         ALT_SDMMC_CMD_START_CMD_SET_MSK);               // and start "command"

	// Wait for the clock to be stable
	// wait for command completion
	while (ALT_SDMMC_CMD_START_CMD_GET(in32(base + ALT_SDMMC_CMD_OFST)));        //				|| ALT_SDMMC_CMD_WAIT_PRVDATA_COMPLETE_GET(in32(base + ALT_SDMMC_CMD_OFST)))
	return;
}



/*****************************************************************************************
 * Function: socfpga_clock
 * Description: Setup SD card clock value
 *
 * SIM_HBA *hba,
 * int *clock,
 * int high_speed
 *
 * return: MMC_SUCCESS
 *         MMC_FAILURE - card is busy now
 * */

static int socfpga_clock(SIM_HBA *hba, int *clock, int high_speed)
{
	SIM_MMC_EXT		*ext;
	socfpga_ext_t		*socfpga;
	uintptr_t		base;
	int				clkd;

	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;
	base  = socfpga->mmc_base;

	// calculate clk_divider0 value

	clkd = socfpga->mmc_clock / (*clock);
	if (socfpga->mmc_clock != (*clock) * clkd)
		clkd++;
	*clock = socfpga->mmc_clock / clkd;			// update clock value

	if(ALT_SDMMC_STAT_DATA_BUSY_GET(in32(base + ALT_SDMMC_STAT_OFST)) != 0)
		return (MMC_FAILURE);

//	if (socfpga->dtoc & SYSCTL_CEN)
	{
		//Reset clock
		// disable clock
		out32(base + ALT_SDMMC_CLKENA_OFST, in32(base + ALT_SDMMC_CLKENA_OFST) & ALT_SDMMC_CLKENA_CCLK_EN_CLR_MSK);
		// switch clock sourse to Clock divider 0 for card 0
		out32(base + ALT_SDMMC_CLKSRC_OFST, in32(base + ALT_SDMMC_CLKSRC_OFST) & ALT_SDMMC_CLKSRC_CLK_SRC_CLR_MSK);

		socfpga_send_clock_update_cmd(hba);

		// setup clk_divider0 of the Clock Divider Register
		out32(base + ALT_SDMMC_CLKDIV_OFST, (in32(base + ALT_SDMMC_CLKDIV_OFST)	& ALT_SDMMC_CLKDIV_CLK_DIVR0_CLR_MSK) | ALT_SDMMC_CLKDIV_CLK_DIVR0_SET(clkd >> 1));

		socfpga_send_clock_update_cmd(hba);

		out32(base + ALT_SDMMC_CLKENA_OFST, in32(base + ALT_SDMMC_CLKENA_OFST)	| ALT_SDMMC_CLKENA_CCLK_EN_SET_MSK);

		socfpga_send_clock_update_cmd(hba);
	}

	return (MMC_SUCCESS);
}

/*****************************************************************************************
 * Function: socfpga_block_size
 * Description: Setup block size
 *
 *
 * return: 	MMC_SUCCESS
 * 			MMC_FAILURE - block size is more then 1024
 * */

static int socfpga_block_size(SIM_HBA *hba, int blksz)
{
	SIM_MMC_EXT		*ext;
	socfpga_ext_t		*socfpga;

	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;

	if (blksz > 1024)
		return (MMC_FAILURE);

	socfpga->blksz = blksz;
	out32(socfpga->mmc_base + ALT_SDMMC_BLKSIZ_OFST, ALT_SDMMC_BLKSIZ_BLOCK_SIZE_SET(socfpga->blksz));
	return (MMC_SUCCESS);
}


/*****************************************************************************************
 * Function: socfpga_powerup
 * Description: Reset host controller and card
 * The clock should be enabled and set to minimum (<400KHz)
 *
 * SIM_HBA *hba
 *
 * return: MMC_SUCCESS
 * */


static int socfpga_powerup(SIM_HBA *hba)
{
	SIM_MMC_EXT		*ext;
	socfpga_ext_t		*socfpga;
	uintptr_t		base;

	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;
	base  = socfpga->mmc_base;

	// Enable power
	out32(base + ALT_SDMMC_PWREN_OFST, 0x0);
	out32(base + ALT_SDMMC_FIFOTH_OFST, ALT_SDMMC_FIFOTH_DW_DMA_MULT_TRANSACTION_SIZE_SET(ALT_SDMMC_FIFOTH_DW_DMA_MULT_TRANSACTION_SIZE_E_RXMSIZEK1) |
										ALT_SDMMC_FIFOTH_RX_WMARK_SET((MMCHS_FIFO_SIZE >> 3)-1 ) |
										ALT_SDMMC_FIFOTH_TX_WMARK_SET((MMCHS_FIFO_SIZE >> 3)));

	// Enable external open-drain pullup           ????? interrupt are disabled
	// out32(base + ALT_SDMMC_CTL_OFST, ALT_SDMMC_CTL_ENABLE_OD_PULLUP_SET_MSK);

	// Configure timeouts
	out32(base + ALT_SDMMC_TMOUT_OFST, ALT_SDMMC_TMOUT_RESPONSE_TMO_SET(0xFF) | ALT_SDMMC_TMOUT_DATA_TMO_SET(0xFFFFFF) );


	out32(base + ALT_SDMMC_DEBNCE_OFST, 0x0FFFFF);

	//out32(base + ALT_SDMMC_CTL_OFST, 1 << 24);


	// Clear any interrupts
	out32(base + ALT_SDMMC_RINTSTS_OFST, 0xFFFFFFFF);

	// Disable All interrupts
	out32(base + ALT_SDMMC_INTMSK_OFST, 0x0);

	out32(base + ALT_SDMMC_CTL_OFST, ALT_SDMMC_CTL_INT_EN_SET(ALT_SDMMC_CTL_INT_EN_E_END));


	// Reset cart type
	// setup card type to 1bit

	out32(base + ALT_SDMMC_CTYPE_OFST, ALT_SDMMC_CTYPE_CARD_WIDTH1_SET(ALT_SDMMC_CTYPE_CARD_WIDTH1_E_NON8BIT) |
			                           ALT_SDMMC_CTYPE_CARD_WIDTH2_SET(ALT_SDMMC_CTYPE_CARD_WIDTH2_E_MOD1BIT));

	out32(base + ALT_SDMMC_CLKENA_OFST, in32(base + ALT_SDMMC_CLKENA_OFST) | ALT_SDMMC_CLKENA_CCLK_EN_SET_MSK);

	out32(base + ALT_SDMMC_CMD_OFST, ALT_SDMMC_CMD_UPDATE_CLK_REGS_ONLY_SET_MSK |
			                         ALT_SDMMC_CMD_WAIT_PRVDATA_COMPLETE_SET_MSK |
			                         ALT_SDMMC_CMD_START_CMD_SET_MSK);

	// Enable power
	out32(base + ALT_SDMMC_PWREN_OFST, 0xFFFF);

	// Enumerated Card Stack

	// Clock Setup
	if(ext->hccap & MMC_HCCAP_DMA)
	{
//		out32(base + ALT_SDMMC_IDINTEN_OFST, ALT_SDMMC_IDINTEN_NI_SET_MSK | ALT_SDMMC_IDINTEN_TI_SET_MSK | ALT_SDMMC_IDINTEN_RI_SET_MSK
//				| ALT_SDMMC_IDINTEN_FBE_SET_MSK | ALT_SDMMC_IDINTEN_DU_SET_MSK  | ALT_SDMMC_IDINTEN_CES_SET_MSK  | ALT_SDMMC_IDINTEN_AI_SET_MSK);
	}

	return (MMC_SUCCESS);
}


/*****************************************************************************************
 * Function: socfpga_detect
 * Description: Return card status
 *
 * SIM_HBA *hba
 *
 * return: MMC_SUCCESS
 * */

static int socfpga_detect(SIM_HBA *hba)
{
// printf("socfpga_detect: \n");

	return (MMC_SUCCESS);//detect;
}


/*****************************************************************************************
 * Function: socfpga_powerdown
 * Description: Disconnect power from SD card and resets a controller, FIFO and DMA
 *
 * SIM_HBA *hba
 *
 * return: MMC_SUCCESS
 * */

static int socfpga_powerdown(SIM_HBA *hba)
{
#ifdef PRINT_DEBUG_LOG
	printf("socfpga_powerdown: \n");
#endif

	SIM_MMC_EXT		*ext;
	socfpga_ext_t		*socfpga;
	uintptr_t		base;
	unsigned int timeout = 100;

	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;
	base  = socfpga->mmc_base;

	// Disable power
	out32(base + ALT_SDMMC_PWREN_OFST, ALT_SDMMC_PWREN_POWER_EN_SET(ALT_SDMMC_PWREN_POWER_EN_E_OFF));
	out32(base + ALT_SDMMC_CTL_OFST, in32(base + ALT_SDMMC_CTL_OFST) & ALT_SDMMC_CTL_USE_INTERNAL_DMAC_CLR_MSK);

	// Disable All interrupts
	out32(base + ALT_SDMMC_INTMSK_OFST, 0);

	// Clear any interrupts
	out32(base + ALT_SDMMC_RINTSTS_OFST, 0xFFFFFFFF);

	if(ext->hccap & MMC_HCCAP_DMA)
	{
		out32(base + ALT_SDMMC_BMOD_OFST, in32(base + ALT_SDMMC_BMOD_OFST) & ALT_SDMMC_BMOD_DE_CLR_MSK);
	}

	// Reset hardware
	out32(base + ALT_SDMMC_CTL_OFST, ALT_SDMMC_CTL_CTLLER_RST_SET_MSK            // reset controller
			   | ALT_SDMMC_CTL_FIFO_RST_SET_MSK									 // and FIFO pointers
			   | ALT_SDMMC_CTL_DMA_RST_SET_MSK);								 // and DMA

	/* hw clears the bit when it's done */
	while (in32(base + ALT_SDMMC_CTL_OFST) & (ALT_SDMMC_CTL_CTLLER_RST_SET_MSK | ALT_SDMMC_CTL_FIFO_RST_SET_MSK	| ALT_SDMMC_CTL_DMA_RST_SET_MSK))
	{
		if (timeout == 0)
		{
			perror("Reset host timeout error\n");
			return (MMC_FAILURE);
		}
		timeout--;
		delay(1);
	}

	return (MMC_SUCCESS);
}

/*****************************************************************************************
 * Function: socfpga_shutdown
 * Description: Perform shutdown of the socfpga sdmmc controller
 *
 * SIM_HBA *hba
 *
 * return: 0
 * */

static int socfpga_shutdown(SIM_HBA *hba)
{
	SIM_MMC_EXT		*ext;
	socfpga_ext_t		*socfpga;

	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;

	socfpga_powerdown(hba);

	munmap_device_io(socfpga->mmc_base, 0xFFF);

	free(socfpga);
	return (0);
}

/*****************************************************************************************
 * Function: socfpga_get_syspage_clk
 * Description: Get a "mmc" clock value from the system page "hwi" area
 *
 *
 * return: Clock value
 *         0 - error
 * */

static unsigned socfpga_get_syspage_clk(void)
{
	unsigned	item, offset, clock = 0;
	char		*name;
	hwi_tag		*tag;

	item = hwi_find_item(HWI_NULL_OFF, HWI_ITEM_DEVCLASS_DISK, "mmc", NULL);
	if (item == HWI_NULL_OFF)
		return 	0;

	offset = item;

	while ((offset = hwi_next_tag(offset, 1)) != HWI_NULL_OFF) {
		tag = hwi_off2tag(offset);
		name = __hwi_find_string(((hwi_tag *)tag)->prefix.name);

		if (strcmp(name, HWI_TAG_NAME_inputclk) == 0)
			clock = ((struct hwi_inputclk *)tag)->clk / ((struct hwi_inputclk *)tag)->div;
	}

	return clock;
}


/*****************************************************************************************
 * Function: socfpga_attach
 * Description: Creation of socfpga context instance. Mapping a sdmmc controller registers in driver's user memory
 *
 * SIM_HBA *hba
 *
 * return:  MMC_SUCCESS
 * 			MMC_FAILURE
 * */


int	socfpga_attach(SIM_HBA *hba)
{
	CONFIG_INFO		*cfg;
	SIM_MMC_EXT		*ext;
	socfpga_ext_t		*socfpga;

	cfg = (CONFIG_INFO *)&hba->cfg;
	ext = (SIM_MMC_EXT *)hba->ext;

	if ((socfpga = calloc(1, sizeof(socfpga_ext_t))) == NULL)
		return (MMC_FAILURE);

	socfpga->mmc_pbase = cfg->IOPort_Base[0];			// get physical address of the sdmmc controller

	if ((socfpga->mmc_base = mmap_device_io(SOCFPGA_MMCHS_SIZE, socfpga->mmc_pbase)) == (uintptr_t)MAP_FAILED)
	{
		slogf (_SLOGC_SIM_MMC, _SLOG_ERROR, "Socfpga MMCSD: mmap_device_io failed");
		goto fail0;
	}

	// Set Capability bits
	ext->hccap |= MMC_HCCAP_HS |			/* Host support high speed		*/
			MMC_HCCAP_DMA |
			MMC_HCCAP_18V |    		/* Host support 1.8V		*/
			MMC_HCCAP_30V |  			/* Host support 3.0V		*/
			MMC_HCCAP_33V | 			/* Host support 3.3V		*/
			MMC_HCCAP_BW1 |			/* Host support 1 bit bus (mandatory)		*/
			MMC_HCCAP_BW4 | 			/* Host support 4 bit bus (mandatory)		*/
		//	MMC_HCCAP_NOCD_BUSY | 		/* Host has card busy detect bug		*/
		//	MMC_HCCAP_CD_INTR | 		/* Host support card detect interrupt		*/
			MMC_HCCAP_ACMD12; 			/* Host support auto-stop command(ACMD12)		*/



	//socfpga->dtoc |= SYSCTL_DTO_MAX; // data timeout
	// clock enable
	//socfpga->dtoc |= SYSCTL_CEN;

	if ((socfpga->mmc_clock = socfpga_get_syspage_clk()) == 0)
		socfpga->mmc_clock = 100000000;

	ext->hba       = hba;
	ext->handle    = socfpga;
	ext->clock     = socfpga->mmc_clock;

	ext->detect    = socfpga_detect;
	ext->powerup   = socfpga_powerup;
	ext->powerdown = socfpga_powerdown;
	ext->command   = socfpga_command;
	ext->cfg_bus   = socfpga_cfg_bus;
	ext->set_clock = socfpga_clock;
	ext->set_blksz = socfpga_block_size;
	ext->interrupt = socfpga_interrupt;
	ext->setup_dma = NULL;
	ext->dma_done  = socfpga_dma_done;
	ext->setup_pio = socfpga_setup_pio;
	ext->pio_done  = socfpga_pio_done;
//	ext->dma_continue = socfpga_dma_continue;
	ext->shutdown  = socfpga_shutdown;

	ext->pwr_delay = 10; //10ms
	ext->pwr_retry = 3;
	ext->busy_retry = 5000;

	ext->mmc_priority = MMC_PRIORITY;
	ext->mmc_irq_priority = MMC_IRQ_PRIORITY;
	ext->sim_priority = SIM_PRIORITY;
	ext->sim_timer_priority = SIM_PRIORITY;

	strncpy(cfg->Description, "Altera SOCFPGA MMCHS", sizeof(cfg->Description));

	if(ext->hccap & MMC_HCCAP_DMA)
	{
		socfpga->dma_num_descs = MMCHS_DMA_DESC_NUM;
		socfpga->dma_cur_pos = 0;

		ext->setup_dma = socfpga_setup_dma;
		ext->dma_done  = socfpga_dma_done;
		if (socfpga_alloc_desc_ring(hba) == MMC_FAILURE)
			goto fail0;
		socfpga_init_desc_ring(hba);
	}

	return (MMC_SUCCESS);

fail0:
	free(socfpga);
	munmap_device_io(socfpga->mmc_base, SOCFPGA_MMCHS_SIZE);

	return (MMC_FAILURE);
}
#ifdef MMCSD_DEBUG

void dump_register(SIM_HBA *hba)
{
	SIM_MMC_EXT		*ext;
	socfpga_ext_t		*socfpga;
	uintptr_t		base;

	int i = ALT_SDMMC_STAT_OFST;//0;
	int j = ALT_SDMMC_STAT_OFST;//ALT_SDMMC_BACK_END_POWER_R_OFST;
	ext   = (SIM_MMC_EXT *)hba->ext;
	socfpga = (socfpga_ext_t *)ext->handle;
	base  = socfpga->mmc_base;

	for(; i <= j; i += 4)
	{
		//put_c_debug(base + i);
		//put_c_debug(in32(base + i));
	}
	return;
}
#endif
#endif

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL$ $Rev$")
#endif


