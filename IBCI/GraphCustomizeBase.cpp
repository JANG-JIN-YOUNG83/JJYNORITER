#include "pch.h"
#include "GraphCustomizeBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

GraphCustomizeBase::GraphCustomizeBase()
	: m_viewSizeX(0)
	, m_viewSizeY(0)
{
}

GraphCustomizeBase::~GraphCustomizeBase()
{
}

void GraphCustomizeBase::UpdateViewSize(CDC* /*dc*/, long cx, long cy)
{
	m_viewSizeX = cx;
	m_viewSizeY = cy;
}
