#pragma once

#ifndef __IGRABGMANDEF_H__
#define  __IGRABGMANDEF_H__

//------------------------------------------------------
#define MAX_DEVICE_NUMBER				4
#define	MAX_CHANNEL_NUMBER				2

//------------------------------------------------------
//	Grab Function Result Status
//------------------------------------------------------


#define		IGSTATUS_GRAB_SUCCESS				WAIT_OBJECT_0	
#define		IGSTATUS_GRAB_TIMEOUT				WAIT_TIMEOUT

#define		IGSTATUS_GRAB_START_SUCCESS			WAIT_OBJECT_0+1
#define		IGSTATUS_GRAB_START_FAIL			WAIT_TIMEOUT+1	

#define		IGSTATUS_GRAB_STOP					WAIT_OBJECT_0+2

#define		IGSTATUS_GRAB_DMA_OVER_FAIL			WAIT_TIMEOUT+3	 
#define		IGSTATUS_GRAB_FIFO_OVER_FAIL		WAIT_TIMEOUT+4		 
#define		IGSTATUS_GRAB_FAIL					WAIT_TIMEOUT+5	
#define		IGSTATUS_GRAB_PT_PASSED_FAIL		WAIT_TIMEOUT+6		 
#define		IGSTATUS_GRAB_PT_NO_DETECTED_FAIL	WAIT_TIMEOUT+7		 
#define		IGSTATUS_GRAB_OPEN_CABLE_FAIL		WAIT_TIMEOUT+8		 	 

#define		SYS_IO_SPATIAL_DIR			0
#define		SYS_IO_DMA_RST				1	
#define		SYS_IO_ACQ_CLK_SEL			2

#define		CC_2						0
#define		CC_3						1
#define		CC_4						2

// ICF Section
enum enumICF_SECTION
{
	ICF_CTL_CAM_MODE,
	ICF_CTL_TAP_SIZE,
	ICF_CTL_IMAGE_BIT,
	ICF_CTL_TRIGGER_MODE,
	ICF_CTL_PAGE_TRIGGER,
	ICF_CTL_TRIGGER_TYPE,
	ICF_CTL_CL_MODE,
	ICF_CTL_EXPOSURE_HIGH_TIME,
	ICF_CTL_EXPOSURE_LOW_TIME,
	ICF_CTL_HORIZONTAL_SIZE,
	ICF_CTL_VERTICAL_SIZE,

	ICF_CTL_TRIGGER_DIV_ENB,
	ICF_CTL_TRIGGER_PULSE_CNT,
	ICF_CTL_TRIGGER_DIV_CNT,

	ICF_CTL_TAP_CON0,
	ICF_CTL_TAP_CON1,
	ICF_CTL_TAP_CON2,
	ICF_CTL_TAP_CON3,
	ICF_CTL_TAP_CON4,
	ICF_CTL_TAP_CON5,
	ICF_CTL_TAP_CON6,
	ICF_CTL_TAP_CON7,
	ICF_CTL_TAP_DIR0,
	ICF_CTL_TAP_DIR1,
	ICF_CTL_TAP_DIR2,
	ICF_CTL_TAP_DIR3,
	ICF_CTL_TAP_DIR4,
	ICF_CTL_TAP_DIR5,
	ICF_CTL_TAP_DIR6,
	ICF_CTL_TAP_DIR7,
	ICF_CTL_TAP_DIR8,		// [2019.03.26] 추가
	ICF_CTL_TAP_DIR9,		// [2019.03.26] 추가
	ICF_CTL_TAP_PARTITION,
	ICF_CTL_TAP_TAPCHANGE,
	ICF_CTL_TAP_IMAGEFLIP,

	ICF_CTL_SPATIALCORR_DIR,
	ICF_CTL_CCD_VALUE,

	ICF_CTL_ACQUISITION_CLK_SEL,
	ICF_CTL_CUSTOM_INTR_NUM
};

enum _enumTriggermode
{
	Freerun = 0,
	Trigger = 1,

};

enum _enumTriggerType
{
	HardwareTrigger = 0,
	OnTriggerEvent,
	FG_Trigger
};

enum _enumImagebit
{
	Image8bit = 0,
	Image10bit,		// not used
	Image12bit,		// not used
	Image14bit,		// not used
	Image16bit,		// not used
	Image24bit = 5
};

