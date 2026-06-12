#include "GPUPipelineRegistry_Vulkan.h"
#include "GPUPipelineBuilder_Vulkan.h"
#include "DescriptorPool_Vulkan.h"
#include "Logger.h"

namespace CSERenderer
{

GPUPipelineRegistry_Vulkan::GPUPipelineRegistry_Vulkan()
	: _device(VK_NULL_HANDLE), _pipelines(), _pipelineHashToPipelineIndex()
{

}

GPUPipelineRegistry_Vulkan::~GPUPipelineRegistry_Vulkan()
{

}

void GPUPipelineRegistry_Vulkan::Initialize(VkDevice device)
{
	_device = device;
}

void GPUPipelineRegistry_Vulkan::Dispose()
{

}

CSECore::Ref<GPUPipeline> GPUPipelineRegistry_Vulkan::RegisterGraphicsPipeline(GPUPipelineParams& pipelineParams)
{
	uint32_t hash = CSECore::FNVHash(*pipelineParams.pipelineInfo);
	auto findResult = _pipelineHashToPipelineIndex.find(hash);
	auto hashMapEnd = _pipelineHashToPipelineIndex.end();
	CSE_ASSERT(findResult == hashMapEnd, "The pipeline registry should never recieve a duplicate pipeline.");

	int32_t pipelineIndex = _pipelines.Add(GPUPipelineEntry(pipelineParams, this));
	CSE_ASSERT(hash == _pipelines.GetAtIndex(pipelineIndex)->GetHashID(), "Pipeline registry hash mismatch has occurred.");
	_pipelineHashToPipelineIndex.insert(std::make_pair(hash, pipelineIndex));
	return CSECore::Ref<GPUPipeline>(_pipelines.GetAtIndex(pipelineIndex), GPUPipelineEntryDeleter());
}

CSECore::Ref<GPUPipeline> GPUPipelineRegistry_Vulkan::GetGraphicsPipeline(const PipelineInfo& pipelineInfo)
{
	uint32_t pipelineHash = CSECore::FNVHash(pipelineInfo);
	auto findResult = _pipelineHashToPipelineIndex.find(pipelineHash);
	auto hashMapEnd = _pipelineHashToPipelineIndex.end();
	if (findResult != hashMapEnd)
	{
		int32_t pipelineIndex = findResult->second;
		return CSECore::Ref<GPUPipeline>(_pipelines.GetAtIndex(pipelineIndex), GPUPipelineEntryDeleter());
	}

	return CSECore::Ref<GPUPipeline>();
}

void GPUPipelineRegistry_Vulkan::RemoveGraphicsPipeline(GPUPipeline* pipeline)
{
	GPUPipelineEntry* entry = static_cast<GPUPipelineEntry*>(pipeline);
	_pipelines.Remove(entry);
}

GPUPipelineRegistry_Vulkan::GPUPipelineEntry::GPUPipelineEntry()
	: GPUPipeline(), _source(nullptr)
{

}

GPUPipelineRegistry_Vulkan::GPUPipelineEntry::GPUPipelineEntry(GPUPipelineParams& pipelineParams, GPUPipelineRegistry_Vulkan* source)
	: GPUPipeline(pipelineParams), _source(source)
{

}

GPUPipelineRegistry_Vulkan::GPUPipelineEntry::~GPUPipelineEntry()
{

}

void GPUPipelineRegistry_Vulkan::GPUPipelineEntry::operator=(GPUPipelineEntry&& entry) noexcept
{

}

}