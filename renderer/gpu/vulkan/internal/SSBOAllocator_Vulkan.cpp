#include "SSBOAllocator_Vulkan.h"
#include "../GPUBackend_Vulkan.h"
#include "../GPUBuffer_Vulkan.h"

namespace CSERenderer
{

static const uint32_t SSBO_BACKING_ALLOC_SIZE = 4096;

SSBOAllocator_Vulkan::SSBOAllocator_Vulkan()
	: _buffer(), _bumpAllocHead(0)
{

}

SSBOAllocator_Vulkan::~SSBOAllocator_Vulkan()
{

}

void SSBOAllocator_Vulkan::Initialize()
{
	BufferCreateInfo createInfo{};
	createInfo.size = SSBO_BACKING_ALLOC_SIZE;
	createInfo.usage = BUFFER_USAGE_STORAGE;

	CSECore::Ref<GPUBuffer> buffer = GPUBackend_Vulkan::Instance_Vulkan()->CreateBuffer(createInfo);
	CSE_ASSERT(buffer.GetRawPointer() != nullptr, "Failed to create SSBO backing allocation buffer.");

	_buffer = buffer;
}

void SSBOAllocator_Vulkan::Dispose()
{

}

CSECore::Ref<SSBO_Vulkan> SSBOAllocator_Vulkan::CreateSSBO(VkDeviceSize size)
{
	CSE_ASSERT(_bumpAllocHead + size < SSBO_BACKING_ALLOC_SIZE, "Requested allocation is too big.");

	VkDeviceSize alloc = _bumpAllocHead;
	_bumpAllocHead += size;

	return CSECore::Ref<SSBO_Vulkan>(new SSBO_Vulkan(this, _buffer, alloc, size));
}

void SSBOAllocator_Vulkan::ReleaseSSBO(SSBO_Vulkan* ssbo)
{
	// do nothing for now, bump alloc deallocations shouldnt do anything
}

SSBO_Vulkan::SSBO_Vulkan(SSBOAllocator_Vulkan* source, CSECore::Ref<GPUBuffer> buffer, VkDeviceSize offset, VkDeviceSize range)
	: _source(source), _buffer(buffer), _offset(offset), _range(range)
{

}

SSBO_Vulkan::~SSBO_Vulkan()
{
	_source->ReleaseSSBO(this);
}

VkBuffer SSBO_Vulkan::GetBuffer()
{
	GPUBuffer_Vulkan* bufferVK = _buffer.GetRawCastedPointer<GPUBuffer_Vulkan>();
	return bufferVK->GetHandle();
}

VkDeviceSize SSBO_Vulkan::GetOffset()
{
	return _offset;
}

VkDeviceSize SSBO_Vulkan::GetRange()
{
	return _range;
}

}