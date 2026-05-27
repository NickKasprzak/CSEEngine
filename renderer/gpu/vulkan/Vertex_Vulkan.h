#pragma once
#include "glm/glm.hpp"
#include "volk.h"
#include <vector>

namespace CSERenderer
{

struct Vertex_Vulkan
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 uv;
};

static VkVertexInputBindingDescription GetVertexBindingDescription()
{
	VkVertexInputBindingDescription bindingDesc{};
	bindingDesc.stride = sizeof(Vertex_Vulkan);
	bindingDesc.binding = 0;
	bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDesc;
}

static std::vector<VkVertexInputAttributeDescription> GetVertexAttributeDescriptions()
{
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

	VkVertexInputAttributeDescription positionAttrib;
	positionAttrib.binding = 0;
	positionAttrib.location = 0;
	positionAttrib.format = VK_FORMAT_R32G32B32_SFLOAT;
	positionAttrib.offset = offsetof(Vertex_Vulkan, position);
	attributeDescriptions.push_back(positionAttrib);

	VkVertexInputAttributeDescription normalAttrib;
	normalAttrib.binding = 0;
	normalAttrib.location = 1;
	normalAttrib.format = VK_FORMAT_R32G32B32_SFLOAT;
	normalAttrib.offset = offsetof(Vertex_Vulkan, normal);
	attributeDescriptions.push_back(normalAttrib);

	VkVertexInputAttributeDescription tangentAttrib;
	tangentAttrib.binding = 0;
	tangentAttrib.location = 2;
	tangentAttrib.format = VK_FORMAT_R32G32B32_SFLOAT;
	tangentAttrib.offset = offsetof(Vertex_Vulkan, tangent);
	attributeDescriptions.push_back(tangentAttrib);

	VkVertexInputAttributeDescription uvAttrib;
	uvAttrib.binding = 0;
	uvAttrib.location = 3;
	uvAttrib.format = VK_FORMAT_R32G32_SFLOAT;
	uvAttrib.offset = offsetof(Vertex_Vulkan, uv);
	attributeDescriptions.push_back(uvAttrib);

	return attributeDescriptions;
}

static bool ValidateShaderVertexAttributes(const std::vector<VkVertexInputAttributeDescription>& vertexAttributes)
{
	const std::vector<VkVertexInputAttributeDescription> supportedAttribs = GetVertexAttributeDescriptions();

	if (vertexAttributes.size() != supportedAttribs.size())
	{
		return false;
	}
	
	for (int i = 0; i < supportedAttribs.size(); i++)
	{
		bool mismatchEncountered = false;
		mismatchEncountered |= (supportedAttribs[i].binding != vertexAttributes[i].binding);
		mismatchEncountered |= (supportedAttribs[i].format != vertexAttributes[i].format);
		mismatchEncountered |= (supportedAttribs[i].location != vertexAttributes[i].location);
		mismatchEncountered |= (supportedAttribs[i].offset != vertexAttributes[i].offset);

		if (mismatchEncountered)
		{
			return false;
		}
	}

	return true;
}

}