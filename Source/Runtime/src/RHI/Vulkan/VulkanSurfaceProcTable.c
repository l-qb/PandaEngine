#ifdef _WIN32
	#include <windows.h>
#endif

#include "RHI/Vulkan/VulkanSurface.h"

const PRHISurfaceProcTable g_vk_surface_ProcTable =
{
	.pfn_FreeSurface = &RHI_Free_Surface,
#if defined(_WIN32) || defined(_WIN64)
	.pfn_CreateSurface_Win32 = &RHI_Create_Surface_From_Win32
#endif
};
const PRHISurfaceProcTable* GetSurfaceProcTable_Vulkan() { return &g_vk_surface_ProcTable; }

void RHI_Free_Surface(PRHIInstanceId instance, PRHISurfaceId surface)
{
	VulkanRHIInstance* vk_inst = (VulkanRHIInstance*)instance;

	vkDestroySurfaceKHR(vk_inst->pVkInstance, (VkSurfaceKHR)surface, RHI_NULL);
}

#if defined(_WIN32) || defined(_WIN64)
RHI_API PRHISurfaceId RHI_Create_Surface_From_Win32(PRHIDeviceId device, HWND hwnd)
{
	VkWin32SurfaceCreateInfoKHR surface_info =
	{
		.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		.flags = 0,
		.pNext = 0,
		.hwnd = hwnd,
		.hinstance = GetModuleHandle(0),
	};

	VulkanRHIInstance* inst = (VulkanRHIInstance*)device->adapter->instance;
	PRHISurfaceId surface;
	VkResult res = vkCreateWin32SurfaceKHR(inst->pVkInstance, &surface_info, RHI_NULL, (VkSurfaceKHR*)&surface);
	RHI_ASSERT(res == VK_SUCCESS && "failed to create win32 surface!");

	return surface;
}
#endif
