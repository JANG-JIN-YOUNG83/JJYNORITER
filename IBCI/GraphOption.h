#pragma once
#include "basedef.h"
#include "../SharedModule/CommonShare/DefectDef.h"

enum class GroupGraphType2
{
	None,
	TopCuttingHeight,
	BottomCuttingHeight,
	TopOverlayWidth,
	BottomOverlayWidth,
	Length,
	Length_2,
	END,
};

enum class GroupGraphType
{
	None,
	Mismatch1,
	Mismatch2,
	Mismatch3,
	Mismatch4,
	Mismatch5,
	Mismatch6,
	Mismatch7,
	Mismatch8,
	END,
};

class GraphOption
{
	public:
		static GroupGraphType GetGroupGraphType(DefectType type);
		static LPCTSTR GetGroupGraphName(GroupGraphType type);
		static bool IsOnlyMaxDiffValueShowed(GroupGraphType type);
};

