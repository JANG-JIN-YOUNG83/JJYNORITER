#pragma once
#include "DlgGraph.h"
#include "../SharedModule/CommonShare/DefectDef.h"
#include <map>


// CDlgGraphFrame 대화 상자
enum class GM
{
	MODE_BASIC,   //기본 틀(Min, Max, Avg)
	MODE_MINMAX   //제목 포함(Min,Max)
};

enum FontMode
{
	GRAPH_TITLE,
	TEXT,
	BOX,
	SPEC,
};

class CDlgGraphFrame : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgGraphFrame)

public:
	CDlgGraphFrame(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgGraphFrame();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_GRAPH_FRAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnUpdateGraphSetData(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_bOK;
	bool m_resultUpDateRequest;
	HBRUSH m_Brushblock;
	HBRUSH m_Brushblue1;

public:
	CDlgGraph* m_pDlgGraph;

	CFont m_font1, m_font2, m_font3, m_font4;

	CString m_graphKey;
	std::map<DefectType, long> m_inspTypeToID;
	bool m_isOnlyMaxDiffValueShowed;

	BOOL		m_bEven = FALSE;
	DefectType	m_inspType;
	int			m_index = 0;
	float		m_value = 0.0;
	BOOL		m_bInvalidData = FALSE;
	CString		m_strJudge = _T("");

public:
	void DataSetZero();

	void SetSpec(float fOffset, float fSpecNominal, float fSapeMin, float fSpecMax, float fLCL, float fUCL, float fLSL, float fUSL, float fSpecMinAi = -9999, float fSpecMaxAi = -9999, float fSpecMinNg = -9999, float fSpecMaxNg = -9999);
	BOOL SetData(BOOL bEven, DefectType inspType, int index, float value, BOOL bInvalidData, CString strJudge);
	BOOL SetData2(BOOL bEven, DefectType inspType, int index, float value, BOOL bInvalidData, CString strJudge);

	void SetLogMode(int nMode);
	LPCTSTR GetGraphKey() const;
	void SetTextBox(BOOL Title, BOOL Min, BOOL Avg, BOOL max, BOOL spec = FALSE);
	void Initialize(const std::vector<DefectType>& inspTypes, LPCTSTR key, LPCTSTR title, bool isOnlyMaxDiffValueShowed = false);
	void SetPosition(int ID, long Left, long Top, long With, long Height);
	void SetFont(int m_nID);
	void FontChange(FontMode m_Mode, long fontSize);
	void ShowAIspec(BOOL bShow);
	bool CheckInspNameValidate(DefectType inspType);
	const std::map<DefectType, long>& GetInspTypes() const;
};
