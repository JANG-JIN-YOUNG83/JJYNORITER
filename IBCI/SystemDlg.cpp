// SystemDlg.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "IBCI.h"
#include "SystemDlg.h"
#include "afxdialogex.h"

#include "IBCIDlg.h"
#include "../Grab/GrabExecutor.h"

#include "../Module/CustomControl/XTPPropertyGridItemButton.h"
#include "../Module/CustomControl/PropertyGridHelper.h"
#include "../VisionModule/Language/LanguageText.h"
#include "../VisionModule/Language/PropertyGridHelperEx.h"

#include <PropertyGrid/XTPPropertyGridDefines.h>

enum
{
	IDC_SYSTEM_CF_PATH = 1001,

	IDC_SYSTEM_CAMERA_GAIN,
	IDC_SYSTEM_LED_BRIGHTNESS,
	
	IDC_SYSTEM_IMAGE_SAVE_PATH,
    IDC_SYSTEM_RESULT_CSV_PATH,

	IDC_SYSTEM_RESOLUTION_Xup,
	IDC_SYSTEM_RESOLUTION_Xdown,
	IDC_SYSTEM_RESOLUTION_Y,
	IDC_SYSTEM_USE_VHOME_OFFSET_IN_SYSTEM,

	IDC_SYSTEM_USE_IMAGE_LINE_MISSED_ALARM,
	IDC_SYSTEM_SAVE_LINE_MISSED_IMAGE,
	IDC_SYSTEM_ROLLMAP,
    IDC_SYSTEM_EDGE_VIEW_SAMPLING,
    IDC_SYSTEM_EDGE_VIEW_SAMPLING_COUNT,

	IDC_CONNECTION_TAPE_INTERLOCK = 2000,
	IDC_CONNECTION_TAPE_X_SIZE,
};

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSystemDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSystemDlg, CDialogEx)

CSystemDlg::CSystemDlg(const SystemParameter& sysParam, CIBCIDlg* pParent)
	: CDialogEx(CSystemDlg::IDD, pParent)
	, m_parent(pParent)
	, m_sysParam(sysParam)
{
	m_systemParaGrid = new CXTPPropertyGrid;
}

CSystemDlg::~CSystemDlg()
{
	delete m_systemParaGrid;
}

void CSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSystemDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SYSTEM_OK, &CSystemDlg::OnBnClickedButtonSystemOk)
	ON_BN_CLICKED(IDC_BUTTON_SYSTEM_CANCEL, &CSystemDlg::OnBnClickedButtonSystemCancel)
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
END_MESSAGE_MAP()


// CSystemDlg 메시지 처리기입니다.


