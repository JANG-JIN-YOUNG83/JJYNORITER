#include "stdafx.h"
#include "OptionCriteria.h"
#include "../CompanySharedLibrary/Persistence/Database.h"
#include "../SharedModule/VisionHostShare/SpecData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//========================================================================================================================

SpecCriteria::SpecCriteria()
{
	m_minusTolerance = 9999;
	m_plusTolerance = -9999;
}

SpecCriteria::~SpecCriteria()
{
}

void SpecCriteria::Init()
{
	m_minusTolerance = 9999;
	m_plusTolerance = -9999;
}

void SpecCriteria::Link(bool bSave, Database &db)
{
	if (true != db[_T("Min")].Link(bSave, m_minusTolerance)) m_minusTolerance = 9999;
	if (true != db[_T("Max")].Link(bSave, m_plusTolerance)) m_plusTolerance = -9999;
}

void SpecCriteria::LinkOld(bool bSave, Database &db, LPCTSTR minusKey, LPCTSTR plusKey)
{
	if (true != db[minusKey].Link(bSave, m_minusTolerance)) m_minusTolerance = 9999;
	if (true != db[plusKey].Link(bSave, m_plusTolerance)) m_plusTolerance = -9999;
}

bool SpecCriteria::In(double value) const
{
	bool useMin = false;
	bool useMax = false;

	// ~ -9998 까지 값은 사용하는 걸로 (=-9999 을 float연산이라 부호변환중 안전해 보이지 않아서)
	if (-m_minusTolerance > -9998) useMin = true;
	if (m_plusTolerance > -9998) useMax = true;

	if (useMin && value < -m_minusTolerance)
	{
		return false;
	}
	else if (useMax && value > m_plusTolerance)
	{
		return false;
	}

	return true;
}

bool SpecCriteria::In(float nominalValue, double value) const
{
	bool useMin = false;
	bool useMax = false;

	// ~ -9998 까지 값은 사용하는 걸로 (=-9999 을 float연산이라 부호변환중 안전해 보이지 않아서)
	if (-m_minusTolerance > -9998) useMin = true;
	if (m_plusTolerance > -9998) useMax = true;

	if (useMin && value < nominalValue - (m_minusTolerance))
	{
		return false;
	}
	else if (useMax && value > nominalValue + (m_plusTolerance))
	{
		return false;
	}

	return true;
}

bool SpecCriteria::IsUsed() const
{
	return !IsNotUsed();
}

bool SpecCriteria::IsNotUsed() const
{
	// ~ -9998 까지 값은 사용하는 걸로 (=-9999 을 float연산이라 부호변환중 안전해 보이지 않아서)
	if (-m_minusTolerance > -9998) return false;
	if (m_plusTolerance > -9998) return false;

	return true;
}

//========================================================================================================================

ObjectCriteria::ObjectCriteria()
{
}

ObjectCriteria::~ObjectCriteria()
{
}

void ObjectCriteria::Init()
{
	m_ai.Init();
	m_marking.Init();
}

void ObjectCriteria::Link(bool bSave, Database &db)
{
	m_ai.Link(bSave, db[_T("Ai")]);
	m_marking.Link(bSave, db[_T("Marking")]);
}

void ObjectCriteria::FromHost(const Share::SpecData& spec)
{
	if (this == nullptr) return;

	// Host는 Minus Spec은 -값으로 관리한다
	m_ai.m_minusTolerance = abs(spec.m_dAiMinusTolerance);
	m_ai.m_plusTolerance = spec.m_dAiPlusTolerance;

	//m_marking.m_minusTolerance = abs(spec.m_dMarkingMinusTolerance);
	//m_marking.m_plusTolerance = spec.m_dMarkingPlusTolerance;
}

void ObjectCriteria::ToHost(Share::SpecData& o_spec)
{
	if (this == nullptr) return;

	// Host는 Minus Spec은 -값으로 관리한다
	o_spec.m_dAiMinusTolerance = -m_ai.m_minusTolerance;
	o_spec.m_dAiPlusTolerance = m_ai.m_plusTolerance;

	//o_spec.m_dMarkingMinusTolerance = -m_marking.m_minusTolerance;
	//o_spec.m_dMarkingPlusTolerance = m_marking.m_plusTolerance;
}
