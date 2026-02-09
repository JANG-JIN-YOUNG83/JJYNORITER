#pragma once
#include <map>
#include <memory>
#include <set>

#include "CandidateGarbageBase.h"

class CandidateGarbageFile;

class CandidateGarbageFolder : public CandidateGarbageBase
{
public:
	CandidateGarbageFolder(LPCTSTR folderPath, long expirationHours,BOOL onoff, BOOL always );
	CandidateGarbageFolder(LPCTSTR folderPath, long expirationHours, BOOL onoff, BOOL always ,const CTime& createdTime, CandidateGarbageFolder* parent);
	~CandidateGarbageFolder();

	CandidateGarbageFile* AddFile(LPCTSTR filePath, long expirationHours, BOOL onoff, BOOL always, const CTime& createdTime);
	bool DeleteSubFile(CandidateGarbageFile* file);
	bool DeleteSubFolder(CandidateGarbageFolder* folder);

	void CollectFolderCompleted(std::multimap<CTime, CandidateGarbageBase*>& garbageCollect);

	virtual bool Delete() override;

	CandidateGarbageFolder* GetParent();

private:
	void AddCollectingSubFolder(CandidateGarbageFolder* folder);

	CandidateGarbageFolder* m_parent;

	std::set<CandidateGarbageFolder*> m_subFolders;
	std::set<CandidateGarbageFile*> m_subFiles;

	bool m_collectCompleted;
};
