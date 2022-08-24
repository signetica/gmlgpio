/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2017 Tom Jones <tj@enoti.me>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

/*
 * Copyright (c) 2016 Mark Kettenis
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#define	GML_PADBAR				0x00C	    /* PAD Base Address */
#define GML_GPIO_PAD_CFG_DW0_GPIOTXSTATE	0x001
#define GML_GPIO_PAD_CFG_DW0_GPIORXSTATE	0x002
#define GML_GPIO_PAD_CFG_DW0_GPIOTXDIS		0x100
#define GML_GPIO_PAD_CFG_DW0_GPIORXDIS		0x200

#define	GML_GPIO_PAD_CFG_DW0_RXEVCFG		0x06000000 /* Level/Edge config mask */
#define	GML_GPIO_PAD_CFG_DW0_RXEVCFG_LEVEL	0x00000000 /* Level */
#define	GML_GPIO_PAD_CFG_DW0_RXEVCFG_EDGE	0x02000000 /* Edge */
#define	GML_GPIO_PAD_CFG_DW0_RXEVCFG_DRIVE_0	0x04000000 /* Drive '0' */
#define	GML_GPIO_PAD_CFG_DW0_RXEVCFG_RISE_FALL	0x06000000 /* Rising or Falling edge */

#define GML_GPIO_PAD_CFG_DW0_RXINV		0x00800000 /* RX invert */

/* Interrupt status offsets */
#define	GML_GPI_IS_0			0x100
#define	GML_GPI_IS_1			0x104
#define	GML_GPI_IS_2			0x108
#define	GML_GPI_IS_3			0x10C

/* Interrupt enable offsets */
#define	GML_GPI_IE_0			0x110
#define	GML_GPI_IE_1			0x114
#define	GML_GPI_IE_2			0x118
#define	GML_GPI_IE_3			0x11C

/* North community interrupt status and enable registers */
#define	GML_GPI_IS_NORTH_0	GML_GPI_IS_0	/* GPIO  76-107 */
#define	GML_GPI_IS_NORTH_1	GML_GPI_IS_1	/* GPIO 108-139 */
#define	GML_GPI_IS_NORTH_2	GML_GPI_IS_2	/* GPIO 140-155 */
#define	GML_GPI_IE_NORTH_0	GML_GPI_IE_0	/* GPIO  76-107 */
#define	GML_GPI_IE_NORTH_1	GML_GPI_IE_1	/* GPIO 108-139 */
#define	GML_GPI_IE_NORTH_2	GML_GPI_IE_2	/* GPIO 140-155 */

/* Northwest community interrupt status and enable registers */
#define	GML_GPI_IS_NORTHWEST_0	GML_GPI_IS_0	/* GPIO   0-31 */
#define	GML_GPI_IS_NORTHWEST_1	GML_GPI_IS_1	/* GPIO  32-63 */
#define	GML_GPI_IS_NORTHWEST_2	GML_GPI_IS_2	/* GPIO  64-75,211-214, vGPIO 0-15 */
#define	GML_GPI_IS_NORTHWEST_3	GML_GPI_IS_3	/* vGPIO 16-30 */
#define	GML_GPI_IE_NORTHWEST_0	GML_GPI_IE_0	/* GPIO   0-31 */
#define	GML_GPI_IE_NORTHWEST_1	GML_GPI_IE_1	/* GPIO  32-63 */
#define	GML_GPI_IE_NORTHWEST_2	GML_GPI_IE_2	/* GPIO  64-75,211-214, vGPIO 0-15 */
#define	GML_GPI_IE_NORTHWEST_3	GML_GPI_IE_3	/* vGPIO 16-30 */

/* SCC community interrupt status and enable registers */
#define	GML_GPI_IS_SCC_0	GML_GPI_IS_0	/* GPIO 176-206 */
#define	GML_GPI_IS_SCC_1	GML_GPI_IS_1	/* GPIO 207-209 */
#define	GML_GPI_IE_SCC_0	GML_GPI_IE_0	/* GPIO 176-206 */
#define	GML_GPI_IE_SCC_1	GML_GPI_IE_1	/* GPIO 207-209 */

/* Audio community interrupt status and enable registers */
#define	GML_GPI_IS_AUDIO_0	GML_GPI_IS_0	/* GPIO 156-175, vGPIO 31-38 */
#define	GML_GPI_IE_AUDIO_0	GML_GPI_IE_0	/* GPIO 156-175, vGPIO 31-38 */

/*
 * The pads for the pins are arranged in groups of maximal 32 pins.
 * The arrays below give the number of pins per group, such that we
 * can validate the pin numbers from ACPI.
 */
#define	NW_UID		1
#define	NW_BANK_PREFIX	"northwestbank"

