#include "stdafx.h"
#include "CISParameter.h"
#include "GrabExecutor.h"

#include "../CompanySharedLibrary/Persistence/Database.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CISParameter::CISParameter(MachineType machineType)
	: m_machineType(machineType)
{
	Reset();
}

CISParameter::~CISParameter()
{
}

void CISParameter::Reset()
{
	// CIS 파라미터 초기화
	for (int i = 0; i < 2; i++)
	{
		m_nCh0[i] = 66;
		m_nCh1[i] = 16;
		m_nCh2[i] = 50;
		m_nNumModules[i] = 0;
#ifndef _USE_CIS_LIVE_
		m_nTriggerInput[i] = 0;
#endif
		m_nTriggerDivision[i] = 6;
		m_bRemoteControl[i] = 0;
		m_nDIOMode[i] = 0;
		m_bDualMode[i] = 0;
		m_nHorizontalSize[i] = 8640;
		m_bEncoderMultiplying[i] = 0;
		m_nLedMode[i] = 3;
		m_bLineDivision[i] = TRUE;
		m_nOddEvenValueBright[i] = 64;
		m_nOddEvenValueDark[i] = 32;
		m_nOddEvenNum[i] = 0;
	}

	for (int i = 0; i < 4; i++)
	{
		m_cameraCellID_isBack[i] = false;
	}

	//if (0 != m_nCamSingleDual)
	if(m_machineType == MachineType::ES_notching_dual)
	{
		// Long Cell
		m_cameraCellID_isBack[1] = true;
		m_cameraCellID_isBack[3] = true;
	}
}

