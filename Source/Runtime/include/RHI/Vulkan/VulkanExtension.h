#pragma once

#include "RHI/PRHI.h"

/* additional extensions*/
typedef struct PRHIInstanceDescriptor_Vulkan
{
	ERenderAPIType api_type;	

	const char** Instance_Layers;
	uint32_t Instance_Layer_Count;

	const char** Instance_Extensions;
	uint32_t Instance_Extension_Count;

	const char** Device_Extensions;
	uint32_t Device_Extension_Count;

	const struct VkDebugUtilsMessengerCreateInfoEXT* pDebugUtilsMessenger;
	const struct VkDebugReportCallbackCreateInfoEXT* pDebugReportMessenger;
}PRHIInstanceDescriptor_Vulkan;