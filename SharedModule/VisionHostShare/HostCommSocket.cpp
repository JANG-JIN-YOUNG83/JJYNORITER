#include "stdafx.h"
#include "HostCommSocket.h"
#include "Alarm.h"
#include "ParameterSystem.h"
#include "ParameterVision.h"
#include "ResultVision.h"
#include "SpecDataVision.h"
#include "LoginInfo.h"
#include "TCPMessageDef.h"
#include "V2I_ReplyProgramBackup.h"
#include "V2I_VisionInformation.h"
#include "V2I_VisionRecipe.h"
#include "ResultIO.h"

using namespace Share;

#include "../../Module/Utility/FileReader.h"
#include "../CommonShare/ArchiveAllType.h"
#include "../CommonShare/DefectDef.h"
#include "../CommonShare/PacketMemFile.h"

#include "Base/LoggerInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HostCommSocket::HostCommSocket(const IPVM::MessageSocket::INIT_INFO initInfo)
	: IPVM::MessageSocket(initInfo)
	, m_loggerTcp(nullptr)
{
	m_bIsCisVision = false;
}

HostCommSocket::HostCommSocket(IPVM::LoggerInterface& loggerTcp, const IPVM::MessageSocket::INIT_INFO initInfo)
	: IPVM::MessageSocket(initInfo)
	, m_loggerTcp(&loggerTcp)
{

}

HostCommSocket::~HostCommSocket()
{
}

bool HostCommSocket::Connect(const wchar_t *szDestIP, unsigned short port, const long autoConnectInterval_sec, bool bIsCisVision)
{
	m_bIsCisVision = bIsCisVision;
	return IPVM::MessageSocket::Connect(szDestIP, port, autoConnectInterval_sec);
}

bool HostCommSocket::IsConnected() const
{
	return IPVM::MessageSocket::IsConnected();
}

bool HostCommSocket::SendAckMessage(long nMessage)
{
	bool res = false;

	for (int i = 0; i < 10; i++)
	{
		res = IPVM::MessageSocket::Write(nMessage, NULL, NULL);

		if (res)
		{
			break;
		}
	}

	return res;
}

bool HostCommSocket::SendAlarm(int nCode, ALARM_LEVEL nLevel, CString strItem, CString strContext, CString strDetail, CString strTroubleShooting, bool bWriteLog)
{
	Share::Alarm alarm;
	alarm.code = nCode;
	alarm.level = nLevel;
	alarm.item = strItem;
	alarm.context = strContext;
	alarm.detail = strDetail;
	alarm.troubleShooting = strTroubleShooting;
	::GetLocalTime(&alarm.outbreakTime);

	CString strLevel;
	if (nLevel == ALARM_LEVEL::yellow)
		strLevel = _T("YELLOW");
	else
		strLevel = _T("RED");

	bool bRes = SendAlarm(alarm);

	if (m_loggerTcp && !bRes)
	{
		if(bWriteLog)
			m_loggerTcp->Log(0, _T("Alarm Message Send Failed   code[%d],Level[%s], Item[%s], Context[%s], Detail[%s] "), nCode, strLevel, strItem, strContext, strDetail);
		bRes = false;
	}
	else
	{
		if (bWriteLog)
			m_loggerTcp->Log(0, _T("!!![[[Send Alarm]]] code - [%d] Level - [%s] Item - [%s] Context - [%s] Detail - [%s]!!!"), nCode, strLevel, strItem, strContext, strDetail);
		bRes = true;

		if (true == m_bIsCisVision)
		{
			{
				CSingleLock lock(&m_csDequeAlarm, TRUE);
				ALARM_INFO alarmInfo;
				alarmInfo.nAlarmCode = nCode;
				alarmInfo.strAlarmItem = strItem;
				m_dequeAlarm.push_back(alarmInfo);

				m_loggerTcp->Log(0, _T("@@@@ Send Alarm 함수 실행, 현재 Alarm Deque Size [%d] @@@@"), m_dequeAlarm.size());
			}

			::PostMessage(m_hwnd, WM_SET_ALARM_REPLY_TIMER, NULL, NULL);
		}
	}

	return bRes;
}

bool HostCommSocket::SendAlarm_NoRecipe()
{
    return SendAlarm(910 /*임시*/, Share::ALARM_LEVEL::red, _T("RECIPE"), _T("Not exist recipe file."), _T(""), _T(""));
}

