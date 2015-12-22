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

#include    "ti814x.h"


/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int		ti814x_ale_set_port_num(uint32_t *entry, int port)

{
	/* Port mask is bit68:66 in the table entry */
	/* Table word 2 bit 4:2 represents this bit */
	entry[2] &= 0xffffffe3;
	entry[2] |= ((port & 0x00000007) << 2);
	return(EOK);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int		ti814x_ale_set_blocked(uint32_t *entry, int blocked)

{
	/* Secure bit is bit65 in the table entry */
	/* Table word 2 bit 1 represents this bit */
	entry[2] &= 0xfffffffd;
	entry[2] |= ((blocked & 0x00000001) << 1);
	return(EOK);
}


/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int		ti814x_ale_set_secure(uint32_t *entry, int secure)

{
	/* Secure bit is bit64 in the table entry */
	/* Table word 2 bit 0 represents this bit */
	entry[2] &= 0xfffffffe;
	entry[2] |= (secure & 0x00000001);
	return(EOK);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

void ti814x_ale_set_vlan (uint32_t *entry, uint16_t vlan)

{
	/* VLAN is bits 59:48 in the table entry */
	/* Table word 1 bits 27:16 represents these bits */
	entry[1] &= 0xf000ffff;
	entry[1] |= (vlan & 0xfff) << 16;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

uint16_t ti814x_ale_get_vlan (uint32_t *entry)
{
	/* VLAN is bits 59:48 in the table entry */
	/* Table word 1 bits 27:16 represents these bits */
	return ((entry[1] >> 16) & 0xfff);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

void ti814x_ale_set_vlan_ports (uint32_t *entry, uint8_t ports)

{
	/* VLAN_MEMBER_LIST is bits 2:0 in the table entry */
	/* Table word 0 bits 2:0 represents these bits */
	entry[0] &= 0xfffffff8;
	entry[0] |= ports & 7;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

void ti814x_ale_set_vlan_untagged_egress (uint32_t *entry, uint8_t val)

{
	/* FORCE_UNTAGGED_EGRESS is bits 26:24 in the table entry */
	/* Table word 0 bits 26:24 represents these bits */
	entry[0] &= 0xf8ffffff;
	entry[0] |= (val & 7) << 24;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

void ti814x_ale_set_vlan_reg_mcast_flood (uint32_t *entry, uint8_t val)

{
	/* REG_MCAST_FLOOD_MASK is bits 18:16 in the table entry */
	/* Table word 0 bits 18:16 represents these bits */
	entry[0] &= 0xfff8ffff;
	entry[0] |= (val & 7) << 16;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

uint8_t ti814x_ale_get_vlan_reg_mcast_flood (uint32_t *entry)

{
	/* REG_MCAST_FLOOD_MASK is bits 18:16 in the table entry */
	/* Table word 0 bits 18:16 represents these bits */
	return ((entry[0] >> 16) & 7);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

void ti814x_ale_set_vlan_unreg_mcast_flood (uint32_t *entry, uint8_t val)

{
	/* UNREG_MCAST_FLOOD_MASK is bits 10:8 in the table entry */
	/* Table word 0 bits 10:8 represents these bits */
	entry[0] &= 0xfffff8ff;
	entry[0] |= (val & 7) << 8;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int		ti814x_ale_get_ucast_type(uint32_t *entry)

{
	/* unicast type field is located in bits 63:62  */
	/* ALE Table Word 1 bits 31:30 represent these bits */
	return((entry[1]>>30)&0x3);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int		ti814x_ale_set_ucast_type(uint32_t *ale_entry, uint8_t type)

{
	/* clear unitcast type bits */
	ale_entry[1] &= 0x3fffffff;
	ale_entry[1] |= (type & 0x3) << 30;
	return(EOK);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int		ti814x_ale_set_mcast_state(uint32_t *ale_entry, uint8_t state)

{
	/* mcast state field is located in bits 63:62 */
	/* ALE Table Word 1 bits 31:30 represent these bits */
	/* clear entry type bits */
	ale_entry[1] &= 0x3fffffff;
	ale_entry[1] |= (state & 0x3)<<30;
	return(EOK);
}


/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int		ti814x_ale_set_addr(uint32_t *entry, uint8_t *addr)

{
	entry[0] = addr[5] | (addr[4]<<8) | (addr[3]<<16) | (addr[2]<<24);
	/* clear ad-dress bits from T1 */
	entry[1] &= 0xffff0000;
	entry[1] |= (0x0000ffff & (addr[1] | (addr[0]<<8)));
	return(EOK);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int		ti814x_ale_get_addr(uint32_t *entry, uint8_t *addr)

{
	addr[5] = (entry[0]	  )  & 0x000000ff;
	addr[4] = (entry[0] >> 8 )  & 0x000000ff;
	addr[3] = (entry[0] >> 16)  & 0x000000ff;
	addr[2] = (entry[0] >> 24)  & 0x000000ff;
	addr[1] = (entry[1]	  )  & 0x000000ff;
	addr[0] = (entry[1] >> 8 )  & 0x000000ff;
	return(EOK);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int		ti814x_ale_set_entry_type(uint32_t *entry, uint8_t entry_type)

{
	/* Entry type field is located in bits 61:60 */
	/* ALE Table Word 1 bits 29:28 represent bits 61:60 of the table entry */
	/* clear entry type bits */
	entry[1] &= 0xcfffffff;
	entry[1] |= (entry_type & 0x3)<<28;
	return(EOK);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int		ti814x_ale_get_entry_type(uint32_t *entry)

{
	/* Entry type field is located in bits 61:60 */
	/* ALE Table Word 1 bits 29:28 represent bits 61:60 of the table entry */
	/* 63.62.61.60-59.58.57.56 / 55.54.53.52-51.50.49.48 / 47.46.45.44-43.42.41.40 / 39.38.37.36-35.34.33.32 */
	/* 31.30.29.28-27.26.25.24 / 23.22.21.20-19.18.17.16 / 15.14.13.12-11.10.09.08 / 07.06.05.04-03.02.01.00 */
	return((entry[1]>>28)&0x3);
	
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int ti814x_ale_read (uintptr_t cpsw_regs, uint32_t idx, uint32_t *entry)

{
	if (idx > ALE_ENTRIES) {
		slogf (_SLOGC_NETWORK, _SLOG_DEBUG1, "%s:%d - invalid ALE table index",
			   __FUNCTION__, __LINE__);
		return EINVAL;
	}

	/* Write index to ALE table control to load table entry into table-word registers */
	out32(cpsw_regs + ALE_TBLCTL, idx & ENTRY_MASK);

	/* Store table entry */
	entry[0] = in32(cpsw_regs + ALE_TBLW0);
	entry[1] = in32(cpsw_regs + ALE_TBLW1);
	entry[2] = in32(cpsw_regs + ALE_TBLW2);
	return(EOK);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

uint32_t ti814x_ale_write (uintptr_t cpsw_regs, uint32_t idx, uint32_t *entry)

{
	/* Populate the table words */
	out32(cpsw_regs + ALE_TBLW0, entry[0]);
	out32(cpsw_regs + ALE_TBLW1, entry[1]);
	out32(cpsw_regs + ALE_TBLW2, entry[2]);

	/* Write to ALE table control to push data to the ALE table */
	out32(cpsw_regs + ALE_TBLCTL, WRITE_RDZ_WRITE | (idx & ENTRY_MASK));

#if 0
	/* For debugging */
	slogf(_SLOGC_NETWORK, _SLOG_DEBUG1, "idx=%d %08x-%08x-%08x",
	      idx, entry[2], entry[1], entry[0]);
#endif

	return(EOK);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/
int ti814x_ale_match_vlan (uintptr_t cpsw_regs, uint16_t vlan)
{
    uint32_t	entry[ALE_ENTRY_WORDS];
    int		type, idx;
    uint16_t	entry_vlan;

    for (idx = 0; idx < ALE_ENTRIES; idx++) {
	ti814x_ale_read(cpsw_regs, idx, entry);
	type = ti814x_ale_get_entry_type(entry);
	if (type != ALE_TYPE_VLAN){
	    continue;
	}
	entry_vlan = ti814x_ale_get_vlan(entry);
	if (entry_vlan == vlan) {
	    return idx;
	}
    }
    return -ENOENT;
}

int ti814x_ale_match_vlan_addr (uintptr_t cpsw_regs, uint8_t* addr,
				uint16_t vlan)
{
    uint32_t	entry[ALE_ENTRY_WORDS];
    int		type, idx;
    uint8_t	entry_addr[ETHER_ADDR_LEN];
    uint16_t	entry_vlan;

    for (idx = 0; idx < ALE_ENTRIES; idx++) {
	ti814x_ale_read(cpsw_regs, idx, entry);
	type = ti814x_ale_get_entry_type(entry);
	if (type != ALE_TYPE_VLAN_ADDR){
	    continue;
	}
	ti814x_ale_get_addr(entry, entry_addr);
	entry_vlan = ti814x_ale_get_vlan(entry);
	if ((entry_vlan == vlan) &&
	    (memcmp(entry_addr, addr, ETHER_ADDR_LEN) == 0)) {
	    return idx;
	}
    }
    return -ENOENT;
}

int ti814x_ale_match_free (uintptr_t cpsw_regs)
{
	uint32_t ale_entry[ALE_ENTRY_WORDS];
	int type, idx;

	for (idx = ALE_DYN_START; idx < ALE_ENTRIES; idx++) {
			ti814x_ale_read(cpsw_regs, idx, ale_entry);
			type = ti814x_ale_get_entry_type(ale_entry);
			if (type == ALE_TYPE_FREE){
					return idx;
			}
	}
	return -ENOENT;
}

int ti814x_ale_find_ageable (uintptr_t cpsw_regs)
{
	uint32_t ale_entry[ALE_ENTRY_WORDS];
	int type, idx;

	for (idx = ALE_DYN_START; idx < ALE_ENTRIES; idx++) {
			ti814x_ale_read(cpsw_regs, idx, ale_entry);
			type = ti814x_ale_get_entry_type(ale_entry);
			if ((type == ALE_TYPE_ADDR) || (type == ALE_TYPE_VLAN_ADDR)) {
				type = ti814x_ale_get_ucast_type(ale_entry);
				if ((type == ALE_UNICAST_AGE_NOT_TOUCHED) ||
					(type == ALE_UNICAST_AGE_TOUCHED)){
					return idx;
				}
			}
	}
	return -ENOENT;
}

int ti814x_ale_print_table (uintptr_t cpsw_regs)
{
	uint32_t idx, type, ucast_type, flag = 0;
	uint32_t ale_entry[ALE_ENTRY_WORDS];
	
	for (idx = 0; idx < ALE_ENTRIES; idx++) {
		ti814x_ale_read(cpsw_regs, idx, ale_entry);
		type = ti814x_ale_get_entry_type(ale_entry);
		ucast_type = ti814x_ale_get_ucast_type(ale_entry);
		if (type == ALE_TYPE_FREE) {
			if (flag == 1) {
				slogf (_SLOGC_NETWORK, _SLOG_DEBUG1,
				       "idx=%d Learned ucast ends", idx - 1);
				flag = 0;
			}
			continue;
		}
		if ((type == ALE_TYPE_VLAN_ADDR) && 
		    (ucast_type == ALE_UNICAST_AGE_TOUCHED)) {
			if (flag == 0) {
				slogf (_SLOGC_NETWORK, _SLOG_DEBUG1,
				       "idx=%d Learned ucast begins", idx);
				flag = 1;
			}
			continue;
		}
		if (flag == 1) {
			slogf (_SLOGC_NETWORK, _SLOG_DEBUG1,
			       "idx=%d Learned ucast ends", idx - 1);
			flag = 0;
		}
		slogf (_SLOGC_NETWORK, _SLOG_DEBUG1, "idx=%d %08x-%08x-%08x",
		       idx, ale_entry[2], ale_entry[1], ale_entry[0]);
	}
	return(0);
}

// index shoud be -1 unless you absolutely know where the entry should go.
int ti814x_ale_add_vlan_ucast (uintptr_t cpsw_regs, uint8_t *addr, int index,
			       int port, int flags, uint16_t vlan)

{

	uint32_t ale_entry[ALE_ENTRY_WORDS] = {0, 0, 0};
	int idx;

	ti814x_ale_set_entry_type(ale_entry, ALE_TYPE_VLAN_ADDR);
	ti814x_ale_set_addr(ale_entry, addr);
	ti814x_ale_set_vlan(ale_entry, vlan);
	ti814x_ale_set_ucast_type(ale_entry, ALE_UNICAST);
	ti814x_ale_set_secure(ale_entry, (flags & ALE_SECURE_FLAG) ? 1 : 0);
	ti814x_ale_set_blocked(ale_entry, (flags & ALE_BLOCKED_FLAG) ? 1 : 0);
	ti814x_ale_set_port_num(ale_entry, port);

	/* If an index was suppied with the call to this function assume the user knows
	 * best and write the entry information to that index in the table, otherwise, verify
	 * if one that matches exists already, faiing that find a spot to put it */
	if (index < 0) {
	  idx = ti814x_ale_match_vlan_addr(cpsw_regs, addr, vlan);
		if (idx < 0)
				idx = ti814x_ale_match_free(cpsw_regs);
		if (idx < 0)
				idx = ti814x_ale_find_ageable(cpsw_regs);
		if (idx < 0)
			return -ENOMEM;
		} else {
			idx = index;
		}

	ti814x_ale_write(cpsw_regs, idx, ale_entry);
	return EOK;

}

int ti814x_ale_add_vlan_mcast (uintptr_t cpsw_regs, uint8_t *addr,
			       int port_mask, uint16_t vlan)
{
    uint32_t ale_entry[ALE_ENTRY_WORDS] = {0, 0, 0};
    int idx;

    ti814x_ale_set_entry_type(ale_entry, ALE_TYPE_VLAN_ADDR);
    ti814x_ale_set_addr(ale_entry, addr);
    ti814x_ale_set_vlan(ale_entry, vlan);
    ti814x_ale_set_mcast_state(ale_entry, ALE_STATE_FWD);
    ti814x_ale_set_port_num(ale_entry, port_mask);

    idx = ti814x_ale_match_vlan_addr(cpsw_regs, addr, vlan);
    if (idx < 0) {
	idx = ti814x_ale_match_free(cpsw_regs);
    }
    if (idx < 0) {
	idx = ti814x_ale_find_ageable(cpsw_regs);
    }
    if (idx < 0) {
      return -ENOMEM;
    }

    ti814x_ale_write(cpsw_regs, idx, ale_entry);
    return EOK;
}

void ti814x_ale_check_vlan_mcast (uintptr_t cpsw_regs, uint16_t vlan,
				  struct ethercom *ec)
{
    int			idx, type;
    uint32_t		entry[ALE_ENTRY_WORDS];
    uint8_t		addr[ETHER_ADDR_LEN];
    uint16_t		entry_vlan;
    struct ether_multi	*enm;

    for (idx = ALE_DYN_START; idx < ALE_ENTRIES; idx++) {
	ti814x_ale_read(cpsw_regs, idx, entry);
	type = ti814x_ale_get_entry_type(entry);
	entry_vlan = ti814x_ale_get_vlan(entry);
	if ((type != ALE_TYPE_VLAN_ADDR) || (entry_vlan != vlan)) {
	    continue;
	}
	ti814x_ale_get_addr(entry, addr);
	/* Check for mcast bit */
	if (!(addr[0] & 1)) {
	  continue;
	}
	ETHER_LOOKUP_MULTI(addr, addr, ec, enm);
	if (enm == NULL) {
	    ti814x_ale_set_entry_type(entry, ALE_TYPE_FREE);
	    ti814x_ale_write(cpsw_regs, idx, entry);
	}
    }
}

void ti814x_ale_del_all_vlan_mcast (uintptr_t cpsw_regs, uint16_t vlan,
				    struct ethercom *ec)
{
    int			idx, type;
    uint32_t		entry[ALE_ENTRY_WORDS];
    uint8_t		addr[ETHER_ADDR_LEN];
    uint16_t		entry_vlan;

    for (idx = ALE_DYN_START; idx < ALE_ENTRIES; idx++) {
	ti814x_ale_read(cpsw_regs, idx, entry);
	type = ti814x_ale_get_entry_type(entry);
	entry_vlan = ti814x_ale_get_vlan(entry);
	if ((type != ALE_TYPE_VLAN_ADDR) || (entry_vlan != vlan)) {
	    continue;
	}
	ti814x_ale_get_addr(entry, addr);
	/* Check for mcast bit */
	if (addr[0] & 1) {
	    ti814x_ale_set_entry_type(entry, ALE_TYPE_FREE);
	    ti814x_ale_write(cpsw_regs, idx, entry);
	}
    }
}

void ti814x_ale_flood_unreg_mcast (uintptr_t cpsw_regs, uint16_t vlan,
				   int flood)
{
    uint32_t	entry[ALE_ENTRY_WORDS];
    uint8_t	mask;
    int		idx;

    idx = ti814x_ale_match_vlan(cpsw_regs, vlan);
    if (idx < 0) {
	slogf(_SLOGC_NETWORK, _SLOG_WARNING,
	      "%s: failed to find vlan %d, flood %d",
	      __FUNCTION__, vlan, flood);
	return;
    }
    ti814x_ale_read(cpsw_regs, idx, entry);

    /* Copy the REG to the UNREG if need to flood */

    if (flood) {
        mask = ti814x_ale_get_vlan_reg_mcast_flood(entry);
	ti814x_ale_set_vlan_unreg_mcast_flood(entry, mask);
    } else {
#ifndef SWITCHMODE
	ti814x_ale_set_vlan_unreg_mcast_flood(entry, 0);
#else
	ti814x_ale_set_vlan_unreg_mcast_flood(entry, PORT1|PORT2);
#endif
    }
    ti814x_ale_write(cpsw_regs, idx, entry);
}

int ti814x_ale_add_vlan (uintptr_t cpsw_regs, uint16_t vlan, uint8_t ports,
			 int index)
{
    uint32_t	ale_entry[ALE_ENTRY_WORDS] = {0, 0, 0};
    int		idx;

    ti814x_ale_set_entry_type(ale_entry, ALE_TYPE_VLAN);
    ti814x_ale_set_vlan(ale_entry, vlan);
    ti814x_ale_set_vlan_ports(ale_entry, ports);
    ti814x_ale_set_vlan_reg_mcast_flood(ale_entry, ports);
    ti814x_ale_set_vlan_untagged_egress(ale_entry, ports);

    if (index < 0) {
	idx = ti814x_ale_match_vlan(cpsw_regs, vlan);
	if (idx < 0) {
	    idx = ti814x_ale_match_free(cpsw_regs);
	}
	if (idx < 0) {
	  idx = ti814x_ale_find_ageable(cpsw_regs);
	}
	if (idx < 0) {
	    return ENOMEM;
	}
    } else {
	idx = index;
    }
    ti814x_ale_write(cpsw_regs, idx, ale_entry);
    return EOK;
}

void ti814x_ale_init(uintptr_t cpsw_regs)

{
	/* Enable ALE and Clear ALE Table */
	outle32(cpsw_regs + ALE_CONTROL, ENABLE_ALE|CLEAR_TABLE);

#ifndef SWITCHMODE
	/* Setup Dual MAC mode (TRM section 9.2.1.5.2) */
	outle32(cpsw_regs + ALE_CONTROL, ENABLE_ALE|ALE_VLAN_AWARE);
	ti814x_ale_add_vlan(cpsw_regs, PORT1_VLAN, PORT0|PORT1,
			    PORT1_VLAN_ENTRY);
	ti814x_ale_add_vlan(cpsw_regs, PORT2_VLAN, PORT0|PORT2,
			    PORT2_VLAN_ENTRY);

	outle32(cpsw_regs + P1_PORT_VLAN, PORT1_VLAN);
	outle32(cpsw_regs + P2_PORT_VLAN, PORT2_VLAN);
#else
	/* Setup VLAN0 to all ports */
	outle32(cpsw_regs + ALE_CONTROL, ENABLE_ALE|ALE_VLAN_AWARE);
	ti814x_ale_add_vlan(cpsw_regs, 0, PORT0|PORT1|PORT2,
			    PORT1_VLAN_ENTRY);
	ti814x_ale_flood_unreg_mcast(cpsw_regs, 0, 0);
	outle32(cpsw_regs + P1_PORT_VLAN, 0);
	outle32(cpsw_regs + P2_PORT_VLAN, 0);
#endif

	outle32(cpsw_regs + ALE_PORTCTL0, PORT_STATE_FORWARD);
	outle32(cpsw_regs + ALE_PORTCTL1, PORT_STATE_FORWARD);
	outle32(cpsw_regs + ALE_PORTCTL2, PORT_STATE_FORWARD);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/lib/io-pkt/sys/dev_qnx/dm814x/ale.c $ $Rev: 697871 $")
#endif
