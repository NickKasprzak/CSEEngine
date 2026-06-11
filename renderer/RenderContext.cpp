#include "RenderContext.h"
#include "CSEAssert.h"
#include "gpu/GPUBackend_Impl.h"

namespace CSERenderer
{

RenderContext::RenderContext()
{

}

RenderContext::~RenderContext()
{

}

void RenderContext::Initialize()
{
	GPUBackend::InitializeBackend<GPUBackend_Impl>();
}

void RenderContext::Dispose()
{
	GPUBackend::DisposeBackend();
}

void RenderContext::SetTargetWindow(const CSECore::Any<64>& windowInfo)
{
	GPUBackend::Instance()->SetTargetWindow(windowInfo);
}

void RenderContext::UpdateWindowSurfaceSize(uint16_t width, uint16_t height)
{
	GPUBackend::Instance()->UpdateWindowSurfaceSize(width, height);
}

}