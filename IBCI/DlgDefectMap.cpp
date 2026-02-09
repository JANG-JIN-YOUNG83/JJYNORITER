// DlgDefectMap.cpp: 구현 파일
#include "pch.h"
#include "DlgDefectMap.h"
#include "afxdialogex.h"
#include "ColorDefine.h"
#include "IBCIDlg.h"
#include "DlgDefectMapUpdate.h"

#include "../Module/Utility/SystemFont.h"
#include <locale.h>
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
    , m_pDlgDefectMapUpdate(nullptr)
    , m_bPauseDefectMapUpdate(FALSE)
    , m_bQuitDefectMapUpdate(FALSE)
    , m_nThreadUIReadNum(0)
    , m_pDlgDefectTimerView(nullptr)
    , m_bTimerDialogView(FALSE)
{
	m_pParent = (CIBCIDlg*)pParent;
    
}

CDlgDefectMap::~CDlgDefectMap()
{
	m_font.DeleteObject();
	m_font_Bold.DeleteObject();

	m_Brush.DeleteObject();
	m_Brush_Title.DeleteObject();
	delete m_dlgDefectView;

	if (m_pDlgDefectMapUpdate)
        delete m_pDlgDefectMapUpdate;

		if (m_pDlgDefectTimerView)
        delete m_pDlgDefectTimerView;
	ThreadStop_UpdateMap();
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
	ON_WM_LBUTTONDOWN()
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
	m_dlgDefectView = new CDlgDefectImageView(4,2);
	m_dlgDefectView->Create(IDD_DLG_DEFECTIMAGE_VIEW,this);
	//m_dlgDefectView->ModifyStyle(WS_CHILD,WS_POPUP); // 스타일 수정 
	m_dlgDefectView->ModifyStyle(WS_CHILD, WS_POPUP | WS_BORDER | WS_CAPTION);

	m_dlgDefectView->SetParent(NULL);
	
	//m_dlgDefectView->SetParent(this);  //  부모 윈도우를 지정 하지 않으면 POPUP 창으로 부모 위에 위치함
	
	//m_dlgDefectView->SetIndex(4, 2);
	CRect rt;
	GetClientRect(&rt);
	m_dlgDefectView->MoveWindow(rt);
	m_dlgDefectView->ShowWindow(SW_HIDE);

	ThreadStart_UpdateMap();
    //ThreadStart_MapFileOpen();

    if (m_pDlgDefectMapUpdate == nullptr)
    {
        m_pDlgDefectMapUpdate = new CDlgDefectMapUpdate(this);
        m_pDlgDefectMapUpdate->Create(IDD_DIALOG_DEFECTMAP_UPDATE, this);
        m_pDlgDefectMapUpdate->CenterWindow(CWnd::GetDesktopWindow());
        m_pDlgDefectMapUpdate->ShowWindow(SW_SHOW);
        
		//::SetEvent(m_threadControl_UpdateMap.m_Signal_Start);
    }

 
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

    //if (m_pParent->m_bProgramOpened == TRUE && m_pParent->CheckFindDefectMapFile() == TRUE)
    if (m_pParent->m_bIBCIDlgOpened == TRUE)
    {
        ::SetEvent(m_threadControl_UpdateMap.m_Signal_Start);
	}
}