void HostCommSocket::SendVisionInformation(CString strVisionVersion, long generationversion)
{
	CMemFile memFile(30000);
	CommonShare::ArchiveAllType ar(&memFile, CommonShare::ArchiveAllType::store);

	TCPMSG::V2I::VisionInformation information;
	if (GenerationVersion::SplitterRemoved == generationversion)
		information.m_version = _T("0.0.0.0");
    information.m_version = strVisionVersion;
	information.Serialize(ar);
	ar.Close();

	memFile.SeekToBegin();
	long length = (long)memFile.GetLength();
	byte* pByte = memFile.Detach();

	auto sendSuccess = IPVM::MessageSocket::Write((long)TCPMSG_V2I::VISION_INFORMATION, length, pByte);

	if (m_loggerTcp && sendSuccess)
	{
		m_loggerTcp->Log(0, _T("Send Vision Information"));
	}

	free(pByte);
}

void HostCommSocket::SendVisionStatus(VisionStatusType type, bool enable)
{
	CMemFile memFile(30000);
	CommonShare::ArchiveAllType ar(&memFile, CommonShare::ArchiveAllType::store);

	ar.Serialize_Element((long &)type);
	ar.Serialize_Element(enable);
	ar.Close();

	memFile.SeekToBegin();
	long length = (long)memFile.GetLength();
	byte* pByte = memFile.Detach();

	auto sendSuccess = IPVM::MessageSocket::Write((long)TCPMSG_V2I::VISION_STATUS, length, pByte);

	if (m_loggerTcp && sendSuccess)
	{
		switch (type)
		{
		case VisionStatusType::VisionRunEnable:
			m_loggerTcp->Log(0, _T("Send [Status] Vision Run Enable=%d"), enable);
			break;
		}
	}

	free(pByte);
}

void HostCommSocket::SendCrashDump(LPCTSTR crashDumpFolder)
{
	CFileFind ff;
	CString filter;
	filter.Format(_T("%s\\*.*"), crashDumpFolder);
	auto foundFile = ff.FindFile(filter);

	while (foundFile)
	{
		foundFile = ff.FindNextFile();
		if (ff.IsDots()) continue;

		if (ff.IsDirectory())
		{
			// 옛날에는 Dump를 폴더로 만들어서 그안에 넣었다. 오래된 덤프이다. 그냥 지우자.
			RemoveFolder(ff.GetFilePath());
			continue;
		}

		// Dump 파일이라 생각하고 전송하자

		CString fileName = ff.GetFileName();
		std::vector<BYTE> dumpFileData;
		if (!Utility::FileReader::Load(ff.GetFilePath(), dumpFileData)) continue;

		auto fileSize = ff.GetLength();

		if (fileSize > 1024 * 1024 * 100)
		{
			// 100MB 이상 전송은 하지 말자. 부담스럽다.
			continue;
		}

		CMemFile memFile(30000);
		CommonShare::ArchiveAllType arl(&memFile, CommonShare::ArchiveAllType::store);
		long dataSize = (long)dumpFileData.size();

		FILETIME creationTime, lastAccessTime, lastWriteTime;

		ff.GetCreationTime(&creationTime);
		ff.GetLastAccessTime(&lastAccessTime);
		ff.GetLastWriteTime(&lastWriteTime);

		arl.Serialize_Element(fileName);
		arl.Serialize_Element(dataSize);
		arl.Write(&creationTime, sizeof(creationTime));
		arl.Write(&lastAccessTime, sizeof(lastAccessTime));
		arl.Write(&lastWriteTime, sizeof(lastWriteTime));
		arl.Write(&dumpFileData[0], dataSize);
		arl.Close();

		memFile.SeekToBegin();
		long length = (long)memFile.GetLength();
		byte* pByte = memFile.Detach();

		auto sendSuccess = IPVM::MessageSocket::Write((long)TCPMSG_V2I::VISION_CRASH_DUMP, length, pByte);

		if (m_loggerTcp)
		{
			if (sendSuccess)
			{
				m_loggerTcp->Log(0, _T("Send Vision Crash File (%s)"), (LPCTSTR)fileName);
				DeleteFile(ff.GetFilePath());
			}
			else
			{
				m_loggerTcp->Log(0, _T("Failed to Send Vision Crash File (%s)"), (LPCTSTR)fileName);
			}
		}

		free(pByte);
	}
}

