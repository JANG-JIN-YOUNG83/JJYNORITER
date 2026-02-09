#pragma once
#include "Gadget/SimpleWorkerThread.h"
#include <afxmt.h>
#include <deque>
#include <memory>

class CSVSaveThread : public IPVM::SimpleWorkerThread
{
public:
	CSVSaveThread();
	~CSVSaveThread();

	void Push(LPCTSTR filePath, LPCTSTR title, LPCTSTR rowText, bool isJustOneTitle);

private:
	virtual bool OnTrigger() override;

	struct FileInfo
	{
		CString m_filePath;
		CString m_title;
		CString m_text;
		bool m_isJustOneTitle;
	};

	CCriticalSection m_cs;
	//std::deque<std::shared_ptr<FileInfo>> m_infos;
    std::vector<std::shared_ptr<FileInfo>> m_infos;
	
	BOOL FileSave(LPCTSTR filePath, LPCTSTR title, bool isJustOneTitle, LPCTSTR rowText);
};