void CDlgDefectMap::ControlSizeChange(int nID)
{
	CWnd* pCtl;

	long	nStartPointX = m_DlgSize.Width() * 0.01;
	long	nStartPointY = m_DlgSize.top;

	long	nStaticSizeX = m_DlgSize.Width() * 0.49;
	long	nTitleSizeX = nStaticSizeX;
	long	nBtnSizeX = m_DlgSize.Width();

	long	nStaticSizeY = m_DlgSize.Height() * 0.9;
	long	nTitleSizeY = m_DlgSize.Height() * 0.05;
	long	nBtnSizeY = m_DlgSize.Height() * 0.05;

	m_nDefectMapSizeX = nStaticSizeX;
	m_nDefectMapSizeY = nStaticSizeY;
    m_rtSize.top = nStartPointY;
    m_rtSize.left = nStartPointX;
    m_rtSize.bottom = m_nDefectMapSizeY;
    m_rtSize.right = nBtnSizeX;
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
		pCtl->MoveWindow(nStartPointX, nStartPointY + nBtnSizeY + nTitleSizeY, nStaticSizeX, nStaticSizeY);
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
////
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

void CDlgDefectMap::SetPoint(long nTopBack, DefectPointXY XY, long nIndex, long nImageSizeX)
{
	XY.nX = (long)(XY.nX) * ((m_nDefectMapSizeX * 0.86) / nImageSizeX);
	XY.nY = (long)(nIndex * (m_nDefectMapSizeY / (double)25000));
	XY.nY %= m_nDefectMapSizeY; // todo:2024.06.04, initial 시퀀스 추가 시 제거

	//strLotID

	/*CRect rt(0, 0, m_viewSizeX, m_viewSizeY);

	auto nWidth = m_viewSizeX;
	auto nHeight = m_viewSizeY;

	CPoint cpAxisSize(nWidth * 0.12, 0);

	CRect rtAxis(0, 0, cpAxisSize.x, nHeight);
	CRect rtGraph(cpAxisSize.x, 0, nWidth, nHeight);*/

	/*CPoint cpGraphStartPoint(rtGraph.left + 2, rtGraph.top + 2);

	CRect	rctArea;
	rctArea.left = (cpGraphStartPoint.x + data.nX) - nRadius;
	rctArea.top = (cpGraphStartPoint.y + data.nY) - nRadius;
	rctArea.right = (cpGraphStartPoint.x + data.nX) + nRadius;
	rctArea.bottom = (cpGraphStartPoint.y + data.nY) + nRadius;*/


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


	//if (pCtl != NULL)
	//{
	//	pCtl->MoveWindow(nStartPointX, nStartPointY + nBtnSizeY + nTitleSizeY, nStaticSizeX, nStaticSizeY);
	//}

	//if (pCtl != NULL)
	//{
	//	pCtl->MoveWindow(nStartPointX + nStaticSizeX, nStartPointY + nBtnSizeY + nTitleSizeY, nStaticSizeX, nStaticSizeY);
	//}

	if (nTopBack == 1)
	{
		m_staDefect_1.SetPoint(XY.nX, XY.nY, XY.Color);
		auto nWidth = m_staDefect_1.m_graphStyle->m_viewSizeX;
		auto nHeight = m_staDefect_1.m_graphStyle->m_viewSizeY;
		CPoint cpAxisSize(nWidth * 0.12, 0);

		CRect rtGraph(cpAxisSize.x, 0, nWidth, nHeight);
		
		CPoint cpGraphStartPoint(rtGraph.left + 2, rtGraph.top + 2);
		XY.nX = cpGraphStartPoint.x + XY.nX + nStartPointX;
		XY.nY = cpGraphStartPoint.y + XY.nY+ nStartPointY + nBtnSizeY + nTitleSizeY;
		m_vecTopDefect.push_back(XY);
	}
    else if (nTopBack == 0)
	{
		auto nWidth = m_staDefect_2.m_graphStyle->m_viewSizeX;
		auto nHeight = m_staDefect_2.m_graphStyle->m_viewSizeY;
		CPoint cpAxisSize(nWidth * 0.12, 0);

		CRect rtGraph(cpAxisSize.x, 0, nWidth, nHeight);

		CPoint cpGraphStartPoint(rtGraph.left + 2, rtGraph.top + 2);

		m_staDefect_2.SetPoint(XY.nX, XY.nY, XY.Color);
		XY.nX = cpGraphStartPoint.x + XY.nX + nStartPointX + nStaticSizeX;
		XY.nY = cpGraphStartPoint.y + XY.nY+ nStartPointY + nBtnSizeY + nTitleSizeY;
		m_vecBtmDefect.push_back(XY);
	}
}

void CDlgDefectMap::DataCountSetZero()
{
	m_staDefect_1.InitStatic();
	m_staDefect_2.InitStatic();
}

void CDlgDefectMap::SaveDefectMapPointData(int nPointCount, int nCellid, int nCamera_Position, int nX, int nY, DefectType inspType)
{
	CString strImageInfoPath = _T("temp");// g_systemParameter.Pathinfo.ImageInfoIniPath(g_stOperatorInfo.strLotID);
	CString strCellid, strPointCount, strCameraPos, strX, strY;
	CString strCameraPosValue, strXValue, strYValue;

	strCellid.Format(_T("%d"), nCellid);
	strPointCount.Format(_T("%d"), nPointCount);
	strCameraPos.Format(_T("DEFECT_CAM_POS_%d"), nPointCount);				// 개수
	strX.Format(_T("DEFECT_POINT_X_%d"), nPointCount);						// 개수
	strY.Format(_T("DEFECT_POINT_Y_%d"), nPointCount);						// 개수

	strCameraPosValue.Format(_T("%d,%d"), inspType, nCamera_Position);		// 디펙타입, 카메라위치
	strXValue.Format(_T("%d"), nX);											// X축위치
	strYValue.Format(_T("%d"), nY);											// Y축위치

	WritePrivateProfileString(strCellid, _T("Defect_Point_Count"), strPointCount, strImageInfoPath);					// 불량 점 총 개수 (Defect Map Point 개수)
	WritePrivateProfileString(strCellid, strCameraPos, strCameraPosValue, strImageInfoPath);				// Camera Position
	WritePrivateProfileString(strCellid, strX, strXValue, strImageInfoPath);				// Camera Point X
	WritePrivateProfileString(strCellid, strY, strYValue, strImageInfoPath);				// Camera Point Y
}

void CDlgDefectMap::LoadDefectMapPointData()
{
	DefectPointXY XY;
	CString strResultLotID, strYValue, strXValue, strCameraPosValue, strPreNGCellid, strLoadLastCellid, strDefectPointCnt;
	CString strNoData = _T("Not Image");
	TCHAR inbuffer[256];

	GetPrivateProfileString(_T("Check Recovery Cell Data"), _T("Last Lot ID"), strNoData, inbuffer, sizeof(inbuffer), _T("C:\\IntekPlus\\CoatorInterface\\Config\\SystemParameter.ini"));
	strResultLotID = (LPCTSTR)inbuffer;
	//CString strFilePath = g_systemParameter.Pathinfo.ImageInfoIniPath(strResultLotID);
	CString strFilePath = _T("");
	// 임의 주소 만듭시다
	GetPrivateProfileString(_T("Check Recovery Cell Data"), _T("Last Cell ID"), strNoData, inbuffer, sizeof(inbuffer), _T("C:\\IntekPlus\\CoatorInterface\\Config\\SystemParameter.ini"));
	strLoadLastCellid = (LPCTSTR)inbuffer;
	GetPrivateProfileString(strLoadLastCellid, _T("Defect_Point_Count"), strNoData, inbuffer, sizeof(inbuffer), strFilePath);
	strDefectPointCnt = (LPCTSTR)inbuffer;

	while (_T("") != strLoadLastCellid || _T("Not Image") != strLoadLastCellid)
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

			//SetPoint(_ttoi(strCameraPos), XY);
		}

		strLoadLastCellid = strPreNGCellid;	// 이전 셀아이디 기억하기.
	}
}

