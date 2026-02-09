#include "stdafx.h"
#include "LoginInfo.h"
#include "../CommonShare/ArchiveAllType.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LoginInfo::LoginInfo()
{
	m_strID = _T("");
	m_strPW = _T("");
	m_nMode = enLoginMode::Guest;
	m_bLoginStatus = FALSE;
}
LoginInfo:: ~LoginInfo()
{

}void LoginInfo::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(m_strID);
	ar.Serialize_Element(m_strPW);
	ar.Serialize_Element(m_nMode);
	ar.Serialize_Element(m_bLoginStatus);
}

void LoginInfo::Init()
{
	m_strID = _T("Guest");
	m_strPW = _T("");
	m_nMode = enLoginMode::Guest;
	m_bLoginStatus = FALSE;
}

LoginInfo& LoginInfo::operator = (const LoginInfo& src)
{
	m_strID = src.m_strID;
	m_strPW = src.m_strPW;
	m_nMode = src.m_nMode;
	m_bLoginStatus = src.m_bLoginStatus;
	return *this;
}

LPCTSTR LoginInfo::GetID() { return m_strID; }
LPCTSTR LoginInfo::GetPW() { return m_strPW; }
enLoginMode LoginInfo::GetMode() { return (enLoginMode)m_nMode; }


void LoginInfo::SetID(CString strID){ m_strID = strID; }
void LoginInfo::SetPW(CString strPW) { m_strPW = strPW; }
void LoginInfo::SetMode(int nMode) { m_nMode = nMode; }
BOOL LoginInfo::Login() 
{
	m_bLoginStatus = TRUE; 
	return m_bLoginStatus;
}
BOOL LoginInfo::Logout()
{
	m_bLoginStatus = FALSE;
	return m_bLoginStatus;
}
BOOL LoginInfo::IsLogin()
{
	return m_bLoginStatus;
}