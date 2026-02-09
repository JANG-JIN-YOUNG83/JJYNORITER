#pragma once
#include "DlgAnimation.h"

class CDlgAnimation;
class CIBCIDlg;


// CDlgAnimationWindow 대화 상자
class CDlgAnimationWindow : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAnimationWindow)

public:
	CDlgAnimationWindow(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgAnimationWindow();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_ANIMATION_WINDOW };

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
	
	CDlgAnimation*		m_pDlgAnimation;
	//std::map<DefectType, CDlgAnimation>	m_mapDlgAnimation;
	void SetAnimationParameter(ResultMismatch Mismatch, float fOffset, float fMin, float fMax, float fSpecNominal, float fSpecMin, float fSpecMax, float fAiSpecMin = -9999, float fAiSpecMax = -9999, float fNgSpecMin = -9999, float fNgSpecMax = -9999, int nLaneCount = 2, bool bOSDS = 1, bool bRollerOS = 1);
public:
	void ArrangeWindow_Graph(long nHorizentalCnt, long nVerticalCount);
	void DataReset();
};
