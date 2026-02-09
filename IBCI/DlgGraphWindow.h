#pragma once
#include "DlgGraphFrame.h"

class CIBCIDlg;
// CDlgGraphWindow 대화 상자

class CDlgGraphWindow : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgGraphWindow)

public:
	CDlgGraphWindow(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgGraphWindow();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_GRAPH_WINDOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()


public:
	CIBCIDlg*							m_pParent;
	CRect								m_rtDlgSize;
	CFont								m_font;
	CFont								m_font_2;
	CBrush								m_BrushBlock;
	CBrush								m_BrushBlue;

	std::vector<CDlgGraphFrame*>			m_pvecDlgGraphFrame;
	std::map<DefectType, CDlgGraphFrame*>	m_mapDlgGraphFrame;

public:
	void ArrangeWindow_Graph(long nHorizentalCnt, long nVerticalCount);
	BOOL SetGraphFrame(const std::vector<DefectType>& vecInspTypes, BOOL bShow);
	long GetGraphFrameCount() const;
	CDlgGraphFrame* GetGraphFrame(long inspIndex);
	CDlgGraphFrame* FindGraphFrame(DefectType inspType);
	void DataReset();
    afx_msg void OnBnClickedButtonSettingSpec();
    CEdit m_editSpecX;
    CEdit m_editSpecY;
    CButton m_btnSettingSpec;
};
