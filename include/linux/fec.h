/* include/linux/fec.h
 *
 * Copyright (c) 2009 Orex Computed Radiography
 *   Baruch Siach <baruch@tkos.co.il>
 *
 * Copyright (C) 2010-2013 Freescale Semiconductor, Inc.
 *
 * Header file for the FEC platform data
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef __LINUX_FEC_H__
#define __LINUX_FEC_H__

#include <linux/phy.h>

struct fec_platform_data {
	int (*init) (struct phy_device *);
	int (*power_hibernate) (struct phy_device *);
	int (*suspend) (struct phy_device *);
	int (*resume) (struct phy_device *);
	phy_interface_t phy;
	unsigned char mac[ETH_ALEN];
	int gpio_irq;
};

#endif
