#pragma once
#include "staticDefect.h"
#include "Common.h"
//#include "Host_InterfaceDlg.h"

// CDlgDefectMap 대화 상자


class CHost_InterfaceDlg;
class CDlgAccDefectMap;

class CDlgDefectMap : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDefectMap)

public:
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
	void SetPoint(long nType, DefectPointXY XY);

	void DataCountSetZero();

	CHost_InterfaceDlg* m_pParent;

	void SaveDefectMapPointData(int nPointCount, int nCellid, int nCamera_Position, int nX, int nY, DefectType inspType);						// 20231010 LJH
	void LoadDefectMapPointData();																												// 20231010 LJH
	afx_msg void OnBnClickedButtonDefectChange();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
