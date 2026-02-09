#include "pch.h"
#include "ProgramBackup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Utility;

ProgramBackup::ProgramBackup()
{
}

ProgramBackup::~ProgramBackup()
{
}

bool ProgramBackup::Set(LPCTSTR sourceFolder, LPCTSTR targetFolder)
{
	m_sourceFolder = sourceFolder;
	m_targetFolder = targetFolder;

	return AddSubFolder(_T(""), false);
}

bool ProgramBackup::AddSubFolder(LPCTSTR relativePath, bool appendSubFolders)
{
	CString sourceFolder = m_sourceFolder + _T("\\") + relativePath;
	CString targetFolder = m_targetFolder + _T("\\") + relativePath;

	if (!CreateDirectory(targetFolder, NULL))
	{
		// 이미 Target Folder가 존재함
		return false;
	}

	if (!CopyFiles(sourceFolder, targetFolder))
	{
		return false;
	}

	if (!appendSubFolders)
	{
		// Sub Folder는 추가하지 않는 옵션이므로 여기까지 수행하고 끝낸다
		return true;
	}

	CFileFind ff;

	auto foundFile = ff.FindFile(sourceFolder + _T("\\*.*"));

	while (foundFile)
	{
		foundFile = ff.FindNextFile();
		if (ff.IsDots()) continue;
		if (ff.IsDirectory())
		{
			return AddSubFolder(CString(relativePath) + _T("\\") + ff.GetFileName(), true);
		}
	}

	return true;
}

bool ProgramBackup::IsSourceSubFolderExit(LPCTSTR relativePath) const
{
	CString sourceFolder = m_sourceFolder + _T("\\") + relativePath;

	CFileFind ff;
	return ff.FindFile(sourceFolder);
}

bool ProgramBackup::Cancel()
{
	if (m_targetFolder.IsEmpty())
	{
		return true;
	}

	return DeleteFolder(m_targetFolder);
}

bool ProgramBackup::CopyFiles(LPCTSTR sourceFolder, LPCTSTR targetFolder)
{
	CFileFind ff;

	auto foundFile = ff.FindFile(CString(sourceFolder) + _T("\\*.*"));

	CString targetRoot = CString(targetFolder) + _T("\\");

	while (foundFile)
	{
		foundFile = ff.FindNextFile();
		if (ff.IsDirectory()) continue;
		if(ff.GetFileName().Right(4) == _T(".zip")) continue; //20221207 [jjh]

		if (!CopyFile(ff.GetFilePath(), targetRoot + ff.GetFileName(), FALSE))
		{
			// 파일 복사 실패
			return false;
		}
	}

	return true;
}

bool ProgramBackup::DeleteFolder(LPCTSTR folderPath)
{
	CFileFind ff;

	auto foundFile = ff.FindFile(CString(folderPath) + _T("\\*.*"));

	CString targetRoot = CString(folderPath) + _T("\\");

	while (foundFile)
	{
		foundFile = ff.FindNextFile();
		if (ff.IsDots()) continue;
		if (ff.IsDirectory())
		{
			DeleteFolder(ff.GetFilePath());
			continue;
		}

		DeleteFile(ff.GetFilePath());
	}

	return RemoveDirectory(folderPath) ? true : false;
}
