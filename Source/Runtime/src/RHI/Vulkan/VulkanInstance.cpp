#include "RHI/Vulkan/VulkanUtils.h"
#include "RHI/Vulkan/VulkanExtension.h"

#include <vector>
#include <memory>
#include <malloc.h>

class VkUtil_Blackboard
{
public:
	VkUtil_Blackboard(const PRHIInstanceDescriptor* desc)
	{
		const PRHIInstanceDescriptor_Vulkan* addtl_exts = (PRHIInstanceDescriptor_Vulkan*)desc->chained;

		int len = sizeof(gs_gpu_instance_exts) / sizeof(char*);
		for (int i = 0; i < len; i++)
		{
			instance_extensions.push_back(gs_gpu_instance_exts[i]);
		}

		len = sizeof(gs_gpu_device_exts) / sizeof(char*);
		for (int i = 0; i < len; i++)
		{
			devices_extensions.push_back(gs_gpu_device_exts[i]);
		}

		if (desc->enable_debug_layer)
			instance_layers.push_back(gs_gpu_validation_layer_name);

		if (desc->enable_set_name)
		{
			instance_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			instance_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}

		if (addtl_exts != RHI_NULL)
		{
			if (addtl_exts->api_type != RENDER_API_TYPE_VULKAN)
				RHI_ASSERT(0 && "chained descriptor render api type must be vulkan!");

			messenger_info_ptr = addtl_exts->pDebugUtilsMessenger;
			report_info_ptr = addtl_exts->pDebugReportMessenger;

			if (addtl_exts->Instance_Extensions != RHI_NULL)
			{
				for (int i = 0; i < addtl_exts->Instance_Extension_Count; i++)
				{
					instance_extensions.push_back(addtl_exts->Instance_Extensions[i]);
				}
			}

			if (addtl_exts->Instance_Layers != RHI_NULL)
			{
				for (int i = 0; i < addtl_exts->Instance_Layer_Count; i++)
				{
					instance_layers.push_back(addtl_exts->Instance_Layers[i]);
				}
			}

			if (addtl_exts->Device_Extensions != RHI_NULL)
			{
				for (int i = 0; i < addtl_exts->Device_Extension_Count; i++)
				{
					instance_layers.push_back(addtl_exts->Device_Extensions[i]);
				}
			}
		}
	}

public:
	const VkDebugUtilsMessengerCreateInfoEXT* messenger_info_ptr = RHI_NULL;
	const VkDebugReportCallbackCreateInfoEXT* report_info_ptr = RHI_NULL;
	std::vector<const char*> instance_extensions;
	std::vector<const char*> instance_layers;
	std::vector<const char*> devices_extensions;
	std::vector<const char*> devices_layer;
};

PRHIInstanceId RHI_Create_Instance_Vulkan(PRHIInstanceDescriptor const* descriptor)
{
	const VkUtil_Blackboard desc_data(descriptor);

	VulkanRHIInstance* inst = (VulkanRHIInstance*)calloc(1,sizeof(VulkanRHIInstance));
	RHI_ASSERT(inst != 0 && "failed to allocate memory for struct of VulkanRHIInstance! ");
	::memset(inst, 0, sizeof(VulkanRHIInstance));

	VkResult volkInit = volkInitialize();
	if (volkInit != VK_SUCCESS)
		RHI_ASSERT(0 && " volk initialize failed! ");

	VkUtil_SelectInstanceLayers(inst, desc_data.instance_layers.data(), desc_data.instance_layers.size());
	VkUtil_SelectInstanceExtensions(inst, desc_data.instance_extensions.data(), desc_data.instance_extensions.size());

	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "PandaEngine";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "No Engine";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_1;

	VkInstanceCreateInfo inst_create_info = {};
	inst_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	inst_create_info.pApplicationInfo = &app_info;
	inst_create_info.enabledLayerCount = inst->layerCount;
	inst_create_info.ppEnabledLayerNames = inst->ppLayerNames;
	inst_create_info.enabledExtensionCount = inst->extensionCount;
	inst_create_info.ppEnabledExtensionNames = inst->ppExtensionNames;

	VkResult create_res = vkCreateInstance(&inst_create_info, nullptr, &inst->pVkInstance);
	if (create_res != VK_SUCCESS)
		RHI_ASSERT(0 && "Vulkan : failed to create vkInstance!");

	volkLoadInstance(inst->pVkInstance);

	VkUtil_QueryAllAdapter(inst,desc_data.devices_layer.data(),desc_data.devices_layer.size(),desc_data.devices_extensions.data(),desc_data.devices_extensions.size());

	inst->bDebugUtils = true;
	if (descriptor->enable_debug_layer)
		VkUtil_EnableValidationLayer(inst, desc_data.messenger_info_ptr, desc_data.report_info_ptr);

	return &(inst->super);
}

