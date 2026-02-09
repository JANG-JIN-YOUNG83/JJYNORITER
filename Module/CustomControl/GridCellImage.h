
#if !defined(AFX_GRIDCELLIMAGE_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_)
#define AFX_GRIDCELLIMAGE_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000



#ifdef DPI_GRIDCTRLDEF_EXPORTS
#define DPI_GRIDCTRLDEF_API __declspec(dllexport)
#else
#define DPI_GRIDCTRLDEF_API __declspec(dllimport)
#endif
#include "atlimage.h"
#include "GridCell.h"

// #pragma onces
// #include "gridcell.h"
// 
// class  :
// 	public CGridCell
// {
// public:
// 	CGridCellImage(void);
// 	~CGridCellImage(void);
// };
class __CUSTOMCONTROL_CLASS__ CGridCellImage : public CGridCell
{
	friend class CGridCtrl;
	DECLARE_DYNCREATE(CGridCellImage)

public:
	CGridCellImage();

public:
// 	BOOL SetCheck(BOOL bChecked = TRUE);
// 	BOOL GetCheck();
// 
// 	// Operations
// 	virtual CSize GetCellExtent(CDC* pDC);
// 	virtual void OnClick( CPoint PointCellRelative);
// 	virtual BOOL GetTextRect( LPRECT pRect);
// 	CRect GetCheckPlacement();
	void SetCellImage(LPCTSTR szImagePathName);
protected:
	
	CImage m_image;
	CString m_strImagePathName;

	virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);


	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDCELLCHECK_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_)
