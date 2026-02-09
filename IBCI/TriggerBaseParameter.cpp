#include "pch.h"
#include "TriggerBaseParameter.h"


TriggerBaseParameter::TriggerBaseParameter()
    : m_nPortNo(0)
    , m_nBaundRate(0)
    , m_idleRollerDiameter(0)
    , m_rotaryEncoderPulse(0)
{
}

TriggerBaseParameter::~TriggerBaseParameter()
{
}

void TriggerBaseParameter::Load(LPCTSTR filePath)
{
    CString strKeyName = _T("Parameter");
    
    TCHAR szBuffer[256] = { NULL, };

    GetPrivateProfileString(strKeyName, _T("PORT"), _T(""), szBuffer, 256, filePath);
    m_nPortNo = _ttoi(szBuffer);
    GetPrivateProfileString(strKeyName, _T("BaudRate"), _T(""), szBuffer, 256, filePath);
    m_nBaundRate = _ttoi(szBuffer);
    /*m_portNo = IniHelper::LoadINT(filePath, strKeyName, _T("COM_Port"), 0);
    m_baundRate = IniHelper::LoadINT(filePath, strKeyName, _T("BaudRate"), 0);
    m_idleRollerDiameter = IniHelper::LoadINT(filePath, strKeyName, _T("IdleRollerDiameter"), 0);
    m_rotaryEncoderPulse = IniHelper::LoadINT(filePath, strKeyName, _T("RotaryENCPulse"), 0);*/
}

void TriggerBaseParameter::Save(LPCTSTR filePath)
{
    CString strKeyName = _T("Parameter");
    CString strValue;

    strValue.Format(_T("%d"), m_nPortNo);
    WritePrivateProfileString(strKeyName, _T("PORT"), strValue, filePath);

    strValue.Format(_T("%d"), m_nBaundRate);
    WritePrivateProfileString(strKeyName, _T("BaudRate"), strValue, filePath);
    /*
    IniHelper::SaveINT(filePath, strKeyName, _T("COM_Port"), m_portNo);
    IniHelper::SaveINT(filePath, strKeyName, _T("BaudRate"), m_baundRate);
    IniHelper::SaveINT(filePath, strKeyName, _T("IdleRollerDiameter"), m_idleRollerDiameter);
    IniHelper::SaveINT(filePath, strKeyName, _T("RotaryENCPulse"), m_rotaryEncoderPulse);*/
}