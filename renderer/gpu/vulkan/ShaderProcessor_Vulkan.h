#pragma once
#include "GPUPipeline_Vulkan.h"
#include "DescriptorSetManager_Vulkan.h"
#include "Expected.h"
#include <string>
#include <vector>

namespace CSERenderer
{

struct ShaderLayoutInfo
{
	// add vertex input layout here
	DescriptorSetLayoutInfo descriptorSetLayoutInfo;
	std::vector<GPUPipelineLayoutInput_Vulkan> layoutInputs;
	std::vector<GPUPipelineLayoutPushConstant_Vulkan> layoutPushConstants;
};

CSECore::Expected<ShaderLayoutInfo, std::string> ProcessGraphicsShaderLayout(const std::string& vertexShaderCode, const std::string& fragmentShaderCode);

}