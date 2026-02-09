#include "stdafx.h"
#include "ErrorProofingPara.h"

#include "../CommonShare/ArchiveAllType.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ErrorProofingPara::ErrorProofingPara()
	: m_proofingCellCount(0)
{
}

ErrorProofingPara::~ErrorProofingPara()
{
}

void ErrorProofingPara::CreateGUID()
{
	GUID guid;

	if (S_OK != ::CoCreateGuid(&guid))
	{
		m_guid = _T("Error");
		return;
	}

	TCHAR guidString[128];

	::StringFromGUID2(guid, guidString, sizeof(guidString) / sizeof(TCHAR));

	m_guid = guidString;
}

void ErrorProofingPara::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(m_guid);
	ar.Serialize_Element(m_proofingCellCount);
}
