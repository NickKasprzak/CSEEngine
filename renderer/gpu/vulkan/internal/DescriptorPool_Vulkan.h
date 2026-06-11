#pragma once
#include "SSBOAllocator_Vulkan.h"
#include "refcount/Ref.h"
#include "refcount/RefCounted.h"
#include "volk.h"
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace CSERenderer
{

class DescriptorPool_Vulkan;

class GPUImage;

class SSBODescriptor : public CSECore::RefCounted
{
public:
	~SSBODescriptor();

	SSBO_Vulkan* GetSSBO();
	uint32_t GetDescriptorID();

private:
	friend class DescriptorPool_Vulkan;

	DescriptorPool_Vulkan* _source;
	CSECore::Ref<SSBO_Vulkan> _ssbo;
	uint32_t _descriptorID;

	SSBODescriptor(DescriptorPool_Vulkan* source,
		CSECore::Ref<SSBO_Vulkan> ssbo,
		uint32_t descriptorID);
};

class SamplerDescriptor : public CSECore::RefCounted
{
public:
	~SamplerDescriptor();

	GPUImage* GetImage();
	VkImageLayout GetRequiredImageLayout();
	VkSampler GetSampler();
	uint32_t GetDescriptorID();

private:
	friend class DescriptorPool_Vulkan;

	DescriptorPool_Vulkan* _source;
	CSECore::Ref<GPUImage> _image;
	VkImageLayout _requiredLayout;
	VkSampler _sampler;
	uint32_t _descriptorID;

	SamplerDescriptor(DescriptorPool_Vulkan* source,
		CSECore::Ref<GPUImage> image,
		VkImageLayout requiredLayout,
		VkSampler sampler,
		uint32_t descriptorID);
};

class DescriptorPool_Vulkan
{
public:
	DescriptorPool_Vulkan();
	~DescriptorPool_Vulkan();

	void Initialize(VkDevice device);
	void Dispose();

	CSECore::Ref<SSBODescriptor> CreateSSBODescriptor(CSECore::Ref<SSBO_Vulkan> ssbo);
	void ReleaseSSBODescriptor(SSBODescriptor* ssboDescriptor);

	CSECore::Ref<SamplerDescriptor> CreateSamplerDescriptor(CSECore::Ref<GPUImage> image, VkImageLayout requiredLayout, VkSampler sampler);
	void ReleaseSamplerDescriptor(SamplerDescriptor* samplerDescriptor);

	VkDescriptorSetLayout GetDescriptorSetLayout();
	VkDescriptorSet GetDescriptorSet();

private:
	VkDevice _device;
	VkDescriptorPool _pool;
	VkDescriptorSetLayout _layout;
	VkDescriptorSet _set;

	// TODO: replace with free list of IDs later
	uint32_t _freeSSBOID;
	uint32_t _freeSamplerID;
};

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

		friend bool operator==(const DescriptorSetBindingInfo& lhs, const DescriptorSetBindingInfo& rhs);
	};

	std::vector<DescriptorSetBindingInfo> bindings;
};

bool ValidateShaderDescriptorSetLayout(const DescriptorSetLayoutInfo& layoutInfo);

}