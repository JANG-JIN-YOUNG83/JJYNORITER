#include "stdafx.h"
#include "basedef.h"
#include "CIDISpecInfo.h"

CIDISpecInfo::CIDISpecInfo(DefectType DT, long index)
	: m_defectType(DT)
	, m_nMeasureType(_T("measure_item_type"), index)
	, m_nMeasureStart(_T("measure_item_start_index"), 0)
	, m_fMeasureStartPosX(_T("measure_item_start_posX"), 0.f)
	, m_nMeasureEnd(_T("measure_item_end_index"), 1)
	, m_fMeasureEndPosX(_T("measure_item_end_posX"), 1.f)
	, m_fMeasureStandard(_T("measure_item_spec_standard"), 100.5)
	, m_fMeasureOffsetUCL(_T("measure_item_spec_offset_ucl"), 0.5)
	, m_fMeasureOffsetLCL(_T("measure_item_spec_offset_lcl"), -0.5)
	, m_fMeasureOffsetUSL(_T("measure_item_spec_offset_usl"), 0.5)
	, m_fMeasureOffsetLSL(_T("measure_item_spec_offset_lsl"), -0.5)
{
}

void CIDISpecInfo::init()
{

}


void CIDISpecInfo::LinkDataBase(bool bSave, Database& db)
{
	m_nMeasureType.Link(bSave, db[_T("measure_item_type")]);
	m_nMeasureStart.Link(bSave, db[_T("measure_item_start_index")]);
	m_fMeasureStartPosX.Link(bSave, db[_T("measure_item_start_posX")]);
	m_nMeasureEnd.Link(bSave, db[_T("measure_item_end_index")]);
	m_fMeasureEndPosX.Link(bSave, db[_T("measure_item_end_posX")]);
	m_fMeasureStandard.Link(bSave, db[_T("measure_item_spec_standard")]);
	m_fMeasureOffsetUCL.Link(bSave, db[_T("measure_item_spec_offset_ucl")]);
	m_fMeasureOffsetLCL.Link(bSave, db[_T("measure_item_spec_offset_lcl")]);
	m_fMeasureOffsetUSL.Link(bSave, db[_T("measure_item_spec_offset_usl")]);
	m_fMeasureOffsetLSL.Link(bSave, db[_T("measure_item_spec_offset_lsl")]);

	if (bSave == false)
	{
		long type = (long)DefectType::START_Coater_Dimension + 1 + m_nMeasureType;
		m_defectType = (DefectType)type;
	}
}