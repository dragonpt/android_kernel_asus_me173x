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

/* drivers/hwmon/mt6516/amit/stk3x1x.c - stk3x1x ALS/PS driver
 * 
 * Author: MingHsien Hsieh <minghsien.hsieh@mediatek.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/kobject.h>
#include <linux/earlysuspend.h>
#include <linux/platform_device.h>
#include <asm/atomic.h>
#include <linux/version.h>
#include <linux/fs.h>   
#include <linux/wakelock.h> 
#include <asm/io.h>

#include <linux/hwmsen_helper.h>
#include <cust_eint.h>
#include <linux/hwmsensor.h>
#include <linux/sensors_io.h>
#include <linux/hwmsen_dev.h>
#include <cust_alsps.h>
#include "stk3x1x.h"
#define DRIVER_VERSION          "3.0.17"
//#define STK_PS_POLLING_LOG
#define STK_TUNE0
//#define STK_TUNE1
//#define STK_FIR

#ifdef MT6516
#include <mach/mt6516_devs.h>
#include <mach/mt6516_typedefs.h>
#include <mach/mt6516_gpio.h>
#include <mach/mt6516_pll.h>
#endif

#ifdef MT6573
#include <mach/mt6573_devs.h>
#include <mach/mt6573_typedefs.h>
#include <mach/mt6573_gpio.h>
#include <mach/mt6573_pll.h>
#endif

#ifdef MT6575
#include <mach/mt6575_devs.h>
#include <mach/mt6575_typedefs.h>
#include <mach/mt6575_gpio.h>
#include <mach/mt6575_pm_ldo.h>
#endif

#ifdef MT6577
#include <mach/mt6577_devs.h>
#include <mach/mt6577_typedefs.h>
#include <mach/mt6577_gpio.h>
#include <mach/mt6577_pm_ldo.h>
#endif

#ifdef MT6589
//#include <mach/mt_devs.h>
#include <mach/mt_typedefs.h>
#include <mach/mt_gpio.h>
#include <mach/mt_pm_ldo.h>
#endif

#if ((defined MT6573) || (defined MT6575) || (defined MT6577) || (defined MT6589))	
extern void mt65xx_eint_unmask(unsigned int line);
extern void mt65xx_eint_mask(unsigned int line);
extern void mt65xx_eint_set_polarity(kal_uint8 eintno, kal_bool ACT_Polarity);
extern void mt65xx_eint_set_hw_debounce(kal_uint8 eintno, kal_uint32 ms);
extern kal_uint32 mt65xx_eint_set_sens(kal_uint8 eintno, kal_bool sens);
extern void mt65xx_eint_registration(kal_uint8 eintno, kal_bool Dbounce_En,
                                     kal_bool ACT_Polarity, void (EINT_FUNC_PTR)(void),
                                     kal_bool auto_umask);

#endif

/*-------------------------MT6516&MT6573 define-------------------------------*/
#ifdef MT6516
#define POWER_NONE_MACRO MT6516_POWER_NONE
#endif

#if ((defined MT6573) || (defined MT6575) || (defined MT6577) || (defined MT6589))	
#define POWER_NONE_MACRO MT65XX_POWER_NONE
#endif

/******************************************************************************
 * configuration
*******************************************************************************/

/*----------------------------------------------------------------------------*/
#define stk3x1x_DEV_NAME     "stk3x1x"
/*----------------------------------------------------------------------------*/
#define APS_TAG                  "[ALS/PS] "
#define APS_FUN(f)               printk(KERN_INFO APS_TAG"%s\n", __FUNCTION__)
#define APS_ERR(fmt, args...)    printk(KERN_ERR  APS_TAG"%s %d : "fmt, __FUNCTION__, __LINE__, ##args)
#define APS_LOG(fmt, args...)    printk(KERN_INFO APS_TAG fmt, ##args)
#define APS_DBG(fmt, args...)    printk(KERN_INFO fmt, ##args)                 
/******************************************************************************
 * extern functions
*******************************************************************************/
#ifdef MT6516
extern void MT6516_EINTIRQUnmask(unsigned int line);
extern void MT6516_EINTIRQMask(unsigned int line);
extern void MT6516_EINT_Set_Polarity(kal_uint8 eintno, kal_bool ACT_Polarity);
extern void MT6516_EINT_Set_HW_Debounce(kal_uint8 eintno, kal_uint32 ms);
extern kal_uint32 MT6516_EINT_Set_Sensitivity(kal_uint8 eintno, kal_bool sens);
extern void MT6516_EINT_Registration(kal_uint8 eintno, kal_bool Dbounce_En,
                                     kal_bool ACT_Polarity, void (EINT_FUNC_PTR)(void),
                                     kal_bool auto_umask);
#endif
/*----------------------------------------------------------------------------*/
#define mt6516_I2C_DATA_PORT        ((base) + 0x0000)
#define mt6516_I2C_SLAVE_ADDR       ((base) + 0x0004)
#define mt6516_I2C_INTR_MASK        ((base) + 0x0008)
#define mt6516_I2C_INTR_STAT        ((base) + 0x000c)
#define mt6516_I2C_CONTROL          ((base) + 0x0010)
#define mt6516_I2C_TRANSFER_LEN     ((base) + 0x0014)
#define mt6516_I2C_TRANSAC_LEN      ((base) + 0x0018)
#define mt6516_I2C_DELAY_LEN        ((base) + 0x001c)
#define mt6516_I2C_TIMING           ((base) + 0x0020)
#define mt6516_I2C_START            ((base) + 0x0024)
#define mt6516_I2C_FIFO_STAT        ((base) + 0x0030)
#define mt6516_I2C_FIFO_THRESH      ((base) + 0x0034)
#define mt6516_I2C_FIFO_ADDR_CLR    ((base) + 0x0038)
#define mt6516_I2C_IO_CONFIG        ((base) + 0x0040)
#define mt6516_I2C_DEBUG            ((base) + 0x0044)
#define mt6516_I2C_HS               ((base) + 0x0048)
#define mt6516_I2C_DEBUGSTAT        ((base) + 0x0064)
#define mt6516_I2C_DEBUGCTRL        ((base) + 0x0068)
/*----------------------------------------------------------------------------*/
#ifdef STK_TUNE0
	#define STK_MAX_MIN_DIFF	500
	#define STK_LT_N_CT	150
	#define STK_HT_N_CT	350

#elif defined(STK_TUNE1)
	#define K_TYPE_CT 	1
	#define K_TYPE_N		2
	#define K_TYPE_NF	3
	#define KTYPE_FAC  K_TYPE_CT
	
	//Common
	#define STK_CALI_VER0				0x48
	#define STK_CALI_VER1				(0x10 | KTYPE_FAC)
	#define STK_CALI_FILE 				"/data/misc/stk3x1xcali.conf"
	//#define STK_CALI_FILE 				"/system/etc/stk3x1xcali.conf"
	#define STK_CALI_FILE_SIZE 			10
	#define STK_CALI_SAMPLE_NO		5
	#define D_CTK_A	400
	
	#if (KTYPE_FAC == K_TYPE_CT)
		#define D_HT  500
		#define D_LT  300
		//#define DCTK	900
		#define STK_CALI_MIN				300	
		#define STK_CALI_MAX				3000
		
		#elif (KTYPE_FAC == K_TYPE_NF)
		#define D_CT					250
		#define STK_CALI_HT_MIN		1000
		#define STK_CALI_HT_MAX		4000
		#define STK_CALI_LT_MIN		1000
		#define STK_CALI_LT_MAX		4000
		#define STK_CALI_HT_LT_MIN	50
		#define STK_CALI_LT_CT_MIN	50
	#else
		#pragma message("Please choose one KTYPE_FAC")		
	#endif	/* #if (KTYPE_FAC == K_TYPE_CT) */
#endif /* #ifdef STK_TUNE0 */

/*----------------------------------------------------------------------------*/
static struct i2c_client *stk3x1x_i2c_client = NULL;
/*----------------------------------------------------------------------------*/
static const struct i2c_device_id stk3x1x_i2c_id[] = {{stk3x1x_DEV_NAME,0},{}};
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(3,0,0))	
static struct i2c_board_info __initdata i2c_stk3x1x={ I2C_BOARD_INFO("stk3x1x", (0x90>>1))};
#else
/*the adapter id & i2c address will be available in customization*/
static unsigned short stk3x1x_force[] = {0x00, 0x00, I2C_CLIENT_END, I2C_CLIENT_END};
static const unsigned short *const stk3x1x_forces[] = { stk3x1x_force, NULL };
static struct i2c_client_address_data stk3x1x_addr_data = { .forces = stk3x1x_forces,};
#endif
/*----------------------------------------------------------------------------*/
static int stk3x1x_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id); 
static int stk3x1x_i2c_remove(struct i2c_client *client);
#if (LINUX_VERSION_CODE<KERNEL_VERSION(3,0,0))	
static int stk3x1x_i2c_detect(struct i2c_client *client, int kind, struct i2c_board_info *info);
#endif
/*----------------------------------------------------------------------------*/
static int stk3x1x_i2c_suspend(struct i2c_client *client, pm_message_t msg);
static int stk3x1x_i2c_resume(struct i2c_client *client);
static int stk3x1x_set_als_int_thd(struct i2c_client *client, u16 als_data_reg);

static struct stk3x1x_priv *g_stk3x1x_ptr = NULL;
//static int is_near = 2;

#ifdef STK_TUNE0

#elif defined(STK_TUNE1)	
static int32_t stk3x1x_get_ps_cali(uint16_t kd[]);
#endif
/*----------------------------------------------------------------------------*/
typedef enum {
    STK_TRC_ALS_DATA= 0x0001,
    STK_TRC_PS_DATA = 0x0002,
    STK_TRC_EINT    = 0x0004,
    STK_TRC_IOCTL   = 0x0008,
    STK_TRC_I2C     = 0x0010,
    STK_TRC_CVT_ALS = 0x0020,
    STK_TRC_CVT_PS  = 0x0040,
    STK_TRC_DEBUG   = 0x8000,
} STK_TRC;
/*----------------------------------------------------------------------------*/
typedef enum {
    STK_BIT_ALS    = 1,
    STK_BIT_PS     = 2,
} STK_BIT;
/*----------------------------------------------------------------------------*/
struct stk3x1x_i2c_addr {    
/*define a series of i2c slave address*/
    u8  state;      	/* enable/disable state */
    u8  psctrl;     	/* PS control */
    u8  alsctrl;    	/* ALS control */
    u8  ledctrl;   		/* LED control */
    u8  intmode;    	/* INT mode */
    u8  wait;     		/* wait time */
    u8  thdh1_ps;   	/* PS INT threshold high 1 */
	u8	thdh2_ps;		/* PS INT threshold high 2 */
    u8  thdl1_ps;   	/* PS INT threshold low 1 */
	u8  thdl2_ps;   	/* PS INT threshold low 2 */
    u8  thdh1_als;   	/* ALS INT threshold high 1 */
	u8	thdh2_als;		/* ALS INT threshold high 2 */
    u8  thdl1_als;   	/* ALS INT threshold low 1 */
	u8  thdl2_als;   	/* ALS INT threshold low 2 */	
	u8  flag;			/* int flag */
	u8  data1_ps;		/* ps data1 */
	u8  data2_ps;		/* ps data2 */
	u8  data1_als;		/* als data1 */
	u8  data2_als;		/* als data2 */
	u8  data1_offset;	/* offset data1 */
	u8  data2_offset;	/* offset data2 */
	u8  data1_ir;		/* ir data1 */
	u8  data2_ir;		/* ir data2 */
	u8  soft_reset;		/* software reset */
};
/*----------------------------------------------------------------------------*/
#ifdef STK_FIR
struct data_filter {
    s16 raw[8];
    int sum;
    int num;
    int idx;
};
#endif

struct stk3x1x_priv {
    struct alsps_hw  *hw;
    struct i2c_client *client;
    struct delayed_work  eint_work;

    /*i2c address group*/
    struct stk3x1x_i2c_addr  addr;
    
    /*misc*/
    atomic_t    trace;
    atomic_t    i2c_retry;
    atomic_t    als_suspend;
    atomic_t    als_debounce;   /*debounce time after enabling als*/
    atomic_t    als_deb_on;     /*indicates if the debounce is on*/
    atomic_t    als_deb_end;    /*the jiffies representing the end of debounce*/
    atomic_t    ps_mask;        /*mask ps: always return far away*/
    atomic_t    ps_debounce;    /*debounce time after enabling ps*/
    atomic_t    ps_deb_on;      /*indicates if the debounce is on*/
    atomic_t    ps_deb_end;     /*the jiffies representing the end of debounce*/
    atomic_t    ps_suspend;


    /*data*/
    u16         als;
    u16         ps;
    u8          _align;
    u16         als_level_num;
    u16         als_value_num;
    u32         als_level[C_CUST_ALS_LEVEL-1];
    u32         als_value[C_CUST_ALS_LEVEL];

	atomic_t	state_val;
	atomic_t 	psctrl_val;
	atomic_t 	alsctrl_val;
	u8 			wait_val;
	u8		 	ledctrl_val;
	u8		 	int_val;
	
    atomic_t    ps_high_thd_val;     /*the cmd value can't be read, stored in ram*/
    atomic_t    ps_low_thd_val;     /*the cmd value can't be read, stored in ram*/
    ulong       enable;         /*enable mask*/
    ulong       pending_intr;   /*pending interrupt*/
	atomic_t	recv_reg;
    /*early suspend*/
#if defined(CONFIG_HAS_EARLYSUSPEND)
    struct early_suspend    early_drv;
#endif     
	bool first_boot;
#ifdef STK_TUNE0
	uint16_t psa;
	uint16_t psi;	
	uint16_t psi_set;		
#elif defined(STK_TUNE1)
	uint16_t cta;
	uint16_t cti;
	uint16_t kdata;	
	uint16_t kdata1;
	uint16_t aoffset;
	uint8_t state;
#endif	
#ifdef STK_FIR
	struct data_filter      fir;
#endif
};
/*----------------------------------------------------------------------------*/
static struct i2c_driver stk3x1x_i2c_driver = {	
	.probe      = stk3x1x_i2c_probe,
	.remove     = stk3x1x_i2c_remove,
#if (LINUX_VERSION_CODE<KERNEL_VERSION(3,0,0))	
	.detect     = stk3x1x_i2c_detect,
#endif
	.suspend    = stk3x1x_i2c_suspend,
	.resume     = stk3x1x_i2c_resume,
	.id_table   = stk3x1x_i2c_id,
#if (LINUX_VERSION_CODE<KERNEL_VERSION(3,0,0))	
	.address_data = &stk3x1x_addr_data,
#endif
	.driver = {
#if (LINUX_VERSION_CODE<KERNEL_VERSION(3,0,0))	
		.owner          = THIS_MODULE,
#endif
		.name           = stk3x1x_DEV_NAME,
	},
};

static struct stk3x1x_priv *stk3x1x_obj = NULL;
static struct platform_driver stk3x1x_alsps_driver;
static int stk3x1x_get_ps_value(struct stk3x1x_priv *obj, u16 ps);
static int stk3x1x_get_ps_value_only(struct stk3x1x_priv *obj, u16 ps);
static int stk3x1x_get_als_value(struct stk3x1x_priv *obj, u16 als);
static int stk3x1x_read_als(struct i2c_client *client, u16 *data);
static int stk3x1x_read_ps(struct i2c_client *client, u16 *data);
struct wake_lock ps_lock;

