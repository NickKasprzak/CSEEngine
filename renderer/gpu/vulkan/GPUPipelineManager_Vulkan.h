#pragma once
#include "GPUPipeline_Vulkan.h"
#include "refcount/Ref.h"
#include "containers/SkipArray.h"
#include "volk.h"
#include <unordered_map>

namespace CSERenderer
{

class GPUPipelineManager_Vulkan
{
public:
	GPUPipelineManager_Vulkan();
	~GPUPipelineManager_Vulkan();

	void Initialize(VkDevice device);
	void Dispose();

	CSECore::Ref<GPUPipeline> RegisterGraphicsPipeline(GPUPipeline_Vulkan& pipeline);
	CSECore::Ref<GPUPipeline> GetGraphicsPipeline(const PipelineInfo& pipelineInfo);

private:
	VkDevice _device;

	CSECore::SkipArray<GPUPipeline_Vulkan, 64> _pipelines;
	std::unordered_map<uint32_t, int32_t> _pipelineHashToPipelineIndex;
};

}