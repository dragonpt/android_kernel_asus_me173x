/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

#ifndef BUILD_LK
#include <linux/string.h>

#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#include <platform/mt_pm_ldo.h>
#include <platform/mt_pmic.h>
#include <debug.h>
#elif defined(BUILD_UBOOT)
//#include <asm/arch/mt6577_gpio.h>
#include <asm/arch/mt_gpio.h>
#include <asm/arch/mt_pm_ldo.h>
#include <asm/arch/mt_pmic.h>
#include <linux/module.h>  /* Needed by all modules */
#include <linux/kernel.h>  /* Needed for KERN_ALERT */
#else
#include <linux/module.h>  /* Needed by all modules */
#include <linux/kernel.h>  /* Needed for KERN_ALERT */
#include <mach/mt_gpio.h>
#include <linux/xlog.h>
#include <mach/mt_pm_ldo.h>
#include <mach/mt_pmic.h>
#include <mach/upmu_common.h>
#endif

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (800)
#define FRAME_HEIGHT (1280)

#define LCM_ID_NT35521 (0x80)

//#define GPIO_LCD_RST_EN      GPIO154
//#define GPIO_LCD_BL_EN       GPIO32
//#define GPIO_LCD_MIPI_PANEL_EN   GPIOEXT18
//#define GPIO_LCD_BIST            GPIOEXT19
#define GPIO_LCD_PANEL_BKLTEN       GPIO177
#define GPIO_LCD_PANEL_RESETINNO    GPIO142
#define GPIO_LCD_PANEL_PANEL_ID     GPIO117

#define PANEL_TYPE_INNOLUX 0
#define PANEL_TYPE_LG 1
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};
static s32 panel_id = 1;
static int panel_type = 1;

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

#define   LCM_DSI_CMD_MODE		0


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)


static void lcd_power_en(unsigned char enabled)
{
#ifndef BUILD_LK
	printk("[DDP] %s : %s\n", __func__, enabled ? "on" : "off");
#endif

	if (enabled) {
		//VGP4 1.8V
		upmu_set_rg_vgp4_en(0x1);
		upmu_set_rg_vgp4_vosel(0x3);
		MDELAY(1);

		 //VGP6 3.3V
		upmu_set_rg_vgp6_en(0x1);
		upmu_set_rg_vgp6_vosel(0x7);
		MDELAY(1);
	} else {
		MDELAY(60);
		//VGP4 1.8V
		upmu_set_rg_vgp4_en(0x0);
		upmu_set_rg_vgp4_vosel(0x0);
		MDELAY(0);

		 //VGP6 3.3V
		upmu_set_rg_vgp6_en(0x0);
		upmu_set_rg_vgp6_vosel(0x0);

		MDELAY(1);
	}
}


static void lcd_backlight_en(unsigned char enabled)
{
	if (enabled) {
		mt_set_gpio_mode(GPIO_LCD_PANEL_BKLTEN, GPIO_MODE_00);
		mt_set_gpio_dir(GPIO_LCD_PANEL_BKLTEN, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_PANEL_BKLTEN, GPIO_OUT_ONE);
	} else {
		mt_set_gpio_mode(GPIO_LCD_PANEL_BKLTEN, GPIO_MODE_00);
		mt_set_gpio_dir(GPIO_LCD_PANEL_BKLTEN, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_PANEL_BKLTEN, GPIO_OUT_ZERO);
	}
}


