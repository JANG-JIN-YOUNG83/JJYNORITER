#include "stdafx.h"
#include "CandidateGarbageBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CandidateGarbageBase::CandidateGarbageBase(LPCTSTR path, long expirationHours, BOOL onoff, BOOL always, const CTime& createdTime)
	: m_path(path)
	, m_expirationHours(expirationHours)
	, m_createdTime(createdTime)
	, m_bAlways(always)
	, m_bOnOff(onoff)
{
}

CandidateGarbageBase::~CandidateGarbageBase()
{
}

const CTime& CandidateGarbageBase::GetCreatedTime() const
{
	return m_createdTime;
}

const long CandidateGarbageBase::GetExpirationHours() const
{
	return m_expirationHours;
}

LPCTSTR CandidateGarbageBase::GetPath() const
{
	return m_path;
}

const BOOL CandidateGarbageBase::GetOnOff() const
{
	return m_bOnOff;
}
const BOOL CandidateGarbageBase::GetAlways() const
{
	return m_bAlways;
}