/*----------------------------------------------------------------------------*/
int stk3x1x_get_addr(struct alsps_hw *hw, struct stk3x1x_i2c_addr *addr)
{
	if(!hw || !addr)
	{
		return -EFAULT;
	}
	addr->state   = STK_STATE_REG; 
	addr->psctrl   = STK_PSCTRL_REG;         
	addr->alsctrl  = STK_ALSCTRL_REG;
	addr->ledctrl  = STK_LEDCTRL_REG;
	addr->intmode    = STK_INT_REG;
	addr->wait    = STK_WAIT_REG;
	addr->thdh1_ps    = STK_THDH1_PS_REG;
	addr->thdh2_ps    = STK_THDH2_PS_REG;
	addr->thdl1_ps = STK_THDL1_PS_REG;
	addr->thdl2_ps = STK_THDL2_PS_REG;
	addr->thdh1_als    = STK_THDH1_ALS_REG;
	addr->thdh2_als    = STK_THDH2_ALS_REG;
	addr->thdl1_als = STK_THDL1_ALS_REG ;
	addr->thdl2_als = STK_THDL2_ALS_REG;
	addr->flag = STK_FLAG_REG;	
	addr->data1_ps = STK_DATA1_PS_REG;
	addr->data2_ps = STK_DATA2_PS_REG;
	addr->data1_als = STK_DATA1_ALS_REG;	
	addr->data2_als = STK_DATA2_ALS_REG;	
	addr->data1_offset = STK_DATA1_OFFSET_REG;
	addr->data2_offset = STK_DATA2_OFFSET_REG;
	addr->data1_ir = STK_DATA1_IR_REG;	
	addr->data2_ir = STK_DATA2_IR_REG;		
	addr->soft_reset = STK_SW_RESET_REG;	
	
	return 0;
}
/*----------------------------------------------------------------------------*/
int stk3x1x_get_timing(void)
{
	return 200;
/*
	u32 base = I2C2_BASE; 
	return (__raw_readw(mt6516_I2C_HS) << 16) | (__raw_readw(mt6516_I2C_TIMING));
*/
}

/*----------------------------------------------------------------------------*/
int stk3x1x_master_recv(struct i2c_client *client, u16 addr, u8 *buf ,int count)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	int ret = 0, retry = 0;
	int trc = atomic_read(&obj->trace);
	int max_try = atomic_read(&obj->i2c_retry);

	while(retry++ < max_try)
	{
		ret = hwmsen_read_block(client, addr, buf, count);
		if(ret == 0)
            break;
		udelay(100);
	}

	if(unlikely(trc))
	{
		if((retry != 1) && (trc & STK_TRC_DEBUG))
		{
			APS_LOG("(recv) %d/%d\n", retry-1, max_try); 

		}
	}

	/* If everything went ok (i.e. 1 msg transmitted), return #bytes
	transmitted, else error code. */
	return (ret == 0) ? count : ret;
}
/*----------------------------------------------------------------------------*/
int stk3x1x_master_send(struct i2c_client *client, u16 addr, u8 *buf ,int count)
{
	int ret = 0, retry = 0;
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	int trc = atomic_read(&obj->trace);
	int max_try = atomic_read(&obj->i2c_retry);


	while(retry++ < max_try)
	{
		ret = hwmsen_write_block(client, addr, buf, count);
		if (ret == 0)
		    break;
		udelay(100);
	}

	if(unlikely(trc))
	{
		if((retry != 1) && (trc & STK_TRC_DEBUG))
		{
			APS_LOG("(send) %d/%d\n", retry-1, max_try);
		}
	}
	/* If everything went ok (i.e. 1 msg transmitted), return #bytes
	transmitted, else error code. */
	return (ret == 0) ? count : ret;
}
/*----------------------------------------------------------------------------*/
int stk3x1x_write_led(struct i2c_client *client, u8 data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);
	int ret = 0;
    
    ret = stk3x1x_master_send(client, obj->addr.ledctrl, &data, 1);
	if(ret < 0)
	{
		APS_ERR("write led = %d\n", ret);
		return -EFAULT;
	}
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_read_als(struct i2c_client *client, u16 *data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);    
	int ret = 0;
	u8 buf[2];
#ifdef STK_FIR
	int idx;   
#endif
	if(NULL == client)
	{
		return -EINVAL;
	}	
	ret = hwmsen_read_block(client, obj->addr.data1_als, buf, 0x02);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}
	else
	{
		*data = (buf[0] << 8) | (buf[1]);
#ifdef STK_FIR
		if(obj->fir.num < 8)
		{                
			obj->fir.raw[obj->fir.num] = *data;
			obj->fir.sum += *data;
			obj->fir.num++;
			obj->fir.idx++;
		}
		else
		{
			idx = obj->fir.idx % 8;
			obj->fir.sum -= obj->fir.raw[idx];
			obj->fir.raw[idx] = *data;
			obj->fir.sum += *data;
			obj->fir.idx++;
			*data = obj->fir.sum/8;
		}	
#endif

	}
	
	if(atomic_read(&obj->trace) & STK_TRC_ALS_DATA)
	{
		APS_DBG("ALS: 0x%04X\n", (u32)(*data));
	}
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_write_als(struct i2c_client *client, u8 data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);
	int ret = 0;
    
    ret = stk3x1x_master_send(client, obj->addr.alsctrl, &data, 1);
	if(ret < 0)
	{
		APS_ERR("write als = %d\n", ret);
		return -EFAULT;
	}
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_read_flag(struct i2c_client *client, u8 *data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);    
	int ret = 0;
	u8 buf;
	
	if(NULL == client)
	{
		return -EINVAL;
	}	
	ret = hwmsen_read_block(client, obj->addr.flag, &buf, 0x01);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}
	else
	{
		*data = buf;
	}
	
	if(atomic_read(&obj->trace) & STK_TRC_ALS_DATA)
	{
		APS_DBG("PS NF flag: 0x%04X\n", (u32)(*data));
	}
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_read_id(struct i2c_client *client)
{
	int ret = 0;
	u8 buf[2];
	
	if(NULL == client)
	{
		return -EINVAL;
	}	
	ret = hwmsen_read_block(client, STK_PDT_ID_REG, buf, 0x02);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}
	APS_LOG("%s: PID=0x%d, VID=0x%x\n", __func__, buf[0], buf[1]);
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_read_ps(struct i2c_client *client, u16 *data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);    
	int ret = 0;
	u8 buf[2];
	
	if(NULL == client)
	{
		APS_ERR("i2c client is NULL\n");
		return -EINVAL;
	}	
	ret = hwmsen_read_block(client, obj->addr.data1_ps, buf, 0x02);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}
	else
	{
		*data = (buf[0] << 8) | (buf[1]);
	}
	
	if(atomic_read(&obj->trace) & STK_TRC_ALS_DATA)
	{
		APS_DBG("PS: 0x%04X\n", (u32)(*data));
	}
	
	return 0;     
}
/*----------------------------------------------------------------------------*/
int stk3x1x_write_ps(struct i2c_client *client, u8 data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	int ret = 0;

    ret = stk3x1x_master_send(client, obj->addr.psctrl, &data, 1);
	if (ret < 0)
	{
		APS_ERR("write ps = %d\n", ret);
		return -EFAULT;
	} 
	return 0;    
}

/*----------------------------------------------------------------------------*/
int stk3x1x_write_wait(struct i2c_client *client, u8 data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	int ret = 0;

    ret = stk3x1x_master_send(client, obj->addr.wait, &data, 1);
	if (ret < 0)
	{
		APS_ERR("write wait = %d\n", ret);
		return -EFAULT;
	} 
	return 0;    
}

/*----------------------------------------------------------------------------*/
int stk3x1x_write_int(struct i2c_client *client, u8 data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	int ret = 0;

    ret = stk3x1x_master_send(client, obj->addr.intmode, &data, 1);
	if (ret < 0)
	{
		APS_ERR("write intmode = %d\n", ret);
		return -EFAULT;
	} 
	return 0;    
}

/*----------------------------------------------------------------------------*/
int stk3x1x_write_state(struct i2c_client *client, u8 data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	int ret = 0;

    ret = stk3x1x_master_send(client, obj->addr.state, &data, 1);
	if (ret < 0)
	{
		APS_ERR("write state = %d\n", ret);
		return -EFAULT;
	} 
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_write_flag(struct i2c_client *client, u8 data)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	int ret = 0;

    ret = stk3x1x_master_send(client, obj->addr.flag, &data, 1);
	if (ret < 0)
	{
		APS_ERR("write ps = %d\n", ret);
		return -EFAULT;
	} 
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_write_sw_reset(struct i2c_client *client)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	u8 buf = 0, r_buf = 0;	
	int ret = 0;

	buf = 0x7F;
    ret = stk3x1x_master_send(client, obj->addr.wait, (char*)&buf, sizeof(buf));
	if (ret < 0)
	{
		APS_ERR("i2c write test error = %d\n", ret);
		return -EFAULT;
	} 	
	
    ret = stk3x1x_master_recv(client, obj->addr.wait, &r_buf, 1);
	if (ret < 0)
	{
		APS_ERR("i2c read test error = %d\n", ret);
		return -EFAULT;
	}	
	
	if(buf != r_buf)
	{
        APS_ERR("i2c r/w test error, read-back value is not the same, write=0x%x, read=0x%x\n", buf, r_buf);		
		return -EIO;
	}
	
	buf = 0;
    ret = stk3x1x_master_send(client, obj->addr.soft_reset, (char*)&buf, sizeof(buf));
	if (ret < 0)
	{
		APS_ERR("write software reset error = %d\n", ret);
		return -EFAULT;
	} 
	msleep(1);
	return 0;    
}

/*----------------------------------------------------------------------------*/
int stk3x1x_write_ps_high_thd(struct i2c_client *client, u16 thd)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	u8 buf[2];
	int ret = 0;
	
    buf[0] = (u8) ((0xFF00 & thd) >> 8);
    buf[1] = (u8) (0x00FF & thd);	
    ret = stk3x1x_master_send(client, obj->addr.thdh1_ps, &buf[0], 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %d\n",  ret);
		return -EFAULT;
	}
	
    ret = stk3x1x_master_send(client, obj->addr.thdh2_ps, &(buf[1]), 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %d\n", ret);
		return -EFAULT;
	}	
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_write_ps_low_thd(struct i2c_client *client, u16 thd)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	u8 buf[2];
	int ret = 0;
	
    buf[0] = (u8) ((0xFF00 & thd) >> 8);
    buf[1] = (u8) (0x00FF & thd);	
    ret = stk3x1x_master_send(client, obj->addr.thdl1_ps, &buf[0], 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}
	
    ret = stk3x1x_master_send(client, obj->addr.thdl2_ps, &(buf[1]), 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}	
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_write_als_high_thd(struct i2c_client *client, u16 thd)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	u8 buf[2];
	int ret = 0;
	
    buf[0] = (u8) ((0xFF00 & thd) >> 8);
    buf[1] = (u8) (0x00FF & thd);	
    ret = stk3x1x_master_send(client, obj->addr.thdh1_als, &buf[0], 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}
	
    ret = stk3x1x_master_send(client, obj->addr.thdh2_als, &(buf[1]), 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}	
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_write_als_low_thd(struct i2c_client *client, u16 thd)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	u8 buf[2];
	int ret = 0;
	
    buf[0] = (u8) ((0xFF00 & thd) >> 8);
    buf[1] = (u8) (0x00FF & thd);	
    ret = stk3x1x_master_send(client, obj->addr.thdl1_als, &buf[0], 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}
	
    ret = stk3x1x_master_send(client, obj->addr.thdl2_als, &(buf[1]), 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}	
	
	return 0;    
}
/*----------------------------------------------------------------------------*/
int stk3x1x_write_foffset(struct i2c_client *client, u16 ofset)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	u8 buf[2];
	int ret = 0;
	
    buf[0] = (u8) ((0xFF00 & ofset) >> 8);
    buf[1] = (u8) (0x00FF & ofset);	
    ret = stk3x1x_master_send(client, obj->addr.data1_offset, &buf[0], 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}
	
    ret = stk3x1x_master_send(client, obj->addr.data2_offset, &(buf[1]), 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}	
	
	return 0;   	
}

/*----------------------------------------------------------------------------*/
int stk3x1x_write_aoffset(struct i2c_client *client,  u16 ofset)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        
	u8 buf[2];
	int ret = 0;
	u8 s_buf = 0, re_en;
    ret = stk3x1x_master_recv(client, obj->addr.state, &s_buf, 1);
	if (ret < 0)
	{
		APS_ERR("i2c read state error = %d\n", ret);
		return -EFAULT;
	}		
	re_en = (s_buf & STK_STATE_EN_AK_MASK) ? 1: 0;
	if(re_en)
	{
		s_buf &= (~STK_STATE_EN_AK_MASK); 		
		ret = stk3x1x_master_send(client, obj->addr.state, &s_buf, 1);
		if (ret < 0)
		{
			APS_ERR("write state = %d\n", ret);
			return -EFAULT;
		} 			
		msleep(3);		
	}	

    buf[0] = (u8) ((0xFF00 & ofset) >> 8);
    buf[1] = (u8) (0x00FF & ofset);	
    ret = stk3x1x_master_send(client, 0x0E, &buf[0], 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}
	
    ret = stk3x1x_master_send(client, 0x0F, &(buf[1]), 1);
	if (ret < 0)
	{
		APS_ERR("WARNING: %s: %d\n", __func__, ret);
		return -EFAULT;
	}	
	if(!re_en)
		return 0;
	s_buf |= STK_STATE_EN_AK_MASK; 		
	ret = stk3x1x_master_send(client, obj->addr.state, &s_buf, 1);
	if (ret < 0)
	{
		APS_ERR("write state = %d\n", ret);
		return -EFAULT;
	} 			
	return 0;  	
}
/*----------------------------------------------------------------------------*/
static void stk3x1x_power(struct alsps_hw *hw, unsigned int on) 
{
	static unsigned int power_on = 0;

	//APS_LOG("power %s\n", on ? "on" : "off");

	if(hw->power_id != POWER_NONE_MACRO)
	{
		if(power_on == on)
		{
			APS_LOG("ignore power control: %d\n", on);
		}
		else if(on)
		{
			if(!hwPowerOn(hw->power_id, hw->power_vol, "stk3x1x")) 
			{
				APS_ERR("power on fails!!\n");
			}
		}
		else
		{
			if(!hwPowerDown(hw->power_id, "stk3x1x")) 
			{
				APS_ERR("power off fail!!\n");   
			}
		}
	}
	power_on = on;
}

/*----------------------------------------------------------------------------*/
static int stk3x1x_enable_als(struct i2c_client *client, int enable)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);
	int err, cur = 0, old = atomic_read(&obj->state_val);
	int trc = atomic_read(&obj->trace);

	APS_LOG("%s: enable=%d\n", __func__, enable);
	cur = old & (~(STK_STATE_EN_ALS_MASK | STK_STATE_EN_WAIT_MASK)); 
	if(enable)
	{
		cur |= STK_STATE_EN_ALS_MASK;
	}
	else if (old & STK_STATE_EN_PS_MASK)
	{
		cur |= STK_STATE_EN_WAIT_MASK;   
	}
	if(trc & STK_TRC_DEBUG)
	{
		APS_LOG("%s: %08X, %08X, %d\n", __func__, cur, old, enable);
	}
	
	if(0 == (cur ^ old))
	{
		return 0;
	}
	if(enable && obj->hw->polling_mode_als == 0)
	{
		stk3x1x_write_als_high_thd(client, 0x0);
		stk3x1x_write_als_low_thd(client, 0xFFFF);
	}
	err = stk3x1x_write_state(client, cur);
	if(err < 0)
		return err;
	else
		atomic_set(&obj->state_val, cur);
	
	if(enable)
	{
		if(obj->hw->polling_mode_als)
		{
			atomic_set(&obj->als_deb_on, 1);
			atomic_set(&obj->als_deb_end, jiffies+atomic_read(&obj->als_debounce)*HZ/1000);
		}
		else
		{
			//set_bit(STK_BIT_ALS,  &obj->pending_intr);
			schedule_delayed_work(&obj->eint_work,220*HZ/1000); 
		}
	}

	if(trc & STK_TRC_DEBUG)
	{
		APS_LOG("enable als (%d)\n", enable);
	}

	return err;
}
/*----------------------------------------------------------------------------*/
static int stk3x1x_enable_ps(struct i2c_client *client, int enable)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);
	int err, cur = 0, old = atomic_read(&obj->state_val);
	int trc = atomic_read(&obj->trace);
