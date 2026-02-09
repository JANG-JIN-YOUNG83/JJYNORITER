#include "stdafx.h"
#include "UrgentVariable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Inspection;

UrgentVariable::UrgentVariable()
	: m_compensateTabPitchOffset(0.f)
	, m_useValueToCloseToTabPitchSpec(false)
	, m_compensateCoatingOffset(-0.1)
{
}

UrgentVariable::~UrgentVariable()
{
}
