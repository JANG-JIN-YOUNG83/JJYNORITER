#pragma once

# include <memory>
#include "AnimationCustomizeBase.h"
#include "AnimationData.h"

// CDlgAnimation 대화 상자
class CDlgAnimationWindow;
class CDlgAnimation : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAnimation)

public:
	CDlgAnimation(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgAnimation();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_ANIMATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

protected:
	CDlgAnimationWindow* m_pParent;
	CDC m_MemDC;
	CBitmap m_MemBitmap;
	CBitmap* m_pOldMemBitmap;
	int m_nLoginMode;

	//std::shared_ptr<AnimationCustomizeBase> m_animationStyle;

	CCriticalSection m_csData;
	AnimationData m_data;
	short	m_dataXaxisCount;
	short	m_dataGroupCount;

	void Ready();
	void End();

public:
    std::shared_ptr<AnimationCustomizeBase> m_animationStyle;
    void SetAnimationParameter(ResultMismatch Mismatch, float fOffset, float fMin, float fMax, float fSpecNominal,
        float fSpecMin, float fSpecMax, float fAiSpecMin = -9999, float fAiSpecMax = -9999, float fNgSpecMin = -9999,
        float fNgSpecMax = -9999, int nLaneCount = 2, bool bOSDS = 1, bool bRollerOS = 1);
	BOOL SetAccMaxDiffData(long id, int index, float value, BOOL judge, BOOL bInvalidData = FALSE);
	BOOL SetData(long id, int index, float value, BOOL judge, BOOL bInvalidData = FALSE);
	void SetDataGroupCount(long groupCount);
	void SetLogMode(int nMode);
	void DataSetZero();

	void SetTitle(LPCTSTR title);
	void SetGraphStyle(std::shared_ptr<AnimationCustomizeBase> animationStyle);
	std::shared_ptr<AnimationCustomizeBase> GetAnimationStyle();
};
