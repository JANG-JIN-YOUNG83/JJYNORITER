#include "stdafx.h"
#include "ResultMarkingIO.h"
#include "SpecData.h"

#include "../CommonShare/ArchiveAllType.h"
#include "../CommonShare/DefectDef.h"

using namespace Share;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

ResultMarkingIO::ResultMarkingIO()
	: m_resultType(MarkingResultType::Pass)
	, m_markCount(0)
	, m_rank(9999)
	, m_masterDefectType(DefectType::END)
{
}

ResultMarkingIO::~ResultMarkingIO()
{
}

void ResultMarkingIO::Reset()
{
	m_resultType = MarkingResultType::Pass;
	m_markCount = 0;
	m_rank = 9999;
	m_masterDefectType = DefectType::END;
}

void ResultMarkingIO::Apply(const Share::SpecData& specItem)
{
	//Apply(specItem.m_inspType, specItem.m_nDefectSection, specItem._NGMarkingCount, specItem._Rank);
}

void ResultMarkingIO::Apply(DefectType defectType, int section, int markingCount, int rank)
{
	if (section != ensection::NOTCHING && section != ensection::ELECTRODE) return;
	if (markingCount == 0) return;	// Marking 을 하는 검사항목이 아니다
	if (rank > m_rank) return;		// 우선순위가 기존것보다 낮다

	m_rank = rank;

	m_markCount = markingCount;
	m_masterDefectType = defectType;

	switch (section)
	{
	case ensection::NOTCHING:	m_resultType = Share::MarkingResultType::Notching;	break;
	case ensection::ELECTRODE:	m_resultType = Share::MarkingResultType::Electrode;	break;
	}
}

void ResultMarkingIO::Serialize(CommonShare::ArchiveAllType& ar)
{
	CString inspName;

	if (ar.IsStoring())
	{
		if (m_masterDefectType != DefectType::END)
		{
			inspName = GetDefectName(m_masterDefectType);
		}
	}

	ar.Serialize_Element((long &)m_resultType);
	ar.Serialize_Element(m_markCount);
	ar.Serialize_Element(m_rank);
	ar.Serialize_Element(inspName);

	if (ar.IsLoading())
	{
		m_masterDefectType = GetDefectType(inspName);
	}
}
