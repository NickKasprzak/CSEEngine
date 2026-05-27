#pragma once
#include "GPUImageFormats.h"
#include "Hash.h"
#include <string>

namespace CSERenderer
{

enum CompareOp
{
	COMPARE_OP_NEVER,
	COMPARE_OP_LESS,
	COMPARE_OP_LESS_EQUAL,
	COMPARE_OP_GREATER,
	COMPARE_OP_GREATER_EQUAL,
	COMPARE_OP_ALWAYS
};

struct PipelineShaderInfo
{
	std::string shaderCode;
	enum ShaderType
	{
		SHADER_TYPE_VERTEX,
		SHADER_TYPE_FRAGMENT
	} shaderType;
};

struct PipelineViewportInfo
{
	struct ViewportInfo
	{
		uint32_t width;
		uint32_t height;
		uint32_t maxDepth;
		uint32_t minDepth;
		uint32_t x;
		uint32_t y;
	}viewport;
};

struct PipelineRasterizationInfo
{
	enum CullMode
	{
		CULL_NONE,
		CULL_FRONT_FACE,
		CULL_BACK_FACE,
		CULL_BOTH
	} cullMode;
	enum FrontFace
	{
		FRONT_FACE_COUNTER_CLOCKWISE,
		FRONT_FACE_CLOCKWISE
	} frontFace;
};

struct PipelineMultisampleInfo
{
	uint8_t sampleCount;
};

struct PipelineDepthStencilInfo
{
	bool depthTestEnabled;
	bool depthWriteEnabled;
	CompareOp depthCompareOp;

	bool stencilTestEnabled;
	struct StencilOpState
	{
		enum StencilOp
		{
			STENCIL_OP_KEEP,
			STENCIL_OP_ZERO,
			STENCIL_OP_REPLACE,
			STENCIL_OP_INCREMENT,
			STENCIL_OP_DECREMENT
		};

		StencilOp passOp;
		StencilOp failOp;
		StencilOp depthFailOp;
		CompareOp compareOp;
	};
	StencilOpState stencilFront;
	StencilOpState stencilBack;
};

struct PipelineColorBlendInfo
{
	bool blendEnabled;

	enum BlendFactor
	{
		BLEND_FACTOR_ZERO,
		BLEND_FACTOR_ONE,
		BLEND_FACTOR_SRC_COLOR,
		BLEND_FACTOR_SRC_COLOR_INVERT,
		BLEND_FACTOR_DST_COLOR,
		BLEND_FACTOR_DST_COLOR_INVERT,
		BLEND_FACTOR_SRC_ALPHA,
		BLEND_FACTOR_SRC_ALPHA_INVERT,
		BLEND_FACTOR_DST_ALPHA,
		BLEND_FACTOR_DST_ALPHA_INVERT
	};
	enum BlendOp
	{
		BLEND_OP_ADD,
		BLEND_OP_SUBTRACT,
	};

	BlendFactor srcColorBlendFactor;
	BlendFactor dstColorBlendFactor;
	BlendOp colorBlendOp;

	BlendFactor srcAlphaBlendFactor;
	BlendFactor dstAlphaBlendFactor;
	BlendOp alphaBlendOp;
};

struct PipelineAttachmentInfo
{
	ImageFormat* colorAttachmentFormats;
	uint8_t colorAttachmentCount;
	ImageFormat depthAttachmentFormat;
	ImageFormat stencilAttachmentFormat;
};

struct PipelineDynamicStateInfo
{
	enum DynamicStateFlags
	{
		DYNAMIC_STATE_VIEWPORT = 0x1,
		DYNAMIC_STATE_SCISSOR = 0x2
	} dynamicStateFlags;
};

struct PipelineInfo
{
	PipelineShaderInfo* shaders;
	uint8_t shaderCount;
	PipelineViewportInfo* viewportInfo;
	PipelineRasterizationInfo* rasterizationInfo;
	PipelineMultisampleInfo* multisampleInfo;
	PipelineDepthStencilInfo* depthStencilInfo;
	PipelineColorBlendInfo* colorBlendInfo;
	PipelineAttachmentInfo* attachmentInfo;
	PipelineDynamicStateInfo* dynamicStateInfo;
};

}

namespace CSECore
{

template<>
static uint32_t FNVHash<CSERenderer::PipelineInfo>(const CSERenderer::PipelineInfo& info)
{
	uint32_t hash = 0;

	for (int i = 0; i < info.shaderCount; i++)
	{
		hash ^= FNVHash(info.shaders[i].shaderCode);
		hash ^= FNVHash(info.shaders[i].shaderType);
	}

	hash ^= FNVHash(*info.viewportInfo);
	hash ^= FNVHash(*info.rasterizationInfo);
	hash ^= FNVHash(*info.multisampleInfo);
	hash ^= FNVHash(*info.depthStencilInfo);
	hash ^= FNVHash(*info.colorBlendInfo);

	for (int i = 0; i < info.attachmentInfo->colorAttachmentCount; i++)
	{
		hash ^= FNVHash(info.attachmentInfo->colorAttachmentFormats[i]);
	}
	hash ^= FNVHash(info.attachmentInfo->depthAttachmentFormat);
	hash ^= FNVHash(info.attachmentInfo->stencilAttachmentFormat);

	hash ^= FNVHash(*info.dynamicStateInfo);

	return hash;
}

}