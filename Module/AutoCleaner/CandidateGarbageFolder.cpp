#include "stdafx.h"
#include "CandidateGarbageFolder.h"
#include "CandidateGarbageFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CandidateGarbageFolder::CandidateGarbageFolder(LPCTSTR folderPath, long expirationHours, BOOL onoff, BOOL always)
	: CandidateGarbageBase(folderPath, expirationHours,onoff,always ,CTime(CTime::GetCurrentTime()))
	, m_parent(nullptr)
	, m_collectCompleted(false)
{
}

CandidateGarbageFolder::CandidateGarbageFolder(LPCTSTR folderPath, long expirationHours, BOOL onoff, BOOL always, const CTime& createdTime, CandidateGarbageFolder* parent)
	: CandidateGarbageBase(folderPath, expirationHours, onoff ,always , createdTime)
	, m_parent(parent)
	, m_collectCompleted(false)
{
	m_parent->AddCollectingSubFolder(this);
}

CandidateGarbageFolder::~CandidateGarbageFolder()
{
	for (auto* subFolder : m_subFolders)
	{
		delete subFolder;
	}

	for (auto* subFile : m_subFiles)
	{
		delete subFile;
	}
}

void CandidateGarbageFolder::AddCollectingSubFolder(CandidateGarbageFolder* folder)
{
	m_subFolders.insert(folder);
}

CandidateGarbageFile* CandidateGarbageFolder::AddFile(LPCTSTR filePath, long expirationHours, BOOL onoff, BOOL always, const CTime& createdTime)
{
	auto* file = new CandidateGarbageFile(filePath, expirationHours, onoff, always, createdTime, this);
	m_subFiles.insert(file);

	return file;
}

bool CandidateGarbageFolder::DeleteSubFile(CandidateGarbageFile* file)
{
	auto itFile = m_subFiles.find(file);
	if (itFile == m_subFiles.end())
	{
		ASSERT(!_T("이게 모여? 왜 목록에 없어?"));
		return false;
	}

	delete file;

	m_subFiles.erase(itFile);

	return true;
}

bool CandidateGarbageFolder::DeleteSubFolder(CandidateGarbageFolder* folder)
{
	auto itFolder = m_subFolders.find(folder);
	if (itFolder == m_subFolders.end())
	{
		ASSERT(!_T("이게 모여? 왜 목록에 없어?"));
		return false;
	}

	if (folder->m_subFiles.size() != 0 || folder->m_subFolders.size() != 0)
	{
		// Folder를 제거할때 이미 File과 Sub Folder는 이미 제거를 시도했었어야 순서에 맞다

		ASSERT(folder->m_subFiles.size() == 0);
		ASSERT(folder->m_subFolders.size() == 0);

		return false;
	}

	delete folder;

	m_subFolders.erase(itFolder);

	return true;
}

void CandidateGarbageFolder::CollectFolderCompleted(std::multimap<CTime, CandidateGarbageBase*>& garbageCollect)
{
	if (m_parent == nullptr) return;

	if (m_collectCompleted)
	{
		ASSERT(!_T("이미 수집을 마쳤는데 왜 또 들어왔나?"));
		return;
	}

	// 해당 Folder의 수집을 마쳤다
	if (true)
	{
		// 부모 Folder는 File이나 Sub Folder보다 제일 늦게 삭제 되어야 한다
		// 시간적으로 그 뒤에 있는것처럼 만들어 주자

		for (auto& subFile : m_subFiles)
		{
			m_createdTime = max(m_createdTime, subFile->GetCreatedTime() + 1);
		}

		for (auto& subFolder : m_subFolders)
		{
			m_createdTime = max(m_createdTime, subFolder->GetCreatedTime() + 1);
		}
	}
	if(GetOnOff() == TRUE)
		garbageCollect.insert(std::make_pair(GetCreatedTime() + CTimeSpan(0, GetExpirationHours(), 0, 0), this));
	m_collectCompleted = true;

	for (auto* subFolder : m_parent->m_subFolders)
	{
		if (!subFolder->m_collectCompleted) return;
	}

	// 부모도 모든 수집을 마쳤다
	m_parent->CollectFolderCompleted(garbageCollect);
}

bool CandidateGarbageFolder::Delete()
{
	bool retValue = true;

	if (m_parent != nullptr)
	{
		// 최상위 폴더는 제거하지 않는다
#ifndef _VIRTUAL_DELETE_
		retValue = ::RemoveDirectory(m_path) ? true : false;
#endif

		m_parent->DeleteSubFolder(this);
	}

	return retValue;
}

CandidateGarbageFolder* CandidateGarbageFolder::GetParent()
{
	return m_parent;
}
