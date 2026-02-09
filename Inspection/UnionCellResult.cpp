#include "stdafx.h"
#include "UnionCellResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UnionCellResult::UnionCellResult()
	: m_isRejectTape(false)
	, m_isRejectUncoatingBoundary(false)
	, m_measureShoulderLine(Inspection::MeasureOffsetType::ShoulderLine)
{
}

UnionCellResult::~UnionCellResult()
{
}