static void init_lcm_registers(void)
{
	unsigned int data_array[16];
#ifndef BUILD_LK
	printk("[DDP] %s\n", __func__);
#endif

	if (panel_type == PANEL_TYPE_LG) {
		//data_array[0] = 0x00011500;  //software reset
		//dsi_set_cmdq(data_array, 1, 1);

		data_array[0] = 0x0BAE1500;
		dsi_set_cmdq(data_array, 1, 1);
		data_array[0] = 0xEAEE1500;
		dsi_set_cmdq(data_array, 1, 1);
		data_array[0] = 0x5FEF1500;
		dsi_set_cmdq(data_array, 1, 1);
		data_array[0] = 0x68F21500;
		dsi_set_cmdq(data_array, 1, 1);
		data_array[0] = 0x03A71500;
		dsi_set_cmdq(data_array, 1, 1);

		//data_array[0] = 0xFFB21500;
		//dsi_set_cmdq(data_array, 1, 1);

		//data_array[0] = 0x10CC1500;
		//dsi_set_cmdq(data_array, 1, 1);

//		MDELAY(20);
	} else {
#if 1
	data_array[0] =0x00010500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(20);

	data_array[0] = 0x00053902;
	data_array[1] = 0xA555AAFF;
	data_array[2] = 0x00000080;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00033902;
	data_array[1] = (((FRAME_HEIGHT/2)&0xFF) << 16) | (((FRAME_HEIGHT/2)>>8) << 8) | 0x44;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(20);

	data_array[0] = 0x00351500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(20);

	data_array[0]=0x703A1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x0000116F;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000020F7;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x066F1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0xA0F71500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x196F1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x12F71500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x086F1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x40FA1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x116F1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x01F31500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000008;
	dsi_set_cmdq(data_array, 3, 1);

//	data_array[0] = 0x80C81500;
//	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000168B1;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x08B61500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x026F1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x08B81500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x005454BB;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000505BC;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x01C71500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x0CB002BD;
	data_array[2] = 0x0000000A;
	dsi_set_cmdq(data_array, 3, 1);

	//Page 1 relative
	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000108;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000505B0;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000505B1;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x00013ABC;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x00013EBD;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00CA1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x04C01500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x80BE1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x002828B3;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001212B4;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003434B9;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001414BA;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000208;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x02EE1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00053902;
	data_array[1] = 0x150609EF;
	data_array[2] = 0x00000018;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00073902;
	data_array[1] = 0x000000B0;
	data_array[2] = 0x00170008;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x066F1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00073902;
	data_array[1] = 0x002500B0;
	data_array[2] = 0x00450030;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x0C6F1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00053902;
	data_array[1] = 0x005600B0;
	data_array[2] = 0x0000007A;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00073902;
	data_array[1] = 0x00A300B1;
	data_array[2] = 0x002001E7;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x066F1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00073902;
	data_array[1] = 0x017A01B1;
	data_array[2] = 0x00C501C2;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x0C6F1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00053902;
	data_array[1] = 0x020602B1;
	data_array[2] = 0x0000005F;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00073902;
	data_array[1] = 0x029202B2;
	data_array[2] = 0x00FC02D0;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x066F1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00073902;
	data_array[1] = 0x033503B2;
	data_array[2] = 0x008B035D;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x0C6F1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00053902;
	data_array[1] = 0x03A203B2;
	data_array[2] = 0x000000BF;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00053902;
	data_array[1] = 0x03E803B3;
	data_array[2] = 0x000000FF;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00073902;
	data_array[1] = 0x000000BC;
	data_array[2] = 0x00180008;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x066F1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00073902;
	data_array[1] = 0x002700BC;
	data_array[2] = 0x00490032;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x0C6F1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00053902;
	data_array[1] = 0x005C00BC;
	data_array[2] = 0x00000083;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00073902;
	data_array[1] = 0x00AF00BD;
	data_array[2] = 0x002A01F3;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x066F1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00073902;
	data_array[1] = 0x018401BD;
	data_array[2] = 0x00CD01CA;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x0C6F1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00053902;
	data_array[1] = 0x020E02BD;
	data_array[2] = 0x00000065;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00073902;
	data_array[1] = 0x029802BE;
	data_array[2] = 0x000003D4;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x066F1500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00073902;
	data_array[1] = 0x033703BE;
	data_array[2] = 0x008D035F;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x0C6F1500;
	dsi_set_cmdq(&data_array,1,1);

	/* 0xBE, 0x03, 0xA4, 0x03, 0xBF */
#if 0
	data_array[0] = 0x00053902;
	data_array[1] = 0x020E02BD;
	data_array[2] = 0x00000065;
	dsi_set_cmdq(data_array, 3, 1);
#else
	data_array[0] = 0x00053902;
	data_array[1] = 0x03A403BE;
	data_array[2] = 0x000000BF;
	dsi_set_cmdq(data_array, 3, 1);
#endif

	data_array[0] = 0x00053902;
	data_array[1] = 0x03E803BF;
	data_array[2] = 0x000000FF;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000608;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001700B0;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001516B1;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001314B2;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001112B3;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x002D10B4;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000801B5;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003109B6;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131B7;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131B8;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131B9;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131BA;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131BB;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131BC;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000931BD;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000108BE;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x00102DBF;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001211C0;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001413C1;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001615C2;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000017C3;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131E5;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001700C4;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001516C5;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001314C6;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001112C7;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x002D10C8;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000801C9;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003109CA;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131CB;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131CC;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131CD;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131CE;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131CF;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131D0;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000931D1;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000108D2;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x00102DD3;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001211D4;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001413D5;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001615D6;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000017D7;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131E6;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x000000D8;
	data_array[2] = 0x00000000;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x000000D9;
	data_array[2] = 0x00000000;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00E71500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000308;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000020B0;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000020B1;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x420005B2;
	data_array[2] = 0x00000000;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x420005B6;
	data_array[2] = 0x00000000;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x420053BA;
	data_array[2] = 0x00000000;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x420053BB;
	data_array[2] = 0x00000000;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x40C41500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000508;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000617B0;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00B81500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x010103BD;
	data_array[2] = 0x00000100;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000617B1;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000100B9;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000617B2;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000100BA;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000617B3;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x00000ABB;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000617B4;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000617B5;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000314B6;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000000B7;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000102BC;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x05C01500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0xA5C41500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003003C8;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x005103C9;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x030500D1;
	data_array[2] = 0x00000000;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x090500D2;
	data_array[2] = 0x00000000;
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x02E51500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x02E61500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x02E71500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x02E91500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0] = 0x33ED1500;
	dsi_set_cmdq(&data_array,1,1);
#endif

#if 1
	data_array[0] = 0x00110500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(20);
	data_array[0] =0x00290500;
	dsi_set_cmdq(data_array, 1, 1);

	data_array[0] = 0x00130500;
	dsi_set_cmdq(data_array, 1, 1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x00000051;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x00002453;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00023902;
	data_array[1] = 0x0000FF51;
	dsi_set_cmdq(data_array, 2, 1);
#endif
	}
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------
static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
        #else
		params->dsi.mode   = BURST_VDO_MODE; //SYNC_PULSE_VDO_MODE BURST_VDO_MODE SYNC_EVENT_VDO_MODE
	#endif

	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM			= LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	if (panel_type == PANEL_TYPE_LG) {
		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting
		params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.word_count=720*3;

		params->dsi.vertical_sync_active					= 1;
		params->dsi.vertical_backporch					= 3;
		params->dsi.vertical_frontporch					= 3;
		params->dsi.vertical_active_line					= FRAME_HEIGHT;

		params->dsi.horizontal_sync_active				= 1;
		params->dsi.horizontal_backporch				= 57;
		params->dsi.horizontal_frontporch				= 32;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		// Bit rate calculation
		//1 Every lane speed
		params->dsi.PLL_CLOCK = LCM_DSI_6589_PLL_CLOCK_208;
		params->dsi.CLK_ZERO = 47;
		params->dsi.HS_ZERO = 36;

		params->dsi.compatibility_for_nvk = 2;		// this parameter would be set to 2 if DriverIC is NTK's (parellel arch) and when force match DSI clock for NTK's
	} else {
		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=512;

		// Video mode setting
		params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.vertical_sync_active				= 2;
		params->dsi.vertical_backporch					= 12;
		params->dsi.vertical_frontporch					= 10;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 2;
		params->dsi.horizontal_backporch				= 40;
		params->dsi.horizontal_frontporch				= 40;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		// Bit rate calculation
		params->dsi.pll_select=1;
		//1 Every lane speed
		params->dsi.PLL_CLOCK = LCM_DSI_6589_PLL_CLOCK_240_5;

		params->dsi.compatibility_for_nvk = 1;		// this parameter would be set to 2 if DriverIC is NTK's (parellel arch) and when force match DSI clock for NTK's
	}

}


