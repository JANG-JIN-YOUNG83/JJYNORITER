#pragma once

#ifdef __GRAB_CLASS_EXPORT__
#define __GRAB_CLASS__ __declspec(dllexport)
#else
#define __GRAB_CLASS__ __declspec(dllimport)
#endif
