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


#ifndef __ARM_MX6SX_IOMUX_H_INCLUDED
#define __ARM_MX6SX_IOMUX_H_INCLUDED


/*
 * Bit definitions for SW_MUX_CTL registers
 */
#define MUX_CTL_SION                        (0x1 << 4)
#define MUX_CTL_MUX_MODE_ALT0                0
#define MUX_CTL_MUX_MODE_ALT1                1
#define MUX_CTL_MUX_MODE_ALT2                2
#define MUX_CTL_MUX_MODE_ALT3                3
#define MUX_CTL_MUX_MODE_ALT4                4
#define MUX_CTL_MUX_MODE_ALT5                5
#define MUX_CTL_MUX_MODE_ALT6                6
#define MUX_CTL_MUX_MODE_ALT7                7


/*
 * Bit definitions for SW_PAD_CTL registers
 */

#define PAD_CTL_HYS_DISABLE             (0x0 << 16)
#define PAD_CTL_HYS_ENABLE              (0x1 << 16)

#define PAD_CTL_PUS_100K_PD             (0x0 << 14)
#define PAD_CTL_PUS_47K_PU              (0x1 << 14)
#define PAD_CTL_PUS_100K_PU             (0x2 << 14)
#define PAD_CTL_PUS_22K_PU              (0x3 << 14)

#define PAD_CTL_PUE_PULL                (0x1 << 13)
#define PAD_CTL_PUE_KEEP                (0x0 << 13)

#define PAD_CTL_PKE_DISABLE             (0x0 << 12)
#define PAD_CTL_PKE_ENABLE              (0x1 << 12)

#define PAD_CTL_ODE_DISABLE             (0x0 << 11)
#define PAD_CTL_ODE_ENABLE              (0x1 << 11)

#define PAD_CTL_ODT
#define PAD_CTL_SPEED_TBD               (0x0 << 6)
#define PAD_CTL_SPEED_LOW               (0x1 << 6)
#define PAD_CTL_SPEED_MEDIUM            (0x2 << 6)
#define PAD_CTL_SPEED_MAX               (0x3 << 6)

#define PAD_CTL_DSE_0                   (0x0 << 3)
#define PAD_CTL_DSE_1                   (0x1 << 3)
#define PAD_CTL_DSE_2                   (0x2 << 3)
#define PAD_CTL_DSE_3                   (0x3 << 3)
#define PAD_CTL_DSE_4                   (0x4 << 3)
#define PAD_CTL_DSE_5                   (0x5 << 3)
#define PAD_CTL_DSE_6                   (0x6 << 3)
#define PAD_CTL_DSE_7                   (0x7 << 3)

/* 3.3V NVCC_GPIO GPIO DSE Names */
#define PAD_CTL_DSE_HIZ                 PAD_CTL_DSE_0
#define PAD_CTL_DSE_260_OHM             PAD_CTL_DSE_1
#define PAD_CTL_DSE_130_OHM             PAD_CTL_DSE_2
#define PAD_CTL_DSE_87_OHM              PAD_CTL_DSE_3
#define PAD_CTL_DSE_65_OHM              PAD_CTL_DSE_4
#define PAD_CTL_DSE_52_OHM              PAD_CTL_DSE_5
#define PAD_CTL_DSE_43_OHM              PAD_CTL_DSE_6
#define PAD_CTL_DSE_37_OHM              PAD_CTL_DSE_7

/* 1.8V NVCC_GPIO GPIO DSE */
#define PAD_CTL_DSE_1V8_150_OHM         PAD_CTL_DSE_1
#define PAD_CTL_DSE_1V8_75_OHM          PAD_CTL_DSE_2
#define PAD_CTL_DSE_1V8_50_OHM          PAD_CTL_DSE_3
#define PAD_CTL_DSE_1V8_38_OHM          PAD_CTL_DSE_4
#define PAD_CTL_DSE_1V8_30_OHM          PAD_CTL_DSE_5
#define PAD_CTL_DSE_1V8_25_OHM          PAD_CTL_DSE_6
#define PAD_CTL_DSE_1V8_21_OHM          PAD_CTL_DSE_7

/* DDR GPIO DSE */
#define PAD_CTL_DSE_DDR_240_OHM         PAD_CTL_DSE_1
#define PAD_CTL_DSE_DDR_120_OHM         PAD_CTL_DSE_2
#define PAD_CTL_DSE_DDR_80_OHM          PAD_CTL_DSE_3
#define PAD_CTL_DSE_DDR_60_OHM          PAD_CTL_DSE_4
#define PAD_CTL_DSE_DDR_48_OHM          PAD_CTL_DSE_5
#define PAD_CTL_DSE_DDR_40_OHM          PAD_CTL_DSE_6
#define PAD_CTL_DSE_DDR_34_OHM          PAD_CTL_DSE_7

#define PAD_CTL_SRE_SLOW                (0x0 << 0)
#define PAD_CTL_SRE_FAST                (0x1 << 0)

/*
 * offsets of IOMUXC registers from MX6SX_IOMUX_SWMUX
 * where MX6SX_IOMUX_SWMUX = MX6SX_IOMUXC_BASE + 0x0014
 */

