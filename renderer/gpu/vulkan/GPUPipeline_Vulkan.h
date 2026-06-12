#pragma once
#include "refcount/RefCounted.h"
#include "../GPUPipelineCreateInfo.h"
#include "../GPUDataLayout.h"
#include "../GPUDataLayoutRegistry.h"
#include "volk.h"

namespace CSERenderer
{

enum GPUPipelineStageFlags
{
	PIPELINE_STAGE_NULL = 0x0,
	PIPELINE_STAGE_VERTEX = 0x1,
	PIPELINE_STAGE_FRAGMENT = 0x2
};

class PushConstantLayout
{
public:
	PushConstantLayout();
	PushConstantLayout(const GPUDataLayout& layout, GPUPipelineStageFlags stage);
	PushConstantLayout(const PushConstantLayout& other);
	~PushConstantLayout();

	void operator=(const PushConstantLayout& other);

	const GPUDataLayout& GetDataLayout() const;
	GPUPipelineStageFlags GetStageFlags() const;

private:
	GPUDataLayout _layout;
	GPUPipelineStageFlags _stage;
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

struct GPUPipelineParams
{
	VkDevice device;
	VkPipeline pipeline;
	VkPipelineLayout layout;
	std::vector<CSECore::Ref<GPUDataLayout>>* ssboLayouts;
	std::vector<PushConstantLayout>* pushConstantLayouts;
	RenderAttachmentLayout* renderAttachmentLayout;
	const PipelineInfo* pipelineInfo;
};

class GPUPipeline : public CSECore::RefCounted
{
public:
	GPUPipeline();
	GPUPipeline(GPUPipelineParams& params);
	GPUPipeline(const GPUPipeline& other) = delete;
	GPUPipeline(GPUPipeline&& other) noexcept;
	virtual ~GPUPipeline();

	void operator=(const GPUPipeline& other) = delete;
	void operator=(GPUPipeline&& other) noexcept;

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