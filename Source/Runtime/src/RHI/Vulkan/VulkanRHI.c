#include "RHI/Vulkan/VulkanRHI.h"

PRHIAdapterId RHI_Enum_Adapter_Vulkan(PRHIInstanceId instance, PRHIAdapterId*const pAdapters, uint32_t* pAdapterCount)
{
	VulkanRHIInstance* vk_inst = (VulkanRHIInstance*)instance;
	*pAdapterCount = vk_inst->vkAdapterCount;
	
	if (pAdapters != RHI_NULL)
	{
		for (uint32_t i = 0; i < vk_inst->vkAdapterCount; i++)
		{
			pAdapters[i] = &(vk_inst->pVkAdapters[i].super);
		}
	}
}

PRHISwapchainId RHI_Create_Swapchain_Vulkan(PRHIDevice* device, const PRHISwapchainDescriptor* descriptor)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)device;
	VulkanRHIAdapter* vk_physicalDevice = (VulkanRHIAdapter*)device->adapter;
	VkSurfaceKHR vk_surface = (VkSurfaceKHR)descriptor->surface;

	VulkanRHISwapchain* pNewSwapchain = calloc(1, sizeof(VulkanRHISwapchain));
	
	//Surface Capabilities
	VkSurfaceCapabilitiesKHR surface_cap = { 0 };
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_physicalDevice->pVkPhysicalDevice, vk_surface, &surface_cap);
	if (surface_cap.maxImageCount > 0 && surface_cap.maxImageCount < descriptor->image_count)
	{
		((PRHISwapchainDescriptor*)descriptor)->image_count = surface_cap.maxImageCount;
	}
	else if (surface_cap.minImageCount > descriptor->image_count)
	{
		((PRHISwapchainDescriptor*)descriptor)->image_count = surface_cap.minImageCount;
	}

	//Format 
	VkSurfaceFormatKHR use_format = { 0 };
	uint32_t format_count = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physicalDevice->pVkPhysicalDevice, vk_surface, &format_count, RHI_NULL);
	RHI_ASSERT(format_count != 0 && "supported surface format number is 0!");
	VkSurfaceFormatKHR* SupportFormats = _alloca(sizeof(VkSurfaceFormatKHR) * format_count);
	memset(SupportFormats, 0, sizeof(VkSurfaceFormatKHR) * format_count);
	vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physicalDevice->pVkPhysicalDevice, vk_surface, &format_count, SupportFormats);

	if (format_count == 1 && SupportFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		use_format.format = VK_FORMAT_B8G8R8A8_UNORM;
		use_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	}
	else
	{
		VkFormat requested_format = VkUtil_TranslateFormatToVk(descriptor->format_type);
		const VkSurfaceFormatKHR hdrSurfaceFormat = {
		VK_FORMAT_A2B10G10R10_UNORM_PACK32,
		VK_COLOR_SPACE_HDR10_ST2084_EXT
		};
		VkColorSpaceKHR requested_color_space =
			requested_format == hdrSurfaceFormat.format ? hdrSurfaceFormat.colorSpace : VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

		for (uint32_t i = 0; i < format_count; i++)
		{
			if (SupportFormats[i].format == requested_format && SupportFormats[i].colorSpace == requested_color_space)
			{
				use_format.format = requested_format;
				use_format.colorSpace = requested_color_space;
				break;
			}
		}

		if (use_format.format == VK_FORMAT_UNDEFINED)
		{
			use_format.format = VK_FORMAT_B8G8R8A8_UNORM;
			use_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		}
	}

	//Present Mode
	VkPresentModeKHR use_present_mode = VK_PRESENT_MODE_FIFO_KHR;
	uint32_t present_mode_count = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(vk_physicalDevice->pVkPhysicalDevice, vk_surface, &present_mode_count, RHI_NULL);
	RHI_ASSERT(present_mode_count != 0 && "support present mode count is 0!");
	VkPresentModeKHR* support_present_mode = _alloca(sizeof(VkPresentModeKHR) * present_mode_count);
	memset(support_present_mode, 0, sizeof(VkPresentModeKHR) * present_mode_count);
	for (uint32_t i = 0; i < present_mode_count; i++)
	{
		if (support_present_mode[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			use_present_mode = VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
		else if (support_present_mode[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			use_present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
			break;
		}
	}

	//Swapchain
	VkExtent2D use_extent;
	if (descriptor->width > surface_cap.maxImageExtent.width)
		use_extent.width = surface_cap.maxImageExtent.width;
	else if (descriptor->width < surface_cap.minImageExtent.width)
		use_extent.width = surface_cap.minImageExtent.width;
	else
		use_extent.width = descriptor->width;

	if (descriptor->height > surface_cap.maxImageExtent.height)
		use_extent.height = surface_cap.maxImageExtent.height;
	else if (descriptor->height < surface_cap.minImageExtent.height)
		use_extent.height = surface_cap.minImageExtent.height;
	else
		use_extent.height = descriptor->height;

	VkSharingMode use_sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkSurfaceTransformFlagBitsKHR use_preTransform;
	if (surface_cap.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		use_preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	else
		use_preTransform = surface_cap.currentTransform;

	VkCompositeAlphaFlagBitsKHR use_composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	const VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[] = {
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
	};
	for (uint32_t i = 0; i < (sizeof(compositeAlphaFlags) / sizeof(compositeAlphaFlags[0])); i++)
	{
		if (compositeAlphaFlags[i] & surface_cap.supportedCompositeAlpha)
		{
			use_composite_alpha = compositeAlphaFlags[i];
			break;
		}
	}

	VkSwapchainCreateInfoKHR swapchain_info =
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = NULL,
		.flags = 0,
		.surface = vk_surface,
		.minImageCount = descriptor->image_count,
		.imageFormat = use_format.format,
		.imageColorSpace = use_format.colorSpace,
		.imageExtent = use_extent,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		.imageSharingMode = use_sharingMode,
		.pQueueFamilyIndices = NULL,
		.queueFamilyIndexCount = 0,
		.preTransform = use_preTransform,
		.compositeAlpha = use_composite_alpha,
		.presentMode = use_present_mode,
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE
	};

	VkSwapchainKHR new_swapchain = VK_NULL_HANDLE;
	VkResult res= vk_device->vkDeviceTable.vkCreateSwapchainKHR(vk_device->pVkDevice, &swapchain_info, RHI_NULL, &new_swapchain);
	RHI_ASSERT(res == VK_SUCCESS && "failed to create swapchain!");
	
	uint32_t image_count = 0;
	vk_device->vkDeviceTable.vkGetSwapchainImagesKHR(vk_device->pVkDevice, new_swapchain, &image_count, RHI_NULL);
	RHI_ASSERT(image_count != 0);

	VkImage* newchain_images = _alloca(sizeof(VkImage) * image_count);
	memset(newchain_images, 0, sizeof(VkImage)* image_count);
	vk_device->vkDeviceTable.vkGetSwapchainImagesKHR(vk_device->pVkDevice, new_swapchain, &image_count, newchain_images);

	VulkanRHITexture* textures = calloc(image_count, sizeof(VulkanRHITexture));
	PRHITextureInfo* textures_info = calloc(image_count, sizeof(PRHITextureInfo));

	for (uint32_t i = 0; i < image_count; i++)
	{
		textures[i].pVkImage = newchain_images[i];

		textures_info[i].width = use_extent.width;
		textures_info[i].height = use_extent.height;
		textures_info[i].depth = 1;
		textures_info[i].format = VkUtil_TranslateFormatToPRHI(use_format.format);

		textures[i].super.texture_info = textures_info;
		textures[i].super.device = &vk_device->super;
	}

	PRHITextureId* ppTextures =calloc(image_count,sizeof(PRHITextureId));
	for (uint32_t i = 0; i < image_count; i++)
	{
		ppTextures[i] = &textures[i].super;
	}

	pNewSwapchain->pVkSurface = vk_surface;
	pNewSwapchain->pVkSwapchin = new_swapchain;
	pNewSwapchain->super.chain_image_count = image_count;
	pNewSwapchain->super.chain_images = ppTextures;

	return &(pNewSwapchain->super);
}

void RHI_Free_Swapchain_Vulkan(PRHISwapchain* swapchain)
{
	VulkanRHISwapchain* vk_swapchain = (VulkanRHISwapchain*)swapchain;
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)swapchain->device;

	if(vk_swapchain->pVkSwapchin)
		vk_device->vkDeviceTable.vkDestroySwapchainKHR(vk_device->pVkDevice, vk_swapchain->pVkSwapchin, RHI_NULL);

	free(vk_swapchain);
}

uint32_t RHI_Acquire_Next_Image_Vulkan(PRHISwapchain* swapchain, const struct PRHIAcquireNextImageDescriptor* descriptor)
{
	VulkanRHISwapchain* vk_swapchain = (VulkanRHISwapchain*)swapchain;
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)swapchain->device;
	VulkanRHISemaphore* vk_signalSemaphore = (VulkanRHISemaphore*)descriptor->signal_semaphore;
	VulkanRHIFence* vk_fence = (VulkanRHIFence*)descriptor->fence;

	VkSemaphore signalSemaphore = vk_signalSemaphore ? vk_signalSemaphore->pVkSemaphore : VK_NULL_HANDLE;
	VkFence fence = vk_fence ? vk_fence->pVkFence : VK_NULL_HANDLE;

	uint32_t image_index;

	VkResult res = vk_device->vkDeviceTable.vkAcquireNextImageKHR(vk_device->pVkDevice, vk_swapchain->pVkSwapchin, UINT64_MAX, signalSemaphore, fence, &image_index);
	RHI_ASSERT(res == VK_SUCCESS && "vulkan API : failed to acquire next image!");
	
	return image_index;
}