void CDlgDefectMap::OnBnClickedButtonDefectChange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//ShowWindow(SW_HIDE);
    //m_pDlgDefectTimerView = new CDlgDefectTimerView(this);
    //m_pDlgDefectTimerView->Create(IDD_DIALOG_DEFECTTIMERVIEW, this);
	//m_pParent->m_->ShowWindow(SW_SHOW);

	if (m_pDlgDefectTimerView == nullptr)
    {
        m_pDlgDefectTimerView = new CDlgDefectTimerView(this);
        m_pDlgDefectTimerView->Create(IDD_DIALOG_DEFECTTIMERVIEW, this);
        //m_pDlgDefectTimerView->CenterWindow(CWnd::GetDesktopWindow());
        //m_pDlgDefectTimerView->ShowWindow(SW_HIDE);
        m_pDlgDefectTimerView->Init();
    }
    //ThreadStart_UpdateMap();
    

    if(m_bTimerDialogView == FALSE)
    {
        m_pDlgDefectTimerView->ShowWindow(SW_SHOW);

        GetDlgItem(IDC_STATIC_DEFECT_1_TITLE)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_DEFECT_2_TITLE)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_DEFECT_1)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_STATIC_DEFECT_2)->ShowWindow(SW_HIDE);
        m_bTimerDialogView = TRUE;
	}
    else
    {
        m_pDlgDefectTimerView->ShowWindow(SW_HIDE);

        GetDlgItem(IDC_STATIC_DEFECT_1_TITLE)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_STATIC_DEFECT_2_TITLE)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_STATIC_DEFECT_1)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_STATIC_DEFECT_2)->ShowWindow(SW_SHOW);
        m_bTimerDialogView = FALSE;
	}
   // m_pDlgDefectTimerView->SetShowWindow(m_bTimerDialogView);

	
	//AfxMessageBox(_T("이거 좀 더 구현 필요"));
	//ShowWindow(SW_SHOW);
}

