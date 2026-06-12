#include "GPUPipelineBuilder_Vulkan.h"
#include "Utils_Vulkan.h"
#include "Vertex_Vulkan.h"

namespace CSERenderer
{

VkShaderStageFlags PipelineStageFlagsToVkShaderStageFlags(GPUPipelineStageFlags flags);
VkCullModeFlags CullModeToVkCullMode(PipelineRasterizationInfo::CullMode cullMode);
VkFrontFace FrontFaceToVkFrontFace(PipelineRasterizationInfo::FrontFace frontFace);
VkCompareOp CompareOpToVkCompareOp(CompareOp compareOp);
VkSampleCountFlags SampleCountToVkSampleCount(uint32_t sampleCount);
VkStencilOpState StencilOpStateToVkStencilOpState(PipelineDepthStencilInfo::StencilOpState stencilOpState);
VkStencilOp StencilOpToVkStencilOp(PipelineDepthStencilInfo::StencilOpState::StencilOp stencilOp);
VkBlendFactor BlendFactorToVkBlendFactor(PipelineColorBlendInfo::BlendFactor blendFactor);
VkBlendOp BlendOpToVkBlendOp(PipelineColorBlendInfo::BlendOp blendOp);
std::vector<VkFormat> ImageFormatsToVkFormats(ImageFormat* formats, uint32_t formatCount);
std::vector<VkDynamicState> DynamicStateFlagsToVkDynamicStates(PipelineDynamicStateInfo::DynamicStateFlags flags);
std::vector<VkPushConstantRange> CreatePushConstantRanges(std::vector<PushConstantLayout> layouts);

GPUPipelineBuilder_Vulkan::GPUPipelineBuilder_Vulkan(VkDevice device)
	: _device(device),
	_shaderInfo(),
	_layout(VK_NULL_HANDLE),
	_viewportInfo(),
	_rasterizationInfo(),
	_multisampleInfo(),
	_depthStencilInfo(),
	_colorBlendInfo(),
	_attachmentInfo(),
	_dynamicStateInfo(),
	_errorMessage()
{

}

GPUPipelineBuilder_Vulkan::~GPUPipelineBuilder_Vulkan()
{
	if (_shaderInfo[0].module != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(_device, _shaderInfo[0].module, nullptr);
	}

	if (_shaderInfo[1].module != VK_NULL_HANDLE)
	{
		vkDestroyShaderModule(_device, _shaderInfo[1].module, nullptr);
	}

	if (_layout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(_device, _layout, nullptr);
	}
}

void GPUPipelineBuilder_Vulkan::SetGraphicsShaderInfo(PipelineShaderInfo& vertexShader, PipelineShaderInfo& fragmentShader)
{
	if (_hasError())
	{
		return;
	}

	VkShaderModuleCreateInfo vertModuleCreateInfo{};
	vertModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(vertexShader.shaderCode.data());
	vertModuleCreateInfo.codeSize = vertexShader.shaderCode.size() / sizeof(uint32_t);

	VkShaderModule vertModule;
	VkResult vertModuleResult = vkCreateShaderModule(_device, &vertModuleCreateInfo, nullptr, &vertModule);
	if (vertModuleResult != VK_SUCCESS)
	{
		switch (vertModuleResult)
		{
		default:
			_errorMessage = std::string("Failed to create vertex shader module for an unknown reason.");
			return;
		}
	}
	_shaderInfo[0].module = vertModule;

	VkPipelineShaderStageCreateInfo vertCreateInfo{};
	vertCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertCreateInfo.module = _shaderInfo[0].module;
	vertCreateInfo.pName = "main";
	_shaderInfo[0].createInfo = vertCreateInfo;

	VkShaderModuleCreateInfo fragModuleCreateInfo{};
	vertModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(fragmentShader.shaderCode.data());
	vertModuleCreateInfo.codeSize = fragmentShader.shaderCode.size() / sizeof(uint32_t);

	VkShaderModule fragModule;
	VkResult fragModuleResult = vkCreateShaderModule(_device, &fragModuleCreateInfo, nullptr, &fragModule);
	if (fragModuleResult != VK_SUCCESS)
	{
		switch (fragModuleResult)
		{
		default:
			_errorMessage = std::string("Failed to create fragment shader module for an unknown reason.");
			return;
		}
	}
	_shaderInfo[1].module = fragModule;

	VkPipelineShaderStageCreateInfo fragCreateInfo{};
	vertCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	vertCreateInfo.module = _shaderInfo[1].module;
	vertCreateInfo.pName = "main";
	_shaderInfo[1].createInfo = fragCreateInfo;
}

void GPUPipelineBuilder_Vulkan::SetLayoutInfo(VkDescriptorSetLayout descSetLayout, const std::vector<PushConstantLayout>& pushConstantLayouts)
{
	std::vector<VkPushConstantRange> pushConstantRanges = CreatePushConstantRanges(pushConstantLayouts);

	VkPipelineLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	layoutInfo.setLayoutCount = 1;
	layoutInfo.pSetLayouts = &descSetLayout;
	layoutInfo.pushConstantRangeCount = pushConstantRanges.size();
	layoutInfo.pPushConstantRanges = pushConstantRanges.data();

	VkPipelineLayout layout;
	VkResult layoutResult = vkCreatePipelineLayout(_device, &layoutInfo, nullptr, &layout);
	if (layoutResult != VK_SUCCESS)
	{
		switch (layoutResult)
		{
		default:
			_errorMessage = std::string("Failed to create pipeline layout for an unknown reason.");
			return;
		}
	}

	_layout = layout;
}

void GPUPipelineBuilder_Vulkan::SetViewportInfo(PipelineViewportInfo& viewportInfo)
{
	if (_hasError())
	{
		return;
	}

	VkViewport viewport;
	viewport.x = viewportInfo.viewport.x;
	viewport.y = viewportInfo.viewport.y;
	viewport.width = viewportInfo.viewport.width;
	viewport.height = viewportInfo.viewport.height;
	viewport.minDepth = viewportInfo.viewport.minDepth;
	viewport.maxDepth = viewportInfo.viewport.maxDepth;
	_viewportInfo.viewport = viewport;

	VkPipelineViewportStateCreateInfo viewportCreateInfo{};
	viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportCreateInfo.viewportCount = 1;
	viewportCreateInfo.pViewports = &_viewportInfo.viewport;

	_viewportInfo.createInfo = viewportCreateInfo;
}

void GPUPipelineBuilder_Vulkan::SetRasterizationInfo(PipelineRasterizationInfo& rasterizationInfo)
{
	if (_hasError())
	{
		return;
	}

	VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo{};
	rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	
	VkCullModeFlags cullMode = CullModeToVkCullMode(rasterizationInfo.cullMode);
	if (cullMode == VK_CULL_MODE_FLAG_BITS_MAX_ENUM)
	{
		_errorMessage = std::string("Invalid cull mode.");
		return;
	}
	rasterizationCreateInfo.cullMode = cullMode;

	VkFrontFace frontFace = FrontFaceToVkFrontFace(rasterizationInfo.frontFace);
	if (frontFace & VK_FRONT_FACE_MAX_ENUM)
	{
		_errorMessage = std::string("Invalid front face definition.");
		return;
	}
	rasterizationCreateInfo.frontFace = frontFace;

	rasterizationCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizationCreateInfo.depthBiasConstantFactor = 0.0f;
	rasterizationCreateInfo.depthBiasClamp = 0.0f;
	rasterizationCreateInfo.depthBiasSlopeFactor = 0.0f;
	rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationCreateInfo.lineWidth = 1.0f;

	_rasterizationInfo = rasterizationCreateInfo;
}

void GPUPipelineBuilder_Vulkan::SetMultisampleInfo(PipelineMultisampleInfo& multisampleInfo)
{
	if (_hasError())
	{
		return;
	}

	VkPipelineMultisampleStateCreateInfo multisampleCreateInfo{};
	multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	
	VkSampleCountFlags sampleCount = SampleCountToVkSampleCount(multisampleInfo.sampleCount);
	if (sampleCount & VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM)
	{
		_errorMessage = std::string("Invalid sample count.");
		return;
	}

	multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleCreateInfo.minSampleShading = 1.0f;
	multisampleCreateInfo.alphaToOneEnable = VK_FALSE;
	multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleCreateInfo.pSampleMask = VK_NULL_HANDLE;

	_multisampleInfo = multisampleCreateInfo;
}

void GPUPipelineBuilder_Vulkan::SetDepthStencilInfo(PipelineDepthStencilInfo& depthStencilInfo)
{
	if (_hasError())
	{
		return;
	}

	VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
	depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

	depthStencilCreateInfo.depthTestEnable = (depthStencilInfo.depthTestEnabled) ? VK_TRUE : VK_FALSE;
	depthStencilCreateInfo.depthWriteEnable = (depthStencilInfo.depthWriteEnabled) ? VK_TRUE : VK_FALSE;
	depthStencilCreateInfo.depthCompareOp = CompareOpToVkCompareOp(depthStencilInfo.depthCompareOp);
	depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilCreateInfo.minDepthBounds = 0.0f;
	depthStencilCreateInfo.maxDepthBounds = 1.0f;

	depthStencilCreateInfo.stencilTestEnable = (depthStencilInfo.stencilTestEnabled) ? VK_TRUE : VK_FALSE;
	if (depthStencilInfo.stencilTestEnabled)
	{
		depthStencilCreateInfo.front = StencilOpStateToVkStencilOpState(depthStencilInfo.stencilFront);
		depthStencilCreateInfo.back = StencilOpStateToVkStencilOpState(depthStencilInfo.stencilBack);
	}

	_depthStencilInfo = depthStencilCreateInfo;
}

void GPUPipelineBuilder_Vulkan::SetColorBlendInfo(PipelineColorBlendInfo& colorBlendInfo)
{
	if (_hasError())
	{
		return;
	}

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.blendEnable = (colorBlendInfo.blendEnabled) ? VK_TRUE : VK_FALSE;
	if (colorBlendInfo.blendEnabled)
	{
		colorBlendAttachment.srcColorBlendFactor = BlendFactorToVkBlendFactor(colorBlendInfo.srcColorBlendFactor);
		colorBlendAttachment.dstColorBlendFactor = BlendFactorToVkBlendFactor(colorBlendInfo.dstColorBlendFactor);
		colorBlendAttachment.colorBlendOp = BlendOpToVkBlendOp(colorBlendInfo.colorBlendOp);

		colorBlendAttachment.srcAlphaBlendFactor = BlendFactorToVkBlendFactor(colorBlendInfo.srcAlphaBlendFactor);
		colorBlendAttachment.dstAlphaBlendFactor = BlendFactorToVkBlendFactor(colorBlendInfo.dstAlphaBlendFactor);
		colorBlendAttachment.alphaBlendOp = BlendOpToVkBlendOp(colorBlendInfo.alphaBlendOp);
	}
	_colorBlendInfo.blendAttachment = colorBlendAttachment;

	VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
	colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendCreateInfo.logicOp = VK_LOGIC_OP_NO_OP;
	colorBlendCreateInfo.blendConstants[0] = 0.0f;
	colorBlendCreateInfo.blendConstants[1] = 0.0f;
	colorBlendCreateInfo.blendConstants[2] = 0.0f;
	colorBlendCreateInfo.blendConstants[3] = 0.0f;
	colorBlendCreateInfo.attachmentCount = 1;
	colorBlendCreateInfo.pAttachments = &_colorBlendInfo.blendAttachment;
	_colorBlendInfo.createInfo = colorBlendCreateInfo;
}

void GPUPipelineBuilder_Vulkan::SetAttachmentInfo(PipelineAttachmentInfo& attachmentInfo)
{
	if (_hasError())
	{
		return;
	}

	_attachmentInfo.colorAttachmentFormats = ImageFormatsToVkFormats(attachmentInfo.colorAttachmentFormats, attachmentInfo.colorAttachmentCount);

	VkPipelineRenderingCreateInfo renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
	renderingInfo.colorAttachmentCount = _attachmentInfo.colorAttachmentFormats.size();
	renderingInfo.pColorAttachmentFormats = _attachmentInfo.colorAttachmentFormats.data();
	renderingInfo.depthAttachmentFormat = ImageFormatToVkFormat(attachmentInfo.depthAttachmentFormat);
	renderingInfo.stencilAttachmentFormat = ImageFormatToVkFormat(attachmentInfo.stencilAttachmentFormat);
	_attachmentInfo.createInfo = renderingInfo;
}

void GPUPipelineBuilder_Vulkan::SetDynamicStateInfo(PipelineDynamicStateInfo& dynamicStateInfo)
{
	if (_hasError())
	{
		return;
	}

	_dynamicStateInfo.dynamicStates = DynamicStateFlagsToVkDynamicStates(dynamicStateInfo.dynamicStateFlags);

	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
	dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateCreateInfo.dynamicStateCount = _dynamicStateInfo.dynamicStates.size();
	dynamicStateCreateInfo.pDynamicStates = _dynamicStateInfo.dynamicStates.data();
	_dynamicStateInfo.createInfo = dynamicStateCreateInfo;
}

CSECore::Expected<GPUPipelineBuilderResult_Vulkan, std::string> GPUPipelineBuilder_Vulkan::Build()
{
	if (_hasError())
	{
		return CSECore::CreateUnexpected<GPUPipelineBuilderResult_Vulkan, std::string>(_errorMessage);
	}

	VkPipelineShaderStageCreateInfo stages[2];
	stages[0] = _shaderInfo[0].createInfo;
	stages[1] = _shaderInfo[1].createInfo;

	VkVertexInputBindingDescription bindingDesc = GetVertexBindingDescription();
	std::vector<VkVertexInputAttributeDescription> attribDescs = GetVertexAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
	vertexInputInfo.vertexAttributeDescriptionCount = attribDescs.size();
	vertexInputInfo.pVertexAttributeDescriptions = attribDescs.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
	inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.stageCount = 2;
	pipelineCreateInfo.pStages = stages;
	pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
	pipelineCreateInfo.pViewportState = &_viewportInfo.createInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineCreateInfo.pRasterizationState = &_rasterizationInfo;
	pipelineCreateInfo.pMultisampleState = &_multisampleInfo;
	pipelineCreateInfo.pDepthStencilState = &_depthStencilInfo;
	pipelineCreateInfo.pColorBlendState = &_colorBlendInfo.createInfo;
	pipelineCreateInfo.pNext = &_attachmentInfo.createInfo;
	pipelineCreateInfo.pDynamicState = &_dynamicStateInfo.createInfo;
	pipelineCreateInfo.layout = _layout;

	VkPipeline pipeline = VK_NULL_HANDLE;
	VkResult result = vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, VK_NULL_HANDLE, &pipeline);
	if (result != VK_SUCCESS)
	{
		switch (result)
		{
		default:
			return CSECore::CreateUnexpected<GPUPipelineBuilderResult_Vulkan, std::string>("Failed to build graphics pipeline for an unknown reason.");
		}
	}

