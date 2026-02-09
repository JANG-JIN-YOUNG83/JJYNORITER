#pragma once

#include "Gadget/SimpleWorkerThread.h"

class ViewGrabImageUpdater : public IPVM::SimpleWorkerThread
{
public:
	ViewGrabImageUpdater() = delete;
	ViewGrabImageUpdater(const ViewGrabImageUpdater &rhs) = delete;
	ViewGrabImageUpdater(IPVM::ImageView &imageViewBright, IPVM::ImageView &imageViewDark, long camIndex);
	virtual ~ViewGrabImageUpdater();

	ViewGrabImageUpdater &operator=(const ViewGrabImageUpdater &rhs) = delete;

public:
	void Push(const IPVM::Image_8u_C1 &iImage);
	long m_camIndex;

protected:
	virtual bool OnTrigger() override;

private:
	IPVM::ImageView &m_imageViewBright;
	IPVM::ImageView &m_imageViewDark;

	CCriticalSection m_cs;

	IPVM::TimeCheck *m_timeCheck;
	IPVM::Image_8u_C1 *m_srcImage;	

	IPVM::Image_8u_C1 *m_resizedFullImage;
	IPVM::Image_8u_C1 *m_resizedHalfImage;
};

