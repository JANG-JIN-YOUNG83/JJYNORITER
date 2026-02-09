#pragma once
#include "CandidateGarbageBase.h"
#include <map>

class CandidateGarbageDrive
{
public:
	CandidateGarbageDrive(long driveIndex);
	~CandidateGarbageDrive();

	static float GetFreeRate(long driveIndex);

	bool IsCapacityNeeded() const;

	bool CollectInfo();
	void RemoveConditionsForDeletion();
	void SetDriveFreeRate(float freeRate);

	std::multimap<CTime, CandidateGarbageBase*> m_garbageObjs;
	std::multimap<CTime, CandidateGarbageBase*> m_garbageObjs_Always;
	// 용량이랑 상관없는거는 따로 놓는게.. 아닌가?
private:
	const long m_driveIndex;
	float m_specFreeRate;
	float m_currentFreeRate;
};
