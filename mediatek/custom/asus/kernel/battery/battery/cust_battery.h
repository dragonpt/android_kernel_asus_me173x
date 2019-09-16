#ifndef _CUST_BAT_H_
#define _CUST_BAT_H_

#ifndef COMPAL_SMART_CHARGING_SUPPORT
#define COMPAL_SMART_CHARGING_SUPPORT
#endif

//superdragonpt added //TODO
typedef enum
{
	Cust_CC_1600MA = 0x0,
	Cust_CC_1500MA = 0x1,
	Cust_CC_1400MA = 0x2,
	Cust_CC_1300MA = 0x3,
	Cust_CC_1200MA = 0x4,
	Cust_CC_1100MA = 0x5,
	Cust_CC_1000MA = 0x6,
	Cust_CC_900MA  = 0x7,
	Cust_CC_800MA  = 0x8,
	Cust_CC_700MA  = 0x9,
	Cust_CC_650MA  = 0xA,
	Cust_CC_550MA  = 0xB,
	Cust_CC_450MA  = 0xC,
	Cust_CC_400MA  = 0xD,
	Cust_CC_200MA  = 0xE,
	Cust_CC_70MA   = 0xF,
//TODO
    //Cust_CC_100MA = 0xF,
    //Cust_CC_500MA = 0xB,
    //Cust_CC_1550MA = 0x1,
//TODO
	Cust_CC_0MA	   = 0xDD
}cust_charging_current_enum;
//superdragonpt added, End

typedef struct{	
	unsigned int BattVolt;
	unsigned int BattPercent;
}VBAT_TO_PERCENT;

/* Battery Temperature Protection */
#define MAX_CHARGE_TEMPERATURE  48
#define MIN_CHARGE_TEMPERATURE  2
#define ERR_CHARGE_TEMPERATURE  0xFF

/* Recharging Battery Voltage */
#define RECHARGING_VOLTAGE      4110

/* Charging Current Setting */
#define CONFIG_USB_IF 						0   
#define USB_CHARGER_CURRENT_SUSPEND			0		// def CONFIG_USB_IF
#define USB_CHARGER_CURRENT_UNCONFIGURED	100	// def CONFIG_USB_IF
#define USB_CHARGER_CURRENT_CONFIGURED		500	// def CONFIG_USB_IF
#define USB_CHARGER_CURRENT					500
#define AC_CHARGER_CURRENT					2036	

/* Battery Meter Solution */
#define CONFIG_ADC_SOLUTION 	1

/* Battery Voltage and Percentage Mapping Table */
VBAT_TO_PERCENT Batt_VoltToPercent_Table[] = {
	/*BattVolt,BattPercent*/
	{3400,0},
	{3641,10},
	{3708,20},
	{3741,30},
	{3765,40},
	{3793,50},
	{3836,60},
	{3891,70},
	{3960,80},
	{4044,90},
	{4183,100},
};

/* Precise Tunning */
#define BATTERY_AVERAGE_SIZE 	30
//#define BATTERY_AVERAGE_SIZE   3

/* Common setting */
#define R_CURRENT_SENSE 2				// 0.2 Ohm
#define R_BAT_SENSE 4					// times of voltage
#define R_I_SENSE 4						// times of voltage
#define R_CHARGER_1 330
#define R_CHARGER_2 39
#define R_CHARGER_SENSE ((R_CHARGER_1+R_CHARGER_2)/R_CHARGER_2)	// times of voltage
#define V_CHARGER_MAX 7000				// 7 V
#define V_CHARGER_MIN 4400				// 4.4 V
#define V_CHARGER_ENABLE 0				// 1:ON , 0:OFF

/* Teperature related setting */
#define RBAT_PULL_UP_R             39000
//#define RBAT_PULL_UP_VOLT          2500
#define RBAT_PULL_UP_VOLT          1800
#define TBAT_OVER_CRITICAL_LOW     68237
//#define TBAT_OVER_CRITICAL_LOW     483954
#define BAT_TEMP_PROTECT_ENABLE    1
#define BAT_NTC_10 1
#define BAT_NTC_47 0

