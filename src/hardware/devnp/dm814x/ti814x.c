/*
 * $QNXLicenseC:
 * Copyright 2011, QNX Software Systems.
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

#include <ti814x.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <device_qnx.h>
#include <sys/mman.h>
#include <quiesce.h>
#include <nw_thread.h>
#include <sys/netmgr.h>
#include <net/ifdrvcom.h>
#include <sys/sockio.h>

int ti814x_entry(void *dll_hdl, struct _iopkt_self *iopkt, char *options);

struct _iopkt_drvr_entry IOPKT_DRVR_ENTRY_SYM(ti814x) = IOPKT_DRVR_ENTRY_SYM_INIT(ti814x_entry);

#ifdef VARIANT_a
#include <nw_dl.h>
/* This is what gets specified in the stack's dl.c */
struct nw_dll_syms device_syms[] = {
		{"iopkt_drvr_entry", &IOPKT_DRVR_ENTRY_SYM(ti814x)},
		{NULL, NULL}
};
#endif

attach_args_t	attach_args;
int single = 0;

int  ti814x_attach(struct device *, struct device *, void *);
int  ti814x_detach(struct device *, int how);
CFATTACH_DECL(ti814x,
	sizeof(ti814x_dev_t),
	NULL,
	ti814x_attach,
	ti814x_detach,
	NULL);


/* Driver options */
static char *ti814x_opts[] = {
	"p0mode",
#define DM814OPT_P0MODE		0
	"p1mode",
#define DM814OPT_P1MODE		1
	"p0speed",
#define DM814OPT_P0SPEED	2
	"p1speed",
#define DM814OPT_P1SPEED	3
	"p0duplex",
#define DM814OPT_P0DUPLEX	4
	"p1duplex",
#define DM814OPT_P1DUPLEX	5
	"p0fix_mii_clk",
#define DM814OPT_P0MIICLK	6
	"p1fix_mii_clk",
#define DM814OPT_P1MIICLK	7
	"p0emu_phy",
#define DM814OPT_P0EMUPHY	8
	"p1emu_phy",
#define DM814OPT_P1EMUPHY	9
	"p0phy_idx",
#define DM814OPT_P0PHYIDX	10
	"p1phy_idx",
#define DM814OPT_P1PHYIDX	11
	"p0mac",
#define DM814OPT_P0MAC		12
	"p1mac",
#define DM814OPT_P1MAC		13
	"p0mru",
#define DM814OPT_P0MRU		14
	"p1mru",
#define DM814OPT_P1MRU		15
	"verbose",
#define DM814OPT_VERBOSE	16
	"p0joinvlan",
#define DM814OPT_P0JOINVLAN	17
	"p1joinvlan",
#define DM814OPT_P1JOINVLAN	18
	"switchvlan",
#define DM814OPT_SWITCHVLAN	19
	"deviceindex",
#define DM814OPT_DEVIDX		20
	"p0txflow",
#define DM814OPT_P0TXFLOW	21
	"p0rxflow",
#define DM814OPT_P0RXFLOW	22
	"p1txflow",
#define DM814OPT_P1TXFLOW	23
	"p1rxflow",
#define DM814OPT_P1RXFLOW	24
#ifdef J6
	"ptp",
#define DM814OPT_PTP		25
#endif
	NULL
};
#define RMII_STRING	"rmii"
#define GMII_STRING	"gmii"
#define RGMII_STRING	"rgmii"


int ti814x_init(struct ifnet *);
int ti814x_setup_descriptors(uintptr_t cppi_base, meminfo_t *meminfo);
int ti814x_hw_config(uintptr_t cpsw_regs, attach_args_t *attach_args);
void ti814x_stop(struct ifnet *ifp, int disable);
int ti814x_hw_reset(uintptr_t cpsw_regs);
int ti814x_read_mac_addr(ti814x_dev_t *ti814x);
void ti814x_sliver_reset(uintptr_t cpsw_regs, uint8_t sid);
void ti814x_shutdown(void *);

static struct device *dev[2] = {NULL, NULL};;
ti814x_dev_t	*ti_dev [2] = {NULL, NULL};

/*
 * ti814x_mapmem  : map in register and buffer descriptor memories
 * - memories are common to both interfaces which is why it is done at
 * such an early stage. Pointers are later propagated to interface
 *         handles in the dev_attach function.
 */
int ti814x_mapmem(attach_args_t *args, nic_config_t *cfg)
{
    int		verbose = 0; /* option not active yet */
    int		err;
    uint32_t id_ver;

    /* Map in CPSW  base */
    args->cpsw_base = mmap_device_io(cfg->io_window_size[0],
				     cfg->io_window_base[0]);
    if (args->cpsw_base == (uintptr_t)MAP_DEVICE_FAILED) {
	slogf(_SLOGC_NETWORK, _SLOG_ERROR,
	      "%s:%d: Unable to mmap_device_io() errno=%d",
	      __FUNCTION__, __LINE__, errno);
	err = errno;
	return err;
    } else {
	args->cppi_base = args->cpsw_base + CPPI_BASE;

	/* Display hardware versions if Verbose is enabled */
	if (verbose) {
	    id_ver = in32(args->cpsw_base + CPSW_ID_VER);
	    slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d CPSW mapped to 0x%08x",
		  __FUNCTION__, __LINE__, args->cpsw_base);
	    slogf(_SLOGC_NETWORK, _SLOG_INFO,
		  "%s:%d: CPSW ID:0x%02x RTL_VER:0x%02x MAJOR:0x%01x MINOR:0x%01x ",
		  __FUNCTION__, __LINE__, CPSW_ID_VER_ID(id_ver),
		  CPSW_ID_VER_RTLVER(id_ver),
		  CPSW_ID_VER_MAJOR(id_ver), CPSW_ID_VER_MINOR(id_ver) );
	    slogf(_SLOGC_NETWORK, _SLOG_ERROR,
		  "%s:%d CPPI RAM mapped to 0x%08x",
		  __FUNCTION__, __LINE__, args->cppi_base);
	}
    }

    args->cmr_base = mmap_device_io(cfg->io_window_size[1],
				    cfg->io_window_base[1]);
    if (args->cmr_base == (uintptr_t)MAP_FAILED) {
	slogf(_SLOGC_NETWORK, _SLOG_ERROR,
	      "%s:%d: Unable to mmap_device_io() errno=%d",
	      __FUNCTION__, __LINE__, errno);
	err = errno;
	/* Unmap CPSW regs as the driver will abort after returning an error */
	munmap_device_io(args->cpsw_base, TI814X_CPSW_SIZE);
	return err;
    }
    if (verbose) {
	slogf(_SLOGC_NETWORK, _SLOG_ERROR,
	      "%s:%d CMR Registers mapped to 0x%08x",
	      __FUNCTION__, __LINE__, args->cmr_base);
    }

#if	!defined(AM335X) && !defined(AM437X)
    args->pllss_base = mmap_device_io(cfg->io_window_size[2],
				      cfg->io_window_base[2]);
    if (args->pllss_base == (uintptr_t)MAP_FAILED) {
	slogf(_SLOGC_NETWORK, _SLOG_ERROR,
	      "%s:%d: Unable to mmap_device_io() errno=%d",
	      __FUNCTION__, __LINE__, errno);
	err = errno;
	/* Unmap CMR & CPSW regs as the driver will abort after returning an error */
	munmap_device_io(args->cmr_base, CMR_MODULE_SIZE);
	munmap_device_io(args->cpsw_base, TI814X_CPSW_SIZE);
	return err;
    }
    if (verbose) {
	slogf(_SLOGC_NETWORK, _SLOG_ERROR,
	      "%s:%d PLLSS Registers mapped to 0x%08x",
	      __FUNCTION__, __LINE__, args->pllss_base);
    }

#endif
#if	!defined(AM335X) && !defined(J6) && !defined(AM437X)
    args->timer_base = mmap_device_io(cfg->io_window_size[3],
				      cfg->io_window_base[3]);
    if (args->timer_base == (uintptr_t)MAP_FAILED) {
	slogf(_SLOGC_NETWORK, _SLOG_ERROR,
	      "%s:%d: Unable to mmap_device_io() errno=%d",
	      __FUNCTION__, __LINE__, errno);
	err = errno;
	/* Unmap PLLSS, CMR & CPSW regs as the driver will abort after returning an error */
	munmap_device_io(args->pllss_base, PLLSS_SIZE);
	munmap_device_io(args->cmr_base, CMR_MODULE_SIZE);
	munmap_device_io(args->cpsw_base, TI814X_CPSW_SIZE);
	return err;
    }
    if (verbose) {
	slogf(_SLOGC_NETWORK, _SLOG_ERROR,
	      "%s:%d TImer Registers mapped to 0x%08x",
	      __FUNCTION__, __LINE__, args->pllss_base);
    }
#endif
    return EOK;
}

int ti814x_parse_options(ti814x_dev_t *ti814x, const char *optstring,
			 nic_config_t *cfg)

