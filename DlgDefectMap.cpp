// DlgDefectMap.cpp: 구현 파일
#include "pch.h"
#include "Host_Interface.h"
//#include "DlgDefectMap.h"
#include "afxdialogex.h"
#include "ColorDefine.h"

#include "DlgAccDefectMap.h"

#include "../IBCI/Common/Utility/SystemFont.h"
// CDlgDefectMap 대화 상자

#define VISION_RAW_IMAGE_SIZE_X	36288	// 비전 원본 이미지 X축 사이즈 (코터)
#define VISION_RAW_IMAGE_SIZE_Y	4230	// 비전 원본 이미지 Y축 사이즈 (코터)

IMPLEMENT_DYNAMIC(CDlgDefectMap, CDialogEx)

CDlgDefectMap::CDlgDefectMap(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CDlgDefectMap::IDD, pParent)
	, m_nDefectMapSizeX(0)
	, m_nDefectMapSizeY(0)
	, m_dImageSizeX(VISION_RAW_IMAGE_SIZE_X)
	, m_dImageSizeY(VISION_RAW_IMAGE_SIZE_Y)
{
	m_pParent = (CHost_InterfaceDlg*)pParent;
}

CDlgDefectMap::~CDlgDefectMap()
{
	m_font.DeleteObject();
	m_font_Bold.DeleteObject();

	m_Brush.DeleteObject();
	m_Brush_Title.DeleteObject();
}

void CDlgDefectMap::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DEFECT_1, m_staDefect_1);
	DDX_Control(pDX, IDC_STATIC_DEFECT_2, m_staDefect_2);
}


BEGIN_MESSAGE_MAP(CDlgDefectMap, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_DEFECT_CHANGE, &CDlgDefectMap::OnBnClickedButtonDefectChange)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CDlgDefectMap 메시지 처리기


BOOL CDlgDefectMap::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CALL_Function_CSC();

	m_Brush.CreateSolidBrush(COLOR_GRAY2);
	m_Brush_Title.CreateSolidBrush(COLOR_BLACK);

	// STATIC
	Utility::SystemFont::CreateGUIFont(m_font, 100);
	GetDlgItem(IDC_STATIC_DEFECT_1)->SetFont(&m_font);
	GetDlgItem(IDC_STATIC_DEFECT_1_TITLE)->SetFont(&m_font);
	GetDlgItem(IDC_STATIC_DEFECT_2)->SetFont(&m_font);
	GetDlgItem(IDC_STATIC_DEFECT_2_TITLE)->SetFont(&m_font);

	Utility::SystemFont::CreateGUIFont_Bold(m_font_Bold, 100);
	GetDlgItem(IDC_BUTTON_DEFECT_CHANGE)->SetFont(&m_font_Bold);

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	LoadDefectMapPointData();

	return TRUE;
}


BOOL CDlgDefectMap::PreTranslateMessage(MSG* pMsg)
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

void CDlgDefectMap::CALL_Function_CSC()
{
	// Static
	ControlSizeChange(IDC_STATIC_DEFECT_1_TITLE);
	ControlSizeChange(IDC_STATIC_DEFECT_2_TITLE);

	ControlSizeChange(IDC_STATIC_DEFECT_1);
	ControlSizeChange(IDC_STATIC_DEFECT_2);
	
	// Btn
	ControlSizeChange(IDC_BUTTON_DEFECT_CHANGE);
}

void CDlgDefectMap::ControlSizeChange(int nID)
{
	CWnd* pCtl;

	long	nStartPointX = m_DlgSize.left;
	long	nStartPointY = m_DlgSize.top;

	long	nStaticSizeX = m_DlgSize.Width() / 2;
	long	nTitleSizeX = nStaticSizeX;
	long	nBtnSizeX = m_DlgSize.Width();

	long	nStaticSizeY = m_DlgSize.Height() * 0.9;
	long	nTitleSizeY = m_DlgSize.Height() * 0.05;
	long	nBtnSizeY = m_DlgSize.Height() * 0.05;

	m_nDefectMapSizeX = nStaticSizeX;
	m_nDefectMapSizeY = nStaticSizeY;

	// Btn 
	pCtl = GetDlgItem(IDC_BUTTON_DEFECT_CHANGE);
	if (pCtl != NULL)
	{
		pCtl->MoveWindow(nStartPointX, nStartPointY, nBtnSizeX, nBtnSizeY);
	}

	// Defect Map (Static)
	pCtl = GetDlgItem(IDC_STATIC_DEFECT_1_TITLE);
	if (pCtl != NULL)
	{
		pCtl->MoveWindow(nStartPointX, nStartPointY + nBtnSizeY, nStaticSizeX, nTitleSizeY);
	}

	pCtl = GetDlgItem(IDC_STATIC_DEFECT_2_TITLE);
	if (pCtl != NULL)
	{
		pCtl->MoveWindow(nStartPointX + nStaticSizeX, nStartPointY + nBtnSizeY, nStaticSizeX, nTitleSizeY);
	}

	pCtl = GetDlgItem(IDC_STATIC_DEFECT_1);
	if (pCtl != NULL)
	{
		pCtl->MoveWindow(nStartPointX, nStartPointY  + nBtnSizeY + nTitleSizeY, nStaticSizeX, nStaticSizeY);
	}

	pCtl = GetDlgItem(IDC_STATIC_DEFECT_2);
	if (pCtl != NULL)
	{
		pCtl->MoveWindow(nStartPointX + nStaticSizeX, nStartPointY + nBtnSizeY + nTitleSizeY, nStaticSizeX, nStaticSizeY);
	}
}

