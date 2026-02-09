// DlgViewCameraSetting.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "DlgViewCameraSetting.h"
#include "afxdialogex.h"

#include "IBCIDlg.h"
#include "../SharedModule/VisionHostShare/ParameterSystem.h"
#include "../Module/Utility/Utility.h"
#include "../Grab/CISComm.h"
#include "../Grab/CisSetting.h"
#include "../Grab/GrabExecutor.h"
#include "../Grab/GrabExecutorContainer.h"
#include "../Module/CustomControl/XTPPropertyGridItemButton.h"
#include "../Module/CustomControl/PropertyGridHelper.h"

#include <Algorithm/ImageProcessing.h>
#include <Base/LoggerInterface.h>
#include <Types/Image_16s_C1.h>
#include <Types/Point_32f_C2.h>
#include <Widget/ImageView.h>
#include <Widget/ProfileView.h>


#include "Mycomm.h"

#include <Windows.h>
#include <SetupAPI.h>
#include <devguid.h>
#include <RegStr.h>
//#include <iostream>
//#include <string>

#pragma comment(lib, "setupapi.lib")

enum
{
    PROPERTY_ID_RESET_LIVE_PROFILE = 100,

#ifndef _USE_CIS_LIVE_
    PROPERTY_ID_TRIGGER_INPUT,
#endif
    PROPERTY_ID_THRESHOLD1,
    PROPERTY_ID_THRESHOLD2,
    PROPERTY_ID_THRESHOLD3,
    PROPERTY_ID_THRESHOLD1_HEX,
    PROPERTY_ID_THRESHOLD2_HEX,
    PROPERTY_ID_THRESHOLD3_HEX,
    IDC_PARA_CAM_EXPOSURE_TIME_BRIGHT_0,
    IDC_PARA_CAM_EXPOSURE_TIME_DARK_0,
    IDC_PARA_CAM_GAIN_0,
    IDC_PARA_CAM_EXPOSURE_TIME_BRIGHT_1,
    IDC_PARA_CAM_EXPOSURE_TIME_DARK_1,
    IDC_PARA_CAM_GAIN_1,
    IDC_PARA_BUTTON,
    IDC_PARA_SENSORGAIN_0,
    IDC_PARA_SENSORGAIN_VALUE_0,
    IDC_PARA_SENSORGAIN_1,
    IDC_PARA_SENSORGAIN_VALUE_1,

    IDC_PARA_CAM_EXPOSURE_SELECTOR_0,
    IDC_PARA_CAM_EXPOSURE_TIME_0,
    IDC_PARA_CAM_ROW_GAIN_SELECTOR_0,
    IDC_PARA_CAM_ROW_GAIN_0,
    IDC_PARA_CAM_RESPONSE_LEVELING_0,
    IDC_PARA_CAM_RESET_0,

    IDC_PARA_CAM_EXPOSURE_SELECTOR_1,
    IDC_PARA_CAM_EXPOSURE_TIME_1,
    IDC_PARA_CAM_ROW_GAIN_SELECTOR_1,
    IDC_PARA_CAM_ROW_GAIN_1,
    IDC_PARA_CAM_RESPONSE_LEVELING_1,
    IDC_PARA_CAM_RESET_1,

    //TRG
    IDC_PARA_TRG_TAB_SENSOR,
    IDC_PARA_NO_TAB,
    IDC_PARA_ENCODER_PULSE,
    IDC_PARA_V1_CELLID_POS,
    IDC_PARA_V1_TRG_POS,
    IDC_PARA_V2_CELLID_POS,
    IDC_PARA_V2_TRG_POS,
    IDC_PARA_V1_LTRG_CYCLE,
    IDC_PARA_V2_LTRG_CYCLE,
    IDC_BUTTON_OPENPORT,
    IDC_PARA_PORTNUM,
    IDC_BUTTON_READDATA,
    IDC_BUTTON_WRITEDATA,

    IDC_BUTTON_NO_TAB_WRITE,    
    IDC_BUTTON_V1_CELLID_POS_WRITE,
    IDC_BUTTON_V1_TRG_POS_WRITE,
    IDC_BUTTON_V2_CELLID_POS_WRITE,
    IDC_BUTTON_V2_TRG_POS_WRITE,

};

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgViewCameraSetting 대화 상자
IMPLEMENT_DYNAMIC(CDlgViewCameraSetting, CDialogEx)

CDlgViewCameraSetting::CDlgViewCameraSetting(IPVM::LoggerInterface& loggerSequence, IPVM::LoggerInterface& loggerParamChanged, CIBCIDlg* pParent)
	: CDialogEx(IDD_DIALOG_VIEW_CAMERA_SETTING, pParent)
	, m_loggerSequence(loggerSequence)
	, m_loggerParamChanged(loggerParamChanged)
	, m_parent(pParent)
	, m_liveView(nullptr)
	, m_meanRoi(100, 100, 900, 900)
	, m_mode(Mode::Offline)
    , m_eventWriteData(FALSE, TRUE)
    , m_eventTerminate(FALSE, TRUE)
    , m_nStep(0)
{
	m_profileView[0] = nullptr;
	m_profileView[1] = nullptr;
	ResetBestFocusInformation(0);
	ResetBestFocusInformation(1);
    m_brs232cState = FALSE;
    m_pButtonOpenPort = new CCustomButton;
    m_bLiveOnOff = FALSE;
    m_pButtonLiveStatus = new CCustomButton;

    m_nValue = 0;
    m_bReset = FALSE;
}

CDlgViewCameraSetting::~CDlgViewCameraSetting()
{
	delete m_profileView[1];
	delete m_profileView[0];
	delete m_liveView;
    delete m_pButtonOpenPort;
    delete m_pButtonLiveStatus;
}

void CDlgViewCameraSetting::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_TITLE, m_staticTitle);
    DDX_Control(pDX, IDC_RADIO_PORT_0, m_buttonPort0);
    DDX_Control(pDX, IDC_RADIO_PORT_1, m_buttonPort1);

    //DDX_Control(pDX, IDC_BUTTON_SETTING_APPLY, m_buttonSettingApply);
    DDX_Control(pDX, IDC_BUTTON_PORTOPEN, *m_pButtonOpenPort);
    DDX_Control(pDX, IDC_BUTTON_TRIGGER_TEST, *m_pButtonLiveStatus);
}


BEGIN_MESSAGE_MAP(CDlgViewCameraSetting, CDialogEx)
	ON_WM_SIZE()
ON_MESSAGE(WM_MYRECEIVE, &CDlgViewCameraSetting::OnReceive)
	ON_BN_CLICKED(IDC_RADIO_PORT_0, &CDlgViewCameraSetting::OnBnClickedRadioPort0)
	ON_BN_CLICKED(IDC_RADIO_PORT_1, &CDlgViewCameraSetting::OnBnClickedRadioPort1)
	//ON_BN_CLICKED(IDC_BUTTON_SETTING_APPLY, &CDlgViewCameraSetting::OnBnClickedButtonSettingApply)
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
	ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTON_PORTOPEN, &CDlgViewCameraSetting::OnBnClickedButtonPortopen)
    ON_BN_CLICKED(IDC_BUTTON_TRIGGER_READ, &CDlgViewCameraSetting::OnBnClickedButtonTriggerRead)
    ON_BN_CLICKED(IDC_BUTTON_TRIGGER_WRITE, &CDlgViewCameraSetting::OnBnClickedButtonTriggerWrite)
    ON_BN_CLICKED(IDC_BUTTON_TRIGGER_WRITE2, &CDlgViewCameraSetting::OnBnClickedButtonLiveOnOff)
    ON_BN_CLICKED(IDC_BUTTON_CAM_SETTING, &CDlgViewCameraSetting::OnBnClickedButtonCamSetting)
    END_MESSAGE_MAP()


// CDlgViewCameraSetting 메시지 처리기
BOOL CDlgViewCameraSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_staticTitle.ModifyStyle(0x0FL, SS_OWNERDRAW);
	m_staticTitle.SetFont(_T("Cambria"), RGB(0, 0, 0), 32, 6, 4);
	m_staticTitle.SetBackColor(RGB(90, 130, 210));
	m_staticTitle.Invalidate();

	m_buttonPort0.SetFont(_T("Cambria"), RGB(0, 0, 0), 16, 5, 5);
	m_buttonPort1.SetFont(_T("Cambria"), RGB(0, 0, 0), 16, 5, 5);
	//m_buttonSettingApply.SetFont(_T("Cambria"), RGB(0, 0, 0), 16, 5, 5);

	m_buttonPort0.SetBackGradation(RGB(250, 250, 250), RGB(215, 215, 215), RGB(228, 228, 228), 1);
	m_buttonPort1.SetBackGradation(RGB(250, 250, 250), RGB(215, 215, 215), RGB(228, 228, 228), 1);
	//m_buttonSettingApply.SetBackGradation(RGB(250, 250, 250), RGB(215, 215, 215), RGB(228, 228, 228), 1);

	m_buttonPort0.SetDraw3D(TRUE);
	m_buttonPort1.SetDraw3D(TRUE);
	m_buttonPort0.EnableWindow(FALSE);
	m_buttonPort1.EnableWindow(TRUE);

	//m_buttonSettingApply.SetDraw3D(TRUE);
	m_property.Create(CRect(0, 0, 1, 1), this, 0);
    m_pButtonOpenPort->SetBackColor(RGB(255, 0, 0));
    m_pButtonLiveStatus->SetBackColor(RGB(255, 0, 0));
    SetDlgItemText(IDC_BUTTON_TRIGGER_WRITE2, _T("Live Off"));

	m_liveView = new IPVM::ImageView(GetSafeHwnd(), IPVM::Rect(0, 0, 1, 1));
#ifndef _USE_CIS_LIVE_
	m_liveView->WindowOverlayAdd(IPVM::Point_32f_C2(10.f, 10.f), _T("LIVE 기능 작업준비중 (아직안됨 - 자리만 만들어 놓음)"), RGB(255, 0, 0), 16);
	m_liveView->WindowOverlayShow();
#else
	m_liveView->ROISet(_T("Mean ROI"), _T("Mean ROI"), m_meanRoi, RGB(0, 255, 0), 120);
	m_liveView->ROISet(_T("Profile ROI"), _T("Profile ROI"), m_profileRoi, RGB(0, 0, 255), 120);
	m_liveView->ROIShow();
#endif

	m_profileView[0] = new IPVM::ProfileView(GetSafeHwnd(), IPVM::Rect(0, 0, 1, 1));
	m_profileView[0]->SetRangeY(0.f, 255.f);
	m_profileView[0]->ShowRangeControlPanel();

	m_profileView[1] = new IPVM::ProfileView(GetSafeHwnd(), IPVM::Rect(0, 0, 1, 1));
	m_profileView[1]->SetRangeY(0.f, 255.f);
	m_profileView[1]->ShowRangeControlPanel();

