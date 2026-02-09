//===========================================================================================
// ALT_LSTPE Series DLL Library
// 2019-09-10
//		Made by woojinauto in ALTSystem 
//===========================================================================================
// History
// 001
// - Receive TimeOut 함수 위치 변경

// 002
// - Manual/IO Lock/Unlock 프로토콜 관련 함수를 추가한다.

#ifndef		__ALT_LSTPE__
#define		__ALT_LSTPE__

//=================================================================================================================================
// Define

#define		CHANNEL_MODE1			(char)0
#define		CHANNEL_MODE2			(char)1
#define		CHANNEL_MODE4			(char)2
#define		CHANNEL_MODE8			(char)3
#define		CHANNEL_MODE16			(char)4
#define		CHANNEL_MODE32			(char)5

#define		MODE1_MAX_CHANNEL		(char)1
#define		MODE2_MAX_CHANNEL		(char)2
#define		MODE4_MAX_CHANNEL		(char)4
#define		MODE8_MAX_CHANNEL		(char)8
#define		MODE16_MAX_CHANNEL		(char)16
#define		MODE32_MAX_CHANNEL		(char)32

#define		MAX_CHANNEL_4			(char)4
#define		MAX_CHANNEL_8			(char)8
#define		MAX_CHANNEL_16			(char)16
#define		MAX_CHANNEL_32			(char)32

#define		MAX_CHANNEL_NUM			MAX_CHANNEL_32

#define		MAX_RUN_CODE			(char)10
#define		MAX_MACRO_DATA_NUM		(int)255

#define		ENC_ND_1_1				(char)0
#define		ENC_ND_1_2				(char)1
#define		ENC_ND_1_4				(char)2
#define		ENC_D_1_1				(char)3
#define		ENC_D_1_2				(char)4
#define		ENC_D_1_4				(char)5

#define		LSTPE_PARAMETER_VALUE_ERROR						(DWORD)0x00000100
#define		LSTPE_MEM_ALLOCATION_ERROR						(DWORD)0x00000200
#define		LSTPE_SYNC_OBJECT_CREATE_ERROR					(DWORD)0x00000300
#define		LSTPE_RECEIVE_THREAD_CREATE_ERROR				(DWORD)0x00000400
#define		LSTPE_RECEIVE_PROTOCOL_TIMEOUT					(DWORD)0x00000500
#define		LSTPE_RECEIVE_NOT_EXIST_COMMAND_ERROR			(DWORD)0x00000600
#define		LSTPE_PARAMETER_EXCEPTION_ERROR					(DWORD)0x00000700

#define		MAX_ENC_OFFSET_LIST_DATA		(unsigned short)256

#define EXPORT extern "C" __declspec(dllexport)

//=================================================================================================================================
// Init Function

EXPORT int ALT_LSTPE_Create ( void ) ;
EXPORT void ALT_LSTPE_Close ( int Index ) ;

EXPORT BOOL ALT_LSTPE_Lan_Start ( int Index, BOOL TCPIP, char *lpServer, int port, char SegmentNum ) ;
EXPORT BOOL ALT_LSTPE_Lan_WStart ( int Index, BOOL TCPIP, wchar_t *lpServer, int port, char SegmentNum ) ;
EXPORT BOOL ALT_LSTPE_Uart_Check ( int Index, char *PortName ) ;
EXPORT BOOL ALT_LSTPE_Uart_WCheck ( int Index, wchar_t *PortName ) ;
EXPORT BOOL ALT_LSTPE_Uart_Start ( int Index, char *PortName, int Borate, char SegmentNum ) ;
EXPORT BOOL ALT_LSTPE_Uart_WStart ( int Index, wchar_t *PortName, int Borate, char SegmentNum ) ;
EXPORT BOOL ALT_LSTPE_Setup ( int Index, char SegmentNum ) ;
EXPORT unsigned long ALT_LSTPE_GetLastErrorCode ( int Index ) ;

//=================================================================================================================================
// Status Function

EXPORT BOOL ALT_LSTPE_IsTcpConnect ( int Index ) ;
EXPORT BOOL ALT_LSTPE_IsUdpConnect ( int Index ) ;
EXPORT BOOL ALT_LSTPE_IsUartConnect ( int Index ) ;

//=================================================================================================================================
// Monitor Protocol Function

EXPORT BOOL ALT_LSTPE_Protocol_Load ( int Index, unsigned long *Time, char *DataMem, int *DataSize, BOOL *SendMode ) ;
EXPORT BOOL ALT_LSTPE_Protocol_LoadW ( int Index, unsigned long *Time, wchar_t *Data, int *BDataLen, BOOL *SendMode ) ;

//=================================================================================================================================
// Common Protocol Function

