#pragma once

#include "RHI/PRHI.h"
#include "volk.h"

#ifdef __cplusplus
extern "C" {
#endif

	RHI_API const PRHIProcTable* GetProcTable_Vulkan();

	/* instance API */
	RHI_API PRHIInstanceId RHI_Create_Instance_Vulkan(PRHIInstanceDescriptor const* descriptor);
	RHI_API void RHI_Free_Instance_Vulkan(PRHIInstance* instance);

	/* adapter API */
	RHI_API PRHIAdapterId RHI_Enum_Adapter_Vulkan(PRHIInstanceId instance, PRHIAdapterId*const pAdapters, uint32_t* pAdapterCount);

	/*device API*/
	RHI_API PRHIDeviceId RHI_Create_Device_Vulkan(PRHIAdapter* adapter, PRHIDeviceDescriptor const* descriptor);
	RHI_API void RHI_Free_Device_Vulkan(PRHIDevice* device);

	/* swapchain API */
	RHI_API PRHISwapchainId RHI_Create_Swapchain_Vulkan(PRHIDevice* device, const PRHISwapchainDescriptor* descriptor);
	RHI_API void RHI_Free_Swapchain_Vulkan(PRHISwapchain* swapchain);
	RHI_API uint32_t RHI_Acquire_Next_Image_Vulkan(PRHISwapchain* swapchain, const struct PRHIAcquireNextImageDescriptor* descriptor);

	/* texture API */
	//RHI_API PRHITextureId RHI_Create_Texture_Vulkan(PRHIDevice* device, const PRHITextureDescriptor* descriptor);
	//RHI_API void RHI_Free_Texture_Vulkan(PRHITexture* texture);
	RHI_API PRHITextureViewId RHI_Create_TextureView_Vulkan(PRHIDevice* device, const PRHITextureViewDescriptor* descriptor);
	RHI_API void RHI_Free_TextureView_Vulkan(PRHITextureView* textureView);

	/* rootSignature API */
	RHI_API PRHIRootSignatureId RHI_Create_RootSignature_Vulkan(PRHIDevice* device, const  PRHIRootSignatureDescriptor* descriptor);
	RHI_API void RHI_Free_RootSignature_Vulkan(PRHIRootSignature* rootSignature);

	/* queue API */
	RHI_API PRHIQueueId RHI_Get_Device_Queue_Vulkan(PRHIDevice* device, EQueueType type, uint32_t index);
	RHI_API void RHI_Queue_Submit_Vulkan(PRHIQueue* queue, const struct PRHIQueueSubmitDescriptor* descriptor);
	RHI_API void RHI_Queue_Present_Vulkan(PRHIQueue* queue, const struct PRHIQueuePresentDescriptor* descriptor);
	RHI_API void RHI_Queue_WaitIdle_Vulkan(PRHIQueue* queue);

	/* command object API*/
	RHI_API PRHICommandPoolId RHI_Create_CommandPool_Vulkan(PRHIQueue* queue, const PRHICommandPoolDescriptor* descriptor);
	RHI_API void RHI_Free_CommandPool_Vulkan(PRHICommandPool* cmdPool);
	RHI_API PRHICommandBufferId RHI_Create_CommandBuffer_Vulkan(PRHICommandPool* cmdPool, const PRHICommandBufferDescriptor* descriptor);
	RHI_API void RHI_Free_CommandBuffer_Vulkan(PRHICommandBuffer* cmdBuffer);

	/* commands API */
	RHI_API void RHI_Cmd_Begin_CommandBuffer_Vulkan(PRHICommandBuffer* cmdBuffer, ECommandBufferUsage usage);
	RHI_API void RHI_Cmd_Begin_RenderPass_Vulkan(PRHICommandBuffer* cmdBuffer, const PRHICmdRenderPassDescriptor* descriptor);
	RHI_API void RHI_Cmd_Bind_Graphics_Pipeline_Vulkan(PRHICommandBuffer* cmdBuffer, PRHIGraphicsPipeline* pipeline);
	RHI_API void RHI_Cmd_Set_Viewport_Vulkan(PRHICommandBuffer* cmdBuffer, float x, float y, float width, float height, float minDepth, float maxDepth);
	RHI_API void RHI_Cmd_Set_Scissor_Vulkan(PRHICommandBuffer* cmdBuffer, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	RHI_API void RHI_Cmd_Draw_Vulkan(PRHICommandBuffer* cmdBuffer, uint32_t vertCount, uint32_t firstVert);
	RHI_API void RHI_Cmd_End_RenderPass_Vulkan(PRHICommandBuffer* cmdBuffer);
	RHI_API void RHI_Cmd_End_CommandBuffer_Vulkan(PRHICommandBuffer* cmdBuffer);

	/* semaphore/fence API */
	RHI_API PRHISemaphoreId RHI_Create_Semaphore_Vulkan(PRHIDevice* device);
	RHI_API void RHI_Free_Semaphore_Vulkan(PRHISemaphore* semaphore);
	RHI_API PRHIFenceId RHI_Create_Fence_Vulkan(PRHIDevice* device);
	RHI_API void RHI_Free_Fence_Vulkan(PRHIFence* fence);

/* other API */
	RHI_API PRHIGraphicsPipelineId RHI_Create_GraphicsPipeline_Vulkan(PRHIDevice* device, const PRHIGraphicsPipelineDescriptor* descriptor);
	RHI_API void RHI_Free_GraphicsPipeline_Vulkan(PRHIGraphicsPipeline* pipeline);

	/* different API */
	RHI_API PRHIFrameBufferId RHI_Create_FrameBuffer_Vulkan(PRHIDevice* device, const PRHIFrameBufferDescriptor* descriptor);
	RHI_API void RHI_Free_FrameBuffer_Vulkan(PRHIFrameBuffer* frameBuffer);
	RHI_API PRHIRenderPassId RHI_Create_RenderPass_Vulkan(PRHIDevice* device, const PRHIRenderPassDescriptor* descriptor);
	RHI_API void RHI_Free_RenderPass_Vulkan(PRHIRenderPass* renderPass);

	/**/
	typedef struct VulkanRHIInstance
	{
		PRHIInstance super;

		VkInstance pVkInstance;
		VkDebugUtilsMessengerEXT pVkDebugUtilsMessenger;
		VkDebugReportCallbackEXT pVkDebugReport;

		struct VulkanRHIAdapter* pVkAdapters;
		uint32_t vkAdapterCount;

		uint32_t layerCount;
		const char** ppLayerNames;
		VkLayerProperties* pLayerProps;

		uint32_t extensionCount;
		const char** ppExtensionNames;
		VkExtensionProperties* pExtensionProps;

		bool bDeviceGroupCreation : 1;
		bool bDebugUtils : 1;
		bool bDebugReport : 1;
	}VulkanRHIInstance;

	typedef struct VulkanRHIAdapter
	{
		PRHIAdapter super;

		VkPhysicalDevice pVkPhysicalDevice;
		VkPhysicalDeviceProperties2KHR vkPhysicalDeviceProps;
		VkPhysicalDeviceFeatures2KHR vkPhysicalDeviceFeatures;

		uint32_t queueFamiliesCount;
		int64_t queueFamilesIndices[DEVICE_QUEUE_TYPE_COUNT];
		VkQueueFamilyProperties* pVkQueueFamilyProps;

		const char** ppLayerNames;
		uint32_t layerCount;
		VkLayerProperties* pLayerProps;

		const char** ppExtensionNames;
		uint32_t extensionCount;
		VkExtensionProperties* pExtensionProps;

	}VulkanRHIAdapter;

	typedef struct VulkanRHIDevice
	{
		PRHIDevice super;

		VkDevice pVkDevice;
		VkRenderPass pVkRenderPass;

		struct VolkDeviceTable vkDeviceTable;
	}VulkanRHIDevice;

	typedef struct VulkanRHISwapchain
	{
		PRHISwapchain super;

		VkSurfaceKHR pVkSurface;
		VkSwapchainKHR pVkSwapchin;

	}VulkanRHISwapchain;

	typedef struct VulkanRHIQueue 
	{
		const PRHIQueue super;
		VkQueue pVkQueue;
		uint32_t queueFamilyIndex;
		PRHICommandPoolId pInnerCmdPool;
		PRHICommandBufferId pInnerCmdBuffer;
	} VulkanRHIQueue;

	typedef struct VulkanRHICommandPool 
	{
		PRHICommandPool super;
		VkCommandPool pVkCmdPool;
	} VulkanRHICommandPool;

	typedef struct VulkanRHICommandBuffer 
	{
		PRHICommandBuffer super;
		
		VkCommandBuffer pVkCmdBuffer;
		VkPipelineLayout pVkBoundPipelineLayout;
		VkRenderPass pVkRenderPass;
		
		uint32_t mNodeIndex;
		uint32_t mType;
	} VulkanRHICommandBuffer;

	typedef struct VulkanRHISemaphore
	{
		PRHISemaphore super;
		VkSemaphore pVkSemaphore;
		uint8_t signaled;
	} VulkanRHISemaphore;

	typedef struct VulkanRHIFence
	{
		PRHIFence super;
		VkFence pVkFence;
		uint32_t submitted;
	} VulkanRHIFence;

	typedef struct VulkanRHITexture
	{
		PRHITexture super;
		
		VkImage pVkImage;

	}VulkanRHITexture;

	typedef struct VulkanRHITextureView
	{
		PRHITextureView super;
		VkImageView pVkRTVDSVDescriptor;
		VkImageView pVkSRVDescriptor;
		VkImageView pVkUAVDescriptor;
	}VulkanRHITextureView;

	typedef struct VulkanRHIRenderPass
	{
		PRHIRenderPass super;

		VkRenderPass pVkRenderPass;

	}VulkanRHIRenderPass;

	typedef struct VulkanRHIFrameBuffer
	{
		PRHIFrameBuffer super;

		VkFramebuffer pVkFrameBuffer;

	}VulkanRHIFrameBuffer;

	typedef struct VulkanRHIGraphicsPipeline
	{
		PRHIGraphicsPipeline super;
		VkPipeline pVkPipeline;
	} VulkanRHIGraphicsPipeline;

	/* shader */
	RHI_API PRHIShaderLibraryId RHI_Create_ShaderLibrary_Vulkan(PRHIDevice* device, const  PRHIShaderLibraryDescriptor* descriptor);
	RHI_API void RHI_Free_ShaderLibrary_Vulkan(PRHIShaderLibrary* shaderLibrary);

	typedef struct VulkanRHISetLayout 
	{
		VkDescriptorSetLayout layout;
		VkDescriptorUpdateTemplate pUpdateTemplate;
		uint32_t updateEntriesCount;
		VkDescriptorSet pEmptyDescSet;
	} VulkanRHISetLayout;

	typedef struct VulkanRHIRootSignature
	{
		PRHIRootSignature super;

		VkPipelineLayout pPipelineLayout;
		VulkanRHISetLayout* pSetLayouts;
		VkDescriptorSetLayout* pVkSetLayouts;
		uint32_t setLayoutCount;
		VkPushConstantRange* pPushConstRanges;

	}VulkanRHIRootSignature;

	typedef struct VulkanRHIShaderLibrary
	{
		PRHIShaderLibrary super;
		
		VkShaderModule pVkShaderModule;
	}VulkanRHIShaderLibrary;

	
#ifdef __cplusplus
}
#endif
