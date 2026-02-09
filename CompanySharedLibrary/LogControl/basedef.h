#pragma once

#ifdef __INTEKPLUS_SHARED_LOGCONTROL_CLASS_EXPORT__
#define __INTEKPLUS_SHARED_LOGCONTROL_CLASS__	__declspec(dllexport)
#else
#define __INTEKPLUS_SHARED_LOGCONTROL_CLASS__	__declspec(dllimport)
#endif
