#pragma once

#include "../SharedModule/CommonShare/DefectDef.h"
#include "../CompanySharedLibrary/Persistence/Database.h"
#include "basedef.h"

//#include "../VisionModule/Base/ParaBool.h"
//#include "../VisionModule/Base/ParaFloat.h"
//#include "../VisionModule/Base/ParaLong.h"


class CIDIDefectResult
{
	//DefectType m_nCandidateDefectCode;
	II_RESULT_VALUE m_Judge;

public:
	CIDIDefectResult::CIDIDefectResult()
	{
		InitResult();
	}
	void CIDIDefectResult::InitResult()
	{
		m_Judge = II_RESULT_VALUE::NOT_MEASURED;
	}
	II_RESULT_VALUE CIDIDefectResult::SetJudge(II_RESULT_VALUE m_Judge)
	{
		this->m_Judge = m_Judge;
		return this->m_Judge;
	}
	II_RESULT_VALUE CIDIDefectResult::GetJudge()
	{
		return this->m_Judge;
	}
};

class CIDIDefectProperty : private CIDIDefectResult
{
public:
	Base::ParaFloat LENGTH;

	CIDIDefectProperty::CIDIDefectProperty()
		: LENGTH(_T("LENGTH"), 0.)
	{
	}
	void InitResult()
	{
		CIDIDefectResult::InitResult();
	}
	II_RESULT_VALUE SetJudge(II_RESULT_VALUE Judge)
	{
		return CIDIDefectResult::SetJudge(Judge);
	}
	II_RESULT_VALUE GetJudge()
	{
		return CIDIDefectResult::GetJudge();
	}
};

class CIDISpecInfo : public CIDIDefectProperty
{
public:
	CIDISpecInfo::CIDISpecInfo(DefectType DT, long index);

	void init();

	DefectType m_defectType;

	Base::ParaLong  m_nMeasureType;
	Base::ParaLong  m_nMeasureStart;
	Base::ParaFloat	m_fMeasureStartPosX;
	Base::ParaLong  m_nMeasureEnd;
	Base::ParaFloat	m_fMeasureEndPosX;
				    
	Base::ParaFloat m_fMeasureStandard;
	Base::ParaFloat m_fMeasureOffsetUCL;  // 현재는 USL, LSL만 사용, USL= 기존 max
	Base::ParaFloat m_fMeasureOffsetLCL;  // 현재는 USL, LSL만 사용, USL= 기존 max
	Base::ParaFloat m_fMeasureOffsetUSL;
	Base::ParaFloat m_fMeasureOffsetLSL;



	void LinkDataBase(bool bSave, Database& db);
};