EXPORT BOOL ALT_LSTPE_SendIPAddress ( int Index, char *IP, char *NetMask, char *GateWay, int PortNum ) ;
EXPORT BOOL ALT_LSTPE_ReadIPAddress ( int Index, char *IP, char *NetMask, char *GateWay, int *PortNum ) ;
EXPORT BOOL ALT_LSTPE_SendMACAddress ( int Index, char *MAC ) ;
EXPORT BOOL ALT_LSTPE_ReadMACAddress ( int Index, char *MAC ) ;
EXPORT BOOL ALT_LSTPE_SendMacroData ( int Index, unsigned char MacroIndex, unsigned char MacroSize, unsigned long *MacroData ) ;
EXPORT BOOL ALT_LSTPE_ReadMacroData ( int Index, unsigned char MacroIndex, unsigned char *MacroSize, unsigned long *MacroData ) ;

//=================================================================================================================================
// Device Open Protocol Function

EXPORT BOOL ALT_LSTPE_ChannelModeSend ( int Index, char ChannelMode ) ;
EXPORT BOOL ALT_LSTPE_ChannelModeRead ( int Index, char *ChannelMode ) ;
EXPORT BOOL ALT_LSTPE_SegmentValueSend ( int Index, char PageIndex, char SegIndex, unsigned short SegValue ) ;
EXPORT BOOL ALT_LSTPE_AllSegmentValueSend ( int Index, char PageIndex, unsigned short *SegValue ) ;
EXPORT BOOL ALT_LSTPE_AllSegmentValueRead ( int Index, char PageIndex, unsigned short *SegValue ) ;
EXPORT BOOL ALT_LSTPE_ChannelValueSend ( int Index, char PageIndex, char ChannelIndex, unsigned short ChValue ) ;
EXPORT BOOL ALT_LSTPE_AllChannelValueSend ( int Index, char PageIndex, unsigned short *ChValue ) ;
EXPORT BOOL ALT_LSTPE_AllChannelValueRead ( int Index, char PageIndex, unsigned short *ChValue ) ;
EXPORT BOOL ALT_LSTPE_AdjustValueSend ( int Index, char AdjustIndex, char AdValue ) ;
EXPORT BOOL ALT_LSTPE_AllAdjustValueSend ( int Index, char *AdValue ) ;
EXPORT BOOL ALT_LSTPE_AllAdjustValueRead ( int Index, char *AdValue ) ;
EXPORT BOOL ALT_LSTPE_StrobeValueSend ( int Index, char PageIndex, char SegmentIndex, unsigned short StrobeValue ) ;
EXPORT BOOL ALT_LSTPE_AllStrobeValueSend ( int Index, char PageIndex, unsigned short *StrobeValue ) ;
EXPORT BOOL ALT_LSTPE_AllStrobeValueRead ( int Index, char PageIndex, unsigned short *StrobeValue ) ;
EXPORT BOOL ALT_LSTPE_SaveSend ( int Index ) ;
EXPORT BOOL ALT_LSTPE_LoadSend ( int Index ) ;
EXPORT BOOL ALT_LSTPE_RunCodeSend ( int Index, char RunCode ) ;
EXPORT BOOL ALT_LSTPE_PageIndexSend ( int Index, char PageIndex ) ;
EXPORT BOOL ALT_LSTPE_PageIndexRead ( int Index, char *PageIndex ) ;
EXPORT BOOL ALT_LSTPE_ConfigModeRead ( int Index, char *ChannelMode, char *SegmentNum, BOOL *TriggerMode, char *EncoderMode ) ;
EXPORT BOOL ALT_LSTPE_TriggerModeSend ( int Index, BOOL TriggerMode ) ;
EXPORT BOOL ALT_LSTPE_EncoderModeSend ( int Index, char EncoderMode ) ;
EXPORT BOOL ALT_LSTPE_EncClearSend ( int Index ) ;
EXPORT BOOL ALT_LSTPE_EncCountRead ( int Index, unsigned long *EncCount ) ;
EXPORT BOOL ALT_LSTPE_SoftTriggerSend ( int Index, unsigned long TriggerHigh, unsigned long TriggerLow ) ;
EXPORT BOOL ALT_LSTPE_EncOffsetListSend ( int Index, unsigned short EncCount, unsigned short *EncOffsetList ) ;
EXPORT BOOL ALT_LSTPE_EncOffsetListRead ( int Index, unsigned short *EncCount, unsigned short *EncOffsetList ) ;
EXPORT BOOL ALT_LSTPE_ManualLockSend ( int Index, unsigned char ManualLock ) ;
EXPORT BOOL ALT_LSTPE_ManualLockRead ( int Index, unsigned char *ManualLock ) ;

//=================================================================================================================================
// Device Hide Protocol Function

EXPORT BOOL ALT_LSTPE_StatusRead ( int Index, unsigned long *Status, unsigned long *Command, unsigned long *Mode ) ;
EXPORT BOOL ALT_LSTPE_MacroStatusRead ( int Index, unsigned char *MacroIndex, unsigned long *MacroCode, unsigned char *MacroLine, unsigned char *MacroStep ) ;

#endif