#pragma once
#include "GPUDataLayout.h"
#include "containers/SkipArray.h"
#include <unordered_map>

namespace CSERenderer
{

class GPUDataLayoutRegistry
{
public:
	GPUDataLayoutRegistry();
	~GPUDataLayoutRegistry();

	CSECore::Ref<GPUDataLayoutRef> AddDataLayout(const GPUDataLayout& layout);
	CSECore::Ref<GPUDataLayoutRef> GetDataLayout(const std::string& layoutName);

private:
	CSECore::SkipArray<GPUDataLayoutRef, 256> _entries;
	std::unordered_map<std::string, int32_t> _nameToIndex;
};

}