void HostCommSocket::SendVisionRecipeAll(LPCTSTR recipeFolder)
{
	// 나중에 Host에서 Recipe를 관리하게 되면 없어져야 할 함수
	CFileFind ff;
	CString filter;
	filter.Format(_T("%s\\*.job"), recipeFolder);
	auto foundFile = ff.FindFile(filter);

	while (foundFile)
	{
		foundFile = ff.FindNextFile();
		if (ff.IsDots()) continue;

		if (ff.IsDirectory()) continue;

		// Recipe 파일이라 생각하고 전송하자
		TCPMSG::V2I::VisionRecipe recipe;
		recipe.m_recipeName = ff.GetFileTitle();
		if (!recipe.m_db.Load(ff.GetFilePath())) continue;

		SendVisionRecipe(recipe);
	}
}

bool HostCommSocket::SendOriginalData(Share::SpecDataVision& data)
{
	CMemFile memFile(30000);
	CommonShare::ArchiveAllType arl(&memFile, CommonShare::ArchiveAllType::store);
	data.Serialize(arl);
	arl.Close();

	memFile.SeekToBegin();
	long length = (long)memFile.GetLength();
	byte* pByte = memFile.Detach();

	bool res = IPVM::MessageSocket::Write((long)TCPMSG_V2I::SPEC_ORIGINAL_DATA, length, pByte);

	free(pByte);

	return res;
}

bool HostCommSocket::SendVisionAlive()
{
	return IPVM::MessageSocket::Write((long)TCPMSG_V2I::VISION_ALIVE, NULL, NULL);
}

bool HostCommSocket::SendVisionRecipe(TCPMSG::V2I::VisionRecipe& data)
{
	CMemFile memFile(30000);
	CommonShare::ArchiveAllType ar(&memFile, CommonShare::ArchiveAllType::store);

	data.Serialize(ar);
	ar.Close();

	memFile.SeekToBegin();
	long length = (long)memFile.GetLength();
	byte* pByte = memFile.Detach();

	auto sendSuccess = IPVM::MessageSocket::Write((long)TCPMSG_V2I::VISION_RECIPE, length, pByte);

	if (m_loggerTcp)
	{
		if (sendSuccess)
		{
			m_loggerTcp->Log(0, _T("Send Vision Recipe"));
		}
		else
		{
			m_loggerTcp->Log(0, _T("Send Vision Recipe Failed"));
		}
	}

	free(pByte);

	return sendSuccess;
}

bool HostCommSocket::SendVisionResultData(Share::ResultVision& data, CommonShare::PacketMemFile &tempMemFile) //cis 에서 쓰는부분
{
	if (!IPVM::MessageSocket::IsConnected())
	{
		if (m_loggerTcp)
		{
			m_loggerTcp->Log(0, _T("Send Vision Result Data Failed (Disconnected)"));
		}

		return false;
	}
    m_loggerTcp->Log(0, _T("Data Serialize Start %d Cll (Disconnected)"), data.m_nCellindex);
	CommonShare::ArchiveAllType arl(&tempMemFile, CommonShare::ArchiveAllType::store);
	data.Serialize(arl);
	arl.Close();
    m_loggerTcp->Log(0, _T("Data Serialize END %d Cll (Disconnected)"), data.m_nCellindex);
	tempMemFile.SeekToBegin();
	tempMemFile.GetLength();

	ULONGLONG bufferSize = tempMemFile.GetBufferSize();
	ULONGLONG dataSize = tempMemFile.GetLength();

	byte* pByte = tempMemFile.Detach();

	bool res = false;

	for (int i = 0; i < 10; i++)
	{
		res = IPVM::MessageSocket::Write((long)TCPMSG_V2I::VISION_RESULT_DATA, static_cast<long>(dataSize), pByte);

		if (res) break;

		if (m_loggerTcp != nullptr) 
			m_loggerTcp->Log(0, _T("Retry Send Vision Result Data"));
	}

	//tempMemFile.Attach(pByte, (UINT)bufferSize, 30000);

	if (m_loggerTcp && !res)
	{
		m_loggerTcp->Log(0, _T("Send Vision Result Data Failed!"));
	}
    free(pByte);
	return res;
}