#ifdef _USE_CIS_LIVE_
	for (int cameraIndex = 0; cameraIndex < 2; cameraIndex++)
	{
		m_parent->m_cisComm->GetGrabExcutor(cameraIndex)->RegisterCallbackOnFrameUpdated(GrabMode::Live, CallbackOnLiveFrameUpdated, this);
	}
#endif

	UpdateControlLayer();

      // 장지수 Trigger Board 제어를 위한 Test Code
    //m_strTBBaseParaPath = _T("D:\\iBCI\\TriggerBoard.ini");
    m_pTBbaseParameter = new TriggerBaseParameter;
    m_pTBbaseParameter->Load(m_strTBBaseParaPath);

    m_triggerParameter = new TriggerRecipeParameter;
    m_triggerParameter->UpdateCalculationParameter(*m_pTBbaseParameter);
    ListCOMpPorts();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgViewCameraSetting::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_staticTitle.GetSafeHwnd() == NULL) return;

	UpdateControlLayer();
}

void CDlgViewCameraSetting::ResetBestFocusInformation(long portIndex)
{
	for (long type = 0; type < 2; type++)
	{
		m_bestFocus[portIndex][type].m_pos = 0.;
		m_bestFocus[portIndex][type].m_profileScore = 0.;
		m_bestFocus[portIndex][type].m_profile.clear();
	}
}

void CDlgViewCameraSetting::MakeHexChannelValue()
{
	auto portNumber = m_parent->m_cisComm->m_selectedPortIndex;

	m_hexChannelValue[0].Format(_T("%02X"), m_parent->m_cisComm->m_paraEdit.m_nCh0[portNumber]);
	m_hexChannelValue[1].Format(_T("%02X"), m_parent->m_cisComm->m_paraEdit.m_nCh1[portNumber]);
	m_hexChannelValue[2].Format(_T("%02X"), m_parent->m_cisComm->m_paraEdit.m_nCh2[portNumber]);
}

void CDlgViewCameraSetting::UpdateControlLayer()
{
	CRect viewRegion;
	GetClientRect(viewRegion);

	long cx = viewRegion.Width();
	long cy = viewRegion.Height();
	long midX = cx / 2;

	m_staticTitle.MoveWindow(0, 0, cx, 40);
	m_property.MoveWindow(10, 100, cx - 20, cy /1.2); //2.5
    m_pButtonOpenPort->MoveWindow(270, 50, 100, 30);
    m_pButtonLiveStatus->MoveWindow(390, 50, 100, 30);
    //m_buttonOpenPort.MoveWindow(10, 100 + cy / 2.5 + 10, midX - 20, cy/2);
	long buttonSizeX = (midX - 20);
	//m_buttonSettingApply.MoveWindow(10, cy - 45, buttonSizeX, 40);

	//::MoveWindow(m_liveView->GetSafeHwnd(), midX, 40, cx - midX, cy - 280, TRUE);
	//::MoveWindow(m_profileView[0]->GetSafeHwnd(), midX, cy - 240, midX / 2, 240, TRUE);
	//::MoveWindow(m_profileView[1]->GetSafeHwnd(), midX + midX / 2, cy - 240, midX / 2, 240, TRUE);
}

void CDlgViewCameraSetting::CISInitialize()
{
	auto& para = m_parent->m_cisComm->m_paraCur;
	para = m_parent->m_cisComm->m_paraEdit;

	//m_parent->SaveRecipe();	
	m_parent->CamsetApply();
}

void CDlgViewCameraSetting::SetInlineEditMode()
{
	m_mode = Mode::Inline;
	//m_buttonSettingApply.EnableWindow(TRUE);

	UpdateUI_Parameter();
}

void CDlgViewCameraSetting::SetOfflineEditMode()
{
	m_mode = Mode::Offline;
	//m_buttonSettingApply.EnableWindow(TRUE);

	UpdateUI_Parameter();
}

void CDlgViewCameraSetting::Disable()
{
	m_mode = Mode::Disable;
	//m_buttonSettingApply.EnableWindow(FALSE);

	UpdateUI_Parameter();
}

void CDlgViewCameraSetting::Show(bool visible)
{
	if (visible)
	{
#ifdef _USE_CIS_LIVE_
		SetTimer(999, 50, nullptr);
#endif
		ShowWindow(SW_SHOW);
		UpdateUI_Parameter();
	}
	else
	{
#ifdef _USE_CIS_LIVE_
		KillTimer(999);
#endif
		ShowWindow(SW_HIDE);
	}
}

void CDlgViewCameraSetting::UpdateChannelValue()
{
	auto portNumber = m_parent->m_cisComm->m_selectedPortIndex;

	m_parent->m_cisComm->m_paraEdit.m_nCh0[portNumber] = _tcstol(m_hexChannelValue[0], nullptr, 16);
	m_parent->m_cisComm->m_paraEdit.m_nCh1[portNumber] = _tcstol(m_hexChannelValue[1], nullptr, 16);
	m_parent->m_cisComm->m_paraEdit.m_nCh2[portNumber] = _tcstol(m_hexChannelValue[2], nullptr, 16);

	if (auto* item = dynamic_cast<CXTPPropertyGridItemNumber*>(m_property.FindItem(PROPERTY_ID_THRESHOLD1)))
	{
		item->SetNumber(m_parent->m_cisComm->m_paraEdit.m_nCh0[portNumber]);
	}

	if (auto* item = dynamic_cast<CXTPPropertyGridItemNumber*>(m_property.FindItem(PROPERTY_ID_THRESHOLD2)))
	{
		item->SetNumber(m_parent->m_cisComm->m_paraEdit.m_nCh1[portNumber]);
	}

	if (auto* item = dynamic_cast<CXTPPropertyGridItemNumber*>(m_property.FindItem(PROPERTY_ID_THRESHOLD3)))
	{
		item->SetNumber(m_parent->m_cisComm->m_paraEdit.m_nCh2[portNumber]);
	}
}

void CDlgViewCameraSetting::UpdateHexChannelValue()
{
	MakeHexChannelValue();

	if (auto* item = m_property.FindItem(PROPERTY_ID_THRESHOLD1_HEX)) item->SetValue(m_hexChannelValue[0]);
	if (auto* item = m_property.FindItem(PROPERTY_ID_THRESHOLD2_HEX)) item->SetValue(m_hexChannelValue[1]);
	if (auto* item = m_property.FindItem(PROPERTY_ID_THRESHOLD3_HEX)) item->SetValue(m_hexChannelValue[2]);
}

