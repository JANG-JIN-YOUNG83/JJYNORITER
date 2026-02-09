#pragma once
#include <map>
#include <vector>

#include "../Language/LanguageText.h"

enum class LanguageType
{
	Korean,
	English,
	Chinese,
	Polish,
};

class LanguageResourceLoader
{
public:
	LanguageResourceLoader();

	LPCTSTR GetText(long index, LanguageType type);
	bool Init(LPCTSTR languageFilePath);

private:
	bool InitFromResource();
	bool Load(LPCTSTR languageFilePath);
	bool Load(const std::vector<BYTE>& binaryData);

	CString ParseLanguageText(CString text, LPCTSTR key);

	struct Word
	{
		CString m_english;
		CString m_korean;
		CString m_chinese;
		CString m_polish;
	};

	std::vector<Word> m_words;
	std::map<CString, long> m_keyToIndex;
};
