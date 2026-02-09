#include "stdafx.h"
#include "ResultDefectROI.h"

#include "../CommonShare/ArchiveAllType.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ResultDefectROI::ResultDefectROI()
{
    m_nIndex = 0;

    m_dRelative_X = 0.0;
    m_dRelative_Y = 0.0;
    m_dRelative_Img_X = 0.0;
    m_dRelative_Img_Y = 0.0;
    m_dWidth = 0.0;
    m_dLength = 0.0;
    m_dImg_Width = 0.0;
    m_dImg_Length = 0.0;
    m_dArea_MM2 = 0.0;
    m_dArea_Pixels = 0.;
    m_dRatio_Width_Length = 0.0;

    m_dGray_LV_Avg = 0.;
    m_dGray_LV_Max = 0.;
    m_dGray_LV_Min = 0.;
    m_dGray_LV_Range = 0.;
    m_dGray_LV_Stddev = 0.;

    m_dRoi_Gray_LV_Max = 0.;
    m_dRoi_Gray_LV_Avg = 0.;
    m_dRoi_Gray_LV_Min = 0.;
    m_dRoi_Gray_LV_Median = 0.;
    m_dRoi_Gray_LV_Stddev = 0.;

    m_strDefect_Position = _T("");
    m_strDefect_Color = _T("");
    m_nDefect_Lane = 0;
    m_bDefect_Left = TRUE;
    m_bDataSave = false;

    m_strImage_File_Name_json = _T("");

    m_rtShowROI = CRect(0, 0, 0, 0);
}

ResultDefectROI::~ResultDefectROI()
{
}

ResultDefectROI::ResultDefectROI(CRect rtShowROI, IPVM::Image_8u_C4* src)
{
    m_nIndex = 0;

    m_dRelative_X = 0.0;
    m_dRelative_Y = 0.0;

    m_dWidth = 0.0;
    m_dLength = 0.0;
    m_dImg_Width = src->GetSizeX();
    m_dImg_Length = src->GetSizeY();
    m_dArea_MM2 = 0.0;
    m_dArea_Pixels = 0.;
    m_dRatio_Width_Length = 0.0;

    m_dGray_LV_Avg = 0.;
    m_dGray_LV_Max = 0.;
    m_dGray_LV_Min = 0.;
    m_dGray_LV_Range = 0.;
    m_dGray_LV_Stddev = 0.;

    m_dRoi_Gray_LV_Max = 0.;
    m_dRoi_Gray_LV_Avg = 0.;
    m_dRoi_Gray_LV_Min = 0.;
    m_dRoi_Gray_LV_Median = 0.;
    m_dRoi_Gray_LV_Stddev = 0.;

    m_strDefect_Position = _T("");
    m_strDefect_Color = _T("");
    m_nDefect_Lane = 0;
    m_bDefect_Left = TRUE;

    m_strImage_File_Name_json = _T("");

    m_rtShowROI.left = rtShowROI.left;
    m_rtShowROI.right = rtShowROI.right;
    m_rtShowROI.top = rtShowROI.top;
    m_rtShowROI.bottom = rtShowROI.bottom;
    
    m_DstCropImage = *src;
}

void ResultDefectROI::Serialize(CommonShare::ArchiveAllType& ar)
{
    ar.Serialize_Element(m_nIndex);

    ar.Serialize_Element(m_dRelative_X);
    ar.Serialize_Element(m_dRelative_Y);
    ar.Serialize_Element(m_dRelative_Img_X);
    ar.Serialize_Element(m_dRelative_Img_Y);
    ar.Serialize_Element(m_dWidth);
    ar.Serialize_Element(m_dLength);
    ar.Serialize_Element(m_dImg_Width);
    ar.Serialize_Element(m_dImg_Length);
    ar.Serialize_Element(m_dArea_MM2);
    ar.Serialize_Element(m_dArea_Pixels);
    ar.Serialize_Element(m_dRatio_Width_Length);

    ar.Serialize_Element(m_dGray_LV_Avg);
    ar.Serialize_Element(m_dGray_LV_Max);
    ar.Serialize_Element(m_dGray_LV_Min);
    ar.Serialize_Element(m_dGray_LV_Range);
    ar.Serialize_Element(m_dGray_LV_Stddev);

    ar.Serialize_Element(m_dRoi_Gray_LV_Max);
    ar.Serialize_Element(m_dRoi_Gray_LV_Avg);
    ar.Serialize_Element(m_dRoi_Gray_LV_Min);
    ar.Serialize_Element(m_dRoi_Gray_LV_Median);
    ar.Serialize_Element(m_dRoi_Gray_LV_Stddev);

    ar.Serialize_Element(m_rtShowROI.left);
    ar.Serialize_Element(m_rtShowROI.right);
    ar.Serialize_Element(m_rtShowROI.top);
    ar.Serialize_Element(m_rtShowROI.bottom);

    ar.Serialize_Element(m_strDefect_Position);
    ar.Serialize_Element(m_strDefect_Color);
    ar.Serialize_Element(m_nDefect_Lane);
    ar.Serialize_Element(m_bDefect_Left);
    ar.Serialize_Element(m_strImage_File_Name_json);
    ar.Serialize_Element(m_bDataSave);

    if (!m_bDataSave)
    {
        if (ar.IsLoading())
        {
            m_DstCropImage_Bright.Create(128, 128);
        }
        long n = m_DstCropImage_Bright.GetSizeX();
        ar.Serialize_RawData(
            (BYTE*)m_DstCropImage_Bright.GetMem(), m_DstCropImage_Bright.GetSizeX() * m_DstCropImage_Bright.GetSizeY());
        //Sleep(10);
        if (ar.IsLoading())
        {
            m_DstCropImage_Dark.Create(128, 128);
        }
        ar.Serialize_RawData(
            (BYTE*)m_DstCropImage_Dark.GetMem(), m_DstCropImage_Dark.GetSizeX() * m_DstCropImage_Dark.GetSizeY());
    }
}
