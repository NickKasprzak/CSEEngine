#pragma once

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
	const char* shaderCode;
	uint32_t shaderCodeLength;
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
	};
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
		FRONT_FACE_COUNTERCLOCKWISE,
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
	enum BlendFactor
	{
		BLEND_FACTOR_ZERO,
		BLEND_FACTOR_ONE,
		BLEND_FACTOR_SRC,
		BLEND_FACTOR_SRC_INVERT,
		BLEND_FACTOR_DST,
		BLEND_FACTOR_DST_INVERT
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
	uint8_t colorAttachmentCount;
	bool depthAttachment;
	bool stencilAttachment;
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