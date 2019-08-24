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

/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#include <utils/Log.h>
#include <fcntl.h>
#include <math.h>

#include "camera_custom_nvram.h"
#include "camera_custom_sensor.h"
#include "image_sensor.h"
#include "kd_imgsensor_define.h"
#include "camera_AE_PLineTable_ov5648mipiraw.h"
#include "camera_info_ov5648mipiraw.h"
#include "camera_custom_AEPlinetable.h"

const NVRAM_CAMERA_ISP_PARAM_STRUCT CAMERA_ISP_DEFAULT_VALUE =
{{
    //Version
    Version: NVRAM_CAMERA_PARA_FILE_VERSION,

    //SensorId
    SensorId: SENSOR_ID,
    ISPComm:{
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    	}
    },
    ISPPca: {
        #include INCLUDE_FILENAME_ISP_PCA_PARAM
    },
    ISPRegs:{
        #include INCLUDE_FILENAME_ISP_REGS_PARAM
    },
    ISPMfbMixer:{{
        {//00: MFB mixer for ISO 100
            0x00000000, 0x00000000
        },
        {//01: MFB mixer for ISO 200
            0x00000000, 0x00000000
        },
        {//02: MFB mixer for ISO 400
            0x00000000, 0x00000000
        },
        {//03: MFB mixer for ISO 800
            0x00000000, 0x00000000
        },
        {//04: MFB mixer for ISO 1600
            0x00000000, 0x00000000
        },
        {//05: MFB mixer for ISO 2400
            0x00000000, 0x00000000
        },
        {//06: MFB mixer for ISO 3200
            0x00000000, 0x00000000
        }
    }},
    ISPCcmPoly22:{
        68280,    // i4R_AVG
        12197,    // i4R_STD
        82180,    // i4B_AVG
        23500,    // i4B_STD
        {  // i4P00[9]
            4824000, -1780000, -488000, -534000, 3088000, 4000, 304000, -2008000, 4266000
        },
        {  // i4P10[9]
            651459, -815811, 170041, 58875, -227543, 162520, 33073, 239927, -274662
        },
        {  // i4P01[9]
            338168, -447912, 113881, -150193, -132417, 276342, -26420, -266750, 289349
        },
        {  // i4P20[9]
            0, 0, 0, 0, 0, 0, 0, 0, 0
        },
        {  // i4P11[9]
            0, 0, 0, 0, 0, 0, 0, 0, 0
        },
        {  // i4P02[9]
            0, 0, 0, 0, 0, 0, 0, 0, 0
        }        
    }
}};

