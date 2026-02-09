#include "stdafx.h"
#include "ArchiveAllType.h"
#include "BaseObject.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace CommonShare;

ArchiveAllType::ArchiveAllType(CFile* pFile, UINT nMode)
	: CArchive(pFile, nMode)
{
}

ArchiveAllType::~ArchiveAllType()
{
}
\
void ArchiveAllType::Serialize_Element(CommonShare::BaseObject& data)
{
	data.Serialize(*this);
}

void ArchiveAllType::Serialize_Element(CStringW& istr)
{
	if (IsStoring())
	{
		(*this) << istr;
	}
	else
	{
		(*this) >> istr;
	}
}

void ArchiveAllType::Serialize_Element(SYSTEMTIME& stime)
{
	BYTE* pData = new BYTE[sizeof(SYSTEMTIME)];
	ULONG nDataNum = sizeof(SYSTEMTIME);

	if (IsStoring())
	{
		memcpy(pData, (BYTE*)&stime, nDataNum);
		Serialize_RawData(pData, nDataNum);
	}
	else
	{
		Serialize_RawData(pData, nDataNum);
		memcpy((BYTE*)&stime, pData, nDataNum);
	}

	delete[]pData;
}

template <class T> void Serialize_Base(ArchiveAllType& ar, T& t)
{
	if (ar.IsStoring())
		ar << t;
	else
		ar >> t;
};

void ArchiveAllType::Serialize_Element(bool& data) { Serialize_Base(*this, data); }
void ArchiveAllType::Serialize_Element(double& data) { Serialize_Base(*this, data); }
void ArchiveAllType::Serialize_Element(float& data) { Serialize_Base(*this, data); }
void ArchiveAllType::Serialize_Element(int& data) { Serialize_Base(*this, data); }
void ArchiveAllType::Serialize_Element(long& data) { Serialize_Base(*this, data); }
void ArchiveAllType::Serialize_Element(int64_t& data) { Serialize_Base(*this, data); }

void ArchiveAllType::Serialize_Binary(BYTEPTR& pData, ULONG& nDataNum)
{
	if (IsStoring())
	{
		(*this) << nDataNum;
		Write(pData, nDataNum);
	}
	else
	{
		(*this) >> nDataNum;

		delete[]pData;
		pData = new BYTE[nDataNum];
		Read(pData, nDataNum);
	}
}

void ArchiveAllType::Serialize_RawData(BYTE* pData, ULONG nDataNum)
{
	if (IsStoring())
	{
		Write(pData, nDataNum);
	}
	else
	{
		Read(pData, nDataNum);
	}
}