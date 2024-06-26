/*
 * Copyright (C) 2012-2013 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _BOARD_MX6DL_NTX_H
#define _BOARD_MX6DL_NTX_H
#include <mach/iomux-mx6dl.h>

#define	MX6DL_SD4_CLK			IMX_GPIO_NR(7, 10)	/* SD4_CLK	 */
#define	MX6DL_SD4_CMD			IMX_GPIO_NR(7, 9)	/* SD4_CMD	 */
#define	MX6DL_SD4_DAT0			IMX_GPIO_NR(2, 8)	/* SD4_DAT0	 */
#define	MX6DL_SD4_DAT1			IMX_GPIO_NR(2, 9)	/* SD4_DAT1	 */
#define	MX6DL_SD4_DAT2			IMX_GPIO_NR(2, 10) 	/* SD4_DAT2	 */
#define	MX6DL_SD4_DAT3			IMX_GPIO_NR(2, 11)	/* SD4_DAT3	 */
#define MX6DL_WIFI_3V3			IMX_GPIO_NR(2, 13)

static iomux_v3_cfg_t mx6dl_ntx_pads[] = {
	/* AUDMUX */

	/* KEY */
	MX6DL_PAD_KEY_COL0__GPIO_4_6,
	MX6DL_PAD_KEY_ROW0__GPIO_4_7,
	MX6DL_PAD_KEY_COL1__GPIO_4_8,
	MX6DL_PAD_KEY_ROW1__GPIO_4_9,
	MX6DL_PAD_KEY_COL2__GPIO_4_10,	/* PMU_INT */
	MX6DL_PAD_KEY_ROW2__GPIO_4_11,
	MX6DL_PAD_KEY_COL4__GPIO_4_14,
	MX6DL_PAD_KEY_ROW4__GPIO_4_15,

	/* ENET */

	/* I2C1 */
	MX6DL_PAD_CSI0_DAT8__I2C1_SDA,
	MX6DL_PAD_CSI0_DAT9__I2C1_SCL,

	/* I2C2 */
	MX6DL_PAD_KEY_COL3__I2C2_SCL,
	MX6DL_PAD_KEY_ROW3__I2C2_SDA,

	/* I2C3 */
	MX6DL_PAD_GPIO_5__I2C3_SCL,
	MX6DL_PAD_GPIO_6__I2C3_SDA,

	/* DISPLAY */
	/* FlexCAN */
	/* CAN1_STBY */

	/* UART1 for debug */
	MX6DL_PAD_CSI0_DAT10__UART1_TXD,
	MX6DL_PAD_CSI0_DAT11__UART1_RXD,

	/* USB */

	/* USDHC1 */
	MX6DL_PAD_SD1_CLK__USDHC1_CLK,
	MX6DL_PAD_SD1_CMD__USDHC1_CMD,
	MX6DL_PAD_SD1_DAT0__USDHC1_DAT0,
	MX6DL_PAD_SD1_DAT1__USDHC1_DAT1,
	MX6DL_PAD_SD1_DAT2__USDHC1_DAT2,
	MX6DL_PAD_SD1_DAT3__USDHC1_DAT3,

	/* USDHC3 */
	MX6DL_PAD_SD3_CLK__USDHC3_CLK_50MHZ,
	MX6DL_PAD_SD3_CMD__USDHC3_CMD_50MHZ,
	MX6DL_PAD_SD3_DAT0__USDHC3_DAT0_50MHZ,
	MX6DL_PAD_SD3_DAT1__USDHC3_DAT1_50MHZ,
	MX6DL_PAD_SD3_DAT2__USDHC3_DAT2_50MHZ,
	MX6DL_PAD_SD3_DAT3__USDHC3_DAT3_50MHZ,
	MX6DL_PAD_SD3_DAT4__USDHC3_DAT4_50MHZ,
	MX6DL_PAD_SD3_DAT5__USDHC3_DAT5_50MHZ,
	MX6DL_PAD_SD3_DAT6__USDHC3_DAT6_50MHZ,
	MX6DL_PAD_SD3_DAT7__USDHC3_DAT7_50MHZ,

	/* USDHC4 */
	MX6DL_PAD_SD4_CLK__USDHC4_CLK_50MHZ,
	MX6DL_PAD_SD4_CMD__USDHC4_CMD_50MHZ,
	MX6DL_PAD_SD4_DAT0__USDHC4_DAT0_50MHZ,
	MX6DL_PAD_SD4_DAT1__USDHC4_DAT1_50MHZ,
	MX6DL_PAD_SD4_DAT2__USDHC4_DAT2_50MHZ,
	MX6DL_PAD_SD4_DAT3__USDHC4_DAT3_50MHZ,
	MX6DL_PAD_SD4_DAT4__GPIO_2_12,
	MX6DL_PAD_SD4_DAT5__GPIO_2_13,
	MX6DL_PAD_SD4_DAT6__GPIO_2_14,
	MX6DL_PAD_SD4_DAT7__GPIO_2_15,

	MX6DL_PAD_SD2_DAT2__GPIO_1_13,	/* ACT_LED */
	MX6DL_PAD_SD2_DAT1__GPIO_1_14,	/* TP_INT */
	MX6DL_PAD_SD2_DAT0__GPIO_1_15,	/* TP_RST */
	MX6DL_PAD_SD3_RST__GPIO_7_8,

	MX6DL_PAD_GPIO_1__GPIO_1_1,
	MX6DL_PAD_GPIO_2__GPIO_1_2,
	MX6DL_PAD_GPIO_4__GPIO_1_4,

	MX6DL_PAD_NANDF_D4__GPIO_2_4,
	MX6DL_PAD_NANDF_D5__GPIO_2_5,
	MX6DL_PAD_NANDF_D6__GPIO_2_6,
	MX6DL_PAD_NANDF_D7__GPIO_2_7,
    MX6DL_PAD_KEY_COL1__GPIO_4_8,

	/* UART 4 */
	MX6DL_PAD_CSI0_DAT12__UART4_TXD,
	MX6DL_PAD_CSI0_DAT13__UART4_RXD,
	MX6DL_PAD_CSI0_DAT16__UART4_RTS,
	MX6DL_PAD_CSI0_DAT17__UART4_CTS,

    MX6DL_PAD_DISP0_DAT18__GPIO_5_12,
    MX6DL_PAD_DISP0_DAT19__GPIO_5_13,
    MX6DL_PAD_DISP0_DAT21__GPIO_5_15,
    MX6DL_PAD_DISP0_DAT23__GPIO_5_17,
    MX6DL_PAD_NANDF_ALE__GPIO_6_8
};

