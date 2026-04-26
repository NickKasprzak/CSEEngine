#include "GPUPipeline_Vulkan.h"
#include "Expected.h"

namespace CSERenderer
{
	
struct PipelineBuilderVulkanResult
{
	VkPipeline pipeline;
};

CSECore::Expected<PipelineBuilderVulkanResult, std::string> BuildPipeline(const PipelineInfo& pipelineInfo);

GPUPipeline_Vulkan::GPUPipeline_Vulkan()
{

}

GPUPipeline_Vulkan::GPUPipeline_Vulkan(const PipelineInfo& pipelineInfo)
{

}

GPUPipeline_Vulkan::~GPUPipeline_Vulkan()
{

}


CSECore::Expected<PipelineBuilderVulkanResult, std::string> BuildPipeline(const PipelineInfo& pipelineInfo)
{
	VkGraphicsPipelineCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	

}

}