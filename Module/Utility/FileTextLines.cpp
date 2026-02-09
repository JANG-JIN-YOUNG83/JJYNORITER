#include "pch.h"
#include "FileTextLines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace Utility;

FileTextLines::FileTextLines()
{
}

FileTextLines::~FileTextLines()
{
}

long FileTextLines::GetCount() const
{
	return (long)m_lineTexts.size();
}

LPCTSTR FileTextLines::GetAt(long index) const
{
	return m_lineTexts[index];
}

void FileTextLines::AddLine(LPCTSTR text)
{
	m_lineTexts.push_back(text);
}

void FileTextLines::SubLines(long startIndex, long count, FileTextLines& o_result)
{
	o_result.m_lineTexts.clear();
	for (long index = 0; index < count; index++)
	{
		o_result.m_lineTexts.push_back(m_lineTexts[startIndex + index]);
	}
}

bool FileTextLines::Load(LPCTSTR filePath)
{
	CStdioFile file;
	if (!file.Open(filePath, CStdioFile::modeRead)) return false;

	m_lineTexts.clear();

	long fileLength = (long)file.GetLength();

	CString fullText;
	bool foundCharectorCode = false;

	if (fileLength >= 2)
	{
		USHORT mode = 0;//Unicode BOM(Byte Order Mark).
		file.Read(&mode, 2);

		if (mode == 0xfeff)
		{
			// UNICODE
			wchar_t* text = new wchar_t[fileLength / 2];
			file.Read(text, (UINT)(fileLength - 2));
			text[fileLength / 2 - 1] = 0;
			fullText = text;
			delete[]text;

			foundCharectorCode = true;
		}
		file.SeekToBegin();
	}

	if (fileLength > 3)
	{
		BYTE mode[3] = { 0 };
		file.Read(mode, 3);

		if ((mode[0] == 0xef) && (mode[1] = 0xbb) && (mode[2] = 0xbf))
		{
			// UTF-8
			char* readData = new char[fileLength - 1];
			file.Read(readData, fileLength - 2);
			readData[fileLength - 2] = 0;

			int nLen = MultiByteToWideChar(CP_UTF8, 0, readData, strlen(readData), NULL, 0);
			MultiByteToWideChar(CP_UTF8, 0, readData, strlen(readData), fullText.GetBuffer(fileLength - 1), nLen);

			fullText.ReleaseBuffer();
			foundCharectorCode = true;
		}
		file.SeekToBegin();
	}

	if (foundCharectorCode)
	{
		file.Close();

		long textFindStartIndex = 0;

		while (true)
		{
			long lineEnd = fullText.Find(_T("\n"), textFindStartIndex);
			if (lineEnd < 0)
			{
				m_lineTexts.push_back(fullText.Mid(textFindStartIndex).Trim());
				break;
			}
			else
			{
				m_lineTexts.push_back(fullText.Mid(textFindStartIndex, lineEnd - textFindStartIndex));
				textFindStartIndex = lineEnd + 1;
			}
		}

		return true;
	}

	CString lineText;
	while (file.ReadString(lineText))
	{
		m_lineTexts.push_back(lineText);
	}

	file.Close();

	return true;
}

bool FileTextLines::Save(LPCTSTR filePath)
{
	CStdioFile file;
	if (!file.Open(filePath, CStdioFile::modeCreate | CStdioFile::modeWrite)) return false;

	for (auto& text : m_lineTexts)
	{
		file.WriteString(text + _T("\n"));
	}

	file.Close();

	return true;
}
