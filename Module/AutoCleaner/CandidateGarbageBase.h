#pragma once

class CandidateGarbageBase
{
public:
	CandidateGarbageBase(LPCTSTR path, long expirationHours, BOOL onoff, BOOL always, const CTime& createdTime);
	~CandidateGarbageBase();

	const CTime& GetCreatedTime() const;
	const long GetExpirationHours() const;
	const BOOL GetOnOff() const;
	const BOOL GetAlways() const;
	LPCTSTR GetPath() const;

	virtual bool Delete() = 0;

protected:
	CTime	m_createdTime;		// 생성된 시간
	long	m_expirationHours;
	CString	m_path;
	BOOL	m_bAlways;
	BOOL	m_bOnOff;
};