{
    char	*value, *ptr;
    int		opt;
    char	*options, *freeptr;
    int		rc = 0;
    int		err = EOK;
    int		tmp, count;

    if (optstring == NULL) {
	return 0;
    }

    /* getsubopt() is destructive */
    freeptr = options = strdup (optstring);

    while (options && *options != '\0') {
	opt = getsubopt (&options, ti814x_opts, &value);

	switch (opt) {
	case DM814OPT_P0MODE:
	    if (ti814x != NULL) {
		if (!strcmp(value, RMII_STRING)) {
		    ti814x->link_mode |= RMII;
		} else if (!strcmp(value, GMII_STRING)) {
		    ti814x->link_mode |= GMII;
		} else if (!strcmp(value, RGMII_STRING)) {
		    ti814x->link_mode |= RGMII;
		} else {
		    slogf(_SLOGC_NETWORK, _SLOG_WARNING,
			  "Invalid Port 0 mode %s", value);
		}
	    }
	    break;
	case DM814OPT_P1MODE:
	    if (ti814x != NULL) {
		if (!strcmp(value, RMII_STRING)) {
		    ti814x->link_mode |= RMII << 16;
		} else if (!strcmp(value, GMII_STRING)) {
		    ti814x->link_mode |= GMII << 16;
		} else if (!strcmp(value, RGMII_STRING)) {
		    ti814x->link_mode |= RGMII << 16;
		} else {
		    slogf(_SLOGC_NETWORK, _SLOG_WARNING,
			  "Invalid Port 1 mode %s", value);
		}
	    }
	    break;
	case DM814OPT_P0SPEED:
	    if (ti814x != NULL) {
		tmp = strtoul(value, 0, 0);
		if ((tmp != 10) && (tmp != 100) && (tmp != 1000)) {
		    slogf(_SLOGC_NETWORK, _SLOG_WARNING,
			  "Invalid Port 0 speed %s", value);
		} else {
		    ti814x->speed |= tmp;
		}
	    }
	    break;
	case DM814OPT_P1SPEED:
	    if (ti814x != NULL) {
		tmp = strtoul(value, 0, 0);
		if ((tmp != 10) && (tmp != 100) && (tmp != 1000)) {
		    slogf(_SLOGC_NETWORK, _SLOG_WARNING,
			  "Invalid Port 1 speed %s", value);
		} else {
		    ti814x->speed |= tmp << 16;
		}
	    }
	    break;
	case DM814OPT_P0DUPLEX:
	    if (ti814x != NULL) {
		tmp = strtoul(value, 0, 0);
		if ((tmp != 0) && (tmp != 1)) {
		    slogf(_SLOGC_NETWORK, _SLOG_WARNING,
			  "Invalid Port 0 duplex %s", value);
		} else {
		    ti814x->duplex |= tmp;
		}
	    }
	    break;
	case DM814OPT_P1DUPLEX:
	    if (ti814x != NULL) {
		tmp = strtoul(value, 0, 0);
		if ((tmp != 0) && (tmp != 1)) {
		    slogf(_SLOGC_NETWORK, _SLOG_WARNING,
			  "Invalid Port 1 duplex %s", value);
		} else {
		    ti814x->duplex |= tmp << 16;
		}
	    }
	    break;
	case DM814OPT_P0MAC:
	    if ((ti814x != NULL) && (ti814x->cfg.device_index == 0)) {
		if (nic_strtomac(value, cfg->current_address)) {
		    slogf(_SLOGC_NETWORK, _SLOG_WARNING,
			  "Invalid mac address %s", value);
		}
	    }
	    break;
	case DM814OPT_P0MRU:
	    if ((ti814x != NULL) && (ti814x->cfg.device_index == 0)) {
		ti814x->cfg.mru = strtoul(value, 0, 0);
	    }
	    break;
	case DM814OPT_VERBOSE:
	    if (ti814x != NULL) {
		if (value) {
		    ti814x->cfg.verbose = strtoul(value, 0, 0);
		} else {
		    ti814x->cfg.verbose++;
		}
	    }
	    break;
	case DM814OPT_P0JOINVLAN:
	    if ((ti814x != NULL) && (ti814x->cfg.device_index == 0)) {
		count = 0;
		ptr = value;
		if (ptr == NULL) {
		    slogf(_SLOGC_NETWORK, _SLOG_WARNING,
			  "Missing joinvlan values");
		    break;
		}
		for (;;) {
		    ptr = strchr(ptr, ';');
		    count++;
		    if (ptr == NULL) {
			break;
		    } else {
			ptr++;
		    }
		}
		ti814x->join_vlan = calloc(count + 1,
					   sizeof(*ti814x->join_vlan));
		ptr = strtok(value, ";");
		count = 0;
		while (ptr != NULL) {
		    tmp = strtoul(ptr, 0, 0);
		    if ((tmp > 0) && (tmp < 4096)) {
			ti814x->join_vlan[count] = tmp;
			count++;
		    } else {
			slogf(_SLOGC_NETWORK, _SLOG_WARNING,
			      "Ignoring invalid join vlan %s", ptr);
		    }
		    ptr = strtok(NULL, ";");
		}
	    }
	    break;
#ifdef SWITCHMODE
	case DM814OPT_SWITCHVLAN:
	    if (ti814x != NULL) {
		count = 0;
		ptr = value;
		if (ptr == NULL) {
		    slogf(_SLOGC_NETWORK, _SLOG_WARNING,
			  "Missing switchvlan values");
		    break;
		}
		for (;;) {
		    ptr = strchr(ptr, ';');
		    count++;
		    if (ptr == NULL) {
			break;
		    } else {
			ptr++;
		    }
		}
		ti814x->switch_vlan = calloc(count + 1,
					   sizeof(*ti814x->switch_vlan));
		ptr = strtok(value, ";");
		count = 0;
		while (ptr != NULL) {
		    tmp = strtoul(ptr, 0, 0);
		    if ((tmp > 0) && (tmp < 4096)) {
		        ti814x->switch_vlan[count] = tmp;
			count++;
		    } else {
			slogf(_SLOGC_NETWORK, _SLOG_WARNING,
			      "Ignoring invalid switch vlan %s", ptr);
		    }
		    ptr = strtok(NULL, ";");
		}
	    }
	    break;
#else
	case DM814OPT_P0MIICLK:
	    if ((ti814x != NULL) && (ti814x->cfg.device_index == 0)) {
		ti814x->fix_mii_clk = 1;
	    }
	    break;
	case DM814OPT_P1MIICLK:
	    if ((ti814x != NULL) && (ti814x->cfg.device_index == 1)) {
		ti814x->fix_mii_clk = 1;
	    }
	    break;
	case DM814OPT_P0EMUPHY:
	    if ((ti814x != NULL) && (ti814x->cfg.device_index == 0)) {
		ti814x->emu_phy = 0;
	    }
	    break;
	case DM814OPT_P1EMUPHY:
	    if ((ti814x != NULL) && (ti814x->cfg.device_index == 1)) {
		ti814x->emu_phy = 1;
	    }
	    break;
	case DM814OPT_P0PHYIDX:
	    if ((ti814x != NULL) && (ti814x->cfg.device_index == 0)) {
		ti814x->phy_idx = strtoul(value, 0, 0);
	    }
	    break;
	case DM814OPT_P1PHYIDX:
	    if ((ti814x != NULL) && (ti814x->cfg.device_index == 1)) {
		ti814x->phy_idx = strtoul(value, 0, 0);
	    }
	    break;
	case DM814OPT_P1MRU:
	    if ((ti814x != NULL) && (ti814x->cfg.device_index == 1)) {
		ti814x->cfg.mru = strtoul(value, 0, 0);
	    }
	    break;
	case DM814OPT_P1MAC:
	    if ((ti814x != NULL) && (ti814x->cfg.device_index == 1)) {
		if (nic_strtomac(value, cfg->current_address)) {
		    slogf(_SLOGC_NETWORK, _SLOG_WARNING,
			  "Invalid mac address %s", value);
		}
	    }
	    break;
	case DM814OPT_P1JOINVLAN:
	    if ((ti814x != NULL) && (ti814x->cfg.device_index == 1)) {
		count = 0;
		ptr = value;
		if (ptr == NULL) {
		    slogf(_SLOGC_NETWORK, _SLOG_WARNING,
			  "Missing joinvlan values");
		    break;
		}
		for (;;) {
		    ptr = strchr(ptr, ';');
		    count++;
		    if (ptr == NULL) {
			break;
		    } else {
			ptr++;
		    }
		}
		ti814x->join_vlan = calloc(count + 1,
					   sizeof(*ti814x->join_vlan));
		ptr = strtok(value, ";");
		count = 0;
		while (ptr != NULL) {
		    tmp = strtoul(ptr, 0, 0);
		    if ((tmp > 0) && (tmp < 4096)) {
			ti814x->join_vlan[count] = tmp;
			count++;
		    } else {
			slogf(_SLOGC_NETWORK, _SLOG_WARNING,
			      "Ignoring invalid join vlan %s", ptr);
		    }
		    ptr = strtok(NULL, ";");
		}
	    }
	    break;
	case DM814OPT_DEVIDX:
	    cfg->device_index = strtoul(value, 0, 0);
	    break;
	case DM814OPT_P0TXFLOW:
	    if ((ti814x != NULL) && (ti814x->cfg.device_index == 0)) {
		if (ti814x->flow == -1) {
		    ti814x->flow = IFM_ETH_TXPAUSE;
		} else {
		    ti814x->flow |= IFM_ETH_TXPAUSE;
		}
	    }
	    break;
	case DM814OPT_P0RXFLOW:
	    if ((ti814x != NULL) && (ti814x->cfg.device_index == 0)) {
		if (ti814x->flow == -1) {
		    ti814x->flow = IFM_ETH_RXPAUSE;
		} else {
		    ti814x->flow |= IFM_ETH_RXPAUSE;
		}
	    }
	    break;
	case DM814OPT_P1TXFLOW:
	    if ((ti814x != NULL) && (ti814x->cfg.device_index == 1)) {
		if (ti814x->flow == -1) {
		    ti814x->flow = IFM_ETH_TXPAUSE;
		} else {
		    ti814x->flow |= IFM_ETH_TXPAUSE;
		}
	    }
	    break;
	case DM814OPT_P1RXFLOW:
	    if ((ti814x != NULL) && (ti814x->cfg.device_index == 1)) {
		if (ti814x->flow == -1) {
		    ti814x->flow = IFM_ETH_RXPAUSE;
		} else {
		    ti814x->flow |= IFM_ETH_RXPAUSE;
		}
	    }
	    break;
#endif
#ifdef J6
	case DM814OPT_PTP:
	    if (ti814x != NULL) {
		if (value) {
		    ti814x->ptp_enable = strtoul(value, 0, 0);
		} else {
		    ti814x->ptp_enable++;
		}
	    }
	    break;
#endif
	default:
	    slogf(_SLOGC_NETWORK, _SLOG_WARNING,
		  "Skipping unknown option %s", value);
	    break;
	}
    }

    free (freeptr, M_DEVBUF);
    errno = err;
    return rc;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int ti814x_read_mac_addr(ti814x_dev_t *ti814x)
{
    uint32_t	mac_hi, mac_lo;
    uint32_t	mac_hi1, mac_lo1;

    if (ti814x->cfg.verbose & DEBUG_MASK) {
	slogf(_SLOGC_NETWORK, _SLOG_DEBUG1,
	      "%s:%d - read_mac_addr for devidx=%d",
	      __FUNCTION__, __LINE__, ti814x->cfg.device_index);
    }

    /* Grab MAC address from NVRAM for permanent address */
    switch (ti814x->cfg.device_index) {
    case 0:
	mac_lo = in32(ti814x->cmr_base + MAC_ID0_LO);
	mac_hi = in32(ti814x->cmr_base + MAC_ID0_HI);
	break;
    case 1:
	mac_lo = in32(ti814x->cmr_base + MAC_ID1_LO);
	mac_hi = in32(ti814x->cmr_base + MAC_ID1_HI);
	break;
    default:
	slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d - invalid device index",
	      __FUNCTION__, __LINE__);
	return -1;
    }

#if !defined(J6)
    ti814x->cfg.permanent_address[5] = (mac_lo >> 8) & 0xff;
    ti814x->cfg.permanent_address[4] = mac_lo & 0xff;
    ti814x->cfg.permanent_address[3] = (mac_hi>>24) & 0xff;
    ti814x->cfg.permanent_address[2] = (mac_hi>>16) & 0xff;
    ti814x->cfg.permanent_address[1] = (mac_hi>>8) & 0xff;
    ti814x->cfg.permanent_address[0] = (mac_hi) & 0xff;
#else
	ti814x->cfg.permanent_address[0] = (mac_hi>>16) & 0xff;
	ti814x->cfg.permanent_address[1] = (mac_hi>>8) & 0xff;
	ti814x->cfg.permanent_address[2] = (mac_hi) & 0xff;
	ti814x->cfg.permanent_address[3] = (mac_lo>>16) & 0xff;
	ti814x->cfg.permanent_address[4] = (mac_lo>>8) & 0xff;
	ti814x->cfg.permanent_address[5] = (mac_lo) & 0xff;
#endif

    /* Check for equal mac addresses */
    if (ti814x->cfg.device_index) {
	mac_lo1 = in32(ti814x->cmr_base + MAC_ID0_LO);
	mac_hi1 = in32(ti814x->cmr_base + MAC_ID0_HI);
	if (mac_hi1 == mac_hi && mac_lo1 == mac_lo) {
	    slogf (_SLOGC_NETWORK, _SLOG_ERROR,
		   "Identical MAC_ID0 and MAC_ID1, incrementing MAC_ID1");
	    ti814x->cfg.permanent_address[5]++;
	}
    }

    /* Not specified on command line, copy to current */
    if (!memcmp(ti814x->cfg.current_address, "\0\0\0\0\0\0", ETHER_ADDR_LEN)) {
	memcpy(ti814x->cfg.current_address, ti814x->cfg.permanent_address,
	       ETHER_ADDR_LEN);
    }

	/* Multicast MAC address is invalid */
	if ((!memcmp(ti814x->cfg.current_address, "\0\0\0\0\0\0", ETHER_ADDR_LEN))
		|| (ti814x->cfg.current_address[0] & 0x1)) {
	slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d: "
		"Invalid MAC address (device index %d): all zero or a multicast MAC address - %x:%x:%x:%x:%x:%x",
		__FUNCTION__, __LINE__, ti814x->cfg.device_index,
		ti814x->cfg.current_address[0], ti814x->cfg.current_address[1],
		ti814x->cfg.current_address[2], ti814x->cfg.current_address[3],
		ti814x->cfg.current_address[4], ti814x->cfg.current_address[5]);
		return (-1);
	}

    return 0;
}


