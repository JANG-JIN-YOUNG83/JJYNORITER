#include "pch.h"
#include "CSVSaveThread.h"
#include <locale.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSVSaveThread::CSVSaveThread()
{
	__super::BeginWorker();
}

CSVSaveThread::~CSVSaveThread()
{
	__super::EndWorker();
}

void CSVSaveThread::Push(LPCTSTR filePath, LPCTSTR title, LPCTSTR rowText, bool isJustOneTitle)
{
	auto info = std::make_shared<FileInfo>();
	info->m_filePath = filePath;
	info->m_title = title;
	info->m_text = rowText;
	info->m_isJustOneTitle = isJustOneTitle;

	m_cs.Lock();
	m_infos.push_back(info);
	m_cs.Unlock();

	__super::Trigger();
}

bool CSVSaveThread::OnTrigger()
{
	m_cs.Lock();
	if (m_infos.size() == 0)
	{
		m_cs.Unlock();
		return true;
	}
	
	auto info = m_infos.front();
	//m_infos.pop_front();
    m_infos.erase(m_infos.begin());
	m_cs.Unlock();

	FileSave(info->m_filePath, info->m_title, info->m_isJustOneTitle, info->m_text);

	m_cs.Lock();
	if (m_infos.size())
	{
		__super::Trigger();
	}
	m_cs.Unlock();

	return true;
}

BOOL CSVSaveThread::FileSave(LPCTSTR filePath, LPCTSTR title, bool isJustOneTitle, LPCTSTR rowText)
{
	// 파일 저장.
	CStdioFile file;
	CFileFind find;
	CFileException ex;
	UINT nCheck = CFile::modeWrite;

	if (FALSE == find.FindFile(filePath))
        nCheck |= CFile::modeCreate;
    //else
    //{
    //    if (DeleteFile(filePath))
    //        nCheck |= CFile::modeCreate;
    //    else
    //        return FALSE;
    //}

	if (!file.Open(filePath, nCheck, &ex))
	{
		ex.ReportError();
		return FALSE;
	}

	setlocale(LC_ALL, "korean");
	file.SeekToEnd();

	if (!isJustOneTitle || file.GetPosition() == 0)
	{
		file.WriteString(title);
		file.WriteString(_T("\n"));
	}

	file.WriteString(rowText);
	file.WriteString(_T("\n"));
	file.Close();

	return TRUE;
}