const int gml_northwest_pins[] = {
	32, 32, 16, 31, -1
};

const char *gml_northwest_pin_names[] = {
	/* Pins 0 - 31 */
 	"TCK",				/* GPIO_0 */
	"TRST_B",			/* GPIO_1 */
	"TMS",				/* GPIO_2 */
	"TDI",				/* GPIO_3 */
	"TDO",				/* GPIO_4 */
	"JTAGX",			/* GPIO_5 */
	"CX_PREQ_B",			/* GPIO_6 */
	"CX_PRDY_B",			/* GPIO_7 */
	"GPIO_8",
	"GPIO_9",
	"GPIO_10",
	"GPIO_11",
	"GPIO_12",
	"GPIO_13",
	"GPIO_14",
	"GPIO_15",
	"GPIO_16",
	"GPIO_17",
	"GPIO_18",
	"GPIO_19",
	"GPIO_20",
	"GPIO_21",
	"GPIO_22",
	"GPIO_23",
	"GPIO_24",
	"GPIO_25",
	"GPIO_26",
	"GPIO_27",
	"GPIO_28",
	"GPIO_29",
	"GPIO_30",
	"GPIO_31",

	/* Pins 32 - 63 */
	"GPIO_32",
	"GPIO_33",
	"GPIO_34",
	"GPIO_35",
	"GPIO_36",
	"GPIO_37",
	"GPIO_38",
	"GPIO_39",
	"GPIO_40",
	"GPIO_41",
	"GP_INTD_DSI_TE1",		/* GPIO_42 */
	"GP_INTD_DSI_TE2",		/* GPIO_43 */
	"USB_OC0_B",			/* GPIO_44 */
	"USB_OC1_B",			/* GPIO_45 */
	"DSI_I2C_SDA",			/* GPIO_46 */
	"DSI_I2C_SCL",			/* GPIO_47 */
	"PMC_I2C_SDA",			/* GPIO_48 */
	"PMC_I2C_SCL",			/* GPIO_49 */
	"LPSS_I2C0_SDA",		/* GPIO_50 */
	"LPSS_I2C0_SCL",		/* GPIO_51 */
	"LPSS_I2C1_SDA",		/* GPIO_52 */
	"LPSS_I2C1_SCL",		/* GPIO_53 */
	"LPSS_I2C2_SDA",		/* GPIO_54 */
	"LPSS_I2C2_SCL",		/* GPIO_55 */
	"LPSS_I2C3_SDA",		/* GPIO_56 */
	"LPSS_I2C3_SCL",		/* GPIO_57 */
	"LPSS_I2C4_SDA",		/* GPIO_58 */
	"LPSS_I2C4_SCL",		/* GPIO_59 */
	"LPSS_UART0_RXD",		/* GPIO_60 */
	"LPSS_UART0_TXD",		/* GPIO_61 */
	"LPSS_UART0_RTX_B",		/* GPIO_62 */
	"LPSS_UART0_CTX_B",		/* GPIO_63 */

	/* Pins 64 - 79 */
	"LPSS_UART2_RXD",		/* GPIO_64 */
	"LPSS_UART2_TXD",		/* GPIO_65 */
	"LPSS_UART2_RTS_B",		/* GPIO_66 */
	"LPSS_UART2_CTS_B",		/* GPIO_67 */
	"PMC_SPI_FS0",			/* GPIO_68 */
	"PMC_SPI_FS1",			/* GPIO_69 */
	"PMC_SPI_FS2",			/* GPIO_70 */
	"PMC_SPI_RXD",			/* GPIO_71 */
	"PMC_SPI_TXD",			/* GPIO_72 */
	"PMC_SPI_CLK",			/* GPIO_73 */
	"THERMTRIP_B",			/* GPIO_74 */
	"PROCHOT_B",			/* GPIO_75 */
	"EMMC_RST_B",			/* GPIO_211 */
	"GPIO_212",
	"GPIO_213",
	"GPIO_214",

	/* Virtual GPIO, pins 80 - 110 */
	"vGPIO_0",
	"vGPIO_1",
	"vGPIO_2",
	"vGPIO_3",
	"vGPIO_4",
	"vGPIO_5",
	"vGPIO_6",
	"vGPIO_7",
	"vGPIO_8",
	"vGPIO_9",
	"vGPIO_10",
	"vGPIO_11",
	"vGPIO_12",
	"vGPIO_13",
	"vGPIO_14",
	"vGPIO_15",
	"vGPIO_16",
	"vGPIO_17",
	"vGPIO_18",
	"vGPIO_19",
	"vGPIO_20",
	"vGPIO_21",
	"vGPIO_22",
	"vGPIO_23",
	"vGPIO_24",
	"vGPIO_25",
	"vGPIO_26",
	"vGPIO_27",
	"vGPIO_28",
	"vGPIO_29",
	"vGPIO_30"
};

