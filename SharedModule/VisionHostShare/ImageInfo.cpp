#include "stdafx.h"
#include "ImageInfo.h"
#include "../CommonShare/ArchiveAllType.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ImageInfo::ImageInfo()
{
    nEdgeNum = 1;
    nLaneNum = 1;
    nSizeX = 0;
    nSizeY = 0;
    nFocus = 0;
    //nGrayLevelValue[2] = 0;
    nGrayLevelValue_Bright = 0;
    nGrayLevelValue_Dark = 0;
    m_SaveImagePath_LaneEdge_Crop = _T("");
}

ImageInfo::~ImageInfo()
{
}

ImageInfo::ImageInfo(IPVM::Image_8u_C1* src)
{
    nSizeX = src->GetSizeX();
    nSizeY = src->GetSizeY();
    m_CropImage_LaneEdge = *src;

    nGrayLevelValue_Bright = 200;
    nGrayLevelValue_Dark = 50;
    //nGrayLevelValue[0] = 200;
    //nGrayLevelValue[1] = 50;
}

void ImageInfo::Serialize(CommonShare::ArchiveAllType& ar)
{
    ar.Serialize_Element(nEdgeNum);
    ar.Serialize_Element(nLaneNum);
    ar.Serialize_Element(nSizeX);
    ar.Serialize_Element(nSizeY);
    ar.Serialize_Element(nFocus);
    ar.Serialize_Element(nGrayLevelValue_Bright);
    ar.Serialize_Element(nGrayLevelValue_Dark);
    //ar.Serialize_Element(nGrayLevelValue[2]);

    if (ar.IsLoading())
    {
        m_CropImage_LaneEdge.Create(nSizeX, nSizeY);
    }
    ar.Serialize_RawData((BYTE*)m_CropImage_LaneEdge.GetMem(), nSizeX * nSizeY);
}