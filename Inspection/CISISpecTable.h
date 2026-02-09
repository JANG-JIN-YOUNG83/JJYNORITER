#pragma once
#include "basedef.h"
#include <map>

#include "CISISpecInfo.h"
#include "../SharedModule/CommonShare/DefectDef.h"


class __INSPECTOIN_CLASS__ CISISpecTable
{
	

public:
	//std::map<DefectType, CISISpecInfo> m_mapSpec;
    std::map<DefectType, std::vector<CISISpecInfo>> m_mapSpec_multi;
	Base::ParaBool  m_bUse;

	CISISpecTable::CISISpecTable();
	std::vector<long> m_vecInspAreaSet;
	long m_nInspAreaSetSize;
	void LinkDataBase(bool bSave, Database& db);

	void SetSpecInfo(DefectType defectName, float fSizeX, float fSizeY, bool ORAND);

	CISISpecInfo* GetSpecInfo(DefectType defectName, long nCondition = 0);
	long GetSpecCount();

	void initSpec();
	void initSmallSpec();

	bool SetValueTogether(DefectType DT, enColTitle title, float value);
	std::vector<DefectType>* GetValueTogether(DefectType DT, enColTitle title);

	void FromHost(const Share::SpecData& data, DefectType defecttype);
	DefectType TranslateDefectType(const DefectType Hostdefecttype);

	std::vector<CISISpecInfo>* GetSpecInfo_Multi(DefectType defectName);
};


