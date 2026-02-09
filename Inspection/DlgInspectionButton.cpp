// DlgInspectionButton.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Accessor.h"
#include "InspectionRoot.h"
#include "DlgInspectionMain.h"
#include "afxdialogex.h"
#include "ImageViewEx.h"
#include "InspectionCell.h"

#include "../VisionModule/Language/LanguageText.h"

#include "Gadget/TimeCheck.h"
#include "Types/Rect.h"
#include "Algorithm/ImageProcessing.h"
#include "../IBCI/SurfaceInspect.h"

#include <vector>
#include <string>
#include <iostream>

#include "Types/Image_8u_C1.h"
#include "Types/Image_16u_C1.h"
#include "Types/Image_32f_C1.h"

#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgInspectionMain 대화 상자입니다.

void CDlgInspectionMain::InitInspecitonButton(CRect viewRect)
{
	CRect dlgRect;
	GetClientRect(dlgRect);

	long dx, dy;

	CRect captionSettingRect;
	m_captionSetting.GetWindowRect(captionSettingRect);
	m_captionSetting.MoveWindow(viewRect.left - ITEM_GAP - captionSettingRect.Width(), viewRect.top, captionSettingRect.Width(), captionSettingRect.Height());

	CRect boxSettingRect;
	m_boxSetting.GetWindowRect(boxSettingRect);
	dx = boxSettingRect.left - captionSettingRect.left;
	dy = boxSettingRect.top - captionSettingRect.top;
	m_boxSetting.MoveWindow(viewRect.left - ITEM_GAP - captionSettingRect.Width() + dx, viewRect.top + dy, boxSettingRect.Width(), boxSettingRect.Height());

	CRect buttonSettingUndoRect;
	m_buttonSettingUndo.GetWindowRect(buttonSettingUndoRect);
	dx = buttonSettingUndoRect.left - captionSettingRect.left;
	dy = buttonSettingUndoRect.top - captionSettingRect.top;
	m_buttonSettingUndo.MoveWindow(viewRect.left - ITEM_GAP - captionSettingRect.Width() + dx, viewRect.top + dy, buttonSettingUndoRect.Width(), buttonSettingUndoRect.Height());
	
	CRect captionImageRect;
	m_captionImage.GetWindowRect(captionImageRect);
	m_captionImage.MoveWindow(viewRect.left - ITEM_GAP - captionImageRect.Width(), viewRect.top + boxSettingRect.Height() + ITEM_GAP, captionImageRect.Width(), captionImageRect.Height());

	CRect boxImageRect;
	m_boxImage.GetWindowRect(boxImageRect);
	dx = boxImageRect.left - captionImageRect.left;
	dy = boxImageRect.top - captionImageRect.top;
	m_boxImage.MoveWindow(viewRect.left - ITEM_GAP - captionSettingRect.Width() + dx, viewRect.top + boxSettingRect.Height() + ITEM_GAP + dy, boxImageRect.Width(), boxImageRect.Height());

	CRect buttonImageLoadRect;
	m_buttonImageLoad.GetWindowRect(buttonImageLoadRect);
	dx = buttonImageLoadRect.left - captionImageRect.left;
	dy = buttonImageLoadRect.top - captionImageRect.top;
	m_buttonImageLoad.MoveWindow(viewRect.left - ITEM_GAP - captionSettingRect.Width() + dx, viewRect.top + boxSettingRect.Height() + ITEM_GAP + dy, buttonImageLoadRect.Width(), buttonImageLoadRect.Height());
	
	CRect buttonmultiCellImageLoadRect;
	m_buttonMultiCellImage.GetWindowRect(buttonmultiCellImageLoadRect);
	dx = buttonmultiCellImageLoadRect.left - captionImageRect.left;
	dy = buttonmultiCellImageLoadRect.top - captionImageRect.top;
	m_buttonMultiCellImage.MoveWindow(viewRect.left - ITEM_GAP - captionSettingRect.Width() + dx, viewRect.top + boxSettingRect.Height() + ITEM_GAP + dy, buttonmultiCellImageLoadRect.Width(), buttonmultiCellImageLoadRect.Height());

	CRect buttonShowBrightImage;
	m_buttonShowBrightImage.GetWindowRect(buttonShowBrightImage);
	dx = buttonShowBrightImage.left - captionImageRect.left;
	dy = buttonShowBrightImage.top - captionImageRect.top;
	m_buttonShowBrightImage.MoveWindow(viewRect.left - ITEM_GAP - captionSettingRect.Width() + dx, viewRect.top + boxSettingRect.Height() + ITEM_GAP + dy, buttonmultiCellImageLoadRect.Width(), buttonmultiCellImageLoadRect.Height());

	CRect buttonShowDarkImage;
	m_buttonShowDarkImage.GetWindowRect(buttonShowDarkImage);
	dx = buttonShowDarkImage.left - captionImageRect.left;
	dy = buttonShowDarkImage.top - captionImageRect.top;
	m_buttonShowDarkImage.MoveWindow(viewRect.left - ITEM_GAP - captionSettingRect.Width() + dx, viewRect.top + boxSettingRect.Height() + ITEM_GAP + dy, buttonmultiCellImageLoadRect.Width(), buttonmultiCellImageLoadRect.Height());


	CRect captionTestRect;
	m_captionTest.GetWindowRect(captionTestRect);
	m_captionTest.MoveWindow(viewRect.left - ITEM_GAP - captionSettingRect.Width(), viewRect.top + boxSettingRect.Height() + boxImageRect.Height() + ITEM_GAP * 2, captionTestRect.Width(), captionTestRect.Height());

	CRect boxTestRect;
	m_boxTest.GetWindowRect(boxTestRect);
	dx = boxTestRect.left - captionTestRect.left;
	dy = boxTestRect.top - captionTestRect.top;
	m_boxTest.MoveWindow(viewRect.left - ITEM_GAP - captionSettingRect.Width() + dx, viewRect.top + boxSettingRect.Height() + boxImageRect.Height() + ITEM_GAP * 2 + dy, boxTestRect.Width(), boxTestRect.Height());

	CRect buttonTestRect;
	m_buttonTest.GetWindowRect(buttonTestRect);
	dx = buttonTestRect.left - captionTestRect.left;
	dy = buttonTestRect.top - captionTestRect.top;
	m_buttonTest.MoveWindow(viewRect.left - ITEM_GAP - captionSettingRect.Width() + dx, viewRect.top + boxSettingRect.Height() + boxImageRect.Height() + ITEM_GAP * 2 + dy, buttonTestRect.Width(), buttonTestRect.Height());

	CRect buttonTestAllRect;
	m_buttonTestAll.GetWindowRect(buttonTestAllRect);
	dx = buttonTestAllRect.left - captionTestRect.left;
	dy = buttonTestAllRect.top - captionTestRect.top;
	m_buttonTestAll.MoveWindow(viewRect.left - ITEM_GAP - captionSettingRect.Width() + dx, viewRect.top + boxSettingRect.Height() + boxImageRect.Height() + ITEM_GAP * 2 + dy, buttonTestAllRect.Width(), buttonTestAllRect.Height());

	CRect buttonShowoptionTextRect;
	GetDlgItem(IDC_CAPTION_SHOWOPTION)->GetWindowRect(buttonShowoptionTextRect);
	GetDlgItem(IDC_CAPTION_SHOWOPTION)->MoveWindow(viewRect.left - ITEM_GAP - captionSettingRect.Width(), viewRect.top + boxSettingRect.Height() + boxImageRect.Height() + boxTestRect.Height() + ITEM_GAP * 3, buttonShowoptionTextRect.Width(), buttonShowoptionTextRect.Height());

	CRect boxshowoptionRect;
	GetDlgItem(IDC_BOX_SHOWOPTION)->GetWindowRect(boxshowoptionRect);
	dx = boxshowoptionRect.left - buttonShowoptionTextRect.left;
	dy = boxshowoptionRect.top - buttonShowoptionTextRect.top;
	GetDlgItem(IDC_BOX_SHOWOPTION)->MoveWindow(viewRect.left - ITEM_GAP - captionSettingRect.Width() + dx, viewRect.top + boxSettingRect.Height() + boxImageRect.Height() + boxTestRect.Height() + ITEM_GAP * 3 + dy, boxshowoptionRect.Width(), boxshowoptionRect.Height());

	CRect buttonShowOKRect;
	GetDlgItem(IDC_BUTTON_SHOWOK)->GetWindowRect(buttonShowOKRect);
	dx = buttonShowOKRect.left - buttonShowoptionTextRect.left;
	dy = buttonShowOKRect.top - buttonShowoptionTextRect.top;
	GetDlgItem(IDC_BUTTON_SHOWOK)->MoveWindow(viewRect.left - ITEM_GAP - captionSettingRect.Width() + dx, viewRect.top + boxSettingRect.Height() + boxImageRect.Height() + boxTestRect.Height() + ITEM_GAP * 3  + dy, buttonShowOKRect.Width(), buttonShowOKRect.Height() );

	CRect buttonShowNGRect;
	GetDlgItem(IDC_BUTTON_SHOWNG)->GetWindowRect(buttonShowNGRect);
	dx = buttonShowNGRect.left - buttonShowoptionTextRect.left;
	dy = buttonShowNGRect.top - buttonShowoptionTextRect.top;
	GetDlgItem(IDC_BUTTON_SHOWNG)->MoveWindow(viewRect.left - ITEM_GAP - captionSettingRect.Width() + dx, viewRect.top + boxSettingRect.Height() + boxImageRect.Height() + boxTestRect.Height() + ITEM_GAP * 3 + dy, buttonShowNGRect.Width(), buttonShowNGRect.Height());

	CRect buttonShowSKIPRect;
	GetDlgItem(IDC_BUTTON_SHOWSKIP)->GetWindowRect(buttonShowSKIPRect);
	dx = buttonShowSKIPRect.left - buttonShowoptionTextRect.left;
	dy = buttonShowSKIPRect.top - buttonShowoptionTextRect.top;
	GetDlgItem(IDC_BUTTON_SHOWSKIP)->MoveWindow(viewRect.left - ITEM_GAP - captionSettingRect.Width() + dx, viewRect.top + boxSettingRect.Height() + boxImageRect.Height() + boxTestRect.Height() + ITEM_GAP * 3 + dy, buttonShowSKIPRect.Width(), buttonShowSKIPRect.Height());

}