#ifdef STK_TUNE1
	uint16_t kda[3];
#endif	
	cur = old;
	hwm_sensor_data sensor_data;
	
#ifdef STK_TUNE0
	//if(obj->first_boot == true)
	//{		
		APS_LOG("%s: TUNE0\n", __func__);	
		//obj->first_boot = false;
		//atomic_set(&obj->ps_high_thd_val, 0xFFFF); 
		//atomic_set(&obj->ps_low_thd_val, 0); 					
		obj->psa = 0;
		obj->psi = 0xFFFF;
		err = stk3x1x_write_aoffset(obj->client, 0xFFFF);
		if (err < 0)
		{
			APS_ERR( "%s: write i2c error\n", __func__);
			//return err;
		}		
		/*
		if((err = stk3x1x_write_ps_high_thd(client, atomic_read(&obj->ps_high_thd_val))))
		{
			APS_ERR("write high thd error: %d\n", err);
			return err;        
		}
		
		if((err = stk3x1x_write_ps_low_thd(client, atomic_read(&obj->ps_low_thd_val))))
		{
			APS_ERR("write low thd error: %d\n", err);
			return err;        
		}
*/		
	//}
#elif defined(STK_TUNE1)	
	#if (KTYPE_FAC==K_TYPE_CT)
		
		if(obj->first_boot == true)
		{			
			APS_LOG("%s: TUNE1 K_TYPE_CT\n", __func__);
			obj->first_boot = false;	
			cur |= 0x8; 
			err = stk3x1x_get_ps_cali(kda);		
			if(err == 0)
			{		
				obj->kdata = kda[0];
				atomic_set(&obj->ps_high_thd_val, kda[1]); 
				atomic_set(&obj->ps_low_thd_val, kda[2]); 							
				APS_LOG("%s: load kda[0]=%d,kda[1]=%d,kda[2]=%d\n", __FUNCTION__, kda[0], kda[1], kda[2]);
				stk3x1x_write_foffset(obj->client, obj->kdata);
				stk3x1x_write_aoffset(obj->client, obj->kdata + D_CTK_A);	
				obj->cta =obj->kdata + D_CTK_A;
				obj->cti = obj->kdata * 4 / 5;
				
				if((err = stk3x1x_write_ps_high_thd(client, atomic_read(&obj->ps_high_thd_val))))
				{
					APS_ERR("write high thd error: %d\n", err);
					return err;        
				}
				
				if((err = stk3x1x_write_ps_low_thd(client, atomic_read(&obj->ps_low_thd_val))))
				{
					APS_ERR("write low thd error: %d\n", err);
					return err;        
				}				
			}
			else
			{
				APS_ERR("%s: no kdata\n", __FUNCTION__);
				/*
				obj->kdata = DCTK;
				APS_LOG("load def kdata=%d\n", DCTK);			
				stk3x1x_write_foffset(obj->client, obj->kdata);
				stk3x1x_write_aoffset(obj->client, obj->kdata*4);	
				obj->cta = 2 * obj->kdata;
				obj->cti = obj->kdata * 4 / 5;
				*/
			}
		}
		else
		{
			if(obj->kdata != 0)
			{		
				stk3x1x_write_foffset(obj->client, obj->kdata);
				stk3x1x_write_aoffset(obj->client, obj->kdata + D_CTK_A);	
				obj->cta = obj->kdata + D_CTK_A;
				obj->cti = obj->kdata * 4 / 5;			
			}
			else
				APS_ERR("%s: no kdata\n", __FUNCTION__);		
		}
	#elif (KTYPE_FAC==K_TYPE_NF)
		if(obj->first_boot == true)
		{	
			APS_LOG("%s: TUNE1 K_TYPE_NF\n", __func__);		
			obj->first_boot = false;	
			cur |= 0x8; 
			err = stk3x1x_get_ps_cali(kda);		
			if(err == 0)
			{
				obj->kdata = kda[0];
				atomic_set(&obj->ps_high_thd_val, kda[1]); 
				atomic_set(&obj->ps_low_thd_val, kda[2]); 					
				APS_LOG( "%s: load kda[0]=%d,kda[1]=%d,kda[2]=%d\n", __FUNCTION__, kda[0], kda[1], kda[2]);
				stk3x1x_write_foffset(obj->client, obj->kdata);
				stk3x1x_write_aoffset(obj->client, obj->kdata + D_CTK_A);	
				obj->cta = obj->kdata + D_CTK_A;
				obj->cti = obj->kdata * 4 / 5;			
				
				if((err = stk3x1x_write_ps_high_thd(client, atomic_read(&obj->ps_high_thd_val))))
				{
					APS_ERR("write high thd error: %d\n", err);
					return err;        
				}
				
				if((err = stk3x1x_write_ps_low_thd(client, atomic_read(&obj->ps_low_thd_val))))
				{
					APS_ERR("write low thd error: %d\n", err);
					return err;        
				}				
			}		
			else
			{
				APS_ERR("%s: no kdata\n", __FUNCTION__);
			}			
		}
		else
		{
			if(obj->kdata != 0)
			{
				stk3x1x_write_foffset(obj->client, obj->kdata);
				stk3x1x_write_aoffset(obj->client, obj->kdata + D_CTK_A);	
				obj->cta = obj->kdata + D_CTK_A;
				obj->cti = obj->kdata * 4 / 5;			
			}
			else
				APS_ERR("%s: no kdata\n", __FUNCTION__);

		}
	#endif	
#else
	if(obj->first_boot == true)
	{			
		obj->first_boot = false;	
		atomic_set(&obj->ps_high_thd_val, obj->hw->ps_high_thd_val ); 
		atomic_set(&obj->ps_low_thd_val, obj->hw->ps_low_thd_val ); 
		
		if((err = stk3x1x_write_ps_high_thd(client, atomic_read(&obj->ps_high_thd_val))))
		{
			APS_ERR("write high thd error: %d\n", err);
			return err;        
		}
		
		if((err = stk3x1x_write_ps_low_thd(client, atomic_read(&obj->ps_low_thd_val))))
		{
			APS_ERR("write low thd error: %d\n", err);
			return err;        
		}		
	}			
#endif

	APS_LOG("%s: enable=%d\n", __FUNCTION__, enable);	
	//cur &= (~(0x65)); 
	cur &= (~(0x45)); 
	if(enable)
	{
		cur |= (STK_STATE_EN_PS_MASK);
		if(!(old & STK_STATE_EN_ALS_MASK))
			cur |= STK_STATE_EN_WAIT_MASK;
#ifdef STK_TUNE0
			cur |= 	STK_STATE_EN_AK_MASK;
	#elif defined(STK_TUNE1)		
			cur |= 	0x60;
#endif	
		if(1 == obj->hw->polling_mode_ps)
			wake_lock(&ps_lock);
	}
	else
	{
		//cur = old & (~STK_STATE_EN_PS_MASK);
		if(1 == obj->hw->polling_mode_ps)		
			wake_unlock(&ps_lock);
	}
	
	if(trc & STK_TRC_DEBUG)
	{
		APS_LOG("%s: %08X, %08X, %d\n", __func__, cur, old, enable);
	}
	
	if(0 == (cur ^ old))
	{
		return 0;
	}
	
	err = stk3x1x_write_state(client, cur);
	if(err < 0)
		return err;
	else
		atomic_set(&obj->state_val, cur);
	
	if(enable)
	{
		if(obj->hw->polling_mode_ps)
		{
			atomic_set(&obj->ps_deb_on, 1);
			atomic_set(&obj->ps_deb_end, jiffies+atomic_read(&obj->ps_debounce)*HZ/1000);
		}
		else
		{
			//set_bit(STK_BIT_PS,  &obj->pending_intr);
			//schedule_delayed_work(&obj->eint_work,5*HZ/1000);
			if((err = stk3x1x_read_ps(obj->client, &obj->ps)))
			{
				APS_ERR("stk3x1x read ps data: %d\n", err);
				return err;
			}
			
			err = stk3x1x_get_ps_value_only(obj, obj->ps);
			if(err < 0)
			{
				APS_ERR("stk3x1x get ps value: %d\n", err);
				return err;
			}
			else if(stk3x1x_obj->hw->polling_mode_ps == 0)
			{	
				sensor_data.values[0] = err;
				sensor_data.value_divide = 1;
				sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
				APS_LOG("%s:ps raw 0x%x -> value 0x%x \n",__FUNCTION__, obj->ps,sensor_data.values[0]);
				//let up layer to know
				if((err = hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data)))
				{	
					APS_ERR("call hwmsen_get_interrupt_data fail = %d\n", err);
				}			
			}			
		}
	}

	if(trc & STK_TRC_DEBUG)
	{
		APS_LOG("enable ps  (%d)\n", enable);
	}

	return err;
}
/*----------------------------------------------------------------------------*/

static int stk3x1x_check_intr(struct i2c_client *client, u8 *status) 
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);
	int err;

	//if (mt_get_gpio_in(GPIO_ALS_EINT_PIN) == 1) /*skip if no interrupt*/  
	//    return 0;

	err = stk3x1x_read_flag(client, status);	
	if (err < 0)
	{
		APS_ERR("WARNING: read flag reg error: %d\n", err);
		return -EFAULT;
	}
	APS_LOG("%s: read status reg: 0x%x\n", __func__, *status);
    
	if(*status & STK_FLG_ALSINT_MASK)
	{
		set_bit(STK_BIT_ALS, &obj->pending_intr);
	}
	else
	{
	   clear_bit(STK_BIT_ALS, &obj->pending_intr);
	}
	
	if(*status & STK_FLG_PSINT_MASK)
	{
		set_bit(STK_BIT_PS,  &obj->pending_intr);
	}
	else
	{
	    clear_bit(STK_BIT_PS, &obj->pending_intr);
	}
	
	if(atomic_read(&obj->trace) & STK_TRC_DEBUG)
	{
		APS_LOG("check intr: 0x%02X => 0x%08lX\n", *status, obj->pending_intr);
	}

	return 0;
}


static int stk3x1x_clear_intr(struct i2c_client *client, u8 status, u8 disable_flag) 
{
    int err = 0;

    status = status | (STK_FLG_ALSINT_MASK | STK_FLG_PSINT_MASK | STK_FLG_OUI_MASK | STK_FLG_IR_RDY_MASK);
    status &= (~disable_flag);
	APS_LOG(" set flag reg: 0x%x\n", status);
	if((err = stk3x1x_write_flag(client, status)))
		APS_ERR("stk3x1x_write_flag failed, err=%d\n", err);
    return err;
}

/*----------------------------------------------------------------------------*/
static int stk3x1x_set_als_int_thd(struct i2c_client *client, u16 als_data_reg) 
{
	s32 als_thd_h, als_thd_l;	
		
    als_thd_h = als_data_reg + STK_ALS_CODE_CHANGE_THD;
    als_thd_l = als_data_reg - STK_ALS_CODE_CHANGE_THD;
    if (als_thd_h >= (1<<16))
        als_thd_h = (1<<16) -1;
    if (als_thd_l <0)
        als_thd_l = 0;
	APS_LOG("stk3x1x_set_als_int_thd:als_thd_h:%d,als_thd_l:%d\n", als_thd_h, als_thd_l);	
		
	stk3x1x_write_als_high_thd(client, als_thd_h);
	stk3x1x_write_als_low_thd(client, als_thd_l);

	return 0;
}

#ifdef STK_TUNE0	
static int stk3x1x_ps_tune_zero_val(void)
{
	int ret, mode;
	int32_t word_data, tmp_word_data, lii;
	
	ret = i2c_smbus_read_byte_data(stk3x1x_obj->client, STK_PSCTRL_REG);
	if (ret < 0)
	{
		APS_ERR("%s: write i2c error\n", __FUNCTION__);
		return ret;
	}
	
	tmp_word_data = i2c_smbus_read_word_data(stk3x1x_obj->client, 0x20);
	if(tmp_word_data < 0)
	{
		APS_ERR("%s fail, err=0x%x", __FUNCTION__, tmp_word_data);
		return tmp_word_data;	   
	}
	word_data = ((tmp_word_data & 0xFF00) >> 8) | ((tmp_word_data & 0x00FF) << 8) ;
	
	tmp_word_data = i2c_smbus_read_word_data(stk3x1x_obj->client, 0x22);
	if(tmp_word_data < 0)
	{
		APS_ERR("%s fail, err=0x%x", __FUNCTION__, tmp_word_data);
		return tmp_word_data;	   
	}	
	word_data += ((tmp_word_data & 0xFF00) >> 8) | ((tmp_word_data & 0x00FF) << 8) ;	
	
	mode = ret & 0x3F;
	if(mode == 0x30)	
		lii = 50;	
	else if (mode == 0x31)
		lii = 100;		
	else if (mode == 0x32)
		lii = 200;						
	else
	{
		APS_ERR("%s: unsupported PS_IT(0x%x)\n", __FUNCTION__, mode);
		return -1;
	}
	
	if(word_data > lii)	
	{
		APS_LOG( "%s: word_data=%d, lii=%d\n", __FUNCTION__, word_data, lii);		
		return 0xFFFF;	
	}
	return 0;
}

static int stk3x1x_ps_tune_zero_pre(uint8_t flag_reg, uint8_t *dis_flag)
{
	int32_t ret;
	int32_t word_data, tmp_word_data;
	
	if(flag_reg & STK_FLG_OUI_MASK)
	{		
		APS_LOG("stk oui change\n");
		*dis_flag |= STK_FLG_OUI_MASK;	
		ret = stk3x1x_ps_tune_zero_val();
		
		if(ret == 0)
		{
			tmp_word_data = i2c_smbus_read_word_data(stk3x1x_obj->client, 0x0E);
			if(tmp_word_data < 0)
			{
				APS_ERR("%s fail, err=0x%x", __FUNCTION__, tmp_word_data);
				return tmp_word_data;	   
			}
			word_data = ((tmp_word_data & 0xFF00) >> 8) | ((tmp_word_data & 0x00FF) << 8) ;
			APS_LOG("%s: word_data=%d\n", __func__, word_data);
			if(word_data > stk3x1x_obj->psa)
			{
				stk3x1x_obj->psa = word_data;
				APS_LOG("%s: psa=%d,psi=%d\n", __FUNCTION__, stk3x1x_obj->psa, stk3x1x_obj->psi);
			}
			if(word_data < stk3x1x_obj->psi)
			{
				stk3x1x_obj->psi = word_data;	
				APS_LOG("%s: psa=%d,psi=%d\n", __FUNCTION__, stk3x1x_obj->psa, stk3x1x_obj->psi);	
			}	
			return 1;	
		}	
		return 2;
	}				
	return 0;
}

