#include "pch.h"
#include "ViewCellImageUpdater.h"
//#include "SystemParameter.h"

#include "Types/Point_32f_C2.h"
#include "Algorithm/ImageProcessing.h"
#include "Gadget/TimeCheck.h"
#include "Widget/ImageView.h"
#include "../Grab/GrabExecutor.h"
#include "SystemParameter.h"

#include "../Inspection/CISICellAllDefect.h"
#include "../Inspection/CISIDefectProperty.h"
#include "../IBCI/DlgViewCellImage.h"
#include "../Inspection/InspectionRoot.h"
#include "../Inspection/Accessor.h"
#include "IBCIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ViewCellImageUpdater::ViewCellImageUpdater(MachineType machineType, IPVM::ImageView &imageViewBright, IPVM::ImageView &imageViewDark, IPVM::ImageView &imageViewBright2, IPVM::ImageView &imageViewDark2)
	: m_imageViewBright(imageViewBright)
	, m_imageViewDark(imageViewDark)
    , m_imageViewBright2(imageViewBright2)
    , m_imageViewDark2(imageViewDark2)
    , m_timeCheck(new IPVM::TimeCheck)
	, m_srcImageBright(new IPVM::Image_8u_C1)
	, m_srcImageDark(new IPVM::Image_8u_C1)
	, m_resizedFullImage(new IPVM::Image_8u_C1)
	, m_resizedHalfImage(new IPVM::Image_8u_C1)
    , m_cellId(LONG_MAX)
    , m_pISICellAllDefect(NULL)
    , m_bInlineCheck(TRUE)
    , m_machineType(machineType)
    , m_bSurfaceInspCheck(FALSE)
{
	__super::BeginWorker();
    if(g_systemParameter.machineType == MachineType::UC1_mismatch)
    {
        m_bInlineCheck = FALSE;
    }
}

ViewCellImageUpdater::~ViewCellImageUpdater()
{
	__super::EndWorker();

	delete m_resizedHalfImage;
	delete m_resizedFullImage;
	delete m_srcImageBright;
	delete m_srcImageDark;
	delete m_timeCheck;
}

void ViewCellImageUpdater::Push(const IPVM::Image_8u_C1& bright, const IPVM::Image_8u_C1& dark, long CellId, CISICellAllDefect& ISICellAllDefect)
{
    CSingleLock lock(&m_cs, TRUE);

    // 이미지 뷰어는 정확한 타이밍에 정확한 이미지를 그릴 필요가 없으므로,
    // 얕은 카피로 포인터만 받아오는 것으로 한다.
    *m_srcImageBright = bright;
    *m_srcImageDark = dark;

    m_cellId = CellId;
    m_pISICellAllDefect = &ISICellAllDefect;

    __super::Trigger();
}

void ViewCellImageUpdater::Push(const IPVM::Image_8u_C1& bright, const IPVM::Image_8u_C1& dark, long CellId)
{
	CSingleLock lock(&m_cs, TRUE);

	// 이미지 뷰어는 정확한 타이밍에 정확한 이미지를 그릴 필요가 없으므로,
	// 얕은 카피로 포인터만 받아오는 것으로 한다.
	*m_srcImageBright = bright;
	*m_srcImageDark = dark;

	m_cellId = CellId;

	__super::Trigger();
}

void ViewCellImageUpdater::Push(const IPVM::Image_8u_C1& bright, const IPVM::Image_8u_C1& dark)
{
	CSingleLock lock(&m_cs, TRUE);

	// 이미지 뷰어는 정확한 타이밍에 정확한 이미지를 그릴 필요가 없으므로,
	// 얕은 카피로 포인터만 받아오는 것으로 한다.
	*m_srcImageBright = bright;
	*m_srcImageDark = dark;

	__super::Trigger();
}

void ViewCellImageUpdater::Push()
{
    CSingleLock lock(&m_cs, TRUE);
    __super::Trigger();
}

bool ViewCellImageUpdater::OnTrigger()
{
	// 이 값을 줄이면 메인 쓰레드 CPU 부하가 커짐.. 줄이지 말 것.
	if (m_timeCheck->Elapsed_ms() < 100.f)
	{
		return true;
	}
		
	m_timeCheck->Reset();

    //jjy
  	IPVM::Image_8u_C1 imageBright, imageDark, imageBright2, imageDark2;
    {

        if (m_machineType == MachineType::ES_rollpress_pinhole)
        {
            CSingleLock lock(&m_cs, TRUE);

            imageBright.Create(m_srcImageBright->GetSizeX() / 2, m_srcImageBright->GetSizeY());
            imageBright2.Create(m_srcImageBright->GetSizeX() / 2, m_srcImageBright->GetSizeY());
            imageDark.Create(m_srcImageDark->GetSizeX() / 2, m_srcImageDark->GetSizeY());
            imageDark2.Create(m_srcImageDark->GetSizeX() / 2, m_srcImageDark->GetSizeY());

            IPVM::Rect copyRectBright(imageBright2);
            IPVM::ImageProcessing::Copy(*m_srcImageBright, copyRectBright, IPVM::Rect(imageBright2), imageBright2);
            copyRectBright.MoveToX(imageBright.GetSizeX());
            IPVM::ImageProcessing::Copy(*m_srcImageBright, copyRectBright, IPVM::Rect(imageBright), imageBright);

            IPVM::Rect copyRectDark(imageDark2);
            IPVM::ImageProcessing::Copy(*m_srcImageDark, copyRectDark, IPVM::Rect(imageDark2), imageDark2);
            copyRectDark.MoveToX(imageDark.GetSizeX());
            IPVM::ImageProcessing::Copy(*m_srcImageDark, copyRectDark, IPVM::Rect(imageDark), imageDark);
        }
        else
        {
            imageBright = *m_srcImageBright;
            imageDark = *m_srcImageDark;
        }
    }

	
	{
        //imageBright.SaveBmp(_T("C:\\aaaa\\cellBright.bmp"));
        //imageDark.SaveBmp(_T("C:\\aaaa\\cellDark.bmp"));
        if (m_bSurfaceInspCheck == FALSE)
        {
            // 인라인인 경우
            if (m_bInlineCheck == TRUE)
            {
                UpdateImageView(imageBright, imageDark, m_imageViewBright, m_imageViewDark);

                if (m_machineType == MachineType::ES_rollpress_pinhole)
                {
                    UpdateImageView(imageBright2, imageDark2, m_imageViewBright2, m_imageViewDark2);
                }
            }
            
            // 베치인 경우
            else
            {
                UpdateImageView_Batch(imageBright, imageDark, m_imageViewBright, m_imageViewDark);

                if (m_machineType == MachineType::ES_rollpress_pinhole)
                {
                    UpdateImageView_Batch(imageBright2, imageDark2, m_imageViewBright2, m_imageViewDark2);
                }
            }
        }

    }

    //if (m_pISICellAllDefect == NULL && m_bInlineCheck==FALSE)
    //{
    //    SurfaceInsp();
    //    if (g_systemParameter.machineType != MachineType::UC1_mismatch)
    //    {
    //        m_bInlineCheck = TRUE;
    //    }
    //}
    //else if (m_nInlineMode == ENUM_VIEW_MODE_CELL_IMAGE::DO_NOT_UPDATE && m_bInlineCheck == TRUE)
    //{
    //    SurfaceInsp();
    //}

    if (m_bInlineCheck == FALSE && m_bSurfaceInspCheck == TRUE
        || m_nInlineMode == ENUM_VIEW_MODE_CELL_IMAGE::DO_NOT_UPDATE && m_bInlineCheck == TRUE
            && m_bSurfaceInspCheck == TRUE)
    {
        SurfaceInsp();
        m_bSurfaceInspCheck = FALSE;
    }


	return true;
}

