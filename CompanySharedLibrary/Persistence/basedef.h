#pragma once

#ifdef __INTEKPLUS_SHARED_PERSISTENCE_EXPORT__
#define __INTEKPLUS_SHARED_PERSISTENCE_API__	__declspec(dllexport)
#else
#define __INTEKPLUS_SHARED_PERSISTENCE_API__	__declspec(dllimport)
#endif