static int stk3x1x_ps_tune_zero_nex(uint8_t proceed)
{
	int32_t ret;
	uint8_t w_state_reg;
	hwm_sensor_data sensor_data;
	u8 flag;
	int val;	
	int old = atomic_read(&stk3x1x_obj->state_val);	
	
	APS_FUN();
	ret = stk3x1x_write_aoffset(stk3x1x_obj->client, 0xFFFF);
	if (ret < 0)
	{
		APS_ERR("%s: write i2c error\n", __FUNCTION__);
		return ret;
	}		
	
	if(proceed == 1)
	{
		if((stk3x1x_obj->psa - stk3x1x_obj->psi) >= STK_MAX_MIN_DIFF)
		{				
			stk3x1x_obj->psi_set = stk3x1x_obj->psi;
				APS_LOG( "%s: psi_set=%d, psa=%d\n", __FUNCTION__, stk3x1x_obj->psi_set, stk3x1x_obj->psa);
			ret = stk3x1x_write_foffset(stk3x1x_obj->client, stk3x1x_obj->psi_set);
			if (ret < 0)
			{
				APS_ERR("%s: write i2c error\n", __FUNCTION__);
				return ret;
			}		
			atomic_set(&stk3x1x_obj->ps_high_thd_val, STK_HT_N_CT); 
			atomic_set(&stk3x1x_obj->ps_low_thd_val, STK_LT_N_CT); 				
			stk3x1x_write_ps_high_thd(stk3x1x_obj->client, STK_HT_N_CT);
			stk3x1x_write_ps_low_thd(stk3x1x_obj->client, STK_LT_N_CT);
			
			/*
			ret = i2c_smbus_read_byte_data(stk3x1x_obj->client, STK_STATE_REG);
			if (ret < 0)
			{
				APS_ERR("%s: write i2c error\n", __FUNCTION__);
				return ret;
			}		
			*/
			w_state_reg = (uint8_t)(old | 0x28); 
			w_state_reg &= (~STK_STATE_EN_AK_MASK); 
			ret = i2c_smbus_write_byte_data(stk3x1x_obj->client, STK_STATE_REG, w_state_reg);
			if (ret < 0)
			{
				APS_ERR("%s: write i2c error\n", __FUNCTION__);
				return ret;
			}		
			atomic_set(&stk3x1x_obj->state_val, w_state_reg);
			APS_LOG( "%s: ps_tune_done\n", __FUNCTION__);				
			//set_bit(STK_BIT_PS,  &obj->pending_intr);
			//schedule_delayed_work(&obj->eint_work,5*HZ/1000);		

			if(stk3x1x_obj->hw->polling_mode_ps != 0)			
			{
				ret = stk3x1x_read_flag(stk3x1x_obj->client, &flag);
				if(ret)
					return ret;
				val = (flag & STK_FLG_NF_MASK)? 1 : 0;
				
				if((ret = stk3x1x_read_ps(stk3x1x_obj->client, &stk3x1x_obj->ps)))
				{
					APS_ERR("stk3x1x read ps data: %d\n", ret);
					return ret;
				}
				sensor_data.values[0] = val;
				sensor_data.value_divide = 1;
				sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
				APS_LOG("%s:ps raw 0x%x -> value 0x%x \n",__func__, stk3x1x_obj->ps,sensor_data.values[0]);
				//let up layer to know
				if((ret = hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data)))
				{	
					APS_ERR("call hwmsen_get_interrupt_data fail = %d\n", ret);
				}
			}		
		}		
	}	
	return 0;
}

#elif defined(STK_TUNE1)
static int stk3x1x_ps_tune_one_pre(uint8_t o_flag_reg, uint8_t *dis_flag)
{
	int32_t tmp_word_data;
	
    if (o_flag_reg & STK_FLG_OUI_MASK)
    {				
		APS_LOG("stk oui change\n");
		*dis_flag |= STK_FLG_OUI_MASK;	
		
		tmp_word_data = i2c_smbus_read_word_data(stk3x1x_obj->client, 0x0E);
		if(tmp_word_data < 0)
		{
			APS_ERR( "%s fail, err=0x%x", __FUNCTION__, tmp_word_data);
			return tmp_word_data;	   
		}
		stk3x1x_obj->aoffset  = ((tmp_word_data & 0xFF00) >> 8) | ((tmp_word_data & 0x00FF) << 8) ;
		APS_LOG("%s: aoffset=%d\n", __FUNCTION__, stk3x1x_obj->aoffset );	
		return 1;
	}		
	return 0;
}

	
static int stk3x1x_ps_tune_one_chkstat(uint8_t w_state_reg, uint8_t target08)
{
	int32_t ret;
	
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}	
	
	if((w_state_reg & 0x8) ^ target08)
	{
		if(target08)
		w_state_reg |= 0x8; 
		else
		w_state_reg &= (~0x8);			
		//printk(KERN_INFO "%s: w_state_reg=0x%x\n", __FUNCTION__, w_state_reg);
		ret = i2c_smbus_write_byte_data(stk3x1x_obj->client, STK_STATE_REG, w_state_reg);
		if (ret < 0)
		{
			APS_ERR("%s: write i2c error\n", __FUNCTION__);
			return ret;
		}
		atomic_set(&stk3x1x_obj->state_val, w_state_reg);
	}
	
	return 0;
}
	

static int stk3x1x_ps_tune_one_nex(void)
{
	int32_t ret;
	int32_t tmp_word_data, word_data2;
	uint8_t w_state_reg;	
	
	ret = i2c_smbus_read_byte_data(stk3x1x_obj->client, STK_STATE_REG);
	if (ret < 0)
	{
		APS_ERR( "%s: write i2c error\n", __FUNCTION__);
		return ret;
	}	
	w_state_reg = ret;
	if(stk3x1x_obj->aoffset <= stk3x1x_obj->cta)
	{
		if(stk3x1x_obj->aoffset < stk3x1x_obj->cti)
		{
			stk3x1x_write_aoffset(stk3x1x_obj->client, stk3x1x_obj->kdata *2);
			stk3x1x_ps_tune_one_chkstat(w_state_reg, 8);
			stk3x1x_obj->state = 1;	
			APS_LOG( "%s: state 1, aoffset=%d, sfoffset=1 \n", __FUNCTION__, stk3x1x_obj->aoffset);		
		}
		else
		{
			tmp_word_data = i2c_smbus_read_word_data(stk3x1x_obj->client, STK_DATA1_OFFSET_REG);
			if(tmp_word_data < 0)
			{
				APS_ERR( "%s fail, err=0x%x", __FUNCTION__, tmp_word_data);
				return tmp_word_data;	   
			}
			word_data2 = ((tmp_word_data & 0xFF00) >> 8) | ((tmp_word_data & 0x00FF) << 8) ;
			APS_LOG("%s: foffset=%d\n", __FUNCTION__, word_data2);
			if(stk3x1x_obj->aoffset > word_data2)
			{
				stk3x1x_ps_tune_one_chkstat(w_state_reg, 0);
				stk3x1x_obj->state = 2;	
				APS_LOG( "%s: state 2, aoffset=%d, sfoffset=0 \n", __FUNCTION__, stk3x1x_obj->aoffset);					
			}
			else
			{
				stk3x1x_ps_tune_one_chkstat(w_state_reg, 8);		
				stk3x1x_obj->state = 3;	
				APS_LOG( "%s: state 3, aoffset=%d, sfoffset=1 \n", __FUNCTION__, stk3x1x_obj->aoffset);	
				
			}
		}
	}		
	else
	{
		stk3x1x_ps_tune_one_chkstat(w_state_reg, 8);	
		stk3x1x_obj->state = 4;	
		APS_LOG( "%s: state 4, aoffset=%d, sfoffset=1 \n", __FUNCTION__, stk3x1x_obj->aoffset);				
	}	
	return 0;
}
#endif	/*#ifdef STK_TUNE0	*/

/*----------------------------------------------------------------------------*/
void stk3x1x_eint_func(void)
{
	struct stk3x1x_priv *obj = g_stk3x1x_ptr;
	APS_LOG(" interrupt fuc\n");
	if(!obj)
	{
		return;
	}
	//schedule_work(&obj->eint_work);
	if(obj->hw->polling_mode_ps == 0 || obj->hw->polling_mode_als == 0)
		schedule_delayed_work(&obj->eint_work,0);
	if(atomic_read(&obj->trace) & STK_TRC_EINT)
	{
		APS_LOG("eint: als/ps intrs\n");
	}
}
/*----------------------------------------------------------------------------*/
static void stk3x1x_eint_work(struct work_struct *work)
{
	struct stk3x1x_priv *obj = g_stk3x1x_ptr;
	int err;
	hwm_sensor_data sensor_data;
	u8 int_status, disable_flag = 0;
#ifdef STK_TUNE0
	uint8_t proceed_tune0 = 0;
#elif defined STK_TUNE1
	uint8_t proceed_tune1 = 0;
#endif
	
	memset(&sensor_data, 0, sizeof(sensor_data));

	APS_LOG(" eint work\n");
	
	if((err = stk3x1x_check_intr(obj->client, &int_status)))
	{
		APS_ERR("stk3x1x_check_intr fail: %d\n", err);
		goto err_i2c_rw;
	}

    APS_LOG(" &obj->pending_intr =%lx\n",obj->pending_intr);
		
#ifdef STK_TUNE0		
	err = stk3x1x_ps_tune_zero_pre(int_status, &disable_flag);
	if(err < 0)
	{
		APS_ERR("%s: stk_ps_tune_zero_pre fail, err=%d", __FUNCTION__, err);
		goto err_i2c_rw;		
	}
	else if (err > 0)
	{
		proceed_tune0 = err;		
		APS_LOG("%s: proceed_tune0=%d", __func__, proceed_tune0);
	}
#elif defined(STK_TUNE1)
	err = stk3x1x_ps_tune_one_pre(int_status, &disable_flag);
	if(err < 0)	
		goto err_i2c_rw;
	else if(err == 1)
	{
		proceed_tune1 = 1;
		APS_LOG("%s: proceed_tune1=1\n", __func__);
	}
#endif	/* #ifdef STK_TUNE0 */	
	
	if(((1<<STK_BIT_ALS) & obj->pending_intr) && (obj->hw->polling_mode_als == 0))
	{
		//get raw data
		APS_LOG("stk als change\n");
		disable_flag |= STK_FLG_ALSINT_MASK;
		if((err = stk3x1x_read_als(obj->client, &obj->als)))
		{
			APS_ERR("stk3x1x_read_als failed %d\n", err);			
			goto err_i2c_rw;
		}
		
		stk3x1x_set_als_int_thd(obj->client, obj->als);
		sensor_data.values[0] = stk3x1x_get_als_value(obj, obj->als);
		sensor_data.value_divide = 1;
		sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
		APS_LOG("%s:als raw 0x%x -> value 0x%x \n", __FUNCTION__, obj->als,sensor_data.values[0]);
		//let up layer to know
		if((err = hwmsen_get_interrupt_data(ID_LIGHT, &sensor_data)))
		{
			APS_ERR("call hwmsen_get_interrupt_data fail = %d\n", err);
		}	  
	}
	if(((1<<STK_BIT_PS) &  obj->pending_intr) && (obj->hw->polling_mode_ps == 0))
	{
		APS_LOG("stk ps change\n");
		disable_flag |= STK_FLG_PSINT_MASK;
		
		if((err = stk3x1x_read_ps(obj->client, &obj->ps)))
		{
			APS_ERR("stk3x1x read ps data: %d\n", err);
			goto err_i2c_rw;
		}
		
		err = stk3x1x_get_ps_value_only(obj, obj->ps);
		if(err < 0)
		{
			APS_ERR("stk3x1x get ps value: %d\n", err);
			goto err_i2c_rw;	
		}
		else
		{
			sensor_data.values[0] = err;
			sensor_data.value_divide = 1;
			sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
			APS_LOG("%s:ps raw 0x%x -> value 0x%x \n",__FUNCTION__, obj->ps,sensor_data.values[0]);
			//let up layer to know
			if((err = hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data)))
			{	
				APS_ERR("call hwmsen_get_interrupt_data fail = %d\n", err);
			}
		}
	}
	
	if((err = stk3x1x_clear_intr(obj->client, int_status, disable_flag)))
	{
		APS_ERR("fail: %d\n", err);
		goto err_i2c_rw;
	}	
	msleep(1);
#ifdef STK_TUNE0	
	if(proceed_tune0)
	{
		err = stk3x1x_ps_tune_zero_nex(proceed_tune0);	
		if(err < 0)
		{
			APS_ERR("%s:stk_ps_tune_zero_nex fail, err=%d\n", __FUNCTION__, err);
			goto err_i2c_rw;
		}			
	}	
#elif defined STK_TUNE1
	if(proceed_tune1)	
	{
		err = stk3x1x_ps_tune_one_nex();
		if(err < 0)
			goto err_i2c_rw;		
	}
#endif		

#ifdef MT6516
	MT6516_EINTIRQUnmask(CUST_EINT_ALS_NUM);      
#endif     
#if ((defined MT6573) || (defined MT6575) || (defined MT6577) || (defined MT6589))
	mt65xx_eint_unmask(CUST_EINT_ALS_NUM);    
#endif	

	return;
	
err_i2c_rw:	
	msleep(30);
#ifdef MT6516
	MT6516_EINTIRQUnmask(CUST_EINT_ALS_NUM);      
#endif     
#if ((defined MT6573) || (defined MT6575) || (defined MT6577) || (defined MT6589))
	mt65xx_eint_unmask(CUST_EINT_ALS_NUM);    
#endif	
	return;
}
/*----------------------------------------------------------------------------*/
int stk3x1x_setup_eint(struct i2c_client *client)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);        

	g_stk3x1x_ptr = obj;
	/*configure to GPIO function, external interrupt*/

  APS_LOG("ALS/PS interrupt pin = %d\n", GPIO_ALS_EINT_PIN);		
	
	mt_set_gpio_mode(GPIO_ALS_EINT_PIN, GPIO_ALS_EINT_PIN_M_EINT);
    mt_set_gpio_dir(GPIO_ALS_EINT_PIN, GPIO_DIR_IN);
	mt_set_gpio_pull_enable(GPIO_ALS_EINT_PIN, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(GPIO_ALS_EINT_PIN, GPIO_PULL_UP);

#ifdef MT6516
	MT6516_EINT_Set_Sensitivity(CUST_EINT_ALS_NUM, CUST_EINT_ALS_SENSITIVE);
	MT6516_EINT_Set_Polarity(CUST_EINT_ALS_NUM, CUST_EINT_ALS_POLARITY);
	MT6516_EINT_Set_HW_Debounce(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_CN);
	MT6516_EINT_Registration(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_EN, CUST_EINT_ALS_POLARITY, stk3x1x_eint_func, 0);
	MT6516_EINTIRQUnmask(CUST_EINT_ALS_NUM);  
#endif
    
#if ((defined MT6573) || (defined MT6575) || (defined MT6577) || (defined MT6589))
    mt65xx_eint_set_sens(CUST_EINT_ALS_NUM, CUST_EINT_ALS_SENSITIVE);
	mt65xx_eint_set_polarity(CUST_EINT_ALS_NUM, CUST_EINT_ALS_POLARITY);
	mt65xx_eint_set_hw_debounce(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_CN);
	mt65xx_eint_registration(CUST_EINT_ALS_NUM, CUST_EINT_ALS_DEBOUNCE_EN, CUST_EINT_ALS_POLARITY, stk3x1x_eint_func, 0);
	mt65xx_eint_unmask(CUST_EINT_ALS_NUM);  
#endif  	
    return 0;
}
/*----------------------------------------------------------------------------*/
static int stk3x1x_init_client(struct i2c_client *client)
{
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);
	int err;
	//u8 int_status;
	
	if((err = stk3x1x_write_sw_reset(client)))
	{
		APS_ERR("software reset error, err=%d", err);
		return err;
	}

	if((err = stk3x1x_read_id(client)))
	{
		APS_ERR("stk3x1x_read_id error, err=%d", err);
		return err;
	}		
	
	//TODO: ok?
	if(obj->hw->polling_mode_ps == 0 || obj->hw->polling_mode_als == 0)
	{
		if((err = stk3x1x_setup_eint(client)))
		{
			APS_ERR("setup eint error: %d\n", err);
			return err;
		}
	}
	
	if((err = stk3x1x_write_state(client, atomic_read(&obj->state_val))))
	{
		APS_ERR("write stete error: %d\n", err);
		return err;        
	}	
	
	/*
	if((err = stk3x1x_check_intr(client, &int_status)))
	{
		APS_ERR("check intr error: %d\n", err);
		//    return err;
	}
	
	if((err = stk3x1x_clear_intr(client, int_status, STK_FLG_PSINT_MASK | STK_FLG_ALSINT_MASK)))
	{
		APS_ERR("clear intr error: %d\n", err);	
		return err;
	}
	*/
	if((err = stk3x1x_write_ps(client, atomic_read(&obj->psctrl_val))))
	{
		APS_ERR("write ps error: %d\n", err);
		return err;        
	}
	
	if((err = stk3x1x_write_als(client, atomic_read(&obj->alsctrl_val))))
	{
		APS_ERR("write als error: %d\n", err);
		return err;
	}	
	
	if((err = stk3x1x_write_led(client, obj->ledctrl_val)))
	{
		APS_ERR("write led error: %d\n", err);
		return err;
	}	
	
	if((err = stk3x1x_write_wait(client, obj->wait_val)))
	{
		APS_ERR("write wait error: %d\n", err);
		return err;
	}	
	/*
	if((err = stk3x1x_write_ps_high_thd(client, atomic_read(&obj->ps_high_thd_val))))
	{
		APS_ERR("write high thd error: %d\n", err);
		return err;        
	}
	
	if((err = stk3x1x_write_ps_low_thd(client, atomic_read(&obj->ps_low_thd_val))))
	{
		APS_ERR("write low thd error: %d\n", err);
		return err;        
	}
	*/
	if((err = stk3x1x_write_int(client, obj->int_val)))
	{
		APS_ERR("write int mode error: %d\n", err);
		return err;        
	}	
