#pragma once
#include "pch.h"
#include "afxdialogex.h"
#include <../../iVisionMadangSDK/include/Widget/ImageView.h>
#include "CDlgDefectRawSearch.h"
#include "DlgLotSearch.h"
#include <vector>
#include "Excel/ExcelWrapClass.h"
#include "..\\SharedModule\CommonShare\DefectDef.h"
#include "CDlgDefectTimeTable.h"

#ifndef GLOBALS_H
#define GLOBALS_H
#endif // GLOBALS_H
// CDlgDataSearch 대화 상자

class CDlgDefectRawSearch;
class CDlgLotSearch;
class CExcelWrapClass;
class CDlgDefectTimeTable;

typedef struct _DefectInfoTable
{
    CString strDefecntName;
    long nDefectNum =0;
} DefectInfoTable;

 typedef struct _DefectTimetable
{
    CString strYear;
    CString strMonth;
    CString strDay;
    CString strHour;
    CString strMin;
    CString strSecond;
    CTime tm;
    CString strDefectName;
    DefectInfoTable DtTable;
    long Line = 0;
    long Pinhole = 0;
    long Horizontal_Line = 0;
    long Scratch = 0;
    long Crack = 0;
    long Stain = 0;
    long Crater = 0;
    long Delamination_Crack = 0;
    long Dent = 0;
    long Balck_Dot = 0;
    long Delamination_Pinhole = 0;
    long Banking = 0;
    long ETC_Defect = 0;
    long Chipping_Pinhole = 0;
    long Drag = 0;
    long Edge_Line = 0;
    long Edge_Defect = 0;
    long Island = 0;
    long Plain_Wrinkle = 0;
    

} DefectTimetable;

class CDlgDataSearch : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDataSearch)