/*****************************************************************************/
/* Resets all modules.                                                       */
/*****************************************************************************/

int		ti814x_hw_reset(uintptr_t cpsw_regs)

{
int		timeout = 10;

	/* Reset CPSW module */
	out32(cpsw_regs + CPSW_SOFT_RESET, CPSW_SOFT_RESET_BIT);
	/* Poll for Reset completion */
	while (in32(cpsw_regs + CPSW_SOFT_RESET) != 0x0) {
		delay(10);
		slogf (_SLOGC_NETWORK, _SLOG_INFO, "%s:%d - CPSW reset timed out", __FUNCTION__, __LINE__);
		if (! timeout--)
			return (-1);
		}

	/* Reset DMA module */
	out32(cpsw_regs + DMA_SOFT_RESET, DMA_SOFT_RESET_BIT);
	/* Poll for Reset completion */
	timeout = 10;
	while (in32(cpsw_regs + DMA_SOFT_RESET) != 0x0) {
		delay(10);
		slogf (_SLOGC_NETWORK, _SLOG_INFO, "%s:%d - CPSW reset timed out", __FUNCTION__, __LINE__);
		if (! timeout--)
			return (-1);
		}
	
	/* Reset subsystem module */
	out32(cpsw_regs + CPSW_SS_SOFT_RESET, CPSW_SS_SOFT_RESET_BIT);
	timeout = 10;
	while (in32(cpsw_regs + CPSW_SS_SOFT_RESET) != 0x0) {
		delay(10);
		slogf (_SLOGC_NETWORK, _SLOG_INFO, "%s:%d - CPSWSS reset timed out", __FUNCTION__, __LINE__);
		if (! timeout--)
			return (-1);
		}

	/* Reset both slivers */
	ti814x_sliver_reset(cpsw_regs, 0);
	ti814x_sliver_reset(cpsw_regs, 1);

	return (0);
}

/*****************************************************************************/
/* Reset a specific interface                                                */
/*****************************************************************************/

void ti814x_sliver_reset(uintptr_t cpsw_regs, uint8_t sid)

{
uint32_t sl_soft_reset;
	
	switch(sid) {
		case 0: sl_soft_reset = SL1_SOFT_RESET; break;
		case 1: sl_soft_reset = SL2_SOFT_RESET; break;
		default:
			slogf (_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d - Invalid device index", __FUNCTION__, __LINE__);
			return;
	}
	
	out32(cpsw_regs + sl_soft_reset, SL_SOFT_RESET_BIT);
	while (in32(cpsw_regs + sl_soft_reset) != 0x0) {
		delay(100);
		slogf (_SLOGC_NETWORK, _SLOG_INFO, "%s:%d - SL1 reset timedout", __FUNCTION__, __LINE__);
	}
}

/*****************************************************************************/
/* Shutdown all interfaces and disable host interface                        */
/*****************************************************************************/

void ti814x_stop (struct ifnet *ifp, int disable)
{
    ti814x_dev_t		*ti814x;
    uintptr_t			mac_ctl = 0;
    uint32_t			mac_ctl_data, i;

    ti814x = ifp->if_softc;

    if (ti814x->cfg.verbose & DEBUG_MASK) {
	slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d - if=%d disable=%d",
	      __FUNCTION__, __LINE__, ti814x->cfg.device_index, disable);
    }

    if (disable) {
	/* Disable the specific interface */
	if (ti814x->cfg.device_index == 0) {
	    mac_ctl = ti814x->cpsw_regs + SL1_MAC_CONTROL;
	} else if (ti814x->cfg.device_index == 1) {
	    mac_ctl = ti814x->cpsw_regs + SL2_MAC_CONTROL;
	} else {
	    slogf(_SLOGC_NETWORK, _SLOG_ERROR,
		  "%s:%d - Invalid device index (%d)",
		  __FUNCTION__, __LINE__, ti814x->cfg.device_index);
	    return;
	}
	mac_ctl_data = in32(mac_ctl);
	outle32 (mac_ctl, (mac_ctl_data & ~GMII_EN));

	/* Stop monitoring phy */
	ti814x->stop_miimon = 1;
	if (ti814x->emu_phy != -1 &&
	    ti814x->emu_phy == ti814x->cfg.device_index) {
	  emu_phy_dinit (ti814x);
	}
    }
	
    /* Stop and reset the specific interface */
    ti814x_sliver_reset(ti814x->cpsw_regs, ti814x->cfg.device_index);
 
    /* Mark the interface as down */
    ifp->if_flags &= ~(IFF_RUNNING | IFF_OACTIVE);

    /* One final reap of the Tx descriptors to clean up */
    for (i = 0; i < NUM_TX_QUEUES; i++) {
	ti814x_reap_pkts(ti814x, i);
    }
}

/*****************************************************************************/
/* Stop, reset, detach and cleanup                                           */
/*****************************************************************************/

int ti814x_detach_cleanup(attach_args_t *attach_args, int how)

