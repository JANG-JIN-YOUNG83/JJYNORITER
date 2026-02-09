#pragma once

#include "../Module/CustomControl/CustomButton.h"
#include "../Grab/GrabExecutor.h"
#include "resource.h"


class CIBCIDlg;
class CInspectionCell;

// CDlgViewCellImage 대화 상자
typedef enum _enumViewCellImageMode
{
    DO_NOT_UPDATE = 0,
    UPDATE_ALWAYS,
    UPDATE_NG_ONLY

} ENUM_VIEW_MODE_CELL_IMAGE;

class CDlgViewCellImage : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgViewCellImage)

public:
    CDlgViewCellImage(MachineType machinType, CWnd* pParent = nullptr); // 표준 생성자입니다.
	virtual ~CDlgViewCellImage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_VIEW_CELL_IMAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCbnSelchangeComboModeUpdate();
    afx_msg void OnCbnSelchangeComboModeInspection();
	DECLARE_MESSAGE_MAP()

	// Variables
public:
	IPVM::ImageView* m_imageViews[10];
	int m_nModeUpdate;
	CComboBox m_ComboModeUpdate;
    CComboBox m_ComboModeGray;
    CComboBox m_ComboModeProcessing;
    int m_nModeGray;
    int m_nModeProcessing;
    CButton m_btn_ImageLoad;
    CButton m_btn_Inspection;
    //CButton m_btn_InlineInspMode;
    CCustomButton* m_pButtonInlineInspMode;
  	CButton m_btn_OriImage;
    CButton m_btn_PreProcessing;
    CButton m_btn_Threshold;

    CString m_strImageName;
    CIBCIDlg* m_pParent;
	
	CString m_strBrightPath;
    CString m_strDarkPath;
    CComboBox m_ComboModeInspection;
    CInspectionCell* m_InspectionCell;
    long m_nModeInspection;
    BOOL m_bSurfaceInsp;

	long m_nCameraIndex;
	// Functions
public:
	void Resize(int cx, int cy);
    afx_msg void OnCbnSelchangeComboModeGray();
    afx_msg void OnCbnSelchangeComboModeProcessing();
	afx_msg void OnBnClickedButtonSufaceImageLoad();
    afx_msg void OnBnClickedButtonSufaceInspection();
    void SetDlg(CIBCIDlg* pParent);
    afx_msg void OnBnClickedButtonInlineInsp();
    void EnableWindow(BOOL bEnable);
    void ImageViewClear();
    afx_msg void OnBnClickedButtonOriImage();
    afx_msg void OnBnClickedButtonPreProcessing();
    afx_msg void OnBnClickedButtonThreshold();
};
