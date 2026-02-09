#pragma once
#include "staticDefect.h"
//#include "Common.h"
//#include "Host_InterfaceDlg.h"
#include "..\\SharedModule\CommonShare\DefectDef.h"
// CDlgDefectMap 대화 상자
#include "resource.h"
#include "CISIViewerProperty.h"
#include "DlgDefectImageView.h"
#include "../IBCI/ThreadControl.h"
#include "CDlgDefectTimerView.h"
class CIBCIDlg;

class CDlgAccDefectMap;
class CDlgDefectMapUpdate;
class CDlgDefectTimerView;
class CDlgDefectMap : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDefectMap)

public:
	CDlgDefectImageView* m_dlgDefectView;
	std::vector<CISIViewerProperty> m_vecISIAllDefectTotal;
	CDlgDefectMap(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgDefectMap();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_DEFECT_MAP };

private:
	CRect		m_DlgSize;

	CFont		m_font, m_font_Bold;
	CBrush		m_Brush, m_Brush_Title;

	long		m_nDefectMapSizeX;
	long		m_nDefectMapSizeY;

	double		m_dImageSizeX;
	double		m_dImageSizeY;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void CALL_Function_CSC();
	void ControlSizeChange(int nID);

	staticDefect m_staDefect_1;
	staticDefect m_staDefect_2;

	//void SetPoint(Share::ResultVision& data, HostData::Result_Cell* VD_BOX);
	void SetPoint(long nTopBack, DefectPointXY XY, long nIndex, long nImageSizeX);

	void DataCountSetZero();
	std::vector< DefectPointXY> m_vecTopDefect;
	std::vector< DefectPointXY> m_vecBtmDefect;

	CIBCIDlg* m_pParent;

	void SaveDefectMapPointData(int nPointCount, int nCellid, int nCamera_Position, int nX, int nY, DefectType inspType);						// 20231010 LJH
	void LoadDefectMapPointData();																												// 20231010 LJH
	afx_msg void OnBnClickedButtonDefectChange();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	CDlgDefectMapUpdate * m_pDlgDefectMapUpdate;
    CDlgDefectTimerView* m_pDlgDefectTimerView;
    BOOL m_bQuitDefectMapUpdate;
    ThreadControl m_threadControl_UpdateMap;

	void ThreadStart_UpdateMap();
    void ThreadStop_UpdateMap();
	static UINT ThreadFunc_UpdateMap(LPVOID pParam);
    UINT ThreadFunc_UpdateMap();

	//static UINT ThreadFunc_UpdateDefectROI(LPVOID pParam);
 //   UINT ThreadFunc_UpdateDefectROI();
 //   ThreadControl m_threadControl_UpdateDefectROI;

    long m_nThreadUIReadNum;
    BOOL StringTokenDivideToCString(CString strFull, TCHAR chDivide, std::vector<CString>& vecstrValue);
    BOOL m_bPauseDefectMapUpdate;
    CRect m_rtSize;
    BOOL m_bTimerDialogView;

	typedef struct _DefectMapPointPath
    {
        int nPointX;
        int nPointY;
        int nDefectCode;
        CString strBrightPath;
        CString strDarkPath;

    } DefectMapPointPath;
    std::vector<DefectMapPointPath> m_vecMapPointPathTop, m_vecMapPointPathBack;
    void DefectMapClick(CString strFilePath, CPoint point, std::vector<DefectPointXY> vecDefectPt,
        std::vector<DefectMapPointPath>& vecMapPointPath);
};
