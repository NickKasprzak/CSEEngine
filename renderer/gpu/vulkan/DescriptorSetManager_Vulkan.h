#pragma once
#include "refcount/Ref.h"
#include "volk.h"
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace CSERenderer
{

class GPUBuffer;
class GPUImage;

struct DescriptorSetLayoutInfo
{
	struct DescriptorSetBindingInfo
	{
		uint32_t set;
		uint32_t binding;
		enum DescriptorType
		{
			DESCRIPTOR_TYPE_NULL,
			DESCRIPTOR_TYPE_SSBO,
			DESCRIPTOR_TYPE_SAMPLER
		} type;

		bool operator==(const DescriptorSetBindingInfo& other);
	};

	std::vector<DescriptorSetBindingInfo> bindings;
};

class DescriptorSetManager_Vulkan
{
public:
	DescriptorSetManager_Vulkan();
	~DescriptorSetManager_Vulkan();

	void Initialize(VkDevice device);
	void Dispose();

	uint32_t AddSSBODescriptor(CSECore::Ref<GPUBuffer> buffer);
	void RemoveSSBODescriptor(uint32_t ssboID);

	uint32_t AddSamplerDescriptor(CSECore::Ref<GPUImage> image);
	void RemoveSamplerDescriptor(uint32_t samplerID);

	VkDescriptorSetLayout GetDescriptorSetLayout();
	VkDescriptorSet GetDescriptorSet();

private:
	VkDevice _device;
	VkDescriptorPool _pool;
	VkDescriptorSetLayout _layout;
	VkDescriptorSet _set;

	std::unordered_map<uint32_t, uint32_t> _bufferIDToSSBOID;
	std::unordered_map<uint32_t, uint32_t> _imageIDToSamplerID;
};

bool ValidateShaderDescriptorSetLayout(const DescriptorSetLayoutInfo& layoutInfo);

}