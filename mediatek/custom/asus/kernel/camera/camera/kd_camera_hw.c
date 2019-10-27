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
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <linux/xlog.h>

#include "kd_camera_hw.h"

#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_camera_feature.h"

/******************************************************************************
 * Debug configuration
******************************************************************************/
#define PFX "[kd_camera_hw]"
#define PK_DBG_NONE(fmt, arg...)    do {} while (0)
#define PK_DBG_FUNC(fmt, arg...)    printk(KERN_INFO PFX "%s: " fmt, __FUNCTION__ ,##arg)

#define DEBUG_CAMERA_HW_K
#ifdef DEBUG_CAMERA_HW_K
#define PK_DBG PK_DBG_FUNC
#define PK_ERR(fmt, arg...)         printk(KERN_ERR PFX "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_XLOG_INFO(fmt, args...) \
                do {    \
                    xlog_printk(ANDROID_LOG_INFO, "kd_camera_hw", fmt, ##args); \
                } while(0)
#else
#define PK_DBG(a,...)
#define PK_ERR(a,...)
#define PK_XLOG_INFO(fmt, args...)
#endif


#define COMPAL_CAMERA

#ifdef COMPAL_CAMERA

#define CAM1_RESET					GPIO_CAMERA_CMRST_PIN
#define CAM1_RESET_MODE			GPIO_CAMERA_CMRST_PIN_M_GPIO
#define CAM1_RESET_ENABLE		GPIO_OUT_ZERO
#define CAM1_RESET_DISABLE	GPIO_OUT_ONE
#define CAM1_PWDN						GPIO_CAMERA_CMPDN_PIN
#define CAM1_PWDN_MODE			GPIO_CAMERA_CMPDN_PIN_M_GPIO
#define CAM1_PWDN_ENABLE		GPIO_OUT_ZERO
#define CAM1_PWDN_DISABLE		GPIO_OUT_ONE

#define CAM2_RESET					GPIO_CAMERA_CMRST1_PIN
#define CAM2_RESET_MODE			GPIO_CAMERA_CMRST1_PIN_M_GPIO
#define CAM2_RESET_ENABLE		GPIO_OUT_ONE
#define CAM2_RESET_DISABLE	GPIO_OUT_ZERO
#define CAM2_PWDN						GPIO_CAMERA_CMPDN1_PIN
#define CAM2_PWDN_MODE			GPIO_CAMERA_CMPDN1_PIN_M_GPIO
#define CAM2_PWDN_ENABLE		GPIO_OUT_ONE
#define CAM2_PWDN_DISABLE		GPIO_OUT_ZERO

#define myLog(fmt, args...) printk(KERN_INFO, fmt, ##args)

void PrepareGPIO_Cam1()
{
	mt_set_gpio_mode(CAM1_RESET, CAM1_RESET_MODE);
	mt_set_gpio_mode(CAM1_PWDN, CAM1_PWDN_MODE);
	mt_set_gpio_dir(CAM1_RESET, GPIO_DIR_OUT);
	mt_set_gpio_dir(CAM1_PWDN, GPIO_DIR_OUT);
}

void PrepareGPIO_Cam2()
{
	//mt_set_gpio_mode(CAM2_RESET, CAM2_RESET_MODE);
	mt_set_gpio_mode(CAM2_PWDN, CAM2_PWDN_MODE);
	//mt_set_gpio_dir(CAM2_RESET, GPIO_DIR_OUT);
	mt_set_gpio_dir(CAM2_PWDN, GPIO_DIR_OUT);
}

void PowerOn(char *modeName)
{
	hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800, modeName);
	mdelay(10);
	hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800, modeName);
	mdelay(10);
}

void PowerOff(char *modeName)
{
	hwPowerDown(CAMERA_POWER_VCAM_D2, modeName);
	hwPowerDown(CAMERA_POWER_VCAM_A, modeName);
}

