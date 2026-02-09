#pragma once

#include "basedef.h"

#include "Gadget/MessageSocket.h"
#include <deque>

namespace Share
{
#define WM_SET_ALARM_REPLY_TIMER WM_USER +7000

	typedef struct
	{
		int nAlarmCode;
		CString strAlarmItem;
	}ALARM_INFO;

	class __VISION_HOST_SHARE_CLASS__ HostCommSocket : IPVM::MessageSocket
	{
	public:
		HostCommSocket(const IPVM::MessageSocket::INIT_INFO initInfo);
		HostCommSocket(IPVM::LoggerInterface& loggerTcp, const IPVM::MessageSocket::INIT_INFO initInfo);
		~HostCommSocket();

		bool Connect(const wchar_t *szDestIP, unsigned short port, const long autoConnectInterval_sec = 0, bool bIsCisVision = false);
		bool IsConnected() const;

		bool SendAlarm(int nCode, ALARM_LEVEL nLevel, CString strItem, CString strContext, CString strDetail, CString strTroubleShooting, bool bWriteLog = true);
		bool SendAlarm_NoRecipe();

		bool SendAckMessage(long nMessage);

		void SendVisionInformation(CString strVisionVersion, long generationversion = GenerationVersion::Original);
		void SendVisionStatus(VisionStatusType type, bool enable);
		void SendCrashDump(LPCTSTR crashDumpFolder);
		void SendVisionRecipeAll(LPCTSTR recipeFolder);
		void SendIni(LPCTSTR Path, LPCTSTR extension);

		bool SendOriginalData(Share::SpecDataVision& data);
		bool SendVisionAlive();
		bool SendVisionRecipe(TCPMSG::V2I::VisionRecipe& data);
		bool SendVisionResultData(Share::ResultVision& data, CommonShare::PacketMemFile& tempMemFile);
		bool SendVisionResultData(Share::ResultVision& data);
		bool SendVisionResetReply(VisionResetReply respone);
		bool SendReplyBarTypeCameraInit(BOOL success);
		bool SendReplyModelChange(int value);
		bool SendReplyOperatorInfo(int value);
		bool SendReplySpecChange(int value);
		bool SendReplyAlarmReset(BOOL success);
		bool SendShowPGM();
		bool SendSpecCurrentData(Share::SpecDataVision& data);
		bool SendVisionGrabReady();
		bool SendVisionGrabStopped();
		bool SendChangedParameterVision(Share::ParameterVision& data);
		bool SendChangedParameterSystem(Share::ParameterSystem& data);
		bool SendVisionSurfaceSimulationStopped();
		bool SendHostLoginAccess();
		bool SendHostLoginTry(Share::LoginInfo& data);
		bool SendHostLoginInit();
		bool SendReplyProgramBackup(LPCTSTR titleName, bool success);
		void SendVisionNormalTurnOff(long V2I_MSG);
		void ProcessReplyVisionAlarm();
		void SendIO(int nIO, AREA_GRABMODE Area_Grabmode, enCAMERA CameraPos);

		void SetHwnd(HWND hwnd)
		{
			m_hwnd = hwnd;
		}

		int GetAlarmDequeSize()
		{
			CSingleLock lock(&m_csDequeAlarm, TRUE);
			return m_dequeAlarm.size();
		}

		void ClearAlarmDeque()
		{
			CSingleLock lock(&m_csDequeAlarm, TRUE);
			m_dequeAlarm.clear();
		}

	private:
		IPVM::LoggerInterface* m_loggerTcp;

		bool RemoveFolder(LPCTSTR folderPath);
		bool SendAlarm(Share::Alarm& alarm);

		HWND m_hwnd;
		CCriticalSection m_csDequeAlarm;
		std::deque <ALARM_INFO> m_dequeAlarm;

		bool m_bIsCisVision;
	};
}
