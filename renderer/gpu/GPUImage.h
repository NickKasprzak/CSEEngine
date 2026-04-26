#pragma once
#include "refcount/RefCounted.h"

namespace CSERenderer
{

enum ImageUsageFlags
{
	IMAGE_USAGE_SAMPLED = 0x1,
	IMAGE_USAGE_COLOR_ATTACHMENT = 0x2,
	IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT = 0x4,
	IMAGE_USAGE_STORAGE = 0x8,
	IMAGE_USAGE_MAPPED = 0x10,
};

enum SamplerFilterMode
{
	FILTER_NEAREST,
	FILTER_LINEAR
};

enum SamplerAddressMode
{
	ADDRESS_MODE_CLAMP_TO_BORDER,
	ADDRESS_MODE_CLAMP_TO_EDGE,
	ADDRESS_MODE_REPEAT
};

class GPUImage : public CSECore::RefCounted
{
public:
	virtual ~GPUImage() {};
};

}