#ifdef STK_FIR
	memset(&obj->fir, 0x00, sizeof(obj->fir));  
#endif
	return 0;
}

#ifdef STK_TUNE1
static int32_t stk3x1x_get_ps_cali_file(char *r_buf, int8_t buf_size)
{
	struct file  *cali_file;
	mm_segment_t fs;	
	ssize_t ret;
	
    cali_file = filp_open(STK_CALI_FILE, O_RDONLY,0);
    if(IS_ERR(cali_file))
	{
        APS_ERR( "%s: filp_open error!\n", __FUNCTION__);
        return -ENOENT;
	}
	else
	{
		fs = get_fs();
		set_fs(get_ds());
		ret = cali_file->f_op->read(cali_file,r_buf,buf_size,&cali_file->f_pos);
		if(ret < 0)
		{
			APS_ERR( "%s: read error, ret=%d\n", __FUNCTION__, ret);
			filp_close(cali_file,NULL);
			return -EIO;
		}		
		set_fs(fs);
    }
	
    filp_close(cali_file,NULL);	
	return 0;	
}


static int32_t stk3x1x_set_ps_cali_file(char *w_buf, int8_t w_buf_size)
{
	struct file  *cali_file;
	char r_buf[STK_CALI_FILE_SIZE] = {0};	
	mm_segment_t fs;	
	ssize_t ret;
	int8_t i;
	
    cali_file = filp_open(STK_CALI_FILE, O_CREAT | O_RDWR,0666);	
    if(IS_ERR(cali_file))
	{
        APS_ERR( "%s: filp_open for write error!\n", __FUNCTION__);
        return -ENOENT;
	}
	else
	{
		fs = get_fs();
		set_fs(get_ds());
		
		ret = cali_file->f_op->write(cali_file,w_buf,w_buf_size,&cali_file->f_pos);
		if(ret != w_buf_size)
		{
			APS_ERR( "%s: write error!\n", __FUNCTION__);
			filp_close(cali_file,NULL);
			return -EIO;
		}
		cali_file->f_pos=0x00;
		ret = cali_file->f_op->read(cali_file ,r_buf ,w_buf_size ,&cali_file->f_pos);
		if(ret < 0)
		{
			APS_ERR( "%s: read error!\n", __FUNCTION__);
			filp_close(cali_file,NULL);
			return -EIO;
		}	
		set_fs(fs);			
		for(i=0;i<w_buf_size;i++)
		{
			if(r_buf[i] != w_buf[i])
			{
				APS_ERR( "%s: read back error!, r_buf[%d](0x%x) != w_buf[%d](0x%x)\n", __FUNCTION__, i, r_buf[i], i, w_buf[i]);
				filp_close(cali_file,NULL);
				return -EIO;
			}
		}
		
    }
    filp_close(cali_file,NULL);	
	
	APS_LOG("%s successfully\n", __FUNCTION__);
	return 0;
}

static int32_t stk3x1x_get_ps_cali(uint16_t kd[])
{
	char r_buf[STK_CALI_FILE_SIZE] = {0};
	int32_t ret;
			
	if ((ret = stk3x1x_get_ps_cali_file(r_buf, STK_CALI_FILE_SIZE)) < 0)
	{
		return ret;
	}
	else
	{
		if(r_buf[0] == STK_CALI_VER0 && r_buf[1] == STK_CALI_VER1)
		{
			kd[0] = ((uint16_t)r_buf[2] << 8) | r_buf[3];
			kd[1] = ((uint16_t)r_buf[4] << 8) | r_buf[5];
			kd[2] = ((uint16_t)r_buf[6] << 8) | r_buf[7];			
		}
		else
		{			
			APS_ERR("cali version number error! r_buf=0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n", r_buf[0], 
			r_buf[1], r_buf[2], r_buf[3], r_buf[4], r_buf[5], r_buf[6], r_buf[7]);				
			return -EINVAL;
		}
	}	
	return 0;
}


static void stk3x1x_set_ps_to_file(uint16_t ps_ave[])
{
	char w_buf[STK_CALI_FILE_SIZE] = {0};	
	
	w_buf[0] = STK_CALI_VER0;
	w_buf[1] = STK_CALI_VER1;
	w_buf[2] = ((ps_ave[0] & 0xFF00) >> 8);	
	w_buf[3] = (ps_ave[0] & 0x00FF);			
	w_buf[4] = ((ps_ave[1] & 0xFF00) >> 8);	
	w_buf[5] = (ps_ave[1] & 0x00FF);			
	w_buf[6] = ((ps_ave[2] & 0xFF00) >> 8);	
	w_buf[7] = (ps_ave[2] & 0x00FF);			
	stk3x1x_set_ps_cali_file(w_buf, sizeof(char)*STK_CALI_FILE_SIZE);	
	return;
}


static int32_t stk3x1x_get_svrl_ps_data(uint16_t ps_stat_data[])
{
	uint16_t ps_adc;
	int8_t data_count = 0;	
	uint32_t ave_ps_int32 = 0;
	int32_t ret;
	uint8_t w_state_reg, org_state_reg, org_int_reg;	
	
	ps_stat_data[0] = 0;
	ps_stat_data[2] = 9999;
	ps_stat_data[1] = 0;
	
	ret = i2c_smbus_read_byte_data(stk3x1x_obj->client, STK_STATE_REG);
	if (ret < 0)
	{
		APS_ERR( "%s: write i2c error\n", __FUNCTION__);
		return ret;
	}		
	org_state_reg = ret;	

	w_state_reg = (uint8_t)(ret & (~(STK_STATE_EN_AK_MASK | STK_STATE_EN_ASO_MASK))); 
	w_state_reg |= STK_STATE_EN_PS_MASK;			
	ret = i2c_smbus_write_byte_data(stk3x1x_obj->client, STK_STATE_REG, w_state_reg);
	if (ret < 0)
	{
		APS_ERR( "%s: write i2c error\n", __FUNCTION__);
		return ret;
	}				

	ret = i2c_smbus_read_byte_data(stk3x1x_obj->client, STK_INT_REG);
	if (ret < 0)
	{
		APS_ERR( "%s: write i2c error\n", __FUNCTION__);
		return ret;
	}		
	org_int_reg = ret;
	ret = i2c_smbus_write_byte_data(stk3x1x_obj->client, STK_INT_REG, 0);
	if (ret < 0)
	{
		APS_ERR( "%s: write i2c error\n", __FUNCTION__);
		return ret;
	}		
	
	
	while(data_count < STK_CALI_SAMPLE_NO)
	{
		msleep(110);	
		ret = stk3x1x_read_ps(stk3x1x_obj->client, &ps_adc);
		if(ret < 0)
			return ret;
		APS_LOG("%s: ps_adc #%d=%d\n", __FUNCTION__, data_count, ps_adc);
		
		ave_ps_int32 +=  ps_adc;			
		if(ps_adc > ps_stat_data[0])
			ps_stat_data[0] = ps_adc;
		if(ps_adc < ps_stat_data[2])
			ps_stat_data[2] = ps_adc;						
		data_count++;	
	}	
	ave_ps_int32 /= STK_CALI_SAMPLE_NO;	
	ps_stat_data[1] = (int16_t)ave_ps_int32;

	ret = i2c_smbus_write_byte_data(stk3x1x_obj->client, STK_INT_REG, org_int_reg);
	if (ret < 0)
	{
		APS_ERR( "%s: write i2c error\n", __FUNCTION__);
		return ret;
	}	
		
	ret = i2c_smbus_write_byte_data(stk3x1x_obj->client, STK_STATE_REG, org_state_reg);
	if (ret < 0)
	{
		APS_ERR( "%s: write i2c error\n", __FUNCTION__);
		return ret;
	}				

	return 0;
}


#if (KTYPE_FAC==K_TYPE_CT)	
static int32_t stk3x1x_judge_ct_range(uint16_t ps_stat)
{
	uint16_t saved_item[3];
	
	if(ps_stat < STK_CALI_MAX && ps_stat > STK_CALI_MIN)
	{
		APS_LOG("%s: ps_stat=%d\n", __FUNCTION__, ps_stat);
		stk3x1x_obj->kdata = ps_stat;		
		saved_item[0] = ps_stat;
		saved_item[1] = D_HT;
		saved_item[2] = D_LT;
		stk3x1x_set_ps_to_file(saved_item);			
		return 0;		
	}
	APS_ERR( "%s: ps_stat(%d) out of range", __FUNCTION__, ps_stat);
	return -1;	
}
#elif (KTYPE_FAC==K_TYPE_NF)
static int32_t stk3x1x_judge_ht_range(uint16_t ps_stat)
{
	if(ps_stat < STK_CALI_HT_MAX && ps_stat > STK_CALI_HT_MIN)
	{
		APS_LOG( "%s: kdata=%d\n", __FUNCTION__, ps_stat);
		stk3x1x_obj->kdata = ps_stat;	
		return 0;
	}
	APS_ERR("%s: kdata(%d) out of range", __FUNCTION__, ps_stat);
	return -1;	
}

static int32_t stk3x1x_judge_lt_range(uint16_t ps_stat)
{
	if(ps_stat < STK_CALI_LT_MAX && ps_stat > STK_CALI_LT_MIN)
	{
		APS_LOG( "%s: kdata1=%d\n", __FUNCTION__, ps_stat);
		stk3x1x_obj->kdata1 = ps_stat;	
		return 0;
	}
	APS_ERR("%s: kdata1(%d) out of range", __FUNCTION__, ps_stat);
	return -1;	
}
#endif

static int32_t stk3x1x_set_ps_cali(int ht)
{
	uint16_t ps_statistic_data[3] = {0};
	int32_t ret = 0;
	uint16_t saved_item[3];
	
	ret = stk3x1x_get_svrl_ps_data(ps_statistic_data);
	if(ret < 0)
	{
		APS_ERR( "%s: calibration fail\n", __FUNCTION__);
		return ret;
	}	
	
#if (KTYPE_FAC==K_TYPE_CT)	
	ret = stk3x1x_judge_ct_range(ps_statistic_data[1]);
	if(ret < 0)	
		APS_ERR("%s: calibration failed\n", __FUNCTION__);
#elif (KTYPE_FAC==K_TYPE_NF)	
	if(ht == 1)
	{
		ret = stk3x1x_judge_ht_range(ps_statistic_data[1]);	
		if(ret < 0)	
			APS_ERR("%s: calibration failed\n", __FUNCTION__);		
	}
	else
	{
		ret = stk3x1x_judge_lt_range(ps_statistic_data[1]);	
		if(ret < 0)	
			APS_ERR("%s: calibration failed\n", __FUNCTION__);
	}
	
	if(stk3x1x_obj->kdata != 0 && stk3x1x_obj->kdata1 != 0)
	{
		if((stk3x1x_obj->kdata - stk3x1x_obj->kdata1) >= STK_CALI_HT_LT_MIN && 
			(stk3x1x_obj->kdata - stk3x1x_obj->kdata1) < (D_CT - STK_CALI_LT_CT_MIN))
		{
			saved_item[0] = stk3x1x_obj->kdata - D_CT;
			saved_item[1] = D_CT;
			saved_item[2] = stk3x1x_obj->kdata1 - stk3x1x_obj->kdata + D_CT;
			APS_LOG( "%s: saved_item=%d, %d,%d", __FUNCTION__, saved_item[0], saved_item[1], saved_item[2]);
			stk3x1x_set_ps_to_file(saved_item);				
		}
		else
			APS_ERR("%s: kdata(%d) and kdata1(%d) are too close\n", __FUNCTION__, stk3x1x_obj->kdata, stk3x1x_obj->kdata1);
	}
#endif		
	return ret;	
}
#endif	/*#ifdef STK_TUNE1	*/