PRHITextureViewId RHI_Create_TextureView_Vulkan(PRHIDevice* device, const PRHITextureViewDescriptor* descriptor)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)device;
	VulkanRHITexture* vk_texture = (VulkanRHITexture*)descriptor->texture;
	VulkanRHITextureView* new_textureView = calloc(1, sizeof(VulkanRHITextureView));

	if (descriptor->usages & PRHI_TVU_RTV_DSV)
	{
		VkImageViewCreateInfo imageView_info =
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = vk_texture->pVkImage,
			.viewType = VkUtil_TranslateTextureDimensionToVk(descriptor->dims),
			.format = VkUtil_TranslateFormatToVk(descriptor->format),
			.components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
			.subresourceRange.aspectMask = descriptor->aspects,
			.subresourceRange.baseMipLevel = descriptor->base_mip_level,
			.subresourceRange.levelCount = descriptor->mip_level_count,
			.subresourceRange.baseArrayLayer = descriptor->base_array_layer,
			.subresourceRange.layerCount = descriptor->array_layer_count,
			.pNext = RHI_NULL,
			.flags = 0
		};

		VkResult res = vk_device->vkDeviceTable.vkCreateImageView(vk_device->pVkDevice, &imageView_info, RHI_NULL, &new_textureView->pVkRTVDSVDescriptor);
		RHI_ASSERT(res == VK_SUCCESS && "failed to create vulkan imageView!");
	}

	return& (new_textureView->super);
}

void RHI_Free_TextureView_Vulkan(PRHITextureView* textureView)
{
	VulkanRHIDevice* vk_device = textureView->device;
	VulkanRHITextureView* vk_imageView = textureView;

	if (vk_imageView->pVkRTVDSVDescriptor)
		vk_device->vkDeviceTable.vkDestroyImageView(vk_device->pVkDevice, vk_imageView->pVkRTVDSVDescriptor, RHI_NULL);

	if (vk_imageView->pVkSRVDescriptor)
		vk_device->vkDeviceTable.vkDestroyImageView(vk_device->pVkDevice, vk_imageView->pVkSRVDescriptor, RHI_NULL);

	if (vk_imageView->pVkUAVDescriptor)
		vk_device->vkDeviceTable.vkDestroyImageView(vk_device->pVkDevice, vk_imageView->pVkUAVDescriptor, RHI_NULL);

	free(vk_imageView);
}

PRHIShaderLibraryId RHI_Create_ShaderLibrary_Vulkan(PRHIDevice* device, const  PRHIShaderLibraryDescriptor* descriptor)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)device;
	VulkanRHIShaderLibrary* pNewShaderLibrary = calloc(1, sizeof(VulkanRHIShaderLibrary));

	VkShaderModuleCreateInfo info = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = descriptor->code_size,
		.pCode = descriptor->code
	};

	if (!descriptor->reflection_only)
	{
		VkResult res = vk_device->vkDeviceTable.vkCreateShaderModule(vk_device->pVkDevice, &info, RHI_NULL, &pNewShaderLibrary->pVkShaderModule);
		RHI_ASSERT(res == VK_SUCCESS && "failed to create vulkan shader module!");
	}

	return &(pNewShaderLibrary->super);
}

