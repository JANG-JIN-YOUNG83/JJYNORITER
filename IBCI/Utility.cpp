#include "pch.h"
#include "Utility.h"
#include "RecyclableImage.h"
//#include "SystemParameter.h"

#include "Base/LoggerInterface.h"
#include <cmath>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool UtilityImage::CreateBrightDarkImage(const IPVM::Image_8u_C1& source, IPVM::Image_8u_C1& o_imageBright, IPVM::Image_8u_C1& o_imageDark, bool bExactly, IPVM::LoggerInterface* logger)
{
	auto imageWidth = source.GetSizeX();
	auto imageHeght = source.GetSizeY();

	if (imageWidth != source.GetWidthBytes())
	{
		if (logger) logger->Log(0, _T("Width와 WidthBytes 가 같은걸로 고려해서 만들어 졌다"));
		ASSERT(!_T("Width와 WidthBytes 가 같은걸로 고려해서 만들어 졌다"));
		return false;
	}

	int offsetBright, offsetDark;

	long nCheckPos = 3; // g_systemParameter.nMultiImageCheckPos
	long nCheckBrightness = 64; // g_systemParameter.nImageCheckBrightness
	long nCheckDarkness = 32; // g_systemParameter.nImageCheckDarkness

	if (source.GetValue(nCheckPos, 0) >= source.GetValue(nCheckPos, 1))
	{
		offsetBright = 0;
		offsetDark = imageWidth;
	}
	else
	{
		offsetBright = imageWidth;
		offsetDark = 0;
	}

	long imageHeightDstHalf = imageHeght / 2;
    //long imageHeightDstHalf = imageHeght;

	//TODO: 이 두 이미지는 포인터만 가지고 있어도 되므로, inputData.imageFull 의 child 이미지를 사용하는 것으로 수정할 수 있는지 확인할 것
	if (!RecyclableImage::Alloc_Channel(o_imageBright, imageWidth, imageHeightDstHalf))
	{
		if (logger) logger->Log(0, _T("Bright Image 생성실패"));
	}

	if (!RecyclableImage::Alloc_Channel(o_imageDark, imageWidth, imageHeightDstHalf))
	{
		if (logger) logger->Log(0, _T("Dark Image 생성실패"));
	}

	BYTE* imageBright = o_imageBright.GetMem();
	BYTE* imageDark = o_imageDark.GetMem();

	auto* sourcePtr = source.GetMem();

	bool res = true;
#if INSPECTION_MACHINE_UC1
    bExactly = false;
#else
    bExactly = true;
#endif // INSPECTION_MACHINE_UC1

	if (bExactly)// 매번 pos를 확인하여 이미지 섞임을 방지 할 수 있음.
	{
		long nPos_Bright = 0;
		long nPos_Dark = 0;
		for (long y = 0; y < imageHeght; y++)
		{
			auto value = source.GetValue(nCheckPos, y);
			if (value == nCheckBrightness) {
				if (nPos_Bright > imageHeightDstHalf)
				{
					res = false;
					continue;
				}
				memcpy(imageBright + (nPos_Bright * imageWidth), sourcePtr + (y * imageWidth), imageWidth);
				nPos_Bright++;
			}
			else if (value == nCheckDarkness) {
				if (nPos_Dark > imageHeightDstHalf)
				{
					res = false;
					continue;
				}
				memcpy(imageDark + (nPos_Dark * imageWidth), sourcePtr + (y * imageWidth), imageWidth);
				nPos_Dark++;
			}
			else
				res = false;
		}
	}
	else//offset을 계산하여 나누기에 jpg, 노이즈성 이미지가 나와도 이미지 분기됨
	{
		for (long y = 0; y < imageHeightDstHalf; y++)
		{
			memcpy(imageBright + (y * imageWidth), sourcePtr + (offsetBright + 2 * y * imageWidth), imageWidth);
			memcpy(imageDark + (y * imageWidth), sourcePtr + (offsetDark + 2 * y * imageWidth), imageWidth);
		}
	}
	return res;
}