void CDlgViewCameraSetting::UpdateUI_Parameter()
{
    auto portNumber = m_parent->m_cisComm->m_selectedPortIndex;
    auto& para = m_parent->m_cisComm->m_paraEdit;

    long nCam = m_parent->m_cisComm->GetCameraCount();

    for (long nCamIndex = 0; nCamIndex < nCam; nCamIndex++)
    {
        auto* CamExcutor = m_parent->m_cisComm->GetGrabExcutor(nCamIndex);
        long nWidth = CamExcutor->GetHorizontalSize();
        long nHeight = CamExcutor->GetVerticalSize();
        //long nWidth = CamExcutor->GetSensorImageMirror();
    }

    MakeHexChannelValue();

    BOOL readonly_offlineParameter = (m_mode != Mode::Offline);
    BOOL readonly_inlineParameter = (m_mode == Mode::Disable);

    m_property.ResetContent();
    if (auto* category = m_property.AddCategory(_T("Board")))
    {
        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Image Width"), para.m_nLedMode[portNumber]))
        {
            item->SetReadOnly(readonly_offlineParameter);

            //if (Share::enLoginMode::Operator >= m_loginMode)
            // Width 는 값 고정
            item->SetReadOnly();
        }
        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Image Height"), para.m_nLedMode[portNumber]))
        {
            item->SetReadOnly(readonly_offlineParameter);

            //if (Share::enLoginMode::Operator >= m_loginMode)
            item->SetReadOnly();
        }
        category->Expand();
    }

    if (auto* category = m_property.AddCategory(_T("Camera_0")))
    {
        // 노출 값과 Gain 값만 일단 조정?
        double dBright, dDark;
        double dRowGainBright, dRowGainDark;
        int nSensorSel = 0;
        double dSensorGain;
        int nTemp = 0;
        double dTemp = 0.f;

        double dGain = m_parent->m_cisComm->GetGrabExcutor(0)->GetSystemGain();
        m_parent->m_cisComm->GetGrabExcutor(0)->GetExposureTime(dBright, dDark);
        m_parent->m_cisComm->GetGrabExcutor(0)->GetSensorInfo(nSensorSel, dSensorGain);
        m_parent->m_cisComm->GetGrabExcutor(0)->GetRowGain(dRowGainBright, dRowGainDark);
        //if (auto* item = PropertyGridHelper::AddLink(category, _T("Bright_Exposure Time(ms)"), _T("%.3f"), dBright))
        //{
        //    item->SetID(IDC_PARA_CAM_EXPOSURE_TIME_BRIGHT_0);
        //    item->SetDescription(_T("Min: 1.24 ~ Max: 1310.68"));
        //    

        //}
        //if (auto* item = PropertyGridHelper::AddLink(category, _T("Dark_Exposure Time(ms)"), _T("%.3f"), dDark))
        //{
        //    item->SetID(IDC_PARA_CAM_EXPOSURE_TIME_DARK_0);
        //    item->SetDescription(_T("Min: 1.24 ~ Max: 1310.68")); 


        //}
        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Exposure Time Selector"), nTemp))
        {

            item->GetConstraints()->AddConstraint(_T("All"), 0);
            item->GetConstraints()->AddConstraint(_T("Row 1"), 1);
            item->GetConstraints()->AddConstraint(_T("Row 2"), 2);

            
            item->SetID(IDC_PARA_CAM_EXPOSURE_SELECTOR_0);
        }

        if (auto* item = PropertyGridHelper::AddLink(category, _T("Exposure Time"), _T("%.3f"), dBright))
        {
            item->SetID(IDC_PARA_CAM_EXPOSURE_TIME_0);
            item->SetDescription(_T("Min: 1.24 ~ Max: 1310.68"));

        }

        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Row Selector"), nTemp))
        {
            item->GetConstraints()->AddConstraint(_T("All Rows"), 0);
            item->GetConstraints()->AddConstraint(_T("Row 1"), 1);
            item->GetConstraints()->AddConstraint(_T("Row 2"), 2);

            item->SetID(IDC_PARA_CAM_ROW_GAIN_SELECTOR_0);
        }

        if (auto* item = PropertyGridHelper::AddLink(category, _T("Row Gain"), _T("%.3f"), dRowGainBright))
        {
            item->SetID(IDC_PARA_CAM_ROW_GAIN_0);
            item->SetDescription(_T("Min: 1.24 ~ Max: 1310.68"));
        }

        if (auto* item = PropertyGridHelper::AddLink(category, _T("System Gain"), _T("%.3f"), dGain))
        {
            item->SetID(IDC_PARA_CAM_GAIN_0);
            item->SetDescription(_T("Min: 1 ~ Max: 7.99"));
        }

        long nSelect = 0;
        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Sensor Selector"), nSensorSel))
        {
            for (long nSensor = 0; nSensor < 16; nSensor++)
            {
                CString str;
                str.Format(_T("%d"), nSensor);
                item->GetConstraints()->AddConstraint(str, nSensor);
            }
            item->SetDescription(_T("Min: 1 ~ Max: 16"));
            item->SetID(IDC_PARA_SENSORGAIN_0);
        }
        if (auto* item = PropertyGridHelper::AddLink(category, _T("Sensor Gain"), _T("%.3f"), dSensorGain))
        {
            item->SetID(IDC_PARA_SENSORGAIN_VALUE_0);
            item->SetDescription(_T("Min: 1 ~ Max: 1.999"));
        }
        if (auto* item = PropertyGridHelper::AddLink_Button(category, _T("Reset"), TRUE, FALSE))
        {
            item->SetID(IDC_PARA_CAM_RESET_0);
            item->SetDescription(_T("RESET"));
        }
        if (auto* item = PropertyGridHelper::AddLink_Button(category, _T("Response Leveling"), TRUE, FALSE))
        {
            item->SetID(IDC_PARA_CAM_RESPONSE_LEVELING_0);
            item->SetDescription(_T("Sensor Gain을 통한 Align 조정"));
        }
#ifdef _USE_CIS_LIVE_
        if (auto* item = category->AddChildItem(new XTPPropertyGridItemButton(_T("Reset Live Profile"))))
        {
            item->SetID(PROPERTY_ID_RESET_LIVE_PROFILE);
            item->SetReadOnly(readonly_offlineParameter);
        }
#endif

        category->Expand();
    }
    if (auto* category = m_property.AddCategory(_T("Camera_1")))
    {
        double dBright, dDark;
        double dRowGainBright, dRowGainDark;
        int nSensorSel =0;
        double dSensorGain;
        int nTemp = 0;
        double dTemp = 0.f;

        double dGain = m_parent->m_cisComm->GetGrabExcutor(1)->GetSystemGain();
        m_parent->m_cisComm->GetGrabExcutor(1)->GetExposureTime(dBright, dDark);
        m_parent->m_cisComm->GetGrabExcutor(1)->GetSensorInfo(nSensorSel, dSensorGain);
        m_parent->m_cisComm->GetGrabExcutor(1)->GetRowGain(dRowGainBright, dRowGainDark);

         //노출 값과 Gain 값만 일단 조정?
        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Exposure Time Selector"), nTemp))
        {
            item->GetConstraints()->AddConstraint(_T("All"), 0);
            item->GetConstraints()->AddConstraint(_T("Row 1"), 1);
            item->GetConstraints()->AddConstraint(_T("Row 2"), 2);

            item->SetID(IDC_PARA_CAM_EXPOSURE_SELECTOR_1);
        }

        if (auto* item = PropertyGridHelper::AddLink(category, _T("Exposure Time"), _T("%.3f"), dBright))
        {
            item->SetID(IDC_PARA_CAM_EXPOSURE_TIME_1);
            item->SetDescription(_T("Min: 1.24 ~ Max: 1310.68"));
        }

        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Row Selector"), nTemp))
        {
            item->GetConstraints()->AddConstraint(_T("All Rows"), 0);
            item->GetConstraints()->AddConstraint(_T("Row 1"), 1);
            item->GetConstraints()->AddConstraint(_T("Row 2"), 2);

            item->SetID(IDC_PARA_CAM_ROW_GAIN_SELECTOR_1);
        }

        if (auto* item = PropertyGridHelper::AddLink(category, _T("Row Gain"), _T("%.3f"), dRowGainBright))
        {
            item->SetID(IDC_PARA_CAM_ROW_GAIN_1);
            item->SetDescription(_T("Min: 1.24 ~ Max: 1310.68"));
        }

        if (auto* item = PropertyGridHelper::AddLink(category, _T("System Gain"), _T("%.3f"), dGain))
        {
            item->SetID(IDC_PARA_CAM_GAIN_1);
            item->SetDescription(_T("Min: 1 ~ Max: 7.99"));
        }

        long nSelect = 0;
        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Sensor Selector"), nSensorSel))
        {
            for (long nSensor = 0; nSensor < 16; nSensor++)
            {
                CString str;
                str.Format(_T("%d"), nSensor);
                item->GetConstraints()->AddConstraint(str, nSensor);
            }
            item->SetDescription(_T("Min: 1 ~ Max: 16"));
            item->SetID(IDC_PARA_SENSORGAIN_1);
        }

        if (auto* item = PropertyGridHelper::AddLink(category, _T("Sensor Gain"), _T("%.3f"), dSensorGain))
        {
            item->SetID(IDC_PARA_SENSORGAIN_VALUE_1);
            item->SetDescription(_T("Min: 1 ~ Max: 1.999"));
        }
        if (auto* item = PropertyGridHelper::AddLink_Button(category, _T("Reset"), TRUE, FALSE))
        {
            item->SetID(IDC_PARA_CAM_RESET_1);
            item->SetDescription(_T("RESET"));
        }
        if (auto* item = PropertyGridHelper::AddLink_Button(category, _T("Response Leveling"), TRUE, FALSE))
        {
            item->SetID(IDC_PARA_CAM_RESPONSE_LEVELING_1);
            item->SetDescription(_T("Sensor Gain을 통한 Align 조정"));
        }
        //if (auto* item = PropertyGridHelper::AddLink(category, _T("Bright_Exposure Time(ms)"), _T("%.3f"), dBright))
        //{
        //    item->SetID(IDC_PARA_CAM_EXPOSURE_TIME_BRIGHT_1);
        //    item->SetDescription(_T("Min: 1.24 ~ Max: 1310.68"));
        //}
        //if (auto* item = PropertyGridHelper::AddLink(category, _T("Dark_Exposure Time(ms)"), _T("%.3f"), dDark))
        //{
        //    item->SetID(IDC_PARA_CAM_EXPOSURE_TIME_DARK_1);
        //    item->SetDescription(_T("Min: 1.24 ~ Max: 1310.68"));
        //}
        //if (auto* item = PropertyGridHelper::AddLink(category, _T("Gain"), _T("%.3f"), dGain))
        //{
        //    item->SetID(IDC_PARA_CAM_GAIN_1);
        //    item->SetDescription(_T("Min: 1 ~ Max: 7.99"));
        //    
        //}

        //long nSelect = 0;
        //if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("Sensor Selector"), nSensorSel))
        //{
        //    for (long nSensor = 0; nSensor < 16; nSensor++)
        //    {
        //        CString str;
        //        str.Format(_T("%d"), nSensor);
        //        item->GetConstraints()->AddConstraint(str, nSensor);
        //    }
        //    item->SetDescription(_T("Min: 1 ~ Max: 16"));
        //    item->SetID(IDC_PARA_SENSORGAIN_1);
        //}
        //if (auto* item = PropertyGridHelper::AddLink(category, _T("Sensor Gain"), _T("%.3f"), dSensorGain))
        //{
        //    item->SetID(IDC_PARA_SENSORGAIN_VALUE_1);
        //    item->SetDescription(_T("Min: 1 ~ Max: 1.999"));
        //}
        category->Expand();



#ifdef _USE_CIS_LIVE_
        if (auto* item = category->AddChildItem(new XTPPropertyGridItemButton(_T("Reset Live Profile"))))
        {
            item->SetID(PROPERTY_ID_RESET_LIVE_PROFILE);
            item->SetReadOnly(readonly_offlineParameter);
        }
#endif

        
    }
    
    if (auto* category = m_property.AddCategory(_T("Trigger")))
    {
        long nValue = 0;
        if (auto* item = PropertyGridHelper::AddLink_Button(category, _T("OpenPort"), TRUE, FALSE))
        {
            item->SetID(IDC_BUTTON_OPENPORT);

        }
        
        if (auto* item = PropertyGridHelper::AddLink_Enum(category, _T("COMPortNum"), nValue))
        {
            for (long nPortNum = 0; nPortNum < m_vecPortName.size(); nPortNum++)
            {
                item->GetConstraints()->AddConstraint(m_vecPortName[nPortNum], nPortNum);
            }
           
            item->SetID(IDC_PARA_PORTNUM);
        }

        if (auto* item = PropertyGridHelper::AddLink_Button(category, _T("Read Data ALL"), TRUE, FALSE))
        {
            item->SetID(IDC_BUTTON_READDATA);
        }

        if (auto* item = PropertyGridHelper::AddLink_Button(category, _T("Write Data ALL"), TRUE, FALSE))
        {
            item->SetID(IDC_BUTTON_WRITEDATA);
        }
        if (auto* item = PropertyGridHelper::AddLink(category, _T("Tab Sensor Detection Min Distance(mm)"),  m_nValue))
        {
            item->SetID(IDC_PARA_TRG_TAB_SENSOR);
            item->SetReadOnly(TRUE);
            item->SetDescription(_T("0x9000 / 0x4000"));
        }
        if (auto* item = PropertyGridHelper::AddLink(category, _T("No Tab Trigger Period"), m_nValue))
        {
            item->SetID(IDC_PARA_NO_TAB);
            item->SetDescription(_T("0x9001 / 0x4001"));
        }
        if (auto* item = PropertyGridHelper::AddLink(category, _T("Encoder Pluse 1mm(PLS)"), m_nValue))
        {
            item->SetID(IDC_PARA_ENCODER_PULSE);
            item->SetReadOnly(TRUE);
            item->SetDescription(_T("0x9002(Write Only)"));
        }
        if (auto* item = PropertyGridHelper::AddLink(category, _T("V#1 CellID Pos(mm)"), m_nValue))
        {
            item->SetID(IDC_PARA_V1_CELLID_POS);
            item->SetDescription(_T("0x9010 / 0x4010"));
        }
        if (auto* item = PropertyGridHelper::AddLink(category, _T("V#1 Trg Pos(mm)"), m_nValue))
        {
            item->SetID(IDC_PARA_V1_TRG_POS);
            item->SetDescription(_T("0x9012 / 0x4012"));
        }
        if (auto* item = PropertyGridHelper::AddLink(category, _T("V#1 LTRG Cycle"), m_nValue))
        {
            item->SetID(IDC_PARA_V1_LTRG_CYCLE);
            item->SetReadOnly(TRUE);
            item->SetDescription(_T("0xA011 / 0x5011"));
        }
        if (auto* item = PropertyGridHelper::AddLink(category, _T("V#2 CellID Pos(mm)"),m_nValue))
        {
            item->SetID(IDC_PARA_V2_CELLID_POS);
            item->SetDescription(_T("0x9020 / 0x4020"));
        }
        if (auto* item = PropertyGridHelper::AddLink(category, _T("V#2 Trg Pos(mm)"), m_nValue))
        {
            item->SetID(IDC_PARA_V2_TRG_POS);
            item->SetDescription(_T("0x9022 / 0x4022"));

        }

        if (auto* item = PropertyGridHelper::AddLink(category, _T("V#2 LTRG Cycle"), m_nValue))
        {
            item->SetID(IDC_PARA_V2_LTRG_CYCLE);
            item->SetReadOnly(TRUE);
            item->SetDescription(_T("0xA021 / 0x5021"));
        }
        category->Expand();
    }

    if (auto* category = m_property.AddCategory(_T("Operation")))
    {
        if (auto* item = PropertyGridHelper::AddLink_Button(category, _T("Live OnOff"), TRUE, FALSE))
        {
            item->SetID(IDC_PARA_BUTTON);
        }
        category->Expand();
    }
}
void CDlgViewCameraSetting::OnBnClickedRadioPort0()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