void RHI_Free_ShaderLibrary_Vulkan(PRHIShaderLibrary* shaderLibrary)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)shaderLibrary->device;
	VulkanRHIShaderLibrary* pShaderLibrary = (VulkanRHIShaderLibrary*)shaderLibrary;
	
	if (pShaderLibrary->pVkShaderModule)
		vk_device->vkDeviceTable.vkDestroyShaderModule(vk_device->pVkDevice, pShaderLibrary->pVkShaderModule, RHI_NULL);

	free(pShaderLibrary);
}

PRHIRootSignatureId RHI_Create_RootSignature_Vulkan(PRHIDevice* device, const  PRHIRootSignatureDescriptor* descriptor)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)device;

	VulkanRHIRootSignature* new_rootSignature = calloc(1, sizeof(VulkanRHIRootSignature));

	VkPipelineLayoutCreateInfo layout_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 0,
		.pSetLayouts = 0,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = 0,
		.pNext = RHI_NULL,
		.flags = 0,
	};

	VkResult res = vk_device->vkDeviceTable.vkCreatePipelineLayout(vk_device->pVkDevice, &layout_info, RHI_NULL, &new_rootSignature->pPipelineLayout);
	RHI_ASSERT(res == VK_SUCCESS && "failed to create vulkan pipeline layout!");

	return &(new_rootSignature->super);
}

void RHI_Free_RootSignature_Vulkan(PRHIRootSignature* rootSignature)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)rootSignature->device;
	VulkanRHIRootSignature* vk_pipelineLayout = (VulkanRHIRootSignature*)rootSignature;

	if (vk_pipelineLayout->pPipelineLayout)
		vk_device->vkDeviceTable.vkDestroyPipelineLayout(vk_device->pVkDevice, vk_pipelineLayout->pPipelineLayout, RHI_NULL);

	free(vk_pipelineLayout);
}

PRHIQueueId RHI_Get_Device_Queue_Vulkan(PRHIDevice* device, EQueueType type, uint32_t index)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)device;
	VulkanRHIAdapter* vk_adapter = (VulkanRHIAdapter*)device->adapter;
	VulkanRHIQueue* new_queue = calloc(1, sizeof(VulkanRHIQueue));
	new_queue->queueFamilyIndex = vk_adapter->queueFamilesIndices[type];

	vk_device->vkDeviceTable.vkGetDeviceQueue(vk_device->pVkDevice, new_queue->queueFamilyIndex, index, &new_queue->pVkQueue);
	
	return new_queue;
}

void RHI_Queue_Submit_Vulkan(PRHIQueue* queue, const struct PRHIQueueSubmitDescriptor* descriptor)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)queue->device;
	VulkanRHIQueue* vk_queue = (VulkanRHIQueue*)queue;
	VulkanRHICommandBuffer** vk_cmdBuffers = (VulkanRHICommandBuffer**)descriptor->cmds;
	VulkanRHISemaphore** vk_signalSemaphores = (VulkanRHISemaphore**)descriptor->signal_semaphores;
	VulkanRHISemaphore** vk_waitSemaphores = (VulkanRHISemaphore**)descriptor->wait_semaphores;
	VulkanRHIFence* vk_signalFences = (VulkanRHIFence*)descriptor->signal_fence;

	VkCommandBuffer* pvkCmds = _alloca(sizeof(VkCommandBuffer) * descriptor->cmds_count);
	memset(pvkCmds, 0, sizeof(VkCommandBuffer) * descriptor->cmds_count);
	for (uint32_t i = 0; i < descriptor->cmds_count; i++)
	{
		pvkCmds[i] = vk_cmdBuffers[i]->pVkCmdBuffer;
	}

	VkSemaphore* pvkSignalSemaphores = _alloca(sizeof(VkSemaphore) * descriptor->signal_semaphore_count);
	memset(pvkSignalSemaphores, 0, sizeof(VkSemaphore) * descriptor->signal_semaphore_count);
	for (uint32_t i = 0; i < descriptor->signal_semaphore_count; i++)
	{
		pvkSignalSemaphores[i] = vk_signalSemaphores[i]->pVkSemaphore;
	}

	VkSemaphore* pvkWaitSemaphores = _alloca(sizeof(VkSemaphore) * descriptor->wait_semaphore_count);
	memset(pvkWaitSemaphores, 0, sizeof(VkSemaphore) * descriptor->wait_semaphore_count);
	for (uint32_t i = 0; i < descriptor->wait_semaphore_count; i++)
	{
		pvkWaitSemaphores[i] = vk_waitSemaphores[i]->pVkSemaphore;
	}
	
	VkPipelineStageFlags use_waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSubmitInfo sb_info =
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = descriptor->cmds_count,
		.pCommandBuffers = pvkCmds,
		.signalSemaphoreCount = descriptor->signal_semaphore_count,
		.pSignalSemaphores = pvkSignalSemaphores,
		.waitSemaphoreCount = descriptor->wait_semaphore_count,
		.pWaitSemaphores = pvkWaitSemaphores,
		.pWaitDstStageMask = use_waitStages,
		.pNext = RHI_NULL
	};

	VkResult res = vk_device->vkDeviceTable.vkQueueSubmit(vk_queue->pVkQueue, 1, &sb_info, vk_signalFences ? vk_signalFences->pVkFence : VK_NULL_HANDLE);
	RHI_ASSERT(res == VK_SUCCESS && "vulkan API : failed to submit draw command buffer!");
}

void RHI_Queue_Present_Vulkan(PRHIQueue* queue, const struct PRHIQueuePresentDescriptor* descriptor)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)queue->device;
	VulkanRHIQueue* vk_queue = (VulkanRHIQueue*)queue;
	VulkanRHISemaphore** vk_waitSemaphores = (VulkanRHISemaphore**)descriptor->wait_semaphores;
	VulkanRHISwapchain* vk_swapchain = (VulkanRHISwapchain*)descriptor->swapchain;

	VkSemaphore* pvkWaitSemaphores = _alloca(sizeof(VkSemaphore) * descriptor->wait_semaphore_count);
	memset(pvkWaitSemaphores, 0, sizeof(VkSemaphore) * descriptor->wait_semaphore_count);
	for (uint32_t i = 0; i < descriptor->wait_semaphore_count; i++)
	{
		pvkWaitSemaphores[i] = vk_waitSemaphores[i]->pVkSemaphore;
	}

	uint32_t prenset_index = descriptor->index;
	VkPresentInfoKHR present_info =
	{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = descriptor->wait_semaphore_count,
		.pWaitSemaphores = pvkWaitSemaphores,
		.swapchainCount = 1,
		.pSwapchains = &vk_swapchain->pVkSwapchin,
		.pImageIndices = &prenset_index,
		.pResults = VK_NULL_HANDLE,
		.pNext = RHI_NULL
	};

	VkResult res = vk_device->vkDeviceTable.vkQueuePresentKHR(vk_queue->pVkQueue, &present_info);
	RHI_ASSERT(res == VK_SUCCESS && "vulkan API : failed to submit present command buffer!");
}

