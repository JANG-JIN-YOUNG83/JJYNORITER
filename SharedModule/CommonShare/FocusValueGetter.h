#pragma once
#include "basedef.h"
#include "Algorithm/basedef.h"
#include "Algorithm/EdgeDetectionDebugInfo.h"


#define SAME_ID_EXCEPTION 0
#define ID_BLANK_EXCEPTION 1

#define	MAX_FRAME_COUNT	3

class __COMMON_SHARE_CLASS__ FocusValueGetter
{
public:
	FocusValueGetter();
	~FocusValueGetter();
	static void GetSharpnessGV(IPVM::EdgeDetectionDebugInfo* pEdgeDetectionDebugInfo, int iEdgeInforNum, float& fEdgeVaule);

private:

};
