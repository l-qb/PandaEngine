#pragma once

#include "PRHI_config.h"
#include "PRHI_flags.h"

#define DECLARE_RHI_OBJECT(classname) struct classname##descriptor; typedef const struct classname* classname##Id;

DECLARE_RHI_OBJECT(PRHISurface)
DECLARE_RHI_OBJECT(PRHIInstance)
DECLARE_RHI_OBJECT(PRHIAdapter)
DECLARE_RHI_OBJECT(PRHIDevice)
DECLARE_RHI_OBJECT(PRHISwapchain)
DECLARE_RHI_OBJECT(PRHIQueue)
DECLARE_RHI_OBJECT(PRHICommandPool)
DECLARE_RHI_OBJECT(PRHICommandBuffer)
DECLARE_RHI_OBJECT(PRHISemaphore)
DECLARE_RHI_OBJECT(PRHIFence)
DECLARE_RHI_OBJECT(PRHITexture)
DECLARE_RHI_OBJECT(PRHITextureView)
DECLARE_RHI_OBJECT(PRHIShaderLibrary)
DECLARE_RHI_OBJECT(PRHIRootSignaturePool)
DECLARE_RHI_OBJECT(PRHIRootSignature)
DECLARE_RHI_OBJECT(PRHIGraphicsPipeline)
DECLARE_RHI_OBJECT(PRHISampler)

DECLARE_RHI_OBJECT(PRHIRenderPass)
DECLARE_RHI_OBJECT(PRHIFrameBuffer)

#define PRHI_MAX_VERTEX_ATTRIBS 16
#define PRHI_MAX_MRT_COUNT 16

