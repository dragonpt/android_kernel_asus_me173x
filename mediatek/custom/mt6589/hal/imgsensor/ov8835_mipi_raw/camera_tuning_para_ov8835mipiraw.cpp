#include <utils/Log.h>
#include <fcntl.h>
#include <math.h>

#include "camera_custom_nvram.h"
#include "camera_custom_sensor.h"
#include "image_sensor.h"
#include "kd_imgsensor_define.h"
#include "camera_AE_PLineTable_ov8835mipiraw.h"
#include "camera_info_ov8835mipiraw.h"
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
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    },
    ISPPca:{
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
        77225,    // i4R_AVG
        19806,    // i4R_STD
        117325,    // i4B_AVG
        30738,    // i4B_STD
        {  // i4P00[9]
            5607500, -2997500, -50000, -802500, 3865000, -502500, -112500, -2167500, 4845000
        },
        {  // i4P10[9]
            1108520, -1388364, 279844, -27029, -77362, 99012, -119193, -261315, 385814
        },
        {  // i4P01[9]
            788467, -1085442, 296974, -168095, -215900, 381771, -178895, -898810, 1081765
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
            1640,    // u4MinGain, 1024 base = 1x
            15872,    // u4MaxGain, 16x
            118,    // u4MiniISOGain, ISOxx  
            128,    // u4GainStepUnit, 1x/8 
            17346,    // u4PreExpUnit 
            29,    // u4PreMaxFrameRate
            13876,    // u4VideoExpUnit  
            30,    // u4VideoMaxFrameRate 
            1024,    // u4Video2PreRatio, 1024 base = 1x 
            13876,    // u4CapExpUnit 
            28,    // u4CapMaxFrameRate
            1024,    // u4Cap2PreRatio, 1024 base = 1x
            28,    // u4LensFno, Fno = 2.8
            350    // u4FocusLength_100x
        },
        // rHistConfig
        {
            2,    // u4HistHighThres
            40,    // u4HistLowThres
            2,    // u4MostBrightRatio
            1,    // u4MostDarkRatio
            160,    // u4CentralHighBound
            20,    // u4CentralLowBound
            {240, 230, 220, 210, 200},    // u4OverExpThres[AE_CCT_STRENGTH_NUM] 
            {75, 108, 128, 148, 170},    // u4HistStretchThres[AE_CCT_STRENGTH_NUM] 
            {18, 22, 26, 30, 34}    // u4BlackLightThres[AE_CCT_STRENGTH_NUM] 
        },
        // rCCTConfig
        {
            TRUE,    // bEnableBlackLight
            TRUE,    // bEnableHistStretch
            FALSE,    // bEnableAntiOverExposure
            TRUE,    // bEnableTimeLPF
            TRUE,    // bEnableCaptureThres
            TRUE,    // bEnableVideoThres
            TRUE,    // bEnableStrobeThres
            47,    // u4AETarget
            0,    // u4StrobeAETarget
            50,    // u4InitIndex
            4,    // u4BackLightWeight
            32,    // u4HistStretchWeight
            4,    // u4AntiOverExpWeight
            2,    // u4BlackLightStrengthIndex
            2,    // u4HistStretchStrengthIndex
            2,    // u4AntiOverExpStrengthIndex
            2,    // u4TimeLPFStrengthIndex
            {1, 3, 5, 7, 8},    // u4LPFConvergeTable[AE_CCT_STRENGTH_NUM] 
            90,    // u4InDoorEV = 9.0, 10 base 
            -8,    // i4BVOffset delta BV = value/10 
            64,    // u4PreviewFlareOffset
            64,    // u4CaptureFlareOffset
            3,    // u4CaptureFlareThres
            64,    // u4VideoFlareOffset
            3,    // u4VideoFlareThres
            64,    // u4StrobeFlareOffset
            3,    // u4StrobeFlareThres
            160,    // u4PrvMaxFlareThres
            0,    // u4PrvMinFlareThres
            160,    // u4VideoMaxFlareThres
            0,    // u4VideoMinFlareThres
            18,    // u4FlatnessThres    // 10 base for flatness condition.
            75    // u4FlatnessStrength
        }
    },
    // AWB NVRAM
    {
        // AWB calibration data
        {
            // rUnitGain (unit gain: 1.0 = 512)
            {
                0,    // i4R
                0,    // i4G
                0    // i4B
            },
            // rGoldenGain (golden sample gain: 1.0 = 512)
            {
                0,    // i4R
                0,    // i4G
                0    // i4B
            },
            // rTuningUnitGain (Tuning sample unit gain: 1.0 = 512)
            {
                0,    // i4R
                0,    // i4G
                0    // i4B
            },
            // rD65Gain (D65 WB gain: 1.0 = 512)
            {
                995,    // i4R
                512,    // i4G
                723    // i4B
            }
        },
        // Original XY coordinate of AWB light source
        {
           // Strobe
            {
                118,    // i4X
                -373    // i4Y
            },
            // Horizon
            {
                -468,    // i4X
                -395    // i4Y
            },
            // A
            {
                -343,    // i4X
                -398    // i4Y
            },
            // TL84
            {
                -178,    // i4X
                -395    // i4Y
            },
            // CWF
            {
                -141,    // i4X
                -476    // i4Y
            },
            // DNP
            {
                -48,    // i4X
                -409    // i4Y
            },
            // D65
            {
                118,    // i4X
                -373    // i4Y
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
                99,    // i4X
                -379    // i4Y
            },
            // Horizon
            {
                -488,    // i4X
                -371    // i4Y
            },
            // A
            {
                -363,    // i4X
                -381    // i4Y
            },
            // TL84
            {
                -198,    // i4X
                -386    // i4Y
            },
            // CWF
            {
                -165,    // i4X
                -469    // i4Y
            },
            // DNP
            {
                -69,    // i4X
                -407    // i4Y
            },
            // D65
            {
                99,    // i4X
                -379    // i4Y
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
                995,    // i4R
                512,    // i4G
                723    // i4B
            },
            // Horizon 
            {
                512,    // i4R
                565,    // i4G
                1817    // i4B
            },
            // A 
            {
                551,    // i4R
                512,    // i4G
                1395    // i4B
            },
            // TL84 
            {
                687,    // i4R
                512,    // i4G
                1113    // i4B
            },
            // CWF 
            {
                805,    // i4R
                512,    // i4G
                1181    // i4B
            },
            // DNP 
            {
                835,    // i4R
                512,    // i4G
                950    // i4B
            },
            // D65 
            {
                995,    // i4R
                512,    // i4G
                723    // i4B
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
            3,    // i4RotationAngle
            256,    // i4Cos
            13    // i4Sin
        },
        // Daylight locus parameter
        {
            -141,    // i4SlopeNumerator
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
            -248,    // i4RightBound
            -898,    // i4LeftBound
            -286,    // i4UpperBound
            -426    // i4LowerBound
            },
            // Warm fluorescent
            {
            -248,    // i4RightBound
            -898,    // i4LeftBound
            -426,    // i4UpperBound
            -546    // i4LowerBound
            },
            // Fluorescent
            {
            -119,    // i4RightBound
            -248,    // i4LeftBound
            -312,    // i4UpperBound
            -427    // i4LowerBound
            },
            // CWF
            {
            -119,    // i4RightBound
            -248,    // i4LeftBound
            -427,    // i4UpperBound
            -519    // i4LowerBound
            },
            // Daylight
            {
            124,    // i4RightBound
            -119,    // i4LeftBound
            -299,    // i4UpperBound
            -459    // i4LowerBound
            },
            // Shade
            {
            484,    // i4RightBound
            124,    // i4LeftBound
            -299,    // i4UpperBound
            -459    // i4LowerBound
            },
            // Daylight Fluorescent
            {
            124,    // i4RightBound
            -119,    // i4LeftBound
            -459,    // i4UpperBound
            -680    // i4LowerBound
            }
        },
        // PWB light area
        {
            // Reference area
            {
            484,    // i4RightBound
            -898,    // i4LeftBound
            0,    // i4UpperBound
            -680    // i4LowerBound
            },
            // Daylight
            {
            149,    // i4RightBound
            -119,    // i4LeftBound
            -299,    // i4UpperBound
            -459    // i4LowerBound
            },
            // Cloudy daylight
            {
            249,    // i4RightBound
            74,    // i4LeftBound
            -299,    // i4UpperBound
            -459    // i4LowerBound
            },
            // Shade
            {
            349,    // i4RightBound
            74,    // i4LeftBound
            -299,    // i4UpperBound
            -459    // i4LowerBound
            },
            // Twilight
            {
            -119,    // i4RightBound
            -279,    // i4LeftBound
            -299,    // i4UpperBound
            -459    // i4LowerBound
            },
            // Fluorescent
            {
            149,    // i4RightBound
            -298,    // i4LeftBound
            -329,    // i4UpperBound
            -519    // i4LowerBound
            },
            // Warm fluorescent
            {
            -263,    // i4RightBound
            -463,    // i4LeftBound
            -329,    // i4UpperBound
            -519    // i4LowerBound
            },
            // Incandescent
            {
            -263,    // i4RightBound
            -463,    // i4LeftBound
            -299,    // i4UpperBound
            -459    // i4LowerBound
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
            894,    // i4R
            512,    // i4G
            815    // i4B
            },
            // Cloudy daylight
            {
            1078,    // i4R
            512,    // i4G
            662    // i4B
            },
            // Shade
            {
            1150,    // i4R
            512,    // i4G
            616    // i4B
            },
            // Twilight
            {
            679,    // i4R
            512,    // i4G
            1104    // i4B
            },
            // Fluorescent
            {
            849,    // i4R
            512,    // i4G
            980    // i4B
            },
            // Warm fluorescent
            {
            587,    // i4R
            512,    // i4G
            1476    // i4B
            },
            // Incandescent
            {
            550,    // i4R
            512,    // i4G
            1393    // i4B
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
            7347    // i4OffsetThr
            },
            // Warm fluorescent	
            {
            0,    // i4SliderValue
            5733    // i4OffsetThr
            },
            // Shade
            {
            50,    // i4SliderValue
            341    // i4OffsetThr
            },
            // Daylight WB gain
            {
            802,    // i4R
            512,    // i4G
            918    // i4B
            },
            // Preference gain: strobe
            {
            512,    // i4R
            512,    // i4G
            512    // i4B
            },
            // Preference gain: tungsten
            {
            512,    // i4R
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
            512    // i4B
            },
            // Preference gain: CWF
            {
            512,    // i4R
            512,    // i4G
            512    // i4B
            },
            // Preference gain: daylight
            {
            512,    // i4R
            512,    // i4G
            512    // i4B
            },
            // Preference gain: shade
            {
            512,    // i4R
            512,    // i4G
            512    // i4B
            },
            // Preference gain: daylight fluorescent
            {
            512,    // i4R
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
                -587,    // i4RotatedXCoordinate[0]
                -462,    // i4RotatedXCoordinate[1]
                -297,    // i4RotatedXCoordinate[2]
                -168,    // i4RotatedXCoordinate[3]
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
}}; // NSFeature


