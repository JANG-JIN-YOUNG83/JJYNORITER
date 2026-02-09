#include "stdafx.h"
#include "InspectionDeeplearningPara.h"
#include "Accessor.h"
#include "../CompanySharedLibrary/Persistence/Database.h"
#include "InspectionConstantPara.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CInspectionDeeplearningPara::CInspectionDeeplearningPara()
	: m_bUseInspection(_T("Use_Inspection"), true)
	, m_nCountDefectType(_T("Count_Defect_Type"), (long)DefectType::END_Coater_Surface - (long)DefectType::START_Coater_Surface - 1)
{
	m_vecDefectType.reserve(m_nCountDefectType);
	m_vecbUseInspection.reserve(m_nCountDefectType);

	for (long cnt = 0; cnt < m_nCountDefectType; cnt++)
	{
		CString strType, strUse;

		strType.Format(_T("Defect_Type_%d"), cnt);
		strUse.Format(_T("Use_Inspection_%d"), cnt);

		m_vecDefectType.push_back(Base::ParaLong(strType, cnt));
		m_vecbUseInspection.push_back(Base::ParaBool(strUse, false));
	}
}

CInspectionDeeplearningPara::~CInspectionDeeplearningPara()
{
}

void CInspectionDeeplearningPara::LinkDataBase(bool bSave, bool isUpCamera, Database &db)
{
	m_bUseInspection.Link(bSave, db[_T("Use_Inspection")]);

	long nCountDefault = m_nCountDefectType;
	m_nCountDefectType.Link(bSave, db[_T("Count_Defect_Type")]);
	if (bSave == false)
	{
		if (nCountDefault != m_nCountDefectType)
		{
			m_nCountDefectType = nCountDefault;
			::MessageBox(NULL, _T("Deeplearning Para Counts Warning"), _T("Warning"), MB_ICONWARNING);
			return;
		}

		m_vecDefectType.clear();
		m_vecbUseInspection.clear();

		m_vecDefectType.reserve(nCountDefault);
		m_vecbUseInspection.reserve(nCountDefault);
	}

	for (long cnt = 0; cnt < m_nCountDefectType; cnt++)
	{
		CString strType, strUse;

		strType.Format(_T("Defect_Type_%d"), cnt);
		strUse.Format(_T("Use_Inspection_%d"), cnt);

		if (bSave == false)
        {
            m_vecDefectType.push_back(Base::ParaLong(strType, cnt));
            m_vecbUseInspection.push_back(Base::ParaBool(strUse, false));
        }

		m_vecDefectType[cnt].Link(bSave, db[strType]);
		m_vecbUseInspection[cnt].Link(bSave, db[strUse]);
	}
}