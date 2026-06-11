#pragma once
#include "../GPUBackend.h"
#include "GPUDevice_Vulkan.h"
#include "internal/RenderGraph_Vulkan.h"
#include "internal/DescriptorPool_Vulkan.h"
#include "internal/SSBOAllocator_Vulkan.h"
#include "internal/GPUPipelineRegistry_Vulkan.h"
#include "../GPUDataLayoutRegistry.h"

namespace CSERenderer
{

class GPUBackend_Vulkan : public GPUBackend
{
public:
	GPUBackend_Vulkan();
	virtual ~GPUBackend_Vulkan();

	static GPUBackend_Vulkan* Instance_Vulkan();

	virtual void Initialize() override;
	virtual void Dispose() override;

	virtual void BeginFrame() override {};
	virtual void DrawFrame() override {};
	virtual void EndFrame() override {};
	virtual void PresentFrame() override {};

	uint32_t GetCurrentFrame();

	virtual CSECore::Ref<GPUBuffer> CreateBuffer(const BufferCreateInfo& createInfo) override;
	virtual void BufferWrite(CSECore::Ref<GPUBuffer> buffer /* args */) override;
	virtual void BufferCopy(CSECore::Ref<GPUBuffer> buffer /* args */) override;

	virtual CSECore::Ref<GPUImage> CreateImage(const ImageCreateInfo& createInfo) override;
	virtual void ImageCopy(CSECore::Ref<GPUImage> image /* args */) override;

	virtual CSECore::Ref<GPUPipeline> CreateGraphicsPipeline(const PipelineInfo& pipelineInfo) override;
	virtual void CreateGraphicsPipelineInputs() override {};
	
	virtual void BindPipeline() override {};
	virtual void BindPipelineInputs() override {};
	virtual void BindVertexBuffer() override {};
	virtual void Draw() override {};

	CSECore::Ref<SSBO_Vulkan> CreateSSBO(size_t size);

	CSECore::Ref<SSBODescriptor> CreateSSBODescriptor(CSECore::Ref<SSBO_Vulkan> ssbo);
	CSECore::Ref<SamplerDescriptor> CreateSamplerDescriptor();

	virtual void SetTargetWindow(const CSECore::Any<64>& windowInfo) override;
	virtual void UpdateWindowSurfaceSize(uint16_t width, uint16_t height) override;

private:
	GPUDevice_Vulkan _device;
	VkSurfaceKHR _windowSurface;
	vkb::Swapchain _swapchain;
	uint32_t _currentFrame;

	RenderGraph_Vulkan _renderGraph;
	DescriptorPool_Vulkan _descriptorSetPool;
	SSBOAllocator_Vulkan _ssboAllocator;
	GPUPipelineRegistry_Vulkan _pipelineRegistry;
	GPUDataLayoutRegistry _dataLayoutRegistry;
};

typedef GPUBackend_Vulkan GPUBackend_Impl;

}