bool HostCommSocket::SendVisionResultData(Share::ResultVision& data) // area에서 쓰는부분
{
	CMemFile memFile(30000);
	CommonShare::ArchiveAllType arl(&memFile, CommonShare::ArchiveAllType::store);
	data.Serialize(arl);
	arl.Close();

	memFile.SeekToBegin();
	long length = (long)memFile.GetLength();
	byte* pByte = memFile.Detach();

	bool res = false;

	for (int i = 0; i < 10; i++)
	{
		res = IPVM::MessageSocket::Write((long)TCPMSG_V2I::VISION_RESULT_DATA, length, pByte);

		if (res) break;

		//		if (m_loggerTcp != nullptr) 
		//			m_loggerTcp->Log(0, _T("Retry Send Vision Result Data"));
	}

	//	if (m_loggerTcp && !res)
	//	{
	//		m_loggerTcp->Log(0, _T("Send Vision Result Data Failed!"));
	//	}


		// Todo: Help 송준호
		//for (int i = 0; i < data.m_vecImage.size(); ++i)
		//{
		//	//data.m_vecImage[i].srcImg.Free();
		//}

	free(pByte);

	return res;
}

bool HostCommSocket::SendVisionResetReply(VisionResetReply respone)
{
	auto res = IPVM::MessageSocket::Write((long)TCPMSG_V2I::VISION_RESET_REPLY, sizeof(long), &respone);

	if (m_loggerTcp)
	{
		if (!res)
		{
			m_loggerTcp->Log(0, _T("Send Vision Reset Reply Failed.. (%d)"), (long)respone);
		}
		else
		{
			m_loggerTcp->Log(0, _T("Send Vision Reset Reply (%d)"), (long)respone);
		}
	}

	return res;
}

bool HostCommSocket::SendReplyBarTypeCameraInit(BOOL success)
{
	auto res = IPVM::MessageSocket::Write((long)TCPMSG_V2I::REPLY_BAR_TYPE_CAMERA_INIT, sizeof(BOOL), &success);

	if (m_loggerTcp && !res)
	{
		m_loggerTcp->Log(0, _T("Camera Initialize Reply Failed.."));
	}

	return res;
}

bool HostCommSocket::SendReplyModelChange(int value)
{
	auto res = IPVM::MessageSocket::Write((long)TCPMSG_V2I::REPLY_MODEL_CHANGE, sizeof(int), &value);

	if (m_loggerTcp && !res)
	{
		m_loggerTcp->Log(0, _T("Send Reply Model Change Failed!"));
	}

	return res;
}

bool HostCommSocket::SendReplyOperatorInfo(int value)
{
	auto res = IPVM::MessageSocket::Write((long)TCPMSG_V2I::REPLY_OPERATOR_INFO, sizeof(int), &value);

	if (m_loggerTcp && !res)
	{
		m_loggerTcp->Log(0, _T("Send Reply Operator Info Failed!"));
	}

	return res;
}

bool HostCommSocket::SendReplySpecChange(int value)
{
	auto res = IPVM::MessageSocket::Write((long)TCPMSG_V2I::REPLY_SPEC_CHANGE, sizeof(int), &value);

	if (m_loggerTcp && !res)
	{
		m_loggerTcp->Log(0, _T("Send Reply Spec Change Failed!"));
	}

	return res;
}

bool HostCommSocket::SendReplyAlarmReset(BOOL success)
{
	auto res = IPVM::MessageSocket::Write((long)TCPMSG_V2I::REPLY_ALARM_RESET, sizeof(BOOL), &success);

	if (m_loggerTcp && !res)
	{
		m_loggerTcp->Log(0, _T("Send Reply Alarm Reset Failed.."));
	}

	return res;
}

bool HostCommSocket::SendShowPGM()
{
	return IPVM::MessageSocket::Write((long)TCPMSG_V2I::SHOW_PGM, 0, NULL);
}