static iomux_v3_cfg_t mx6dl_ntx_epdc_enable_pads[] = {
	/* EPDC */
	MX6DL_PAD_EIM_A16__EPDC_SDDO_0,
	MX6DL_PAD_EIM_DA10__EPDC_SDDO_1,
	MX6DL_PAD_EIM_DA12__EPDC_SDDO_2,
	MX6DL_PAD_EIM_DA11__EPDC_SDDO_3,
	MX6DL_PAD_EIM_LBA__EPDC_SDDO_4,
	MX6DL_PAD_EIM_EB2__EPDC_SDDO_5,
	MX6DL_PAD_EIM_CS0__EPDC_SDDO_6,
	MX6DL_PAD_EIM_RW__EPDC_SDDO_7,
	MX6DL_PAD_EIM_A21__EPDC_GDCLK,
	MX6DL_PAD_EIM_A22__EPDC_GDSP,
	MX6DL_PAD_EIM_A23__EPDC_GDOE,
	MX6DL_PAD_EIM_A24__EPDC_GDRL,
	MX6DL_PAD_EIM_D31__EPDC_SDCLK,
	MX6DL_PAD_EIM_D27__EPDC_SDOE,
	MX6DL_PAD_EIM_DA1__EPDC_SDLE,
	MX6DL_PAD_EIM_EB1__EPDC_SDSHR,
	MX6DL_PAD_EIM_DA2__EPDC_BDR_0,
	MX6DL_PAD_EIM_DA4__EPDC_SDCE_0,
	MX6DL_PAD_EIM_DA5__EPDC_SDCE_1,
//	MX6DL_PAD_EIM_DA6__EPDC_SDCE_2,

	/* EPD PMIC (Maxim 17135) pins */
//	MX6DL_PAD_EIM_A17__GPIO_2_21, /* EPDC_PWRSTAT */
//	MX6DL_PAD_EIM_D17__GPIO_3_17, /* EPDC_VCOM0 */
//	MX6DL_PAD_EIM_A18__GPIO_2_20, /* EPDC_PWRCTRL0 */
//	MX6DL_PAD_EIM_D20__GPIO_3_20, /* EPDC_PMIC_WAKEUP */
	/*
	 * Depopulate R121, R123, R133, R138, R139,
	 * R167, R168, and R627 when using E-BOOK
	 * Card in 16-bit Data Mode. Meanwhile, comments
	 * the conflict PIN configurations in above tables
	 */
	
	MX6DL_PAD_EIM_CS1__EPDC_SDDO_8,
	MX6DL_PAD_EIM_DA15__EPDC_SDDO_9,
	MX6DL_PAD_EIM_D16__EPDC_SDDO_10,
	MX6DL_PAD_EIM_D23__EPDC_SDDO_11,
	MX6DL_PAD_EIM_D19__EPDC_SDDO_12,
	MX6DL_PAD_EIM_DA13__EPDC_SDDO_13,
	MX6DL_PAD_EIM_DA14__EPDC_SDDO_14,
	MX6DL_PAD_EIM_A25__EPDC_SDDO_15,
	
};

