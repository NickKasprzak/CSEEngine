#pragma once
#include "application/App.h"
#include "RenderContext.h"

namespace CSEEngine
{

class EngineApp : public CSEApplication::App
{
public:
	EngineApp();
	~EngineApp();

	virtual void Initialize(std::string name) override;
	virtual void Run() override;
	virtual void Dispose() override;

private:
	CSERenderer::RenderContext _renderContext;
};

}