#include "stdafx.h"
#include "ImageViewEx.h"

#include "Algorithm/ImageProcessing.h"
#include "Base/basedef.h"
#include "Types/LineSeg_32f.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Inspection;

ImageViewEx::ImageViewEx(HWND hwndParent, const IPVM::Rect &rtPosition, const long dlgCtrlID)
	: IPVM::ImageView(hwndParent, rtPosition, dlgCtrlID)
{
}

ImageViewEx::ImageViewEx(HWND hwndParent, const int frameRsrcID, const long dlgCtrlID)
	: IPVM::ImageView(hwndParent, frameRsrcID, dlgCtrlID)
{
}

void ImageViewEx::SetImage(const IPVM::Image_8u_C1& image)
{
	m_sourceImage.Create(image.GetSizeX(), image.GetSizeY());
	IPVM::ImageProcessing::Copy(image, IPVM::Rect(image), m_sourceImage);

	IPVM::ImageView::SetImage(m_sourceImage);
}

void ImageViewEx::GetProfile(const IPVM::LineSeg_32f& line, std::vector<BYTE>& o_profiles)
{
	IPVM::Point_32s_C2 begin((long)line.m_sx, (long)line.m_sy);
	IPVM::Point_32s_C2 end((long)line.m_ex, (long)line.m_ey);

	long maxLength = m_sourceImage.GetSizeX() + m_sourceImage.GetSizeY();
	long length = 0;
	o_profiles.resize(maxLength);

	if (IPVM::ImageProcessing::SampleLine_Nearest(m_sourceImage, begin, end, maxLength, &o_profiles[0], length) != IPVM::Status::success)
	{
		o_profiles.clear();
		return;
	}

	o_profiles.resize(length);
}
