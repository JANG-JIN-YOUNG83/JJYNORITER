#pragma once
#include "CandidateGarbageBase.h"

class CandidateGarbageFolder;

class CandidateGarbageFile : public CandidateGarbageBase
{
public:
	CandidateGarbageFile(LPCTSTR filePath, long expirationHours, BOOL onoff, BOOL always, const CTime& createdTime, CandidateGarbageFolder* parent);
	~CandidateGarbageFile();

	virtual bool Delete() override;

private:
	CandidateGarbageFolder* m_parent;
};

