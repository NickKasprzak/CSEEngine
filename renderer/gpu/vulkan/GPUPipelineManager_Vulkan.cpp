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

CSECore::Ref<GPUPipeline> GPUPipelineManager_Vulkan::CreateGraphicsPipeline(const PipelineInfo& pipelineInfo, VkDescriptorSetLayout descSetLayout)
{
	CSE_ASSERT(pipelineInfo.shaderCount == 2, "Can only specify two shaders when creating a graphics pipeline.");

	uint32_t pipelineHash = CSECore::FNVHash(pipelineInfo);
	auto findResult = _pipelineHashToPipelineIndex.find(pipelineHash);
	auto hashMapEnd = _pipelineHashToPipelineIndex.end();
	if (findResult != hashMapEnd)
	{
		int32_t pipelineIndex = findResult->second;
		return CSECore::MakeNonOwningRef<GPUPipeline>(_pipelines.GetAtIndex(pipelineIndex));
	}

	GPUPipelineBuilder_Vulkan builder(_device);
	builder.SetGraphicsShaderInfo(pipelineInfo.shaders[0], pipelineInfo.shaders[1], descSetLayout);
	builder.SetViewportInfo(*pipelineInfo.viewportInfo);
	builder.SetRasterizationInfo(*pipelineInfo.rasterizationInfo);
	builder.SetMultisampleInfo(*pipelineInfo.multisampleInfo);
	builder.SetDepthStencilInfo(*pipelineInfo.depthStencilInfo);
	builder.SetColorBlendInfo(*pipelineInfo.colorBlendInfo);
	builder.SetAttachmentInfo(*pipelineInfo.attachmentInfo);
	builder.SetDynamicStateInfo(*pipelineInfo.dynamicStateInfo);

	CSECore::Expected<GPUPipelineBuilderResult_Vulkan, std::string> buildResult = builder.Build();
	if (buildResult.HasUnexpected())
	{
		CSE_LOGE("Failed to build new pipeline. Reason: " + buildResult.GetUnexpected());
		return CSECore::MakeEmptyRef<GPUPipeline>();
	}


	GPUPipelineBuilderResult_Vulkan result = buildResult.GetExpected();
	GPUPipeline_Vulkan pipeline(_device, result.pipeline, result.layout, result.layoutInfo, pipelineHash);
	int32_t pipelineIndex = _pipelines.Add(std::move(pipeline));
	_pipelineHashToPipelineIndex.insert(std::make_pair(pipelineHash, pipelineIndex));
	return CSECore::MakeNonOwningRef<GPUPipeline>(_pipelines.GetAtIndex(pipelineIndex));
}

}