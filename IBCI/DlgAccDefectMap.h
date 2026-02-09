#pragma once
//#include "Common.h"
//#include "Host_InterfaceDlg.h"

#include "ipvmbasedef.h"

#include "Types/Image_8u_C1.h"
#include "Types/Image_8u_C3.h"
#include "Types/Image_8u_C4.h"
#include "resource.h"

// CDlgAccDefectMap 대화 상자

class CDlgDefectMap;
class CDlgImageView;
class CIBCIDlg;
class CDlgAccDefectMap : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgAccDefectMap)

public:
	CDlgAccDefectMap(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgAccDefectMap();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_ACC_DEFECTMAP};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	CRect		m_DlgSize;
	CRect		m_MapSize;

	CFont		m_font, m_font_Bold;
	CBrush		m_Brush, m_Brush_Title;

	double		m_dImageSizeX;
	double		m_dImageSizeY;


	// 일단은 6 X 20 으로 고정 (추후 바꿔달라하면 옵션으로 빼던 하자)
	int			m_nCountTop[6][20];
	int			m_nCountBtm[6][20];

	int			m_nFlaTop[6][20];
	int			m_nFlaBtm[6][20];

public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//void SetPoint(Share::ResultVision& data, HostData::Result_Cell* VD_BOX);
	void SetPoint(long nType, DefectPointRect XY);

	void DataCountSetZero();

	void CALL_Function_CSC();
	void ControlSizeChange(int nID);

	void TopUpdateUI();
	void BtmUpdateUI();

	BOOL CheckTopImage();
	BOOL CheckBtmImage();

	int SetCount(long nType, DefectPointRect XY);
	void SetCountClear();

	CIBCIDlg* m_pParent;

	IPVM::Image_8u_C1 imageTop;
	IPVM::Image_8u_C1 imageBtm;

	// Image View
	IPVM::ImageView* m_pImageViewTop;
	IPVM::ImageView* m_pImageViewBtm;

	CCriticalSection m_CS;
	afx_msg void OnBnClickedButtonDefectChange();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