static void lcm_init_power(void)
{
#ifndef BUILD_LK
	return; //do nothing in LK
#else
	lcd_power_en(1);
	MDELAY(1);
	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(1);
	SET_RESET_PIN(1);
	MDELAY(115);
#endif
}

static void lcm_init(void)
{
	panel_id = mt_get_gpio_in(GPIO_LCD_PANEL_PANEL_ID); //id=0 : innolux, id=1 : LG

	if (panel_id > 0)
		panel_type = PANEL_TYPE_LG;
	else
		panel_type = PANEL_TYPE_INNOLUX;
    panel_type = PANEL_TYPE_LG;

#ifndef BUILD_LK
	printk("[DDP] %s, panel type : %s, ID : %d\n", __func__, panel_id ? "LG" : "INNOLUX", panel_id);
#else
	init_lcm_registers();
#endif

}

static void lcm_suspend_power(void)
{
#ifndef BUILD_LK
	printk("[DDP] %s\n", __func__);
#endif

	if (panel_type == PANEL_TYPE_LG) {
		lcd_power_en(0);
		MDELAY(1005);
	} else {
		MDELAY(105);
		mt_set_gpio_mode(GPIO_LCD_PANEL_RESETINNO, GPIO_MODE_00);
		mt_set_gpio_dir(GPIO_LCD_PANEL_RESETINNO, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_PANEL_RESETINNO, GPIO_OUT_ZERO);
		MDELAY(1);

		lcd_power_en(0);
		MDELAY(1);
	}
}


static void lcm_suspend(void)
{
	unsigned int data_array[16];
#ifndef BUILD_LK
	printk("[DDP] %s\n", __func__);
#endif

	if (panel_type == PANEL_TYPE_LG) {
		data_array[0]=0x00280500; // Display Off
		dsi_set_cmdq(data_array, 1, 1);
		MDELAY(1);

		data_array[0] = 0x00111500; // Sleep In
		dsi_set_cmdq(data_array, 1, 1);
	} else {
		lcd_backlight_en(0);
		MDELAY(1);

		/* set display off */
		data_array[0] = 0x00280500;
		dsi_set_cmdq(data_array, 1, 1);
		MDELAY(10);

		/* enter sleep mode */
		data_array[0] = 0x00100500;
		dsi_set_cmdq(data_array, 1, 1);
	}
}