#ifdef _USE_CIS_LIVE_
	m_parent->m_cisComm->GrabExcutorContainerStop();
	m_cellImageMerger->Stop();
#endif

	m_parent->m_cisComm->m_selectedPortIndex = 0;
	m_buttonPort0.EnableWindow(FALSE);
	m_buttonPort1.EnableWindow(TRUE);

	UpdateUI_Parameter();

	ResetBestFocusInformation(0);
	ResetBestFocusInformation(1);

#ifdef _USE_CIS_LIVE_
	m_parent->m_cisComm->GrabExcutorContainerStart(GrabMode::Live);
	m_cellImageMerger->Start();
#endif
}

void CDlgViewCameraSetting::OnBnClickedRadioPort1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

#ifdef _USE_CIS_LIVE_
	m_parent->m_cisComm->GrabExcutorContainerStop();
	m_cellImageMerger->Stop();
#endif

	m_parent->m_cisComm->m_selectedPortIndex = 1;
	m_buttonPort0.EnableWindow(TRUE);
	m_buttonPort1.EnableWindow(FALSE);

	UpdateUI_Parameter();

	ResetBestFocusInformation(0);
	ResetBestFocusInformation(1);
#ifdef _USE_CIS_LIVE_
	m_parent->m_cisComm->GrabExcutorContainerStart(GrabMode::Live);
	m_cellImageMerger->Start();
#endif
}

void CDlgViewCameraSetting::OnBnClickedButtonSettingApply()
{
	if (m_mode != Mode::Inline)
	{
		CISInitialize();
		return;
	}

	// Inline 모드 때는 조명값만 다운로드 시킬 수 있다

	UINT32 prevBrightness0[2] = { m_parent->m_cisComm->m_paraCur.m_nCh0[0], m_parent->m_cisComm->m_paraCur.m_nCh0[1] };
	UINT32 prevBrightness1[2] = { m_parent->m_cisComm->m_paraCur.m_nCh1[0], m_parent->m_cisComm->m_paraCur.m_nCh1[1] };
	UINT32 prevBrightness2[2] = { m_parent->m_cisComm->m_paraCur.m_nCh2[0], m_parent->m_cisComm->m_paraCur.m_nCh2[1] };

	auto& para = m_parent->m_cisComm->m_paraCur;
	para = m_parent->m_cisComm->m_paraEdit;

	auto* cisSetting = m_parent->m_cisComm->GetSetting();

	if (!cisSetting->m_bOpen)
	{
		m_loggerSequence.Log(0, _T("Can not apply lighting."));
	}
	else
	{
		for (long paraIndex = 0; paraIndex < 2; paraIndex++)
		{
			UINT32 Num1 = para.m_nCh0[paraIndex];
			UINT32 Num2 = para.m_nCh1[paraIndex];
			UINT32 Num3 = para.m_nCh2[paraIndex];

			//*need to update
			//long cameraIndex = (g_systemParameter.nCamSingleDual == 0) ? paraIndex : paraIndex * 2;
			long cameraIndex = paraIndex;

			if (!cisSetting->Camera_RedCBRegWrite(cameraIndex, 0, Num1))
			{
				m_loggerSequence.Log(0, cisSetting->m_strError);
			}

			if (!cisSetting->Camera_GreenCBRegWrite(cameraIndex, 0, Num2))
			{
				m_loggerSequence.Log(0, cisSetting->m_strError);
			}

			if (!cisSetting->Camera_BlueCBRegWrite(cameraIndex, 0, Num3))
			{
				m_loggerSequence.Log(0, cisSetting->m_strError);
			}

			if (prevBrightness0[paraIndex] != Num1)
			{
				CString strLog;
				CString strCIS;

				//* need to update
				//if (g_systemParameter.nPC_ID == 1)
				//	strCIS = _T("Dimension");
				//else
				//	strCIS = _T("Surface");
				strCIS = _T("Surface");

				CString strTopBottom = (paraIndex == 0) ? _T("Top") : _T("Back");

				//strLog.Format(_T("@%s@ $%s$ %s Brightness Changed from CH 0 %d to %d"), strCIS, g_systemParameter.m_currentModel, strTopBottom, prevBrightness0[paraIndex], Num1);
				strLog.Format(_T("@%s@ $%s$ %s Brightness Changed from CH 0 %d to %d"), strCIS, _T("Model Name"), strTopBottom, prevBrightness0[paraIndex], Num1);

				m_loggerParamChanged.Log(0, strLog);
			}

			if (prevBrightness1[paraIndex] != Num2)
			{
				CString strLog;
				CString strCIS;

				//* need to update
				//if (g_systemParameter.nPC_ID == 1)
				//	strCIS = _T("Dimension");
				//else
				//	strCIS = _T("Surface");
				strCIS = _T("Surface");

				CString strTopBottom = (paraIndex == 0) ? _T("Top") : _T("Back");

				//strLog.Format(_T("@%s@ $%s$ %s Brightness Changed from CH 1 %d to %d"), strCIS, g_systemParameter.m_currentModel, strTopBottom, prevBrightness0[paraIndex], Num1);
				strLog.Format(_T("@%s@ $%s$ %s Brightness Changed from CH 1 %d to %d"), strCIS, _T("Model Name"), strTopBottom, prevBrightness0[paraIndex], Num1);

				m_loggerParamChanged.Log(0, strLog);
			}

			if (prevBrightness2[paraIndex] != Num3)
			{
				CString strLog;
				CString strCIS;
				
				//*need to update
				//if (g_systemParameter.nPC_ID == 1)
				//	strCIS = _T("Dimension");
				//else
				//	strCIS = _T("Surface");
				strCIS = _T("Surface");

				CString strTopBottom = (paraIndex == 0) ? _T("Top") : _T("Back");

				//strLog.Format(_T("@%s@ $%s$ %s Brightness Changed from CH 2 %d to %d"), strCIS, g_systemParameter.m_currentModel, strTopBottom, prevBrightness0[paraIndex], Num1);
				strLog.Format(_T("@%s@ $%s$ %s Brightness Changed from CH 2 %d to %d"), strCIS, _T("Model Name"), strTopBottom, prevBrightness0[paraIndex], Num1);

				m_loggerParamChanged.Log(0, strLog);
			}
		}
	}

	//*need to update
	//m_parent->SaveRecipe();
}

void CDlgViewCameraSetting::CallbackOnLiveFrameUpdated(void* callbackData, int cameraIndex, UINT horizontalSize, UINT verticalSize, DWORD grabCount)
{
	((CDlgViewCameraSetting*)callbackData)->CallbackOnLiveFrameUpdated(cameraIndex, horizontalSize, verticalSize, grabCount);
}

void CDlgViewCameraSetting::CallbackOnLiveFrameUpdated(int cameraIndex, UINT horizontalSize, UINT verticalSize, DWORD grabCount)
{
	const auto frameIndex = grabCount - 1;

	const auto pImage = m_parent->m_cisComm->GetGrabExcutor(cameraIndex)->GetFrameAddress(frameIndex);

	IPVM::Image_8u_C1 image(horizontalSize, horizontalSize, verticalSize, pImage);
	m_csImage.Lock();
	m_liveImage.Create(image.GetSizeX(), image.GetSizeY());
	IPVM::ImageProcessing::Copy(image, IPVM::Rect(image), m_liveImage);
	m_csImage.Unlock();

	m_parent->m_cisComm->GetGrabExcutor(cameraIndex)->ProceedHeadFrameIndex(1);
}