void RHI_Queue_WaitIdle_Vulkan(PRHIQueue* queue)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)queue->device;
	VulkanRHIQueue* vk_queue = (VulkanRHIQueue*)queue;

	vk_device->vkDeviceTable.vkQueueWaitIdle(vk_queue->pVkQueue);
}

PRHICommandPoolId RHI_Create_CommandPool_Vulkan(PRHIQueue* queue, const PRHICommandPoolDescriptor* descriptor)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)queue->device;
	VulkanRHIQueue* vk_queue = (VulkanRHIQueue*)queue;

	VkCommandPoolCreateInfo cp_info =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = vk_queue->queueFamilyIndex,
		.pNext = RHI_NULL,
		.flags = 0
	};

	VulkanRHICommandPool* new_commandPool = calloc(1, sizeof(VulkanRHICommandPool));
	VkResult res = vk_device->vkDeviceTable.vkCreateCommandPool(vk_device->pVkDevice, &cp_info, RHI_NULL, &new_commandPool->pVkCmdPool);
	RHI_ASSERT(res == VK_SUCCESS && "failed to create vulkan commandPool!");

	return &(new_commandPool->super);
}

void RHI_Free_CommandPool_Vulkan(PRHICommandPool* cmdPool)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)cmdPool->queue->device;
	VulkanRHICommandPool* vk_cmdPool = (VulkanRHICommandPool*)cmdPool;
	
	if (vk_cmdPool->pVkCmdPool)
		vk_device->vkDeviceTable.vkDestroyCommandPool(vk_device->pVkDevice, vk_cmdPool->pVkCmdPool, RHI_NULL);

	free(vk_cmdPool);
}

PRHICommandBufferId RHI_Create_CommandBuffer_Vulkan(PRHICommandPool* cmdPool, const PRHICommandBufferDescriptor* descriptor)
{
	VulkanRHICommandPool* vk_cmdPool = (VulkanRHICommandPool*)cmdPool;
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)cmdPool->queue->device;

	uint32_t use_level = descriptor->is_secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	VkCommandBufferAllocateInfo alloc_info =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = vk_cmdPool->pVkCmdPool,
		.level = use_level,
		.commandBufferCount = 1,
		.pNext = RHI_NULL
	};

	VulkanRHICommandBuffer* new_cmdBuffer = calloc(1, sizeof(VulkanRHICommandBuffer));
	VkResult res = vk_device->vkDeviceTable.vkAllocateCommandBuffers(vk_device->pVkDevice, &alloc_info, &new_cmdBuffer->pVkCmdBuffer);
	RHI_ASSERT(res == VK_SUCCESS && "failed to allocate vulkan command buffer!");

	return &(new_cmdBuffer->super);
}

void RHI_Free_CommandBuffer_Vulkan(PRHICommandBuffer* cmdBuffer)
{
	VulkanRHICommandBuffer* vk_cmdBuffer = (VulkanRHICommandBuffer*)cmdBuffer;
	VulkanRHICommandPool* vk_cmdPool = (VulkanRHICommandPool*)vk_cmdBuffer->super.pool;
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)cmdBuffer->device;

	if (vk_cmdBuffer->pVkCmdBuffer)
		vk_device->vkDeviceTable.vkFreeCommandBuffers(vk_device->pVkDevice, vk_cmdPool->pVkCmdPool, 1, &vk_cmdBuffer->pVkCmdBuffer);

	free(vk_cmdBuffer);
}

void RHI_Cmd_Begin_CommandBuffer_Vulkan(PRHICommandBuffer* cmdBuffer, ECommandBufferUsage usage)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)cmdBuffer->device;
	VulkanRHICommandBuffer* vk_cmdBuffer = (VulkanRHICommandBuffer*)cmdBuffer;

	VkCommandBufferBeginInfo begin_info =
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pInheritanceInfo = RHI_NULL,
		.pNext = RHI_NULL,
		.flags = VkUtil_TranslateCMDBufferUsageToVk(usage)
	};

	VkResult res = vk_device->vkDeviceTable.vkBeginCommandBuffer(vk_cmdBuffer->pVkCmdBuffer, &begin_info);
	RHI_ASSERT(res == VK_SUCCESS && "vulkan API : failed to begin recording command!");
}

void RHI_Cmd_Begin_RenderPass_Vulkan(PRHICommandBuffer* cmdBuffer, const PRHICmdRenderPassDescriptor* descriptor)
{
	VulkanRHICommandBuffer* vk_cmdBuffer = (VulkanRHICommandBuffer*)cmdBuffer;
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)cmdBuffer->device;
	VulkanRHIRenderPass* vk_renderPass = (VulkanRHIRenderPass*)descriptor->render_pass;
	VulkanRHIFrameBuffer* vk_frameBuffer = (VulkanRHIFrameBuffer*)descriptor->frame_buffer;
	
	VkExtent2D use_extent = { descriptor->width,descriptor->height };
	VkClearColorValue use_clearValue = { 0.0f,0.0f,0.0f,1.0f };
	VkRenderPassBeginInfo rpb_info =
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderArea.offset = {0,0},
		.renderArea.extent = use_extent,
		.renderPass = vk_renderPass->pVkRenderPass,
		.framebuffer = vk_frameBuffer->pVkFrameBuffer,
		.clearValueCount = 1,
		.pClearValues = &use_clearValue,
		.pNext = RHI_NULL
	};

	vk_device->vkDeviceTable.vkCmdBeginRenderPass(vk_cmdBuffer->pVkCmdBuffer, &rpb_info, VK_SUBPASS_CONTENTS_INLINE);
}

void RHI_Cmd_Bind_Graphics_Pipeline_Vulkan(PRHICommandBuffer* cmdBuffer, PRHIGraphicsPipeline* pipeline)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)cmdBuffer->device;
	VulkanRHICommandBuffer* vk_cmdBuffer = (VulkanRHICommandBuffer*)cmdBuffer;
	VulkanRHIGraphicsPipeline* vk_pipeline = (VulkanRHIGraphicsPipeline*)pipeline;

	vk_device->vkDeviceTable.vkCmdBindPipeline(vk_cmdBuffer->pVkCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline->pVkPipeline);
}