{
    int			i;
    struct mbuf		*m;

    if (attach_args->cfg.verbose & DEBUG_MASK) {
	slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d - how %d",
	      __FUNCTION__, __LINE__, how);
    }

    /* Reset and stop the hardware */
    ti814x_hw_reset(attach_args->cpsw_base);

    shutdownhook_disestablish(attach_args->sd_hook);

    switch (how) {
    case -1:
	InterruptDetach(attach_args->iid[2]);

    case 7:
	InterruptDetach(attach_args->iid[1]);

    case 6:
	InterruptDetach(attach_args->iid[0]);

    case 5:
	nw_pthread_reap(attach_args->tid);
	ConnectDetach(attach_args->coid);
	ChannelDestroy(attach_args->chid);
	for (i = 0; i< NUM_TX_DMA_CHAN; i++) {
	    interrupt_entry_remove(&attach_args->inter_tx[i], NULL);
	}

    case 3:
	interrupt_entry_remove(&attach_args->inter_link, NULL);
	interrupt_entry_remove(&attach_args->inter_stat, NULL);

    case 2:
	if (attach_args->meminfo.rx_mbuf) {
	    for (i = 0; i < NUM_RX_PKTS * NUM_RX_DMA_CHAN; i++) {
		if ((m = attach_args->meminfo.rx_mbuf[i]) != NULL) {
		    m_freem(m);
		    attach_args->meminfo.rx_mbuf[i] = NULL;
		}
	    }
	    free(attach_args->meminfo.rx_mbuf, M_DEVBUF);
	    attach_args->meminfo.rx_mbuf = NULL;
	}
	cache_fini(&attach_args->meminfo.cachectl);

    case 1:
#if	!defined(AM335X) && !defined(J6)
	if (attach_args->timer_base) {
		munmap_device_io(attach_args->timer_base, TIMER_SIZE);
		attach_args->timer_base = NULL;
	}
#endif
#if	!defined(AM335X) && !defined(J6)
	if (attach_args->pllss_base) {
		munmap_device_io(attach_args->pllss_base, PLLSS_SIZE);
		attach_args->pllss_base = NULL;
	}
#endif
	if (attach_args->cmr_base) {
		munmap_device_io(attach_args->cmr_base, CMR_MODULE_SIZE);
		attach_args->cmr_base = NULL;
	}

	if (attach_args->cpsw_base) {
		munmap_device_io(attach_args->cpsw_base, TI814X_CPSW_SIZE);
		attach_args->cpsw_base = NULL;
	}
    }

    return EOK;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int ti814x_hw_config(uintptr_t cpsw_regs, attach_args_t *attach_args)
{
    int	i = 0;

    /* Carve the port FIFOs for pause */
    outle32(cpsw_regs + P1_MAX_BLKS, 0xE6);
    outle32(cpsw_regs + P2_MAX_BLKS, 0xE6);

    /* Enable Switch flow control */
    outle32(cpsw_regs + CPSW_FLOW_CONTROL, 7);

    /*
     * Setup some QoS priorities and shaping, see sprugz8d.pdf 9.2.1.6.2.1
     * Don't actually enable here as otherwise will block VLAN priority
     * traffic. Leave enables until we handle set bandwidth ioctl,
     * see ti814x_set_tx_bw()
     */
    outle32(cpsw_regs + P0_TX_IN_CTL,
	    NORM_PRI_MODE | TX_BLKS_REM | TX_PRI_WDS);
    outle32(cpsw_regs + P0_TX_PRI_MAP, 0x33332100);

    outle32(cpsw_regs + P1_TX_IN_CTL,
	    NORM_PRI_MODE | TX_BLKS_REM | TX_PRI_WDS);
    outle32(cpsw_regs + P1_TX_PRI_MAP, 0x33332100);

    outle32(cpsw_regs + P2_TX_IN_CTL,
	    NORM_PRI_MODE | TX_BLKS_REM | TX_PRI_WDS);
    outle32(cpsw_regs + P2_TX_PRI_MAP, 0x33332100);

    outle32(cpsw_regs + CPDMA_RX_CH_MAP, 0x32103210);

    /* Initialize DMA Host 2.2.1.1 cpdma 1.8.1 */
    /* 2. Initialize HDP registers to Zero */
    for (i = 0; i < NUM_TX_DMA_CHAN; i++) {
	outle32 (cpsw_regs + TX0_HDP + (4 * i), 0);
    }
    for (i = 0; i < NUM_RX_DMA_CHAN; i++) {
	outle32 (cpsw_regs + RX0_HDP + (4 * i), 0);
    }
    /* 3. Enable the interrupts in the Int Mask Registers */
    outle32(cpsw_regs + RX_INTMASK_SET,
	    RX0_PEND_MASK | RX1_PEND_MASK | RX2_PEND_MASK | RX3_PEND_MASK);
    outle32(cpsw_regs + TX_INTMASK_SET,
	    TX0_PEND_MASK | TX1_PEND_MASK | TX2_PEND_MASK | TX3_PEND_MASK |
	    TX4_PEND_MASK | TX5_PEND_MASK | TX6_PEND_MASK | TX7_PEND_MASK);
    /* 4. Write the rx_buffer_offset register value */
    outle32(cpsw_regs + RX_BUFFER_OFFSET, 0);
    /* 5. setup the receive buffer descriptors as required by CPPI 3.0 */
    if (ti814x_setup_descriptors(attach_args->cppi_base,
				 &attach_args->meminfo)) {
	slogf (_SLOGC_NETWORK, _SLOG_ERROR, "%s: setup_descriptors error",
	       __FUNCTION__);
	return (-1);
    }
    /* 6. Set RX_EN bit in the RX_Control register to enable Rx DMA*/
    outle32(cpsw_regs + RX_CONTROL, RX_EN);
    outle32(cpsw_regs + TX_CONTROL, TX_EN);

    /* enable interrupts */
    outle32(cpsw_regs + C0_RX_EN, RX_EN_CH0 | RX_EN_CH1 | RX_EN_CH2 |
	    RX_EN_CH3 | RX_EN_CH4 | RX_EN_CH5 | RX_EN_CH6 | RX_EN_CH7);
    outle32(cpsw_regs + C0_TX_EN, TX_EN_CH0 | TX_EN_CH1 | TX_EN_CH2 |
	    TX_EN_CH3 | TX_EN_CH4 | TX_EN_CH5 | TX_EN_CH6 | TX_EN_CH7);

    /* Tell the DMA engine where to put the RX Data */
    for (i = 0; i < NUM_RX_DMA_CHAN; i++) {
	out32(cpsw_regs + RX0_HDP + (i * sizeof(uint32_t)), CPPI_DESC_PHYS +
	      (i * NUM_RX_PKTS * sizeof(cppi_desc_t)));
    }

    return EOK;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int ti814x_setup_descriptors(uintptr_t cppi_base, meminfo_t *meminfo)
{
    uint32_t			chan, pkt, size;
    uint32_t			desc_phys;
    off64_t			phys;
    cppi_desc_t			*desc;
    struct mbuf			*m;
    struct nw_work_thread	*wtp;
	
    wtp = WTP;

    if (cache_init (0, &meminfo->cachectl, NULL) == -1) {
	slogf (_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d: cache_init() failed",
	       __FUNCTION__, __LINE__);
	return errno;
    }

    /* Allocate array of mbuf pointers for receiving */
    size = NUM_RX_PKTS * NUM_RX_DMA_CHAN * sizeof(meminfo->rx_mbuf);
    if ((meminfo->rx_mbuf = malloc(size, M_DEVBUF, M_NOWAIT)) == NULL) {
	return errno;
    }
    memset (meminfo->rx_mbuf, 0, size);

    desc_phys = CPPI_DESC_PHYS;
    desc = (cppi_desc_t *)cppi_base;

    /* Carve the Rx descriptors */
    for (chan = 0; chan < NUM_RX_DMA_CHAN; chan++) {
	for (pkt = 0; pkt < NUM_RX_PKTS; pkt++, desc++) {
	    if (meminfo->rx_mbuf [(chan * NUM_RX_PKTS) + pkt] != NULL) {
		continue;
	    }

	    m = m_getcl_wtp (M_DONTWAIT, MT_DATA, M_PKTHDR, wtp);
	    if (m == NULL) {
		return (-1);
	    }
	    meminfo->rx_mbuf [(chan * NUM_RX_PKTS) + pkt] = m;
	    desc_phys += sizeof (cppi_desc_t);
	    if (pkt == (NUM_RX_PKTS - 1)) {
		desc->next = 0;
	    } else {
		desc->next = desc_phys;
	    }
	    phys = pool_phys(m->m_data, m->m_ext.ext_page);
	    CACHE_INVAL (&meminfo->cachectl, m->m_data, phys,
			 m->m_ext.ext_size);
	    desc->buffer = (uint32_t) phys;
	    desc->off_len = MAX_PKT_SIZE;
	    desc->flag_len = DESC_FLAG_OWN;
	}
    }


    /*
     * Setup/Initialize Transmit buffer descriptor memory space
     * TX descriptors start at 0x4a103000
     * Initialze all descriptors, max=128 per port
     * 128(desc. per port) * 2(ports) * 16(bytes per desc.) = 0x1000.
     * We'll carve up the pool
     * per interface later in the initialization
     */
    desc = (cppi_desc_t*)(cppi_base + (uintptr_t)CPPI_TX_DESC_OFFSET);
    for (pkt = 0; pkt < TOTAL_NUM_TX_PKTS; pkt++, desc++) {
	desc->next = 0;
	desc->buffer = 0;
	desc->off_len = 0;
	desc->flag_len = DESC_FLAG_EOQ; /* Make first transmit kick the DMA */
    }
    return 0;
}

/****************************************************************************/
/*                                                                          */
/****************************************************************************/

void ti814x_shutdown (void *arg)

{
    attach_args_t	*attach_args = arg;

    if (attach_args->cfg.verbose & DEBUG_MASK) {
        slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d",
	      __FUNCTION__, __LINE__);
    }

    ti814x_hw_reset(attach_args->cpsw_base);
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int ti814x_detach (struct device *dev, int flags)

{
    struct ti814x_dev	*idev;
    ti814x_dev_t	*ti814x;
    struct  ifnet	*ifp;
    int			i, j, offset;
    struct mbuf		*m;

    idev = (struct ti814x_dev *)dev;
    ti814x = idev->sc_ti814x;
    ifp = &ti814x->ecom.ec_if;

    /* Don't init() while we're dying. */
    ti814x->dying = 1;

    /* Stop the timers */
    callout_stop(&ti814x->mii_callout);

    ti814x_stop(ifp, 1);

    IF_PURGE(&ti814x->rx_queue);
    pthread_mutex_destroy(&ti814x->rx_mutex);

    /* Free up allocated Tx resources */
    if (ti814x->tx_mbuf) {
	for (i = 0; i < NUM_TX_DMA_CHAN; i++) {
	    offset = i * NUM_TX_PKTS;
	    for (j = 0; j < NUM_TX_PKTS; j++) {
		if ((m = ti814x->tx_mbuf[j + offset]) != NULL) {
		    m_freem (m);
		    ti814x->tx_mbuf[j + offset] = NULL;
		}
	    }
	}
	free(ti814x->tx_mbuf, M_DEVBUF);
	ti814x->tx_mbuf = NULL;
    }

    interrupt_entry_remove(&ti814x->inter, NULL);

    if (ti814x->join_vlan != NULL) {
	(free)(ti814x->join_vlan);
	ti814x->join_vlan = NULL;
    }
    if (ti814x->switch_vlan != NULL) {
	(free)(ti814x->switch_vlan);
	ti814x->switch_vlan = NULL;
    }

    ether_ifdetach(ifp);
    if_detach(ifp);

    ti_dev[ti814x->cfg.device_index] = NULL;

    if (((ti814x->cfg.device_index == 0) && (ti_dev[1] != NULL)) ||
		((ti814x->cfg.device_index == 1) && (ti_dev[0] != NULL))) {
		/* We've removed one interface but the other is still present */
		return EOK;
	}

    return ti814x_detach_cleanup(&attach_args, -1);
}

void ti814x_rx_thread_quiesce (void *arg, int die)
{
    attach_args_t		*attach_args = arg;

    MsgSendPulse(attach_args->coid, SIGEV_PULSE_PRIO_INHERIT,
		 TI814X_QUIESCE_PULSE, die);

    return;
}

static int ti814x_rx_thread_init (void *arg)
{
    attach_args_t		*attach_args = arg;
    struct nw_work_thread	*wtp = WTP;

    pthread_setname_np(gettid(), "dm814x Rx");

    wtp->quiesce_callout = ti814x_rx_thread_quiesce;
    wtp->quiesce_arg = attach_args;
    return EOK;
}

/*****************************************************************************/
/* Initial driver entry point.                                               */
/* Returns -1 on failure; errno will be set to ENODEV if no devices detected.*/
/*****************************************************************************/

int ti814x_entry(void *dll_hdl, struct _iopkt_self *iopkt, char *options)

{
    int		instance;
    int		err, idx, i;
    uintptr_t	cpsw_regs = 0;
    struct nw_stk_ctl	*sctlp;
    struct ifnet	*ifp;
    struct drvcom_config	*dcon;

    /* Check if it is already mounted by doing a "nicinfo" on each interface */
    dcon = (malloc)(sizeof(*dcon));
    if (dcon == NULL) {
		slogf(_SLOGC_NETWORK, _SLOG_ERROR,
		      "%s:%d - malloc failed", __FUNCTION__, __LINE__);
		return ENOMEM;
    }
    IFNET_FOREACH(ifp) {
		dcon->dcom_cmd.ifdc_cmd = DRVCOM_CONFIG;
		dcon->dcom_cmd.ifdc_len = sizeof(dcon->dcom_config);
		err = ifp->if_ioctl(ifp, SIOCGDRVCOM, (caddr_t)dcon);
		if ((err == EOK) && (dcon->dcom_config.num_io_windows > 0) &&
		    (dcon->dcom_config.io_window_base[0] == TI814X_CPSW_BASE)) {
			slogf(_SLOGC_NETWORK, _SLOG_ERROR,
			      "%s:%d - Driver already loaded for %s",
			      __FUNCTION__, __LINE__, ifp->if_xname);
			return EBUSY;
		}
    }
    (free)(dcon);

    sctlp = &stk_ctl;

    /* initialize arguments/parameters for ti814x_attach() */
    memset(&attach_args, 0x00, sizeof(attach_args));


    /* Initialize device_index to -1 to see if parse_option sets it */
    attach_args.cfg.device_index = -1;
    attach_args.iopkt = iopkt;
    attach_args.options = options;

    if (options != NULL) {
		if ((err = ti814x_parse_options(NULL, options,
					&attach_args.cfg)) != EOK) {
		    return(err);
		}
    }

    /* Configure Interrupts and memory mappings */
    attach_args.cfg.num_io_windows = 2;
    attach_args.cfg.io_window_base[0] = TI814X_CPSW_BASE;
    attach_args.cfg.io_window_size[0] = TI814X_CPSW_SIZE;
    attach_args.cfg.io_window_base[1] = CMR_BASE;
    attach_args.cfg.io_window_size[1] = CMR_MODULE_SIZE;
#if	!defined(AM335X) && !defined(AM437X)
    attach_args.cfg.num_io_windows++;
    attach_args.cfg.io_window_base[2] = PLLSS_BASE;
    attach_args.cfg.io_window_size[2] = PLLSS_SIZE;
#endif
#if	!defined(AM335X) && !defined(J6) && !defined(AM437X)
    attach_args.cfg.num_io_windows++;
    attach_args.cfg.io_window_base[3] = TIMER2_BASE;
    attach_args.cfg.io_window_size[3] = TIMER_SIZE;
#endif
    attach_args.cfg.num_irqs = 3;
    attach_args.cfg.irq[0] = TI814x_RX_INT0;
    attach_args.cfg.irq[1] = TI814x_MISC_INT0;
    attach_args.cfg.irq[2] = TI814x_TX_INT0;
#if	!defined(AM335X) && !defined(J6) && !defined(AM437X)
    attach_args.cfg.num_irqs += 1;
    attach_args.cfg.irq[3] = TINT2;
#endif
    idx = attach_args.cfg.device_index;
    instance = (idx == -1) ? 0 : idx;	   /* Start at first instance */
    if (attach_args.cfg.device_index != -1) {
		/* configure only the interface specified */
		single = 1;
    }

#ifdef SWITCHMODE
    single = 1;
    attach_args.cfg.device_index = instance = idx = 0;
#endif

    if ((err = ti814x_mapmem(&attach_args, &attach_args.cfg)) != EOK) {
	slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d Problem mapping memories",
	      __FUNCTION__, __LINE__);
	return err;
    }
    cpsw_regs = attach_args.cpsw_base;
    if (ti814x_hw_reset(cpsw_regs)) {
        ti814x_detach_cleanup(&attach_args, 1);
	return ENODEV;
    }

    attach_args.sd_hook = shutdownhook_establish(ti814x_shutdown,
						 &attach_args);

    /* Initialize all CPPI memory before splitting off for each initialized interface */
    memset((void*)attach_args.cppi_base, 0x00, CPPI_DESC_MEM_SIZE);

    ti814x_hw_config(cpsw_regs, &attach_args);

    /* Enable ALE and Clear ALE Table */
    ti814x_ale_init(cpsw_regs);

    /* Setup interrupt related info */
    attach_args.inter_link.func = ti814x_process_link_interrupt;
    attach_args.inter_link.enable = ti814x_enable_link_interrupt;
    attach_args.inter_link.arg = &attach_args;
    attach_args.inter_stat.func = ti814x_process_stat_interrupt;
    attach_args.inter_stat.enable = ti814x_enable_stat_interrupt;
    attach_args.inter_stat.arg = &attach_args;
    attach_args.isrp = ti814x_isr;
    attach_args.isrp_lnk = ti814x_isr_link;
    attach_args.isrp_tx = ti814x_isr_tx;
    for (i = 0; i < NUM_TX_DMA_CHAN; i++) {
		attach_args.inter_tx[i].func = ti814x_process_tx_interrupt;
		attach_args.inter_tx[i].enable = ti814x_enable_tx_interrupt;
		attach_args.inter_tx[i].arg = (void*)i;
    }

    err = interrupt_entry_init (&attach_args.inter_link, 0, NULL,
				IRUPT_PRIO_DEFAULT);
    if (err != EOK) {
        ti814x_detach_cleanup(&attach_args, 2);
		return err;
    }
    err = interrupt_entry_init (&attach_args.inter_stat, 0, NULL,
				IRUPT_PRIO_DEFAULT);
    if (err != EOK) {
        ti814x_detach_cleanup(&attach_args, 3);
		return err;
    }
    for (i = 0; i < NUM_TX_DMA_CHAN; i++) {
		err = interrupt_entry_init (&attach_args.inter_tx[i], 0, NULL,
				    IRUPT_PRIO_DEFAULT);
		if (err != EOK) {
	        ti814x_detach_cleanup(&attach_args, 4);
		    return err;
		}
    }

    attach_args.chid = ChannelCreate(0);
    attach_args.coid = ConnectAttach(ND_LOCAL_NODE, 0, attach_args.chid,
				      _NTO_SIDE_CHANNEL, 0);

    for (i = 0; i < NUM_RX_DMA_CHAN; i++) {
	/*
	 * Create a pulse for each class of traffic. Start at the default
	 * rx_prio but increment by 2 for each class to allow a process to
	 * run at (class - 1) receiving and still have priority over the
	 * lower class traffic without impacting the dequeueing of packets
	 * from the very limited Rx descriptors.
	 */
	SIGEV_PULSE_INIT(&attach_args.isr_event[i], attach_args.coid,
			 sctlp->rx_prio + (2 * i),
			 TI814X_RX_PULSE, i);
    }
    nw_pthread_create(&attach_args.tid, NULL,
		      ti814x_rx_thread, &attach_args, 0,
		      ti814x_rx_thread_init, &attach_args);

    if ((err = InterruptAttach_r(attach_args.cfg.irq[0],
				 attach_args.isrp,
				 &attach_args, sizeof(attach_args),
				 _NTO_INTR_FLAGS_TRK_MSK)) < 0) {
	err = -err;
	slogf (_SLOGC_NETWORK, _SLOG_ERROR,
	       "%s:%d InterruptAttach_r error (%d) ",
	       __FUNCTION__, __LINE__, err);
        ti814x_detach_cleanup(&attach_args, 5);
	return err;
    }
    attach_args.iid[0] = err;

    if ((err = InterruptAttach_r(attach_args.cfg.irq[1],
				 attach_args.isrp_lnk,
				 &attach_args, sizeof(attach_args),
				 _NTO_INTR_FLAGS_TRK_MSK)) < 0) {
	err = -err;
	slogf (_SLOGC_NETWORK, _SLOG_ERROR,
	       "%s:%d InterruptAttach_r error (%d) ",
	       __FUNCTION__, __LINE__, err);
        ti814x_detach_cleanup(&attach_args, 6);
	return err;
    }
    attach_args.iid[1] = err;

    if ((err = InterruptAttach_r(attach_args.cfg.irq[2],
				 attach_args.isrp_tx,
				 &attach_args, sizeof(attach_args),
				 _NTO_INTR_FLAGS_TRK_MSK)) < 0) {
	err = -err;
	slogf (_SLOGC_NETWORK, _SLOG_ERROR,
	       "%s:%d InterruptAttach_r error (%d) ",
	       __FUNCTION__, __LINE__, err);
        ti814x_detach_cleanup(&attach_args, 7);
	return err;
    }
    attach_args.iid[2] = err;

    /* Call dev_attach for every instance found */
    while(instance < 2) {
	if (!single) {
	    attach_args.cfg.device_index = instance;
	}
	err = dev_attach("dm", options, &ti814x_ca, &attach_args,
			 &single, &dev[instance], NULL);
	if (err != EOK) {
	    if ((instance == 1) && (ti_dev[0] != NULL)) {
	        /* dm1 failed so cleanup dm0 */
	      ti_dev[0]->dev.dv_dll_hdl = NULL;
	      dev_detach(&ti_dev[0]->dev, ti_dev[0]->dev.dv_detflags);
	    }
	    return err;
	}

	dev[instance]->dv_dll_hdl = dll_hdl;

	instance++;
	if (single) {
	  break;
	}
    }

    /*
     * If there is more than one interfaces enabled we should store their
     * ecom structures for use in the RX interrupt processing since one
     * interface may be receiving for the other, which forces us to have
     * the ability to tell io-pkt what interface the packet originated
     * from.
     */

    if (!single) {
	/*
	 * Only run this code if two interfaces were detected,
	 * i.e. dev structure is populated by dev_attach
	 */
        ((ti814x_dev_t*)dev[0])->common_ecom[0] = &((ti814x_dev_t*)dev[0])->ecom;
	((ti814x_dev_t*)dev[0])->common_ecom[1] = &((ti814x_dev_t*)dev[1])->ecom;
	((ti814x_dev_t*)dev[1])->common_ecom[0] = &((ti814x_dev_t*)dev[0])->ecom;
	((ti814x_dev_t*)dev[1])->common_ecom[1] = &((ti814x_dev_t*)dev[1])->ecom;
    } else {
	if (idx == 0) {
	    ((ti814x_dev_t*)dev[0])->common_ecom[0] = &((ti814x_dev_t*)dev[0])->ecom;
	    if (dev[1] != NULL) {
		((ti814x_dev_t*)dev[0])->common_ecom[1] = &((ti814x_dev_t*)dev[1])->ecom;
		((ti814x_dev_t*)dev[1])->common_ecom[0] = &((ti814x_dev_t*)dev[0])->ecom;
	    }
	} else {
	    ((ti814x_dev_t*)dev[1])->common_ecom[1] = &((ti814x_dev_t*)dev[1])->ecom;
	    if (dev[0] != NULL) {
		((ti814x_dev_t*)dev[1])->common_ecom[0] = &((ti814x_dev_t*)dev[0])->ecom;
		((ti814x_dev_t*)dev[0])->common_ecom[1] = &((ti814x_dev_t*)dev[1])->ecom;
	    }
	}
    }
#if	!defined(AM335X) && !defined(AM437X)
#ifdef J6
    ti814x_ptp_init(ti_dev[instance - 1]);
#else
    ti814x_ptp_init(&attach_args);
#endif
#endif
    if (instance > 0) {
      return EOK;
    }
    return ENODEV;
}

