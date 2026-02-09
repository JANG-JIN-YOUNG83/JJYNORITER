#include "stdafx.h"
#include "CandidateGarbageDrive.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CandidateGarbageDrive::CandidateGarbageDrive(long driveIndex)
	: m_driveIndex(driveIndex)
	, m_specFreeRate(-1.f)
	, m_currentFreeRate(0.f)
{
}

CandidateGarbageDrive::~CandidateGarbageDrive()
{
}

float CandidateGarbageDrive::GetFreeRate(long driveIndex)
{
	char drive = (char)('A' + driveIndex);
	// HDD 용량 체크
	LONGLONG lFreeBytesAvailable, lTotalBytes, lTotalFree;

	int nPos = 0;
	CStringA strToken;
	strToken.Format("%c:\\", drive);

	CString strDriver(strToken);

	if (0 == GetDiskFreeSpaceEx(strDriver, (PULARGE_INTEGER)&lFreeBytesAvailable, (PULARGE_INTEGER)&lTotalBytes, (PULARGE_INTEGER)&lTotalFree))
	{
		return 100.f;
	}

	if (lTotalBytes <= 0) return 100.f;

	return ((float)lTotalFree / lTotalBytes) * 100.f; // 단위 : %
}

bool CandidateGarbageDrive::IsCapacityNeeded() const
{
	if (m_specFreeRate < 0.f) return false;

	if (m_specFreeRate > m_currentFreeRate)
	{
		return true;
	}

	return false;
}

bool CandidateGarbageDrive::CollectInfo()
{
	char drive = (char)('A' + m_driveIndex);
	// HDD 용량 체크
	LONGLONG lFreeBytesAvailable, lTotalBytes, lTotalFree;

	int nPos = 0;
	CStringA strToken;
	strToken.Format("%c:\\", drive);

	CString strDriver(strToken);

	// return "0"이면 실패
	if (0 == GetDiskFreeSpaceEx(strDriver, (PULARGE_INTEGER)&lFreeBytesAvailable, (PULARGE_INTEGER)&lTotalBytes, (PULARGE_INTEGER)&lTotalFree))
	{
		return false;
	}

	if (lTotalBytes <= 0) return false;
	m_currentFreeRate = ((float)lTotalFree / lTotalBytes) * 100.f; // 단위 : %

	return true;
}

void CandidateGarbageDrive::RemoveConditionsForDeletion()
{
	m_specFreeRate = -1.f;
}

void CandidateGarbageDrive::SetDriveFreeRate(float freeRate)
{
	m_specFreeRate = freeRate;
}