int EnableCam1(char *modeName)
{
	PrepareGPIO_Cam1();
	mt_set_gpio_out(CAM1_RESET, CAM1_RESET_DISABLE);
	mt_set_gpio_out(CAM1_PWDN, CAM1_PWDN_ENABLE);
	PrepareGPIO_Cam2();
	mt_set_gpio_out(CAM2_PWDN, CAM2_PWDN_ENABLE);
	PowerOn(modeName);
	mt_set_gpio_out(CAM1_PWDN, CAM1_PWDN_DISABLE);
	mdelay(25);
	return 0;
}

int DisableCam1(char *modeName)
{
	PrepareGPIO_Cam1();
	mt_set_gpio_out(CAM1_PWDN, CAM1_PWDN_ENABLE);
	PowerOff(modeName);
	return 0;
}

int EnableCam2(char *modeName)
{
	PrepareGPIO_Cam1();
	mt_set_gpio_out(CAM1_PWDN, CAM1_PWDN_ENABLE);
	PrepareGPIO_Cam2();
	mt_set_gpio_out(CAM2_PWDN, CAM2_PWDN_ENABLE);
	PowerOn(modeName);
	mt_set_gpio_out(CAM2_PWDN, CAM2_PWDN_DISABLE);
	mdelay(25);
	return 0;
}

int DisableCam2(char *modeName)
{
	PrepareGPIO_Cam2();
	mt_set_gpio_mode(CAM2_PWDN, CAM2_PWDN_ENABLE);
	PowerOff(modeName);

	return 0;
}

int kdCISModulePowerOn(CAMERA_DUAL_CAMERA_SENSOR_ENUM id, char *sensorName, BOOL on, char *modeName)
{
	PK_DBG_FUNC("ZARA_CAM: idx=%d, on=%d, sensor=%s, mode=%s\n", id, on, sensorName, modeName);

	switch(id)
	{
		case DUAL_CAMERA_MAIN_SENSOR:
			return on ? EnableCam1(modeName) : DisableCam1(modeName);
		case DUAL_CAMERA_SUB_SENSOR:
			return on ? EnableCam2(modeName) : DisableCam2(modeName);
		default:
			return -EIO;
	}
}


#else