/******************************************************************************
 * Sysfs attributes
*******************************************************************************/
static ssize_t stk3x1x_show_config(struct device_driver *ddri, char *buf)
{
	ssize_t res;
	
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	res = scnprintf(buf, PAGE_SIZE, "(%d %d %d %d %d %d)\n", 
		atomic_read(&stk3x1x_obj->i2c_retry), atomic_read(&stk3x1x_obj->als_debounce), 
		atomic_read(&stk3x1x_obj->ps_mask), atomic_read(&stk3x1x_obj->ps_high_thd_val),atomic_read(&stk3x1x_obj->ps_low_thd_val), atomic_read(&stk3x1x_obj->ps_debounce));     
	return res;    
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_store_config(struct device_driver *ddri, const char *buf, size_t count)
{
	int retry, als_deb, ps_deb, mask, hthres, lthres, err;
	struct i2c_client *client;
	client = stk3x1x_i2c_client;
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	if(6 == sscanf(buf, "%d %d %d %d %d %d", &retry, &als_deb, &mask, &hthres, &lthres, &ps_deb))
	{ 
		atomic_set(&stk3x1x_obj->i2c_retry, retry);
		atomic_set(&stk3x1x_obj->als_debounce, als_deb);
		atomic_set(&stk3x1x_obj->ps_mask, mask);
		atomic_set(&stk3x1x_obj->ps_high_thd_val, hthres);    
		atomic_set(&stk3x1x_obj->ps_low_thd_val, lthres);        
		atomic_set(&stk3x1x_obj->ps_debounce, ps_deb);

		if((err = stk3x1x_write_ps_high_thd(client, atomic_read(&stk3x1x_obj->ps_high_thd_val))))
		{
			APS_ERR("write high thd error: %d\n", err);
			return err;        
		}
		
		if((err = stk3x1x_write_ps_low_thd(client, atomic_read(&stk3x1x_obj->ps_low_thd_val))))
		{
			APS_ERR("write low thd error: %d\n", err);
			return err;        
		}
	}
	else
	{
		APS_ERR("invalid content: '%s', length = %d\n", buf, count);
	}
	return count;    
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_trace(struct device_driver *ddri, char *buf)
{
	ssize_t res;
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}

	res = scnprintf(buf, PAGE_SIZE, "0x%04X\n", atomic_read(&stk3x1x_obj->trace));     
	return res;    
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_store_trace(struct device_driver *ddri, const char *buf, size_t count)
{
    int trace;
    if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	if(1 == sscanf(buf, "0x%x", &trace))
	{
		atomic_set(&stk3x1x_obj->trace, trace);
	}
	else 
	{
		APS_ERR("invalid content: '%s', length = %d\n", buf, (int)count);
	}
	return count;    
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_als(struct device_driver *ddri, char *buf)
{
	int res;
	
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	if((res = stk3x1x_read_als(stk3x1x_obj->client, &stk3x1x_obj->als)))
	{
		return scnprintf(buf, PAGE_SIZE, "ERROR: %d\n", res);
	}
	else
	{
		return scnprintf(buf, PAGE_SIZE, "0x%04X\n", stk3x1x_obj->als);     
	}
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_ps(struct device_driver *ddri, char *buf)
{
	int res;
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	if((res = stk3x1x_read_ps(stk3x1x_obj->client, &stk3x1x_obj->ps)))
	{
		return scnprintf(buf, PAGE_SIZE, "ERROR: %d\n", res);
	}
	else
	{
		return scnprintf(buf, PAGE_SIZE, "0x%04X\n", stk3x1x_obj->ps);     
	}
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_reg(struct device_driver *ddri, char *buf)
{
	u8 int_status;
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	/*read*/
	stk3x1x_check_intr(stk3x1x_obj->client, &int_status);
	//stk3x1x_clear_intr(stk3x1x_obj->client, int_status, 0x0);
	stk3x1x_read_ps(stk3x1x_obj->client, &stk3x1x_obj->ps);
	stk3x1x_read_als(stk3x1x_obj->client, &stk3x1x_obj->als);
	/*write*/
	stk3x1x_write_als(stk3x1x_obj->client, atomic_read(&stk3x1x_obj->alsctrl_val));
	stk3x1x_write_ps(stk3x1x_obj->client, atomic_read(&stk3x1x_obj->psctrl_val)); 
	stk3x1x_write_ps_high_thd(stk3x1x_obj->client, atomic_read(&stk3x1x_obj->ps_high_thd_val));
	stk3x1x_write_ps_low_thd(stk3x1x_obj->client, atomic_read(&stk3x1x_obj->ps_low_thd_val));
	return 0;
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_send(struct device_driver *ddri, char *buf)
{
    return 0;
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_store_send(struct device_driver *ddri, const char *buf, size_t count)
{
	int addr, cmd;
	u8 dat;

	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	else if(2 != sscanf(buf, "%x %x", &addr, &cmd))
	{
		APS_ERR("invalid format: '%s'\n", buf);
		return 0;
	}

	dat = (u8)cmd;
	APS_LOG("send(%02X, %02X) = %d\n", addr, cmd, 
	stk3x1x_master_send(stk3x1x_obj->client, (u16)addr, &dat, sizeof(dat)));
	
	return count;
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_recv(struct device_driver *ddri, char *buf)
{
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	return scnprintf(buf, PAGE_SIZE, "0x%04X\n", atomic_read(&stk3x1x_obj->recv_reg));     	
  //  return 0;
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_store_recv(struct device_driver *ddri, const char *buf, size_t count)
{
	int addr;
	u8 dat;
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	else if(1 != sscanf(buf, "%x", &addr))
	{
		APS_ERR("invalid format: '%s'\n", buf);
		return 0;
	}

	APS_LOG("recv(%02X) = %d, 0x%02X\n", addr, 
	stk3x1x_master_recv(stk3x1x_obj->client, (u16)addr, (char*)&dat, sizeof(dat)), dat);
	atomic_set(&stk3x1x_obj->recv_reg, dat);	
	return count;
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_allreg(struct device_driver *ddri, char *buf)
{
	int ret = 0;
	u8 rbuf[25];
	int cnt;	
	ssize_t len = 0;
	
	memset(rbuf, 0, sizeof(rbuf));
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}

	ret = hwmsen_read_block(stk3x1x_obj->client, 0, &rbuf[0], 7);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}	

	ret = hwmsen_read_block(stk3x1x_obj->client, 7, &rbuf[7], 7);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}	
	
	ret = hwmsen_read_block(stk3x1x_obj->client, 14, &rbuf[14], 7);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}	

	ret = hwmsen_read_block(stk3x1x_obj->client, 21, &rbuf[21], 4);
	if(ret < 0)
	{
		APS_DBG("error: %d\n", ret);
		return -EFAULT;
	}	
	
	for(cnt=0;cnt<25;cnt++)
	{
		len += scnprintf(buf+len, PAGE_SIZE - len, "%x ", rbuf[cnt]);
		APS_LOG("reg[0x%x]=0x%x\n", cnt, rbuf[cnt]);
	}	
	
	//return scnprintf(buf, PAGE_SIZE, "0x%04X\n", rbuf[0]);     	
	//len += scnprintf(buf+len, PAGE_SIZE - len, "\n");
	return len;
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_status(struct device_driver *ddri, char *buf)
{
	ssize_t len = 0;
	
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	if(stk3x1x_obj->hw)
	{
		len += scnprintf(buf+len, PAGE_SIZE-len, "CUST: %d, (%d %d) (%02X) (%02X %02X %02X) (%02X %02X %02X %02X)\n", 
			stk3x1x_obj->hw->i2c_num, stk3x1x_obj->hw->power_id, stk3x1x_obj->hw->power_vol, stk3x1x_obj->addr.flag, 
			stk3x1x_obj->addr.alsctrl, stk3x1x_obj->addr.data1_als, stk3x1x_obj->addr.data2_als, stk3x1x_obj->addr.psctrl, 
			stk3x1x_obj->addr.data1_ps, stk3x1x_obj->addr.data2_ps, stk3x1x_obj->addr.thdh1_ps);
	}
	else
	{
		len += scnprintf(buf+len, PAGE_SIZE-len, "CUST: NULL\n");
	}
	
	len += scnprintf(buf+len, PAGE_SIZE-len, "REGS: %02X %02X %02X %02X %02X %02X %02X %02X %02lX %02lX\n", 
				atomic_read(&stk3x1x_obj->state_val), atomic_read(&stk3x1x_obj->psctrl_val), atomic_read(&stk3x1x_obj->alsctrl_val), 
				stk3x1x_obj->ledctrl_val, stk3x1x_obj->int_val, stk3x1x_obj->wait_val, 
				atomic_read(&stk3x1x_obj->ps_high_thd_val), atomic_read(&stk3x1x_obj->ps_low_thd_val),stk3x1x_obj->enable, stk3x1x_obj->pending_intr);
#ifdef MT6516
	len += scnprintf(buf+len, PAGE_SIZE-len, "EINT: %d (%d %d %d %d)\n", mt_get_gpio_in(GPIO_ALS_EINT_PIN),
				CUST_EINT_ALS_NUM, CUST_EINT_ALS_POLARITY, CUST_EINT_ALS_DEBOUNCE_EN, CUST_EINT_ALS_DEBOUNCE_CN);

	len += scnprintf(buf+len, PAGE_SIZE-len, "GPIO: %d (%d %d %d %d)\n",	GPIO_ALS_EINT_PIN, 
				mt_get_gpio_dir(GPIO_ALS_EINT_PIN), mt_get_gpio_mode(GPIO_ALS_EINT_PIN), 
				mt_get_gpio_pull_enable(GPIO_ALS_EINT_PIN), mt_get_gpio_pull_select(GPIO_ALS_EINT_PIN));
#endif

	len += scnprintf(buf+len, PAGE_SIZE-len, "MISC: %d %d\n", atomic_read(&stk3x1x_obj->als_suspend), atomic_read(&stk3x1x_obj->ps_suspend));	
	len += scnprintf(buf+len, PAGE_SIZE-len, "VER.: %s\n", DRIVER_VERSION);

	return len;
}
/*----------------------------------------------------------------------------*/
#define IS_SPACE(CH) (((CH) == ' ') || ((CH) == '\n'))
/*----------------------------------------------------------------------------*/
static int read_int_from_buf(struct stk3x1x_priv *obj, const char* buf, size_t count,
                             u32 data[], int len)
{
	int idx = 0;
	char *cur = (char*)buf, *end = (char*)(buf+count);

	while(idx < len)
	{
		while((cur < end) && IS_SPACE(*cur))
		{
			cur++;        
		}

		if(1 != sscanf(cur, "%d", &data[idx]))
		{
			break;
		}

		idx++; 
		while((cur < end) && !IS_SPACE(*cur))
		{
			cur++;
		}
	}
	return idx;
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_alslv(struct device_driver *ddri, char *buf)
{
	ssize_t len = 0;
	int idx;
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	for(idx = 0; idx < stk3x1x_obj->als_level_num; idx++)
	{
		len += scnprintf(buf+len, PAGE_SIZE-len, "%d ", stk3x1x_obj->hw->als_level[idx]);
	}
	len += scnprintf(buf+len, PAGE_SIZE-len, "\n");
	return len;    
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_store_alslv(struct device_driver *ddri, const char *buf, size_t count)
{
//	struct stk3x1x_priv *obj;
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	else if(!strcmp(buf, "def"))
	{
		memcpy(stk3x1x_obj->als_level, stk3x1x_obj->hw->als_level, sizeof(stk3x1x_obj->als_level));
	}
	else if(stk3x1x_obj->als_level_num != read_int_from_buf(stk3x1x_obj, buf, count, 
			stk3x1x_obj->hw->als_level, stk3x1x_obj->als_level_num))
	{
		APS_ERR("invalid format: '%s'\n", buf);
	}    
	return count;
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_show_alsval(struct device_driver *ddri, char *buf)
{
	ssize_t len = 0;
	int idx;
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	for(idx = 0; idx < stk3x1x_obj->als_value_num; idx++)
	{
		len += scnprintf(buf+len, PAGE_SIZE-len, "%d ", stk3x1x_obj->hw->als_value[idx]);
	}
	len += scnprintf(buf+len, PAGE_SIZE-len, "\n");
	return len;    
}
/*----------------------------------------------------------------------------*/
static ssize_t stk3x1x_store_alsval(struct device_driver *ddri, const char *buf, size_t count)
{
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	else if(!strcmp(buf, "def"))
	{
		memcpy(stk3x1x_obj->als_value, stk3x1x_obj->hw->als_value, sizeof(stk3x1x_obj->als_value));
	}
	else if(stk3x1x_obj->als_value_num != read_int_from_buf(stk3x1x_obj, buf, count, 
			stk3x1x_obj->hw->als_value, stk3x1x_obj->als_value_num))
	{
		APS_ERR("invalid format: '%s'\n", buf);
	}    
	return count;
}

#ifdef STK_TUNE0
static ssize_t stk3x1x_show_cali(struct device_driver *ddri, char *buf)
{
	int ret;	
	int32_t word_data, tmp_word_data;
	
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	ret = i2c_smbus_read_byte_data(stk3x1x_obj->client, STK_PSCTRL_REG);
	if (ret < 0)
	{
		APS_ERR("%s: write i2c error\n", __FUNCTION__);
		return ret;
	}
	
	tmp_word_data = i2c_smbus_read_word_data(stk3x1x_obj->client, 0x20);
	if(tmp_word_data < 0)
	{
		APS_ERR("%s fail, err=0x%x", __FUNCTION__, tmp_word_data);
		return tmp_word_data;	   
	}
	word_data = ((tmp_word_data & 0xFF00) >> 8) | ((tmp_word_data & 0x00FF) << 8) ;
	
	tmp_word_data = i2c_smbus_read_word_data(stk3x1x_obj->client, 0x22);
	if(tmp_word_data < 0)
	{
		APS_ERR("%s fail, err=0x%x", __FUNCTION__, tmp_word_data);
		return tmp_word_data;	   
	}	
	word_data += ((tmp_word_data & 0xFF00) >> 8) | ((tmp_word_data & 0x00FF) << 8) ;	

	APS_LOG("%s: psi_set=%d, psa=%d,psi=%d, word_data=%d\n", __FUNCTION__, 
		stk3x1x_obj->psi_set, stk3x1x_obj->psa, stk3x1x_obj->psi, word_data);	
	
	return 0;
}

#elif defined(STK_TUNE1)	
static ssize_t stk3x1x_show_cali(struct device_driver *ddri, char *buf)
{
	uint16_t kd = 0xFFFF;
		
	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	
	stk3x1x_get_ps_cali(&kd);	

	return scnprintf(buf, PAGE_SIZE, "%d\n", kd);     	
}


static ssize_t stk3x1x_store_cali(struct device_driver *ddri, const char *buf, size_t count)
{
	int cmd, ret;

	if(!stk3x1x_obj)
	{
		APS_ERR("stk3x1x_obj is null!!\n");
		return 0;
	}
	else if(1 != sscanf(buf, "%x", &cmd))
	{
		APS_ERR("invalid format: '%s'\n", buf);
		return 0;
	}

	if(cmd > 0)	
	{
		/* 1 for HT and 2 for LH */		
		ret = stk3x1x_set_ps_cali(cmd);	
		if(ret < 0)
			return ret;
	}
	else
		APS_ERR( "%s: (%d) not supported mode\n", __FUNCTION__, cmd);
	
	return count;
}
#endif
/*----------------------------------------------------------------------------*/
static DRIVER_ATTR(als,     S_IWUSR | S_IRUGO, stk3x1x_show_als,   NULL);
static DRIVER_ATTR(ps,      S_IWUSR | S_IRUGO, stk3x1x_show_ps,    NULL);
static DRIVER_ATTR(config,  S_IWUSR | S_IRUGO, stk3x1x_show_config,stk3x1x_store_config);
static DRIVER_ATTR(alslv,   S_IWUSR | S_IRUGO, stk3x1x_show_alslv, stk3x1x_store_alslv);
static DRIVER_ATTR(alsval,  S_IWUSR | S_IRUGO, stk3x1x_show_alsval,stk3x1x_store_alsval);
static DRIVER_ATTR(trace,   S_IWUSR | S_IRUGO, stk3x1x_show_trace, stk3x1x_store_trace);
static DRIVER_ATTR(status,  S_IWUSR | S_IRUGO, stk3x1x_show_status,  NULL);
static DRIVER_ATTR(send,    S_IWUSR | S_IRUGO, stk3x1x_show_send,  stk3x1x_store_send);
static DRIVER_ATTR(recv,    S_IWUSR | S_IRUGO, stk3x1x_show_recv,  stk3x1x_store_recv);
static DRIVER_ATTR(reg,     S_IWUSR | S_IRUGO, stk3x1x_show_reg,   NULL);
static DRIVER_ATTR(allreg,  S_IWUSR | S_IRUGO, stk3x1x_show_allreg,   NULL);
#ifdef STK_TUNE0
static DRIVER_ATTR(cali,    S_IWUSR | S_IRUGO, stk3x1x_show_cali,  NULL);
#elif defined STK_TUNE1
static DRIVER_ATTR(cali,    S_IWUSR | S_IRUGO, stk3x1x_show_cali,  stk3x1x_store_cali);
#endif
/*----------------------------------------------------------------------------*/
static struct driver_attribute *stk3x1x_attr_list[] = {
    &driver_attr_als,
    &driver_attr_ps,    
    &driver_attr_trace,        /*trace log*/
    &driver_attr_config,
    &driver_attr_alslv,
    &driver_attr_alsval,
    &driver_attr_status,
    &driver_attr_send,
    &driver_attr_recv,
    &driver_attr_allreg,
//    &driver_attr_i2c,
    &driver_attr_reg,
#if (defined(STK_TUNE1) || defined(STK_TUNE0))
    &driver_attr_cali,
#endif	
};

/*----------------------------------------------------------------------------*/
static int stk3x1x_create_attr(struct device_driver *driver) 
{
	int idx, err = 0;
	int num = (int)(sizeof(stk3x1x_attr_list)/sizeof(stk3x1x_attr_list[0]));
	if (driver == NULL)
	{
		return -EINVAL;
	}

	for(idx = 0; idx < num; idx++)
	{
		if((err = driver_create_file(driver, stk3x1x_attr_list[idx])))
		{            
			APS_ERR("driver_create_file (%s) = %d\n", stk3x1x_attr_list[idx]->attr.name, err);
			break;
		}
	}    
	return err;
}
/*----------------------------------------------------------------------------*/
static int stk3x1x_delete_attr(struct device_driver *driver)
{
	int idx ,err = 0;
	int num = (int)(sizeof(stk3x1x_attr_list)/sizeof(stk3x1x_attr_list[0]));

	if (!driver)
	return -EINVAL;

	for (idx = 0; idx < num; idx++) 
	{
		driver_remove_file(driver, stk3x1x_attr_list[idx]);
	}
	
	return err;
}
/****************************************************************************** 
 * Function Configuration
******************************************************************************/
static int stk3x1x_get_als_value(struct stk3x1x_priv *obj, u16 als)
{
	int idx;
	int invalid = 0;
	for(idx = 0; idx < obj->als_level_num; idx++)
	{
		if(als < obj->hw->als_level[idx])
		{
			break;
		}
	}
	
	if(idx >= obj->als_value_num)
	{
		APS_ERR("exceed range\n"); 
		idx = obj->als_value_num - 1;
	}
	
	if(1 == atomic_read(&obj->als_deb_on))
	{
		unsigned long endt = atomic_read(&obj->als_deb_end);
		if(time_after(jiffies, endt))
		{
			atomic_set(&obj->als_deb_on, 0);
		}
		
		if(1 == atomic_read(&obj->als_deb_on))
		{
			invalid = 1;
		}
	}

	if(!invalid)
	{
		if (atomic_read(&obj->trace) & STK_TRC_CVT_ALS)
		{
			APS_DBG("ALS: %05d => %05d\n", als, obj->hw->als_value[idx]);
		}
		
		return obj->hw->als_value[idx];
	}
	else
	{
		if(atomic_read(&obj->trace) & STK_TRC_CVT_ALS)
		{
			APS_DBG("ALS: %05d => %05d (-1)\n", als, obj->hw->als_value[idx]);    
		}
		return -1;
	}
}

/*----------------------------------------------------------------------------*/
static int stk3x1x_get_ps_value_only(struct stk3x1x_priv *obj, u16 ps)
{
	int mask = atomic_read(&obj->ps_mask);
	int invalid = 0, val;
	int err;
	u8 flag;

	err = stk3x1x_read_flag(obj->client, &flag);
	if(err)
		return err;
	val = (flag & STK_FLG_NF_MASK)? 1 : 0;	
	
	
/*
	if( obj->hw->polling_mode_ps == 1 )
	{	
		if( is_near != 1 && ps >= atomic_read(&obj->ps_high_thd_val))
		{
			val = 0;  //close
			is_near = 1;
		}
		else if( is_near != 0 && ps <= atomic_read(&obj->ps_low_thd_val))
		{
			val = 1;  //far away
			is_near = 0;
		}
		else
		{
			if(is_near==1)
				val = 0;//near
			else
				val = 1;//far
		}
	}
	else
	{
		if( ps >= atomic_read(&obj->ps_high_thd_val))
		{
			val = 0;  //close	
		}
		else
		{
			val = 1;  //far away
		}		
	}		

	if((err = stk3x1x_clear_intr(obj->client, flag, STK_FLG_OUI_MASK)))
	{
		APS_ERR("fail: %d\n", err);
		return err;
	}	
*/	
	if(atomic_read(&obj->ps_suspend))
	{
		invalid = 1;
	}
	else if(1 == atomic_read(&obj->ps_deb_on))
	{
		unsigned long endt = atomic_read(&obj->ps_deb_end);
		if(time_after(jiffies, endt))
		{
			atomic_set(&obj->ps_deb_on, 0);
		}
		
		if (1 == atomic_read(&obj->ps_deb_on))
		{
			invalid = 1;
		}
	}
		
	
	if(!invalid)
	{
		if(unlikely(atomic_read(&obj->trace) & STK_TRC_CVT_PS))
		{
			if(mask)
			{
				APS_DBG("PS:  %05d => %05d [M] \n", ps, val);
			}
			else
			{
				APS_DBG("PS:  %05d => %05d\n", ps, val);
			}
		}
		return val;
		
	}	
	else
	{
		APS_ERR(" ps value is invalid, PS:  %05d => %05d\n", ps, val);
		if(unlikely(atomic_read(&obj->trace) & STK_TRC_CVT_PS))
		{
			APS_DBG("PS:  %05d => %05d (-1)\n", ps, val);    
		}
		return -1;
	}	
}

/*----------------------------------------------------------------------------*/
static int stk3x1x_get_ps_value(struct stk3x1x_priv *obj, u16 ps)
{
	int mask = atomic_read(&obj->ps_mask);
	int invalid = 0, val;
	int err;
	u8 flag;
#ifdef STK_TUNE0
	uint8_t proceed_tune0 = 0;
	uint8_t disable_flag = 0;
#elif defined(STK_TUNE1)
	uint8_t disable_flag = 0;
	uint8_t proceed_tune1 = 0;
#endif

	err = stk3x1x_read_flag(obj->client, &flag);
	if(err)
		return err;
	val = (flag & STK_FLG_NF_MASK)? 1 : 0;
#ifdef STK_TUNE0		
	err = stk3x1x_ps_tune_zero_pre(flag, &disable_flag);
	if(err < 0)
	{
		APS_ERR("%s: stk_ps_tune_zero_pre fail, err=%d", __FUNCTION__, err);
		return err;		
	}
	else if (err > 0)
	{
		proceed_tune0 = err;		
		APS_LOG("%s: proceed_tune0=%d", __func__, proceed_tune0);	
	}
#elif defined(STK_TUNE1)
	err = stk3x1x_ps_tune_one_pre(err, &disable_flag);
	if(err < 0)
	{
		APS_ERR("%s: stk_ps_tune_one_pre fail, err=%d", __FUNCTION__, err);
		return err;
	}
	else if(err == 1)
	{
		proceed_tune1 = 1;	
	}
#endif		
	
	
/*
	if( obj->hw->polling_mode_ps == 1 )
	{	
		if( is_near != 1 && ps >= atomic_read(&obj->ps_high_thd_val))
		{
			val = 0;  //close
			is_near = 1;
		}
		else if( is_near != 0 && ps <= atomic_read(&obj->ps_low_thd_val))
		{
			val = 1;  //far away
			is_near = 0;
		}
		else
		{
			if(is_near==1)
				val = 0;//near
			else
				val = 1;//far
		}
	}
	else
	{
		if( ps >= atomic_read(&obj->ps_high_thd_val))
		{
			val = 0;  //close	
		}
		else
		{
			val = 1;  //far away
		}		
	}		
*/
	if((err = stk3x1x_clear_intr(obj->client, flag, STK_FLG_OUI_MASK)))
	{
		APS_ERR("fail: %d\n", err);
		return err;
	}	
#ifdef STK_TUNE0		
	if(proceed_tune0)
	{
		err = stk3x1x_ps_tune_zero_nex(proceed_tune0);	
		if(err < 0)
		{
			APS_ERR("%s:stk_ps_tune_zero_nex fail, err=%d\n", __FUNCTION__, err);
			return err;
		}		
	}	
#elif defined STK_TUNE1		
	if(proceed_tune1)
	{
		err = stk3x1x_ps_tune_one_nex();
		if(err < 0)
		{
			APS_ERR( "%s:stk3x1x_ps_tune_one_nex fail, err=%d\n", __FUNCTION__, err);
			return err;
		}	
	}
#endif	

	if(atomic_read(&obj->ps_suspend))
	{
		invalid = 1;
	}
	else if(1 == atomic_read(&obj->ps_deb_on))
	{
		unsigned long endt = atomic_read(&obj->ps_deb_end);
		if(time_after(jiffies, endt))
		{
			atomic_set(&obj->ps_deb_on, 0);
		}
		
		if (1 == atomic_read(&obj->ps_deb_on))
		{
			invalid = 1;
		}
	}
		
	
	if(!invalid)
	{
		if(unlikely(atomic_read(&obj->trace) & STK_TRC_CVT_PS))
		{
			if(mask)
			{
				APS_DBG("PS:  %05d => %05d [M] \n", ps, val);
			}
			else
			{
				APS_DBG("PS:  %05d => %05d\n", ps, val);
			}
		}
		return val;
		
	}	
	else
	{
		APS_ERR(" ps value is invalid, PS:  %05d => %05d\n", ps, val);
		if(unlikely(atomic_read(&obj->trace) & STK_TRC_CVT_PS))
		{
			APS_DBG("PS:  %05d => %05d (-1)\n", ps, val);    
		}
		return -1;
	}	
}
/****************************************************************************** 
 * Function Configuration
******************************************************************************/
static int stk3x1x_open(struct inode *inode, struct file *file)
{
	file->private_data = stk3x1x_i2c_client;

	if (!file->private_data)
	{
		APS_ERR("null pointer!!\n");
		return -EINVAL;
	}
	
	return nonseekable_open(inode, file);
}
/*----------------------------------------------------------------------------*/
static int stk3x1x_release(struct inode *inode, struct file *file)
{
	file->private_data = NULL;
	return 0;
}
/*----------------------------------------------------------------------------*/
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,36))	
static long stk3x1x_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#else
static int stk3x1x_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)      
#endif
{
	struct i2c_client *client = (struct i2c_client*)file->private_data;
	struct stk3x1x_priv *obj = i2c_get_clientdata(client);  
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,36))	
	long err = 0;
#else
	int err = 0;
#endif
	void __user *ptr = (void __user*) arg;
	int dat;
	uint32_t enable;

	switch (cmd)
	{
		case ALSPS_SET_PS_MODE:
			if(copy_from_user(&enable, ptr, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			if(enable)
			{
				if((err = stk3x1x_enable_ps(obj->client, 1)))
				{
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,36))	
					APS_ERR("enable ps fail: %ld\n", err); 
#else
					APS_ERR("enable ps fail: %d\n", err); 
#endif
					goto err_out;
				}
				
				set_bit(STK_BIT_PS, &obj->enable);
			}
			else
			{
				if((err = stk3x1x_enable_ps(obj->client, 0)))
				{
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,36))	
					APS_ERR("disable ps fail: %ld\n", err); 
#else
					APS_ERR("disable ps fail: %d\n", err); 
#endif
	
					goto err_out;
				}
				
				clear_bit(STK_BIT_PS, &obj->enable);
			}
			break;

		case ALSPS_GET_PS_MODE:
			enable = test_bit(STK_BIT_PS, &obj->enable) ? (1) : (0);
			if(copy_to_user(ptr, &enable, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			break;

		case ALSPS_GET_PS_DATA:    
			if((err = stk3x1x_read_ps(obj->client, &obj->ps)))
			{
				goto err_out;
			}
			
			dat = stk3x1x_get_ps_value(obj, obj->ps);
			if(dat < 0)
			{
				err = dat;
				goto err_out;
			}
#ifdef STK_PS_POLLING_LOG	
			APS_LOG("%s:ps raw 0x%x -> value 0x%x \n",__FUNCTION__, obj->ps, dat);			
#endif			
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}  
			break;

		case ALSPS_GET_PS_RAW_DATA:    
			if((err = stk3x1x_read_ps(obj->client, &obj->ps)))
			{
				goto err_out;
			}
			
			dat = obj->ps;
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}  
			break;            

		case ALSPS_SET_ALS_MODE:
			if(copy_from_user(&enable, ptr, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			if(enable)
			{
				if((err = stk3x1x_enable_als(obj->client, 1)))
				{
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,36))	
					APS_ERR("enable als fail: %ld\n", err); 
#else
					APS_ERR("enable als fail: %d\n", err); 
#endif

					goto err_out;
				}
				set_bit(STK_BIT_ALS, &obj->enable);
			}
			else
			{
				if((err = stk3x1x_enable_als(obj->client, 0)))
				{
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,36))	
					APS_ERR("disable als fail: %ld\n", err); 
#else
					APS_ERR("disable als fail: %d\n", err); 
#endif

					goto err_out;
				}
				clear_bit(STK_BIT_ALS, &obj->enable);
			}
			break;

		case ALSPS_GET_ALS_MODE:
			enable = test_bit(STK_BIT_ALS, &obj->enable) ? (1) : (0);
			if(copy_to_user(ptr, &enable, sizeof(enable)))
			{
				err = -EFAULT;
				goto err_out;
			}
			break;

		case ALSPS_GET_ALS_DATA: 
			if((err = stk3x1x_read_als(obj->client, &obj->als)))
			{
				goto err_out;
			}

			dat = stk3x1x_get_als_value(obj, obj->als);
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}              
			break;

		case ALSPS_GET_ALS_RAW_DATA:    
			if((err = stk3x1x_read_als(obj->client, &obj->als)))
			{
				goto err_out;
			}

			dat = obj->als;
			if(copy_to_user(ptr, &dat, sizeof(dat)))
			{
				err = -EFAULT;
				goto err_out;
			}              
			break;
		
		default:
			APS_ERR("%s not supported = 0x%04x", __FUNCTION__, cmd);
			err = -ENOIOCTLCMD;
			break;
	}

	err_out:
	return err;    
}
/*----------------------------------------------------------------------------*/
static struct file_operations stk3x1x_fops = {
#if (LINUX_VERSION_CODE<KERNEL_VERSION(3,0,0))	//TODO: remove?
	.owner = THIS_MODULE,
#endif
	.open = stk3x1x_open,
	.release = stk3x1x_release,
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(2,6,36))	
	.unlocked_ioctl = stk3x1x_unlocked_ioctl,