void ViewCellImageUpdater::SetGrayMode(long nType)
{
    m_nGrayType = nType;
}

void ViewCellImageUpdater::SetInspMode(long nType, BOOL bInlineCheck, BOOL bBatch)
{
    m_nModeInspection = nType;
    m_bInlineCheck = bInlineCheck;
    m_bBatchCheck = bBatch;

}

//void ViewCellImageUpdater::SurfaceInsp()
//{
//    CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell((bool)true);
//    auto* itemCellAlign = (CInspectionCellAlign*)unit->GetProcessingItem(ProcItemCategory::cellAlign);
//
//    CString strKey, strName;
//
//    if (m_nModeInspection == 0)
//    {
//        for (long nCnt = 0; nCnt < itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect.size()-1; nCnt++)
//        {
//            strName = _T("align");
//            strKey.Format(_T("Align_%d"), nCnt);
//            IPVM::Rect rtAlign = unit->m_unionResult.m_rectAreaMeasure[nCnt];
//            m_imageViewBright.ROISet(strKey, strName, rtAlign, RGB(255, 80, 180), 5);
//            m_imageViewDark.ROISet(strKey, strName, rtAlign, RGB(255, 80, 180), 5);
//        }
//    }
//
//    if (m_nModeInspection == 1)
//    {
//        for (long nCnt = 0; nCnt < unit->m_ISICellAllDefect.m_vecCandidate.size(); nCnt++)
//        {
//            long nType = unit->m_ISICellAllDefect.m_vecISIDefectType[nCnt];
//            strName = GetDefectName((DefectType)nType);
//            strKey.Format(_T("Defect_%d"), nCnt);
//            IPVM::Rect rect = unit->m_ISICellAllDefect.m_vecCandidate[nCnt].rtRect_pixel;
//            m_imageViewBright.ROISet(strKey, strName, rect, RGB(255, 0, 0), 50);
//            m_imageViewDark.ROISet(strKey, strName, rect, RGB(255, 0, 0), 50);
//        }
//    }
//
//    if (m_nModeInspection == 2)
//    {
//        IPVM::LineSeg_32f line_center;
//
//        for (long nCnt = 0; nCnt < unit->m_ISICellAllDefect.m_vecCandidate.size(); nCnt++)
//        {
//            long nType = unit->m_ISICellAllDefect.m_vecISIDefectType[nCnt];
//            strName = GetDefectName((DefectType)nType);
//            strKey.Format(_T("Defect_%d"), nCnt);
//            IPVM::Rect rect = unit->m_ISICellAllDefect.m_vecCandidate[nCnt].rtRect_pixel;
//            m_imageViewBright.ROISet(strKey, strName, rect, RGB(255, 0, 0), 50);
//            m_imageViewDark.ROISet(strKey, strName, rect, RGB(255, 0, 0), 50);
//        }
//
//        for (long nCnt = 0; nCnt < itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect.size() - 1; nCnt++)
//        {
//            IPVM::Rect rtAlign = unit->m_unionResult.m_rectAreaMeasure[nCnt];
//            if (nCnt==0)
//            {
//                line_center.m_sx = unit->m_unionResult.m_rectAreaMeasure[nCnt].m_left;
//                line_center.m_sy = unit->m_unionResult.m_rectAreaMeasure[nCnt].CenterPoint().m_y;
//            }
//            if (nCnt == itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect.size() - 2)
//            {
//                line_center.m_ex = unit->m_unionResult.m_rectAreaMeasure[nCnt].m_right;
//                line_center.m_ey = unit->m_unionResult.m_rectAreaMeasure[nCnt].CenterPoint().m_y;
//            }
//        }
//        m_imageViewBright.ImageOverlayAdd(line_center, RGB(255, 0, 0), 1);
//        m_imageViewDark.ImageOverlayAdd(line_center, RGB(255, 0, 0), 1);
//    }
//
//    m_imageViewBright.SetImage(unit->m_inspectionImageBright);
//    m_imageViewBright.ImageOverlayShow();
//    m_imageViewBright.ROIShow();
//
//    m_imageViewDark.SetImage(unit->m_inspectionImageDark);
//    m_imageViewDark.ImageOverlayShow();
//    m_imageViewDark.ROIShow();
//}

void ViewCellImageUpdater::SetInlineUpdateMode(long nType)
{
    m_nInlineMode = nType;
}

