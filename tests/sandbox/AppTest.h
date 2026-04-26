#pragma once
#include "EngineApp.h"

int main()
{
	CSEEngine::EngineApp engine;
	engine.Initialize("AppTest");
	engine.Run();
	engine.Dispose();
}