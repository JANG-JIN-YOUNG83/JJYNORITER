#include "pch.h"
#include "ViewGrabImageUpdater.h"
//#include "SystemParameter.h"

#include "Algorithm/ImageProcessing.h"
#include "Gadget/TimeCheck.h"
#include "Widget/ImageView.h"
#include "../Grab/GrabExecutor.h"
#include "SystemParameter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ViewGrabImageUpdater::ViewGrabImageUpdater(IPVM::ImageView &imageViewBright, IPVM::ImageView &imageViewDark, long camIndex)
	: m_imageViewBright(imageViewBright)
	, m_imageViewDark(imageViewDark)
	, m_timeCheck(new IPVM::TimeCheck)
	, m_srcImage(new IPVM::Image_8u_C1)
	, m_resizedFullImage(new IPVM::Image_8u_C1)
	, m_resizedHalfImage(new IPVM::Image_8u_C1)
	, m_camIndex(camIndex)
{
	__super::BeginWorker();
}

ViewGrabImageUpdater::~ViewGrabImageUpdater()
{
	__super::EndWorker();

	delete m_resizedHalfImage;
	delete m_resizedFullImage;
	delete m_srcImage;
	delete m_timeCheck;
}

void ViewGrabImageUpdater::Push(const IPVM::Image_8u_C1 &iImage)
{
	CSingleLock lock(&m_cs, TRUE);

	// 이미지 뷰어는 정확한 타이밍에 정확한 이미지를 그릴 필요가 없으므로,
	// 얕은 카피로 포인터만 받아오는 것으로 한다.
	*m_srcImage = iImage;

	__super::Trigger();
}

bool ViewGrabImageUpdater::OnTrigger()
{
	// 이 값을 줄이면 메인 쓰레드 CPU 부하가 커짐.. 줄이지 말 것.
	if (m_timeCheck->Elapsed_ms() < 100.f)
	{
		return true;
	}

	m_timeCheck->Reset();

	IPVM::Image_8u_C1 srcImage;
	{
		CSingleLock lock(&m_cs, TRUE);
		srcImage = *m_srcImage;
	}

	if (g_systemParameter.machineType == MachineType::ES_mismatch
		|| g_systemParameter.machineType == MachineType::ES_rollpress_pinhole
		|| g_systemParameter.machineType == MachineType::ES_rollpress_surface)
	{
		const IPVM::Image_8u_C1 imageBright(srcImage.GetWidthBytes(), srcImage.GetSizeX(), srcImage.GetSizeY() / 2, srcImage.GetMem());
		const IPVM::Image_8u_C1 imageDark(  srcImage.GetWidthBytes(), srcImage.GetSizeX(), srcImage.GetSizeY() / 2, srcImage.GetMem() + (srcImage.GetWidthBytes() * srcImage.GetSizeY() / 2));

		m_resizedHalfImage->Create(imageBright.GetSizeX() / 4, imageBright.GetSizeY() / 4);
		IPVM::ImageProcessing::Resize_Nearest(imageBright, *m_resizedHalfImage);
		m_imageViewBright.SetImage(*m_resizedHalfImage);
		
		m_resizedHalfImage->Create(imageDark.GetSizeX() / 4, imageDark.GetSizeY() / 4);
		IPVM::ImageProcessing::Resize_Nearest(imageDark, *m_resizedHalfImage);
		m_imageViewDark.SetImage(*m_resizedHalfImage);
	}
	else
	{
		const IPVM::Image_8u_C1 imageAlpha(srcImage.GetWidthBytes() * 2, srcImage.GetSizeX(), srcImage.GetSizeY() / 2, srcImage.GetMem());
		const IPVM::Image_8u_C1 imageBeta(srcImage.GetWidthBytes() * 2, srcImage.GetSizeX(), srcImage.GetSizeY() / 2, srcImage.GetMem() + srcImage.GetWidthBytes());

		//const bool alphaIsBright = imageAlpha.GetValue(g_systemParameter.nMultiImageCheckPos, 0) >= g_systemParameter.nImageCheckBrightness;
		const bool alphaIsBright = true;

		const IPVM::Image_8u_C1 imageBright = alphaIsBright ? imageAlpha : imageBeta;
		{
			m_resizedHalfImage->Create(imageBright.GetSizeX() / 4, imageBright.GetSizeY() / 4);
			IPVM::ImageProcessing::Resize_Nearest(imageBright, *m_resizedHalfImage);
			m_imageViewBright.SetImage(*m_resizedHalfImage);
		}
		const IPVM::Image_8u_C1 imageDark = alphaIsBright ? imageBeta : imageAlpha;
		{
			m_resizedHalfImage->Create(imageDark.GetSizeX() / 4, imageDark.GetSizeY() / 4);
			IPVM::ImageProcessing::Resize_Nearest(imageDark, *m_resizedHalfImage);
			m_imageViewDark.SetImage(*m_resizedHalfImage);
		}
	}

	return true;
}
