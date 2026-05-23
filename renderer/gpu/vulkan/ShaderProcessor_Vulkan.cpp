#include "ShaderProcessor_Vulkan.h"
#include "Logger.h"
#include "spirv_reflect.h"
#include <set>

namespace CSERenderer
{

struct ShaderProcessingInfo
{
	struct PushConstantLayout
	{
		GPUDataLayout_Vulkan layout;
		GPUPipelineStageFlags_Vulkan stage = PIPELINE_STAGE_NULL;
	};
	std::vector<DescriptorSetLayoutInfo::DescriptorSetBindingInfo> descriptorBindings;
	std::vector<GPUDataLayout_Vulkan> inputLayouts;
	std::vector<PushConstantLayout> pushConstantLayouts;
};

class ShaderLayoutInfoBuilder
{
	struct InputEntry
	{
		GPUDataLayout_Vulkan layout;

		InputEntry(const GPUDataLayout_Vulkan& layout);
		InputEntry(const InputEntry& other);
		~InputEntry();

		void operator=(const InputEntry& other);
	};

	struct PushConstantEntry
	{
		GPUDataLayout_Vulkan layout;
		GPUPipelineStageFlags_Vulkan stage;

		PushConstantEntry(const GPUDataLayout_Vulkan& layout, GPUPipelineStageFlags_Vulkan stage);
		PushConstantEntry(const PushConstantEntry& other);
		~PushConstantEntry();

		void operator=(const PushConstantEntry& other);
	};

public:
	ShaderLayoutInfoBuilder();
	~ShaderLayoutInfoBuilder();

	void AddShaderProcessingInfoData(const ShaderProcessingInfo& data);

	void AddDescriptorBinding(const DescriptorSetLayoutInfo::DescriptorSetBindingInfo& descBinding);
	void AddInputEntry(const GPUDataLayout_Vulkan& layout);
	void AddPushConstantEntry(const GPUDataLayout_Vulkan& layout, GPUPipelineStageFlags_Vulkan stage);

