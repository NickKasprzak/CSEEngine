#include "RenderContext.h"
#include "CSEAssert.h"
#include "gpu/GPUBackend_Impl.h"

namespace CSERenderer
{

RenderContext::RenderContext()
	: _backend(nullptr)
{

}

RenderContext::~RenderContext()
{

}

void RenderContext::Initialize()
{
	_backend = new GPUBackend_Impl();
	_backend->Initialize();
}

void RenderContext::Dispose()
{
	if (_backend != nullptr)
	{
		_backend->Dispose();
		delete _backend;
	}
}

GPUBackend* RenderContext::GetBackend()
{
	return _backend;
}

}