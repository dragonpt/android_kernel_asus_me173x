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
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
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

/*****************************************************************************
 *
 * Filename:
 * ---------
 *   sensor.h
 *
 * Project:
 * --------
 *   DUMA
 *
 * Description:
 * ------------
 *   CMOS sensor header file
 *
 ****************************************************************************/
#ifndef _OV9726MIPIRAW_SENSOR_H
#define _OV9726MIPIRAW_SENSOR_H

//#define OV9726MIPI_DEBUG
//#define OV9726MIPI_DRIVER_TRACE
//#define OV9726MIPI_TEST_PATTEM
#ifdef OV9726MIPI_DEBUG
//#define SENSORDB printk
#else
//#define SENSORDB(x,...)
#endif

#define OV9726MIPI_FACTORY_START_ADDR 0
#define OV9726MIPI_ENGINEER_START_ADDR 10
 
typedef enum OV9726MIPI_group_enum
{
  OV9726MIPI_PRE_GAIN = 0,
  OV9726MIPI_CMMCLK_CURRENT,
  OV9726MIPI_FRAME_RATE_LIMITATION,
  OV9726MIPI_REGISTER_EDITOR,
  OV9726MIPI_GROUP_TOTAL_NUMS
} OV9726MIPI_FACTORY_GROUP_ENUM;

typedef enum OV9726MIPI_register_index
{
  OV9726MIPI_SENSOR_BASEGAIN = OV9726MIPI_FACTORY_START_ADDR,
  OV9726MIPI_PRE_GAIN_R_INDEX,
  OV9726MIPI_PRE_GAIN_Gr_INDEX,
  OV9726MIPI_PRE_GAIN_Gb_INDEX,
  OV9726MIPI_PRE_GAIN_B_INDEX,
  OV9726MIPI_FACTORY_END_ADDR
} OV9726MIPI_FACTORY_REGISTER_INDEX;

typedef enum OV9726MIPI_engineer_index
{
  OV9726MIPI_CMMCLK_CURRENT_INDEX = OV9726MIPI_ENGINEER_START_ADDR,
  OV9726MIPI_ENGINEER_END
} OV9726MIPI_FACTORY_ENGINEER_INDEX;

typedef struct _sensor_data_struct
{
  SENSOR_REG_STRUCT reg[OV9726MIPI_ENGINEER_END];
  SENSOR_REG_STRUCT cct[OV9726MIPI_FACTORY_END_ADDR];
} sensor_data_struct;

/* SENSOR PREVIEW/CAPTURE VT CLOCK */
#define OV9726MIPI_PREVIEW_CLK                     42000000
#define OV9726MIPI_CAPTURE_CLK                     42000000

#define OV9726MIPI_COLOR_FORMAT                    SENSOR_OUTPUT_FORMAT_RAW_B//SENSOR_OUTPUT_FORMAT_RAW_R

#define OV9726MIPI_MIN_ANALOG_GAIN				1	/* 1x */
#define OV9726MIPI_MAX_ANALOG_GAIN				16	/* 32x */


/* FRAME RATE UNIT */
#define OV9726MIPI_FPS(x)                          (10 * (x))

/* SENSOR PIXEL/LINE NUMBERS IN ONE PERIOD */
//#define OV9726MIPI_FULL_PERIOD_PIXEL_NUMS          2700 /* 9 fps */
#define OV9726MIPI_FULL_PERIOD_PIXEL_NUMS          1280 /* 8 fps */
#define OV9726MIPI_FULL_PERIOD_LINE_NUMS           720
#define OV9726MIPI_PV_PERIOD_PIXEL_NUMS            1280 /* 30 fps */
#define OV9726MIPI_PV_PERIOD_LINE_NUMS             720

/* SENSOR START/END POSITION */
#define OV9726MIPI_FULL_X_START                    2
#define OV9726MIPI_FULL_Y_START                    2
#define OV9726MIPI_IMAGE_SENSOR_FULL_WIDTH         (1280) /* 2560 */
#define OV9726MIPI_IMAGE_SENSOR_FULL_HEIGHT        (720) /* 1920 */
#define OV9726MIPI_PV_X_START                      2
#define OV9726MIPI_PV_Y_START                      2
#define OV9726MIPI_IMAGE_SENSOR_PV_WIDTH           (1280) /* 1264 */
#define OV9726MIPI_IMAGE_SENSOR_PV_HEIGHT          (720) /* 948 */

#define OV9726MIPI_DEFAULT_DUMMY_PIXELS			(384)
#define OV9726MIPI_DEFAULT_DUMMY_LINES			(120)

/* SENSOR READ/WRITE ID */
#define OV9726MIPI_WRITE_ID (0x20)


/* SENSOR ID */
//#define OV9726MIPI_SENSOR_ID						(0x9726)

/* SENSOR PRIVATE STRUCT */
typedef struct OV9726MIPI_sensor_STRUCT
{
  MSDK_SENSOR_CONFIG_STRUCT cfg_data;
  sensor_data_struct eng; /* engineer mode */
  MSDK_SENSOR_ENG_INFO_STRUCT eng_info;
  kal_uint8 mirror;
  kal_bool pv_mode;
  kal_bool video_mode;
  kal_uint16 normal_fps; /* video normal mode max fps */
  kal_uint16 night_fps; /* video night mode max fps */
  kal_uint16 shutter;
  kal_uint16 gain;
  kal_uint32 pclk;
  kal_uint16 frame_height;
  kal_uint16 default_height;
  kal_uint16 line_length;  
} OV9726MIPI_sensor_struct;

//export functions
UINT32 OV9726MIPIOpen(void);
UINT32 OV9726MIPIControl(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *pImageWindow, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 OV9726MIPIFeatureControl(MSDK_SENSOR_FEATURE_ENUM FeatureId, UINT8 *pFeaturePara,UINT32 *pFeatureParaLen);
UINT32 OV9726MIPIGetInfo(MSDK_SCENARIO_ID_ENUM ScenarioId, MSDK_SENSOR_INFO_STRUCT *pSensorInfo, MSDK_SENSOR_CONFIG_STRUCT *pSensorConfigData);
UINT32 OV9726MIPIGetResolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *pSensorResolution);
UINT32 OV9726MIPIClose(void);

#define Sleep(ms) mdelay(ms)

#endif 