BOOL CSystemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	SetBackgroundColor(RGB(255, 255, 255));

	SetWindowText(LANG_TEXT(LanguageDef::System));
	GetDlgItem(IDC_BUTTON_SYSTEM_OK)->SetWindowText(LANG_TEXT(LanguageDef::OK));
	GetDlgItem(IDC_BUTTON_SYSTEM_CANCEL)->SetWindowText(LANG_TEXT(LanguageDef::CANCEL));

	// 언어가 현재 똑같이 지정된 텍스트 내용입니다.
	// 언어 별로 다르게 표시하시려면 설정관련 내용을 한쪽으로 몰기 위해 LanguageDef.h에 넣어서 이용해 주세요.

	strCamSingleDual = _T("Single / Dual Camera");
	strCameraGain = _T("Camera Gain");
	strLedBrightness = _T("Led Brightness");
	strAnodeCathode = _T("Anode Cathode");
	strUseAreaCam = _T("Use Area Cam");
	strCellIDBit = _T("Cell ID Bit");
	strIsLaserNotching = _T("Laser Notching");
	strInsulateType = _T("Insulate Type");
	strEncoderAlarmTimer = _T("Encoder Alarm Timer (ms)");
	strInterlockSleepTime = _T("Interlock Sleep Time (ms)");
	strInspectTilt = _T("Inspect Tilt");
	strInspectMisalign = _T("Inspect Misalign");
	strInspectPetVinyl = _T("Inspect PET Vinyl");
	strMismatchAbsolute = _T("Mismatch Absolute Value");
	strPitchUpDownUse = _T("Pitch Up Down Mid Best");
	strSensorImageMirror = _T("Sensor Image Mirror");
	strExistGrabHardware = _T("Exist Grab Hardware");
	strScrabInterlock = _T("Scrab Interlock Use");
	strTabSensorExceptionHandling = _T("Tab Sensor Exception Handling");
	strTabCrossCheck = _T("Tab Cross Check");
	strTabHeightTop = _T("Use Tab Height Top");
	strTabShapeRectCount = _T("Tab Shape Rect Count");

	strBrightImageSend = _T("Bright Image Send");
	strOverlayImage = _T("Overlay Dark Image Send");
	strTabShapeImage = _T("Tab Shpe Bright Image Send");

	strTab = _T("Tab");
	strSurface = _T("Surface");
	strCamSingle = _T("Single");
	strCamDual = _T("Dual");

	strVHomeOffsetUp = _T("V Home Offset Up [px]");
	strVHomeOffsetDown = _T("V Home Offset Down [px]");

	strMultiImage = _T("Multi Image");
	strImageCheckPos = _T("Multi Image Check Position");
	strImageCheckBrightness = _T("Multi Image Check Brightness");

	strInspection = _T("Inspection");
	strInspectionMargin = _T("Cell Cutting Margin [px]");

	strCore = _T("Core");

	strResultDataSavePath = _T("Result Save Path");
	strResultAName = _T("Result A Name");
	strResultBName = _T("Result B Name");
	strMachineNumber = _T("Machine Number");
	strResultImageSave = _T("Result Image Save");
	strDebugImageSave = _T("Debug Image Save");
	strMarkingCsvSave = _T("Marking CSV Save");

	strSendImageInfo = _T("Send Image Info");
	strUpImageLeft = _T("Up Image Left Pos");
	strUpImageRight = _T("Up Image Right Pos");
	strDownImageLeft = _T("Down Image Left Pos");
	strDownImageRight = _T("Down Image Right Pos");

	strConnectIP = _T("Connect IP");
	strConnectPort = _T("Connect Port");
	strFriendIP = _T("Friend IP");
	strFriendPort = _T("Friend Port");

	strRollMapRotation = _T("Rotation(cw)");
	strRollMapXaxisMirror = _T("X Axis Mirror");
	strRollMapYaxisMirror = _T("Y Axis Mirror");
	strRollMapXOffset = _T("X offset(px)");
	strRollMapYOffset = _T("Y offset(px)");
	strRollMapImageQuality = _T("Save Image Quality(%)%");
    strMotorIORetry = _T("Motor I/O Retry");


	CRect rect;
	GetClientRect(rect);

	RECT gridRect = { 0, 0, rect.Width(), rect.Height() * 0.9 };

	m_systemParaGrid->Create(gridRect, this, 0);
	m_systemParaGrid->SetViewDivider(0.5);
	m_systemParaGrid->HighlightChangedItems(TRUE);
	m_systemParaGrid->SetBorderStyle(XTPPropertyGridBorderStyle::xtpGridBorderFlat);
	m_systemParaGrid->SetShowInplaceButtonsAlways(TRUE);
	m_systemParaGrid->SetViewDivider(0.4);

	UpdateGridList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CSystemDlg::UpdateGridList()
{
	m_systemParaGrid->ResetContent();

	CString strPleaseRestart;

	if (m_sysParam.nLanguage == 1)
	{
		strPleaseRestart = _T("※설정 값 변경 후 반드시 프로그램을 재실행해주세요.※");
	}
	else if (m_sysParam.nLanguage == 2 || m_sysParam.nLanguage == 4)
	{
		strPleaseRestart = _T("※Be sure to re-run the program after changing the setting value.※");
	}
	else if (m_sysParam.nLanguage == 3)
	{
		strPleaseRestart = _T("※请务必在更改设置值后重新运行程序。※");
	}

	//{{ system category
	if (auto* category = m_systemParaGrid->AddCategory(_T("Equipment Option")))
	{
		if (auto* item = PropertyGridHelperEx::AddLink_Enum(category, LanguageDef::Tooltip_Electrode, m_sysParam.nAnodeCathode))
		{
			item->GetConstraints()->AddConstraint(_T("Cathode"), 0);
			item->GetConstraints()->AddConstraint(_T("Anode"), 1);
			item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_Electrode));
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_Electrode));

			if (Share::enLoginMode::CrewLeader >= m_loginMode)
				item->SetReadOnly();
		}
		if (auto* item = PropertyGridHelperEx::AddLink(category, LanguageDef::Tooltip_MachineNumber, m_sysParam.szMachineNumber))
		{
			item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_MachineNumber));
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_MachineNumber));

			//if (Share::enLoginMode::CrewLeader >= m_loginMode)
			//	item->SetReadOnly();
		}

		if (auto* item = PropertyGridHelperEx::AddLink(category, LanguageDef::Tooltip_UseRecipePath, m_sysParam.m_currentModel))
		{
			item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_UseRecipePath));
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_UseRecipePath));

			if (Share::enLoginMode::CrewLeader >= m_loginMode)
				item->SetReadOnly();
		}
		if (auto* item = PropertyGridHelperEx::AddLink_Enum(category, LanguageDef::Tooltip_MachineType, (long&)m_sysParam.machineType))
		{
			item->GetConstraints()->AddConstraint(_T("UC1_mismatch"), 0);
			item->GetConstraints()->AddConstraint(_T("ES_mismatch"), 1);
			item->GetConstraints()->AddConstraint(_T("ES_notching_single"), 2);
			item->GetConstraints()->AddConstraint(_T("ES_notching_dual"), 3);
            item->GetConstraints()->AddConstraint(_T("ES_rollpress_pinhole"), 4);
            item->GetConstraints()->AddConstraint(_T("ES_rollpress_surface"), 5);
            item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_MachineType));
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_MachineType));

			//if (Share::enLoginMode::CrewLeader >= m_loginMode)
			//	item->SetReadOnly();
		}
		if (auto* item = PropertyGridHelperEx::AddLink_Enum(category, LanguageDef::Tooltip_ExecutorType, (long&)m_sysParam.executorType))
		{
			item->GetConstraints()->AddConstraint(_T("iGrab"), 0);
			item->GetConstraints()->AddConstraint(_T("Sapera"), 1);
			item->GetConstraints()->AddConstraint(_T("SharedMem"), 2);
            item->GetConstraints()->AddConstraint(_T("Euresys"), 3);
            item->GetConstraints()->AddConstraint(_T("Insnex"), 4);
			item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_ExecutorType));
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_ExecutorType));

			//if (Share::enLoginMode::CrewLeader >= m_loginMode)
			//	item->SetReadOnly();
		}


		category->Expand();
	}
	//{{ Image Save Path category
	if (auto* category = m_systemParaGrid->AddCategory(LANG_TEXT(LanguageDef::ImageSavePath)))
	{
		if (auto* item = PropertyGridHelperEx::AddLink_Enum(category, LanguageDef::Tooltip_SaveImage, m_sysParam.m_imageSaveOption))
		{
			item->GetConstraints()->AddConstraint(_T("None"), 0);
			//			item->GetConstraints()->AddConstraint(_T("Raw Image save"), 1);		// 사용 안함
			item->GetConstraints()->AddConstraint(_T("Cell Image before Inspection save"), 2);
			item->GetConstraints()->AddConstraint(_T("All Cell Image after Inspection save"), 3);
			item->GetConstraints()->AddConstraint(_T("Reject Cell Image after Inspection save"), 4);
            item->GetConstraints()->AddConstraint(_T("Image Resize Inspection save"), 5);
			item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_SaveImage));
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_SaveImage));
		}

		if (auto* item = PropertyGridHelperEx::AddLink_Enum(category, LanguageDef::Tooltip_SaveImage_Format, (int&)m_sysParam.m_imageSaveType))
		{
			item->GetConstraints()->AddConstraint(_T("Jpg"), (int)ImageSaveType::Jpg);
			item->GetConstraints()->AddConstraint(_T("Bmp"), (int)ImageSaveType::Bmp);
			item->GetConstraints()->AddConstraint(_T("Png"), (int)ImageSaveType::Png);

			item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_SaveImage_Format));
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_SaveImage_Format));

			//if (Share::enLoginMode::CrewLeader >= m_loginMode)
			//	item->SetReadOnly();
		}
 
		if (auto* item = PropertyGridHelperEx::AddLink(category, _T("Image Resize Reduce 1/? "), m_sysParam.m_nImageResize))
        {
			
		}
  
        if (auto* item = PropertyGridHelper::AddLink(category, _T("이미지 저장 경로"), m_sysParam.strPathImageSave))
        {
            item->AddExpandButton();
            item->SetID(IDC_SYSTEM_IMAGE_SAVE_PATH);
		}
        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Batch Folder Tree"), m_sysParam.bBatchFolderTree))
        {
            item->GetConstraints()->AddConstraint(_T("Default"), 0);
            item->GetConstraints()->AddConstraint(_T("Current"), 1);
        }
		category->Expand();
	}
	//}} Image Save Path category
	if (auto* category = m_systemParaGrid->AddCategory(LANG_TEXT(LanguageDef::System)))
	{
		
		if (auto* item = PropertyGridHelperEx::AddLink(category, LanguageDef::Tooltip_CfPath, m_sysParam.strCfPath))
		{
			item->AddExpandButton();
			item->SetID(IDC_SYSTEM_CF_PATH);
			item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_CfPath) + _T("\n") + strPleaseRestart);
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_CfPath));

			//if (Share::enLoginMode::CrewLeader >= m_loginMode)
			//	item->SetReadOnly();
		}



		if (auto* item = PropertyGridHelperEx::AddLink_Enum(category, LanguageDef::Tooltip_PCID, m_sysParam.nPC_ID))
		{
			item->GetConstraints()->AddConstraint(_T("1"), 1);
            item->GetConstraints()->AddConstraint(_T("2"), 2);
			item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_PCID));
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_PCID));

			//if (Share::enLoginMode::CrewLeader >= m_loginMode)
			//	item->SetReadOnly();
		}

		if (auto* item = PropertyGridHelperEx::AddLink_Bool(category, LanguageDef::Tooltip_SensorImageMirror, m_sysParam.bSensorImageMirror))
		{
			item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_SensorImageMirror));
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_SensorImageMirror));

			//if (Share::enLoginMode::CrewLeader >= m_loginMode)
			//	item->SetReadOnly();
		}
		if (auto* item = PropertyGridHelperEx::AddLink_Bool(category, LanguageDef::Tooltip_ExistCam, m_sysParam.m_isExistGrabHardware))
		{
			item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_ExistCam) + _T("\n") + strPleaseRestart);
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_ExistCam));

			if (Share::enLoginMode::CrewLeader >= m_loginMode)
				item->SetReadOnly();
		}

		if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("TOP/BACK"), m_sysParam.bDirection))
		{
			item->GetConstraints()->AddConstraint(_T("TOP"), 0);
			item->GetConstraints()->AddConstraint(_T("BACK"), 1);
		}
        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Mismatch Animation 방향"), m_sysParam.bOSDS))
        {
            item->GetConstraints()->AddConstraint(_T("DS/OS"), 0);
            item->GetConstraints()->AddConstraint(_T("OS/DS"), 1);
        }
        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("롤러 시작 방향"), m_sysParam.bRollerOS))
        {
            item->GetConstraints()->AddConstraint(_T("DS"), 0);
            item->GetConstraints()->AddConstraint(_T("OS"), 1);
        }
        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("코팅 단면/양면"), m_sysParam.bRollerTwoSide))
        {
            item->GetConstraints()->AddConstraint(_T("단면"), 0);
            item->GetConstraints()->AddConstraint(_T("양면"), 1);
        }
		if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Defect CSV Save"), m_sysParam.bDefectCsvSave))
		{
			item->GetConstraints()->AddConstraint(_T("NO"), 0);
			item->GetConstraints()->AddConstraint(_T("YES"), 1);
		}
		if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("MSA CSV Save"), m_sysParam.bMSASave))
		{
			item->GetConstraints()->AddConstraint(_T("NO"), 0);
			item->GetConstraints()->AddConstraint(_T("YES"), 1);
		}
        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Measure CSV Save"), m_sysParam.bMeasureSave))
        {
            item->GetConstraints()->AddConstraint(_T("NO"), 0);
            item->GetConstraints()->AddConstraint(_T("YES"), 1);
        }
        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Mismatch CSV Save"), m_sysParam.bMismatchSave))
        {
            item->GetConstraints()->AddConstraint(_T("NO"), 0);
            item->GetConstraints()->AddConstraint(_T("YES"), 1);
        }
        if (auto* item = PropertyGridHelper::AddLink(category,_T("Warning Count"), m_sysParam.nWarningCnt))
        {
			
        }
        if (auto* item = PropertyGridHelper::AddLink(category, _T("Spec Count"), m_sysParam.nSpecCnt))
        {
        }
        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("EdgeView Image Type"),m_sysParam.nEdgeViewImageType))
        {
            item->GetConstraints()->AddConstraint(_T("Default"), 0);
            item->GetConstraints()->AddConstraint(_T("Bright"), 1);
            item->GetConstraints()->AddConstraint(_T("Dark"), 2);
        }

        if (auto* itemUse = PropertyGridHelper::AddLink_Enum(category, _T("EdgeView Sampling"), m_sysParam.bEdgeViewSampling))
        {
            itemUse->SetID(IDC_SYSTEM_EDGE_VIEW_SAMPLING);
            itemUse->GetConstraints()->AddConstraint(_T("Off"), 0);
            itemUse->GetConstraints()->AddConstraint(_T("On"), 1);

			if (auto* item = PropertyGridHelper::AddLink(category, _T("Sampling Count"), m_sysParam.nSamplingCnt))
            {
                item->SetID(IDC_SYSTEM_EDGE_VIEW_SAMPLING_COUNT);

                if (!m_sysParam.bEdgeViewSampling)
                {
                    item->SetReadOnly();
                    //item->SetValue(_T("0"));
                }
                
            }
        }	


		/*if (auto* item = PropertyGridHelperEx::AddLink_Bool(category, LanguageDef::Tooltip_Tab_Sensor_Exception, m_sysParam.bTabSensorExceptionHandling))
		{
			item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_Tab_Sensor_Exception));
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_Tab_Sensor_Exception));

			if (Share::enLoginMode::CrewLeader >= m_loginMode)
				item->SetReadOnly();
		}*/

		category->Expand();
	}
	//}} system category

	if (auto* category = m_systemParaGrid->AddCategory(LANG_TEXT(LanguageDef::RollMapImageSend)))
	{

		if (auto* item = PropertyGridHelperEx::AddLink_Bool(category, strRollMapXaxisMirror, m_sysParam.bXaxisMirror))
		{
			//item->SetHidden(!m_sysParam.bSendRollMap);
			item->SetDescription(LANG_TEXT(LanguageDef::ToolTip_RollMapXaxisMirror));
			item->SetTooltip(LANG_TEXT(LanguageDef::ToolTip_RollMapXaxisMirror));

			if (Share::enLoginMode::CrewLeader >= m_loginMode)
				item->SetReadOnly();
		}

		if (auto* item = PropertyGridHelperEx::AddLink_Bool(category, strRollMapYaxisMirror, m_sysParam.bYaxisMirror))
		{
			//item->SetHidden(!m_sysParam.bSendRollMap);
			item->SetDescription(LANG_TEXT(LanguageDef::ToolTip_RollMapYaxisMirror));
			item->SetTooltip(LANG_TEXT(LanguageDef::ToolTip_RollMapYaxisMirror));

			if (Share::enLoginMode::CrewLeader >= m_loginMode)
				item->SetReadOnly();
		}
		category->Expand();
	}

	//{{ optics category
	if (auto* category = m_systemParaGrid->AddCategory(LANG_TEXT(LanguageDef::Optics)))
	{
		/*if (auto *item = PropertyGridHelperEx::AddLink_Enum(category, LanguageDef::SensorSizeX, m_nImageSizeX))
		{
		item->GetConstraints()->AddConstraint(_T("2592"), 2592);
		}*/

		if (auto* item1 = PropertyGridHelperEx::AddLink(category, LanguageDef::Tooltip_ResolutionXUp, _T("%lf"), m_sysParam.m_dPixelToUmXUp))
		{
			item1->SetDescription(LANG_TEXT(LanguageDef::Tooltip_ResolutionXUp));
			item1->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_ResolutionXUp));

			auto pButton = item1->GetInplaceButtons()->AddButton(new CXTPPropertyGridInplaceButton(IDC_SYSTEM_RESOLUTION_Xup));
			pButton->SetCaption(_T("Setting"));
		}
		if (auto* item2 = PropertyGridHelperEx::AddLink(category, LanguageDef::Tooltip_ResolutionXDown, _T("%lf"), m_sysParam.m_dPixelToUmXDown))
		{
			item2->SetDescription(LANG_TEXT(LanguageDef::Tooltip_ResolutionXDown));
			item2->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_ResolutionXDown));

			auto pButton = item2->GetInplaceButtons()->AddButton(new CXTPPropertyGridInplaceButton(IDC_SYSTEM_RESOLUTION_Xdown));
			pButton->SetCaption(_T("Setting"));
		}
		if (auto* item3 = PropertyGridHelperEx::AddLink(category, LanguageDef::Tooltip_ResolutionY, _T("%lf"), m_sysParam.m_dPixelToUmY))
		{
			item3->SetDescription(LANG_TEXT(LanguageDef::Tooltip_ResolutionY));
			item3->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_ResolutionY));

			auto pButton = item3->GetInplaceButtons()->AddButton(new CXTPPropertyGridInplaceButton(IDC_SYSTEM_RESOLUTION_Y));
			pButton->SetCaption(_T("Setting"));
		}
		if (auto* item = PropertyGridHelperEx::AddLink(category, LanguageDef::Tooltip_CuttingPitch, _T("%.2f"), m_sysParam.fCuttingPitch))
		{
			item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_CuttingPitch));
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_CuttingPitch));

			//if (Share::enLoginMode::CrewLeader >= m_loginMode)
			//	item->SetReadOnly();
		}
		if (auto* item = PropertyGridHelperEx::AddLink(category, LanguageDef::Tooltip_CuttingMargin, m_sysParam.nCuttingMargin))
		{
			item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_CuttingMargin));
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_CuttingMargin));

			//if (Share::enLoginMode::CrewLeader >= m_loginMode)
			//	item->SetReadOnly();
		}
		category->Expand();
	}
	//}} optics category




	//{{ grab category
	if (auto* category = m_systemParaGrid->AddCategory(LANG_TEXT(LanguageDef::Grab)))
	{
		//if (auto* item = PropertyGridHelperEx::AddLink_Enum(category, LanguageDef::Tooltip_InspMode, m_sysParam.nInspectionMode))
		//{
		//	item->GetConstraints()->AddConstraint(LANG_TEXT(LanguageDef::OnlyGrab), 0);
		//	item->GetConstraints()->AddConstraint(LANG_TEXT(LanguageDef::GrabAndCutting), 1);
		//	item->GetConstraints()->AddConstraint(LANG_TEXT(LanguageDef::GrabAndInspection), 2);
		//	item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_InspMode));
		//	item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_InspMode));

		//}

		category->Expand();
	}
	//}} grab category


	//{{ save category
	if (auto* category = m_systemParaGrid->AddCategory(LANG_TEXT(LanguageDef::FileSave)))
	{
		if (auto* item = PropertyGridHelperEx::AddLink(category, LanguageDef::Tooltip_Image_Queue_Count, m_sysParam.m_imageQueueSize))
		{
			item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_Image_Queue_Count));
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_Image_Queue_Count));

			if (Share::enLoginMode::CrewLeader >= m_loginMode)
				item->SetReadOnly();
		}
		category->Expand();
	}
	//}} save category


	if (auto* category = m_systemParaGrid->AddCategory(LANG_TEXT(LanguageDef::TCPIP_Info)))
	{
		if (auto* item = PropertyGridHelperEx::AddLink(category, LanguageDef::Tooltip_ConnectIP, m_sysParam.szConnectIP))
		{
			item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_ConnectIP) + _T("\n") + strPleaseRestart);
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_ConnectIP));

			//if (Share::enLoginMode::CrewLeader >= m_loginMode)
			//	item->SetReadOnly();
		}
		if (auto* item = PropertyGridHelperEx::AddLink(category, LanguageDef::Tooltip_ConnectPort, m_sysParam.nConnectPort))
		{
			item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_ConnectPort) + _T("\n") + strPleaseRestart);
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_ConnectPort));

			//if (Share::enLoginMode::CrewLeader >= m_loginMode)
			//	item->SetReadOnly();
		}
		category->Expand();
	}

	if (auto* category = m_systemParaGrid->AddCategory(_T("Advenced System Option")))
	{
		//{{ IO category
		PropertyGridHelperEx::AddLink_Bool(category, _T("UseISI"), m_sysParam.bUseISI)->SetReadOnly();

		//{{ core category
		if (auto* item = PropertyGridHelperEx::AddLink(category, LanguageDef::Tooltip_CoreCount, m_sysParam.m_coreCount))
		{

			item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_CoreCount));
			item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_CoreCount));
			item->SetReadOnly();

		}
		//}} core category
	}

	if (auto* category = m_systemParaGrid->AddCategory(_T("Option")))
	{
		auto* itemUse = PropertyGridHelperEx::AddLink_Bool(category, _T("USE"), m_sysParam.bUseServer);

		if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Client/Server"), m_sysParam.bServer))
		{	
			item->GetConstraints()->AddConstraint(_T("Client"), 0);
			item->GetConstraints()->AddConstraint(_T("Server"), 1);

			if (itemUse->GetValue() == _T("Not Use"))
				item->SetReadOnly();
			
		}

		if (auto* item = PropertyGridHelper::AddLink(category, _T("Host IP"), m_sysParam.strHostIP))
		{
			//item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_ConnectIP));
			//item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_ConnectIP));

			if (itemUse->GetValue() == _T("Not Use"))
				item->SetReadOnly();
		}
		if (auto* item = PropertyGridHelper::AddLink(category, _T("Port"), m_sysParam.nPort))
		{
			//item->SetDescription(LANG_TEXT(LanguageDef::Tooltip_ConnectPort));
			//item->SetTooltip(LANG_TEXT(LanguageDef::Tooltip_ConnectPort));

			if (itemUse->GetValue() == _T("Not Use"))
				item->SetReadOnly();
		}
		if (auto* item = PropertyGridHelper::AddLink(category, _T("Client Name"), m_sysParam.nClientPCID))
		{
			//item->SetDescription(LANG_TEXT(LanguageDef::PC_ID));
			//item->SetTooltip(LANG_TEXT(LanguageDef::PC_ID));

			if (itemUse->GetValue() == _T("Not Use"))
				item->SetReadOnly();
		}
		if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Lane 1/2"), m_sysParam.nLaneCount))
		{
			item->GetConstraints()->AddConstraint(_T("Lane 1"), 1);
			item->GetConstraints()->AddConstraint(_T("Lane 2"), 2);
		}
        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Not Pair"), m_sysParam.bNotPair))
        {
            item->GetConstraints()->AddConstraint(_T("Not Use"), 0);
            item->GetConstraints()->AddConstraint(_T("Use"), 1);
        }
        if (auto* item = PropertyGridHelper::AddLink(category, _T("Path_Result_Data_Csv"), m_sysParam.pathResultDataCsv))
        {
            item->AddExpandButton();
            item->SetID(IDC_SYSTEM_RESULT_CSV_PATH);
        }
        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Display Show"), m_sysParam.bDisplayShow))
        {
            item->GetConstraints()->AddConstraint(_T("Not Show Display"), 0);
            item->GetConstraints()->AddConstraint(_T("Show Display"), 1);
            
        }

        if (auto* category = m_systemParaGrid->AddCategory(_T("Motor")))
        {
            if (auto* item = PropertyGridHelperEx::AddLink(category, _T("Motor I/O Retry"), m_sysParam.m_nIORetry))
            {
            }
        }
		
		
		category->Expand();
	}
}

