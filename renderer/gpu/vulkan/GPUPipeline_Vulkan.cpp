#include "GPUPipeline_Vulkan.h"
#include "Expected.h"
#include "Logger.h"

namespace CSERenderer
{
	
GPUPipeline_Vulkan::GPUPipeline_Vulkan()
	: _device(VK_NULL_HANDLE), _pipeline(VK_NULL_HANDLE), _layout(VK_NULL_HANDLE), _layoutInfo(), _stateHashID(UINT32_MAX)
{

}

GPUPipeline_Vulkan::GPUPipeline_Vulkan(VkDevice device, VkPipeline pipeline, VkPipelineLayout layout, const GPUPipelineLayoutInfo_Vulkan& layoutInfo, uint32_t stateHashID)
	: _device(device), _pipeline(pipeline), _layout(layout), _layoutInfo(layoutInfo), _stateHashID(stateHashID)
{

}

GPUPipeline_Vulkan::GPUPipeline_Vulkan(GPUPipeline_Vulkan&& other) noexcept
	: _device(VK_NULL_HANDLE), _pipeline(VK_NULL_HANDLE), _layout(VK_NULL_HANDLE), _layoutInfo(), _stateHashID(UINT32_MAX)
{
	_device = other._device;
	_pipeline = other._pipeline;
	_layout = other._layout;
	_layoutInfo = other._layoutInfo;
	_stateHashID = other._stateHashID;

	other._device = VK_NULL_HANDLE;
	other._pipeline = VK_NULL_HANDLE;
	other._layout = VK_NULL_HANDLE;
	other._layoutInfo.~GPUPipelineLayoutInfo_Vulkan();
	other._stateHashID = UINT32_MAX;
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
	_layoutInfo = other._layoutInfo;
	_stateHashID = other._stateHashID;

	other._device = VK_NULL_HANDLE;
	other._pipeline = VK_NULL_HANDLE;
	other._layout = VK_NULL_HANDLE;
	other._layoutInfo.~GPUPipelineLayoutInfo_Vulkan();
	other._stateHashID = UINT32_MAX;
}

bool GPUPipeline_Vulkan::operator==(const GPUPipeline_Vulkan& other)
{
	return _stateHashID == other._stateHashID;
}

bool GPUPipeline_Vulkan::operator!=(const GPUPipeline_Vulkan& other)
{
	return _stateHashID != other._stateHashID;
}

VkPipeline GPUPipeline_Vulkan::GetPipeline() const
{
	return _pipeline;
}

VkPipelineLayout GPUPipeline_Vulkan::GetPipelineLayout() const
{
	return _layout;
}

const GPUPipelineLayoutInfo_Vulkan& GPUPipeline_Vulkan::GetPipelineLayoutInfo() const
{
	return _layoutInfo;
}

uint32_t GPUPipeline_Vulkan::GetStateHashID() const
{
	return _stateHashID;
}

GPUPipelineLayoutInfo_Vulkan::GPUPipelineLayoutInfo_Vulkan()
	: _inputs(), _pushConstants(), _hashID(UINT32_MAX)
{

}

GPUPipelineLayoutInfo_Vulkan::GPUPipelineLayoutInfo_Vulkan(const std::vector<GPUPipelineLayoutInput_Vulkan>& inputs, const std::vector<GPUPipelineLayoutPushConstant_Vulkan>& pushConstants)
	: _inputs(inputs), _pushConstants(pushConstants), _hashID(UINT32_MAX)
{
	uint32_t hash = 0;
	for (int i = 0; i < _inputs.size(); i++)
	{
		hash = hash ^ _inputs[i].hashID;
	}
	for (int i = 0; i < _pushConstants.size(); i++)
	{
		hash = hash ^ _pushConstants[i].hashID;
	}
	_hashID = hash;
}

GPUPipelineLayoutInfo_Vulkan::GPUPipelineLayoutInfo_Vulkan(const GPUPipelineLayoutInfo_Vulkan& other)
	: _inputs(other._inputs), _pushConstants(other._pushConstants), _hashID(other._hashID)
{

}

GPUPipelineLayoutInfo_Vulkan::~GPUPipelineLayoutInfo_Vulkan()
{

}

void GPUPipelineLayoutInfo_Vulkan::operator=(const GPUPipelineLayoutInfo_Vulkan& other)
{
	_inputs = other._inputs;
	_pushConstants = other._pushConstants;
	_hashID = other._hashID;
}

bool GPUPipelineLayoutInfo_Vulkan::operator==(const GPUPipelineLayoutInfo_Vulkan& other)
{
	return _hashID == other._hashID;
}

bool GPUPipelineLayoutInfo_Vulkan::operator!=(const GPUPipelineLayoutInfo_Vulkan& other)
{
	return _hashID != other._hashID;
}

const std::vector<GPUPipelineLayoutInput_Vulkan>& GPUPipelineLayoutInfo_Vulkan::GetInputs() const
{
	return _inputs;
}

const std::vector<GPUPipelineLayoutPushConstant_Vulkan>& GPUPipelineLayoutInfo_Vulkan::GetPushConstants() const
{
	return _pushConstants;
}

uint32_t GPUPipelineLayoutInfo_Vulkan::GetHashID() const
{
	return _hashID;
}

GPUPipelineLayoutInput_Vulkan::GPUPipelineLayoutInput_Vulkan()
	: inputLayout(), hashID(UINT32_MAX)
{

}

GPUPipelineLayoutInput_Vulkan::GPUPipelineLayoutInput_Vulkan(const GPUDataLayout_Vulkan& inputLayout)
	: inputLayout(inputLayout), hashID(UINT32_MAX)
{
	uint32_t hash = 0;
	hash = hash ^ inputLayout.GetHashID();
	hashID = hash;
}

GPUPipelineLayoutInput_Vulkan::GPUPipelineLayoutInput_Vulkan(const GPUPipelineLayoutInput_Vulkan& other)
	: inputLayout(other.inputLayout), hashID(other.hashID)
{

}

GPUPipelineLayoutInput_Vulkan::~GPUPipelineLayoutInput_Vulkan()
{

}

void GPUPipelineLayoutInput_Vulkan::operator=(const GPUPipelineLayoutInput_Vulkan& other)
{
	inputLayout = other.inputLayout;
	hashID = other.hashID;
}

GPUPipelineLayoutPushConstant_Vulkan::GPUPipelineLayoutPushConstant_Vulkan()
	: pushConstantLayout(), stage(PIPELINE_STAGE_NULL), hashID(UINT32_MAX)
{

}

GPUPipelineLayoutPushConstant_Vulkan::GPUPipelineLayoutPushConstant_Vulkan(const GPUDataLayout_Vulkan& pushConstantLayout, GPUPipelineStageFlags_Vulkan stage)
	: pushConstantLayout(pushConstantLayout), stage(stage), hashID(UINT32_MAX)
{
	uint32_t hash = 0;
	hash = hash ^ pushConstantLayout.GetHashID();
	hash = hash ^ std::hash<GPUPipelineStageFlags_Vulkan>{}(stage);
	hashID = hash;
}

GPUPipelineLayoutPushConstant_Vulkan::GPUPipelineLayoutPushConstant_Vulkan(const GPUPipelineLayoutPushConstant_Vulkan& other)
	: pushConstantLayout(other.pushConstantLayout), stage(other.stage), hashID(other.hashID)
{

}

GPUPipelineLayoutPushConstant_Vulkan::~GPUPipelineLayoutPushConstant_Vulkan()
{

}

void GPUPipelineLayoutPushConstant_Vulkan::operator=(const GPUPipelineLayoutPushConstant_Vulkan& other)
{
	pushConstantLayout = other.pushConstantLayout;
	stage = other.stage;
	hashID = other.hashID;
}

}