#define	N_UID		2
#define	N_BANK_PREFIX	"northbank"

const int gml_north_pins[] = {
	32, 32, 16, -1
};

const char *gml_north_pin_names[] = {
	/* Pins 0 - 31 */
	"SVID0_ALERT_B",		/* GPIO_76 */
	"SVID0_DATA",			/* GPIO_77 */
	"SVID0_CLK",			/* GPIO_78 */
	"LPSS_SPI_0_CLK",		/* GPIO_79 */
	"LPSS_SPI_0_FS0",		/* GPIO_80 */
	"LPSS_SPI_0_FS1",		/* GPIO_81 */
	"LPSS_SPI_0_RXD",		/* GPIO_82 */
	"LPSS_SPI_0_TXD",		/* GPIO_83 */
	"LPSS_SPI_2_CLK",		/* GPIO_84 */
	"LPSS_SPI_2_FS0",		/* GPIO_85 */
	"LPSS_SPI_2_FS1",		/* GPIO_86 */
	"LPSS_SPI_2_FS2",		/* GPIO_87 */
	"LPSS_SPI_2_RXD",		/* GPIO_88 */
	"LPSS_SPI_2_TXD",		/* GPIO_89 */
	"FST_SPI_CS0_B",		/* GPIO_90 */
	"FST_SPI_CS1_B",		/* GPIO_91 */
	"FST_SPI_MOSI_IO0",		/* GPIO_92 */
	"FST_SPI_MISO_IO1",		/* GPIO_93 */
	"FST_SPI_IO2",			/* GPIO_94 */
	"FST_SPI_IO3",			/* GPIO_95 */
	"FST_SPI_CLK",			/* GPIO_96 */
	"FST_SPI_CLK_FB",		/* GPIO_97 */
	"PMU_PLTRST_B",			/* GPIO_98 */
	"PMU_PWRBTN_B",			/* GPIO_99 */
	"PMU_SLP_S0_B",			/* GPIO_100 */
	"PMU_SLP_S3_B",			/* GPIO_101 */
	"PMU_SLP_S4_B",			/* GPIO_102 */
	"SUSPWRDNACK",			/* GPIO_103 */
	"EMMC_DNX_PWR_EN_B",		/* GPIO_104 */
	"GPIO_105",
	"PMU_BATLOW_B",			/* GPIO_106 */
	"PMU_RESETBUTTON_B",		/* GPIO_107 */

	/* Pins 32 - 63 */
	"PMU_SUSCLK",			/* GPIO_108 */
	"SUS_STAT_B",			/* GPIO_109 */
	"LPSS_I2C5_SDA",		/* GPIO_110 */
	"LPSS_I2C5_SCL",		/* GPIO_111 */
	"LPSS_I2C6_SDA",		/* GPIO_112 */
	"LPSS_I2C6_SCL",		/* GPIO_113 */
	"LPSS_I2C7_SDA",		/* GPIO_114 */
	"LPSS_I2C7_SCL",		/* GPIO_115 */
	"PCIE_WAKE0_B",			/* GPIO_116 */
	"PCIE_WAKE1_B",			/* GPIO_117 */
	"PCIE_WAKE2_B",			/* GPIO_118 */
	"PCIE_WAKE3_B",			/* GPIO_119 */
	"PCIE_CLK_REQ0_B",		/* GPIO_120 */
	"PCIE_CLI_REQ1_B",		/* GPIO_121 */
	"PCIE_CLK_REQ2_B",		/* GPIO_122 */
	"PCIE_CLK_REQ3_B",		/* GPIO_123 */
	"HV_DDI0_DDC_SDA",		/* GPIO_124 */
	"HV_DDI0_DDC_SCL",		/* GPIO_125 */
	"HV_DDI1_DDC_SDA",		/* GPIO_126 */
	"HV_DDI1_DDC_SCL",		/* GPIO_127 */
	"PANEL0_VDDEN",			/* GPIO_128 */
	"PANEL0_BKLTEN",		/* GPIO_129 */
	"PANEL0_BKLTCTL",		/* GPIO_130 */
	"HV_DDI0_HPD",			/* GPIO_131 */
	"HV_DDI1_HPD",			/* GPIO_132 */
	"HV_EDP_HPD",			/* GPIO_133 */
	"GPIO_134",
	"GPIO_135",
	"GPIO_136",
	"GPIO_137",
	"GPIO_138",
	"GPIO_139",

	/* Pins 64 - 79 */
	"GPIO_140",
	"GPIO_141",
	"GPIO_142",
	"GPIO_143",
	"GPIO_144",
	"GPIO_145",
	"GPIO_146",
	"LPC_ILB_SERIRQ",		/* GPIO_147 */
	"LPC_CLK_OUT0",			/* GPIO_148 */
	"LPC_CLK_OUT1",			/* GPIO_149 */
	"LPC_AD0",			/* GPIO_150 */
	"LPC_AD1",			/* GPIO_151 */
	"LPC_AD2",			/* GPIO_152 */
	"LPC_AD3",			/* GPIO_153 */
	"LPC_CLKRUNB",			/* GPIO_154 */
	"LPC_FRAMEB"			/* GPIO_155 */
};

