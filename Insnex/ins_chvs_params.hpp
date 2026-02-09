#pragma once

#include <stdint.h>
#include <vector>
#include "ins_chvs_def.h"

/*
Callback event type
*/
#define INS_Event_GrabStart				    (0x00000000)
#define INS_Event_FrameReady			    (0x00000001)
#define INS_Event_GrabStop				    (0x00000002)
#define	INS_Event_FrameLost				    (0x00000003)
#define INS_Event_TimeOut					(0x00000004)

/*
User defined callback
*/

#define CALLBACK    __stdcall
typedef void(CALLBACK* RunHookFnPtr)(void* handle, void* pInfo, void* pBuffer, void* pUser);
typedef void (CALLBACK* RemoveHookFnPtr)(void* pUser);

#define UNREFERENCED_PARAMETER(P)          (P)

typedef float float32_t;

#define MAX_NUM_DEVICE              16               // Maximum number of supported devices
#define MAX_NUM_PORTS               4                // Maximum supported network ports


#define INS_INFO_MAX_LENGTH         64
#define INS_ERROR_MAX_LENGTH        64

// device type
enum class InsCHVS_DeviceType
{//INS_10GM
    INS_CHVS_363_10GM_BB = INS_TYPE(INS_CHVS_363, INS_GIGE, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_363_10GM_AA = INS_TYPE(INS_CHVS_363, INS_GIGE, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),
    INS_CHVS_363_10GM_BA = INS_TYPE(INS_CHVS_363, INS_GIGE, INS_MONO, INS_DPI_600, INS_HIGH_SPEED),
    INS_CHVS_363_10GM_AA1 = INS_TYPE(INS_CHVS_363, INS_GIGE, INS_MONO, INS_DPI_300, INS_HIGH_SPEED),

    INS_CHVS_550_10GM_BB = INS_TYPE(INS_CHVS_550, INS_GIGE, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_550_10GM_AA = INS_TYPE(INS_CHVS_550, INS_GIGE, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),
    INS_CHVS_550_10GM_BA = INS_TYPE(INS_CHVS_550, INS_GIGE, INS_MONO, INS_DPI_600, INS_HIGH_SPEED),
    INS_CHVS_550_10GM_AA1 = INS_TYPE(INS_CHVS_550, INS_GIGE, INS_MONO, INS_DPI_300, INS_HIGH_SPEED),
    INS_CHVS_550_10GM_CC = INS_TYPE(INS_CHVS_550, INS_GIGE, INS_MONO, INS_DPI_1200, INS_MEDIUM_SPEED),

    INS_CHVS_934_10GM_BB = INS_TYPE(INS_CHVS_934, INS_GIGE, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_934_10GM_AA = INS_TYPE(INS_CHVS_934, INS_GIGE, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),
    INS_CHVS_934_10GM_BA = INS_TYPE(INS_CHVS_934, INS_GIGE, INS_MONO, INS_DPI_600, INS_HIGH_SPEED),
    INS_CHVS_934_10GM_AA1 = INS_TYPE(INS_CHVS_934, INS_GIGE, INS_MONO, INS_DPI_300, INS_HIGH_SPEED),

    INS_CHVS_620_10GM_BA = INS_TYPE(INS_CHVS_620, INS_GIGE, INS_MONO, INS_DPI_600, INS_HIGH_SPEED),
    INS_CHVS_620_10GM_AA1 = INS_TYPE(INS_CHVS_620, INS_GIGE, INS_MONO, INS_DPI_300, INS_HIGH_SPEED),

    INS_CHVS_1245_10GM_BB = INS_TYPE(INS_CHVS_1245, INS_GIGE, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_1245_10GM_AA = INS_TYPE(INS_CHVS_1245, INS_GIGE, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),

    INS_CHVS_1350_10GM_BB = INS_TYPE(INS_CHVS_1350, INS_GIGE, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_1350_10GM_AA = INS_TYPE(INS_CHVS_1350, INS_GIGE, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),
    INS_CHVS_1350_10GM_BA = INS_TYPE(INS_CHVS_1350, INS_GIGE, INS_MONO, INS_DPI_600, INS_HIGH_SPEED),
    INS_CHVS_1350_10GM_AA1 = INS_TYPE(INS_CHVS_1350, INS_GIGE, INS_MONO, INS_DPI_300, INS_HIGH_SPEED),

    INS_CHVS_1550_10GM_BB = INS_TYPE(INS_CHVS_1550, INS_GIGE, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_1550_10GM_AA = INS_TYPE(INS_CHVS_1550, INS_GIGE, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),
    INS_CHVS_1550_10GM_BA = INS_TYPE(INS_CHVS_1550, INS_GIGE, INS_MONO, INS_DPI_600, INS_HIGH_SPEED),
    INS_CHVS_1550_10GM_AA1 = INS_TYPE(INS_CHVS_1550, INS_GIGE, INS_MONO, INS_DPI_300, INS_HIGH_SPEED),

    INS_CHVS_366_10GM_BC = INS_TYPE(INS_CHVS_366, INS_GIGE, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_366_10GM_AB = INS_TYPE(INS_CHVS_366, INS_GIGE, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),
    INS_CHVS_366_10GM_CC = INS_TYPE(INS_CHVS_366, INS_GIGE, INS_MONO, INS_DPI_1200, INS_MEDIUM_SPEED),

    INS_CHVS_310_10GM_BA = INS_TYPE(INS_CHVS_310, INS_GIGE, INS_MONO, INS_DPI_600, INS_HIGH_SPEED),
    INS_CHVS_310_10GM_AA1 = INS_TYPE(INS_CHVS_310, INS_GIGE, INS_MONO, INS_DPI_300, INS_HIGH_SPEED),

    INS_CHVS_190_10GM_BB = INS_TYPE(INS_CHVS_190, INS_GIGE, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_190_10GM_AA = INS_TYPE(INS_CHVS_190, INS_GIGE, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),

    INS_CHVS_230_10GM_BB = INS_TYPE(INS_CHVS_230, INS_GIGE, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_230_10GM_AA = INS_TYPE(INS_CHVS_230, INS_GIGE, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),

    INS_CHVS_1453_10GM_BB = INS_TYPE(INS_CHVS_1453, INS_GIGE, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_1453_10GM_AA = INS_TYPE(INS_CHVS_1453, INS_GIGE, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),

    INS_CHVS_813_10GM_BB = INS_TYPE(INS_CHVS_813, INS_GIGE, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_813_10GM_AA = INS_TYPE(INS_CHVS_813, INS_GIGE, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),

    INS_CHVS_1600_10GM_BB = INS_TYPE(INS_CHVS_1600, INS_GIGE, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_1600_10GM_AA = INS_TYPE(INS_CHVS_1600, INS_GIGE, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),

    INS_CHVS_415_10GM_BB = INS_TYPE(INS_CHVS_415, INS_GIGE, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_415_10GM_AA = INS_TYPE(INS_CHVS_415, INS_GIGE, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),

    INS_CHVS_450_10GM_BB = INS_TYPE(INS_CHVS_450, INS_GIGE, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_450_10GM_AA = INS_TYPE(INS_CHVS_450, INS_GIGE, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),

    INS_CHVS_940_10GM_CC = INS_TYPE(INS_CHVS_940, INS_GIGE, INS_MONO, INS_DPI_1200, INS_MEDIUM_SPEED),

    INS_CHVS_311_10GC_BB = INS_TYPE(INS_CHVS_311, INS_GIGE, INS_COLOR, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_311_10GC_AA = INS_TYPE(INS_CHVS_311, INS_GIGE, INS_COLOR, INS_DPI_300, INS_MEDIUM_SPEED),

    INS_CHVS_69_10GC_BB = INS_TYPE(INS_CHVS_69, INS_GIGE, INS_COLOR, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_69_10GC_AA = INS_TYPE(INS_CHVS_69, INS_GIGE, INS_COLOR, INS_DPI_300, INS_MEDIUM_SPEED),

    INS_CHVS_35_10GC_BB = INS_TYPE(INS_CHVS_35, INS_GIGE, INS_COLOR, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_35_10GC_AA = INS_TYPE(INS_CHVS_35, INS_GIGE, INS_COLOR, INS_DPI_300, INS_MEDIUM_SPEED),

    INS_CHVS_363_10GC_BB = INS_TYPE(INS_CHVS_363, INS_GIGE, INS_COLOR, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_363_10GC_AA = INS_TYPE(INS_CHVS_363, INS_GIGE, INS_COLOR, INS_DPI_300, INS_MEDIUM_SPEED),

    INS_LXM_114_10GM_18L = INS_TYPE(INS_LXM_114, INS_GIGE, INS_MONO, INS_DPI_1800, INS_LOW_SPEED),
    INS_LXM_114_10GM_9L = INS_TYPE(INS_LXM_114, INS_GIGE, INS_MONO, INS_DPI_900, INS_LOW_SPEED),

    INS_LXM_228_10GM_18L = INS_TYPE(INS_LXM_228, INS_GIGE, INS_MONO, INS_DPI_1800, INS_LOW_SPEED),
    INS_LXM_228_10GM_9L = INS_TYPE(INS_LXM_228, INS_GIGE, INS_MONO, INS_DPI_900, INS_LOW_SPEED),

    INS_LXM_340_10GM_18L = INS_TYPE(INS_LXM_340, INS_GIGE, INS_MONO, INS_DPI_1800, INS_LOW_SPEED),
    INS_LXM_340_10GM_9M = INS_TYPE(INS_LXM_340, INS_GIGE, INS_MONO, INS_DPI_900, INS_LOW_SPEED),

    INS_LXM_328_10GM_18L = INS_TYPE(INS_LXM_328, INS_GIGE, INS_MONO, INS_DPI_1800, INS_LOW_SPEED),
    INS_LXM_328_10GM_9M = INS_TYPE(INS_LXM_328, INS_GIGE, INS_MONO, INS_DPI_900, INS_LOW_SPEED),

    INS_LXM_360_10GM_18L = INS_TYPE(INS_LXM_360, INS_GIGE, INS_MONO, INS_DPI_1800, INS_LOW_SPEED),
    INS_LXM_360_10GM_9M = INS_TYPE(INS_LXM_360, INS_GIGE, INS_MONO, INS_DPI_900, INS_LOW_SPEED),

    INS_LXM_656_10GM_18L = INS_TYPE(INS_LXM_656, INS_GIGE, INS_MONO, INS_DPI_1800, INS_LOW_SPEED),
    INS_LXM_656_10GM_9M = INS_TYPE(INS_LXM_656, INS_GIGE, INS_MONO, INS_DPI_900, INS_LOW_SPEED),

    INS_LXM_164_10GM_36L = INS_TYPE(INS_LXM_164, INS_GIGE, INS_MONO, INS_DPI_3600, INS_LOW_SPEED),

    INS_LXM_340_10GC_18L = INS_TYPE(INS_LXM_340, INS_GIGE, INS_COLOR, INS_DPI_1800, INS_LOW_SPEED),
    INS_LXM_340_10GC_9M = INS_TYPE(INS_LXM_340, INS_GIGE, INS_COLOR, INS_DPI_900, INS_LOW_SPEED),

    INS_CHVS_363_CLM_6M = INS_TYPE(INS_CHVS_363, INS_CAMERALINK, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_363_CLM_3F = INS_TYPE(INS_CHVS_363, INS_CAMERALINK, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),

    INS_CHVS_366_CLM_12L = INS_TYPE(INS_CHVS_366, INS_CAMERALINK, INS_MONO, INS_DPI_1200, INS_MEDIUM_SPEED),

    INS_CHVS_550_CLM_6M = INS_TYPE(INS_CHVS_550, INS_CAMERALINK, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_550_CLM_3F = INS_TYPE(INS_CHVS_550, INS_CAMERALINK, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),

    INS_CHVS_934_CLM_6M = INS_TYPE(INS_CHVS_934, INS_CAMERALINK, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_934_CLM_3F = INS_TYPE(INS_CHVS_934, INS_CAMERALINK, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),

    INS_CHVS_1245_CLM_6M = INS_TYPE(INS_CHVS_1245, INS_CAMERALINK, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_1245_CLM_3F = INS_TYPE(INS_CHVS_1245, INS_CAMERALINK, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),

    INS_CHVS_1350_CLM_6M = INS_TYPE(INS_CHVS_1350, INS_CAMERALINK, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_1350_CLM_3F = INS_TYPE(INS_CHVS_1350, INS_CAMERALINK, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),

    INS_CHVS_1550_CLM_6M = INS_TYPE(INS_CHVS_1550, INS_CAMERALINK, INS_MONO, INS_DPI_600, INS_MEDIUM_SPEED),
    INS_CHVS_1550_CLM_3F = INS_TYPE(INS_CHVS_1550, INS_CAMERALINK, INS_MONO, INS_DPI_300, INS_MEDIUM_SPEED),
};

// GigE device infomation
typedef struct InsCHVS_GigeInfo
{
    uint32_t                BoardIndex;                              /* Index of the acquisition card. */
    uint32_t                MasterPort;                              /* Network port of the acquisition card. */
    char                    MAC[INS_INFO_MAX_LENGTH];                /* MAC address. */
    char                    Ip[INS_INFO_MAX_LENGTH];                 /* Current IP address. */
    char                    GateWay[INS_INFO_MAX_LENGTH];            /* Gateway address. */
    char                    SubNetMask[INS_INFO_MAX_LENGTH];         /* Subnet mask. */
    char                    DevType[INS_INFO_MAX_LENGTH];            /* Device type. */
    char                    SN[INS_INFO_MAX_LENGTH];                 /* Device serial number. */
    char                    SoftVersion[INS_INFO_MAX_LENGTH];        /* Device firmware version. */
    char                    SoftVersionDate[INS_INFO_MAX_LENGTH];    /* Version release date. */
    char                    Vendor[INS_INFO_MAX_LENGTH];             /* Manufacturer. */
    unsigned int            Reserved[4];                             /* Reserved for future use. */
} InsCHVS_GigeInfo;

// CameraLink device infomation
typedef struct InsCHVS_CamLInfo
{
    uint32_t HostInterface;                         /* Host interface type: 1 - USB 3.0; 2 - PCIe x4. */
    uint32_t BoardIndex;                            /* Index of the board. */
    char FullName[INS_INFO_MAX_LENGTH];             /* Full unique device name for identification. */
    char FriendlyName[INS_INFO_MAX_LENGTH];         /* User-friendly device name for display purposes. */
    char VendorName[INS_INFO_MAX_LENGTH];           /* Manufacturer of the device. */
    char ModelName[INS_INFO_MAX_LENGTH];            /* Model name of the device. */
    char SerialNumber[INS_INFO_MAX_LENGTH];         /* Serial number of the device. */
    char DeviceClass[INS_INFO_MAX_LENGTH];          /* Device class (transport layer) used by the device. */
    char DeviceVersion[INS_INFO_MAX_LENGTH];        /* Device version as reported by the device. */
    char UserDefinedName[INS_INFO_MAX_LENGTH];      /* User-defined name, if present, as reported by the device. */
    char DevType[INS_INFO_MAX_LENGTH];              /* Device type. */
    char SN[INS_INFO_MAX_LENGTH];                   /* Device serial number. */
    char SoftVersion[INS_INFO_MAX_LENGTH];          /* Device firmware version. */
    char SoftVersionDate[INS_INFO_MAX_LENGTH];      /* Version release date. */
    char COM[INS_INFO_MAX_LENGTH];                  /* Device communication port. */
    char Reserved[4];                               /* Reserved for future use. */
} InsCHVS_CamLInfo;


#define INS_UNKNOW_DEVICE                0x00000000                  /* Unknown device type, reserved meaning. */
#define INS_GIGE_DEVICE                  0x00000001                  /* GigE device. */
#define INS_CAMERALINK_DEVICE            0x00000002                  /* CameraLink device. */

// Camera device information
typedef struct InsCHVS_DeviceInfo
{
    InsCHVS_DeviceType      InsType;                        /* Device type. */
    unsigned int            TransLayerType;                 /* Device transport layer protocol type. */
    unsigned int            InfoArrayCount;                 /* Count of information arrays. */
    union
    {
        InsCHVS_GigeInfo    GigeInfo[MAX_NUM_PORTS];        /* GigE device information for multiple ports. */
        InsCHVS_CamLInfo    CamLInfo[MAX_NUM_PORTS];        /* CameraLink device information for multiple ports. */
    } SpecialInfo;                                          /* Special information for different device types. */
} InsCHVS_DeviceInfo;

// List of stored enumerated devices
typedef struct InsCHVS_DeviceInfoList
{
    InsCHVS_DeviceInfo DeviceInfo[MAX_NUM_DEVICE];           /* Array of device information. */
    int DeviceCount;                                         /* Count of devices. */
} InsCHVS_DeviceInfoList;

// Device running status
enum class InsCHVS_RunState
{
    Running = 0,    /* Device is running. */
    Pause = 1,      /* Device is paused. */
    Stop = 2        /* Device is stopped. */
};

// Device connection status
enum class InsCHVS_ConnectionState {
    Disconnected,           /* Not connected or turn off the camera normally */
    Connecting,             /* On connection */
    Connected,              /* Connected already */
    ConnectionFailed,       /* Connection failed */
    AbnormalDisconnection,  /* Abnormal disconnection */
};

// Function enable mode
enum class InsCHVS_FuncEnable
{
    Ins_Disable = 0,    /* Function is disabled. */
    Ins_Enable = 1      /* Function is enabled. */
};

// Image transmission format (PixelFormat)
enum class InsCHVS_PixelFormat
{
    Mono8 = 0,         /* 8-bit monochrome. */
    Mono10 = 1,        /* 10-bit monochrome. */
    RGB888 = 2,        /* 24-bit RGB color. */
};

// Line scan trigger source
enum class InsCHVS_LineTriggerSource
{
    Internal_Clock = 0,    /* Internal clock trigger. */
    External_IO = 1,       /* External I/O trigger. */
    External_Encoder = 2,  /* External encoder trigger. */
    Fake_Pattern = 3,      /* Fake pattern trigger. */
    No_active = 0xff       /* No active trigger. */
};

// DPI settings
enum class InsCHVS_DPI
{
    DPI300 = 0,    /* 300 DPI. */
    DPI600 = 1,    /* 600 DPI. */
    DPI1200 = 2,   /* 1200 DPI. */
    DPI900 = 3,    /* 900 DPI. */
    DPI1800 = 4,   /* 1800 DPI. */
    DPI3600 = 5,   /* 3600 DPI. */
};

// Encoder counting mode
enum class InsCHVS_Encoder_CountMode
{
    PhA_RisingEdge_OneWay = 0,    /* Count on A phase rising edge - one way. */
    PhA_RisingEdge_TwoWay = 1,    /* Count on A phase rising edge - two way. */
    PhA_BothEdges_TwoWay = 2,      /* Count on both edges of A phase - two way. */
    PhA_PhB_BothEdges_TwoWay = 3   /* Count on both edges of A and B phases - two way. */
};

// Encoder trigger direction
enum class InsCHVS_Encoder_TriggerMode
{
    Forward_Scan = 0,      /* Forward scanning. */
    Reverse_Tracking = 1,  /* Reverse tracking, ignore backward. */
    Reverse_Ignore = 2,    /* Reverse tracking, backward distance is not counted. */
    TwoWay_Scan = 3        /* Two-way scanning. */
};

// External I/O trigger mode
enum class InsCHVS_IO_TriggerMode
{
    Input0_Start_Input1_End = 0,    /* Input 0 as start, input 1 as stop. */
    Input1_Start_Input0_End = 1,    /* Input 1 as start, input 0 as stop. */
    Input0_MultiFrames = 2,          /* Input 0 as start, fixed trigger numbers for one input pulse. */
    Input1_MultiFrames = 3,          /* Input 1 as start, fixed trigger numbers for one input pulse. */
    Input0_BothEdges = 4,            /* Input 0 is valid, rising as start and falling as stop. */
    Input1_BothEdges = 5,            /* Input 1 is valid, rising as start and falling as stop. */
    Input0_Divide = 6,               /* Input 0 as pulse, fixed pulses make one (or more) trigger. */
    Input1_Divide = 7                 /* Input 1 as pulse, fixed pulses make one (or more) trigger. */
};

// Fake pattern mode
enum class InsCHVS_FakePatternMode
{
    FPGA_AllBlack = 0,    /* All black pattern. */
    FPGA_AllWhite = 1,    /* All white pattern. */
    FPGA_Zebra = 2,       /* Black and white striped pattern. */
    FPGA_Increase = 3,    /* Horizontally increasing brightness pattern. */
    FPGA_Column8Inc = 4   /* 8-column incrementing sequence. */
};

// Image information
enum class INS_Result; // Forward declaration
typedef struct InsCHVS_ProcessInfo
{
    INS_Result      RetVal;            /* Result of the process. */
    long long       FrameCount;        /* Number of frames processed. */
    bool            MetaEnable;        /* Flag to enable metadata. */
    int             FrameRate;         /* Frame rate of the acquisition. */
    double          AcquisitionRate;   /* Rate of acquisition. */
    double          BandWidth;         /* Bandwidth used. */
    int             NetworkPorts;      /* Number of network ports. */
    uint32_t        ErrNum;            /* Error number. */
    char            ErrStr[INS_ERROR_MAX_LENGTH]; /* Error string. */
} InsCHVS_ProcessInfo;

// Image buffer
typedef struct InsCHVS_Buffer
{
    unsigned char* p_data;             /* Pointer to image data. */
    unsigned char* p_data2;            /* Pointer to second image data (if applicable). */
    unsigned char* p_data3;            /* Pointer to third image data (if applicable). */
    unsigned int            width;      /* Width of the image. */
    unsigned int            height;     /* Height of the image. */
    unsigned int            step;       /* Step size in bytes. */
    unsigned long long      frameIndex; /* Index of the frame. */
    InsCHVS_PixelFormat image_type;    /* Format of the image. */
} InsCHVS_Buffer;

// LED trigger mode
enum class InsCHVS_LED_TriggerMode
{
    LED_SimultaneousDualBrightness = 0x01, /* Simultaneous dual brightness mode. */
    LED_SeparateTimedFlashingBrightness = 0x03, /* Separate timed flashing brightness mode. */
    LED_TripleIndependentBrightness = 0x07, /* Triple independent brightness mode. */
    LED_BacklightOnly = 0x04                /* Backlight only mode. */
};

// Blend trigger start input source
enum class InsCHVS_BlendTriggerStartSource
{
    ManualControl = 0,    /* Manual control start. */
    External_IO = 1,      /* External I/O start. */
};

// Blend trigger end input source
enum class InsCHVS_BlendTriggerEndSource
{
    ManualControl = 0,                /* Manual control end. */
    External_IO = 1,                  /* External I/O end. */
    External_EncoderWidth = 2,        /* External encoder width end. */
    External_TriggerNum = 3,          /* External trigger number end. */
    Timer = 4,                        /* Timer end. */
};

// Blend trigger I/O mode
enum class InsCHVS_BlendTriggerIOMode
{
    Input0_Rising = 0,      /* Rising edge of input 0. */
    Input0_Falling,         /* Falling edge of input 0. */
    Input1_Rising,          /* Rising edge of input 1. */
    Input1_Falling,         /* Falling edge of input 1. */
};

// Return type
enum class INS_Result
{
    INS_OK = 0,                     // Operation successful;
    INS_GENERIC_FAILED,             // Operation failed (generic), for errors that are not easily categorized, classified as basic error;
    INS_TIMEOUT,                    // Operation timed out, mainly during device operations like bright calibration, dark correction, and pixel precision calibration;
    INS_CANCEL,                     // Operation canceled, manually interrupted ongoing operation (currently unused);
    INS_ABORTED,                    // Operation aborted, interrupted based on device running status (currently unused);
    INS_NO_LISENCE,                 // No available license for the upper computer SDK;
    INS_NO_DEVICE,                  // Operation failed, no available device object;
    // 
    INS_STREAM_ERR,                 // Error occurred while operating image stream object;
    INS_FRAME_LOST,                 // Image data transmission anomaly, frame loss detected;
    INS_PARAM_IMPROPER,             // Exception occurred when loading parameters from the device, data mismatch (possibly due to version misalignment);
    INS_PARAM_ERR,                  // Parameter error;
    INS_RUNNING_FORBIDDEN,          // Device is in image capturing process, modification of capture-related parameters is forbidden;
    INS_NOT_RUNNING_FORBIDDEN,      // Device has not started the image capturing process, operation forbidden;
    // 
    INS_FILE_OPEN_ERR,              // Exception opening parameter configuration file for saving (invalid file path or file);
    INS_FILE_SAVE_ERR,              // Exception saving parameter configuration file (invalid file path or file);
    INS_FILE_DEVICE_TYPE_ERR,       // Configuration file type does not match camera type when loading;
    INS_FILE_INVALID,               // Exception loading/saving parameter configuration file (invalid file path or file);
    // 
    INS_ACQ_NO_SOURCE_ACTIVE,       // Error before starting image acquisition, no acquisition signal source configured;
    INS_ACQ_SOURCE_INVALID,         // Error before starting image acquisition, invalid acquisition signal source (rarely triggered);
    // 
    INS_DEVICE_NOT_FOUND,           // Device scan failed, no device connected;
    INS_DEVICE_CONNECTED,           // Device connected;
    INS_DEVICE_NO_MATCHING,         // Failed to connect to device, no matching IP found;
    INS_DEVICE_CONNECT_ERR,         // Connection failure due to error during device connection;
    INS_DEVICE_ID_ERR,              // Connected device object ID is invalid;
    // 
    INS_SERIAL_TIMEOUT,             // Serial read/write timeout;
    INS_SERIAL_NOT_OPEN,            // Serial port not open when operating;
    INS_DISCONNECTED,               // Device disconnected;
    INS_SERIAL_COMM_ERR,            // General serial communication error (currently unused);
    INS_SERIAL_VERIFY_ERR,          // Serial communication protocol verification error;
    INS_SERIAL_SENSOR_ERR,          // Error when operating sensor in serial communication;
    // 
    INS_IPORT_TIMEOUT,              // Timeout when directly operating iport;
    INS_IPORT_INVALID_PARAM,        // Invalid parameter input when directly operating iport;
    // 
    INS_CALIBRATE_DEFAULT_ERR,      // Camera calibration failed;
    INS_CALIBRATE_WARNING,          // Warning of significant differences in light source or reflective board, results may be inadequate;
    INS_LIGHT_ERR,                  // Brightness cannot be achieved;
    INS_LIGHT_AUTOADJUST_ERR,       // Brightness cannot be adjusted automatically;
    // 
    INS_CALLBACK_ERROR,             // Callback registration failed;
    // 
    INS_NOT_DEVELOPED,              // Reserved functionality not implemented;
    INS_MEMORY_ERR,                 // Insufficient memory space, or memory error;
    // 
    INS_SIZE_ERROR,                 // Container count error;
    INS_SET_INFO_ERROR,             // Failed to set parameter;
    INS_BOARD_INIT_EER,             // Acquisition card initialization failed;
    INS_BOARD_FRAME_COUNT_EER,      // Failed to set acquisition card buffer count;
    INS_INSUFFICIENT_RESOURCES,     // No acquisition card found;
    // 
    INS_BUFFER_HEIGHT_FORBIDDEN,    // Operation forbidden due to image height;
    // 
    INS_PIXEL_ERROR,                // MONO error;
    INS_CHAR_LENGTH_LESS,           // Parameter string length too short;
    INS_HEIGHT_TOO_SMALL,           // Height is invalid;
    INS_HEIGHT_TOO_BIG,             // Height is invalid;
    // 
    INS_LED_TRIGGERMODE_FORBIDDEN,  // Current light source mode is not allowed;
    INS_STROBING_MULTIPLE,          // This function is not enabled, automatic segmentation under strobing;
    INS_ROI_CONFLICT,               // Conflict with ROI function;
    INS_META_CONFLICT,              // Conflict with META function;
    INS_FUSION_CONFLICT,            // Conflict with fusion algorithm;
    INS_FLIP_CONFLICT,              // Conflict with mirroring;

    INS_ENOCDER_SETTING_ILLEGAL,    // Encoder mode setting is illegal, please refer to the manual for reasonable settings;
    INS_FPGA_FIRMWARE_NOT_SUPPORTED, // FPGA hardware does not support this feature;

    INS_CAMERALINK_NOT_SUPPORTED,           // CameraLink camera does not support this feature;
    INS_COMPUTER_FIRMWARE_NOT_SUPPORTED,    // Computer does not have NVIDIA graphics card installed;
    INS_LED_TDI_ERROR,                      // Current light source quantity does not match light source mode;
    INS_RIGISTER_READ_TIMEOUT,              // Camera communication timeout;
    INS_RIGISTER_WRITE_TIMEOUT,             // Camera communication timeout;
    INS_RIGISTER_BLOCK_READ_TIMEOUT,        // Camera communication timeout;
    INS_RIGISTER_BLOCK_WRITE_TIMEOUT,       // Camera communication timeout;

    INS_GET_VALUE_ERROR,                    // Error in the obtained value;
    INS_IMAGE_SIZE_ERROR,                   // Image size error;

    INS_ACTIVE_MODE_CONFLICT,               // Callback registration failed, current mode is active capture mode, cannot set callback;
    INS_CALLBACK_MODE_CONFLICT,             // Callback registration failed, current mode is passive callback mode, cannot set callback;
    // 
    INS_WARN_VALUE_TOO_LARGE,               // The set exposure value exceeds the allowed maximum, automatically limited to maximum value;
    INS_FUNCTION_NOT_MATCH,                 // The called function does not match the light source mode;
    INS_NEED_SET_EXPOSURE_VALUE,            // After changing the light source mode, need to set exposure value;
    INS_NEED_SET_ENCODER_TRIGGER_MODE,      // After changing encoder counting mode, need to reset trigger direction mode;
    INS_NEED_SET_ENCODER_TRIGGER1_PERIOD,   // After changing light source mode, need to set trigger frequency for trigger source 1;
    INS_NEED_SET_ENCODER_TRIGGER2_PERIOD,   // After changing light source mode, need to set trigger frequency for trigger source 2;
    // 
    INS_ACQ2_FORBIDDEN,     // Current camera, no need to set acquisition mode trigger, use capture mode directly;
    INS_STITCH_CONFLICT,    // Conflict with splicing
    INS_NO_NVIDIA_GPU,    // No Nvidia GPU detected
    INS_NOT_ALLOWED_TO_SWITCH_MODEL,    // Switching between GPU mode and CPU mode is not allowed during image retrieval and runtime
    INS_EXCEEDING_HEIGHT_OF_ALGORITHM_STITCH,    // Exceeding the height of algorithm stitching
    INS_HIGH_SPEED_MODE_TURNED_OFF,    // The maximum speed of the current firmware's medium speed mode and high speed mode are the same, and the high speed mode is no longer open
};


struct InsCHVS_LoggerParam {
    char path[260]; // Set folder path, e.g. "C:/Users/UserName/Desktop" or "logs"

    int recordOperationLogSize = 20;        /* Default max file size: 20MB */
    int recordOperationLogMaxFiles = 2;     /* Default max files count: 2 */

    bool recordImageLog = true;              /* Default use it */
    int recordImageLogSize = 20;             /* Default max file size: 20MB */
    int recordImageLogMaxFiles = 2;          /* Default max files count: 2 */
};

#ifdef INS_CHVS_DLL_EXPORT
#define INS_CHVS_DLL_API  __declspec(dllexport)
#else
#define INS_CHVS_DLL_API __declspec(dllimport)
#endif

#define INS_CHVS_DLL_DEPRECATED __declspec(deprecated)
#define INS_CHVS_DLL_DEPRECATED_X(text) __declspec(deprecated(text))

class InsCHVSCamera_p;