void CDlgDefectMap::OnLButtonDown(UINT nFlags, CPoint point)
{
    CString filePathTop, filePathBack;
    filePathTop.Format(_T("%s\\DefectMapData_Top.csv"), Path::GetBackupUIFolder());
    filePathBack.Format(_T("%s\\DefectMapData_Back.csv"), Path::GetBackupUIFolder());
    // top
    if (point.x < m_nDefectMapSizeX)
    {
        DefectMapClick(filePathTop, point, m_vecTopDefect, m_vecMapPointPathTop);
    }
    // back
    if (point.x > m_nDefectMapSizeX)
    {
        DefectMapClick(filePathBack, point, m_vecBtmDefect, m_vecMapPointPathBack);
    }

	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    //StartUIUpdateThread();
    //return;
	//// 이거 좀 더 생각해서 만들필요 있음.

	//long nTemp = 0;
	//CString strTempAdd = _T("");
	//std::vector<long> vecnView;
 //   for (long nDfct = 0; nDfct < m_vecTopDefect.size(); nDfct++)
	//{
	//	m_vecTopDefect[nDfct].nX;
	//	m_vecTopDefect[nDfct].nY;
	//	CRect rtTemp;
	//	long nRadius = 4; // 안에 2더라
 //       rtTemp.left = m_vecTopDefect[nDfct].nX - nRadius + 5;
 //       rtTemp.right = m_vecTopDefect[nDfct].nX + nRadius + 5;
 //       rtTemp.top = m_vecTopDefect[nDfct].nY - nRadius;
 //       rtTemp.bottom = m_vecTopDefect[nDfct].nY + nRadius;

	//	if (PtInRect(rtTemp, point))
	//	{
	//		long nPt = 0;
	//		CString strTemp;// = _T("");
 //           CString str = GetDefectName((DefectType)m_vecISIAllDefectTotal[nDfct].m_nCandidateDefectCode);
 //           strTemp.Format(_T("Top Clicked %d "), nDfct);
	//		strTempAdd += strTemp;
	//		strTempAdd += str;
 //           vecnView.push_back(nDfct);
	//		
	//	}
	//}

	//for (long nDfct = 0; nDfct < m_vecBtmDefect.size(); nDfct++)
	//{
	//	m_vecBtmDefect[nDfct].nX;
	//	m_vecBtmDefect[nDfct].nY;
	//	CRect rtTemp;

	//	long nRadius = 3; // 안에 2더라
 //       rtTemp.left = m_vecBtmDefect[nDfct].nX - nRadius;
 //       rtTemp.right = m_vecBtmDefect[nDfct].nX + nRadius;
 //       rtTemp.top = m_vecBtmDefect[nDfct].nY - nRadius;
 //       rtTemp.bottom = m_vecBtmDefect[nDfct].nY + nRadius;

	//	if (PtInRect(rtTemp, point))
	//	{
 //           CString str = GetDefectName((DefectType)m_vecISIAllDefectTotal[nDfct].m_nCandidateDefectCode);
	//		long nPt = 0;
	//		CString strTemp;// = _T("");
 //           strTemp.Format(_T("Btm Clicked %d "), nDfct);
	//		strTempAdd += strTemp;
	//		strTempAdd += str;
 //           vecnView.push_back(nDfct);
	//	}
	//}

	//if(strTempAdd!=_T(""))
	//{
	//	AfxMessageBox(strTempAdd);
	//	long nTemp = 0;
	//	for (long nView : vecnView)
	//	{
	//		CString str = GetDefectName((DefectType)m_vecISIAllDefectTotal[nView].m_nCandidateDefectCode);
 //           m_dlgDefectView->UpdateUI(&m_vecISIAllDefectTotal[nView].brightImage, &m_vecISIAllDefectTotal[nView].darkImage);
	//		nTemp++;
	//		if (nTemp > 4)
	//		{
	//			return;
	//		}
	//	}
	//	m_dlgDefectView->ShowWindow(SW_SHOW);
	//}
		
}