	GPUPipelineBuilderResult_Vulkan buildResult{};
	buildResult.pipeline = pipeline;
	buildResult.layout = _layout;

	vkDestroyShaderModule(_device, _shaderInfo[0].module, nullptr);
	_shaderInfo[0].module = VK_NULL_HANDLE;

	vkDestroyShaderModule(_device, _shaderInfo[1].module, nullptr);
	_shaderInfo[1].module = VK_NULL_HANDLE;

	return CSECore::CreateExpected<GPUPipelineBuilderResult_Vulkan, std::string>(buildResult);
}

bool GPUPipelineBuilder_Vulkan::_hasError()
{
	return !_errorMessage.empty();
}

VkShaderStageFlags PipelineStageFlagsToVkShaderStageFlags(GPUPipelineStageFlags flags)
{
	VkShaderStageFlags vkFlags = 0;

	if (flags | PIPELINE_STAGE_VERTEX)
	{
		vkFlags |= VK_SHADER_STAGE_VERTEX_BIT;
	}

	if (flags | PIPELINE_STAGE_FRAGMENT)
	{
		vkFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
	}

	return vkFlags;
}

VkCullModeFlags CullModeToVkCullMode(PipelineRasterizationInfo::CullMode cullMode)
{
	switch (cullMode)
	{
	case PipelineRasterizationInfo::CullMode::CULL_NONE:
		return VK_CULL_MODE_NONE;
	case PipelineRasterizationInfo::CullMode::CULL_FRONT_FACE:
		return VK_CULL_MODE_FRONT_BIT;
	case PipelineRasterizationInfo::CullMode::CULL_BACK_FACE:
		return VK_CULL_MODE_BACK_BIT;
	case PipelineRasterizationInfo::CullMode::CULL_BOTH:
		return VK_CULL_MODE_FRONT_AND_BACK;
	default:
		return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
	}
}