#else
	.ioctl = stk3x1x_ioctl,
#endif

};
/*----------------------------------------------------------------------------*/
static struct miscdevice stk3x1x_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "als_ps",
	.fops = &stk3x1x_fops,
};
/*----------------------------------------------------------------------------*/
static int stk3x1x_i2c_suspend(struct i2c_client *client, pm_message_t msg) 
{
	APS_FUN();    
/*
	if(msg.event == PM_EVENT_SUSPEND)
	{   
		if(!obj)
		{
			APS_ERR("null pointer!!\n");
			return -EINVAL;
		}
		
		atomic_set(&obj->als_suspend, 1);
		if((err = stk3x1x_enable_als(client, 0)))
		{
			APS_ERR("disable als: %d\n", err);
			return err;
		}

		atomic_set(&obj->ps_suspend, 1);
		if((err = stk3x1x_enable_ps(client, 0)))
		{
			APS_ERR("disable ps:  %d\n", err);
			return err;
		}
		
		stk3x1x_power(obj->hw, 0);
	}
	
*/
	return 0;
}
/*----------------------------------------------------------------------------*/
static int stk3x1x_i2c_resume(struct i2c_client *client)
{
	APS_FUN();
/*
	if(!obj)
	{
		APS_ERR("null pointer!!\n");
		return -EINVAL;
	}

	stk3x1x_power(obj->hw, 1);
	if((err = stk3x1x_init_client(client)))
	{
		APS_ERR("initialize client fail!!\n");
		return err;        
	}
	atomic_set(&obj->als_suspend, 0);
	if(test_bit(STK_BIT_ALS, &obj->enable))
	{
		if((err = stk3x1x_enable_als(client, 1)))
		{
			APS_ERR("enable als fail: %d\n", err);        
		}
	}
	atomic_set(&obj->ps_suspend, 0);
	if(test_bit(STK_BIT_PS,  &obj->enable))
	{
		if((err = stk3x1x_enable_ps(client, 1)))
		{
			APS_ERR("enable ps fail: %d\n", err);                
		}
	}
*/
	return 0;
}
/*----------------------------------------------------------------------------*/
static void stk3x1x_early_suspend(struct early_suspend *h) 
{   /*early_suspend is only applied for ALS*/
	int err;
	struct stk3x1x_priv *obj = container_of(h, struct stk3x1x_priv, early_drv);   	
	int old = atomic_read(&obj->state_val);
	APS_FUN();    

	if(!obj)
	{
		APS_ERR("null pointer!!\n");
		return;
	}
	
	if(old & STK_STATE_EN_ALS_MASK)
	{
		atomic_set(&obj->als_suspend, 1);    
		if((err = stk3x1x_enable_als(obj->client, 0)))
		{
			APS_ERR("disable als fail: %d\n", err); 
		}
	}
}
/*----------------------------------------------------------------------------*/
static void stk3x1x_late_resume(struct early_suspend *h)
{   /*early_suspend is only applied for ALS*/
	int err;
	hwm_sensor_data sensor_data;
	struct stk3x1x_priv *obj = container_of(h, struct stk3x1x_priv, early_drv);         
	
	memset(&sensor_data, 0, sizeof(sensor_data));
	APS_FUN();

	if(!obj)
	{
		APS_ERR("null pointer!!\n");
		return;
	}
	if(atomic_read(&obj->als_suspend))
	{
		atomic_set(&obj->als_suspend, 0);
		if(test_bit(STK_BIT_ALS, &obj->enable))
		{
			if((err = stk3x1x_enable_als(obj->client, 1)))
			{
				APS_ERR("enable als fail: %d\n", err);        

			}
		}
	}
}

