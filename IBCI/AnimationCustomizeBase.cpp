#include "pch.h"
#include "AnimationCustomizeBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

AnimationCustomizeBase::AnimationCustomizeBase()
	: m_viewSizeX(0)
	, m_viewSizeY(0)
{
}

AnimationCustomizeBase::~AnimationCustomizeBase()
{
}

void AnimationCustomizeBase::UpdateViewSize(CDC* /*dc*/, long cx, long cy)
{
	m_viewSizeX = cx;
	m_viewSizeY = cy;
}
