#pragma once
#include "../GPUPipeline.h"
#include "../GPUDataLayout.h"
#include "../GPUDataLayoutRegistry.h"
#include "volk.h"

namespace CSERenderer
{

enum GPUPipelineStageFlags_Vulkan
{
	PIPELINE_STAGE_NULL = 0x0,
	PIPELINE_STAGE_VERTEX = 0x1,
	PIPELINE_STAGE_FRAGMENT = 0x2
};

class PushConstantLayout
{
public:
	PushConstantLayout();
	PushConstantLayout(const GPUDataLayout& layout, GPUPipelineStageFlags_Vulkan stage);
	PushConstantLayout(const PushConstantLayout& other);
	~PushConstantLayout();

	void operator=(const PushConstantLayout& other);

	const GPUDataLayout& GetDataLayout() const;
	GPUPipelineStageFlags_Vulkan GetStageFlags() const;

private:
	GPUDataLayout _layout;
	GPUPipelineStageFlags_Vulkan _stage;
};

class RenderAttachmentLayout
{
public:
	RenderAttachmentLayout();
	RenderAttachmentLayout(const std::vector<VkFormat>& colorAttachmentFormats, VkFormat depthAttachmentFormat, VkFormat stencilAttachmentFormat);
	RenderAttachmentLayout(const RenderAttachmentLayout& other);
	~RenderAttachmentLayout();

	void operator=(const RenderAttachmentLayout& other);

	const std::vector<VkFormat>& GetColorFormats() const;
	VkFormat GetDepthFormat() const;
	VkFormat GetStencilFormat() const;

private:
	std::vector<VkFormat> _colorAttachmentFormats;
	VkFormat _depthAttachmentFormat;
	VkFormat _stencilAttachmentFormat;
};

struct GPUPipelineParams_Vulkan
{
	VkDevice device;
	VkPipeline pipeline;
	VkPipelineLayout layout;
	std::vector<CSECore::Ref<GPUDataLayout>>* ssboLayouts;
	std::vector<PushConstantLayout>* pushConstantLayouts;
	RenderAttachmentLayout* renderAttachmentLayout;
	const PipelineInfo* pipelineInfo;
};

class GPUPipeline_Vulkan : public GPUPipeline
{
public:
	GPUPipeline_Vulkan();
	GPUPipeline_Vulkan(GPUPipelineParams_Vulkan& params);
	GPUPipeline_Vulkan(const GPUPipeline_Vulkan& other) = delete;
	GPUPipeline_Vulkan(GPUPipeline_Vulkan&& other) noexcept;
	virtual ~GPUPipeline_Vulkan();

	void operator=(const GPUPipeline_Vulkan& other) = delete;
	void operator=(GPUPipeline_Vulkan&& other) noexcept;

	VkPipeline GetPipeline() const;
	VkPipelineLayout GetPipelineLayout() const;
	const std::vector<CSECore::Ref<GPUDataLayout>>& GetSSBOLayouts() const;
	const std::vector<PushConstantLayout>& GetPushConstantLayouts() const;
	const RenderAttachmentLayout& GetRenderAttachmentLayout() const;
	uint32_t GetHashID() const;

private:
	VkDevice _device;
	VkPipeline _pipeline;
	VkPipelineLayout _layout;
	std::vector<CSECore::Ref<GPUDataLayout>> _ssboLayouts;
	std::vector<PushConstantLayout> _pushConstantLayouts;
	RenderAttachmentLayout _renderAttachmentLayout;
	uint32_t _hashID;
};

}