const NVRAM_CAMERA_3A_STRUCT CAMERA_3A_NVRAM_DEFAULT_VALUE =
{
    NVRAM_CAMERA_3A_FILE_VERSION, // u4Version
    SENSOR_ID, // SensorId

    // AE NVRAM
    {
        // rDevicesInfo
        {
            1136,    // u4MinGain, 1024 base = 1x
            10240,    // u4MaxGain, 16x
            51,    // u4MiniISOGain, ISOxx  
            64,    // u4GainStepUnit, 1x/8 
            35,    // u4PreExpUnit 
            30,    // u4PreMaxFrameRate
            35,    // u4VideoExpUnit  
            30,    // u4VideoMaxFrameRate 
            1024,    // u4Video2PreRatio, 1024 base = 1x 
            35,    // u4CapExpUnit 
            15,    // u4CapMaxFrameRate
            1024,    // u4Cap2PreRatio, 1024 base = 1x
            24,    // u4LensFno, Fno = 2.8
            350    // u4FocusLength_100x
         },
         // rHistConfig
        {
            2,   // u4HistHighThres
            40,  // u4HistLowThres
            3,   // u4MostBrightRatio
            1,   // u4MostDarkRatio
            160, // u4CentralHighBound
            20,  // u4CentralLowBound
            {240, 230, 220, 210, 200}, // u4OverExpThres[AE_CCT_STRENGTH_NUM]
            {86, 108, 138, 148, 170},  // u4HistStretchThres[AE_CCT_STRENGTH_NUM]
            {18, 22, 26, 30, 34}       // u4BlackLightThres[AE_CCT_STRENGTH_NUM]
        },
        // rCCTConfig
        {
            TRUE,            // bEnableBlackLight
            TRUE,            // bEnableHistStretch
            FALSE,           // bEnableAntiOverExposure
            TRUE,            // bEnableTimeLPF
            FALSE,    // bEnableCaptureThres
            FALSE,    // bEnableVideoThres
            FALSE,    // bEnableStrobeThres
            47,                // u4AETarget
            47,    // u4StrobeAETarget
            50,                // u4InitIndex
            4,                 // u4BackLightWeight
            32,                // u4HistStretchWeight
            4,                 // u4AntiOverExpWeight
            2,                 // u4BlackLightStrengthIndex
            3, //2                // u4HistStretchStrengthIndex
            2,                 // u4AntiOverExpStrengthIndex
            3,    // u4TimeLPFStrengthIndex
            {1, 3, 5, 7, 8}, // u4LPFConvergeTable[AE_CCT_STRENGTH_NUM]
            90,                // u4InDoorEV = 9.0, 10 base
            -3,    // i4BVOffset delta BV = value/10 
            54,//64,    // u4PreviewFlareOffset
            54,//64,    // u4CaptureFlareOffset
            5,    // u4CaptureFlareThres
            64,//64,    // u4VideoFlareOffset
            5,    // u4VideoFlareThres
            64,//64,    // u4StrobeFlareOffset
            2,    // u4StrobeFlareThres
            8,    // u4PrvMaxFlareThres
            0,    // u4PrvMinFlareThres
            8,    // u4VideoMaxFlareThres
            0,    // u4VideoMinFlareThres
            18,    // u4FlatnessThres    // 10 base for flatness condition.
            55    // u4FlatnessStrength
         }
    },

    // AWB NVRAM
    {
    	// AWB calibration data
    	{
    		// rUnitGain (unit gain: 1.0 = 512)
    		{
    			0,	// i4R
    			0,	// i4G
    			0	// i4B
    		},
    		// rGoldenGain (golden sample gain: 1.0 = 512)
    		{
	            0,	// i4R
	            0,	// i4G
	            0	// i4B
            },
    		// rTuningUnitGain (Tuning sample unit gain: 1.0 = 512)
    		{
	            0,	// i4R
	            0,	// i4G
	            0	// i4B
            },
            // rD65Gain (D65 WB gain: 1.0 = 512)
            {
                709,    // i4R   //749---->709
                512,    // i4G
                575    // i4B
    		}
    	},
    	// Original XY coordinate of AWB light source
    	{
		    // Strobe
		    {
                -18,    // i4X
                -263    // i4Y
            },
            // Horizon
            {
                -341,    // i4X
                -259    // i4Y
            },
            // A
            {
                -264,    // i4X
                -285    // i4Y
            },
            // TL84
            {
                -150,    // i4X
                -291    // i4Y
            },
            // CWF
            {
                -95,    // i4X
                -382    // i4Y
            },
            // DNP
            {
                -18,    // i4X
                -263    // i4Y
            },
            // D65
            {
                98,    // i4X
                -183    // i4Y
            },
            // DF
            {
                0,    // i4X
                0    // i4Y
            }
        },
        // Rotated XY coordinate of AWB light source
        {
            // Strobe
            {
                -81,    // i4X
                -250    // i4Y
            },
            // Horizon
            {
                -393,    // i4X
                -168    // i4Y
            },
            // A
            {
                -325,    // i4X
                -212    // i4Y
            },
            // TL84
            {
                -216,    // i4X
                -246    // i4Y
            },
            // CWF
            {
                -185,    // i4X
                -347    // i4Y
            },
            // DNP
            {
                -81,    // i4X
                -250    // i4Y
            },
            // D65
            {
                51,    // i4X
                -201    // i4Y
            },
            // DF
            {
                0,    // i4X
                0    // i4Y
            }
        },
        // AWB gain of AWB light source
        {
            // Strobe 
            {
                713,    // i4R
                512,    // i4G
                749    // i4B
            },
            // Horizon 
            {
                512,    // i4R
                572,    // i4G
                1290    // i4B
            },
            // A 
            {
                527,    // i4R
                512,    // i4G
                1077    // i4B
            },
            // TL84 
            {
                620,    // i4R
                512,    // i4G
                930    // i4B
            },
            // CWF 
            {
                755,    // i4R
                512,    // i4G
                977    // i4B
            },
            // DNP 
            {
                713,    // i4R
                512,    // i4G
                749    // i4B
            },
            // D65 
            {
                749,    // i4R
                512,    // i4G
                575    // i4B
            },
            // DF 
            {
                512,    // i4R
                512,    // i4G
                512    // i4B
            }
        },
        // Rotation matrix parameter
        {
            14,    // i4RotationAngle
            248,    // i4Cos
            62    // i4Sin
        },
        // Daylight locus parameter
        {
            -208,    // i4SlopeNumerator
            128    // i4SlopeDenominator
        },
        // AWB light area
        {
            // Strobe:FIXME
            {
            0,    // i4RightBound
            0,    // i4LeftBound
            0,    // i4UpperBound
            0    // i4LowerBound
            },
            // Tungsten
            {
            -276,    // i4RightBound
            -916,    // i4LeftBound
            -90,    // i4UpperBound
            -240    // i4LowerBound
            },
            // Warm fluorescent
            {
            -276,    // i4RightBound
            -916,    // i4LeftBound
            -240,    // i4UpperBound
            -360    // i4LowerBound
            },
            // Fluorescent
            {
            -131,    // i4RightBound
            -276,    // i4LeftBound
            -130,    // i4UpperBound
            -286    // i4LowerBound
            },
            // CWF
            {
            -131,    // i4RightBound
            -276,    // i4LeftBound
            -286,    // i4UpperBound
            -397    // i4LowerBound
            },
            // Daylight
            {
            120,    // i4RightBound
            -131,    // i4LeftBound
            -111,    // i4UpperBound
            -291    // i4LowerBound
            },
            // Shade
            {
            436,    // i4RightBound
            120,    // i4LeftBound
            -111,    // i4UpperBound
            -291    // i4LowerBound
            },
            // Daylight Fluorescent
            {
            40,    // i4RightBound
            -131,    // i4LeftBound
            -291,    // i4UpperBound
            -377    // i4LowerBound
            }
        },
        // PWB light area
        {
            // Reference area
            {
            436,    // i4RightBound
            -916,    // i4LeftBound
            0,    // i4UpperBound
            -397    // i4LowerBound
            },
            // Daylight
            {
            101,    // i4RightBound
            -131,    // i4LeftBound
            -111,    // i4UpperBound
            -291    // i4LowerBound
            },
            // Cloudy daylight
            {
            201,    // i4RightBound
            26,    // i4LeftBound
            -111,    // i4UpperBound
            -291    // i4LowerBound
            },
            // Shade
            {
            301,    // i4RightBound
            26,    // i4LeftBound
            -111,    // i4UpperBound
            -291    // i4LowerBound
            },
            // Twilight
            {
            -131,    // i4RightBound
            -291,    // i4LeftBound
            -111,    // i4UpperBound
            -291    // i4LowerBound
            },
            // Fluorescent
            {
            101,    // i4RightBound
            -316,    // i4LeftBound
            -151,    // i4UpperBound
            -397    // i4LowerBound
            },
            // Warm fluorescent
            {
            -225,    // i4RightBound
            -425,    // i4LeftBound
            -151,    // i4UpperBound
            -397    // i4LowerBound
            },
            // Incandescent
            {
            -225,    // i4RightBound
            -425,    // i4LeftBound
            -111,    // i4UpperBound
            -291    // i4LowerBound
            },
            // Gray World
            {
            5000,    // i4RightBound
            -5000,    // i4LeftBound
            5000,    // i4UpperBound
            -5000    // i4LowerBound
            }
        },
        // PWB default gain	
        {
            // Daylight
            {
            702,    // i4R
            512,    // i4G
            640    // i4B
            },
            // Cloudy daylight
            {
            797,    // i4R
            512,    // i4G
            518    // i4B
            },
            // Shade
            {
            837,    // i4R
            512,    // i4G
            477    // i4B
            },
            // Twilight
            {
            579,    // i4R
            512,    // i4G
            883    // i4B
            },
            // Fluorescent
            {
            723,    // i4R
            512,    // i4G
            801    // i4B
            },
            // Warm fluorescent
            {
            583,    // i4R
            512,    // i4G
            1145    // i4B
            },
            // Incandescent
            {
            517,    // i4R
            512,    // i4G
            1065    // i4B
            },
            // Gray World
            {
            512,    // i4R
            512,    // i4G
            512    // i4B
            }
        },
        // AWB preference color	
        {
            // Tungsten
            {
            0,    // i4SliderValue
            5741    // i4OffsetThr
            },
            // Warm fluorescent	
            {
            0,    // i4SliderValue
            5594    // i4OffsetThr
            },
            // Shade
            {
            0,    // i4SliderValue
            1351    // i4OffsetThr
            },
            // Daylight WB gain
            {
            678,    // i4R
            512,    // i4G
            573    // i4B
            },
            // Preference gain: strobe
            {
            512,    // i4R
            512,    // i4G
            512    // i4B
            },
            // Preference gain: tungsten
            {
            500,    // i4R
            512,    // i4G
            512    // i4B
            },
            // Preference gain: warm fluorescent
            {
            512,    // i4R
            512,    // i4G
            512    // i4B
            },
            // Preference gain: fluorescent
            {
            512,    // i4R
            512,    // i4G
            520    // i4B
            },
            // Preference gain: CWF
            {
            512,    // i4R
            512,    // i4G
            518    // i4B
            },
            // Preference gain: daylight
            {
            508,    // i4R
            512,    // i4G
            512    // i4B
            },
            // Preference gain: shade
            {
            512,    // i4R
            512,    // i4G
            480    // i4B
            },
            // Preference gain: daylight fluorescent
            {
            508,    // i4R
            512,    // i4G
            512    // i4B
            }
        },
        {// CCT estimation
            {// CCT
                2300,    // i4CCT[0]
                2850,    // i4CCT[1]
                4100,    // i4CCT[2]
                5100,    // i4CCT[3]
                6500    // i4CCT[4]
            },
            {// Rotated X coordinate
                -444,    // i4RotatedXCoordinate[0]
                -376,    // i4RotatedXCoordinate[1]
                -267,    // i4RotatedXCoordinate[2]
                -132,    // i4RotatedXCoordinate[3]
                0    // i4RotatedXCoordinate[4]
    		}
    	}
    },
	{0}
};
 
