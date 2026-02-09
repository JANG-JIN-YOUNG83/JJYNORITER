#pragma once

// DlgDefectImageView 대화 상자
class CIBCIDlg;
class CCustomStatic;

class DlgDefectImageView : public CDialogEx
{
	DECLARE_DYNAMIC(DlgDefectImageView)

public:
	DlgDefectImageView( CWnd* pParent = nullptr);   // 표준 생성자입니다.
	void SetIndex(long nRow = 6, long nCol = 3);
	
	virtual ~DlgDefectImageView();
	void SetDlg(CIBCIDlg* pParent);
	long m_nRow; // 행 가로
	long m_nCol;// 열 세로 
	std::vector< CCustomStatic*> m_vecCustomStatic;
	std::vector<IPVM::Rect> m_vecRect;
	int m_nCount;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DEFECTIMAGE_VIEW };
#endif
	CIBCIDlg* m_parent;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	IPVM::Image_8u_C1 m_imageDefect;
	std::vector<IPVM::ImageView*> m_pVecImageView;
	CRect m_DlgSize;
	
	long m_nVecSize;

	virtual BOOL OnInitDialog();
	void UpdateUI(int nNumber, IPVM::Image_8u_C1* brightImage, IPVM::Image_8u_C1* darkImage);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void ControlSizeChange(int nID, int nNum, long nRow,long nCol);
	void SetPoint(long nType, DefectPointRect XY, CString str, int nNum);

};
