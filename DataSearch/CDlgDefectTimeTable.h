#pragma once
#include "afxdialogex.h"
#include <vector>
class CDlgDataSearch;

// CDlgDefectTimeTable 대화 상자



class CDlgDefectTimeTable : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDefectTimeTable)

public:
	CDlgDefectTimeTable(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgDefectTimeTable();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DEFECTTIMETABLE };
#endif
    CDlgDataSearch* m_pParent;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    CListCtrl m_list_DefectTimeTable;

	void ListLoad(std::vector<CString> vecPath, std::vector<CString> vecTime);

    //std::vector<DefectTimetable> m_vecTimeTable;
};
