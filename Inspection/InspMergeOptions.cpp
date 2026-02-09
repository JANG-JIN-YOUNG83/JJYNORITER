#include "stdafx.h"
#include "InspMergeOptions.h"

#include "../CompanySharedLibrary/Persistence/Database.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Inspection;

InspMergeOptions::InspMergeOptions()
{
}

InspMergeOptions::~InspMergeOptions()
{
}

void InspMergeOptions::LinkDataBase(bool bSave, Database &dbJob)
{
	m_mergeLength.LinkDataBase(bSave, dbJob[_T("Length")]);
	m_mergeTopTilt.LinkDataBase(bSave, dbJob[_T("Top Tilt")]);
	m_mergeBottomTilt.LinkDataBase(bSave, dbJob[_T("Bottom Tilt")]);
	m_mergeMisalign.LinkDataBase(bSave, dbJob[_T("Misalign")]);
}
