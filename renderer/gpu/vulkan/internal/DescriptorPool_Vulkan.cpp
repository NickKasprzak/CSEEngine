#include "DescriptorPool_Vulkan.h"
#include "../GPUBuffer_Vulkan.h"
#include "../GPUImage_Vulkan.h"
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

DescriptorPool_Vulkan::DescriptorPool_Vulkan()
	: _device(VK_NULL_HANDLE), _pool(VK_NULL_HANDLE), _layout(VK_NULL_HANDLE), _set(VK_NULL_HANDLE),
	_freeSSBOID(0), _freeSamplerID(0)
{

}

DescriptorPool_Vulkan::~DescriptorPool_Vulkan()
{

}

void DescriptorPool_Vulkan::Initialize(VkDevice device)
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

void DescriptorPool_Vulkan::Dispose()
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

CSECore::Ref<SSBODescriptor> DescriptorPool_Vulkan::CreateSSBODescriptor(CSECore::Ref<SSBO_Vulkan> ssbo)
{
	uint32_t descriptorID = _freeSSBOID;

	VkDescriptorBufferInfo descriptorInfo{};
	descriptorInfo.buffer = ssbo->GetBuffer();
	descriptorInfo.offset = ssbo->GetOffset();
	descriptorInfo.range = ssbo->GetRange();

	VkWriteDescriptorSet setWrite{};
	setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	setWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	setWrite.descriptorCount = 1;
	setWrite.dstSet = _set;
	setWrite.dstBinding = SSBO_BINDING_LOCATION;
	setWrite.dstArrayElement = descriptorID;
	setWrite.pBufferInfo = &descriptorInfo;

	vkUpdateDescriptorSets(_device, 1, &setWrite, 0, nullptr);

	++_freeSSBOID;

	return CSECore::Ref<SSBODescriptor>(new SSBODescriptor(this, ssbo, descriptorID));
}

void DescriptorPool_Vulkan::ReleaseSSBODescriptor(SSBODescriptor* ssboDescriptor)
{
	/*
	* This should just return the given descriptor ID
	* back to the free list when implemented. No need
	* for doing anything else.
	*/
}

CSECore::Ref<SamplerDescriptor> DescriptorPool_Vulkan::CreateSamplerDescriptor(CSECore::Ref<GPUImage> image, VkImageLayout requiredLayout, VkSampler sampler)
{
	GPUImage* imageVK = image.GetRawCastedPointer<GPUImage>();
	uint32_t descriptorID = _freeSamplerID;

	VkDescriptorImageInfo descriptorInfo{};
	descriptorInfo.imageView = imageVK->GetImageViewHandle();
	descriptorInfo.imageLayout = requiredLayout;
	descriptorInfo.sampler = sampler;

	VkWriteDescriptorSet setWrite{};
	setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	setWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	setWrite.descriptorCount = 1;
	setWrite.dstSet = _set;
	setWrite.dstBinding = SAMPLER_BINDING_LOCATION;
	setWrite.dstArrayElement = descriptorID;
	setWrite.pImageInfo = &descriptorInfo;

	vkUpdateDescriptorSets(_device, 1, &setWrite, 0, nullptr);

	return CSECore::Ref<SamplerDescriptor>(new SamplerDescriptor(this, image, requiredLayout, sampler, descriptorID));
}

void DescriptorPool_Vulkan::ReleaseSamplerDescriptor(SamplerDescriptor* samplerID)
{
	/*
	* This should just return the given descriptor ID
	* back to the free list when implemented. No need
	* for doing anything else.
	*/
}

VkDescriptorSetLayout DescriptorPool_Vulkan::GetDescriptorSetLayout()
{
	return _layout;
}

VkDescriptorSet DescriptorPool_Vulkan::GetDescriptorSet()
{
	return _set;
}

bool operator==(const DescriptorSetLayoutInfo::DescriptorSetBindingInfo& lhs, const DescriptorSetLayoutInfo::DescriptorSetBindingInfo& rhs)
{
	bool sameSet = lhs.set == rhs.set;
	bool sameBind = lhs.binding == rhs.binding;
	bool sameType = lhs.type == rhs.type;
	return sameSet && sameBind && sameType;
}

SSBODescriptor::SSBODescriptor(DescriptorPool_Vulkan* source,
	CSECore::Ref<SSBO_Vulkan> ssbo,
	uint32_t descriptorID)
	: _source(source),
	_ssbo(ssbo),
	_descriptorID(descriptorID)
{

}

SSBODescriptor::~SSBODescriptor()
{
	_source->ReleaseSSBODescriptor(this);
}

const SSBO_Vulkan* SSBODescriptor::GetSSBO()
{
	return _ssbo.GetRawPointer();
}

uint32_t SSBODescriptor::GetDescriptorID()
{
	return _descriptorID;
}

SamplerDescriptor::SamplerDescriptor(DescriptorPool_Vulkan* source,
	CSECore::Ref<GPUImage> image,
	VkImageLayout requiredLayout,
	VkSampler sampler,
	uint32_t descriptorID)
	: _source(source),
	_image(image),
	_requiredLayout(requiredLayout),
	_sampler(sampler),
	_descriptorID(descriptorID)
{

}

SamplerDescriptor::~SamplerDescriptor()
{

}

const GPUImage* SamplerDescriptor::GetImage()
{
	return _image.GetRawPointer();
}

VkImageLayout SamplerDescriptor::GetRequiredImageLayout()
{
	return _requiredLayout;
}

VkSampler SamplerDescriptor::GetSampler()
{
	return _sampler;
}

uint32_t SamplerDescriptor::GetDescriptorID()
{
	return _descriptorID;
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