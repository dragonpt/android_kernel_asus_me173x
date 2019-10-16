#include <mach/mt_typedefs.h>

#define TEMPERATURE_T0                  110
#define TEMPERATURE_T1                  0
#define TEMPERATURE_T2                  25
#define TEMPERATURE_T3                  50
#define TEMPERATURE_T                   255 // This should be fixed, never change the value

#define FG_METER_RESISTANCE 	0

#define MAX_BOOTING_TIME_FGCURRENT	1*10 // 10s

#define OCV_BOARD_COMPESATE	0 //mV 
#define R_FG_BOARD_BASE		1000
#define R_FG_BOARD_SLOPE	1000 //slope

#define Q_MAX_POS_50	4074
#define Q_MAX_POS_25	4032
#define Q_MAX_POS_0	3864
#define Q_MAX_NEG_10	3254

#define Q_MAX_POS_50_H_CURRENT	4039
#define Q_MAX_POS_25_H_CURRENT	3921
#define Q_MAX_POS_0_H_CURRENT	3380
#define Q_MAX_NEG_10_H_CURRENT	2275

#define R_FG_VALUE 				20 // mOhm, base is 20
#define CURRENT_DETECT_R_FG	10  //1mA

#define OSR_SELECT_7			0

#define CAR_TUNE_VALUE			120 //100 //1.00

/////////////////////////////////////////////////////////////////////
// <DOD, Battery_Voltage> Table
/////////////////////////////////////////////////////////////////////
typedef struct _BATTERY_PROFILE_STRUC
{
    kal_int32 percentage;
    kal_int32 voltage;
} BATTERY_PROFILE_STRUC, *BATTERY_PROFILE_STRUC_P;

typedef enum
{
    T1_0C,
    T2_25C,
    T3_50C
} PROFILE_TEMPERATURE;


// T0 -10C
BATTERY_PROFILE_STRUC battery_profile_t0[] =
{
	{	0	,	4156	},
	{	2	,	4126	},
	{	4	,	4097	},
	{	6	,	4069	},
	{	7	,	4044	},
	{	9	,	4026	},
	{	11	,	4011	},
	{	13	,	3997	},
	{	15	,	3985	},
	{	17	,	3975	},
	{	18	,	3965	},
	{	20	,	3957	},
	{	22	,	3947	},
	{	24	,	3937	},
	{	26	,	3927	},
	{	28	,	3917	},
	{	29	,	3906	},
	{	31	,	3896	},
	{	33	,	3885	},
	{	35	,	3875	},
	{	37	,	3865	},
	{	39	,	3856	},
	{	40	,	3847	},
	{	42	,	3838	},
	{	44	,	3829	},
	{	46	,	3822	},
	{	48	,	3817	},
	{	50	,	3810	},
	{	52	,	3805	},
	{	53	,	3800	},
	{	55	,	3796	},
	{	57	,	3794	},
	{	59	,	3790	},
	{	61	,	3788	},
	{	63	,	3786	},
	{	64	,	3784	},
	{	66	,	3782	},
	{	68	,	3778	},
	{	70	,	3775	},
	{	72	,	3772	},
	{	74	,	3769	},
	{	75	,	3765	},
	{	77	,	3762	},
	{	79	,	3758	},
	{	81	,	3752	},
	{	83	,	3747	},
	{	85	,	3741	},
	{	86	,	3735	},
	{	88	,	3728	},
	{	90	,	3722	},
	{	91	,	3720	},
	{	92	,	3717	},
	{	93	,	3716	},
	{	93	,	3714	},
	{	94	,	3713	},
	{	94	,	3711	},
	{	95	,	3710	},
	{	95	,	3708	},
	{	95	,	3708	},
	{	96	,	3706	},
	{	96	,	3704	},
	{	96	,	3703	},
	{	97	,	3702	},
	{	97	,	3701	},
	{	97	,	3700	},
	{	97	,	3699	},
	{	98	,	3699	},
	{	98	,	3697	},
	{	98	,	3696	},
	{	98	,	3695	},
	{	98	,	3694	},
	{	98	,	3693	},
	{	99	,	3693	},
	{	99	,	3691	},
	{	99	,	3692	},
	{	99	,	3690	},
	{	99	,	3689	},
	{	99	,	3689	},
	{	100	,	3689	},
	{	100	,	3688	},
	{	100	,	3687	},
	{	100	,	3400	}
};      