void RHI_Cmd_Set_Viewport_Vulkan(PRHICommandBuffer* cmdBuffer, float x, float y, float width, float height, float minDepth, float maxDepth)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)cmdBuffer->device;
	VulkanRHICommandBuffer* vk_cmdBuffer = (VulkanRHICommandBuffer*)cmdBuffer;
	
	VkViewport use_viewport =
	{
		.x = x,
		.y = y,
		.width = width,
		.height = height,
		.minDepth = minDepth,
		.maxDepth = maxDepth
	};

	vk_device->vkDeviceTable.vkCmdSetViewport(vk_cmdBuffer->pVkCmdBuffer, 0, 1, &use_viewport);
}

void RHI_Cmd_Set_Scissor_Vulkan(PRHICommandBuffer* cmdBuffer, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)cmdBuffer->device;
	VulkanRHICommandBuffer* vk_cmdBuffer = (VulkanRHICommandBuffer*)cmdBuffer;

	VkRect2D use_scissor =
	{
		.offset.x = x,
		.offset.y = y,
		.extent.width = width,
		.extent.height = height
	};

	vk_device->vkDeviceTable.vkCmdSetScissor(vk_cmdBuffer->pVkCmdBuffer, 0, 1, &use_scissor);
}

void RHI_Cmd_Draw_Vulkan(PRHICommandBuffer* cmdBuffer, uint32_t vertCount, uint32_t firstVert)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)cmdBuffer->device;
	VulkanRHICommandBuffer* vk_cmdBuffer = (VulkanRHICommandBuffer*)cmdBuffer;

	vk_device->vkDeviceTable.vkCmdDraw(vk_cmdBuffer->pVkCmdBuffer, vertCount, 1, firstVert, 0);
}

void RHI_Cmd_End_RenderPass_Vulkan(PRHICommandBuffer* cmdBuffer)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)cmdBuffer->device;
	VulkanRHICommandBuffer* vk_cmdBuffer = (VulkanRHICommandBuffer*)cmdBuffer;

	vk_device->vkDeviceTable.vkCmdEndRenderPass(vk_cmdBuffer->pVkCmdBuffer);
}

void RHI_Cmd_End_CommandBuffer_Vulkan(PRHICommandBuffer* cmdBuffer)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)cmdBuffer->device;
	VulkanRHICommandBuffer* vk_cmdBuffer = (VulkanRHICommandBuffer*)cmdBuffer;

	VkResult res = vk_device->vkDeviceTable.vkEndCommandBuffer(vk_cmdBuffer->pVkCmdBuffer);
	RHI_ASSERT(res == VK_SUCCESS && "failed to record command buffer!");
}

PRHISemaphoreId RHI_Create_Semaphore_Vulkan(PRHIDevice* device)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)device;

	VkSemaphoreCreateInfo semaphore_info =
	{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = RHI_NULL,
		.flags = 0
	};

	VulkanRHISemaphore* new_semaphore = calloc(1, sizeof(VulkanRHISemaphore));
	
	VkResult res = vk_device->vkDeviceTable.vkCreateSemaphore(vk_device->pVkDevice, &semaphore_info, RHI_NULL, &new_semaphore->pVkSemaphore);
	RHI_ASSERT(res == VK_SUCCESS && "vulkan API : failed to create semaphore!");

	return &(new_semaphore->super);
}

void RHI_Free_Semaphore_Vulkan(PRHISemaphore* semaphore)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)semaphore->device;
	VulkanRHISemaphore* vk_semaphore = (VulkanRHISemaphore*)semaphore;

	vk_device->vkDeviceTable.vkDestroySemaphore(vk_device->pVkDevice, vk_semaphore->pVkSemaphore, RHI_NULL);

	free(vk_semaphore);
}

PRHIFenceId RHI_Create_Fence_Vulkan(PRHIDevice* device)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)device;

	VkFenceCreateInfo fc_info =
	{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = RHI_NULL,
		.flags = 0
	};

	VulkanRHIFence* new_fence = calloc(1, sizeof(VulkanRHIFence));

	VkResult res = vk_device->vkDeviceTable.vkCreateFence(vk_device->pVkDevice, &fc_info, RHI_NULL, &new_fence->pVkFence);
	RHI_ASSERT(res == VK_SUCCESS && "vulkan API : failed to create fence!");

	return &(new_fence->super);
}

void RHI_Free_Fence_Vulkan(PRHIFence* fence)
{
	VulkanRHIFence* vk_fence = (VulkanRHIFence*)fence;
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)fence->device;

	vk_device->vkDeviceTable.vkDestroyFence(vk_device->pVkDevice, vk_fence->pVkFence, RHI_NULL);

	free(vk_fence);
}

