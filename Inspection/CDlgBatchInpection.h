#pragma once

#include "resource.h"
#include <iostream>
#include <sstream>
#include <string>
#include "../IBCI/ThreadControl.h"
#include "InspectionCell.h"

// CDlgBatchInpection 대화 상자
class CDlgInspectionMain;

class CDlgBatchInpection : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgBatchInpection)

public:
	CDlgBatchInpection(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgBatchInpection();
    void SetDlg(CDlgInspectionMain* pParent);
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_BATCH_INSPECTIONMODE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedButtonAdd();
    CListCtrl m_listBatchCtrl;
    std::vector<CString> m_multiInlinePath;
    afx_msg void OnBnClickedButtonClear();
    long m_rowNumber;
    long m_cellID;
    long m_index;

    void GetValue(std::vector<long>& vecCellID);
    afx_msg void OnBnClickedButtonStart();
    afx_msg void OnBnClickedButtonStop();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    CDlgInspectionMain* m_parent;

virtual BOOL OnInitDialog();
    CButton m_chk_ExcelSave;
afx_msg void OnNMDblclkListBatch(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBnClickedButtonNext();

    long m_nSelect;
    afx_msg void OnBnClickedButtonPrevious();
    afx_msg void OnBnClickedButtonReset();
    afx_msg void OnBnClickedButtonInspection();
    afx_msg void OnBnClickedButtonExcelsavepath();
    afx_msg void OnNMCustomdrawListBatch(NMHDR* pNMHDR, LRESULT* pResult);

    void UpadateList();
    CEdit m_editGrapSpeed;
    float m_fGrabSpeed;
    void GetGrapSpeed();

    void ThreadStart_BatchInspection();
    void ThreadStop_BatchInspection();
    
    std::vector<ThreadControl> m_threadControl_BatchInspection;
    std::vector<CInspectionCell> m_batchInspectionCellGroup;
    //ThreadControl m_threadControl_BatchInspection;
    static UINT ThreadFunc_BatchInspection(LPVOID pParam);
    UINT ThreadFunc_BatchInspection();

    ThreadControl m_threadControl_BatchSimulation;
    static UINT ThreadFunc_BatchSimulation(LPVOID pParam);
    UINT ThreadFunc_BatchSimulation();

    
    CCriticalSection m_csBatch;
    HANDLE m_threadWaitInspEnd;
    BOOL m_stopBatchInsp;

    long m_nTheadReset;
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    CButton m_chk_DefectPosAdd;
    void ControlEnable(BOOL bEnable);
};
