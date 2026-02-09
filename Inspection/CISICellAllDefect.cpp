#include "stdafx.h"
#include "CISICellAllDefect.h"

CISICellAllDefect::CISICellAllDefect()
{
	m_bUse = true;
}


void CISICellAllDefect::Init(const int& nCellindex, IPVM::Image_8u_C1* pSrcBright, IPVM::Image_8u_C1* pSrcDark)
{
	//m_vvMeasureDefect.clear();
	m_vecCandidate.clear();
	m_mapInspAreaInfo.clear();

	if (nullptr != pSrcBright)
		m_pSrcBright = pSrcBright;

	if (nullptr != pSrcDark)
		m_pSrcDark = pSrcDark;


	m_nCellindex = nCellindex;
	for (int i = 0; i < enInspectionArea::IA_END; i++)
		m_mapInspAreaInfo.insert(std::pair<long, IPVM::Rect>(i, IPVM::Rect(0, 0, 0, 0)));


}

void CISICellAllDefect::ResetResult()
{
	for (int i = 0; i < m_vecCandidate.size(); i++)
	{
		m_vecCandidate[i].InitResult();
	}

	m_Judge.InitResult();
}

void CISICellAllDefect::SetResultData()
{
	m_vecISIResultCode.resize(m_vecCandidate.size());
	m_vecISIDefectType.resize(m_vecCandidate.size());
	for (long n = 0; n < m_vecCandidate.size(); n++)
	{
		m_vecISIResultCode[n] = (long)m_vecCandidate[n].GetJudge();
		m_vecISIDefectType[n] = (long)m_vecCandidate[n].GetDefectResultCode(); 
	}
}

void CISICellAllDefect::SetCandidate(CISIDefectProperty* candidate)
{
	m_vecCandidate.push_back(*candidate);
}

void CISICellAllDefect::SetCandidate(std::vector<CISIDefectProperty> vecCandidate)
{
	for (int i = 0; i < vecCandidate.size(); i++)
	{
		m_vecCandidate.push_back(vecCandidate[i]);
	}
}