RHI_API void RHI_Free_Instance_Vulkan(PRHIInstance* instance)
{
	VulkanRHIInstance* vk_inst = (VulkanRHIInstance*)instance;
	
	if (vk_inst->pVkDebugUtilsMessenger)
	{
		RHI_ASSERT(vkDestroyDebugUtilsMessengerEXT && "Load vkDestroyDebugUtilsMessengerEXT failed!");
		vkDestroyDebugUtilsMessengerEXT(vk_inst->pVkInstance, vk_inst->pVkDebugUtilsMessenger, RHI_NULL);
	}

	if(vk_inst->pVkInstance)
		vkDestroyInstance(vk_inst->pVkInstance, RHI_NULL);

	free(vk_inst);
}



const float g_queuePriorities[] = {
	1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, //
	1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, //
	1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, //
	1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, //
};

RHI_API PRHIDeviceId RHI_Create_Device_Vulkan(PRHIAdapter* adapter, PRHIDeviceDescriptor const* descriptor)
{
	const VulkanRHIInstance* pInst = (VulkanRHIInstance*)adapter->instance;
	const VulkanRHIAdapter* pAdapter = (VulkanRHIAdapter*)adapter;

	VulkanRHIDevice* pLogicDevice = (VulkanRHIDevice*)calloc(1, sizeof(VulkanRHIDevice));
	pLogicDevice->super.adapter = adapter;

	std::vector<VkDeviceQueueCreateInfo> queues(descriptor->queue_group_count);
	for (uint32_t i = 0; i < queues.size(); i++)
	{
		PRHIQueueGroupDescriptor* queue_desc = &descriptor->queue_group[i];
		queues[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queues[i].queueFamilyIndex = pAdapter->queueFamilesIndices[queue_desc->queue_type];
		queues[i].queueCount = queue_desc->queue_count;
		queues[i].pQueuePriorities = g_queuePriorities;
	}
	
	VkDeviceCreateInfo devce_info = {};
	devce_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	devce_info.pQueueCreateInfos = queues.data();
	devce_info.queueCreateInfoCount = queues.size();
	devce_info.pNext = &pAdapter->vkPhysicalDeviceFeatures;
	devce_info.pEnabledFeatures = RHI_NULL;
	devce_info.ppEnabledLayerNames = pAdapter->ppLayerNames;
	devce_info.enabledLayerCount = pAdapter->layerCount;
	devce_info.ppEnabledExtensionNames = pAdapter->ppExtensionNames;
	devce_info.enabledExtensionCount = pAdapter->extensionCount;

	VkResult res = vkCreateDevice(pAdapter->pVkPhysicalDevice, &devce_info, RHI_NULL, &pLogicDevice->pVkDevice);
	RHI_ASSERT(res == VK_SUCCESS && " failed to create vulkan logic device!");

	volkLoadDeviceTable(&pLogicDevice->vkDeviceTable, pLogicDevice->pVkDevice);
	RHI_ASSERT(pLogicDevice->vkDeviceTable.vkCreateSwapchainKHR && "failed to load create swapchin proc!");

	return &(pLogicDevice->super);
}

void RHI_Free_Device_Vulkan(PRHIDevice* device)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)device;

	if(vk_device->pVkDevice)
		vkDestroyDevice(vk_device->pVkDevice, RHI_NULL);

	free(vk_device);
}