#pragma once
#include "Any.h"
#include "refcount/Ref.h"

namespace CSERenderer
{

class GPUBuffer;
enum BufferUsageFlags;

class GPUImage;
enum ImageUsageFlags;
enum SamplerFilterMode;
enum SamplerAddressMode;

class GPUBackend
{
public:
	virtual void Initialize() = 0;
	virtual void Dispose() = 0;

	virtual void BeginFrame() = 0;
	virtual void DrawFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void PresentFrame() = 0;

	virtual CSECore::Ref<GPUBuffer> CreateBuffer(BufferUsageFlags usage, uint32_t size) = 0;
	virtual void BufferWrite(CSECore::Ref<GPUBuffer> buffer) = 0;
	virtual void BufferCopy(CSECore::Ref<GPUBuffer> buffer) = 0;

	virtual CSECore::Ref<GPUImage> CreateImage(ImageUsageFlags usage, uint32_t width, uint32_t height) = 0;
	virtual void SetImageSampler(CSECore::Ref<GPUImage> image, SamplerFilterMode filter, SamplerAddressMode addressMode) = 0;
	virtual void ImageCopy(CSECore::Ref<GPUImage> image) = 0;

	virtual void SetTargetWindow(const CSECore::Any<64>& windowInfo) = 0;
	virtual void UpdateWindowSurfaceSize(uint16_t width, uint16_t height) = 0;
};

}