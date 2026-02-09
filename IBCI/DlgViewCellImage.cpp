// CDlgViewCellImage.cpp: 구현 파일
//

#include "pch.h"
#include "IBCI.h"
#include "DlgViewCellImage.h"
#include "afxdialogex.h"

#include <Widget/ImageView.h>
#include "IBCIDlg.h"
#include "../Inspection/InspectionMeasure.h"
#include "../Inspection/DlgInspectionMain.h"
#include "../Inspection/InspectionRoot.h"
#include "../Inspection/Accessor.h"
#include "ViewCellImageUpdater.h"

#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgViewCellImage 대화 상자

IMPLEMENT_DYNAMIC(CDlgViewCellImage, CDialogEx)

CDlgViewCellImage::CDlgViewCellImage(MachineType machinType, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VIEW_CELL_IMAGE, pParent)
	, m_nModeUpdate(0)
    , m_nModeGray(4)
    , m_nModeProcessing(0)
    , m_nModeInspection(0)
    , m_nCameraIndex(0)
    , m_bSurfaceInsp(FALSE)
{
	::memset(m_imageViews, 0, sizeof(m_imageViews));

    m_nCameraIndex = (machinType == MachineType::ES_rollpress_pinhole) ? 4 : 2;
    m_pButtonInlineInspMode = new CCustomButton;
}

CDlgViewCellImage::~CDlgViewCellImage()
{
    for (int cameraIndex = 0; cameraIndex < m_nCameraIndex; cameraIndex++)
	{
		delete m_imageViews[cameraIndex];
	}
    delete m_pButtonInlineInspMode;
}


void CDlgViewCellImage::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_MODE_UPDATE, m_ComboModeUpdate);
    DDX_Control(pDX, IDC_COMBO_MODE_GRAY, m_ComboModeGray);
    DDX_Control(pDX, IDC_BUTTON_SURFACE_IMAGE_LOAD, m_btn_ImageLoad);
    DDX_Control(pDX, IDC_BUTTON_SURFACE_INSPECTION, m_btn_Inspection);
    DDX_Control(pDX, IDC_COMBO_MODE_INSPECTION, m_ComboModeInspection);
    DDX_Control(pDX, IDC_BUTTON_INLINE_INSP, *m_pButtonInlineInspMode);
    DDX_Control(pDX, IDC_BUTTON_ORI_IMAGE, m_btn_OriImage);
    DDX_Control(pDX, IDC_BUTTON_PRE_PROCESSING, m_btn_PreProcessing);
    DDX_Control(pDX, IDC_BUTTON_THRESHOLD, m_btn_Threshold);
}


