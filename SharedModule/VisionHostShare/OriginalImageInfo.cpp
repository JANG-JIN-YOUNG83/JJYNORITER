#include "stdafx.h"
#include "OriginalImageInfo.h"
#include "../CommonShare/ArchiveAllType.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

OriginalImageInfo::OriginalImageInfo()
    : nChannel(0)
    , nCamera(0)
    , nSizeX(0)
    , nSizeY(0)
{
}

OriginalImageInfo::OriginalImageInfo(const OriginalImageInfo& rhs)
    : nChannel(rhs.nChannel)
    , nCamera(rhs.nCamera)
    , nSizeX(rhs.nSizeX)
    , nSizeY(rhs.nSizeY)
    , srcImg(rhs.srcImg)
{
}

OriginalImageInfo::OriginalImageInfo(int nCa, int nCh, IPVM::Image_8u_C1* src)
    : nChannel(nCh)
    , nCamera(nCa)
{
    nSizeX = src->GetSizeX();
    nSizeY = src->GetSizeY();
    srcImg = *src;
}

OriginalImageInfo::~OriginalImageInfo()
{
}

void OriginalImageInfo::Serialize(CommonShare::ArchiveAllType& ar)
{
    ar.Serialize_Element(nCamera);
    ar.Serialize_Element(nChannel);
    ar.Serialize_Element(nSizeX);
    ar.Serialize_Element(nSizeY);

    if (ar.IsLoading())
    {
        srcImg.Create(nSizeX, nSizeY);
    }
    ar.Serialize_RawData(srcImg.GetMem(), nSizeX * nSizeY);
}

OriginalImageInfo& OriginalImageInfo::operator = (const OriginalImageInfo& rhs)
{
    nCamera = rhs.nCamera;
    nChannel = rhs.nChannel;
    srcImg = rhs.srcImg;
    nSizeX = rhs.nSizeX;
    nSizeY = rhs.nSizeY;

    return *this;
}