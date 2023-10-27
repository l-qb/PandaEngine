#pragma once

#include "RHI/PRHI_config.h"
#include "RHI/Vulkan/VulkanRHI.h"

#include "vulkan/vulkan_core.h"

#if defined(_MACOS)
	#include "vulkan/vulkan_macos.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

	PE_UNUSED static const char* gs_gpu_validation_layer_name = "VK_LAYER_KHRONOS_validation";

	PE_UNUSED static const char* gs_gpu_instance_exts[] =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,

		#if defined(VK_USE_PLATFORM_WIN32_KHR)
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
		#endif

		VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME
	};

	PE_UNUSED static const char* gs_gpu_device_exts[] =
	{
		"VK_KHR_portability_subset",
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	/*debug helper*/
	static VKAPI_ATTR VkBool32 VKAPI_CALL VkUtil_DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT MessageServerity,
		VkDebugUtilsMessageSeverityFlagsEXT MessageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	);

	static VKAPI_ATTR VkBool32 VKAPI_CALL VkUtil_DebugReportCallback(
		VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType,
		uint64_t object, size_t location, int32_t messageCode,
		const char* pLayerPrefix, const char* pMessage, void* pUserData);

	/*instance helper*/
	void VkUtil_EnableValidationLayer(struct VulkanRHIInstance* vk_inst,
		const VkDebugUtilsMessengerCreateInfoEXT* messenger_info, const VkDebugReportCallbackCreateInfoEXT* report_info);

	void VkUtil_QueryAllAdapter(struct VulkanRHIInstance* vk_inst, const char* const* device_layers, uint32_t device_layers_count,
		const char* const* device_exts, uint32_t device_exts_count);

	void VkUtil_SelectInstanceLayers(struct VulkanRHIInstance* vk_inst, const char* const* inst_layers, uint32_t inst_layers_count);
	void VkUtil_SelectInstanceExtensions(struct VulkanRHIInstance* vk_inst, const char* const* inst_exts, uint32_t inst_exts_count);
	void VkUtil_SelectPhysicalDeviceLayers(struct VulkanRHIAdapter* vk_adapter, const char* const* device_layers, uint32_t device_layers_count);
	void VkUtil_SelectPhysicalDeviceExtensions(struct VulkanRHIAdapter* vk_adapter, const char* const* device_exts, uint32_t device_exts_count);
	void VkUtil_SelectQueueIndices(struct VulkanRHIAdapter* vk_adapter);

	/* no idea */
	//VkFormat VkUtil_TranslateFormatToVk(EFormatType format);
	//VkFormat VkUtil_TranslateFormatToPRHI(VkFormat format);
	//VkPrimitiveTopology VkUtil_TranslatePrimitiveTopologyToVk(EPrimitiveTopology prim_topology);
	//VkPolygonMode VkUtil_TranslatePolygonToVk(EFillMode filMode);
	//VkCullModeFlags VkUtil_TranslateCullModeToVk(ECullMode cullMode);
	//VkFrontFace VkUtil_TranslateFrontFaceToVk(EFrontFace frontFace);
	//VkSampleCountFlagBits VkUtil_TranslateSampleCountToVk(ESampleCount sampleCount);
	//VkColorComponentFlagBits VkUtil_TranslateColorWriteMaskToVk(EColorWriteMasks masks);
	//VkAttachmentStoreOp VkUtil_TranslateRenderTargetStoreOpToVk(EStoreAction storeAction);
	//VkAttachmentLoadOp VkUtil_TranslateRenderTargetLoadOpToVk(ELoadAction LoadAction);
	//VkImageViewType VkUtil_TranslateTextureDimensionToVk(ETextureDimension dimension);
	//VkCommandBufferUsageFlags VkUtil_TranslateCMDBufferUsageToVk(ECommandBufferUsage usage);

	#ifdef __cplusplus
}
#endif

#include "RHI/Vulkan/VulkanUtils.inl"