LRESULT CDlgViewCameraSetting::OnGridNotify(WPARAM wparam, LPARAM lparam)
{
	auto portNumber = m_parent->m_cisComm->m_selectedPortIndex;
    if (wparam == XTP_PGN_INPLACEBUTTONDOWN)
    {
        auto index = ((CXTPPropertyGridInplaceButton*)lparam)->GetID();

        //switch (index)
        //{
        //    case IDC_BUTTON_NO_TAB_WRITE:
        //        break;
        //}
    }

	if (wparam != XTP_PGN_CUSTOM_BUTTON_CLICK)
	{
        CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lparam;
        if (pItem == NULL)
        {
            return 0;
        }
        auto index = pItem->GetID();

		//auto nIndex = ((CXTPPropertyGridItem*)lparam)->GetID();
  //      CXTPPropertyGridItem* pItemParent = (CXTPPropertyGridItem*)pItem->GetParentItem();
  //      UINT indexParent = 0;
  //      if (pItemParent != NULL)
  //      {
  //          indexParent = pItemParent->GetID();
  //          ;
  //      }
		switch (index)
		{
		case PROPERTY_ID_RESET_LIVE_PROFILE:
			ResetBestFocusInformation(0);
			ResetBestFocusInformation(1);
			break;
        case IDC_PARA_BUTTON:
            OnLiveCamStart();
            break;
        case IDC_PARA_PORTNUM:
            GetCOMPortNum();
            break;
        case IDC_BUTTON_OPENPORT:
            OnBnClickedButtonPortopen();
            //OpenRS232CPort(m_strCOMPort, m_strBaurdRate);
            //::AfxBeginThread(ThreadRS232Proc, this);
            break;
        case IDC_BUTTON_READDATA:
            OnBnClickedButtonTriggerRead();
            break;
        case IDC_BUTTON_WRITEDATA:
            OnBnClickedButtonTriggerWrite();
            break;
        case IDC_PARA_CAM_RESET_0:
            Reset(0);
            break;
        case IDC_PARA_CAM_RESET_1:
            Reset(1);
            break;
		}
	}

	if (wparam == XTP_PGN_ITEMVALUE_CHANGED)
	{
		auto index = ((CXTPPropertyGridItem*)lparam)->GetID();

		switch (index)
		{
#ifndef _USE_CIS_LIVE_
		case PROPERTY_ID_TRIGGER_INPUT:
			if (m_parent->m_cisComm->m_paraEdit.m_nTriggerInput[portNumber] == 1)
			{
				//*need to update
				//if (g_systemParameter.nInspectionMode != InspectionMode::Grabonly)
				//{
				//	if (MessageBox(_T("Do you want to change only grab mode?"), _T(""), MB_YESNO | MB_ICONQUESTION) == IDYES)
				//		g_systemParameter.nInspectionMode = InspectionMode::Grabonly;
				//}
			}
			break;
#endif
		case PROPERTY_ID_THRESHOLD1:
		case PROPERTY_ID_THRESHOLD2:
		case PROPERTY_ID_THRESHOLD3:
			UpdateHexChannelValue();
			break;

		case PROPERTY_ID_THRESHOLD1_HEX:
		case PROPERTY_ID_THRESHOLD2_HEX:
		case PROPERTY_ID_THRESHOLD3_HEX:
			UpdateChannelValue();
            break;
        case IDC_PARA_CAM_EXPOSURE_TIME_BRIGHT_0:
            OnChangeExposureTime(0, 1, IDC_PARA_CAM_EXPOSURE_TIME_BRIGHT_0);
            break;
        case IDC_PARA_CAM_EXPOSURE_TIME_BRIGHT_1:
            OnChangeExposureTime(1, 1, IDC_PARA_CAM_EXPOSURE_TIME_BRIGHT_1);
            break;
        case IDC_PARA_CAM_EXPOSURE_TIME_DARK_0:
            OnChangeExposureTime(0, 2, IDC_PARA_CAM_EXPOSURE_TIME_DARK_0);
            break;
        case IDC_PARA_CAM_EXPOSURE_TIME_DARK_1:
            OnChangeExposureTime(1, 2, IDC_PARA_CAM_EXPOSURE_TIME_DARK_1);
            break;
        case IDC_PARA_CAM_GAIN_0:
            OnChangeGain(0, IDC_PARA_CAM_GAIN_0);
            break;
        case IDC_PARA_CAM_GAIN_1:
            OnChangeGain(1, IDC_PARA_CAM_GAIN_1);
            break;

        case IDC_PARA_SENSORGAIN_0:
            OnChangeSensorGain(0, IDC_PARA_SENSORGAIN_0);
            break;
        case IDC_PARA_SENSORGAIN_VALUE_0:
            OnChangeSensorGainValue(0, IDC_PARA_SENSORGAIN_VALUE_0);
            break;
        case IDC_PARA_CAM_EXPOSURE_SELECTOR_0: 
            OnChangeExposureSelector(0, IDC_PARA_CAM_EXPOSURE_SELECTOR_0);
            break;
        case IDC_PARA_CAM_EXPOSURE_TIME_0:
            OnChangeExposureTime(0,IDC_PARA_CAM_EXPOSURE_TIME_0);
            break;
        case IDC_PARA_CAM_ROW_GAIN_SELECTOR_0:
            OnChangeExposureSelector(0, IDC_PARA_CAM_ROW_GAIN_SELECTOR_0);
            break;
        case IDC_PARA_CAM_ROW_GAIN_0:
            OnChangeRowGain(0, IDC_PARA_CAM_ROW_GAIN_0);
            break;
        case IDC_PARA_CAM_RESPONSE_LEVELING_0:
            ResponseLeveling(0);
            break;

        case IDC_PARA_SENSORGAIN_1:
            OnChangeSensorGain(1, IDC_PARA_SENSORGAIN_1);
            break;
        case IDC_PARA_SENSORGAIN_VALUE_1:
            OnChangeSensorGainValue(1, IDC_PARA_SENSORGAIN_VALUE_1);
            break;
        case IDC_PARA_CAM_EXPOSURE_SELECTOR_1:
            OnChangeExposureSelector(1, IDC_PARA_CAM_EXPOSURE_SELECTOR_1);
            break;
        case IDC_PARA_CAM_EXPOSURE_TIME_1:
            OnChangeExposureTime(1, IDC_PARA_CAM_EXPOSURE_TIME_1);
            break;
        case IDC_PARA_CAM_ROW_GAIN_SELECTOR_1:
            OnChangeExposureSelector(1, IDC_PARA_CAM_ROW_GAIN_SELECTOR_1);
            break;
        case IDC_PARA_CAM_ROW_GAIN_1:
            OnChangeRowGain(1, IDC_PARA_CAM_ROW_GAIN_1);
            break;
        case IDC_PARA_CAM_RESPONSE_LEVELING_1:
            ResponseLeveling(1);
            break;

		}
	}

	return 0;
}

void CDlgViewCameraSetting::OnTimer(UINT_PTR nIDEvent)
{
	auto portNumber = m_parent->m_cisComm->m_selectedPortIndex;

	IPVM::Image_8u_C1 liveImage;
	m_csImage.Lock();
	if (m_liveImage.GetSizeX() > 0)
	{
		liveImage = m_liveImage;
		m_liveImage = IPVM::Image_8u_C1();
	}
	m_csImage.Unlock();
	m_liveView->SetImage(liveImage);

	if (liveImage.GetSizeX() == 0)
	{
		CDialogEx::OnTimer(nIDEvent);

		return;
	}

	IPVM::Image_8u_C1 subImage[2];
	//*need to update
	//if (!Utility::CreateBrightDarkImage(liveImage, subImage[0], subImage[1], false))
	//{
	//	CDialogEx::OnTimer(nIDEvent);
	//	return;
	//}

	m_profileView[0]->Clear();
	m_profileView[1]->Clear();

	m_liveView->ROIGet(_T("Mean ROI"), m_meanRoi);
	m_liveView->ROIGet(_T("Profile ROI"), m_profileRoi);
	m_liveView->WindowOverlayClear();

	if (m_profileRoi.IsRectEmpty())
	{
		m_profileRoi = IPVM::Rect(liveImage);
		m_profileRoi.m_left = 70;

		m_liveView->ROISet(_T("Profile ROI"), _T("Profile ROI"), m_profileRoi, RGB(0, 0, 255), 120);
	}

	// Dark, Bright로 나뉘므로 Roi를 절반으로 줄이자..
	auto meanRoi = m_meanRoi;
	auto profileRoi = m_profileRoi;

	meanRoi.m_top /= 2;
	meanRoi.m_bottom /= 2;
	profileRoi.m_top /= 2;
	profileRoi.m_bottom /= 2;
	profileRoi = profileRoi & IPVM::Rect(subImage[0]);

	if ((meanRoi & IPVM::Rect(subImage[0])) == meanRoi)
	{
		double mean_bright = 0., mean_dark = 0.;
		IPVM::ImageProcessing::GetMean(subImage[0], meanRoi, mean_bright);
		IPVM::ImageProcessing::GetMean(subImage[1], meanRoi, mean_dark);
		CString text;
		text.Format(_T("Mean:%.1f, %.1f"), mean_bright, mean_dark);
		m_liveView->WindowOverlayAdd(IPVM::Point_32f_C2(10.f, 10.f), text, RGB(0, 255, 0), 16);
	}

	for (long type = 0; type < 2; type++)
	{
		auto& bestFocusProfile = m_bestFocus[portNumber][type].m_profile;
		long UpdateUI_ParameternoNeedX = 3;	// 이미지 정보가 아닌 Splitter가 주는 정보는 버리고 계산하자
		if (!profileRoi.IsRectEmpty() && profileRoi.Width() > 5)
		{
			// 맨 마지막 줄만 이용하여 갱신한다
			std::vector<IPVM::Point_32f_C2> points(profileRoi.Width());
			for (long x = profileRoi.m_left; x < profileRoi.m_right; x++)
			{
				points[x - profileRoi.m_left].m_x = (float)x;
			}

			auto* liveY = subImage[type].GetMem(0, profileRoi.m_bottom - 1);

			for (long x = profileRoi.m_left; x < profileRoi.m_right; x++)
			{
				points[x - profileRoi.m_left].m_y = (float)liveY[x];
			}

			float scorePos = 0.;
			double score = -(DBL_MAX);
			for (long x = profileRoi.m_left + 2; x < profileRoi.m_right - 2; x++)
			{
				double value = fabs(
					+liveY[x + 2] * 4.
					+ liveY[x + 1] * 2.
					- liveY[x - 1] * 2.
					- liveY[x - 2] * 4.);

				if (score < value)
				{
					scorePos = (float)x;
					score = max(score, value);
				}
			}

			if (m_bestFocus[portNumber][type].m_profileScore < score || bestFocusProfile.size() == 0)
			{
				bestFocusProfile = points;
				m_bestFocus[portNumber][type].m_pos = scorePos;
				m_bestFocus[portNumber][type].m_profileScore = score;
			}

			IPVM::Point_32f_C2 position[2];
			position[0].m_x = m_bestFocus[portNumber][type].m_pos;
			position[0].m_y = 0.f;
			position[1].m_x = m_bestFocus[portNumber][type].m_pos;
			position[1].m_y = 255.f;

			m_profileView[type]->SetData(0, &bestFocusProfile[0], (long)bestFocusProfile.size(), NOISE_VALUE_32F, RGB(0, 255, 0), RGB(255, 255, 255));
			m_profileView[type]->SetData(1, &points[0], (long)points.size(), NOISE_VALUE_32F, RGB(255, 0, 0), RGB(255, 255, 255));
			m_profileView[type]->SetData(2, &position[0], 2, NOISE_VALUE_32F, RGB(0, 0, 255), RGB(255, 255, 255));
			m_profileView[type]->ShowCursor_PositionValue();

			CString text;
			text.Format(_T("%s - Focus Score:%.1f (Max: %.1f)"), (type == 0 ? _T("Bright") : _T("Dark")), score, m_bestFocus[portNumber][type].m_profileScore);
			m_liveView->WindowOverlayAdd(IPVM::Point_32f_C2(10.f, 30.f + type * 20.f), text, RGB(0, 255, 0), 16);
		}
	}

	m_liveView->WindowOverlayShow();

	CDialogEx::OnTimer(nIDEvent);
}

void CDlgViewCameraSetting::OnBnClickedButtonPortopen()
{
    CString strPort;
    strPort = _T("COM2");
    strPort.Format(_T("COM%d"), m_strCOMPort);
    CString strBaurdRate;
    strBaurdRate.Format(_T("%d"), m_strBaurdRate);
    OpenRS232CPort(m_strCOMPort, m_strBaurdRate);
    Sleep(1);
    ::AfxBeginThread(ThreadRS232Proc, this);
}

