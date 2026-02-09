#ifndef __COMM_DEFINE_HEADER_FILE__
#define __COMM_DEFINE_HEADER_FILE__

//--------------------------------------------------------------------------------------------------------------
// Control Bytes
//--------------------------------------------------------------------------------------------------------------

enum
{
	LEVEL_CHANNEL_0_PZT0_H = 0xa0,		// (0xa0)
	LEVEL_CHANNEL_0_PZT0_L,				// (0xa1)
	LEVEL_CHANNEL_0_PZT1_H,				// (0xa2)
	LEVEL_CHANNEL_0_PZT1_L,				// (0xa3)
	LEVEL_CHANNEL_0_PZT2_H,				// (0xa4)
	LEVEL_CHANNEL_0_PZT2_L,				// (0xa5)
	LEVEL_CHANNEL_0_PZT3_H,				// (0xa6)
	LEVEL_CHANNEL_0_PZT3_L,				// (0xa7)

	LEVEL_CHANNEL_1_PZT0_H = 0xa8,		// (0xa8)
	LEVEL_CHANNEL_1_PZT0_L,				// (0xa9)
	LEVEL_CHANNEL_1_PZT1_H,				// (0xaa)
	LEVEL_CHANNEL_1_PZT1_L,				// (0xab)
	LEVEL_CHANNEL_1_PZT2_H,				// (0xac)
	LEVEL_CHANNEL_1_PZT2_L,				// (0xad)
	LEVEL_CHANNEL_1_PZT3_H,				// (0xae)
	LEVEL_CHANNEL_1_PZT3_L,				// (0xaf)

	ILLUM_COMMAND					= 0xb0,
	PZT_COMMAND						= 0xb1,

	ILL_2D_COUNT					= 0xb2,				// 2D Frame Count ( 1 ~ 10 )

	USE_3D_CHANNEL_0				= 0xb3,
	USE_3D_CHANNEL_1				= 0xb4,	

	SET_CHANNEL_0_BUCKET_MODE		= 0xb5,				// (0xb6) Set : Bucket
	SET_CHANNEL_1_BUCKET_MODE		= 0xb6,				// (0xb6) Set : Bucket

	SET_CHANNEL_0_MULTIBAND_NUM		= 0xb7,
	SET_CHANNEL_1_MULTIBAND_NUM		= 0xb8,

	SET_PZT_MANUAL_H				= 0xb9,				// Manual : PZT SET H
	SET_PZT_MANUAL_L				= 0xba,				// Manual : PZT SET L
	SET_2D_ILLUM_SEL				= 0xbb,

	SET_PZTPOSVALUE_START_L			= 0xbc,				// PZT의 Start Position을 지정한다.
	SET_PZTPOSVALUE_START_H			= 0xbd,				// 
	SET_PZTPOSVALUE_FRINGE_L		= 0xbe,				// PZT의 Fringe Position을 지정한다.
	SET_PZTPOSVALUE_FRINGE_H		= 0xbf,				//

	SET_TIME_FRAME_PERIOD_L			= 0xc0,				// Main Trigger Period   1=12.8us  65535=838.848ms
	SET_TIME_FRAME_PERIOD_H			= 0xc1,	
	
	SET_TIME_HIGH_DURATION_L		= 0xc2,
	SET_TIME_HIGH_DURATION_H		= 0xc3,

	SET_TIME_ILLUMINATION_L			= 0xc4,
	SET_TIME_ILLUMINATION_H			= 0xc5,

	SET_MB0_PZT_SCANSTEP			= 0xc8,				// Multiband0 (0xc8) ~ Multiband7 (0xcf)
	// PZT의 Scan Step을 설정(nm) 1:20nm
	// 3 :100,  4:120,  5:140,  6:160, 7:180,  8:200,  9:220
	// 10:240, 11:260, 12:300, 13:320,14:350, 16:400, 18:450, 20:500

	ILL_3D_MB0_COUNT_L				= 0xd0,				// 3D 프레임의 수(L)  mb0_cntl(0xd0) ~ mb7_cntl(0xd7) 
	ILL_3D_MB0_COUNT_H				= 0xd8,				// 3D 프레임의 수(H)  mb0_cnth(0xd8) ~ mb7_cnth(0xdf) 

	ILLUM_3D_CHANNEL_0_MB0_VALUE_LSB	= 0Xe0,				// 3D Illum Value (channel 0) MultiBand 0(0xe0)~ 7(0xe7)
	ILLUM_3D_CHANNEL_0_MB0_VALUE_MSB	= 0Xe4,				// 3D Illum Value (channel 0) MultiBand 0(0xe0)~ 7(0xe7)
	ILLUM_3D_CHANNEL_0_MB1_VALUE_LSB	= 0xe8,				// 3D Illum Value (channel 0) MultiBand 1(0xe8)~ 7(0xef)
	ILLUM_3D_CHANNEL_0_MB1_VALUE_MSB	= 0xec,				// 3D Illum Value (channel 0) MultiBand 1(0xe8)~ 7(0xef)