static iomux_v3_cfg_t mx6dl_ntx_epdc_disable_pads[] = {
	/* EPDC */
	MX6DL_PAD_EIM_A16__GPIO_2_22, // EPDC_SDO_0
	MX6DL_PAD_EIM_DA10__GPIO_3_10, // EPDC_SDO_1
	MX6DL_PAD_EIM_DA12__GPIO_3_12, // EPDC_SDO_2
	MX6DL_PAD_EIM_DA11__GPIO_3_11, // EPDC_SDO_3
	MX6DL_PAD_EIM_LBA__GPIO_2_27, // EPDC_SDO_4
	MX6DL_PAD_EIM_EB2__GPIO_2_30, // EPDC_SDO_5
	MX6DL_PAD_EIM_CS0__GPIO_2_23, // EPDC_SDO_6
	MX6DL_PAD_EIM_RW__GPIO_2_26, // EPDC_SDO_7

	MX6DL_PAD_EIM_A21__GPIO_2_17, // EPDC_GDCLK 
	MX6DL_PAD_EIM_A22__GPIO_2_16, // EPDC_GDSP 
	MX6DL_PAD_EIM_A23__GPIO_6_6, // EPDC_GDOE 
	MX6DL_PAD_EIM_A24__GPIO_5_4, // EPDC_GDRL
	MX6DL_PAD_EIM_D31__GPIO_3_31, // EPDC_SDCLK
	MX6DL_PAD_EIM_D27__GPIO_3_27, // EPDC_SDOE 
	MX6DL_PAD_EIM_DA1__GPIO_3_1, // EPDC_SDLE
	MX6DL_PAD_EIM_EB1__GPIO_2_29, // EPDC_SDSHR
	MX6DL_PAD_EIM_DA2__GPIO_3_2, // EPDC_BDR_0
	MX6DL_PAD_EIM_DA4__GPIO_3_4, // EPDC_SDCE_0
	MX6DL_PAD_EIM_DA5__GPIO_3_5, // EPDC_SDCE_1
//	MX6DL_PAD_EIM_DA6__GPIO_3_6, // EPDC_SDCE_2

	/*
	 * Depopulate R121, R123, R133, R138, R139,
	 * R167, R168, and R627 when using E-BOOK
	 * Card in 16-bit Data Mode. Meanwhile, comments
	 * the conflict PIN configurations in above tables
	 */

	MX6DL_PAD_EIM_CS1__GPIO_2_24, // EPDC_SDO_8
	MX6DL_PAD_EIM_DA15__GPIO_3_15, // EPDC_SDO_9
	MX6DL_PAD_EIM_D16__GPIO_3_16, // EPDC_SDO_10
	MX6DL_PAD_EIM_D23__GPIO_3_23, // EPDC_SDO_11
	MX6DL_PAD_EIM_D19__GPIO_3_19, // EPDC_SDO_12
	MX6DL_PAD_EIM_DA13__GPIO_3_13, // EPDC_SDO_13
	MX6DL_PAD_EIM_DA14__GPIO_3_14, // EPDC_SDO_14
	MX6DL_PAD_EIM_A25__GPIO_5_2, // EPDC_SDO_15
	
};

static iomux_v3_cfg_t mx6dl_ntx_sd4_wifi_pads[] = {
	MX6DL_PAD_SD4_CLK__USDHC4_CLK_50MHZ,
	MX6DL_PAD_SD4_CMD__USDHC4_CMD_50MHZ,
	MX6DL_PAD_SD4_DAT0__USDHC4_DAT0_50MHZ,
	MX6DL_PAD_SD4_DAT1__USDHC4_DAT1_50MHZ,
	MX6DL_PAD_SD4_DAT2__USDHC4_DAT2_50MHZ,
	MX6DL_PAD_SD4_DAT3__USDHC4_DAT3_50MHZ,
};
static iomux_v3_cfg_t mx6dl_ntx_sd4_gpio_pads[] = {
	MX6DL_PAD_SD4_CLK__GPIO_7_10,
	MX6DL_PAD_SD4_CMD__GPIO_7_9,
	MX6DL_PAD_SD4_DAT0__GPIO_2_8,
	MX6DL_PAD_SD4_DAT1__GPIO_2_9,
	MX6DL_PAD_SD4_DAT2__GPIO_2_10,
	MX6DL_PAD_SD4_DAT3__GPIO_2_11,
};

static iomux_v3_cfg_t mx6dl_ntx_uart5_pads[] = {
	MX6DL_PAD_CSI0_DAT14__UART5_TXD,
	MX6DL_PAD_CSI0_DAT15__UART5_RXD,
	MX6DL_PAD_CSI0_DAT19__UART5_CTS,
	MX6DL_PAD_CSI0_DAT18__UART5_RTS,
};

static iomux_v3_cfg_t mx6dl_ntx_uart5_gpio_pads[] = {
	MX6DL_PAD_CSI0_DAT14__GPIO_6_0,
	MX6DL_PAD_CSI0_DAT15__GPIO_6_1,
	MX6DL_PAD_CSI0_DAT19__GPIO_6_5,
	MX6DL_PAD_CSI0_DAT18__GPIO_6_4,
};
#endif
