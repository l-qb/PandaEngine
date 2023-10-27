#include "RHI/Vulkan/VulkanUtils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

VKAPI_ATTR VkBool32 VKAPI_CALL VkUtil_DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageServerity, VkDebugUtilsMessageSeverityFlagsEXT MessageType, 
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	switch (MessageServerity)
	{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			printf("vulkan validation layer:%s\n", pCallbackData->pMessage);
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			printf("\033[1;32mvulkan validation layer info: %s \033[0m\n", pCallbackData->pMessage);
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			printf("\033[1;33mvulkan validation layer warn: %s \033[0m\n", pCallbackData->pMessage);
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			printf("\033[1;31mvulkan validation layer error: %s \033[0m\n", pCallbackData->pMessage);
			break;

		default:
			return VK_TRUE;
	}

	return VK_FALSE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VkUtil_DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, 
	uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
	switch (flags)
	{
		case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
			printf("\033[1;32mvulkan validation layer info: %s \033[0m\n", pMessage);

		case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
			printf("\033[1;33mvulkan validation layer warn: %s \033[0m\n", pMessage);
			break;

		case VK_DEBUG_REPORT_WARNING_BIT_EXT:
			printf("\033[1;34mvulkan validation layer warn: %s \033[0m\n", pMessage);
			break;

		case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
			printf("\033[1;31mvulkan validation layer debug: %s \033[0m\n", pMessage);
			break;

		case VK_DEBUG_REPORT_ERROR_BIT_EXT:
			printf("\033[1;31mvulkan validation layer error: %s \033[0m\n", pMessage);
			break;

		default:
			return VK_TRUE;
	}

	return VK_FALSE;
}

void VkUtil_EnableValidationLayer(struct VulkanRHIInstance* vk_inst, const VkDebugUtilsMessengerCreateInfoEXT* messenger_info, const VkDebugReportCallbackCreateInfoEXT* report_info)
{
	if (vk_inst->bDebugUtils)
	{
		VkDebugUtilsMessengerCreateInfoEXT create_info = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.flags=0,
			.messageSeverity = 
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType = 
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			.pfnUserCallback = VkUtil_DebugCallback,
			.pUserData=0
		};
		
		const VkDebugUtilsMessengerCreateInfoEXT* pMessengerInfo = (messenger_info != 0 ? messenger_info : &create_info);
		RHI_ASSERT(pMessengerInfo != 0 && " failed to load VkDebugUtilsMessengerCreateInfoEXT!");

		RHI_ASSERT(vkCreateDebugUtilsMessengerEXT && "Load vkCreateDebugUtilsMessengerEXT failed!");
		VkResult res = vkCreateDebugUtilsMessengerEXT(vk_inst->pVkInstance, pMessengerInfo, RHI_NULL, &(vk_inst->pVkDebugUtilsMessenger));
		RHI_ASSERT(res == VK_SUCCESS && " failed to create VkDebugUtilsMessengerEXT!");
	}
	else if (vk_inst->bDebugReport)
	{
		VkDebugReportCallbackCreateInfoEXT create_info = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT,
			.pfnCallback = VkUtil_DebugReportCallback,
			.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT,
			.pNext = 0,
			.pUserData = 0
		};

		const VkDebugReportCallbackCreateInfoEXT* pReportInfo = (report_info != 0 ? report_info : &create_info);
		RHI_ASSERT(pReportInfo != 0 && " failed to load VkDebugReportCallbackCreateInfoEXT!");

		VkResult res = vkCreateDebugReportCallbackEXT(vk_inst->pVkInstance, pReportInfo, RHI_NULL, &(vk_inst->pVkDebugReport));
		RHI_ASSERT(res == VK_SUCCESS && " failed to create vkCreateDebugReportCallbackEXT!");
	}
}