#define SWMUX_GPIO1_0                    0
#define SWMUX_GPIO1_1                    1
#define SWMUX_GPIO1_2                    2
#define SWMUX_GPIO1_3                    3
#define SWMUX_GPIO1_4                    4
#define SWMUX_GPIO1_5                    5
#define SWMUX_GPIO1_6                    6
#define SWMUX_GPIO1_7                    7
#define SWMUX_GPIO1_8                    8
#define SWMUX_GPIO1_9                    9
#define SWMUX_GPIO1_10                  10
#define SWMUX_GPIO1_11                  11
#define SWMUX_GPIO1_12                  12
#define SWMUX_GPIO1_13                  13
#define SWMUX_CSI_DATA00                14
#define SWMUX_CSI_DATA01                15
#define SWMUX_CSI_DATA02                16
#define SWMUX_CSI_DATA03                17
#define SWMUX_CSI_DATA04                18
#define SWMUX_CSI_DATA05                19
#define SWMUX_CSI_DATA06                20
#define SWMUX_CSI_DATA07                21
#define SWMUX_CSI_HSYNC                 22
#define SWMUX_CSI_MCLK                  23
#define SWMUX_CSI_PIXCLK                24
#define SWMUX_CSI_VSYNC                 25
#define SWMUX_ENET1_COL                 26
#define SWMUX_ENET1_CRS                 27
#define SWMUX_ENET1_MDC                 28
#define SWMUX_ENET1_MDIO                29
#define SWMUX_ENET1_RX_CLK              30
#define SWMUX_ENET1_TX_CLK              31
#define SWMUX_ENET2_COL                 32
#define SWMUX_ENET2_CRS                 33
#define SWMUX_ENET2_RX_CLK              34
#define SWMUX_ENET2_TX_CLK              35
#define SWMUX_KEY_COL0                  36
#define SWMUX_KEY_COL1                  37
#define SWMUX_KEY_COL2                  38
#define SWMUX_KEY_COL3                  39
#define SWMUX_KEY_COL4                  40
#define SWMUX_KEY_ROW0                  41
#define SWMUX_KEY_ROW1                  42
#define SWMUX_KEY_ROW2                  43
#define SWMUX_KEY_ROW3                  44
#define SWMUX_KEY_ROW4                  45
#define SWMUX_LCD1_CLK                  46
#define SWMUX_LCD1_DATA00               47
#define SWMUX_LCD1_DATA01               48
#define SWMUX_LCD1_DATA02               49
#define SWMUX_LCD1_DATA03               50
#define SWMUX_LCD1_DATA04               51
#define SWMUX_LCD1_DATA05               52
#define SWMUX_LCD1_DATA06               53
#define SWMUX_LCD1_DATA07               54
#define SWMUX_LCD1_DATA08               55
#define SWMUX_LCD1_DATA09               56
#define SWMUX_LCD1_DATA10               57
#define SWMUX_LCD1_DATA11               58
#define SWMUX_LCD1_DATA12               59
#define SWMUX_LCD1_DATA13               60
#define SWMUX_LCD1_DATA14               61
#define SWMUX_LCD1_DATA15               62
#define SWMUX_LCD1_DATA16               63
#define SWMUX_LCD1_DATA17               64
#define SWMUX_LCD1_DATA18               65
#define SWMUX_LCD1_DATA19               66
#define SWMUX_LCD1_DATA20               67
#define SWMUX_LCD1_DATA21               68
#define SWMUX_LCD1_DATA22               69
#define SWMUX_LCD1_DATA23               70
#define SWMUX_LCD1_ENABLE               71
#define SWMUX_LCD1_HSYNC                72
#define SWMUX_LCD1_RESET                73
#define SWMUX_LCD1_VSYNC                74
#define SWMUX_NAND_ALE                  75
#define SWMUX_NAND_CE0_B                76
#define SWMUX_NAND_CE1_B                77
#define SWMUX_NAND_CLE                  78
#define SWMUX_NAND_DATA00               79
#define SWMUX_NAND_DATA01               80
#define SWMUX_NAND_DATA02               81
#define SWMUX_NAND_DATA03               82
#define SWMUX_NAND_DATA04               83
#define SWMUX_NAND_DATA05               84
#define SWMUX_NAND_DATA06               85
#define SWMUX_NAND_DATA07               86
#define SWMUX_NAND_RE_B                 87
#define SWMUX_NAND_READY_B              88
#define SWMUX_NAND_WE_B                 89
#define SWMUX_NAND_WP_B                 90
#define SWMUX_QSPI1A_DATA0              91
#define SWMUX_QSPI1A_DATA1              92
#define SWMUX_QSPI1A_DATA2              93
#define SWMUX_QSPI1A_DATA3              94
#define SWMUX_QSPI1A_DQS                95
#define SWMUX_QSPI1A_SCLK               96
#define SWMUX_QSPI1A_SS0_B              97
#define SWMUX_QSPI1A_SS1_B              98
#define SWMUX_QSPI1B_DATA0              99
#define SWMUX_QSPI1B_DATA1             100
#define SWMUX_QSPI1B_DATA2             101
#define SWMUX_QSPI1B_DATA3             102
#define SWMUX_QSPI1B_DQS               103
#define SWMUX_QSPI1B_SCLK              104
#define SWMUX_QSPI1B_SS0_B             105
#define SWMUX_QSPI1B_SS1_B             106
#define SWMUX_RGMII1_RD0               107
#define SWMUX_RGMII1_RD1               108
#define SWMUX_RGMII1_RD2               109
#define SWMUX_RGMII1_RD3               110
#define SWMUX_RGMII1_RX_CTL            111
#define SWMUX_RGMII1_RXC               112
#define SWMUX_RGMII1_TD0               113
#define SWMUX_RGMII1_TD1               114
#define SWMUX_RGMII1_TD2               115
#define SWMUX_RGMII1_TD3               116
#define SWMUX_RGMII1_TX_CTL            117
#define SWMUX_RGMII1_TXC               118
#define SWMUX_RGMII2_RD0               119
#define SWMUX_RGMII2_RD1               120
#define SWMUX_RGMII2_RD2               121
#define SWMUX_RGMII2_RD3               122
#define SWMUX_RGMII2_RX_CTL            123
#define SWMUX_RGMII2_RXC               124
#define SWMUX_RGMII2_TD0               125
#define SWMUX_RGMII2_TD1               126
#define SWMUX_RGMII2_TD2               127
#define SWMUX_RGMII2_TD3               128
#define SWMUX_RGMII2_TX_CTL            129
#define SWMUX_RGMII2_TXC               130
#define SWMUX_SD1_CLK                  131
#define SWMUX_SD1_CMD                  132
#define SWMUX_SD1_DATA0                133
#define SWMUX_SD1_DATA1                134
#define SWMUX_SD1_DATA2                135
#define SWMUX_SD1_DATA3                136
#define SWMUX_SD2_CLK                  137
#define SWMUX_SD2_CMD                  138
#define SWMUX_SD2_DATA0                139
#define SWMUX_SD2_DATA1                140
#define SWMUX_SD2_DATA2                141
#define SWMUX_SD2_DATA3                142
#define SWMUX_SD3_CLK                  143
#define SWMUX_SD3_CMD                  144
#define SWMUX_SD3_DATA0                145
#define SWMUX_SD3_DATA1                146
#define SWMUX_SD3_DATA2                147
#define SWMUX_SD3_DATA3                148
#define SWMUX_SD3_DATA4                149
#define SWMUX_SD3_DATA5                150
#define SWMUX_SD3_DATA6                151
#define SWMUX_SD3_DATA7                152
#define SWMUX_SD4_CLK                  153
#define SWMUX_SD4_CMD                  154
#define SWMUX_SD4_DATA0                155
#define SWMUX_SD4_DATA1                156
#define SWMUX_SD4_DATA2                157
#define SWMUX_SD4_DATA3                158
#define SWMUX_SD4_DATA4                159
#define SWMUX_SD4_DATA5                160
#define SWMUX_SD4_DATA6                161
#define SWMUX_SD4_DATA7                162
#define SWMUX_SD4_RESET_B              163
#define SWMUX_USB_H_DATA               164
#define SWMUX_USB_H_STROBE             165