void CDlgInspectionMain::OnBnClickedButtonImageLoad()
{
	
	CWnd* pBtn = GetDlgItem(IDC_BUTTON_SHOW_BRIGHT_IMAGE);
    CWnd* pBtn2 = GetDlgItem(IDC_BUTTON_SHOW_DARK_IMAGE);
	
	if (pBtn && pBtn->IsWindowEnabled() && pBtn2 && pBtn2->IsWindowEnabled())
    {
        pBtn->EnableWindow(FALSE);
        pBtn2->EnableWindow(FALSE);
		
    }
	
	m_vecMultiCellImage.clear();
    m_vecsetupImage_Area.clear();
    std::vector<Inspection::SetupImage>().swap(m_vecsetupImage_Area);

	OPENFILENAME ofn;
    memset(&ofn, 0, sizeof(ofn));
    std::vector<TCHAR> pathName(32768, L'\0');
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = m_hWnd;
    ofn.lpstrFilter
        = _T("All Supported Image Files (*.bmp;*.jpg;*.jpeg;*.png)\0*.bmp;*.jpg;*.jpeg;*.png\0BMP Image Files ")
          _T("(*.bmp)\0*.bmp\0PNG Image Files (*.png)\0*.png\0JPEG Image Files (*.jpg;*.jpeg)\0*.jpg;*.jpeg\0\0");
    ofn.lpstrFile = &pathName[0];
    ofn.nMaxFile = 32768;
    ofn.lpstrDefExt = _T("*.bmp");
    ofn.Flags = OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT
        | OFN_EXPLORER;

    if (::GetOpenFileName(&ofn) == FALSE)
    {
        return ;
    }

    // 다중 파일 파싱
    std::vector<std::wstring> filePaths;
    TCHAR* p = ofn.lpstrFile;
    std::wstring directory = p;
    p += directory.length() + 1;

	
   
	IPVM::Image_8u_C1 tempTopImg;
    IPVM::Image_8u_C1 tempBottomImg;
    
	if (*p == 0)
    {
        // 단일 파일 선택
        filePaths.push_back(directory);
    }
    else
    {
        // 다중 파일 선택
        while (*p)
        {
            std::wstring fileName = p;
            filePaths.push_back(directory + _T("\\") + fileName);
            p += fileName.length() + 1;
        }
    }

    // 첫 번째 이미지 로드 (m_inputImage)
    if (filePaths.size() > 0)
    {
        
        long horSize = 0;
        long verSize = 0;
        long bpp = 0;
     
		IPVM::Image::InfoFrom(filePaths[0].c_str(), horSize, verSize, bpp);
        tempTopImg.LoadFrom(filePaths[0].c_str());
	  		
    }
    // 두 번째 이미지 로드 (m_inputImage2)
    if (filePaths.size() > 1)
    {
        
        long horSize2 = 0;
        long verSize2 = 0;
        long bpp2 = 0;
       
		IPVM::Image::InfoFrom(filePaths[1].c_str(), horSize2, verSize2, bpp2);
        tempBottomImg.LoadFrom(filePaths[1].c_str());
      
    }
    int32_t imageSizeX = tempTopImg.GetSizeX();
    int32_t imageSizeY = tempTopImg.GetSizeY();
    ////////////// 로드 끝난 다음에.... 영상 처리 하자
    m_ResultABSImage->Create(imageSizeX, imageSizeY);
    m_ResultABSImage->FillZero();

	m_ResultGammaImage->Create(imageSizeX, imageSizeY);
    m_ResultGammaImage->FillZero();
    
	IPVM::TimeCheck timeCheck;
	

    for (long y = 0; y < imageSizeY; y++)
    {
        const auto* TopImageMem = tempTopImg.GetMem(0, y);
        const auto* BottomImageMem = tempBottomImg.GetMem(0, y);
        auto* ResultImageMem = m_ResultGammaImage->GetMem(0, y);
        auto* ResultABSImageMem = m_ResultABSImage->GetMem(0, y);

        for (long x = 0; x < imageSizeX; x++)
        {
            ///// 일반 기울기
            /*int gradient = TopImageMem[x] - BottomImageMem[x];
			gradient = std::clamp(gradient + 128, 0, 255);
			ResultImageMem[x] = static_cast<uint8_t>(gradient);*/

            ///절대값
           int gradient = std::abs( TopImageMem[x] - BottomImageMem[x]);
			gradient = std::clamp(gradient * 2 , 0, 255);
           ResultABSImageMem[x] = static_cast<uint8_t>(gradient);

            //감마보정
            float g = float(TopImageMem[x] - BottomImageMem[x]) / (float(TopImageMem[x] + BottomImageMem[x]) + 1.0f);
            int vis = int((g * 0.5f + 0.5f) * 255.0f);
            vis = std::clamp(vis, 0, 255);

            float f = vis / 255.0f;
            f = std::pow(f, 0.7f);
            vis = int(f * 255.0f);

            ResultImageMem[x] = static_cast<uint8_t>(vis);
        }
    }
    
	float time = timeCheck.Elapsed_ms();
    
	CString strTimeCheck;
	strTimeCheck.Format(_T("%f"), time);
	
	TRACE1("%s\n", strTimeCheck);
  
	
	
	//UpdateEditInspectionItem();
	
	GetDlgItem(IDC_BUTTON_SHOW_BRIGHT_IMAGE)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_SHOW_DARK_IMAGE)->EnableWindow(true);
    GetDlgItem(IDC_BUTTON_IMAGESAVE)->SetWindowTextW(m_strImageName);
	
    return;
}

