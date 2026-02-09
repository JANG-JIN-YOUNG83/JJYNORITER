#pragma once

#include "../SharedModule/VisionHostShare//LoginInfo.h"
// CDlgSpecMeasure 대화 상자
const long COUNT_CELL_MEASURE = 50;
class CInspectionMeasure;
class CDlgInspectionMain;

class CDlgSpecMeasure : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSpecMeasure)

public:
	CDlgSpecMeasure(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgSpecMeasure();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SPEC_MEASURE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
    CXTPPropertyGrid* m_inspectionItemGrid;
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    virtual BOOL OnInitDialog();
    void updateGrid();
    CDlgInspectionMain* m_parent;
    Share::enLoginMode m_loginMode;

};
