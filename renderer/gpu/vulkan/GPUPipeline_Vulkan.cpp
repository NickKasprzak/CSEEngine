#include "GPUPipeline_Vulkan.h"
#include "Expected.h"
#include "Logger.h"

namespace CSERenderer
{
	
GPUPipeline_Vulkan::GPUPipeline_Vulkan()
	: _device(VK_NULL_HANDLE),
	_pipeline(VK_NULL_HANDLE),
	_layout(VK_NULL_HANDLE),
	_ssboLayouts(),
	_pushConstantLayouts(),
	_renderAttachmentLayout(),
	_hashID(UINT32_MAX)
{

}

GPUPipeline_Vulkan::GPUPipeline_Vulkan(VkDevice device,
	VkPipeline pipeline,
	VkPipelineLayout layout,
	const std::vector<CSECore::Ref<GPUDataLayoutRef>>& ssboLayouts,
	const std::vector<PushConstantLayout>& pushConstantLayouts,
	const RenderAttachmentLayout& renderAttachmentLayout,
	const PipelineInfo& pipelineInfo)
	: _device(device), 
	_pipeline(pipeline),
	_layout(layout),
	_ssboLayouts(ssboLayouts),
	_pushConstantLayouts(pushConstantLayouts),
	_renderAttachmentLayout(renderAttachmentLayout),
	_hashID(UINT32_MAX)
{
	_hashID = CSECore::FNVHash(pipelineInfo);
}

GPUPipeline_Vulkan::GPUPipeline_Vulkan(GPUPipeline_Vulkan&& other) noexcept
	: _device(VK_NULL_HANDLE),
	_pipeline(VK_NULL_HANDLE),
	_layout(VK_NULL_HANDLE),
	_ssboLayouts(),
	_pushConstantLayouts(),
	_renderAttachmentLayout(),
	_hashID(UINT32_MAX)
{
	_device = other._device;
	_pipeline = other._pipeline;
	_layout = other._layout;
	_ssboLayouts = other._ssboLayouts;
	_pushConstantLayouts = other._pushConstantLayouts;
	_renderAttachmentLayout = other._renderAttachmentLayout;
	_hashID = other._hashID;

	other._device = VK_NULL_HANDLE;
	other._pipeline = VK_NULL_HANDLE;
	other._layout = VK_NULL_HANDLE;
	other._ssboLayouts.~vector();
	other._pushConstantLayouts.~vector();
	other._renderAttachmentLayout.~RenderAttachmentLayout();
	other._hashID = UINT32_MAX;
}

GPUPipeline_Vulkan::~GPUPipeline_Vulkan()
{
	if (_layout != VK_NULL_HANDLE)
	{
		vkDestroyPipelineLayout(_device, _layout, nullptr);
	}

	if (_pipeline != VK_NULL_HANDLE)
	{
		vkDestroyPipeline(_device, _pipeline, nullptr);
	}
}

void GPUPipeline_Vulkan::operator=(GPUPipeline_Vulkan&& other) noexcept
{
	this->~GPUPipeline_Vulkan();

	_device = other._device;
	_pipeline = other._pipeline;
	_layout = other._layout;
	_ssboLayouts = other._ssboLayouts;
	_pushConstantLayouts = other._pushConstantLayouts;
	_renderAttachmentLayout = other._renderAttachmentLayout;

	other._device = VK_NULL_HANDLE;
	other._pipeline = VK_NULL_HANDLE;
	other._layout = VK_NULL_HANDLE;
	other._ssboLayouts.~vector();
	other._pushConstantLayouts.~vector();
	other._renderAttachmentLayout.~RenderAttachmentLayout();
	other._hashID = UINT32_MAX;
}

VkPipeline GPUPipeline_Vulkan::GetPipeline() const
{
	return _pipeline;
}

VkPipelineLayout GPUPipeline_Vulkan::GetPipelineLayout() const
{
	return _layout;
}

const std::vector<CSECore::Ref<GPUDataLayoutRef>>& GPUPipeline_Vulkan::GetSSBOLayouts() const
{
	return _ssboLayouts;
}

const std::vector<PushConstantLayout>& GPUPipeline_Vulkan::GetPushConstantLayouts() const
{
	return _pushConstantLayouts;
}

const RenderAttachmentLayout& GPUPipeline_Vulkan::GetRenderAttachmentLayout() const
{
	return _renderAttachmentLayout;
}

uint32_t GPUPipeline_Vulkan::GetHashID() const
{
	return _hashID;
}

PushConstantLayout::PushConstantLayout()
	: _layout(), _stage(PIPELINE_STAGE_NULL)
{

}

PushConstantLayout::PushConstantLayout(const GPUDataLayout& layout, GPUPipelineStageFlags_Vulkan stage)
	: _layout(layout), _stage(stage)
{

}

PushConstantLayout::PushConstantLayout(const PushConstantLayout& other)
	: _layout(other._layout), _stage(other._stage)
{

}

PushConstantLayout::~PushConstantLayout()
{

}

void PushConstantLayout::operator=(const PushConstantLayout& other)
{
	_layout = other._layout;
	_stage = other._stage;
}

const GPUDataLayout& PushConstantLayout::GetDataLayout() const
{
	return _layout;
}

GPUPipelineStageFlags_Vulkan PushConstantLayout::GetStageFlags() const
{
	return _stage;
}

RenderAttachmentLayout::RenderAttachmentLayout()
	: _colorAttachmentFormats(),
	_depthAttachmentFormat(VK_FORMAT_UNDEFINED),
	_stencilAttachmentFormat(VK_FORMAT_UNDEFINED)
{

}

RenderAttachmentLayout::RenderAttachmentLayout(const std::vector<VkFormat>& colorAttachmentFormats,
	VkFormat depthAttachmentFormat,
	VkFormat stencilAttachmentFormat)
	: _colorAttachmentFormats(colorAttachmentFormats), 
	_depthAttachmentFormat(depthAttachmentFormat),
	_stencilAttachmentFormat(stencilAttachmentFormat)
{

}

RenderAttachmentLayout::RenderAttachmentLayout(const RenderAttachmentLayout& other)
	: _colorAttachmentFormats(other._colorAttachmentFormats),
	_depthAttachmentFormat(other._depthAttachmentFormat),
	_stencilAttachmentFormat(other._stencilAttachmentFormat)
{

}

RenderAttachmentLayout::~RenderAttachmentLayout()
{

}

void RenderAttachmentLayout::operator=(const RenderAttachmentLayout& other)
{
	_colorAttachmentFormats = other._colorAttachmentFormats;
	_depthAttachmentFormat = other._depthAttachmentFormat;
	_stencilAttachmentFormat = other._stencilAttachmentFormat;
}

const std::vector<VkFormat>& RenderAttachmentLayout::GetColorFormats() const
{
	return _colorAttachmentFormats;
}

VkFormat RenderAttachmentLayout::GetDepthFormat() const
{
	return _depthAttachmentFormat;
}

VkFormat RenderAttachmentLayout::GetStencilFormat() const
{
	return _stencilAttachmentFormat;
}

}