BEGIN_MESSAGE_MAP(CDlgViewCellImage, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_CBN_SELCHANGE(IDC_COMBO_MODE_UPDATE, &CDlgViewCellImage::OnCbnSelchangeComboModeUpdate)
    ON_CBN_SELCHANGE(IDC_COMBO_MODE_GRAY, &CDlgViewCellImage::OnCbnSelchangeComboModeGray)
    ON_BN_CLICKED(IDC_BUTTON_SURFACE_IMAGE_LOAD, &CDlgViewCellImage::OnBnClickedButtonSufaceImageLoad)
    ON_BN_CLICKED(IDC_BUTTON_SURFACE_INSPECTION, &CDlgViewCellImage::OnBnClickedButtonSufaceInspection)
    ON_CBN_SELCHANGE(IDC_COMBO_MODE_INSPECTION, &CDlgViewCellImage::OnCbnSelchangeComboModeInspection)
    ON_BN_CLICKED(IDC_BUTTON_INLINE_INSP, &CDlgViewCellImage::OnBnClickedButtonInlineInsp)
    ON_BN_CLICKED(IDC_BUTTON_ORI_IMAGE, &CDlgViewCellImage::OnBnClickedButtonOriImage)
    ON_BN_CLICKED(IDC_BUTTON_PRE_PROCESSING, &CDlgViewCellImage::OnBnClickedButtonPreProcessing)
    ON_BN_CLICKED(IDC_BUTTON_THRESHOLD, &CDlgViewCellImage::OnBnClickedButtonThreshold)
    END_MESSAGE_MAP()


// CDlgViewCellImage 메시지 처리기
BOOL CDlgViewCellImage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	LPCTSTR captions[2] =
	{
        //_T("CAM #1_B"),
        //_T("CAM #1_D"),
        //_T("CAM #2_B"),
        //_T("CAM #2_D"),
        _T("Gradient Image"),
		_T("ABS Image"),
	};

	for (int cameraIndex = 0; cameraIndex < m_nCameraIndex; cameraIndex++)
	{
		m_imageViews[cameraIndex] = new IPVM::ImageView(GetSafeHwnd(), IPVM::Rect(0, 0, 0, 0));
        m_imageViews[cameraIndex]->WindowOverlayAdd(IPVM::Point_32s_C2(3, 0), captions[cameraIndex], RGB(0, 255, 0), 18);
        m_imageViews[cameraIndex]->WindowOverlayShow();
	}

	CRect rtClient;
	GetClientRect(rtClient);

	Resize(rtClient.Width(), rtClient.Height());
	
	m_ComboModeUpdate.SetCurSel(m_nModeUpdate);
    m_ComboModeGray.SetCurSel(m_nModeGray);
    m_ComboModeGray.InsertString(0,_T("Both"));
    m_ComboModeGray.InsertString(1, _T("Bright"));
    m_ComboModeGray.InsertString(2, _T("Dark"));
    m_ComboModeGray.InsertString(3, _T("Clear"));
    m_ComboModeGray.InsertString(4, _T("Normal"));

    m_ComboModeInspection.InsertString(0, _T("Align"));
    m_ComboModeInspection.InsertString(1, _T("Blob"));
    m_ComboModeInspection.InsertString(2, _T("All"));
    m_ComboModeInspection.SetCurSel(0);

    m_pButtonInlineInspMode->SetDraw3D(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgViewCellImage::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_imageViews[0]->GetSafeHwnd())
	{
		Resize(cx, cy);
	}
}

void CDlgViewCellImage::Resize(int cx, int cy)
{
	CRect rtCombo;
	rtCombo.left = 0;
	rtCombo.top = 0;
	rtCombo.right = (long)cx * 0.1;
	rtCombo.bottom = (long)cy * 0.1;
	
	int nGap = 5;

	int nViewCx		= cx - 2 * nGap;
    int nViewCy     = cy - rtCombo.Height() - 2 * nGap;
	int nViewWidth	= nViewCx / 2;
    int nViewHeight = (m_nCameraIndex == 4) ? nViewCy / 2 : nViewCy + nGap;

    int nBtnXsize = 80;
    int nBtnYsize = 20;

	m_ComboModeUpdate.MoveWindow(0, 0, rtCombo.Width(), rtCombo.Height(), TRUE);
    m_ComboModeGray.MoveWindow(150, 0, rtCombo.Width(), rtCombo.Height(), TRUE);
    m_ComboModeInspection.MoveWindow(760, 0, rtCombo.Width()-50, rtCombo.Height(), TRUE);
    //m_btn_ImageLoad.MoveWindow(1030, 0, rtCombo.Width() - 30, rtCombo.Height(), TRUE);
    //m_btn_Inspection.MoveWindow(1140, 0, rtCombo.Width() - 30, rtCombo.Height(), TRUE);
    //m_btn_InlineInspMode.MoveWindow(750, 0, rtCombo.Width(), rtCombo.Height(), TRUE);

    m_btn_ImageLoad.MoveWindow(840, 0, nBtnXsize, nBtnYsize, TRUE);
    m_btn_Inspection.MoveWindow(920, 0, nBtnXsize, nBtnYsize, TRUE);

    m_btn_OriImage.MoveWindow(1000, 0, nBtnXsize, nBtnYsize, TRUE);
    m_btn_PreProcessing.MoveWindow(1080, 0, nBtnXsize + 5, nBtnYsize, TRUE);
    m_btn_Threshold.MoveWindow(1170, 0, nBtnXsize + 5, nBtnYsize, TRUE);
    m_pButtonInlineInspMode->MoveWindow(620, 0, nBtnXsize + 25, nBtnYsize, TRUE);
    m_pButtonInlineInspMode->SetFont(_T("HY 견고딕"), RGB(0, 0, 0), 11, 5, 6);

    if (m_nCameraIndex == 4)
    {
        ::MoveWindow(m_imageViews[0]->GetSafeHwnd(), 0                  , rtCombo.bottom                        , nViewWidth, nViewHeight, TRUE);
        ::MoveWindow(m_imageViews[1]->GetSafeHwnd(), 0                  , rtCombo.bottom + nViewHeight + nGap   , nViewWidth, nViewHeight, TRUE);
        ::MoveWindow(m_imageViews[2]->GetSafeHwnd(), nViewWidth + nGap  , rtCombo.bottom                        , nViewWidth, nViewHeight, TRUE);
        ::MoveWindow(m_imageViews[3]->GetSafeHwnd(), nViewWidth + nGap  , rtCombo.bottom + nViewHeight + nGap   , nViewWidth,
            nViewHeight, TRUE);
    }
    else
    {
        ::MoveWindow(m_imageViews[0]->GetSafeHwnd(), 0                  , rtCombo.bottom                        , nViewWidth, nViewHeight, TRUE);
        ::MoveWindow(m_imageViews[1]->GetSafeHwnd(), nViewWidth + nGap  , rtCombo.bottom                        , nViewWidth, nViewHeight, TRUE);
    }
}


