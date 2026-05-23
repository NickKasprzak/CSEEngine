#pragma once
#include "../GPUPipeline.h"
#include "GPUDataLayout_Vulkan.h"
#include "refcount/Ref.h"
#include "volk.h"

namespace CSERenderer
{

enum GPUPipelineStageFlags_Vulkan
{
	PIPELINE_STAGE_NULL = 0x0,
	PIPELINE_STAGE_VERTEX = 0x1,
	PIPELINE_STAGE_FRAGMENT = 0x2
};

/*
* Stores a collection of GPUDataLayouts 
* representing all state that can be passed into
* the pipeline. This should only be for SSBOs 
* and a push constant.
* 
* If this is created using multiple shaders on one
* pipeline (ie. vertex and fragment shaders), any
* duplicate type layouts will be omitted.
* 
* This should also input info on what attachment types
* can be attached.
*/

struct GPUPipelineLayoutInput_Vulkan
{
	GPUDataLayout_Vulkan inputLayout;
	uint32_t hashID;

	GPUPipelineLayoutInput_Vulkan();
	GPUPipelineLayoutInput_Vulkan(const GPUDataLayout_Vulkan& inputLayout);
	GPUPipelineLayoutInput_Vulkan(const GPUPipelineLayoutInput_Vulkan& other);
	~GPUPipelineLayoutInput_Vulkan();

	void operator=(const GPUPipelineLayoutInput_Vulkan& other);
};

struct GPUPipelineLayoutPushConstant_Vulkan
{
	GPUDataLayout_Vulkan pushConstantLayout;
	GPUPipelineStageFlags_Vulkan stage;
	uint32_t hashID;

	GPUPipelineLayoutPushConstant_Vulkan();
	GPUPipelineLayoutPushConstant_Vulkan(const GPUDataLayout_Vulkan& pushConstantLayout, GPUPipelineStageFlags_Vulkan stage);
	GPUPipelineLayoutPushConstant_Vulkan(const GPUPipelineLayoutPushConstant_Vulkan& other);
	~GPUPipelineLayoutPushConstant_Vulkan();

	void operator=(const GPUPipelineLayoutPushConstant_Vulkan& other);
};

class GPUPipelineLayoutInfo_Vulkan
{
public:
	GPUPipelineLayoutInfo_Vulkan();
	GPUPipelineLayoutInfo_Vulkan(const std::vector<GPUPipelineLayoutInput_Vulkan>& inputs, const std::vector<GPUPipelineLayoutPushConstant_Vulkan>& pushConstants);
	GPUPipelineLayoutInfo_Vulkan(const GPUPipelineLayoutInfo_Vulkan& other);
	~GPUPipelineLayoutInfo_Vulkan();

	void operator=(const GPUPipelineLayoutInfo_Vulkan& other);
	bool operator==(const GPUPipelineLayoutInfo_Vulkan& other);
	bool operator!=(const GPUPipelineLayoutInfo_Vulkan& other);

	const std::vector<GPUPipelineLayoutInput_Vulkan>& GetInputs() const;
	const std::vector<GPUPipelineLayoutPushConstant_Vulkan>& GetPushConstants() const;
	uint32_t GetHashID() const;

private:
	std::vector<GPUPipelineLayoutInput_Vulkan> _inputs;
	std::vector<GPUPipelineLayoutPushConstant_Vulkan> _pushConstants;
	uint32_t _hashID;
};

class GPUPipeline_Vulkan : public GPUPipeline
{
public:
	GPUPipeline_Vulkan();
	GPUPipeline_Vulkan(VkDevice device, VkPipeline pipeline, VkPipelineLayout layout, const GPUPipelineLayoutInfo_Vulkan& layoutInfo, uint32_t stateHashID);
	GPUPipeline_Vulkan(GPUPipeline_Vulkan&& other) noexcept;
	virtual ~GPUPipeline_Vulkan();

	void operator=(GPUPipeline_Vulkan&& other) noexcept;
	bool operator==(const GPUPipeline_Vulkan& other);
	bool operator!=(const GPUPipeline_Vulkan& other);

	VkPipeline GetPipeline() const;
	VkPipelineLayout GetPipelineLayout() const;
	const GPUPipelineLayoutInfo_Vulkan& GetPipelineLayoutInfo() const;
	uint32_t GetStateHashID() const;

private:
	VkDevice _device;
	VkPipeline _pipeline;
	VkPipelineLayout _layout;
	GPUPipelineLayoutInfo_Vulkan _layoutInfo;
	uint32_t _stateHashID;
};

}