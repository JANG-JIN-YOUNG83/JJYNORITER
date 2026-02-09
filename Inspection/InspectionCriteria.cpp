#include "stdafx.h"
#include "InspectionCriteria.h"

#include "../CompanySharedLibrary/Persistence/Database.h"
#include "../SharedModule/VisionHostShare/SpecData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Inspection;

InspectionCriteria::InspectionCriteria(bool useStandardValue)
	: m_useStandardValue(useStandardValue)
	, m_standardValue_mm(0.)
	, m_specMin_mm(0.)
	, m_specMax_mm(0.)
{
}

InspectionCriteria::~InspectionCriteria()
{
}

void InspectionCriteria::FromHost(const Share::SpecData& data)
{
	if (m_useStandardValue)
	{
		m_standardValue_mm = data.m_dStandard_Value;
	}

	m_specMin_mm = data.m_dMinusTolerance;
	m_specMax_mm = data.m_dPlusTolerance;
}

void InspectionCriteria::Reset()
{
	m_standardValue_mm = 0.;
	m_specMin_mm = 0.;
	m_specMax_mm = 0.;
}

void InspectionCriteria::LinkDataBase(bool bSave, Database &db)
{
	if (!bSave) Reset();

	if (m_useStandardValue)
	{
		db[_T("StandardValue")].Link(bSave, m_standardValue_mm);
	}

	db[_T("Spec Min")].Link(bSave, m_specMin_mm);
	db[_T("Spec Max")].Link(bSave, m_specMax_mm);
}

void InspectionCriteria::LoadDataBaseOld(Database &db, LPCTSTR useMin, LPCTSTR useMax, LPCTSTR valueMin, LPCTSTR valueMax)
{
	// 구버전 호환용
	bool enableMin = false;
	bool enableMax = false;

	float valueMin_um = -9999999.f;
	float valueMax_um = 9999999.f;

	if (true != db[useMin].Link(FALSE, enableMin)) enableMin = false;
	if (true != db[useMax].Link(FALSE, enableMax)) enableMax = false;

	if (enableMin)
	{
		if (true != db[valueMin].Link(FALSE, valueMin_um)) valueMin_um = 0.f;
	}

	if (enableMax)
	{
		if (true != db[valueMax].Link(FALSE, valueMax_um)) valueMax_um = 0.f;
	}

	m_standardValue_mm = (valueMin_um + valueMax_um) / 2000.;
	m_specMin_mm = valueMin_um / 1000. - m_standardValue_mm;
	m_specMax_mm = valueMax_um / 1000. - m_standardValue_mm;
}

void InspectionCriteria::LoadDataBaseOld(Database &db, LPCTSTR valueMin, LPCTSTR valueMax)
{
	float valueMin_um = 0.f;
	float valueMax_um = 0;

	if (true != db[valueMin].Link(FALSE, valueMin_um)) valueMin_um = 0.f;
	if (true != db[valueMax].Link(FALSE, valueMax_um)) valueMax_um = 0.f;
	m_specMin_mm = -valueMin_um / 1000.;
	m_specMax_mm = valueMax_um / 1000.;
}

void InspectionCriteria::LoadDataBaseOld(Database &db, LPCTSTR standard, LPCTSTR valueMin, LPCTSTR valueMax)
{
	LoadDataBaseOld(db, valueMin, valueMax);

	float standard_um = 0.f;
	if (true != db[standard].Link(FALSE, standard_um)) standard_um = 0.f;

	ASSERT(m_useStandardValue);

	if (m_useStandardValue)
	{
		m_standardValue_mm = standard_um / 1000.;
	}
}

bool InspectionCriteria::In(double value_mm) const
{
	if (value_mm > m_standardValue_mm + m_specMax_mm ||
		value_mm < m_standardValue_mm + m_specMin_mm)
	{
		return false;
	}

	return true;
}

void InspectionCriteria::ToHost(Share::SpecData& o_data) const
{
	if (m_useStandardValue)
	{
		o_data.m_dStandard_Value = m_standardValue_mm;
	}

	o_data.m_dMinusTolerance = m_specMin_mm;
	o_data.m_dPlusTolerance = m_specMax_mm;
}
