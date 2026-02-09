#include "stdafx.h"
#include "CountWarehouse.h"

#include "../CommonShare/ArchiveAllType.h"
#include "../CommonShare/DefectDef.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


DefectCountInfo::DefectCountInfo()
{
	m_ninspType = 0;
	m_npairType = 0;
	bPairCheck = 0;
	nAINGCount = 0;

	NG_COUNT[0][0] = 0;
	NG_COUNT[0][1] = 0;
	NG_COUNT[1][0] = 0;
	NG_COUNT[1][1] = 0;
	PAIR_NG_COUNT[0][0] = 0;
	PAIR_NG_COUNT[0][1] = 0;
	PAIR_NG_COUNT[1][0] = 0;
	PAIR_NG_COUNT[1][1] = 0;
}


void DefectCountInfo::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(m_ninspType);
	ar.Serialize_Element(m_npairType);
	ar.Serialize_Element(bPairCheck);
	ar.Serialize_Element(nAINGCount);

	for (int j = 0; j < 2; j++)
	{
		for (int i = 0; i < 2; i++)
		{
			ar.Serialize_Element(NG_COUNT[j][i]);
			ar.Serialize_Element(PAIR_NG_COUNT[j][i]);
		}
	}
}

DefectCountInfo& DefectCountInfo::operator = (const DefectCountInfo& src)
{
	m_ninspType = src.m_ninspType; 
	m_npairType = src.m_npairType;
	bPairCheck = src.bPairCheck;
	nAINGCount = src.nAINGCount;

	memcpy((long*)NG_COUNT, (long*)src.NG_COUNT, sizeof(long) * 4);
	memcpy((long*)PAIR_NG_COUNT, (long*)src.PAIR_NG_COUNT, sizeof(long) * 4);

	return *this;
}














CountWarehouse::CountWarehouse()
{
	nTotal = 0;
	nReject = 0;
	nGood = 0;
	nInvalid = 0;

	nRejectDimension = 0;	//Ä¡¼ö
	nRejectDefect = 0;	//roi
	
	nRejectDimension = 0;
	nRejectDefect = 0;
	nRejectTotal = 0;

	strLot = _T("NA");
	strRoll = _T("NA");
}

CountWarehouse::~CountWarehouse()
{
}



void CountWarehouse::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(nTotal);
	ar.Serialize_Element(nReject);
	ar.Serialize_Element(nGood);
	ar.Serialize_Element(nInvalid);
	ar.Serialize_Element(nRejectDimension);
	ar.Serialize_Element(nRejectDefect);

	ar.Serialize_Element(vecDefectCount);
}

CountWarehouse& CountWarehouse::operator = (const CountWarehouse& src)
{
	nTotal = src.nTotal;
	nReject = src.nReject;
	nGood = src.nGood;
	nInvalid = src.nInvalid;
	nRejectDimension = src.nRejectDimension;
	nRejectDefect = src.nRejectDefect;
	vecDefectCount = src.vecDefectCount;

	return *this;
}