// CDlgClassificationST.cpp: 구현 파일
//

#include "stdafx.h"
#include "Inspection.h"
#include "CDlgClassificationST.h"
#include "afxdialogex.h"
#include "DlgInspectionArea.h"

#include "basedef.h"
#include "CDlgEditGridText.h"
#include "CISISpecTable.h"
#include "Widget/ImageView.h"
#include "Algorithm/ImageProcessing.h"
#include "DlgInspectionMain.h"
#include "DlgClassificationCondition.h"
#include "../Inspection/Accessor.h"

#define	MESSAGE_GRID	(WM_USER + 400)
#define UWM_UPDATE_GRID (WM_USER + 401)

CMutex m_mutex;
//bool compare(std::pair<DefectType, CISISpecInfo>& a, std::pair<DefectType, CISISpecInfo>& b)
//{
//    if (a.second.PRIORITY < b.second.PRIORITY)
//        return true;
//    else
//        return false;
//}

bool compare(std::pair<DefectType, std::vector<CISISpecInfo>*>& a, std::pair<DefectType, std::vector<CISISpecInfo>*>& b)
{
    if (a.second->size() == 0 || b.second->size() == 0)
	{
              return true;
	}

	if (a.second->at(0).PRIORITY < b.second->at(0).PRIORITY)
    {
        return true;
    }
    else
        return false;
}

BOOL virtualGrid(GV_DISPINFO* pDisplay, LPARAM lparam)
{
	CDlgClassificationST* pParent = (CDlgClassificationST*)lparam;

	return pParent->CallBackGrid(pDisplay);
}



using namespace classification;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDlgClassificationST 대화 상자

IMPLEMENT_DYNAMIC(CDlgClassificationST, CDialogEx)

CDlgClassificationST::CDlgClassificationST(CISISpecTable& spec, CWnd* pParent, CISICellAllDefect* defect, long blobconditionSize, long nSelectindex)
	: CDialogEx(IDD_DIALOG_CLASSIFICATION, pParent)
	, m_ISI_Spec_table(spec)
	, m_classfication(&m_ISI_Spec_table)
	, m_vecTogeter(nullptr)
	, m_TableDefectInfo(DefectType::Defect_Info, 0.0f, enCompareOperator::CO_END, 0.0f, enCompareOperator::CO_END, enCode::CD_AND, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,true,true,true,true,true,true,true,true) // [sm]
	, m_use_mode(0)
	, m_blobConditionSize(blobconditionSize)
    , m_nExcelRow(0)
{
	m_ctrGrid.SetVirtualMode(true);
	m_ctrGrid.SetCallbackFunc(virtualGrid, (LPARAM)this);
	// 현식 스몰디펙 쓰는 곳
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_CraterLevel1, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_CraterLevel2, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_CraterLevel3, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_CraterLevel4, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_CraterLevel5, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_PinholeLevel1, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_PinholeLevel2, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_PinholeLevel3, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_PinholeLevel4, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_PinholeLevel5, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_DentLevel1, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_DentLevel2, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_DentLevel3, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_DentLevel4, true));
    m_mapSD.insert(std::make_pair(DefectType::SmallDefect_DentLevel5, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ProtrutionLevel1, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ProtrutionLevel2, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ProtrutionLevel3, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ProtrutionLevel4, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ProtrutionLevel5, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_IslandLevel1, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_IslandLevel2, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_IslandLevel3, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_IslandLevel4, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_IslandLevel5, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ScratchLevel1, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ScratchLevel2, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ScratchLevel3, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ScratchLevel4, true));
	m_mapSD.insert(std::make_pair(DefectType::SmallDefect_ScratchLevel5, true));

	m_mapSDCol.insert(std::make_pair(enColTitle::USE, true));
	m_mapSDCol.insert(std::make_pair(enColTitle::ENABLE, true));
	m_mapSDCol.insert(std::make_pair(enColTitle::SIZE_X, true));
	m_mapSDCol.insert(std::make_pair(enColTitle::SIZE_X_CO, true));
	m_mapSDCol.insert(std::make_pair(enColTitle::SIZE_Y, true));
	m_mapSDCol.insert(std::make_pair(enColTitle::SIZE_Y_CO, true));
	m_mapSDCol.insert(std::make_pair(enColTitle::ORAND, true));
	m_mapSDCol.insert(std::make_pair(enColTitle::MINIMUM_SIZE_X, true));
	m_mapSDCol.insert(std::make_pair(enColTitle::MINIMUM_SIZE_Y, true));
	m_mapSDCol.insert(std::make_pair(enColTitle::COUNT, true));
	m_mapSDCol.insert(std::make_pair(enColTitle::PARENT, true));






	m_nSelectDefectIndex = nSelectindex;
	if (nullptr != defect)
	{
		m_DefectInfoOrg = m_DefectInfoProcess = *defect;
		//m_classfication.Start(&m_DefectInfoProcess);
	}
    m_pParent = (CDlgInspectionMain*)pParent;
}

CDlgClassificationST::~CDlgClassificationST()
{
	if (m_imageView_Bright != NULL)
		delete m_imageView_Bright;
	if (m_imageView_Dark != NULL)
		delete m_imageView_Dark;
	if (m_imageView_Binary != NULL)
		delete m_imageView_Binary;
    if (m_classificationCondition != NULL)
        delete m_classificationCondition;
	delete m_pDlgDefectGrid;
}

void CDlgClassificationST::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TAB1, m_ctrTab);
    DDX_Control(pDX, IDC_TAB2, m_ctrTab2);
    DDX_Control(pDX, IDC_STATIC_CLASSIFICATION, m_ctrStaticClassification);
    DDX_Control(pDX, IDC_STATIC_FINAL_JUDGE, m_ctrStaticFinaljudge);
    DDX_Control(pDX, IDC_STATIC_IMAGE1, m_ctrStatic_image1);
    DDX_Control(pDX, IDC_STATIC_IMAGE2, m_ctrStatic_image2);
    DDX_Control(pDX, IDC_STATIC_IMAGE3, m_ctrStatic_image3);
    DDX_Control(pDX, IDC_BTN_RE_FRESH, m_ctrBtnRefresh);
    DDX_Control(pDX, IDOK, m_ctrBtnExist);
    DDX_Control(pDX, IDCANCEL, m_ctrBtnCancel);
    DDX_Control(pDX, IDC_LIST2, m_ctrList);
    DDX_Control(pDX, IDC_BTN_INIT_SPEC, m_ctrBtnSpecInit);
    DDX_Control(pDX, IDC_BTN_FGZoomFit, m_cntBtn_FgZoomFit);
    DDX_Control(pDX, IDC_BTN_FGZoomPlus, m_cntBtn_FgZoomPlus);
    DDX_Control(pDX, IDC_BTN_FGZoomMinus, m_cntBtn_FgZoomMinus);
    DDX_Control(pDX, IDC_CHECK1, m_ctrUse);
    DDX_Control(pDX, IDC_BTN_USE_ADMIN, m_btn_useAdmin);
    DDX_Control(pDX, IDC_BTN_USE_NORMAL, m_btn_useNormal);
    DDX_Control(pDX, IDC_BTN_INSPECTION_AREA, m_btnInspectionArea);
    DDX_Control(pDX, IDC_BTN_RE_SAVEEXCEL, m_ctrBtnSaveExcel);
    DDX_Control(pDX, IDC_BTN_SAVE_SPEC_CSV, m_ctrBtnSaveSpec);
    DDX_Control(pDX, IDC_BTN_LOAD_SPEC_CSV, m_ctrBtnLoadSpec);
    DDX_Control(pDX, IDC_BTN_PRIORITY_UP, m_ctrBtnSetPriorityUp);
    DDX_Control(pDX, IDC_BTN_PRIORITY_DOWN, m_ctrBtnSetPriorityDown);
	
}


BEGIN_MESSAGE_MAP(CDlgClassificationST, CDialogEx)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CDlgClassificationST::OnTcnSelchangeTab1)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDOK, &CDlgClassificationST::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgClassificationST::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_RE_FRESH, &CDlgClassificationST::OnBnClickedBtnReFresh)
	ON_BN_CLICKED(IDC_BTN_INIT_SPEC, &CDlgClassificationST::OnBnClickedBtnInitSpec)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB2, &CDlgClassificationST::OnTcnSelchangeTab2)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_FGZoomFit, &CDlgClassificationST::OnBnClickedBtnFgzoomfit)
	ON_BN_CLICKED(IDC_BTN_FGZoomPlus, &CDlgClassificationST::OnBnClickedBtnFgzoomplus)
	ON_BN_CLICKED(IDC_BTN_FGZoomMinus, &CDlgClassificationST::OnBnClickedBtnFgzoomminus)
	ON_BN_CLICKED(IDC_CHECK1, &CDlgClassificationST::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BTN_USE_NORMAL, &CDlgClassificationST::OnBnClickedBtnUseNormal)
	ON_BN_CLICKED(IDC_BTN_USE_ADMIN, &CDlgClassificationST::OnBnClickedBtnUseAdmin)
	ON_BN_CLICKED(IDC_BTN_INSPECTION_AREA, &CDlgClassificationST::OnBnClickedBtnInspectionArea)
    ON_BN_CLICKED(IDC_BTN_RE_SAVEEXCEL, &CDlgClassificationST::OnBnClickedBtnReSaveexcel)
    ON_BN_CLICKED(IDC_BTN_PRIORITY_UP, &CDlgClassificationST::OnBnClickedBtnPriorityUp)
	ON_BN_CLICKED(IDC_BTN_PRIORITY_DOWN, &CDlgClassificationST::OnBnClickedBtnPriorityDown)
    ON_MESSAGE(UWM_UPDATE_GRID, &CDlgClassificationST::OnUWMGridUpdate)
    ON_BN_CLICKED(IDC_BTN_SAVE_SPEC_CSV, &CDlgClassificationST::OnBnClickedBtnSaveSpecCsv)
    ON_BN_CLICKED(IDC_BTN_LOAD_SPEC_CSV, &CDlgClassificationST::OnBnClickedBtnLoadSpecCsv)
    END_MESSAGE_MAP()


// CDlgClassificationST 메시지 처리기


void CDlgClassificationST::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


