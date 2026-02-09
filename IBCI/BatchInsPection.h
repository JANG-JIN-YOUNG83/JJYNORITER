#pragma once
#include "ThreadControl.h"
#include <vector>
#include "../Inspection/InspectionCell.h"
#include "../Module/Utility/Logger.h"

// CBatchInsPection 대화 상자
class CIBCIDlg;
class CDlgDefectMap;
class CDlgInspectionMain;

typedef struct _ThreadInspectInfo
{
    long m_nCellID;
    IPVM::Image_8u_C1 Brightimg;
    IPVM::Image_8u_C1 Darkimg;
    CListCtrl* listCtrl;
    CIBCIDlg* m_pParent;

} ThreadInspInfo;

class CBatchInsPection : public CDialogEx
{
	DECLARE_DYNAMIC(CBatchInsPection)

public:
	CBatchInsPection(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CBatchInsPection();
	void SetDlg(CIBCIDlg* pParent);
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_BATCH_INSPECTION };
#endif
	CIBCIDlg* m_parent;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listBatchCtrl;
	long m_mcidStart;
	long m_mcidEnd;
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickListBatch(NMHDR* pNMHDR, LRESULT* pResult);
	long m_rowNumber;
	long m_cellID;
	long m_index;
	std::vector<CString> m_multiInlinePath;
	void GetValue(std::vector<long>& vecCellID);

	afx_msg void OnBnClickedButtonClear();
	CButton m_buttonStop;
	afx_msg void OnBnClickedButtonMakeCellid();
	afx_msg void OnBnClickedButtonReset();
    afx_msg void OnDblclkListBatch(NMHDR* pNMHDR, LRESULT* pResult);
    long m_nSelect;

	afx_msg void OnClickedButtonNext();
    afx_msg void OnClickedButtonPrev();
    afx_msg void OnClickedButtonInsp();

	//std::vector<CInspectionCell> m_batchInspectionCellGroup;
    HANDLE m_threadWaitInspEnd;

    float m_fGrabSpeed;

    void ImageLoad();
    std::vector<IPVM::Image_8u_C1> m_vecImageBright;
    std::vector<IPVM::Image_8u_C1> m_vecImageDark;

    std::vector<CInspectionCell> m_vecInspectionCell;
    std::vector<ThreadControl> m_vecThreadControl;

    static UINT ThreadFunc_BatchInspection(LPVOID pParam);
    UINT ThreadFunc_BatchInspection();
    std::vector<Share::ResultVision> m_vecResultVision;
    afx_msg void OnBnClickedButtonHostMessageSend();
    long m_nLoopCnt;
    long m_nResultCheckCnt;

    Utility::Logger m_loggerSequence;
    Utility::Logger m_loggerResult;
    void ControlEnable(BOOL bEnable);
};