int kdCISModulePowerOn(CAMERA_DUAL_CAMERA_SENSOR_ENUM SensorIdx, char *currSensorName, BOOL On, char* mode_name)
{
	u32 pinSetIdx = 0;//default main sensor

	#define IDX_PS_CMRST 0
	#define IDX_PS_CMPDN 4

	#define IDX_PS_MODE 1
	#define IDX_PS_ON   2
	#define IDX_PS_OFF  3


	u32 pinSet[3][8] = {
		//for main sensor
		{
			GPIO_CAMERA_CMRST_PIN,
			GPIO_CAMERA_CMRST_PIN_M_GPIO,   /* mode */
			GPIO_OUT_ONE,                   /* ON state */
			GPIO_OUT_ZERO,                  /* OFF state */
			GPIO_CAMERA_CMPDN_PIN,
			GPIO_CAMERA_CMPDN_PIN_M_GPIO,
			GPIO_OUT_ONE,
			GPIO_OUT_ZERO,
		},
		//for sub sensor
		{
			GPIO_CAMERA_CMRST1_PIN,
			GPIO_CAMERA_CMRST1_PIN_M_GPIO,
			GPIO_OUT_ZERO,
			GPIO_OUT_ONE,
			GPIO_CAMERA_CMPDN1_PIN,
			GPIO_CAMERA_CMPDN1_PIN_M_GPIO,
			GPIO_OUT_ZERO,
			GPIO_OUT_ONE,
		},
		//for main_2 sensor
		{
			GPIO_CAMERA_INVALID,	//GPIO_CAMERA_2_CMRST_PIN,
			GPIO_CAMERA_2_CMRST_PIN_M_GPIO,   /* mode */
			GPIO_OUT_ONE,                   /* ON state */
			GPIO_OUT_ZERO,                  /* OFF state */
			GPIO_CAMERA_INVALID,	//GPIO_CAMERA_2_CMPDN_PIN,
			GPIO_CAMERA_2_CMPDN_PIN_M_GPIO,
			GPIO_OUT_ZERO,
			GPIO_OUT_ONE,
		}
	};

	if (DUAL_CAMERA_MAIN_SENSOR == SensorIdx){
		pinSetIdx = 0;
	}
	else if (DUAL_CAMERA_SUB_SENSOR == SensorIdx) {
		pinSetIdx = 1;
	}
	else if (DUAL_CAMERA_MAIN_2_SENSOR == SensorIdx) {
		pinSetIdx = 2;
		return 0;	//Skip Main2 camera flow
	}

	//power ON
	if (On) {
		//Disable active camera
		if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
		if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
		if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
		if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
		if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} //high == power down lens module
		mdelay(1);
		if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //low == reset sensor
		mdelay(2);
		//~Disable active camera

		PK_DBG("sensor:%d\n",pinSetIdx);

		if(pinSetIdx==1) //(currSensorName && (0 == strcmp(SENSOR_DRVNAME_OV7675_YUV,currSensorName)))
		{
			PK_DBG("[ON_OV7675_YUV case 2.8V]sensorIdx:%d \n",SensorIdx);

			if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))	//VDD_IO, VOVDD
			{
				PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
			}
			
			mdelay(1);
						
			if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))  //VDD_A
			{
				PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
			}
			
		}
		else
		{

			if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
			if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
			if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
			if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}

			if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
			mdelay(1);

			if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
			mdelay(2);
		    
			if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))
			{
				PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
			}
			if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))
			{
				PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
			}
			mdelay(5);
		}


		#if 1
		//disable inactive sensor
		if (GPIO_CAMERA_INVALID != pinSet[1-pinSetIdx][IDX_PS_CMRST]) {
			if(mt_set_gpio_mode(pinSet[1-pinSetIdx][IDX_PS_CMRST],pinSet[1-pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
			if(mt_set_gpio_mode(pinSet[1-pinSetIdx][IDX_PS_CMPDN],pinSet[1-pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
			if(mt_set_gpio_dir(pinSet[1-pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
			if(mt_set_gpio_dir(pinSet[1-pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
			if(mt_set_gpio_out(pinSet[1-pinSetIdx][IDX_PS_CMRST],pinSet[1-pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //low == reset sensor
			if(mt_set_gpio_out(pinSet[1-pinSetIdx][IDX_PS_CMPDN],pinSet[1-pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} //high == power down lens module
		}        
		#endif

		//enable active sensor
		//RST pin
		if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST]) {
			if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
			if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
			if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
			if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}

			if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
			msleep(1);
			
			if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
			msleep(2);

			//PDN pin
			if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
			mdelay(10);
			
			if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_ON])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
			msleep(5);

		}
	}
	else {
		//PK_DBG("[OFF]sensorIdx:%d \n",SensorIdx);
		if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST]) {
			if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
			if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
			if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
			if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
			if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} //high == power down lens module
			mdelay(1);
			if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //low == reset sensor
			mdelay(2);
		}
		if(pinSetIdx==0)// (currSensorName && (0 == strcmp(SENSOR_DRVNAME_HI542_RAW,currSensorName)))
		{

		}

		if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name)) {
			PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
			//return -EIO;
			goto _kdCISModulePowerOn_exit_;
		}

		if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name))
		{
			PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
			//return -EIO;
			goto _kdCISModulePowerOn_exit_;
		}  

		//For Power Saving
		#if 0
		if(pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF] != GPIO_OUT_ZERO)
		{
			mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN], GPIO_OUT_ZERO);
		}
		if(pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF] != GPIO_OUT_ZERO)
		{
			mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST], GPIO_OUT_ZERO);
		}
		#endif
		//~For Power Saving
	}//

	return 0;

	_kdCISModulePowerOn_exit_:
	return -EIO;
}

#endif	// COMPAL_CAMERA

EXPORT_SYMBOL(kdCISModulePowerOn);


//!--
//




