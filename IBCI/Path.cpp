#include "pch.h"
#include "Path.h"
#include "../Module/Utility/IBNI_PCSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class IBNIFolder
{
public:
	IBNIFolder()
	{
		if (Utility::IBNI_PCSetting::IsEnabledSubOption_UseRelativePathForCIS())
		{
			TCHAR szTemp[300];
			::GetModuleFileName(NULL, szTemp, 300);
			CString path = szTemp;

			if (0 < path.ReverseFind('\\')) path = path.Left(path.ReverseFind('\\'));
			if (0 < path.ReverseFind('\\')) path = path.Left(path.ReverseFind('\\'));

			SetRoot(path);
		}
		else
		{
			SetRoot(_T("C:\\intekplus\\IBCI"));
		}
	}

	void SetRoot(LPCTSTR rootPath)
	{
		m_folderRoot = rootPath;
		m_folderCrashDump = m_folderRoot + _T("\\Crash");
		m_folderRecipe = m_folderRoot + _T("\\Recipe");
		m_folderRecipeBackup = m_folderRecipe + _T("\\Backup");
        m_folderBackupUI = m_folderRoot + _T("\\backup_UI");
	}

public:
	CString m_folderRoot;
	CString m_folderCrashDump;
	CString m_folderRecipe;
	CString m_folderRecipeBackup;
    CString m_folderBackupUI;
};

IBNIFolder g_ibniFolder;
#define IBNI_BACKUP_FOLDER			_T("D:\\BACKUP")

static CString g_imageFolder;
static CString g_LogFolder;

LPCTSTR Path::GetRootFolder()
{
	return g_ibniFolder.m_folderRoot;
}

LPCTSTR Path::GetBackupFolder()
{
	return IBNI_BACKUP_FOLDER;
}

LPCTSTR Path::GetCrashDumpFolder()
{
	return g_ibniFolder.m_folderCrashDump;
}

LPCTSTR Path::GetRecipeFolder()
{
	return g_ibniFolder.m_folderRecipe;
}

LPCTSTR Path::GetRecipeBackupFolder()
{
	return g_ibniFolder.m_folderRecipeBackup;
}

LPCTSTR Path::GetBackupUIFolder()
{
    return g_ibniFolder.m_folderBackupUI;
}

LPCTSTR Path::GetImageFolder()
{
	return g_imageFolder;
}

LPCTSTR Path::GetlogFolder()
{
	return g_LogFolder;
}

void Path::SetImageFolder(LPCTSTR path)
{
	g_imageFolder = path;
}

void Path::SetRootFolder(LPCTSTR path)
{
	g_ibniFolder.m_folderRoot = path;
}

void Path::SetLogFolder(LPCTSTR path)
{
	g_LogFolder = path;
}