#define SWMUX_QSPI2A_SS0_B              75  /* IOMUXC_SW_MUX_CTL_PAD_NAND_ALE */
#define SWMUX_QSPI2A_DATA2              76  /* IOMUXC_SW_MUX_CTL_PAD_NAND_CE0_B */
#define SWMUX_QSPI2A_DATA3              77  /* IOMUXC_SW_MUX_CTL_PAD_NAND_CE1_B */
#define SWMUX_QSPI2A_SCLK               78  /* IOMUXC_SW_MUX_CTL_PAD_NAND_CLE */
#define SWMUX_QSPI2B_DATA1              79  /* IOMUXC_SW_MUX_CTL_PAD_NAND_DATA00 */
#define SWMUX_QSPI2B_DATA0              80  /* IOMUXC_SW_MUX_CTL_PAD_NAND_DATA01 */
#define SWMUX_QSPI2B_SCLK               81  /* IOMUXC_SW_MUX_CTL_PAD_NAND_DATA02 */
#define SWMUX_QSPI2B_SS0_B              82  /* IOMUXC_SW_MUX_CTL_PAD_NAND_DATA03 */
#define SWMUX_QSPI2B_SS1_B              83  /* IOMUXC_SW_MUX_CTL_PAD_NAND_DATA04 */
#define SWMUX_QSPI2B_DQS                84  /* IOMUXC_SW_MUX_CTL_PAD_NAND_DATA05 */
#define SWMUX_QSPI2A_SS1_B              85  /* IOMUXC_SW_MUX_CTL_PAD_NAND_DATA06 */
#define SWMUX_QSPI2A_DQS                86  /* IOMUXC_SW_MUX_CTL_PAD_NAND_DATA07 */
#define SWMUX_QSPI2B_DATA3              87  /* IOMUXC_SW_MUX_CTL_PAD_NAND_RE_B */
#define SWMUX_QSPI2A_DATA1              88  /* IOMUXC_SW_MUX_CTL_PAD_NAND_READY_B */
#define SWMUX_QSPI2B_DATA2              89  /* IOMUXC_SW_MUX_CTL_PAD_NAND_WE_B */
#define SWMUX_QSPI2A_DATA0              90  /* IOMUXC_SW_MUX_CTL_PAD_NAND_WP_B */


