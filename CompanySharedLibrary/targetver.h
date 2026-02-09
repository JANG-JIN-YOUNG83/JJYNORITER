#pragma once

// SDKDDKVer.h를 포함하면 최고 수준의 가용성을 가진 Windows 플랫폼이 정의됩니다.

// 이전 Windows 플랫폼에 대해 응용 프로그램을 빌드하려는 경우에는 SDKDDKVer.h를 포함하기 전에
// WinSDKVer.h를 포함하고 _WIN32_WINNT 매크로를 지원하려는 플랫폼으로 설정하십시오.

#include <WinSDKVer.h>

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#ifdef WINVER
#undef WINVER
#endif

#define _WIN32_WINNT	_WIN32_WINNT_WIN7
#define WINVER			_WIN32_WINNT_WIN7

#ifndef _WIN7_PLATFORM_UPDATE
#define _WIN7_PLATFORM_UPDATE
#endif

#include <SDKDDKVer.h>
