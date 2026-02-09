#include "pch.h"
#include "RecipeSaver.h"
#include "FileReader.h"
#include "IniPackage.h"

#include "../../CompanySharedLibrary/Persistence/Database.h"
#include "Gadget/Miscellaneous.h"
#include <afxmt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace Utility;

CCriticalSection m_csRecipeSave;
CString m_recipeBackupFolder;
bool m_recipeBackup = false;

RecipeSaver::BackupInfo::BackupInfo()
	: m_makeSuccess(false)
	, m_isFirstBackup(false)
	, m_isExist(false)
{
}

RecipeSaver::BackupInfo::~BackupInfo()
{
}

void RecipeSaver::SetBackupOption(LPCTSTR backupFolder, bool enable)
{
	m_recipeBackupFolder = backupFolder;
	m_recipeBackupFolder.Replace(_T("/"), _T("\\"));

	if (m_recipeBackupFolder.GetLength() > 0)
	{
		if (m_recipeBackupFolder[m_recipeBackupFolder.GetLength() - 1] != _T('\\'))
		{
			m_recipeBackupFolder.Append(_T("\\"));
		}
	}

	m_recipeBackup = enable;
}

bool RecipeSaver::Do(Database& data, LPCTSTR filePath)
{
	CSingleLock lock(&m_csRecipeSave, TRUE);
	BackupInfo backupInfo;

	if (!PathFileExists(filePath))
	{
		// 처음 만들어진 파일이다
		// 백업할 것이 없다
		if (!data.Save(filePath)) return false;

		Backup(filePath, backupInfo);
		return true;
	}

	Backup(filePath, backupInfo);
	bool success = data.Save(filePath);

	DoPost(filePath, backupInfo);

	return success;
}

bool RecipeSaver::Do(IniPackage& data, LPCTSTR filePath)
{
	CSingleLock lock(&m_csRecipeSave, TRUE);
	BackupInfo backupInfo;

	if (!PathFileExists(filePath))
	{
		// 처음 만들어진 파일이다
		// 백업할 것이 없다
		if (!data.Save(filePath)) return false;

		Backup(filePath, backupInfo);
		return true;
	}

	Backup(filePath, backupInfo);
	bool success = data.Save(filePath);

	DoPost(filePath, backupInfo);

	return success;
}

bool RecipeSaver::IsExistFirstBackup(LPCTSTR masterFilePath)
{
	CString filePath = CString(masterFilePath);	// 경로\\파일명.확장자
	filePath.Replace(_T("/"), _T("\\"));

	long ext = filePath.ReverseFind(_T('.'));
	if (ext < 0) return false;

	filePath = filePath.Left(ext);				// 경로\\파일명

	long pathEnd = filePath.ReverseFind(_T('\\'));
	if (pathEnd < 0) return false;

	CString fileTitle = filePath.Mid(pathEnd + 1);

	CFileFind ff;
	return ff.FindFile(m_recipeBackupFolder + fileTitle + _T(".*")) ? false : true;
}

void RecipeSaver::Backup(LPCTSTR masterFilePath, BackupInfo& io_backupInfo)
{
	if (!m_recipeBackup) return;

	IPVM::CreateDirectories(m_recipeBackupFolder);

	// 파일 이름이 같은 모든 파일 찾기
	CString fileFilter = CString(masterFilePath);	// 경로\\파일명.확장자
	fileFilter.Replace(_T("/"), _T("\\"));

	long ext = fileFilter.ReverseFind(_T('.'));
	if (ext < 0) return;

	fileFilter = fileFilter.Left(ext);				// 경로\\파일명
	fileFilter = fileFilter.Left(ext) + _T(".*");	// 경로\\파일명.*

	CFileFind ff;
	auto foundFile = ff.FindFile(fileFilter);

	SYSTEMTIME tm;
	GetLocalTime(&tm);

	io_backupInfo.m_isFirstBackup = IsExistFirstBackup(masterFilePath);

	while (foundFile)
	{
		foundFile = ff.FindNextFile();
		if (ff.IsDirectory()) continue;

		CString originalFilePath = ff.GetFilePath();
		CString fileName = PathFindFileName(originalFilePath);
		
		CString tempFilePath;

		if (!io_backupInfo.m_isFirstBackup)
		{
			tempFilePath.Format(_T("%s%04d%02d%02d_%02d%02d%02d_%s"),
				(LPCTSTR)m_recipeBackupFolder,
				tm.wYear,
				tm.wMonth,
				tm.wDay,
				tm.wHour,
				tm.wMinute,
				tm.wSecond,
				(LPCTSTR)fileName);
		}
		else
		{
			tempFilePath = m_recipeBackupFolder + fileName;
		}

		io_backupInfo.m_isExist |= (PathFileExists(tempFilePath) ? true : false);

		if (!CopyFile(originalFilePath, tempFilePath, FALSE)) return;

		io_backupInfo.m_backupFiles.emplace_back(originalFilePath, tempFilePath);
	}

	io_backupInfo.m_makeSuccess = true;
}

void RecipeSaver::DoPost(LPCTSTR filePath, BackupInfo& io_backupInfo)
{
	if (!io_backupInfo.m_makeSuccess) return;
	if (io_backupInfo.m_isExist) return;		// 동시에 파일 요청이 되어 같은명의 파일이 존재했다. 그냥 덮고 끝내자
	if (io_backupInfo.m_isFirstBackup) return;	// 첫번째 백업은 무조건 남겨둔다

	bool isChanged = false;

	for (auto& backup : io_backupInfo.m_backupFiles)
	{
		std::vector<BYTE> srcBytes;
		std::vector<BYTE> dstBytes;

		if (!FileReader::Load(backup.first, srcBytes) ||
			!FileReader::Load(backup.second, dstBytes))
		{
			// 파일을 못읽은 것이긴 하지만 만일을 위해 그냥 백업하자
			isChanged = true;
			break;
		}

		if (srcBytes != dstBytes)
		{
			// 파일이 달라졌다.
			isChanged = true;
			break;
		}
	}

	if (!isChanged)
	{
		// 파일이 바뀐게 없다. 백업했던 파일을 제거하자
		for (auto& backup : io_backupInfo.m_backupFiles)
		{
			DeleteFile(backup.second);
		}
	}
}
