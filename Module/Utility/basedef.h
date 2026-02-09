#pragma once

#ifdef __UTILITY_CLASS_EXPORT__
#define __UTILITY_CLASS__ __declspec(dllexport)
#else
#define __UTILITY_CLASS__ __declspec(dllimport)
#endif

class Database;

namespace Utility
{
	class BaseLogger;
	class IBNI_PCSetting;

	class Logger;
	class LoggerDate;

	class FileReader;
	class FileTextLines;
	class IniPackage;

	class ProgramBackup;
	class RecipeSaver;
	class SystemFont;
}
