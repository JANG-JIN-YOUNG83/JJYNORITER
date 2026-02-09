// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#include "../targetver.h"		// 상위 폴더에 임의로 만들어줘야 하는 파일임
								// Windows SDK 버전에 상관없이, Windows 의 최소 버전을 지정하기 위한 것임
								// CompanySharedLibrary 저장소의 targetver.h 파일을 참조할 것

#include <windows.h>

#define __INTEKPLUS_SHARED_CRASHHANDLER_EXPORT__