void CSystemDlg::OnBnClickedButtonSystemOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}


void CSystemDlg::OnBnClickedButtonSystemCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnCancel();
}


void CCustomXTPCombo::OnValueChanged(CString strValue)
{
	CXTPPropertyGridItem::OnValueChanged(strValue);

	int nIndex = GetConstraints()->GetCurrent();

	m_strItemValues[nIndex] = GetValue();

	GetConstraints()->RemoveAll();

	m_bValueChanged = TRUE;

	for (int i = 0; i < m_strItemValues.GetCount(); i++)
	{
		GetConstraints()->AddConstraint(m_strItemValues[i]);
	}

	m_bValueChanged = FALSE;
}


void CCustomXTPCombo::OnConstraintsChanged()
{
	CXTPPropertyGridItem::OnConstraintsChanged();

	if (GetConstraints()->GetCount() && !m_bValueChanged)
	{
		m_strItemValues.SetSize(GetConstraints()->GetCount());

		for (int i = 0; i < GetConstraints()->GetCount(); i++)
		{
			m_strItemValues[i] = GetConstraints()->GetAt(i);
		}
	}
}

LRESULT CSystemDlg::OnGridNotify(WPARAM wparam, LPARAM lparam)
{
	//	if (wparam != XTP_PGN_CUSTOM_BUTTON_CLICK)
	//	{
	//		return 0;
	//	}
	
	if (wparam == XTP_PGN_INPLACEBUTTONDOWN)
	{
		auto index = ((CXTPPropertyGridInplaceButton*)lparam)->GetID();
		switch (index)
		{
		case 	IDC_SYSTEM_RESOLUTION_Xup:
		case 	IDC_SYSTEM_RESOLUTION_Xdown:
		case 	IDC_SYSTEM_RESOLUTION_Y:
			SetResolution(index);
			break;
		}
	}
	if (wparam == XTP_PGN_SELECTION_CHANGED || wparam == XTP_PGN_ITEMVALUE_CHANGED || wparam == XTP_PGN_EDIT_CHANGED)
	{
		if (wparam == XTP_PGN_ITEMVALUE_CHANGED)
		{
			auto index = ((CXTPPropertyGridItem*)lparam)->GetID();

			switch (index)
			{
			case IDC_SYSTEM_USE_IMAGE_LINE_MISSED_ALARM:
			{
				auto* item = (CXTPPropertyGridItemBool*)m_systemParaGrid->FindItem(index);

				if (item->GetBool() == true)
				{
					auto* item2 = (CXTPPropertyGridItemBool*)m_systemParaGrid->FindItem(IDC_SYSTEM_SAVE_LINE_MISSED_IMAGE);
					item2->SetReadOnly(FALSE);
				}
				else
				{
					auto* item2 = (CXTPPropertyGridItemBool*)m_systemParaGrid->FindItem(IDC_SYSTEM_SAVE_LINE_MISSED_IMAGE);
					item2->SetBool(FALSE);
					item2->SetReadOnly(TRUE);
				}
			}
			case IDC_CONNECTION_TAPE_INTERLOCK:
			{
				auto* item2 = (CXTPPropertyGridItemBool*)m_systemParaGrid->FindItem(IDC_CONNECTION_TAPE_X_SIZE);
				item2->SetHidden(!m_sysParam.bConnectionTapeInterlock);
				break;
			}
            case IDC_SYSTEM_EDGE_VIEW_SAMPLING:
            {
                auto* item = (CXTPPropertyGridItemBool*)m_systemParaGrid->FindItem(IDC_SYSTEM_EDGE_VIEW_SAMPLING);
                auto* item2 = m_systemParaGrid->FindItem(IDC_SYSTEM_EDGE_VIEW_SAMPLING_COUNT);

                if (item->GetValue() == _T("On"))
                {
                    item2->SetReadOnly(FALSE);
                    if (item2->GetValue() == _T("0"))
                    {
                        MessageBox(_T("SamplingCount Value Can't be '0'"));
                    }
                }
                else if (item->GetValue() == _T("Off"))
                {
                    item2->SetReadOnly(TRUE);
                }
                break;
            }
			}
		}
	}
	else
	{
		return 0;
	}

	auto index = ((CXTPPropertyGridItem*)lparam)->GetID();

	switch (index)
	{
	case IDC_SYSTEM_CF_PATH:
		OnBtnClickedCfPath();
		break;
	case IDC_SYSTEM_ROLLMAP:
		SetSiblingItems(index, lparam);
		break;
	case IDC_SYSTEM_USE_VHOME_OFFSET_IN_SYSTEM:
		SetSiblingItems(index, lparam);
		break;
    case IDC_SYSTEM_IMAGE_SAVE_PATH:
        OnBtnClickedImageSavePath();
        break;
    case IDC_SYSTEM_RESULT_CSV_PATH:
        OnBtnClickedResultCSVPath();
        break;
		//default:	///BUG: 수정 필요
		//	AfxMessageBox(_T("잘못된 선택입니다."));
		//	break;

			/*default:
			if (index >= IDC_PARA_NG_MARK_START && index < IDC_PARA_NG_MARK_END)
			{
			GetParaNgMark(index);
			}*/

			//			AfxMessageBox(_T("what?"));
			//break;
	}

	return 0;
}

