#pragma once
#include "basedef.h"
#include <vector>

namespace Utility
{
	class __UTILITY_CLASS__ RecipeSaver
	{
	public:
		static void SetBackupOption(LPCTSTR backupFolder, bool enable);
		static bool Do(Database& data, LPCTSTR filePath);
		static bool Do(IniPackage& data, LPCTSTR filePath);

	private:
		class BackupInfo
		{
		public:
			BackupInfo();
			~BackupInfo();

			bool m_isFirstBackup;
			bool m_makeSuccess;
			bool m_isExist;

			std::vector<std::pair<CString, CString>> m_backupFiles;	// Source, Backup
		};

		static bool IsExistFirstBackup(LPCTSTR masterFilePath);
		static void Backup(LPCTSTR masterFilePath, BackupInfo& io_backupInfo);
		static void DoPost(LPCTSTR filePath, BackupInfo& io_backupInfo);
	};
}