/*
 * offsets of IOMUXC registers from MX6SX_IOMUX_SWPAD
 * where MX6SX_IOMUX_SWPAD = MX6SX_IOMUXC_BASE + 0x02AC
 */

#define SWPAD_DRAM_ADDR00                0
#define SWPAD_DRAM_ADDR01                1
#define SWPAD_DRAM_ADDR02                2
#define SWPAD_DRAM_ADDR03                3
#define SWPAD_DRAM_ADDR04                4
#define SWPAD_DRAM_ADDR05                5
#define SWPAD_DRAM_ADDR06                6
#define SWPAD_DRAM_ADDR07                7
#define SWPAD_DRAM_ADDR08                8
#define SWPAD_DRAM_ADDR09                9
#define SWPAD_DRAM_ADDR10               10
#define SWPAD_DRAM_ADDR11               11
#define SWPAD_DRAM_ADDR12               12
#define SWPAD_DRAM_ADDR13               13
#define SWPAD_DRAM_ADDR14               14
#define SWPAD_DRAM_ADDR15               15
#define SWPAD_DRAM_DQM0                 16
#define SWPAD_DRAM_DQM1                 17
#define SWPAD_DRAM_DQM2                 18
#define SWPAD_DRAM_DQM3                 19
#define SWPAD_DRAM_RAS_B                20
#define SWPAD_DRAM_CAS_B                21
#define SWPAD_DRAM_CS0_B                22
#define SWPAD_DRAM_CS1_B                23
#define SWPAD_DRAM_SDWE_B               24
#define SWPAD_DRAM_ODT0                 25
#define SWPAD_DRAM_ODT1                 26
#define SWPAD_DRAM_SDBA0                27
#define SWPAD_DRAM_SDBA1                28
#define SWPAD_DRAM_SDBA2                29
#define SWPAD_DRAM_SDCKE0               30
#define SWPAD_DRAM_SDCKE1               31
#define SWPAD_DRAM_SDCLK0_P             32
#define SWPAD_DRAM_SDQS0_P              33
#define SWPAD_DRAM_SDQS1_P              34
#define SWPAD_DRAM_SDQS2_P              35
#define SWPAD_DRAM_SDQS3_P              36
#define SWPAD_DRAM_RESET                37
#define SWPAD_JTAG_MOD                  38
#define SWPAD_JTAG_TCK                  39
#define SWPAD_JTAG_TDI                  40
#define SWPAD_JTAG_TDO                  41
#define SWPAD_JTAG_TMS                  42
#define SWPAD_JTAG_TRST_B               43
#define SWPAD_GPIO1_0                   44
#define SWPAD_GPIO1_1                   45
#define SWPAD_GPIO1_2                   46
#define SWPAD_GPIO1_3                   47
#define SWPAD_GPIO1_4                   48
#define SWPAD_GPIO1_5                   49
#define SWPAD_GPIO1_6                   50
#define SWPAD_GPIO1_7                   51
#define SWPAD_GPIO1_8                   52
#define SWPAD_GPIO1_9                   53
#define SWPAD_GPIO1_10                  54
#define SWPAD_GPIO1_11                  55
#define SWPAD_GPIO1_12                  56
#define SWPAD_GPIO1_13                  57
#define SWPAD_CSI_DATA00                58
#define SWPAD_CSI_DATA01                59
#define SWPAD_CSI_DATA02                60
#define SWPAD_CSI_DATA03                61
#define SWPAD_CSI_DATA04                62
#define SWPAD_CSI_DATA05                63
#define SWPAD_CSI_DATA06                64
#define SWPAD_CSI_DATA07                65
#define SWPAD_CSI_HSYNC                 66
#define SWPAD_CSI_MCLK                  67
#define SWPAD_CSI_PIXCLK                68
#define SWPAD_CSI_VSYNC                 69
#define SWPAD_ENET1_COL                 70
#define SWPAD_ENET1_CRS                 71
#define SWPAD_ENET1_MDC                 72
#define SWPAD_ENET1_MDIO                73
#define SWPAD_ENET1_RX_CLK              74
#define SWPAD_ENET1_TX_CLK              75
#define SWPAD_ENET2_COL                 76
#define SWPAD_ENET2_CRS                 77
#define SWPAD_ENET2_RX_CLK              78
#define SWPAD_ENET2_TX_CLK              79
#define SWPAD_KEY_COL0                  80
#define SWPAD_KEY_COL1                  81
#define SWPAD_KEY_COL2                  82
#define SWPAD_KEY_COL3                  83
#define SWPAD_KEY_COL4                  84
#define SWPAD_KEY_ROW0                  85
#define SWPAD_KEY_ROW1                  86
#define SWPAD_KEY_ROW2                  87
#define SWPAD_KEY_ROW3                  88
#define SWPAD_KEY_ROW4                  89
#define SWPAD_LCD1_CLK                  90
#define SWPAD_LCD1_DATA00               91
#define SWPAD_LCD1_DATA01               92
#define SWPAD_LCD1_DATA02               93
#define SWPAD_LCD1_DATA03               94
#define SWPAD_LCD1_DATA04               95
#define SWPAD_LCD1_DATA05               96
#define SWPAD_LCD1_DATA06               97
#define SWPAD_LCD1_DATA07               98
#define SWPAD_LCD1_DATA08               99
#define SWPAD_LCD1_DATA09              100
#define SWPAD_LCD1_DATA10              101
#define SWPAD_LCD1_DATA11              102
#define SWPAD_LCD1_DATA12              103
#define SWPAD_LCD1_DATA13              104
#define SWPAD_LCD1_DATA14              105
#define SWPAD_LCD1_DATA15              106
#define SWPAD_LCD1_DATA16              107
#define SWPAD_LCD1_DATA17              108
#define SWPAD_LCD1_DATA18              109
#define SWPAD_LCD1_DATA19              110
#define SWPAD_LCD1_DATA20              111
#define SWPAD_LCD1_DATA21              112
#define SWPAD_LCD1_DATA22              113
#define SWPAD_LCD1_DATA23              114
#define SWPAD_LCD1_ENABLE              115
#define SWPAD_LCD1_HSYNC               116
#define SWPAD_LCD1_RESET               117
#define SWPAD_LCD1_VSYNC               118
#define SWPAD_NAND_ALE                 119
#define SWPAD_NAND_CE0_B               120
#define SWPAD_NAND_CE1_B               121
#define SWPAD_NAND_CLE                 122
#define SWPAD_NAND_DATA00              123
#define SWPAD_NAND_DATA01              124
#define SWPAD_NAND_DATA02              125
#define SWPAD_NAND_DATA03              126
#define SWPAD_NAND_DATA04              127
#define SWPAD_NAND_DATA05              128
#define SWPAD_NAND_DATA06              129
#define SWPAD_NAND_DATA07              130
#define SWPAD_NAND_RE_B                131
#define SWPAD_NAND_READY_B             132
#define SWPAD_NAND_WE_B                133
#define SWPAD_NAND_WP_B                134
#define SWPAD_QSPI1A_DATA0             135
#define SWPAD_QSPI1A_DATA1             136
#define SWPAD_QSPI1A_DATA2             137
#define SWPAD_QSPI1A_DATA3             138
#define SWPAD_QSPI1A_DQS               139
#define SWPAD_QSPI1A_SCLK              140
#define SWPAD_QSPI1A_SS0_B             141
#define SWPAD_QSPI1A_SS1_B             142
#define SWPAD_QSPI1B_DATA0             143
#define SWPAD_QSPI1B_DATA1             144
#define SWPAD_QSPI1B_DATA2             145
#define SWPAD_QSPI1B_DATA3             146
#define SWPAD_QSPI1B_DQS               147
#define SWPAD_QSPI1B_SCLK              148
#define SWPAD_QSPI1B_SS0_B             149
#define SWPAD_QSPI1B_SS1_B             150
#define SWPAD_RGMII1_RD0               151
#define SWPAD_RGMII1_RD1               152
#define SWPAD_RGMII1_RD2               153
#define SWPAD_RGMII1_RD3               154
#define SWPAD_RGMII1_RX_CTL            155
#define SWPAD_RGMII1_RXC               156
#define SWPAD_RGMII1_TD0               157
#define SWPAD_RGMII1_TD1               158
#define SWPAD_RGMII1_TD2               159
#define SWPAD_RGMII1_TD3               160
#define SWPAD_RGMII1_TX_CTL            161
#define SWPAD_RGMII1_TXC               162
#define SWPAD_RGMII2_RD0               163
#define SWPAD_RGMII2_RD1               164
#define SWPAD_RGMII2_RD2               165
#define SWPAD_RGMII2_RD3               166
#define SWPAD_RGMII2_RX_CTL            167
#define SWPAD_RGMII2_RXC               168
#define SWPAD_RGMII2_TD0               169
#define SWPAD_RGMII2_TD1               170
#define SWPAD_RGMII2_TD2               171
#define SWPAD_RGMII2_TD3               172
#define SWPAD_RGMII2_TX_CTL            173
#define SWPAD_RGMII2_TXC               174
#define SWPAD_SD1_CLK                  175
#define SWPAD_SD1_CMD                  176
#define SWPAD_SD1_DATA0                177
#define SWPAD_SD1_DATA1                178
#define SWPAD_SD1_DATA2                179
#define SWPAD_SD1_DATA3                180
#define SWPAD_SD2_CLK                  181
#define SWPAD_SD2_CMD                  182
#define SWPAD_SD2_DATA0                183
#define SWPAD_SD2_DATA1                184
#define SWPAD_SD2_DATA2                185
#define SWPAD_SD2_DATA3                186
#define SWPAD_SD3_CLK                  187
#define SWPAD_SD3_CMD                  188
#define SWPAD_SD3_DATA0                189
#define SWPAD_SD3_DATA1                190
#define SWPAD_SD3_DATA2                191
#define SWPAD_SD3_DATA3                192
#define SWPAD_SD3_DATA4                193
#define SWPAD_SD3_DATA5                194
#define SWPAD_SD3_DATA6                195
#define SWPAD_SD3_DATA7                196
#define SWPAD_SD4_CLK                  197
#define SWPAD_SD4_CMD                  198
#define SWPAD_SD4_DATA0                199
#define SWPAD_SD4_DATA1                200
#define SWPAD_SD4_DATA2                201
#define SWPAD_SD4_DATA3                202
#define SWPAD_SD4_DATA4                203
#define SWPAD_SD4_DATA5                204
#define SWPAD_SD4_DATA6                205
#define SWPAD_SD4_DATA7                206
#define SWPAD_SD4_RESET_B              207
#define SWPAD_USB_H_DATA               208
#define SWPAD_USB_H_STROBE             209
#define SWPAD_ADDDS                    210
#define SWPAD_DDRMODE_CTL              211
#define SWPAD_DDRPKE                   212
#define SWPAD_DDRPK                    213
#define SWPAD_DDRHYS                   214
#define SWPAD_DDRMODE                  215
#define SWPAD_B0DS                     216
#define SWPAD_B1DS                     217
#define SWPAD_CTLDS                    218
#define SWPAD_DDR_TYPE                 219
#define SWPAD_B2DS                     220
#define SWPAD_B3DS                     221