BOOL CDlgViewCameraSetting::ReadData(long nAdd)
{
    BYTE byTxMsg[8];

    // Write시 Header 'AAAA'
    byTxMsg[0] = byTxMsg[1] = 0x55;

    byTxMsg[4] = 0;
    byTxMsg[5] = 0;
    byTxMsg[6] = 0;
    byTxMsg[7] = 0;

    switch (nAdd)
    {
        case _TB02_SensorMinDistance:
            byTxMsg[2] = 0x40;
            byTxMsg[3] = 0x00;
            break; 
        case _TB03_VirtualTrigger:
            byTxMsg[2] = 0x40;
            byTxMsg[3] = 0x01;
            break; //20230602 [jjh]
        case _TB_EncorderPulsePer1mm:
            byTxMsg[2] = 0x40;
            byTxMsg[3] = 0x02;
            break; //20230602 [jjh] start
        case _TB_V1_CellIDPos:
            byTxMsg[2] = 0x40;
            byTxMsg[3] = 0x12;
            break; //20230602 [jjh] start
        case _TB04_V1_Trigger:
            byTxMsg[2] = 0x40;
            byTxMsg[3] = 0x10;
            break;
        case _TB09_V2_CellID: 
            byTxMsg[2] = 0x40;
            byTxMsg[3] = 0x22;
            break;
        case _TB08_V2_Trigger: 
            byTxMsg[2] = 0x40;
            byTxMsg[3] = 0x20;
            break;
        case _TB15_Line_TriggerRatio:
            byTxMsg[2] = 0x50;
            byTxMsg[3] = 0x11;
            break;
        case _TB_V2_LTRGCycle: 
            byTxMsg[2] = 0x50;
            byTxMsg[3] = 0x21;
            break; //20230602 [jjh] start


       

       
            //20231031 [jjh] 800 버전부터 적용 end

        default:
            // 여기에 MessageBox 띄어도 돼나?
            CString strError;
            strError.Format(_T("트리거보드 enum %d 명령어가 정의되어 있지 않음."), nAdd);
            //SetMessageBox(strError); //20231026 [jjh] delete
            // m_pDlgInterface->SetEventMessage(strError, 5000); //20231018 [jjh]
            return FALSE;
            break;
    }

    BYTE bySendMsg[8];
    for (int i = 0; i < 8; i++)
        bySendMsg[i] = byTxMsg[i];

    m_stMSGMAP.push(bySendMsg);
    m_eventWriteData.SetEvent();

    return TRUE;
}

BOOL CDlgViewCameraSetting::WriteData(long nAdd, unsigned int nData)
{
    BYTE byTxMsg[8];

    // Write시 Header 'AAAA'
    byTxMsg[0] = byTxMsg[1] = 0xAA;

    union
    {
        unsigned int a;
        unsigned char b[4];
    } Data;

    Data.a = nData;

    byTxMsg[4] = Data.b[3];
    byTxMsg[5] = Data.b[2];
    byTxMsg[6] = Data.b[1];
    byTxMsg[7] = Data.b[0];

    switch (nAdd)
    {
        case _TB02_SensorMinDistance: // o
            byTxMsg[2] = 0x90;
            byTxMsg[3] = 0x00;
            break;
        case _TB03_VirtualTrigger: // o
            byTxMsg[2] = 0x90;
            byTxMsg[3] = 0x01;
            break;
        case _TB01_EncoderPulse: // o(write only)
            byTxMsg[2] = 0x90;
            byTxMsg[3] = 0x02;
            break;
        case _TB04_V1_Trigger: // o
            byTxMsg[2] = 0x90;
            byTxMsg[3] = 0x10;
            break;
        case _TB05_V1_CellID: // o
            byTxMsg[2] = 0x90;
            byTxMsg[3] = 0x12;
            break;
        case _TB08_V2_Trigger: // o
            byTxMsg[2] = 0x90;
            byTxMsg[3] = 0x20;
            break;
        case _TB09_V2_CellID: // o
            byTxMsg[2] = 0x90;
            byTxMsg[3] = 0x22;
            break;
        case _TB15_Line_TriggerRatio: // o
            byTxMsg[2] = 0xA0;
            byTxMsg[3] = 0x11;
            break;
        case _TB_Line_TriggerOnTime_2: // o
            byTxMsg[2] = 0xA0;
            byTxMsg[3] = 0x21;
            break;
        case _TB79_EEPROM_WRITE_0_1_0: // o
            byTxMsg[2] = 0xEE;
            byTxMsg[3] = 0xEE;
            break;
        default:
            // 여기에 MessageBox 띄어도 돼나?
            return FALSE;
            break;
    }

    BYTE bySendMsg[8];
    for (int i = 0; i < 8; i++)
        bySendMsg[i] = byTxMsg[i];

    m_stMSGMAP.push(bySendMsg);
    m_eventWriteData.SetEvent();

    return TRUE;
}

BOOL CDlgViewCameraSetting::OpenRS232CPort(CString strComPort, CString strBaudrate)
{
    if (m_brs232cState)
    {
        return TRUE;
    }

    //5LINE_POUCH RS-232C : 115200bps, DATA-8BIT, STOP-1BIT, PARITY-NOPARITY
    m_rs232c = new CMycomm(_T("\\\\.\\") + strComPort, strBaudrate, _T("None"), _T("8 Bit"), _T("1 Bit"));
    if (m_rs232c->Create(this->m_hWnd) != 0)
    {
        m_brs232cState = TRUE;

        ReadData(_TB37_Etc_Version);
        m_pButtonOpenPort->SetBackColor(RGB(0, 255, 0));
        return TRUE;
    }
    MessageBox(_T("Not Connected.."));
    delete m_rs232c;
    m_rs232c = nullptr;

    return FALSE;
}

BOOL CDlgViewCameraSetting::SendDataRS232C(LPCTSTR outbuf, int len)
{
    //int nPort = m_cboPortNo.GetCurSel();
    //int nBaudRate = m_cboBaudRate.GetCurSel();

    //if (IsOpenrs232cPort() == FALSE)
    //{
    //    if (OpenRS232CPort(enum_COMPort[nPort], enum_BaudRate[nBaudRate]) == FALSE)
    //        return FALSE;
    //}

    //if (TRUE == IsOpenrs232cPort())
    {
        //m_bWriteComplete = FALSE;

        BOOL bRet = m_rs232c->Send((LPCTSTR)outbuf, len);

        Sleep(10);

        return bRet;
    }
    return FALSE;
}

BOOL CDlgViewCameraSetting::ReciveDataProcess(BYTE* byRecValue, int nLen)
{
    if (0x55 != byRecValue[0] || 0x55 != byRecValue[1])
        return FALSE;

    int nValue = (byRecValue[5] << 16) | (byRecValue[6] << 8) | byRecValue[7];

    if (0xFF == byRecValue[2] && 0xFF == byRecValue[3])
    {
        //버전
        CString strVersion;
        strVersion.Format(_T("%d"), nValue);
        //SetDlgItemText(IDC_STATIC_VERSION, strVersion);
        //m_strTriggerVer = strVersion.Right(3); //20220919 [jjh]
        //m_pDlgInterface->m_strTriggerVersion = strVersion; //20230129 [jjh]
        return TRUE;
    }

    SetReadData(byRecValue[2], byRecValue[3], nValue);

    return TRUE;
}
void CDlgViewCameraSetting::SetReadData(BYTE by2, BYTE by3, int value)
{
    int nPreReadValue = -1; //20230602 [jjh]

    unsigned int nEncoderPulse = 806; //20230602 [jjh] start

    if (m_triggerParameter->m_nArrData[_TB01_EncoderPulse] > 1)
    {
        nEncoderPulse = m_triggerParameter->m_nArrData[_TB01_EncoderPulse]; //20230602 [jjh] end
    }

    if (0x40 == by2 && 0x00 == by3)
    {
        if (m_nCommon_VirtualTrigger != -1)
            nPreReadValue = m_nCommon_VirtualTrigger;
        m_nCommon_VirtualTrigger = value;
        CString strVal;
        strVal.Format(_T("%d"), m_nCommon_VirtualTrigger);
        //SetDlgItemText(IDC_EDIT_SETTING_2, strVal);
        if (auto* item = m_property.FindItem(IDC_PARA_TRG_TAB_SENSOR))
        {
            item->SetValue(strVal);
        }
        //AddSpecLogReadTriggerBoard(enum_ParaDesc[_TB03_VirtualTrigger], nPreReadValue, m_nTB03_VirtualTrigger, m_triggerParameter->m_nArrData[_TB03_VirtualTrigger]);
    }

    if (0x40 == by2 && 0x01 == by3)
    {
        if (m_nCommon_VirtualTrigger != -1)
            nPreReadValue = m_nCommon_VirtualTrigger;
        m_nCommon_VirtualTrigger = value / nEncoderPulse /*//20230602 [jjh]*/;
        CString strVal;
        strVal.Format(_T("%d"), m_nCommon_VirtualTrigger);
        SetDlgItemText(IDC_EDIT_SETTING_3, strVal);
        if (auto* item = m_property.FindItem(IDC_PARA_NO_TAB))
        {
            item->SetValue(strVal);
        }
        //AddSpecLogReadTriggerBoard(enum_ParaDesc[_TB03_VirtualTrigger], nPreReadValue, m_nTB03_VirtualTrigger, m_triggerParameter->m_nArrData[_TB03_VirtualTrigger]);
    }

    if (0x40 == by2 && 0x02 == by3)
    {
        if (m_nCommon_VirtualTrigger != -1)
            nPreReadValue = m_nCommon_VirtualTrigger;
        m_nCommon_VirtualTrigger = value;
        CString strVal;
        strVal.Format(_T("%d"), m_nCommon_VirtualTrigger);
        if (auto* item = m_property.FindItem(IDC_PARA_ENCODER_PULSE))
        {
            item->SetValue(strVal);
        }
        //AddSpecLogReadTriggerBoard(enum_ParaDesc[_TB03_VirtualTrigger], nPreReadValue, m_nTB03_VirtualTrigger, m_triggerParameter->m_nArrData[_TB03_VirtualTrigger]);
    }

    if (0x40 == by2 && 0x12 == by3)
    {
        if (m_nCommon_VirtualTrigger != -1)
            nPreReadValue = m_nCommon_VirtualTrigger;
        m_nCommon_VirtualTrigger = value / nEncoderPulse /*//20230602 [jjh]*/;
        CString strVal;
        strVal.Format(_T("%d"), m_nCommon_VirtualTrigger);
        if (auto* item = m_property.FindItem(IDC_PARA_V1_CELLID_POS))
        {
            item->SetValue(strVal);
        }
        //AddSpecLogReadTriggerBoard(enum_ParaDesc[_TB03_VirtualTrigger], nPreReadValue, m_nTB03_VirtualTrigger, m_triggerParameter->m_nArrData[_TB03_VirtualTrigger]);
    }

    if (0x40 == by2 && 0x10 == by3)
    {
        if (m_nCommon_VirtualTrigger != -1)
            nPreReadValue = m_nCommon_VirtualTrigger;
        m_nCommon_VirtualTrigger = value / nEncoderPulse /*//20230602 [jjh]*/;
        CString strVal;
        strVal.Format(_T("%d"), m_nCommon_VirtualTrigger);
        if (auto* item = m_property.FindItem(IDC_PARA_V1_TRG_POS))
        {
            item->SetValue(strVal);
        }
        //AddSpecLogReadTriggerBoard(enum_ParaDesc[_TB03_VirtualTrigger], nPreReadValue, m_nTB03_VirtualTrigger, m_triggerParameter->m_nArrData[_TB03_VirtualTrigger]);
    }

    if (0x40 == by2 && 0x22 == by3)
    {
        if (m_nCommon_VirtualTrigger != -1)
            nPreReadValue = m_nCommon_VirtualTrigger;
        m_nCommon_VirtualTrigger = value / nEncoderPulse /*//20230602 [jjh]*/;
        CString strVal;
        strVal.Format(_T("%d"), m_nCommon_VirtualTrigger);
        if (auto* item = m_property.FindItem(IDC_PARA_V2_CELLID_POS))
        {
            item->SetValue(strVal);
        }
        //AddSpecLogReadTriggerBoard(enum_ParaDesc[_TB03_VirtualTrigger], nPreReadValue, m_nTB03_VirtualTrigger, m_triggerParameter->m_nArrData[_TB03_VirtualTrigger]);
    }

    if (0x40 == by2 && 0x20 == by3)
    {
        if (m_nCommon_VirtualTrigger != -1)
            nPreReadValue = m_nCommon_VirtualTrigger;
        m_nCommon_VirtualTrigger = value / nEncoderPulse /*//20230602 [jjh]*/;
        CString strVal;
        strVal.Format(_T("%d"), m_nCommon_VirtualTrigger);
        if (auto* item = m_property.FindItem(IDC_PARA_V2_TRG_POS))
        {
            item->SetValue(strVal);
        }
        //AddSpecLogReadTriggerBoard(enum_ParaDesc[_TB03_VirtualTrigger], nPreReadValue, m_nTB03_VirtualTrigger, m_triggerParameter->m_nArrData[_TB03_VirtualTrigger]);
    }

    if (0x50 == by2 && 0x11 == by3)
    {
        if (m_nCommon_VirtualTrigger != -1)
            nPreReadValue = m_nCommon_VirtualTrigger;
        m_nCommon_VirtualTrigger = value;
        CString strVal;
        strVal.Format(_T("%d"), m_nCommon_VirtualTrigger);
        if (auto* item = m_property.FindItem(IDC_PARA_V1_LTRG_CYCLE))
        {
            item->SetValue(strVal);
        }
        //AddSpecLogReadTriggerBoard(enum_ParaDesc[_TB03_VirtualTrigger], nPreReadValue, m_nTB03_VirtualTrigger, m_triggerParameter->m_nArrData[_TB03_VirtualTrigger]);
    }

    if (0x50 == by2 && 0x21 == by3)
    {
        if (m_nCommon_VirtualTrigger != -1)
            nPreReadValue = m_nCommon_VirtualTrigger;
        m_nCommon_VirtualTrigger = value;
        CString strVal;
        strVal.Format(_T("%d"), m_nCommon_VirtualTrigger);
        if (auto* item = m_property.FindItem(IDC_PARA_V2_LTRG_CYCLE))
        {
            item->SetValue(strVal);
        }
        //AddSpecLogReadTriggerBoard(enum_ParaDesc[_TB03_VirtualTrigger], nPreReadValue, m_nTB03_VirtualTrigger, m_triggerParameter->m_nArrData[_TB03_VirtualTrigger]);
    }
}

