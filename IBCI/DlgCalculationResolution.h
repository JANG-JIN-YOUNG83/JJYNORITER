#pragma once
#include "ipvmbasedef.h"
#include "Types/Image_8u_C1.h"
#include "Widget/ImageView.h"
#include "CalMap_Distortion_Impl.h"
#include "CalMap_Distortion.h"
// CDlgCalculationResolution 대화 상자
enum
{
	IDC_PARA_SHOW_ROI_RESOLUTION,
};

class CDlgCalculationResolution : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgCalculationResolution)

public:
	CDlgCalculationResolution(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgCalculationResolution();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CALC_RESOLUTION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	//CXTPPropertyGrid* m_gridCalcResol;
	CStatic m_imgResol;
	afx_msg void OnBnClickedButtonImgOpen();

	IPVM::ImageView* m_pImageView;
	IPVM::Image_8u_C1 m_image;

	afx_msg void OnBnClickedButtonShowLine();
	long m_nFindEdge;
	long m_nSearchWidth;
    afx_msg void OnBnClickedButtonSetLineTh();
    CEdit m_editLineThreshold;
    long m_nLineThreshold;
	//왜곡 보정 및 Resolution 계산
    afx_msg void OnBnClickedBtnCalcres();
    afx_msg void OnBnClickedBtnSavedistor();
    BOOL CalcScale(BYTE* byImage, IPVM::ImageView* pImage, IPVM::Rect rtDown, IPVM::Rect rtRight);
    BOOL GetHorLinePoints(const IPVM::Point_32f_C2* pPoints, long nPointCount,
        std::vector<std::vector<IPVM::Point_32f_C2>>& vecHorLinePoints,
        std::vector<IPVM::Point_32f_C2>& vecSortedPointsByX);
    BOOL GetHorLineEquations(CRect rtImageROI, const std::vector<std::vector<IPVM::Point_32f_C2>>& vecHorLinePoints,
        std::vector<IPVM::LineEq_32f>& vecHorLineEquations);
    BOOL GetVerLinePoints(const IPVM::Point_32f_C2* pPoints, long nPointCount, std::vector<std::vector<IPVM::Point_32f_C2>>& vecVerLinePoints);
    BOOL GetVerLineEquations(CRect rtImageROI, const std::vector<std::vector<IPVM::Point_32f_C2>>& vecVerLinePoints,
        std::vector<IPVM::LineEq_32f>& vecVerLineEquations);
    BOOL GetReferencePosition(const std::vector<IPVM::LineEq_32f>& vecHorLineEquations,
        const std::vector<IPVM::LineEq_32f>& vecVerLineEquations, const std::vector<IPVM::Point_32f_C2>& vecSortedInputPoints,
        std::vector<IPVM::Point_32f_C2>& vecRefPositions);
    BOOL GetDistortion(const std::vector<IPVM::Point_32f_C2>& vecRefPoints, CCalMap_Distortion& distortionMap,
        std::vector<float>& vecfDistortion);
    void Get1LineAngle(float x0, float y0, float x1, float y1, float& o_angle);
    std::vector<IPVM::Point_32f_C2> m_vecTemp;
    std::vector<IPVM::Point_32f_C2> m_vecTemp2;
    double m_tempX;
    double m_tempY;
    IPVM::Rect m_rtROI;
};
