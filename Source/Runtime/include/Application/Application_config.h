#pragma once

#include "Platform/configure.h"

#ifdef _WIN32 || _WIN64
	#include "Windows.h"
#endif

#ifdef __cplusplus
#define APP_NULL nullptr
#else
#define APP_NULL 0
#endif

#ifndef APP_API
	#define APP_API PANDA_RUNTIME_API
#endif

