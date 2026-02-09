#pragma once

#ifdef __CUSTOMCONTROL_CLASS_EXPORT__
#define __CUSTOMCONTROL_CLASS__ __declspec(dllexport)
#else
#define __CUSTOMCONTROL_CLASS__ __declspec(dllimport)
#endif