BOOL CDlgClassificationST::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_pDlgDefectGrid = new CDlgDefectGrid(this, &m_DefectInfoProcess);
	m_pDlgDefectGrid->Create(IDD_DIALOG_DEFECT_GRID, this);

	m_screenSizeX = GetSystemMetrics(SM_CXSCREEN);
	m_screenSizeY = GetSystemMetrics(SM_CYSCREEN);
	MoveWindow(0, 0, m_screenSizeX, m_screenSizeY);
	//ISI
	/*CRect rtTab;
	CRect rtTitle;

	m_ctrStaticClassification.GetWindowRect(rtTitle);
	m_ctrTab.MoveWindow(rtTitle.left, rtTitle.bottom, m_screenSizeX * 0.5, 22);
	m_ctrTab.GetWindowRect(rtTab);
	ScreenToClient(rtTab);
	CRect rtGrid(m_screenSizeX * 0.5, rtTitle.bottom, m_screenSizeX - 50, m_screenSizeY * 0.6);
	m_rtGrid = rtGrid;

	CRect rtFinalJudge(rtTitle.right + 500, rtTitle.top - 30, m_rtGrid.left, rtTitle.bottom);
	m_ctrStaticFinaljudge.MoveWindow(rtFinalJudge);

	CRect rtButton(m_rtGrid.left, m_rtGrid.bottom + 10, m_rtGrid.left + 150, m_rtGrid.bottom + 40);
	m_ctrBtnRefresh.MoveWindow(rtButton);

	CRect rtButtonApply(rtButton.right + 10, rtButton.top, rtButton.right + 160, rtButton.bottom);
	m_ctrBtnExist.MoveWindow(rtButtonApply);
	CRect rtButtonCancel(rtButtonApply.right + 10, rtButtonApply.top, rtButtonApply.right + 160, rtButtonApply.bottom);
	m_ctrBtnCancel.MoveWindow(rtButtonCancel);

	CRect rtButtonSpecInit(rtGrid.right - 120, rtGrid.top - 25, rtGrid.right, rtGrid.top);
	m_ctrBtnSpecInit.MoveWindow(rtButtonSpecInit);

	CRect rtTab2(rtGrid.left, rtGrid.top - 30, rtGrid.left + 500, rtGrid.top);
	m_ctrTab2.MoveWindow(rtTab2);

	CRect rtDefectGrid(rtButton.left, rtButton.bottom + 10, m_rtGrid.right, m_screenSizeY - 250);
	m_pDlgDefectGrid->MoveWindow(rtDefectGrid.left, rtDefectGrid.top, rtDefectGrid.Width(), rtDefectGrid.Height());
	m_pDlgDefectGrid->MoveWindows();

	CRect rtImage(rtDefectGrid.left, rtDefectGrid.bottom + 10, rtDefectGrid.right, m_screenSizeY - 50);
	CRect rtImage1 = rtImage;
	CRect rtImage2 = rtImage;
	CRect rtImage3 = rtImage;

	rtImage1.right = rtImage.left + rtImage.Width() * 0.3;
	rtImage2.left = rtImage1.right + 10;
	rtImage2.right = rtImage2.left + rtImage.Width() * 0.3;
	rtImage3.left = rtImage2.right + 10;
	rtImage3.right = rtImage3.left + rtImage.Width() * 0.3;

	m_ctrStatic_image1.MoveWindow(rtImage1);
	m_ctrStatic_image2.MoveWindow(rtImage2);
	m_ctrStatic_image3.MoveWindow(rtImage3);

	m_imageView_Bright  = new IPVM::ImageView(GetSafeHwnd(), IPVM::FromMFC(rtImage1));
	m_imageView_Dark = new IPVM::ImageView(GetSafeHwnd(), IPVM::FromMFC(rtImage2));
	m_imageView_Binary = new IPVM::ImageView(GetSafeHwnd(), IPVM::FromMFC(rtImage3));

	m_rtXTPFlowChart = CRect(rtTab.left, rtTab.bottom, rtTab.right - 20, m_screenSizeY - 50);
	m_wndControl.Create(WS_VSCROLL | WS_HSCROLL | WS_CHILD | WS_VISIBLE, m_rtXTPFlowChart, this, ID_FLOWGRAPH_CONTROL);

	CRect rtButtonFGZoom(rtFinalJudge.left - 300, rtFinalJudge.top, rtFinalJudge.left, rtFinalJudge.bottom);
	CRect rtButtonFGZoomFit(rtButtonFGZoom);
	CRect rtButtonFGZoomPlus(rtButtonFGZoom);
	CRect rtButtonFGZoomMinus(rtButtonFGZoom);

	rtButtonFGZoomFit.right = rtButtonFGZoom.left + rtButtonFGZoom.Width() * 0.3;
	rtButtonFGZoomPlus.left = rtButtonFGZoomFit.right + 10;
	rtButtonFGZoomPlus.right = rtButtonFGZoomPlus.left + rtButtonFGZoom.Width() * 0.3;
	rtButtonFGZoomMinus.left = rtButtonFGZoomPlus.right + 10;
	rtButtonFGZoomMinus.right = rtButtonFGZoomMinus.left + rtButtonFGZoom.Width() * 0.3;

	m_cntBtn_FgZoomFit.MoveWindow(rtButtonFGZoomFit);
	m_cntBtn_FgZoomPlus.MoveWindow(rtButtonFGZoomPlus);
	m_cntBtn_FgZoomMinus.MoveWindow(rtButtonFGZoomMinus);*/


	//임시 1.5
	CRect rtTab;
	CRect rtTitle;

	m_ctrStaticClassification.GetWindowRect(rtTitle);
	m_ctrTab.MoveWindow(rtTitle.left, rtTitle.bottom, m_screenSizeX * 0.3, 22);
	m_ctrTab.GetWindowRect(rtTab);
	ScreenToClient(rtTab);

	CRect rtUse(rtTitle.right + 30, rtTitle.top - 20, rtTitle.right + 200, rtTitle.bottom);
	m_ctrUse.MoveWindow(rtUse);

	CRect rtButtonInsArea(rtUse.right + 150, rtUse.top + 10, rtUse.right + 280, rtUse.bottom);
	m_btnInspectionArea.MoveWindow(rtButtonInsArea);


	CRect rtGrid(m_screenSizeX * 0.3, rtTitle.bottom, m_screenSizeX - 50, m_screenSizeY * 0.6);
	m_rtGrid = rtGrid;

	CRect rtButton(m_rtGrid.left, m_rtGrid.bottom + 10, m_rtGrid.left + 150, m_rtGrid.bottom + 40);
	m_ctrBtnRefresh.MoveWindow(rtButton);

	CRect rtButtonApply(rtButton.right + 10, rtButton.top, rtButton.right + 160, rtButton.bottom);
	m_ctrBtnExist.MoveWindow(rtButtonApply);
	CRect rtButtonCancel(rtButtonApply.right + 10, rtButtonApply.top, rtButtonApply.right + 160, rtButtonApply.bottom);
	m_ctrBtnCancel.MoveWindow(rtButtonCancel);

	CRect rtFinalJudge(rtButtonCancel.right + 10, rtButtonCancel.top, rtButtonCancel.right + 300, rtButtonCancel.bottom);
	m_ctrStaticFinaljudge.MoveWindow(rtFinalJudge);

	CRect rtButtonSpecInit(rtGrid.right - 120, rtGrid.top - 25, rtGrid.right, rtGrid.top);
	m_ctrBtnSpecInit.MoveWindow(rtButtonSpecInit);

	CRect rtButtonAdmin(rtGrid.right - 250, rtGrid.top - 25, rtGrid.right - 130, rtGrid.top);
	m_btn_useAdmin.MoveWindow(rtButtonAdmin);

	CRect rtButtonNormal(rtGrid.right - 380, rtGrid.top - 25, rtGrid.right - 260, rtGrid.top);
	m_btn_useNormal.MoveWindow(rtButtonNormal);
	
	CRect rtButtonSetPriUp(rtButtonNormal.left - 85, rtButtonNormal.top, rtButtonNormal.left - 5, rtButtonNormal.bottom);
    m_ctrBtnSetPriorityUp.MoveWindow(rtButtonSetPriUp);

	CRect rtButtonSetPriDown(rtButtonSetPriUp.left - 85, rtButtonNormal.top, rtButtonSetPriUp.left - 5, rtButtonNormal.bottom);
    m_ctrBtnSetPriorityDown.MoveWindow(rtButtonSetPriDown);

	CRect rtTab2(rtGrid.left, rtGrid.top - 30, rtGrid.left + 500, rtGrid.top);
	m_ctrTab2.MoveWindow(rtTab2);

	//24.05.31 장지수 Excel 저장 버튼 추가
    CRect reButtonExcelSave(rtButtonSpecInit.left, rtFinalJudge.top, rtButtonSpecInit.right, rtFinalJudge.bottom);
    m_ctrBtnSaveExcel.MoveWindow(reButtonExcelSave);

    CRect BtnSaveSpec(reButtonExcelSave.left - 120, reButtonExcelSave.top, reButtonExcelSave.left - 20, reButtonExcelSave.bottom);
    m_ctrBtnSaveSpec.MoveWindow(BtnSaveSpec);

     CRect BtnLoadSpec(BtnSaveSpec.left - 120, BtnSaveSpec.top, BtnSaveSpec.left - 20, BtnSaveSpec.bottom);
    m_ctrBtnLoadSpec.MoveWindow(BtnLoadSpec);

	CRect rtDefectGrid(rtButton.left, rtButton.bottom + 10, m_rtGrid.right, m_screenSizeY - 250);
	m_pDlgDefectGrid->MoveWindow(rtDefectGrid.left, rtDefectGrid.top, rtDefectGrid.Width(), rtDefectGrid.Height());
	m_pDlgDefectGrid->MoveWindows();

	CRect rtImage(rtDefectGrid.left, rtDefectGrid.bottom + 10, rtDefectGrid.right, m_screenSizeY - 50);
	CRect rtImage1 = rtImage;
	CRect rtImage2 = rtImage;
	CRect rtImage3 = rtImage;

	rtImage1.right = rtImage.left + rtImage.Width() * 0.3;
	rtImage2.left = rtImage1.right + 10;
	rtImage2.right = rtImage2.left + rtImage.Width() * 0.3;
	rtImage3.left = rtImage2.right + 10;
	rtImage3.right = rtImage3.left + rtImage.Width() * 0.3;

	m_ctrStatic_image1.MoveWindow(rtImage1);
	m_ctrStatic_image2.MoveWindow(rtImage2);
	m_ctrStatic_image3.MoveWindow(rtImage3);

	m_imageView_Bright = new IPVM::ImageView(GetSafeHwnd(), IPVM::FromMFC(rtImage1));
	m_imageView_Dark = new IPVM::ImageView(GetSafeHwnd(), IPVM::FromMFC(rtImage2));
	m_imageView_Binary = new IPVM::ImageView(GetSafeHwnd(), IPVM::FromMFC(rtImage3));

	m_rtXTPFlowChart = CRect(rtTab.left, rtTab.bottom, rtTab.right - 20, m_screenSizeY - 50);
	//m_wndControl.Create(WS_VSCROLL | WS_HSCROLL | WS_CHILD | WS_VISIBLE, m_rtXTPFlowChart, this, ID_FLOWGRAPH_CONTROL);

	CRect rtButtonFGZoom(rtFinalJudge.left - 300, rtFinalJudge.top, rtFinalJudge.left, rtFinalJudge.bottom);
	CRect rtButtonFGZoomFit(rtButtonFGZoom);
	CRect rtButtonFGZoomPlus(rtButtonFGZoom);
	CRect rtButtonFGZoomMinus(rtButtonFGZoom);

	rtButtonFGZoomFit.right = rtButtonFGZoom.left + rtButtonFGZoom.Width() * 0.3;
	rtButtonFGZoomPlus.left = rtButtonFGZoomFit.right + 10;
	rtButtonFGZoomPlus.right = rtButtonFGZoomPlus.left + rtButtonFGZoom.Width() * 0.3;
	rtButtonFGZoomMinus.left = rtButtonFGZoomPlus.right + 10;
	rtButtonFGZoomMinus.right = rtButtonFGZoomMinus.left + rtButtonFGZoom.Width() * 0.3;

	//m_cntBtn_FgZoomFit.ShowWindow(SW_HIDE);
	//m_cntBtn_FgZoomPlus.ShowWindow(SW_HIDE);
	//m_cntBtn_FgZoomMinus.ShowWindow(SW_HIDE);
	//m_wndControl.ShowWindow(SW_HIDE);


	CFont* pFont;
	pFont = new CFont();

	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(lf));
	wsprintf(lf.lfFaceName, _T("%s"), _T("Arial"));

	lf.lfHeight = 30;
	pFont->CreateFontIndirect(&lf);

	m_ctrStaticFinaljudge.SetFont(pFont);
	pFont->Detach();

	delete pFont;

	{
		CFont* pFont;
		pFont = new CFont();

		LOGFONT lf;
		::ZeroMemory(&lf, sizeof(lf));
		wsprintf(lf.lfFaceName, _T("%s"), _T("Arial"));

		lf.lfHeight = 30;
		pFont->CreateFontIndirect(&lf);

		m_ctrUse.SetFont(pFont);
		pFont->Detach();

		delete pFont;
	}


	std::vector<CString> vecTabTitle;
	//vecTabTitle.push_back(_T(" BACKGROUND_BURR "));
	//vecTabTitle.push_back(_T(" SCRAB "));
	//vecTabTitle.push_back(_T(" TAB "));
	//vecTabTitle.push_back(_T(" TAB_INSULATION "));
	//vecTabTitle.push_back(_T(" ELECTRODE_EDGE "));
	//vecTabTitle.push_back(_T(" ELECTRODE "));
	//vecTabTitle.push_back(_T(" END "));
	//
	//if (enInspectionArea::IA_END != vecTabTitle.size())
	//{
	//	//이넘 하고, 위에 vec 하고 짝을 맞춰야해...
	//	ASSERT(1);
	//}

	for (int i = 0; i < enInspectionArea::IA_TAB_END; i++)
	{
		vecTabTitle.push_back((CString)ConvertInspectionArea().GetString((enInspectionArea)i));
		m_ctrTab.InsertItem(i, vecTabTitle[i]);
	}

	m_pDlgDefectGrid->SelectDefectIndex(m_nSelectDefectIndex);

	/*m_pDlgDefectGrid->EnsureVisible(m_nSelectDefectIndex, TRUE);
    m_pDlgDefectGrid->SetFocus();*/
	m_ctrGrid.Create(rtGrid, this, MESSAGE_GRID);

	initGrid();

    
	//UpdateDefectGrid();
	//defect info
	//if(true == m_bValiddefect && m_nSelectDefectIndex < m_DefectInfoProcess.m_vecCandidate.size())
	//	*vecItem[vecItem.size() - 1].second = m_DefectInfoProcess.m_vecCandidate[m_nSelectDefectIndex];

	SetTab();

	CString strColumn;
	int nWidth;

	// Insert 3 columns in the list view control. Make the width of
	// the column be the width of the column header plus 50%.

	int col = 0;
	strColumn = _T("NO     ");
	nWidth = 3 * m_ctrList.GetStringWidth(strColumn) / 2;
	m_ctrList.InsertColumn(col++, strColumn, LVCFMT_LEFT, nWidth);

	strColumn = _T("NAME              .");
	nWidth = 3 * m_ctrList.GetStringWidth(strColumn) / 2;
	m_ctrList.InsertColumn(col++, strColumn, LVCFMT_LEFT, nWidth);

	strColumn = _T("DEFECT TYPE      ");
	nWidth = 3 * m_ctrList.GetStringWidth(strColumn) / 2;
	m_ctrList.InsertColumn(col++, strColumn, LVCFMT_LEFT, nWidth);

	strColumn = _T("LOGIC");
	nWidth = 3 * m_ctrList.GetStringWidth(strColumn) / 2;
	m_ctrList.InsertColumn(col++, strColumn, LVCFMT_LEFT, nWidth);

	strColumn = _T("NOTE                                       .");
	nWidth = 3 * m_ctrList.GetStringWidth(strColumn) / 2;
	m_ctrList.InsertColumn(col++, strColumn, LVCFMT_LEFT, nWidth);

	//CRect rtList(rtDefectGrid.left, rtDefectGrid.bottom + 10, rtDefectGrid.right, m_screenSizeY - 30);
	CRect rtList(m_rtXTPFlowChart);
	m_ctrList.MoveWindow(rtList);

	

	InitImages();
	updateLogic();

	m_ctrTab2.InsertItem(0, _T("Noraml Spec Table"));
	m_ctrTab2.InsertItem(1, _T("Small Spec Table"));
	SetGridType(0);

	DisplayFinalJudge(m_DefectInfoProcess.m_Judge.GetJudge(), m_DefectInfoProcess.m_Judge.GetDefectResultCode());

	m_ctrList.ShowWindow(SW_SHOW);

	((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(m_ISI_Spec_table.m_bUse);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgClassificationST::updateLogic()
{
	if (0 > m_nSelectDefectIndex || m_DefectInfoProcess.m_vecCandidate.size() <= m_nSelectDefectIndex)
		return;
	// 임시로 막자...
	ShowImage();
	m_ctrList.DeleteAllItems();

	if (0 > m_nSelectDefectIndex)
		return;

	CString strText;
	int nColumnCount = m_ctrList.GetHeaderCtrl()->GetItemCount();

	// Insert 10 items in the list view control.
	std::vector<enLogicResult> vecAnswerJunction;
	for (int i = 0; i < m_DefectInfoProcess.m_vecCandidate[m_nSelectDefectIndex].GetLogicStepNum(); i++)
	{
		strText.Format(TEXT("%d"), i);

		// Insert the item, select every other item.
		m_ctrList.InsertItem(LVIF_TEXT | LVIF_STATE, i, strText,
			(i % 2) == 0 ? LVIS_SELECTED : 0, LVIS_SELECTED, 0, 0);

		// Initialize the text of the subitems.
		long nKey = 0;
		classification::enLogicResult nNO0_YES1 = classification::enLogicResult::NONE;
		CString strNote;
		CString strDefectName;
		m_DefectInfoProcess.m_vecCandidate[m_nSelectDefectIndex].GetLogicStep(i, strDefectName, nKey, nNO0_YES1, strNote);

		m_ctrList.SetItemText(i, 1, strDefectName);
		m_ctrList.SetItemText(i, 2, LogicKey2String().GetString((enLogicKey)nKey));
        
		// 몇번째 조건인지 알려준다...
        //CString strCondition;
        //strCondition.Format(_T("조건_%d"), m_DefectInfoProcess.m_vecCandidate[m_nSelectDefectIndex].CONDITION_INDEX.GetValue());
        //m_ctrList.SetItemText(i, 3, strCondition);
		switch (nNO0_YES1)
		{
		case classification::NO:
			strText = _T("NO");
			vecAnswerJunction.push_back(classification::NO);
			break;
		case classification::YES:
			strText = _T("YES");
			vecAnswerJunction.push_back(classification::YES);
			break;
		case classification::NONE:
			strText = _T("");
			vecAnswerJunction.push_back(classification::NONE);
			break;
		default:
			break;
		}

		m_ctrList.SetItemText(i, 3, strText);

		m_ctrList.SetItemText(i, 4, strNote);
	}
	//DrawFlow(vecAnswerJunction);
}

void CDlgClassificationST::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ctrGrid.Invalidate();
	m_pDlgDefectGrid->m_ctrDefectGrid.Invalidate();

	CString textTab;
	TCITEM tcItem;
	tcItem.mask = TCIF_TEXT;
	tcItem.pszText = textTab.GetBuffer(256);
	tcItem.cchTextMax = 256;
	m_ctrTab.GetItem(m_ctrTab.GetCurSel(), &tcItem);
	textTab.ReleaseBuffer();

	/*if (m_ctrTab.GetCurSel() == 0)
	{
		m_wndControl.ShowWindow(SW_HIDE);
		m_ctrList.ShowWindow(SW_SHOW);
	}
	else
	{
		m_wndControl.ShowWindow(SW_SHOW);
		m_ctrList.ShowWindow(SW_HIDE);
	}

	if (textTab == _T("TAB"))
	{
		StartFlowChart();
	}
	else
	{
		if(m_wndControl.GetActivePage())
			m_wndControl.GetActivePage()->Remove();
	}*/
	* pResult = 0;
}

void CDlgClassificationST::OnTcnSelchangeTab2(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetGridType(m_ctrTab2.GetCurSel());
	*pResult = 0;
}

void CDlgClassificationST::SetGrid()
{
	if (m_nSelectDefectIndex < m_DefectInfoProcess.m_vecCandidate.size())
	{
		DefectType dt = m_DefectInfoProcess.m_vecCandidate[m_nSelectDefectIndex].GetDefectResultCode();

		if (DefectType::SmallDefect_CraterLevel1 <= dt && DefectType::SmallDefect_ScratchLevel5 >= dt)
		{
			m_ctrTab2.SetCurSel(1);
			SetGridType(1);

			return;
		}
	}
	m_ctrTab2.SetCurSel(0);	// 기본
	SetGridType(0);
}

void CDlgClassificationST::SetTab()
{
	if (m_nSelectDefectIndex < m_DefectInfoProcess.m_vecCandidate.size())
	{
		if (m_ctrTab.GetItemCount() > m_DefectInfoProcess.m_vecCandidate[m_nSelectDefectIndex].nInspectionArea)
		{
			m_ctrTab.SetCurSel(ConvertInspectionArea().InspArea2Tabindex(m_DefectInfoProcess.m_vecCandidate[m_nSelectDefectIndex].nInspectionArea));
			if (m_ctrTab.GetCurSel())
			{
				//m_wndControl.ShowWindow(SW_SHOW);
				//m_ctrList.ShowWindow(SW_HIDE);
			}
			else
			{
				//m_wndControl.ShowWindow(SW_HIDE);
				//m_ctrList.ShowWindow(SW_SHOW);
			}
		}
		else
		{
			ASSERT(1);
		}
	}
	m_ctrGrid.Invalidate();
}


void CDlgClassificationST::initGrid()
{
	long nRawNum = m_ISI_Spec_table.GetSpecCount() + 1;
	//long nColNum = 83; // 예진 -- 컬럼 넣을 때 수정
    long nColNum = 11+2; // 예진 -- 컬럼 넣을 때 수정

	m_ctrGrid.SetColumnCount(nColNum);
	m_ctrGrid.SetRowCount(nRawNum);
	m_ctrGrid.SetEditable(FALSE);
	m_ctrGrid.SetSingleColSelection(FALSE);
	m_ctrGrid.SetSingleRowSelection(FALSE);
	m_ctrGrid.SetFixedColumnSelection(FALSE);
	m_ctrGrid.SetFixedRowSelection(TRUE);
	m_ctrGrid.SetColumnResize(TRUE);
	m_ctrGrid.SetRowResize(TRUE);
	m_ctrGrid.SetFixedRowCount();
	m_ctrGrid.SetFixedColumnCount();


	/*0*/vecTitle.push_back(_T("         ITEM         "));
	/*1*/vecTitle.push_back(_T("USE"));
	/*2*/vecTitle.push_back(_T("ENABLE"));
    /*2*/vecTitle.push_back(_T("PRIORITY"));
	/*3*/vecTitle.push_back(_T("SIZE X(mm)"));
	/*4*/vecTitle.push_back(_T(">,<   "));
	/*5*/vecTitle.push_back(_T("SIZE Y(mm)"));
	/*6*/vecTitle.push_back(_T(">,<   "));
	/*7*/vecTitle.push_back(_T("OR/AND"));
    
	///*8*/vecTitle.push_back(_T("Area(㎟)"));
	///*9*/vecTitle.push_back(_T(">,<   "));
	///*10*/vecTitle.push_back(_T("Area(%)"));

	///*11*/vecTitle.push_back(_T(">,<   "));
	///*12*/vecTitle.push_back(_T("Small Insp"));
	///*12*/vecTitle.push_back(_T("Level"));
	/*12*/vecTitle.push_back(_T("Inspection Area"));
    vecTitle.push_back(_T("Condition Script"));

    ///*12*/ vecTitle.push_back(_T("Small Insp"));
    ///*12*/ vecTitle.push_back(_T("Level"));

    /**/ vecTitle.push_back(_T("COUNT"));
    /**/ vecTitle.push_back(_T("Parent              "));



	LinkSpecAndTable();

	for (int col = 1; col < nColNum; col++)
	{
		if (vecTitle.size() > col)
		{
			m_ctrGrid.SetItemText(0, col, vecTitle[col]);
		}
	}
    
	m_ctrGrid.AutoSizeColumns();

	for (int col = 0; col < nColNum; col++)
	{
		m_vecnSizeCol.push_back(m_ctrGrid.GetColumnWidth(col));
	}
    
	m_nSizeRow = m_ctrGrid.GetRowHeight(0);
}


BOOL CDlgClassificationST::CallBackGrid(GV_DISPINFO* pDisplay)
{// 현식 색 넣기
	int nrow = pDisplay->item.row - 1;
	int ncor = pDisplay->item.col;

	switch (pDisplay->item.row)
	{
	case 0:	//row : title 영역
		if (vecTitle.size() > ncor)
			pDisplay->item.strText = vecTitle[ncor];
		break;

	default: //row : data 영역 n개
		if (true)
		{
			if (vecItem_multi.size() <= nrow)
				break;
			
			// 검사 항목 추가 시 Priority 중복 방지
			// 맨처음 항목만 가져와서 Priority 값을 비교... 같은 Defect Name의 우선순위는 같음
            long nAddStartNum = 0;
			for (long n = 0; n < vecItem_multi.size(); n++)
            {
                for (long nCmp = n + 1; nCmp < vecItem_multi.size(); nCmp++)
                {
                    if (vecItem_multi[n].second->at(0).PRIORITY.GetValue() == vecItem_multi[nCmp].second->at(0).PRIORITY.GetValue())
                    {
                        vecItem_multi[nCmp].second->at(0).PRIORITY += 1;
                    }
				}
            }
			for (long nVal =0; nVal < vecItem_multi.size(); nVal++)
            {
				for (long nSub = 0; nSub< vecItem_multi[nVal].second->size(); nSub++)
                {
                    vecItem_multi[nVal].second->at(nSub).PRIORITY = vecItem_multi[nVal].second->at(0).PRIORITY;
				}
			}
			// 
            sort(vecItem_multi.begin(), vecItem_multi.end(), compare);

			// 검사 항목 추가 후 삭제하면 priority 하나씩 밀리는 현상 방지
            for (long n = 0; n < vecItem_multi.size(); n++)
            {
                if (vecItem_multi[n].second->at(0).PRIORITY.GetValue() != n)
                {
                    vecItem_multi[n].second->at(0).PRIORITY = n;
				} 
			}
			DisplayData(pDisplay, nrow, ncor);


		}        
	}

	if (0 < ncor)
	{
		// 0 : item 은 건들지 않는다. 
		if (_T("") == pDisplay->item.strText)
		{
			pDisplay->item.crBkClr = RGB(240, 240, 240);
		}


		if (enColTitle::USE == ncor ||
			enColTitle::ENABLE == ncor || 
			enColTitle::PRIORITY == ncor ||
			enColTitle::ORAND == ncor ||
			enColTitle::SIZE_X_CO == ncor ||
			enColTitle::SIZE_Y_CO == ncor ||
			enColTitle::RATIO_LW_CO == ncor ||
			enColTitle::COMPACT_CO == ncor ||
			enColTitle::SIZE_X == ncor ||
			enColTitle::SIZE_Y == ncor ||
			enColTitle::COMPACT == ncor ||
			enColTitle::RATIO_LW == ncor ||
			enColTitle::RATIO_WL == ncor ||
			enColTitle::RATIO_YX == ncor ||
			enColTitle::RATIO_XY == ncor ||
			enColTitle::AREA_Ratio == ncor ||
			enColTitle::LENGTH == ncor ||
			enColTitle::WIDTH == ncor ||
			enColTitle::WIDTH_CO == ncor ||
			//예진아 CO 넣어서 색 나오게 바꿔 주삼
			enColTitle::LENGTH_CO == ncor ||
			enColTitle::AREA_Ratio_CO == ncor ||
			enColTitle::RATIO_WL_CO == ncor ||
			enColTitle::RATIO_XY_CO == ncor ||
			enColTitle::RATIO_YX_CO == ncor ||
			enColTitle::ANGLE_CO == ncor ||
			enColTitle::STD_CO == ncor ||
			enColTitle::ANGLE == ncor ||
			enColTitle::STD == ncor ||
			enColTitle::INSPECTION_IMAGE_TYPE == ncor ||
			enColTitle::DEFECT_BRIGHTNESS == ncor ||
			enColTitle::BLOB_THRESHOLD == ncor ||
			enColTitle::BLOB_THRESHOLD_CO == ncor ||
			enColTitle::CATEGORY == ncor ||
			enColTitle::MIN == ncor ||
			enColTitle::MIN_CO == ncor ||
			enColTitle::AVG == ncor ||
			enColTitle::AVG_CO == ncor ||
			enColTitle::MAX == ncor ||
			enColTitle::MAX_CO == ncor ||
			enColTitle::BRIGHT_AVG == ncor ||
			enColTitle::BRIGHT_AVG_CO == ncor ||
			enColTitle::DARK_AVG == ncor ||
			enColTitle::DARK_AVG_CO == ncor ||
			enColTitle::MINIMUM_SIZE_X == ncor ||
			enColTitle::MINIMUM_SIZE_Y == ncor ||
			enColTitle::INSPECTION_AREA == ncor ||
			enColTitle::COUNT == ncor ||
			enColTitle::RATIO_WHITE == ncor ||
			enColTitle::RATIO_WHITE_CO == ncor ||
			enColTitle::RATIO_DARK == ncor ||
			enColTitle::RATIO_DARK_CO == ncor ||
			enColTitle::EDGE_ENERGY == ncor ||
			enColTitle::EDGE_ENERGY_CO == ncor || 
			enColTitle::BLACK_EDGE_ENERGY == ncor ||
			enColTitle::BLACK_EDGE_ENERGY_CO == ncor ||
			enColTitle::WHITE_EDGE_ENERGY == ncor ||
			enColTitle::WHITE_EDGE_ENERGY_CO == ncor ||
			enColTitle::SECTION_THRESHOLD == ncor|| 
			enColTitle::SECTION_THRESHOLD_CO == ncor||
			enColTitle::MASS_CENTERGV == ncor ||
			enColTitle::MASS_CENTERGV_CO == ncor ||
			enColTitle::HOR == ncor ||
			enColTitle::HOR_CO == ncor ||
			enColTitle::VER == ncor ||
			enColTitle::VER_CO == ncor ||
			enColTitle::RATIO_HV == ncor ||
			enColTitle::RATIO_HV_CO == ncor ||
			enColTitle::AREA == ncor ||
			enColTitle::AREA_CO == ncor ||
			enColTitle::RATIO_VH == ncor ||
			enColTitle::RATIO_VH_CO == ncor||
			enColTitle::SMALL_INSP == ncor||
			enColTitle::SMALL_INSP_LEVEL == ncor|| 
			enColTitle::PARENT == ncor ||
			enColTitle::OVER_GV_PX_COUNT == ncor ||
			enColTitle::OVER_GV_PX_COUNT_CO == ncor ||
			enColTitle::DIS_TO_EDGE_mm == ncor ||
			enColTitle::DIS_TO_EDGE_mm_CO == ncor || 
			enColTitle::DIS_TO_EDGE_2_mm == ncor|| 
			enColTitle::DIS_TO_EDGE_2_mm_CO == ncor ||
			enColTitle::IS_NEAR_EDGE==ncor|| 
			enColTitle::IS_NEAR_EDGE_CO == ncor
			)
		{

		}
		else
		{
			pDisplay->item.crBkClr = RGB(240, 240, 240);
		}
	}

	return true;
}


void CDlgClassificationST::DisplayData(GV_DISPINFO* pDisplay, long nrow, long ncor)
{
	if (0/*item*/ == ncor)
	{
		CString strName = GetDefectName(vecItem_multi[nrow].first);
		pDisplay->item.strText = strName;

		COLORREF RGB_BK;
		BOOL binclude = BackGroundColor(nrow, RGB_BK);
		pDisplay->item.crBkClr = RGB_BK;

		//defect info
		if (0 <= m_nSelectDefectIndex && true == binclude)
		{
			if (m_nSelectDefectIndex < m_DefectInfoProcess.m_vecCandidate.size() && vecItem_multi[nrow].first == m_DefectInfoProcess.m_vecCandidate[m_nSelectDefectIndex].GetDefectResultCode())
				pDisplay->item.crBkClr = DefectBackGroundColor(m_DefectInfoProcess.m_vecCandidate[m_nSelectDefectIndex].GetJudge());
		}
        if (m_nDefectSelect > 0)
        {
            if (nrow > vecItem_multi.size())
            {
                CString str;
                CString strNum = _T("_");
                str = GetDefectName(vecItem_multi[nrow].first) + strNum;
                pDisplay->item.strText = str;
			}

		}
	}
	// --
	else if (enColTitle::USE == ncor)
	{
		pDisplay->item.strText = Enable2String(vecItem_multi[nrow].second->at(0).USE.GetValue());
	}
	else if (enColTitle::ENABLE == ncor)
	{
        pDisplay->item.strText = Enable2String(vecItem_multi[nrow].second->at(0).ENABLE.GetValue());
		//pDisplay->item.crBkClr = BackGroundColor(nrow);
	}
    else if (enColTitle::PRIORITY == ncor)
    {
        DisplayItemNumber(pDisplay, vecItem_multi[nrow].second->at(0).PRIORITY.GetValue() + 1, _T("%d"), vecItem_multi[nrow].second->at(0).GetUsePriority(), nrow);
    }
	else if (enColTitle::SIZE_X == ncor)
	{
        DisplayItemNumber<float>(pDisplay, vecItem_multi[nrow].second->at(0).SIZE_X_mm.GetValue(), _T("%.3f"), vecItem_multi[nrow].second->at(0).GetUseSize(), nrow);
	}
    else if (enColTitle::SIZE_X_CO /**/ == ncor && DefectType::Defect_Info != vecItem_multi[nrow].first)
	{
        DisplayItemCompare(pDisplay, (enCompareOperator)vecItem_multi[nrow].second->at(0).SIZEX_COMPARE_OPERATOR.GetValue(), vecItem_multi[nrow].second->at(0).GetUseSize(), nrow);
	}
	else if (enColTitle::SIZE_Y/*SIZE Y mm*/ == ncor)
	{
        DisplayItemNumber<float>(pDisplay, vecItem_multi[nrow].second->at(0).SIZE_Y_mm.GetValue(), _T("%.3f"), vecItem_multi[nrow].second->at(0).GetUseSize(), nrow);
	}
    else if (enColTitle::SIZE_Y_CO /**/ == ncor && DefectType::Defect_Info != vecItem_multi[nrow].first)
	{
        DisplayItemCompare(pDisplay, (enCompareOperator)vecItem_multi[nrow].second->at(0).SIZEY_COMPARE_OPERATOR.GetValue(), vecItem_multi[nrow].second->at(0).GetUseSize(), nrow);
	}
    else if (enColTitle::ORAND /**/ == ncor && DefectType::Defect_Info != vecItem_multi[nrow].first)
	{
        if (vecItem_multi[nrow].second->at(0).GetUseSize())
		{
            pDisplay->item.strText = enumtoString().OrAnd2String(vecItem_multi[nrow].second->at(0).ORAND.GetValue());
		}
	}

	else if (enColTitle::AREA/**/ == ncor)
	{
        pDisplay->item.strText = InspectionArea2String((enInspectionArea)vecItem_multi[nrow].second->at(0).nInspectionArea.GetValue());

	}
    else if (enColTitle::AREA_CO/**/ == ncor)
	{
        //DisplayItemCompare(pDisplay, (enCompareOperator)vecItem[nrow].second->AREA_COMPARE_OPERATOR.GetValue(),
        //    vecItem[nrow].second->GetUseArea(), nrow);
        //pDisplay->item.strText = InitDisplayCondition(nrow);
        std::vector<CString> vecTotalCondition;
        vecTotalCondition = SelectDispCondition(nrow);
        CString strTotal;
		for (long nCondition = 0; nCondition < vecTotalCondition.size(); nCondition++)
        {
            if (nCondition == vecTotalCondition.size() -1 )
            {
                strTotal += vecTotalCondition[nCondition];
			}
			else
            {
                strTotal += vecTotalCondition[nCondition] + _T(" |(OR) ");
			}
            
		}
        pDisplay->item.strText = strTotal;
        //if (m_bUpdate == TRUE)
        //{
        //    strTemp = SelectDispCondition(m_nDbSelectRow);

        //    if (nrow < strTemp.size())
        //    {
        //        pDisplay->item.strText = strTemp[nrow];
        //    }
        //    else
        //    {
        //        pDisplay->item.strText = _T("");
        //    }
        //}
        //else
        //{
        //    pDisplay->item.strText = InitDisplayCondition(nrow);
        //}
	}
    else if (enColTitle::COUNT == ncor)
    {
        DisplayItemNumber<long>(pDisplay, vecItem_multi[nrow].second->at(0).COUNT.GetValue(), _T("%d"),
            vecItem_multi[nrow].second->at(0).GetUseCount(), nrow);
    }
    else if (enColTitle::PARENT == ncor)
    {
        if (vecItem_multi[nrow].second->at(0).GetUseParent())
        {
            pDisplay->item.strText = enumtoString().Parent2String(
                (enParent)vecItem_multi[nrow].second->at(0).PARENT_COMPARE_OPERATOR.GetValue());
        }
    }

	//defect info
	/*if (vecItem.size() - 1 == nrow && true == m_bValiddefect)
	{
		if (m_nSelectDefectIndex < m_DefectInfoProcess.m_vecCandidate.size())
			pDisplay->item.crBkClr = DefectBackGroundColor(m_DefectInfoProcess.m_vecCandidate[m_nSelectDefectIndex].GetJudge());
	}*/
}




bool CDlgClassificationST::DisplayItemCompare(GV_DISPINFO* pDisplay, enCompareOperator compare, bool bUse, long nrow)
{
	if (true == bUse)
	{
		pDisplay->item.strText = enumtoString().CompareOperator2String(compare);
		//pDisplay->item.crBkClr = BackGroundColor(nrow);
	}

	return bUse;
}


bool CDlgClassificationST::BackGroundColor(long nrow, COLORREF& RGB)
{
	BOOL binclude = false;
    for (int i = 0; i < vecItem_multi[nrow].second->at(0).vecTabInfo.size(); i++)
	{
        if (vecItem_multi[nrow].second->at(0).vecTabInfo[i] == m_ctrTab.GetCurSel())
		{
			binclude = true;
			break;
		}
	}
	if (true == binclude)
		RGB = RGB(255, 255, 255);
	else
		RGB = RGB(200, 200, 200);

	return binclude;
}

COLORREF CDlgClassificationST::DefectBackGroundColor(II_RESULT_VALUE result)
{
	switch (result)
	{
	case PASS:
		return RGB(128, 255, 128);
		break;
	case REWORK:
		break;
	case REJECT:
		return RGB(255, 128, 128);
		break;
	case INVALID:
		break;
	case NOT_MEASURED:
		return RGB(128, 128, 255);
		break;
	default:
		break;
	}

	return RGB(255, 255, 255);
}


BOOL CDlgClassificationST::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	if (MESSAGE_GRID == wParam)
	{
		//grid event
		NM_GRIDVIEW* nmgv = (NM_GRIDVIEW*)lParam;

		int nCol = nmgv->iColumn;
		int nRaw = nmgv->iRow;

		nRaw = nRaw - 1;

		switch (nmgv->hdr.code)
		{
		case GVN_SELCHANGING:
			break;

		case GVN_SELCHANGED:
			break;

		case NM_DBLCLK:
		{
			NextOrAnd(nRaw, nCol);
			NextCompareOperator(nRaw, nCol);
			EditGridText(nRaw, nCol);
			NextInspectionImgType(nRaw, nCol);
			NextAreaOperator(nRaw, nCol);
            
            
            
			if (nCol == 0 || nCol == enColTitle::AREA_CO)
            {
                if (nRaw == -1)
                {
                    m_ctrGrid.Invalidate();
                    break;
				}
                CDlgClassificationCondition* dlg = new CDlgClassificationCondition(this, vecItem_multi[nRaw].first);
                dlg->Create(IDD_DIALOG_CLASSIFICATION_CONDITION, this);
                dlg->CenterWindow(CWnd::GetDesktopWindow());
                dlg->ShowWindow(SW_SHOW);	

			}
            m_ctrGrid.Invalidate();
			//SendMessage(UWM_UPDATE_GRID);
		}
		break;

		case NM_CLICK:
            m_nDefectSelect = nRaw + 1;
			TogeterValue(nRaw, nCol);
            m_ctrGrid.SetItemState(nRaw + 1, nCol, GVNI_FOCUSED | GVIS_SELECTED);
			m_ctrGrid.Invalidate();

			break;

		case NM_RCLICK:
			break;

		case GVN_BEGINLABELEDIT:
			break;

		case GVN_ENDLABELEDIT:
			break;

		default:
			break;
		}

	}

	return CDialogEx::OnNotify(wParam, lParam, pResult);
}


