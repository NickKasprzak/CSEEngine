#pragma once
#include "volk.h"
#include "refcount/Ref.h"
#include "refcount/RefCounted.h"
#include <cstdint>

namespace CSERenderer
{

class SSBOAllocator_Vulkan;
class GPUBuffer;

class SSBO_Vulkan : public CSECore::RefCounted
{
public:
	~SSBO_Vulkan();

	VkBuffer GetBuffer();
	VkDeviceSize GetOffset();
	VkDeviceSize GetRange();

private:
	friend class SSBOAllocator_Vulkan;
	friend struct SSBO_VulkanDeleter;

	SSBOAllocator_Vulkan* _source;
	CSECore::Ref<GPUBuffer> _buffer;
	VkDeviceSize _offset;
	VkDeviceSize _range;

	SSBO_Vulkan(SSBOAllocator_Vulkan* source, CSECore::Ref<GPUBuffer> buffer, VkDeviceSize offset, VkDeviceSize range);
};

struct SSBO_VulkanDeleter
{
	void operator()(SSBO_Vulkan* ssbo);
};

class SSBOAllocator_Vulkan
{
public:
	SSBOAllocator_Vulkan();
	~SSBOAllocator_Vulkan();

	void Initialize();
	void Dispose();

	CSECore::Ref<SSBO_Vulkan> CreateSSBO(VkDeviceSize size);
	void ReleaseSSBO(SSBO_Vulkan* ssbo);

private:
	CSECore::Ref<GPUBuffer> _buffer;

	// temp bump allocator
	VkDeviceSize _bumpAllocHead;
};

}