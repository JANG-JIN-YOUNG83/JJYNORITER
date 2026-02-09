#include "stdafx.h"
#include "V2I_VisionRecipe.h"
#include "../CommonShare/ArchiveAllType.h"

using namespace TCPMSG::V2I;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

VisionRecipe::VisionRecipe()
{
}

VisionRecipe::~VisionRecipe()
{
}

void VisionRecipe::Serialize(CommonShare::ArchiveAllType& ar)
{
	ar.Serialize_Element(m_recipeName);

	long bufferSize = 0;

	if (ar.IsStoring())
	{
		void* buffer = nullptr;
		m_db.GetBuffer(&buffer, &bufferSize);

		ar.Serialize_Element(bufferSize);
		ar.Serialize_RawData((BYTE *)buffer, bufferSize);

		m_db.ReleaseBuffer(buffer);
	}
	else
	{
		ar.Serialize_Element(bufferSize);
		BYTE* buffer = new BYTE[bufferSize];
		ar.Serialize_RawData(buffer, bufferSize);
		m_db.Load(buffer, bufferSize);
		delete[]buffer;
	}
}