void CDlgViewCellImage::OnCbnSelchangeComboModeUpdate()
{
	m_nModeUpdate = m_ComboModeUpdate.GetCurSel();
    if (m_pParent->m_bInline)
    {
        if (m_nModeUpdate == ENUM_VIEW_MODE_CELL_IMAGE::DO_NOT_UPDATE)
        {
            m_pButtonInlineInspMode->SetBackGradation(RGB(26, 237, 26), RGB(163, 255, 73), 1);
            EnableWindow(TRUE);
        }
        else if (m_nModeUpdate == ENUM_VIEW_MODE_CELL_IMAGE::UPDATE_ALWAYS || m_nModeUpdate == ENUM_VIEW_MODE_CELL_IMAGE::UPDATE_NG_ONLY)
        {
            m_pButtonInlineInspMode->SetBackGradation(RGB(237, 26, 4), RGB(255, 163, 73), 1);
            EnableWindow(FALSE);
        }

        Invalidate(false);
    }
    m_pParent->m_viewCellImageUpdater->SetInlineUpdateMode(m_nModeUpdate);
}

void CDlgViewCellImage::OnCbnSelchangeComboModeGray()
{
    m_nModeGray = m_ComboModeGray.GetCurSel();
}
void CDlgViewCellImage::OnCbnSelchangeComboModeProcessing()
{
    m_nModeProcessing = m_ComboModeProcessing.GetCurSel();
}


