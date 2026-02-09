#pragma once
#include "basedef.h"

class Database;
enum class MachineType;

class __GRAB_CLASS__ CISParameter
{
public:
	CISParameter(MachineType machineType);
	~CISParameter();

	void Reset();
	void LinkDataBase(bool bSave, Database &dbJob);
	bool IsCameraCellID_Back(long splitterHandleIndex) const;

	int m_nOddEvenNum[2];
	long m_nOddEvenValueBright[2];
	long m_nOddEvenValueDark[2];
	long m_nCh0[2];
	long m_nCh1[2];
	long m_nCh2[2];
	long m_nNumModules[2];
#ifndef _USE_CIS_LIVE_
	int m_nTriggerInput[2];
#endif
	long m_nTriggerDivision[2];
	BOOL m_bRemoteControl[2];
	int m_nDIOMode[2];
	BOOL m_bDualMode[2];
	long m_nHorizontalSize[2];
	BOOL m_bEncoderMultiplying[2];
	int m_nLedMode[2];
	BOOL m_bLineDivision[2];

private:
	MachineType m_machineType;

	// Cell ID를 Image의 앞에 붙일 것인가, 뒤에 붙일것인가
	// FALSE: ID-----------, TRUE: -----------ID
	// 해당값은 Normal Cell, Long Cell에 따라 하드 코딩되어 있다

	bool m_cameraCellID_isBack[4];
};