PRHIGraphicsPipelineId RHI_Create_GraphicsPipeline_Vulkan(PRHIDevice* device, const PRHIGraphicsPipelineDescriptor* descriptor)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)device;
	VulkanRHIRootSignature* vk_pipelineLayout = (VulkanRHIRootSignature*)descriptor->root_signature;

	/* shader stage*/
	VkSpecializationInfo* use_specialization_info = VK_NULL_HANDLE;
	VkPipelineShaderStageCreateInfo shader_stage_info[5] = { 0 };
	uint32_t shader_stage_count = 0;
	for (uint32_t i = 0; i < 5; i++)
	{
		shader_stage_info[shader_stage_count].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shader_stage_info[shader_stage_count].pNext = RHI_NULL;
		shader_stage_info[shader_stage_count].flags = 0;
		shader_stage_info[shader_stage_count].pSpecializationInfo = use_specialization_info;
		EShaderStage mask = (EShaderStage)(1 << i);
		switch (mask)
		{
			case PRHI_SHADER_STAGE_VERT: 
			{
				if (descriptor->vertex_shader)
				{
					shader_stage_info[shader_stage_count].pName = descriptor->vertex_shader->entry;
					shader_stage_info[shader_stage_count].stage = VK_SHADER_STAGE_VERTEX_BIT;
					shader_stage_info[shader_stage_count].module = ((VulkanRHIShaderLibrary*)descriptor->vertex_shader->library)->pVkShaderModule;
					shader_stage_count++;
				}
			}
			break;

			case PRHI_SHADER_STAGE_TESC:
			{
				if (descriptor->tesc_shader)
				{
					shader_stage_info[shader_stage_count].pName = descriptor->tesc_shader->entry;
					shader_stage_info[shader_stage_count].stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
					shader_stage_info[shader_stage_count].module = ((VulkanRHIShaderLibrary*)descriptor->tesc_shader->library)->pVkShaderModule;
					shader_stage_count++;
				}
			}
			break;

			case PRHI_SHADER_STAGE_TESE:
			{
				if (descriptor->tese_shader)
				{
					shader_stage_info[shader_stage_count].pName = descriptor->tese_shader->entry;
					shader_stage_info[shader_stage_count].stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
					shader_stage_info[shader_stage_count].module = ((VulkanRHIShaderLibrary*)descriptor->tese_shader->library)->pVkShaderModule;
					shader_stage_count++;
				}
			}
			break;

			case PRHI_SHADER_STAGE_GEOM:
			{
				if (descriptor->geom_shader)
				{
					shader_stage_info[shader_stage_count].pName = descriptor->geom_shader->entry;
					shader_stage_info[shader_stage_count].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
					shader_stage_info[shader_stage_count].module = ((VulkanRHIShaderLibrary*)descriptor->geom_shader->library)->pVkShaderModule;
					shader_stage_count++;
				}
			}
			break;

			case PRHI_SHADER_STAGE_FRAG:
			{
				if (descriptor->fragment_shader)
				{
					shader_stage_info[shader_stage_count].pName = descriptor->fragment_shader->entry;
					shader_stage_info[shader_stage_count].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
					shader_stage_info[shader_stage_count].module = ((VulkanRHIShaderLibrary*)descriptor->fragment_shader->library)->pVkShaderModule;
					shader_stage_count++;
				}
			}
			break;
			
		default:
			RHI_ASSERT(0 && "no support shader stage!");
			break;
		}
	}

	/* vertex input */
	VkPipelineVertexInputStateCreateInfo vert_input_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 0,
		.pVertexBindingDescriptions = RHI_NULL,
		.vertexAttributeDescriptionCount = 0,
		.pVertexAttributeDescriptions = RHI_NULL
	};

	/* IA stage */
	VkPrimitiveTopology use_prim_topology = VkUtil_TranslatePrimitiveTopologyToVk(descriptor->prim_topology);
	VkPipelineInputAssemblyStateCreateInfo IA_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = use_prim_topology,
		.primitiveRestartEnable = VK_FALSE
	};

	/* viewport state*/
	VkPipelineViewportStateCreateInfo viewport_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.pViewports = RHI_NULL,
		.viewportCount = 1,
		.pScissors = RHI_NULL,
		.scissorCount = 1,
		.pNext = RHI_NULL,
		.flags = 0
	};

	/* rasterizer */
	VkBool32 enable_depthClamp = VK_FALSE;
	VkPolygonMode use_polygonMode = VK_POLYGON_MODE_FILL;
	VkCullModeFlags use_cullMode =VK_CULL_MODE_BACK_BIT;
	VkFrontFace use_frontFace = VK_FRONT_FACE_CLOCKWISE;
	float use_depthBias = 0.0f;
	float use_depthSlopeFactor = 0.0f;
	if (descriptor->rasterizer_state)
	{
		enable_depthClamp = descriptor->rasterizer_state->enable_depth_clamp ? VK_TRUE : VK_FALSE;
		use_polygonMode = VkUtil_TranslatePolygonToVk(descriptor->rasterizer_state->fill_mode);
		use_cullMode = VkUtil_TranslateCullModeToVk(descriptor->rasterizer_state->cull_mode);
		use_frontFace = VkUtil_TranslateFrontFaceToVk(descriptor->rasterizer_state->front_face);
		use_depthBias = descriptor->rasterizer_state->depth_bias;
		use_depthSlopeFactor = descriptor->rasterizer_state->slope_scaled_depth_bias;
	}

	VkPipelineRasterizationStateCreateInfo rasterize_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = enable_depthClamp,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = use_polygonMode,
		.lineWidth = 1.0f,
		.cullMode = use_cullMode,
		.frontFace = use_frontFace,
		.depthBiasEnable = (use_depthBias != 0) ? VK_TRUE : VK_FALSE,
		.depthBiasConstantFactor = use_depthBias,
		.depthBiasClamp = 0.0f,
		.depthBiasSlopeFactor = use_depthSlopeFactor,
		.pNext = RHI_NULL,
		.flags = 0
	};

	/* multiple sample */
	VkPipelineMultisampleStateCreateInfo multiSample_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.sampleShadingEnable = VK_FALSE,
		.rasterizationSamples = VkUtil_TranslateSampleCountToVk(descriptor->sample_count),
		.minSampleShading = 1.0f,
		.pSampleMask = RHI_NULL,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE,
		.pNext = RHI_NULL,
		.flags = 0
	};

	/* depth stencil*/
	VkPipelineDepthStencilStateCreateInfo depth_stencil_info = { 0 };

	/* color blend */
	VkPipelineColorBlendAttachmentState colorBlendAttachments[PRHI_MAX_MRT_COUNT] = { 0 };
	int blendStateIndex = 0;
	const PRHIBlendStateDescriptor* blendStateDesc = descriptor->blend_state;
	for (uint32_t i = 0; i < PRHI_MAX_MRT_COUNT; i++)
	{
		VkBool32 enable_blend = (gs_vk_BlendConstantTranslator[blendStateDesc->src_factors[blendStateIndex]] != VK_BLEND_FACTOR_ONE) ||
			(gs_vk_BlendConstantTranslator[blendStateDesc->dst_factors[blendStateIndex]] != VK_BLEND_FACTOR_ZERO) ||
			(gs_vk_BlendConstantTranslator[blendStateDesc->src_alpha_factors[blendStateIndex]] != VK_BLEND_FACTOR_ONE) ||
			(gs_vk_BlendConstantTranslator[blendStateDesc->dst_alpha_factors[blendStateIndex]] != VK_BLEND_FACTOR_ZERO);

		colorBlendAttachments[i].blendEnable = enable_blend;
		colorBlendAttachments[i].colorWriteMask = VkUtil_TranslateColorWriteMaskToVk(blendStateDesc->masks[blendStateIndex]);
		colorBlendAttachments[i].colorBlendOp = gs_vk_BlendConstantTranslator[blendStateDesc->blend_modes[blendStateIndex]];
		colorBlendAttachments[i].alphaBlendOp= gs_vk_BlendConstantTranslator[blendStateDesc->blend_alpha_modes[blendStateIndex]];
		colorBlendAttachments[i].srcColorBlendFactor= gs_vk_BlendConstantTranslator[blendStateDesc->src_factors[blendStateIndex]];
		colorBlendAttachments[i].dstColorBlendFactor= gs_vk_BlendConstantTranslator[blendStateDesc->dst_factors[blendStateIndex]];
		colorBlendAttachments[i].srcAlphaBlendFactor= gs_vk_BlendConstantTranslator[blendStateDesc->src_alpha_factors[blendStateIndex]];
		colorBlendAttachments[i].dstAlphaBlendFactor= gs_vk_BlendConstantTranslator[blendStateDesc->dst_alpha_factors[blendStateIndex]];

		if (blendStateDesc->independent_blend)
			blendStateIndex++;
	}

	VkPipelineColorBlendStateCreateInfo colorBlend_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = descriptor->render_target_count,
		.pAttachments = colorBlendAttachments,
		.blendConstants[0] = 0.0f,
		.blendConstants[1] = 0.0f,
		.blendConstants[2] = 0.0f,
		.blendConstants[3] = 0.0f,
		.pNext = RHI_NULL,
		.flags = 0,
	};

	/* dynamic state */
	VkDynamicState dynamic_states[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamic_info =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = 3,
		.pDynamicStates = dynamic_states,
		.pNext = RHI_NULL,
		.flags = 0
	};

	/* graphics pipeline */
	VkGraphicsPipelineCreateInfo graphics_pipe_info =
	{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = shader_stage_count,
		.pStages = shader_stage_info,
		.pVertexInputState = &vert_input_info,
		.pInputAssemblyState = &IA_info,
		.pViewportState = &viewport_info,
		.pRasterizationState = &rasterize_info,
		.pMultisampleState = &multiSample_info,
		.pDepthStencilState = &depth_stencil_info,
		.pColorBlendState = &colorBlend_info,
		.pDynamicState = &dynamic_info,
		.layout = vk_pipelineLayout->pPipelineLayout,
		.renderPass = vk_device->pVkRenderPass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = 0,
		.pNext = RHI_NULL,
		.flags = 0
	};
	
	VulkanRHIGraphicsPipeline* new_graphicsPipeline = calloc(1, sizeof(VulkanRHIGraphicsPipeline));
	VkResult res2 = vk_device->vkDeviceTable.vkCreateGraphicsPipelines(vk_device->pVkDevice, RHI_NULL, 1, &graphics_pipe_info, RHI_NULL, &new_graphicsPipeline->pVkPipeline);
	RHI_ASSERT(res2 == VK_SUCCESS && "failed to create vulkan graphics pipeline!");

	return &(new_graphicsPipeline->super);
}