#define SWPAD_QSPI2A_DATA0             119 /* IOMUXC_SW_PAD_CTL_PAD_NAND_ALE */
#define SWPAD_QSPI2A_DATA1             120 /* IOMUXC_SW_PAD_CTL_PAD_NAND_CE0_B */
#define SWPAD_QSPI2A_DATA2             121 /* IOMUXC_SW_PAD_CTL_PAD_NAND_CE1_B */
#define SWPAD_QSPI2A_DATA3             122 /* IOMUXC_SW_PAD_CTL_PAD_NAND_CLE */
#define SWPAD_QSPI2A_DQS               123 /* IOMUXC_SW_PAD_CTL_PAD_NAND_DATA00 */
#define SWPAD_QSPI2A_SCLK              124 /* IOMUXC_SW_PAD_CTL_PAD_NAND_DATA01 */
#define SWPAD_QSPI2A_SS0_B             125 /* IOMUXC_SW_PAD_CTL_PAD_NAND_DATA02 */
#define SWPAD_QSPI2A_SS1_B             126 /* IOMUXC_SW_PAD_CTL_PAD_NAND_DATA03 */
#define SWPAD_QSPI2B_DATA0             127 /* IOMUXC_SW_PAD_CTL_PAD_NAND_DATA04 */
#define SWPAD_QSPI2B_DATA1             128 /* IOMUXC_SW_PAD_CTL_PAD_NAND_DATA05 */
#define SWPAD_QSPI2B_DATA2             129 /* IOMUXC_SW_PAD_CTL_PAD_NAND_DATA06 */
#define SWPAD_QSPI2B_DATA3             130 /* IOMUXC_SW_PAD_CTL_PAD_NAND_DATA07 */
#define SWPAD_QSPI2B_DQS               131 /* IOMUXC_SW_PAD_CTL_PAD_NAND_RE_B */
#define SWPAD_QSPI2B_SCLK              132 /* IOMUXC_SW_PAD_CTL_PAD_NAND_READY_B */
#define SWPAD_QSPI2B_SS0_B             133 /* IOMUXC_SW_PAD_CTL_PAD_NAND_WE_B */
#define SWPAD_QSPI2B_SS1_B             134 /* IOMUXC_SW_PAD_CTL_PAD_NAND_WP_B */