void CDlgClassificationST::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CDialogEx::OnChar(nChar, nRepCnt, nFlags);
}


void CDlgClassificationST::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CDlgClassificationST::NextOrAnd(int nRow, int nCol)
{
	if (enColTitle::USE == nCol)
	{
		if (vecItem_multi.size() > nRow)
		{
			std::vector<CISISpecInfo>* vecSpec = m_ISI_Spec_table.GetSpecInfo_Multi(vecItem_multi[nRow].first);
			for (long n = 0; n < vecItem_multi[nRow].second->size(); n++)
            {
                bool bUse = (*vecSpec)[n].USE.GetValue();
                (*vecSpec)[n].USE = !bUse;
			}
		}
	}
	if (enColTitle::ENABLE == nCol)
	{
		if (vecItem_multi.size() > nRow)
		{
            std::vector<CISISpecInfo>* vecSpec = m_ISI_Spec_table.GetSpecInfo_Multi(vecItem_multi[nRow].first);
            for (long n = 0; n < vecItem_multi[nRow].second->size(); n++)
            {
                bool bUse = (*vecSpec)[n].ENABLE.GetValue();
                (*vecSpec)[n].ENABLE = !bUse;
            }
		}
	}

	if (enColTitle::ORAND == nCol)
	{
		if (vecItem_multi.size() > nRow)
		{
            std::vector<CISISpecInfo>* vecSpec = m_ISI_Spec_table.GetSpecInfo_Multi(vecItem_multi[nRow].first);
            for (long n = 0; n < vecItem_multi[nRow].second->size(); n++)
            {
                bool bUse = (*vecSpec)[n].ORAND.GetValue();
                (*vecSpec)[n].ORAND = !bUse;
            }
		}


	}
  //  if (enColTitle::SMALL_INSP == nCol)
  //  {
  //      if (vecItem_multi.size() > nRow)
  //      {
  //          std::vector<CISISpecInfo>* vecSpec = m_ISI_Spec_table.GetSpecInfo_Multi(vecItem_multi[nRow].first);
  //          for (long n = 0; n < vecItem_multi[nRow].second->size(); n++)
  //          {
  //              bool bUse = (*vecSpec)[n].SMALL_INSP.GetValue();
  //              (*vecSpec)[n].SMALL_INSP = !bUse;
  //          }
		//}

  //  }

	//if (enColTitle::DEFECT_BRIGHTNESS == nCol)
	//{
	//	if (vecItem.size() > nRow)
	//	{
	//		bool bDefectBrightness = vecItem[nRow].second->nDefectBrightness.GetValue();
	//		vecItem[nRow].second->nDefectBrightness = !bDefectBrightness;
	//	}
	//}
}


void CDlgClassificationST::NextInspectionImgType(int nRow, int nCol)
{

	BOOL bUse = false;
	if (vecItem_multi.size() <= nRow)
	{
		ASSERT(1);
		return;
	}
	Base::ParaLong* pValue = nullptr;
	//switch (nCol)
	//{
	//case enColTitle::INSPECTION_IMAGE_TYPE:
	//{
	//	pValue = &vecItem[nRow].second->INSPECTION_IMAGE_TYPE;
	//	bUse = true;
	//}
	//if (nullptr != pValue && true == bUse)
	//{
	//	*pValue = (pValue->GetValue() + 1) % enInspectionImageType::INS_END;
	//}
	//break;

	//case enColTitle::DEFECT_BRIGHTNESS:
	//{
	//	pValue = &vecItem[nRow].second->nDefectBrightness;
	//	bUse = true;
	//}
	//if (nullptr != pValue && true == bUse)
	//{
	//	*pValue = (pValue->GetValue() + 1) % enDefectBrightness::DFB_NO_USE;
	//}
	//break;
	//}
}

void CDlgClassificationST::NextCompareOperator(int nRow, int nCol)
{

	BOOL bUse = false;
	if (vecItem_multi.size() <= nRow)
	{
		ASSERT(1);
		return;
	}

	//Base::ParaLong* pValue = nullptr;
    Base::ParaLong* pValue_Multi = nullptr;
    std::vector<Base::ParaLong*> vecpValue;
	//현식 동작을 원한다면 이걸 바꿔야 한다.
	// 예진아 이거 넣어서 동작 되도록 바꿔야 한다...
	switch (nCol)
	{
	case enColTitle::SIZE_X_CO:
	{
		std::vector<CISISpecInfo>* vecSpec = m_ISI_Spec_table.GetSpecInfo_Multi(vecItem_multi[nRow].first);
        for (long n = 0; n < vecItem_multi[nRow].second->size(); n++)
        {
            pValue_Multi = &(*vecSpec)[n].SIZEX_COMPARE_OPERATOR;
            vecpValue.push_back(pValue_Multi);
        }
        bUse = true;
	}
	break;
	case enColTitle::SIZE_Y_CO:
	{
		std::vector<CISISpecInfo>* vecSpec = m_ISI_Spec_table.GetSpecInfo_Multi(vecItem_multi[nRow].first);
        for (long n = 0; n < vecItem_multi[nRow].second->size(); n++)
        {
            pValue_Multi = &(*vecSpec)[n].SIZEY_COMPARE_OPERATOR;
            vecpValue.push_back(pValue_Multi);
        }
        bUse = true;
	}
	break;
	default:
		break;
	}

	if (nullptr != pValue_Multi && true == bUse)
	{
        *pValue_Multi = (pValue_Multi->GetValue() + 1) % enCompareOperator::CO_END;

		for (long nValue = 0; nValue < vecpValue.size(); nValue++)
        {
            *vecpValue[nValue] = (vecpValue[nValue]->GetValue() + 1) % enCompareOperator::CO_END;
		}
	}
}

// 예진 INSPECTION_AREA
void CDlgClassificationST::NextAreaOperator(int nRow, int nCol)
{

	BOOL bUse = false;
	if (vecItem_multi.size() <= nRow)
	{
		ASSERT(1);
		return;
	}

	Base::ParaLong* pValue = nullptr;
	//현식 동작을 원한다면 이걸 바꿔야 한다.
	// 예진아 이거 넣어서 동작 되도록 바꿔야 한다...
    //Base::ParaFloat* pValue = nullptr;
    std::vector<Base::ParaLong*> vecpValue;
    std::vector<CISISpecInfo>* vecSpec = m_ISI_Spec_table.GetSpecInfo_Multi(vecItem_multi[nRow].first);
	switch (nCol)
	{
        case enColTitle::AREA:
        {
            for (long n = 0; n < vecItem_multi[nRow].second->size(); n++)
            {
                pValue = &(*vecSpec)[n].nInspectionArea;
                vecpValue.push_back(pValue);
            }
            bUse = true;
        }
        break;
        case enColTitle::PARENT:
        {
            for (long n = 0; n < vecItem_multi[nRow].second->size(); n++)
            {
                pValue = &(*vecSpec)[n].PARENT_COMPARE_OPERATOR;
                vecpValue.push_back(pValue);
            }
            bUse = true;
        }
        break;
	}

	if (nCol == enColTitle::INSPECTION_AREA &&nullptr != pValue && true == bUse)
	{
        *pValue = (pValue->GetValue() + 1) % enInspectionArea::IA_TAB_END;
	}
    if (nCol == enColTitle::AREA && nullptr != pValue && true == bUse)
    {
        *pValue = (pValue->GetValue() + 1) % enInspectionArea::IA_TAB_END;
        for (long nVal = 0; nVal < vecpValue.size(); nVal++)
        {
            *vecpValue[nVal] = (vecpValue[nVal]->GetValue() + 1) % enInspectionArea::IA_TAB_END;
		}
    }
	if (nCol == enColTitle::SMALL_INSP_LEVEL && nullptr != pValue && true == bUse)
    {
        *pValue = (pValue->GetValue() + 1) % enSmallInsplevel::end;
    }
	
	if (nCol == enColTitle::PARENT && nullptr != pValue && true == bUse)
    {
        *pValue = (pValue->GetValue() + 1) % enParent::End;
    }

}

void CDlgClassificationST::TogeterValue(int nRow, int nCol)
{
	if (vecItem_multi.size() <= nRow)
		return;

	m_mutex.Lock();
	m_vecTogeter = m_ISI_Spec_table.GetValueTogether(vecItem_multi[nRow].first, (enColTitle)nCol);
	m_nSelectCol = nCol;
	m_mutex.Unlock();
}

void CDlgClassificationST::EditGridText(int nRow, int nCol)
{
	if (vecItem_multi.size() <= nRow)
	{
		ASSERT(1);
		return;
	}

	BOOL bUse = false;
    BOOL bUse_Multi = false;
    {
        Base::ParaFloat* pValue = nullptr;
        std::vector<Base::ParaFloat*> vecpValue;
        std::vector<CISISpecInfo>* vecSpec = m_ISI_Spec_table.GetSpecInfo_Multi(vecItem_multi[nRow].first);
        switch (nCol)
        {
            case enColTitle::SIZE_X:
                for (long n = 0; n < vecItem_multi[nRow].second->size(); n++)
                {
                    pValue = &(*vecSpec)[n].SIZE_X_mm;
                    vecpValue.push_back(pValue);
                }
                bUse_Multi = true;
                break;

            case enColTitle::SIZE_Y:
                for (long n = 0; n < vecItem_multi[nRow].second->size(); n++)
                {
                    pValue = &(*vecSpec)[n].SIZE_Y_mm;
                    vecpValue.push_back(pValue);
                }
                bUse_Multi = true;
                break;

            default:
                break;
        }
        if (nullptr != pValue && true == bUse_Multi)
        {
            CDlgEditGridText dlg(GetDefectName(vecItem_multi[nRow].first),
                Numeral2String(_T("%.3f"), pValue->GetValue()), GetCellPoint(nRow, nCol), this);
            if (dlg.DoModal() == IDOK)
            {
                *pValue = _wtof(dlg.m_strValue);
                for (long nVal = 0; nVal < vecpValue.size(); nVal++)
                {
                    *vecpValue[nVal] = _wtof(dlg.m_strValue);
                }

                m_ISI_Spec_table.SetValueTogether(vecItem_multi[nRow].first, (enColTitle)nCol, _wtof(dlg.m_strValue));
            }
        }
    }

    {
        Base::ParaLong* pValue = nullptr;
        std::vector<CISISpecInfo>* vecSpec = m_ISI_Spec_table.GetSpecInfo_Multi(vecItem_multi[nRow].first);
        switch (nCol)
        {
            case enColTitle::COUNT:
                // for (long n = 0; n < vecItem_multi[nRow].second->size(); n++)
                {
                    pValue = &(*vecSpec)[0].COUNT;
                    //vecpValue.push_back((float)pValue);
                }
                bUse = true;

                break;

            default:
                break;
        }
        if (nullptr != pValue && true == bUse)
        {
            CDlgEditGridText dlg(GetDefectName(vecItem_multi[nRow].first),
                Numeral2String(_T("%d"), pValue->GetValue()),
                GetCellPoint(nRow, nCol), this);
            if (dlg.DoModal() == IDOK)
            {
                *pValue = _wtoi(dlg.m_strValue);

                //m_ISI_Spec_table.SetValueTogether(vecItem[nRow].first, (enColTitle)nCol, _wtoi(dlg.m_strValue));
            }
        }
    }
        
		
	
}

CPoint CDlgClassificationST::GetCellPoint(int nRow, int nCol)
{
	int nScrollPosX = m_ctrGrid.GetScrollPos(0);

	CRect roi;

	long nWidth = 0;
	for (int c = 0; c < nCol; c++)
	{
		m_ctrGrid.GetCellRect(nRow, c, LPRECT(roi));

		nWidth += roi.Width();
	}

	long nHeight = 0;
	for (int h = 0; h < nRow; h++)
	{
		m_ctrGrid.GetCellRect(h, 0, LPRECT(roi));
		nHeight += roi.Height();
	}

	CRect gridrect;
	m_ctrGrid.GetWindowRect(gridrect);

	CPoint cp(nWidth - nScrollPosX, nHeight + m_rtGrid.top);
	cp.x += gridrect.left;
	return cp;
}


void CDlgClassificationST::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    //SpecTableTrans();
	CDialogEx::OnOK();
}


void CDlgClassificationST::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();

}

void CDlgClassificationST::OnBnClickedBtnFgzoomfit()
{
	m_wndControl.ZoomFitToWindow();
}

void CDlgClassificationST::OnBnClickedBtnFgzoomplus()
{
	double zoomLevel = m_wndControl.GetZoomLevel();
	if (zoomLevel >= 1.)
		zoomLevel += 0.5;
	else
		zoomLevel += 0.1;
	m_wndControl.SetZoomLevel(zoomLevel);
}

void CDlgClassificationST::OnBnClickedBtnFgzoomminus()
{
	double zoomLevel = m_wndControl.GetZoomLevel();
	if (zoomLevel >= 1.5)
		zoomLevel -= 0.5;
	else
		zoomLevel -= 0.1;
	m_wndControl.SetZoomLevel(zoomLevel);
}

