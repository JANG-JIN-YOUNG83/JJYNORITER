#include "stdafx.h"
#include "ProcessingItemDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Module Name은 저장과 관련 있으므로 함부로 바꾸지 않는다. Recipe가 깨진다

LPCTSTR g_procItemCategory[] =
{
	//_T("Configuration"),	// configuration
	_T("CellAlign"),		// cellAlign
	_T("Abnormal Inspection"),
	//_T("Uncoating"),		// uncoating,
	//_T("Tape"),			// tape,
	//_T("Surface"),		// Surface,
	//_T("Scrab"),			// Spec,
	//_T("TabFolding"),		// Tab,
	//_T("ShoulderLine"),	// shoulder,
	////_T("TabFolding"),	// tabFolding,
	//_T("ElectrodeTorn"),	// electrodeTorn,
	//_T("NgMark"),			// ngMark,
    _T("Chattering"),		// Chattering,
	_T("Measure"),			// measure,
	//_T("Burr"),			// burr,
	//_T("ETC"),			// Etc,
	//_T("ISI"),			// ISI,
	_T("Blob"),				// Blob,
	_T("Deeplearning"),		
	
};

//bool g_procItemCheck[((long)ProcItemCategory::END) / sizeof(LPCTSTR) == (long)ProcItemCategory::END ? true : false];
bool g_procItemCheck[sizeof(g_procItemCategory) / sizeof(LPCTSTR) == (long)ProcItemCategory::END ? true : false];

LPCTSTR __INSPECTOIN_CLASS__ GetProcItemModuleName(ProcItemCategory categoryIndex)
{
	return g_procItemCategory[(long)categoryIndex];
}