void CDlgViewCellImage::OnBnClickedButtonSufaceImageLoad()
{
    for (long n=0;n<2;n++)
    {
        m_imageViews[n]->ROIClear();
        m_imageViews[n]->ImageOverlayClear();
        m_imageViews[n]->ROIShow();
        m_imageViews[n]->ImageOverlayShow();
    }

    const TCHAR szFilters[] = _T("Image files (*.bmp, *.png, *.jpg) | *.bmp;*.png;*.jpg|");
    CString strFullFilename;

	CFileDialog dlg(TRUE, _T(""), _T(""),
        OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT, szFilters);

    CString strFileList;
    const int nMaxFiles = 400;
    const int nBuffSize = (nMaxFiles * (MAX_PATH + 1)) + 1;
    dlg.GetOFN().lpstrFile = strFileList.GetBuffer(nBuffSize);
    dlg.GetOFN().nMaxFile = nBuffSize;

    if (dlg.DoModal() != IDOK)
        return;

    IPVM::Image_8u_C1 brightImage, darkImage;

    CString strTemp;
    strTemp = dlg.GetPathName();
    m_strImageName = strTemp;
    if (strTemp.Find(_T("BRIGHT")) != -1)
    {
        brightImage.LoadFrom(dlg.GetPathName());
        m_imageViews[0]->SetImage(brightImage);
        m_strBrightPath = dlg.GetPathName();
        long nFindText = strTemp.Find(_T("BRIGHT"));
        strTemp.Right(nFindText);
        CString strDirectory = strTemp.Left(strTemp.ReverseFind(_T('\\')) + 1);
        CString strName = strTemp.Right(strTemp.GetLength() - strTemp.ReverseFind(_T('\\')) - 18);
        CString strTime = strTemp.Mid(strTemp.ReverseFind(_T('\\')) + 1, 8);
        CString strPattern = strDirectory + strTime + _T("*.*");

        CFileFind finder;

        BOOL bFind;
        bFind = finder.FindFile(strPattern);
        BOOL bOpen = False;
        while (bFind)
        {
            bFind = finder.FindNextFileW();

            if (finder.IsDirectory() || finder.IsDots())
                continue;

            CString fileName = finder.GetFileName(); // 확장자까지 가져옴

            CString strBeforeName = dlg.GetPathName();
            strBeforeName = strBeforeName.Right(strBeforeName.GetLength() - strBeforeName.ReverseFind(_T('\\')) - 18);
            strName.Replace(_T("BRIGHT"), _T("DARK"));
            long nFindName = fileName.Find(strName);
            CString strTime2 = strTemp.Mid(strTemp.ReverseFind(_T('\\')) + 1, 17);

            if (nFindName > 0)
            {
                CString strFullFileName = strDirectory + fileName;
                int ID = MessageBox(_T("선택 이미지:") + strTime2 + strBeforeName + "\n" + _T("추천 이미지:") + fileName
                        + "\n" + _T("을 여시겠습니까?"),
                    _T("DARK Image Open"), MB_OKCANCEL);
                if (ID == 1)
                {
                    darkImage.LoadFrom(strFullFileName);
                    m_imageViews[1]->SetImage(darkImage);
                    m_strDarkPath = strFullFileName;
                    bOpen = true;
                    bFind = false;
                }
            }
        }
    }
}

void CDlgViewCellImage::OnBnClickedButtonSufaceInspection()
{
    if (m_strBrightPath.IsEmpty() && m_strDarkPath.IsEmpty())
    {
        return;
    }
    CString strCellID;
    strCellID = m_strBrightPath.Mid(m_strBrightPath.ReverseFind('_') + 1);
    strCellID = strCellID.Mid(0, strCellID.ReverseFind('.'));
    if (m_pParent!=nullptr)
    {
        const long n = _ttol(strCellID);
        BOOL bTopBack = g_systemParameter.bDirection;
        m_bSurfaceInsp = TRUE;
        m_pParent->m_viewCellImageUpdater->SetSurfaceInspCheck(m_bSurfaceInsp);
        m_pParent->m_viewCellImageUpdater->SetInspMode(m_nModeInspection, m_pParent->m_bInline, m_pParent->m_bBatchView);
        m_pParent->Inspection(m_strBrightPath, m_strDarkPath, n, bTopBack);
        m_pParent->m_viewCellImageUpdater->SetInspectionCell(m_InspectionCell);
        m_pParent->m_viewCellImageUpdater->Push();
    }
    m_bSurfaceInsp = FALSE;
}

void CDlgViewCellImage::SetDlg(CIBCIDlg* pParent)
{
    m_pParent = (CIBCIDlg*)pParent;
}

void CDlgViewCellImage::OnCbnSelchangeComboModeInspection()
{
    m_nModeInspection = m_ComboModeInspection.GetCurSel();
}

