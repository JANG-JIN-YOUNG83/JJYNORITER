#include "stdafx.h"
#include "InspectionChatteringPara.h"
#include "Accessor.h"
#include "../CompanySharedLibrary/Persistence/Database.h"
#include "InspectionConstantPara.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CInspectionChatteringPara::CInspectionChatteringPara()
    : m_nDivCount(_T("Div_Count"), 0)
    , m_nStandard(_T("Standard"), 0)
    , m_nPeriodMax(_T("Preiod_Max"), 0)
    , m_nPeriodMin(_T("Preiod_Min"), 0)
    , m_nAmpMax(_T("Amp_Max"), 0)
    , m_nAmpMin(_T("Amp_Min"), 0)
    , m_rtSetInspArea(_T("Insp Area Set"), IPVM::Rect(0, 0, 0, 0))
    , m_rtOffsetRoi(_T("Set Offset"), IPVM::Rect(0, 0, 0, 0))
    , m_bUseInspection(_T("Use Inspection"), false)
    , m_nSelectDarkImage(_T("Select Image(Bright/Dark)"), 0)
    , m_rtSetAlignArea(_T("Align Area Set"), IPVM::Rect(0,0,0,0))
{
}

CInspectionChatteringPara::~CInspectionChatteringPara()
{
}

void CInspectionChatteringPara::LinkDataBase(bool bSave, bool isUpCamera, Database& db)
{
    long version = 20211203;

    if (!db[_T("Version")].Link(bSave, version))
    {
        version = 0;
    }
    m_nDivCount.Link(bSave, db[_T("Div_Count")]);
    m_nStandard.Link(bSave, db[_T("Standard")]);

    m_nPeriodMax.Link(bSave, db[_T("Preiod_Max")]);
    m_nPeriodMin.Link(bSave, db[_T("Preiod_Min")]);
    m_nAmpMax.Link(bSave, db[_T("Amp_Max")]);
    m_nAmpMin.Link(bSave, db[_T("Amp_Min")]);
    m_rtSetInspArea.Link(bSave, db[_T("Insp Area Set")]);
    m_rtOffsetRoi.Link(bSave, db[_T("Set Offset")]);
    m_bUseInspection.Link(bSave, db[_T("Use Inspection")]);
    m_nSelectDarkImage.Link(bSave, db[_T("Select Image(Bright/Dark)")]);
    m_rtSetAlignArea.Link(bSave, db[_T("Align Area Set")]);

}