void CDlgClassificationST::OnBnClickedBtnReFresh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_DefectInfoProcess = m_DefectInfoOrg;
	m_classfication.Start(&m_DefectInfoProcess,true);

	updateLogic();

	SetTab();
	SetGrid();

	DisplayFinalJudge(m_DefectInfoProcess.m_Judge.GetJudge(), m_DefectInfoProcess.m_Judge.GetDefectResultCode());
    
	m_pDlgDefectGrid->ReFresh();

}

void CDlgClassificationST::DisplayFinalJudge(II_RESULT_VALUE Result, DefectType dt)
{
	CString str = _T("[ none ] ");
	switch (Result)
	{
	case PASS:
		str = _T("[ OK ] ");
		break;
	case REWORK:
		break;
	case REJECT:
		str = _T("[ NG ] ");
		break;
	case INVALID:
		break;
	case NOT_MEASURED:
		break;
	default:
		break;
	}

	if (DefectType::SmallDefect_CraterLevel1 <= dt && DefectType::SmallDefect_ScratchLevel5 >= dt)
	{
		str += _T("[ ");
		str += GetDefectName(dt);
		str += _T(" ] ");
	}


	SetDlgItemText(IDC_STATIC_FINAL_JUDGE, str);
}

void CDlgClassificationST::ShowImage()
{
	if (0 > m_nSelectDefectIndex || m_DefectInfoProcess.m_vecCandidate.size() <= m_nSelectDefectIndex)
		return;

	auto& Defect = m_DefectInfoProcess.m_vecCandidate[m_nSelectDefectIndex];
	IPVM::Rect rect = Defect.rtRect_pixel;

	int OriginRect_Width = rect.Width();
	int OriginRect_Height = rect.Height();
	int Offset_X = 0;
	int Offset_Y = 0;
	if (100 > max(rect.Width(), rect.Height()))
	{
		Offset_X = 50;
		Offset_Y = 50;
	}
	else if (rect.Width() > rect.Height())
	{
		int Diff = rect.Width() - rect.Height();
		int Diff_half = Diff / 2;
		Offset_Y = Diff_half;
	}
	else
	{
		int Diff = rect.Height() - rect.Width();
		int Diff_half = Diff / 2;
		Offset_X = Diff_half;
	}
	rect.InflateRect(Offset_X, Offset_Y, Offset_X, Offset_Y);
	rect &= IPVM::Rect(*m_DefectInfoProcess.m_pSrcBright);

	int offset_left = Defect.rtRect_pixel.m_left - rect.m_left;
	int offset_top = Defect.rtRect_pixel.m_top - rect.m_top;
	int offset_right = Defect.rtRect_pixel.m_right - rect.m_right;
	int offset_bottom = Defect.rtRect_pixel.m_bottom - rect.m_bottom;
	IPVM::Image_8u_C1 CropImage_Bright(rect.Width(), rect.Height());
	IPVM::Image_8u_C1 CropImage_Dark(rect.Width(), rect.Height());
	IPVM::Rect OriginRect(CropImage_Bright);
	OriginRect.DeflateRect(offset_left, offset_top, -offset_right, -offset_bottom);

	IPVM::Image_8u_C1* OriginImage_Bright;
	OriginImage_Bright = m_DefectInfoProcess.m_pSrcBright;
	m_imageView_Bright->ImageOverlayClear();
	IPVM::ImageProcessing::Copy(*OriginImage_Bright, rect, IPVM::Rect(CropImage_Bright), CropImage_Bright);
	m_imageView_Bright->SetImage(CropImage_Bright);
	m_imageView_Bright->ImageOverlayAdd(OriginRect, RGB(0, 255, 0));
	m_imageView_Bright->ImageOverlayShow();
    long nX = CropImage_Bright.GetSizeX();
    long nY = CropImage_Bright.GetSizeY();
    //CropImage_Bright.SaveJpeg(_T("D:\\123.jpg"));
	IPVM::Image_8u_C1* OriginImage_Dark;
	OriginImage_Dark = m_DefectInfoProcess.m_pSrcDark;
	m_imageView_Dark->ImageOverlayClear();
	IPVM::ImageProcessing::Copy(*OriginImage_Dark, rect, IPVM::Rect(CropImage_Dark), CropImage_Dark);
	m_imageView_Dark->SetImage(CropImage_Dark);
	m_imageView_Dark->ImageOverlayAdd(OriginRect, RGB(0, 255, 0));
	m_imageView_Dark->ImageOverlayShow();

	m_imageView_Binary->SetImage(Defect.srcImage);
}

void CDlgClassificationST::LinkSpecAndTable()
{
	vecItem_multi.clear();

	for (int n = 0; n < (long)DefectType::ISI_END; n++)
	{
        std::vector<CISISpecInfo>* spec = m_ISI_Spec_table.GetSpecInfo_Multi((DefectType)n);
        CString str = GetDefectName((DefectType)n);
        if (nullptr != spec)
        {
            vecItem_multi.push_back(std::make_pair((DefectType)n, spec));
		}
        
	}
}


void CDlgClassificationST::OnBnClickedBtnInitSpec()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// A message box that uses a string from a string table
	// with yes and no buttons and the stop icon.
	// NOTE: nStringID is an integer that contains a valid id of
	// a string in the current resource.

	m_vecTogeter = nullptr;

	if (IDYES == AfxMessageBox(_T("Do you want to reset? (YES/NO)"), MB_YESNO | MB_ICONSTOP))
	{
		m_ISI_Spec_table.initSpec();

		LinkSpecAndTable();

		OnBnClickedBtnReFresh();
	}
}


BOOL CDlgClassificationST::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (VK_RETURN || VK_ESCAPE == pMsg->wParam)
	{
		return FALSE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDlgClassificationST::SetGridType(bool bSmallDefect)
{
	switch (m_use_mode)
	{
	case 0:
            //for (int col = 1; col < enColTitle::CT_END; col++)
        for (int col = 1; col < enColTitle::AREA_Ratio; col++)
		{
			if (true == bSmallDefect)
			{
                m_ctrGrid.SetColumnWidth(0, 200);
				if (true == isSmallDefectUsingCol((enColTitle)col))
                {
                    m_ctrGrid.SetColumnWidth(col, m_vecnSizeCol[col]);
                    if ((enColTitle)col == (enColTitle)ITEM)
                    {
                        m_ctrGrid.SetColumnWidth(col, 200);
					}
				}
				else
					m_ctrGrid.SetColumnWidth(col, 0);
			}
			else
			{

				m_ctrGrid.SetColumnWidth(col, m_vecnSizeCol[col]);
                if (col == (enColTitle)AREA_CO)
				{
                    m_ctrGrid.SetColumnWidth(col, 1000);
				}
				if (col == (enColTitle)MINIMUM_SIZE_X || col == (enColTitle)MINIMUM_SIZE_Y || col == (enColTitle)COUNT || col == (enColTitle)PARENT)
				{
					m_ctrGrid.SetColumnWidth(col, 0);
				}

			}
		}
		m_ctrGrid.SetColumnWidth(1, 0);
		for (int row = 0; row < vecItem_multi.size(); row++)
		{
			if (!bSmallDefect == isSmallDefectType(vecItem_multi[row].first))
				m_ctrGrid.SetRowHeight(row + 1, 0);
            else
            {
                m_ctrGrid.SetRowHeight(row + 1, m_nSizeRow);
                if (vecItem_multi[row].first >= DefectType::SmallDefect_ProtrutionLevel1 
					&& vecItem_multi[row].first <= DefectType::SmallDefect_ProtrutionLevel5)
                {
                    m_ctrGrid.SetRowHeight(row + 1, 0);
                }

				if (vecItem_multi[row].first >= DefectType::SmallDefect_ScratchLevel1
                    && vecItem_multi[row].first <= DefectType::SmallDefect_ScratchLevel5)
                {
                    m_ctrGrid.SetRowHeight(row + 1, 0);
                }
            }
		}

		for (int row = 0; row < m_ctrGrid.GetRowCount(); row++)
		{
			if (m_ctrGrid.GetItemText(row, 1) == _T("x"))
			{
				m_ctrGrid.SetRowHeight(row, 0);
			}
		}
		break;
	case 1:
		for (int col = 1; col < enColTitle::CT_END; col++)
		{
			if (true == bSmallDefect)
			{
				if (true == isSmallDefectUsingCol((enColTitle)col))
					m_ctrGrid.SetColumnWidth(col, m_vecnSizeCol[col]);
				else
					m_ctrGrid.SetColumnWidth(col, 0);
			}
			else
			{
				m_ctrGrid.SetColumnWidth(col, m_vecnSizeCol[col]);
                if (col == (enColTitle)MINIMUM_SIZE_X || col == (enColTitle)MINIMUM_SIZE_Y || col == (enColTitle)COUNT
                    || col == (enColTitle)PARENT)
                {
                    m_ctrGrid.SetColumnWidth(col, 0);
                }
			}
		}

		for (int row = 0; row < vecItem_multi.size(); row++)
		{
			if (!bSmallDefect == isSmallDefectType(vecItem_multi[row].first))
				m_ctrGrid.SetRowHeight(row + 1, 0);
			else
				m_ctrGrid.SetRowHeight(row + 1, m_nSizeRow);
		}
		break;
	}
	m_ctrGrid.Invalidate();
}

bool CDlgClassificationST::isSmallDefectUsingCol(enColTitle ct)
{
	return (m_mapSDCol.end() == m_mapSDCol.find(ct)) ? false : true;

}

bool CDlgClassificationST::isSmallDefectType(DefectType dt)
{
	return (m_mapSD.end() == m_mapSD.find(dt)) ? false : true;
}




HBRUSH CDlgClassificationST::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.


	if (pWnd->GetDlgCtrlID() == IDC_STATIC_FINAL_JUDGE)
	{
		II_RESULT_VALUE ret = m_DefectInfoProcess.m_Judge.GetJudge();
		switch (ret)
		{
		case PASS:
			pDC->SetTextColor(RGB(128, 128, 255));
			break;
		case REJECT:
			pDC->SetTextColor(RGB(255, 128, 128));
			break;
		case INVALID:
		case NOT_MEASURED:
		case REWORK:
		default:
			pDC->SetTextColor(RGB(200, 200, 200));
			break;
		}
	}


	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

void CDlgClassificationST::OnBnClickedCheck1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_ISI_Spec_table.m_bUse = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();
}

// Normal - USE가 true 값인 경우에 대한 아이템만 보임
void CDlgClassificationST::OnBnClickedBtnUseNormal()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_use_mode = 0;
	m_ctrGrid.SetColumnWidth(1, 0);
	for (int row = 0; row < m_ctrGrid.GetRowCount(); row++)
	{
		if (m_ctrGrid.GetItemText(row, 1) == _T("x"))
		{
			m_ctrGrid.SetRowHeight(row, 0);
		}
	}
	m_ctrGrid.Invalidate();
}

// Admin - USE가 true, x인 경우 둘 다 아이템 보임
void CDlgClassificationST::OnBnClickedBtnUseAdmin()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_use_mode = 1;
	SetGridType(m_ctrTab2.GetCurSel());
}


void CDlgClassificationST::OnBnClickedBtnInspectionArea()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	std::vector<long> vecInspAreaSet;
	CDlgInspectionArea dlg(this, &m_DefectInfoProcess, m_blobConditionSize, m_ISI_Spec_table.m_vecInspAreaSet);

	if (dlg.DoModal() == IDOK)
	{
		m_ISI_Spec_table.m_vecInspAreaSet = dlg.m_vecInspAreaSet;
		m_ISI_Spec_table.m_nInspAreaSetSize = m_ISI_Spec_table.m_vecInspAreaSet.size();
	}

	long nTemp = 0;
}

void CDlgClassificationST::OnBnClickedBtnReSaveexcel()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_pParent!=nullptr)
    {
        m_pParent->m_nExcelRow = 0;
        m_pParent->ExcelRejectDataInput();
    }

    


	//long nImageSizeX = 100;
 //   long nImageSizeY = 100;
	//m_ExcelSave.Create(CRect(0, 0, nImageSizeX, nImageSizeY), 8); // zoomsize 설정.. 32 - 16 - 8

	//CFileDialog SaveDlg(FALSE, _T("xlsx"), _T(""), OFN_HIDEREADONLY, _T("xlsx Files (*..xlsx)|*..xlsx||"));

 //   if (SaveDlg.DoModal() != IDOK)
 //   {
 //       return;
 //   }

 //   CString strExcelSaveFilePath = SaveDlg.GetPathName();

	////CString strExcelSaveFilePath;

	////strExcelSaveFilePath.Format(_T("D:\\iBCI\\Result"));

	////// Excel File 저장할곳 디렉토리 생성
	////CreateDir(strExcelSaveFilePath);

	//// 검사 결과 항목 Data 삽입
 //   vector<CString> vecData;

	//vecData.push_back(_T("NAME"));
 //   vecData.push_back(_T("OK/NG"));
 //   vecData.push_back(_T("Bright"));
 //   vecData.push_back(_T("Dark"));
 //   vecData.push_back(_T("Bin"));

 //   for (long nResultName = 0; nResultName < m_pDlgDefectGrid->vecDefectTitle.size() ; nResultName++)
 //   {
 //       if (nResultName > 26)
 //       {
 //           continue;
	//	}
 //       CString str = m_pDlgDefectGrid->vecDefectTitle[nResultName];
 //       vecData.push_back(str);

	//}

	//m_ExcelSave.RejectListName(vecData);
	//// Bright 및 Dark Crop Image 가져오기
 //   vector<long> vecResult;
 //   
 //   for (long nDefect = 0; nDefect < m_DefectInfoProcess.m_vecCandidate.size(); nDefect++)
 //   {
 //       auto& Defect = m_DefectInfoProcess.m_vecCandidate[nDefect];
 //       CISIDefectProperty Result;

	//	
	//	IPVM::Rect BrightROI;
 //       IPVM::Rect DarkROI;

 //       IPVM::Rect rect = Defect.rtRect_pixel;

 //       rect &= IPVM::Rect(*m_DefectInfoProcess.m_pSrcBright);

	//	int OriginRect_Width = rect.Width();
 //       int OriginRect_Height = rect.Height();
 //       int Offset_X = 0;
 //       int Offset_Y = 0;
 //       if (100 > max(rect.Width(), rect.Height()))
 //       {
 //           Offset_X = 50;
 //           Offset_Y = 50;
 //       }
 //       else if (rect.Width() > rect.Height())
 //       {
 //           int Diff = rect.Width() - rect.Height();
 //           int Diff_half = Diff / 2;
 //           Offset_Y = Diff_half;
 //       }
 //       else
 //       {
 //           int Diff = rect.Height() - rect.Width();
 //           int Diff_half = Diff / 2;
 //           Offset_X = Diff_half;
 //       }
 //       rect.InflateRect(Offset_X, Offset_Y, Offset_X, Offset_Y);
 //       rect &= IPVM::Rect(*m_DefectInfoProcess.m_pSrcBright);

 //       IPVM::Image_8u_C1 CropImage_Bright(rect.Width(), rect.Height());
 //       IPVM::Image_8u_C1 CropImage_Dark(rect.Width(), rect.Height());
 //       IPVM::Rect OriginRect(CropImage_Bright);
 //       //OriginRect.DeflateRect(offset_left, offset_top, -offset_right, -offset_bottom);
 //       
 //       IPVM::Image_8u_C1* OriginImage_Bright;
 //       OriginImage_Bright = m_DefectInfoProcess.m_pSrcBright;
 //       IPVM::ImageProcessing::Copy(*OriginImage_Bright, rect, IPVM::Rect(CropImage_Bright), CropImage_Bright);

 //       //CropImage_Bright.SavePng(_T("D:\\Crop_Ori.jpg"));

	//	IPVM::Image_8u_C1* OriginImage_Dark;
 //       OriginImage_Dark = m_DefectInfoProcess.m_pSrcDark;
 //       IPVM::ImageProcessing::Copy(*OriginImage_Dark, rect, IPVM::Rect(CropImage_Dark), CropImage_Dark);
 //       //CropImage_Dark.SaveJpeg(_T("D:\\CropImage_Dark123.jpg"));

 //       long nX = rect.Width();
 //       long nY = rect.Height();
 //       long nOriX = rect.Width();
 //       long nOriY = rect.Height();
 //      // IPVM::ImageProcessing::Fill((IPVM::Rect)CropOffsetImg, 0, CropOffsetImg);
 //       //CropOffsetImg.SaveJpeg(_T("D:\\CropOffsetImage2.jpg"));
	//	long nOffX = CropImage_Bright.GetSizeX();
 //       long nOffY = CropImage_Bright.GetSizeY();
 //       IPVM::Rect rt;

 //       long MoveX = (nX - nOffX) / 2;
 //       long MoveY = (nY - nOffY) / 2;
 //       rt = IPVM::Rect(CropImage_Bright);
 //       rt.MoveToX(MoveX);
 //       rt.MoveToY(MoveY);
 //      // IPVM::ImageProcessing::Copy(CropImage_Bright, IPVM::Rect(CropImage_Bright), /*IPVM::Rect(CropImage_Bright)*/ rt, CropOffsetImg);
 //       //CropOffsetImg.SaveJpeg(_T("D:\\CropOffsetImage.jpg"));
 //       //CropImage_Bright.SaveJpeg(_T("D:\\CropImage_Bright.jpg"));



 //       nX = rect.Width();
 //       nY = rect.Height();

	//	vector<CString> vecDefectName;
 //       for (int i = 0; i < m_DefectInfoProcess.m_vecCandidate[nDefect].GetLogicStepNum(); i++)
 //       {
 //           long nKey = 0;
 //           classification::enLogicResult nNO0_YES1 = classification::enLogicResult::NONE;
 //           CString strNote;
 //           CString strDefectName;
 //           m_DefectInfoProcess.m_vecCandidate[nDefect].GetLogicStep(i, strDefectName, nKey, nNO0_YES1, strNote);

 //           switch (nNO0_YES1)
 //           {
 //               case classification::NO:
 //                   strText = _T("NO");
 //                   //vecDefectName.push_back(strDefectName);
 //                   break;
 //               case classification::YES:
 //                   strText = _T("YES");
 //                   vecDefectName.push_back(strDefectName);
 //                   break;
 //               case classification::NONE:
 //                   strText = _T("");
 //                   //vecDefectName.push_back(strDefectName);
 //                   break;
 //               default:
 //                   break;
 //           }
 //       }

	//	if (vecDefectName.size() == 0)
	//	{
 //           vecDefectName.push_back(_T("OK"));
	//	}
 //       vecResult.push_back(vecDefectName.size());
	//	m_nExcelRow += 1;
 //       II_RESULT_VALUE Ret = m_DefectInfoProcess.m_vecCandidate[nDefect].GetJudge();
 //       
	//	CString ResultName = _T("[ none ] ");
 //       switch (Ret)
 //       {
 //           case PASS:
 //               ResultName = _T("OK");
 //               break;
 //           case REWORK:
 //               break;
 //           case REJECT:
 //               ResultName = _T("NG");
 //               break;
 //           case INVALID:
 //               break;
 //           case NOT_MEASURED:
 //               break;
 //           default:
 //               break;
 //       }

	//	CString strBrigtImage;
 //       strBrigtImage.Format(_T("D:\\iBCI\\Result\\BrightOri_%d.jpg"),nDefect);
 //       //CropImage_Bright.SaveJpeg(strBrigtImage);
 //       CString strDarkImage;
 //       strDarkImage.Format(_T("D:\\iBCI\\Result\\DarkOri_%d.jpg"), nDefect);
 //       //CropImage_Dark.SaveJpeg(strDarkImage);
 //       CString strBinImage;
 //       strBinImage.Format(_T("D:\\iBCI\\Result\\Bin_%d.jpg"), nDefect);
 //       //Defect.srcImage.SaveJpeg(strBinImage);

	//	//m_ExcelSave.SetBrightImage(CropImage_Bright);
 //       //m_ExcelSave.SetDarkImage(CropImage_Dark);

	//	IPVM::Rect Binrect(Defect.srcImage);
 //       nX = Binrect.Width();
 //       nY = Binrect.Height();

	//	IPVM::Image_8u_C1 CropOffsetImg_BIN;
 //       CropOffsetImg_BIN.Create(nOriX, nOriY);

 //       IPVM::ImageProcessing::Fill((IPVM::Rect)CropOffsetImg_BIN, 0, CropOffsetImg_BIN);

	//	long nXOffset = (abs(nOriX - nX)) / 2;
 //       long nYOffset = (abs(nOriY - nY)) / 2;
 //       Binrect.MoveToX(nXOffset);
 //       Binrect.MoveToY(nYOffset);
	//	//IPVM::ImageProcessing::Copy(Defect.srcImage, IPVM::Rect(Defect.srcImage), /*IPVM::Rect(CropImage_Bright)*/ rt, CropOffsetImg_BIN);
 //       IPVM::ImageProcessing::Copy(Defect.srcImage, IPVM::Rect(Defect.srcImage), Binrect, CropOffsetImg_BIN);

	//	Defect.srcImage.SaveJpeg(_T("D:\\BinOri.jpg"));
 //       CropOffsetImg_BIN.SaveJpeg(_T("D:\\BinOffset.jpg"));

	//	m_ExcelSave.SetBrightImage(CropImage_Bright);
 //       m_ExcelSave.SetDarkImage(CropImage_Dark);
 //       m_ExcelSave.SetBinImage(CropOffsetImg_BIN);


	//	vector<CString> vecResultData;

 //       // 여기서 각 Defect 결과 값들을 담는다.

 //       CString strResult;
 //       strResult.Format(_T("%d"), nDefect+1);
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%s"), ResultName);
 //       vecResultData.push_back(strResult);
 //       DefectType dt = m_DefectInfoProcess.m_vecCandidate[nDefect].GetDefectResultCode();
 //       CString strdtName = GetDefectName(dt);
 //       strResult.Format(_T("Defect[%d]_%s"), nDefect, strdtName);
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%s"), strdtName);
 //       vecResultData.push_back(strResult);
	//	strResult.Format(_T("%.3f"), Defect.SIZE_X_mm.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%.3f"), Defect.SIZE_Y_mm.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%.3f"), Defect.AREA.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%.3f"), Defect.AREA_Ratio.GetValue());
 //       vecResultData.push_back(strResult);

	//	strResult = InspectionArea2String((enInspectionArea)Defect.nInspectionArea.GetValue());
 //       vecResultData.push_back(strResult);
 //       
	//	strResult = enumtoString().InspectionImageType2String((enInspectionImageType)Defect.nInspectImage.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%.3f"), Defect.RATIO_YX.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%.3f"), Defect.RATIO_XY.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%.3f"), Defect.WIDTH_mm.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%.3f"), Defect.LENGTH_mm.GetValue());
 //       vecResultData.push_back(strResult);
	//	//RATIO_T? RATIO_L?
 //       strResult.Format(_T("%.3f"), Defect.RATIO_LW.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%.3f"), Defect.RATIO_WL.GetValue());
 //       vecResultData.push_back(strResult);

	//	strResult.Format(_T("%d"), Defect.MIN_GV.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%d"), Defect.AVG_GV.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%d"), Defect.MAX_GV.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%.3f"), Defect.MASS_CENTER_GV_GV.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%.3f"), Defect.SECTION_THRESHOLD.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%.3f"), Defect.HOR.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%.3f"), Defect.VER.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%.3f"), Defect.RATIO_HV.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%.3f"), Defect.RATIO_VH.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%.3f"), Defect.RATIO_WHITE.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%.3f"), Defect.RATIO_DARK.GetValue());
 //       vecResultData.push_back(strResult);
 //       strResult.Format(_T("%d"), Defect.OVER_200_GV_PIXEL_COUNT.GetValue());
 //       vecResultData.push_back(strResult);

 //       m_ExcelSave.AddImageFileDatas(m_nExcelRow, vecDefectName[0], strDarkImage, strBrigtImage, strBinImage, vecResultData, ResultName, OriginRect);


 //       //m_ExcelSave.RejectListName(m_DefectInfoProcess.m_vecCandidate[nDefect].GetLogicStep());
	//}


	//m_ExcelSave.Savexlsfile(strExcelSaveFilePath, FALSE);

}

