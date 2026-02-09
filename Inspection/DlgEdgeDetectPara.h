#pragma once
#include "basedef.h"
#include <XTToolkitPro.h>

// DlgEdgeDetectPara 대화 상자

class DlgEdgeDetectPara : public CDialogEx
{
	DECLARE_DYNAMIC(DlgEdgeDetectPara)

public:
	DlgEdgeDetectPara(Base::ParaEdgeDetection& paraEdgeDetection, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~DlgEdgeDetectPara();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_EDGE };
#endif

protected:
	CXTPPropertyGrid m_property;
	Base::ParaEdgeDetection& m_paraEdgeDetection;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};