// T1 0C 
BATTERY_PROFILE_STRUC battery_profile_t1[] =
{              
	{	0	,	4177	},
	{	2	,	4148	},
	{	3	,	4128	},
	{	5	,	4112	},
	{	6	,	4097	},
	{	8	,	4085	},
	{	9	,	4073	},
	{	11	,	4059	},
	{	12	,	4045	},
	{	14	,	4031	},
	{	16	,	4017	},
	{	17	,	4006	},
	{	19	,	3994	},
	{	20	,	3984	},
	{	22	,	3974	},
	{	23	,	3965	},
	{	25	,	3956	},
	{	26	,	3946	},
	{	28	,	3937	},
	{	29	,	3928	},
	{	31	,	3917	},
	{	33	,	3907	},
	{	34	,	3899	},
	{	36	,	3888	},
	{	37	,	3877	},
	{	39	,	3867	},
	{	40	,	3857	},
	{	42	,	3848	},
	{	43	,	3839	},
	{	45	,	3832	},
	{	46	,	3825	},
	{	48	,	3818	},
	{	50	,	3812	},
	{	51	,	3808	},
	{	53	,	3803	},
	{	54	,	3798	},
	{	56	,	3795	},
	{	57	,	3791	},
	{	59	,	3788	},
	{	60	,	3785	},
	{	62	,	3784	},
	{	64	,	3782	},
	{	65	,	3779	},
	{	67	,	3778	},
	{	68	,	3775	},
	{	70	,	3773	},
	{	71	,	3769	},
	{	73	,	3766	},
	{	74	,	3762	},
	{	76	,	3759	},
	{	77	,	3754	},
	{	79	,	3748	},
	{	81	,	3742	},
	{	82	,	3735	},
	{	84	,	3726	},
	{	85	,	3719	},
	{	87	,	3711	},
	{	88	,	3702	},
	{	90	,	3692	},
	{	91	,	3681	},
	{	93	,	3671	},
	{	95	,	3659	},
	{	96	,	3648	},
	{	97	,	3641	},
	{	97	,	3633	},
	{	98	,	3625	},
	{	98	,	3615	},
	{	98	,	3604	},
	{	99	,	3595	},
	{	99	,	3588	},
	{	99	,	3580	},
	{	99	,	3572	},
	{	99	,	3567	},
	{	99	,	3561	},
	{	100	,	3555	},
	{	100	,	3550	},
	{	100	,	3546	},
	{	100	,	3543	},
	{	100	,	3538	},
	{	100	,	3535	},
	{	100	,	3531	},
	{	100	,	3400	}
};