#include INCLUDE_FILENAME_ISP_LSC_PARAM
//};  //  namespace


typedef NSFeature::RAWSensorInfo<SENSOR_ID> SensorInfoSingleton_T;


namespace NSFeature {
template <>
UINT32
SensorInfoSingleton_T::
impGetDefaultData(CAMERA_DATA_TYPE_ENUM const CameraDataType, VOID*const pDataBuf, UINT32 const size) const
{
    UINT32 dataSize[CAMERA_DATA_TYPE_NUM] = {sizeof(NVRAM_CAMERA_ISP_PARAM_STRUCT),
                                             sizeof(NVRAM_CAMERA_3A_STRUCT),
                                             sizeof(NVRAM_CAMERA_SHADING_STRUCT),
                                             sizeof(NVRAM_LENS_PARA_STRUCT),
                                             sizeof(AE_PLINETABLE_T)};

    if (CameraDataType > CAMERA_DATA_AE_PLINETABLE || NULL == pDataBuf || (size < dataSize[CameraDataType]))
    {
        return 1;
    }

    switch(CameraDataType)
    {
        case CAMERA_NVRAM_DATA_ISP:
            memcpy(pDataBuf,&CAMERA_ISP_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_ISP_PARAM_STRUCT));
            break;
        case CAMERA_NVRAM_DATA_3A:
            memcpy(pDataBuf,&CAMERA_3A_NVRAM_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_3A_STRUCT));
            break;
        case CAMERA_NVRAM_DATA_SHADING:
            memcpy(pDataBuf,&CAMERA_SHADING_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_SHADING_STRUCT));
            break;
        case CAMERA_DATA_AE_PLINETABLE:
            memcpy(pDataBuf,&g_PlineTableMapping,sizeof(AE_PLINETABLE_T));
            break;
        default:
            break;
    }
    return 0;
}};  //  NSFeature