void ViewCellImageUpdater::SurfaceInsp() 
{
    //CInspectionCell* unit = Inspection::Accessor::GetRoot().GetInspectionCell((bool)true);
    auto* itemCellAlign = (CInspectionCellAlign*)m_pInspectionCell->GetProcessingItem(ProcItemCategory::cellAlign);

    CString strKey, strName;

    if (m_nModeInspection == 0)
    {
        for (long nCnt = 0; nCnt < itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect.size() - 1; nCnt++)
        {
            strName = _T("align");
            strKey.Format(_T("Align_%d"), nCnt);
            IPVM::Rect rtAlign = m_pInspectionCell->m_unionResult.m_rectAreaMeasure[nCnt];
            m_imageViewBright.ROISet(strKey, strName, rtAlign, RGB(255, 80, 180), 5);
            m_imageViewDark.ROISet(strKey, strName, rtAlign, RGB(255, 80, 180), 5);
        }
    }

    if (m_nModeInspection == 1)
    {
        for (long nCnt = 0; nCnt < m_pInspectionCell->m_ISICellAllDefect.m_vecCandidate.size(); nCnt++)
        {
            long nType = m_pInspectionCell->m_ISICellAllDefect.m_vecISIDefectType[nCnt];
            strName = GetDefectName((DefectType)nType);
            strKey.Format(_T("Defect_%d"), nCnt);
            IPVM::Rect rect = m_pInspectionCell->m_ISICellAllDefect.m_vecCandidate[nCnt].rtRect_pixel;
            m_imageViewBright.ROISet(strKey, strName, rect, RGB(255, 0, 0), 50);
            m_imageViewDark.ROISet(strKey, strName, rect, RGB(255, 0, 0), 50);
        }
    }

    if (m_nModeInspection == 2)
    {
        IPVM::LineSeg_32f line_center;

        for (long nCnt = 0; nCnt < m_pInspectionCell->m_ISICellAllDefect.m_vecCandidate.size(); nCnt++)
        {
            long nType = m_pInspectionCell->m_ISICellAllDefect.m_vecISIDefectType[nCnt];
            strName = GetDefectName((DefectType)nType);
            strKey.Format(_T("Defect_%d"), nCnt);
            IPVM::Rect rect = m_pInspectionCell->m_ISICellAllDefect.m_vecCandidate[nCnt].rtRect_pixel;
            m_imageViewBright.ROISet(strKey, strName, rect, RGB(255, 0, 0), 50);
            m_imageViewDark.ROISet(strKey, strName, rect, RGB(255, 0, 0), 50);
        }

        for (long nCnt = 0; nCnt < itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect.size() - 1; nCnt++)
        {
            IPVM::Rect rtAlign = m_pInspectionCell->m_unionResult.m_rectAreaMeasure[nCnt];
            if (nCnt == 0)
            {
                line_center.m_sx = m_pInspectionCell->m_unionResult.m_rectAreaMeasure[nCnt].m_left;
                line_center.m_sy = m_pInspectionCell->m_unionResult.m_rectAreaMeasure[nCnt].CenterPoint().m_y;
            }
            if (nCnt == itemCellAlign->m_paraEdgeDetectMeasure.m_vecrtDetect.size() - 2)
            {
                line_center.m_ex = m_pInspectionCell->m_unionResult.m_rectAreaMeasure[nCnt].m_right;
                line_center.m_ey = m_pInspectionCell->m_unionResult.m_rectAreaMeasure[nCnt].CenterPoint().m_y;
            }
        }
        m_imageViewBright.ImageOverlayAdd(line_center, RGB(255, 0, 0), 1);
        m_imageViewDark.ImageOverlayAdd(line_center, RGB(255, 0, 0), 1);
    }

    m_imageViewBright.SetImage(m_pInspectionCell->m_inspectionImageBright);
    m_imageViewBright.ImageOverlayShow();
    m_imageViewBright.ROIShow();

    m_imageViewDark.SetImage(m_pInspectionCell->m_inspectionImageDark);
    m_imageViewDark.ImageOverlayShow();
    m_imageViewDark.ROIShow();
}