public:
	CDlgDataSearch(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgDataSearch();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
    enum
    {
        IDD = IDD_DIALOG_DATA_SEARCH
    };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:



    typedef struct _DefectInfo
    {
        CString strDefecntName;
        long nDefectNum;
        CString strCSVPath;
    } DefectInfo;

    typedef struct _DefectFilterInfo
    {
        std::vector<DefectInfo> defectinfo;
        CString strCSVPath;
        CString strLastPush;
        long nTotalNum;
    } DefectFilterInfo;

    typedef struct _LotDefectData
    {
        CString strDefectName;
        CString  strTotalNum;
        _LotDefectData() : strTotalNum(_T("0")) {}
        _LotDefectData(CString name, CString totalNum) : strDefectName(name), strTotalNum(totalNum) {}

    } LotDefectData;

    typedef struct _LotLainData
    {
        long nLain;
        long nTotalNum;
    } LotLainData;

    typedef struct _CellDefectInfo
    {
        CString strLotStartTime;
        CString strLotEndTime;
        CString strRecipe;
        long nMeter;
        long nMachineType; // 검사기 타입
        long nPatternNum;
        long nLainNum;
        long nStatus;
        float fAVG_GV;
        float fMIN_GV;
        float fMAX_GV;
        float fBrightAVG;
        float fDarkAVG;
        float fSize_X;
        float fSize_Y;
        float fArea_Ratio;
        float fRatio_YX;
        float fRatio_XY;
        float fCompact;
        float fRatio_White;
        float fRatio_Dark;
        float fEdge_Energy;
        float fMass_Center_GV;
        CString strLotID;
        CString strCellID;
        CString strTime;
        CString strDefectName;
        CString strCamTopBack;
        //불량 검출 영역(코팅/무지부)의 평균 GV?
        float fDefectX;
        float fDefectY;
        long nJudge;
        CString strBrightImagePath;
        CString strDarkImagePath;
        CString strCSVPath;
        long nCSVRow;
        std::vector<std::vector<CString>> vecvecRowColInfo;
        IPVM::Image_8u_C1 DefectImage;

        long nTotalDefectCount;
        //std::vector<DefectInfo> vecdefectinfo;
        //DefectInfo defectinfo;
        std::vector<std::vector<DefectInfo>> vecvecDefectInfo;
    } CellDefectInfo;

    typedef struct _LotDefectInfo
    {
        CString strLotID;
        CString strRecipe;
        CString strLotStartTime;
        CString strLotEndTime;       
        CString strTotalMeter;
        CString strMachiceType;
        CString strTotalNum;
        std::vector<LotDefectData> vecDefectData;
        std::vector<LotLainData> vecLainData;
        CString strDefectName;        
        CString  strLain; 
        std::vector<CString> vecIncludeCSVFilePath;

        //long nTotalNum;
        ///치수종류별/레인별/Total PPk 값 * 최대 8레인???
        //이건 어케 추가할 것인지
        //일단 대기...
        void init()
        {
                std::vector<CString> vecStrDefectName = {
                _T("Line"),
                _T("Pinhole"),
                _T("Horizontal_Line"), 
                _T("Scratch"), 
                _T("Crack"),
                _T("Stain"),
                _T("Crater"),
                _T("Delamination_Crack"),
                _T("Dent"),
                _T("Black_Dot"), 
                _T("Delamination_Pinhole"),
                _T("Banking"),
                _T("ETC_Defect"),
                _T("Edge_ChippingHole"),
                _T("Drag"),
                _T("Edge_Line"), 
                _T("Edge_Defect"),
                _T("Island"), 
                _T("Plain_Wrinkle")
                };
                LotDefectInfo lotdefect;
                CString strName;
                for (long n = 0; n < vecStrDefectName.size(); n++)
                {
                    vecDefectData.push_back(LotDefectData(vecStrDefectName[n], _T("0")));
                }       
        }
    } LotDefectInfo;


    //template<typename T>
    //struct ValuePair
    //{
    //    T defectValue;
    //    T infoValue;
    //};

    // control variable
    CDateTimeCtrl m_dateStartMonth;
    CDateTimeCtrl m_dateEndMonth;
    CDateTimeCtrl m_dateStartTime;
    CDateTimeCtrl m_dateEndTime;
    CButton m_radioBtnRaw;
    CButton m_radioBtnLot;
    CListCtrl m_listCtrlCsvList;
    CListCtrl m_listCtrlCsvInfo;
    CButton m_btnFind;
    CEdit m_editSearchBlank;
    CStatic m_staticDefectInfo;
    CListCtrl m_listCtrlExcelSelect;
    CButton m_checkCropSave;


    CDlgDefectTimeTable* m_pDlgDefectTimeTable;
    // member variable
    int m_nRadioSelect;
    IPVM::ImageView* m_pImageView[2];
    long m_nSelect;
    CDlgLotSearch* m_pDlgLotSearch;
    BOOL m_bFirstClick;
    std::vector<std::vector<CString>> m_vecvecRowColInfo;
    std::vector<CString> m_vecTitle;
    long m_nSelectInfo;
    CRect m_rtDlgSize;
    std::vector<std::vector<CString>> m_vecAllData;
    long m_nExcelSelectInfo;
    long m_nExcelRow;
    CExcelWrapClass m_ExcelSave;
    CDlgDefectRawSearch* m_pDlgDefectRawSearch;
    BOOL m_bInputCheck;
    std::vector<CTime> m_vecIncludeTime;
    std::vector<LotDefectInfo> m_vecLotInfo; 
    std::vector<CellDefectInfo> m_vecRawInfo;
    std::vector<std::vector<CString>> m_vecFilteredRows;
    std::vector<CString> m_vecDefectName; // Defect텍스트 
    std::vector<CString> m_vecCSVFilePath; // csvFind List Path
    long m_nEmpty;
    long m_nRawEmpty;
    long m_nRadioID;
    CFont m_ListFont;
    std::vector<CellDefectInfo> m_vecFilterRawInfo;
    std::vector<std::vector<CString>> m_vecvecInfoList;
    CString m_strSystemCSVPath;


    // function
    void DlgControlSetting();
    BOOL BetweenTimeRange(const CTime& fileTime, const CTime& startTime, const CTime& endTime);
    afx_msg void OnBnClickedButtonFind();
    virtual BOOL OnInitDialog();
    afx_msg void OnNMDblclkListCsvFindFile(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    void TotalControlResize();
    void ControlSizeChange(int nID);
    afx_msg void OnBnClickedRadioRaw();
    afx_msg void OnBnClickedRadioLot();
    void LoadCSVFile(CString strCSVPath);
    BOOL StringTokenDivideToCString(CString strFull, TCHAR chDivide, std::vector<CString>& vecstrValue);
    afx_msg void OnNMDblclkListCsvInfo(NMHDR* pNMHDR, LRESULT* pResult);
    void GetImagePath(long nSel, CString& strBrightImagePath, CString& strDarkImagePath);
    BOOL ImageLoad(CString strBrightImagePath, CString strDarkImagePath);
    void UpdateListCtrl();
    afx_msg void OnNMClickListCsvInfo(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBnClickedButtonInput();
    afx_msg void OnBnClickedButtonOutput();
    afx_msg void OnNMClickListSelectInfo(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBnClickedButtonExport();
    void ExcelRejectDataInput();
    afx_msg void OnBnClickedButtonReset();
    afx_msg void OnBnClickedButtonExcelclear();

    long LotValueCheck(LotDefectInfo Lotinfo);
    void LotTxtFileRead(CString strTxtFilePath, std::vector<CString>& vecstrValue);
    void LotSummaryInfo(CString strTxtFilePath, std::vector<LotDefectInfo>& vecstrValue);
    long RawValueCheck(CellDefectInfo Rawinfo);  
    void OverlapDelete(std::vector<CString>& vecTaget);
    afx_msg void OnBnClickedRadioNone();

    void LotSearchMode();
    void RawSearchMode();
    void GatherRecursively(LPCTSTR szDirectory, std::vector<CString>& listFiles, const CString& fileType);
    int ListRowSelect(int nID, LPNMITEMACTIVATE pNMItemActivate);
    void VectorAndListClear();

    std::vector<DefectInfo> m_vecDefectInfo;
    std::vector<std::vector<DefectInfo>> m_vecvecDefectInfo;
    std::vector<std::vector<DefectInfo>> m_vecvecFilterDefectInfo;
    std::vector<DefectFilterInfo> m_vecDefectFilterInfo;
    std::vector<DefectFilterInfo> m_vecDefectFilterInfo_;
    void InitDefectNum();
    void DefectInfoSet(CString strDefectName, DefectType dt, long nIndex);
    void DefectInfoSet(CString strDefectName, long nIndex);
    afx_msg void OnBnClickedButtonDefectcsvexport();

    BOOL CreateFileTimeCheck(SYSTEMTIME StartDate, SYSTEMTIME EndDate, SYSTEMTIME StartTime, SYSTEMTIME EndTime, SYSTEMTIME CheckTime);
    CListCtrl m_list_DefectTimeTable;
    std::vector<DefectTimetable> m_vecTimeTable;
    CButton m_chk_DefectPosAdd;
};

