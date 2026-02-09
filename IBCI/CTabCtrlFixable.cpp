// CTabCtrlFixable.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "CTabCtrlFixable.h"


// CTabCtrlFixable

IMPLEMENT_DYNAMIC(CTabCtrlFixable, CTabCtrl)

CTabCtrlFixable::CTabCtrlFixable()
{

}

CTabCtrlFixable::~CTabCtrlFixable()
{
}


BEGIN_MESSAGE_MAP(CTabCtrlFixable, CTabCtrl)
ON_WM_HSCROLL()
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CTabCtrlFixable 메시지 처리기




void CTabCtrlFixable::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

    CTabCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
    Invalidate();
}

void CTabCtrlFixable::OnPaint()
{
    CPaintDC dc(this);

    CRect clientRect;
    GetClientRect(&clientRect);

    DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, 0);

    if (GetItemCount() > 0)
    {
        TCITEM tci = {0};
        TCHAR szText[256] = {0};
        tci.mask = TCIF_TEXT;
        tci.pszText = szText;
        tci.cchTextMax = sizeof(szText) / sizeof(TCHAR);

        GetItem(0, &tci);

        CRect fixedTabRect;
        GetItemRect(0, &fixedTabRect);

        fixedTabRect.OffsetRect(-fixedTabRect.left, 0);

        dc.FillSolidRect(fixedTabRect, GetSysColor(COLOR_BTNFACE));

        dc.DrawText(tci.pszText, &fixedTabRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

void CTabCtrlFixable::OnLButtonDown(UINT nFlags, CPoint point)
{
    CRect fixedTabRect(0, 0, 78, 20);

    if (fixedTabRect.PtInRect(point))
    {
        SetCurSel(0);

        NMHDR nmhdr = {0};
        nmhdr.hwndFrom = m_hWnd;
        nmhdr.idFrom = GetDlgCtrlID();
        nmhdr.code = TCN_SELCHANGE;
        GetParent()->SendMessage(WM_NOTIFY, nmhdr.idFrom, (LPARAM)&nmhdr);

        return;
    }
    CTabCtrl::OnLButtonDown(nFlags, point);
}