void CDlgViewCameraSetting::OnBnClickedButtonTriggerRead()
{    
    Sleep(1);
    ReadData(_TB02_SensorMinDistance); //4000
    Sleep(1);
    ReadData(_TB03_VirtualTrigger);     // 4001
    Sleep(1);
    ReadData(_TB_EncorderPulsePer1mm); // 4002
    Sleep(1);
    ReadData(_TB_V1_CellIDPos);         // 4012
    Sleep(1);
    ReadData(_TB04_V1_Trigger);         // 4010
    Sleep(1);
    ReadData(_TB09_V2_CellID);          //4022 
    Sleep(1);
    ReadData(_TB08_V2_Trigger);         // 4020  
    Sleep(1);
    ReadData(_TB15_Line_TriggerRatio); // 5011
    Sleep(1);
    ReadData(_TB_V2_LTRGCycle);         // 5021
    Sleep(1);

}

LRESULT CDlgViewCameraSetting::OnReceive(WPARAM length, LPARAM lpara)
{
    BYTE byRecValue[8] = { 0x00, }; //받는거...

    if (length != 8)
        return 1;

    char data[8];

    if (m_rs232c)
    {
        m_rs232c->Receive(data, length);

        if (8 == length)
        {
            memcpy(m_byRecive, data, length);
            m_bRecive = TRUE;
        }

        return 1;
    }
    return 0;
}

void CDlgViewCameraSetting::OnBnClickedButtonTriggerWrite()
{
    unsigned int nWriteData = 0;

    nWriteData = GetDlgItemInt(IDC_EDIT_SETTING_2); //0x9000
    WriteData(_TB02_SensorMinDistance, nWriteData);

    nWriteData = GetDlgItemInt(IDC_EDIT_SETTING_3); //0x9001 
    WriteData(_TB03_VirtualTrigger, nWriteData);

    long index=m_parent->m_cisComm->GetCameraCount();
    for (size_t i = 0; i < m_parent->m_cisComm->GetCameraCount(); i++)
    {
		m_parent->m_cisComm->GetGrabExcutor(i)->SetVerticalLength(nWriteData);
    }
    
    nWriteData = GetDlgItemInt(IDC_EDIT_SETTING_4); //0x9002
    WriteData(_TB01_EncoderPulse, nWriteData);

    nWriteData = GetDlgItemInt(IDC_EDIT_SETTING_5); //0x9010
    WriteData(_TB04_V1_Trigger, nWriteData);

    nWriteData = GetDlgItemInt(IDC_EDIT_SETTING_6); //0x9012
    WriteData(_TB05_V1_CellID, nWriteData);

    nWriteData = GetDlgItemInt(IDC_EDIT_SETTING_7); //0x9020
    WriteData(_TB08_V2_Trigger, nWriteData);

    nWriteData = GetDlgItemInt(IDC_EDIT_SETTING_8); //0x9022
    WriteData(_TB09_V2_CellID, nWriteData);

    nWriteData = GetDlgItemInt(IDC_EDIT_SETTING_9); //0xA011
    WriteData(_TB15_Line_TriggerRatio, nWriteData);

    nWriteData = GetDlgItemInt(IDC_EDIT_SETTING_10); //0xA021 h추가 
    WriteData(_TB_Line_TriggerOnTime_2, nWriteData); 

    EEPROMWrite();

}
void CDlgViewCameraSetting::EEPROMWrite()
{
    Sleep(1);
    WriteData(_TB79_EEPROM_WRITE_0_1_0, 0);
    Sleep(1);
    WriteData(_TB79_EEPROM_WRITE_0_1_0, 1);
    Sleep(1);
    WriteData(_TB79_EEPROM_WRITE_0_1_0, 0);
}

UINT CDlgViewCameraSetting::ThreadRS232Proc(LPVOID pParam)
{
    CDlgViewCameraSetting* pThis = (CDlgViewCameraSetting*)pParam;
    pThis->ThreadRS232Data();
    return 0;
}

void CDlgViewCameraSetting::ThreadRS232Data()
{
    BOOL bRetry = FALSE;

    clock_t start, end;

    HANDLE waitEvents[] = {m_eventTerminate, m_eventWriteData};

    while (WaitForSingleObject(m_eventTerminate, 20) == WAIT_TIMEOUT)
    {
        
        if (m_nStep == 0)
        {
            auto selectedEvent = WaitForMultipleObjects(2, waitEvents, FALSE, INFINITE);

            if (selectedEvent == WAIT_OBJECT_0)
                break; // Terminate 이벤트
            m_nStep = 1;
            m_eventWriteData.ResetEvent();
            continue;
        }

        switch (m_nStep)
        {
            case 1:
            { //send
                if (FALSE == bRetry && FALSE == m_stMSGMAP.pop(*m_bySend))
                {
                    m_nStep = 0; //사실 여기 안들어옴!!
                    break;
                }

                if (TRUE == bRetry)
                {
                    SendDataRS232C((LPCTSTR)m_bySend, 8);
                }

                m_bRecive = FALSE;
                SendDataRS232C((LPCTSTR)m_bySend, 8);

                start = clock();

                CString strText;
                strText.Format(_T("SEND RS232 - [%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x]"), m_bySend[0], m_bySend[1], m_bySend[2], m_bySend[3], m_bySend[4], m_bySend[5],
                    m_bySend[6], m_bySend[7]);
                //g_LogEtc.Log(0, strText);
                m_nStep = 2;
            }
            break;
            case 2:
            { //recive check
                if (TRUE == m_bRecive)
                {
                    if (TRUE == ReciveDataProcess(m_byRecive, 8))
                    {
                    }
                    else
                    {
                        if (0 == memcmp(m_bySend, m_byRecive, 8))
                        {
                        }
                        else
                        {
                            if (FALSE == bRetry)
                            {
                                m_nStep = 1;
                                bRetry = TRUE;
                                break;
                            }
                            else
                            {
                                //error
                                CString strText;
                                strText.Format(_T("Message Fail [%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x]"), m_bySend[0], m_bySend[1],
                                    m_bySend[2], m_bySend[3], m_bySend[4], m_bySend[5], m_bySend[6], m_bySend[7]);
                                AfxMessageBox(_T("strText"));
                                //g_LogEtc.Log(0, strText);
                                //SetMessageBox(strText); //20230602 [jjh] delete
                                //m_pDlgInterface->SetEventMessage(strText, 5000); //20230602 [jjh]
                            }
                        }
                    }

                    bRetry = FALSE;
                    if (0 < m_stMSGMAP.GetSize())
                    {
                        m_nStep = 1;
                    }
                    else
                    {
                        m_nStep = 0;
                    }
                }
                else
                {
                    end = clock();

                    if (2000 < end - start)
                    {
                        m_stMSGMAP.Clear();
                        m_nStep = 0;

                        //g_LogEtc.Log(0, g_LanguageTranslation.m_vecstrDlgMessage[8]); //20230602 [jjh]

                        //SetMessageBox(g_LanguageTranslation.m_vecstrDlgMessage[8]); //20230602 [jjh] delete
                        //m_pDlgInterface->SetEventMessage(g_LanguageTranslation.m_vecstrDlgMessage[8], 5000); //20230602 [jjh]
                    }
                }
            }
            break;
            default:
                break;
        }

        m_eventWriteData.ResetEvent();
    }
}

void CDlgViewCameraSetting::TriggerPeriodSet(long nValue)
{
    long nPeriod = nValue / 28.5;

    CString strText;
    strText.Format(_T("%d"), nPeriod);
    SetDlgItemText(IDC_EDIT_SETTING_3, strText);

	OnBnClickedButtonTriggerWrite();
}

void CDlgViewCameraSetting::OnBnClickedButtonLiveOnOff()
{
    
}




void CDlgViewCameraSetting::OnBnClickedButtonCamSetting()
{

}

void CDlgViewCameraSetting::OnChangeSetCameraPara(long nCameIndex, long nID)
{

}

void CDlgViewCameraSetting::OnChangeGain(long nCamIndex,long nID)
{
    if (auto* item = m_property.FindItem(nID))
    {
        m_strGain = item->GetValue();
    }

    m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->SetSystemGain(m_strGain);
}

