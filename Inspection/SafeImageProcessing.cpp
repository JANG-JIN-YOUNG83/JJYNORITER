#include "stdafx.h"
#include "SafeImageProcessing.h"

#include <Algorithm/ImageProcessing.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IPVM::Status SafeImageProcessing::Fill(IPVM::Quadrangle_32f roi, BYTE value, IPVM::Image_8u_C1& dstImage)
{
	long imageSizeX = dstImage.GetSizeX();
	long imageSizeY = dstImage.GetSizeY();

	roi.m_fLTx = min(imageSizeX - 1, max(0, roi.m_fLTx));
	roi.m_fRTx = min(imageSizeX - 1, max(0, roi.m_fRTx));
	roi.m_fLBx = min(imageSizeX - 1, max(0, roi.m_fLBx));
	roi.m_fRBx = min(imageSizeX - 1, max(0, roi.m_fRBx));

	roi.m_fLTy = min(imageSizeY - 1, max(0, roi.m_fLTy));
	roi.m_fRTy = min(imageSizeY - 1, max(0, roi.m_fRTy));
	roi.m_fLBy = min(imageSizeY - 1, max(0, roi.m_fLBy));
	roi.m_fRBy = min(imageSizeY - 1, max(0, roi.m_fRBy));

	return IPVM::ImageProcessing::Fill(roi, value, dstImage);
}
