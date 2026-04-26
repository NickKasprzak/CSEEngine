#pragma once
#include <cstdint>

namespace CSECore
{

inline void* AlignAddress(void* address, size_t alignment)
{
	uintptr_t arithAddress = reinterpret_cast<uintptr_t>(address);
	uintptr_t alignOffset = (arithAddress % alignment == 0) ? 0 : alignment - (arithAddress % alignment);
	return reinterpret_cast<void*>(arithAddress + alignOffset);
}

inline bool CanAllocateAligned(void* address, size_t bufferSize, size_t allocSize, size_t allocAlign)
{
	uintptr_t arithAddress = reinterpret_cast<uintptr_t>(address);
	uintptr_t bufferEndAddress = arithAddress + bufferSize;
	uintptr_t arithAddressAligned = reinterpret_cast<uintptr_t>(AlignAddress(address, allocAlign));
	return (arithAddressAligned + allocSize) < bufferEndAddress;
}

}