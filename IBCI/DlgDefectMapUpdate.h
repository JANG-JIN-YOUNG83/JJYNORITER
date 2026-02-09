#pragma once

#define UWM_DEFECTMAP_UPDATE_QUIT (WM_USER + 8001)
// CDlgDefectMapUpdate 대화 상자
class CDlgDefectMap;

class CDlgDefectMapUpdate : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDefectMapUpdate)

public:
	CDlgDefectMapUpdate(CWnd* pParent);   // 표준 생성자입니다.
	virtual ~CDlgDefectMapUpdate();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DEFECTMAP_UPDATE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
    CProgressCtrl m_progCtrlDefectmap;
    afx_msg void OnBnClickedCancel();
    virtual BOOL OnInitDialog();
    void EndUIUpdate();
	afx_msg void OnBnClickedRestart();
	afx_msg void OnBnClickedQuit();
	CDlgDefectMap* m_pParent;
    void StopUIUpdate();
};
