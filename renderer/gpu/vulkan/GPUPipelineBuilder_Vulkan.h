#pragma once
#include "GPUPipeline_Vulkan.h"
#include "DescriptorSetManager_Vulkan.h"
#include "ShaderProcessor_Vulkan.h"
#include "refcount/Ref.h"
#include "Expected.h"

namespace CSERenderer
{

struct GPUPipelineBuilderResult_Vulkan
{
	VkPipeline pipeline;
	VkPipelineLayout layout;
	GPUPipelineLayoutInfo_Vulkan layoutInfo;
};

class GPUPipelineBuilder_Vulkan
{
	struct ShaderInfo
	{
		VkShaderModule module{};
		VkPipelineShaderStageCreateInfo createInfo{};
	};

	struct ViewportInfo
	{
		VkPipelineViewportStateCreateInfo createInfo{};
		VkViewport viewport{};
	};

	struct ColorBlendInfo
	{
		VkPipelineColorBlendStateCreateInfo createInfo{};
		VkPipelineColorBlendAttachmentState blendAttachment{};
	};

	struct AttachmentInfo
	{
		VkPipelineRenderingCreateInfo createInfo{};
		std::vector<VkFormat> colorAttachmentFormats;
	};

	struct DynamicStateInfo
	{
		VkPipelineDynamicStateCreateInfo createInfo{};
		std::vector<VkDynamicState> dynamicStates;
	};

public:
	GPUPipelineBuilder_Vulkan(VkDevice device);
	~GPUPipelineBuilder_Vulkan();

	void SetGraphicsShaderInfo(PipelineShaderInfo& vertexShader, PipelineShaderInfo& fragmentShader, VkDescriptorSetLayout descSetLayout);
	void SetViewportInfo(PipelineViewportInfo& viewportInfo);
	void SetRasterizationInfo(PipelineRasterizationInfo& rasterizationInfo);
	void SetMultisampleInfo(PipelineMultisampleInfo& multisampleInfo);
	void SetDepthStencilInfo(PipelineDepthStencilInfo& depthStencilInfo);
	void SetColorBlendInfo(PipelineColorBlendInfo& colorBlendInfo);
	void SetAttachmentInfo(PipelineAttachmentInfo& attachmentInfo);
	void SetDynamicStateInfo(PipelineDynamicStateInfo& dynamicStateInfo);

	CSECore::Expected<GPUPipelineBuilderResult_Vulkan, std::string> Build();

private:
	VkDevice _device;

	ShaderInfo _shaderInfo[2];
	ShaderLayoutInfo _layoutInfo{};
	VkPipelineLayout _layout;
	ViewportInfo _viewportInfo;
	VkPipelineRasterizationStateCreateInfo _rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo _multisampleInfo;
	VkPipelineDepthStencilStateCreateInfo _depthStencilInfo;
	ColorBlendInfo _colorBlendInfo;
	AttachmentInfo _attachmentInfo;
	DynamicStateInfo _dynamicStateInfo;

	std::string _errorMessage;

	CSECore::Expected<VkPipelineLayout, std::string> _createPipelineLayout(VkDescriptorSetLayout descSetLayout);
	bool _hasError();
};


}