VkFrontFace FrontFaceToVkFrontFace(PipelineRasterizationInfo::FrontFace frontFace)
{
	switch (frontFace)
	{
	case PipelineRasterizationInfo::FrontFace::FRONT_FACE_CLOCKWISE:
		return VK_FRONT_FACE_CLOCKWISE;
	case PipelineRasterizationInfo::FrontFace::FRONT_FACE_COUNTER_CLOCKWISE:
		return VK_FRONT_FACE_COUNTER_CLOCKWISE;
	default:
		return VK_FRONT_FACE_MAX_ENUM;
	}
}

VkCompareOp CompareOpToVkCompareOp(CompareOp compareOp)
{
	switch (compareOp)
	{
	case COMPARE_OP_NEVER:
		return VK_COMPARE_OP_NEVER;
	case COMPARE_OP_LESS:
		return VK_COMPARE_OP_LESS;
	case COMPARE_OP_LESS_EQUAL:
		return VK_COMPARE_OP_LESS_OR_EQUAL;
	case COMPARE_OP_GREATER:
		return VK_COMPARE_OP_GREATER;
	case COMPARE_OP_GREATER_EQUAL:
		return VK_COMPARE_OP_GREATER_OR_EQUAL;
	case COMPARE_OP_ALWAYS:
		return VK_COMPARE_OP_ALWAYS;
	default:
		return VK_COMPARE_OP_MAX_ENUM;
	}
}

