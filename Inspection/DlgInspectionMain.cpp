// DlgInspectionMain.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "InspectionRoot.h"
#include "DlgInspectionMain.h"
#include "afxdialogex.h"
#include "ImageViewEx.h"

#include "../Module/CustomControl/GridCtrl.h"

#include "Widget/ProfileView.h"
#include "Types/Rect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CDlgInspectionMain 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgInspectionMain, CDialog)

CDlgInspectionMain::CDlgInspectionMain(Inspection::Root* inspection, int nLanguage, Inspection::SetupImage& setupImage, long nType, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInspectionMain::IDD, pParent)
	, m_inspectionItemGrid(new CXTPPropertyGrid)
	, m_inspectionParaGrid(new CXTPPropertyGrid)
	, m_setupImage(setupImage)
	, m_debugInfoGrid(new CGridCtrl)
	, m_profileView(nullptr)
	, m_imageView(nullptr)
	, m_inspectionRoot(inspection)
	, m_nLanguage(nLanguage)
	, m_isFirstRun(true)
	, m_ExcelOpen(false)
	, m_curItemCategory(ProcItemCategory::END)
	, m_oldItemCategory(ProcItemCategory::END)
	, m_needToRecipeSave(false)
	//, m_DlgSurfaceDisplay(nullptr)
	, m_bParamForceSave(false)
	, m_nMultiImageIndex(0)
	, bcheckbox(false)
	, bisBrightImage(true)
	, m_bShow_OK(true)
	, m_bShow_NG(true)
	, m_bShow_SKIP(true)
    , m_nMachineType(nType)
    , m_ResultABSImage(new IPVM::Image_8u_C1)
    , m_ResultGammaImage(new IPVM::Image_8u_C1)
{
#ifdef _DRAW_DEFECT_
	m_isUseDrawDefect = false;
	m_BrightColor = RGB(255,255,255);
	m_DarkColor = RGB(100, 100, 100);
	m_Size = 1;
	m_Type = (int)DefectType::END;
#endif
}

CDlgInspectionMain::~CDlgInspectionMain()
{
	delete m_inspectionItemGrid;
	//delete m_DlgSurfaceDisplay;
	delete m_inspectionParaGrid;
	delete m_imageView;
	delete m_profileView;
	delete m_debugInfoGrid;
    delete m_ResultABSImage;
    delete m_ResultGammaImage;


	m_mapExplainParam.clear();
}

void CDlgInspectionMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAPTION_INSPECTION_ITEM, m_captionInspectionItem);
	DDX_Control(pDX, IDC_CAPTION_INSPECTION_PARA, m_captionInspectionPara);
	DDX_Control(pDX, IDC_CAPTION_DEBUG_INFO, m_captionDebugInfo);
	DDX_Control(pDX, IDC_CAPTION_IMAGE, m_captionImage);
	DDX_Control(pDX, IDC_BOX_IMAGE, m_boxImage);
	DDX_Control(pDX, IDC_CAPTION_SETTING, m_captionSetting);
	DDX_Control(pDX, IDC_BUTTON_SETTING_UNDO, m_buttonSettingUndo);
	DDX_Control(pDX, IDC_BOX_SETTING, m_boxSetting);
	DDX_Control(pDX, IDC_CAPTION_IMAGE, m_captionImage);
	DDX_Control(pDX, IDC_BUTTON_IMAGE_LOAD, m_buttonImageLoad);
	DDX_Control(pDX, IDC_BOX_IMAGE, m_boxImage);
	DDX_Control(pDX, IDC_CAPTION_TEST, m_captionTest);
	DDX_Control(pDX, IDC_BOX_TEST, m_boxTest);
	DDX_Control(pDX, IDC_BUTTON_TEST, m_buttonTest);
	DDX_Control(pDX, IDC_CAPTION_TEXT_INFO, m_captionTextInfo);
	DDX_Control(pDX, IDC_BUTTON_TEST_ALL, m_buttonTestAll);
	DDX_Control(pDX, IDC_BUTTON_SHOW_BRIGHT_IMAGE,m_buttonShowBrightImage);
	DDX_Control(pDX, IDC_BUTTON_SHOW_DARK_IMAGE, m_buttonShowDarkImage);
	
	DDX_Control(pDX, IDC_BUTTON_CELL_IMAGE_MULTI_SELECT, m_buttonMultiCellImage);
}


BEGIN_MESSAGE_MAP(CDlgInspectionMain, CDialog)
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_LOAD, &CDlgInspectionMain::OnBnClickedButtonImageLoad)
	ON_BN_CLICKED(IDC_BUTTON_SETTING_UNDO, &CDlgInspectionMain::OnBnClickedButtonSettingUndo)
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CDlgInspectionMain::OnBnClickedButtonTest)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_BRIGHT_IMAGE, &CDlgInspectionMain::OnBnClickedButtonShowBrightImage)
	ON_BN_CLICKED(IDC_BUTTON_SHOW_DARK_IMAGE, &CDlgInspectionMain::OnBnClickedButtonShowDarkImage)
	ON_NOTIFY(GVN_SELCHANGED, IDC_GRID_DEBUG_INFO, OnSelChangedGridDebugInfo)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_COPY_PARAMS_SAME_ITEM, &CDlgInspectionMain::OnBnClickedButtonCopyParamsSameItem)
	ON_BN_CLICKED(IDC_BUTTON_TEST_ALL, &CDlgInspectionMain::OnBnClickedButtonTestAll)
	ON_BN_CLICKED(IDC_CHECK_COPY_SAME_UPDOWN, &CDlgInspectionMain::OnBnClickedCheckCopySameUpdown)
	ON_BN_CLICKED(IDC_CHECK_COPY_SAME_TAB, &CDlgInspectionMain::OnBnClickedCheckCopySameTab)
	ON_BN_CLICKED(IDC_CHECK_COPY_SAME_POLAR, &CDlgInspectionMain::OnBnClickedCheckCopySamePolar)
	ON_BN_CLICKED(IDC_BUTTON_CELL_IMAGE_MULTI_SELECT, &CDlgInspectionMain::OnBnClickedButtonCellImageMultiSelect)
	ON_MESSAGE(WM_USER_MESSAGE_BOX_YES, &CDlgInspectionMain::OnUserMessageBoxYes)
	ON_WM_TIMER()
	ON_MESSAGE(WM_UPDATE_PROFILEVIEW, &CDlgInspectionMain::ShowProfileView)
	ON_BN_CLICKED(IDC_BUTTON_SHOWOK, &CDlgInspectionMain::OnBnClickedButtonShowok)
	ON_BN_CLICKED(IDC_BUTTON_SHOWNG, &CDlgInspectionMain::OnBnClickedButtonShowng)
	ON_BN_CLICKED(IDC_BUTTON_SHOWSKIP, &CDlgInspectionMain::OnBnClickedButtonShowskip)
    ON_MESSAGE(UWM_UPDATE_EDIT_INSPECTIONITEM, &CDlgInspectionMain::OnUwmUpdateEditInspectionItem)
    END_MESSAGE_MAP()


// CDlgInspectionMain 메시지 처리기입니다.


