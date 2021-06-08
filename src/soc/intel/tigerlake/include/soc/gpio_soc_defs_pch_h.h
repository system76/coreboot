/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _SOC_TIGERLAKE_GPIO_SOC_DEFS_PCH_H_H_
#define _SOC_TIGERLAKE_GPIO_SOC_DEFS_PCH_H_H_

/*
 * Most of the fixed numbers and macros are based on the GPP groups.
 * The GPIO groups are accessed through register blocks called
 * communities.
 */
#define GPP_A			0x0
#define GPP_R			0x1
#define GPP_B			0x2
#define GPP_D			0x3
#define GPP_C			0x4
#define GPP_S			0x5
#define GPP_G			0x6
#define GPD			0x7
#define GPP_E			0x8
#define GPP_F			0x9
#define GPP_H			0xA
#define GPP_J			0xB
#define GPP_K			0xC
#define GPP_I			0xD

#define GPIO_MAX_NUM_PER_GROUP	24

#define COMM_0			0
#define COMM_1			1
#define COMM_2			2
#define COMM_3			3
#define COMM_4			4
#define COMM_5			5

/*
 * GPIOs are ordered monotonically increasing to match ACPI/OS driver.
 */

/* Group A */
#define GPP_A0			0
#define GPP_A1			1
#define GPP_A2			2
#define GPP_A3			3
#define GPP_A4			4
#define GPP_A5			5
#define GPP_A6			6
#define GPP_A7			7
#define GPP_A8			8
#define GPP_A9			9
#define GPP_A10			10
#define GPP_A11			11
#define GPP_A12			12
#define GPP_A13			13
#define GPP_A14			14
/* Reserved 15 */
/* Reserved 16 */

/* Group R */
#define GPP_R0			17
#define GPP_R1			18
#define GPP_R2			19
#define GPP_R3			20
#define GPP_R4			21
#define GPP_R5			22
#define GPP_R6			23
#define GPP_R7			24
#define GPP_R8			25
#define GPP_R9			26
#define GPP_R10			27
#define GPP_R11			28
#define GPP_R12			29
#define GPP_R13			30
#define GPP_R14			31
#define GPP_R15			32
#define GPP_R16			33
#define GPP_R17			34
#define GPP_R18			35
#define GPP_R19			36

/* Group B */
#define GPP_B0			37
#define GPP_B1			38
#define GPP_B2			39
#define GPP_B3			40
#define GPP_B4			41
#define GPP_B5			42
#define GPP_B6			43
#define GPP_B7			44
#define GPP_B8			45
#define GPP_B9			46
#define GPP_B10			47
#define GPP_B11			48
#define GPP_B12			49
#define GPP_B13			50
#define GPP_B14			51
#define GPP_B15			52
#define GPP_B16			53
#define GPP_B17			54
#define GPP_B18			55
#define GPP_B19			56
#define GPP_B20			57
#define GPP_B21			58
#define GPP_B22			59
#define GPP_B23			60

#define GPIO_COM0_START		GPP_A0
#define GPIO_COM0_END		GPP_B23
#define NUM_GPIO_COM0_PADS	(GPP_B23 - GPP_A0 + 1)

/* Group D */
#define GPP_D0			61
#define GPP_D1			62
#define GPP_D2			63
#define GPP_D3			64
#define GPP_D4			65
#define GPP_D5			66
#define GPP_D6			67
#define GPP_D7			68
#define GPP_D8			69
#define GPP_D9			70
#define GPP_D10			71
#define GPP_D11			72
#define GPP_D12			73
#define GPP_D13			74
#define GPP_D14			75
#define GPP_D15			76
#define GPP_D16			77
#define GPP_D17			78
#define GPP_D18			79
#define GPP_D19			80
#define GPP_D20			81
#define GPP_D21			82
#define GPP_D22			83
#define GPP_D23			84
/* Reserved 85 */
/* Reserved 86 */

/* Group C */
#define GPP_C0			87
#define GPP_C1			88
#define GPP_C2			89
#define GPP_C3			90
#define GPP_C4			91
#define GPP_C5			92
#define GPP_C6			93
#define GPP_C7			94
#define GPP_C8			95
#define GPP_C9			96
#define GPP_C10			97
#define GPP_C11			98
#define GPP_C12			99
#define GPP_C13			100
#define GPP_C14			101
#define GPP_C15			102
#define GPP_C16			103
#define GPP_C17			104
#define GPP_C18			105
#define GPP_C19			106
#define GPP_C20			107
#define GPP_C21			108
#define GPP_C22			109
#define GPP_C23			110

/* Group S */
#define GPP_S0			111
#define GPP_S1			112
#define GPP_S2			113
#define GPP_S3			114
#define GPP_S4			115
#define GPP_S5			116
#define GPP_S6			117
#define GPP_S7			118