enum _enumCameraMode						//[2013.08.26]추가
{
	AreaScan,
	LineScan
};

enum _ACQ_VIDEO_SELECTION
{
	Lval = 0,
	Dval = 1
};

enum _IGRAB_BOARD_TYPE
{
	IG_BDTYPE_BASE = 0,
	IG_BDTYPE_MEDIUM = 1,
	IG_BDTYPE_FULL = 2,
	IG_BDTYPE_DUALBASE = 3

};


enum _enumGrabberChannel
{
	CHANNEL_A = 0,
	CHANNEL_B

};

enum MtgWaitOp {
	MTG_END_WAIT = 0,
	MTG_GRB_ISSUE
};



//Ig_GrabMutliProcess callback type.
typedef void(*IGCALLBACK_GRBEVT)(LPVOID, //[in]pUserPtr	: User Pointer
	DWORD,	//[out]dwCount	: Grab count.(1,2,3,4,.......)
	LPVOID,	//[out]pParam1	: At DMAMemoryMode, Direct Grab Address(Current Grabbed DMA Memory : Virtual Address)
	LPVOID	//[out]pParam2	: Reserve(NULL)	
	);


//------------------------------------------------------

typedef struct __tagTapConfig
{
	union
	{
		struct
		{
			UINT	con0 : 1;	//[0]
			UINT	con1 : 1;	//[1]
			UINT	con2 : 1;	//[2]
			UINT	con3 : 1;	//[3]
			UINT	con4 : 1;	//[4]
			UINT	con5 : 1;	//[5]
			UINT	con6 : 1;	//[6]
			UINT	con7 : 1;	//[7]

			UINT	dir0 : 1;	//[8]
			UINT	dir1 : 1;	//[9]
			UINT	dir2 : 1;	//[10]
			UINT	dir3 : 1;	//[11]
			UINT	dir4 : 1;	//[12]
			UINT	dir5 : 1;	//[13]
			UINT	dir6 : 1;	//[14]
			UINT	dir7 : 1;	//[15] 
			UINT	dir8 : 1;	//[16], 10Tap, [2019.03.26] 추가
			UINT	dir9 : 1; //[17], 10Tap, [2019.03.26] 추가
			UINT	res : 14;
		};

		UINT		nTAPCONFIG;			//__tagTapConfig;		
	};
}igs_tapconfig;


//GCF Parmaeter structure
typedef struct __tagICF_PARAM
{
	UINT	CamMode;						//[2013.08.26]추가-[0]:AreaScan, [1]:LineScan
	UINT	TriggerMode;					//Trigger : Freerun / External
	UINT	TriggerType;					//external / On trigger / Fg Tr
	UINT	PageTrigger;
	UINT	ClSel;
	UINT	TapSize;
	UINT	ExposureHighCount;
	FLOAT	ExposureHighTime;
	UINT	ExposureLowCount;
	FLOAT	ExposureLowTime;
	UINT	ImageBit;
	UINT	TriggerDivision;
	UINT	HorizontalSize;
	UINT	VerticalSize;

	UINT	CcdDir;
	UINT	CcdVal;

	//Tap
	UINT	TrgDivEnb;
	UINT	TrgPulseCnt;
	UINT	TrgDivCnt;
	UINT	AcqClkSel;

	UINT	Cc2;
	UINT	Cc3;
	UINT	Cc4;
	igs_tapconfig	stTap;	//tap configuration 
}IFG_ICFPARAM;

typedef struct __tagSystemMemoryInfo32 {
	ULONG	nNumberOfMemory;
	ULONG   nMemorySize;		//bytes size
	ULONG	nUserAddr[4096];
	ULONG	nPhysAddr[4096];
} IGSYSTEM_MEM_INFO32;//32bit OS

typedef struct __tagSystemMemoryInfo {
	ULONG		nNumberOfMemory;
	ULONG		nMemorySize;	//bytes size
	ULONGLONG	nUserAddr[4096];
	ULONGLONG	nPhysAddr[4096];
}IGSYSTEM_MEM_INFO;//64bit OS

				   //}}}}

typedef struct _tagfileInfoHead
{
	char sDate[50];
	char sEditor[50];
	char sCamCompany[50];
	char sCamName[50];

}IGS_FILEINFORM;



#endif //__IGRABGMANDEF_H__


