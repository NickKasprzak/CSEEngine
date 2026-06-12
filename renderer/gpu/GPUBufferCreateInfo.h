#pragma once
#include <cstdint>

namespace CSERenderer
{

enum BufferUsageFlags
{
	BUFFER_USAGE_VERTEX = 0x1,
	BUFFER_USAGE_INDEX = 0x2,
	BUFFER_USAGE_STORAGE = 0x4,
	BUFFER_USAGE_UNIFORM = 0x8,
	BUFFER_USAGE_MAPPED = 0x10
};

struct BufferCreateInfo
{
	BufferUsageFlags usage;
	uint32_t size;
};

}