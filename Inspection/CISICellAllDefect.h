/*******************************************************************************
* 1cell에 모든 불량을 가지고 있다.
********************************************************************************/

#pragma once
#include <map>
#include <vector>
#include "CISIDefectProperty.h"
#include "CISIDefectResult.h"
#include "CIDISpecInfo.h"


class CISICellAllDefect
{
public:
	bool m_bUse;
	CISICellAllDefect::CISICellAllDefect();

	void Init(const int& nCellindex, IPVM::Image_8u_C1* pSrcBright = nullptr, IPVM::Image_8u_C1* pSrcDark = nullptr);
	void ResetResult();

	void SetCandidate(CISIDefectProperty* candidate);
	void SetCandidate(std::vector<CISIDefectProperty> vecCandidate);

	
	/*CISICellAllDefect& operator =(const CISICellAllDefect& hrs)
	{


		return *this;
	}*/

	std::vector<std::vector<CIDISpecInfo>> m_vvMeasureDefect;
	std::vector<CISIDefectProperty> m_vecCandidate;
	std::vector<long> m_vecISIResultCode;
	std::vector<long> m_vecISIDefectType;
	void SetResultData();
	std::map<long, IPVM::Rect>	m_mapInspAreaInfo;

	IPVM::Image_8u_C1* m_pSrcBright;
	IPVM::Image_8u_C1* m_pSrcDark;

	bool bisAnode;	//TRUE : 양극, FALSE : 음극
	long m_nCellindex;
	bool bisUp;		//TURE : Top, FALSE : Back
	CString strLotID;
    CString m_strTime;
	CISIDefectResult m_Judge;
    double m_dInspTotalTime_ms;
};

