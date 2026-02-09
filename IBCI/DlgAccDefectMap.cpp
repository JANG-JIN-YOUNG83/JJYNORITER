// DlgAccDefectMap.cpp: 구현 파일
#include "pch.h"
#include "DlgAccDefectMap.h"

#include "afxdialogex.h"
#include "ColorDefine.h"
//#include "Log.h"
#include "IBCIDlg.h"

#include "Widget/ImageView.h"
#include "Types/Image_8u_C1.h"
#include "Types/Image_8u_C4.h"
#include "Types/Point_32f_C2.h"
#include "Types/LineSeg_32f.h"
#include "Types/Rect.h"
#include "Algorithm/ImageProcessing.h"
#include "basedef.h"

#include "../Module/Utility/SystemFont.h"
#include "resource.h"

// CDlgAccDefectMap 대화 상자
#define VISION_RAW_IMAGE_SIZE_X	36288	// 비전 원본 이미지 X축 사이즈 (코터)
#define VISION_RAW_IMAGE_SIZE_Y	4230	// 비전 원본 이미지 Y축 사이즈 (코터)

IMPLEMENT_DYNAMIC(CDlgAccDefectMap, CDialogEx)

CDlgAccDefectMap::CDlgAccDefectMap(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CDlgAccDefectMap::IDD, pParent)
	, m_pImageViewTop(nullptr)
	, m_pImageViewBtm(nullptr)
	, m_dImageSizeX(VISION_RAW_IMAGE_SIZE_X)
	, m_dImageSizeY(VISION_RAW_IMAGE_SIZE_Y)
{
	m_pParent = (CIBCIDlg*)pParent;
}

CDlgAccDefectMap::~CDlgAccDefectMap()
{
	if (m_pImageViewTop != NULL)
	{
		delete m_pImageViewTop;
		m_pImageViewTop = NULL;
	}

	if (m_pImageViewBtm != NULL)
	{
		delete m_pImageViewBtm;
		m_pImageViewBtm = NULL;
	}

	m_font.DeleteObject();
	m_font_Bold.DeleteObject();

	m_Brush.DeleteObject();
	m_Brush_Title.DeleteObject();
}

void CDlgAccDefectMap::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgAccDefectMap, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_DEFECT_CHANGE, &CDlgAccDefectMap::OnBnClickedButtonDefectChange)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CDlgAccDefectMap::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 0으로 초기화
	for (int i = 0; i < 6; ++i) 
	{
		for (int j = 0; j < 20; ++j) 
		{
			m_nCountTop[i][j] = 0;
			m_nCountBtm[i][j] = 0;
		}
	}

	int value = 1;
	// Flag
	for (int i = 0; i < 6; ++i) 
	{
		for (int j = 0; j < 20; ++j) 
		{
			m_nFlaTop[i][j] = value;
			m_nFlaBtm[i][j] = value++;
		}
	}

	CALL_Function_CSC();

	m_Brush.CreateSolidBrush(COLOR_GRAY2);
	m_Brush_Title.CreateSolidBrush(COLOR_BLACK);

	// STATIC
	Utility::SystemFont::CreateGUIFont(m_font, 100);	
	GetDlgItem(IDC_STATIC_DEFECT_TOP)->SetFont(&m_font);
	GetDlgItem(IDC_STATIC_DEFECT_TOP_TITLE)->SetFont(&m_font);
	GetDlgItem(IDC_STATIC_DEFECT_BTM)->SetFont(&m_font);
	GetDlgItem(IDC_STATIC_DEFECT_BOTTOM_TITLE)->SetFont(&m_font);

	Utility::SystemFont::CreateGUIFont_Bold(m_font_Bold, 100);
	GetDlgItem(IDC_BUTTON_DEFECT_CHANGE)->SetFont(&m_font_Bold);

	TopUpdateUI();
	BtmUpdateUI();


	return TRUE;
}
// CDlgAccDefectMap 메시지 처리기