void CDlgInspectionMain::OnBnClickedButtonCellImageMultiSelect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (m_dlgBatch == nullptr)
    {
        m_dlgBatch = new CDlgBatchInpection;
        m_dlgBatch->Create(IDD_DLG_BATCH_INSPECTIONMODE, this);
        m_dlgBatch->CenterWindow(CWnd::GetDesktopWindow());
        m_dlgBatch->SetDlg(this);
    }
    m_dlgBatch->ShowWindow(SW_SHOW);

	/*m_vecMultiCellImage.clear();
	m_nMultiImageIndex = 0;

	const TCHAR szFilters[] =
		_T("Image files (*.bmp, *.png, *.jpg) | *.bmp;*.png;*.jpg|");

	CFileDialog dlg(TRUE, _T(""), _T(""), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT, szFilters);

	CString strFileList;
	const int nMaxFiles = 400;
	const int nBuffSize = (nMaxFiles * (MAX_PATH + 1)) + 1;
	dlg.GetOFN().lpstrFile = strFileList.GetBuffer(nBuffSize);
	dlg.GetOFN().nMaxFile = nBuffSize;

	if (dlg.DoModal() == IDOK)
	{
		POSITION pos;
		pos = dlg.GetStartPosition();
		CString strFullFilename;

		while (pos)
		{
			strFullFilename = dlg.GetNextPathName(pos);
			IPVM::Image_8u_C1 image;
			image.LoadFrom(strFullFilename);
			m_vecMultiCellImage.push_back(image);
		}

		IPVM::Image_8u_C1 cellImage, brightImage, darkImage;

		cellImage = m_vecMultiCellImage[m_nMultiImageIndex];

		int nImageWidth = cellImage.GetSizeX(), nImageHeight = cellImage.GetSizeY();

		brightImage.Create(nImageWidth, nImageHeight / 2);
		darkImage.Create(nImageWidth, nImageHeight / 2);

		int offsetBright, offsetDark;
		const BYTE* imageFull = cellImage.GetMem();

		if (imageFull[nImageWidth / 2] > imageFull[nImageWidth + nImageWidth / 2])
		{
			offsetBright = 0;
			offsetDark = nImageWidth;
		}
		else
		{
			offsetBright = nImageWidth;
			offsetDark = 0;
		}

		BYTE* byBrightImage = brightImage.GetMem();
		BYTE* byDarkImage = darkImage.GetMem();
		for (long i = 0; i < (nImageHeight / 2); i++)
		{
			memcpy(byBrightImage + (i * nImageWidth), imageFull + (offsetBright + 2 * i * nImageWidth), nImageWidth);
			memcpy(byDarkImage + (i * nImageWidth), imageFull + (offsetDark + 2 * i * nImageWidth), nImageWidth);
		}

		m_setupImage.m_imageBright[0] = m_setupImage.m_imageBright[1] = brightImage;
		m_setupImage.m_imageDark[0] = m_setupImage.m_imageDark[1] = darkImage;

		UpdateEditInspectionItem();
	}*/
}