int stk3x1x_ps_operate(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout)
{
	int err = 0;
	int value;
	hwm_sensor_data* sensor_data;
	struct stk3x1x_priv *obj = (struct stk3x1x_priv *)self;
	
	//APS_FUN(f);
	switch (command)
	{
		case SENSOR_DELAY:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Set delay parameter error!\n");
				err = -EINVAL;
			}
			// Do nothing
			break;

		case SENSOR_ENABLE:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Enable sensor parameter error!\n");
				err = -EINVAL;
			}
			else
			{				
				value = *(int *)buff_in;
				if(value)
				{
					if((err = stk3x1x_enable_ps(obj->client, 1)))
					{
						APS_ERR("enable ps fail: %d\n", err); 
						return -1;
					}
					set_bit(STK_BIT_PS, &obj->enable);
				}
				else
				{
					if((err = stk3x1x_enable_ps(obj->client, 0)))
					{
						APS_ERR("disable ps fail: %d\n", err); 
						return -1;
					}
					clear_bit(STK_BIT_PS, &obj->enable);
				}
			}
			break;

		case SENSOR_GET_DATA:
			if((buff_out == NULL) || (size_out< sizeof(hwm_sensor_data)))
			{
				APS_ERR("get sensor data parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				sensor_data = (hwm_sensor_data *)buff_out;				
				
				if((err = stk3x1x_read_ps(obj->client, &obj->ps)))
				{
					err = -1;
				}
				else
				{
					value = stk3x1x_get_ps_value(obj, obj->ps);
					if(value < 0)
					{
						err = -1;
					}
					else
					{
						sensor_data->values[0] = value;
						sensor_data->value_divide = 1;
						sensor_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;
#ifdef STK_PS_POLLING_LOG						
						APS_LOG("%s:ps raw 0x%x -> value 0x%x \n",__FUNCTION__, obj->ps, sensor_data->values[0]);					
#endif				
					}	
				}				
			}
			break;
		default:
			APS_ERR("proxmy sensor operate function no this parameter %d!\n", command);
			err = -1;
			break;
	}
	
	return err;
}

int stk3x1x_als_operate(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout)
{
	int err = 0;
	int value;
	hwm_sensor_data* sensor_data;
	struct stk3x1x_priv *obj = (struct stk3x1x_priv *)self;
	
	//APS_FUN(f);
	switch (command)
	{
		case SENSOR_DELAY:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Set delay parameter error!\n");
				err = -EINVAL;
			}
			// Do nothing
			break;

		case SENSOR_ENABLE:
			if((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Enable sensor parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				value = *(int *)buff_in;				
				if(value)
				{
					if((err = stk3x1x_enable_als(obj->client, 1)))
					{
						APS_ERR("enable als fail: %d\n", err); 
						return -1;
					}
					set_bit(STK_BIT_ALS, &obj->enable);
				}
				else
				{
					if((err = stk3x1x_enable_als(obj->client, 0)))
					{
						APS_ERR("disable als fail: %d\n", err); 
						return -1;
					}
					clear_bit(STK_BIT_ALS, &obj->enable);
				}
				
			}
			break;

		case SENSOR_GET_DATA:
			if((buff_out == NULL) || (size_out< sizeof(hwm_sensor_data)))
			{
				APS_ERR("get sensor data parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				sensor_data = (hwm_sensor_data *)buff_out;
								
				if((err = stk3x1x_read_als(obj->client, &obj->als)))
				{
					err = -1;
				}
				else
				{
				    #if defined(MTK_AAL_SUPPORT)
					sensor_data->values[0] = obj->als;
					#else
					sensor_data->values[0] = stk3x1x_get_als_value(obj, obj->als);
					#endif
					sensor_data->value_divide = 1;
					sensor_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;
				}				
			}
			break;
		default:
			APS_ERR("light sensor operate function no this parameter %d!\n", command);
			err = -1;
			break;
	}
	
	return err;
}


/*----------------------------------------------------------------------------*/
#if (LINUX_VERSION_CODE<KERNEL_VERSION(3,0,0))	
static int stk3x1x_i2c_detect(struct i2c_client *client, int kind, struct i2c_board_info *info) 
{    
	strcpy(info->type, stk3x1x_DEV_NAME);
	return 0;
}
#endif
/*----------------------------------------------------------------------------*/
static int stk3x1x_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct stk3x1x_priv *obj;
	struct hwmsen_object obj_ps, obj_als;
	int err = 0;

	APS_LOG("%s: driver version: %s\n", __FUNCTION__, DRIVER_VERSION);

	if(!(obj = kzalloc(sizeof(*obj), GFP_KERNEL)))
	{
		err = -ENOMEM;
		goto exit;
	}
	memset(obj, 0, sizeof(*obj));
	stk3x1x_obj = obj;
	obj->hw = get_cust_alsps_hw();
	stk3x1x_get_addr(obj->hw, &obj->addr);

	INIT_DELAYED_WORK(&obj->eint_work, stk3x1x_eint_work);
	obj->client = client;
	i2c_set_clientdata(client, obj);	
	atomic_set(&obj->als_debounce, 200);
	atomic_set(&obj->als_deb_on, 0);
	atomic_set(&obj->als_deb_end, 0);
	atomic_set(&obj->ps_debounce, 3);
	atomic_set(&obj->ps_deb_on, 0);
	atomic_set(&obj->ps_deb_end, 0);
	atomic_set(&obj->ps_mask, 0);
	atomic_set(&obj->trace, 0x00);
	atomic_set(&obj->als_suspend, 0);

	atomic_set(&obj->state_val, SKT3X1X_STATE_VAL);
	atomic_set(&obj->psctrl_val, SKT3X1X_PSCTRL_VAL);
	atomic_set(&obj->alsctrl_val, SKT3X1X_ALSTRL_VAL);
	obj->ledctrl_val = SKT3X1X_LEDCTRL_VAL;
	obj->wait_val = SKT3X1X_WAIT_VAL;
	obj->int_val = 0;
	obj->first_boot = true;		
#ifdef STK_TUNE0	
	obj->psi_set = 0;
#elif defined(STK_TUNE1)	
	obj->state = 0;	
	obj->kdata = 0;
	obj->kdata1 = 0;
#endif		 

	atomic_set(&obj->recv_reg, 0);  
	
	if(obj->hw->polling_mode_ps == 0)
	{
		APS_LOG("%s: enable PS interrupt\n", __FUNCTION__);
	}
	obj->int_val |= STK_INT_PS_MODE1;
	
	if(obj->hw->polling_mode_als == 0)
	{
	  obj->int_val |= STK_INT_ALS;		
	  APS_LOG("%s: enable ALS interrupt\n", __FUNCTION__);
	}	

#if (defined(STK_TUNE0) || defined(STK_TUNE1))
	obj->int_val |= 0x10;
#endif
	
	APS_LOG("%s: state_val=0x%x, psctrl_val=0x%x, alsctrl_val=0x%x, ledctrl_val=0x%x, wait_val=0x%x, int_val=0x%x\n", 
		__FUNCTION__, atomic_read(&obj->state_val), atomic_read(&obj->psctrl_val), atomic_read(&obj->alsctrl_val), 
		obj->ledctrl_val, obj->wait_val, obj->int_val);
	
	APS_LOG("stk3x1x_i2c_probe() OK!\n");
	obj->enable = 0;
	obj->pending_intr = 0;
	obj->als_level_num = sizeof(obj->hw->als_level)/sizeof(obj->hw->als_level[0]);
	obj->als_value_num = sizeof(obj->hw->als_value)/sizeof(obj->hw->als_value[0]);   
	BUG_ON(sizeof(obj->als_level) != sizeof(obj->hw->als_level));
	memcpy(obj->als_level, obj->hw->als_level, sizeof(obj->als_level));
	BUG_ON(sizeof(obj->als_value) != sizeof(obj->hw->als_value));
	memcpy(obj->als_value, obj->hw->als_value, sizeof(obj->als_value));
	atomic_set(&obj->i2c_retry, 3);
	if(atomic_read(&obj->state_val) & STK_STATE_EN_ALS_MASK)
	{
		set_bit(STK_BIT_ALS, &obj->enable);
	}
	
	if(atomic_read(&obj->state_val) & STK_STATE_EN_PS_MASK)
	{
		set_bit(STK_BIT_PS, &obj->enable);
	}
	
	stk3x1x_i2c_client = client;

	
	if((err = stk3x1x_init_client(client)))
	{
		goto exit_init_failed;
	}
	
	if((err = misc_register(&stk3x1x_device)))
	{
		APS_ERR("stk3x1x_device register failed\n");
		goto exit_misc_device_register_failed;
	}

	if((err = stk3x1x_create_attr(&stk3x1x_alsps_driver.driver)))
	{
		APS_ERR("create attribute err = %d\n", err);
		goto exit_create_attr_failed;
	}
	obj_ps.self = stk3x1x_obj;
	if(1 == obj->hw->polling_mode_ps)
	{
		obj_ps.polling = 1;
		wake_lock_init(&ps_lock,WAKE_LOCK_SUSPEND,"ps wakelock");
	}
	else
	{
	  obj_ps.polling = 0;//interrupt mode
	}
	
	obj_ps.sensor_operate = stk3x1x_ps_operate;
	if((err = hwmsen_attach(ID_PROXIMITY, &obj_ps)))
	{
		APS_ERR("attach fail = %d\n", err);
		goto exit_create_attr_failed;
	}
	
	obj_als.self = stk3x1x_obj;
	if(1 == obj->hw->polling_mode_als)
	{
	  obj_als.polling = 1;
	}
	else
	{
	  obj_als.polling = 0;//interrupt mode
	}
	obj_als.sensor_operate = stk3x1x_als_operate;
	if((err = hwmsen_attach(ID_LIGHT, &obj_als)))
	{
		APS_ERR("attach fail = %d\n", err);
		goto exit_create_attr_failed;
	}


#if defined(CONFIG_HAS_EARLYSUSPEND)
	obj->early_drv.level    = EARLY_SUSPEND_LEVEL_DISABLE_FB - 1,
	obj->early_drv.suspend  = stk3x1x_early_suspend,
	obj->early_drv.resume   = stk3x1x_late_resume,    
	register_early_suspend(&obj->early_drv);
#endif

	APS_LOG("%s: OK\n", __FUNCTION__);

	return 0;

	exit_create_attr_failed:
	misc_deregister(&stk3x1x_device);
	exit_misc_device_register_failed:
	exit_init_failed:
	//i2c_detach_client(client);
//	exit_kfree:
	kfree(obj);
	exit:
	stk3x1x_i2c_client = NULL;           
	#ifdef MT6516        
	MT6516_EINTIRQMask(CUST_EINT_ALS_NUM);  /*mask interrupt if fail*/
	#endif
	APS_ERR("%s: err = %d\n", __FUNCTION__, err);
	return err;
}
/*----------------------------------------------------------------------------*/
static int stk3x1x_i2c_remove(struct i2c_client *client)
{
	int err;	
	
	if((err = stk3x1x_delete_attr(&stk3x1x_i2c_driver.driver)))
	{
		APS_ERR("stk3x1x_delete_attr fail: %d\n", err);
	} 

	if((err = misc_deregister(&stk3x1x_device)))
	{
		APS_ERR("misc_deregister fail: %d\n", err);    
	}
	
	stk3x1x_i2c_client = NULL;
	i2c_unregister_device(client);
	kfree(i2c_get_clientdata(client));

	return 0;
}
/*----------------------------------------------------------------------------*/
static int stk3x1x_probe(struct platform_device *pdev) 
{
	struct alsps_hw *hw = get_cust_alsps_hw();
	struct stk3x1x_i2c_addr addr;

	stk3x1x_power(hw, 1);    
	stk3x1x_get_addr(hw, &addr);
#if (LINUX_VERSION_CODE<KERNEL_VERSION(3,0,0))	
	stk3x1x_force[0] = hw->i2c_num;
	stk3x1x_force[1] = hw->i2c_addr[0];
#endif
	if(i2c_add_driver(&stk3x1x_i2c_driver))
	{
		APS_ERR("add driver error\n");
		return -1;
	} 

	return 0;
}
/*----------------------------------------------------------------------------*/
static int stk3x1x_remove(struct platform_device *pdev)
{
	struct alsps_hw *hw = get_cust_alsps_hw();
	APS_FUN();    
	stk3x1x_power(hw, 0);    
	i2c_del_driver(&stk3x1x_i2c_driver);
	return 0;
}
/*----------------------------------------------------------------------------*/
static struct platform_driver stk3x1x_alsps_driver = {
	.probe      = stk3x1x_probe,
	.remove     = stk3x1x_remove,    
	.driver     = {
		.name  = "als_ps",
#if (LINUX_VERSION_CODE<KERNEL_VERSION(3,0,0))	
		.owner = THIS_MODULE,
#endif
	}
};
/*----------------------------------------------------------------------------*/
static int __init stk3x1x_init(void)
{
#if (LINUX_VERSION_CODE>=KERNEL_VERSION(3,0,0))	
	struct alsps_hw *hw = get_cust_alsps_hw();
	APS_LOG("%s: i2c_number=%d\n", __func__,hw->i2c_num); 
	i2c_register_board_info(hw->i2c_num, &i2c_stk3x1x, 1);	
#endif
	APS_FUN();
	if(platform_driver_register(&stk3x1x_alsps_driver))
	{
		APS_ERR("failed to register driver");
		return -ENODEV;
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
static void __exit stk3x1x_exit(void)
{
	APS_FUN();
	platform_driver_unregister(&stk3x1x_alsps_driver);
}
/*----------------------------------------------------------------------------*/
module_init(stk3x1x_init);
module_exit(stk3x1x_exit);
/*----------------------------------------------------------------------------*/
MODULE_AUTHOR("MingHsien Hsieh");
MODULE_DESCRIPTION("SensorTek stk3x1x proximity and light sensor driver");
MODULE_LICENSE("GPL");