BOOL CDlgInspectionMain::OnInitDialog()
{
	CDialog::OnInitDialog();

	//m_DlgSurfaceDisplay = new CSurface_Display(this);
	//m_DlgSurfaceDisplay->Create(IDD_DIALOG_SURFACE_DISPLAY, this);

	//m_nLanguage 언어 변경
	strOption = _T("Option");

	if (m_nLanguage == 3)
	{
		SetWindowText(_T("Cell检查"));
		((CStatic*)(GetDlgItem(IDC_CAPTION_INSPECTION_ITEM)))->SetWindowText(_T("检查项目"));
		((CStatic*)(GetDlgItem(IDC_CAPTION_INSPECTION_PARA)))->SetWindowText(_T("检查结果价"));
		((CStatic*)(GetDlgItem(IDC_CAPTION_DEBUG_INFO)))->SetWindowText(_T("调试信息"));
		((CStatic*)(GetDlgItem(IDC_CAPTION_TEXT_INFO)))->SetWindowText(_T("文本信息"));
		((CStatic*)(GetDlgItem(IDC_CAPTION_SETTING)))->SetWindowText(_T("设定"));
		((CStatic*)(GetDlgItem(IDC_CAPTION_IMAGE)))->SetWindowText(_T("形象"));
		((CStatic*)(GetDlgItem(IDC_CAPTION_TEST)))->SetWindowText(_T("检察"));
		((CButton*)(GetDlgItem(IDC_BUTTON_SETTING_UNDO)))->SetWindowText(_T("取消"));
		((CButton*)(GetDlgItem(IDC_BUTTON_IMAGE_LOAD)))->SetWindowText(_T("叫来 "));
		((CButton*)(GetDlgItem(IDC_BUTTON_TEST)))->SetWindowText(_T("检察"));
		((CButton*)(GetDlgItem(IDC_BUTTON_TEST_ALL)))->SetWindowText(_T("全体检查"));
		((CButton*)(GetDlgItem(IDC_CHECK_COPY_SAME_UPDOWN)))->SetWindowText(_T("把设定值上面/下面的项目相同复制"));
		((CButton*)(GetDlgItem(IDC_CHECK_COPY_SAME_TAB)))->SetWindowText(_T("把设定值Tab/NoTab的项目相同复制"));
		((CButton*)(GetDlgItem(IDC_CHECK_COPY_SAME_POLAR)))->SetWindowText(_T("把设定值电极项目相同复制"));
		((CButton*)(GetDlgItem(IDC_BUTTON_COPY_PARAMS_SAME_ITEM)))->SetWindowText(_T("设定值复制"));
		
		////////////////////////////////////////////////////////////////////////////////
		//Common
		strLeft = _T("左边");
		strRight = _T("右边");
		strUse = _T("使用");
		strNotUse = _T("不使用");
		strUseInspection = _T("使用检验");
		strImageBrightBackImage = _T("图像A");
		strImageDarkBackImage = _T("图像B");
		strImageUseIndex = _T("图像使用索引");
		strOffset = _T("关于不检测区域");
		strThreshold = _T("阈值");
		strSpec = _T("关于检测规格");
		strPreprocess = _T("Preprocessing");
		strLengthSpec = _T("长度规格 [um]");
		strLengthSpecMM = _T("长度规格 [mm]");
		strMeasureOffset = _T("Offset");
		strAnd = _T("And");
		strOr = _T("Or");
		////////////////////////////////////////////////////////////////////////////////
		// roller mark
		strMarkThresholdValue = _T("Threshold Value");
		strMarkThickness = _T("Thickess");
		strMarkScanLeft = _T("Scan Left Position");
		strMarkScanRight = _T("Scan Right Position");
		strMarkEdgeDirection = _T("Edge Direction");
		strAutoManual = _T("Auto/Manual");
		////////////////////////////////////////////////////////////////////////////////
		//EdgeDetectFilter
		strHalfFilterLength = _T("[Filter]]HalfFilter Length[px]");
		strGaussianDeviation = _T("[Filter]Gaussian Deviation");
		////////////////////////////////////////////////////////////////////////////////
		//EdgeDetectPara
		strUseFirstEdge = _T("[Para]Use The First Edge");
		strEdgeTheshold = _T("[Para]Edge Threshold[Edge]");
		strDirection = _T("[Para]Search Drection");
		strDetectType = _T("[Para]Edge Detect Method");
		////////////////////////////////////////////////////////////////////////////////
		//Inline
		strInlineLeft = _T("Inline左边");
		strInlineRight = _T("Inline右边");
		strInlineTabThreshold = _T("Inline Tab阈值");
		strBackgroundLeft = _T("Background Left");
		strBackgroundRight = _T("Background Right");
		strVhomeExistance = _T("V Home Existance");

		strVhomeMakeUpper_USING_IMAGE = _T("Using Bright Image");
		strVhomeMakeUpper_LEFT = _T("[LEFT] V Home ROI GV MakeUpper");
		strVhomeMakeUpper_RIGHT = _T("[RIGHT] V Home ROI GV MakeUpper");
		
		strVhomeMakeUpper_LEFT_VALUE = _T("       └ MakeUpper GV Value");
		strVhomeMakeUpper_RIGHT_VALUE = _T("       └ MakeUpper GV Value");

		strVhomeMakeUpper_MoveROI = _T("(option)Use vHome ROI Move");
		strVhomeMakeUpper_CompareXPos = _T("3Edge X Point Compare(pixel) ");
		
		strUseVhomeSpecDistance = _T("Use V Home Spec Distance");
		strVhomeMoveTogether = _T("V Home Move Together");
		strVhomeBlobMerge = _T("V Home Blob Merge");
		strVhomeBlobMergeDistance = _T("V Home Blob Merge Distance [px]");
		strWhite = _T("白色");
		strBlack = _T("黑色");

		strWhiteAuto = _T("White Auto Threshold");
		strBlackAuto = _T("Black Auto Threshold");
		strWhiteManualSimple = _T("White Simple Manual Threshold");
		strBlackManualSimple = _T("Black Simple Manual Threshold");
		strWhiteEdgeDetect = _T("White Edge Detect");
		strBlackEdgeDetect = _T("Black Edge Detect");
		////////////////////////////////////////////////////////////////////////////////
		//Cell
		strTabDirection = _T("Tab方向");
		strBoundarySearchSizeUPDOWN = _T("Boundary Search Y Size (OnlyTilt) [px]");
		strBoundarySearchSize = _T("Boundary Search X Size [px]");
		strShowVHomeROI = _T("Adjust Detect Line ROI");
        strSplitCenterROI = _T("Add Center ROI Split");
		strTabExistance = _T("Tab存在");
		strTabThreshold = _T("Tab阈值");
		strTabMerge = _T("Tab合并");
		strSearchTabLeftFromVHome = _T("Use Search Tab Left From V Home");
		strTabBoundaryIgnore = _T("Tab Boundary Ignore Size [um]");
		strCellBoundaryPosition = _T("Cell Boundary Position [um]");
		strGage = _T("Gage Mode");
		strManualTabROI = _T("Manual Tab ROI");
		strRollerROI = _T("Roller ROI");
		strTabInnerDetailSearchOffset = _T("Inner Detail Search Offset");
		strTabOuterDetailSearchOffset = _T("Outer Detail Search Offset");
		strTabBoundaryFirstEdge = _T("Tab Boundary First Edge");
		strTabBoundaryEdgeThreshold = _T("Tab Boundary Edge Threshold");
		strRollerBoundaryLineForTab = _T("Roller Boundary Line For Tab");
		strVHomeLeftTop = _T("V-沟西上");
		strVHomeLeftBot = _T("V-沟西下");
		strVHomeRightTop = _T("V-沟东上");
		strVHomeRightBot = _T("V-沟东下");

		strUseIntekTabAlign = _T("Use Intek Tab Align");
		strDetectTabBothSide = _T("Detect Tab Both Side");
		strTabThresholdTopSide = _T("Tab Threshold Top Side");
		strTabThresholdBottomSide = _T("Tab Threshold Bottom Side");
		strTabTapeThresholdTopSide = _T("Tab Tape Threshold Top Side");
		strTabTapeThresholdBottomSide = _T("Tab Tape Threshold Bottom Side");
		strShadowThreshold = _T("Shadow Threshold");
		strRollerEndThreshold = _T("Roller End Threshold");
		strTabTopBottomForceCorrection = _T("Tab Top Bottom Force Correction");
		strTabRightPosCorrection = _T("Tab Right Pos Correction");
		strTabEdgeSearchHeightPixel = _T("Tab Edge Search Height Pixel");
		strEdgePointFilterValue = _T("Edge Point Filter Value");
		strectTabROI = _T("rect Tab ROI");
		strForceCorrectionActivated = _T("Force Correction Activated");
		strTabEdgeSelectImage = _T("Tab Edge Select Image");
		strTabEdgeRightSkipPixel = _T("Tab Edge Right Skip Pixel");
		strTabEdgeCriteria = _T("Tab Edge Criteria");
		strTabWidth = _T("Tab Width");
		strTabHeight = _T("Tab Height");
		strTabEdgeInspRange = _T("Tab Edge Insp Range");
		strTabRightOffset = _T("Tab Right Offset");
		strTabWhiteCount = _T("Tab White Count");
		strTabSearchPixel = _T("Tab Search Pixel");
		////////////////////////////////////////////////////////////////////////////////
		//Uncoating
		strUncoating = _T("Uncoating");
		strBoundaryOffset = _T("边界偏移");
		strLeftBoundaryOuterOffset = _T("Left 边界外部偏移");
		strLeftBoundaryInnerOffset = _T("Left 边界内部偏移");
		strRightBoundaryOuterOffset = _T("Right 边界外部偏移");
		strRightBoundaryInnerOffset = _T("Right 边界内部偏移");
		strBoundaryThreshold = _T("边界阈值");
		strElectrodeThreshold = _T("电极阈值");
		strVHomeIgnore = _T("V-沟无视");
		strVHomeWidth = _T("V-沟宽度");
		strVHomeHeight = _T("V-沟高度");
		strGeneral = _T("总括号");
		strScrab = _T("异物");
		strBoundarySizeSpecX = _T("Uncoating 边界长度规格 X[um]");
		strElectrodeSizeSpecX = _T("Uncoating 电极长度规格 X[um]");
		strScrabSizeSpecX = _T("异物长度规格 X[um]");
		strBoundarySizeSpecY = _T("Uncoating边界长度规格 Y[um]");
		strElectrodeSizeSpecY = _T("Uncoating 电极长度规格 Y[um]");
		strScrabSizeSpecY = _T("异物长度规格 Y[um]");
		strOverlaySizeSpecMin = _T("异物长度规格 [um]");
		strOverlaySizeSpecMax = _T("异物长度规格 [um]");
		strInsulationSizeSpecMin = _T("异物长度规格 [um]");
		strInsulationSizeSpecMax = _T("异物长度规格 [um]");

		strUseBoundaryInspection = _T("边境检查");
		strUseElectrodeInspection = _T("电极检查");
		strUseScrabInspection = _T("异物检查");
		strUseSkipScrabInspAfterTape = _T("连接胶带后跳过刮痕检查");
		strUseOverlayInspection = _T("异物检查");
		strUseInsulationInspection = _T("异物检查");
		strUseSurfaceInspection = _T("异物检查");
		strUseTabInspection = _T("TabSurface检查");
		strUseSpurInspection = _T(" ┗ TabSpur检查");

		strBoundaryInspOption = _T("Uncoaint Boundary Inspection Option");
		strElectrodeInspOption = _T("Uncoaint Electrode Inspection Option");
		strScrabInspOption = _T("Scrab Inspection Option");

		strImageUseBoundaryIndex = _T("Image Use Boundary Index");
		strImageUseElectrodeIndex = _T("Image Use Electrode Index");
		strImageUseScrabIndex = _T("Image Use Scrab Index");


		strImageBoundaryBrightBackImage = _T("Bright Background Image");
		strImageBoundaryDarkBackImage = _T("Dark Background Image");


		strImageElectrodeBrightBackImage = _T("Bright Background Image");
		strImageElectrodeDarkBackImage = _T("Dark Background Image");


		strImageScrabBrightBackImage = _T("Bright Background Image");
		strImageScrabDarkBackImage = _T("Dark Background Image");
		////////////////////////////////////////////////////////////////////////////////
		//Tape
		strThresholdPlusOffset = _T("参考资料1 + 抵消");
		strThresholdMinusOffset = _T("参考资料2 - 抵消");
		strTapeReference1 = _T("参考资料1 : 直方图百分比 %");
		strTapeReference2 = _T("参考资料2 : 直方图百分比 %");
		strWhiteSpec = _T("白色规格 [um]");
		strBlackSpec = _T("黑色规格 [um]");
		strReductionRate = _T("Reduction Rate");
		////////////////////////////////////////////////////////////////////////////////
		//EletrodeTorn
		strElectrodeTorn = _T("电极裂痕");
		strVHomeWidthSize = _T("V-沟宽度尺寸");
		strVHomeHeightSize = _T("V-沟高度尺寸");
		strNearOffset = _T("近偏移距");
		strFarOffset = _T("远偏移距");
		strDarkThresholdRatio = _T("暗阈值比");
		strBrightThresholdOffset = _T("明亮的阈值抵消");
		////////////////////////////////////////////////////////////////////////////////
		//Tab
		strOffsetTop = _T("偏移顶部 [px]");
		strOffsetBottom = _T("偏移底部 [px]");
		strOffsetRight = _T("偏移右侧 [px]");
		strTopThreshold = _T("阈值最高");
		strBottomThreshold = _T("阈值底部");
		strInspectionIgnoreInnerOffset = _T("偏移左侧 [px]");
		strCapIgnoreOffset = _T("Cap无视抵消 [px]");
		strThresholdType = _T("阈值类型");
		strUseSpecHeightForInspection = _T("Use Spec Height For Inspection");
		strUseRightInspection = _T("Use Tab Head Inspection");
		strTabHeadInspectionMethod = _T("Tab Head Inspection Method");
		strTabHeadInspectionEdge = _T("Edge");
		strTabHeadInspectionBlob = _T("Blob");
		strNoTabHeightError = _T("No Tab Height Offest Spec [%:1~100]");
		strNoTabWidthError = _T("No Tab Width Offest Spec [%:1~100]");
		strTabWidthMinSpec = _T("Tab Width Min Offset Spec [mm]");
		strTabWidthMaxSpec = _T("Tab Width Max Offset Spec [mm]");
		strTabHeightMinSpec = _T("Tab Height Min Offset Spec [mm]");
		strTabHeightMaxSpec = _T("Tab Height Max Offset Spec [mm]");
		strHeightError = _T("高度错误 [mm]");
		strWidthError = _T("宽度错误 [mm]");
		strUseAndOr = _T("Use And Or");
		strAreaMin = _T("区最小 [um^2]");
		strHeightOffset = _T("高度抵消(Gain)[um]");
		strThresholdOffset = _T("阈值抵消");
		strEdgeThreshold = _T("阈值边缘");
		////////////////////////////////////////////////////////////////////////////////
		//burr
		strBurrThreshold_TopOuter = _T("Top Line Outer (High)");
		strBurrThreshold_TopOuter_Electrode = _T("Top Line Outer (Low)");
		strBurrThreshold_TopInner = _T("Top Line Inner");
		strBurrThreshold_BottomInner = _T("Bottom Line Inner");
		strBurrThreshold_BottomOuter = _T("Bottom Line Outer (High)");
		strBurrThreshold_BottomOuter_Electrode = _T("Bottom Line Outer (Low)");
		strBurrThreshold_RoiRight = _T("Above Tab ROI (High)");
		strBurrThreshold_RoiRight_Lower = _T("Above Tab ROI (Low)");
		strBurrThreshold_RoiLeft = _T("Below Cell Bottom Edge ROI (High)");
		strBurrThreshold_RoiLeft_Electrode = _T("Below Cell Bottom Edge ROI (Low)");
		strBurrLeft = _T("Left Rect");
		strBurrTopOuter = _T("Top Outer Rect");
		strBurrBottomOuter = _T("Bottom Outer Rect");
		strBurrRight = _T("Right Rect");
		strOffsetX 	  =_T("Offset_X(px)");
		strOffsetY 	  =_T("Offset_Y(px)");
		strRoiSizeX	  =_T("RoiSIze_X(px)");
		strRoiSizeY	  =_T("RoiSIze_Y(px)");
		strBurrROIPosX = _T("ROI X Position (mm)");
		strBurrROIPosY = _T("ROI Y Position (mm)");
		strBurrSpecSizeX = _T("Burr Left Spec Size X");
		strBurrSpecSizeY = _T("Burr Left Spec Size Y");
		////////////////////////////////////////////////////////////////////////////////
		//NGMark
		strFind = _T("找到");
		strNGMarkFindOffsetLeft = _T("找到抵消左 [px]");
		strNGMarkFindOffsetRight = _T("找到抵消右 [px]");
		strNGMarkFindOffsetY = _T("找到抵消Y [px]");
		strNGMark1 = _T("NG标志1");
		strNGMark2 = _T("NG标志2");
		strNGMark3 = _T("NG标志3");
		strEnable = _T("有效");
		strDisable = _T("丧失能力");
		strNGMarkSizeX = _T("NG标志尺寸X [mm]");
		strNGMarkSizeY = _T("NG标志尺寸Y [mm]");
		strNGMarkPitch = _T("NG标志螺距 [mm]");
		strNGMarkCount = _T("NG标志计数");
		strNGMarkMaxAverageIntensity = _T("NG标志最大平均Intensity");
		strNGMarkYaxisMergeCount = _T("Y Sum Merge Pixel");
		strNGMarkFrequencyFilter = _T("NG Mark Frequency Filter");
		strNGMarkMinBlackPercent = _T("NG标志最小黑色百分号");
		NGMarkFindCount = _T("NG标志找到计数");
		NGMarkOption = _T("NG标志功能");
		SubMark = _T("候选标志计数");
	}
	else
	{
		////////////////////////////////////////////////////////////////////////////////
		//Common
		strLeft = _T("Left");
		strRight = _T("Right");
		strUse = _T("Use");
		strNotUse = _T("Not Use");
		strUseInspection = _T("Use Inspection");
		strImageBrightBackImage = _T("Bright Background Image");
		strImageDarkBackImage = _T("Dark Background Image");
		strImageUseIndex = _T("Image Use Index");
		strOffset = _T("Offset");
		strThreshold = _T("Threshold");
		strSpec = _T("Spec");
		strPreprocess = _T("Preprocessing");
		strLengthSpec = _T("Length Spec [um]");
		strLengthSpecMM = _T("Length Spec [mm]");
		strMeasureOffset = _T("Offset");
		strAnd = _T("And");
		strOr = _T("Or");
		////////////////////////////////////////////////////////////////////////////////
		// roller mark
		strMarkThresholdValue = _T("Threshold Value");
		strMarkThickness = _T("Thickess");
		strMarkScanLeft = _T("Scan Left Position");
		strMarkScanRight = _T("Scan Right Position");
		strMarkEdgeDirection = _T("Edge Direction");
		strAutoManual = _T("Auto/Manual");
		////////////////////////////////////////////////////////////////////////////////
		//EdgeDetectFilter
		strHalfFilterLength = _T("[Filter]HalfFilter Length[px]");
		strGaussianDeviation = _T("[Filter]Gaussian Deviation");
		////////////////////////////////////////////////////////////////////////////////
		//EdgeDetectPara
		strUseFirstEdge = _T("[Para]Use The First Edge");
		strEdgeTheshold = _T("[Para]Edge Threshold[Edge]");
		strDirection = _T("[Para]Search Drection");
		strDetectType = _T("[Para]Edge Detect Method");
		////////////////////////////////////////////////////////////////////////////////
		//Inline
		strInlineLeft = _T("Inline Left");
		strInlineRight = _T("Inline Right");
		strInlineTabThreshold = _T("Inline Tab Threshold");
		strBackgroundLeft = _T("Background Left");
		strBackgroundRight = _T("Background Right");
		strVhomeExistance = _T("V Home Existance");
		
		strVhomeMakeUpper_USING_IMAGE = _T("Using Bright Image");
		strVhomeMakeUpper_LEFT = _T("[LEFT] V Home ROI GV MakeUpper");
		strVhomeMakeUpper_RIGHT = _T("[RIGHT] V Home ROI GV MakeUpper");
		
		strVhomeMakeUpper_LEFT_VALUE = _T("       └ MakeUpper GV Value");
		strVhomeMakeUpper_RIGHT_VALUE = _T("       └ MakeUpper GV Value");

		strVhomeMakeUpper_MoveROI = _T("(Option)Use vHome ROI Move");
		strVhomeMakeUpper_CompareXPos = _T("3Edge X Point Compare(pixel)");

		strUseVhomeSpecDistance = _T("Use V Home Spec Distance");
		strVhomeMoveTogether = _T("V Home Move Together");
		strVhomeBlobMerge = _T("V Home Blob Merge");
		strVhomeBlobMergeDistance = _T("V Home Blob Merge Distance [px]");

		strWhiteAuto = _T("White Auto Threshold");
		strBlackAuto = _T("Black Auto Threshold");
		strWhiteManualSimple = _T("White Simple Manual Threshold");
		strBlackManualSimple = _T("Black Simple Manual Threshold");
		strWhiteEdgeDetect = _T("White Edge Detect");
		strBlackEdgeDetect = _T("Black Edge Detect");
		strTest = _T("Test");

		strWhite = _T("White");
		strBlack = _T("Black");
		////////////////////////////////////////////////////////////////////////////////
		//Cell
		strTabDirection = _T("Tab Direction");
		strBoundarySearchSizeUPDOWN = _T("Boundary Search Y Size (OnlyTilt) [px]");
		strBoundarySearchSize = _T("Boundary Search X Size [px]");
		strShowVHomeROI = _T("Adjust Detect Line ROI");
        strSplitCenterROI = _T("Add Center ROI Split");
        strTabExistance = _T("Tab Existance");
		strTabThreshold = _T("Tab Threshold[px]");
		strTabMerge = _T("Tab Merge");
		strSearchTabLeftFromVHome = _T("Use Search Tab Left From V Home");
		strTabBoundaryIgnore = _T("Tab Boundary Ignore Size [um]");
		strCellBoundaryPosition = _T("Cell Boundary Position [um]");
		strGage = _T("Gage Mode");
		strManualTabROI = _T("Manual Tab ROI");
		strRollerROI = _T("Roller ROI");
		strTabInnerDetailSearchOffset = _T("Inner Detail Search Offset[px]");
		strTabOuterDetailSearchOffset = _T("Outer Detail Search Offset[px]");
		strTabBoundaryFirstEdge = _T("Tab Boundary First Edge");
		strTabBoundaryEdgeThreshold = _T("Tab Boundary Edge Threshold");
		strRollerBoundaryLineForTab = _T("Roller Boundary Line For Tab");
		strVHomeLeftTop = _T("V Home Left Top");
		strVHomeLeftBot = _T("V Home Left Bottom");
		strVHomeRightTop = _T("V Home Right Top");
		strVHomeRightBot = _T("V Home Right Bottom");
		
		strUseIntekTabAlign = _T("Use Intek Tab Align");
		strDetectTabBothSide = _T("Detect Tab Both Side");
		strTabThresholdTopSide = _T("Tab Threshold Top Side");
		strTabThresholdBottomSide = _T("Tab Threshold Bottom Side");
		strTabTapeThresholdTopSide = _T("Tab Tape Threshold Top Side");
		strTabTapeThresholdBottomSide = _T("Tab Tape Threshold Bottom Side");
		strShadowThreshold = _T("Shadow Threshold");
		strRollerEndThreshold = _T("Roller End Threshold");
		strTabTopBottomForceCorrection = _T("Tab Top Bottom Force Correction");
		strTabRightPosCorrection = _T("Tab Right Pos Correction");
		strTabEdgeSearchHeightPixel = _T("Tab Edge Search Height Pixel");
		strEdgePointFilterValue = _T("Edge Point Filter Value");
		strectTabROI = _T("rect Tab ROI");
		strForceCorrectionActivated = _T("Force Correction Activated");
		strTabEdgeSelectImage = _T("Tab Edge Select Image");
		strTabEdgeRightSkipPixel = _T("Tab Edge Right Skip Pixel");
		strTabEdgeCriteria = _T("Tab Edge Criteria");
		strTabWidth = _T("Tab Width");
		strTabHeight = _T("Tab Height");
		strTabEdgeInspRange = _T("Tab Edge Insp Range");
		strTabRightOffset = _T("Tab Right Offset");
		strTabWhiteCount = _T("Tab White Count");
		strTabSearchPixel = _T("Tab Search Pixel");
		////////////////////////////////////////////////////////////////////////////////
		//Uncoating
		strUncoating = _T("Uncoating");
		strBoundaryOffset = _T("Boundary Offset");
		strLeftBoundaryOuterOffset = _T("Left Outer Offset");
		strLeftBoundaryInnerOffset = _T("Left Inner Offset");
		strRightBoundaryOuterOffset = _T("Right Outer Offset");
		strRightBoundaryInnerOffset = _T("Right Inner Offset");
		strBoundaryThreshold = _T("Boundary Threshold");
		strElectrodeThreshold = _T("Electrode Threshold");
		strVHomeIgnore = _T("V Home Ignore");
		strVHomeWidth = _T("V Home Width");
		strVHomeHeight = _T("V Home Height");
		strGeneral = _T("General");
		strScrab = _T("Scrab");
		strBoundarySizeSpecX = _T("Uncoaint Boundary Size X Spec [um]");
		strElectrodeSizeSpecX = _T("Uncoaint Electrode Size X Spec [um]");
		strScrabSizeSpecX = _T("Scrab Size X Spec [um]");

		strBoundarySizeSpecY = _T("Uncoaint Boundary Size Y Spec [um]");
		strElectrodeSizeSpecY = _T("Uncoaint Electrode Size Y Spec [um]");
		strScrabSizeSpecY = _T("Scrab Size Y Spec [um]");

		strOverlaySizeSpecMin = _T("Overlay Size Spec Min [um]");
		strOverlaySizeSpecMax = _T("Overlay Size Spec Max [um]");
		strInsulationSizeSpecMin = _T("Insulation Size Spec Min [um]");
		strInsulationSizeSpecMax = _T("Insulation Size Spec Max  [um]");

		strUseBoundaryInspection = _T("Use Boundary Inspection");
		strUseElectrodeInspection = _T("Use Electrode Inspection");
		strUseScrabInspection = _T("Use Scrab Inspection");
		strUseSkipScrabInspAfterTape = _T("Skip Scrab Insp After Tape");
		strUseOverlayInspection = _T("Use Overlay Inspection");
		strUseInsulationInspection = _T("Use Insulation Inspection");
		strUseSurfaceInspection = _T("Use Surface Inspection");
		strUseTabInspection = _T("Use Tab Surface Inspection");
		strUseSpurInspection = _T(" ┗ Use Spur Inspection");
		
		strBoundaryInspOption = _T("Uncoaint Boundary Inspection Option");
		strElectrodeInspOption = _T("Uncoaint Electrode Inspection Option");
		strScrabInspOption = _T("Scrab Inspection Option");
		
		strImageUseBoundaryIndex = _T("Image Use Boundary Index");
		strImageUseElectrodeIndex = _T("Image Use Electrode Index");
		strImageUseScrabIndex = _T("Image Use Scrab Index");
		
		strImageBoundaryBrightBackImage = _T("Bright Background Image");
		strImageBoundaryDarkBackImage = _T("Dark Background Image");

		strImageElectrodeBrightBackImage = _T("Bright Background Image");
		strImageElectrodeDarkBackImage = _T("Dark Background Image");

		strImageScrabBrightBackImage = _T("Bright Background Image");
		strImageScrabDarkBackImage = _T("Dark Background Image");
		////////////////////////////////////////////////////////////////////////////////
		//Tape
		strThresholdPlusOffset = _T("Reference1 + Offset");
		strThresholdMinusOffset = _T("Reference2 - Offset");
		strTapeReference1 = _T("Reference1 : Histogram Percentage %");
		strTapeReference2 = _T("Reference2 : Histogram Percentage %");
		strWhiteSpec = _T("White Spec [um]");
		strBlackSpec = _T("Black Spec [um]");
		strReductionRate = _T("Reduction Rate");
		////////////////////////////////////////////////////////////////////////////////
		//EletrodeTorn
		strElectrodeTorn = _T("Electrode Torn");
		strVHomeWidthSize = _T("V Home Width Size");
		strVHomeHeightSize = _T("V Home Height Size");
		strNearOffset = _T("Near Offset");
		strFarOffset = _T("Far Offset");
		strDarkThresholdRatio = _T("Dark Threshold Ratio");
		strBrightThresholdOffset = _T("Bright Threshold Offset");
		////////////////////////////////////////////////////////////////////////////////
		//Tab
		strOffsetTop = _T("Offset Top [px]");
		strOffsetBottom = _T("Offset Bottom [px]");
		strOffsetRight = _T("Offset Right [px]");
		strTopThreshold = _T("Top Threshold [px]");
		strBottomThreshold = _T("Bottom Threshold");
		strInspectionIgnoreInnerOffset = _T("Offset Left [px]");
		strCapIgnoreOffset = _T("Cap Ignore Offset [px]");
		strThresholdType = _T("Threshold Type");
		strUseSpecHeightForInspection = _T("Use Spec Height For Inspection");
		strUseRightInspection = _T("Use Tab Head Inspection");
		strTabHeadInspectionMethod = _T("Tab Head Inspection Method");
		strTabHeadInspectionEdge = _T("Edge");
		strTabHeadInspectionBlob = _T("Blob");
		strHeightError = _T("Height Error [mm]");
		strWidthError = _T("Width Error [mm]");
		strNoTabHeightError = _T("No Tab Height Offest Spec [%:1~100]");
		strNoTabWidthError = _T("No Tab Width Offest Spec [%:1~100]");
		strTabWidthMinSpec = _T("Tab Width Min Offset Spec [mm]");
		strTabWidthMaxSpec = _T("Tab Width Max Offset Spec [mm]");
		strTabHeightMinSpec = _T("Tab Height Min Offset Spec [mm]");
		strTabHeightMaxSpec = _T("Tab Height Max Offset Spec [mm]");
		strUseAndOr = _T("Use And Or");
		strAreaMin = _T("Area Min [um^2]");
		strHeightOffset = _T("Height Offset(Gain)[um]");
		strThresholdOffset = _T("Threshold Offset");
		strEdgeThreshold = _T("Edge Threshold");
		////////////////////////////////////////////////////////////////////////////////
		//burr
		strBurrThreshold_TopOuter = _T("Top Line Outer (High)");
		strBurrThreshold_TopOuter_Electrode = _T("Top Line Outer (Low)");
		strBurrThreshold_TopInner= _T("Top Line Inner");
		strBurrThreshold_BottomInner= _T("Bottom Line Inner");
		strBurrThreshold_BottomOuter = _T("Bottom Line Outer (High)");
		strBurrThreshold_BottomOuter_Electrode = _T("Bottom Line Outer (Low)");
		strBurrThreshold_RoiRight = _T("Above Tab ROI (High)");
		strBurrThreshold_RoiRight_Lower = _T("Above Tab ROI (Low)");
		strBurrThreshold_RoiLeft = _T("Below Cell Bottom Edge ROI (High)");
		strBurrThreshold_RoiLeft_Electrode = _T("Below Cell Bottom Edge ROI (Low)");
		strBurrLeft = _T("Left Rect");
		strBurrTopOuter = _T("Top Outer Rect");
		strBurrBottomOuter = _T("Bottom Outer Rect");
		strBurrRight = _T("Right Rect");
		//strBurrBottomInner = _T("Bottom Inner");
		strOffsetX = _T("Offset_X(px)");
		strOffsetY = _T("Offset_Y(px)");
		strRoiSizeX = _T("RoiSIze_X(px)");
		strRoiSizeY = _T("RoiSIze_Y(px)");
		strBurrROIPosX = _T("ROI X Position (mm)");
		strBurrROIPosY = _T("ROI Y Position (mm)");
		strBurrSpecSizeX = _T("Burr Left Spec Size X");
		strBurrSpecSizeY = _T("Burr Left Spec Size Y");
		////////////////////////////////////////////////////////////////////////////////
		//NGMark
		strFind = _T("Find");
		strNGMarkFindOffsetLeft = _T("NG Mark Find Offset L [px]");
		strNGMarkFindOffsetRight = _T("NG Mark Find Offset R [px]");
		strNGMarkFindOffsetY = _T("NG Mark Find Offset Y [px]");
		strNGMark1 = _T("NG Mark 1");
		strNGMark2 = _T("NG Mark 2");
		strNGMark3 = _T("NG Mark 3");
		strEnable = _T("Enable");
		strDisable = _T("Disable");
		strNGMarkSizeX = _T("NG Mark Size X [mm]");
		strNGMarkSizeY = _T("NG Mark Size Y [mm]");
		strNGMarkPitch = _T("NG Mark Pitch [mm]");
		strNGMarkCount = _T("NG Mark Count");
		strNGMarkMaxAverageIntensity = _T("NG Mark Max Average Intensity");
		strNGMarkYaxisMergeCount = _T("Y Sum Merge Pixel");
		strNGMarkFrequencyFilter = _T("NG Mark Frequency Filter");
		strNGMarkMinBlackPercent = _T("NG Mark Min BlackPercent");
		NGMarkFindCount = _T("NG Mark Find Count");
		NGMarkOption = _T("NG Mark Option(Sub Mark)");
		SubMark = _T("Sub Mark Hertz/Hz(Default:5)");
	}

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	long screenSizeX = GetSystemMetrics(SM_CXSCREEN);
	long screenSizeY = GetSystemMetrics(SM_CYSCREEN);
	MoveWindow(0, 0, screenSizeX, screenSizeY);

	CRect clientRect;
	GetClientRect(clientRect);

	long cx = clientRect.Width();
	long cy = clientRect.Height();

	m_inspectionGridSizeX = (cx / 2 - ITEM_BOUNDARY_OFFSET1 - ITEM_GAP * 2) / 2;
//	long imageViewSizeX = cx / 2 - ITEM_BOUNDARY_OFFSET1;
	long imageViewSizeY = cy * 2 / 3 - ITEM_BOUNDARY_OFFSET1;

//	CRect dlgRect;
//	GetWindowRect(dlgRect);

	CRect captionItemRect;
	m_captionInspectionItem.MoveWindow(ITEM_BOUNDARY_OFFSET1, ITEM_BOUNDARY_OFFSET1, m_inspectionGridSizeX, CAPTION_SIZE_Y);
	m_captionInspectionItem.GetWindowRect(captionItemRect);

	CRect captionParaRect;
	m_captionInspectionPara.MoveWindow(ITEM_BOUNDARY_OFFSET1 + m_inspectionGridSizeX + ITEM_GAP, ITEM_BOUNDARY_OFFSET1, m_inspectionGridSizeX, CAPTION_SIZE_Y);
	m_captionInspectionPara.GetWindowRect(captionParaRect);

	CRect captionDebugInfoRect;
	m_captionDebugInfo.MoveWindow(ITEM_BOUNDARY_OFFSET1 + m_inspectionGridSizeX + ITEM_GAP, cy * 2 / 3 + ITEM_GAP, m_inspectionGridSizeX, CAPTION_SIZE_Y);
	m_captionDebugInfo.GetWindowRect(captionDebugInfoRect);

	m_captionTextInfo.MoveWindow(5 + captionDebugInfoRect.right, cy * 2 / 3 + ITEM_GAP, 450, CAPTION_SIZE_Y);

//	RECT gridItemRect = { ITEM_BOUNDARY_OFFSET1, captionItemRect.Height() + ITEM_BOUNDARY_OFFSET1 + ITEM_GAP, ITEM_BOUNDARY_OFFSET1 + m_inspectionGridSizeX, dlgRect.Height() - ITEM_BOUNDARY_OFFSET1 - 60 };
	RECT gridItemRect = { ITEM_BOUNDARY_OFFSET1, captionItemRect.Height() + ITEM_BOUNDARY_OFFSET1 + ITEM_GAP, ITEM_BOUNDARY_OFFSET1 + m_inspectionGridSizeX, cy - ITEM_BOUNDARY_OFFSET1 - 150 };
	m_inspectionItemGrid->Create(gridItemRect, this, 0);
	m_inspectionItemGrid->SetViewDivider(0.5);
	m_inspectionItemGrid->HighlightChangedItems(TRUE);
	m_inspectionItemGrid->SetBorderStyle(XTPPropertyGridBorderStyle::xtpGridBorderFlat);
	m_inspectionItemGrid->SetShowInplaceButtonsAlways(TRUE);

	RECT gridParaRect = { gridItemRect.right + ITEM_GAP, gridItemRect.top, gridItemRect.right + ITEM_GAP + m_inspectionGridSizeX, ITEM_BOUNDARY_OFFSET1 + imageViewSizeY };
	m_inspectionParaGrid->Create(gridParaRect, this, 0);
	m_inspectionParaGrid->SetViewDivider(0.5);
	m_inspectionParaGrid->HighlightChangedItems(TRUE);
	m_inspectionParaGrid->SetBorderStyle(XTPPropertyGridBorderStyle::xtpGridBorderFlat);
	m_inspectionParaGrid->SetShowInplaceButtonsAlways(TRUE);

	//CRect copyRect, polarRect, tabRect, updownRect;
	//GetDlgItem(IDC_BUTTON_COPY_PARAMS_SAME_ITEM)->GetWindowRect(copyRect);
	//GetDlgItem(IDC_CHECK_COPY_SAME_POLAR)->GetWindowRect(polarRect);
	//GetDlgItem(IDC_CHECK_COPY_SAME_TAB)->GetWindowRect(tabRect);
	//GetDlgItem(IDC_CHECK_COPY_SAME_UPDOWN)->GetWindowRect(updownRect);
	//((CButton*)GetDlgItem(IDC_BUTTON_COPY_PARAMS_SAME_ITEM))->MoveWindow(gridParaRect.left + 10, gridParaRect.bottom - 100, copyRect.Width(), copyRect.Height());
	//((CButton*)GetDlgItem(IDC_CHECK_COPY_SAME_POLAR))->MoveWindow(gridParaRect.left + 10, gridParaRect.bottom - 105 - copyRect.Height(), polarRect.Width(), copyRect.Height());
	//((CButton*)GetDlgItem(IDC_CHECK_COPY_SAME_TAB))->MoveWindow(gridParaRect.left + 10, gridParaRect.bottom - 110 - copyRect.Height() * 2, tabRect.Width(), copyRect.Height());
	//((CButton*)GetDlgItem(IDC_CHECK_COPY_SAME_UPDOWN))->MoveWindow(gridParaRect.left + 10, gridParaRect.bottom - 115 - copyRect.Height() * 3, updownRect.Width(), copyRect.Height());

	CRect captionSettingRect;
	m_captionSetting.GetWindowRect(captionSettingRect);

	CRect viewRect = { gridParaRect.right + captionSettingRect.Width() + ITEM_GAP * 2, ITEM_BOUNDARY_OFFSET1, cx - ITEM_BOUNDARY_OFFSET1, ITEM_BOUNDARY_OFFSET1 + imageViewSizeY - ITEM_IMAGENAMESIZE_OFFSET1 };
	m_imageView = new Inspection::ImageViewEx(GetSafeHwnd(), IPVM::FromMFC(viewRect));

	GetDlgItem(IDC_BUTTON_IMAGESAVE)->MoveWindow(gridParaRect.right + captionSettingRect.Width() + ITEM_GAP * 2, imageViewSizeY - ITEM_BOUNDARY_OFFSET1 * 3, cx / 3 + ITEM_IMAGENAMESIZE_OFFSET1 * 3.4, ITEM_IMAGENAMESIZE_OFFSET1 - ITEM_BOUNDARY_OFFSET1);  //다시보고 roi 맞추기

	RECT gridDebugInfoRect = { gridParaRect.left, gridParaRect.bottom + ITEM_GAP * 2 + CAPTION_SIZE_Y, gridParaRect.right, cy - ITEM_BOUNDARY_OFFSET1 };
	m_debugInfoGrid->Create(gridDebugInfoRect, this, IDC_GRID_DEBUG_INFO);
	rtDisplay = gridDebugInfoRect;
	m_debugInfoGrid->EnableDragAndDrop(FALSE);
	m_debugInfoGrid->SetEditable(FALSE);

	//m_DlgSurfaceDisplay->MoveWindow(gridDebugInfoRect.left, gridDebugInfoRect.top - ITEM_GAP * 2, 1450, 350);

	RECT textBoxRect = { gridDebugInfoRect.right + ITEM_GAP, gridDebugInfoRect.top, gridDebugInfoRect.right + 450 + ITEM_GAP, cy - ITEM_BOUNDARY_OFFSET1 };

	m_editTextInformation.Create(WS_CHILD | WS_VISIBLE | SS_NOTIFY | WS_TABSTOP | ES_MULTILINE | WS_BORDER | ES_AUTOVSCROLL, textBoxRect, this, IDC_EDIT_BOX);
	m_editTextInformation.ShowWindow(SW_SHOW);

	CRect profileViewRect(textBoxRect.right + ITEM_GAP, gridParaRect.bottom + ITEM_GAP, cx - ITEM_GAP, textBoxRect.bottom);

	m_profileView = new IPVM::ProfileView(GetSafeHwnd(), IPVM::FromMFC(profileViewRect));
	m_profileView->ShowRangeControlPanel();

	//m_DlgSurfaceDisplay->InitGrid();
	InitInspecitonItemGrid();
	InitInspecitonDebugInfo();
	InitInspecitonButton(viewRect);
	
	CRect copyRect, polarRect, tabRect, updownRect, AreaEditRect, AreaStaticRect;
	GetDlgItem(IDC_BUTTON_COPY_PARAMS_SAME_ITEM)->GetWindowRect(copyRect);
	GetDlgItem(IDC_CHECK_COPY_SAME_POLAR)->GetWindowRect(polarRect);
	GetDlgItem(IDC_CHECK_COPY_SAME_TAB)->GetWindowRect(tabRect);
	GetDlgItem(IDC_CHECK_COPY_SAME_UPDOWN)->GetWindowRect(updownRect);
	long dx = ITEM_BOUNDARY_OFFSET1;
//	long screenSizeY = GetSystemMetrics(SM_CYSCREEN);
	long dy = screenSizeY - copyRect.Height() - 50;
	GetDlgItem(IDC_BUTTON_COPY_PARAMS_SAME_ITEM)->MoveWindow(dx, dy, copyRect.Width(), copyRect.Height());
	((CButton*)GetDlgItem(IDC_CHECK_COPY_SAME_POLAR))->MoveWindow(dx, dy - 5 - copyRect.Height(), polarRect.Width(), copyRect.Height());
	((CButton*)GetDlgItem(IDC_CHECK_COPY_SAME_TAB))->MoveWindow(dx, dy - 10 - copyRect.Height() - polarRect.Height(), tabRect.Width(), copyRect.Height());
	((CButton*)GetDlgItem(IDC_CHECK_COPY_SAME_UPDOWN))->MoveWindow(dx, dy - 15 - copyRect.Height() - polarRect.Height() * 2, updownRect.Width(), copyRect.Height());

	GetDlgItem(IDC_EDIT_AREAINDEX)->GetWindowRect(AreaEditRect);
	SetDlgItemInt(IDC_EDIT_AREAINDEX, 1);
    GetDlgItem(IDC_EDIT_AREAINDEX)->MoveWindow(dx, dy - 40 - copyRect.Height() - polarRect.Height() * 2, AreaEditRect.Width(), AreaEditRect.Height());
    GetDlgItem(IDC_STATIC_AREAINDEX)->GetWindowRect(AreaStaticRect);
    GetDlgItem(IDC_STATIC_AREAINDEX)->MoveWindow(dx + 5 + AreaEditRect.Width(), dy - 40 - copyRect.Height() - polarRect.Height() * 2 + (AreaEditRect.Height()/4),AreaStaticRect.Width(), AreaStaticRect.Height());

	m_imageView->RegisterCallback_ROIChangeEnd(GetSafeHwnd(), this, CallbackOnROIChanged);
	m_imageView->RegisterCallback_MeasurementBegin(GetSafeHwnd(), this, CallbackOnMeasurementChanged);
	m_imageView->RegisterCallback_MeasurementChangeEnd(GetSafeHwnd(), this, CallbackOnMeasurementChanged);
	m_imageView->RegisterCallback_MeasurementEnd(GetSafeHwnd(), this, CallbackOnMeasurementEnd);
#ifdef _DRAW_DEFECT_
	m_imageView->RegisterCallback_MouseLButtonDown(GetSafeHwnd(), this, callbackMouseClickedDown);
	m_imageView->RegisterCallback_MouseMove(GetSafeHwnd(), this, callbackMouseMoved);
	m_imageView->RegisterCallback_MouseLButtonUp(GetSafeHwnd(), this, callbackMouseClickedUp);
#endif
	// 첫시작은 Up / Cell Align에서 시작한다
	ChangeEditInspectionItem(true, ProcItemCategory::cellAlign);

	//AllLamiObjectNotUse();
	GetDlgItem(IDC_BUTTON_SHOW_BRIGHT_IMAGE)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_SHOW_DARK_IMAGE)->EnableWindow(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgInspectionMain::OnBnClickedButtonShowok()
{
	m_bShow_OK = !m_bShow_OK;
	if (true == m_bShow_OK)
		GetDlgItem(IDC_BUTTON_SHOWOK)->SetWindowTextW(_T("PASS(ON)"));
	else
		GetDlgItem(IDC_BUTTON_SHOWOK)->SetWindowTextW(_T("PASS(OFF)"));
	ShowRejectROI_Event();
}

void CDlgInspectionMain::OnBnClickedButtonShowng()
{
	m_bShow_NG = !m_bShow_NG;
	if (true == m_bShow_NG)
		GetDlgItem(IDC_BUTTON_SHOWNG)->SetWindowTextW(_T("REJECT(ON)"));
	else
		GetDlgItem(IDC_BUTTON_SHOWNG)->SetWindowTextW(_T("REJECT(OFF)"));
	ShowRejectROI_Event();
}

void CDlgInspectionMain::OnBnClickedButtonShowskip()
{
	m_bShow_SKIP = !m_bShow_SKIP;
	if (true == m_bShow_SKIP)
		GetDlgItem(IDC_BUTTON_SHOWSKIP)->SetWindowTextW(_T("SKIP(ON)"));
	else
		GetDlgItem(IDC_BUTTON_SHOWSKIP)->SetWindowTextW(_T("SKIP(OFF)"));
	ShowRejectROI_Event();
}

void CDlgInspectionMain::ShowISIOption(bool bShow)
{
	GetDlgItem(IDC_CAPTION_SHOWOPTION)->ShowWindow(bShow);
	GetDlgItem(IDC_BOX_SHOWOPTION)->ShowWindow(bShow);
	GetDlgItem(IDC_BUTTON_SHOWSKIP)->ShowWindow(bShow);
	GetDlgItem(IDC_BUTTON_SHOWNG)->ShowWindow(bShow);
	GetDlgItem(IDC_BUTTON_SHOWOK)->ShowWindow(bShow);
}
void CDlgInspectionMain::ExcelRejectInput()
{
    long nImageSizeX = 100;
    long nImageSizeY = 100;
    m_ExcelSave.Create(CRect(0, 0, nImageSizeX, nImageSizeY), 8); // zoomsize 설정.. 32 - 16 - 8

    CString strExcelSaveFilePath;

    strExcelSaveFilePath.Format(_T("D:\\iBCI\\Result"));

    // Excel File 저장할곳 디렉토리 생성
    CreateDir(strExcelSaveFilePath);
	
    vector<CString> vecData;

	if (m_dlgBatch->m_chk_DefectPosAdd.GetCheck() == TRUE)
	{
        // 검사 결과 항목 Data 삽입

        // 추후 for 문을 통해 값들을 집어넣게 구현해야함.
        vecData.push_back(_T("NAME"));
        vecData.push_back(_T("OK/NG"));
        vecData.push_back(_T("Bright"));
        vecData.push_back(_T("Dark"));
        vecData.push_back(_T("Bin"));
        //24.10.10
        vecData.push_back(_T(""));
        vecData.push_back(_T("ImagePosition"));
        vecData.push_back(_T(""));
        //
        vecData.push_back(_T("         DEFECT         "));
        vecData.push_back(_T("Result        "));
        vecData.push_back(_T("SIZE X(mm)"));
        vecData.push_back(_T("SIZE Y(mm)"));
        vecData.push_back(_T("Area(㎟)"));
        vecData.push_back(_T("Area(%)"));
        vecData.push_back(_T("Inspection Area"));
        vecData.push_back(_T("InspectionImageType"));
        vecData.push_back(_T("RATIO_YX"));
        vecData.push_back(_T("RATIO_XY"));
        vecData.push_back(_T("Width(mm)"));
        vecData.push_back(_T("Length(mm)"));
        vecData.push_back(_T("RATIO_T"));
        vecData.push_back(_T("RATIO_L"));
        vecData.push_back(_T("MIN(GV)"));
        vecData.push_back(_T("AVG(GV)"));
        vecData.push_back(_T("MAX(GV)"));
        vecData.push_back(_T("MASS_CENTER_GV_GV"));
        vecData.push_back(_T("EDGE_ENERGY"));
        vecData.push_back(_T("EDGE_ENERGY_WHITE"));
        vecData.push_back(_T("EDGE_ENERGY_BLACK"));
        vecData.push_back(_T("Section Th"));
        vecData.push_back(_T("HOR"));
        vecData.push_back(_T("VER"));
        vecData.push_back(_T("RATIO_HV"));
        vecData.push_back(_T("RATIO_VH"));
        vecData.push_back(_T("RATIO_WHITE")); // [sm]
        vecData.push_back(_T("RATIO_DARK"));
        vecData.push_back(_T("200 > CNT"));
        vecData.push_back(_T("DIS_2_EDGE"));
        vecData.push_back(_T("IS_NEAR_EDGE"));
        vecData.push_back(_T("EDGE_ENERGY_SHARPNESS"));
        vecData.push_back(_T("EDGE_ENERGY_SHARPNESS_WHITE"));
        vecData.push_back(_T("EDGE_ENERGY_SHARPNESS_BLACK"));
	}
	else
	{
        vecData.push_back(_T("NAME"));
        vecData.push_back(_T("OK/NG"));
        vecData.push_back(_T("Bright"));
        vecData.push_back(_T("Dark"));
        vecData.push_back(_T("Bin"));
        vecData.push_back(_T("         DEFECT         "));
        vecData.push_back(_T("Result        "));
        vecData.push_back(_T("SIZE X(mm)"));
        vecData.push_back(_T("SIZE Y(mm)"));
        vecData.push_back(_T("Area(㎟)"));
        vecData.push_back(_T("Area(%)"));
        vecData.push_back(_T("Inspection Area"));
        vecData.push_back(_T("InspectionImageType"));
        vecData.push_back(_T("RATIO_YX"));
        vecData.push_back(_T("RATIO_XY"));
        vecData.push_back(_T("Width(mm)"));
        vecData.push_back(_T("Length(mm)"));
        vecData.push_back(_T("RATIO_T"));
        vecData.push_back(_T("RATIO_L"));
        vecData.push_back(_T("MIN(GV)"));
        vecData.push_back(_T("AVG(GV)"));
        vecData.push_back(_T("MAX(GV)"));
        vecData.push_back(_T("BRIGHT AVG(GV)"));
        vecData.push_back(_T("DARK AVG(GV)"));
        vecData.push_back(_T("MASS_CENTER_GV_GV"));
        vecData.push_back(_T("EDGE_ENERGY"));
        vecData.push_back(_T("EDGE_ENERGY_WHITE"));
        vecData.push_back(_T("EDGE_ENERGY_BLACK"));
        vecData.push_back(_T("Section Th"));
        vecData.push_back(_T("HOR"));
        vecData.push_back(_T("VER"));
        vecData.push_back(_T("RATIO_HV"));
        vecData.push_back(_T("RATIO_VH"));
        vecData.push_back(_T("RATIO_WHITE")); // [sm]
        vecData.push_back(_T("RATIO_DARK"));
        vecData.push_back(_T("200 > CNT"));
        vecData.push_back(_T("DIS_2_EDGE"));
        vecData.push_back(_T("IS_NEAR_EDGE"));
        vecData.push_back(_T("EDGE_ENERGY_SHARPNESS"));
        vecData.push_back(_T("EDGE_ENERGY_SHARPNESS_WHITE"));
        vecData.push_back(_T("EDGE_ENERGY_SHARPNESS_BLACK"));
	}
    

    m_ExcelSave.RejectListName(vecData);
}

void CDlgInspectionMain::ExcelRejectDataInput()
{
    CInspectionCell* parent = m_inspectionRoot->GetInspectionCell(m_curIsUp);
    long nDefectSize = parent->m_ISICellAllDefect.m_vecCandidate.size();
    if (m_dlgBatch==nullptr)
    {
        long nImageSizeX = 100;
        long nImageSizeY = 100;
        m_ExcelSave.Create(CRect(0, 0, nImageSizeX, nImageSizeY), 8);

        vector<CString> vecData;

        vecData.push_back(_T("NAME"));
        vecData.push_back(_T("OK/NG"));
        vecData.push_back(_T("Bright"));
        vecData.push_back(_T("Dark"));
        vecData.push_back(_T("Bin"));
        vecData.push_back(_T("         DEFECT         "));
        vecData.push_back(_T("Result        "));
        vecData.push_back(_T("SIZE X(mm)"));
        vecData.push_back(_T("SIZE Y(mm)"));
        vecData.push_back(_T("Area(㎟)"));
        vecData.push_back(_T("Area(%)"));
        vecData.push_back(_T("Inspection Area"));
        vecData.push_back(_T("InspectionImageType"));
        vecData.push_back(_T("RATIO_YX"));
        vecData.push_back(_T("RATIO_XY"));
        vecData.push_back(_T("Width(mm)"));
        vecData.push_back(_T("Length(mm)"));
        vecData.push_back(_T("RATIO_T"));
        vecData.push_back(_T("RATIO_L"));
        vecData.push_back(_T("MIN(GV)"));
        vecData.push_back(_T("AVG(GV)"));
        vecData.push_back(_T("MAX(GV)"));
        vecData.push_back(_T("BRIGHT AVG(GV)"));
        vecData.push_back(_T("DARK AVG(GV)"));
        vecData.push_back(_T("MASS_CENTER_GV_GV"));
        vecData.push_back(_T("EDGE_ENERGY"));
        vecData.push_back(_T("EDGE_ENERGY_WHITE"));
        vecData.push_back(_T("EDGE_ENERGY_BLACK"));
        vecData.push_back(_T("Section Th"));
        vecData.push_back(_T("HOR"));
        vecData.push_back(_T("VER"));
        vecData.push_back(_T("RATIO_HV"));
        vecData.push_back(_T("RATIO_VH"));
        vecData.push_back(_T("RATIO_WHITE")); // [sm]
        vecData.push_back(_T("RATIO_DARK"));
        vecData.push_back(_T("200 > CNT"));
        vecData.push_back(_T("DIS_2_EDGE"));
        vecData.push_back(_T("IS_NEAR_EDGE"));
        vecData.push_back(_T("EDGE_ENERGY_SHARPNESS"));
        vecData.push_back(_T("EDGE_ENERGY_SHARPNESS_WHITE"));
        vecData.push_back(_T("EDGE_ENERGY_SHARPNESS_BLACK"));

        m_ExcelSave.RejectListName(vecData);

        CFileDialog SaveDlg(FALSE, _T("xlsx"), _T(""), OFN_HIDEREADONLY, _T("xlsx Files (*..xlsx)|*..xlsx||"));

        if (SaveDlg.DoModal() != IDOK)
        {
            return;
        }
        CString strPath;
        strPath = SaveDlg.GetPathName();

        for (long i = 0; i < nDefectSize; i++)
        {
            auto& Defect = parent->m_ISICellAllDefect.m_vecCandidate[i];
            CISIDefectProperty Result;

            IPVM::Rect BrightROI;
            IPVM::Rect DarkROI;

            IPVM::Rect rect = Defect.rtRect_pixel;
            IPVM::Rect DefectROI = rect;

            rect &= IPVM::Rect(*parent->m_ISICellAllDefect.m_pSrcBright);

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
            rect &= IPVM::Rect(*parent->m_ISICellAllDefect.m_pSrcBright);

            IPVM::Image_8u_C1 CropImage_Bright(rect.Width(), rect.Height());
            IPVM::Image_8u_C1 CropImage_Dark(rect.Width(), rect.Height());
            IPVM::Rect OriginRect(CropImage_Bright);

            IPVM::Image_8u_C1* OriginImage_Bright;
            OriginImage_Bright = parent->m_ISICellAllDefect.m_pSrcBright;
            IPVM::ImageProcessing::Copy(*OriginImage_Bright, rect, IPVM::Rect(CropImage_Bright), CropImage_Bright);

            IPVM::Image_8u_C1* OriginImage_Dark;
            OriginImage_Dark = parent->m_ISICellAllDefect.m_pSrcDark;
            IPVM::ImageProcessing::Copy(*OriginImage_Dark, rect, IPVM::Rect(CropImage_Dark), CropImage_Dark);

            long nX = rect.Width();
            long nY = rect.Height();
            long nOriX = rect.Width();
            long nOriY = rect.Height();

            long nOffX = CropImage_Bright.GetSizeX();
            long nOffY = CropImage_Bright.GetSizeY();
            IPVM::Rect rt;

            long MoveX = (nX - nOffX) / 2;
            long MoveY = (nY - nOffY) / 2;
            rt = IPVM::Rect(CropImage_Bright);
            rt.MoveToX(MoveX);
            rt.MoveToY(MoveY);

            nX = rect.Width();
            nY = rect.Height();

            vector<CString> vecDefectName;
            for (int j = 0; j < parent->m_ISICellAllDefect.m_vecCandidate[i].GetLogicStepNum(); j++)
            {
                long nKey = 0;
                classification::enLogicResult nNO0_YES1 = classification::enLogicResult::NONE;
                CString strNote;
                CString strDefectName;
                parent->m_ISICellAllDefect.m_vecCandidate[i].GetLogicStep(j, strDefectName, nKey, nNO0_YES1, strNote);
                II_RESULT_VALUE PASS = parent->m_ISICellAllDefect.m_vecCandidate[i].GetJudge();

                CString strText;
                switch (nNO0_YES1)
                {
                    case classification::NO:
                        strText = _T("NO");
                        //vecDefectName.push_back(strDefectName);
                        break;
                    case classification::YES:
                        strText = _T("YES");
                        vecDefectName.push_back(strDefectName);
                        break;
                    case classification::NONE:
                        strText = _T("");
                        //vecDefectName.push_back(strDefectName);
                        break;
                    default:
                        break;
                }
            }

            if (vecDefectName.size() == 0)
            {
                vecDefectName.push_back(_T("OK"));
            }
            //vecResult.push_back(vecDefectName.size());
            II_RESULT_VALUE Ret = parent->m_ISICellAllDefect.m_vecCandidate[i].GetJudge();
            if (Ret == PASS)
            {
                continue;
            }

            if (m_nMachineType != 0)
            {
                m_nExcelRow += 1;
            }
            //II_RESULT_VALUE Ret = parent->m_ISICellAllDefect.m_vecCandidate[i].GetJudge();

            CString ResultName = _T("[ none ] ");
            switch (Ret)
            {
                case PASS:
                    ResultName = _T("OK");
                    break;
                case REWORK:
                    break;
                case REJECT:
                    ResultName = _T("NG");
                    break;
                case INVALID:
                    break;
                case NOT_MEASURED:
                    break;
                default:
                    break;
            }

			if (Ret == REJECT && m_nMachineType == 0)
            {
                m_nExcelRow += 1;
                CString strBrigtImage;
                strBrigtImage.Format(_T("D:\\iBCI\\Result\\BrightOri_%d.jpg"), i);
                CString strDarkImage;
                strDarkImage.Format(_T("D:\\iBCI\\Result\\DarkOri_%d.jpg"), i);
                CString strBinImage;
                strBinImage.Format(_T("D:\\iBCI\\Result\\Bin_%d.jpg"), i);

                IPVM::Rect Binrect(Defect.srcImage);
                nX = Binrect.Width();
                nY = Binrect.Height();

                IPVM::Image_8u_C1 CropOffsetImg_BIN;
                CropOffsetImg_BIN.Create(nOriX, nOriY);

                IPVM::ImageProcessing::Fill((IPVM::Rect)CropOffsetImg_BIN, 0, CropOffsetImg_BIN);

                long nXOffset = (abs(nOriX - nX)) / 2;
                long nYOffset = (abs(nOriY - nY)) / 2;

                Binrect.MoveToX(nXOffset);
                Binrect.MoveToY(nYOffset);

                IPVM::ImageProcessing::Copy(Defect.srcImage, IPVM::Rect(Defect.srcImage), Binrect, CropOffsetImg_BIN);

                Defect.srcImage.SaveJpeg(_T("D:\\BinOri.jpg"));
                CropOffsetImg_BIN.SaveJpeg(_T("D:\\BinOffset.jpg"));

                m_ExcelSave.SetBrightImage(CropImage_Bright);
                m_ExcelSave.SetDarkImage(CropImage_Dark);
                m_ExcelSave.SetBinImage(CropOffsetImg_BIN);

                vector<CString> vecResultData;
                CString strResult;

                strResult.Format(_T("%d"), i + 1);
                vecResultData.push_back(strResult);
                strResult.Format(_T("%s"), ResultName);
                vecResultData.push_back(strResult);
                DefectType dt = parent->m_ISICellAllDefect.m_vecCandidate[i].GetDefectResultCode();
                CString strdtName = GetDefectName(dt);
                strResult.Format(_T("Defect[%d]_%s"), i, strdtName);
                vecResultData.push_back(strResult);
                strResult.Format(_T("%s"), strdtName);
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.SIZE_X_mm.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.SIZE_Y_mm.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.AREA.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.AREA_Ratio.GetValue());
                vecResultData.push_back(strResult);

                strResult = InspectionArea2String((enInspectionArea)Defect.nInspectionArea.GetValue());
                vecResultData.push_back(strResult);

                strResult
                    = enumtoString().InspectionImageType2String((enInspectionImageType)Defect.nInspectImage.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.RATIO_YX.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.RATIO_XY.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.WIDTH_mm.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.LENGTH_mm.GetValue());
                vecResultData.push_back(strResult);
                //RATIO_T? RATIO_L?
                strResult.Format(_T("%.3f"), Defect.RATIO_LW.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.RATIO_WL.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%d"), Defect.MIN_GV.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%d"), Defect.AVG_GV.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%d"), Defect.MAX_GV.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%d"), Defect.BRIGHT_AVG_GV.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%d"), Defect.DARK_AVG_GV.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.MASS_CENTER_GV_GV.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_WHITE.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_BLACK.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.SECTION_THRESHOLD.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.HOR.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.VER.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.RATIO_HV.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.RATIO_VH.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.RATIO_WHITE.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.RATIO_DARK.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%d"), Defect.OVER_200_GV_PIXEL_COUNT.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.DISTANCE_TO_EDGE_mm.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%d"), Defect.IS_NEAR_EDGE.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_SHARPNESS.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_SHARPNESS_WHITE.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_SHARPNESS_BLACK.GetValue());
                vecResultData.push_back(strResult);
                m_ExcelSave.AddImageFileDatas(m_nExcelRow, vecDefectName[0], strDarkImage, strBrigtImage, strBinImage,
                    vecResultData, ResultName, OriginRect, DefectROI);
            }
            else if (m_nMachineType != 0)
            {
                CString strBrigtImage;
                strBrigtImage.Format(_T("D:\\iBCI\\Result\\BrightOri_%d.jpg"), i);
                CString strDarkImage;
                strDarkImage.Format(_T("D:\\iBCI\\Result\\DarkOri_%d.jpg"), i);
                CString strBinImage;
                strBinImage.Format(_T("D:\\iBCI\\Result\\Bin_%d.jpg"), i);

                IPVM::Rect Binrect(Defect.srcImage);
                nX = Binrect.Width();
                nY = Binrect.Height();

                IPVM::Image_8u_C1 CropOffsetImg_BIN;
                CropOffsetImg_BIN.Create(nOriX, nOriY);

                IPVM::ImageProcessing::Fill((IPVM::Rect)CropOffsetImg_BIN, 0, CropOffsetImg_BIN);

                long nXOffset = (abs(nOriX - nX)) / 2;
                long nYOffset = (abs(nOriY - nY)) / 2;

                Binrect.MoveToX(nXOffset);
                Binrect.MoveToY(nYOffset);

                IPVM::ImageProcessing::Copy(Defect.srcImage, IPVM::Rect(Defect.srcImage), Binrect, CropOffsetImg_BIN);

                Defect.srcImage.SaveJpeg(_T("D:\\BinOri.jpg"));
                CropOffsetImg_BIN.SaveJpeg(_T("D:\\BinOffset.jpg"));

                m_ExcelSave.SetBrightImage(CropImage_Bright);
                m_ExcelSave.SetDarkImage(CropImage_Dark);
                m_ExcelSave.SetBinImage(CropOffsetImg_BIN);

                vector<CString> vecResultData;
                CString strResult;

                strResult.Format(_T("%d"), i + 1);
                vecResultData.push_back(strResult);
                strResult.Format(_T("%s"), ResultName);
                vecResultData.push_back(strResult);
                DefectType dt = parent->m_ISICellAllDefect.m_vecCandidate[i].GetDefectResultCode();
                CString strdtName = GetDefectName(dt);
                strResult.Format(_T("Defect[%d]_%s"), i, strdtName);
                vecResultData.push_back(strResult);
                strResult.Format(_T("%s"), strdtName);
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.SIZE_X_mm.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.SIZE_Y_mm.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.AREA.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.AREA_Ratio.GetValue());
                vecResultData.push_back(strResult);

                strResult = InspectionArea2String((enInspectionArea)Defect.nInspectionArea.GetValue());
                vecResultData.push_back(strResult);

                strResult
                    = enumtoString().InspectionImageType2String((enInspectionImageType)Defect.nInspectImage.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.RATIO_YX.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.RATIO_XY.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.WIDTH_mm.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.LENGTH_mm.GetValue());
                vecResultData.push_back(strResult);
                //RATIO_T? RATIO_L?
                strResult.Format(_T("%.3f"), Defect.RATIO_LW.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.RATIO_WL.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%d"), Defect.MIN_GV.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%d"), Defect.AVG_GV.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%d"), Defect.MAX_GV.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%d"), Defect.BRIGHT_AVG_GV.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%d"), Defect.DARK_AVG_GV.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.MASS_CENTER_GV_GV.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_WHITE.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_BLACK.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.SECTION_THRESHOLD.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.HOR.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.VER.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.RATIO_HV.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.RATIO_VH.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.RATIO_WHITE.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.RATIO_DARK.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%d"), Defect.OVER_200_GV_PIXEL_COUNT.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.DISTANCE_TO_EDGE_mm.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%d"), Defect.IS_NEAR_EDGE.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_SHARPNESS.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_SHARPNESS_WHITE.GetValue());
                vecResultData.push_back(strResult);
                strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_SHARPNESS_BLACK.GetValue());
                vecResultData.push_back(strResult);

               m_ExcelSave.AddImageFileDatas(m_nExcelRow, vecDefectName[0], strDarkImage, strBrigtImage, strBinImage,
                    vecResultData, ResultName, OriginRect, DefectROI);
            }
            
        }
        m_ExcelSave.Savexlsfile(strPath, FALSE);
        return;

    }

    if (m_dlgBatch->m_chk_ExcelSave.GetCheck() == TRUE)
    {
        vector<long> vecResult;

        CString strExcelSaveFilePath;

        strExcelSaveFilePath.Format(_T("D:\\iBCI\\Result"));

        // Excel File 저장할곳 디렉토리 생성
        CreateDir(strExcelSaveFilePath);
         for (long nDefect = 0; nDefect < nDefectSize; nDefect++)
         {
             auto& Defect = parent->m_ISICellAllDefect.m_vecCandidate[nDefect];

             IPVM::Rect BrightROI;
             IPVM::Rect DarkROI;

             IPVM::Rect rect = Defect.rtRect_pixel;
             IPVM::Rect DefectROI = rect;
             rect &= IPVM::Rect(*parent->m_ISICellAllDefect.m_pSrcBright);

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
             rect &= IPVM::Rect(*parent->m_ISICellAllDefect.m_pSrcBright);

			 int offset_left = Defect.rtRect_pixel.m_left - rect.m_left;
             int offset_top = Defect.rtRect_pixel.m_top - rect.m_top;
             int offset_right = Defect.rtRect_pixel.m_right - rect.m_right;
             int offset_bottom = Defect.rtRect_pixel.m_bottom - rect.m_bottom;

             IPVM::Image_8u_C1 CropImage_Bright(rect.Width(), rect.Height());
             IPVM::Image_8u_C1 CropImage_Dark(rect.Width(), rect.Height());
             IPVM::Rect OriginRect(CropImage_Bright);
             OriginRect.DeflateRect(offset_left, offset_top, -offset_right, -offset_bottom);
             IPVM::Image_8u_C1* OriginImage_Bright;
             OriginImage_Bright = parent->m_ISICellAllDefect.m_pSrcBright;
             IPVM::ImageProcessing::Copy(*OriginImage_Bright, rect, IPVM::Rect(CropImage_Bright), CropImage_Bright);
             long nX = CropImage_Bright.GetSizeX();
             long nY = CropImage_Bright.GetSizeY();
             //CropImage_Bright.SaveJpeg(_T("D:\\Crop_Offset.jpg"));
             IPVM::Image_8u_C1* OriginImage_Dark;
             OriginImage_Dark = parent->m_ISICellAllDefect.m_pSrcDark;
             IPVM::ImageProcessing::Copy(*OriginImage_Dark, rect, IPVM::Rect(CropImage_Dark), CropImage_Dark);

             vector<CString> vecDefectName;

             CString strText;
             for (int i = 0; i < parent->m_ISICellAllDefect.m_vecCandidate[nDefect].GetLogicStepNum(); i++)
             {
                 long nKey = 0;
                 classification::enLogicResult nNO0_YES1 = classification::enLogicResult::NONE;
                 CString strNote;
                 CString strDefectName;
                 parent->m_ISICellAllDefect.m_vecCandidate[nDefect].GetLogicStep(
                     i, strDefectName, nKey, nNO0_YES1, strNote);

                 switch (nNO0_YES1)
                 {
                     case classification::NO:
                         strText = _T("NO");
                         //vecDefectName.push_back(strDefectName);
                         break;
                     case classification::YES:
                         strText = _T("YES");
                         vecDefectName.push_back(strDefectName);
                         break;
                     case classification::NONE:
                         strText = _T("");
                         //vecDefectName.push_back(strDefectName);
                         break;
                     default:
                         break;
                 }
             }

             II_RESULT_VALUE Ret = parent->m_ISICellAllDefect.m_vecCandidate[nDefect].GetJudge();
             if (Ret == PASS)
             {
                 continue;
             }

             if (vecDefectName.size() == 0)
             {
                 vecDefectName.push_back(_T("OK"));
             }
             vecResult.push_back(vecDefectName.size());
             if (m_nMachineType != 0)
			 {
                 m_nExcelRow += 1;
			 }
             
             //II_RESULT_VALUE Ret = parent->m_ISICellAllDefect.m_vecCandidate[nDefect].GetJudge();

             CString ResultName = _T("[ none ] ");
             switch (Ret)
             {
                 case PASS:
                     ResultName = _T("OK");
                     break;
                 case REWORK:
                     break;
                 case REJECT:
                     ResultName = _T("NG");
                     break;
                 case INVALID:
                     break;
                 case NOT_MEASURED:
                     break;
                 default:
                     break;
             }
             if (Ret == REJECT && m_nMachineType == 0)
			 {
                 m_nExcelRow += 1;
                 CString strBrigtImage;
                 strBrigtImage.Format(_T("D:\\iBCI\\Result\\BrightOri_%d.jpg"), nDefect);
                 //CropImage_Bright.SaveJpeg(strBrigtImage);
                 CString strDarkImage;
                 strDarkImage.Format(_T("D:\\iBCI\\Result\\DarkOri_%d.jpg"), nDefect);
                 //CropImage_Dark.SaveJpeg(strDarkImage);
                 CString strBinImage;
                 strBinImage.Format(_T("D:\\iBCI\\Result\\Bin_%d.jpg"), nDefect);
                 //Defect.srcImage.SaveJpeg(strBinImage);

				 IPVM::Image_8u_C1 OriginImage;
                 TCHAR programpath[_MAX_PATH];
                 GetCurrentDirectory(_MAX_PATH, programpath);
                 CString ImageFolderName = _T("\\DataSearchImage.jpg");
                 CString strImagePath = _T("D:\\UC1\\DataSearchImage.jpg");
                 //programpath + ImageFolderName;
                 
                 OriginImage.LoadFrom(strImagePath);

                 IPVM::Rect Binrect(CropImage_Bright);
                 nX = Binrect.Width();
                 nY = Binrect.Height();

                 IPVM::Image_8u_C1 CropOffsetImg_BIN;
                 CropOffsetImg_BIN.Create(nX, nY);

                 IPVM::ImageProcessing::Fill((IPVM::Rect)CropOffsetImg_BIN, 0, CropOffsetImg_BIN);

                 IPVM::Rect rt;

                 long MoveX = (nX - OriginRect_Width) / 2;
                 long MoveY = (nY - OriginRect_Height) / 2;
                 rt = IPVM::Rect(Defect.srcImage);
                 rt.MoveToX(MoveX);
                 rt.MoveToY(MoveY);

                 IPVM::ImageProcessing::Copy(Defect.srcImage, IPVM::Rect(Defect.srcImage),
                     /*IPVM::Rect(CropImage_Bright)*/ rt, CropOffsetImg_BIN);

                 //Defect.srcImage.SaveJpeg(_T("D:\\srcImage.jpg"));
                 //  CropOffsetImg_BIN.SaveJpeg(_T("D:\\CropOffsetImg_BIN.jpg"));
                 m_ExcelSave.SetBrightImage(CropImage_Bright);
                 m_ExcelSave.SetDarkImage(CropImage_Dark);
                 m_ExcelSave.SetBinImage(CropOffsetImg_BIN);

                 m_ExcelSave.SetSourceImage(OriginImage);

                 vector<CString> vecResultData;
                 // 여기서 각 Defect 결과 값들을 담는다.
                 CString strResult;
                 CString strTemp = m_dlgBatch->m_multiInlinePath[m_dlgBatch->m_index - 2].Mid(
                     m_dlgBatch->m_multiInlinePath[m_dlgBatch->m_index - 2].ReverseFind(_T('\\')) + 1,
                     m_dlgBatch->m_multiInlinePath[m_dlgBatch->m_index - 2].ReverseFind(_T('.')) + 1);
                 strResult = strTemp;
                 strResult.Format(_T("%s"), strResult);
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%s"), ResultName);
                 vecResultData.push_back(strResult);
                 DefectType dt = parent->m_ISICellAllDefect.m_vecCandidate[nDefect].GetDefectResultCode();
                 CString strdtName = GetDefectName(dt);
                 strResult.Format(_T("Defect[%d]_%s"), nDefect, strdtName);
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%s"), strdtName);
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.SIZE_X_mm.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.SIZE_Y_mm.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.AREA.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.AREA_Ratio.GetValue());
                 vecResultData.push_back(strResult);

                 strResult = InspectionArea2String((enInspectionArea)Defect.nInspectionArea.GetValue());
                 vecResultData.push_back(strResult);

                 strResult = enumtoString().InspectionImageType2String(
                     (enInspectionImageType)Defect.nInspectImage.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.RATIO_YX.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.RATIO_XY.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.WIDTH_mm.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.LENGTH_mm.GetValue());
                 vecResultData.push_back(strResult);
                 //RATIO_T? RATIO_L?
                 strResult.Format(_T("%.3f"), Defect.RATIO_LW.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.RATIO_WL.GetValue());
                 vecResultData.push_back(strResult);

                 strResult.Format(_T("%d"), Defect.MIN_GV.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%d"), Defect.AVG_GV.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%d"), Defect.MAX_GV.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%d"), Defect.BRIGHT_AVG_GV.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%d"), Defect.DARK_AVG_GV.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.MASS_CENTER_GV_GV.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_WHITE.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_BLACK.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.SECTION_THRESHOLD.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.HOR.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.VER.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.RATIO_HV.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.RATIO_VH.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.RATIO_WHITE.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.RATIO_DARK.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%d"), Defect.OVER_200_GV_PIXEL_COUNT.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.DISTANCE_TO_EDGE_mm.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%d"), Defect.IS_NEAR_EDGE.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_SHARPNESS.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_SHARPNESS_WHITE.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_SHARPNESS_BLACK.GetValue());
                 vecResultData.push_back(strResult);

				 if (m_dlgBatch->m_chk_DefectPosAdd.GetCheck() == TRUE)
				 {
                     m_ExcelSave.AddImageFileDatas(m_nExcelRow, vecDefectName[0], strDarkImage, strBrigtImage,
                         strBinImage, vecResultData, ResultName, OriginRect, DefectROI, strImagePath);
				 }
				 else
				 {
                     m_ExcelSave.AddImageFileDatas(m_nExcelRow, vecDefectName[0], strDarkImage, strBrigtImage,
                         strBinImage, vecResultData, ResultName, OriginRect);
				 }

             }
             else if (m_nMachineType != 0)
             {
                 CString strBrigtImage;
                 strBrigtImage.Format(_T("D:\\iBCI\\Result\\BrightOri_%d.jpg"), nDefect);
                 //CropImage_Bright.SaveJpeg(strBrigtImage);
                 CString strDarkImage;
                 strDarkImage.Format(_T("D:\\iBCI\\Result\\DarkOri_%d.jpg"), nDefect);
                 //CropImage_Dark.SaveJpeg(strDarkImage);
                 CString strBinImage;
                 strBinImage.Format(_T("D:\\iBCI\\Result\\Bin_%d.jpg"), nDefect);
                 //Defect.srcImage.SaveJpeg(strBinImage);

                 IPVM::Image_8u_C1 OriginImage;
                 TCHAR programpath[_MAX_PATH];
                 GetCurrentDirectory(_MAX_PATH, programpath);
                 CString ImageFolderName = _T("\\DataSearchImage.jpg");
                 CString strImagePath = _T("D:\\UC1\\DataSearchImage.jpg");
                 //programpath + ImageFolderName;

                 OriginImage.LoadFrom(strImagePath);

                 IPVM::Rect Binrect(CropImage_Bright);
                 nX = Binrect.Width();
                 nY = Binrect.Height();

                 IPVM::Image_8u_C1 CropOffsetImg_BIN;
                 CropOffsetImg_BIN.Create(nX, nY);

                 IPVM::ImageProcessing::Fill((IPVM::Rect)CropOffsetImg_BIN, 0, CropOffsetImg_BIN);

                 IPVM::Rect rt;

                 long MoveX = (nX - OriginRect_Width) / 2;
                 long MoveY = (nY - OriginRect_Height) / 2;
                 rt = IPVM::Rect(Defect.srcImage);
                 rt.MoveToX(MoveX);
                 rt.MoveToY(MoveY);

                 IPVM::ImageProcessing::Copy(Defect.srcImage, IPVM::Rect(Defect.srcImage),
                     /*IPVM::Rect(CropImage_Bright)*/ rt, CropOffsetImg_BIN);

                 //Defect.srcImage.SaveJpeg(_T("D:\\srcImage.jpg"));
                 //  CropOffsetImg_BIN.SaveJpeg(_T("D:\\CropOffsetImg_BIN.jpg"));
                 m_ExcelSave.SetBrightImage(CropImage_Bright);
                 m_ExcelSave.SetDarkImage(CropImage_Dark);
                 m_ExcelSave.SetBinImage(CropOffsetImg_BIN);

                 m_ExcelSave.SetSourceImage(OriginImage);

                 vector<CString> vecResultData;
                 // 여기서 각 Defect 결과 값들을 담는다.
                 CString strResult;
                 CString strTemp = m_dlgBatch->m_multiInlinePath[m_dlgBatch->m_index - 2].Mid(
                     m_dlgBatch->m_multiInlinePath[m_dlgBatch->m_index - 2].ReverseFind(_T('\\')) + 1,
                     m_dlgBatch->m_multiInlinePath[m_dlgBatch->m_index - 2].ReverseFind(_T('.')) + 1);
                 strResult = strTemp;
                 strResult.Format(_T("%s"), strResult);
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%s"), ResultName);
                 vecResultData.push_back(strResult);
                 DefectType dt = parent->m_ISICellAllDefect.m_vecCandidate[nDefect].GetDefectResultCode();
                 CString strdtName = GetDefectName(dt);
                 strResult.Format(_T("Defect[%d]_%s"), nDefect, strdtName);
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%s"), strdtName);
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.SIZE_X_mm.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.SIZE_Y_mm.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.AREA.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.AREA_Ratio.GetValue());
                 vecResultData.push_back(strResult);

                 strResult = InspectionArea2String((enInspectionArea)Defect.nInspectionArea.GetValue());
                 vecResultData.push_back(strResult);

                 strResult = enumtoString().InspectionImageType2String(
                     (enInspectionImageType)Defect.nInspectImage.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.RATIO_YX.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.RATIO_XY.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.WIDTH_mm.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.LENGTH_mm.GetValue());
                 vecResultData.push_back(strResult);
                 //RATIO_T? RATIO_L?
                 strResult.Format(_T("%.3f"), Defect.RATIO_LW.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.RATIO_WL.GetValue());
                 vecResultData.push_back(strResult);

                 strResult.Format(_T("%d"), Defect.MIN_GV.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%d"), Defect.AVG_GV.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%d"), Defect.MAX_GV.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%d"), Defect.BRIGHT_AVG_GV.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%d"), Defect.DARK_AVG_GV.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.MASS_CENTER_GV_GV.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_WHITE.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_BLACK.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.SECTION_THRESHOLD.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.HOR.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.VER.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.RATIO_HV.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.RATIO_VH.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.RATIO_WHITE.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.RATIO_DARK.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%d"), Defect.OVER_200_GV_PIXEL_COUNT.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.DISTANCE_TO_EDGE_mm.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%d"), Defect.IS_NEAR_EDGE.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_SHARPNESS.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_SHARPNESS_WHITE.GetValue());
                 vecResultData.push_back(strResult);
                 strResult.Format(_T("%.3f"), Defect.EDGE_ENERGY_SHARPNESS_BLACK.GetValue());
                 vecResultData.push_back(strResult);

                 if (m_dlgBatch->m_chk_DefectPosAdd.GetCheck() == TRUE)
                 {
                     m_ExcelSave.AddImageFileDatas(m_nExcelRow, vecDefectName[0], strDarkImage, strBrigtImage,
                         strBinImage, vecResultData, ResultName, OriginRect, DefectROI, strImagePath);
                 }
                 else
                 {
                     m_ExcelSave.AddImageFileDatas(m_nExcelRow, vecDefectName[0], strDarkImage, strBrigtImage,
                         strBinImage, vecResultData, ResultName, OriginRect);
                 }
             }
         }
    }

}
void CDlgInspectionMain::ExcelFileSave(CString strSavePath)
{
	CTime time = CTime::GetCurrentTime();
	if (strSavePath == _T(""))
	{
		strSavePath.Format(_T("D:\\iBCI\\Result\\temp.xlsx"));
	}
	else
	{
		CString strTime;
        strTime.Format(_T("\\%04d-%02d-%02d_%02d_%02d_%02d.xlsx"), time.GetYear(), time.GetMonth(), time.GetDay() ,time.GetHour(), time.GetMinute(), time.GetSecond());
		strSavePath = strSavePath + strTime;
	}
    CreateDir(strSavePath);
	m_ExcelSave.Savexlsfile(strSavePath, FALSE);
}

void CDlgInspectionMain::CreateDir(CString strFolderPath)
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



BOOL CDlgInspectionMain::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
        {
            return TRUE;
		}
	}
    return CDialog::PreTranslateMessage(pMsg);
}