LRESULT CDlgInspectionMain::OnUwmUpdateEditInspectionItem(WPARAM wParam, LPARAM lParam)
{
    UpdateEditInspectionItem();
    OnBnClickedButtonTestAll();
    return 0;
}

void CDlgInspectionMain::OnBnClickedButtonSettingUndo()
{
	auto* inspectionBase = m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);

	m_inspectionRoot->LinkDataBaseCommon(false, m_dbEdit_Common);
	inspectionBase->LinkDataBase(false, m_dbEdit_Module);
	UpdateInspectionParaGrid();
}

void CDlgInspectionMain::OnBnClickedButtonTest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (::GetKeyState(VK_SHIFT) < 0)
    {
        int nCnt = 1;
        nCnt = GetDlgItemInt(IDC_EDIT_AREAINDEX);
        UpdateEditInspectionItem(nCnt);
    }
	else
	{
		m_inspectionRoot->GetInspectionCell(m_curIsUp)->m_isTest = TRUE;
		if (m_curItemCategory != ProcItemCategory::END)
		{
			auto* inspectionBase = m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
			CInspectionCell* parent = m_inspectionRoot->GetInspectionCell(m_curIsUp);
			parent->SetInspectionSplitImage(m_inspectionImageBright, m_inspectionImageDark);
			parent->m_nAnodeCathode = m_inspectionRoot->m_nAnodeCathode;
			parent->m_nInsulateType = m_inspectionRoot->m_nInsulateType;
			parent->PrePareInsp();
			// 시간 측정 시작
			m_TestTimer.Clear();
			m_TestTimer.StartTimer();

			IPVM::TimeCheck timeCheck;
			timeCheck.Reset();

			parent->m_bInlineTest = false;
			bool bTimeout = false;
			//auto* config = (CInspectionConfiguration*)parent->GetProcessingItem(ProcItemCategory::electrodeTorn);
			auto* align = (CInspectionCellAlign*)parent->GetProcessingItem(ProcItemCategory::cellAlign);
	
			//config->DoInspection(&m_debugInfo);
#if INSPECTION_MACHINE_UC1
			align->DoInspection(&m_debugInfo);
			switch (m_curItemCategory)
			{
		

			case ProcItemCategory::cellAlign:
			/*	if (auto inspection = dynamic_cast<CInspectionCellAlign*>(inspectionBase))
				{
					inspection->DoInspection(&m_debugInfo);
				}*/
				break;

			case ProcItemCategory::abnormalInspection:
			{
                m_imageView->ROIClear();
                m_imageView->ImageOverlayClear();
				auto* abnormalInspection = (CInspectionAbnormal*)parent->GetProcessingItem(ProcItemCategory::abnormalInspection);
				if (align->m_resultValue == II_RESULT_VALUE::REJECT)
					return;
				abnormalInspection->DoInspection(&m_debugInfo);
				//ShowRejectROI_Event();
				break;
			}
			case ProcItemCategory::measure:
				m_imageView->ROIClear();
				if (auto inspection = dynamic_cast<CInspectionMeasure*>(inspectionBase))
				{
					//tab->DoInspection(&m_debugInfo);
					if (inspection->DoInspection(&m_debugInfo))
					{
						SaveMeasureResult();
                        ShowRejectROI();
					}
				}
                break;
            case ProcItemCategory::Chattering:
            {
                m_imageView->ROIClear();
                m_imageView->ImageOverlayClear();
                auto* inspection = (CInspectionBlob*)parent->GetProcessingItem(ProcItemCategory::Chattering);	
				if (align->m_resultValue == II_RESULT_VALUE::REJECT)
                    return;
                inspection->DoInspection(&m_debugInfo);
				break;
			}
			case ProcItemCategory::Blob:
			{
				timeCheck.Reset();
				auto* inspection = (CInspectionBlob*)parent->GetProcessingItem(ProcItemCategory::Blob);	
				if (align->m_resultValue == II_RESULT_VALUE::REJECT)
					return;
				bTimeout = inspection->DoInspection(&m_debugInfo);
				parent->GetSurfaceResult_ISI(true);
				ShowRejectROI_Event();
				break;
			}
			default:
				AfxMessageBox(_T("Under construction"));
				break;
			}
#else
            align->DoInspection(&m_debugInfo);
            switch (m_curItemCategory)
            {
                case ProcItemCategory::cellAlign:
                    /*	if (auto inspection = dynamic_cast<CInspectionCellAlign*>(inspectionBase))
				{
					inspection->DoInspection(&m_debugInfo);
				}*/
                    break;

                case ProcItemCategory::abnormalInspection:
                {
                    m_imageView->ROIClear();
                    m_imageView->ImageOverlayClear();
                    auto* abnormalInspection
                        = (CInspectionAbnormal*)parent->GetProcessingItem(ProcItemCategory::abnormalInspection);
                    if (align->m_resultValue == II_RESULT_VALUE::REJECT)
                        return;
                    abnormalInspection->DoInspection(&m_debugInfo);
                    //ShowRejectROI_Event();
                    break;
                }
                case ProcItemCategory::measure:
                    m_imageView->ROIClear();
                    if (auto inspection = dynamic_cast<CInspectionMeasure*>(inspectionBase))
                    {
                        //tab->DoInspection(&m_debugInfo);
                        if (inspection->DoInspection(&m_debugInfo))
                        {
                            SaveMeasureResult();
                            ShowRejectROI();
                        }
                    }
                    break;
                case ProcItemCategory::Chattering:
                {
                    m_imageView->ROIClear();
                    m_imageView->ImageOverlayClear();
                    auto* inspection = (CInspectionBlob*)parent->GetProcessingItem(ProcItemCategory::Chattering);
                    if (align->m_resultValue == II_RESULT_VALUE::REJECT)
                        return;
                    inspection->DoInspection(&m_debugInfo);
                    break;
                }
                case ProcItemCategory::Blob:
                {
                    timeCheck.Reset();
                    auto* inspection = (CInspectionBlob*)parent->GetProcessingItem(ProcItemCategory::Blob);
                    if (align->m_resultValue == II_RESULT_VALUE::REJECT)
                        return;
                    bTimeout = inspection->DoInspection(&m_debugInfo);
                    parent->GetSurfaceResult_ISI(true);
                    ShowRejectROI_Event();
                    break;
                }
                default:
                    AfxMessageBox(_T("Under construction"));
                    break;
            }
#endif
			float time = timeCheck.Elapsed_ms();

			// 시간 측정 완료
			m_TestTimer.EndTimer();
			double dElapTime = m_TestTimer.GetTime();
		
			UpdateInspectionTextInfoTestTime(time, UNIT_MILLI_SECOND, bTimeout);
			UpdateInspectionDebugInfoColor();

			//ShowRejectROI();
			if (m_curItemCategory == ProcItemCategory::cellAlign) //얼라인 후 결과 보여주기
			{
				auto& debugItem = m_debugInfo.GetAt(0);

				debugItem.UpdateImageView(m_imageView);
			}
		}
		else
		{
			CString title = LANG_TEXT(LanguageDef::Inspection);
			AfxGetApp()->m_pszAppName = title;

			AfxMessageBox(LANG_TEXT(LanguageDef::MsgSelectInspectionItem));
		}
		m_inspectionRoot->GetInspectionCell(m_curIsUp)->m_isTest = FALSE;
	}
}