void CDlgDefectMap::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	
	GetClientRect(m_DlgSize);
    m_rtSize = m_DlgSize;
	CALL_Function_CSC();
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

void CDlgDefectMap::ThreadStart_UpdateMap()
{
    m_threadControl_UpdateMap.m_Signal_Kill = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_threadControl_UpdateMap.m_Signal_Ready = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_threadControl_UpdateMap.m_Signal_Start = CreateEvent(NULL, FALSE, FALSE, NULL);

	HANDLE hInspThread = ::AfxBeginThread(ThreadFunc_UpdateMap, LPVOID(this))->m_hThread;
    ::DuplicateHandle(::GetCurrentProcess(), hInspThread, ::GetCurrentProcess(), &m_threadControl_UpdateMap.m_Handle_Thread,
        DUPLICATE_SAME_ACCESS, FALSE, DUPLICATE_SAME_ACCESS);
}


void CDlgDefectMap::ThreadStop_UpdateMap()
{
    m_threadControl_UpdateMap.Terminate();
}

UINT CDlgDefectMap::ThreadFunc_UpdateMap(LPVOID pParam)
{
    CDlgDefectMap* pDlg = (CDlgDefectMap*)pParam;
    return pDlg->ThreadFunc_UpdateMap();
}

UINT CDlgDefectMap::ThreadFunc_UpdateMap()
{
    auto& control = m_threadControl_UpdateMap;
    ::SetEvent(control.m_Signal_Ready);
    HANDLE pHandles[] = {control.m_Signal_Kill, control.m_Signal_Start};
    const DWORD nSignalCount = sizeof(pHandles) / sizeof(HANDLE);

    while (1)
    {
        DWORD dwRet = WaitForMultipleObjects(nSignalCount, pHandles, FALSE, INFINITE);
        if ((WAIT_OBJECT_0 + 1) == dwRet)
        {
            CStdioFile file;
            CFileFind find;
            CFileException ex;
            UINT nCheck = CFile::modeRead | CFile::typeText;

            //CString filePath = _T("C:\\Intekplus\\IBCI\\backup_UI\\DefectMapData.csv");
            CString filePathTop, filePathBack;
            std::vector<CString> vecFilePath;
            filePathBack.Format(_T("%s\\DefectMapData_Back.csv"), Path::GetBackupUIFolder());//[sm]
            filePathTop.Format(_T("%s\\DefectMapData_Top.csv"), Path::GetBackupUIFolder());
            if (find.FindFile(filePathBack))
                vecFilePath.push_back(filePathBack);
            if (find.FindFile(filePathTop))
                vecFilePath.push_back(filePathTop);
            //long nUp = !g_systemParameter.bDirection;
            std::vector<CString> vecStrSpec;
            int nTopBack;
            CString strTopBack;

            for (long nIdx = 0; nIdx < vecFilePath.size(); nIdx++)
            {
                strTopBack = vecFilePath[nIdx].Mid(vecFilePath[nIdx].ReverseFind('_') + 1);
                strTopBack = strTopBack.Mid(0, strTopBack.ReverseFind('.'));
                if (strTopBack == _T("Back"))
                    nTopBack = 0; // back
                else
                    nTopBack = 1; // top

                if (!file.Open(vecFilePath[nIdx], nCheck, &ex))
                {
                    m_pDlgDefectMapUpdate->EndUIUpdate();
                    SetEvent(control.m_Signal_Kill);
                    return 0;
                }
                setlocale(LC_ALL, "korean");

                CString line;
                while (file.ReadString(line))
                {
                    vecStrSpec.push_back(line);
                }
                file.Close();
                for (long n = m_nThreadUIReadNum; n < vecStrSpec.size(); n++)
                {
                    if (m_bPauseDefectMapUpdate == TRUE)
                    {
                        ::SetEvent(m_threadControl_UpdateMap.m_Signal_Ready);
                        goto WAIT_STOP;
                    }
                    if (m_bQuitDefectMapUpdate == TRUE)
                    {
                        ::SetEvent(m_threadControl_UpdateMap.m_Signal_Kill);
                        break;
                    }
                    std::vector<CString> vecDiv;
                    StringTokenDivideToCString(vecStrSpec[n], ',', vecDiv);
                    //m_pParent->ImportDefectMapData(nUp, _ttoi(vecDiv[0]), _ttoi(vecDiv[1]), _ttoi(vecDiv[2]), _ttoi(vecDiv[3]));
                    m_pParent->ImportDefectMapData(
                        nTopBack, _ttoi(vecDiv[0]), _ttoi(vecDiv[1]), _ttoi(vecDiv[2]), _ttoi(vecDiv[3]));
                    //m_nThreadUIReadNum++;
                }
                vecStrSpec.clear();
            }
            ::SetEvent(control.m_Signal_Kill);
        }
        else if (WAIT_OBJECT_0 == dwRet)
        {
            m_pDlgDefectMapUpdate->EndUIUpdate();
            SetEvent(control.m_Handle_Thread);
            break;
        }
        // 업데이트 중지하게 되면 Kill하면 안된다.
WAIT_STOP:;
    }
    return 0;
}

