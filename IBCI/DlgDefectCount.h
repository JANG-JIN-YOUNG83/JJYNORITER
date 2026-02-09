#pragma once


// CDlgDefectCount 대화 상자
class CCustomStatic;
class DefectCount 
{
public:
	CString m_strName;
	long m_nTopNum;
	long m_nBottomNum;
	long m_nSumNum;

	double m_fTopValue;
    double m_fBottomValue;
    double m_fMismatchValue;

    bool m_bMismatch;

	DefectCount::DefectCount(CString strName, long nTopNum, long nBottomNum)
	{
		m_strName = strName;
		m_nTopNum = nTopNum;
		m_nBottomNum = nBottomNum;
		m_nSumNum = nTopNum+ nBottomNum;


	}

	DefectCount::DefectCount(CString strName, double nTopNum, double nBottomNum, double fMismatchNum, bool bMismatch)
    {
        m_strName = strName;
        m_fTopValue = nTopNum;
        m_fBottomValue = nBottomNum;
        m_fMismatchValue = fMismatchNum;
        m_bMismatch = bMismatch;
    }


	DefectCount& DefectCount::operator = (const DefectCount& Object)
	{
		m_strName = Object.m_strName;
		m_nTopNum = Object.m_nTopNum;
		m_nBottomNum = Object.m_nBottomNum;
		m_nSumNum = Object.m_nSumNum;

        m_fTopValue = Object.m_fTopValue;
        m_fBottomValue = Object.m_fBottomValue;
        m_fMismatchValue = Object.m_fMismatchValue;

		return *this;
	}
};

class CDlgDefectCount : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDefectCount)

public:
	CDlgDefectCount(long nRow = 3,long nCol = 3,CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgDefectCount();
    std::vector<CCustomStatic*> m_vecCustomTitle;
	std::vector< CCustomStatic*> m_vecCustomStatic;
    std::vector<CCustomStatic*> m_vecCustiomStaticMismatch;
    std::vector<DefectCount> m_vecMismatchData;
	long m_nRow; // 행 가로
	long m_nCol;// 열 세로 
	CRect m_rtDlgSize;
	void SetUI();
	BOOL m_bInit;
	std::vector< DefectCount> m_vecData;
	void DefectDraw();
    void WarningSpecCountBackColor(CCustomStatic* customStatic, int nValue);
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DEFECTCOUNT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
    void SetZero();
    void DefectMismatchDraw();
};
