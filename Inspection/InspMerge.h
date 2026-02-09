#pragma once
#include "basedef.h"

enum class InspMergeType : long
{
	OR,
	AND,
};

class Database;
class RESULT_INFO;

class __INSPECTOIN_CLASS__ InspMerge
{
public:
	InspMerge();
	~InspMerge();

	void LinkDataBase(bool bSave, Database &dbJob);
	bool IsReject(float value1, float value2, const Inspection::InspectionCriteria& spec, void* result1, void* result2, void** o_masterResult) const;

	InspMergeType m_type;
};