/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int ti814x_attach (struct device *parent, struct device *self, void *aux)

{
    ti814x_dev_t		*ti814x;
    struct  ifnet		*ifp;
    attach_args_t		*attach_args;
    struct _iopkt_self	*iopkt;
    char				*options;
    int					err, tx_desc_offset=0, size=0;
    uint32_t			link_mode;
    int					idx, i;

    attach_args = aux;
    iopkt = attach_args->iopkt;
    options = attach_args->options;
    ti814x = (ti814x_dev_t*)self;
    ti814x->iopkt = iopkt;	
    ifp = &ti814x->ecom.ec_if;
    ifp->if_softc = ti814x;

    if (ti814x->cfg.verbose & DEBUG_MASK) {
		slogf(_SLOGC_NETWORK, _SLOG_INFO, "%s:%d devid=%d self=%p",
	      __FUNCTION__, __LINE__, attach_args->cfg.device_index, self);
    }

    /* Copy memory/buffer information intitalized in ti814x_entry */
    memcpy(&ti814x->meminfo, &attach_args->meminfo, sizeof(meminfo_t));
    /* Copy various config parameters initilized in ti814x_entry */
    memcpy(&ti814x->cfg, &attach_args->cfg, sizeof(nic_config_t));
	if (single)
		ti814x->get_stats = 1;
	else
		if (!ti814x->cfg.device_index)
			ti814x->get_stats = 1;

    /* Set interface name */
    strcpy (ifp->if_xname, ti814x->dev.dv_xname);
    strcpy ((char *) ti814x->cfg.uptype, "en");
    strcpy ((char *) ti814x->cfg.device_description, "ti814x");
    /* Store mmap information initialized in ti814x_entry */
    ti814x->cpsw_regs = attach_args->cpsw_base;
    ti814x->cppi_base = attach_args->cppi_base;
    ti814x->cmr_base  = attach_args->cmr_base;
#if	!defined(AM335X) && !defined(AM437X)
    ti814x->pllss_base  = attach_args->pllss_base;
#endif
#if	!defined(AM335X) && !defined(J6) && !defined(AM437X)
    ti814x->timer_base  = attach_args->timer_base;
#endif

    /* Only 1 set of stats for both ports so can't report them */
    ti814x->stats.un.estats.valid_stats = 0;
	if (ti814x->get_stats) {
	    ti814x->stats.un.estats.valid_stats = VALID_STATS;
		}

    /* Generic networking stats we are interested in */
    ti814x->stats.valid_stats =
      NIC_STAT_TX_FAILED_ALLOCS			|
      NIC_STAT_RX_FAILED_ALLOCS			|
      NIC_STAT_RXED_MULTICAST			|
      NIC_STAT_RXED_BROADCAST			|
      NIC_STAT_TXED_BROADCAST			|
      NIC_STAT_TXED_MULTICAST;

    ti814x->cfg.priority = IRUPT_PRIO_DEFAULT;  
    ti814x->cfg.lan = ti814x->dev.dv_unit;					   
    ti814x->cfg.media_rate = -1;
    ti814x->cfg.duplex = -1;
    ti814x->force_link = -1;
    ti814x->flow = -1;
    ti814x->emu_phy = -1;
    for (i = 0; i < NUM_RX_DMA_CHAN; i++) {
		attach_args->rx_tail[i] = NUM_RX_PKTS - 1;
    }
    ti814x->cfg.mtu = ETH_MAX_DATA_LEN;
    ti814x->cfg.mru = ETH_MAX_DATA_LEN;
    ti814x->cfg.flags |= NIC_FLAG_MULTICAST;
    ti814x->cfg.mac_length = ETH_MAC_LEN;
    attach_args->meminfo.rx_desc = (cppi_desc_t*)attach_args->cppi_base;
    ti814x->phy_idx = 0;

    /* Allocate array of mbuf pointers for tracking pending transmit packets */
    size = NUM_TX_PKTS * sizeof(ti814x->tx_mbuf) * NUM_TX_DMA_CHAN;
    if ((ti814x->tx_mbuf = malloc(size, M_DEVBUF, M_NOWAIT)) == NULL) {
		err = errno;
		ti814x_detach_cleanup (attach_args, -1);
		return (err);
    }
    memset(ti814x->tx_mbuf, 0, size);

    /* Adjust tx descriptor pointers so that they are interface exclusive */
    tx_desc_offset = ti814x->cfg.device_index * NUM_TX_PKTS
			* sizeof(cppi_desc_t);
    ti814x->meminfo.tx_desc = (cppi_desc_t*)(ti814x->cppi_base +
					     CPPI_TX_DESC_OFFSET +
					     tx_desc_offset);
    ti814x->meminfo.tx_phys = CPPI_DESC_PHYS + CPPI_TX_DESC_OFFSET +
				tx_desc_offset;

    /* Setup ethercomm */
    ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;
    ifp->if_ioctl = ti814x_ioctl;
    ifp->if_start = ti814x_start;
    ifp->if_init  = ti814x_init;
    ifp->if_stop  = ti814x_stop;
    IFQ_SET_READY(&ifp->if_snd);
    ti814x->ecom.ec_capabilities |= ETHERCAP_VLAN_MTU;

    /* Setup interrupt related info */
    ti814x->inter.func = ti814x_process_interrupt;
    ti814x->inter.enable = ti814x_enable_interrupt;
    ti814x->inter.arg = ti814x;
    if ((err = interrupt_entry_init (&ti814x->inter, 0, NULL,
				     IRUPT_PRIO_DEFAULT)) != EOK) {
		ti814x_detach_cleanup (attach_args, -1);
		return err;
    }

    pthread_mutex_init(&ti814x->rx_mutex, NULL);
    IFQ_SET_MAXLEN(&ti814x->rx_queue, IFQ_MAXLEN);

    ti814x->link_mode = 0;
    ti814x->speed = 0;
    ti814x->duplex = 0;

    /* Parse options */
    if (ti814x_parse_options(ti814x, options, &ti814x->cfg) == -1) {
		err = errno;
		if (ti814x->join_vlan != NULL) {
		    (free)(ti814x->join_vlan);
		    ti814x->join_vlan = NULL;
		}
		if (ti814x->switch_vlan != NULL) {
		    (free)(ti814x->switch_vlan);
		    ti814x->switch_vlan = NULL;
		}
		IF_PURGE(&ti814x->rx_queue);
		pthread_mutex_destroy(&ti814x->rx_mutex);
		interrupt_entry_remove(&ti814x->inter, NULL);
		ti814x_detach_cleanup (attach_args, -1);
		return (err);
    }

    /* Configure CMR GMII_SET */
    link_mode = 0;
#ifdef	J6
    link_mode = inle32 (ti814x->cmr_base + GMII_SET) & ~0x33;
#endif
#ifdef AM335X
    /*
     * SPRZ360F errata advisory 1.0.10: AM335x does not support internal delay mode.
     * Also set RMII ref clock as input to prevent contention with the phy on some boards
     */
    link_mode = GMII0_ID_MODE | GMII1_ID_MODE |
		GMII0_RMII_CLKIN | GMII1_RMII_CLKIN;
#endif
    /* GMII is 0 hence no test for it */
    if ((ti814x->link_mode & 0xFFFF) == RMII) {
		link_mode |= GMII0_SEL_RMII;
    }
    /* RGMII is default */
    if (((ti814x->link_mode & 0xFFFF) == RGMII) ||
		((ti814x->link_mode & 0xFFFF) == 0)) {
#ifndef	J6
		link_mode |= GMII0_SEL_RGMII | RGMII0_EN;
#else
		link_mode |= GMII0_SEL_RGMII;
#endif
    }
    if ((ti814x->link_mode >> 16) == RMII) {
		link_mode |= GMII1_SEL_RMII;
    }
    if (((ti814x->link_mode >> 16) == RGMII) ||
		((ti814x->link_mode >> 16) == 0)) {
#ifndef	J6
		link_mode |= GMII1_SEL_RGMII | RGMII1_EN;
#else
		link_mode |= GMII1_SEL_RGMII;
#endif
    }

    /* PLL & CM registers are in privileged memory */
    if (ThreadCtl(_NTO_TCTL_IO_PRIV, NULL) == -1) {
	    slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d ThreadCtl failed: %s",
              __FUNCTION__, __LINE__, strerror(errno));
        /* Continue in user mode using default GMII_SEL reg value.
           Ptp master will work, slave is broken */
    }
    outle32(ti814x->cmr_base + GMII_SET, link_mode);