void ViewCellImageUpdater::UpdateImageView_Batch(IPVM::Image_8u_C1& imageBright, IPVM::Image_8u_C1& imageDark, IPVM::ImageView& imageViewBright, IPVM::ImageView& imageViewDark)
{
    imageViewBright.ImageOverlayClear();
    imageViewBright.ROIClear();
    imageViewDark.ImageOverlayClear();
    imageViewDark.ROIClear();

    long brightImageSizeX = imageBright.GetSizeX() ;
    long brightImageSizeY = imageBright.GetSizeY();

    long darkImageSizeX = imageDark.GetSizeX() ;
    long darkImageSizeY = imageDark.GetSizeY() ;

    m_resizedHalfImage->Create(brightImageSizeX, brightImageSizeY);
   // IPVM::ImageProcessing::Resize_Nearest(imageBright, *m_resizedHalfImage);
    imageViewBright.SetImage(*m_resizedHalfImage);

    m_resizedHalfImage->Create(darkImageSizeX, darkImageSizeY);
    //IPVM::ImageProcessing::Resize_Nearest(imageDark, *m_resizedHalfImage);
    imageViewDark.SetImage(*m_resizedHalfImage);

    if (m_cellId != LONG_MAX)
    {
        CString cellid;
        cellid.Format(_T("%d"), m_cellId);

        imageViewBright.ImageOverlayAdd(IPVM::Point_32f_C2(3, 3), cellid, RGB(0, 128, 0), 100);
    }

    if (m_bBatchCheck==TRUE)
    {
        // 1. align 박스 표시
        auto* itemCellAlign = (CInspectionCellAlign*)m_pInspectionCell->GetProcessingItem(ProcItemCategory::cellAlign);

        for (long nCnt = 0; nCnt < itemCellAlign->m_paraEdgeDetectBlob.m_vecrtDetect.size() - 1; nCnt++)
        {
            CString strName = _T("align");
            CString strKey;
            strKey.Format(_T("Align_%d"), nCnt);
            IPVM::Rect rtAlign = itemCellAlign->m_unionResult.m_rectAreaBlob[nCnt];

            // 이미지 1/4 배 한 것임으로 ROI 사이즈 조정
            rtAlign.m_left /= 4;
            rtAlign.m_right /= 4;
            rtAlign.m_top /= 4;
            rtAlign.m_bottom /= 4;
            imageViewBright.ImageOverlayAdd(rtAlign, RGB(255, 80, 180), 1);
            imageViewDark.ImageOverlayAdd(rtAlign, RGB(255, 80, 180), 1);
        }
        imageViewBright.ImageOverlayShow();
        imageViewDark.ImageOverlayShow();

        // 2. blob 박스 표시
        if (m_pISICellAllDefect != NULL)
        {
            auto& Defects = m_pISICellAllDefect->m_vecCandidate;
            int nIndex = -1;
            COLORREF color;
            for (auto& Defect : Defects)
            {
                nIndex++;
                if (_T("NG") == Defect.strTempUI)
                {
                    long nType = m_pISICellAllDefect->m_vecISIDefectType[nIndex];
                    color = RGB(200, 50, 0);
                    Defect;
                    CString strDefectName, strXYSize;
                    //strDefectName = GetDefectName(Defect.GetDefectResultCode());
                    //strDefectName = _T("Defect");
                    strDefectName = GetDefectName((DefectType)nType);
                    strXYSize.Format(_T("%d.%s"), nIndex, strDefectName);
                    IPVM::Rect rect = Defect.rtRect_pixel;

                    // Rect 1/4
                    rect.m_top /= 4;
                    rect.m_left /= 4;
                    rect.m_bottom /= 4;
                    rect.m_right /= 4;
                    IPVM::Point_32s_C2 pt = rect.TopLeft();
                    pt.m_y -= 45;
                    CString strKey;
                    strKey.Format(_T("Defect_%d"), nIndex);

                    IPVM::Quadrangle_32f quadROI(Defect.quadROI);
                    quadROI.m_fLBx /= 4;
                    quadROI.m_fLBy /= 4;
                    quadROI.m_fLTx /= 4;
                    quadROI.m_fLTy /= 4;
                    quadROI.m_fRBx /= 4;
                    quadROI.m_fRBy /= 4;
                    quadROI.m_fRTx /= 4;
                    quadROI.m_fRTy /= 4;

                    if (Defect.HOR > 0 && Defect.VER > 0)
                    {
                        imageViewBright.ROISet(strKey, strXYSize, rect, color, TRUE, FALSE, 80);
                        imageViewBright.ImageOverlayAdd(quadROI, RGB(100, 100, 150), 4);

                        imageViewDark.ROISet(strKey, strXYSize, rect, color, TRUE, FALSE, 80);
                        imageViewDark.ImageOverlayAdd(quadROI, RGB(100, 100, 150), 4);
                    }
                }
                //m_imageView->ImageOverlayAdd(rect, color, 8);
                imageViewBright.ImageOverlayShow();
                imageViewBright.ROIShow();

                imageViewDark.ImageOverlayShow();
                imageViewDark.ROIShow();
            }
            m_pISICellAllDefect = NULL;
        }
    }
    
    long resizeHalfimageSizeX = m_resizedHalfImage->GetSizeX();
    long resizeHalfimageSizeY = m_resizedHalfImage->GetSizeY();

    if (m_nGrayType == 1) //bright
    {
        IPVM::LineSeg_32f lineMax;
        lineMax.m_sx = 0;
        lineMax.m_sy = resizeHalfimageSizeY / 2;
        lineMax.m_ex = resizeHalfimageSizeX;
        lineMax.m_ey = resizeHalfimageSizeY / 2;
        IPVM::Point_32f_C2 ptMax(lineMax.m_sx, lineMax.m_sy);

        IPVM::LineSeg_32f lineMin;
        lineMin.m_sx = 0;
        lineMin.m_sy = resizeHalfimageSizeY;
        lineMin.m_ex = resizeHalfimageSizeX;
        lineMin.m_ey = resizeHalfimageSizeY;
        IPVM::Point_32f_C2 ptMin(lineMin.m_sx, lineMin.m_sy);

        IPVM::LineSeg_32f line_50;
        line_50.m_sx = 0;
        line_50.m_sy = resizeHalfimageSizeY - ((50 * (resizeHalfimageSizeY / 2)) / 255);
        line_50.m_ex = resizeHalfimageSizeX;
        line_50.m_ey = resizeHalfimageSizeY - ((50 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_50(line_50.m_sx, line_50.m_sy);

        IPVM::LineSeg_32f line_100;
        line_100.m_sx = 0;
        line_100.m_sy = resizeHalfimageSizeY - ((100 * (resizeHalfimageSizeY / 2)) / 255);
        line_100.m_ex = resizeHalfimageSizeX;
        line_100.m_ey = resizeHalfimageSizeY - ((100 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_100(line_100.m_sx, line_100.m_sy);

        IPVM::LineSeg_32f line_150;
        line_150.m_sx = 0;
        line_150.m_ey = resizeHalfimageSizeY - ((150 * (resizeHalfimageSizeY / 2)) / 255);
        line_150.m_sy = resizeHalfimageSizeY - ((150 * (resizeHalfimageSizeY / 2)) / 255);
        line_150.m_ex = resizeHalfimageSizeX;
        IPVM::Point_32f_C2 pt_150(line_150.m_sx, line_150.m_sy);

        IPVM::LineSeg_32f line_200;
        line_200.m_sx = 0;
        line_200.m_sy = resizeHalfimageSizeY - ((200 * (resizeHalfimageSizeY / 2)) / 255);
        line_200.m_ex = resizeHalfimageSizeX;
        line_200.m_ey = resizeHalfimageSizeY - ((200 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_200(line_200.m_sx, line_200.m_sy);

        imageViewBright.ImageOverlayAdd(lineMax, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(line_50, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(line_100, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(line_150, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(line_200, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(lineMin, RGB(0, 255, 0), 1);

        imageViewBright.ImageOverlayAdd(ptMax, _T("255"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(pt_50, _T("50"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(pt_100, _T("100"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(pt_150, _T("150"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(pt_200, _T("200"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(ptMin, _T("0"), RGB(0, 255, 0), 50);

        BYTE* pData = m_resizedHalfImage->GetMem();

        std::vector<IPVM::Point_32f_C2> vecGVHalfLinePoint;
        for (long nX = 0; nX < resizeHalfimageSizeX; nX++)
        {
            IPVM::Point_32f_C2 pt;
            long nValue = m_resizedHalfImage->GetSubpixValue(nX, resizeHalfimageSizeY / 2);
            pt.m_x = nX;
            pt.m_y = resizeHalfimageSizeY - ((nValue * (resizeHalfimageSizeY / 2)) / 255);
            vecGVHalfLinePoint.push_back(pt);

            imageViewBright.ImageOverlayAdd(pt, RGB(255, 0, 0), 1);
        }
        imageViewBright.ImageOverlayShow();
        imageViewBright.ROIShow();

        const long nHorLineCount = (long)vecGVHalfLinePoint.size();
        std::vector<IPVM::LineEq_32f> vecHorLineEquations;
        vecHorLineEquations.resize(nHorLineCount);

        //imageViewDark.ImageOverlayHide();
        //imageViewDark.ImageOverlayClear();
        //imageViewDark.ImageOverlayShow();
    }
    else if (m_nGrayType == 2) // Dark
    {
        IPVM::LineSeg_32f lineMax;
        lineMax.m_sx = 0;
        lineMax.m_sy = resizeHalfimageSizeY / 2;
        lineMax.m_ex = resizeHalfimageSizeX;
        lineMax.m_ey = resizeHalfimageSizeY / 2;
        IPVM::Point_32f_C2 ptMax(lineMax.m_sx, lineMax.m_sy);

        IPVM::LineSeg_32f lineMin;
        lineMin.m_sx = 0;
        lineMin.m_sy = resizeHalfimageSizeY;
        lineMin.m_ex = resizeHalfimageSizeX;
        lineMin.m_ey = resizeHalfimageSizeY;
        IPVM::Point_32f_C2 ptMin(lineMin.m_sx, lineMin.m_sy);

        IPVM::LineSeg_32f line_50;
        line_50.m_sx = 0;
        line_50.m_sy = resizeHalfimageSizeY - ((50 * (resizeHalfimageSizeY / 2)) / 255);
        line_50.m_ex = resizeHalfimageSizeX;
        line_50.m_ey = resizeHalfimageSizeY - ((50 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_50(line_50.m_sx, line_50.m_sy);

        IPVM::LineSeg_32f line_100;
        line_100.m_sx = 0;
        line_100.m_sy = resizeHalfimageSizeY - ((100 * (resizeHalfimageSizeY / 2)) / 255);
        line_100.m_ex = resizeHalfimageSizeX;
        line_100.m_ey = resizeHalfimageSizeY - ((100 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_100(line_100.m_sx, line_100.m_sy);

        IPVM::LineSeg_32f line_150;
        line_150.m_sx = 0;
        line_150.m_ey = resizeHalfimageSizeY - ((150 * (resizeHalfimageSizeY / 2)) / 255);
        line_150.m_sy = resizeHalfimageSizeY - ((150 * (resizeHalfimageSizeY / 2)) / 255);
        line_150.m_ex = resizeHalfimageSizeX;
        IPVM::Point_32f_C2 pt_150(line_150.m_sx, line_150.m_sy);

        IPVM::LineSeg_32f line_200;
        line_200.m_sx = 0;
        line_200.m_sy = resizeHalfimageSizeY - ((200 * (resizeHalfimageSizeY / 2)) / 255);
        line_200.m_ex = resizeHalfimageSizeX;
        line_200.m_ey = resizeHalfimageSizeY - ((200 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_200(line_200.m_sx, line_200.m_sy);

        imageViewDark.ImageOverlayAdd(lineMax, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(line_50, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(line_100, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(line_150, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(line_200, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(lineMin, RGB(0, 255, 0), 1);

        imageViewDark.ImageOverlayAdd(ptMax, _T("255"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(pt_50, _T("50"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(pt_100, _T("100"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(pt_150, _T("150"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(pt_200, _T("200"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(ptMin, _T("0"), RGB(0, 255, 0), 50);

        std::vector<IPVM::Point_32f_C2> vecGVHalfLinePoint;
        for (long nX = 0; nX < resizeHalfimageSizeX; nX++)
        {
            IPVM::Point_32f_C2 pt;
            long nValue = m_resizedHalfImage->GetSubpixValue(nX, resizeHalfimageSizeY / 2);
            pt.m_x = nX;
            pt.m_y = resizeHalfimageSizeY - ((nValue * (resizeHalfimageSizeY / 2)) / 255);
            vecGVHalfLinePoint.push_back(pt);

            imageViewDark.ImageOverlayAdd(pt, RGB(255, 0, 0), 1);
        }

        const long nHorLineCount = (long)vecGVHalfLinePoint.size();
        std::vector<IPVM::LineEq_32f> vecHorLineEquations;
        vecHorLineEquations.resize(nHorLineCount);

        imageViewDark.ImageOverlayShow();
        imageViewDark.ROIShow();

        //imageViewBright.ImageOverlayHide();
        //imageViewBright.ImageOverlayClear();
        //imageViewBright.ImageOverlayShow();
        //IPVM::LineEq_32f line = vecHorLineEquations[0];
    }
    else if (m_nGrayType == 0) // Both
    {
        IPVM::LineSeg_32f lineMax;
        lineMax.m_sx = 0;
        lineMax.m_sy = resizeHalfimageSizeY / 2;
        lineMax.m_ex = resizeHalfimageSizeX;
        lineMax.m_ey = resizeHalfimageSizeY / 2;
        IPVM::Point_32f_C2 ptMax(lineMax.m_sx, lineMax.m_sy);

        IPVM::LineSeg_32f lineMin;
        lineMin.m_sx = 0;
        lineMin.m_sy = resizeHalfimageSizeY;
        lineMin.m_ex = resizeHalfimageSizeX;
        lineMin.m_ey = resizeHalfimageSizeY;
        IPVM::Point_32f_C2 ptMin(lineMin.m_sx, lineMin.m_sy);

        IPVM::LineSeg_32f line_50;
        line_50.m_sx = 0;
        line_50.m_sy = resizeHalfimageSizeY - ((50 * (resizeHalfimageSizeY / 2)) / 255);
        line_50.m_ex = resizeHalfimageSizeX;
        line_50.m_ey = resizeHalfimageSizeY - ((50 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_50(line_50.m_sx, line_50.m_sy);

        IPVM::LineSeg_32f line_100;
        line_100.m_sx = 0;
        line_100.m_sy = resizeHalfimageSizeY - ((100 * (resizeHalfimageSizeY / 2)) / 255);
        line_100.m_ex = resizeHalfimageSizeX;
        line_100.m_ey = resizeHalfimageSizeY - ((100 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_100(line_100.m_sx, line_100.m_sy);

        IPVM::LineSeg_32f line_150;
        line_150.m_sx = 0;
        line_150.m_ey = resizeHalfimageSizeY - ((150 * (resizeHalfimageSizeY / 2)) / 255);
        line_150.m_sy = resizeHalfimageSizeY - ((150 * (resizeHalfimageSizeY / 2)) / 255);
        line_150.m_ex = resizeHalfimageSizeX;
        IPVM::Point_32f_C2 pt_150(line_150.m_sx, line_150.m_sy);

        IPVM::LineSeg_32f line_200;
        line_200.m_sx = 0;
        line_200.m_sy = resizeHalfimageSizeY - ((200 * (resizeHalfimageSizeY / 2)) / 255);
        line_200.m_ex = resizeHalfimageSizeX;
        line_200.m_ey = resizeHalfimageSizeY - ((200 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_200(line_200.m_sx, line_200.m_sy);

        imageViewBright.ImageOverlayAdd(lineMax, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(line_50, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(line_100, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(line_150, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(line_200, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(lineMin, RGB(0, 255, 0), 1);

        imageViewBright.ImageOverlayAdd(ptMax, _T("255"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(pt_50, _T("50"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(pt_100, _T("100"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(pt_150, _T("150"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(pt_200, _T("200"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(ptMin, _T("0"), RGB(0, 255, 0), 50);

        imageViewDark.ImageOverlayAdd(lineMax, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(line_50, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(line_100, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(line_150, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(line_200, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(lineMin, RGB(0, 255, 0), 1);

        imageViewDark.ImageOverlayAdd(ptMax, _T("255"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(pt_50, _T("50"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(pt_100, _T("100"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(pt_150, _T("150"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(pt_200, _T("200"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(ptMin, _T("0"), RGB(0, 255, 0), 50);
        BYTE* pData = m_resizedHalfImage->GetMem();

        std::vector<IPVM::Point_32f_C2> vecGVHalfLinePoint;
        for (long nX = 0; nX < resizeHalfimageSizeX; nX++)
        {
            IPVM::Point_32f_C2 pt;
            long nValue = m_resizedHalfImage->GetSubpixValue(nX, resizeHalfimageSizeY / 2);
            pt.m_x = nX;
            pt.m_y = resizeHalfimageSizeY - ((nValue * (resizeHalfimageSizeY / 2)) / 255);
            vecGVHalfLinePoint.push_back(pt);

            imageViewBright.ImageOverlayAdd(pt, RGB(255, 0, 0), 1);
            imageViewDark.ImageOverlayAdd(pt, RGB(255, 0, 0), 1);
        }
        const long nHorLineCount = (long)vecGVHalfLinePoint.size();
        std::vector<IPVM::LineEq_32f> vecHorLineEquations;
        vecHorLineEquations.resize(nHorLineCount);

        imageViewBright.ImageOverlayShow();
        imageViewBright.ROIShow();
        imageViewDark.ImageOverlayShow();
        imageViewDark.ROIShow();
    }
    else if (m_nGrayType == 3) //Clear
    {
        imageViewBright.ROIClear();
        imageViewDark.ROIClear();
        imageViewBright.ImageOverlayClear();
        imageViewDark.ImageOverlayClear();
        imageViewBright.ImageOverlayShow();
        imageViewDark.ImageOverlayShow();
    }

}

void ViewCellImageUpdater::UpdateImageView(IPVM::Image_8u_C1& imageBright, IPVM::Image_8u_C1& imageDark, IPVM::ImageView& imageViewBright, IPVM::ImageView& imageViewDark)
{
    imageViewBright.ImageOverlayClear();
    imageViewBright.ROIClear();
    imageViewDark.ImageOverlayClear();
    imageViewDark.ROIClear();

    long brightImageSizeX = imageBright.GetSizeX();
    long brightImageSizeY = imageBright.GetSizeY();

    long darkImageSizeX = imageDark.GetSizeX();
    long darkImageSizeY = imageDark.GetSizeY();

    // 원본 사이즈로 SetImage
    imageViewBright.SetImage(imageBright);
    imageViewDark.SetImage(imageDark);

    if (m_cellId != LONG_MAX)
    {
        CString cellid;
        cellid.Format(_T("%d"), m_cellId);

        imageViewBright.ImageOverlayAdd(IPVM::Point_32f_C2(3, 3), cellid, RGB(0, 128, 0), 100);
    }

    //// 1. align 박스 표시
    //auto* itemCellAlign = (CInspectionCellAlign*)m_pInspectionCell->GetProcessingItem(ProcItemCategory::cellAlign);

    //for (long nCnt = 0; nCnt < itemCellAlign->m_paraEdgeDetectBlob.m_vecrtDetect.size() - 1; nCnt++)
    //{
    //    CString strName = _T("align");
    //    CString strKey;
    //    strKey.Format(_T("Align_%d"), nCnt);
    //    IPVM::Rect rtAlign = itemCellAlign->m_unionResult.m_rectAreaBlob[nCnt];
    //    
    //    // 이미지 1/4 배 한 것임으로 ROI 사이즈 조정
    //    rtAlign.m_left /= 4;
    //    rtAlign.m_right /= 4;
    //    rtAlign.m_top /= 4;
    //    rtAlign.m_bottom /= 4;
    //    imageViewBright.ImageOverlayAdd(rtAlign, RGB(255, 80, 180), 1);
    //    imageViewDark.ImageOverlayAdd(rtAlign, RGB(255, 80, 180), 1);
    //}
    //imageViewBright.ImageOverlayShow();
    //imageViewDark.ImageOverlayShow();

    //// 2. blob 박스 표시
    //if (m_pISICellAllDefect != NULL)
    //{
    //    auto& Defects = m_pISICellAllDefect->m_vecCandidate;
    //    int nIndex = -1;
    //    COLORREF color;
    //    for (auto& Defect : Defects)
    //    {
    //        nIndex++;
    //        //if (_T("OK") == Defect.strTempUI)
    //        //{
    //        //    //color = RGB(50, 200, 0);
    //        //    //if (false == m_bShow_OK)
    //        //        continue;
    //        //}
    //        //else if (_T("SKIP") == Defect.strTempUI)
    //        //{
    //        //    //color = RGB(100, 100, 150);
    //        //    //if (false == m_bShow_SKIP)
    //        //        continue;
    //        //}
    //        //else
    //        //{
    //        //    color = RGB(200, 50, 0);
    //        //    //if (false == m_bShow_NG)
    //        //    //    continue;
    //        //}

    //        if (_T("NG") == Defect.strTempUI)
    //        {
    //            long nType = m_pISICellAllDefect->m_vecISIDefectType[nIndex];
    //            color = RGB(200, 50, 0);
    //            Defect;
    //            CString strDefectName, strXYSize;
    //            //strDefectName = GetDefectName(Defect.GetDefectResultCode());
    //            //strDefectName = _T("Defect");
    //            strDefectName = GetDefectName((DefectType)nType);
    //            strXYSize.Format(_T("%d.%s"), nIndex, strDefectName);
    //            IPVM::Rect rect = Defect.rtRect_pixel;

    //            // Rect 1/4
    //            rect.m_top /= 4;
    //            rect.m_left /= 4;
    //            rect.m_bottom /= 4;
    //            rect.m_right /= 4;
    //            IPVM::Point_32s_C2 pt = rect.TopLeft();
    //            pt.m_y -= 45;
    //            CString strKey;
    //            strKey.Format(_T("Defect_%d"), nIndex);

    //            IPVM::Quadrangle_32f quadROI(Defect.quadROI);
    //            quadROI.m_fLBx /= 4;
    //            quadROI.m_fLBy /= 4;
    //            quadROI.m_fLTx /= 4;
    //            quadROI.m_fLTy /= 4;
    //            quadROI.m_fRBx /= 4;
    //            quadROI.m_fRBy /= 4;
    //            quadROI.m_fRTx /= 4;
    //            quadROI.m_fRTy /= 4;

    //            if (Defect.HOR > 0 && Defect.VER > 0)
    //            {
    //                imageViewBright.ROISet(strKey, strXYSize, rect, color, TRUE, FALSE, 80);
    //                imageViewBright.ImageOverlayAdd(quadROI, RGB(100, 100, 150), 4);

    //                imageViewDark.ROISet(strKey, strXYSize, rect, color, TRUE, FALSE, 80);
    //                imageViewDark.ImageOverlayAdd(quadROI, RGB(100, 100, 150), 4);
    //            }
    //        }
    //        //m_imageView->ImageOverlayAdd(rect, color, 8);
    //        imageViewBright.ImageOverlayShow();
    //        imageViewBright.ROIShow();

    //        imageViewDark.ImageOverlayShow();
    //        imageViewDark.ROIShow();
    //    }
    //    m_pISICellAllDefect = NULL;
    //}

    long resizeHalfimageSizeX = imageBright.GetSizeX();
    long resizeHalfimageSizeY = imageBright.GetSizeY();

    if (m_nGrayType == 1) //bright
    {
        IPVM::LineSeg_32f lineMax;
        lineMax.m_sx = 0;
        lineMax.m_sy = resizeHalfimageSizeY / 2;
        lineMax.m_ex = resizeHalfimageSizeX;
        lineMax.m_ey = resizeHalfimageSizeY / 2;
        IPVM::Point_32f_C2 ptMax(lineMax.m_sx, lineMax.m_sy);

        IPVM::LineSeg_32f lineMin;
        lineMin.m_sx = 0;
        lineMin.m_sy = resizeHalfimageSizeY;
        lineMin.m_ex = resizeHalfimageSizeX;
        lineMin.m_ey = resizeHalfimageSizeY;
        IPVM::Point_32f_C2 ptMin(lineMin.m_sx, lineMin.m_sy);

        IPVM::LineSeg_32f line_50;
        line_50.m_sx = 0;
        line_50.m_sy = resizeHalfimageSizeY - ((50 * (resizeHalfimageSizeY / 2)) / 255);
        line_50.m_ex = resizeHalfimageSizeX;
        line_50.m_ey = resizeHalfimageSizeY - ((50 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_50(line_50.m_sx, line_50.m_sy);

        IPVM::LineSeg_32f line_100;
        line_100.m_sx = 0;
        line_100.m_sy = resizeHalfimageSizeY - ((100 * (resizeHalfimageSizeY / 2)) / 255);
        line_100.m_ex = resizeHalfimageSizeX;
        line_100.m_ey = resizeHalfimageSizeY - ((100 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_100(line_100.m_sx, line_100.m_sy);

        IPVM::LineSeg_32f line_150;
        line_150.m_sx = 0;
        line_150.m_ey = resizeHalfimageSizeY - ((150 * (resizeHalfimageSizeY / 2)) / 255);
        line_150.m_sy = resizeHalfimageSizeY - ((150 * (resizeHalfimageSizeY / 2)) / 255);
        line_150.m_ex = resizeHalfimageSizeX;
        IPVM::Point_32f_C2 pt_150(line_150.m_sx, line_150.m_sy);

        IPVM::LineSeg_32f line_200;
        line_200.m_sx = 0;
        line_200.m_sy = resizeHalfimageSizeY - ((200 * (resizeHalfimageSizeY / 2)) / 255);
        line_200.m_ex = resizeHalfimageSizeX;
        line_200.m_ey = resizeHalfimageSizeY - ((200 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_200(line_200.m_sx, line_200.m_sy);

        imageViewBright.ImageOverlayAdd(lineMax, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(line_50, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(line_100, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(line_150, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(line_200, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(lineMin, RGB(0, 255, 0), 1);

        imageViewBright.ImageOverlayAdd(ptMax, _T("255"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(pt_50, _T("50"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(pt_100, _T("100"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(pt_150, _T("150"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(pt_200, _T("200"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(ptMin, _T("0"), RGB(0, 255, 0), 50);

        std::vector<IPVM::Point_32f_C2> vecGVHalfLinePoint;
        for (long nX = 0; nX < resizeHalfimageSizeX; nX++)
        {
            IPVM::Point_32f_C2 pt;
            long nValue = imageBright.GetSubpixValue(nX, resizeHalfimageSizeY / 2);
            pt.m_x = nX;
            pt.m_y = resizeHalfimageSizeY - ((nValue * (resizeHalfimageSizeY / 2)) / 255);
            vecGVHalfLinePoint.push_back(pt);

            imageViewBright.ImageOverlayAdd(pt, RGB(255, 0, 0), 1);
        }
        imageViewBright.ImageOverlayShow();
        imageViewBright.ROIShow();

        const long nHorLineCount = (long)vecGVHalfLinePoint.size();
        std::vector<IPVM::LineEq_32f> vecHorLineEquations;
        vecHorLineEquations.resize(nHorLineCount);

        //imageViewDark.ImageOverlayHide();
        //imageViewDark.ImageOverlayClear();
        //imageViewDark.ImageOverlayShow();
    }
    else if (m_nGrayType == 2) // Dark
    {
        IPVM::LineSeg_32f lineMax;
        lineMax.m_sx = 0;
        lineMax.m_sy = resizeHalfimageSizeY / 2;
        lineMax.m_ex = resizeHalfimageSizeX;
        lineMax.m_ey = resizeHalfimageSizeY / 2;
        IPVM::Point_32f_C2 ptMax(lineMax.m_sx, lineMax.m_sy);

        IPVM::LineSeg_32f lineMin;
        lineMin.m_sx = 0;
        lineMin.m_sy = resizeHalfimageSizeY;
        lineMin.m_ex = resizeHalfimageSizeX;
        lineMin.m_ey = resizeHalfimageSizeY;
        IPVM::Point_32f_C2 ptMin(lineMin.m_sx, lineMin.m_sy);

        IPVM::LineSeg_32f line_50;
        line_50.m_sx = 0;
        line_50.m_sy = resizeHalfimageSizeY - ((50 * (resizeHalfimageSizeY / 2)) / 255);
        line_50.m_ex = resizeHalfimageSizeX;
        line_50.m_ey = resizeHalfimageSizeY - ((50 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_50(line_50.m_sx, line_50.m_sy);

        IPVM::LineSeg_32f line_100;
        line_100.m_sx = 0;
        line_100.m_sy = resizeHalfimageSizeY - ((100 * (resizeHalfimageSizeY / 2)) / 255);
        line_100.m_ex = resizeHalfimageSizeX;
        line_100.m_ey = resizeHalfimageSizeY - ((100 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_100(line_100.m_sx, line_100.m_sy);

        IPVM::LineSeg_32f line_150;
        line_150.m_sx = 0;
        line_150.m_ey = resizeHalfimageSizeY - ((150 * (resizeHalfimageSizeY / 2)) / 255);
        line_150.m_sy = resizeHalfimageSizeY - ((150 * (resizeHalfimageSizeY / 2)) / 255);
        line_150.m_ex = resizeHalfimageSizeX;
        IPVM::Point_32f_C2 pt_150(line_150.m_sx, line_150.m_sy);

        IPVM::LineSeg_32f line_200;
        line_200.m_sx = 0;
        line_200.m_sy = resizeHalfimageSizeY - ((200 * (resizeHalfimageSizeY / 2)) / 255);
        line_200.m_ex = resizeHalfimageSizeX;
        line_200.m_ey = resizeHalfimageSizeY - ((200 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_200(line_200.m_sx, line_200.m_sy);

        imageViewDark.ImageOverlayAdd(lineMax, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(line_50, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(line_100, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(line_150, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(line_200, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(lineMin, RGB(0, 255, 0), 1);

        imageViewDark.ImageOverlayAdd(ptMax, _T("255"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(pt_50, _T("50"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(pt_100, _T("100"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(pt_150, _T("150"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(pt_200, _T("200"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(ptMin, _T("0"), RGB(0, 255, 0), 50);

        std::vector<IPVM::Point_32f_C2> vecGVHalfLinePoint;
        for (long nX = 0; nX < resizeHalfimageSizeX; nX++)
        {
            IPVM::Point_32f_C2 pt;
            long nValue = imageDark.GetSubpixValue(nX, resizeHalfimageSizeY / 2);
            pt.m_x = nX;
            pt.m_y = resizeHalfimageSizeY - ((nValue * (resizeHalfimageSizeY / 2)) / 255);
            vecGVHalfLinePoint.push_back(pt);

            imageViewDark.ImageOverlayAdd(pt, RGB(255, 0, 0), 1);
        }

        const long nHorLineCount = (long)vecGVHalfLinePoint.size();
        std::vector<IPVM::LineEq_32f> vecHorLineEquations;
        vecHorLineEquations.resize(nHorLineCount);

        imageViewDark.ImageOverlayShow();
        imageViewDark.ROIShow();

        //imageViewBright.ImageOverlayHide();
        //imageViewBright.ImageOverlayClear();
        //imageViewBright.ImageOverlayShow();
        //IPVM::LineEq_32f line = vecHorLineEquations[0];
    }
    else if (m_nGrayType == 0) // Both
    {
        IPVM::LineSeg_32f lineMax;
        lineMax.m_sx = 0;
        lineMax.m_sy = resizeHalfimageSizeY / 2;
        lineMax.m_ex = resizeHalfimageSizeX;
        lineMax.m_ey = resizeHalfimageSizeY / 2;
        IPVM::Point_32f_C2 ptMax(lineMax.m_sx, lineMax.m_sy);

        IPVM::LineSeg_32f lineMin;
        lineMin.m_sx = 0;
        lineMin.m_sy = resizeHalfimageSizeY;
        lineMin.m_ex = resizeHalfimageSizeX;
        lineMin.m_ey = resizeHalfimageSizeY;
        IPVM::Point_32f_C2 ptMin(lineMin.m_sx, lineMin.m_sy);
        
        IPVM::LineSeg_32f line_50;
        line_50.m_sx = 0;
        line_50.m_sy = resizeHalfimageSizeY - ((50 * (resizeHalfimageSizeY / 2)) / 255);
        line_50.m_ex = resizeHalfimageSizeX;
        line_50.m_ey = resizeHalfimageSizeY - ((50 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_50(line_50.m_sx, line_50.m_sy);

        IPVM::LineSeg_32f line_100;
        line_100.m_sx = 0;
        line_100.m_sy = resizeHalfimageSizeY - ((100 * (resizeHalfimageSizeY / 2)) / 255);
        line_100.m_ex = resizeHalfimageSizeX;
        line_100.m_ey = resizeHalfimageSizeY - ((100 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_100(line_100.m_sx, line_100.m_sy);

        IPVM::LineSeg_32f line_150;
        line_150.m_sx = 0;
        line_150.m_ey = resizeHalfimageSizeY - ((150 * (resizeHalfimageSizeY / 2)) / 255);
        line_150.m_sy = resizeHalfimageSizeY - ((150 * (resizeHalfimageSizeY / 2)) / 255);
        line_150.m_ex = resizeHalfimageSizeX;
        IPVM::Point_32f_C2 pt_150(line_150.m_sx, line_150.m_sy);

        IPVM::LineSeg_32f line_200;
        line_200.m_sx = 0;
        line_200.m_sy = resizeHalfimageSizeY - ((200 * (resizeHalfimageSizeY / 2)) / 255);
        line_200.m_ex = resizeHalfimageSizeX;
        line_200.m_ey = resizeHalfimageSizeY - ((200 * (resizeHalfimageSizeY / 2)) / 255);
        IPVM::Point_32f_C2 pt_200(line_200.m_sx, line_200.m_sy);

        imageViewBright.ImageOverlayAdd(lineMax, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(line_50, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(line_100, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(line_150, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(line_200, RGB(0, 255, 0), 1);
        imageViewBright.ImageOverlayAdd(lineMin, RGB(0, 255, 0), 1);

        imageViewBright.ImageOverlayAdd(ptMax, _T("255"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(pt_50, _T("50"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(pt_100, _T("100"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(pt_150, _T("150"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(pt_200, _T("200"), RGB(0, 255, 0), 50);
        imageViewBright.ImageOverlayAdd(ptMin, _T("0"), RGB(0, 255, 0), 50);

        imageViewDark.ImageOverlayAdd(lineMax, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(line_50, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(line_100, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(line_150, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(line_200, RGB(0, 255, 0), 1);
        imageViewDark.ImageOverlayAdd(lineMin, RGB(0, 255, 0), 1);

        imageViewDark.ImageOverlayAdd(ptMax, _T("255"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(pt_50, _T("50"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(pt_100, _T("100"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(pt_150, _T("150"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(pt_200, _T("200"), RGB(0, 255, 0), 50);
        imageViewDark.ImageOverlayAdd(ptMin, _T("0"), RGB(0, 255, 0), 50);

        std::vector<IPVM::Point_32f_C2> vecGVHalfLinePoint;
        for (long nX = 0; nX < resizeHalfimageSizeX; nX++)
        {
            IPVM::Point_32f_C2 pt;
            long nValue = imageBright.GetSubpixValue(nX, resizeHalfimageSizeY / 2);
            pt.m_x = nX;
            pt.m_y = resizeHalfimageSizeY - ((nValue * (resizeHalfimageSizeY / 2)) / 255);
            vecGVHalfLinePoint.push_back(pt);

            imageViewBright.ImageOverlayAdd(pt, RGB(255, 0, 0), 1);
            imageViewDark.ImageOverlayAdd(pt, RGB(255, 0, 0), 1);
        }
        const long nHorLineCount = (long)vecGVHalfLinePoint.size();
        std::vector<IPVM::LineEq_32f> vecHorLineEquations;
        vecHorLineEquations.resize(nHorLineCount);

        imageViewBright.ImageOverlayShow();
        imageViewBright.ROIShow();
        imageViewDark.ImageOverlayShow();
        imageViewDark.ROIShow();
    }
    else if (m_nGrayType == 3) //Clear
    {
        imageViewBright.ROIClear();
        imageViewDark.ROIClear();
        imageViewBright.ImageOverlayClear();
        imageViewDark.ImageOverlayClear();
        imageViewBright.ImageOverlayShow();
        imageViewDark.ImageOverlayShow();
    }
}

void ViewCellImageUpdater::SetInspectionCell(CInspectionCell *CellInfo)
{
    m_pInspectionCell = CellInfo;
}

void ViewCellImageUpdater::SetSurfaceInspCheck(BOOL bCheck)
{
    m_bSurfaceInspCheck = bCheck;
}
void ViewCellImageUpdater::SetImageRoiClear()
{
    m_imageViewBright.ROIClear();
    m_imageViewDark.ROIClear();
    m_imageViewBright.ImageOverlayClear();
    m_imageViewDark.ImageOverlayClear();
   
    m_imageViewBright.ImageOverlayShow();
    m_imageViewDark.ImageOverlayShow();
}