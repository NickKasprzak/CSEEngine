#pragma once
#include "../GPUPipeline_Vulkan.h"
#include "refcount/Ref.h"
#include "containers/SkipArray.h"
#include "volk.h"
#include <unordered_map>

namespace CSERenderer
{

class GPUPipelineRegistry_Vulkan
{
	struct GPUPipelineEntryDeleter;

	class GPUPipelineEntry : public GPUPipeline
	{
	public:
		GPUPipelineEntry();
		GPUPipelineEntry(GPUPipelineParams& pipelineParams, GPUPipelineRegistry_Vulkan* source);
		~GPUPipelineEntry();

		void operator=(GPUPipelineEntry&& entry) noexcept;

	private:
		friend struct GPUPipelineEntryDeleter;

		GPUPipelineRegistry_Vulkan* _source;
	};

	struct GPUPipelineEntryDeleter
	{
		void operator()(GPUPipeline* pipeline)
		{
			GPUPipelineEntry* entry = static_cast<GPUPipelineEntry*>(pipeline);
			entry->_source->RemoveGraphicsPipeline(pipeline);
		}
	};

public:
	GPUPipelineRegistry_Vulkan();
	~GPUPipelineRegistry_Vulkan();

	void Initialize(VkDevice device);
	void Dispose();

	CSECore::Ref<GPUPipeline> RegisterGraphicsPipeline(GPUPipelineParams& pipelineParams);
	CSECore::Ref<GPUPipeline> GetGraphicsPipeline(const PipelineInfo& pipelineInfo);
	void RemoveGraphicsPipeline(GPUPipeline* pipeline);

private:
	VkDevice _device;

	CSECore::SkipArray<GPUPipelineEntry, 64> _pipelines;
	std::unordered_map<uint32_t, int32_t> _pipelineHashToPipelineIndex;
};

}