void RHI_Free_GraphicsPipeline_Vulkan(PRHIGraphicsPipeline* pipeline)
{
	VulkanRHIGraphicsPipeline* vk_pipeline = (VulkanRHIGraphicsPipeline*)pipeline;
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)pipeline->device;

	vk_device->vkDeviceTable.vkDestroyPipeline(vk_device->pVkDevice, vk_pipeline->pVkPipeline, RHI_NULL);

	free(vk_pipeline);
}


////////////////////////////////////different//////////////////////////////////////////

PRHIFrameBufferId RHI_Create_FrameBuffer_Vulkan(PRHIDevice* device, const PRHIFrameBufferDescriptor* descriptor)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)device;
	VulkanRHIRenderPass* vk_renderPass = (VulkanRHIRenderPass*)descriptor->render_pass;

	VkImageView* attachments = calloc(descriptor->attachment_count, sizeof(VkImageView));
	for (uint32_t i = 0; i < descriptor->attachment_count; i++)
	{
		VulkanRHITextureView* vk_attachment = (VulkanRHITextureView*)descriptor->attachments[i];
		if (vk_attachment->super.info.usages & PRHI_TVU_SRV)
			attachments[i] = vk_attachment->pVkSRVDescriptor;
		else 	if (vk_attachment->super.info.usages & PRHI_TVU_RTV_DSV)
			attachments[i] = vk_attachment->pVkRTVDSVDescriptor;
		else 	if (vk_attachment->super.info.usages & PRHI_TVU_UAV)
			attachments[i] = vk_attachment->pVkUAVDescriptor;
		else
			RHI_ASSERT(0);
	}

	VkFramebufferCreateInfo fb_info =
	{
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass = vk_renderPass->pVkRenderPass,
		.width = descriptor->width,
		.height = descriptor->height,
		.attachmentCount = descriptor->attachment_count,
		.pAttachments = attachments,
		.layers = 1,
		.pNext = RHI_NULL,
		.flags = 0
	};

	VulkanRHIFrameBuffer* new_frameBuffer = calloc(1, sizeof(VulkanRHIFrameBuffer));
	VkResult res = vk_device->vkDeviceTable.vkCreateFramebuffer(vk_device->pVkDevice, &fb_info, RHI_NULL, &new_frameBuffer->pVkFrameBuffer);
	RHI_ASSERT(res == VK_SUCCESS && "vulkan API : failed to create frameBuffer!");

	return &(new_frameBuffer->super);
}

void RHI_Free_FrameBuffer_Vulkan(PRHIFrameBuffer* frameBuffer)
{
	VulkanRHIFrameBuffer* vk_frameBuffer = (VulkanRHIFrameBuffer*)frameBuffer;
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)frameBuffer->device;

	vk_device->vkDeviceTable.vkDestroyFramebuffer(vk_device->pVkDevice, vk_frameBuffer->pVkFrameBuffer, RHI_NULL);

	free(vk_frameBuffer);
}

