#include "stdafx.h"
#include "LanguageResourceLoader.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LanguageResourceLoader::LanguageResourceLoader()
{
}

LPCTSTR LanguageResourceLoader::GetText(long index, LanguageType type)
{
	if (index < 0 || index >= m_words.size())
	{
		ASSERT(!_T("정의 되지 않은 Define을 사용했습니다."));
		return _T("");
	}

	auto& word = m_words[index];

	switch (type)
	{
	case LanguageType::Korean:	return word.m_korean.IsEmpty() ? word.m_english : word.m_korean;
	case LanguageType::English:	return word.m_english;
	case LanguageType::Chinese:	return word.m_chinese.IsEmpty() ? word.m_english : word.m_chinese;
	case LanguageType::Polish:	return word.m_polish.IsEmpty() ? word.m_english : word.m_polish;
	}

	return _T("");
}

bool LanguageResourceLoader::Init(LPCTSTR languageFilePath)
{
	if (!InitFromResource()) return false;
	Load(languageFilePath);

	return true;
}

bool LanguageResourceLoader::InitFromResource()
{
	auto module = AfxGetInstanceHandle();
	HRSRC hRes = ::FindResource(module, MAKEINTRESOURCE(IDR_LANGUAGE_DEF), _T("Language"));
	if (NULL == hRes)
	{
		return false;
	}

	DWORD dwResourceSize = ::SizeofResource(module, hRes);

	if (0 == dwResourceSize)
	{
		return false;
	}

	LPVOID pData = ::LockResource(::LoadResource(module, hRes));
	if (NULL == pData)
	{
		return false;
	}

	std::vector<BYTE> resourceData;
	resourceData.resize(dwResourceSize + 1);
	resourceData[dwResourceSize] = 0;
	memcpy(&resourceData[0], pData, dwResourceSize);

	return Load(resourceData);
}

bool LanguageResourceLoader::Load(LPCTSTR languageFilePath)
{
	CFile file;
	if (!file.Open(languageFilePath, CFile::modeRead))
	{
		return false;
	}

	long fileSize = (long)file.GetLength();
	std::vector<BYTE> fileData(fileSize);
	if (fileSize > 0)
	{
		file.Read(&fileData[0], fileSize);
	}

	file.Close();

	return Load(fileData);
}

bool LanguageResourceLoader::Load(const std::vector<BYTE>& binaryData)
{

	CString uniCode;
	long utf8Length = ((long)binaryData.size()) - 1;
	int nLen = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)&binaryData[0], utf8Length, NULL, NULL);
	MultiByteToWideChar(CP_UTF8, 0, (LPCCH)&binaryData[0], utf8Length, uniCode.GetBuffer(nLen), nLen);
	uniCode.ReleaseBuffer();

	bool firstLoad = m_words.size() == 0 ? true : false;
	long endChar = -1;
	while (true)
	{
		endChar = uniCode.Find(_T('\n'));

		CString lineText = (endChar >= 0 ? uniCode.Left(endChar) : uniCode).Trim();

		do
		{
			if (lineText.IsEmpty()) break;
			if (lineText.Left(1) == _T("#")) break;
			if (lineText.Left(2) == _T("//")) break;
			if (lineText.Left(3) == _T("enum")) break;
			if (lineText.Left(1) == _T("{")) break;
			if (lineText.Left(1) == _T("}")) break;

			long enumValue = lineText.Find(_T(','));
			if (enumValue < 0) break;

			CString enumKey = lineText.Left(enumValue).Trim();

			if (firstLoad)
			{
				Word word;
				word.m_english = ParseLanguageText(lineText, _T("E"));
				word.m_korean = ParseLanguageText(lineText, _T("K"));
				word.m_chinese = ParseLanguageText(lineText, _T("C"));
				word.m_polish = ParseLanguageText(lineText, _T("P"));
				m_words.push_back(word);
				m_keyToIndex[enumKey] = ((long)m_words.size()) - 1;
			}
			else
			{
				auto itKeyToIndex = m_keyToIndex.find(enumKey);
				if (itKeyToIndex != m_keyToIndex.end())
				{
					long index = itKeyToIndex->second;

					auto& word = m_words[index];
					word.m_english = ParseLanguageText(lineText, _T("E"));
					word.m_korean = ParseLanguageText(lineText, _T("K"));
					word.m_chinese = ParseLanguageText(lineText, _T("C"));
					word.m_polish = ParseLanguageText(lineText, _T("P"));
				}
			}

		} while (false);

		if (endChar < 0)
		{
			break;
		}
		else
		{
			uniCode = uniCode.Mid(endChar + 1);
		}
	}

	return true;
}

CString LanguageResourceLoader::ParseLanguageText(CString text, LPCTSTR key)
{
	CString keyText;
	keyText.Format(_T("[%s@"), key);
	long startIndex = text.Find(keyText);
	if (startIndex < 0) return _T("");

	startIndex += keyText.GetLength();

	long endIndex = text.Find(_T(']'), startIndex);
	if (endIndex < 0) return _T("");

	return text.Mid(startIndex, endIndex - startIndex);
}