void VkUtil_QueryAllAdapter(struct VulkanRHIInstance* vk_inst, const char* const* device_layers, uint32_t device_layers_count,
	const char* const* device_exts, uint32_t device_exts_count)
{
	vkEnumeratePhysicalDevices(vk_inst->pVkInstance, &vk_inst->vkAdapterCount, RHI_NULL);
	if (vk_inst->vkAdapterCount == 0)
		RHI_ASSERT(0 && "can not find physical device!");

	vk_inst->pVkAdapters = calloc(vk_inst->vkAdapterCount, sizeof(VulkanRHIAdapter));
	VkPhysicalDevice* pAllDevices = _alloca(sizeof(VkPhysicalDevice) * vk_inst->vkAdapterCount);
	memset(pAllDevices, 0, sizeof(VkPhysicalDevice) * vk_inst->vkAdapterCount);

	vkEnumeratePhysicalDevices(vk_inst->pVkInstance, &vk_inst->vkAdapterCount, pAllDevices);
	
	for (uint32_t i = 0; i < vk_inst->vkAdapterCount; i++)
	{
		vk_inst->pVkAdapters[i].super.instance = &(vk_inst->super);
		vk_inst->pVkAdapters[i].pVkPhysicalDevice = pAllDevices[i];

		vk_inst->pVkAdapters[i].vkPhysicalDeviceProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2_KHR;
		vkGetPhysicalDeviceProperties2(pAllDevices[i], &vk_inst->pVkAdapters[i].vkPhysicalDeviceProps);
		
		vk_inst->pVkAdapters[i].vkPhysicalDeviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR;
		vkGetPhysicalDeviceFeatures2(pAllDevices[i], &vk_inst->pVkAdapters[i].vkPhysicalDeviceFeatures);
		
		printf("device %d : %s\n", i, vk_inst->pVkAdapters[i].vkPhysicalDeviceProps.properties.deviceName);

		VkUtil_SelectPhysicalDeviceLayers(&vk_inst->pVkAdapters[i], device_layers, device_layers_count);
		VkUtil_SelectPhysicalDeviceExtensions(&vk_inst->pVkAdapters[i], device_exts, device_exts_count);
		VkUtil_SelectQueueIndices(&vk_inst->pVkAdapters[i]);
	}


}

void VkUtil_SelectInstanceLayers(struct VulkanRHIInstance* vk_inst, const char* const* inst_layers, uint32_t inst_layers_count)
{
	uint32_t count = 0;
	vkEnumerateInstanceLayerProperties(&count, 0);

	if (!count)
		return;

	vk_inst->ppLayerNames = calloc(inst_layers_count, sizeof(char*));
	vk_inst->pLayerProps = calloc(inst_layers_count, sizeof(VkLayerProperties));

	VkLayerProperties* pLayerProps = _alloca(sizeof(VkLayerProperties) * count);
	memset(pLayerProps, 0, sizeof(VkLayerProperties) * count);

	vkEnumerateInstanceLayerProperties(&count, pLayerProps);
	uint32_t filled_count = 0;
	for (uint32_t i = 0; i < inst_layers_count; i++)
	{
		for (uint32_t j = 0; j < count; j++)
		{
			if (strcmp(pLayerProps[j].layerName, inst_layers[i]) == 0)
			{
				VkLayerProperties* pProps = &pLayerProps[j];
				vk_inst->pLayerProps[filled_count] = *pProps;
				vk_inst->ppLayerNames[filled_count] = vk_inst->pLayerProps[filled_count].layerName;
				filled_count++;
				break;
			}
		}
	}

	vk_inst->layerCount = filled_count;
}

void VkUtil_SelectInstanceExtensions(struct VulkanRHIInstance* vk_inst, const char* const* inst_exts, uint32_t inst_exts_count)
{
	const char* layer_name = 0;
	uint32_t count = 0;
	vkEnumerateInstanceExtensionProperties(layer_name, &count, 0);

	if (!count)
		return;

	vk_inst->ppExtensionNames = calloc(inst_exts_count, sizeof(char*));
	vk_inst->pExtensionProps = calloc(inst_exts_count, sizeof(VkExtensionProperties));

	VkExtensionProperties* pExtsProps = _alloca(sizeof(VkExtensionProperties) * count);
	memset(pExtsProps, 0, sizeof(VkExtensionProperties) * count);

	vkEnumerateInstanceExtensionProperties(layer_name, &count, pExtsProps);
	uint32_t filled_count = 0;
	for (uint32_t i = 0; i < inst_exts_count; i++)
	{
		for (uint32_t j = 0; j < count; j++)
		{
			if (strcmp(pExtsProps[j].extensionName, inst_exts[i]) == 0)
			{
				VkExtensionProperties* pProps = &pExtsProps[j];
				vk_inst->pExtensionProps[filled_count] = *pProps;
				vk_inst->ppExtensionNames[filled_count] = vk_inst->pExtensionProps[filled_count].extensionName;
				filled_count++;
				break;
			}
		}
	}

	vk_inst->extensionCount = filled_count;
}