bool HostCommSocket::SendSpecCurrentData(Share::SpecDataVision& data)
{
	CMemFile memFile(30000);
	CommonShare::ArchiveAllType arl(&memFile, CommonShare::ArchiveAllType::store);
	data.Serialize(arl);
	arl.Close();

	memFile.SeekToBegin();
	long length = (long)memFile.GetLength();
	byte* pByte = memFile.Detach();

	bool res = IPVM::MessageSocket::Write((long)TCPMSG_V2I::SPEC_CURRENT_DATA, length, pByte);
	free(pByte);

	if (m_loggerTcp)
	{
		if (res)
		{
			m_loggerTcp->Log(0, _T("Send Current Spec"));
		}
		else
		{
			m_loggerTcp->Log(0, _T("Send Current Spec Failed"));
		}

		for (int i = 0; i < data.m_vecSpecDataList.size(); i++)
		{
			CString strTmp, strSendSpec;

			strSendSpec.Format(_T("Send Spec   "));
			strTmp.Format(_T("Name - %s , "), GetDefectName(data.m_vecSpecDataList[i].m_inspType));
			strSendSpec.Append(strTmp);
			strTmp.Format(_T("Offset %.3f , "), data.m_vecSpecDataList[i].m_dOffset);
			strSendSpec.Append(strTmp);
			strTmp.Format(_T("Standard %.3f , "), data.m_vecSpecDataList[i].m_dStandard_Value);
			strSendSpec.Append(strTmp);
			strTmp.Format(_T("Spec Min %.3f , "), data.m_vecSpecDataList[i].m_dMinusTolerance);
			strSendSpec.Append(strTmp);
			strTmp.Format(_T("Spec Max %.3f , "), data.m_vecSpecDataList[i].m_dPlusTolerance);
			strSendSpec.Append(strTmp);
			strTmp.Format(_T("AI Spec Min %.3f , "), data.m_vecSpecDataList[i].m_dAiMinusTolerance);
			strSendSpec.Append(strTmp);
			strTmp.Format(_T("AI Spec Max %.3f , "), data.m_vecSpecDataList[i].m_dAiPlusTolerance);
			strSendSpec.Append(strTmp);
			//strTmp.Format(_T("Marking Spec Min %.3f , "), data.m_vecSpecDataList[i].m_dMarkingMinusTolerance);
			//strSendSpec.Append(strTmp);
			//strTmp.Format(_T("Marking Spec Max %.3f"), data.m_vecSpecDataList[i].m_dMarkingPlusTolerance);
			//strSendSpec.Append(strTmp);
			m_loggerTcp->Log(0, strSendSpec);

			//m_loggerTcp->Log(0, _T("Send Spec Offset %f"), data.m_vecSpecDataList[i].m_dOffset);
			////m_loggerTcp->Log(0, _T("Send Spec Min %f"), data.m_vecSpecDataList[i].m_dMin_Value);
			////m_loggerTcp->Log(0, _T("Send Spec Max %f"), data.m_vecSpecDataList[i].m_dMax_Value);
			////m_loggerTcp->Log(0, _T("Send Spec AI Min %f"), data.m_vecSpecDataList[i].m_dAiMin_Value);
			////m_loggerTcp->Log(0, _T("Send Spec AI Max %f"), data.m_vecSpecDataList[i].m_dAiMax_Value);
			////m_loggerTcp->Log(0, _T("Send Spec Marking Min %f"), data.m_vecSpecDataList[i].m_dMarkingMin_Value);
			////m_loggerTcp->Log(0, _T("Send Spec Marking Max %f"), data.m_vecSpecDataList[i].m_dMarkingMax_Value);
			//m_loggerTcp->Log(0, _T("Send Spec Standard %f"), data.m_vecSpecDataList[i].m_dStandard_Value);
			//m_loggerTcp->Log(0, _T("Send Spec Minus Tolerance %f"), data.m_vecSpecDataList[i].m_dMinusTolerance);
			//m_loggerTcp->Log(0, _T("Send Spec Plus Tolerance %f"), data.m_vecSpecDataList[i].m_dPlusTolerance);
			//m_loggerTcp->Log(0, _T("Send Spec AI Minus Tolerance %f"), data.m_vecSpecDataList[i].m_dAiMinusTolerance);
			//m_loggerTcp->Log(0, _T("Send Spec AI Plus Tolerance %f"), data.m_vecSpecDataList[i].m_dAiPlusTolerance);
			//m_loggerTcp->Log(0, _T("Send Spec Marking Minus Tolerance %f"), data.m_vecSpecDataList[i].m_dMarkingMinusTolerance);
			//m_loggerTcp->Log(0, _T("Send Spec Marking Plus Tolerance %f"), data.m_vecSpecDataList[i].m_dMarkingPlusTolerance);
		}
	}

	return res;
}