/* Battery Notify */
//#define BATTERY_NOTIFY_CASE_0001
//#define BATTERY_NOTIFY_CASE_0002
//#define BATTERY_NOTIFY_CASE_0003
//#define BATTERY_NOTIFY_CASE_0004
//#define BATTERY_NOTIFY_CASE_0005

//#define CONFIG_POWER_VERIFY

#endif /* _CUST_BAT_H_ */

#ifdef COMPAL_SMART_CHARGING_SUPPORT
typedef struct{
	unsigned int SM_ibatt;
}LEVEL_TO_CURRENT;

typedef struct{
	unsigned int bq24196_voltage;
	int bq24196_temp;
}VOL_TO_TEMP;

LEVEL_TO_CURRENT thermal_LevelToIbatt_Table[] = {
	/*Ibattery*/
	{2048}, // Ignore
	{2048},	// level 1
	{512},	// level 2
	{256},	// level 3
};

LEVEL_TO_CURRENT battery_LevelToIbatt_Table[] = {
	/*Ibattery*/
	{2048}, // Ignore
	{2048},	// Ignore
	{2048},	// Ignore
};

LEVEL_TO_CURRENT charger_LevelToIbatt_Table[] = {
	/*Ibattery*/
	{2048}, // Ignore
	{2048},	// level 1
	{512},	// level 2
	{256},	// level 3
};

VOL_TO_TEMP Batt_VoltToTemp_Table[] = {
	/*BattVolt,BattTemp*/
	{1480, -20},
	{1470, -19},
	{1460, -18},
	{1450, -17},
	{1440, -17},
	{1430, -16},
	{1420, -15},
	{1410, -14},
	{1400, -14},

	{1390, -13},
	{1380, -12},
	{1370, -12},
	{1360, -11},
	{1350, -11},
	{1340, -10},
	{1330, -9},
	{1320, -9},
	{1310, -8},
	{1300, -7},

	{1290, -7},
	{1280, -6},
	{1270, -6},
	{1260, -5},
	{1250, -4},
	{1240, -4},
	{1230, -3},
	{1220, -3},
	{1210, -2},
	{1200, -2},

	{1190, -1},
	{1180, 0},
	{1170, 0},
	{1160, 1},
	{1150, 1},
	{1140, 2},
	{1130, 2},
	{1120, 3},
	{1110, 3},
	{1100, 4},

	{1090, 4},
	{1080, 5},
	{1070, 6},
	{1060, 6},
	{1050, 7},
	{1040, 7},
	{1030, 8},
	{1020, 8},
	{1010, 9},
	{1000, 9},

	{990, 10},
	{980, 10},
	{970, 11},
	{960, 12},
	{950, 12},
	{940, 13},
	{930, 13},
	{920, 14},
	{910, 14},
	{900, 15},

	{890, 15},
	{880, 16},
	{870, 17},
	{860, 17},
	{850, 18},
	{840, 18},
	{830, 19},
	{820, 19},
	{810, 20},
	{800, 20},

	{790, 21},
	{780, 22},
	{770, 22},
	{760, 23},
	{750, 23},
	{740, 24},
	{730, 25},
	{720, 25},
	{710, 26},
	{700, 26},

	{690, 27},
	{680, 28},
	{670, 28},
	{660, 29},
	{650, 30},
	{640, 30},
	{630, 31},
	{620, 32},
	{610, 32},
	{600, 33},

	{590, 34},
	{580, 34},
	{570, 35},
	{560, 36},
	{550, 36},
	{540, 37},
	{530, 38},
	{520, 39},
	{510, 39},
	{500, 40},

	{490, 41},
	{480, 42},
	{470, 43},
	{460, 44},
	{450, 44},
	{440, 45},
	{430, 46},
	{420, 47},
	{410, 48},
	{400, 49},

	{390, 50},
	{380, 51},
	{370, 52},
	{360, 53},
	{350, 54},
	{340, 55},
	{330, 56},
	{320, 57},
	{310, 59},
	{300, 60},

	{290, 61},
	{280, 63},
	{270, 64},
	{260, 65},
	{250, 67},
	{240, 68},
	{230, 70},
	{220, 72},
	{210, 74},
	{200, 75},

	{190, 77},
	{180, 79},
	{170, 82},
	{160, 84},
	{150, 86},
	{140, 89},
	{130, 92},
	{120, 95},
	{110, 99},
};
#endif
