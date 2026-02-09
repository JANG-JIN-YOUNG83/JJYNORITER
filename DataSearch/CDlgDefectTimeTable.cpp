// CDlgDefectTimeTable.cpp: 구현 파일
//

#include "pch.h"
#include "DataSearch.h"
#include "CDlgDefectTimeTable.h"
#include "afxdialogex.h"
#include "DlgDataSearch.h"

// CDlgDefectTimeTable 대화 상자

IMPLEMENT_DYNAMIC(CDlgDefectTimeTable, CDialogEx)

CDlgDefectTimeTable::CDlgDefectTimeTable(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_DEFECTTIMETABLE, pParent)
{

}

CDlgDefectTimeTable::~CDlgDefectTimeTable()
{
}

void CDlgDefectTimeTable::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_DEFECTTIMETABLE, m_list_DefectTimeTable);
}


BEGIN_MESSAGE_MAP(CDlgDefectTimeTable, CDialogEx)
END_MESSAGE_MAP()


// CDlgDefectTimeTable 메시지 처리기

BOOL CDlgDefectTimeTable::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    m_list_DefectTimeTable.InsertColumn(1, _T("Time"), NULL, 100, NULL);
    m_list_DefectTimeTable.InsertColumn(2, _T("Total Num"), NULL, 150, NULL);
    m_list_DefectTimeTable.InsertColumn(3, _T("File Name"), NULL, 250, NULL);
       
    ////DefectTimetable defectTm;
    //defectTm.strStartTimeHour = _T("00");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("00");
    //defectTm.strEndTimeMin = _T("30");

    //CString strTime;

    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin, defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_list_DefectTimeTable.InsertItem(0, strTime);
    //m_vecTimeTable.push_back(defectTm);

    //defectTm.strStartTimeHour = _T("00");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("01");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(1, strTime);
    //


    //    defectTm.strStartTimeHour = _T("01");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("01");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(2, strTime);

    //    defectTm.strStartTimeHour = _T("01");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("02");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(3, strTime);

    //    defectTm.strStartTimeHour = _T("02");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("02");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(4, strTime);

    //    defectTm.strStartTimeHour = _T("02");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("03");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(5, strTime);

    //    defectTm.strStartTimeHour = _T("03");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("03");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(6, strTime);

    //    defectTm.strStartTimeHour = _T("03");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("04");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(7, strTime);

    //    defectTm.strStartTimeHour = _T("04");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("04");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(8, strTime);

    //    defectTm.strStartTimeHour = _T("04");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("05");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(9, strTime);

    //    defectTm.strStartTimeHour = _T("05");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("05");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(10, strTime);

    //    defectTm.strStartTimeHour = _T("05");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("06");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(11, strTime);

    //    defectTm.strStartTimeHour = _T("06");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("06");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(12, strTime);

    //    defectTm.strStartTimeHour = _T("06");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("07");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(13, strTime);

    //    defectTm.strStartTimeHour = _T("07");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("07");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(14, strTime);

    //    defectTm.strStartTimeHour = _T("07");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("08");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(15, strTime);

    //    defectTm.strStartTimeHour = _T("08");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("08");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(16, strTime);

    //    defectTm.strStartTimeHour = _T("08");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("09");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(17, strTime);

    //    defectTm.strStartTimeHour = _T("09");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("09");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(18, strTime);

    //    defectTm.strStartTimeHour = _T("09");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("10");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(19, strTime);

    //    defectTm.strStartTimeHour = _T("10");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("10");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(20, strTime);

    //    defectTm.strStartTimeHour = _T("10");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("11");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(21, strTime);

    //    defectTm.strStartTimeHour = _T("11");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("11");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(22, strTime);

    //    defectTm.strStartTimeHour = _T("11");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("12");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(23, strTime);

    //    defectTm.strStartTimeHour = _T("12");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("12");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(24, strTime);

    //    defectTm.strStartTimeHour = _T("12");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("13");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(25, strTime);

    //    defectTm.strStartTimeHour = _T("13");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("13");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(26, strTime);

    //    defectTm.strStartTimeHour = _T("13");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("14");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //;
    //m_list_DefectTimeTable.InsertItem(27, strTime);

    //    defectTm.strStartTimeHour = _T("14");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("14");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(28, strTime);

    // defectTm.strStartTimeHour = _T("14");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("15");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //;
    //m_list_DefectTimeTable.InsertItem(29, strTime);

    // defectTm.strStartTimeHour = _T("15");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("15");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(30, strTime);

    //defectTm.strStartTimeHour = _T("15");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("16");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(31, strTime);

    //defectTm.strStartTimeHour = _T("16");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("16");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(32, strTime);

    //defectTm.strStartTimeHour = _T("16");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("17");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(33, strTime);

    //defectTm.strStartTimeHour = _T("17");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("17");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(34, strTime);

    //defectTm.strStartTimeHour = _T("17");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("18");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(35, strTime);

    //  defectTm.strStartTimeHour = _T("18");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("18");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(36, strTime);

    //    defectTm.strStartTimeHour = _T("18");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("19");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(37, strTime);

    //defectTm.strStartTimeHour = _T("19");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("19");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(33, strTime);

    //defectTm.strStartTimeHour = _T("19");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("20");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(39, strTime);

    //defectTm.strStartTimeHour = _T("20");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("20");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(40, strTime);

    //defectTm.strStartTimeHour = _T("20");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("21");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(41, strTime);

    //defectTm.strStartTimeHour = _T("21");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("21");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(42, strTime);

    //defectTm.strStartTimeHour = _T("21");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("22");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(43, strTime);

    // defectTm.strStartTimeHour = _T("22");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("22");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(44, strTime);

    //   defectTm.strStartTimeHour = _T("22");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("23");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(45, strTime);

    //    defectTm.strStartTimeHour = _T("23");
    //defectTm.strStartTimeMin = _T("00");
    //defectTm.strEndTimeHour = _T("23");
    //defectTm.strEndTimeMin = _T("30");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(46, strTime);

    //    defectTm.strStartTimeHour = _T("23");
    //defectTm.strStartTimeMin = _T("30");
    //defectTm.strEndTimeHour = _T("00");
    //defectTm.strEndTimeMin = _T("00");
    //strTime.Format(_T("%s : %s ~ %s : %s"), defectTm.strStartTimeHour, defectTm.strStartTimeMin,
    //    defectTm.strEndTimeHour, defectTm.strEndTimeMin);
    //m_vecTimeTable.push_back(defectTm);
    //m_list_DefectTimeTable.InsertItem(47, strTime);

    
    return TRUE; // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgDefectTimeTable::ListLoad(std::vector<CString> vecPath, std::vector<CString> vecTime)
{
    /*long nTotal = 0;

    for (long nIndex = 0; nIndex < vecTime.size(); nIndex++)
    {
        CString strTime;
        CString strHour = vecTime[nIndex].Mid(8, 2);
        CString strMinute = vecTime[nIndex].Mid(8, 2);

        CTime time = CTime::GetCurrentTime();
        time.GetYear();
        CTime tmCheck(time.GetYear(), time.GetMonth(), time.GetDay(), _ttoi(strHour), _ttoi(strMinute), 0, 0);

        for (long nCheck = 0; nCheck < m_vecTimeTable.size(); nCheck++)
        {
            CString strStartHour = m_vecTimeTable[nCheck].strStartTimeHour;
            CString strStartMin = m_vecTimeTable[nCheck].strStartTimeMin;
            CString strEndHour = m_vecTimeTable[nCheck].strEndTimeHour;
            CString strEndMin = m_vecTimeTable[nCheck].strEndTimeMin;

            CTime tmStart(
                time.GetYear(), time.GetMonth(), time.GetDay(), _ttoi(strStartHour), _ttoi(strStartMin), 0, 0);

            CTime tmEnd(time.GetYear(), time.GetMonth(), time.GetDay(), _ttoi(strEndHour), _ttoi(strEndMin), 0, 0);

            if ((tmCheck > tmStart) && (tmCheck < tmEnd))
            {
                m_vecTimeTable[nCheck].nNum++;
            }
        }

    }*/

    //for (long nList = 0; nList < m_vecTimeTable.size(); nList++)
    //{
    //    CString str;
    //    str.Format(_T("%d"), m_vecTimeTable[nList]);
    //    m_list_DefectTimeTable.SetItemText(0, 0, str);
    //}
}