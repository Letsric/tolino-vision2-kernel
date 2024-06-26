/*
* Customer code to add GPIO control during WLAN start/stop
* Copyright (C) 1999-2011, Broadcom Corporation
* 
*         Unless you and Broadcom execute a separate written software license
* agreement governing use of this software, this software is licensed to you
* under the terms of the GNU General Public License version 2 (the "GPL"),
* available at http://www.broadcom.com/licenses/GPLv2.php, with the
* following added to such license:
* 
*      As a special exception, the copyright holders of this software give you
* permission to link this software with independent modules, and to copy and
* distribute the resulting executable under terms of your choice, provided that
* you also meet, for each linked independent module, the terms and conditions of
* the license of that module.  An independent module is a module which is not
* derived from this software.  The special exception does not apply to any
* modifications of the software.
* 
*      Notwithstanding the above, under no circumstances may you combine this
* software in any way with any other Broadcom software provided under a license
* other than the GPL, without Broadcom's express prior written consent.
*
* $Id: dhd_custom_gpio.c,v 1.2.42.1 2010-10-19 00:41:09 Exp $
*/

#include <osl.h>

#ifdef CUSTOMER_HW

#include <mach/gpio.h>


extern int ntx_get_wifi_irq_gpio (void);
#ifdef CUSTOMER_OOB
int bcm_wlan_get_oob_irq(void)
{
	int host_oob_irq = 0;
	int host_irq_gpio = ntx_get_wifi_irq_gpio();

	gpio_request(host_irq_gpio, "oob irq");
	gpio_direction_input(host_irq_gpio);
	host_oob_irq = gpio_to_irq(host_irq_gpio);
	printk ("[%s-%d] gpio %d, irq %d\n",__func__,__LINE__, host_irq_gpio, host_oob_irq);

	return host_oob_irq;
}
#endif

extern void ntx_wifi_power_ctrl(int iIsWifiEnable);
extern void ntx_set_wifi_rst(bool val);

void bcm_wlan_power_on(int flag)
{
 if (2 == flag) { // do nothing for reset off
  printk ("[%s-%d] WLAN_RESET_ON\n",__func__,__LINE__);
  ntx_set_wifi_rst(1);
 }
 else
  ntx_wifi_power_ctrl(1);
}

void bcm_wlan_power_off(int flag)
{
 if (2 == flag) { // do nothing for reset off
  printk ("[%s-%d] WLAN_RESET_OFF\n",__func__,__LINE__);
  ntx_set_wifi_rst(0);
 }
 else
  ntx_wifi_power_ctrl(0);
}

#endif /* CUSTOMER_HW */

