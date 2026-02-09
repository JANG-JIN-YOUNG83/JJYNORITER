#pragma once

#ifdef __INTEKPLUS_SHARED_CRASHHANDLER_EXPORT__
#define __INTEKPLUS_SHARED_CRASHHANDLER_API__	__declspec(dllexport)
#else
#define __INTEKPLUS_SHARED_CRASHHANDLER_API__	__declspec(dllimport)
#endif

/*

크래시 덤프를 사용하고 싶은 EXE 내에서 아래처럼 사용할 것
 
CrashHandler::Register(false, L"D:\\Temp\\", L"MyDump");

위 구문처럼 객체를 정의하면, 크래시 발생시에 아래처럼 덤프를 남긴다.

D:\Temp\MyDump_20220309_120302.dmp

*/

class __INTEKPLUS_SHARED_CRASHHANDLER_API__ CrashHandler
{
public:
	// Windows SDK 8.1 "DbgHelp.h" Compatible
	enum class MiniDumpType
	{
		MiniDumpNormal = 0x00000000,
		MiniDumpWithDataSegs = 0x00000001,
		MiniDumpWithFullMemory = 0x00000002,
		MiniDumpWithHandleData = 0x00000004,
		MiniDumpFilterMemory = 0x00000008,
		MiniDumpScanMemory = 0x00000010,
		MiniDumpWithUnloadedModules = 0x00000020,
		MiniDumpWithIndirectlyReferencedMemory = 0x00000040,
		MiniDumpFilterModulePaths = 0x00000080,
		MiniDumpWithProcessThreadData = 0x00000100,
		MiniDumpWithPrivateReadWriteMemory = 0x00000200,
		MiniDumpWithoutOptionalData = 0x00000400,
		MiniDumpWithFullMemoryInfo = 0x00000800,
		MiniDumpWithThreadInfo = 0x00001000,
		MiniDumpWithCodeSegs = 0x00002000,
		MiniDumpWithoutAuxiliaryState = 0x00004000,
		MiniDumpWithFullAuxiliaryState = 0x00008000,
		MiniDumpWithPrivateWriteCopyMemory = 0x00010000,
		MiniDumpIgnoreInaccessibleMemory = 0x00020000,
		MiniDumpWithTokenInformation = 0x00040000,
		MiniDumpWithModuleHeaders = 0x00080000,
		MiniDumpFilterTriage = 0x00100000,
		MiniDumpValidTypeFlags = 0x001fffff,
	};

public:
	// 아래 함수를 호출하면, IntekPlus.Shared.CrashHandler.dll 이 있는 디렉토리에서 IntekPlus.Shared.CrashDumper.exe 를 실행한다.
	// IntekPlus.Shared.CrashDumper.exe 를 실행하는데 실패하거나, 초기화하는데 실패하면 현재 프로세스를 강제 종료하도록 구현함.

	static void Register(const bool showDialog, const wchar_t* szDumpDirectory, const wchar_t* szDumpTitle, const MiniDumpType miniDumpType = MiniDumpType::MiniDumpNormal);
};
