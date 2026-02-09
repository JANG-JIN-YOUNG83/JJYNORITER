#pragma once

class Path
{
public:
	static LPCTSTR GetRootFolder();
	static LPCTSTR GetBackupFolder();
	static LPCTSTR GetCrashDumpFolder();
	static LPCTSTR GetRecipeFolder();
	static LPCTSTR GetRecipeBackupFolder();
	static LPCTSTR GetBackupUIFolder();
	static LPCTSTR GetImageFolder();
	static LPCTSTR GetlogFolder();
	static void SetImageFolder(LPCTSTR path);
	static void SetRootFolder(LPCTSTR path);
	static void SetLogFolder(LPCTSTR path);
};

