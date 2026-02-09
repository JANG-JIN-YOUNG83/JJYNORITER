#pragma once

#include "basedef.h"
// 검사 순서를 결정하는 부분!!!!!!!!!!!!! 동네사람들 이거줌 봐유
enum class ProcItemCategory
{
	
	cellAlign,
	abnormalInspection,
	Chattering,
	measure,
	Blob,
	deeplearning,
	
	END,
	surface,
	configuration,
	uncoating,
	tape,
	scrab,
	tab,
	shoulder,
	electrodeTorn,
	ngMark,
	burr,
	ETC,
	ISI,
	
};

LPCTSTR __INSPECTOIN_CLASS__ GetProcItemModuleName(ProcItemCategory categoryIndex);
