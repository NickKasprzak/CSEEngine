#pragma once
#include "../GPUBackend.h"
#include "GPUDevice_Vulkan.h"
#include "Compositor_Vulkan.h"
#include "DescriptorSetManager_Vulkan.h"
#include "GPUPipelineManager_Vulkan.h"

namespace CSERenderer
{

class GPUBackend_Vulkan : public GPUBackend
{
public:
	GPUBackend_Vulkan();
	virtual ~GPUBackend_Vulkan();

	virtual void Initialize() override;
	virtual void Dispose() override;

	virtual void BeginFrame() override {};
	virtual void DrawFrame() override {};
	virtual void EndFrame() override {};
	virtual void PresentFrame() override {};

	virtual CSECore::Ref<GPUBuffer> CreateBuffer(BufferUsageFlags usage, uint32_t size) override;
	virtual void BufferWrite(CSECore::Ref<GPUBuffer> buffer /* args */) override;
	virtual void BufferCopy(CSECore::Ref<GPUBuffer> buffer /* args */) override;

	virtual CSECore::Ref<GPUImage> CreateImage(ImageUsageFlags usage, ImageFormat format, uint32_t width, uint32_t height) override;
	virtual void SetImageSampler(CSECore::Ref<GPUImage> image, SamplerFilterMode filter, SamplerAddressMode addressMode) override;
	virtual void ImageCopy(CSECore::Ref<GPUImage> image /* args */) override;

	virtual CSECore::Ref<GPUPipeline> CreatePipeline(const PipelineInfo& pipelineInfo) override;
	// create pipeline parameter set? have it pass in the args to assign as a big list of key/values? have it do any of the buffer creation/writes/descriptor stuff needed? only allow for specification of non-static members.
	// given above, how do we handle array indexing? have array index be part of array data? do array indexing as dynamic value? what'd the difference be in indexing for material data and object data per draw?
	
	// bind pipeline?
	// bind parameter parameter set? have it check to ensure the pipeline and parameter set are compatible?
	// draw using the bound pipeline and parameter set?

	virtual void SetTargetWindow(const CSECore::Any<64>& windowInfo) override;
	virtual void UpdateWindowSurfaceSize(uint16_t width, uint16_t height) override;

private:
	GPUDevice_Vulkan _device;
	VkSurfaceKHR _windowSurface;
	vkb::Swapchain _swapchain;

	Compositor_Vulkan _compositor;
	DescriptorSetManager_Vulkan _descriptorSetManager;
	GPUPipelineManager_Vulkan _pipelineManager;
};

typedef GPUBackend_Vulkan GPUBackend_Impl;

}