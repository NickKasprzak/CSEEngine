#include "DescriptorSetManager_Vulkan.h"
#include "GPUBuffer_Vulkan.h"
#include "GPUImage_Vulkan.h"
#include "CSEAssert.h"

namespace CSERenderer
{

static const uint32_t SSBO_DESCRIPTOR_COUNT = 1024;
static const uint32_t SSBO_BINDING_LOCATION = 0;

static const uint32_t SAMPLER_DESCRIPTOR_COUNT = 1024;
static const uint32_t SAMPLER_BINDING_LOCATION = 1;

static const VkDescriptorPoolCreateFlags DESCRIPTOR_POOL_FLAGS = 
VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

static const VkDescriptorBindingFlags DESCRIPTOR_BINDING_FLAGS =
VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

static const VkDescriptorSetLayoutCreateFlags DESCRIPTOR_SET_LAYOUT_FLAGS =
VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

DescriptorSetManager_Vulkan::DescriptorSetManager_Vulkan()
	: _device(VK_NULL_HANDLE), _pool(VK_NULL_HANDLE), _layout(VK_NULL_HANDLE), _set(VK_NULL_HANDLE),
	_bufferIDToSSBOID(), _imageIDToSamplerID()
{

}

DescriptorSetManager_Vulkan::~DescriptorSetManager_Vulkan()
{

}

void DescriptorSetManager_Vulkan::Initialize(VkDevice device)
{
	_device = device;

	std::vector<VkDescriptorPoolSize> poolSizes;

	VkDescriptorPoolSize ssboPoolSize{};
	ssboPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	ssboPoolSize.descriptorCount = SSBO_DESCRIPTOR_COUNT;
	poolSizes.push_back(ssboPoolSize);

	VkDescriptorPoolSize samplerPoolSize{};
	samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerPoolSize.descriptorCount = SAMPLER_DESCRIPTOR_COUNT;
	poolSizes.push_back(samplerPoolSize);

	VkDescriptorPoolCreateInfo poolCreateInfo{};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.poolSizeCount = poolSizes.size();
	poolCreateInfo.pPoolSizes = poolSizes.data();
	poolCreateInfo.maxSets = 1;
	poolCreateInfo.flags = DESCRIPTOR_POOL_FLAGS;

	VkDescriptorPool pool;
	VkResult poolResult = vkCreateDescriptorPool(device, &poolCreateInfo, nullptr, &pool);
	CSE_ASSERT(poolResult == VK_SUCCESS, "Failed to create descriptor pool.");
	_pool = pool;

	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

	VkDescriptorSetLayoutBinding ssboBinding{};
	ssboBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	ssboBinding.descriptorCount = SSBO_DESCRIPTOR_COUNT;
	ssboBinding.binding = SSBO_BINDING_LOCATION;
	ssboBinding.stageFlags = VK_SHADER_STAGE_ALL;
	layoutBindings.push_back(ssboBinding);

	VkDescriptorSetLayoutBinding samplerBinding{};
	samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerBinding.descriptorCount = SAMPLER_DESCRIPTOR_COUNT;
	samplerBinding.binding = SAMPLER_BINDING_LOCATION;
	samplerBinding.stageFlags = VK_SHADER_STAGE_ALL;
	layoutBindings.push_back(samplerBinding);

	std::vector<VkDescriptorBindingFlags> bindingFlags{ DESCRIPTOR_BINDING_FLAGS, DESCRIPTOR_BINDING_FLAGS };
	VkDescriptorSetLayoutBindingFlagsCreateInfo layoutFlags{};
	layoutFlags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
	layoutFlags.bindingCount = layoutBindings.size();
	layoutFlags.pBindingFlags = bindingFlags.data();

	VkDescriptorSetLayoutCreateInfo layoutCreateInfo{};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutCreateInfo.pNext = &layoutFlags;
	layoutCreateInfo.bindingCount = layoutBindings.size();
	layoutCreateInfo.pBindings = layoutBindings.data();
	layoutCreateInfo.flags = DESCRIPTOR_SET_LAYOUT_FLAGS;

	VkDescriptorSetLayout layout;
	VkResult layoutResult = vkCreateDescriptorSetLayout(_device, &layoutCreateInfo, nullptr, &layout);
	CSE_ASSERT(layoutResult == VK_SUCCESS, "Failed to create descriptor set layout.");
	_layout = layout;

	VkDescriptorSetAllocateInfo setAllocateInfo{};
	setAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	setAllocateInfo.descriptorPool = _pool;
	setAllocateInfo.descriptorSetCount = 1;
	setAllocateInfo.pSetLayouts = &_layout;

	VkDescriptorSet set;
	VkResult setResult = vkAllocateDescriptorSets(_device, &setAllocateInfo, &set);
	CSE_ASSERT(setResult == VK_SUCCESS, "Failed to allocate descriptor set.");
	_set = set;
}

void DescriptorSetManager_Vulkan::Dispose()
{
	if (_layout != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(_device, _layout, nullptr);
	}

	if (_pool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(_device, _pool, nullptr);
	}
}

uint32_t DescriptorSetManager_Vulkan::AddSSBODescriptor(CSECore::Ref<GPUBuffer> buffer)
{
	CSE_ASSERT(false, "Unimplemented.");
	return 0;
}

void DescriptorSetManager_Vulkan::RemoveSSBODescriptor(uint32_t ssboID)
{
	CSE_ASSERT(false, "Unimplemented.");
}

uint32_t DescriptorSetManager_Vulkan::AddSamplerDescriptor(CSECore::Ref<GPUImage> image)
{
	CSE_ASSERT(false, "Unimplemented.");
	return 0;
}

void DescriptorSetManager_Vulkan::RemoveSamplerDescriptor(uint32_t samplerID)
{
	CSE_ASSERT(false, "Unimplemented.");
}

VkDescriptorSetLayout DescriptorSetManager_Vulkan::GetDescriptorSetLayout()
{
	return _layout;
}

VkDescriptorSet DescriptorSetManager_Vulkan::GetDescriptorSet()
{
	return _set;
}

bool DescriptorSetLayoutInfo::DescriptorSetBindingInfo::operator==(const DescriptorSetLayoutInfo::DescriptorSetBindingInfo& other)
{
	bool sameSet = set == other.set;
	bool sameBind = binding == other.binding;
	bool sameType = type == other.type;
	return sameSet && sameBind && sameType;
}

bool ValidateShaderDescriptorSetLayout(const DescriptorSetLayoutInfo& layoutInfo)
{
	for (int i = 0; i < layoutInfo.bindings.size(); i++)
	{
		const DescriptorSetLayoutInfo::DescriptorSetBindingInfo& binding = layoutInfo.bindings[i];

		switch (binding.type)
		{
		case DescriptorSetLayoutInfo::DescriptorSetBindingInfo::DESCRIPTOR_TYPE_SSBO:
			if (binding.set != 0 || binding.binding != SSBO_BINDING_LOCATION)
			{
				return false;
			}
			break;

		case DescriptorSetLayoutInfo::DescriptorSetBindingInfo::DESCRIPTOR_TYPE_SAMPLER:
			if (binding.set != 0 || binding.binding != SAMPLER_BINDING_LOCATION)
			{
				return false;
			}
			break;

		default:
			return false;
		}
	}

	return true;
}

}