#pragma once


// CDlgDefectTimerView 대화 상자
class CDlgDefectMap;
class CDlgDefectTimerView : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDefectTimerView)

public:
	CDlgDefectTimerView(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgDefectTimerView();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DEFECTTIMERVIEW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();

	CDlgDefectMap* m_pParent;

	IPVM::ImageView* m_pImageView;
    afx_msg void OnSize(UINT nType, int cx, int cy);
    CRect m_DlgSize;
	void CALL_Function_CSC();
    void ControlSizeChange(int nID);
    void SetShowWindow(bool bShow);

	void Init();

	void SetROI(std::vector<IPVM::Rect> vecROI);
    void ROIClear();
};
