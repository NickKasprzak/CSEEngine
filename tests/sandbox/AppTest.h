#pragma once
#include "EngineApp.h"

class AppTestEngine : public CSEEngine::EngineApp
{
public:

protected:
	virtual void PostInitialize() override
	{

	}

	virtual void PreDispose() override
	{

	}

private:

};

int main()
{
	AppTestEngine engine;
	engine.Initialize("AppTest");
	engine.Run();
	engine.Dispose();
}