BOOL CDlgAccDefectMap::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN) // ENTER키 눌릴 시
			return TRUE;
		else if (pMsg->wParam == VK_ESCAPE) // ESC키 눌릴 시
			return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
//
//void CDlgAccDefectMap::SetPoint(Share::ResultVision& data, HostData::Result_Cell* VD_BOX)
//{
//	if (data.m_strCell_Final_Judge != _T("NG"))
//	{
//		return;
//	}
//
//	for (long i = 0; i < data.m_vecCamera.size(); i++)
//	{
//		if (data.m_vecCamera[i].m_nScreen_Image_Size_X != m_dImageSizeX || data.m_vecCamera[i].m_nScreen_Image_Size_Y != m_dImageSizeY)
//		{
//			m_dImageSizeX = data.m_vecCamera[i].m_nScreen_Image_Size_X;
//			m_dImageSizeY = data.m_vecCamera[i].m_nScreen_Image_Size_Y;
//
//			break;
//		}
//	}
//
//	DefectPointXY XY;
//
//	for (long i = 0; i < data.m_vecDefect.size(); i++)
//	{
//		for (long d = 0; d < data.m_vecDefect[i].m_vecstDefectROI.size(); d++)
//		{
//			//XY.nX = (long)((double)((data.m_vecDefect[i].m_dCriteria_Point_X) + (data.m_vecDefect[i].m_vecstDefectROI[d].m_dRelative_Img_X)));
//			XY.nX = (long)(data.m_vecDefect[i].m_vecstDefectROI[d].m_dRelative_Img_X);
//			XY.nY = (long)(data.m_vecDefect[i].m_vecstDefectROI[d].m_dRelative_Img_Y);
//
//			//if (data.m_vecDefect[i].m_inspType == DefectType::TopFoilExposure || data.m_vecDefect[i].m_inspType == DefectType::BottomFoilExposure)
//			//{
//			//	XY.Color = COLOR_RED;
//			//}
//			//else if (data.m_vecDefect[i].m_inspType == DefectType::TopSurface || data.m_vecDefect[i].m_inspType == DefectType::BottomSurface)
//			//{
//			//	XY.Color = COLOR_YELLOW;
//			//}
//			//else if (data.m_vecDefect[i].m_inspType == DefectType::TopDross || data.m_vecDefect[i].m_inspType == DefectType::BottomDross)
//			//{
//			//	XY.Color = COLOR_BLUE;
//			//}
//			//else
//			{
//				XY.Color = COLOR_RED;	// 20230524 LJH Color 쓰레기 값이 들어와 추가
//			}
//
//			SetPoint(data.m_vecDefect[i].m_nCamera_Position, XY);
//		}
//	}
//}

void CDlgAccDefectMap::SetPoint(long nType, DefectPointRect XY)
{
	int nNGCount = SetCount(nType, XY);

	if (nType == 1)
	{
		m_pImageViewBtm->ImageOverlayAdd(IPVM::Point_32f_C2(XY.nX, XY.nY), XY.Color, 5.0);			// Point
		m_pImageViewBtm->ImageOverlayShow();
	}
	else if (nType == 0)
	{
		m_pImageViewTop->ImageOverlayAdd(IPVM::Point_32f_C2(XY.nX, XY.nY), XY.Color, 5.0);			// Point
		m_pImageViewTop->ImageOverlayShow();
	}
}

void CDlgAccDefectMap::DataCountSetZero()
{
	// 임시 0으로 초기화
	for (int i = 0; i < 6; ++i) 
	{
		for (int j = 0; j < 20; ++j) 
		{
			m_nCountTop[i][j] = 0;
			m_nCountBtm[i][j] = 0;
		}
	}

	SetCountClear();

	TopUpdateUI();
	BtmUpdateUI();
}

