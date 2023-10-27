#pragma once

#include "Platform/configure.h"
#include "Platform/marcos.h"

#define _DEBUG
#ifdef _DEBUG
#include "assert.h"
#define RHI_ASSERT assert
#else
#define RHI_ASSERT(expr) (void)(expr);
#endif

#define RHI_USE_VULKAN

#ifdef __cplusplus
	#define RHI_NULL nullptr
#else
	#define RHI_NULL 0
#endif

#ifndef RHI_API
	#define RHI_API PANDA_RUNTIME_API
#endif