#ifndef SWITCHMODE
    if ((ti814x->cfg.device_index == 0) && ((ti814x->speed & 0xffff) != 0)) {
		ti814x->cfg.media_rate = (ti814x->speed & 0xffff) * 1000;
		ti814x->cfg.duplex = ti814x->duplex & 0xffff;
    }
    if ((ti814x->cfg.device_index == 1) && ((ti814x->speed >> 16) != 0)) {
		ti814x->cfg.media_rate = (ti814x->speed >> 16) * 1000;
		ti814x->cfg.duplex = ti814x->duplex >> 16;
    }

	/* Some boards only have a 10/100 PHY, so we init the phy first so that
	   we can find out whether the board has gigabit or not. Once this has been
	   established, we can call the bsd_mii_initmedia function to advertise
	   our supported rates.
	 */

	if (ti814x_findphy (ti814x) != EOK) {
		slogf (_SLOGC_NETWORK, _SLOG_ERROR, "Unable to find a PHY");
		ti814x_detach_cleanup (attach_args, -1);
		return (EINVAL);
	}

    /* hook up so media devctls work */
    bsd_mii_initmedia(ti814x);
#endif

    if (ti814x->cfg.verbose & DEBUG_MASK) {
		slogf(_SLOGC_NETWORK, _SLOG_ERROR,
	      "%s:%d - Attaching if=%s unit=%d ti814x=%p sizeof(ti814x)=%d",
	      __FUNCTION__, __LINE__, ti814x->dev.dv_xname,
	      ti814x->cfg.device_index, ti814x, sizeof(ti814x_dev_t));
    }

    /* If media was not specified on cmdline, default to NIC_MEDIA_802_3 */
    if (ti814x->cfg.media == -1) {
		ti814x->cfg.media = NIC_MEDIA_802_3;
		ti814x->stats.media = ti814x->cfg.media;
    } else {
		ti814x->stats.media = ti814x->cfg.media;
    }

    if (ti814x->cfg.mtu == 0 || ti814x->cfg.mtu > ETH_MAX_PKT_LEN) {
		ti814x->cfg.mtu = ETH_MAX_PKT_LEN;
    }

    if (ti814x->cfg.mru == 0 || ti814x->cfg.mru > ETH_MAX_PKT_LEN) {
		ti814x->cfg.mru = ETH_MAX_PKT_LEN;
    }

    idx = ti814x->cfg.device_index;