void CDlgAccDefectMap::CALL_Function_CSC()
{
	// Static
	ControlSizeChange(IDC_STATIC_DEFECT_TOP_TITLE);
	ControlSizeChange(IDC_STATIC_DEFECT_BOTTOM_TITLE);

	ControlSizeChange(IDC_STATIC_DEFECT_TOP);
	ControlSizeChange(IDC_STATIC_DEFECT_BTM);

	// Btn
	ControlSizeChange(IDC_BUTTON_DEFECT_CHANGE);
}

void CDlgAccDefectMap::ControlSizeChange(int nID)
{
	CWnd* pCtl;

	long	nStartPointX = m_DlgSize.left;
	long	nStartPointY = m_DlgSize.top;

	long	nStaticSizeX = m_DlgSize.Width() * 0.95;
	long	nTitleSizeX = m_DlgSize.Width() * 0.05;

	long	nBtnSizeX = m_DlgSize.Width();
	long	nBtnSizeY = m_DlgSize.Height() * 0.05;

	long	nStaticSizeY = (m_DlgSize.Height() - nBtnSizeY) / 2;
	long	nTitleSizeY = nStaticSizeY;

	// Btn 
	pCtl = GetDlgItem(IDC_BUTTON_DEFECT_CHANGE);
	if (pCtl != NULL)
	{
		pCtl->MoveWindow(nStartPointX, nStartPointY, nBtnSizeX, nBtnSizeY);
	}

	// 분포도 (Static)
	pCtl = GetDlgItem(IDC_STATIC_DEFECT_TOP_TITLE);
	if (pCtl != NULL)
	{
		pCtl->MoveWindow(nStartPointX, nStartPointY + nBtnSizeY, nTitleSizeX, nTitleSizeY);
	}

	pCtl = GetDlgItem(IDC_STATIC_DEFECT_BOTTOM_TITLE);
	if (pCtl != NULL)
	{
		pCtl->MoveWindow(nStartPointX, nStartPointY + nBtnSizeY + nTitleSizeY, nTitleSizeX, nTitleSizeY);
	}

	pCtl = GetDlgItem(IDC_STATIC_DEFECT_TOP);
	if (pCtl != NULL)
	{
		pCtl->MoveWindow(nStartPointX + nTitleSizeX, nStartPointY + nBtnSizeY, nStaticSizeX, nStaticSizeY);
	
		m_MapSize.left = nStartPointX + nTitleSizeX;
		m_MapSize.top = nStartPointY + nBtnSizeY;
		m_MapSize.right = nStaticSizeX;
		m_MapSize.bottom = nStaticSizeY - 2;

		if (nullptr == m_pImageViewTop)
		{
			IPVM::Rect rect1(m_MapSize.left, m_MapSize.top, m_MapSize.right, m_MapSize.bottom);
			m_pImageViewTop = new IPVM::ImageView(this->GetSafeHwnd(), rect1, (long)IDC_STATIC_DEFECT_TOP);
		}

		::MoveWindow(m_pImageViewTop->GetSafeHwnd(), m_MapSize.left, m_MapSize.top, m_MapSize.right, m_MapSize.bottom, TRUE);
	}

	pCtl = GetDlgItem(IDC_STATIC_DEFECT_BTM);
	if (pCtl != NULL)
	{
		pCtl->MoveWindow(nStartPointX + nTitleSizeX, nStartPointY + nBtnSizeY + nStaticSizeY, nStaticSizeX, nStaticSizeY);
	
		m_MapSize.left = nStartPointX + nTitleSizeX;
		m_MapSize.top = nStartPointY + nBtnSizeY + nStaticSizeY + 2;
		m_MapSize.right = nStaticSizeX;
		m_MapSize.bottom = nStaticSizeY - 2;

		if (nullptr == m_pImageViewBtm)
		{
			IPVM::Rect rect1(m_MapSize.left, m_MapSize.top, m_MapSize.right, m_MapSize.bottom);
			m_pImageViewBtm = new IPVM::ImageView(this->GetSafeHwnd(), rect1, (long)IDC_STATIC_DEFECT_BTM);
		}

		::MoveWindow(m_pImageViewBtm->GetSafeHwnd(), m_MapSize.left, m_MapSize.top, m_MapSize.right, m_MapSize.bottom, TRUE);
	}
}

