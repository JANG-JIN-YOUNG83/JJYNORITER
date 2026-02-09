#include "stdafx.h"
#include "CISIDefectResult.h"

#include "../SharedModule/CommonShare/DefectDef.h"


CISIDefectResult::CISIDefectResult()
{
	InitResult();
}

void CISIDefectResult::SetLogicStep(CString strDefectName, long nKey, classification::enLogicResult nNO0_YES1, CString strNote)
{
	m_vecLogicStep.push_back(stLogicStep(strDefectName, nKey, nNO0_YES1, strNote));
}

long CISIDefectResult::GetLogicStepNum()
{
	return m_vecLogicStep.size();
}
void CISIDefectResult::GetLogicStep(long index, CString& strDefectName, long& nKey, classification::enLogicResult& nNO0_YES1, CString& strNote)
{
	if (index >= m_vecLogicStep.size())
	{
		nKey = 0;
		nNO0_YES1 = classification::enLogicResult::NONE;
		return;
	}

	strDefectName = m_vecLogicStep[index].strDefectName;
	nKey = m_vecLogicStep[index].nStep;
	nNO0_YES1 = m_vecLogicStep[index].nNO0_YES1;
	strNote = m_vecLogicStep[index].strNote;
}


void CISIDefectResult::InitResult()
{
	m_Judge = II_RESULT_VALUE::NOT_MEASURED;
	m_nCandidateDefectCode = DefectType::END;
	m_vecLogicStep.clear();
}

II_RESULT_VALUE CISIDefectResult::SetJudge(DefectType DefectName, II_RESULT_VALUE m_Judge)
{
	this->m_nCandidateDefectCode = DefectName;
	this->m_Judge = m_Judge;

	return this->m_Judge;
}


II_RESULT_VALUE CISIDefectResult::GetJudge()
{
	return this->m_Judge;
}

DefectType CISIDefectResult::GetDefectResultCode()
{
	return this->m_nCandidateDefectCode;
}