BOOL CDlgDefectMap::StringTokenDivideToCString(CString strFull, TCHAR chDivide, std::vector<CString>& vecstrValue)
{
    long nDivideTotalCount = 0;
    CString strSub;
    while (FALSE != AfxExtractSubString(strSub, strFull, nDivideTotalCount++, chDivide))
    {
        vecstrValue.push_back(strSub);
    }
    return TRUE;
}

void CDlgDefectMap::DefectMapClick(CString strFilePath, CPoint point, std::vector<DefectPointXY> vecDefectPt,
    std::vector<DefectMapPointPath>& vecMapPointPath)
{
    CStdioFile file;
    CFileFind find;
    CFileException ex;
    UINT nCheck = CFile::modeRead | CFile::typeText;
    DefectMapPointPath defectmappointpath;
    std::vector<CString> vecStrSpec;
    std::vector<long> vecIndex;
    vecStrSpec.clear();
    CString strTopBack;
    strTopBack = strFilePath.Mid(strFilePath.ReverseFind('_') + 1);
    strTopBack = strTopBack.Mid(0, strTopBack.ReverseFind('.'));
    CString strTempAdd = _T("");

    if (!file.Open(strFilePath, nCheck, &ex))
    {
        return;
    }
    setlocale(LC_ALL, "korean");

    CString line;
    while (file.ReadString(line))
    {
        vecStrSpec.push_back(line);
    }
    file.Close();
    for (long n = 0; n < vecStrSpec.size(); n++)
    {
        std::vector<CString> vecDiv;
        StringTokenDivideToCString(vecStrSpec[n], ',', vecDiv);
        defectmappointpath.nPointX = _ttoi(vecDiv[1]);
        defectmappointpath.nPointY = _ttoi(vecDiv[2]);
        defectmappointpath.nDefectCode = _ttoi(vecDiv[4]);
        defectmappointpath.strBrightPath = vecDiv[5];
        defectmappointpath.strDarkPath = vecDiv[6];
        vecMapPointPath.push_back(defectmappointpath);
    }
    //CString strTemp2;

    for (long nDfct = 0; nDfct < vecDefectPt.size(); nDfct++)
    {
        CRect rtTemp;
        long nRadius = 4; // 안에 2더라
        rtTemp.left = vecDefectPt[nDfct].nX - nRadius + 5;
        rtTemp.right = vecDefectPt[nDfct].nX + nRadius + 5;
        rtTemp.top = vecDefectPt[nDfct].nY - nRadius;
        rtTemp.bottom = vecDefectPt[nDfct].nY + nRadius;

        if (PtInRect(rtTemp, point))
        {
            if (nDfct >= vecMapPointPath.size())
            {
                continue;
            }
            long nPt = 0;
            CString strTemp; // = _T("");
            CString str = GetDefectName((DefectType)vecMapPointPath[nDfct].nDefectCode);
            strTemp.Format(strTopBack + _T(" Clicked %d "), nDfct);
            //strTemp2.Format(
            //    _T("x: %d   y: %d"), m_vecMapPointPathTop[nDfct].nPointX, m_vecMapPointPathTop[nDfct].nPointY);
            strTempAdd += strTemp;
            strTempAdd += str + _T("\n");
            vecIndex.push_back(nDfct);
            //MessageBox(strTempAdd);
        }
    }
    if (strTempAdd != _T(""))
    {
        AfxMessageBox(strTempAdd);
        long nTemp = 0;
        m_dlgDefectView->DefectMapImageClear();
        for (long nIdx = 0; nIdx < vecIndex.size(); nIdx++)
        {
            long nVecIndex = vecIndex[nIdx];
            CString str = GetDefectName((DefectType)vecMapPointPath[nVecIndex].nDefectCode);
            if (nTemp < m_dlgDefectView->m_nVecSize)
            {
                IPVM::Image_8u_C1 imgB, imgD;
                if (vecMapPointPath[nVecIndex].strBrightPath.IsEmpty())
                    return;
                IPVM::Status status = IPVM::Status::unknownError;
				
                status = imgB.LoadFrom(g_systemParameter.pathResultDataCsv + vecMapPointPath[nVecIndex].strBrightPath);
                status = imgD.LoadFrom(g_systemParameter.pathResultDataCsv + vecMapPointPath[nVecIndex].strDarkPath);
                if (status == IPVM::Status::cannotOpenFile)
                {
                    MessageBox(_T("Image does not exist"));
                    return;
                }
                m_dlgDefectView->DefectMapImagePopup(&imgB, &imgD, nTemp, str);

                nTemp += 2;
            }
        }
        m_dlgDefectView->ShowWindow(SW_SHOW);
        vecIndex.clear();
    }
}