void CDlgAccDefectMap::TopUpdateUI()
{
	if (CheckTopImage())
	{
		m_pImageViewTop->ImageOverlayClear();
		//CString strOriginImageTop = g_systemParameter.m_recipeOption.strRecipeFolder + _T("\\") + g_stOperatorInfo.strReceipeName + _T("_TOP.jpg");

		CString strOriginImageTop = _T("C:\\intekplus\\CoatorInterface\\ReceipeList\\E78_TOP.bmp");

		imageTop.LoadFrom(strOriginImageTop);

		m_pImageViewTop->SetImage(imageTop);
		m_pImageViewTop->ImageOverlayShow();
	}
}

void CDlgAccDefectMap::BtmUpdateUI()
{
	if (CheckBtmImage())
	{
		m_pImageViewBtm->ImageOverlayClear();

		//CString strOriginImageBtm = g_systemParameter.m_recipeOption.strRecipeFolder + _T("\\") + g_stOperatorInfo.strReceipeName + _T("_BTM.jpg");
		CString strOriginImageBtm = _T("C:\\intekplus\\CoatorInterface\\ReceipeList\\E78_BTM.bmp");

		imageBtm.LoadFrom(strOriginImageBtm);

		m_pImageViewBtm->SetImage(imageBtm);
		m_pImageViewBtm->ImageOverlayShow();
	}
}

BOOL CDlgAccDefectMap::CheckTopImage()
{
	CFile file;
	//CString strOriginImageTop = g_systemParameter.m_recipeOption.strRecipeFolder + _T("\\") + g_stOperatorInfo.strReceipeName + _T("_TOP.jpg");
	CString strOriginImageTop = _T("C:\\intekplus\\CoatorInterface\\ReceipeList\\E78_TOP.bmp");

	if (!file.Open(strOriginImageTop, CFile::modeRead)) return FALSE;	// 파일 못읽음
	file.Close();

	return TRUE;
}

BOOL CDlgAccDefectMap::CheckBtmImage()
{
	CFile file;
	//CString strOriginImageBtm = g_systemParameter.m_recipeOption.strRecipeFolder + _T("\\") + g_stOperatorInfo.strReceipeName + _T("_BTM.jpg");
	
	CString strOriginImageBtm = _T("C:\\intekplus\\CoatorInterface\\ReceipeList\\E78_BTM.bmp");

	if (!file.Open(strOriginImageBtm, CFile::modeRead)) return FALSE;	// 파일 못읽음
	file.Close();

	return TRUE;
}

