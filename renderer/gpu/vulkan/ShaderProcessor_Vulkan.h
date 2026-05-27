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
	std::vector<VkVertexInputAttributeDescription> vertexAttributes;
	DescriptorSetLayoutInfo descriptorSetLayoutInfo;
	std::vector<GPUDataLayout> ssboLayouts;
	std::vector<PushConstantLayout> pushConstantLayouts;
};

CSECore::Expected<ShaderLayoutInfo, std::string> ProcessGraphicsShaderLayout(const std::string& vertexShaderCode, const std::string& fragmentShaderCode);

}