void CDlgViewCellImage::OnBnClickedButtonInlineInsp()
{
    //if (m_ComboModeUpdate.GetCurSel()==ENUM_VIEW_MODE_CELL_IMAGE::UPDATE_ALWAYS)
    //{
    //    m_ComboModeUpdate.SetCurSel(0);
    //    EnableWindow(TRUE);

    //}
    //else if (m_ComboModeUpdate.GetCurSel() == ENUM_VIEW_MODE_CELL_IMAGE::DO_NOT_UPDATE)
    //{
    //    m_ComboModeUpdate.SetCurSel(1);     
    //    EnableWindow(FALSE);
    //}

    //OnCbnSelchangeComboModeUpdate();
    //m_pParent->m_viewCellImageUpdater->SetInlineUpdateMode(m_nModeUpdate);
}

void CDlgViewCellImage::EnableWindow(BOOL bEnable)
{
    GetDlgItem(IDC_COMBO_MODE_INSPECTION)->EnableWindow(bEnable);
    GetDlgItem(IDC_BUTTON_SURFACE_INSPECTION)->EnableWindow(bEnable);
    GetDlgItem(IDC_BUTTON_SURFACE_IMAGE_LOAD)->EnableWindow(bEnable);
}

void CDlgViewCellImage::ImageViewClear()
{
    m_pParent->m_viewCellImageUpdater->SetImageRoiClear();
}
void CDlgViewCellImage::OnBnClickedButtonOriImage()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_imageViews[0]->SetImage(m_InspectionCell->m_inspectionImageBright);
    m_imageViews[0]->ImageOverlayShow();
    m_imageViews[0]->ROIShow();

    m_imageViews[1]->SetImage(m_InspectionCell->m_inspectionImageDark);
    m_imageViews[1]->ImageOverlayShow();
    m_imageViews[1]->ROIShow();
}

void CDlgViewCellImage::OnBnClickedButtonPreProcessing()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    IPVM::Image_8u_C1 Img_PreProcessing;
    auto* itemCellBlob = (CInspectionBlob*)m_InspectionCell->GetProcessingItem(ProcItemCategory::Blob);

    Img_PreProcessing = itemCellBlob->m_bufferThreshold;
    m_imageViews[0]->SetImage(Img_PreProcessing);
    m_imageViews[1]->SetImage(Img_PreProcessing);
}