void CDlgViewCameraSetting::OnChangeExposureTime(long nCamIndex, long nRow,long nID)
{

    if (auto* item = m_property.FindItem(nID))
    {
        m_strExposure = item->GetValue();
    }

    m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->SetExposureTime(m_strExposure, nRow);
}

void CDlgViewCameraSetting::OnLiveCamStart()
{
    if(m_parent->m_bInline == TRUE)
    {
        AfxMessageBox(_T("Now Inlinle Mode Starting"));
        return;
    }

    if (m_bLiveOnOff)
    {
        // end
        m_parent->OnInlineStop();
        m_bLiveOnOff = FALSE;
        m_pButtonLiveStatus->SetBackColor(RGB(255, 0, 0));
        SetDlgItemText(IDC_BUTTON_TRIGGER_WRITE2, _T("Live Off"));
    }
    else
    {
        // start
        m_bLiveOnOff = TRUE;
        m_parent->OnInlineStart(m_bLiveOnOff);
        m_parent->m_nCellID_sync = 0;
        m_parent->m_nCellID_calc = 0;
        m_parent->m_nCellID_finalcalc = 0;
        m_pButtonLiveStatus->SetBackColor(RGB(0, 255, 0));
        SetDlgItemText(IDC_BUTTON_TRIGGER_WRITE2, _T("Live On"));
    }

}

void CDlgViewCameraSetting::ListCOMpPorts()
{
    HDEVINFO deviceInfoSet = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, NULL, nullptr, DIGCF_PRESENT);

    if (deviceInfoSet == INVALID_HANDLE_VALUE)
    {
        // error
        return;
    }

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (DWORD i = 0; SetupDiEnumDeviceInfo(deviceInfoSet,i,&deviceInfoData);i++)
    {
        TCHAR deviceInstanceID[200];
        if (SetupDiGetDeviceInstanceId,deviceInfoSet,&deviceInfoData,deviceInstanceID,sizeof(deviceInstanceID),NULL)
        {
            //std::cout
        }
        HKEY hDeviceRegistrykey = SetupDiOpenDevRegKey(deviceInfoSet, &deviceInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);

        if (hDeviceRegistrykey != INVALID_HANDLE_VALUE)
        {
            TCHAR portName[256];
            DWORD portnameSize = sizeof(portName);
            DWORD regType = 0;
            if (RegQueryValueEx(hDeviceRegistrykey, REGSTR_VAL_PORTNAME, NULL, &regType, (LPBYTE)portName, &portnameSize) == ERROR_SUCCESS)
            {
                if (regType == REG_SZ)
                {
                    //portName 출력
                }
                CString strText;
                CString strPortName(portName);
                m_vecPortName.push_back(portName);
            }

        }
        RegCloseKey(hDeviceRegistrykey);
    }
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
}
void CDlgViewCameraSetting::GetCOMPortNum()
{
    if (auto* item = (m_property.FindItem(IDC_PARA_PORTNUM)))
    {
        m_strCOMPort = item->GetValue();
    }
    m_strBaurdRate = _T("9600");
}


void CDlgViewCameraSetting::OnChangeSensorGain(long nCamIndex, long nID)
{
    if (auto* item = m_property.FindItem(nID))
    {
        m_strSesorGain = item->GetValue();
    }
    double dGainValue;
    int nSensor = _ttoi(m_strSesorGain);
    m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->SetSensorSelector(m_strSesorGain);
    m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->GetSensorInfo(nSensor, dGainValue);

    if (nID == IDC_PARA_SENSORGAIN_0)
    {
        if (auto* item = m_property.FindItem(IDC_PARA_SENSORGAIN_VALUE_0))
        {
            CString strCurrentGain;
            strCurrentGain.Format(_T("%lf"), dGainValue);
            item->SetValue(strCurrentGain);
        }    
    }
    
    else
    {
        if (auto* item = m_property.FindItem(IDC_PARA_SENSORGAIN_VALUE_1))
        {
            CString strCurrentGain;
            strCurrentGain.Format(_T("%lf"), dGainValue);
            item->SetValue(strCurrentGain);
        }
    }


}


void CDlgViewCameraSetting::OnChangeSensorGainValue(long nCamIndex, long nID)
{
    if (auto* item = m_property.FindItem(nID))
    {
        m_strSesorGainValue = item->GetValue();
    }

    m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->SetSensorGain(m_strSesorGainValue);
}

void CDlgViewCameraSetting::OnChangeExposureSelector(long nCamIndex, long nID)
{
    long nRow;
    CString strExposure;

    if (auto* item = m_property.FindItem(nID))
    {
        m_strExposureSelector = item->GetValue();

    }
    if (nID == IDC_PARA_CAM_EXPOSURE_SELECTOR_0)
    {
        if (auto* item = m_property.FindItem(IDC_PARA_CAM_EXPOSURE_TIME_0))
        {
            double dBright, dDark;
            m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->GetExposureTime(dBright, dDark);

            if (m_strExposureSelector == _T("All"))
            {
                strExposure.Format(_T("%.3f"), dBright);
                item->SetValue(strExposure);
                nRow = 1;
            }
            else if (m_strExposureSelector == _T("Row 1"))
            {
                strExposure.Format(_T("%.3f"), dBright);
                item->SetValue(strExposure);
                nRow = 1;
            }
            else if (m_strExposureSelector == _T("Row 2"))
            {
                strExposure.Format(_T("%.3f"), dDark);
                item->SetValue(strExposure);
                nRow = 2;
            }
        }
    }

    if (nID == IDC_PARA_CAM_EXPOSURE_SELECTOR_1)
    {
        if (auto* item = m_property.FindItem(IDC_PARA_CAM_EXPOSURE_TIME_1))
        {
            double dBright, dDark;
            m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->GetExposureTime(dBright, dDark);

            if (m_strExposureSelector == _T("All"))
            {
                strExposure.Format(_T("%.3f"), dBright);
                item->SetValue(strExposure);
                nRow = 1;
            }
            else if (m_strExposureSelector == _T("Row 1"))
            {
                strExposure.Format(_T("%.3f"), dBright);
                item->SetValue(strExposure);
                nRow = 1;
            }
            else if (m_strExposureSelector == _T("Row 2"))
            {
                strExposure.Format(_T("%.3f"), dDark);
                item->SetValue(strExposure);
                nRow = 2;
            }
        }
    }

    if (nID == IDC_PARA_CAM_ROW_GAIN_SELECTOR_0)
    {
        if (auto* item = m_property.FindItem(IDC_PARA_CAM_ROW_GAIN_0))
        {
            double dRowGainBright, dRowGainDark;
            m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->GetRowGain(dRowGainBright, dRowGainDark);

            if (m_strExposureSelector == _T("All Rows"))
            {
                strExposure.Format(_T("%.3f"), dRowGainBright);
                item->SetValue(strExposure);
                nRow = 1;
            }
            else if (m_strExposureSelector == _T("Row 1"))
            {
                strExposure.Format(_T("%.3f"), dRowGainBright);
                item->SetValue(strExposure);
                nRow = 1;
            }
            else if (m_strExposureSelector == _T("Row 2"))
            {
                strExposure.Format(_T("%.3f"), dRowGainDark);
                item->SetValue(strExposure);
                nRow = 2;
            }
        }
    }

    if (nID == IDC_PARA_CAM_ROW_GAIN_SELECTOR_1)
    {
        if (auto* item = m_property.FindItem(IDC_PARA_CAM_ROW_GAIN_1))
        {
            double dRowGainBright, dRowGainDark;
            m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->GetRowGain(dRowGainBright, dRowGainDark);

            if (m_strExposureSelector == _T("All Rows"))
            {
                strExposure.Format(_T("%.3f"), dRowGainBright);
                item->SetValue(strExposure);
                nRow = 1;
            }
            else if (m_strExposureSelector == _T("Row 1"))
            {
                strExposure.Format(_T("%.3f"), dRowGainBright);
                item->SetValue(strExposure);
                nRow = 1;
            }
            else if (m_strExposureSelector == _T("Row 2"))
            {
                strExposure.Format(_T("%.3f"), dRowGainDark);
                item->SetValue(strExposure);
                nRow = 2;
            }
        }
    }

}

void CDlgViewCameraSetting::OnChangeExposureTime(long nCamIndex, long nID)
{
    long nRow;
    if (auto* item = m_property.FindItem(nID))
    {
        m_strExposure = item->GetValue();
    }

    if (m_strExposureSelector == _T("All"))
    {  // All은 Row1 Row2 두개다 셋팅하기 위함
        m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->SetExposureTime(m_strExposure, 1);
        m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->SetExposureTime(m_strExposure, 2);
    }
    else if (m_strExposureSelector == _T("Row 1"))
    {
        nRow = 1;
        m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->SetExposureTime(m_strExposure, nRow);
    }
    else if (m_strExposureSelector == _T("Row 2"))
    {
        nRow = 2;
        m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->SetExposureTime(m_strExposure, nRow);
    }


}

void CDlgViewCameraSetting::OnChangeRowGain(long nCamIndex, long nID)
{
    long nRow;
    if (auto* item = m_property.FindItem(nID))
    {
        m_strRowGain = item->GetValue();
    }

    if (m_strExposureSelector == _T("All Rows"))
    { // All은 Row1 Row2 두개다 셋팅하기 위함
        m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->SetRowGain(m_strRowGain, 1);
        m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->SetRowGain(m_strRowGain, 2);
    }
    else if (m_strExposureSelector == _T("Row 1"))
    {
        nRow = 1;
        m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->SetRowGain(m_strRowGain, nRow);
    }
    else if (m_strExposureSelector == _T("Row 2"))
    {
        nRow = 2;
        m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->SetRowGain(m_strRowGain, nRow);
    }
}

void CDlgViewCameraSetting::ResponseLeveling(long nCamIndex)
{
    if (m_bReset == FALSE)
    {
        AfxMessageBox(_T("Please First Press Reset Button"));
        return;
    }
    m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->ResponseLeveling();
    m_bReset = FALSE;
}

void CDlgViewCameraSetting::Reset(long nCamIndex)
{
    // 먼저 리셋하기 이전 값들을 txt 파일로 따로 저장 하자...
    int nSensor = 0;
    for (long nBefore = 0; nBefore < 16; nBefore++)
    {
     
        double dSensorGain;
        m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->GetSensorInfo(nSensor, dSensorGain); // nBefore, vecSensorGain[nBefore]

        CString strApp;
        strApp.Format(_T("SensorGain"));
        CString strKey;
        strKey.Format(_T("Value_%d"),nBefore);
        CString strVal;
        strVal.Format(_T("%f"), dSensorGain);
        WritePrivateProfileString(strApp, strKey, strVal, _T("C:\\Intekplus\\IBCI\\SensorGainBefore.txt"));
        nSensor++;
    }

    m_parent->m_cisComm->GetGrabExcutor(nCamIndex)->Reset();
    
    m_bReset = TRUE;
}