	ShaderLayoutInfo Build();

private:
	std::vector<DescriptorSetLayoutInfo::DescriptorSetBindingInfo> _descriptorBindings;
	std::vector<InputEntry> _inputEntries;
	std::vector<PushConstantEntry> _pushConstantEntries;
};

CSECore::Expected<ShaderProcessingInfo, std::string> ProcessShaderLayout(const std::string& shaderCode, GPUPipelineStageFlags_Vulkan stage);
CSECore::Expected<ShaderProcessingInfo, std::string> ProcessShaderModule(SpvReflectShaderModule& module, GPUPipelineStageFlags_Vulkan stage);

CSECore::Expected<GPUDataLayout_Vulkan, std::string> CreateDataLayoutFromBlock(const SpvReflectBlockVariable& block);
DataLayoutMemberType SPVOpToMemberType(SpvOp op, SpvReflectNumericTraits numTraits);
DescriptorSetLayoutInfo::DescriptorSetBindingInfo::DescriptorType SpvReflectDescriptorTypeToDescriptorType(SpvReflectDescriptorType descType);

CSECore::Expected<ShaderLayoutInfo, std::string> ProcessGraphicsShaderLayout(const std::string& vertexShaderCode, const std::string& fragmentShaderCode)
{
	CSECore::Expected<ShaderProcessingInfo, std::string> vertResult = ProcessShaderLayout(vertexShaderCode, PIPELINE_STAGE_VERTEX);
	if (vertResult.HasUnexpected())
	{
		return CSECore::CreateUnexpected<ShaderLayoutInfo, std::string>("Failed to process vertex shader. Reason: " + vertResult.GetUnexpected());
	}

	CSECore::Expected<ShaderProcessingInfo, std::string> fragResult = ProcessShaderLayout(fragmentShaderCode, PIPELINE_STAGE_FRAGMENT);
	if (fragResult.HasUnexpected())
	{
		return CSECore::CreateUnexpected<ShaderLayoutInfo, std::string>("Failed to process fragment shader. Reason: " + fragResult.GetUnexpected());
	}

	ShaderLayoutInfoBuilder builder;
	builder.AddShaderProcessingInfoData(vertResult.GetExpected());
	builder.AddShaderProcessingInfoData(fragResult.GetExpected());
	return CSECore::CreateExpected<ShaderLayoutInfo, std::string>(builder.Build());
}

CSECore::Expected<ShaderProcessingInfo, std::string> ProcessShaderLayout(const std::string& shaderCode, GPUPipelineStageFlags_Vulkan stage)
{
	SpvReflectShaderModule reflModule;
	SpvReflectResult result = spvReflectCreateShaderModule(shaderCode.size(), shaderCode.data(), &reflModule);
	if (result != SPV_REFLECT_RESULT_SUCCESS)
	{
		return CSECore::CreateUnexpected<ShaderProcessingInfo, std::string>("Failed to load SPIRV-Reflect module.");
	}

	return ProcessShaderModule(reflModule, stage);
}

CSECore::Expected<ShaderProcessingInfo, std::string> ProcessShaderModule(SpvReflectShaderModule& module, GPUPipelineStageFlags_Vulkan stage)
{
	uint32_t setCount = 0;
	spvReflectEnumerateDescriptorSets(&module, &setCount, nullptr);
	std::vector<SpvReflectDescriptorSet*> sets;
	sets.resize(setCount);
	spvReflectEnumerateDescriptorSets(&module, &setCount, sets.data());

	std::vector<DescriptorSetLayoutInfo::DescriptorSetBindingInfo> descriptors;
	std::vector<GPUDataLayout_Vulkan> inputLayouts;
	for (int i = 0; i < setCount; i++)
	{
		uint32_t bindingCount = sets[i]->binding_count;
		
		for (int j = 0; j < bindingCount; j++)
		{
			SpvReflectDescriptorBinding* binding = sets[i]->bindings[j];

			if (binding->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER)
			{
				CSECore::Expected<GPUDataLayout_Vulkan, std::string> result = CreateDataLayoutFromBlock(binding->block);
				if (result.HasUnexpected())
				{
					return CSECore::CreateUnexpected<ShaderProcessingInfo, std::string>(result.GetUnexpected());
				}

				inputLayouts.push_back(result.GetExpected());
			}

			DescriptorSetLayoutInfo::DescriptorSetBindingInfo bindingInfo;
			bindingInfo.set = i;
			bindingInfo.binding = j;
			bindingInfo.type = SpvReflectDescriptorTypeToDescriptorType(binding->descriptor_type);

			descriptors.push_back(bindingInfo);
		}
	}

	uint32_t pushConstantCount = 0;
	spvReflectEnumeratePushConstants(&module, &pushConstantCount, nullptr);
	std::vector<SpvReflectBlockVariable*> pushConstants;
	pushConstants.resize(pushConstantCount);
	spvReflectEnumeratePushConstants(&module, &pushConstantCount, pushConstants.data());

	std::vector<ShaderProcessingInfo::PushConstantLayout> pushConstantLayouts;
	for (int i = 0; i < pushConstantCount; i++)
	{
		CSECore::Expected<GPUDataLayout_Vulkan, std::string> result = CreateDataLayoutFromBlock(*pushConstants[i]);
		if (result.HasUnexpected())
		{
			return CSECore::CreateUnexpected<ShaderProcessingInfo, std::string>(result.GetUnexpected());
		}
		
		struct ShaderProcessingInfo::PushConstantLayout pushConstant;
		pushConstant.layout = result.GetExpected();
		pushConstant.stage = stage;

		pushConstantLayouts.push_back(pushConstant);
	}

	ShaderProcessingInfo result;
	result.descriptorBindings = descriptors;
	result.inputLayouts = inputLayouts;
	result.pushConstantLayouts = pushConstantLayouts;

	return CSECore::CreateExpected<ShaderProcessingInfo, std::string>(result);
}

CSECore::Expected<GPUDataLayout_Vulkan, std::string> CreateDataLayoutFromBlock(const SpvReflectBlockVariable& block)
{
	GPUDataLayoutBuilder_Vulkan builder;
	builder.SetName(std::string(block.name));

	for (int i = 0; i < block.member_count; i++)
	{
		const SpvReflectBlockVariable& member = block.members[i];

		std::string name(member.name);
		size_t size = member.size;
		size_t offset = member.offset;
		DataLayoutMemberType type = SPVOpToMemberType(member.type_description->op, member.numeric);
		
		if (type == MEMBER_TYPE_NULL)
		{
			return CSECore::CreateUnexpected<GPUDataLayout_Vulkan, std::string>("Encountered an unsupported member type.");
		}

		if (type == DataLayoutMemberType::MEMBER_TYPE_STRUCT || type == DataLayoutMemberType::MEMBER_TYPE_ARRAY)
		{
			CSECore::Expected<GPUDataLayout_Vulkan, std::string> structLayoutResult = CreateDataLayoutFromBlock(member);
			if (structLayoutResult.HasUnexpected())
			{
				return structLayoutResult;
			}

			GPUDataLayout_Vulkan* layoutPtr = structLayoutResult.GetExpectedPtr();
			builder.AppendMember(name, size, offset, type, layoutPtr);
			continue;
		}

		builder.AppendMember(name, size, offset, type, nullptr);
	}

	return CSECore::CreateExpected<GPUDataLayout_Vulkan, std::string>(builder.Build());
}

DataLayoutMemberType SPVOpToMemberType(SpvOp op, SpvReflectNumericTraits traits)
{
	switch (op)
	{
	case SpvOpTypeInt:
		return MEMBER_TYPE_INT;
	case SpvOpTypeFloat:
		return MEMBER_TYPE_FLOAT;
	case SpvOpTypeVector:
		switch (traits.vector.component_count)
		{
		case 3:
			return MEMBER_TYPE_VEC3;
		case 4:
			return MEMBER_TYPE_VEC4;
		default:
			return MEMBER_TYPE_NULL;
		}
	case SpvOpTypeMatrix:
		if (traits.matrix.column_count == 3 && traits.matrix.row_count == 3)
		{
			return MEMBER_TYPE_MAT3;
		}

		else if (traits.matrix.column_count == 4 && traits.matrix.row_count == 4)
		{
			return MEMBER_TYPE_MAT4;
		}

		return MEMBER_TYPE_NULL;
	case SpvOpTypeStruct:
		return MEMBER_TYPE_STRUCT;
	case SpvOpTypeRuntimeArray:
		return MEMBER_TYPE_ARRAY;
	default:
		return MEMBER_TYPE_NULL;
	}
}

DescriptorSetLayoutInfo::DescriptorSetBindingInfo::DescriptorType SpvReflectDescriptorTypeToDescriptorType(SpvReflectDescriptorType descType)
{
	switch (descType)
	{
	case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
		return DescriptorSetLayoutInfo::DescriptorSetBindingInfo::DescriptorType::DESCRIPTOR_TYPE_SSBO;
	case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
		return DescriptorSetLayoutInfo::DescriptorSetBindingInfo::DescriptorType::DESCRIPTOR_TYPE_SAMPLER;
	default:
		return DescriptorSetLayoutInfo::DescriptorSetBindingInfo::DescriptorType::DESCRIPTOR_TYPE_NULL;
	}
}

ShaderLayoutInfoBuilder::ShaderLayoutInfoBuilder()
	: _descriptorBindings(), _inputEntries(), _pushConstantEntries()
{

}

ShaderLayoutInfoBuilder::~ShaderLayoutInfoBuilder()
{

}

void ShaderLayoutInfoBuilder::AddShaderProcessingInfoData(const ShaderProcessingInfo& data)
{
	for (int i = 0; i < data.descriptorBindings.size(); i++)
	{
		AddDescriptorBinding(data.descriptorBindings[i]);
	}

	for (int i = 0; i < data.inputLayouts.size(); i++)
	{
		AddInputEntry(data.inputLayouts[i]);
	}

	for (int i = 0; i < data.pushConstantLayouts.size(); i++)
	{
		AddPushConstantEntry(data.pushConstantLayouts[i].layout, data.pushConstantLayouts[i].stage);
	}
}

void ShaderLayoutInfoBuilder::AddDescriptorBinding(const DescriptorSetLayoutInfo::DescriptorSetBindingInfo& descBinding)
{
	for (int i = 0; i < _descriptorBindings.size(); i++)
	{
		if (_descriptorBindings[i] == descBinding)
		{
			return;
		}
	}

	_descriptorBindings.push_back(descBinding);
}

void ShaderLayoutInfoBuilder::AddInputEntry(const GPUDataLayout_Vulkan& layout)
{
	for (int i = 0; i < _inputEntries.size(); i++)
	{
		if (_inputEntries[i].layout == layout)
		{
			return;
		}
	}

	_inputEntries.push_back(InputEntry(layout));
}

void ShaderLayoutInfoBuilder::AddPushConstantEntry(const GPUDataLayout_Vulkan& layout, GPUPipelineStageFlags_Vulkan stage)
{
	for (int i = 0; i < _pushConstantEntries.size(); i++)
	{
		if (_pushConstantEntries[i].layout == layout)
		{
			_pushConstantEntries[i].stage = (GPUPipelineStageFlags_Vulkan)(_pushConstantEntries[i].stage | stage);
			return;
		}
	}

	_pushConstantEntries.push_back(PushConstantEntry(layout, stage));
}

ShaderLayoutInfo ShaderLayoutInfoBuilder::Build()
{
	std::vector<GPUPipelineLayoutInput_Vulkan> inputs;
	for (int i = 0; i < _inputEntries.size(); i++)
	{
		inputs.push_back(GPUPipelineLayoutInput_Vulkan(_inputEntries[i].layout));
	}

	std::vector<GPUPipelineLayoutPushConstant_Vulkan> pushConstants;
	for (int i = 0; i < _pushConstantEntries.size(); i++)
	{
		pushConstants.push_back(GPUPipelineLayoutPushConstant_Vulkan(_pushConstantEntries[i].layout, _pushConstantEntries[i].stage));
	}

	ShaderLayoutInfo layoutInfo;
	layoutInfo.descriptorSetLayoutInfo.bindings = _descriptorBindings;
	layoutInfo.layoutInputs = inputs;
	layoutInfo.layoutPushConstants = pushConstants;

	return layoutInfo;
}

ShaderLayoutInfoBuilder::InputEntry::InputEntry(const GPUDataLayout_Vulkan& layout)
	: layout(layout)
{

}

ShaderLayoutInfoBuilder::InputEntry::InputEntry(const InputEntry& other)
	: layout(other.layout)
{

}

ShaderLayoutInfoBuilder::InputEntry::~InputEntry()
{

}

void ShaderLayoutInfoBuilder::InputEntry::operator=(const InputEntry& other)
{
	layout = other.layout;
}

ShaderLayoutInfoBuilder::PushConstantEntry::PushConstantEntry(const GPUDataLayout_Vulkan& layout, GPUPipelineStageFlags_Vulkan stage)
	: layout(layout), stage(stage)
{

}

ShaderLayoutInfoBuilder::PushConstantEntry::PushConstantEntry(const PushConstantEntry& other)
	: layout(other.layout), stage(other.stage)
{

}

ShaderLayoutInfoBuilder::PushConstantEntry::~PushConstantEntry()
{

}

void ShaderLayoutInfoBuilder::PushConstantEntry::operator=(const PushConstantEntry& other)
{
	layout = other.layout;
	stage = other.stage;
}

}