void CDlgDefectMap::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnTimer(nIDEvent);
}
//
//void CDlgDefectMap::SetPoint(Share::ResultVision& data, HostData::Result_Cell* VD_BOX)
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
//	int nPointCount = 1;
//
//	for (long i = 0; i < data.m_vecDefect.size(); i++)
//	{
//		for (long d = 0; d < data.m_vecDefect[i].m_vecstDefectROI.size(); d++)
//		{
//			XY.nX = (long)(data.m_vecDefect[i].m_vecstDefectROI[d].m_dRelative_Img_X) * ((m_nDefectMapSizeX * 0.86) / m_dImageSizeX);
//			//XY.nX = (long)m_nDefectMapSizeX* 0.86; // 이렇게 0.86이라는 값을 곱해야만 맨 끝 위치가 맞는다..
//
//			// 25000이라는 값의 의미는 2500M(롤의 총 길이) * 0.1M(100mm = Vision이 줄수 있는 이미지 의 길이)
//			// 즉, 한롤에 들어가는 CELL의 수량을 의미
//			// 물론. 모델별로 롤 전체 길이가 다를거지만,, 그냥 이정도면 될듯하니 대충 2500M로 맞춰자..
//			XY.nY = (long)(VD_BOX->m_nCellindex * (m_nDefectMapSizeY / (double)25000));
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
//			
//			// 20231010 LJH SetPoint 정보 저장
//			SaveDefectMapPointData(nPointCount, VD_BOX->m_nCellindex, data.m_vecDefect[i].m_nCamera_Position, XY.nX, XY.nY, data.m_vecDefect[i].m_inspType);
//			nPointCount++;
//		}
//	}
//}

void CDlgDefectMap::SetPoint(long nType, DefectPointXY XY)
{
	if (nType == 0)
	{
		m_staDefect_1.SetPoint(XY.nX, XY.nY, XY.Color);
	}
	else if (nType == 1)
	{
		m_staDefect_2.SetPoint(XY.nX, XY.nY, XY.Color);
	}	
}

void CDlgDefectMap::DataCountSetZero()
{
	m_staDefect_1.InitStatic();
	m_staDefect_2.InitStatic();
}

void CDlgDefectMap::SaveDefectMapPointData(int nPointCount, int nCellid, int nCamera_Position, int nX, int nY, DefectType inspType)
{
	CString strImageInfoPath = g_systemParameter.Pathinfo.ImageInfoIniPath(g_stOperatorInfo.strLotID);
	CString strCellid, strPointCount, strCameraPos, strX, strY;
	CString strCameraPosValue, strXValue, strYValue;

	strCellid.Format(_T("%d"), nCellid);
	strPointCount.Format(_T("%d"), nPointCount);		
	strCameraPos.Format(_T("DEFECT_CAM_POS_%d"), nPointCount);				// 개수
	strX.Format(_T("DEFECT_POINT_X_%d"), nPointCount);						// 개수
	strY.Format(_T("DEFECT_POINT_Y_%d"), nPointCount);						// 개수

	strCameraPosValue.Format(_T("%d,%d"), inspType ,nCamera_Position);		// 디펙타입, 카메라위치
	strXValue.Format(_T("%d"), nX);											// X축위치
	strYValue.Format(_T("%d"), nY);											// Y축위치

	WritePrivateProfileString(strCellid, _T("Defect_Point_Count"), strPointCount, strImageInfoPath);					// 불량 점 총 개수 (Defect Map Point 개수)
	WritePrivateProfileString(strCellid, strCameraPos,		strCameraPosValue,		  strImageInfoPath);				// Camera Position
	WritePrivateProfileString(strCellid, strX,				strXValue,				  strImageInfoPath);				// Camera Point X
	WritePrivateProfileString(strCellid, strY,				strYValue,				  strImageInfoPath);				// Camera Point Y
}