/* Group G */
#define GPP_G0			119
#define GPP_G1			120
#define GPP_G2			121
#define GPP_G3			122
#define GPP_G4			123
#define GPP_G5			124
#define GPP_G6			125
#define GPP_G7			126
#define GPP_G8			127
#define GPP_G9			128
#define GPP_G10			129
#define GPP_G11			130
#define GPP_G12			131
#define GPP_G13			132
#define GPP_G14			133
#define GPP_G15			134

#define GPIO_COM1_START		GPP_D0
#define GPIO_COM1_END		GPP_G15
#define NUM_GPIO_COM1_PADS	(GPP_G15 - GPP_D0 + 1)

/* Group GPD */
#define GPD0			135
#define GPD1			136
#define GPD2			137
#define GPD3			138
#define GPD4			139
#define GPD5			140
#define GPD6			141
#define GPD7			142
#define GPD8			143
#define GPD9			144
#define GPD10			145
#define GPD11			146
#define GPD12			147

#define GPIO_COM2_START		GPD0
#define GPIO_COM2_END		GPD12
#define NUM_GPIO_COM2_PADS	(GPD12 - GPD0 + 1)

/* Group E */
#define GPP_E0			148
#define GPP_E1			149
#define GPP_E2			150
#define GPP_E3			151
#define GPP_E4			152
#define GPP_E5			153
#define GPP_E6			154
#define GPP_E7			155
#define GPP_E8			156
#define GPP_E9			157
#define GPP_E10			158
#define GPP_E11			159
#define GPP_E12			160

/* Group F */
#define GPP_F0			161
#define GPP_F1			162
#define GPP_F2			163
#define GPP_F3			164
#define GPP_F4			165
#define GPP_F5			166
#define GPP_F6			167
#define GPP_F7			168
#define GPP_F8			169
#define GPP_F9			170
#define GPP_F10			171
#define GPP_F11			172
#define GPP_F12			173
#define GPP_F13			174
#define GPP_F14			175
#define GPP_F15			176
#define GPP_F16			177
#define GPP_F17			178
#define GPP_F18			179
#define GPP_F19			180
#define GPP_F20			181
#define GPP_F21			182
#define GPP_F22			183
#define GPP_F23			184

#define GPIO_COM3_START		GPP_E0
#define GPIO_COM3_END		GPP_F23
#define NUM_GPIO_COM3_PADS	(GPP_F23 - GPP_E0 + 1)

/* Group H */
#define GPP_H0			185
#define GPP_H1			186
#define GPP_H2			187
#define GPP_H3			188
#define GPP_H4			189
#define GPP_H5			190
#define GPP_H6			191
#define GPP_H7			192
#define GPP_H8			193
#define GPP_H9			194
#define GPP_H10			195
#define GPP_H11			196
#define GPP_H12			197
#define GPP_H13			198
#define GPP_H14			199
#define GPP_H15			200
#define GPP_H16			201
#define GPP_H17			202
#define GPP_H18			203
#define GPP_H19			204
#define GPP_H20			205
#define GPP_H21			206
#define GPP_H22			207
#define GPP_H23			208

/* Group J */
#define GPP_J0			209
#define GPP_J1			210
#define GPP_J2			211
#define GPP_J3			212
#define GPP_J4			213
#define GPP_J5			214
#define GPP_J6			215
#define GPP_J7			216
#define GPP_J8			217
#define GPP_J9			218

/* Group K */
#define GPP_K0			219
#define GPP_K1			220
#define GPP_K2			221
#define GPP_K3			222
#define GPP_K4			223
#define GPP_K5			224
#define GPP_K6			225
#define GPP_K7			226
#define GPP_K8			227
#define GPP_K9			228
#define GPP_K10			229
#define GPP_K11			230

#define GPIO_COM4_START		GPP_H0
#define GPIO_COM4_END		GPP_K11
#define NUM_GPIO_COM4_PADS	(GPP_K11 - GPP_H0 + 1)

/* Group I */
#define GPP_I0			231
#define GPP_I1			232
#define GPP_I2			233
#define GPP_I3			234
#define GPP_I4			235
#define GPP_I5			236
#define GPP_I6			237
#define GPP_I7			238
#define GPP_I8			239
#define GPP_I9			240
#define GPP_I10			241
#define GPP_I11			242
#define GPP_I12			243
#define GPP_I13			244
#define GPP_I14			245

#define GPIO_COM5_START		GPP_I0
#define GPIO_COM5_END		GPP_I14
#define NUM_GPIO_COM5_PADS	(GPP_I14 - GPP_I0 + 1)

#define TOTAL_GPIO_COMM		(COMM_5 + 1)
#define TOTAL_PADS		246

#endif