void CDlgInspectionMain::OnBnClickedButtonTestAll()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_inspectionRoot->GetInspectionCell(m_curIsUp)->m_isTest = TRUE;

	
	ManualTestAll();
	m_inspectionRoot->GetInspectionCell(m_curIsUp)->m_isTest = FALSE;

}

BOOL CDlgInspectionMain::ManualTestAll(bool bUpdate)
{
	if (m_curItemCategory != ProcItemCategory::END)
	{
		auto* inspection = m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
		CInspectionCell* parent = m_inspectionRoot->GetInspectionCell(m_curIsUp);
		parent->SetInspectionSplitImage(m_inspectionImageBright, m_inspectionImageDark);
		parent->ResetResult();

		m_TestTimer.Clear();
		m_TestTimer.StartTimer();

		IPVM::TimeCheck timeCheck;
		timeCheck.Reset();
        //m_inspectionImageBright.SaveJpeg(_T("D:\\Bright.jpg"));
        //m_inspectionImageDark.SaveJpeg(_T("D:\\Dark.jpg"));
		parent->m_bInlineTest = false;
		if (parent->DoInspection(&m_debugInfo))
		{
			SaveMeasureResult();
            //parent->GetSurfaceResult_ISI();
            ShowRejectROI_Event();
		}
        else
        {
            m_nRejectType = parent->m_resultValue;
            return FALSE;      
		}

		m_nRejectType = parent->m_resultValue;

        if (parent->m_ISICellAllDefect.m_Judge.GetJudge() == REJECT)
        {
            m_bReject = TRUE;
		}
		else
        {
            m_bReject = FALSE;
		}
		float time = timeCheck.Elapsed_ms();

		m_TestTimer.EndTimer();
		double elapTime = m_TestTimer.GetTime();

		UpdateInspectionTextInfoTestTime(time, UNIT_MILLI_SECOND);
		UpdateInspectionDebugInfoColor();

		//ShowTotalRejectROI();
		ShowAllRejectROI();
	}
	else
	{
		CString title = LANG_TEXT(LanguageDef::Inspection);
		AfxGetApp()->m_pszAppName = title;

		AfxMessageBox(LANG_TEXT(LanguageDef::MsgSelectInspectionItem));

		return FALSE;
	}

	return TRUE;
}
void CDlgInspectionMain::ShowROI(bool isup, ProcItemCategory& ItemCategory)
{
	auto inspection = m_inspectionRoot->GetProcessingItem(isup, ItemCategory);
	CInspectionCell* parent = m_inspectionRoot->GetInspectionCell(isup);
	//auto* surface = dynamic_cast<CInspectionUncoatingSurface*>(inspection);
	//auto* uncoating = dynamic_cast<CInspectionUncoating*>(inspection);
	//auto* tape = dynamic_cast<CInspectionTape*>(inspection);
	//auto* scrab = dynamic_cast<CInspectionScrab*>(inspection);

	CString strDefectText;

	int n = 0;
	//long nSufaceDefectCount = 0;
	//int nTabDefectCount = 0;
	for (auto& rejectObj : inspection->m_rejectInfo)
	{
		if (CheckSurfaceDefect(rejectObj.m_detail))
		{
			COLORREF color;
			CString strName = rejectObj.m_detail;
			if (strName == parent->GetDefectName_G(-1) || strName == parent->GetDefectName_G(-2) || strName == parent->GetDefectName_G(-3) || strName == parent->GetDefectName_G(0)) {
				color = RGB(0, 255, 0);
				strName = _T("OK");
			}
			else
			{
				color = RGB(255, 0, 0);
			}

			strDefectText.Format(_T("%s\n"), strName);
			float SizeX_mm = rejectObj.m_width ;
			float SizeY_mm = rejectObj.m_height;
			strDefectText.AppendFormat(_T("X:%0.3f, Y:%0.3f"), SizeX_mm, SizeY_mm);

			m_imageView->ImageOverlayAdd(rejectObj.m_rect, color, 2);
			m_imageView->ImageOverlayAdd((IPVM::Point_32f_C2(rejectObj.m_rect.m_left, rejectObj.m_rect.m_top)), strDefectText, color, 5);
		}
		else
		{
			COLORREF color;
			CString strName = rejectObj.m_detail;
			if (strName == _T("OK")) {
				color = RGB(0, 255, 0);
			}
			else {
				color = RGB(255, 0, 0);
			}

			if (inspection->m_vecIsNoTabRect.size() > 0)
			{
				if (n < inspection->m_vecIsNoTabRect.size())
				{
					strDefectText = inspection->m_vecIsNoTabRect[n] ? _T("[No Tab]") : _T("");
					strName.Append(strDefectText);
					m_imageView->ImageOverlayAdd(IPVM::Point_32f_C2(rejectObj.m_rect.TopLeft()), strName, color, 5);
				}
			}
			else
			{
				strDefectText = rejectObj.m_detail;
				m_imageView->ImageOverlayAdd(IPVM::Point_32f_C2(rejectObj.m_rect.TopLeft()), strDefectText, color, 5);
			}
				m_imageView->ImageOverlayAdd(rejectObj.m_rect, color, 2);
		}
		n++;

	}

}
void CDlgInspectionMain::ShowRejectROI()
{
	m_imageView->ImageOverlayClear();

	ShowROI(m_curIsUp, m_curItemCategory);
	
	m_imageView->ImageOverlayShow();
}

