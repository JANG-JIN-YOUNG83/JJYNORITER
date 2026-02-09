#include "stdafx.h"
#include "CandidateGarbageFile.h"
#include "CandidateGarbageFolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CandidateGarbageFile::CandidateGarbageFile(LPCTSTR filePath, long expirationHours, BOOL onoff, BOOL always, const CTime& createdTime, CandidateGarbageFolder* parent)
	: CandidateGarbageBase(filePath, expirationHours, onoff,always, createdTime)
	, m_parent(parent)
{
}

CandidateGarbageFile::~CandidateGarbageFile()
{
}

bool CandidateGarbageFile::Delete()
{
	bool retValue = true;
#ifndef _VIRTUAL_DELETE_
	retValue = ::DeleteFile(m_path) ? true : false;
#endif

	m_parent->DeleteSubFile(this);

	return retValue;
}