void CISParameter::LinkDataBase(bool bSave, Database &dbJob)
{
	if (!bSave) Reset();

	switch (m_machineType)
	{
	case MachineType::ES_notching_single:
	{
		auto& db_Up = dbJob[_T("CIS UP")];

		db_Up[_T("Num Of Modules[0]")].Link(bSave, m_nNumModules[0]);
#ifndef _USE_CIS_LIVE_
		db_Up[_T("Trigger Input[0]")].Link(bSave, m_nTriggerInput[0]);
#endif
		db_Up[_T("Trigger Division[0]")].Link(bSave, m_nTriggerDivision[0]);
		db_Up[_T("Remote Control[0]")].Link(bSave, m_bRemoteControl[0]);
		db_Up[_T("DIO Mode[0]")].Link(bSave, m_nDIOMode[0]);
		db_Up[_T("Dual Mode[0]")].Link(bSave, m_bDualMode[0]);
		db_Up[_T("Horizontal Size[0]")].Link(bSave, m_nHorizontalSize[0]);
		db_Up[_T("Encoder Multiplying[0]")].Link(bSave, m_bEncoderMultiplying[0]);
		db_Up[_T("LED Mode[0]")].Link(bSave, m_nLedMode[0]);
		db_Up[_T("Ch0[0]")].Link(bSave, m_nCh0[0]);
		db_Up[_T("Ch1[0]")].Link(bSave, m_nCh1[0]);
		db_Up[_T("Ch2[0]")].Link(bSave, m_nCh2[0]);
		db_Up[_T("Odd Even Num[0]")].Link(bSave, m_nOddEvenNum[0]);
		db_Up[_T("Odd Even Value(Bright)[0]")].Link(bSave, m_nOddEvenValueBright[0]);
		db_Up[_T("Odd Even Value(Dark)[0]")].Link(bSave, m_nOddEvenValueDark[0]);
		db_Up[_T("Line Division[0]")].Link(bSave, m_bLineDivision[0]);

		auto& db_Down = dbJob[_T("CIS DOWN")];

		db_Down[_T("Num Of Modules[1]")].Link(bSave, m_nNumModules[1]);
#ifndef _USE_CIS_LIVE_
		db_Down[_T("Trigger Input[1]")].Link(bSave, m_nTriggerInput[1]);
#endif
		db_Down[_T("Trigger Division[1]")].Link(bSave, m_nTriggerDivision[1]);
		db_Down[_T("Remote Control[1]")].Link(bSave, m_bRemoteControl[1]);
		db_Down[_T("DIO Mode[1]")].Link(bSave, m_nDIOMode[1]);
		db_Down[_T("Dual Mode[1]")].Link(bSave, m_bDualMode[1]);
		db_Down[_T("Horizontal Size[1]")].Link(bSave, m_nHorizontalSize[1]);
		db_Down[_T("Encoder Multiplying[1]")].Link(bSave, m_bEncoderMultiplying[1]);
		db_Down[_T("LED Mode[1]")].Link(bSave, m_nLedMode[1]);
		db_Down[_T("Ch0[1]")].Link(bSave, m_nCh0[1]);
		db_Down[_T("Ch1[1]")].Link(bSave, m_nCh1[1]);
		db_Down[_T("Ch2[1]")].Link(bSave, m_nCh2[1]);
		db_Down[_T("Odd Even Num[1]")].Link(bSave, m_nOddEvenNum[1]);
		db_Down[_T("Odd Even Value(Bright)[1]")].Link(bSave, m_nOddEvenValueBright[1]);
		db_Down[_T("Odd Even Value(Dark)[1]")].Link(bSave, m_nOddEvenValueDark[1]);
		db_Down[_T("Line Division[1]")].Link(bSave, m_bLineDivision[1]);

		break;
	}
	case MachineType::ES_notching_dual:
	{
		auto& db_Up = dbJob[_T("CIS UP(Long)")];

		db_Up[_T("Num Of Modules[0]")].Link(bSave, m_nNumModules[0]);
#ifndef _USE_CIS_LIVE_
		db_Up[_T("Trigger Input[0]")].Link(bSave, m_nTriggerInput[0]);
#endif
		db_Up[_T("Trigger Division[0]")].Link(bSave, m_nTriggerDivision[0]);
		db_Up[_T("Remote Control[0]")].Link(bSave, m_bRemoteControl[0]);
		db_Up[_T("DIO Mode[0]")].Link(bSave, m_nDIOMode[0]);
		db_Up[_T("Dual Mode[0]")].Link(bSave, m_bDualMode[0]);
		db_Up[_T("Horizontal Size[0]")].Link(bSave, m_nHorizontalSize[0]);
		db_Up[_T("Encoder Multiplying[0]")].Link(bSave, m_bEncoderMultiplying[0]);
		db_Up[_T("LED Mode[0]")].Link(bSave, m_nLedMode[0]);
		db_Up[_T("Ch0[0]")].Link(bSave, m_nCh0[0]);
		db_Up[_T("Ch1[0]")].Link(bSave, m_nCh1[0]);
		db_Up[_T("Ch2[0]")].Link(bSave, m_nCh2[0]);
		db_Up[_T("Odd Even Num[0]")].Link(bSave, m_nOddEvenNum[0]);
		db_Up[_T("Odd Even Value(Bright)[0]")].Link(bSave, m_nOddEvenValueBright[0]);
		db_Up[_T("Odd Even Value(Dark)[0]")].Link(bSave, m_nOddEvenValueDark[0]);
		db_Up[_T("Line Division[0]")].Link(bSave, m_bLineDivision[0]);

		auto& db_Down = dbJob[_T("CIS DOWN(Long)")];

		db_Down[_T("Num Of Modules[2]")].Link(bSave, m_nNumModules[1]);
#ifndef _USE_CIS_LIVE_
		db_Down[_T("Trigger Input[2]")].Link(bSave, m_nTriggerInput[1]);
#endif
		db_Down[_T("Trigger Division[2]")].Link(bSave, m_nTriggerDivision[1]);
		db_Down[_T("Remote Control[2]")].Link(bSave, m_bRemoteControl[1]);
		db_Down[_T("DIO Mode[2]")].Link(bSave, m_nDIOMode[1]);
		db_Down[_T("Dual Mode[2]")].Link(bSave, m_bDualMode[1]);
		db_Down[_T("Horizontal Size[2]")].Link(bSave, m_nHorizontalSize[1]);
		db_Down[_T("Encoder Multiplying[2]")].Link(bSave, m_bEncoderMultiplying[1]);
		db_Down[_T("LED Mode[2]")].Link(bSave, m_nLedMode[1]);
		db_Down[_T("Ch0[2]")].Link(bSave, m_nCh0[1]);
		db_Down[_T("Ch1[2]")].Link(bSave, m_nCh1[1]);
		db_Down[_T("Ch2[2]")].Link(bSave, m_nCh2[1]);
		db_Down[_T("Odd Even Num[2]")].Link(bSave, m_nOddEvenNum[1]);
		db_Down[_T("Odd Even Value(Bright)[2]")].Link(bSave, m_nOddEvenValueBright[1]);
		db_Down[_T("Odd Even Value(Dark)[2]")].Link(bSave, m_nOddEvenValueDark[1]);
		db_Down[_T("Line Division[2]")].Link(bSave, m_bLineDivision[1]);

		break;
	}
	}
}

bool CISParameter::IsCameraCellID_Back(long splitterHandleIndex) const
{
	return m_cameraCellID_isBack[splitterHandleIndex];
}
