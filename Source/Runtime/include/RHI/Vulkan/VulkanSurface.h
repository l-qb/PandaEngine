#pragma once

#include "RHI/Vulkan/VulkanRHI.h"

#ifdef __cplusplus
extern "C" {
#endif

	RHI_API const PRHISurfaceProcTable* GetSurfaceProcTable_Vulkan();

	RHI_API void RHI_Free_Surface(PRHIInstanceId instance,PRHISurfaceId surface);

	#if defined(_WIN32) || (_WIN64)
	RHI_API PRHISurfaceId RHI_Create_Surface_From_Win32(PRHIDeviceId device, HWND hwnd);
#endif

	

#ifdef __cplusplus
}
#endif