bool HostCommSocket::SendVisionGrabReady()
{
	return IPVM::MessageSocket::Write((long)TCPMSG_V2I::VISION_GRAB_READY, 0, NULL);
}

bool HostCommSocket::SendVisionGrabStopped()
{
	return IPVM::MessageSocket::Write((long)TCPMSG_V2I::VISION_GRAB_STOPPED, 0, NULL);
}
bool HostCommSocket::SendVisionSurfaceSimulationStopped()
{
	return IPVM::MessageSocket::Write((long)TCPMSG_V2I::VISION_SURFACE_TEST, 0, NULL);
}

bool HostCommSocket::SendChangedParameterVision(Share::ParameterVision& data)
{
	CMemFile memFile(30000);
	CommonShare::ArchiveAllType arl(&memFile, CommonShare::ArchiveAllType::store);
	data.Serialize(arl);
	arl.Close();

	memFile.SeekToBegin();
	long length = (long)memFile.GetLength();
	byte* pByte = memFile.Detach();

	bool res = IPVM::MessageSocket::Write((long)TCPMSG_V2I::PARAMETER_VISION, length, pByte);

	free(pByte);

	return res;
}

bool HostCommSocket::SendChangedParameterSystem(Share::ParameterSystem& data)
{
	CMemFile memFile(30000);
	CommonShare::ArchiveAllType arl(&memFile, CommonShare::ArchiveAllType::store);
	data.Serialize(arl);
	arl.Close();

	memFile.SeekToBegin();
	long length = (long)memFile.GetLength();
	byte* pByte = memFile.Detach();

	bool res = IPVM::MessageSocket::Write((long)TCPMSG_V2I::PARAMETER_SYSTEM, length, pByte);

	free(pByte);

	return res;
}

bool HostCommSocket::SendAlarm(Share::Alarm& alarm)
{
	CMemFile memFile(30000);
	CommonShare::ArchiveAllType ar((CFile*)&memFile, CommonShare::ArchiveAllType::store);
	alarm.Serialize(ar);
	ar.Close();
	
	memFile.SeekToBegin();
	long length = (long)memFile.GetLength();
	byte* pByte = memFile.Detach();

	bool res = false;

	for (int i = 0; i < 10; i++)
	{
		res = IPVM::MessageSocket::Write((long)TCPMSG_V2I::ALARM, length, pByte);

		if (res)
		{
			break;
		}
	}

	free(pByte);

	return res;
}
bool HostCommSocket::SendHostLoginAccess()
{
	return IPVM::MessageSocket::Write((long)TCPMSG_V2I::LOGIN_ACCESS_REQUEST, 0, NULL);
}

bool HostCommSocket::SendHostLoginTry(Share::LoginInfo& data)
{
	CMemFile memFile(30000);
	CommonShare::ArchiveAllType arl(&memFile, CommonShare::ArchiveAllType::store);
	data.Serialize(arl);
	arl.Close();

	memFile.SeekToBegin();
	long length = (long)memFile.GetLength();
	byte* pByte = memFile.Detach();

	bool res = IPVM::MessageSocket::Write((long)TCPMSG_V2I::LOGIN_TRY, length, pByte);

	free(pByte);

	return res;
}

bool HostCommSocket::SendHostLoginInit()
{
	return IPVM::MessageSocket::Write((long)TCPMSG_V2I::LOGIN_INIT_REQUEST, 0, NULL);
}

bool HostCommSocket::SendReplyProgramBackup(LPCTSTR titleName, bool success)
{
	TCPMSG::V2I::ReplyProgramBackup data;
	CMemFile memFile(30000);
	CommonShare::ArchiveAllType arl(&memFile, CommonShare::ArchiveAllType::store);
	data.m_titleName = titleName;
	data.m_success = success;
	data.Serialize(arl);
	arl.Close();

	memFile.SeekToBegin();
	long length = (long)memFile.GetLength();
	byte* pByte = memFile.Detach();

	bool res = IPVM::MessageSocket::Write((long)TCPMSG_V2I::REPLY_PROGRAM_BACKUP, length, pByte);
	free(pByte);

	if (m_loggerTcp && res)
	{
		m_loggerTcp->Log(0, _T("Reply Program Backup %s (%s)"), titleName, (success ? _T("TRUE") : _T("FALSE")));
	}

	return res;
}