PRHIRenderPassId RHI_Create_RenderPass_Vulkan(PRHIDevice* device, const PRHIRenderPassDescriptor* descriptor)
{
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)device;

	VkAttachmentDescription* attachments = _alloca(sizeof(VkAttachmentDescription) * descriptor->attachment_count);
	for (uint32_t i = 0; i < descriptor->attachment_count; i++)
	{
		attachments[i].format = VkUtil_TranslateFormatToVk(descriptor->attachments_desc[i].format);
		attachments[i].samples = VkUtil_TranslateSampleCountToVk(descriptor->attachments_desc[i].sample_count);
		attachments[i].loadOp = VkUtil_TranslateRenderTargetLoadOpToVk(descriptor->attachments_desc[i].loadOp);
		attachments[i].storeOp = VkUtil_TranslateRenderTargetStoreOpToVk(descriptor->attachments_desc[i].storeOp);
		attachments[i].stencilLoadOp = VkUtil_TranslateRenderTargetLoadOpToVk(descriptor->attachments_desc[i].stencil_loadOp);
		attachments[i].stencilStoreOp = VkUtil_TranslateRenderTargetStoreOpToVk(descriptor->attachments_desc[i].stencil_storeOp);
		attachments[i].initialLayout = descriptor->attachments_desc[i].init_layout;
		attachments[i].finalLayout = descriptor->attachments_desc[i].final_layout;
		attachments[i].flags = 0;
	}

	VkSubpassDescription* subpasss = _alloca(sizeof(VkSubpassDescription) * descriptor->subpass_count);
	for (uint32_t i = 0; i < descriptor->subpass_count; i++)
	{
		VkAttachmentReference* color_attachmentRefs = RHI_NULL;
		if (descriptor->subpasss_desc[i].color_attachment_count)
		{
			color_attachmentRefs = _alloca(sizeof(VkAttachmentReference) * descriptor->subpasss_desc[i].color_attachment_count);
			for (uint32_t j = 0; j < descriptor->subpasss_desc[i].color_attachment_count; j++)
			{
				color_attachmentRefs[j].attachment = descriptor->subpasss_desc[i].color_attachments[j].attachment_desc_index;
				color_attachmentRefs[j].layout = descriptor->subpasss_desc[i].color_attachments[j].layout;
			}
		}

		VkAttachmentReference* input_attachmentRefs = RHI_NULL;
		if (descriptor->subpasss_desc[i].input_attachment_count)
		{
			input_attachmentRefs = _alloca(sizeof(VkAttachmentReference) * descriptor->subpasss_desc[i].input_attachment_count);
			for (uint32_t j = 0; j < descriptor->subpasss_desc[i].input_attachment_count; j++)
			{
				input_attachmentRefs[j].attachment = descriptor->subpasss_desc[i].input_attachments[j].attachment_desc_index;
				input_attachmentRefs[j].layout = descriptor->subpasss_desc[i].input_attachments[j].layout;
			}
		}

		uint32_t* preserve_attachmentRefs = RHI_NULL;
		if (descriptor->subpasss_desc[i].preserve_attachments)
		{
			preserve_attachmentRefs = _alloca(sizeof(uint32_t) * descriptor->subpasss_desc[i].preserve_attachment_count);
			for (uint32_t j = 0; j < descriptor->subpasss_desc[i].preserve_attachment_count; j++)
			{
				preserve_attachmentRefs[j] = descriptor->subpasss_desc[i].preserve_attachments[j];
			}
		}

		VkAttachmentReference* resolve_attachmentRefs = RHI_NULL;
		if (descriptor->subpasss_desc[i].resolve_attachments)
		{
			resolve_attachmentRefs = _alloca(sizeof(VkAttachmentReference));
			resolve_attachmentRefs->attachment = descriptor->subpasss_desc[i].resolve_attachments->attachment_desc_index;
			resolve_attachmentRefs->layout = descriptor->subpasss_desc[i].resolve_attachments->layout;
		}

		VkAttachmentReference* depthStencil_attachmentRefs = RHI_NULL;
		if (descriptor->subpasss_desc[i].depth_stencil_attachment)
		{
			depthStencil_attachmentRefs = _alloca(sizeof(VkAttachmentReference));
			depthStencil_attachmentRefs->attachment = descriptor->subpasss_desc[i].depth_stencil_attachment->attachment_desc_index;
			depthStencil_attachmentRefs->layout = descriptor->subpasss_desc[i].depth_stencil_attachment->layout;
		}

		subpasss[i].pipelineBindPoint = descriptor->subpasss_desc[i].pipeline_bindPoint;
		subpasss[i].colorAttachmentCount = descriptor->subpasss_desc[i].color_attachment_count;
		subpasss[i].pColorAttachments = color_attachmentRefs;
		subpasss[i].inputAttachmentCount = descriptor->subpasss_desc[i].input_attachment_count;
		subpasss[i].pInputAttachments = input_attachmentRefs;
		subpasss[i].preserveAttachmentCount = descriptor->subpasss_desc[i].preserve_attachment_count;
		subpasss[i].pPreserveAttachments = preserve_attachmentRefs;
		subpasss[i].pResolveAttachments = resolve_attachmentRefs;
		subpasss[i].pDepthStencilAttachment = depthStencil_attachmentRefs;
		subpasss[i].flags = 0;
	}
	
	VkSubpassDependency* dependencies = _alloca(sizeof(VkSubpassDependency) * descriptor->dependency_count);
	for (uint32_t i = 0; i < descriptor->dependency_count; i++)
	{
		dependencies[i].dependencyFlags = descriptor->dependencies_desc[i].dependencyFlags;
		dependencies[i].srcSubpass = descriptor->dependencies_desc[i].src_subpass;
		dependencies[i].dstSubpass = descriptor->dependencies_desc[i].dst_subpass;
		dependencies[i].srcStageMask = descriptor->dependencies_desc[i].srcStageMask;
		dependencies[i].dstStageMask = descriptor->dependencies_desc[i].dstStageMask;
		dependencies[i].srcAccessMask = descriptor->dependencies_desc[i].srcAccessMask;
		dependencies[i].dstAccessMask = descriptor->dependencies_desc[i].dstAccessMask;
	}

	VkRenderPassCreateInfo render_pass_info =
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = descriptor->attachment_count,
		.pAttachments = attachments,
		.dependencyCount = descriptor->dependency_count,
		.pDependencies = dependencies,
		.subpassCount = descriptor->subpass_count,
		.pSubpasses = subpasss,
		.pNext = RHI_NULL,
		.flags = 0
	};

	VulkanRHIRenderPass* new_renderPass = calloc(1, sizeof(VulkanRHIRenderPass));
	
	VkResult res = vk_device->vkDeviceTable.vkCreateRenderPass(vk_device->pVkDevice, &render_pass_info, RHI_NULL, &new_renderPass->pVkRenderPass);
	RHI_ASSERT(res == VK_SUCCESS && "failed to create vulkan render pass!");

	vk_device->pVkRenderPass = new_renderPass->pVkRenderPass;

	return &(new_renderPass->super);
}

void RHI_Free_RenderPass_Vulkan(PRHIRenderPass* renderPass)
{
	VulkanRHIRenderPass* vk_renderPass = (VulkanRHIRenderPass*)renderPass;
	VulkanRHIDevice* vk_device = (VulkanRHIDevice*)renderPass->device;

	vk_device->vkDeviceTable.vkDestroyRenderPass(vk_device->pVkDevice, vk_renderPass->pVkRenderPass, RHI_NULL);

	free(vk_renderPass);
}