//T2 25C
BATTERY_PROFILE_STRUC battery_profile_t2[] =
{              
	{	0	,	4192	},
	{	1	,	4170	},
	{	3	,	4152	},
	{	4	,	4136	},
	{	6	,	4122	},
	{	7	,	4109	},
	{	9	,	4095	},
	{	10	,	4082	},
	{	12	,	4071	},
	{	13	,	4058	},
	{	15	,	4044	},
	{	16	,	4030	},
	{	18	,	4019	},
	{	19	,	4007	},
	{	21	,	3995	},
	{	22	,	3985	},
	{	24	,	3975	},
	{	25	,	3966	},
	{	27	,	3957	},
	{	28	,	3948	},
	{	30	,	3939	},
	{	31	,	3931	},
	{	33	,	3923	},
	{	34	,	3914	},
	{	36	,	3905	},
	{	37	,	3897	},
	{	39	,	3888	},
	{	40	,	3878	},
	{	42	,	3868	},
	{	43	,	3856	},
	{	45	,	3845	},
	{	46	,	3836	},
	{	48	,	3829	},
	{	49	,	3823	},
	{	50	,	3817	},
	{	52	,	3811	},
	{	53	,	3807	},
	{	55	,	3801	},
	{	56	,	3798	},
	{	58	,	3794	},
	{	59	,	3790	},
	{	61	,	3787	},
	{	62	,	3784	},
	{	64	,	3781	},
	{	65	,	3780	},
	{	67	,	3777	},
	{	68	,	3776	},
	{	70	,	3774	},
	{	71	,	3771	},
	{	73	,	3767	},
	{	74	,	3764	},
	{	76	,	3758	},
	{	77	,	3752	},
	{	79	,	3747	},
	{	80	,	3741	},
	{	82	,	3735	},
	{	83	,	3728	},
	{	85	,	3719	},
	{	86	,	3710	},
	{	88	,	3701	},
	{	89	,	3690	},
	{	91	,	3683	},
	{	92	,	3677	},
	{	94	,	3670	},
	{	95	,	3657	},
	{	97	,	3620	},
	{	98	,	3548	},
	{	100	,	3446	},
	{	100	,	3393	},
	{	100	,	3363	},
	{	100	,	3346	},
	{	100	,	3334	},
	{	100	,	3325	},
	{	100	,	3320	},
	{	100	,	3315	},
	{	100	,	3313	},
	{	100	,	3311	},
	{	100	,	3309	},
	{	100	,	3308	},
	{	100	,	3307	},
	{	100	,	3306	},
	{	100	,	3305	}
};             



// T3 50C
BATTERY_PROFILE_STRUC battery_profile_t3[] =
{              
	{	0	,	4198	},
	{	1	,	4177	},
	{	3	,	4159	},
	{	4	,	4144	},
	{	6	,	4129	},
	{	7	,	4115	},
	{	9	,	4101	},
	{	10	,	4088	},
	{	12	,	4075	},
	{	13	,	4062	},
	{	15	,	4050	},
	{	16	,	4038	},
	{	18	,	4026	},
	{	19	,	4015	},
	{	21	,	4004	},
	{	22	,	3993	},
	{	24	,	3982	},
	{	25	,	3972	},
	{	26	,	3963	},
	{	28	,	3952	},
	{	29	,	3943	},
	{	31	,	3934	},
	{	32	,	3925	},
	{	34	,	3916	},
	{	35	,	3909	},
	{	37	,	3901	},
	{	38	,	3892	},
	{	40	,	3885	},
	{	41	,	3876	},
	{	43	,	3866	},
	{	44	,	3852	},
	{	46	,	3840	},
	{	47	,	3831	},
	{	49	,	3824	},
	{	50	,	3818	},
	{	51	,	3812	},
	{	53	,	3807	},
	{	54	,	3802	},
	{	56	,	3797	},
	{	57	,	3793	},
	{	59	,	3789	},
	{	60	,	3785	},
	{	62	,	3782	},
	{	63	,	3778	},
	{	65	,	3776	},
	{	66	,	3773	},
	{	68	,	3771	},
	{	69	,	3768	},
	{	71	,	3765	},
	{	72	,	3756	},
	{	73	,	3747	},
	{	75	,	3740	},
	{	76	,	3735	},
	{	78	,	3729	},
	{	79	,	3723	},
	{	81	,	3718	},
	{	82	,	3713	},
	{	84	,	3706	},
	{	85	,	3697	},
	{	87	,	3689	},
	{	88	,	3679	},
	{	90	,	3672	},
	{	91	,	3667	},
	{	93	,	3663	},
	{	94	,	3657	},
	{	96	,	3641	},
	{	97	,	3592	},
	{	98	,	3517	},
	{	100	,	3407	},
	{	100	,	3274	},
	{	100	,	3263	},
	{	100	,	3260	},
	{	100	,	3259	},
	{	100	,	3258	},
	{	100	,	3257	},
	{	100	,	3258	},
	{	100	,	3257	},
	{	100	,	3256	},
	{	100	,	3256	},
	{	100	,	3256	},
	{	100	,	3256	},
	{	100	,	3255	}
};             
           
