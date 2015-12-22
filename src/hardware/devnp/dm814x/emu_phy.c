/*
 * $QNXLicenseC:
 * Copyright 2009, QNX Software Systems. 
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

#include "ti814x.h"

/*****************************************************************************/
/* This routine emulates the read from a PHY. The routine reads the          */
/* information from the phy_regs structure, setup in the emu_phy_init        */
/* routine.                                                                  */
/*****************************************************************************/

uint16_t	emu_phy_read (ti814x_dev_t * ti814x, uint8_t phy_add, uint8_t reg_add)

{
	if (reg_add > MII_NUM_REGS)
		return (0xffff);
	if (ti814x->cfg.verbose > 3)
		slogf (_SLOGC_NETWORK, _SLOG_DEBUG1, "%s - reg %d - data 0x%x", __FUNCTION__, reg_add, ti814x->phy_regs [reg_add]);
	return (ti814x->phy_regs [reg_add]);
}

/*****************************************************************************/
/* Setup the phy_regs structure with the information necessary to emulate    */
/* a gigabit PHY. This is required for an interface that is connected to a   */
/* switch.                                                                   */
/*****************************************************************************/

int		emu_phy_init (ti814x_dev_t *ti814x)

{
nic_config_t	*cfg = &ti814x->cfg;
uint16_t		bmsr, bmcr;
int				i;

	bmsr = BMSR_AN_ABILITY | BMSR_LINK_STATUS | BMSR_EXT_STATUS;
	bmcr = 0;
	ti814x->phy_regs = malloc (MII_NUM_REGS, M_DEVBUF, M_NOWAIT);
	if (ti814x->phy_regs == NULL) {
		slogf (_SLOGC_NETWORK, _SLOG_ERROR, "%s(): malloc phy_regs failed", __FUNCTION__);
		return (-1);
		}
	memset ((char *) ti814x->phy_regs, 0, sizeof (ti814x->phy_regs));
	cfg->phy_addr = 31 - cfg->device_index;
	if (ti814x->force_link == -1) {
		bmcr = BMCR_FULL_DUPLEX | BMCR_SPEED_1000;
		bmsr |= (BMSR_EXTENDED_CAP | BMSR_100bXFD | BMSR_100bX | BMSR_10FD | BMSR_10);
		ti814x->phy_regs [MDI_MSSR] = (MSSR_CONFIG_RESULT | MSSR_LOCAL_RX_STATUS | MSSR_REMOTE_RX_STATUS);
		ti814x->phy_regs [MDI_MSSR] |= MSSR_LP_1000bTFD;
		ti814x->phy_regs [MDI_ANAR] = (ANAR_100bTFD | ANAR_100bT | ANAR_10bTFD | ANAR_10bT | 1);
		}
	else {
		if (cfg->duplex)
			bmcr |= BMCR_FULL_DUPLEX;
		ti814x->phy_regs [MDI_ANAR] = (ti814x->force_link & 0x1f) << 5;
		switch (cfg->media_rate) {
			case	10*1000:
				bmsr |= (cfg->duplex) ? BMSR_10FD : BMSR_10;
				ti814x->phy_regs [MDI_ANAR] = (ANAR_10bTFD | ANAR_10bT | 1);
				break;
			case	100*1000:
				bmsr |= (cfg->duplex) ? BMSR_100bXFD : BMSR_100bX;
				bmcr |= BMCR_SPEED_100;
				ti814x->phy_regs [MDI_ANAR] = (ANAR_100bTFD | ANAR_100bT | 1);
				break;
			case	1000*1000:
				bmsr |= BMSR_EXTENDED_CAP;
				bmcr |= BMCR_SPEED_1000;
				ti814x->phy_regs [MDI_MSSR] = (MSSR_CONFIG_RESULT | MSSR_LOCAL_RX_STATUS | MSSR_REMOTE_RX_STATUS);
				ti814x->phy_regs [MDI_MSSR] |= (cfg->duplex) ? MSSR_LP_1000bTFD : MSSR_LP_1000bT;
				break;
			}
		}
	bmsr |= BMSR_EXTENDED_CAP;
	bmsr |= (BMSR_LINK_STATUS | BMSR_AN_COMPLETE);
	ti814x->phy_regs [MDI_BMSR] = bmsr;
	ti814x->phy_regs [MDI_BMCR] = bmcr;
	ti814x->phy_regs [MDI_PHYID_1] = 0x0f;
	ti814x->phy_regs [MDI_PHYID_2] = 0xc6c2;
	ti814x->phy_regs [MDI_EMSR] = (EMSR_1000bTFD | EMSR_1000bT);
//	ti814x->phy_regs [MDI_ANAR] = (ANAR_100bTFD | ANAR_100bT | ANAR_10bTFD | ANAR_10bT | 1);
	ti814x->phy_regs [MDI_MSCR] = (MSCR_PORT_TYPE | MSCR_ADV_1000bTFD | MSCR_ADV_1000bT);
	ti814x->phy_regs [MDI_ANLPAR] = (ANLPAR_NEXT_PAGE | ANLPAR_ACKNOWLEDGE);
	ti814x->phy_regs [MDI_ANLPAR] |= ti814x->phy_regs [MDI_ANAR];
	ti814x->phy_regs [MDI_ANAE] = (ANAE_LP_NP_ABLE | ANAE_NP_ABLE | ANAE_LP_AN_ABLE);
	if (cfg->verbose > 3) {
		for (i = 0; i < MII_NUM_REGS; i++)
			slogf (_SLOGC_NETWORK, _SLOG_DEBUG1, "Phy REG %d - 0x%04x", i, ti814x->phy_regs [i]);
		}
	return (0);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int		emu_phy_dinit (ti814x_dev_t *ti814x)

{
	free (ti814x->phy_regs, M_DEVBUF);
	return (0);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/lib/io-pkt/sys/dev_qnx/dm814x/emu_phy.c $ $Rev: 680336 $")
#endif
