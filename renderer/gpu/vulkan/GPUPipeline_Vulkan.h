#pragma once
#include "../GPUPipeline.h"
#include "vulkan/vulkan.h"
#include <vector>

namespace CSERenderer
{

enum InputLayoutMemberType
{
	MEMBER_TYPE_INT,
	MEMBER_TYPE_FLOAT,
	MEMBER_TYPE_VEC3,
	MEMBER_TYPE_VEC4,
	MEMBER_TYPE_MAT3,
	MEMBER_TYPE_MAT4
};

struct InputLayoutMemberDescription
{
	uint32_t nameHash;
	uint32_t offset;
	InputLayoutMemberType type;
};

enum InputLayoutType
{
	LAYOUT_TYPE_DYNAMIC,
	LAYOUT_TYPE_MUTABLE,
	LAYOUT_TYPE_STATIC
};

class InputLayoutDescription
{
public:
	InputLayoutDescription();
	~InputLayoutDescription();

	void AppendMember(uint32_t nameHash, InputLayoutMemberType type);
	const std::vector<InputLayoutMemberDescription>& GetMembers();

private:
	uint32_t nameHash;
	std::vector<InputLayoutMemberDescription> _members;
};

class GPUPipelineInputs_Vulkan
{
public:
	GPUPipelineInputs_Vulkan();
	~GPUPipelineInputs_Vulkan();

	void AddSSBODescription(const InputLayoutDescription& description);
	void SetPushConstantDescription(const InputLayoutDescription& description);

private:
	std::vector<InputLayoutMemberDescription> _ssboDescriptions;
	InputLayoutMemberDescription _pushConstantDescription;
};

// add param set that can be created from the above
// grab static buffer references to static types
// allocate mutable type pool
// allocate dynamic type stack?
	// - how would this work for object data? the object data buffer is technically static but the elements that exist in it are dynamic. make the buffer static but the push constant accessor dynamic?
	// - i feel like the above would work. scene data is also static and would need to be accessed via id. the specific object index could be the dynamic part?

class GPUPipelineParameterSet_Vulkan
{
public:


private:

};

class GPUPipeline_Vulkan : public GPUPipeline
{
public:
	GPUPipeline_Vulkan();
	GPUPipeline_Vulkan(const PipelineInfo& pipelineInfo);
	virtual ~GPUPipeline_Vulkan();

	// create unique front facing pipeline parameter set to bind to this pipeline
	// bind parameter set for use in draws?
	// unbind parameter set for use in draws?
	// other bind/unbind funcs for vertex and index data?
	// draw using pipeline?

private:
	//VkPipeline _pipeline;
	//GPUPipelineInterface_Vulkan _interface;
};

}