// battery profile for actual temperature. The size should be the same as T1, T2 and T3
BATTERY_PROFILE_STRUC battery_profile_temperature[] =
{
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },          
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
	{0  , 0 }, 
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 }, 
	{0  , 0 },	    
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 },   
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }  
};      

/////////////////////////////////////////////////////////////////////
// <Rbat, Battery_Voltage> Table
/////////////////////////////////////////////////////////////////////
typedef struct _R_PROFILE_STRUC
{
    kal_int32 resistance; // Ohm
    kal_int32 voltage;
} R_PROFILE_STRUC, *R_PROFILE_STRUC_P;


// T0 -10C
R_PROFILE_STRUC r_profile_t0[] =
{              
	{	285	,	4156	},
	{	508	,	4126	},
	{	543	,	4097	},
	{	578	,	4069	},
	{	613	,	4044	},
	{	640	,	4026	},
	{	650	,	4011	},
	{	655	,	3997	},
	{	660	,	3985	},
	{	670	,	3975	},
	{	678	,	3965	},
	{	693	,	3957	},
	{	698	,	3947	},
	{	703	,	3937	},
	{	710	,	3927	},
	{	720	,	3917	},
	{	723	,	3906	},
	{	728	,	3896	},
	{	728	,	3885	},
	{	733	,	3875	},
	{	738	,	3865	},
	{	748	,	3856	},
	{	750	,	3847	},
	{	755	,	3838	},
	{	758	,	3829	},
	{	765	,	3822	},
	{	778	,	3817	},
	{	780	,	3810	},
	{	793	,	3805	},
	{	800	,	3800	},
	{	808	,	3796	},
	{	825	,	3794	},
	{	835	,	3790	},
	{	855	,	3788	},
	{	873	,	3786	},
	{	895	,	3784	},
	{	913	,	3782	},
	{	935	,	3778	},
	{	958	,	3775	},
	{	983	,	3772	},
	{	1008	,	3769	},
	{	1033	,	3765	},
	{	1065	,	3762	},
	{	1095	,	3758	},
	{	1123	,	3752	},
	{	1155	,	3747	},
	{	1190	,	3741	},
	{	1223	,	3735	},
	{	1270	,	3728	},
	{	1305	,	3722	},
	{	1300	,	3720	},
	{	1293	,	3717	},
	{	1293	,	3716	},
	{	1285	,	3714	},
	{	1285	,	3713	},
	{	1278	,	3711	},
	{	1278	,	3710	},
	{	1270	,	3708	},
	{	1273	,	3708	},
	{	1268	,	3706	},
	{	1263	,	3704	},
	{	1260	,	3703	},
	{	1258	,	3702	},
	{	1255	,	3701	},
	{	1253	,	3700	},
	{	1250	,	3699	},
	{	1253	,	3699	},
	{	1248	,	3697	},
	{	1240	,	3696	},
	{	1243	,	3695	},
	{	1238	,	3694	},
	{	1235	,	3693	},
	{	1235	,	3693	},
	{	1233	,	3691	},
	{	1235	,	3692	},
	{	1228	,	3690	},
	{	1228	,	3689	},
	{	1225	,	3689	},
	{	1225	,	3689	},
	{	1220	,	3688	},
	{	1218	,	3687	},
	{	505	,	3400	}
};             
 