void CSystemDlg::OnBtnClickedImageSavePath()
{
    CFolderPickerDialog dlg(NULL, OFN_FILEMUSTEXIST | OFN_CREATEPROMPT);
	if (dlg.DoModal() == IDOK)
    {
        CString path = dlg.GetFolderPath();
        auto* imgSaveItem = (CXTPPropertyGridItem*)m_systemParaGrid->FindItem(IDC_SYSTEM_IMAGE_SAVE_PATH);
        m_sysParam.strPathImageSave = path;
        imgSaveItem->SetValue(path);
	}
}

void CSystemDlg::OnBtnClickedCfPath()
{
	uint32_t flag = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT;

	CFileDialog dlgICF(TRUE, _T("ICF"), _T("*.ICF"), flag, _T("ICF Files (*.ICF)|*.ICF||"));
	CFileDialog dlgCCF(TRUE, _T("CCF"), _T("*.CCF"), flag, _T("CCF Files (*.CCF)|*.CCF||"));

	CFileDialog* pDlg = m_sysParam.executorType == ExecutorType::iGrab ? &dlgICF : &dlgCCF;
	CFileDialog& dlg = *pDlg;
	if (dlg.DoModal() == IDOK)
	{
		CString path = dlg.GetPathName();
		auto* cfPathItem = (CXTPPropertyGridItem*)m_systemParaGrid->FindItem(IDC_SYSTEM_CF_PATH);
		m_sysParam.strCfPath = path;
		cfPathItem->SetValue(path);
	}
}

void CSystemDlg::SetSiblingItems(int nID, LPARAM lparam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;
	CXTPPropertyGridItems* pSiblingItems = pItem->GetParentItem()->GetChilds();

	for (int i = 0; i < pSiblingItems->GetCount(); i++)
	{
		if (pSiblingItems->GetAt(i) != pItem)
		{
			pSiblingItems->GetAt(i)->SetHidden(!((CXTPPropertyGridItemBool*)pItem)->GetBool());
		}
	}

}
void CSystemDlg::SetResolution(int nID)
{

}

void CSystemDlg::OnBtnClickedResultCSVPath()
{
    CFolderPickerDialog dlg(NULL, OFN_FILEMUSTEXIST | OFN_CREATEPROMPT);
    if (dlg.DoModal() == IDOK)
    {
        CString path = dlg.GetFolderPath();
        auto* imgSaveItem = (CXTPPropertyGridItem*)m_systemParaGrid->FindItem(IDC_SYSTEM_RESULT_CSV_PATH);
        m_sysParam.pathResultDataCsv = path;
        imgSaveItem->SetValue(path);
    }
}