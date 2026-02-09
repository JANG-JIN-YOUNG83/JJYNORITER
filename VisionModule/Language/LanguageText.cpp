#include "stdafx.h"
#include "LanguageText.h"
#include "LanguageResourceLoader.h"

#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

LanguageResourceLoader g_languageLoder;

long g_languageType = 1;

void Language::InitInstance(LPCTSTR languageFilePath)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	g_languageLoder.Init(languageFilePath);
}

void Language::SetType(long languageType)
{
	g_languageType = languageType;
}

LPCTSTR	Language::GetText(LanguageDef def)
{
	// 1:한국어, 2:영어, 3:중국어

	switch (g_languageType)
	{
	case 1:	return g_languageLoder.GetText(long(def), LanguageType::Korean);
	case 2:	return g_languageLoder.GetText(long(def), LanguageType::English);
	case 3:	return g_languageLoder.GetText(long(def), LanguageType::Chinese);
	case 4:	return g_languageLoder.GetText(long(def), LanguageType::Polish);
	}

	return _T("");
}
