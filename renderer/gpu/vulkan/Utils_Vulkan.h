#pragma once
#include "../GPUImageFormats.h"

namespace CSERenderer
{

inline VkFormat ImageFormatToVkFormat(ImageFormat format)
{
	switch (format)
	{
	case FORMAT_R8G8B8A8_UNORM:
		return VK_FORMAT_R8G8B8A8_UNORM;
	case FORMAT_D32_SFLOAT_S8_UINT:
		return VK_FORMAT_D32_SFLOAT_S8_UINT;
	default:
		return VK_FORMAT_UNDEFINED;
	}
}

}