#include "stdafx.h"
#include "MeasureValue.h"
#include "Accessor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Inspection;

MeasureValue::MeasureValue(MeasureOffsetType offsetType)
	: m_offsetType(offsetType)
{
	Reset();
}

MeasureValue::~MeasureValue()
{
}

double MeasureValue::GetValue_um() const
{
	return m_value_um;
}

double MeasureValue::GetValue_mm() const
{
	return m_value_um / 1000.;
}

MeasureValue& MeasureValue::operator = (const MeasureValue& rhs)
{
	ASSERT(m_offsetType == rhs.m_offsetType);

	m_value_um = rhs.m_value_um;

	return *this;
}

void MeasureValue::Reset()
{
	m_value_um = 0.;
}

void MeasureValue::Set_mm_ApplyOffset(bool isUp, double value)
{
	Set_um_ApplyOffset(isUp, value * 1000.);
}

void MeasureValue::Set_um_ApplyOffset(bool isUp, double value)
{
	double offset_um = Accessor::Get().m_measureOffsets.GetValue_mm(isUp, m_offsetType) * 1000.;

	m_value_um = value + offset_um;
}

void MeasureValue::Set_mm(double value)
{
	m_value_um = value * 1000.;
}

void MeasureValue::Set_um(double value)
{
	m_value_um = value;
}
