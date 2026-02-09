#pragma once

#include <vector>
#include "basedef.h"

struct stLogicStep
{
	stLogicStep()
	{
		strDefectName = _T("");
		nStep = 0;
		nNO0_YES1 = classification::enLogicResult::YES;
		strNote = _T("");
	}

	stLogicStep(CString strDefectName, long nStep, classification::enLogicResult nNO0_YES1, CString strNote = _T(""))
	{
		this->strDefectName = strDefectName;
		this->nStep = nStep;
		this->nNO0_YES1 = nNO0_YES1;
		this->strNote = strNote;
	}

	long nStep;
	classification::enLogicResult nNO0_YES1;
	CString strDefectName;
	CString strNote;
};

class CISIDefectResult
{
	std::vector<stLogicStep> m_vecLogicStep;
	DefectType m_nCandidateDefectCode;
	II_RESULT_VALUE m_Judge;

public:
	CISIDefectResult::CISIDefectResult();

	void InitResult();
	void SetLogicStep(CString strDefectName, long nKey, classification::enLogicResult nNO0_YES1, CString strNote);

	long GetLogicStepNum();
	void GetLogicStep(long index, CString& strDefectName, long& nKey, classification::enLogicResult& nNO0_YES1, CString& strNote);

	II_RESULT_VALUE SetJudge(DefectType DefectName, II_RESULT_VALUE judge);
	II_RESULT_VALUE GetJudge();

	DefectType GetDefectResultCode();

};