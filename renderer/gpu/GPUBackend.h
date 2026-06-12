#pragma once
#include "GPUBufferCreateInfo.h"
#include "GPUImageCreateInfo.h"
#include "GPUPipelineCreateInfo.h"
#include "GPUDataLayout.h"
#include "Any.h"
#include "refcount/Ref.h"

namespace CSERenderer
{

class GPUBuffer;
class GPUImage;
class GPUPipeline;
class GPUPipelineData;
class GPUPipelineInputs;

class GPUBackend
{
public:
	template<typename BackendImpl>
	static void InitializeBackend();
	static void DisposeBackend();
	static GPUBackend* Instance();

	virtual void BeginFrame() = 0;
	virtual void DrawFrame() = 0;
	virtual void EndFrame() = 0;
	virtual void PresentFrame() = 0;

	virtual CSECore::Ref<GPUBuffer> CreateBuffer(const BufferCreateInfo& createInfo) = 0;
	virtual void BufferWrite(CSECore::Ref<GPUBuffer> buffer) = 0;
	virtual void BufferCopy(CSECore::Ref<GPUBuffer> buffer) = 0;

	virtual CSECore::Ref<GPUImage> CreateImage(const ImageCreateInfo& createInfo) = 0;
	virtual void ImageCopy(CSECore::Ref<GPUImage> image) = 0;

	virtual CSECore::Ref<GPUPipeline> CreateGraphicsPipeline(const PipelineInfo& pipelineInfo) = 0;
	virtual CSECore::Ref<GPUPipelineData> CreatePipelineData(CSECore::Ref<GPUDataLayout> dataLayout) = 0;

	virtual void BindPipeline() = 0;
	virtual void BindPipelineInputs() = 0;
	virtual void BindVertexBuffer() = 0;
	virtual void Draw() = 0;

	virtual void SetTargetWindow(const CSECore::Any<64>& windowInfo) = 0;
	virtual void UpdateWindowSurfaceSize(uint16_t width, uint16_t height) = 0;

protected:
	virtual void Initialize() = 0;
	virtual void Dispose() = 0;

private:
	static GPUBackend* _instance;
};

template<typename BackendImpl>
void GPUBackend::InitializeBackend()
{
	_instance = new BackendImpl();
	_instance->Initialize();
}

inline void GPUBackend::DisposeBackend()
{
	if (_instance != nullptr)
	{
		_instance->Dispose();
		_instance = nullptr;
	}
}

inline GPUBackend* GPUBackend::Instance()
{
	return _instance;
}

}