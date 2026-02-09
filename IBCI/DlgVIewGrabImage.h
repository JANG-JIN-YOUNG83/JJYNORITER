#pragma once


// CDlgVIewGrabImage 대화 상자

class CDlgVIewGrabImage : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVIewGrabImage)

public:
	CDlgVIewGrabImage(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgVIewGrabImage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VIEW_GRAB_IMAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

public:
	//IPVM::ImageView* m_imageViews[6];
    IPVM::ImageView* m_imageViews[10];

	long m_cameraIndex;

	// Functions
public:
	void Resize(int cx, int cy);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

};