static void lcm_resume_power(void)
{
#ifndef BUILD_LK
	printk("[DDP] %s\n", __func__);
#endif

	if (panel_type == PANEL_TYPE_LG) {
		lcd_power_en(1);
		MDELAY(1);
		SET_RESET_PIN(1);
		SET_RESET_PIN(0);
		MDELAY(1);
		SET_RESET_PIN(1);
		MDELAY(115);
	} else {
		MDELAY(8);
		lcd_power_en(1);
		MDELAY(1);
	}
}


static void lcm_resume(void)
{
	unsigned int data_array[16];

#ifndef BUILD_LK
	printk("[DDP] %s\n", __func__);
#endif

	if (panel_type == PANEL_TYPE_LG) {
		init_lcm_registers();
#if 0
		data_array[0] = 0x00101500; // Sleep Out
		dsi_set_cmdq(data_array, 1, 1);
		MDELAY(20);

		data_array[0] = 0x00290500; // Display On
		dsi_set_cmdq(data_array, 1, 1);
#endif
	} else {
		/* exit sleep mode */
		data_array[0] = 0x00110500;
		dsi_set_cmdq(data_array, 1, 1);
		MDELAY(10);

		/* set display on */
		data_array[0] = 0x00290500;
		dsi_set_cmdq(data_array, 1, 1);
		MDELAY(50);

		mt_set_gpio_mode(GPIO_LCD_PANEL_RESETINNO, GPIO_MODE_00);
		mt_set_gpio_dir(GPIO_LCD_PANEL_RESETINNO, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_PANEL_RESETINNO, GPIO_OUT_ONE);

		mt_set_gpio_mode(GPIO_LCD_PANEL_RESETINNO, GPIO_MODE_00);
		mt_set_gpio_dir(GPIO_LCD_PANEL_RESETINNO, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_PANEL_RESETINNO, GPIO_OUT_ZERO);
		MDELAY(10);

		mt_set_gpio_mode(GPIO_LCD_PANEL_RESETINNO, GPIO_MODE_00);
		mt_set_gpio_dir(GPIO_LCD_PANEL_RESETINNO, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_PANEL_RESETINNO, GPIO_OUT_ONE);
		MDELAY(120);

		init_lcm_registers();
		MDELAY(8);

		lcd_backlight_en(1);
	}
}


#if (LCM_DSI_CMD_MODE)
static void lcm_update(
    unsigned int x,
    unsigned int y,
    unsigned int width,
    unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x00290508; //HW bug, so need send one HS packet
	dsi_set_cmdq(data_array, 1, 1);

	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);

}
#endif


#if 0
static unsigned int lcm_compare_id(void)
{
	unsigned int id=0;
	unsigned char buffer[2];
	unsigned int array[16];

	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(1);

	SET_RESET_PIN(1);
	MDELAY(20);

	array[0] = 0x00033700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x04, buffer, 2);
	id = buffer[1]; //we only need ID

    #ifdef BUILD_LK
		printf("%s, LK nt35521 debug: nt35521 id = 0x%08x\n", __func__, id);
    #else
		printk("%s, kernel nt35521 horse debug: nt35521 id = 0x%08x\n", __func__, id);
    #endif

    if(id == LCM_ID_NT35521)
        return 1;
    else
        return 0;
}



static void lcm_setbacklight(unsigned int level)
{
	unsigned int data_array[16];
#if defined(BUILD_LK)
	printf("%s, %d\n", __func__, level);
#else
	printk("lcm_setbacklight = %d\n", level);
#endif

	if(level > 255)
		level = 255;

	data_array[0] = 0x00023902;
	data_array[1] = (0x51|(level<<8));
	dsi_set_cmdq(data_array, 2, 1);
}
#endif


LCM_DRIVER lgld070wx3_dsi_vdo_lcm_drv =
{
	.name			= "lgld070wx3_dsi_vdo",
	.set_util_funcs		= lcm_set_util_funcs,
	.get_params		= lcm_get_params,
	.init				= lcm_init,
//	.set_backlight		= lcm_setbacklight,
	.suspend			= lcm_suspend,
	.resume			= lcm_resume,
	.suspend_power	= lcm_suspend_power,
	.resume_power	= lcm_resume_power,
	.init_power		= lcm_init_power,
//	.compare_id		= lcm_compare_id,
#if (LCM_DSI_CMD_MODE)
	.update			= lcm_update,
#endif
};



