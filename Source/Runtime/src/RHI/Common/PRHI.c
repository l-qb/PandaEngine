#include "RHI/PRHI.h"

#ifdef RHI_USE_VULKAN
#include "RHI/Vulkan/VulkanRHI.h"
#endif

PRHIInstanceId RHICreateInstance(const struct PRHIInstanceDescriptor* descriptor)
{
	const PRHIProcTable* proc_table = RHI_NULL;
	const PRHISurfaceProcTable* surface_proc_table = RHI_NULL;

	if (descriptor->api_type == RENDER_API_TYPE_COUNT)
	{

	}
#ifdef RHI_USE_VULKAN
	else if (descriptor->api_type == RENDER_API_TYPE_VULKAN)
	{
		proc_table = GetProcTable_Vulkan();
		surface_proc_table = GetSurfaceProcTable_Vulkan();
	}
#endif
#ifdef RHI_USE_D3D12
	else if (descriptor->api_type == RENDER_API_TYPE_D3D)
	{

	}
#endif
#ifdef RHI_USE_METAL
	else if (descriptor->api_type == RENDER_API_TYPE_METAL)
	{

	}
#endif

	PRHIInstance* inst = proc_table->pfn_CreateInstance(descriptor);
	inst->proc_table_cache = proc_table;
	inst->surface_proc_table = surface_proc_table;

	return inst;
}

void RHIFreeInstance(PRHIInstanceId instance)
{
	instance->proc_table_cache->pfn_FreeInstance(instance);
}

void RHIEnumAdapter(PRHIInstanceId instance, PRHIAdapterId* const pAdapters, uint32_t* pAdapterCount)
{
	instance->proc_table_cache->pfn_EnumAdapter(instance, pAdapters, pAdapterCount);
	if (pAdapters != RHI_NULL)
	{
		for (uint32_t i = 0; i < *pAdapterCount; i++)
		{
			*(const PRHIProcTable**)&pAdapters[i]->proc_table_cache = instance->proc_table_cache;
			*(PRHIInstance**)&pAdapters[i]->instance = instance;
		}
	}
}

PRHIDeviceId RHICreateDevice(PRHIAdapterId adapter, const struct PRHIDeviceDescriptor* descriptor)
{
	PRHIDevice* device = adapter->proc_table_cache->pfn_CreateDevice(adapter, descriptor);
	device->proc_table_cache = adapter->proc_table_cache;

	return device;
}

void RHIFreeDevice(PRHIDeviceId device)
{
	device->proc_table_cache->pfn_FreeDevice(device);
}

PRHISwapchainId RHICreateSwapchain(PRHIDeviceId device, const struct PRHISwapchainDescriptor* descriptor)
{
	PRHISwapchain* swapchain = device->proc_table_cache->pfn_CreateSwapchain(device, descriptor);
	swapchain->device = device;

	return swapchain;
}

void RHIFreeSwapchain(PRHISwapchainId swapchain)
{
	swapchain->device->proc_table_cache->pfn_FreeSwapchain(swapchain);
}

uint32_t RHIAcquireNextImage(PRHISwapchainId swapchain, const struct PRHIAcquireNextImageDescriptor* descriptor)
{
	swapchain->device->proc_table_cache->pfn_AcquireNextImage(swapchain, descriptor);
}

PRHITextureViewId RHICreateTextureView(PRHIDeviceId device, const struct PRHITextureViewDescriptor* descriptor)
{
	PRHITextureView* textureView = device->proc_table_cache->pfn_CreateTextureView(device, descriptor);
	textureView->device = device;
	textureView->info = *descriptor;

	return textureView;
}

void RHIFreeTextureView(PRHITextureViewId textureView)
{
	textureView->device->proc_table_cache->pfn_FreeTextureView(textureView);
}

PRHIRootSignatureId RHICreateRootSignature(PRHIDeviceId device, const struct PRHIRootSignatureDescriptor* descriptor)
{
	PRHIRootSignature* rootSignature = device->proc_table_cache->pfn_CreateRootSignature(device, descriptor);
	rootSignature->device = device;
	
	return rootSignature;
}

