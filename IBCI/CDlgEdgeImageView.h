#pragma once


// CDlgEdgeImageView 대화 상자
class CIBCIDlg;
class CCustomStatic;

class CDlgEdgeImageView : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgEdgeImageView)

public:
    CDlgEdgeImageView(long nRow = 4, long nCol = 1, CWnd* pParent = nullptr); // 표준 생성자입니다.
	virtual ~CDlgEdgeImageView();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_EDGEIMAGE_VIEW };
#endif
    CIBCIDlg* m_parent;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    void SetDlg(CIBCIDlg* pParent);
    long m_nRow; // 행 가로
    long m_nCol; // 열 세로
    void SetIndex(long nRow = 4, long nCol = 1);

    CRect m_DlgSize;
    int m_nCount;
    long m_indexUpdate;

    long m_nVecSize;
    std::vector<IPVM::ImageView*> m_pVecImageView;
    void ControlSizeChange(int nID, int nNum, long nRow, long nCol);
    void UpdateUI(std::vector<IPVM::Image_8u_C1> EdgeImages);
    void UpdateUI(std::vector<IPVM::Image_8u_C1> EdgeImages, std::vector<IPVM::LineSeg_32f> lineFinds, std::vector<CString> vecTexts);
    void ModifyLaneCount(long count);
};
