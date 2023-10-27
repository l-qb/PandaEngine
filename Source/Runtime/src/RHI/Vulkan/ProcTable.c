#include "RHI/Vulkan/VulkanRHI.h"

const PRHIProcTable g_vk_ProcTable =
{
	/* instance API */
	.pfn_CreateInstance = &RHI_Create_Instance_Vulkan,
	.pfn_FreeInstance = &RHI_Free_Instance_Vulkan,

	/* adapter API */
	.pfn_EnumAdapter = &RHI_Enum_Adapter_Vulkan,

	/* device API */
	.pfn_CreateDevice = &RHI_Create_Device_Vulkan,
	.pfn_FreeDevice = &RHI_Free_Device_Vulkan,

	/* other object API*/
	.pfn_CreateGraphicsPipeline = &RHI_Create_GraphicsPipeline_Vulkan,
	.pfn_FreeGraphicsPipeline = &RHI_Free_GraphicsPipeline_Vulkan,

	/* queue API */
	.pfn_GetDeviceQueue = &RHI_Get_Device_Queue_Vulkan,
	.pfn_queue_Submit = &RHI_Queue_Submit_Vulkan,
	.pfn_queue_Present = &RHI_Queue_Present_Vulkan,
	.pfn_queue_WaitIdle = &RHI_Queue_WaitIdle_Vulkan,

	/* command object API */
	.pfn_CreateCommandPool = &RHI_Create_CommandPool_Vulkan,
	.pfn_FreeCommandPool = &RHI_Free_CommandPool_Vulkan,
	.pfn_CreateCommandBuffer = &RHI_Create_CommandBuffer_Vulkan,
	.pfn_FreeCommandBuffer = &RHI_Free_CommandBuffer_Vulkan,

	/* commands API */
	.pfn_cmd_BeginCommandBuffer = &RHI_Cmd_Begin_CommandBuffer_Vulkan,
	.pfn_cmd_BeginRenderPass = &RHI_Cmd_Begin_RenderPass_Vulkan,
	.pfn_cmd_BindGraphicsPipeline = &RHI_Cmd_Bind_Graphics_Pipeline_Vulkan,
	.pfn_cmd_SetViewport = &RHI_Cmd_Set_Viewport_Vulkan,
	.pfn_cmd_SetScissor = &RHI_Cmd_Set_Scissor_Vulkan,
	.pfn_cmd_Draw = &RHI_Cmd_Draw_Vulkan,
	.pfn_cmd_EndRenderPass = &RHI_Cmd_End_RenderPass_Vulkan,
	.pfn_cmd_EndCommandBuffer = &RHI_Cmd_End_CommandBuffer_Vulkan,

	/* semaphore/fence API */
	.pfn_CreateSemaphore = &RHI_Create_Semaphore_Vulkan,
	.pfn_FreeSemaphore = &RHI_Free_Semaphore_Vulkan,
	.pfn_CreateFence = &RHI_Create_Fence_Vulkan,
	.pfn_FreeFence = &RHI_Free_Fence_Vulkan,

	/* rootSignature API*/
	.pfn_CreateRootSignature = &RHI_Create_RootSignature_Vulkan,
	.pfn_FreeRootSignature = &RHI_Free_RootSignature_Vulkan,

	/* shader API */
	.pfn_CreateShaderLibrary = &RHI_Create_ShaderLibrary_Vulkan,
	.pfn_FreeShaderLibrary = &RHI_Free_ShaderLibrary_Vulkan,

	/* buffer API */

	/* texture/textureView API */
	//.pfn_CreateTexture = &RHI_Create_Texture_Vulkan,
	//.pfn_FreeTexture = &RHI_Free_Texture_Vulkan,
	.pfn_CreateTextureView = &RHI_Create_TextureView_Vulkan,
	.pfn_FreeTextureView = &RHI_Free_TextureView_Vulkan,

	/* shared resource API */

	/* sampler API */

	/* swapchain API */
	.pfn_CreateSwapchain = &RHI_Create_Swapchain_Vulkan,
	.pfn_FreeSwapchain = &RHI_Free_Swapchain_Vulkan,
	.pfn_AcquireNextImage = &RHI_Acquire_Next_Image_Vulkan,


	/* events */

	/* compute CMDs */

	/* render CMDs */

	/* compiled/linked ISA API */

	/* statebuffer API */

	/* raster state encoder API */

	/* shader state encoder API */

	/* user ctx encoder API */

	/* binder API */

	/*different API */
	.pfn_CreateFrameBuffer = &RHI_Create_FrameBuffer_Vulkan,
	.pfn_FreeFrameBuffer = &RHI_Free_FrameBuffer_Vulkan,
	.pfn_CreateRenderPass = &RHI_Create_RenderPass_Vulkan,
	.pfn_FreeRenderPass = &RHI_Free_RenderPass_Vulkan
};
const PRHIProcTable* GetProcTable_Vulkan() { return &g_vk_ProcTable; }