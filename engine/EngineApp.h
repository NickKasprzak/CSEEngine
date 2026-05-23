#pragma once
#include "application/App.h"
#include "RenderContext.h"

namespace CSEEngine
{

class EngineApp : public CSEApplication::App
{
public:
	EngineApp();
	virtual ~EngineApp();

	virtual void Initialize(std::string name) override;
	virtual void Run() override;
	virtual void Dispose() override;

protected:
	virtual void PostInitialize() = 0;
	virtual void PreDispose() = 0;

	CSERenderer::RenderContext& GetRenderContext();
private:
	CSERenderer::RenderContext _renderContext;
};

}