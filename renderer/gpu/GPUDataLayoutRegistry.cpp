#include "GPUDataLayoutRegistry.h"
#include "Logger.h"

namespace CSERenderer
{

GPUDataLayoutRegistry::GPUDataLayoutRegistry()
	: _entries(), _nameToIndex()
{
	
}

GPUDataLayoutRegistry::~GPUDataLayoutRegistry()
{

}

CSECore::Ref<GPUDataLayoutRef> GPUDataLayoutRegistry::AddDataLayout(const GPUDataLayout& layout)
{
	auto findResult = _nameToIndex.find(layout.GetName());
	auto hashMapEnd = _nameToIndex.end();
	if (findResult != hashMapEnd)
	{
		int32_t layoutIndex = findResult->second;

		if (layout != _entries.GetAtIndex(layoutIndex)->GetLayout())
		{
			CSE_LOGE("A layout entry with the name " << layout.GetName() << " already exists with a different structure.");
			return CSECore::MakeEmptyRef<GPUDataLayoutRef>();
		}

		return CSECore::MakeNonOwningRef<GPUDataLayoutRef>(_entries.GetAtIndex(layoutIndex));
	}

	int32_t layoutIndex = _entries.Add(GPUDataLayoutRef(layout));
	_nameToIndex.insert(std::make_pair(layout.GetName(), layoutIndex));
	return CSECore::MakeNonOwningRef<GPUDataLayoutRef>(_entries.GetAtIndex(layoutIndex));
}

CSECore::Ref<GPUDataLayoutRef> GPUDataLayoutRegistry::GetDataLayout(const std::string& layoutName)
{
	auto findResult = _nameToIndex.find(layoutName);
	auto hashMapEnd = _nameToIndex.end();
	if (findResult != hashMapEnd)
	{
		int32_t layoutIndex = findResult->second;
		return CSECore::MakeNonOwningRef<GPUDataLayoutRef>(_entries.GetAtIndex(layoutIndex));
	}

	return CSECore::MakeEmptyRef<GPUDataLayoutRef>();
}

}