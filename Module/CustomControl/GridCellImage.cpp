#include "pch.h"
#include "GridCell.h"
#include "GridCtrl.h"

#include "GridCellImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CGridCellImage, CGridCell)

CGridCellImage::CGridCellImage() : CGridCell()
{
    //m_bChecked = FALSE;
    //m_Rect.IsRectNull();
}
// 
// CSize CGridCellImage::GetCellExtent(CDC* pDC)
// {
//     // Using SM_CXHSCROLL as a guide to the size of the checkbox
// 	int nWidth = GetSystemMetrics(SM_CXHSCROLL) + 2*GetMargin();
// 	return CGridCell::GetCellExtent(pDC) + CSize(nWidth, nWidth);
// }
// 
// // i/o:  i=dims of cell rect; o=dims of text rect
// BOOL CGridCellImage::GetTextRect( LPRECT pRect)
// {
//     BOOL bResult = CGridCell::GetTextRect(pRect);
//     if (bResult)
//     {
//         int nWidth = GetSystemMetrics(SM_CXHSCROLL) + 2*GetMargin();
//         pRect->left += nWidth;
//         if (pRect->left > pRect->right)
//             pRect->left = pRect->right;
//     }
//     return bResult;
// }

// Override draw so that when the cell is selected, a drop arrow is shown in the RHS.
BOOL CGridCellImage::Draw(CDC* pDC, int nRow, int nCol, CRect rect,  BOOL bEraseBkgnd /*=TRUE*/)
{
    BOOL bResult = CGridCell::Draw(pDC, nRow, nCol, rect,  bEraseBkgnd);

#ifndef _WIN32_WCE
    if(m_image)
	{
		pDC->SetStretchBltMode(HALFTONE); //이미지 확대 축소시 깨짐방지
		m_image.StretchBlt(pDC->GetSafeHdc(), rect, SRCCOPY); //이미지를 클라이언트 크기에 맞도록 화면에 출력한다.
	}
#endif

    return bResult;
}
void CGridCellImage::SetCellImage(LPCTSTR szImagePathName)
{
	// 이미지가 없거나, 이미지 이름이 달라진 경우에 새로 읽어들인다.
	if(m_image == NULL || m_strImagePathName != szImagePathName)
	{
		if(m_image)
		{
			m_image.Destroy();
		}

		//이미지 경로를 넣어 이미지를 불러온다 (BMP, JPG 모두 가능)
		if(S_OK == m_image.Load(szImagePathName))
		{
			m_strImagePathName = szImagePathName;
		}
		else
		{
			m_strImagePathName.Empty();
		}
	}
}