/*
 * offsets of IOMUXC registers from MX6SX_IOMUX_INPUT
 * where MX6SX_IOMUX_INPUT = MX6SX_IOMUXC_BASE + 0x0624
 */
#define SWINPUT_ANATOP_USB_OTG_ID               0
#define SWINPUT_ANATOP_USB_UH1_ID               1
#define SWINPUT_AUDMUX_P3_INPUT_DA_AMX          2
#define SWINPUT_AUDMUX_P3_INPUT_DB_AMX          3
#define SWINPUT_AUDMUX_P3_INPUT_RXCLK_AMX       4
#define SWINPUT_AUDMUX_P3_INPUT_RXFS_AMX        5
#define SWINPUT_AUDMUX_P3_INPUT_TXCLK_AMX       6
#define SWINPUT_AUDMUX_P3_INPUT_TXFS_AMX        7
#define SWINPUT_AUDMUX_P4_INPUT_DA_AMX          8
#define SWINPUT_AUDMUX_P4_INPUT_DB_AMX          9
#define SWINPUT_AUDMUX_P4_INPUT_RXCLK_AMX      10
#define SWINPUT_AUDMUX_P4_INPUT_RXFS_AMX       11
#define SWINPUT_AUDMUX_P4_INPUT_TXCLK_AMX      12
#define SWINPUT_AUDMUX_P4_INPUT_TXFS_AMX       13
#define SWINPUT_AUDMUX_P5_INPUT_DA_AMX         14
#define SWINPUT_AUDMUX_P5_INPUT_DB_AMX         15
#define SWINPUT_AUDMUX_P5_INPUT_RXCLK_AMX      16
#define SWINPUT_AUDMUX_P5_INPUT_RXFS_AMX       17
#define SWINPUT_AUDMUX_P5_INPUT_TXCLK_AMX      18
#define SWINPUT_AUDMUX_P5_INPUT_TXFS_AMX       19
#define SWINPUT_AUDMUX_P6_INPUT_DA_AMX         20
#define SWINPUT_AUDMUX_P6_INPUT_DB_AMX         21
#define SWINPUT_AUDMUX_P6_INPUT_RXCLK_AMX      22
#define SWINPUT_AUDMUX_P6_INPUT_RXFS_AMX       23
#define SWINPUT_AUDMUX_P6_INPUT_TXCLK_AMX      24
#define SWINPUT_AUDMUX_P6_INPUT_TXFS_AMX       25
#define SWINPUT_CAN1_IPP_IND_CANRX             26
#define SWINPUT_CAN2_IPP_IND_CANRX             27
#define SWINPUT_CANFD_IPD_M_CAN_0_RX           28
#define SWINPUT_CANFD_IPD_M_CAN_1_RX           29
#define SWINPUT_CCM_PMIC_VFUNCIONAL_READY      30
#define SWINPUT_CSI1_IPP_CSI_D_0               31
#define SWINPUT_CSI1_IPP_CSI_D_1               32
#define SWINPUT_CSI1_IPP_CSI_D_2               33
#define SWINPUT_CSI1_IPP_CSI_D_3               34
#define SWINPUT_CSI1_IPP_CSI_D_4               35
#define SWINPUT_CSI1_IPP_CSI_D_5               36
#define SWINPUT_CSI1_IPP_CSI_D_6               37
#define SWINPUT_CSI1_IPP_CSI_D_7               38
#define SWINPUT_CSI1_IPP_CSI_D_8               39
#define SWINPUT_CSI1_IPP_CSI_D_9               40
#define SWINPUT_CSI1_IPP_CSI_D_11              41
#define SWINPUT_CSI1_IPP_CSI_D_12              42
#define SWINPUT_CSI1_IPP_CSI_D_13              43
#define SWINPUT_CSI1_IPP_CSI_D_14              44
#define SWINPUT_CSI1_IPP_CSI_D_15              45
#define SWINPUT_CSI1_IPP_CSI_D_16              46
#define SWINPUT_CSI1_IPP_CSI_D_17              47
#define SWINPUT_CSI1_IPP_CSI_D_18              48
#define SWINPUT_CSI1_IPP_CSI_D_19              49
#define SWINPUT_CSI1_IPP_CSI_D_20              50
#define SWINPUT_CSI1_IPP_CSI_D_21              51
#define SWINPUT_CSI1_IPP_CSI_D_22              52
#define SWINPUT_CSI1_IPP_CSI_D_23              53
#define SWINPUT_CSI1_IPP_CSI_D_10              54
#define SWINPUT_CSI1_IPP_CSI_HSYNC             55
#define SWINPUT_CSI1_IPP_CSI_PIXCLK            56
#define SWINPUT_CSI1_IPP_CSI_VSYNC             57
#define SWINPUT_CSI1_TVDECODER_IN_FIELD        58
#define SWINPUT_ECSPI1_IPP_CSPI_CLK_IN         59
#define SWINPUT_ECSPI1_IPP_IND_MISO            60
#define SWINPUT_ECSPI1_IPP_IND_MOSI            61
#define SWINPUT_ECSPI1_IPP_IND_SS_B            62
#define SWINPUT_ECSPI2_IPP_CSPI_CLK_IN         63
#define SWINPUT_ECSPI2_IPP_IND_MISO            64
#define SWINPUT_ECSPI2_IPP_IND_MOSI            65
#define SWINPUT_ECSPI2_IPP_IND_SS_B            66
#define SWINPUT_ECSPI3_IPP_CSPI_CLK_IN         67
#define SWINPUT_ECSPI3_IPP_IND_MISO            68
#define SWINPUT_ECSPI3_IPP_IND_MOSI            69
#define SWINPUT_ECSPI3_IPP_IND_SS_B            70
#define SWINPUT_ECSPI4_IPP_CSPI_CLK_IN         71
#define SWINPUT_ECSPI4_IPP_IND_MISO            72
#define SWINPUT_ECSPI4_IPP_IND_MOSI            73
#define SWINPUT_ECSPI4_IPP_IND_SS_B            74
#define SWINPUT_ECSPI5_IPP_CSPI_CLK_IN         75
#define SWINPUT_ECSPI5_IPP_IND_MISO            76
#define SWINPUT_ECSPI5_IPP_IND_MOSI            77
#define SWINPUT_ECSPI5_IPP_IND_SS_B            78
#define SWINPUT_ENET1_IPG_CLK_RMII             79
#define SWINPUT_ENET1_IPP_IND_MAC0_MDIO        80
#define SWINPUT_ENET1_IPP_IND_MAC0_RXCLK       81
#define SWINPUT_ENET2_IPG_CLK_RMII             82
#define SWINPUT_ENET2_IPP_IND_MAC0_MDIO        83
#define SWINPUT_ENET2_IPP_IND_MAC0_RXCLK       84
#define SWINPUT_ESAI_IPP_IND_FSR               85
#define SWINPUT_ESAI_IPP_IND_FST               86
#define SWINPUT_ESAI_IPP_IND_HCKR              87
#define SWINPUT_ESAI_IPP_IND_HCKT              88
#define SWINPUT_ESAI_IPP_IND_SCKR              89
#define SWINPUT_ESAI_IPP_IND_SCKT              90
#define SWINPUT_ESAI_IPP_IND_SDO0              91
#define SWINPUT_ESAI_IPP_IND_SDO1              92
#define SWINPUT_ESAI_IPP_IND_SDO2_SDI3         93
#define SWINPUT_ESAI_IPP_IND_SDO3_SDI2         94
#define SWINPUT_ESAI_IPP_IND_SDO4_SDI1         95
#define SWINPUT_ESAI_IPP_IND_SDO5_SDI0         96
#define SWINPUT_I2C1_IPP_SCL_IN                97
#define SWINPUT_I2C1_IPP_SDA_IN                98
#define SWINPUT_I2C2_IPP_SCL_IN                99
#define SWINPUT_I2C2_IPP_SDA_IN               100
#define SWINPUT_I2C3_IPP_SCL_IN               101
#define SWINPUT_I2C3_IPP_SDA_IN               102
#define SWINPUT_I2C4_IPP_SCL_IN               103
#define SWINPUT_I2C4_IPP_SDA_IN               104
#define SWINPUT_KPP_IPP_IND_COL_5             105
#define SWINPUT_KPP_IPP_IND_COL_6             106
#define SWINPUT_KPP_IPP_IND_COL_7             107
#define SWINPUT_KPP_IPP_IND_ROW_5             108
#define SWINPUT_KPP_IPP_IND_ROW_6             109
#define SWINPUT_KPP_IPP_IND_ROW_7             110
#define SWINPUT_LCD1_BUSY                     111
#define SWINPUT_LCD2_BUSY                     112
#define SWINPUT_MLB_MLB_CLK_IN                113
#define SWINPUT_MLB_MLB_DATA_IN               114
#define SWINPUT_MLB_MLB_SIG_IN                115
#define SWINPUT_SAI1_IPP_IND_SAI_RXBCLK       116
#define SWINPUT_SAI1_IPP_IND_SAI_RXDATA       117
#define SWINPUT_SAI1_IPP_IND_SAI_RXSYNC       118
#define SWINPUT_SAI1_IPP_IND_SAI_TXBCLK       119
#define SWINPUT_SAI1_IPP_IND_SAI_TXSYNC       120
#define SWINPUT_SAI2_IPP_IND_SAI_RXBCLK       121
#define SWINPUT_SAI2_IPP_IND_SAI_RXDATA       122
#define SWINPUT_SAI2_IPP_IND_SAI_RXSYNC       123
#define SWINPUT_SAI2_IPP_IND_SAI_TXBCLK       124
#define SWINPUT_SAI2_IPP_IND_SAI_TXSYNC       125
#define SWINPUT_SDMA_EVENTS_14                126
#define SWINPUT_SDMA_EVENTS_15                127
#define SWINPUT_SPDIF_SPDIF_IN1               128
#define SWINPUT_SPDIF_TX_CLK2                 129
#define SWINPUT_UART1_IPP_UART_RTS_B          130
#define SWINPUT_UART1_IPP_UART_RXD_MUX        131
#define SWINPUT_UART2_IPP_UART_RTS_B          132
#define SWINPUT_UART2_IPP_UART_RXD_MUX        133
#define SWINPUT_UART3_IPP_UART_RTS_B          134
#define SWINPUT_UART3_IPP_UART_RXD_MUX        135
#define SWINPUT_UART4_IPP_UART_RTS_B          136
#define SWINPUT_UART4_IPP_UART_RXD_MUX        137
#define SWINPUT_UART5_IPP_UART_RTS_B          138
#define SWINPUT_UART5_IPP_UART_RXD_MUX        139
#define SWINPUT_UART6_IPP_UART_RTS_B          140
#define SWINPUT_UART6_IPP_UART_RXD_MUX        141
#define SWINPUT_USB_IPP_IND_OTG2_OC           142
#define SWINPUT_USB_IPP_IND_OTG_OC            143
#define SWINPUT_USDHC1_IPP_CARD_DET           144
#define SWINPUT_USDHC1_IPP_WP_ON              145
#define SWINPUT_USDHC2_IPP_CARD_DET           146
#define SWINPUT_USDHC2_IPP_WP_ON              147
#define SWINPUT_USDHC4_IPP_CARD_DET           148
#define SWINPUT_USDHC4_IPP_WP_ON              149


/*
 * Function prototypes
 */
void pinmux_set_swmux(int pin, int mux_config);
void pinmux_set_padcfg(int pin, int pad_config);
void pinmux_set_input(int pin, int input_config);


#endif  /* __ARM_MX6SX_IOMUX_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/hardware/startup/lib/public/arm/mx6sx_iomux.h $ $Rev: 753777 $")
#endif