void CDlgClassificationST::CreateDir(CString strFolderPath)
{
    if (strFolderPath.ReverseFind(_T('.')) != -1)
    {
        return;
    }

    long nDivideTotalCount = 0;
    CString strSub;
    std::vector<CString> vecstrFolderName;
    while (FALSE != AfxExtractSubString(strSub, strFolderPath, nDivideTotalCount++, _T('\\')))
    {
        vecstrFolderName.push_back(strSub);
    }

    CString strMakeFolderPath = _T("");
    for (long nFolderIndex = 0; nFolderIndex < vecstrFolderName.size(); nFolderIndex++)
    {
        strMakeFolderPath = strMakeFolderPath + vecstrFolderName[nFolderIndex] + _T("\\");

        if (GetFileAttributes(strMakeFolderPath) == -1)
            CreateDirectory(strMakeFolderPath, NULL);
    }
}

void CDlgClassificationST::OnBnClickedBtnPriorityDown()
{
    if (m_nDefectSelect <= 1)
        return;


    long nIndex = 0;
    for (int i = (long)DefectType::START_Coater_Surface + 1; i < (long)DefectType::END_Coater_Surface; i++)
    {
        std::vector<CISISpecInfo>* spec = vecItem_multi[nIndex].second;
        if (nullptr != spec)
        {
            if (nIndex == m_nDefectSelect - 1)
            {
                std::vector<CISISpecInfo>* specChanged = vecItem_multi[nIndex + 1].second;
                Base::ParaLong temp(specChanged->at(0).PRIORITY);
                specChanged->at(0).PRIORITY = spec->at(0).PRIORITY.GetValue();
                spec->at(0).PRIORITY = temp.GetValue();
                break;
            }
        }
        nIndex++;
    }
    for (long nVal = 0; nVal < vecItem_multi.size(); nVal++)
    {
        for (long nSub = 0; nSub < vecItem_multi[nVal].second->size(); nSub++)
        {
            vecItem_multi[nVal].second->at(nSub).PRIORITY = vecItem_multi[nVal].second->at(0).PRIORITY;
        }
    }
    sort(vecItem_multi.begin(), vecItem_multi.end(), compare);

	m_ctrGrid.SetItemState(m_nDefectSelect, 3, GVIS_MODIFIED);
    m_nDefectSelect += 1;
    m_ctrGrid.SetFocusCell(m_nDefectSelect, 3);
    m_ctrGrid.SetItemState(m_nDefectSelect, 3, GVIS_SELECTED);
    m_ctrGrid.Invalidate();
}

void CDlgClassificationST::OnBnClickedBtnPriorityUp()
{
    
    if (m_nDefectSelect <= 1)
        return;

    long nIndex = 0;
    for (int i = (long)DefectType::START_Coater_Surface +1; i < (long)DefectType::END_Coater_Surface; i++)
    {
        std::vector<CISISpecInfo>* spec = vecItem_multi[nIndex].second;
        if (nullptr != spec)
        {
            if (nIndex == m_nDefectSelect - 1)
            {
                std::vector<CISISpecInfo>* specChanged = vecItem_multi[nIndex - 1].second;
                Base::ParaLong temp(specChanged->at(0).PRIORITY);
                specChanged->at(0).PRIORITY = spec->at(0).PRIORITY.GetValue();
                spec->at(0).PRIORITY = temp.GetValue();
            }
        }
        nIndex++;
    }
	for (long nVal = 0; nVal < vecItem_multi.size(); nVal++)
    {
		for (long nSub = 0; nSub < vecItem_multi[nVal].second->size(); nSub++)
        {
            vecItem_multi[nVal].second->at(nSub).PRIORITY = vecItem_multi[nVal].second->at(0).PRIORITY;
		}
	}
    sort(vecItem_multi.begin(), vecItem_multi.end(), compare);


    m_ctrGrid.SetItemState(m_nDefectSelect, 3, GVIS_MODIFIED);
    m_nDefectSelect -= 1;
    m_ctrGrid.SetFocusCell(m_nDefectSelect, 3);
    m_ctrGrid.SetItemState(m_nDefectSelect, 3, GVIS_SELECTED);
    m_ctrGrid.Invalidate();
    
}