void RHIFreeRootSignature(PRHIRootSignatureId rootSignature)
{
	rootSignature->device->proc_table_cache->pfn_FreeRootSignature(rootSignature);
}

PRHIQueueId RHIGetDeviceQueue(PRHIDeviceId device, EQueueType type, uint32_t index)
{
	PRHIQueue* queue = device->proc_table_cache->pfn_GetDeviceQueue(device, type, index);
	queue->device = device;
	queue->index = index;
	queue->type = type;

	return queue;
}

void RHIQueueSubmit(PRHIQueueId queue, const struct PRHIQueueSubmitDescriptor* descriptor)
{
	queue->device->proc_table_cache->pfn_queue_Submit(queue, descriptor);
}

void RHIQueuePresent(PRHIQueueId queue, const struct PRHIQueuePresentDescriptor* descriptor)
{
	queue->device->proc_table_cache->pfn_queue_Present(queue, descriptor);
}

void RHIQueueWaitIdle(PRHIQueueId queue)
{
	queue->device->proc_table_cache->pfn_queue_WaitIdle(queue);
}

PRHICommandPoolId RHICreateCommandPool(PRHIQueueId queue, const struct PRHICommandPoolDescriptor* descriptor)
{
	PRHICommandPool* cmdPool = queue->device->proc_table_cache->pfn_CreateCommandPool(queue, descriptor);
	cmdPool->queue = queue;

	return cmdPool;
}

void RHIFreeCommandPool(PRHICommandPoolId cmdPool)
{
	cmdPool->queue->device->proc_table_cache->pfn_FreeCommandPool(cmdPool);
}

PRHICommandBufferId RHICreateCommandBuffer(PRHICommandPoolId cmdPool, const struct PRHICommandBufferDescriptor* descriptor)
{
	PRHICommandBuffer* cmdBuffer = cmdPool->queue->device->proc_table_cache->pfn_CreateCommandBuffer(cmdPool, descriptor);
	cmdBuffer->device = cmdPool->queue->device;
	cmdBuffer->pool = cmdPool;

	return cmdBuffer;
}

void RHIFreeCommandBuffer(PRHICommandBufferId cmdBuffer)
{
	cmdBuffer->device->proc_table_cache->pfn_FreeCommandBuffer(cmdBuffer);
}

void RHICmdBeginCommandBuffer(PRHICommandBufferId cmdBuffer, ECommandBufferUsage usage)
{
	cmdBuffer->device->proc_table_cache->pfn_cmd_BeginCommandBuffer(cmdBuffer, usage);
}

void RHICmdBeginRenderPass(PRHICommandBufferId cmdBuffer, const struct PRHICmdRenderPassDescriptor* descriptor)
{
	cmdBuffer->device->proc_table_cache->pfn_cmd_BeginRenderPass(cmdBuffer, descriptor);
}

void RHICmdBindGraphicsPipeline(PRHICommandBufferId cmdBuffer, PRHIGraphicsPipelineId pipeline)
{
	cmdBuffer->device->proc_table_cache->pfn_cmd_BindGraphicsPipeline(cmdBuffer, pipeline);
}

void RHICmdSetViewport(PRHICommandBufferId cmdBuffer, float x, float y, float width, float height, float minDepth, float maxDepth)
{
	cmdBuffer->device->proc_table_cache->pfn_cmd_SetViewport(cmdBuffer, x, y, width, height, minDepth, maxDepth);
}

void RHICmdSetScissor(PRHICommandBufferId cmdBuffer, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	cmdBuffer->device->proc_table_cache->pfn_cmd_SetScissor(cmdBuffer, x, y, width, height);
}

void RHICmdDraw(PRHICommandBufferId cmdBuffer, uint32_t vertCount, uint32_t firstVert)
{
	cmdBuffer->device->proc_table_cache->pfn_cmd_Draw(cmdBuffer, vertCount, firstVert);
}

