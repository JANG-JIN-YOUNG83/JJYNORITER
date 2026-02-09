#pragma once

#include "basedef.h"
#include "../Module/CustomControl/gridctrl.h"

#include "CDlgDefectGrid.h"
#include "CISISpecTable.h"
#include "CISIClassification.h"
#include "Process_XTPFlowGraph.h"
//#include "ExcelWrapClass.h"
#include "Excel/ExcelWrapClass.h"
#include "DlgInspectionMain.h"

// CDlgClassificationST 대화 상자

class CDlgInspectionMain;
class CDlgClassificationCondition;

class CDlgClassificationST : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgClassificationST)

public:
	CDlgClassificationST(CISISpecTable& spec, CWnd* pParent, CISICellAllDefect* defect, long blobconditionSize, long nSelectindex = -1);   // 표준 생성자입니다.
	virtual ~CDlgClassificationST();

	enum { IDD = IDD_DIALOG_CLASSIFICATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CDlgDefectGrid* m_pDlgDefectGrid;

	CISISpecTable m_ISI_Spec_table;
	IPVM::ImageView* m_imageView_Bright;
	IPVM::ImageView* m_imageView_Dark;
	IPVM::ImageView* m_imageView_Binary;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);


	virtual BOOL OnInitDialog();

	//// XTPFlowChart
	const UINT ID_FLOWGRAPH_CONTROL = 100;
	CRect m_rtXTPFlowChart;
	CXTPFlowGraphControl m_wndControl;
	void StartFlowChart(BOOL bResetPage = TRUE);
	void DrawFlow(std::vector<enLogicResult> answers);
	std::vector<Process_XTPFlowGraph::CNodeWrapped> m_nodes;

	void InitImages();
	void AddYes(CXTPFlowGraphControl* wndControl, CPoint location, CSize size);
	void AddNo(CXTPFlowGraphControl* wndControl, CPoint location, CSize size);
	// end XTPFlowChart

	//test
	CISICellAllDefect m_DefectInfoOrg;
	CISICellAllDefect m_DefectInfoProcess;
	long m_nSelectDefectIndex;
	CISIClassification m_classfication;

	CISISpecInfo	m_TableDefectInfo;

	CTabCtrl m_ctrTab;
	CTabCtrl m_ctrTab2;
	CStatic m_ctrStaticClassification;
	CStatic m_ctrStaticFinaljudge;

	template <typename T>
	CString Numeral2String(CString type, T value)
	{
		CString strValue;
		strValue.Format(type, value);
		return strValue;
	}

	void DisplayFinalJudge(II_RESULT_VALUE Result, DefectType dt);


	CString Enable2String(bool bEnable)
	{
		switch (bEnable)
		{
		case 0:
			return _T("x");
			break;
		case 1:
			return _T("true");
			break;
		default:
			break;
		}
	}
	bool String2Enable(CString text)
	{
		if (_T("x") == text)
			return 0;

		if (_T("true") == text)
			return 1;

		return 1;
	}


	CString InspectionArea2String(enInspectionArea type)
	{
		switch (type)
		{
		case enInspectionArea::IA_NONE:
			return _T("None");
			break;
		case enInspectionArea::IA_BACKGROUND_BURR:
			return _T("Burr");
			break;
		case enInspectionArea::IA_SCRAB:
			return _T("Scrab");
			break;
		case enInspectionArea::IA_TAB:
			return _T("Tab");
			break;
		case enInspectionArea::IA_TAB_INSULATION:
		case enInspectionArea::IA_CUTTINGHEIGHT_TOP:
		case enInspectionArea::IA_CUTTINGHEIGHT_BOT:
			return _T("Insulation");
			break;
		case enInspectionArea::IA_ELECTRODE_EDGE:
		case enInspectionArea::IA_ELECTRODE_EDGE_LEFT:
		case enInspectionArea::IA_ELECTRODE_EDGE_RIGHT:
			return _T("Electrode Edge");
			break;
		case enInspectionArea::IA_ELECTRODE:
			return _T("Electrode");
			break;
		default:
			return _T("error");
			break;
		}
	}

	void NextOrAnd(int nRow, int nCol);
	void NextInspectionImgType(int nRow, int nCol);
	void NextCompareOperator(int nRow, int nCol);
	void EditGridText(int nRow, int nCol);
	void NextAreaOperator(int nRow, int nCol);
	void TogeterValue(int nRow, int nCol);

	long m_nSelectCol;
	std::vector<DefectType>* m_vecTogeter;

	CPoint GetCellPoint(int nRow, int nCol);

	CRect m_rtGrid;

	long m_screenSizeX;
	long m_screenSizeY;

	void SetTab();
	void SetGrid();

	//GRID 관련한 내용
	CGridCtrl m_ctrGrid;
	std::vector<CString> vecTitle;
	std::vector<int> m_vecnSizeCol;
	int m_nSizeRow;
	//std::vector<std::pair<DefectType, CISISpecInfo*>> vecItem;
    std::vector<std::pair<DefectType, std::vector<CISISpecInfo>*>> vecItem_multi;
	
	void initGrid();
	//void UpdateDefectGrid();

	void updateLogic();
	void LinkSpecAndTable();

	CString strText;

	bool BackGroundColor(long nrow, COLORREF& RGB);
	COLORREF DefectBackGroundColor(II_RESULT_VALUE result = II_RESULT_VALUE::INVALID);

	void DisplayData(GV_DISPINFO* pDisplay, long nrow, long ncor);

	template <typename T>
	bool DisplayItemNumber(GV_DISPINFO* pDisplay, T value, CString DataType, bool bUse, long nrow)
	{
		if (true == bUse)
		{
			pDisplay->item.strText = Numeral2String(DataType, value);
			//pDisplay->item.crBkClr = BackGroundColor(nrow);
		}

		return bUse;
	}

	bool DisplayItemCompare(GV_DISPINFO* pDisplay, enCompareOperator compare, bool bUse, long nrow);

	BOOL CallBackGrid(GV_DISPINFO* pDisplay);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	CStatic m_ctrStatic_image1;
	CStatic m_ctrStatic_image2;
	CStatic m_ctrStatic_image3;

	CButton m_ctrBtnExist;
	CButton m_ctrBtnCancel;
	CButton m_ctrBtnRefresh;
	CButton m_ctrBtnSpecInit;

	CButton m_ctrBtnSaveExcel; // Excel File 저장 Btn
	CButton m_ctrBtnSaveSpec; 
	CButton m_ctrBtnLoadSpec; 
	CButton m_ctrBtnSetPriority; 
	CButton m_ctrBtnSetPriorityUp; 
	CButton m_ctrBtnSetPriorityDown; 

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnReFresh();

	void ShowImage();

	void SetGridType(bool bSmallDefect);
	std::map<DefectType, bool> m_mapSD;
	std::map<enColTitle, bool> m_mapSDCol;

	bool isSmallDefectType(DefectType dt);
	bool isSmallDefectUsingCol(enColTitle ct);

	CListCtrl m_ctrList;
	afx_msg void OnBnClickedBtnInitSpec();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTcnSelchangeTab2(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CButton m_cntBtn_FgZoomFit;
	CButton m_cntBtn_FgZoomPlus;
	CButton m_cntBtn_FgZoomMinus;
	afx_msg void OnBnClickedBtnFgzoomfit();
	afx_msg void OnBnClickedBtnFgzoomplus();
	afx_msg void OnBnClickedBtnFgzoomminus();
	afx_msg void OnBnClickedCheck1();
	CButton m_ctrUse;
	CButton m_btn_useAdmin;
	CButton m_btn_useNormal;
	afx_msg void OnBnClickedBtnUseNormal();
	afx_msg void OnBnClickedBtnUseAdmin();
	bool m_use_mode;
	CButton m_btnInspectionArea;
	afx_msg void OnBnClickedBtnInspectionArea();
	long m_blobConditionSize;

	CExcelWrapClass m_ExcelSave;
    
	afx_msg void OnBnClickedBtnReSaveexcel();

	void CreateDir(CString strFolderPath);

	long m_nExcelRow; // Excel Fie 작성 줄..
    afx_msg void OnBnClickedBtnPriorityUp();
    afx_msg void OnBnClickedBtnPriorityDown();
    
	long m_nDefectSelect;
    CDlgInspectionMain* m_pParent;
    CDlgClassificationCondition* m_classificationCondition;

	void SpecTableTrans(std::vector<CString>& vecConditions, DefectType DT);
    void SpecTableLoad(CString strConditions, CString strIndex, CString strName, long nSize);
    BOOL StringTokenDivideToCString(CString strFull, TCHAR chDivide, std::vector<CString>& vecstrValue);
    void InitSpecSize(DefectType dt, CISISpecInfo& spec);
    CString InitDisplayCondition(long nRow, BOOL bTotal = FALSE);
    std::vector<CString> SelectDispCondition(long nRow);
    afx_msg LRESULT OnUWMGridUpdate(WPARAM wParam, LPARAM lParam);
    std::vector<CString> strTemp;
    long m_nDbSelectRow;
    afx_msg void OnBnClickedBtnSaveSpecCsv();
    afx_msg void OnBnClickedBtnLoadSpecCsv();
    BOOL m_bCheckSpecError;
    std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>> m_vecTableTemp;
};