// T1 0C
R_PROFILE_STRUC r_profile_t1[] =
{              
	{	213	,	4177	},
	{	320	,	4148	},
	{	328	,	4128	},
	{	330	,	4112	},
	{	333	,	4097	},
	{	343	,	4085	},
	{	353	,	4073	},
	{	355	,	4059	},
	{	363	,	4045	},
	{	365	,	4031	},
	{	365	,	4017	},
	{	370	,	4006	},
	{	373	,	3994	},
	{	373	,	3984	},
	{	378	,	3974	},
	{	383	,	3965	},
	{	385	,	3956	},
	{	390	,	3946	},
	{	393	,	3937	},
	{	398	,	3928	},
	{	395	,	3917	},
	{	395	,	3907	},
	{	400	,	3899	},
	{	395	,	3888	},
	{	393	,	3877	},
	{	393	,	3867	},
	{	388	,	3857	},
	{	385	,	3848	},
	{	385	,	3839	},
	{	385	,	3832	},
	{	388	,	3825	},
	{	390	,	3818	},
	{	390	,	3812	},
	{	398	,	3808	},
	{	400	,	3803	},
	{	403	,	3798	},
	{	408	,	3795	},
	{	410	,	3791	},
	{	418	,	3788	},
	{	420	,	3785	},
	{	433	,	3784	},
	{	443	,	3782	},
	{	450	,	3779	},
	{	465	,	3778	},
	{	480	,	3775	},
	{	495	,	3773	},
	{	508	,	3769	},
	{	528	,	3766	},
	{	545	,	3762	},
	{	565	,	3759	},
	{	585	,	3754	},
	{	603	,	3748	},
	{	630	,	3742	},
	{	653	,	3735	},
	{	683	,	3726	},
	{	713	,	3719	},
	{	750	,	3711	},
	{	790	,	3702	},
	{	840	,	3692	},
	{	895	,	3681	},
	{	973	,	3671	},
	{	1063	,	3659	},
	{	1123	,	3648	},
	{	1105	,	3641	},
	{	1083	,	3633	},
	{	1063	,	3625	},
	{	1040	,	3615	},
	{	1015	,	3604	},
	{	990	,	3595	},
	{	973	,	3588	},
	{	953	,	3580	},
	{	935	,	3572	},
	{	920	,	3567	},
	{	908	,	3561	},
	{	890	,	3555	},
	{	878	,	3550	},
	{	868	,	3546	},
	{	858	,	3543	},
	{	845	,	3538	},
	{	845	,	3535	},
	{	828	,	3531	},
	{	505	,	3400	}
}; 


// T2 25C
R_PROFILE_STRUC r_profile_t2[] =
{              
	{	125	,	4192	},
	{	178	,	4170	},
	{	180	,	4152	},
	{	183	,	4136	},
	{	185	,	4122	},
	{	190	,	4109	},
	{	190	,	4095	},
	{	190	,	4082	},
	{	198	,	4071	},
	{	200	,	4058	},
	{	203	,	4044	},
	{	203	,	4030	},
	{	208	,	4019	},
	{	208	,	4007	},
	{	205	,	3995	},
	{	210	,	3985	},
	{	210	,	3975	},
	{	213	,	3966	},
	{	215	,	3957	},
	{	215	,	3948	},
	{	213	,	3939	},
	{	218	,	3931	},
	{	220	,	3923	},
	{	218	,	3914	},
	{	218	,	3905	},
	{	220	,	3897	},
	{	218	,	3888	},
	{	215	,	3878	},
	{	213	,	3868	},
	{	200	,	3856	},
	{	195	,	3845	},
	{	190	,	3836	},
	{	190	,	3829	},
	{	190	,	3823	},
	{	188	,	3817	},
	{	190	,	3811	},
	{	190	,	3807	},
	{	190	,	3801	},
	{	195	,	3798	},
	{	195	,	3794	},
	{	198	,	3790	},
	{	203	,	3787	},
	{	203	,	3784	},
	{	205	,	3781	},
	{	213	,	3780	},
	{	210	,	3777	},
	{	215	,	3776	},
	{	220	,	3774	},
	{	220	,	3771	},
	{	223	,	3767	},
	{	225	,	3764	},
	{	225	,	3758	},
	{	223	,	3752	},
	{	230	,	3747	},
	{	233	,	3741	},
	{	238	,	3735	},
	{	245	,	3728	},
	{	248	,	3719	},
	{	255	,	3710	},
	{	260	,	3701	},
	{	265	,	3690	},
	{	283	,	3683	},
	{	310	,	3677	},
	{	348	,	3670	},
	{	383	,	3657	},
	{	405	,	3620	},
	{	450	,	3548	},
	{	558	,	3446	},
	{	485	,	3393	},
	{	408	,	3363	},
	{	365	,	3346	},
	{	338	,	3334	},
	{	315	,	3325	},
	{	300	,	3320	},
	{	293	,	3315	},
	{	285	,	3313	},
	{	283	,	3311	},
	{	278	,	3309	},
	{	275	,	3308	},
	{	273	,	3307	},
	{	275	,	3306	},
	{	273	,	3305	}
};


