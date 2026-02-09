#include "stdafx.h"
#include "CandidateCollector.h"
#include "CandidateGarbageFile.h"

#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CandidateCollector::CandidateCollector(long driveIndex)
	: m_driveIndex(driveIndex)
	, m_isCompleted(false)
{
}

CandidateCollector::~CandidateCollector()
{
}

long CandidateCollector::GetCollectedItemCount() const
{
	return (long)m_garbageCollect.size();
}

bool CandidateCollector::IsCompleted()
{
	return m_isCompleted;
}

void CandidateCollector::Request(const std::vector<MonitoringFolderInfo>& folders)
{
	m_garbageCollect.clear();

	m_folders.clear();
	m_workingFolders.clear();
	m_delegateFodlers.clear();

	for (auto& folderInfo : folders)
	{
		CString folderPath = GetRealPath(folderInfo.m_folderPath);
		long driveIndex = 0;

		if (true)
		{
			driveIndex = CStringA(folderPath.Left(1)).MakeUpper().GetAt(0) - 'A';
			if (driveIndex < 0 && driveIndex < 26)
			{
				continue;
			}
		}

		if (driveIndex != m_driveIndex)
		{
			// 내가 관심없는 드라이브의 폴더이다
			continue;
		}

		if (m_delegateFodlers.find(folderPath) != m_delegateFodlers.end())
		{
			// 이미 추가된 폴더이다
			continue;
		}

		auto folder = std::make_shared<CandidateGarbageFolder>(folderPath, folderInfo.m_expirationHours, folderInfo.m_OnOfff, folderInfo.m_Always);

		m_delegateFodlers.insert(folder->GetPath());
		m_folders.push_back(folder);

		RequestFolder add_info;
		add_info.m_parent = nullptr;
		add_info.m_folder = folder.get();

		m_workingFolders.push_back(add_info);
	}

	m_requestTime = CTime::GetCurrentTime();
	m_isCompleted = (m_workingFolders.size() == 0);
}

void CandidateCollector::Run()
{
	CandidateGarbageFolder* parent = nullptr;
	CandidateGarbageFolder* folder = nullptr;
	long expirationHours = 30 * 24;
	BOOL onoff(FALSE), always(FALSE);

	if (true)
	{
		if (m_workingFolders.size() == 0)
		{
			return;
		}

		auto info = m_workingFolders.front();
		m_workingFolders.pop_front();

		parent = info.m_parent;
		folder = info.m_folder;
		onoff = folder->GetOnOff();
		always = folder->GetAlways();
		expirationHours = folder->GetExpirationHours();
	}

	std::vector<CandidateGarbageFolder *> subFolders;
	CFileFind ff;

	CString findFilter;
	findFilter.Format(_T("%s\\*.*"), folder->GetPath());

	auto foundFile = ff.FindFile(findFilter);

	while (foundFile)
	{
		foundFile = ff.FindNextFile();
		if (ff.IsDots()) continue;

		CTime createdTime;
		ff.GetCreationTime(createdTime);

		CString path = ff.GetFilePath();

		if (ff.IsDirectory())
		{
			if (m_delegateFodlers.find(path) != m_delegateFodlers.end())
			{
				// 이 폴더는 모니터링 폴더이다.
				// 모니터링 폴더끼리 겹쳤으므로 더이상 탐색하지 말자.
				// 탐색하면 중복일 뿐 아니라 모니터링 폴더를 지워버리는 사태가 발생할 수 있다
				continue;
			}
			subFolders.push_back(new CandidateGarbageFolder(path, expirationHours, onoff, always, createdTime, folder));
		}
		else
		{
			auto* file = folder->AddFile(path, expirationHours, onoff, always, createdTime);
			if(onoff == TRUE)
				m_garbageCollect.insert(std::make_pair(file->GetCreatedTime() + CTimeSpan(0, file->GetExpirationHours(), 0, 0), file));
		}
	}

	for (auto subFolder : subFolders)
	{
		RequestFolder add_info;
		add_info.m_parent = folder;
		add_info.m_folder = subFolder;

		m_workingFolders.push_back(add_info);
	}

	if (subFolders.size() == 0)
	{
		// 하위 폴더가 없다 이 폴더가 해당 Tree의 마지막 원소이다
		folder->CollectFolderCompleted(m_garbageCollect);
	}

	if (m_workingFolders.size() == 0)
	{
		m_isCompleted = true;
	}
}

void CandidateCollector::Pop(CandidateGarbageDrive& drive) const
{
	drive.m_garbageObjs.clear();
	drive.m_garbageObjs_Always.clear();
	for (auto Garbage : m_garbageCollect)
	{
		if (Garbage.second->GetOnOff() )
		{
			if(Garbage.second->GetAlways())
			{
				drive.m_garbageObjs_Always.insert({ Garbage.first, Garbage.second });
			}
			else
			{
				drive.m_garbageObjs.insert({ Garbage.first, Garbage.second });

			}
		}

	}
	//drive.m_garbageObjs = m_garbageCollect;

#ifdef _AUTOCLEANER_DEBUG_
	if (m_garbageCollect[driveIndex].size() > 0)
	{
		CString text;

		for (auto& itInfo : m_garbageCollect)
		{
			text.AppendFormat(_T("%s\n"), itInfo.second->GetPath());
		}

		CString filePath;
		filePath.Format(_T("D:\\Drive%d.txt"), driveIndex);
		CFile file;
		if (file.Open(filePath, CFile::modeCreate | CFile::modeWrite))
		{
			file.Write(text.GetBuffer(), text.GetLength() * sizeof(TCHAR));
			file.Close();
		}
	}
#endif
}

CString CandidateCollector::GetRealPath(LPCTSTR path)
{
	CFileFind ff;
	if (!ff.FindFile(path)) return path;

	ff.FindNextFile();
	return ff.GetFilePath();
}
