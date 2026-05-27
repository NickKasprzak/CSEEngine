#include "GPUPipelineManager_Vulkan.h"
#include "GPUPipelineBuilder_Vulkan.h"
#include "DescriptorSetManager_Vulkan.h"
#include "Logger.h"

namespace CSERenderer
{

GPUPipelineManager_Vulkan::GPUPipelineManager_Vulkan()
	: _device(VK_NULL_HANDLE), _pipelines(), _pipelineHashToPipelineIndex()
{

}

GPUPipelineManager_Vulkan::~GPUPipelineManager_Vulkan()
{

}

void GPUPipelineManager_Vulkan::Initialize(VkDevice device)
{
	_device = device;
}

void GPUPipelineManager_Vulkan::Dispose()
{

}

CSECore::Ref<GPUPipeline> GPUPipelineManager_Vulkan::RegisterGraphicsPipeline(GPUPipeline_Vulkan& pipeline)
{
	auto findResult = _pipelineHashToPipelineIndex.find(pipeline.GetHashID());
	auto hashMapEnd = _pipelineHashToPipelineIndex.end();
	if (findResult != hashMapEnd)
	{
		int32_t pipelineIndex = findResult->second;
		return CSECore::MakeNonOwningRef<GPUPipeline>(_pipelines.GetAtIndex(pipelineIndex));
	}

	int32_t pipelineIndex = _pipelines.Add(std::move(pipeline));
	_pipelineHashToPipelineIndex.insert(std::make_pair(pipeline.GetHashID(), pipelineIndex));
	return CSECore::MakeNonOwningRef<GPUPipeline>(_pipelines.GetAtIndex(pipelineIndex));
}

CSECore::Ref<GPUPipeline> GPUPipelineManager_Vulkan::GetGraphicsPipeline(const PipelineInfo& pipelineInfo)
{
	uint32_t pipelineHash = CSECore::FNVHash(pipelineInfo);
	auto findResult = _pipelineHashToPipelineIndex.find(pipelineHash);
	auto hashMapEnd = _pipelineHashToPipelineIndex.end();
	if (findResult != hashMapEnd)
	{
		int32_t pipelineIndex = findResult->second;
		return CSECore::MakeNonOwningRef<GPUPipeline>(_pipelines.GetAtIndex(pipelineIndex));
	}

	return CSECore::MakeEmptyRef<GPUPipeline>();
}

}