VkSampleCountFlags SampleCountToVkSampleCount(uint32_t sampleCount)
{
	switch (sampleCount)
	{
	case 1:
		return VK_SAMPLE_COUNT_1_BIT;
	case 2:
		return VK_SAMPLE_COUNT_2_BIT;
	case 4:
		return VK_SAMPLE_COUNT_4_BIT;
	case 8:
		return VK_SAMPLE_COUNT_8_BIT;
	case 16:
		return VK_SAMPLE_COUNT_16_BIT;
	case 32:
		return VK_SAMPLE_COUNT_32_BIT;
	case 64:
		return VK_SAMPLE_COUNT_64_BIT;
	default:
		return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
	}
}

VkStencilOpState StencilOpStateToVkStencilOpState(PipelineDepthStencilInfo::StencilOpState stencilOpState)
{
	VkStencilOpState vkStencilOpState;
	vkStencilOpState.passOp = StencilOpToVkStencilOp(stencilOpState.passOp);
	vkStencilOpState.failOp = StencilOpToVkStencilOp(stencilOpState.failOp);
	vkStencilOpState.depthFailOp = StencilOpToVkStencilOp(stencilOpState.depthFailOp);
	vkStencilOpState.compareOp = CompareOpToVkCompareOp(stencilOpState.compareOp);
	return vkStencilOpState;
}