void RHICmdEndRenderPass(PRHICommandBufferId cmdBuffer)
{
	cmdBuffer->device->proc_table_cache->pfn_cmd_EndRenderPass(cmdBuffer);
}

void RHICmdEndCommandBuffer(PRHICommandBufferId cmdBuffer)
{
	cmdBuffer->device->proc_table_cache->pfn_cmd_EndCommandBuffer(cmdBuffer);
}

PRHISemaphoreId RHICreateSemaphore(PRHIDeviceId device)
{
	PRHISemaphore* semaphore = device->proc_table_cache->pfn_CreateSemaphore(device);
	semaphore->device = device;

	return semaphore;
}

void RHIFreeSemaphore(PRHISemaphoreId semaphore)
{
	semaphore->device->proc_table_cache->pfn_FreeSemaphore(semaphore);
}

PRHIFenceId RHICreateFence(PRHIDeviceId device)
{
	PRHIFence* fence = device->proc_table_cache->pfn_CreateFence(device);
	fence->device = device;
	
	return fence;
}

void RHIFreeFence(PRHIFenceId fence)
{
	fence->device->proc_table_cache->pfn_FreeFence(fence);
}

PRHIGraphicsPipelineId RHICreateGraphicsPipeline(PRHIDeviceId device, const struct PRHIGraphicsPipelineDescriptor* descriptor)
{
	PRHIGraphicsPipeline* graphicsPipeline = device->proc_table_cache->pfn_CreateGraphicsPipeline(device, descriptor);
	graphicsPipeline->device = device;
	graphicsPipeline->rootSignature = descriptor->root_signature;

	return graphicsPipeline;
}

void RHIFreeGraphicsPipeline(PRHIGraphicsPipelineId pipeline)
{
	pipeline->device->proc_table_cache->pfn_FreeGraphicsPipeline(pipeline);
}

#if defined(_WIN32) || defined(_WIN64)
PRHISurfaceId RHICreateSurface_Win32(PRHIDeviceId device, HWND hwnd)
{
	return device->adapter->instance->surface_proc_table->pfn_CreateSurface_Win32(device, hwnd);
}
#endif

void RHIFreeSurface(PRHIInstanceId instance, PRHISurfaceId surface)
{
	instance->surface_proc_table->pfn_FreeSurface(instance, surface);
}

PRHIShaderLibraryId RHICreateShaderLibrary(PRHIDeviceId device, const struct PRHIShaderLibraryDescriptor* descriptor)
{
	PRHIShaderLibrary* shaderLibrary = device->proc_table_cache->pfn_CreateShaderLibrary(device, descriptor);
	shaderLibrary->device = device;
	
	return shaderLibrary;
}

void RHIFreeShaderLibrary(PRHIShaderLibraryId shaderLibrary)
{
	shaderLibrary->device->proc_table_cache->pfn_FreeShaderLibrary(shaderLibrary);
}



///////////////////////////////////////Different///////////////////////////////////////////////
PRHIFrameBufferId RHICreateFrameBuffer(PRHIDeviceId device, const struct PRHIFrameBufferDescriptor* descriptor)
{
	PRHIFrameBuffer* frameBuffer = device->proc_table_cache->pfn_CreateFrameBuffer(device, descriptor);
	frameBuffer->device = device;

	return frameBuffer;
}

void RHIFreeFrameBuffer(PRHIFrameBufferId frameBuffer)
{
	frameBuffer->device->proc_table_cache->pfn_FreeFrameBuffer(frameBuffer);
}

PRHIRenderPassId RHICreateRenderPass(PRHIDeviceId device, const struct PRHIRenderPassDescriptor* descriptor)
{
	PRHIRenderPass* renderPass = device->proc_table_cache->pfn_CreateRenderPass(device, descriptor);
	renderPass->device = device;

	return renderPass;
}

void RHIFreeRenderPass(PRHIRenderPassId renderPass)
{
	renderPass->device->proc_table_cache->pfn_FreeRenderPass(renderPass);
}