bool HostCommSocket::RemoveFolder(LPCTSTR folderPath)
{
	CFileFind ff;
	CString filter;
	filter.Format(_T("%s\\*.*"), folderPath);
	auto foundFile = ff.FindFile(filter);

	while (foundFile)
	{
		foundFile = ff.FindNextFile();
		if (ff.IsDots()) continue;

		if (ff.IsDirectory())
		{
			RemoveFolder(ff.GetFilePath());
		}
		else
		{
			DeleteFile(ff.GetFilePath());
		}
	}

	ff.Close();

	return ::RemoveDirectory(folderPath) ? true : false;
}


void HostCommSocket::SendIni(LPCTSTR Path, LPCTSTR extension)
{
	CFileFind ff;
	CString filter;
	filter.Format(_T("%s\\%s"), Path, extension);
	auto foundFile = ff.FindFile(filter);

	while (foundFile)
	{
		foundFile = ff.FindNextFile();
		if (ff.IsDots()) continue;

		// Dump 파일이라 생각하고 전송하자

		CString fileName = ff.GetFileName();
		std::vector<BYTE> FileData;
		if (!Utility::FileReader::Load(ff.GetFilePath(), FileData)) continue;

		auto fileSize = ff.GetLength();

		if (fileSize > 1024 * 1024 * 10)
		{
			// 10MB 이상 전송은 하지 말자. 부담스럽다.
			continue;
		}

		CMemFile memFile(30000);
		CommonShare::ArchiveAllType arl(&memFile, CommonShare::ArchiveAllType::store);
		long dataSize = (long)FileData.size();

		FILETIME creationTime, lastAccessTime, lastWriteTime;

		ff.GetCreationTime(&creationTime);
		ff.GetLastAccessTime(&lastAccessTime);
		ff.GetLastWriteTime(&lastWriteTime);

		arl.Serialize_Element(fileName);
		arl.Serialize_Element(dataSize);
		arl.Write(&creationTime, sizeof(creationTime));
		arl.Write(&lastAccessTime, sizeof(lastAccessTime));
		arl.Write(&lastWriteTime, sizeof(lastWriteTime));
		arl.Write(&FileData[0], dataSize);
		arl.Close();

		memFile.SeekToBegin();
		long length = (long)memFile.GetLength();
		byte* pByte = memFile.Detach();

		auto sendSuccess = IPVM::MessageSocket::Write((long)TCPMSG_V2I::VISION_INI_FILE, length, pByte);

		if (m_loggerTcp)
		{
			if (sendSuccess)
			{
				m_loggerTcp->Log(0, _T("Send Vision Ini File (%s)"), (LPCTSTR)fileName);
			}
			else
			{
				m_loggerTcp->Log(0, _T("Failed to Send Vision Ini File (%s)"), (LPCTSTR)fileName);
			}
		}

		free(pByte);
	}
}

void HostCommSocket::SendVisionNormalTurnOff(long V2I_MSG)
{
	IPVM::MessageSocket::Write(V2I_MSG, NULL, NULL);
}

void HostCommSocket::ProcessReplyVisionAlarm()
{
	if (0 < m_dequeAlarm.size())
	{
		CSingleLock lock(&m_csDequeAlarm, TRUE);
		m_dequeAlarm.pop_front();

		m_loggerTcp->Log(0, _T("@@@@Reply Vision Alarm Message 처리 함수 실행, 현재 Alarm Deque Size [%d]@@@@"), m_dequeAlarm.size());
	}
}

void HostCommSocket::SendIO(int nIO, AREA_GRABMODE Area_Grabmode, enCAMERA CameraPos)
{
	Share::ResultIO resultio;
	resultio.SetData(nIO, Area_Grabmode, CameraPos);
	CMemFile memFile(30000);
	CommonShare::ArchiveAllType arl(&memFile, CommonShare::ArchiveAllType::store);
	resultio.Serialize(arl);
	arl.Close();

	memFile.SeekToBegin();
	long length = (long)memFile.GetLength();
	byte* pByte = memFile.Detach();

	bool res = IPVM::MessageSocket::Write((long)TCPMSG_V2I::VISION_IO, length, pByte);
	free(pByte);
}