bool UtilityImage::CreateBrightDarkImage_sapera(const IPVM::Image_8u_C1& source, IPVM::Image_8u_C1& o_imageBright, IPVM::Image_8u_C1& o_imageDark, bool bExactly, IPVM::LoggerInterface* logger)
{
	auto imageWidth = source.GetSizeX();
	auto imageHeght = source.GetSizeY();

	if (imageWidth != source.GetWidthBytes())
	{
		if (logger) logger->Log(0, _T("Width와 WidthBytes 가 같은걸로 고려해서 만들어 졌다"));
		ASSERT(!_T("Width와 WidthBytes 가 같은걸로 고려해서 만들어 졌다"));
		return false;
	}

	int offsetBright, offsetDark;

	long nCheckPos = 3; // g_systemParameter.nMultiImageCheckPos
	long nCheckBrightness = 64; // g_systemParameter.nImageCheckBrightness
	long nCheckDarkness = 32; // g_systemParameter.nImageCheckDarkness

	if (source.GetValue(nCheckPos, 0) >= source.GetValue(nCheckPos, 1))
	{
		offsetBright = 0;
		offsetDark = imageWidth;
	}
	else
	{
		offsetBright = imageWidth;
		offsetDark = 0;
	}

	long imageHeightDstHalf = imageHeght / 2;

	//TODO: 이 두 이미지는 포인터만 가지고 있어도 되므로, inputData.imageFull 의 child 이미지를 사용하는 것으로 수정할 수 있는지 확인할 것
	if (!RecyclableImage::Alloc_Channel(o_imageBright, imageWidth, imageHeightDstHalf))
	{
		if (logger) logger->Log(0, _T("Bright Image 생성실패"));
	}

	if (!RecyclableImage::Alloc_Channel(o_imageDark, imageWidth, imageHeightDstHalf))
	{
		if (logger) logger->Log(0, _T("Dark Image 생성실패"));
	}

	BYTE* imageBright = o_imageBright.GetMem();
	BYTE* imageDark = o_imageDark.GetMem();

	auto* sourcePtr = source.GetMem();

	bool res = true;

	//offset을 계산하여 나누기에 jpg, 노이즈성 이미지가 나와도 이미지 분기됨
	{
		for (long y = 0; y < imageHeightDstHalf; y++)
		{
			memcpy(imageBright + (y * imageWidth), sourcePtr + (y * imageWidth), imageWidth);
			memcpy(imageDark + (y * imageWidth), sourcePtr + ((source.GetSizeY() / 2 + y) * imageWidth), imageWidth);
		}
	}
	return res;
}

 bool UtilityImage::CreateGradientImage_insnex(const IPVM::Image_8u_C1& Topsource, const IPVM::Image_8u_C1& bottomource,
    IPVM::Image_8u_C1& o_imageGradient, IPVM::Image_8u_C1& o_imageABS, bool bExactly, IPVM::LoggerInterface* logger)
{
    // 256x256 LUT를 한 번만 미리 계산 (std::pow, float 나눗셈 등 고비용 연산 제거)
    static uint8_t gradientLUT[256][256];
    static uint8_t absLUT[256][256];
    static bool lutInitialized = false;

    if (!lutInitialized)
    {
        for (int t = 0; t < 256; t++)
        {
            for (int b = 0; b < 256; b++)
            {
                ///절대값
                int absDiff = std::abs(t - b);
                absLUT[t][b] = static_cast<uint8_t>(std::min<int>(absDiff * 2, 255));

                //감마보정
                float g = float(t - b) / (float(t + b) + 1.0f);
                int vis = int((g * 0.5f + 0.5f) * 255.0f);
                vis = std::clamp(vis, 0, 255);
                float f = vis / 255.0f;
                f = std::pow(f, 0.7f);
                vis = int(f * 255.0f);
                gradientLUT[t][b] = static_cast<uint8_t>(vis);
            }
        }
        lutInitialized = true;
    }

    auto imageWidth = Topsource.GetSizeX();
    auto imageHeght = Topsource.GetSizeY();

	 	if (!RecyclableImage::Alloc_Channel(o_imageGradient, imageWidth, imageHeght))
    {
        if (logger)
            logger->Log(0, _T("gradient Image 생성실패"));

			TRACE(_T(" gradient Image 생성실패 \n"));

    }

    if (!RecyclableImage::Alloc_Channel(o_imageABS, imageWidth, imageHeght))
    {
        if (logger)
            logger->Log(0, _T("Abs Image 생성실패"));

			TRACE(_T(" Abs Image 생성실패 \n"));
    }

    for (long y = 0; y < imageHeght; y++)
    {
        auto* imageGradient = o_imageGradient.GetMem(0,y);
        auto* imageABS = o_imageABS.GetMem(0, y);

        auto* sourceTopPtr = Topsource.GetMem(0, y);
        auto* sourceBottomPtr = bottomource.GetMem(0, y);

		for (long x = 0; x < imageWidth; x++)
        {
            const uint8_t top = sourceTopPtr[x];
            const uint8_t bot = sourceBottomPtr[x];
            imageABS[x] = absLUT[top][bot];
            imageGradient[x] = gradientLUT[top][bot];
        }
    }


	 return true;

 }