void CDlgClassificationST::SpecTableTrans(std::vector<CString>& vecConditions, DefectType DT)
{
	// Spec Table 값의 갯수 만큼 확인해서 돌려야지....

	std::vector<std::vector<CString>> vecvecDefectType;
    std::vector<std::vector<CString>> vecvecDefectCompare;
    std::vector<std::vector<CString>> vecvecDefectValue;

	vecvecDefectType.resize(vecConditions.size());
    vecvecDefectCompare.resize(vecConditions.size());
    vecvecDefectValue.resize(vecConditions.size());

	for (long nCondition = 0; nCondition < vecConditions.size(); nCondition++)
	{
        CString strDefectType;
        CString strDefectCompare;
        CString strDefectValue;

		std::vector<CString> vecDefectFullCondition;
        StringTokenDivideToCString(vecConditions[nCondition], '&', vecDefectFullCondition);

		for (long nSub = 0; nSub < vecDefectFullCondition.size(); nSub++)
		{
            if (vecConditions[nCondition] == _T(""))
            {
                continue;
            }
            vecDefectFullCondition[nSub].Trim();
            CString str = vecDefectFullCondition[nSub].Mid(0, vecDefectFullCondition[nSub].ReverseFind(':'));
            if (str == _T("ORAND ") || str == _T("Inspection_Area ") || str == _T("Inspection_ImageType "))
            {
                strDefectType = vecDefectFullCondition[nSub].Mid(0, vecDefectFullCondition[nSub].ReverseFind(':'));
                strDefectCompare = vecDefectFullCondition[nSub].Mid(vecDefectFullCondition[nSub].ReverseFind(':') + 2, vecDefectFullCondition[nSub].GetLength());
                strDefectValue = _T("");
                //continue;
            }
            else
            {
                strDefectType = vecDefectFullCondition[nSub].Mid(0, vecDefectFullCondition[nSub].ReverseFind('('));
                strDefectCompare = vecDefectFullCondition[nSub].Mid(vecDefectFullCondition[nSub].ReverseFind('('), vecDefectFullCondition[nSub].ReverseFind(' '));

				strDefectCompare = strDefectCompare.Left(4);
                //if (strDefectCompare.Find('<') == TRUE)
                //{
                //    strDefectCompare = strDefectCompare.Mid(0, strDefectCompare.ReverseFind('<') + 1);
                //}
                //else if (strDefectCompare.Find('>') == TRUE)
                //{
                //    strDefectCompare = strDefectCompare.Mid(0, strDefectCompare.ReverseFind('>') + 1);
                //}
                strDefectValue = vecDefectFullCondition[nSub].Mid(vecDefectFullCondition[nSub].ReverseFind(' ') + 1, vecDefectFullCondition[nSub].GetLength());
            }

			vecvecDefectType[nCondition].push_back(strDefectType);
            vecvecDefectCompare[nCondition].push_back(strDefectCompare);
            vecvecDefectValue[nCondition].push_back(strDefectValue);
		}

	}



    std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>> vecSpec(m_ISI_Spec_table.m_mapSpec_multi.begin(), m_ISI_Spec_table.m_mapSpec_multi.end());
 //  
	////std::sort(vecSpec.begin(), vecSpec.end(), compare_multi);

    std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>>::iterator vecitrStartVec, vecitrEndVec;
    vecitrStartVec = vecSpec.begin();
    vecitrEndVec = vecSpec.end();

    m_vecTableTemp.clear();
    m_vecTableTemp = vecSpec;

	vecitrStartVec->second.clear();

	while (vecitrStartVec != vecitrEndVec)
	{
        if (vecitrStartVec->first == DT)
		{   
			std::vector<CISISpecInfo>* vecSpec = m_ISI_Spec_table.GetSpecInfo_Multi((DefectType)vecitrStartVec->first);
            vecSpec->clear();
			if (vecvecDefectCompare.size() == 0)
			{
				CISISpecInfo spec(vecitrStartVec->first, 0, enCompareOperator::BIGGER_THAN_SPEC, 0, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, true, true, true, true,
                    true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
                    true);
                InitSpecSize(vecitrStartVec->first, spec);
				vecSpec->push_back(spec);
			}
            for (long nCnt = 0; nCnt < vecvecDefectCompare.size(); nCnt++)
            {
                CString strDt = GetDefectName(vecitrStartVec->first);
                CISISpecInfo spec(vecitrStartVec->first, 0, enCompareOperator::BIGGER_THAN_SPEC, 0, enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, true, true, true, true,
                    true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
                    true);

                if (vecitrStartVec->first == DT)
                {
                    for (long nCondition = 0; nCondition < vecvecDefectCompare[nCnt].size(); nCondition++)
                    {
                        vecvecDefectType[nCnt][nCondition].Trim();
                        long nDefectCompare = 0;
                        if (vecvecDefectCompare[nCnt][nCondition] == _T("(크)<"))
                        {
                            nDefectCompare = BIGGER_THAN_SPEC;
                        }
                        else if (vecvecDefectCompare[nCnt][nCondition] == _T("(작)>"))
                        {
                            nDefectCompare = SMALLER_THAN_SPEC;
                        }
                        InitSpecSize(vecitrStartVec->first, spec);

                        //spec.CONDITION_INDEX = nCnt;
                        if (vecvecDefectType[nCnt][nCondition] == _T("RATIO_XY"))
                        {
                            
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.RATIO_XY_COMPARE_OPERATOR = nDefectCompare;
                                spec.RATIO_XY = _ttof(vecvecDefectValue[nCnt][nCondition]);
                            }
							else
                            {
                                spec.RATIO_XY_COMPARE_OPERATOR_2 = nDefectCompare;
                                spec.RATIO_XY_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
							}

                            
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("RATIO_YX"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.RATIO_YX = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.RATIO_YX_COMPARE_OPERATOR = nDefectCompare;
							}
							else
							{
                                spec.RATIO_YX_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.RATIO_YX_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("AVG_GV"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.AVG_GV = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.AVG_COMPARE_OPERATOR = nDefectCompare;
							}
                            else
							{
                                spec.AVG_GV_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.AVG_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("ORAND"))
                        {
                            if (vecvecDefectCompare[nCnt][nCondition] == _T("OR"))
                            {
                                spec.ORAND = false;
                            }
                            else if (vecvecDefectCompare[nCnt][nCondition] == _T("AND"))
                            {
                                spec.ORAND = true;
                            }
                            //Property.orand = _ttof(vecDefectValue[nCnt]);
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("Inspection_Area"))
                        {
                            if (vecvecDefectCompare[nCnt][nCondition] == _T("None"))
                            {
                                spec.nInspectionArea = enInspectionArea::IA_NONE;
                            }
                            if (vecvecDefectCompare[nCnt][nCondition] == _T("Burr"))
                            {
                                spec.nInspectionArea = enInspectionArea::IA_BACKGROUND_BURR;
                            }
                            if (vecvecDefectCompare[nCnt][nCondition] == _T("Scrab"))
                            {
                                spec.nInspectionArea = enInspectionArea::IA_SCRAB;
                            }
                            if (vecvecDefectCompare[nCnt][nCondition] == _T("Tab"))
                            {
                                spec.nInspectionArea = enInspectionArea::IA_TAB;
                            }
                            if (vecvecDefectCompare[nCnt][nCondition] == _T("Insulation"))
                            {
                                spec.nInspectionArea = enInspectionArea::IA_TAB_INSULATION;
                            }
                            if (vecvecDefectCompare[nCnt][nCondition] == _T("Electrode Edge"))
                            {
                                spec.nInspectionArea = enInspectionArea::IA_ELECTRODE_EDGE;
                            }
                            if (vecvecDefectCompare[nCnt][nCondition] == _T("Electrode"))
                            {
                                spec.nInspectionArea = enInspectionArea::IA_ELECTRODE;
                            }
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("Inspection_ImageType"))
                        {
                            if (vecvecDefectCompare[nCnt][nCondition] == _T("Bright"))
                            {
                                spec.nInspectImage = enInspectionImageType::Bright;
                                spec.INSPECTION_IMAGE_TYPE = enInspectionImageType::Bright;
                            }
                            else if (vecvecDefectCompare[nCnt][nCondition] == _T("Dark"))
                            {
                                spec.nInspectImage = enInspectionImageType::Dark;
                                spec.INSPECTION_IMAGE_TYPE = enInspectionImageType::Dark;
                            }
                            //spec.nInspectImage = _ttof(vecvecDefectValue[nCnt][nCondition]);
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("ANGLE"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.ANGLE = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.ANGLE_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
							{
                                spec.ANGLE_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.ANGLE_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("Area"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.AREA = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.AREA_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
							{
                                spec.AREA_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.AREA_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("RATIO_Area"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
								spec.AREA_Ratio = _ttof(vecvecDefectValue[nCnt][nCondition]);
								spec.AREA_RATIO_COMPARE_OPERATOR = nDefectCompare;
							}
							else
							{
                                spec.AREA_Ratio_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.AREA_RATIO_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("BLOB_THRESHOLD_T"))
                        {
                            spec.BLOB_THRESHOLD = _ttof(vecvecDefectValue[nCnt][nCondition]);
                            //spec.compare = nDefectCompare;
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("BRIGHT_AVG_GV"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.BRIGHT_AVG_GV = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.BRIGHT_AVG_GV_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
							{
                                spec.BRIGHT_AVG_GV_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.BRIGHT_AVG_GV_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("COMPACT"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.COMPACT = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.COMPACT_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
                            {
                                spec.COMPACT_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.COMPACT_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("DARK_AVG_GV"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.DARK_AVG_GV = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.DARK_AVG_GV_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
							{
                                spec.DARK_AVG_GV_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.DARK_AVG_GV_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("EDGE_ENERGY"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.EDGE_ENERGY = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.EDGE_ENERGY_COMPARE_OPERATOR = nDefectCompare;
                            }
                            else
                            {
                                spec.EDGE_ENERGY_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.EDGE_ENERGY_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("EDGE_ENERGY_BLACK"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
								spec.EDGE_ENERGY_BLACK = _ttof(vecvecDefectValue[nCnt][nCondition]);
								spec.BLACK_EDGE_ENERGY_COMPARE_OPERATOR = nDefectCompare;
							}
							else
							{
                                spec.EDGE_ENERGY_BLACK_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.BLACK_EDGE_ENERGY_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("EDGE_ENERGY_WHITE"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.EDGE_ENERGY_WHITE = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.WHITE_EDGE_ENERGY_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
                            {
                                spec.EDGE_ENERGY_WHITE_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.WHITE_EDGE_ENERGY_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("DISTANCE_TO_EDGE_2"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
								spec.DISTANCE_TO_EDGE_2_mm = _ttof(vecvecDefectValue[nCnt][nCondition]);
								spec.DIS_TO_EDGE_2_mm_COMPARE_OPERATOR = nDefectCompare;
							}
							else
                            {
                                spec.DISTANCE_TO_EDGE_2_mm_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.DIS_TO_EDGE_2_mm_COMPARE_OPERATOR_2 = nDefectCompare;

							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("DISTANCE_TO_EDGE"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.DISTANCE_TO_EDGE_mm = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.DIS_TO_EDGE_mm_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
							{
                                spec.DISTANCE_TO_EDGE_mm_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.DIS_TO_EDGE_mm_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("HOR"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.HOR = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.HOR_COMPARE_OPERATOR = nDefectCompare;
							}
							else
                            {
                                spec.HOR_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.HOR_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("IS_NEAR_EDGE"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.IS_NEAR_EDGE = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.IS_NEAR_EDGE_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
							{
                                spec.IS_NEAR_EDGE_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.IS_NEAR_EDGE_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("Length"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
								spec.LENGTH_mm = _ttof(vecvecDefectValue[nCnt][nCondition]);
								spec.LENGTH_COMPARE_OPERATOR = nDefectCompare;
							}
							else
                            {
                                spec.LENGTH_mm_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.LENGTH_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("MASS_CENTER_GV"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.MASS_CENTER_GV_GV = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.MASS_CENTER_GV_GV_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
							{
                                spec.MASS_CENTER_GV_GV_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.MASS_CENTER_GV_GV_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("MAX_GV"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.MAX_GV = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.MAX_GV_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
							{
                                spec.MAX_GV_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.MAX_GV_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("MIN_GV"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.MIN_GV = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.MIN_GV_COMPARE_OPERATOR = nDefectCompare;
                            }
                            else
                            {
                                spec.MIN_GV_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.MIN_GV_COMPARE_OPERATOR_2 = nDefectCompare;
                            }
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("250>CNT"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.OVER_200_GV_PIXEL_COUNT = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.OVER_GV_PX_COUNT_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
							{
                                spec.OVER_200_GV_PIXEL_COUNT_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.OVER_GV_PX_COUNT_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("VER"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.VER = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.VER_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
							{
                                spec.VER_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.VER_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("RATIO_HV"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.RATIO_HV = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.RATIO_HV_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
							{
                                spec.RATIO_HV_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.RATIO_HV_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("RATIO_VH"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.RATIO_VH = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.RATIO_VH_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
							{
                                spec.RATIO_VH_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.RATIO_VH_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("RATIO_WHITE"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.RATIO_WHITE = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.RATIO_WHITE_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
							{
                                spec.RATIO_WHITE_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.RATIO_WHITE_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("RATIO_DARK"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.RATIO_DARK = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.RATIO_DARK_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
                            {
                                spec.RATIO_DARK_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.RATIO_DARK_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("Width"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
								spec.WIDTH_mm = _ttof(vecvecDefectValue[nCnt][nCondition]);
								spec.WIDTH_COMPARE_OPERATOR = nDefectCompare;
							}
							else
							{
                                spec.WIDTH_mm_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.WIDTH_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("RATIO_WL"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.RATIO_WL = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.RATIO_WL_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
                            {
                                spec.RATIO_WL_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.RATIO_WL_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("RATIO_LW"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.RATIO_LW = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.RATIO_LW_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
							{
                                spec.RATIO_LW_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.RATIO_LW_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("SECTION_THRESHOLD"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.SECTION_THRESHOLD = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.SECTION_THRESHOLD_COMPARE_OPERATOR = nDefectCompare;
                            }
							else
							{
                                spec.SECTION_THRESHOLD_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.SECTION_THRESHOLD_COMPARE_OPERATOR_2 = nDefectCompare;
							}
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("EDGE_ENERGY_SHARPNESS"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.EDGE_ENERGY_SHARPNESS = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR = nDefectCompare;
                            }
                            else
                            {
                                spec.EDGE_ENERGY_SHARPNESS_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2 = nDefectCompare;
                            }
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("EDGE_ENERGY_SHARPNESS_WHITE"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.EDGE_ENERGY_SHARPNESS_WHITE = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR = nDefectCompare;
                            }
                            else
                            {
                                spec.EDGE_ENERGY_SHARPNESS_WHITE_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2 = nDefectCompare;
                            }
                        }
                        if (vecvecDefectType[nCnt][nCondition] == _T("EDGE_ENERGY_SHARPNESS_BLACK"))
                        {
                            if (nDefectCompare == BIGGER_THAN_SPEC)
                            {
                                spec.EDGE_ENERGY_SHARPNESS_BLACK = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR = nDefectCompare;
                            }
                            else
                            {
                                spec.EDGE_ENERGY_SHARPNESS_BLACK_2 = _ttof(vecvecDefectValue[nCnt][nCondition]);
                                spec.BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2 = nDefectCompare;
                            }
                        }
                        //vecSpec->push_back((CISISpecInfo)sp);
                        //CISISpecInfo* spec;
                        //vecSpecInfo.push_back(&spec);
                        //vecitrStartVec->second.push_back(*spec);
                    }
                }
                if (vecvecDefectCompare[nCnt].size() != 0)
                {
                    vecSpec->push_back(spec);
				}
                
            }

		}
        vecitrStartVec++;
        
	}
    //vecitrStartVec->second.erase(vecitrStartVec->second.begin());
	std::vector<CISISpecInfo>* test = m_ISI_Spec_table.GetSpecInfo_Multi(DT);

	m_ctrGrid.Invalidate();
}
void CDlgClassificationST::SpecTableLoad(CString strConditions, CString strIndex, CString strName, long nSize)
{
    for (long nCur = 0; nCur < vecItem_multi.size(); nCur++)
    {
        /*if ((DefectType)vecItem_multi[nCur].first >= DefectType::SmallDefect_CraterLevel1)
            return;*/

        std::vector<CString> vecIndex;
        StringTokenDivideToCString(strIndex, ',', vecIndex);

        std::vector<CString> vecDiv;
        StringTokenDivideToCString(strConditions, ',', vecDiv);

        std::vector<CString> vecName;
        StringTokenDivideToCString(strName, ',', vecName);

        if (vecDiv[0] == _T("ITEM"))
            return;

        if (vecName[0] == GetDefectName(vecItem_multi[nCur].first))
        {
            if (nSize > 0)
            {
                CISISpecInfo spec(vecItem_multi[nCur].first, 0, enCompareOperator::BIGGER_THAN_SPEC, 0,
                    enCompareOperator::BIGGER_THAN_SPEC, enCode::CD_OR, true, true, true, true, true, true, true, true,
                    true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true,
                    true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true);

                vecItem_multi[nCur].second->push_back(spec);
            }
        }
        if (vecName[0] == GetDefectName(vecItem_multi[nCur].first))
        {
            for (long n = 0; n < vecIndex.size(); n++)
            {
                // 잘못된 값 들어가면 return
                if (vecIndex[n] != _T("ITEM") && vecIndex[n] != _T("ENABLE") && vecIndex[n] != _T("PRIORITY")
                    && vecIndex[n] != _T("SIZE_X_CO") && vecIndex[n] != _T("SIZE_Y_CO") && vecIndex[n] != _T("ORAND")
                    && vecIndex[n] != _T("INSPECTION_AREA") && vecIndex[n] != _T("INSPECTION_IMAGE_TYPE"))
                {
                    if (vecDiv[n] != _T("0"))
                    {
                        BOOL bCheckSpecial = false;
                        for (long nChar = 0; nChar < vecDiv[n].GetLength(); nChar++)
                        {
                            if (vecDiv[n].GetAt(0) == '-')
                                continue;
                            if (n == 0 || n == vecDiv[n].GetLength() - 1)
                            {
                                if (vecDiv[n].GetAt(n) == '.')
                                    continue;
                            }
                            if (isdigit(vecDiv[n].GetAt(nChar)) == 0)
                            {
                                if (bCheckSpecial == false && vecDiv[n].GetAt(nChar) == '.')
                                {
                                    bCheckSpecial = true;
                                    continue;
                                }
                                m_bCheckSpecError = true;
                                return;
                            }
                        }
                    }
                }
                // DefectType 당 OR조건 여러개일 때 해당 DefectType 기본 스펙 받아옴
                if (nSize > 0)
                {
                    if (vecIndex[n] == _T("ENABLE"))
                    {
                        if (vecName[n] == _T("true"))
                        {
                            vecItem_multi[nCur].second->at(nSize).ENABLE = true;
                        }
                        else
                        {
                            vecItem_multi[nCur].second->at(nSize).ENABLE = false;
                        }
                    }
                    if (vecIndex[n] == _T("PRIORITY"))
                    {
                        vecItem_multi[nCur].second->at(nSize).PRIORITY = _ttoi(vecName[n]);
                    }
                    if (vecIndex[n] == _T("SIZE_X"))
                    {
                        vecItem_multi[nCur].second->at(nSize).SIZE_X_mm = _ttof(vecName[n]);
                        if (vecItem_multi[nCur].second->at(nSize).SIZE_X_mm > 0)
                            vecItem_multi[nCur].second->at(nSize).SIZEX_COMPARE_OPERATOR
                                = enumtoString().String2CompareOperator(vecDiv[n + 1]);
                    }
                    if (vecIndex[n] == _T("SIZE_Y"))
                    {
                        vecItem_multi[nCur].second->at(nSize).SIZE_Y_mm = _ttof(vecName[n]);
                        if (vecItem_multi[nCur].second->at(nSize).SIZE_Y_mm > 0)
                            vecItem_multi[nCur].second->at(nSize).SIZEY_COMPARE_OPERATOR
                                = enumtoString().String2CompareOperator(vecDiv[n + 1]);
                    }
                    if (vecIndex[n] == _T("ORAND"))
                    {
                        if (vecName[n] == _T("OR"))
                        {
                            vecItem_multi[nCur].second->at(nSize).ORAND = false;
                        }
                        else if (vecName[n] == _T("AND"))
                        {
                            vecItem_multi[nCur].second->at(nSize).ORAND = true;
                        }
                    }
                    if (vecIndex[n] == _T("INSPECTION_AREA"))
                    {
                        if (vecName[n] == _T("NONE"))
                        {
                            vecItem_multi[nCur].second->at(nSize).nInspectionArea = enInspectionArea::IA_NONE;
                        }
                        else if (vecName[n] == _T("BURR"))
                        {
                            vecItem_multi[nCur].second->at(nSize).nInspectionArea
                                = enInspectionArea::IA_BACKGROUND_BURR;
                        }
                        else if (vecName[n] == _T("SCRAB"))
                        {
                            vecItem_multi[nCur].second->at(nSize).nInspectionArea = enInspectionArea::IA_SCRAB;
                        }
                        else if (vecName[n] == _T("TAB"))
                        {
                            vecItem_multi[nCur].second->at(nSize).nInspectionArea = enInspectionArea::IA_TAB;
                        }
                        else if (vecName[n] == _T("ELECTROD_EDGE"))
                        {
                            vecItem_multi[nCur].second->at(nSize).nInspectionArea = enInspectionArea::IA_ELECTRODE_EDGE;
                        }
                        else if (vecName[n] == _T("ELECTRODE"))
                        {
                            vecItem_multi[nCur].second->at(nSize).nInspectionArea = enInspectionArea::IA_ELECTRODE;
                        }
                    }
                }
                else
                {
                    if (vecIndex[n] == _T("ENABLE"))
                    {
                        if (vecName[n] == _T("true"))
                        {
                            vecItem_multi[nCur].second->at(nSize).ENABLE = true;
                        }
                        else
                        {
                            vecItem_multi[nCur].second->at(nSize).ENABLE = false;
                        }
                    }
                    if (vecIndex[n] == _T("PRIORITY"))
                    {
                        vecItem_multi[nCur].second->at(nSize).PRIORITY = _ttoi(vecName[n]);
                    }
                    if (vecIndex[n] == _T("SIZE_X"))
                    {
                        vecItem_multi[nCur].second->at(nSize).SIZE_X_mm = _ttof(vecDiv[n]);
                        if (vecItem_multi[nCur].second->at(nSize).SIZE_X_mm > 0)
                            vecItem_multi[nCur].second->at(nSize).SIZEX_COMPARE_OPERATOR
                                = classification::BIGGER_THAN_SPEC;
                    }
                    if (vecIndex[n] == _T("SIZE_Y"))
                    {
                        vecItem_multi[nCur].second->at(nSize).SIZE_Y_mm = _ttof(vecDiv[n]);
                        if (vecItem_multi[nCur].second->at(nSize).SIZE_Y_mm > 0)
                            vecItem_multi[nCur].second->at(nSize).SIZEY_COMPARE_OPERATOR
                                = classification::BIGGER_THAN_SPEC;
                    }
                    if (vecIndex[n] == _T("ORAND"))
                    {
                        if (vecDiv[n] == _T("OR"))
                        {
                            vecItem_multi[nCur].second->at(nSize).ORAND = false;
                        }
                        else if (vecDiv[n] == _T("AND"))
                        {
                            vecItem_multi[nCur].second->at(nSize).ORAND = true;
                        }
                    }
                    if (vecIndex[n] == _T("INSPECTION_AREA"))
                    {
                        if (vecDiv[n] == _T("NONE"))
                        {
                            vecItem_multi[nCur].second->at(nSize).nInspectionArea = enInspectionArea::IA_NONE;
                        }
                        else if (vecDiv[n] == _T("BURR"))
                        {
                            vecItem_multi[nCur].second->at(nSize).nInspectionArea
                                = enInspectionArea::IA_BACKGROUND_BURR;
                        }
                        else if (vecDiv[n] == _T("SCRAB"))
                        {
                            vecItem_multi[nCur].second->at(nSize).nInspectionArea = enInspectionArea::IA_SCRAB;
                        }
                        else if (vecDiv[n] == _T("TAB"))
                        {
                            vecItem_multi[nCur].second->at(nSize).nInspectionArea = enInspectionArea::IA_TAB;
                        }
                        else if (vecDiv[n] == _T("ELECTROD_EDGE"))
                        {
                            vecItem_multi[nCur].second->at(nSize).nInspectionArea = enInspectionArea::IA_ELECTRODE_EDGE;
                        }
                        else if (vecDiv[n] == _T("ELECTRODE"))
                        {
                            vecItem_multi[nCur].second->at(nSize).nInspectionArea = enInspectionArea::IA_ELECTRODE;
                        }
                    }
                }

                if (vecIndex[n] == _T("AREA"))
                {
                    vecItem_multi[nCur].second->at(nSize).AREA = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).AREA > 0)
                        vecItem_multi[nCur].second->at(nSize).AREA_COMPARE_OPERATOR = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("AREA_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).AREA_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).AREA_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).AREA_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("AREA_Ratio"))
                {
                    vecItem_multi[nCur].second->at(nSize).AREA_Ratio = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).AREA_Ratio > 0)
                        vecItem_multi[nCur].second->at(nSize).AREA_RATIO_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("AREA_Ratio_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).AREA_Ratio_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).AREA_Ratio_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).AREA_RATIO_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("INSPECTION_IMAGE_TYPE"))
                {
                    if (vecDiv[n] == _T("Bright"))
                    {
                        vecItem_multi[nCur].second->at(nSize).nInspectImage = enInspectionImageType::Bright;
                        vecItem_multi[nCur].second->at(nSize).INSPECTION_IMAGE_TYPE = enInspectionImageType::Bright;
                    }
                    else if (vecDiv[n] == _T("Dark"))
                    {
                        vecItem_multi[nCur].second->at(nSize).nInspectImage = enInspectionImageType::Dark;
                        vecItem_multi[nCur].second->at(nSize).INSPECTION_IMAGE_TYPE = enInspectionImageType::Dark;
                    }
                }

                if (vecIndex[n] == _T("RATIO_XY"))
                {
                    vecItem_multi[nCur].second->at(nSize).RATIO_XY = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).RATIO_XY > 0)
                        vecItem_multi[nCur].second->at(nSize).RATIO_XY_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("RATIO_XY_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).RATIO_XY_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).RATIO_XY_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).RATIO_XY_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("RATIO_YX"))
                {
                    vecItem_multi[nCur].second->at(nSize).RATIO_YX = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).RATIO_YX > 0)
                        vecItem_multi[nCur].second->at(nSize).RATIO_YX_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("RATIO_YX_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).RATIO_YX_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).RATIO_YX_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).RATIO_YX_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("WIDTH"))
                {
                    vecItem_multi[nCur].second->at(nSize).WIDTH_mm = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).WIDTH_mm > 0)
                        vecItem_multi[nCur].second->at(nSize).WIDTH_COMPARE_OPERATOR = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("WIDTH_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).WIDTH_mm_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).WIDTH_mm_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).WIDTH_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("LENGTH"))
                {
                    vecItem_multi[nCur].second->at(nSize).LENGTH_mm = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).LENGTH_mm > 0)
                        vecItem_multi[nCur].second->at(nSize).LENGTH_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("LENGTH_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).LENGTH_mm_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).LENGTH_mm_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).LENGTH_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("RATIO_WL"))
                {
                    vecItem_multi[nCur].second->at(nSize).RATIO_WL = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).RATIO_WL > 0)
                        vecItem_multi[nCur].second->at(nSize).RATIO_WL_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("RATIO_WL_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).RATIO_WL_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).RATIO_WL_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).RATIO_WL_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("RATIO_LW"))
                {
                    vecItem_multi[nCur].second->at(nSize).RATIO_LW = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).RATIO_LW > 0)
                        vecItem_multi[nCur].second->at(nSize).RATIO_LW_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("RATIO_LW_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).RATIO_LW_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).RATIO_LW_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).RATIO_LW_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("MIN"))
                {
                    vecItem_multi[nCur].second->at(nSize).MIN_GV = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).MIN_GV > 0)
                        vecItem_multi[nCur].second->at(nSize).MIN_GV_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("MIN_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).MIN_GV_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).MIN_GV_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).MIN_GV_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("AVG"))
                {
                    vecItem_multi[nCur].second->at(nSize).AVG_GV = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).AVG_GV > 0)
                        vecItem_multi[nCur].second->at(nSize).AVG_COMPARE_OPERATOR = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("AVG_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).AVG_GV_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).AVG_GV_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).AVG_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("MAX"))
                {
                    vecItem_multi[nCur].second->at(nSize).MAX_GV = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).MAX_GV > 0)
                        vecItem_multi[nCur].second->at(nSize).MAX_GV_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("MAX_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).MAX_GV_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).MAX_GV_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).MAX_GV_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("BRIGHT_AVG"))
                {
                    vecItem_multi[nCur].second->at(nSize).BRIGHT_AVG_GV = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).BRIGHT_AVG_GV > 0)
                        vecItem_multi[nCur].second->at(nSize).BRIGHT_AVG_GV_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("BRIGHT_AVG_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).BRIGHT_AVG_GV_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).BRIGHT_AVG_GV_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).BRIGHT_AVG_GV_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("DARK_AVG"))
                {
                    vecItem_multi[nCur].second->at(nSize).DARK_AVG_GV = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).DARK_AVG_GV > 0)
                        vecItem_multi[nCur].second->at(nSize).DARK_AVG_GV_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("DARK_AVG_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).DARK_AVG_GV_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).DARK_AVG_GV_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).DARK_AVG_GV_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("MASS_CENTERGV"))
                {
                    vecItem_multi[nCur].second->at(nSize).MASS_CENTER_GV_GV = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).MASS_CENTER_GV_GV > 0)
                        vecItem_multi[nCur].second->at(nSize).MASS_CENTER_GV_GV_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("MASS_CENTERGV_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).MASS_CENTER_GV_GV_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).MASS_CENTER_GV_GV_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).MASS_CENTER_GV_GV_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("EDGE_ENERGY"))
                {
                    vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY > 0)
                        vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("EDGE_ENERGY_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("BLACK_EDGE_ENERGY"))
                {
                    vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_BLACK = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_BLACK > 0)
                        vecItem_multi[nCur].second->at(nSize).BLACK_EDGE_ENERGY_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("BLACK_EDGE_ENERGY_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_BLACK_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_BLACK_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).BLACK_EDGE_ENERGY_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("WHITE_EDGE_ENERGY"))
                {
                    vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_WHITE = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_WHITE > 0)
                        vecItem_multi[nCur].second->at(nSize).WHITE_EDGE_ENERGY_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("WHITE_EDGE_ENERGY_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_WHITE_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_WHITE_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).WHITE_EDGE_ENERGY_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("SECTION_THRESHOLD"))
                {
                    vecItem_multi[nCur].second->at(nSize).SECTION_THRESHOLD = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).SECTION_THRESHOLD > 0)
                        vecItem_multi[nCur].second->at(nSize).SECTION_THRESHOLD_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("SECTION_THRESHOLD_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).SECTION_THRESHOLD_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).SECTION_THRESHOLD_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).SECTION_THRESHOLD_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("HOR"))
                {
                    vecItem_multi[nCur].second->at(nSize).HOR = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).HOR > 0)
                        vecItem_multi[nCur].second->at(nSize).HOR_COMPARE_OPERATOR = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("HOR_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).HOR_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).HOR_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).HOR_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("VER"))
                {
                    vecItem_multi[nCur].second->at(nSize).VER = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).VER > 0)
                        vecItem_multi[nCur].second->at(nSize).VER_COMPARE_OPERATOR = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("VER_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).VER_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).VER_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).VER_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("RATIO_HV"))
                {
                    vecItem_multi[nCur].second->at(nSize).RATIO_HV = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).RATIO_HV > 0)
                        vecItem_multi[nCur].second->at(nSize).RATIO_HV_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("RATIO_HV_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).RATIO_HV_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).RATIO_HV_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).RATIO_HV_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("RATIO_VH"))
                {
                    vecItem_multi[nCur].second->at(nSize).RATIO_VH = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).RATIO_VH > 0)
                        vecItem_multi[nCur].second->at(nSize).RATIO_VH_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("RATIO_VH_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).RATIO_VH_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).RATIO_VH_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).RATIO_VH_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("RATIO_WHITE"))
                {
                    vecItem_multi[nCur].second->at(nSize).RATIO_WHITE = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).RATIO_WHITE > 0)
                        vecItem_multi[nCur].second->at(nSize).RATIO_WHITE_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("RATIO_WHITE_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).RATIO_WHITE_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).RATIO_WHITE_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).RATIO_WHITE_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("RATIO_DARK"))
                {
                    vecItem_multi[nCur].second->at(nSize).RATIO_DARK = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).RATIO_DARK > 0)
                        vecItem_multi[nCur].second->at(nSize).RATIO_DARK_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("RATIO_DARK_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).RATIO_DARK_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).RATIO_DARK_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).RATIO_DARK_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("OVER_GV_PX_COUNT"))
                {
                    vecItem_multi[nCur].second->at(nSize).OVER_200_GV_PIXEL_COUNT = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).OVER_200_GV_PIXEL_COUNT > 0)
                        vecItem_multi[nCur].second->at(nSize).OVER_GV_PX_COUNT_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("OVER_GV_PX_COUNT_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).OVER_200_GV_PIXEL_COUNT_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).OVER_200_GV_PIXEL_COUNT_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).OVER_GV_PX_COUNT_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("DIS_TO_EDGE_mm"))
                {
                    vecItem_multi[nCur].second->at(nSize).DISTANCE_TO_EDGE_2_mm = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).DISTANCE_TO_EDGE_2_mm > 0)
                        vecItem_multi[nCur].second->at(nSize).DIS_TO_EDGE_2_mm_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("DIS_TO_EDGE_mm_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).DISTANCE_TO_EDGE_2_mm_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).DISTANCE_TO_EDGE_2_mm_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).DIS_TO_EDGE_2_mm_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("IS_NEAR_EDGE"))
                {
                    vecItem_multi[nCur].second->at(nSize).IS_NEAR_EDGE = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).IS_NEAR_EDGE > -1)
                        vecItem_multi[nCur].second->at(nSize).IS_NEAR_EDGE_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("IS_NEAR_EDGE_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).IS_NEAR_EDGE_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).IS_NEAR_EDGE_2 > -1)
                        vecItem_multi[nCur].second->at(nSize).IS_NEAR_EDGE_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("EDGE_ENERGY_SHARPNESS"))
                {
                    vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_SHARPNESS = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_SHARPNESS > 0)
                        vecItem_multi[nCur].second->at(nSize).SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("EDGE_ENERGY_SHARPNESS_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_SHARPNESS_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_SHARPNESS_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }
                if (vecIndex[n] == _T("EDGE_ENERGY_SHARPNESS_WHITE"))
                {
                    vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_SHARPNESS_WHITE = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_SHARPNESS_WHITE > 0)
                        vecItem_multi[nCur].second->at(nSize).WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("EDGE_ENERGY_SHARPNESS_WHITE_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_SHARPNESS_WHITE_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_SHARPNESS_WHITE_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }
                if (vecIndex[n] == _T("EDGE_ENERGY_SHARPNESS_BLACK"))
                {
                    vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_SHARPNESS_BLACK = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_SHARPNESS_BLACK > 0)
                        vecItem_multi[nCur].second->at(nSize).BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR
                            = classification::BIGGER_THAN_SPEC;
                }

                if (vecIndex[n] == _T("EDGE_ENERGY_SHARPNESS_BLACK_2"))
                {
                    vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_SHARPNESS_BLACK_2 = _ttof(vecDiv[n]);
                    if (vecItem_multi[nCur].second->at(nSize).EDGE_ENERGY_SHARPNESS_BLACK_2 > 0)
                        vecItem_multi[nCur].second->at(nSize).BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2
                            = classification::SMALLER_THAN_SPEC;
                }
            }
        }
        SelectDispCondition(nCur);
    }
}

void CDlgClassificationST::InitSpecSize(DefectType dt, CISISpecInfo& spec)
{
    if (dt == DefectType::ConnectionTape || dt == DefectType::DarkConnectionTape)
    {
        spec.nCategory = enCategory::CTG_CONNECTION_TAPE;
    }

#if INSPECTION_MACHINE_UC1
    spec.SIZE_X_mm = m_vecTableTemp[(long)dt - (long)DefectType::White_Line].second.at(0).SIZE_X_mm;
    spec.SIZE_Y_mm = m_vecTableTemp[(long)dt - (long)DefectType::White_Line].second.at(0).SIZE_Y_mm;
    spec.SIZEX_COMPARE_OPERATOR = m_vecTableTemp[(long)dt - (long)DefectType::White_Line].second.at(0).SIZEX_COMPARE_OPERATOR;
    spec.SIZEY_COMPARE_OPERATOR = m_vecTableTemp[(long)dt - (long)DefectType::White_Line].second.at(0).SIZEY_COMPARE_OPERATOR;
    spec.ORAND = m_vecTableTemp[(long)dt - (long)DefectType::White_Line].second.at(0).ORAND;
    spec.nInspectionArea = m_vecTableTemp[(long)dt - (long)DefectType::White_Line].second.at(0).nInspectionArea;
#else
    spec.SIZE_X_mm = m_vecTableTemp[(long)dt - (long)DefectType::Line].second.at(0).SIZE_X_mm;
    spec.SIZE_Y_mm = m_vecTableTemp[(long)dt - (long)DefectType::Line].second.at(0).SIZE_Y_mm;
    spec.SIZEX_COMPARE_OPERATOR = m_vecTableTemp[(long)dt - (long)DefectType::Line].second.at(0).SIZEX_COMPARE_OPERATOR;
    spec.SIZEY_COMPARE_OPERATOR = m_vecTableTemp[(long)dt - (long)DefectType::Line].second.at(0).SIZEY_COMPARE_OPERATOR;
    spec.ORAND = m_vecTableTemp[(long)dt - (long)DefectType::Line].second.at(0).ORAND;
    spec.nInspectionArea = m_vecTableTemp[(long)dt - (long)DefectType::Line].second.at(0).nInspectionArea;
#endif // INSPECTION_MACHINE_UC1


    

    //if (dt == DefectType::Line)
    //{
    //    spec.SIZE_X_mm = 0.05;
    //    spec.SIZE_Y_mm = 5;
    //    spec.ORAND = true;
    //    spec.nInspectionArea = enInspectionArea::IA_ELECTRODE;
    //}

    //if (dt == DefectType::Pinhole)
    //{
    //    /*spec.SIZE_X_mm = 3;
    //    spec.SIZE_Y_mm = 3;
    //    spec.ORAND = false;
    //    spec.nInspectionArea = enInspectionArea::IA_ELECTRODE;*/
    //}

    //if (dt == DefectType::Dent)
    //{
    //    /*spec.SIZE_X_mm = 5;
    //    spec.SIZE_Y_mm = 5;
    //    spec.ORAND = true;
    //    spec.nInspectionArea = enInspectionArea::IA_ELECTRODE;*/
    //}

    //if (dt == DefectType::Horizontal_Line)
    //{
    //    /*spec.SIZE_X_mm = 0.1;
    //    spec.SIZE_Y_mm = 10;
    //    spec.ORAND = true;
    //    spec.nInspectionArea = enInspectionArea::IA_ELECTRODE;*/
    //}

    //if (dt == DefectType::Stain)
    //{
    //    /*spec.SIZE_X_mm = 3;
    //    spec.SIZE_Y_mm = 5;
    //    spec.ORAND = true;
    //    spec.nInspectionArea = enInspectionArea::IA_ELECTRODE;*/
    //}

    //if (dt == DefectType::Crater)
    //{
    //    //spec.SIZE_X_mm = 3;
    //    //spec.SIZE_Y_mm = 5;
    //    //spec.ORAND = true;
    //    //spec.nInspectionArea = enInspectionArea::IA_ELECTRODE;
    //}

    //if (dt == DefectType::Island)
    //{
    //    //spec.SIZE_X_mm = 1;
    //    //spec.SIZE_Y_mm = 1;
    //    //spec.ORAND = false;
    //    //spec.nInspectionArea = enInspectionArea::IA_TAB;
    //}

    //if (dt == DefectType::Chipping_Pinhole)
    //{
    //    spec.SIZE_X_mm = 0.8;
    //    spec.SIZE_Y_mm = 0.8;
    //    spec.ORAND = false;
    //    spec.nInspectionArea = enInspectionArea::IA_ELECTRODE_EDGE;
    //}

    //if (dt == DefectType::Drag)
    //{
    //    spec.SIZE_X_mm = 1;
    //    spec.SIZE_Y_mm = 10;
    //    spec.ORAND = false;
    //    spec.nInspectionArea = enInspectionArea::IA_ELECTRODE_EDGE;
    //}

    //if (dt == DefectType::Edge_Line)
    //{
    //    spec.SIZE_X_mm = 0.05;
    //    spec.SIZE_Y_mm = 5;
    //    spec.ORAND = true;
    //    spec.nInspectionArea = enInspectionArea::IA_ELECTRODE_EDGE;
    //}

    //if (dt == DefectType::Balck_Dot)
    //{
    //    spec.SIZE_X_mm = 3;
    //    spec.SIZE_Y_mm = 5;
    //    spec.ORAND = false;
    //    spec.nInspectionArea = enInspectionArea::IA_ELECTRODE;
    //}

    //if (dt == DefectType::Scratch)
    //{
    //    spec.SIZE_X_mm = 0.1;
    //    spec.SIZE_Y_mm = 1;
    //    spec.ORAND = false;
    //    spec.nInspectionArea = enInspectionArea::IA_ELECTRODE;
    //}

    //if (dt == DefectType::Crack)
    //{
    //    spec.SIZE_X_mm = 1;
    //    spec.SIZE_Y_mm = 1;
    //    spec.ORAND = false;
    //    spec.nInspectionArea = enInspectionArea::IA_ELECTRODE;
    //}

    //if (dt == DefectType::Delamination_Crack)
    //{
    //    spec.SIZE_X_mm = 20;
    //    spec.SIZE_Y_mm = 20;
    //    spec.ORAND = true;
    //    spec.nInspectionArea = enInspectionArea::IA_ELECTRODE;
    //}

    //if (dt == DefectType::Plain_Wrinkle)
    //{
    //    spec.SIZE_X_mm = 1;
    //    spec.SIZE_Y_mm = 1;
    //    spec.ORAND = false;
    //    spec.nInspectionArea = enInspectionArea::IA_TAB;
    //    spec.nInspectImage = enInspectionImageType::Dark;
    //    spec.INSPECTION_IMAGE_TYPE = enInspectionImageType::Dark;
    //}

    //if (dt == DefectType::Edge_Defect)
    //{
    //    spec.SIZE_X_mm = 1;
    //    spec.SIZE_Y_mm = 1;
    //    spec.ORAND = false;
    //    spec.nInspectionArea = enInspectionArea::IA_ELECTRODE_EDGE;
    //}

    //if (dt == DefectType::ETC_Defect)
    //{
    //    spec.SIZE_X_mm = 1;
    //    spec.SIZE_Y_mm = 1;
    //    spec.ORAND = false;
    //    spec.nInspectionArea = enInspectionArea::IA_ELECTRODE;
    //}
}

BOOL CDlgClassificationST::StringTokenDivideToCString(CString strFull, TCHAR chDivide, std::vector<CString>& vecstrValue)
    {
    long nDivideTotalCount = 0;
    CString strSub;
    while (FALSE != AfxExtractSubString(strSub, strFull, nDivideTotalCount++, chDivide))
    {
        vecstrValue.push_back(strSub);
    }
    return TRUE;
}

std::vector<CString> CDlgClassificationST::SelectDispCondition(long nRow)
{
    //CString strDisplayCondition;
    
    std::vector<CString> vecTotalCondtion;

    std::vector<CISISpecInfo>* vecSpec = m_ISI_Spec_table.GetSpecInfo_Multi(vecItem_multi[nRow].first);

    
    for (long n = 0; n < vecSpec->size(); n++)
    {
        CString strTemp;
        CString strDisplayCondition;
        if ((*vecSpec)[n].AREA_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("Area %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].AREA_COMPARE_OPERATOR.GetValue()), (*vecSpec)[n].AREA.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].AREA_RATIO_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("RATIO_Area %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].AREA_RATIO_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].AREA_Ratio.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].RATIO_XY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("RATIO_XY %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].RATIO_XY_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].RATIO_XY.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].RATIO_YX_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            if ((*vecSpec)[n].RATIO_YX.GetValue() > 0)
            {
                strTemp.Format(_T("RATIO_YX %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].RATIO_YX_COMPARE_OPERATOR.GetValue()),
                    (*vecSpec)[n].RATIO_YX.GetValue());
                strDisplayCondition += strTemp;
            }
        }

        if ((*vecSpec)[n].WIDTH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("Width %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].WIDTH_COMPARE_OPERATOR.GetValue()), (*vecSpec)[n].WIDTH_mm.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].LENGTH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            if ((*vecSpec)[n].LENGTH_mm.GetValue() > 0)
            {
                strTemp.Format(_T("Length %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].LENGTH_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].LENGTH_mm.GetValue());
                strDisplayCondition += strTemp;
            }
        }

        if ((*vecSpec)[n].RATIO_WL_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("RATIO_WL %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].RATIO_WL_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].RATIO_WL.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].RATIO_LW_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("RATIO_LW %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].RATIO_LW_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].RATIO_LW.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].MIN_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            if ((*vecSpec)[n].MIN_GV.GetValue() > 0)
            {
                strTemp.Format(_T("MIN_GV %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].MIN_GV_COMPARE_OPERATOR.GetValue()), (*vecSpec)[n].MIN_GV.GetValue());
                strDisplayCondition += strTemp;
            }
        }

        if ((*vecSpec)[n].AVG_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            if ((*vecSpec)[n].AVG_GV.GetValue() > 0)
            {
                strTemp.Format(_T("AVG_GV %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].AVG_COMPARE_OPERATOR.GetValue()), (*vecSpec)[n].AVG_GV.GetValue());
                strDisplayCondition += strTemp;
            }
        }

        if ((*vecSpec)[n].MAX_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            if ((*vecSpec)[n].MAX_GV.GetValue() > 0)
            {
                strTemp.Format(_T("MAX_GV %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].MAX_GV_COMPARE_OPERATOR.GetValue()), (*vecSpec)[n].MAX_GV.GetValue());
                strDisplayCondition += strTemp;
            }
        }

        if ((*vecSpec)[n].BRIGHT_AVG_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            if ((*vecSpec)[n].BRIGHT_AVG_GV.GetValue() > 0)
            {
                strTemp.Format(_T("BRIGHT_AVG_GV %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].BRIGHT_AVG_GV_COMPARE_OPERATOR.GetValue()), (*vecSpec)[n].BRIGHT_AVG_GV.GetValue());
                strDisplayCondition += strTemp;
            }
        }

        if ((*vecSpec)[n].DARK_AVG_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("DARK_AVG_GV %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].DARK_AVG_GV_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].DARK_AVG_GV.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].MASS_CENTER_GV_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("MASS_CENTER_GV %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].MASS_CENTER_GV_GV_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].MASS_CENTER_GV_GV.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("EDGE_ENERGY %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].EDGE_ENERGY.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].BLACK_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("EDGE_ENERGY_BLACK %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].BLACK_EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].EDGE_ENERGY_BLACK.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].WHITE_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("EDGE_ENERGY_WHITE %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].WHITE_EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].EDGE_ENERGY_WHITE.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].SECTION_THRESHOLD_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("SECTION_THRESHOLD %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].SECTION_THRESHOLD_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].SECTION_THRESHOLD.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].HOR_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("HOR %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].HOR_COMPARE_OPERATOR.GetValue()), (*vecSpec)[n].HOR.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].VER_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("VER %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].VER_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].VER.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].RATIO_HV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("RATIO_HV %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].RATIO_HV_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].RATIO_HV.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].RATIO_VH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("RATIO_VH %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].RATIO_VH_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].RATIO_VH.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].RATIO_WHITE_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            if ((*vecSpec)[n].RATIO_WHITE.GetValue() > 0)
            {
                strTemp.Format(_T("RATIO_WHITE %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].RATIO_WHITE_COMPARE_OPERATOR.GetValue()),
                    (*vecSpec)[n].RATIO_WHITE.GetValue());
                strDisplayCondition += strTemp;
            }
        }

        if ((*vecSpec)[n].RATIO_DARK_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            if ((*vecSpec)[n].RATIO_DARK.GetValue() > 0)
            {
                strTemp.Format(_T("RATIO_DARK %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].RATIO_DARK_COMPARE_OPERATOR.GetValue()),
                    (*vecSpec)[n].RATIO_DARK.GetValue());
                strDisplayCondition += strTemp;
            }
        }

        if ((*vecSpec)[n].OVER_GV_PX_COUNT_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("250>CNT %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].OVER_GV_PX_COUNT_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].OVER_200_GV_PIXEL_COUNT.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].DIS_TO_EDGE_mm_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("DISTANCE_TO_EDGE %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].DIS_TO_EDGE_mm_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].DISTANCE_TO_EDGE_mm.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].IS_NEAR_EDGE_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("IS_NEAR_EDGE %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].IS_NEAR_EDGE_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].IS_NEAR_EDGE.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("SHARPNESS_EDGE_ENERGY %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].EDGE_ENERGY_SHARPNESS.GetValue());
            strDisplayCondition += strTemp;
        }
        if ((*vecSpec)[n].WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("WHITE_SHARPNESS_EDGE_ENERGY %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].EDGE_ENERGY_SHARPNESS_WHITE.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("BLACK_SHARPNESS_EDGE_ENERGY %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
                (*vecSpec)[n].EDGE_ENERGY_SHARPNESS_BLACK.GetValue());
            strDisplayCondition += strTemp;
        }
		//_2 변수 Display Data 추가
        if ((*vecSpec)[n].AREA_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(
                _T("Area %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].AREA_COMPARE_OPERATOR_2.GetValue()), (*vecSpec)[n].AREA_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].AREA_RATIO_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("RATIO_Area %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].AREA_RATIO_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].AREA_Ratio_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].RATIO_XY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("RATIO_XY %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].RATIO_XY_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].RATIO_XY_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].RATIO_YX_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("RATIO_YX %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].RATIO_YX_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].RATIO_YX_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].WIDTH_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("Width %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].WIDTH_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].WIDTH_mm_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].LENGTH_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("Length %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].LENGTH_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].LENGTH_mm_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].RATIO_WL_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("RATIO_WL %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].RATIO_WL_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].RATIO_WL_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].RATIO_LW_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("RATIO_LW %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].RATIO_LW_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].RATIO_LW_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].MIN_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            if ((*vecSpec)[n].MIN_GV_2.GetValue() > 0)
            {
                strTemp.Format(
                    _T("MIN_GV %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].MIN_GV_COMPARE_OPERATOR_2.GetValue()), (*vecSpec)[n].MIN_GV_2.GetValue());
                strDisplayCondition += strTemp;
            }
        }

        if ((*vecSpec)[n].AVG_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            if ((*vecSpec)[n].AVG_GV_2.GetValue() > 0)
            {
                strTemp.Format(
                    _T("AVG_GV %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].AVG_COMPARE_OPERATOR_2.GetValue()), (*vecSpec)[n].AVG_GV_2.GetValue());
                strDisplayCondition += strTemp;
            }
        }

        if ((*vecSpec)[n].MAX_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(
                _T("MAX_GV %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].MAX_GV_COMPARE_OPERATOR_2.GetValue()), (*vecSpec)[n].MAX_GV_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].BRIGHT_AVG_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("BRIGHT_AVG_GV %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].BRIGHT_AVG_GV_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].BRIGHT_AVG_GV_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].DARK_AVG_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("DARK_AVG_GV %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].DARK_AVG_GV_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].DARK_AVG_GV_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].MASS_CENTER_GV_GV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("MASS_CENTER_GV %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].MASS_CENTER_GV_GV_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].MASS_CENTER_GV_GV_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("EDGE_ENERGY %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].EDGE_ENERGY_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].BLACK_EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("EDGE_ENERGY_BLACK %s %.2f & "),
                enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].BLACK_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()), (*vecSpec)[n].EDGE_ENERGY_BLACK_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].WHITE_EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("EDGE_ENERGY_WHITE %s %.2f & "),
                enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].WHITE_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()), (*vecSpec)[n].EDGE_ENERGY_WHITE_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].SECTION_THRESHOLD_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("SECTION_THRESHOLD %s %.2f & "),
                enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].SECTION_THRESHOLD_COMPARE_OPERATOR_2.GetValue()), (*vecSpec)[n].SECTION_THRESHOLD_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].HOR_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(
                _T("HOR %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].HOR_COMPARE_OPERATOR_2.GetValue()), (*vecSpec)[n].HOR_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].VER_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(
                _T("VER %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].VER_COMPARE_OPERATOR_2.GetValue()), (*vecSpec)[n].VER_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].RATIO_HV_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("RATIO_HV %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].RATIO_HV_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].RATIO_HV_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].RATIO_VH_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("RATIO_VH %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].RATIO_VH_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].RATIO_VH_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].RATIO_WHITE_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("RATIO_WHITE %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].RATIO_WHITE_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].RATIO_WHITE_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].RATIO_DARK_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("RATIO_DARK %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].RATIO_DARK_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].RATIO_DARK_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].OVER_GV_PX_COUNT_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("250>CNT %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].OVER_GV_PX_COUNT_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].OVER_200_GV_PIXEL_COUNT_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].DIS_TO_EDGE_mm_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("DISTANCE_TO_EDGE %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].DIS_TO_EDGE_mm_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].DISTANCE_TO_EDGE_mm_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].IS_NEAR_EDGE_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("IS_NEAR_EDGE %s %d & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].IS_NEAR_EDGE_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].IS_NEAR_EDGE_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("SHARPNESS_EDGE_ENERGY %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].EDGE_ENERGY_SHARPNESS_2.GetValue());
            strDisplayCondition += strTemp;
        }
        if ((*vecSpec)[n].WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("WHITE_SHARPNESS_EDGE_ENERGY)2 %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].WHITE_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].EDGE_ENERGY_SHARPNESS_WHITE_2.GetValue());
            strDisplayCondition += strTemp;
        }

        if ((*vecSpec)[n].BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2 != enCompareOperator::NOT_USE)
        {
            strTemp.Format(_T("BLACK_SHARPNESS_EDGE_ENERGY_2 %s %.2f & "), enumtoString().CompareOperator2String((enCompareOperator)(*vecSpec)[n].BLACK_SHARPNESS_EDGE_ENERGY_COMPARE_OPERATOR_2.GetValue()),
                (*vecSpec)[n].EDGE_ENERGY_SHARPNESS_BLACK_2.GetValue());
            strDisplayCondition += strTemp;
        }
        //strTemp.Format(_T("Inspection_ImageType : %s"), enumtoString().InspectionImageType2String((enInspectionImageType)(*vecSpec)[n].INSPECTION_IMAGE_TYPE.GetValue()));
        //strDisplayCondition += strTemp;
        strDisplayCondition = strDisplayCondition.Mid(0, strDisplayCondition.GetLength() - 3);
        vecTotalCondtion.push_back(strDisplayCondition);

        //DisplayData(pDisplay, nrow, ncor);
    }

    return vecTotalCondtion;
}

CString CDlgClassificationST::InitDisplayCondition(long nRow, BOOL bTotal)
{
    CString strDisplayCondition;
    CString strTemp;

    //if (vecItem_multi[nRow].second[0][0].AREA_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("Area %s %.2f,"), enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].AREA_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].AREA.GetValue());
    //    strDisplayCondition += strTemp;
    //}
    //if (vecItem_multi[nRow].second[0][0].AREA_RATIO_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("RATIO_Area %s %.2f,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].AREA_RATIO_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].AREA_Ratio.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].RATIO_XY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("RATIO_XY %s %.2f,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].RATIO_XY_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].RATIO_XY.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].RATIO_YX_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("RATIO_YX %s %.2f,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].RATIO_YX_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].RATIO_YX.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].WIDTH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("Width %s %.2f,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].WIDTH_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].WIDTH_mm.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].LENGTH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("Length %s %.2f,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].LENGTH_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].LENGTH_mm.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].RATIO_WL_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("RATIO_WL %s %.2f,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].RATIO_WL_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].RATIO_WL.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].RATIO_LW_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("RATIO_LW %s %.2f,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].RATIO_LW_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].RATIO_LW.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].MIN_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("MIN_GV %s %d,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].MIN_GV_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].MIN_GV.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].AVG_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("AVG_GV %s %d,"), enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].AVG_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].AVG_GV.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].MAX_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("MAX_GV %s %d,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].MAX_GV_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].MAX_GV.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].BRIGHT_AVG_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("BRIGHT_AVG_GV %s %d,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].BRIGHT_AVG_GV_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].BRIGHT_AVG_GV.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].DARK_AVG_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("DARK_AVG_GV %s %d,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].DARK_AVG_GV_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].DARK_AVG_GV.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].MASS_CENTER_GV_GV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("MASS_CENTER_GV %s %.2f,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].MASS_CENTER_GV_GV_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].MASS_CENTER_GV_GV.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("EDGE_ENERGY %s %.2f,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].EDGE_ENERGY.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].BLACK_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("EDGE_ENERGY_BLACK %s %.2f,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].BLACK_EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].EDGE_ENERGY_BLACK.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].WHITE_EDGE_ENERGY_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("EDGE_ENERGY_WHITE %s %.2f,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].WHITE_EDGE_ENERGY_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].EDGE_ENERGY_WHITE.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].SECTION_THRESHOLD_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("SECTION_THRESHOLD %s %.2f,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].SECTION_THRESHOLD_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].SECTION_THRESHOLD.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].HOR_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("HOR %s %.2f,"), enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].HOR_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].HOR.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].VER_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("VER %s %.2f,"), enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].VER_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].VER.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].RATIO_HV_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("RATIO_HV %s %.2f,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].RATIO_HV_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].RATIO_HV.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].RATIO_VH_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("RATIO_VH %s %.2f,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].RATIO_VH_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].RATIO_VH.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].RATIO_WHITE_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("RATIO_WHITE %s %.2f,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].RATIO_WHITE_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].RATIO_WHITE.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].RATIO_DARK_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("RATIO_DARK %s %.2f,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].RATIO_DARK_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].RATIO_DARK.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].OVER_GV_PX_COUNT_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("250>CNT %s %d,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].OVER_GV_PX_COUNT_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].OVER_200_GV_PIXEL_COUNT.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].DIS_TO_EDGE_mm_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("DISTANCE_TO_EDGE %s %.2f,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].DIS_TO_EDGE_mm_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].DISTANCE_TO_EDGE_mm.GetValue());
    //    strDisplayCondition += strTemp;
    //}

    //if (vecItem_multi[nRow].second[0][0].IS_NEAR_EDGE_COMPARE_OPERATOR != enCompareOperator::NOT_USE)
    //{
    //    strTemp.Format(_T("IS_NEAR_EDGE %s %d,"),
    //        enumtoString().CompareOperator2String((enCompareOperator)vecItem_multi[nRow].second[0][0].IS_NEAR_EDGE_COMPARE_OPERATOR.GetValue()),
    //        vecItem_multi[nRow].second[0][0].IS_NEAR_EDGE.GetValue());
    //    strDisplayCondition += strTemp;
    //}


    strTemp.Format(
        _T("Inspection_ImageType : %s"), enumtoString().InspectionImageType2String((enInspectionImageType)vecItem_multi[nRow].second[0][0].INSPECTION_IMAGE_TYPE.GetValue()));
    strDisplayCondition += strTemp;

	return strDisplayCondition;

}

LRESULT CDlgClassificationST::OnUWMGridUpdate(WPARAM wParam, LPARAM lParam)
{
    m_ctrGrid.SetRedraw(TRUE);
    m_ctrGrid.Invalidate();
	return 0;
}

void CDlgClassificationST::OnBnClickedBtnSaveSpecCsv()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    const TCHAR szFilters[] = _T("CSV Files (*.csv)|*.csv||");

    CFileDialog dlg(FALSE, _T(""), _T("*.csv"), OFN_CREATEPROMPT, szFilters);
    
    std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>> vecSpec(m_ISI_Spec_table.m_mapSpec_multi.begin(), m_ISI_Spec_table.m_mapSpec_multi.end());
    std::vector<std::pair<DefectType, std::vector<CISISpecInfo>>>::iterator vecitrStartVec, vecitrEndVec;
    vecitrStartVec = vecSpec.begin();
    vecitrEndVec = vecSpec.end();
    long nIndex = 0;

    if (dlg.DoModal() == IDOK)
    {
        CString strPath = dlg.GetPathName();

        CFile file;
        CFileException fe;

        if (file.Open(strPath, CFile::modeCreate | CFile::modeWrite | CFile::typeUnicode | CFile::modeNoTruncate))
        {
            CString str = _T("ITEM,CONDITION \r\n");
            CString strTitle = _T("ITEM,SIZE_X,SIZE_X_CO,SIZE_Y,SIZE_Y_CO,ORAND,INSPECTION_AREA,AREA,AREA_CO,AREA_Ratio,AREA_Ratio_CO,INSPECTION_IMAGE_TYPE,RATIO_XY,RATIO_XY_CO,RATIO_YX,RATIO_YX_CO,WIDTH,WIDTH_CO,LENGTH,LENGTH_CO,RATIO_WL,RATIO_WL_CO,RATIO_LW,RATIO_LW_CO,MIN,MIN_CO,AVG,AVG_CO,MAX,MAX_CO,BRIGHT_AVG,BRIGHT_AVG_CO,DARK_AVG,DARK_AVG_CO,MASS_CENTERGV,MASS_CENTERGV_CO,EDGE_ENERGY,EDGE_ENERGY_CO,BLACK_EDGE_ENERGY,BLACK_EDGE_ENERGY_CO,WHITE_EDGE_ENERGY,WHITE_EDGE_ENERGY_CO,SECTION_THRESHOLD,SECTION_THRESHOLD_CO,HOR,HOR_CO,VER,VER_CO,RATIO_HV,RATIO_HV_CO,RATIO_VH,RATIO_VH_CO,RATIO_WHITE,RATIO_WHITE_CO,RATIO_DARK,RATIO_DARK_CO,OVER_GV_PX_COUNT,OVER_GV_PX_COUNT_CO,DIS_TO_EDGE_mm,DIS_TO_EDGE_mm_CO,IS_NEAR_EDGE,IS_NEAR_EDGE_CO,\r\n");
            CString strTitle2 = _T("ITEM,SIZE_X,SIZE_X_CO,SIZE_Y,SIZE_Y_CO,ORAND,INSPECTION_AREA,AREA,AREA_CO,AREA_2,AREA_CO_2,AREA_Ratio,AREA_Ratio_CO,AREA_Ratio_2,AREA_Ratio_CO_2,INSPECTION_IMAGE_TYPE,RATIO_XY,RATIO_XY_CO,RATIO_XY_2,RATIO_XY_CO_2,RATIO_YX,RATIO_YX_CO,RATIO_YX_2,RATIO_YX_CO_2,WIDTH,WIDTH_CO,WIDTH_2,WIDTH_CO_2,LENGTH,LENGTH_CO,LENGTH_2,LENGTH_CO_2,RATIO_WL,RATIO_WL_CO,RATIO_WL_2,RATIO_WL_CO_2,RATIO_LW,RATIO_LW_CO,RATIO_LW_2,RATIO_LW_CO_2,MIN,MIN_CO,MIN_2,MIN_CO_2,AVG,AVG_CO,AVG_2,AVG_CO_2,MAX,MAX_CO,MAX_2,MAX_CO_2,BRIGHT_AVG,BRIGHT_AVG_CO,BRIGHT_AVG_2,BRIGHT_AVG_CO_2,DARK_AVG,DARK_AVG_CO,DARK_AVG_2,DARK_AVG_CO_2,MASS_CENTERGV,MASS_CENTERGV_CO,MASS_CENTERGV_2,MASS_CENTERGV_CO_2,EDGE_ENERGY,EDGE_ENERGY_CO,EDGE_ENERGY_2,EDGE_ENERGY_CO_2,BLACK_EDGE_ENERGY,BLACK_EDGE_ENERGY_CO,BLACK_EDGE_ENERGY_2,BLACK_EDGE_ENERGY_CO_2,WHITE_EDGE_ENERGY,WHITE_EDGE_ENERGY_CO,WHITE_EDGE_ENERGY_2,WHITE_EDGE_ENERGY_CO_2,SECTION_THRESHOLD,SECTION_THRESHOLD_CO,SECTION_THRESHOLD_2,SECTION_THRESHOLD_CO_2,HOR,HOR_CO,HOR_2,HOR_CO_2,VER,VER_CO,VER_2,VER_CO_2,RATIO_HV,RATIO_HV_CO,RATIO_HV_2,RATIO_HV_CO_2,RATIO_VH,RATIO_VH_CO,RATIO_VH_2,RATIO_VH_CO_2,RATIO_WHITE,RATIO_WHITE_CO,RATIO_WHITE_2,RATIO_WHITE_CO_2,RATIO_DARK,RATIO_DARK_CO,RATIO_DARK_2,RATIO_DARK_CO_2,OVER_GV_PX_COUNT,OVER_GV_PX_COUNT_CO,OVER_GV_PX_COUNT_2,OVER_GV_PX_COUNT_CO_2,DIS_TO_EDGE_mm,DIS_TO_EDGE_mm_CO,DIS_TO_EDGE_mm_2,DIS_TO_EDGE_mm_CO_2,IS_NEAR_EDGE,IS_NEAR_EDGE_COIS_NEAR_EDGE_2,IS_NEAR_EDGE_CO_2,\r\n");
            CString strTitle3 = _T("ITEM,SIZE_X,SIZE_X_CO,SIZE_Y,SIZE_Y_CO,ORAND,INSPECTION_AREA,AREA,AREA_2,AREA_Ratio,AREA_Ratio_2,INSPECTION_IMAGE_TYPE,RATIO_XY,RATIO_XY_2,RATIO_YX,RATIO_YX_2,WIDTH,WIDTH_2,LENGTH,LENGTH_2,RATIO_WL,RATIO_WL_2,RATIO_LW,RATIO_LW_2,MIN,MIN_2,AVG,AVG_2,MAX,MAX_2,BRIGHT_AVG,BRIGHT_AVG_2,DARK_AVG,DARK_AVG_2,MASS_CENTERGV,MASS_CENTERGV_2,EDGE_ENERGY,EDGE_ENERGY_2,BLACK_EDGE_ENERGY,BLACK_EDGE_ENERGY_2,WHITE_EDGE_ENERGY,WHITE_EDGE_ENERGY_2,SECTION_THRESHOLD,SECTION_THRESHOLD_2,HOR,HOR_2,VER,VER_2,RATIO_HV,RATIO_HV_2,RATIO_VH,RATIO_VH_2,RATIO_WHITE,RATIO_WHITE_2,RATIO_DARK,RATIO_DARK_2,OVER_GV_PX_COUNT,OVER_GV_PX_COUNT_2,DIS_TO_EDGE_mm,DIS_TO_EDGE_mm_2,IS_NEAR_EDGE,IS_NEAR_EDGE_2,\r\n");
            CString strTitle4 = _T("ITEM,ENABLE,PRIORITY,SIZE_X,SIZE_X_CO,SIZE_Y,SIZE_Y_CO,ORAND,INSPECTION_AREA,AREA,AREA_2,AREA_Ratio,AREA_Ratio_2,INSPECTION_IMAGE_TYPE,RATIO_XY,RATIO_XY_2,RATIO_YX,RATIO_YX_2,WIDTH,WIDTH_2,LENGTH,LENGTH_2,RATIO_WL,RATIO_WL_2,RATIO_LW,RATIO_LW_2,MIN,MIN_2,AVG,AVG_2,MAX,MAX_2,BRIGHT_AVG,BRIGHT_AVG_2,DARK_AVG,DARK_AVG_2,MASS_CENTERGV,MASS_CENTERGV_2,EDGE_ENERGY,EDGE_ENERGY_2,BLACK_EDGE_ENERGY,BLACK_EDGE_ENERGY_2,WHITE_EDGE_ENERGY,WHITE_EDGE_ENERGY_2,SECTION_THRESHOLD,SECTION_THRESHOLD_2,HOR,HOR_2,VER,VER_2,RATIO_HV,RATIO_HV_2,RATIO_VH,RATIO_VH_2,RATIO_WHITE,RATIO_WHITE_2,RATIO_DARK,RATIO_DARK_2,OVER_GV_PX_COUNT,OVER_GV_PX_COUNT_2,DIS_TO_EDGE_mm,DIS_TO_EDGE_mm_2,IS_NEAR_EDGE,IS_NEAR_EDGE_2,EDGE_ENERGY_SHARPNESS,EDGE_ENERGY_SHARPNESS_2,EDGE_ENERGY_SHARPNESS_WHITE,EDGE_ENERGY_SHARPNESS_WHITE_2,EDGE_ENERGY_SHARPNESS_BLACK,EDGE_ENERGY_SHARPNESS_BLACK_2, \r\n");
            CStringA strK(strTitle4);
            file.Write(strK, strK.GetLength() * sizeof(char));

            while (vecitrStartVec != vecitrEndVec)
            {
                if ((DefectType)vecitrStartVec->first > DefectType::START_Coater_Surface && (DefectType)vecitrStartVec->first < DefectType::END_Coater_Surface)
                {
                    for (long n = 0; n < vecitrStartVec->second.size(); n++)
                    {
                        CString strTotal, strVal;

                        if (n == 0)
                        {
                            strTotal += GetDefectName(vecitrStartVec->first);
                            strTotal += _T(",");

                            strTotal += vecitrStartVec->second[n].ENABLE.GetValue() ? _T("true,") : _T("x,");

                            strVal.Format(_T("%d,"), vecitrStartVec->second[n].PRIORITY.GetValue()+1);
                            strTotal += strVal;

                            strVal.Format(_T("%f,"), vecitrStartVec->second[n].SIZE_X_mm.GetValue());
                            strTotal += strVal;
                            strTotal += enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].SIZEX_COMPARE_OPERATOR.GetValue());
                            strTotal += _T(",");
                            
                            strVal.Format(_T("%f,"), vecitrStartVec->second[n].SIZE_Y_mm.GetValue());
                            strTotal += strVal;
                            strTotal += enumtoString().CompareOperator2String((enCompareOperator)vecitrStartVec->second[n].SIZEY_COMPARE_OPERATOR.GetValue());
                            strTotal += _T(",");

                            strTotal += enumtoString().OrAnd2String(vecitrStartVec->second[n].ORAND.GetValue());
                            strTotal += _T(",");

                            strTotal += ConvertInspectionArea().GetString((enInspectionArea)vecitrStartVec->second[n].nInspectionArea.GetValue());
                            strTotal += _T(",");
                        }
                        else
                        {
                            strTotal += _T(",,,,,,,,,");
                            //strTotal += _T(",,,,,,,");
                        }

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].AREA.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].AREA_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].AREA_Ratio.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].AREA_Ratio_2.GetValue());
                        strTotal += strVal;

                        strTotal += enumtoString().InspectionImageType2String((enInspectionImageType)vecitrStartVec->second[n].INSPECTION_IMAGE_TYPE.GetValue());
                        strTotal += _T(",");

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].RATIO_XY.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].RATIO_XY_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].RATIO_YX.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].RATIO_YX_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].WIDTH_mm.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].WIDTH_mm_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].LENGTH_mm.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].LENGTH_mm_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].RATIO_WL.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].RATIO_WL_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].RATIO_LW.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].RATIO_LW_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%d,"), vecitrStartVec->second[n].MIN_GV.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%d,"), vecitrStartVec->second[n].MIN_GV_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%d,"), vecitrStartVec->second[n].AVG_GV.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%d,"), vecitrStartVec->second[n].AVG_GV_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%d,"), vecitrStartVec->second[n].MAX_GV.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%d,"), vecitrStartVec->second[n].MAX_GV_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%d,"), vecitrStartVec->second[n].BRIGHT_AVG_GV.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%d,"), vecitrStartVec->second[n].BRIGHT_AVG_GV_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%d,"), vecitrStartVec->second[n].DARK_AVG_GV.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%d,"), vecitrStartVec->second[n].DARK_AVG_GV_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%d,"), vecitrStartVec->second[n].MASS_CENTER_GV_GV.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%d,"), vecitrStartVec->second[n].MASS_CENTER_GV_GV_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].EDGE_ENERGY.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].EDGE_ENERGY_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].EDGE_ENERGY_BLACK.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].EDGE_ENERGY_BLACK_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].EDGE_ENERGY_WHITE.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].EDGE_ENERGY_WHITE_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%d,"), vecitrStartVec->second[n].SECTION_THRESHOLD.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%d,"), vecitrStartVec->second[n].SECTION_THRESHOLD_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].HOR.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].HOR_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].VER.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].VER_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].RATIO_HV.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].RATIO_HV_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].RATIO_VH.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].RATIO_VH_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].RATIO_WHITE.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].RATIO_WHITE_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].RATIO_DARK.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].RATIO_DARK_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%d,"), vecitrStartVec->second[n].OVER_200_GV_PIXEL_COUNT.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%d,"), vecitrStartVec->second[n].OVER_200_GV_PIXEL_COUNT_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].DISTANCE_TO_EDGE_2_mm.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].DISTANCE_TO_EDGE_2_mm_2.GetValue());
                        strTotal += strVal;

                        if (vecitrStartVec->second[n].IS_NEAR_EDGE_COMPARE_OPERATOR == enCompareOperator::BIGGER_THAN_SPEC)
                        {
                            strVal.Format(_T("%d,"), vecitrStartVec->second[n].IS_NEAR_EDGE.GetValue());
                            strTotal += strVal;
                        }
                        else
                        {
                            strVal.Format(_T("-1,"));
                            strTotal += strVal;
                        }

                        if (vecitrStartVec->second[n].IS_NEAR_EDGE_COMPARE_OPERATOR_2 == enCompareOperator::SMALLER_THAN_SPEC)
                        {
                            strVal.Format(_T("%d,"), vecitrStartVec->second[n].IS_NEAR_EDGE_2.GetValue());
                            strTotal += strVal;
                        }
                        else
                        {
                            strVal.Format(_T("-1,"));
                            strTotal += strVal;
                        }

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].EDGE_ENERGY_SHARPNESS.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].EDGE_ENERGY_SHARPNESS_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].EDGE_ENERGY_SHARPNESS_WHITE.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].EDGE_ENERGY_SHARPNESS_WHITE_2.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].EDGE_ENERGY_SHARPNESS_BLACK.GetValue());
                        strTotal += strVal;

                        strVal.Format(_T("%f,"), vecitrStartVec->second[n].EDGE_ENERGY_SHARPNESS_BLACK_2.GetValue());
                        strTotal += strVal;

                        strTotal += _T("\r\n");

                        CStringA strKor(strTotal);
                        file.Write(strKor, strKor.GetLength() * sizeof(char));
                    }
                }
                vecitrStartVec++;
            }
            file.Close();
        }
    }
}

