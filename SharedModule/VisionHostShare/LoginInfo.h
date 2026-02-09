#pragma once
#include "basedef.h"

#include <afxmt.h>



namespace Share
{

	enum enLoginMode
	{
		Guest = 0,
		Operator,
		CrewLeader,
		Engineer,
		Administrator
	};
	

	class __VISION_HOST_SHARE_CLASS__ LoginInfo : public CommonShare::BaseObject
	{
	public:

		LoginInfo();
		~LoginInfo();
	public:

		virtual void Serialize(CommonShare::ArchiveAllType& ar) override;
		LoginInfo& operator = (const LoginInfo& src);

		void Init();

		LPCTSTR GetID();
		LPCTSTR GetPW();
		enLoginMode GetMode();

		void SetID(CString strID);
		void SetPW(CString strPW);
		void SetMode(int nMode);
		BOOL Login();
		BOOL Logout();
		BOOL IsLogin();


	private:
		BOOL m_bLoginStatus;
		CString m_strID;
		CString m_strPW;
		int m_nMode;

	};
}