void CDlgViewCellImage::OnBnClickedButtonThreshold()
{
    IPVM::Image_8u_C1 ImageThreshold, ImageThresholdTemp;
    ImageThresholdTemp.Create(
        m_InspectionCell->m_inspectionImageBright.GetSizeX(), m_InspectionCell->m_inspectionImageBright.GetSizeY());
    auto* inspection = (CInspectionBlob*)m_InspectionCell->GetProcessingItem(ProcItemCategory::Blob);
    
    long nTotalInspCount = inspection->m_para.m_vecBlobParaCondition.size();
    for (int nIdex = 0; nIdex < nTotalInspCount; nIdex++)
    {
        CBlobParaRoi& blobPara = inspection->m_para.m_vecBlobParaCondition[nIdex];
        bool bCheck = blobPara.m_bUseInspection;

        if (bCheck)
        {
            IPVM::Rect rectCal = blobPara.m_rtSetInspArea;
            ImageThreshold.Create(m_InspectionCell->m_inspectionImageBright.GetSizeX(),
                m_InspectionCell->m_inspectionImageBright.GetSizeY());

            IPVM::Image_8u_C1* ImageSrc;
            if (blobPara.m_nImageType == (long)ENUM_BLOB_TYPE_SRCIMAGE::Brightness) //bright
            {
                ImageSrc = &m_InspectionCell->m_inspectionImageBright;
            }
            else if (blobPara.m_nImageType == (long)ENUM_BLOB_TYPE_SRCIMAGE::Darkness) //bright
            {
                ImageSrc = &m_InspectionCell->m_inspectionImageDark;
            }
            else
            {
                IPVM::ImageProcessing::Max(m_InspectionCell->m_inspectionImageDark,
                    m_InspectionCell->m_inspectionImageBright, IPVM::Rect(m_InspectionCell->m_inspectionImageBright),
                    inspection->m_bufferThreshold6);
                ImageSrc = &inspection->m_bufferThreshold6;
            }

            long nVerDivSave = blobPara.m_nThresholdHorDiv;
            long nHorDivSave = blobPara.m_nThresholdVerDiv;

            IPVM::Rect rtOrig = blobPara.m_rtSetInspArea;

            std::vector<IPVM::Rect> rtDiv;

            long nSizeOrigX = rtOrig.Width();
            long nSizeOrigY = rtOrig.Height();

            float fHorzSize = rtOrig.Height() / (float)nHorDivSave;
            float fVertSize = rtOrig.Width() / (float)nVerDivSave;

            int x, y, k;
            if (nVerDivSave == 999)
            {
                float fSensorPosOffset = m_InspectionCell->m_inspectionImageBright.GetSizeX() / 32;
                float fPosCheckY = rtOrig.CenterPoint().m_y;

                IPVM::Point_32f_C2 PtCheckingSensor;
                IPVM::Rect rtValidRect;

                long nValidCnt = 0;
                for (int cnt = 0; cnt < 32; cnt++)
                {
                    PtCheckingSensor.m_x = fSensorPosOffset * cnt;
                    PtCheckingSensor.m_y = fPosCheckY;
                    if (rtOrig.PtInRect(PtCheckingSensor))
                    {
                        // Left 맞추기
                        if (nValidCnt == 0)
                        {
                            rtValidRect.m_left = rtOrig.m_left;
                        }
                        else
                        {
                            rtValidRect.m_left = fSensorPosOffset * (cnt - 1);
                        }

                        rtValidRect.m_top = rtOrig.m_top;
                        rtValidRect.m_bottom = rtOrig.m_bottom;
                        rtValidRect.m_right = PtCheckingSensor.m_x;

                        rtDiv.push_back(rtValidRect);
                        nValidCnt++;
                    }
                    // Right 맞추기
                    if (fSensorPosOffset * (cnt - 1) < rtOrig.m_right && rtOrig.m_right <= fSensorPosOffset * cnt)
                    {
                        rtValidRect.m_left = rtDiv[nValidCnt - 1].m_right;
                        rtValidRect.m_top = rtOrig.m_top;
                        rtValidRect.m_bottom = rtOrig.m_bottom;
                        rtValidRect.m_right = rtOrig.m_right;
                        rtDiv.push_back(rtValidRect);
                        break;
                    }
                }
            }
            else
            {
                for (y = 0; y < nHorDivSave; y++)
                {
                    for (x = 0; x < nVerDivSave; x++)
                    {
                        k = y * nVerDivSave + x;
                        IPVM::Rect temp = IPVM::Rect((int)(rtOrig.m_left + x * fVertSize + .5f),
                            (int)(rtOrig.m_top + y * fHorzSize + .5f), (int)(rtOrig.m_left + (x + 1) * fVertSize + .5f),
                            (int)(rtOrig.m_top + (y + 1) * fHorzSize + .5f));
                        rtDiv.push_back(temp);
                    }
                }
            }

            float fAverage;
            {
                //		BinarizationGreater_Mean(CopyImage, IPVM::Rect(InspImage), m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold, InspImage, fAverage);
                for (long n = 0; n < rtDiv.size(); n++)
                {
                    //inspection->BinarizationGreater_Mean(CopyImage, rtDiv[n], m_para.m_vecBlobParaCondition[nInspBlob].m_nThreshold, InspImage, fAverage);
                    inspection->BinarizationGreater_Mean(
                        *ImageSrc, rtDiv[n], blobPara.m_nThreshold, ImageThreshold, fAverage);
                }
            }
            IPVM::ImageProcessing::Copy(ImageThreshold, rectCal, ImageThresholdTemp);
        }
    }
    m_imageViews[0]->SetImage(ImageThresholdTemp);
    m_imageViews[1]->SetImage(ImageThresholdTemp);
}
