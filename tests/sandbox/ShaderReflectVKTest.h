#pragma once
//#include "../../renderer/gpu/vulkan/ShaderProcessor_Vulkan.h"
#include "../../core/Logger.h"
#include <fstream>

int main()
{
	/*
	std::fstream spvFileVert("C:/Users/AquaB/Documents/Projects/CSEEngine/assets/materials/vert.spv", std::fstream::in | std::fstream::binary);
	std::stringstream sstreamVert;
	sstreamVert << spvFileVert.rdbuf();
	std::string strVert = sstreamVert.str();
	std::vector<char> spvvert(strVert.size());
	memcpy(spvvert.data(), strVert.data(), strVert.size());
	
	std::fstream spvFileFrag("C:/Users/AquaB/Documents/Projects/CSEEngine/assets/materials/frag.spv", std::fstream::in | std::fstream::binary);
	std::stringstream sstreamFrag;
	sstreamFrag << spvFileFrag.rdbuf();
	std::string strFrag = sstreamFrag.str();
	std::vector<char> spvfrag(strFrag.size());
	memcpy(spvfrag.data(), strFrag.data(), strFrag.size());

	auto vertresult = CSERenderer::ProcessShaderDataLayout(strVert);
	if (vertresult.HasUnexpected())
	{
		CSE_LOGE(vertresult.GetUnexpected());
		return 1;
	}
	auto vertlayout = vertresult.GetExpected();

	auto fragresult = CSERenderer::ProcessShaderDataLayout(strFrag);
	if (fragresult.HasUnexpected())
	{
		CSE_LOGE(fragresult.GetUnexpected());
		return 1;
	}
	auto fraglayout = fragresult.GetExpected();

	CSE_LOGI("Vertex layout");
	for (int i = 0; i < vertlayout.size(); i++)
	{
		CSE_LOGI("Name: " << vertlayout[i].GetName() << "Hash: " << vertlayout[i].GetHashID());
	}

	CSE_LOGI("Fragment layout");
	for (int i = 0; i < fraglayout.size(); i++)
	{
		CSE_LOGI("Name: " << fraglayout[i].GetName() << "Hash: " << fraglayout[i].GetHashID());
	}
	return 0;
	*/
}