#ifndef SWITCHMODE
    if (ti814x->cfg.media_rate != -1) {
		/* Don't autoneg flow if speed configured */
		if (ti814x->flow == -1) {
		    ti814x->flow = 0;
		}

		switch (ti814x->cfg.media_rate / 1000) {
			case	1000:
			    if (ti814x->link_mode != RMII) {
					ti814x->force_link = MDI_1000bTFD;
			    } else {
		      slogf (_SLOGC_NETWORK, _SLOG_DEBUG1,
			     "Gigabit speed not valid in RMII mode");
		      if (ti814x->join_vlan != NULL) {
				  (free)(ti814x->join_vlan);
				  ti814x->join_vlan = NULL;
		      }
		      if (ti814x->switch_vlan != NULL) {
				  (free)(ti814x->switch_vlan);
				  ti814x->switch_vlan = NULL;
		      }
		      IF_PURGE(&ti814x->rx_queue);
		      pthread_mutex_destroy(&ti814x->rx_mutex);
		      interrupt_entry_remove(&ti814x->inter, NULL);
			  ti814x_detach_cleanup (attach_args, -1);

		      return EINVAL;
		    }
		    break;
			case	100:
			    ti814x->force_link = (ti814x->cfg.duplex > 0) ?
					MDI_100bTFD : MDI_100bT;
			    break;
			case	10:
			    ti814x->force_link = (ti814x->cfg.duplex > 0) ?
					MDI_10bTFD : MDI_10bT;
			    break;
			default:
			    break;
		}
    }

    ti814x->cfg.flags |= NIC_FLAG_LINK_DOWN;
#endif


    /* If the emu_phy command line option is != to the index, set it to unused */
    if (ti814x->emu_phy != -1 && ti814x->emu_phy != idx) {
		ti814x->emu_phy = -1;
    }
    callout_init(&ti814x->mii_callout);

    ti_dev [idx] = ti814x;

    if (ti814x_read_mac_addr(ti814x)) {
		slogf (_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d: hw_config failed",
	       __FUNCTION__, __LINE__);
		if (ti814x->join_vlan != NULL) {
		    (free)(ti814x->join_vlan);
		    ti814x->join_vlan = NULL;
		}
		if (ti814x->switch_vlan != NULL) {
		    (free)(ti814x->switch_vlan);
		    ti814x->switch_vlan = NULL;
		}
		IF_PURGE(&ti814x->rx_queue);
		pthread_mutex_destroy(&ti814x->rx_mutex);
		interrupt_entry_remove(&ti814x->inter, NULL);
		ti814x_detach_cleanup (attach_args, -1);
		return (-1);
    }

    if_attach (ifp);
    ether_ifattach (ifp, ti814x->cfg.current_address);

    /* Intercept if_output for pulling off AVB packets */
    ti814x->stack_output = ti814x->ecom.ec_if.if_output;
    ti814x->ecom.ec_if.if_output = ti814x_output;

    if (ti814x->cfg.verbose) {
		nic_dump_config (&ti814x->cfg);
    }

    return EOK;
}