void CDlgClassificationST::OnBnClickedBtnLoadSpecCsv()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    const TCHAR szFilters[] = _T("CSV Files (*.csv)|*.csv||");

    CFileDialog dlg(TRUE, _T(""), _T(""), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT, szFilters);

    CString strFileList;
    const int nMaxFiles = 400;
    const int nBuffSize = (nMaxFiles * (MAX_PATH + 1)) + 1;
    dlg.GetOFN().lpstrFile = strFileList.GetBuffer(nBuffSize);
    dlg.GetOFN().nMaxFile = nBuffSize;

    std::vector<CString> vecCond;
    CString strPath;
    CStdioFile file;

    std::vector<CString> vecStrSpec;
    std::vector<CString> vecStrName;

    if (dlg.DoModal() == IDOK)
    {
        long nIndex = 0;
        POSITION pos;
        pos = dlg.GetStartPosition();
        CString strFullFilename;

        strPath = dlg.GetPathName();

        CString m_strPath, str;
        FILE* m_pFile;
        errno_t err = _tfopen_s(&m_pFile, strPath, _T("rt,ccs=UNICODE"));

        if (err != 0)
        {
            return;
        }

        CStdioFile m_File(m_pFile);

        while (m_File.ReadString(str))
        {
            vecStrSpec.push_back(str);
        }
        
        std::vector<std::vector<CString>> vecDiv;
        vecDiv.resize(vecStrSpec.size());

        for (long n = 0; n < vecStrSpec.size(); n++)
        {
            StringTokenDivideToCString(vecStrSpec[n], ',', vecDiv[n]);
        }

        for (long n = 0; n < vecDiv.size(); n++)
        {
            CString str;
            // 7
            for (long nIndex = 0; nIndex < 9; nIndex++)
            {
                str += vecDiv[n][nIndex];
                str += _T(",");
            }
            vecStrName.push_back(str);
        }

        std::vector<long> vecDefectCount;
        vecDefectCount.resize(vecStrName.size());

        CString strTemp = vecStrName[0];
        long nCnt = 0;
        for (long n = 0; n < vecStrName.size(); n++)
        {    
            if (vecStrName[n].Left(1) == _T(","))
            {
                nCnt++;
                vecDefectCount[n] = nCnt;
                vecStrName[n].Format(strTemp);
            }
            else
            {
                nCnt = 0;
                vecDefectCount[n] = nCnt;
                strTemp = vecStrName[n];
            }
        }

        auto& isi_spec = Inspection::Accessor::Get().m_isi_spec;
        // 기존 스펙 임시저장
        auto tebleTemp = isi_spec;

        m_ISI_Spec_table.initSpec();
        LinkSpecAndTable();
        m_bCheckSpecError = false;

        for (long n = 0; n < vecStrSpec.size(); n++)
        {
            SpecTableLoad(vecStrSpec[n], vecStrSpec[0], vecStrName[n], vecDefectCount[n]);

            if (m_bCheckSpecError)
            {
                m_ISI_Spec_table = tebleTemp;
                LinkSpecAndTable();
                break;
            }
        }   
        m_File.Close();
    }
    else
        return;

    //Inspection::Accessor::Get().m_isi_spec = m_ISI_Spec_table;
    m_ctrGrid.Invalidate();

}
