#pragma once
#include "gpu/GPUBackend.h"

namespace CSERenderer
{

class GPUBackend;

class RenderContext
{
public:
	RenderContext();
	~RenderContext();

	void Initialize();
	void Dispose();

	GPUBackend* GetBackend();

private:
	GPUBackend* _backend;
};

}