void VkUtil_SelectPhysicalDeviceLayers(struct VulkanRHIAdapter* vk_adapter, const char* const* device_layers, uint32_t device_layers_count)
{
	uint32_t count = 0;
	vkEnumerateDeviceLayerProperties(vk_adapter->pVkPhysicalDevice, &count, RHI_NULL);
	if (!count)
		return;

	vk_adapter->ppLayerNames = calloc(device_layers_count, sizeof(char*));
	vk_adapter->pLayerProps = calloc(device_layers_count, sizeof(VkLayerProperties));

	VkLayerProperties* pAllLayers = _alloca(sizeof(VkLayerProperties) * count);
	memset(pAllLayers, 0, sizeof(VkLayerProperties) * count);
	vkEnumerateDeviceLayerProperties(vk_adapter->pVkPhysicalDevice, &count,pAllLayers);

	uint32_t filled_count = 0;
	for (uint32_t i = 0; i < device_layers_count; i++)
	{
		for (uint32_t j = 0; j < count; j++)
		{
			if (strcmp(pAllLayers[j].layerName, device_layers[i]) == 0)
			{
				vk_adapter->pLayerProps[filled_count] = pAllLayers[j];
				vk_adapter->ppLayerNames[filled_count] = vk_adapter->pLayerProps[filled_count].layerName;
				filled_count++;
				break;
			}
		}
	}

	vk_adapter->layerCount = filled_count;
}

void VkUtil_SelectPhysicalDeviceExtensions(struct VulkanRHIAdapter* vk_adapter, const char* const* device_exts, uint32_t device_exts_count)
{
	uint32_t count = 0;
	vkEnumerateDeviceExtensionProperties(vk_adapter->pVkPhysicalDevice, RHI_NULL, &count, RHI_NULL);
	if (!count)
		return;

	vk_adapter->ppExtensionNames = calloc(device_exts_count, sizeof(char*));
	vk_adapter->pExtensionProps = calloc(device_exts_count, sizeof(VkExtensionProperties));

	VkExtensionProperties* pAllExts = _alloca(sizeof(VkExtensionProperties) * count);
	memset(pAllExts, 0, sizeof(VkExtensionProperties) * count);
	vkEnumerateDeviceExtensionProperties(vk_adapter->pVkPhysicalDevice, RHI_NULL, &count, pAllExts);

	uint32_t filled_count = 0;
	for (uint32_t i = 0; i < device_exts_count; i++)
	{
		for (uint32_t j = 0; j < count; j++)
		{
			if (strcmp(pAllExts[j].extensionName, device_exts[i]) == 0)
			{
				vk_adapter->pExtensionProps[filled_count] = pAllExts[j];
				vk_adapter->ppExtensionNames[filled_count] = vk_adapter->pExtensionProps[filled_count].extensionName;
				filled_count++;
				break;
			}
		}
	}

	vk_adapter->extensionCount = filled_count;
}

void VkUtil_SelectQueueIndices(struct VulkanRHIAdapter* vk_adapter)
{
	vkGetPhysicalDeviceQueueFamilyProperties(vk_adapter->pVkPhysicalDevice, &vk_adapter->queueFamiliesCount, RHI_NULL);
	vk_adapter->pVkQueueFamilyProps = calloc(vk_adapter->queueFamiliesCount, sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(vk_adapter->pVkPhysicalDevice, &vk_adapter->queueFamiliesCount, vk_adapter->pVkQueueFamilyProps);

	memset(vk_adapter->queueFamilesIndices, -1, sizeof(int64_t) * DEVICE_QUEUE_TYPE_COUNT);
	for (uint32_t i = 0; i < vk_adapter->queueFamiliesCount; i++)
	{
		const VkQueueFamilyProperties* props = &vk_adapter->pVkQueueFamilyProps[i];
		if ((props->queueFlags & VK_QUEUE_GRAPHICS_BIT) && (vk_adapter->queueFamilesIndices[DEVICE_QUEUE_TYPE_GRAPHICS] ==-1))
		{
			vk_adapter->queueFamilesIndices[DEVICE_QUEUE_TYPE_GRAPHICS] = i;
		}
		else if(props->queueFlags & VK_QUEUE_TRANSFER_BIT && (vk_adapter->queueFamilesIndices[DEVICE_QUEUE_TYPE_TRANSFER] == -1))
		{
			vk_adapter->queueFamilesIndices[DEVICE_QUEUE_TYPE_TRANSFER] = i;
		}
		else if (props->queueFlags & VK_QUEUE_COMPUTE_BIT && (vk_adapter->queueFamilesIndices[DEVICE_QUEUE_TYPE_COMPUTE] == -1))
		{
			vk_adapter->queueFamilesIndices[DEVICE_QUEUE_TYPE_COMPUTE] = i;
		}
		else if (props->queueFlags & VK_QUEUE_SPARSE_BINDING_BIT && (vk_adapter->queueFamilesIndices[DEVICE_QUEUE_TYPE_TILE_MAPPING] == -1))
		{
			vk_adapter->queueFamilesIndices[DEVICE_QUEUE_TYPE_TILE_MAPPING] = i;
		}
	}
}



////////////////////////////////////////////





