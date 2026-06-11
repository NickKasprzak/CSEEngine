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

	void SetTargetWindow(const CSECore::Any<64>& windowInfo);
	void UpdateWindowSurfaceSize(uint16_t width, uint16_t height);

private:
};

}