void CDlgInspectionMain::ShowRejectROI_Event()
{
	CInspectionCell* parent = m_inspectionRoot->GetInspectionCell(m_curIsUp);
	m_imageView->ROIClear();
    m_imageView->ImageOverlayClear();
   // m_imageView->ImageOverlayClear();

   // ShowROI(m_curIsUp, m_curItemCategory);

    //m_imageView->ImageOverlayShow();
	COLORREF color;
	auto& Defects = parent->m_ISICellAllDefect.m_vecCandidate;
	int nIndex = -1;
	for (auto& Defect : Defects)
	{
		nIndex++;
		if (_T("OK") == Defect.strTempUI)
		{
			color = RGB(50, 200, 0);
			if (false == m_bShow_OK)
				continue;
		}
		else if (_T("SKIP") == Defect.strTempUI)
		{
			color = RGB(100, 100, 150);
			if (false == m_bShow_SKIP)
				continue;
		}
		else
		{
			color = RGB(200, 50, 0);
			if (false == m_bShow_NG)
				continue;
		}

		CString strDefectName, strXYSize;
		strDefectName = GetDefectName(Defect.GetDefectResultCode());
		strXYSize.Format(_T("%d.%s"), nIndex, strDefectName);
		IPVM::Rect rect = Defect.rtRect_pixel;
		IPVM::Point_32s_C2 pt = rect.TopLeft();
		pt.m_y -= 45;
		CString strKey;
		strKey.Format(_T("Defect_%d"), nIndex);
        if (Defect.HOR > 0 && Defect.VER > 0)
        {
            m_imageView->ROISet(strKey, strXYSize, rect, color, TRUE, FALSE, 80);
            m_imageView->ImageOverlayAdd(Defect.quadROI, RGB(100, 100, 150), 4);
		}
        //m_imageView->ImageOverlayAdd(rect, color, 8);
	}
    
	m_imageView->ROIShow();
    m_imageView->ImageOverlayShow();
}

void CDlgInspectionMain::ShowAllRejectROI()
{
	
	for (long categortIndex = 0; categortIndex < (long)ProcItemCategory::END; categortIndex++)
	{
		ShowROI(m_curIsUp, (ProcItemCategory&)categortIndex);
	}
	m_imageView->ImageOverlayShow();
}
bool CDlgInspectionMain::CheckSurfaceDefect(CString Detail)
{
	if ((Detail == _T("Line"))
		|| (Detail == _T("Scratch"))
		|| (Detail == _T("Line"))
		|| (Detail == _T("Heating Wrinkle"))
		|| (Detail == _T("Coating Wrinkle"))
		|| (Detail == _T("MiniPinhole"))
		|| (Detail == _T("Pinhole"))
		|| (Detail == _T("Delamination_Crack"))
		|| (Detail == _T("MiniDelamination_Crack"))
		|| (Detail == _T("Dent"))
		|| (Detail == _T("MiniDent"))
		|| (Detail == _T("MiniCrater"))
		|| (Detail == _T("Stain_Pinhole"))
		|| (Detail == _T("Crack"))
		|| (Detail == _T("None Coating"))
		|| (Detail == _T("Insulation Island"))
		|| (Detail == _T("Wrinkle"))
		|| (Detail == _T("Connection Tape"))
		|| (Detail == _T("Delamination_Crack"))
		|| (Detail == _T("Stain_Pinhole"))
		|| (Detail == _T("NGTag"))
		|| (Detail == _T("Pinhole"))
		|| (Detail == _T("Line"))
		|| (Detail == _T("Burr"))
		|| (Detail == _T("Dross"))
		|| (Detail == _T("Spatter"))
		|| (Detail == _T("Scrab"))
		|| (Detail == _T("White OK"))
		|| (Detail == _T("Black OK"))
		|| (Detail == _T("Island OK"))
		//|| (Detail == GetDefectName(DefectType::Island))
		|| (Detail == GetDefectName(DefectType::InsulationPinhole))//절연 핀홀
		|| (Detail == GetDefectName(DefectType::InsulationLine))//절연 라인
#ifdef _BURR_
		|| (Detail == GetDefectName(DefectType::Burr))
#endif
		|| (Detail == _T("OK")))
	{
		return true;
	}
	else
		return false;
}