// T3 50C
R_PROFILE_STRUC r_profile_t3[] =
{              
	{	103	,	4198	},
	{	138	,	4177	},
	{	138	,	4159	},
	{	140	,	4144	},
	{	138	,	4129	},
	{	140	,	4115	},
	{	140	,	4101	},
	{	140	,	4088	},
	{	140	,	4075	},
	{	140	,	4062	},
	{	140	,	4050	},
	{	140	,	4038	},
	{	143	,	4026	},
	{	143	,	4015	},
	{	145	,	4004	},
	{	145	,	3993	},
	{	143	,	3982	},
	{	145	,	3972	},
	{	148	,	3963	},
	{	145	,	3952	},
	{	148	,	3943	},
	{	150	,	3934	},
	{	150	,	3925	},
	{	150	,	3916	},
	{	155	,	3909	},
	{	155	,	3901	},
	{	155	,	3892	},
	{	160	,	3885	},
	{	160	,	3876	},
	{	160	,	3866	},
	{	150	,	3852	},
	{	145	,	3840	},
	{	140	,	3831	},
	{	140	,	3824	},
	{	143	,	3818	},
	{	140	,	3812	},
	{	143	,	3807	},
	{	140	,	3802	},
	{	140	,	3797	},
	{	143	,	3793	},
	{	143	,	3789	},
	{	143	,	3785	},
	{	143	,	3782	},
	{	143	,	3778	},
	{	148	,	3776	},
	{	150	,	3773	},
	{	150	,	3771	},
	{	150	,	3768	},
	{	153	,	3765	},
	{	143	,	3756	},
	{	143	,	3747	},
	{	143	,	3740	},
	{	140	,	3735	},
	{	143	,	3729	},
	{	143	,	3723	},
	{	143	,	3718	},
	{	145	,	3713	},
	{	145	,	3706	},
	{	145	,	3697	},
	{	145	,	3689	},
	{	143	,	3679	},
	{	148	,	3672	},
	{	150	,	3667	},
	{	155	,	3663	},
	{	163	,	3657	},
	{	160	,	3641	},
	{	158	,	3592	},
	{	163	,	3517	},
	{	173	,	3407	},
	{	190	,	3274	},
	{	160	,	3263	},
	{	155	,	3260	},
	{	150	,	3259	},
	{	150	,	3258	},
	{	148	,	3257	},
	{	148	,	3258	},
	{	145	,	3257	},
	{	145	,	3256	},
	{	145	,	3256	},
	{	145	,	3256	},
	{	145	,	3256	},
	{	143	,	3255	}
};

// r-table profile for actual temperature. The size should be the same as T1, T2 and T3
R_PROFILE_STRUC r_profile_temperature[] =
{
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },         
	{0  , 0 },          
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
  {0  , 0 },
	{0  , 0 }, 
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 },
	{0  , 0 }, 
	{0  , 0 },	    
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 },   
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }, 
	{0  , 0 }  
};   


int fgauge_get_saddles(void);
BATTERY_PROFILE_STRUC_P fgauge_get_profile(kal_uint32 temperature);

int fgauge_get_saddles_r_table(void);
R_PROFILE_STRUC_P fgauge_get_profile_r_table(kal_uint32 temperature);

//#define CONFIG_POWER_VERIFY