VkStencilOp StencilOpToVkStencilOp(PipelineDepthStencilInfo::StencilOpState::StencilOp stencilOp)
{
	switch (stencilOp)
	{
	case PipelineDepthStencilInfo::StencilOpState::StencilOp::STENCIL_OP_KEEP:
		return VK_STENCIL_OP_KEEP;
	case PipelineDepthStencilInfo::StencilOpState::StencilOp::STENCIL_OP_ZERO:
		return VK_STENCIL_OP_ZERO;
	case PipelineDepthStencilInfo::StencilOpState::StencilOp::STENCIL_OP_REPLACE:
		return VK_STENCIL_OP_REPLACE;
	case PipelineDepthStencilInfo::StencilOpState::StencilOp::STENCIL_OP_INCREMENT:
		return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
	case PipelineDepthStencilInfo::StencilOpState::StencilOp::STENCIL_OP_DECREMENT:
		return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
	default:
		return VK_STENCIL_OP_MAX_ENUM;
	}
}

VkBlendFactor BlendFactorToVkBlendFactor(PipelineColorBlendInfo::BlendFactor blendFactor)
{
	switch (blendFactor)
	{
	case PipelineColorBlendInfo::BlendFactor::BLEND_FACTOR_ZERO:
		return VK_BLEND_FACTOR_ZERO;
	case PipelineColorBlendInfo::BlendFactor::BLEND_FACTOR_ONE:
		return VK_BLEND_FACTOR_ONE;

	case PipelineColorBlendInfo::BlendFactor::BLEND_FACTOR_SRC_COLOR:
		return VK_BLEND_FACTOR_SRC_COLOR;
	case PipelineColorBlendInfo::BlendFactor::BLEND_FACTOR_SRC_COLOR_INVERT:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
	case PipelineColorBlendInfo::BlendFactor::BLEND_FACTOR_DST_COLOR:
		return VK_BLEND_FACTOR_DST_COLOR;
	case PipelineColorBlendInfo::BlendFactor::BLEND_FACTOR_DST_COLOR_INVERT:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;

	case PipelineColorBlendInfo::BlendFactor::BLEND_FACTOR_SRC_ALPHA:
		return VK_BLEND_FACTOR_SRC_ALPHA;
	case PipelineColorBlendInfo::BlendFactor::BLEND_FACTOR_SRC_ALPHA_INVERT:
		return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	case PipelineColorBlendInfo::BlendFactor::BLEND_FACTOR_DST_ALPHA:
		return VK_BLEND_FACTOR_DST_ALPHA;
	case PipelineColorBlendInfo::BlendFactor::BLEND_FACTOR_DST_ALPHA_INVERT:
		return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;

	default:
		return VK_BLEND_FACTOR_MAX_ENUM;
	}
}

