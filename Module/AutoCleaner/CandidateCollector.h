#pragma once

#include "CandidateGarbageFolder.h"
#include "CandidateGarbageDrive.h"
#include "MonitoringFolderInfo.h"

#include <deque>
#include <map>
#include <memory>
#include <set>
#include <vector>

class CandidateGarbageFile;

class CandidateCollector
{
public:
	CandidateCollector(long driveIndex);
	~CandidateCollector();

	long GetCollectedItemCount() const;
	bool IsCompleted();
	void Request(const std::vector<MonitoringFolderInfo>& folders);
	void Run();

	void Pop(CandidateGarbageDrive& drive) const;

private:
	const long m_driveIndex;

	struct RequestFolder
	{
		CandidateGarbageFolder* m_parent;
		CandidateGarbageFolder* m_folder;
	};

	CTime m_requestTime;
	std::deque<RequestFolder> m_workingFolders;
	std::vector<std::shared_ptr<CandidateGarbageFolder>> m_folders;
	std::set<CString> m_delegateFodlers;

	std::multimap<CTime, CandidateGarbageBase*> m_garbageCollect;

	bool m_isCompleted;

	CString GetRealPath(LPCTSTR path);
};
