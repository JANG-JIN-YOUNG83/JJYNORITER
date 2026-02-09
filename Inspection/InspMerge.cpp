#include "stdafx.h"
#include "InspMerge.h"
#include "InspectionCriteria.h"

#include "../CompanySharedLibrary/Persistence/Database.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

InspMerge::InspMerge()
	: m_type(InspMergeType::OR)
{
}

InspMerge::~InspMerge()
{
}

void InspMerge::LinkDataBase(bool bSave, Database &dbJob)
{
	if (!dbJob[_T("Type")].Link(bSave, (long &)m_type))
	{
		m_type = InspMergeType::OR;
	}
}

bool InspMerge::IsReject(float value1, float value2, const Inspection::InspectionCriteria& spec, void* result1, void* result2, void** o_masterResult) const
{
	*o_masterResult = result1;

	if (m_type == InspMergeType::OR)
	{
		if (value1 > -9998 && value2 > -9998)
		{
			if (spec.In(value1))
			{
				return false;
			}

			if (spec.In(value2))
			{
				*o_masterResult = result2;
				return false;
			}

			return true;
		}

		if (value1 > -9998)
		{
			if (!spec.In(value1)) return true;
		}
		else
		{
			*o_masterResult = result2;
			if (value2 > -9998 && !spec.In(value2)) return true;
		}
	}
	else
	{
		if (value1 > -9998 && !spec.In(value1)) return true;
		if (value2 > -9998 && !spec.In(value2))
		{
			*o_masterResult = result2;
			return true;
		}
	}

	return false;
}
