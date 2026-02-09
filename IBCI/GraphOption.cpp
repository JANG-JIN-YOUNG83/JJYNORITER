#include "pch.h"
#include "GraphOption.h"


#ifdef _DEBUG
#	define new DEBUG_NEW
#	undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

GroupGraphType GraphOption::GetGroupGraphType(DefectType type)
{

#ifndef __disable_graph_group__
	switch (type)
	{
	case DefectType::Dimension_Mismatch1:
		return GroupGraphType::Mismatch1;
	case DefectType::Dimension_Mismatch2:
		return GroupGraphType::Mismatch2;
	case DefectType::Dimension_Mismatch3:
		return GroupGraphType::Mismatch3;
	case DefectType::Dimension_Mismatch4:
		return GroupGraphType::Mismatch4;
	case DefectType::Dimension_Mismatch5:
		return GroupGraphType::Mismatch5;
	case DefectType::Dimension_Mismatch6:
		return GroupGraphType::Mismatch6;
	case DefectType::Dimension_Mismatch7:
		return GroupGraphType::Mismatch7;
	case DefectType::Dimension_Mismatch8:
		return GroupGraphType::Mismatch8;
	}
 
  return GroupGraphType::None;
#endif
}

LPCTSTR GraphOption::GetGroupGraphName(GroupGraphType type)
{
	switch (type)
	{
	case GroupGraphType::Mismatch1:		return _T("Mismatch #1");
	case GroupGraphType::Mismatch2:		return _T("Mismatch #2");
	case GroupGraphType::Mismatch3:		return _T("Mismatch #3");
	case GroupGraphType::Mismatch4:		return _T("Mismatch #4");
	case GroupGraphType::Mismatch5:		return _T("Mismatch #5");
	case GroupGraphType::Mismatch6:		return _T("Mismatch #6");
	case GroupGraphType::Mismatch7:		return _T("Mismatch #7");
	case GroupGraphType::Mismatch8:		return _T("Mismatch #8");
	}

	return _T("Unknown");
}

bool GraphOption::IsOnlyMaxDiffValueShowed(GroupGraphType type)
{
	//if (type == GroupGraphType::Length || type == GroupGraphType::Length_2)
	//{
	//	// 전장은 Data중 Spec과 가장 차이가 많이 나는 녀석만 보여준다.
	//	return true;
	//}

	return false;
}
