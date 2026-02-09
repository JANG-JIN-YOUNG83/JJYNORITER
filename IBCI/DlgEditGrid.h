#pragma once


// CDlgEditGrid 대화 상자

class CDlgEditGrid : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgEditGrid)

public:
	CDlgEditGrid(CString strTitle, CString strValue, CPoint cPoint, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgEditGrid();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_EDIT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ctrEdit;
	CString m_strValue;
	CString m_strTitle;
	CPoint m_cPoint;
	CWnd* m_cWnd;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