void CDlgDefectMap::LoadDefectMapPointData()
{
	DefectPointXY XY;
	CString strResultLotID, strYValue, strXValue, strCameraPosValue, strPreNGCellid, strLoadLastCellid, strDefectPointCnt;
	CString strNoData = _T("Not Image");
	TCHAR inbuffer[256];

	GetPrivateProfileString(_T("Check Recovery Cell Data"), _T("Last Lot ID"), strNoData, inbuffer, sizeof(inbuffer), _T("C:\\IntekPlus\\CoatorInterface\\Config\\SystemParameter.ini"));
	strResultLotID = (LPCTSTR)inbuffer;
	CString strFilePath = g_systemParameter.Pathinfo.ImageInfoIniPath(strResultLotID);

	GetPrivateProfileString(_T("Check Recovery Cell Data"), _T("Last Cell ID"), strNoData, inbuffer, sizeof(inbuffer), _T("C:\\IntekPlus\\CoatorInterface\\Config\\SystemParameter.ini"));
	strLoadLastCellid = (LPCTSTR)inbuffer;
	GetPrivateProfileString(strLoadLastCellid, _T("Defect_Point_Count"), strNoData, inbuffer, sizeof(inbuffer), strFilePath);
	strDefectPointCnt = (LPCTSTR)inbuffer;

	while(_T("") != strLoadLastCellid || _T("Not Image") != strLoadLastCellid)
	{
		GetPrivateProfileString(strLoadLastCellid, _T("PRE NG CELL ID"), strNoData, inbuffer, sizeof(inbuffer), strFilePath);
		strPreNGCellid = (LPCTSTR)inbuffer;
		if (strNoData == strPreNGCellid)
		{
			break;
		}

		if (strLoadLastCellid == strPreNGCellid)			// 중복 Cell ID 일 경우 탈출
		{
			break;
		}

		for (int x = 0; x < _ttoi(strDefectPointCnt); x++)
		{
			CString strDefect, strCameraPos, strCount;
			strCount.Format(_T("%d"), x + 1);
			GetPrivateProfileString(strLoadLastCellid, _T("DEFECT_CAM_POS_") + strCount, strNoData, inbuffer, sizeof(inbuffer), strFilePath);
			strCameraPosValue = (LPCTSTR)inbuffer;

			AfxExtractSubString(strDefect, strCameraPosValue, 0, ',');
			AfxExtractSubString(strCameraPos, strCameraPosValue, 1, ',');

			GetPrivateProfileString(strLoadLastCellid, _T("DEFECT_POINT_X_") + strCount, strNoData, inbuffer, sizeof(inbuffer), strFilePath);
			strXValue = (LPCTSTR)inbuffer;
			XY.nX = _ttoi(strXValue);

			GetPrivateProfileString(strLoadLastCellid, _T("DEFECT_POINT_Y_") + strCount, strNoData, inbuffer, sizeof(inbuffer), strFilePath);
			strYValue = (LPCTSTR)inbuffer;
			XY.nY = _ttoi(strYValue);

			//if (_T("0") == strDefect || _T("1") == strDefect)				// Foil Exposure
			//{																 
			//	XY.Color = COLOR_RED;										 
			//}
			//else if (_T("2") == strDefect || _T("3") == strDefect)			// Surface
			//{
			//	XY.Color = COLOR_BLUE;
			//}
			//else if (_T("4") == strDefect || _T("5") == strDefect)			// Dross
			//{
			//	XY.Color = COLOR_YELLOW;
			//}
			//else
			{
				XY.Color = COLOR_RED;	// 20230524 LJH Color 쓰레기 값이 들어와 추가
			}

			SetPoint(_ttoi(strCameraPos), XY);
		}

		strLoadLastCellid = strPreNGCellid;	// 이전 셀아이디 기억하기.
	}
}

void CDlgDefectMap::OnBnClickedButtonDefectChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ShowWindow(SW_HIDE);
	
	m_pParent->m_pDlgAccDefectMap->ShowWindow(SW_SHOW);
}


void CDlgDefectMap::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CRect rtROI;
	m_pParent->GetDlgItem(IDC_STATIC_DEFECTMAP)->GetWindowRect(rtROI);
	m_pParent->ScreenToClient(&rtROI);

	MoveWindow(rtROI);

	GetClientRect(m_DlgSize);
}


HBRUSH CDlgDefectMap::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_DEFECT_1_TITLE || pWnd->GetDlgCtrlID() == IDC_STATIC_DEFECT_2_TITLE)
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