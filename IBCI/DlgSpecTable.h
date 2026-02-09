#pragma once
#include "basedef.h"
#include "../Module/CustomControl/gridctrl.h"
#include "../Inspection/CISISpecInfo.h"
#include "../Inspection/CISISpecTable.h"

// CDlgSpecTable 대화 상자

class CDlgSpecTable : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSpecTable)

public:
    CDlgSpecTable(CWnd* pParent = nullptr); // 표준 생성자입니다.
	virtual ~CDlgSpecTable();
    CISISpecTable m_ISI_Spec_table;

	//GRID 관련한 내용
    CTabCtrl m_ctrTab;

    CGridCtrl m_ctrGrid;
    std::vector<CString> vecTitle;
    std::vector<int> m_vecnSizeCol;
    int m_nSizeRow;
    long m_nRow;
    long m_nCol;
    std::vector<std::pair<DefectType, CISISpecInfo*>> vecItem;
    CRect m_dlgSize;
    long m_screenSizeX;
    long m_screenSizeY;
    CRect m_rtGrid;

    long m_nSelectDefectIndex;
    //CISICellAllDefect m_DefectInfoProcess;

    std::vector<DefectType>* m_vecTogeter;
    long m_nSelectCol;



// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SPEC_TABLE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    void initGrid();
    
    void NextOrAnd(int nRow, int nCol);
    void NextInspectionImgType(int nRow, int nCol);
    void NextCompareOperator(int nRow, int nCol);
    void EditGridText(int nRow, int nCol);
    void NextAreaOperator(int nRow, int nCol);
    void TogeterValue(int nRow, int nCol);
    CPoint GetCellPoint(int nRow, int nCol);
    void LinkSpecAndTable();
    void DisplayData(GV_DISPINFO* pDisplay, long nrow, long ncor);
    BOOL CallBackGrid(GV_DISPINFO* pDisplay);
    void OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
    bool JobFileSave();
    void ControlSizeChange(int nID);
    void SetGridType(bool bSmallDefect);
    std::map<DefectType, bool> m_mapSD;
    std::map<enColTitle, bool> m_mapSDCol;

    bool isSmallDefectType(DefectType dt);
    bool isSmallDefectUsingCol(enColTitle ct);

    template<typename T>
    CString Numeral2String(CString type, T value)
    {
        CString strValue;
        strValue.Format(type, value);
        return strValue;
    } 

    template<typename T>
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
    afx_msg void OnBnClickedButtonUseNormal();
    afx_msg void OnBnClickedButtonUseAdmin();
    afx_msg void OnBnClickedButtonInitSpec();
    CButton m_btn_useAdmin;
    CButton m_btn_useNormal;
    bool m_use_mode;
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};
