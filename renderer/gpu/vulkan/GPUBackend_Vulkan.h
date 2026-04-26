#pragma once
#include "../GPUBackend.h"
#include "GPUDevice_Vulkan.h"
#include "Compositor_Vulkan.h"

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

	virtual CSECore::Ref<GPUImage> CreateImage(ImageUsageFlags usage, uint32_t width, uint32_t height) override;
	virtual void SetImageSampler(CSECore::Ref<GPUImage> image, SamplerFilterMode filter, SamplerAddressMode addressMode) override;
	virtual void ImageCopy(CSECore::Ref<GPUImage> image /* args */) override;

	virtual void SetTargetWindow(const CSECore::Any<64>& windowInfo) override;
	virtual void UpdateWindowSurfaceSize(uint16_t width, uint16_t height) override;

private:
	GPUDevice_Vulkan _device;
	VkSurfaceKHR _windowSurface;
	vkb::Swapchain _swapchain;

	Compositor_Vulkan _compositor;
};

typedef GPUBackend_Vulkan GPUBackend_Impl;

}