#include "stdafx.h"
#include "InspectionAbnormalPara.h"
#include "Accessor.h"
#include "../CompanySharedLibrary/Persistence/Database.h"
#include "InspectionConstantPara.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CInspectionAbnormalPara::CInspectionAbnormalPara()
: m_nGrayValue(_T("GrayValue of ROI"), 0)
, m_nGrayValue_2(_T("GrayValue of ROI 2"), 0)
, m_rtOffsetRoi(_T("Adjust Para OffsetRoi"), IPVM::Rect(0, 0, 0, 0))
, m_rtSetInspArea(_T("Insp Area Set"), IPVM::Rect(0, 0, 0, 0))
, m_bInspectionSkip(_T("Inspeciton Skip"), 0)
, m_rtSetInspArea_2(_T("Insp Area Set 2"), IPVM::Rect(0, 0, 0, 0))
{
}

CInspectionAbnormalPara::~CInspectionAbnormalPara()
{
}

void CInspectionAbnormalPara::LinkDataBase(bool bSave, bool isUpCamera, Database& db)
{
	long version = 20211203;

	if (!db[_T("Version")].Link(bSave, version))
	{
		version = 0;
	}
	m_nGrayValue.Link(bSave, db[_T("GrayValue of ROI")]);
	m_rtOffsetRoi.Link(bSave, db[_T("Adjust Para OffsetRoi")]);
	m_rtSetInspArea.Link(bSave, db[_T("Insp Area Set")]);
	m_bInspectionSkip.Link(bSave, db[_T("Inspeciton Skip")]);
    m_rtSetInspArea_2.Link(bSave, db[_T("Insp Area Set 2")]);
    m_nGrayValue_2.Link(bSave, db[_T("GrayValue of ROI 2")]);
}
