#pragma once
#include "LanguageDef.h"

#ifdef __LANGUAGE_CLASS_EXPORT__
#define __LANGUAGE_CLASS__ __declspec(dllexport)
#else
#define __LANGUAGE_CLASS__ __declspec(dllimport)
#endif

class __LANGUAGE_CLASS__ Language
{
public:
	static void		InitInstance(LPCTSTR languageFilePath);
	static void		SetType(long languageType);
	static LPCTSTR	GetText(LanguageDef def);
};

#define LANG_TEXT(def)	CString(Language::GetText(def))
