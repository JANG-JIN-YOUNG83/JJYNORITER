#include "pch.h"
#include "IBNI_PCSetting.h"
#include "../../CompanySharedLibrary/Persistence/IniHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Utility;

class InnerPCSetting
{
public:
	InnerPCSetting()
		: m_simulationMode(FALSE)
		, m_doNotSaveVisionImage(FALSE)
		, m_noHardware(FALSE)
		, m_noSpcTrans(FALSE)
		, m_useRelativePathForCIS(FALSE)
		, m_useLocalneworkForVision(FALSE)
	{
		wchar_t szPath[MAX_PATH] = { 0, };
		//SHGetSpecialFolderPath(NULL, szPath, CSIDL_MYDOCUMENTS, FALSE);
		::GetModuleFileName(NULL, szPath, 300);

		CString path = szPath;
		// 예시) D:\\Intekplus\\CIS1\release
		if (0 < path.ReverseFind('\\')) path = path.Left(path.ReverseFind('\\'));	// release
		if (0 < path.ReverseFind('\\')) path = path.Left(path.ReverseFind('\\'));	// CIS1
		if (0 < path.ReverseFind('\\')) path = path.Left(path.ReverseFind('\\'));	// intekplus


		CString directory;
		directory.Format(_T("%s\\IBCI"), path);
		CreateDirectory(directory, nullptr);

		m_setupFilePath.Format(_T("%s\\Setup.ini"), (LPCTSTR)directory);
		Link(false);
	}

	void Save()
	{
		Link(true);
	}

	BOOL m_simulationMode;
	BOOL m_doNotSaveVisionImage;
	BOOL m_noHardware;
	BOOL m_noSpcTrans;
	BOOL m_useRelativePathForCIS;
	BOOL m_useLocalneworkForVision;
	CString m_setupFilePath;

private:
	void Link(bool save)
	{
		IniHelper::Link(save, m_setupFilePath, _T("Mode"), _T("Simulation"), &m_simulationMode);
		IniHelper::Link(save, m_setupFilePath, _T("Option"), _T("Do Not Save Vision Image"), &m_doNotSaveVisionImage);
		IniHelper::Link(save, m_setupFilePath, _T("Option"), _T("No Hardware"), &m_noHardware);
		IniHelper::Link(save, m_setupFilePath, _T("Option"), _T("No SpcTrans"), &m_noSpcTrans);
		IniHelper::Link(save, m_setupFilePath, _T("Option"), _T("Use Relative Path For CIS"), &m_useRelativePathForCIS);
		IniHelper::Link(save, m_setupFilePath, _T("Option"), _T("Use Local Network For Vision"), &m_useLocalneworkForVision);
	}
};

InnerPCSetting g_pcSetting;

bool IBNI_PCSetting::IsEnableSimulationMode()
{
	return g_pcSetting.m_simulationMode;
}

bool IBNI_PCSetting::IsEnabledSubOption_DoNotSaveVisionImage()
{
	if (g_pcSetting.m_simulationMode) return true;
	return g_pcSetting.m_doNotSaveVisionImage;
}

bool IBNI_PCSetting::IsEnabledSubOption_NoHardware()
{
	if (g_pcSetting.m_simulationMode) return true;
	return g_pcSetting.m_noHardware;
}

bool IBNI_PCSetting::IsEnabledSubOption_NoSpcTrans()
{
	//성재민 아래 주석을 풀어서 사용 ( 20220502 )
	//return false;
	if (g_pcSetting.m_simulationMode) return true;
	return g_pcSetting.m_noSpcTrans;
}

bool IBNI_PCSetting::IsEnabledSubOption_UseRelativePathForCIS()
{
	if (g_pcSetting.m_simulationMode) return true;
	return g_pcSetting.m_useRelativePathForCIS;
}

bool IBNI_PCSetting::IsEnabledSubOption_UseLocalNetworkForVision()
{
	if (g_pcSetting.m_simulationMode) return true;
	return g_pcSetting.m_useLocalneworkForVision;
}

void IBNI_PCSetting::SetEnableSimulationMode(bool enable)
{
	g_pcSetting.m_simulationMode = enable;
	g_pcSetting.Save();
}

void IBNI_PCSetting::SetEnableSubOption_DoNotSaveVisionImage(bool enable)
{
	g_pcSetting.m_doNotSaveVisionImage = enable;
	g_pcSetting.Save();
}

void IBNI_PCSetting::SetEnableSubOption_NoHardware(bool enable)
{
	g_pcSetting.m_noHardware = enable;
	g_pcSetting.Save();
}

void IBNI_PCSetting::SetEnableSubOption_NoSpcTrans(bool enable)
{
	g_pcSetting.m_noSpcTrans = enable;
	g_pcSetting.Save();
}

void IBNI_PCSetting::SetEnableSubOption_UseRelativePathForCIS(bool enable)
{
	g_pcSetting.m_useRelativePathForCIS = enable;
	g_pcSetting.Save();
}

void IBNI_PCSetting::SetEnableSubOption_UseLocalNetworkForVision(bool enable)
{
	g_pcSetting.m_useLocalneworkForVision = enable;
	g_pcSetting.Save();
}