/*****************************************************************************/
/*                                                                           */
/*****************************************************************************/

int ti814x_init (struct ifnet *ifp)

{
ti814x_dev_t			*ti814x;
struct nw_work_thread	*wtp;
struct _iopkt_self		*iopkt;
uint32_t				value;
uint8_t					port_mask;
int						loop;

	ti814x = ifp->if_softc;
	iopkt = ti814x->iopkt;
	wtp = WTP;

	if (ti814x->cfg.verbose & DEBUG_MASK) {
		slogf(_SLOGC_NETWORK, _SLOG_ERROR, "%s:%d - Start devidx=%d",
			__FUNCTION__, __LINE__,ti814x->cfg.device_index);
	}

	if (ti814x->dying == 1)
		return (0);

	ti814x->cfg.mtu = ifp->if_mtu;
	ti814x->cfg.mru = ifp->if_mtu;

#ifndef SWITCHMODE
	switch(ti814x->cfg.device_index) {
		case 0:
			/* init receive buffer offset and max length */
			if (ti814x->ecom.ec_capenable & ETHERCAP_VLAN_MTU) {
				out32(ti814x->cpsw_regs + SL1_RX_MAXLEN,
				      ifp->if_mtu + ETHER_HDR_LEN +
				      ETHER_CRC_LEN + ETHER_VLAN_ENCAP_LEN);
			} else {
				out32(ti814x->cpsw_regs + SL1_RX_MAXLEN,
				      ifp->if_mtu + ETHER_HDR_LEN +
				      ETHER_CRC_LEN);
			}
			outle32 (ti814x->cpsw_regs + SL1_MAC_CONTROL, in32(ti814x->cpsw_regs+SL1_MAC_CONTROL)|GMII_EN);

			/* Setup MAC address */
			out32(ti814x->cpsw_regs + SL1_SA_LO,
				get_mac_lo(ti814x->cfg.current_address));
			out32(ti814x->cpsw_regs + SL1_SA_HI,
				get_mac_hi(ti814x->cfg.current_address));
			ti814x_ale_add_vlan_ucast(ti814x->cpsw_regs,
						  ti814x->cfg.current_address,
						  PORT1_UCAST_ENTRY,
						  0, 0, PORT1_VLAN);

			port_mask = PORT0|PORT1;
			break;
		case 1:
			/* init receive buffer offset and max length */
			if (ti814x->ecom.ec_capenable & ETHERCAP_VLAN_MTU) {
				out32(ti814x->cpsw_regs + SL2_RX_MAXLEN,
				      ifp->if_mtu + ETHER_HDR_LEN +
				      ETHER_CRC_LEN + ETHER_VLAN_ENCAP_LEN);
			} else {
				out32(ti814x->cpsw_regs + SL2_RX_MAXLEN,
				      ifp->if_mtu + ETHER_HDR_LEN +
				      ETHER_CRC_LEN);
			}
			outle32 (ti814x->cpsw_regs + SL2_MAC_CONTROL, in32(ti814x->cpsw_regs+SL2_MAC_CONTROL)|GMII_EN);

			/* Setup MAC address */
			out32(ti814x->cpsw_regs + SL2_SA_LO,
				get_mac_lo(ti814x->cfg.current_address));
			out32(ti814x->cpsw_regs + SL2_SA_HI,
				get_mac_hi(ti814x->cfg.current_address));
			ti814x_ale_add_vlan_ucast(ti814x->cpsw_regs,
						  ti814x->cfg.current_address,
						  PORT2_UCAST_ENTRY,
						  0, 0, PORT2_VLAN);

			port_mask = PORT0|PORT2;
			break;
		default:
			slogf (_SLOGC_NETWORK, _SLOG_ERROR,
				   "%s:%d: set slave MAC ADDR: invalid device index (%d)",
					__FUNCTION__, __LINE__, ti814x->cfg.device_index);
			return(-1);
	}
#else
	if(ti814x->ecom.ec_capenable & ETHERCAP_VLAN_MTU) {
		outle32 (ti814x->cpsw_regs + SL1_RX_MAXLEN,
			ifp->if_mtu + ETHER_HDR_LEN + ETHER_CRC_LEN + ETHER_VLAN_ENCAP_LEN);
		outle32 (ti814x->cpsw_regs + SL2_RX_MAXLEN,
			ifp->if_mtu + ETHER_HDR_LEN + ETHER_CRC_LEN + ETHER_VLAN_ENCAP_LEN);
	}
	else {
		outle32 (ti814x->cpsw_regs + SL1_RX_MAXLEN,
			ifp->if_mtu + ETHER_HDR_LEN + ETHER_CRC_LEN);
		outle32 (ti814x->cpsw_regs + SL2_RX_MAXLEN,
			ifp->if_mtu + ETHER_HDR_LEN + ETHER_CRC_LEN);
	}

	/* Sort out speed / duplex settings */
	value = RX_FLOW_EN | TX_FLOW_EN | GMII_EN;
	if (ti814x->duplex & 0xFFFF) {
	    value |= FULLDUPLEX;
	}
	if ((ti814x->speed & 0xFFFF) == 1000) {
		value |= GIG_MODE;
	}
	if ((ti814x->speed & 0xFFFF) == 100) {
		value |= MACIFCTL_A;
	}
	outle32 (ti814x->cpsw_regs + SL1_MAC_CONTROL, value);

	value = RX_FLOW_EN | TX_FLOW_EN | GMII_EN;
	if (ti814x->duplex >> 16) {
	    value |= FULLDUPLEX;
	}
	if ((ti814x->speed >> 16) == 1000) {
		value |= GIG_MODE;
	}
	if ((ti814x->speed >> 16) == 100) {
		value |= MACIFCTL_A;
	}
	outle32 (ti814x->cpsw_regs + SL2_MAC_CONTROL, value);

	out32(ti814x->cpsw_regs + SL1_SA_LO,
	      get_mac_lo(ti814x->cfg.current_address));
	out32(ti814x->cpsw_regs + SL1_SA_HI,
	      get_mac_hi(ti814x->cfg.current_address));
	out32(ti814x->cpsw_regs + SL2_SA_LO,
	      get_mac_lo(ti814x->cfg.current_address));
	out32(ti814x->cpsw_regs + SL2_SA_HI,
	      get_mac_hi(ti814x->cfg.current_address));

	port_mask = PORT0|PORT1|PORT2;
#endif

	/* Setup any VLANs */
	if (ti814x->join_vlan != NULL) {
	    loop = 0;
	    while (ti814x->join_vlan[loop] != 0) {
		ti814x_ale_add_vlan(ti814x->cpsw_regs,
				    ti814x->join_vlan[loop],
				    port_mask, -1);
		loop++;
	    }
	}
	if (ti814x->switch_vlan != NULL) {
	    loop = 0;
	    while (ti814x->switch_vlan[loop] != 0) {
		ti814x_ale_add_vlan(ti814x->cpsw_regs,
				    ti814x->switch_vlan[loop],
				    PORT1|PORT2, -1);
		loop++;
	    }
	}

	/* Unfortunately promiscuous affects both interfaces */
	value = inle32(ti814x->cpsw_regs + ALE_CONTROL);
#ifndef SWITCHMODE
	if (ifp->if_flags & IFF_PROMISC) {
	    value |= (ALE_BYPASS | AGE_OUT_NOW);
	} else {
	    value &= ~ALE_BYPASS;
	    value |= AGE_OUT_NOW;
	}
#else
	value |= AGE_OUT_NOW;
#endif
	outle32(ti814x->cpsw_regs + ALE_CONTROL, value);

	ti814x_filter(ti814x);

#ifndef SWITCHMODE
	/* Init the PHY */
	if (ti814x_initphy(ti814x)) {
		slogf (_SLOGC_NETWORK, _SLOG_DEBUG1, "PHY init failure");
		return (-1);
	}
	if (single) {
		if (!ti814x->cfg.device_index)
			out32 (ti814x->cpsw_regs + CPSW_STAT_PORT_EN, P1_STAT_EN);
		else
			out32 (ti814x->cpsw_regs + CPSW_STAT_PORT_EN, P2_STAT_EN);
		}
	else {
		out32 (ti814x->cpsw_regs + CPSW_STAT_PORT_EN, P1_STAT_EN);
		}
#endif

	/* Port 1 runs on link IRQ, but this will reap Tx as well */
	callout_msec (&ti814x->mii_callout, 3 * 1000,
		      ti814x_MDI_MonitorPhy, ti814x);

	ti814x->stop_miimon = 0;
	if (ti814x->cfg.device_index == 0) {
	    ti814x_enable_link_interrupt(&attach_args);
	}
	if (ti814x->cfg.device_index == 0 || single) {
	    ti814x_enable_stat_interrupt(&attach_args);
	}

	NW_SIGLOCK_P (&ifp->if_snd_ex, iopkt, wtp);
	ifp->if_flags_tx |= IFF_RUNNING;
	ifp->if_flags_tx &= ~IFF_OACTIVE;
	NW_SIGUNLOCK_P (&ifp->if_snd_ex, iopkt, wtp);
	ifp->if_flags |= IFF_RUNNING;

	return(EOK);
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/lib/io-pkt/sys/dev_qnx/dm814x/ti814x.c $ $Rev: 758089 $")
#endif