#define	AUDIO_UID		3
#define	AUDIO_BANK_PREFIX	"audiobank"

const int gml_audio_pins[] = {
	20, 8, -1
};

const char *gml_audio_pin_names[] = {
	/* Pins 0 - 19 */
	"AVS_I2S0_MCLK",		/* GPIO_156 */
	"AVS_I2S0_BCLK",		/* GPIO_157 */
	"AVS_I2S0_WS_SYNC",		/* GPIO_158 */
	"AVS_I2S0_SDI",			/* GPIO_159 */
	"AVS_I2S0_SDO",			/* GPIO_160 */
	"AVS_I2S1_MCLK",		/* GPIO_161 */
	"AVS_I2S1_BCLK",		/* GPIO_162 */
	"AVS_I2S1_WS_SYNC",		/* GPIO_163 */
	"AVS_I2S1_SDI",			/* GPIO_164 */
	"AVS_I2S1_SDO",			/* GPIO_165 */
	"AVS_HDA_BCLK",			/* GPIO_166 */
	"AVS_HDA_WS_SYNC",		/* GPIO_167 */
	"AVS_HDA_SDI",			/* GPIO_168 */
	"AVS_HDA_SDO",			/* GPIO_169 */
	"AVS_HDA_RST_N",		/* GPIO_170 */
	"AVS_DMIC_CLK_A1",		/* GPIO_171 */
	"AVS_DMIC_CLK_B1",		/* GPIO_172 */
	"AVS_DMIC_DATA_1",		/* GPIO_173 */
	"AVS_DMIC_CLK_AB2",		/* GPIO_174 */
	"AVS_DMIC_DATA_2",		/* GPIO_175 */

	/* Virtual GPIO, pins 20 - 27 */
	"vGPIO_31",
	"vGPIO_32",
	"vGPIO_33",
	"vGPIO_34",
	"vGPIO_35",
	"vGPIO_36",
	"vGPIO_37",
	"vGPIO_38"
};

#define	SCC_UID		4
#define	SCC_BANK_PREFIX	"sccbank"

const int gml_scc_pins[] = {
	32, 3, -1
};

const char *gml_scc_pin_names[] = {
	/* Pins 0 - 31 */
	"SMB_ALERT_N",			/* GPIO_176 */
	"SMB_CLK",			/* GPIO_177 */
	"SMB_DATA",			/* GPIO_178 */
	"SDCARD_CLK",			/* GPIO_179 */
	"GPIO_180",
	"SDCARD_D0",			/* GPIO_181 */
	"SDCARD_D1",			/* GPIO_182 */
	"SDCARD_D2",			/* GPIO_183 */
	"SDCARD_D3",			/* GPIO_184 */
	"SDCARD_CMD",			/* GPIO_185 */
	"SDCARD_CD_N",			/* GPIO_186 */
	"SDCARD_LVL_WP",		/* GPIO_187 */
	"SDCARD_PWR_DWN_N",		/* GPIO_188 */
	"GPIO_210",
	"OSC_CLK_OUT_0",		/* GPIO_189 */
	"OSC_CLK_OUT_1",		/* GPIO_190 */
	"CNV_BRI_DT",			/* GPIO_191 */
	"CNV_BRI_RSP",			/* GPIO_192 */
	"CNV_RGI_DT",			/* GPIO_193 */
	"CNV_RGI_RSP",			/* GPIO_194 */
	"CNV_RF_RESET_N",		/* GPIO_195 */
	"XTAL_CLKREQ",			/* GPIO_196 */
	"GPIO_197",
	"EMMC_CLK",			/* GPIO_198 */
	"GPIO_199",
	"EMMC_D0",			/* GPIO_200 */
	"EMMC_D1",			/* GPIO_201 */
	"EMMC_D2",			/* GPIO_202 */
	"EMMC_D3",			/* GPIO_203 */
	"EMMC_D4",			/* GPIO_204 */
	"EMMC_D5",			/* GPIO_205 */
	"EMMC_D6",			/* GPIO_206 */

	/* Pins 32 - 34 */
	"EMMC_D7",			/* GPIO_207 */
	"EMMC_CMD",			/* GPIO_208 */
	"EMMC_RCLK"			/* GPIO_209 */
};