bool UtilityImage::CreateBrightDarkImage_MSA(const IPVM::Image_8u_C1& source, IPVM::Image_8u_C1& o_imageBright,
    IPVM::Image_8u_C1& o_imageDark, bool bExactly, IPVM::LoggerInterface* logger)
{
    auto imageWidth = source.GetSizeX();
    auto imageHeght = source.GetSizeY();

    if (imageWidth != source.GetWidthBytes())
    {
        if (logger)
            logger->Log(0, _T("Width와 WidthBytes 가 같은걸로 고려해서 만들어 졌다"));
        ASSERT(!_T("Width와 WidthBytes 가 같은걸로 고려해서 만들어 졌다"));
        return false;
    }

    int offsetBright, offsetDark;

    long nCheckPos = 3; // g_systemParameter.nMultiImageCheckPos
    long nCheckBrightness = 64; // g_systemParameter.nImageCheckBrightness
    long nCheckDarkness = 32; // g_systemParameter.nImageCheckDarkness

    if (source.GetValue(nCheckPos, 0) >= source.GetValue(nCheckPos, 1))
    {
        offsetBright = 0;
        offsetDark = imageWidth;
    }
    else
    {
        offsetBright = imageWidth;
        offsetDark = 0;
    }

    long imageHeightDstHalf = imageHeght;

    //TODO: 이 두 이미지는 포인터만 가지고 있어도 되므로, inputData.imageFull 의 child 이미지를 사용하는 것으로 수정할 수 있는지 확인할 것
    if (!RecyclableImage::Alloc_Channel(o_imageBright, imageWidth, imageHeightDstHalf))
    {
        if (logger)
            logger->Log(0, _T("Bright Image 생성실패"));
    }

    if (!RecyclableImage::Alloc_Channel(o_imageDark, imageWidth, imageHeightDstHalf))
    {
        if (logger)
            logger->Log(0, _T("Dark Image 생성실패"));
    }

    BYTE* imageBright = o_imageBright.GetMem();
    BYTE* imageDark = o_imageDark.GetMem();

    auto* sourcePtr = source.GetMem();

    bool res = true;
	//임시로 그냥 원본을 카피
	memcpy(imageBright, sourcePtr, imageWidth * imageHeght);
    memcpy(imageDark, sourcePtr, imageWidth * imageHeght);
	return res;
}

void UtilityImage::SaveImage(IPVM::Image_8u_C1 image, CString path)
{
    CString ext, noExt;
    ext = path.Right(path.GetLength() - path.ReverseFind('.')).MakeLower();
    noExt = path.Left(path.ReverseFind('.'));

	// move file
    CFileFind find;
    if (find.FindFile(path)) 
    {
        CTime cTime = CTime::GetCurrentTime();
        CString strTime;
        strTime.Format(_T("%04d%02d%02d_%02d%02d%02d"), cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute(), cTime.GetSecond());

		CString pathTime;
        pathTime.Format(_T("%s_%s%s"), noExt, strTime, ext);

		MoveFile(path, pathTime);
    }

	if (ext == _T(".jpg"))
    {
        image.SaveJpeg(path);
    }
    else if (ext == _T(".jpeg"))
    {
        image.SaveJpeg(path);
    }
    else if (ext == _T(".png"))
    {
        image.SavePng(path);
    }
    else if (ext == _T(".bmp"))
    {
        image.SaveBmp(path);
    }
}