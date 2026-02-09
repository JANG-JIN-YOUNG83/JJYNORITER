#pragma once
#include "basedef.h"

namespace Utility
{
	class __UTILITY_CLASS__ ProgramBackup
	{
	public:
		ProgramBackup();
		~ProgramBackup();

		bool Set(LPCTSTR sourceFolder, LPCTSTR targetFolder);
		bool AddSubFolder(LPCTSTR relativePath, bool appendSubFolders);
		bool IsSourceSubFolderExit(LPCTSTR relativePath) const;
		bool Cancel();

	private:
		CString m_sourceFolder;
		CString m_targetFolder;

		bool CopyFiles(LPCTSTR sourceFolder, LPCTSTR targetFolder);
		bool DeleteFolder(LPCTSTR folderPath);
	};
}
