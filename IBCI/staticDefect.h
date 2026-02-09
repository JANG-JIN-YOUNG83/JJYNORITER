#pragma once
//#include "Common.h"
#include <vector>
#include <memory> 
#include "GraphCustomizeBase.h"

class staticDefect : public CStatic
{
	DECLARE_DYNAMIC(staticDefect)

public:
	staticDefect();
	virtual ~staticDefect();

	void SetPoint(long x, long y, COLORREF Color);

	void InitStatic();
	std::shared_ptr<GraphCustomizeBase> m_graphStyle;
protected:
	DECLARE_MESSAGE_MAP()

	// 그래프로 하려고 추가
	CDC m_MemDC;
	CBitmap m_MemBitmap;
	CBitmap* m_pOldMemBitmap;

	CDC* m_memDC;
	

	void Ready();
	void End();

private:
	bool	m_bFirst;
	bool	m_bClear;

public:
	afx_msg void OnPaint();
};