int CDlgAccDefectMap::SetCount(long nType, DefectPointRect XY)
{
	////////// 영역에 대한 Count 올리기 /////////////
	int nNumColumns = 20;									// 임시 20개
	int nNumRows = 6;										// 임시 6개


	// 이미지에서 각 영역의 너비와 높이 계산
	int nAreaWidth = m_dImageSizeX / nNumColumns;
	int nAreaHeight = m_dImageSizeY / nNumRows;

	// 점이 속한 영역 계산
	int nColumnIdx = XY.nX / nAreaWidth;
	int nRowIdx = XY.nY / nAreaHeight;

	CString strTopNgCount = _T("");
	CString stBtmNgCount = _T("");
	
	CSingleLock Lock(&m_CS, true);

	// 라인 그리기.
	long nsX = 0, nsY = 0, neX = m_dImageSizeX, neY = 0;			// Line 필요 계수
	for (int i = 0; i < 7; i++)
	{
		IPVM::LineSeg_32f LineX(nsX, nsY, neX, neY);
		m_pImageViewTop->ImageOverlayAdd(LineX, RGB(0, 255, 0));
		m_pImageViewBtm->ImageOverlayAdd(LineX, RGB(0, 255, 0));

		// 다음 라인을 위해 좌표 업데이트
		nsY += nAreaHeight;
		neY += nAreaHeight;
	}

	nsX = 0; nsY = 0; neX = 0; neY = m_dImageSizeY;				// Line 필요 계수
	for (int i = 0; i < 21; i++)
	{
		IPVM::LineSeg_32f LineY(nsX, nsY, neX, neY);
		m_pImageViewTop->ImageOverlayAdd(LineY, RGB(0, 255, 0));
		m_pImageViewBtm->ImageOverlayAdd(LineY, RGB(0, 255, 0));

		// 다음 라인을 위해 좌표 업데이트
		nsX += nAreaWidth;
		neX += nAreaWidth;
	}

	if (0 == nType)
	{
		m_nCountTop[nRowIdx][nColumnIdx]++;
	
		strTopNgCount.Format(_T("%d"), m_nCountTop[nRowIdx][nColumnIdx]);
	
		m_pImageViewTop->ImageOverlayClear(m_nFlaTop[nRowIdx][nColumnIdx]);
	
		m_pImageViewTop->ImageOverlayAdd(m_nFlaTop[nRowIdx][nColumnIdx], IPVM::Point_32f_C2(nColumnIdx * nAreaWidth, nRowIdx * nAreaHeight), strTopNgCount, RGB(255, 0, 0), 80);
	
		m_pImageViewTop->ImageOverlayShow(m_nFlaTop[nRowIdx][nColumnIdx]);
	
		return m_nCountTop[nRowIdx][nColumnIdx];
	}
	else
	{
		m_nCountBtm[nRowIdx][nColumnIdx]++;
	
		stBtmNgCount.Format(_T("%d"), m_nCountBtm[nRowIdx][nColumnIdx]);
	
		m_pImageViewBtm->ImageOverlayClear(m_nFlaBtm[nRowIdx][nColumnIdx]);
	
		m_pImageViewBtm->ImageOverlayAdd(m_nFlaBtm[nRowIdx][nColumnIdx], IPVM::Point_32f_C2(nColumnIdx * nAreaWidth, nRowIdx * nAreaHeight), stBtmNgCount, RGB(255, 0, 0), 80);
	
		m_pImageViewBtm->ImageOverlayShow(m_nFlaBtm[nRowIdx][nColumnIdx]);
	
		return m_nCountBtm[nRowIdx][nColumnIdx];
	}

	Lock.Unlock();
}

void CDlgAccDefectMap::OnBnClickedButtonDefectChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ShowWindow(SW_HIDE);

	//m_pParent->m_pDlgDefectMap->ShowWindow(SW_SHOW);
	
}

void CDlgAccDefectMap::SetCountClear()
{
	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 20; ++j) 
		{
			m_pImageViewTop->ImageOverlayClear(m_nFlaTop[i][j]);
			m_pImageViewTop->ImageOverlayShow(m_nFlaTop[i][j]);

			m_pImageViewBtm->ImageOverlayClear(m_nFlaBtm[i][j]);
			m_pImageViewBtm->ImageOverlayShow(m_nFlaBtm[i][j]);
		}
	}
}

void CDlgAccDefectMap::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	GetClientRect(m_DlgSize); 
	CALL_Function_CSC();
}


HBRUSH CDlgAccDefectMap::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_DEFECT_TOP_TITLE || pWnd->GetDlgCtrlID() == IDC_STATIC_DEFECT_BOTTOM_TITLE)
	{
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkColor(COLOR_BLACK);
		pDC->SetBkMode(TRANSPARENT);
		return m_Brush_Title;
	}

	if (pWnd->GetDlgCtrlID() == IDC_BUTTON_DEFECT_CHANGE)
	{
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkColor(COLOR_GRAY2);
		pDC->SetBkMode(TRANSPARENT);
		return m_Brush;
	}

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}