void CDlgInspectionMain::OnBnClickedButtonCopyParamsSameItem()
{
	auto inspectionBase = (CInspectionBase*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
	bool bUpdownCheck = ((CButton*)GetDlgItem(IDC_CHECK_COPY_SAME_UPDOWN))->GetCheck();
	//현식 데이터카피 신경써야함
	switch (m_curItemCategory)
	{

	case ProcItemCategory::cellAlign:
		if (auto* item = dynamic_cast<CInspectionCellAlign*>(inspectionBase))
		{
			auto inspectionItem = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem((bool)TRUE, m_curItemCategory);
			auto bCopy = true;
			if (bUpdownCheck && (m_curIsUp != true)) bCopy = false;
			inspectionItem = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem((bool)TRUE, m_curItemCategory);
			if (bCopy) inspectionItem->m_para = item->m_para;

			bCopy = true;
			if (bUpdownCheck && (m_curIsUp != false)) bCopy = false;
			inspectionItem = (CInspectionCellAlign*)m_inspectionRoot->GetProcessingItem((bool)FALSE, m_curItemCategory);
			if (bCopy) inspectionItem->m_para = item->m_para;
		}
		break;


	case ProcItemCategory::measure:
		if (auto* item = dynamic_cast<CInspectionMeasure*>(inspectionBase))
		{
		}
	}
}

void CDlgInspectionMain::AllLamiObjectNotUse()
{
	
}

void CDlgInspectionMain::SaveMeasureResult()
{
	auto inspectionItem = (CInspectionMeasure*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::measure);
	//auto inspectionItemTabFolding = (CInspectionTabFolding*)m_inspectionRoot->GetProcessingItem(m_curIsUp, ProcItemCategory::tab);
	if (NULL != inspectionItem )
	{
		if (inspectionItem->m_para.m_bSaveMeasureResult)
		{
			CStdioFile file;
			CTime time = CTime::GetCurrentTime();
			CString filePath;
			CFileStatus fs;
			CString strLine;
			filePath.Format(_T("C:\\Intekplus\\IBNI\\MeasureResult_%04d%02d%02d.csv"), time.GetYear(), time.GetMonth(), time.GetDay());
			bool bExist = false;
			bool bFileOpen = false;
			if (CFile::GetStatus(filePath, fs))
			{
				bExist = true;
			}
			if (!bExist || file.m_hFile != INVALID_HANDLE_VALUE)
			{
				//file.Close();
				bFileOpen = file.Open(filePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
				if (!bFileOpen)
				{
					AfxMessageBox(_T("GageRnR Excel 파일 끄세요^^ "));
					return;
				}
				strLine = _T("ShoulderLine,");
				strLine += _T("Pitch,");
				strLine += _T("Length(Left),");
				strLine += _T("Length(Right),");
				strLine += _T("TabWidth,");
				strLine += _T("TabHeight,");
				strLine += _T("Misalign,");
				strLine += _T("Tilt1,");
				strLine += _T("Tilt2,");
				strLine += _T("Tab Side,");
				strLine += _T("Tab Folding Size X,");
				strLine += _T("Tab Folding Size Y,");
				strLine += _T("\n");
				file.WriteString(strLine);
			}
			else
			{
				bFileOpen = file.Open(filePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
				if (!bFileOpen)
				{
					AfxMessageBox(_T("GageRnR Excel 파일 끄세요^^ "));
					return;
				}
				if (bExist)
				{
					SetFilePointer(file.m_hFile, 0, NULL, FILE_END);
				}
				else
				{
					strLine = _T("ShoulderLine,");
					strLine += _T("Pitch,");
					strLine += _T("Length(Left),");
					strLine += _T("Length(Right),");
					strLine += _T("TabWidth,");
					strLine += _T("TabHeight,");
					strLine += _T("Misalign,");
					strLine += _T("Tilt1,");
					strLine += _T("Tilt2,");
					strLine += _T("Tab Side,");
					strLine += _T("Tab Folding Size X,");
					strLine += _T("Tab Folding Size Y,");
					strLine += _T("\n");
					file.WriteString(strLine);
				}
				m_ExcelOpen = true;
			}
			CStdioFile& Excelfile = file;
			
			CString strShoulderLine, strPitch, strLength1, strLength2, strTabWidth, strTabHeight, strMisalign, strTilt1, strTilt2, strTabSide, strTabFoldingSizeX, strTabFoldingSizeY;
			strShoulderLine.Format(_T("%.4f"), inspectionItem->m_unionResult.m_measureShoulderLine.GetValue_mm());
			strPitch.Format(_T("%.4f"), inspectionItem->m_measureTabPitch.GetValue_mm());
			strLength1.Format(_T("%.4f"), inspectionItem->m_measureCellLengthTop.GetValue_mm());
			strLength2.Format(_T("%.4f"), inspectionItem->m_measureCellLengthBot.GetValue_mm());
			strTabWidth.Format(_T("%.4f"), inspectionItem->m_measureTabWidth.GetValue_mm());
			strTabHeight.Format(_T("%.4f"), inspectionItem->m_measureTabHeight.GetValue_mm());
			strMisalign.Format(_T("%.4f"), inspectionItem->m_measureMisalign.GetValue_mm());
			strTilt1.Format(_T("%.4f"), inspectionItem->m_measureVHomeTiltBot.GetValue_mm());
			strTilt2.Format(_T("%.4f"), inspectionItem->m_measureVHomeTiltTop.GetValue_mm());
			strTabSide.Format(_T("%.4f"), inspectionItem->m_measureTabSide.GetValue_mm());

		

			strLine = strShoulderLine + _T(",");
			strLine += strPitch + _T(",");
			strLine += strLength1 + _T(",");
			strLine += strLength2 + _T(",");
			strLine += strTabWidth + _T(",");
			strLine += strTabHeight + _T(",");
			strLine += strMisalign + _T(",");
			strLine += strTilt1 + _T(",");
			strLine += strTilt2 + _T(",");
			strLine += strTabSide + _T(",");
			strLine += strTabFoldingSizeX + _T(",");
			strLine += strTabFoldingSizeY + _T(",");
			strLine += _T("\n");
			file.WriteString(strLine);
			file.Close();
			/*BOOL res = file.Open(filePath, CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate);
			if (res)
			{
			ULONGLONG nend = file.SeekToEnd();
			CString str;
			if (nend == 0)
			{
			str = _T("ShoulderLine, Pitch, Length1, Legnth2, Tab Width, Tab Height, Misalign, Tilt1, Tilt2, Tab Side\n");
			file.WriteString(str);
			}

			str.Format(_T("%.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f\n"), inspectionItem->m_unionResult.m_lineShoulderValue_um * 0.001, inspectionItem->m_fCellPitchUm * 0.001, inspectionItem->m_fCellLengthTopUm * 0.001, inspectionItem->m_fCellLengthBotUm * 0.001, inspectionItem->m_fTabWidthUm * 0.001, inspectionItem->m_fTabHeightUm * 0.001, inspectionItem->m_fMisalignUm * 0.001, inspectionItem->m_fVhomeTiltBotUm * 0.001, inspectionItem->m_fVhomeTiltTopUm * 0.001, inspectionItem->m_fTabSideUm * 0.001);
			file.WriteString(str);
			file.Close();
			}*/
		}
	}
}

void CDlgInspectionMain::OnBnClickedButtonShowBrightImage()
{
    /*bisBrightImage = true;
    m_imageView->ImageOverlayClear();
    m_imageView->SetImage(m_inspectionImageBright);
    m_imageView->ImageOverlayShow();
    if (::GetKeyState(VK_SHIFT) < 0)
    {
        auto* inspection = (CInspectionBlob*)m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
        m_imageView->SetImage(inspection->m_bufferThreshold);
	}
 
	m_curSelImage = &m_inspectionImageBright;*/

	 m_imageView->SetImage(*m_ResultGammaImage);

}

void CDlgInspectionMain::OnBnClickedButtonShowDarkImage()
{
   /* bisBrightImage = false; 
	m_imageView->ImageOverlayClear();
	m_imageView->SetImage(m_inspectionImageDark);
	m_imageView->ImageOverlayShow();

	m_curSelImage = &m_inspectionImageDark;*/
    m_imageView->SetImage(*m_ResultABSImage);
}

void CDlgInspectionMain::ShowBatchInspRejectROI()
{
    m_inspectionRoot->GetInspectionCell(m_curIsUp)->m_isTest = TRUE;
    if (m_curItemCategory != ProcItemCategory::END)
    {
        auto* inspectionBase = m_inspectionRoot->GetProcessingItem(m_curIsUp, m_curItemCategory);
        CInspectionCell* parent = m_inspectionRoot->GetInspectionCell(m_curIsUp);
        parent->SetInspectionSplitImage(m_inspectionImageBright, m_inspectionImageBright);
        parent->m_nAnodeCathode = m_inspectionRoot->m_nAnodeCathode;
        parent->m_nInsulateType = m_inspectionRoot->m_nInsulateType;
        parent->PrePareInsp();
        // 시간 측정 시작
        m_TestTimer.Clear();
        m_TestTimer.StartTimer();

        IPVM::TimeCheck timeCheck;
        timeCheck.Reset();

        parent->m_bInlineTest = false;
        bool bTimeout = false;
        //auto* config = (CInspectionConfiguration*)parent->GetProcessingItem(ProcItemCategory::electrodeTorn);
        auto* align = (CInspectionCellAlign*)parent->GetProcessingItem(ProcItemCategory::cellAlign);

        //config->DoInspection(&m_debugInfo);
        align->DoInspection(&m_debugInfo);
        switch (m_curItemCategory)
        {
            case ProcItemCategory::cellAlign:
                if (auto inspection = dynamic_cast<CInspectionCellAlign*>(inspectionBase))
				{
					inspection->DoInspection(&m_debugInfo);
				}
                break;

            case ProcItemCategory::abnormalInspection:
            {
                auto* abnormalInspection = (CInspectionAbnormal*)parent->GetProcessingItem(ProcItemCategory::abnormalInspection);
                if (align->m_resultValue == II_RESULT_VALUE::REJECT)
                    return;
                abnormalInspection->DoInspection(&m_debugInfo);
                //ShowRejectROI_Event();
                break;
            }
            case ProcItemCategory::measure:
                m_imageView->ROIClear();
                if (auto inspection = dynamic_cast<CInspectionMeasure*>(inspectionBase))
                {
                    //tab->DoInspection(&m_debugInfo);
                    if (inspection->DoInspection(&m_debugInfo))
                    {
                        SaveMeasureResult();
                        ShowRejectROI();
                    }
                }
                break;

            case ProcItemCategory::Blob:
            {
                timeCheck.Reset();
                auto* inspection = (CInspectionBlob*)parent->GetProcessingItem(ProcItemCategory::Blob);
                if (align->m_resultValue == II_RESULT_VALUE::REJECT)
                    return;
                bTimeout = inspection->DoInspection(&m_debugInfo);
				//이것도 속도 줄이기
                parent->GetSurfaceResult_ISI();
                ShowRejectROI_Event();
                break;
            }
            default:
                AfxMessageBox(_T("Under construction"));
                break;
        }

        float time = timeCheck.Elapsed_ms();

        // 시간 측정 완료
        m_TestTimer.EndTimer();
        double dElapTime = m_TestTimer.GetTime();

        UpdateInspectionTextInfoTestTime(time, UNIT_MILLI_SECOND, bTimeout);
        UpdateInspectionDebugInfoColor();

        //ShowRejectROI();
        if (m_curItemCategory == ProcItemCategory::cellAlign) //얼라인 후 결과 보여주기
        {
            auto& debugItem = m_debugInfo.GetAt(0);

            debugItem.UpdateImageView(m_imageView);
        }
    }
    else
    {
        //CString title = LANG_TEXT(LanguageDef::Inspection);
        //AfxGetApp()->m_pszAppName = title;

        //AfxMessageBox(LANG_TEXT(LanguageDef::MsgSelectInspectionItem));
    }
    m_inspectionRoot->GetInspectionCell(m_curIsUp)->m_isTest = FALSE;
}
