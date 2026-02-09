#pragma once
#include "../VisionModule/Base/ParaBool.h"
#include "../VisionModule/Base/ParaFloat.h"
#include "../VisionModule/Base/ParaLong.h"

class Database;

class CInspectionDeeplearningPara
{
public:
	CInspectionDeeplearningPara();
	~CInspectionDeeplearningPara();

public:
	void LinkDataBase(bool bSave, bool isUpCamera, Database &db);

public:
	Base::ParaLong m_nCountDefectType;

	Base::ParaBool	m_bUseInspection;
	std::vector< Base::ParaLong> m_vecDefectType;
	std::vector< Base::ParaBool> m_vecbUseInspection;
};