VkBlendOp BlendOpToVkBlendOp(PipelineColorBlendInfo::BlendOp blendOp)
{
	switch (blendOp)
	{
	case PipelineColorBlendInfo::BlendOp::BLEND_OP_ADD:
		return VK_BLEND_OP_ADD;
	case PipelineColorBlendInfo::BlendOp::BLEND_OP_SUBTRACT:
		return VK_BLEND_OP_SUBTRACT;
	default:
		return VK_BLEND_OP_MAX_ENUM;
	}
}

std::vector<VkFormat> ImageFormatsToVkFormats(ImageFormat* formats, uint32_t formatCount)
{
	std::vector<VkFormat> vkFormats;

	for (int i = 0; i < formatCount; i++)
	{
		vkFormats.push_back(ImageFormatToVkFormat(formats[i]));
	}

	return vkFormats;
}

std::vector<VkDynamicState> DynamicStateFlagsToVkDynamicStates(PipelineDynamicStateInfo::DynamicStateFlags flags)
{
	std::vector<VkDynamicState> dynamicState;

	if (flags | PipelineDynamicStateInfo::DynamicStateFlags::DYNAMIC_STATE_VIEWPORT)
	{
		dynamicState.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	}

	if (flags | PipelineDynamicStateInfo::DynamicStateFlags::DYNAMIC_STATE_SCISSOR)
	{
		dynamicState.push_back(VK_DYNAMIC_STATE_SCISSOR);
	}

	return dynamicState;
}

std::vector<VkPushConstantRange> CreatePushConstantRanges(std::vector<PushConstantLayout> layouts)
{
	std::vector<VkPushConstantRange> ranges;
	for (int i = 0; i < layouts.size(); i++)
	{
		VkPushConstantRange range;
		range.stageFlags = PipelineStageFlagsToVkShaderStageFlags(layouts[i].GetStageFlags());
		range.size = layouts[i].GetDataLayout().GetSize();
		range.offset = 0;
		ranges.push_back(range);
	}
	return ranges;
}

}