	SET_MEASURE_MODE				= 0xf0,				// pzt 구동 모드 셋(1: wsi, 0, 모아레)
	SET_EXPOSURE_POLARITY			= 0xf1,				// exposure polarity set(1:
	//SET_BUCKET					= 0xf2,				//Set : Bucket
	SET_LASER_ON_OFF				= 0xf3,				//Set : Laser

	ILL_2D_SET_MODE					= 0xf4,

	SET_MB_INTERVAL_ADJ_L			= 0xf5,				// wsi, multiband간 delay_l ;2010.3.31
	SET_MB_INTERVAL_ADJ_H			= 0xf6,				// wsi, multiband간 delay_h ;2010.3.31

	SET_DUAL_GRAB					= 0xf8,				// wis, Mutli ZMap

	ILLUM_CONTROLLER_TYPE			= 0xfe,
};

/*
						   ┌────────┐        ┌─
CPLD_FRAMEPERIOD       ──┘                └────┘

						   ┌───┐                  ┌─
CPLD_EXPOSUREDELAY     ──┘      └─────────┘

						   ┌─────┐
CPLD_EXPOSURTIME       ──┘          └─────────

								   ┌────┐
IllumExposure          ──────┘        └─────────

									   ┌──┐
Rest Time              ────────┘    └──────


CPLD_EXPOSUREDELAY < CPLD_EXPOSURTIME < CPLD_FRAMEPERIOD

*/
//--------------------------------------------------------------------------------------------------------------
// Illumination Command List (ILLUM_COMMAND)
//--------------------------------------------------------------------------------------------------------------

enum
{
	COMMAND_2D_0		= 0x10,		// Turn On 2D Frame - 0
	COMMAND_2D_1,					// Turn On 2D Frame - 1
	COMMAND_2D_2,					// Turn On 2D Frame - 2
	COMMAND_2D_3,					// Turn On 2D Frame - 3
	COMMAND_2D_4,					// Turn On 2D Frame - 4
	COMMAND_2D_5,					// Turn On 2D Frame - 5
	COMMAND_2D_6,					// Turn On 2D Frame - 6
	COMMAND_2D_7,					// Turn On 2D Frame - 7
	COMMAND_2D_8,					// Turn On 2D Frame - 8
	COMMAND_2D_9,					// Turn On 2D Frame - 9

	COMMAND_3D_0		= 0x20,		// Turn On 3D Frame - 0
	COMMAND_3D_1		= 0x30,		// Turn On 3D Frame - 1

	COMMAND_MAUAL_ON	= 0x41,	// 
	COMMAND_MAUAL_ONE	= 0x42,

	COMMAND_MAKE_2D_LOOKUP_TABLE	= 0x60,
	COMMAND_MAKE_2D_ILLUM_TABLE		= 0x61,


	MEASURE_START		= 0xf0,		// Start Measurement
	COMMAND_CLEAR		= 0xff,		// Turn Off All Illuminations

};


//--------------------------------------------------------------------------------------------------------------
// PZT Command List (PZT_COMMAND)
//--------------------------------------------------------------------------------------------------------------
enum
{
	PZTMOVE_FRINGEPOS	= 0x40,			// 20 um 위치 fringe가 있는 위치 
	// 초기 위치가 20 um 위치에 있다가 이때 화면에 간섭무늬가 보인다.
	// start position 10 um 위치로 이동한 후에 이 위치 부터 scanning을 시작한다.
	PZTMOVE_STARTPOS	= 0x41,			// 10 um 위치 scanning 시작 위치 

	PZTMOVE_MANUAL_CH0	= 0x50,		// 입력한 위치로 이동.
	PZTMOVE_MANUAL_CH1	= 0x51,		// 입력한 위치로 이동.
};

//--------------------------------------------------------------------------------------------------------------
// 3D Bucket Mode
//--------------------------------------------------------------------------------------------------------------
enum
{
	BUCKET_3	= 0x00,	// 3Frame
	BUCKET_3x2_4_90,	// 4Frame
	BUCKET_3x2_6_9045,	// 6Frame
	BUCKET_3x4_8_9045,	// 8Frame
	BUCKET_3x4_7,		// 7Frame

	BUCKET_WSI_128,		// 128Frame
	BUCKET_WSI_250,		// 128Frame
	BUCKET_WSI_256,		// 256Frame
	BUCKET_WSI_298,		// 298Frame
};
//--------------------------------------------------------------------------------------------------------------
// 3D Measure Mode
//--------------------------------------------------------------------------------------------------------------
enum
{
	MEASURE_WSI_MODE	= 0x00,	
	MEASURE_MOIRE_MODE 
};

enum
{	
	EXPOSURE_POLARITY_LOW = 0x0A,
	EXPOSURE_POLARITY_HIGH 
};
#endif