#ifdef __cplusplus
extern "C" {
#endif

	typedef enum ERenderAPIType
	{
		RENDER_API_TYPE_VULKAN = 0,
		RENDER_API_TYPE_D3D = 1,
		RENDER_API_TYPE_METAL = 2,
		RENDER_API_TYPE_COUNT,
		RENDER_API_TYPE_MAX_BIT = 0x7FFFFFFF
	}ERenderAPIType;

	typedef enum EDeviceQueueType
	{
		DEVICE_QUEUE_TYPE_GRAPHICS = 0,
		DEVICE_QUEUE_TYPE_COMPUTE = 1,
		DEVICE_QUEUE_TYPE_TRANSFER = 2,
		DEVICE_QUEUE_TYPE_TILE_MAPPING = 3,
		DEVICE_QUEUE_TYPE_COUNT,
		DEVICE_QUEUE_TYPE_MAX_ENUM_BIT = 0x7FFFFFFF
	}EDeviceQueueType;

	/* additional desc*/
	typedef struct PRHIChainedDescriptor
	{
		ERenderAPIType api_type;
	}PRHIChainedDescriptor;

	/*API Object Descriptor*/
	typedef struct PRHIInstanceDescriptor
	{
		ERenderAPIType api_type;
		const PRHIChainedDescriptor* chained;
		bool enable_debug_layer;
		bool enable_gpu_based_validation;
		bool enable_set_name;
	}PRHIInstanceDescriptor;

	typedef struct PRHIQueueGroupDescriptor
	{
		EDeviceQueueType queue_type;
		uint32_t queue_count;
	}PRHIQueueGroupDescriptor;

	typedef struct PRHIDeviceDescriptor
	{
		bool disable_pipeline_cache;
		PRHIQueueGroupDescriptor* queue_group;
		uint32_t queue_group_count;
	}PRHIDeviceDescriptor;

	typedef struct PRHISwapchainDescriptor
	{
		PRHISurfaceId surface;

		EFormatType format_type;

		uint32_t width;
		uint32_t height;
		uint32_t image_count;

		bool enable_vsync;
		bool use_flip_swap_effect;

		float clear_color[4];
	}PRHISwapchainDescriptor;

	typedef struct PRHIAcquireNextImageDescriptor {
		PRHISemaphoreId signal_semaphore;
		PRHIFenceId fence;
	} PRHIAcquireNextImageDescriptor;

	typedef struct PRHITextureDescriptor 
	{
		const char* name;
		const void* native_handle;
		ETextureCreationFlags flags;
		//EClearValue clear_value;
		uint64_t width;
		uint64_t height;
		uint64_t depth;
		uint32_t array_size;
		EFormatType format;
		uint32_t mip_levels;
		ESampleCount sample_count;
		uint32_t sample_quality;
		PRHIQueueId owner_queue;
		EResourceState start_state;
		EResourceTypes descriptors;
		uint32_t is_restrict_dedicated;
	} PRHITextureDescriptor;

	typedef struct PRHITextureViewDescriptor 
	{
		const char* name;
		PRHITextureId texture;
		EFormatType format;
		ETextureViewUsages usages : 8;
		ETextureViewAspects aspects : 8;
		ETextureDimension dims : 8;
		uint32_t base_array_layer : 8;
		uint32_t array_layer_count : 8;
		uint32_t base_mip_level : 8;
		uint32_t mip_level_count : 8;
	} PRHITextureViewDescriptor;

	typedef struct PRHIFrameBufferDescriptor
	{
		const char*							name;
		PRHIRenderPassId				render_pass;
		uint32_t								attachment_count;
		const PRHITextureViewId*	attachments;
		uint32_t								width;
		uint32_t								height;
		uint32_t								layers;
	} PRHIFrameBufferDescriptor;

	typedef struct PRHIConstantSpecialization 
	{
		uint32_t constantID;
		union
		{
			uint64_t u;
			int64_t i;
			double f;
		};
	} PRHIConstantSpecialization;

	typedef struct PRHIVertexAttribute 
	{
		char semantic_name[64];
		uint32_t array_size;
		EFormatType format;
		uint32_t binding;
		uint32_t offset;
		uint32_t elem_stride;
		EVertexInputRate rate;
	} PRHIVertexAttribute;

	typedef struct PRHIVertexLayout 
	{
		uint32_t attribute_count;
		PRHIVertexAttribute attributes[PRHI_MAX_VERTEX_ATTRIBS];
	} PRHIVertexLayout;

	typedef struct PRHIBlendStateDescriptor 
	{
		EBlendConstant src_factors[PRHI_MAX_MRT_COUNT];
		EBlendConstant dst_factors[PRHI_MAX_MRT_COUNT];
		EBlendConstant src_alpha_factors[PRHI_MAX_MRT_COUNT];
		EBlendConstant dst_alpha_factors[PRHI_MAX_MRT_COUNT];
		EBlendMode blend_modes[PRHI_MAX_MRT_COUNT];
		EBlendMode blend_alpha_modes[PRHI_MAX_MRT_COUNT];
		EColorWriteMasks masks[PRHI_MAX_MRT_COUNT];
		bool alpha_to_coverage;
		bool independent_blend;
	} PRHIBlendStateDescriptor;

	typedef struct PRHIDepthStateDescriptor 
	{
		bool depth_test;
		bool depth_write;
		ECompareMode depth_func;
		bool stencil_test;
		uint8_t stencil_read_mask;
		uint8_t stencil_write_mask;
		ECompareMode stencil_front_func;
		EStencilOp stencil_front_fail;
		EStencilOp depth_front_fail;
		EStencilOp stencil_front_pass;
		ECompareMode stencil_back_func;
		EStencilOp stencil_back_fail;
		EStencilOp depth_back_fail;
		EStencilOp stencil_back_pass;
	} PRHIDepthStateDescriptor;

	typedef struct PRHIRasterizerStateDescriptor 
	{
		ECullMode cull_mode;
		int32_t depth_bias;
		float slope_scaled_depth_bias;
		EFillMode fill_mode;
		EFrontFace front_face;
		bool enable_multi_sample;
		bool enable_scissor;
		bool enable_depth_clamp;
	} PRHIRasterizerStateDescriptor;

	typedef struct PRHIShaderEntryDescriptor 
	{
		PRHIShaderLibraryId library;
		const char* entry;
		EShaderStage stage;
		const PRHIConstantSpecialization* constants;
		uint32_t num_constants;
	} PRHIShaderEntryDescriptor;

	typedef struct PRHIGraphicsPipelineDescriptor 
	{
		PRHIRootSignatureId root_signature;
		const PRHIShaderEntryDescriptor* vertex_shader;
		const PRHIShaderEntryDescriptor* tesc_shader;
		const PRHIShaderEntryDescriptor* tese_shader;
		const PRHIShaderEntryDescriptor* geom_shader;
		const PRHIShaderEntryDescriptor* fragment_shader;
		const PRHIVertexLayout* vertex_layout;
		const PRHIBlendStateDescriptor* blend_state;
		const PRHIDepthStateDescriptor* depth_state;
		const PRHIRasterizerStateDescriptor* rasterizer_state;
		const EFormatType* color_formats;
		uint32_t render_target_count;
		ESampleCount sample_count;
		uint32_t sample_quality;
		ESlotMask color_resolve_disable_mask;
		EFormatType depth_stencil_format;
		EPrimitiveTopology prim_topology;
		bool enable_indirect_command;
	} PRHIGraphicsPipelineDescriptor;

	typedef struct PRHIRootSignatureDescriptor 
	{
		struct PRHIShaderEntryDescriptor* shaders;
		uint32_t shader_count;
		const PRHISamplerId* static_samplers;
		const char* const* static_sampler_names;
		uint32_t static_sampler_count;
		const char* const* push_constant_names;
		uint32_t push_constant_count;
		PRHIRootSignaturePoolId pool;
	} PRHIRootSignatureDescriptor;

	typedef struct PRHIShaderLibraryDescriptor 
	{
		const char* name;
		const uint32_t* code;
		uint32_t code_size;
		EShaderStage stage;
		bool reflection_only;
	} PRHIShaderLibraryDescriptor;

	typedef struct PRHIQueueSubmitDescriptor {
		PRHICommandBufferId* cmds;
		PRHIFenceId signal_fence;
		PRHISemaphoreId* wait_semaphores;
		PRHISemaphoreId* signal_semaphores;
		uint32_t cmds_count;
		uint32_t wait_semaphore_count;
		uint32_t signal_semaphore_count;
	} PRHIQueueSubmitDescriptor;

	typedef struct PRHIQueuePresentDescriptor {
		PRHISwapchainId swapchain;
		const PRHISemaphoreId* wait_semaphores;
		uint32_t wait_semaphore_count;
		uint8_t index;
	} PRHIQueuePresentDescriptor;

	typedef struct PRHICommandPoolDescriptor 
	{
		const char* name;
	} PRHICommandPoolDescriptor;

	typedef struct PRHICommandBufferDescriptor {
		bool is_secondary : 1;
	} PRHICommandBufferDescriptor;

	typedef struct PRHIAttachmentDescription
	{
		EFormatType		format;
		ESampleCount	sample_count;
		ELoadAction		loadOp;
		EStoreAction		storeOp;
		ELoadAction		stencil_loadOp;
		EStoreAction		stencil_storeOp;
		EImageLayout		init_layout;
		EImageLayout		final_layout;
	}PRHIAttachmentDescription;

	typedef struct PRHIAttachmentReference
	{
		uint32_t				attachment_desc_index;
		EImageLayout		layout;
	}PRHIAttachmentReference;

	typedef struct PRHISubpassDescriptor
	{
		EPipelineBindPoint             pipeline_bindPoint;
		uint32_t								input_attachment_count;
		const PRHIAttachmentReference*	input_attachments;
		uint32_t								color_attachment_count;
		const PRHIAttachmentReference*	color_attachments;
		uint32_t                        preserve_attachment_count;
		const uint32_t* preserve_attachments;
		const PRHIAttachmentReference* resolve_attachments;
		const PRHIAttachmentReference* depth_stencil_attachment;
	}PRHISubpassDescriptor;

	typedef struct PRHISubpassDependencyDescriptor
	{
		uint32_t src_subpass;
		uint32_t dst_subpass;
		EPipelineStageFlags   srcStageMask;
		EPipelineStageFlags    dstStageMask;
		EAccessFlags           srcAccessMask;
		EAccessFlags           dstAccessMask;
		EDependencyFlags       dependencyFlags;
	} PRHISubpassDependencyDescriptor;

	typedef struct PRHIRenderPassDescriptor
	{
		uint32_t attachment_count;
		const PRHIAttachmentDescription* attachments_desc;

		uint32_t dependency_count;
		const PRHISubpassDependencyDescriptor* dependencies_desc;

		uint32_t subpass_count;
		const PRHISubpassDescriptor* subpasss_desc;
	}PRHIRenderPassDescriptor;

	typedef struct PRHICmdRenderPassDescriptor
	{
		PRHIRenderPassId render_pass;
		PRHIFrameBufferId frame_buffer;
		uint32_t width;
		uint32_t height;
	} PRHICmdRenderPassDescriptor;

	/*instance API */
	RHI_API PRHIInstanceId RHICreateInstance(const struct PRHIInstanceDescriptor* descriptor);
	typedef PRHIInstanceId(*PFN_RHICreateInstance)(const struct PRHIInstanceDescriptor* descriptor);
	RHI_API void RHIFreeInstance(PRHIInstanceId instance);
	typedef void(*PFN_RHIFreeInstance)(PRHIInstanceId instance);

	/* adapter API */
	RHI_API void RHIEnumAdapter(PRHIInstanceId instance, PRHIAdapterId*const pAdapters, uint32_t* pAdapterCount);
	typedef void(*PFN_RHIEnumAdapter)(PRHIInstanceId instance, PRHIAdapterId*const pAdapters, uint32_t* pAdapterCount);

	/* device API */
	RHI_API PRHIDeviceId RHICreateDevice(PRHIAdapterId adapter, const struct PRHIDeviceDescriptor* descriptor);
	typedef PRHIDeviceId(*PFN_RHICreateDevice)(PRHIAdapterId adapter, const struct PRHIDeviceDescriptor* descriptor);
	RHI_API void RHIFreeDevice(PRHIDeviceId device);
	typedef void(*PFN_RHIFreeDevice)(PRHIDeviceId device);

	/* swapchain API */
	RHI_API PRHISwapchainId RHICreateSwapchain(PRHIDeviceId device, const struct PRHISwapchainDescriptor* descriptor);
	typedef PRHISwapchainId(*PFN_RHICreateSwapchain)(PRHIDeviceId device, const struct PRHISwapchainDescriptor* descriptor);
	RHI_API void RHIFreeSwapchain(PRHISwapchainId swapchain);
	typedef void(*PFN_RHIFreeSwapchain)(PRHISwapchainId swapchain);
	RHI_API uint32_t RHIAcquireNextImage(PRHISwapchainId swapchain, const struct PRHIAcquireNextImageDescriptor* descriptor);
	typedef uint32_t(*PFN_RHIAcquireNextImage)(PRHISwapchainId swapchain, const struct PRHIAcquireNextImageDescriptor* descriptor);

	/* texture API*/
	//RHI_API PRHITextureId RHICreateTexture(PRHIDeviceId device, const struct PRHITextureDescriptor* descriptor);
	//typedef PRHITextureId(*PFN_RHICreateTexture)(PRHIDeviceId device, const struct PRHITextureDescriptor* descriptor);
	//RHI_API void RHIFreeTexture(PRHITextureId texture);
	//typedef void(*PFN_RHIFreeTexture)(PRHITextureId texture);
	RHI_API PRHITextureViewId RHICreateTextureView(PRHIDeviceId device, const struct PRHITextureViewDescriptor* descriptor);
	typedef PRHITextureViewId(*PFN_RHICreateTextureView)(PRHIDeviceId device, const struct PRHITextureViewDescriptor* descriptor);
	RHI_API void RHIFreeTextureView(PRHITextureViewId textureView);
	typedef void(*PFN_RHIFreeTextureView)(PRHITextureViewId textureView);

	/* rootSignature API */
	RHI_API PRHIRootSignatureId RHICreateRootSignature(PRHIDeviceId device, const struct PRHIRootSignatureDescriptor* descriptor);
	typedef PRHIRootSignatureId(*PFN_RHICreateRootSignature)(PRHIDeviceId device, const struct PRHIRootSignatureDescriptor* descriptor);
	RHI_API void RHIFreeRootSignature(PRHIRootSignatureId rootSignature);
	typedef void (*PFN_RHIFreeRootSignature)(PRHIRootSignatureId rootSignature);

	/* queue API*/
	RHI_API PRHIQueueId RHIGetDeviceQueue(PRHIDeviceId device, EQueueType type, uint32_t index);
	typedef PRHIQueueId(*PFN_RHIGetDeviceQueue)(PRHIDeviceId device, EQueueType type, uint32_t index);
	RHI_API void RHIQueueSubmit(PRHIQueueId queue, const struct PRHIQueueSubmitDescriptor* descriptor);
	typedef void (*PFN_RHIQueueSubmit)(PRHIQueueId queue, const struct PRHIQueueSubmitDescriptor* descriptor);
	RHI_API void RHIQueuePresent(PRHIQueueId queue, const struct PRHIQueuePresentDescriptor* descriptor);
	typedef void (*PFN_RHIQueuePresent)(PRHIQueueId queue, const struct PRHIQueuePresentDescriptor* descriptor);
	RHI_API void RHIQueueWaitIdle(PRHIQueueId queue);
	typedef void (*PFN_RHIQueueWaitIdle)(PRHIQueueId queue);

	/* command object API */
	RHI_API PRHICommandPoolId RHICreateCommandPool(PRHIQueueId queue, const struct PRHICommandPoolDescriptor* descriptor);
	typedef PRHICommandPoolId(*PFN_RHICreateCommandPool)(PRHIQueueId queue, const struct PRHICommandPoolDescriptor* descriptor);
	RHI_API void RHIFreeCommandPool(PRHICommandPoolId cmdPool);
	typedef void(*PFN_RHIFreeCommandPool)(PRHICommandPoolId cmdPool);
	RHI_API PRHICommandBufferId RHICreateCommandBuffer(PRHICommandPoolId cmdPool, const struct PRHICommandBufferDescriptor* descriptor);
	typedef PRHICommandBufferId(*PFN_RHICreateCommandBuffer)(PRHICommandPoolId cmdPool, const struct PRHICommandBufferDescriptor* descriptor);
	RHI_API void RHIFreeCommandBuffer(PRHICommandBufferId cmdBuffer);
	typedef void(*PFN_RHIFreeCommandBuffer)(PRHICommandBufferId cmdBuffer);
		
	/*commands API*/
	RHI_API void RHICmdBeginCommandBuffer(PRHICommandBufferId cmdBuffer, ECommandBufferUsage usage);
	typedef void(*PFN_RHICmdBeginCommandBuffer)(PRHICommandBufferId cmdBuffer, ECommandBufferUsage usage);

		RHI_API void RHICmdBeginRenderPass(PRHICommandBufferId cmdBuffer, const struct PRHICmdRenderPassDescriptor* descriptor);
		typedef void(*PFN_RHICmdBeginRenderPass)(PRHICommandBufferId cmdBuffer, const struct PRHICmdRenderPassDescriptor* descriptor);
			
			RHI_API void RHICmdBindGraphicsPipeline(PRHICommandBufferId cmdBuffer, PRHIGraphicsPipelineId pipeline);
			typedef void(*PFN_RHICmdBindGraphicsPipeline)(PRHICommandBufferId cmdBuffer, PRHIGraphicsPipelineId pipeline);
			RHI_API void RHICmdSetViewport(PRHICommandBufferId cmdBuffer, float x, float y, float width, float height, float minDepth, float maxDepth);
			typedef void(*PFN_RHICmdSetViewport)(PRHICommandBufferId cmdBuffer, float x, float y, float width, float height, float minDepth, float maxDepth);
			RHI_API void RHICmdSetScissor(PRHICommandBufferId cmdBuffer, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
			typedef void(*PFN_RHICmdSetScissor)(PRHICommandBufferId cmdBuffer, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
			RHI_API void RHICmdDraw(PRHICommandBufferId cmdBuffer, uint32_t vertCount, uint32_t firstVert);
			typedef void(*PFN_RHICmdDraw)(PRHICommandBufferId cmdBuffer, uint32_t vertCount, uint32_t firstVert);
		
		RHI_API void RHICmdEndRenderPass(PRHICommandBufferId cmdBuffer);
		typedef void(*PFN_RHICmdEndRenderPass)(PRHICommandBufferId cmdBuffer);
	
	RHI_API void RHICmdEndCommandBuffer(PRHICommandBufferId cmdBuffer);
	typedef void(*PFN_RHICmdEndCommandBuffer)(PRHICommandBufferId cmdBuffer);

	/* semaphore/fence API */
	RHI_API PRHISemaphoreId RHICreateSemaphore(PRHIDeviceId device);
	typedef PRHISemaphoreId(*PFN_RHICreateSemaphore)(PRHIDeviceId device);
	RHI_API void RHIFreeSemaphore(PRHISemaphoreId semaphore);
	typedef void(*PFN_RHIFreeSemaphore)(PRHISemaphoreId semaphore);
	RHI_API PRHIFenceId RHICreateFence(PRHIDeviceId device);
	typedef PRHIFenceId(*PFN_RHICreateFence)(PRHIDeviceId deivce);
	RHI_API void RHIFreeFence(PRHIFenceId fence);
	typedef void(*PFN_RHIFreeFence)(PRHIFenceId fence);

	/* other API */
	RHI_API PRHIGraphicsPipelineId RHICreateGraphicsPipeline(PRHIDeviceId device, const struct PRHIGraphicsPipelineDescriptor* descriptor);
	typedef PRHIGraphicsPipelineId (*PFN_RHICreateGraphicsPipeline)(PRHIDeviceId device, const struct PRHIGraphicsPipelineDescriptor* descriptor);
	RHI_API void RHIFreeGraphicsPipeline(PRHIGraphicsPipelineId pipeline);
	typedef void(*PFN_RHIFreeGraphicsPipeline)(PRHIGraphicsPipelineId pipeline);

	/* shader API*/
	RHI_API PRHIShaderLibraryId RHICreateShaderLibrary(PRHIDeviceId device, const struct PRHIShaderLibraryDescriptor* descriptor);
	typedef PRHIShaderLibraryId(*PFN_RHICreateShaderLibrary)(PRHIDeviceId device, const struct PRHIShaderLibraryDescriptor* descriptor);
	RHI_API void RHIFreeShaderLibrary(PRHIShaderLibraryId shaderLibrary);
	typedef void(*PFN_RHIFreeShaderLibrary)(PRHIShaderLibraryId shaderLibrary);

	/* different API */
	RHI_API PRHIFrameBufferId RHICreateFrameBuffer(PRHIDeviceId device, const struct PRHIFrameBufferDescriptor* descriptor);
	typedef PRHIFrameBufferId(*PFN_RHICreateFrameBuffer)(PRHIDeviceId device, const struct PRHIFrameBufferDescriptor* descriptor);
	RHI_API void RHIFreeFrameBuffer(PRHIFrameBufferId frameBuffer);
	typedef void(*PFN_RHIFreeFrameBuffer)(PRHIFrameBufferId frameBuffer);
	RHI_API PRHIRenderPassId RHICreateRenderPass(PRHIDeviceId device, const struct PRHIRenderPassDescriptor* descriptor);
	typedef PRHIRenderPassId(*PFN_RHICreateRenderPass)(PRHIDeviceId device, const struct PRHIRenderPassDescriptor* descriptor);
	RHI_API void RHIFreeRenderPass(PRHIRenderPassId renderPass);
	typedef void(*PFN_RHIFreeRenderPass)(PRHIRenderPassId renderPass);

	/*API Function Pointer List */
	typedef struct PRHIProcTable
	{
		/* instance API */
		const PFN_RHICreateInstance pfn_CreateInstance;
		const PFN_RHIFreeInstance pfn_FreeInstance;

		/* adapter API */
		const PFN_RHIEnumAdapter pfn_EnumAdapter;

		/* device API */
		const PFN_RHICreateDevice pfn_CreateDevice;
		const PFN_RHIFreeDevice pfn_FreeDevice;

		/* swapchain API */
		const PFN_RHICreateSwapchain pfn_CreateSwapchain;
		const PFN_RHIFreeSwapchain pfn_FreeSwapchain;
		const PFN_RHIAcquireNextImage pfn_AcquireNextImage;

		/* queue API */
		const PFN_RHIGetDeviceQueue pfn_GetDeviceQueue;
		const PFN_RHIQueueSubmit pfn_queue_Submit;
		const PFN_RHIQueuePresent pfn_queue_Present;
		const PFN_RHIQueueWaitIdle pfn_queue_WaitIdle;

		/* command object API */
		const PFN_RHICreateCommandPool pfn_CreateCommandPool;
		const PFN_RHIFreeCommandPool pfn_FreeCommandPool;
		const PFN_RHICreateCommandBuffer pfn_CreateCommandBuffer;
		const PFN_RHIFreeCommandBuffer pfn_FreeCommandBuffer;

		/* commands API */
		const PFN_RHICmdBeginCommandBuffer pfn_cmd_BeginCommandBuffer;
		const PFN_RHICmdBeginRenderPass pfn_cmd_BeginRenderPass;
		const PFN_RHICmdBindGraphicsPipeline pfn_cmd_BindGraphicsPipeline;
		const PFN_RHICmdSetViewport pfn_cmd_SetViewport;
		const PFN_RHICmdSetScissor pfn_cmd_SetScissor;
		const PFN_RHICmdDraw pfn_cmd_Draw;
		const PFN_RHICmdEndRenderPass pfn_cmd_EndRenderPass;
		const PFN_RHICmdEndCommandBuffer pfn_cmd_EndCommandBuffer;

		/* semaphore/fence API */
		const PFN_RHICreateSemaphore pfn_CreateSemaphore;
		const PFN_RHIFreeSemaphore pfn_FreeSemaphore;
		const PFN_RHICreateFence pfn_CreateFence;
		const PFN_RHIFreeFence pfn_FreeFence;

		/* texture API */
		//const PFN_RHICreateTexture pfn_CreateTexture;
		//const PFN_RHIFreeTexture pfn_FreeTexture;
		const PFN_RHICreateTextureView pfn_CreateTextureView;
		const PFN_RHIFreeTextureView pfn_FreeTextureView;

		/* rootSignature API*/
		const PFN_RHICreateRootSignature pfn_CreateRootSignature;
		const PFN_RHIFreeRootSignature pfn_FreeRootSignature;

		/* shaderAPI */
		const PFN_RHICreateShaderLibrary pfn_CreateShaderLibrary;
		const PFN_RHIFreeShaderLibrary pfn_FreeShaderLibrary;

		/* other API */
		const PFN_RHICreateGraphicsPipeline pfn_CreateGraphicsPipeline;
		const PFN_RHIFreeGraphicsPipeline pfn_FreeGraphicsPipeline;

		/*different API */
		const PFN_RHICreateFrameBuffer pfn_CreateFrameBuffer;
		const PFN_RHIFreeFrameBuffer pfn_FreeFrameBuffer;
		const PFN_RHICreateRenderPass pfn_CreateRenderPass;
		const PFN_RHIFreeRenderPass pfn_FreeRenderPass;

	}PRHIProcTable;

	/* surface API*/
	#if defined(_WIN32) || defined(_WIN64)
		typedef struct HWND__* HWND;
		RHI_API PRHISurfaceId RHICreateSurface_Win32(PRHIDeviceId device,HWND hwnd);
		typedef PRHISurfaceId(*PFN_RHICreateSurface_Win32)(PRHIDeviceId device, HWND hwnd);
	#endif
	RHI_API void RHIFreeSurface(PRHIInstanceId instance,PRHISurfaceId surface);
	typedef void(*PFN_RHIFreeSurface)(PRHIInstanceId instance,PRHISurfaceId surface);

	typedef struct PRHISurfaceProcTable
	{
		const PFN_RHIFreeSurface pfn_FreeSurface;

		#if defined(_WIN32) || defined(_WIN64)
				const PFN_RHICreateSurface_Win32 pfn_CreateSurface_Win32;
		#elif defined(__APPLE__)
				//...
		#endif
	}PRHISurfaceProcTable;

	/*API Object*/
	typedef struct PRHIInstance
	{
		ERenderAPIType api_type;
		const PRHIProcTable* proc_table_cache;
		const PRHISurfaceProcTable* surface_proc_table;
	}PRHIInstance;

	typedef struct PRHIAdapter
	{
		PRHIInstanceId instance;
		const PRHIProcTable* proc_table_cache;
	}PRHIAdapter;

	typedef struct PRHIDevice
	{
		PRHIAdapterId adapter;
		const PRHIProcTable* proc_table_cache;
	}PRHIDevice;

	typedef struct PRHISwapchain
	{
		PRHIDeviceId device;
		const PRHITextureId* chain_images;
		uint32_t chain_image_count;
	}PRHISwapchain;

	typedef struct PRHIQueue 
	{
		PRHIDeviceId device;
		EQueueType type;
		uint32_t index;
	} PRHIQueue;

	typedef struct PRHICommandPool 
	{
		PRHIQueueId queue;
	} PRHICommandPool;

	typedef struct PRHICommandBuffer {
		PRHIDeviceId device;
		PRHICommandPoolId pool;
		EPipelineType current_dispatch;
	} PRHICommandBuffer;

	typedef struct PRHISemaphore 
	{
		PRHIDeviceId device;
	} PRHISemaphore;

	typedef struct PRHIFence
	{
		PRHIDeviceId device;
	} PRHIFence;

	typedef struct PRHITextureInfo
	{
		uint64_t width;
		uint64_t height;
		uint64_t depth;
		EFormatType format;
	}PRHITextureInfo,PTextureInfo;

	typedef struct PRHITexture
	{
		PRHIDeviceId device;
		PRHITextureInfo* texture_info;

	}PRHITexture, PTexture;

	typedef struct PRHITextureView 
	{
		PRHIDeviceId device;
		PRHITextureViewDescriptor info;
	} PRHITextureView;

	typedef struct PRHIRenderPass
	{
		PRHIDeviceId device;
	} PRHIRenderPass;

	typedef struct PRHIFrameBuffer
	{
		PRHIDeviceId device;
	} PRHIFrameBuffer;

	typedef struct PRHIShaderResource
	{
		const char* name;
		uint64_t name_hash;
		EShaderResourceType type;
		ETextureDimension dim;
		uint32_t set;
		uint32_t binding;
		uint32_t size;
		uint32_t offset;
		EShaderStages stages;
	}PRHIShaderResource;

	typedef struct PRHIParameterTable 
	{
		PRHIShaderResource* resources;
		uint32_t resources_count;
		uint32_t set_index;
	} PRHIParameterTable;

	typedef struct PRHIRootSignaturePool 
	{
		PRHIDeviceId device;
		EPipelineType pipeline_type;
	} PRHIRootSignaturePool;

	typedef struct PRHIRootSignature 
	{
		PRHIDeviceId device;
		PRHIParameterTable* tables;
		uint32_t table_count;
		PRHIShaderResource* push_constants;
		uint32_t push_constant_count;
		PRHIShaderResource* static_samplers;
		uint32_t static_sampler_count;
		EPipelineType pipeline_type;
		PRHIRootSignaturePoolId pool;
		PRHIRootSignatureId pool_sig;
	} PRHIRootSignature;

	typedef struct PRHIGraphicsPipeline
	{
		PRHIDeviceId device;
		PRHIRootSignatureId rootSignature;
	}PRHIGraphicsPipeline;

	typedef struct PRHISampler
	{
		PRHIDeviceId device;
	}PRHISampler;


	/* shader API */
	typedef struct PRHIVertexInput
	{
		const char* name;
		const char* semantics;
		EFormatType format;
	}PRHIVertexInput;

	typedef struct PRHIShaderReflection
	{
		const char* entry_name;
		EShaderStage stage;
		PRHIVertexInput* vertex_inputs;
		PRHIShaderResource* shader_resources;
		uint32_t vertex_inputs_count;
		uint32_t shader_resources_count;
		uint32_t thread_group_sizes[3];
	}PRHIShaderReflection;

	typedef struct PRHIShaderLibrary
	{
		PRHIDeviceId device;
		char* name;
		PRHIShaderReflection* entry_reflections;
		uint32_t entry_count;
	}PRHIShaderLibrary;


#ifdef __cplusplus
}
#endif 
