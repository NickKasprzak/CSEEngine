#include "GPUDataLayoutRegistry.h"
#include "Logger.h"

namespace CSERenderer
{

GPUDataLayoutRegistry::GPUDataLayoutRegistry()
	: _entries(), _nameHashToIndex()
{
	
}

GPUDataLayoutRegistry::~GPUDataLayoutRegistry()
{

}

CSECore::Ref<GPUDataLayout> GPUDataLayoutRegistry::AddDataLayout(const GPUDataLayout& layout)
{
	auto findResult = _nameHashToIndex.find(layout.GetNameHash());
	auto hashMapEnd = _nameHashToIndex.end();
	if (findResult != hashMapEnd)
	{
		int32_t layoutIndex = findResult->second;

		if (layout.GetLayoutHash() != _entries.GetAtIndex(layoutIndex)->GetLayoutHash())
		{
			CSE_LOGE("A layout entry with the given name already exists with a different structure.");
			return CSECore::Ref<GPUDataLayout>();
		}

		return CSECore::Ref<GPUDataLayout>(_entries.GetAtIndex(layoutIndex), GPUDataLayoutEntryDeleter());
	}

	int32_t layoutIndex = _entries.Add(GPUDataLayoutEntry(layout, this));
	_nameHashToIndex.insert(std::make_pair(layout.GetNameHash(), layoutIndex));
	return CSECore::Ref<GPUDataLayout>(_entries.GetAtIndex(layoutIndex), GPUDataLayoutEntryDeleter());
}

CSECore::Ref<GPUDataLayout> GPUDataLayoutRegistry::GetDataLayout(uint32_t layoutNameHash)
{
	auto findResult = _nameHashToIndex.find(layoutNameHash);
	auto hashMapEnd = _nameHashToIndex.end();
	if (findResult != hashMapEnd)
	{
		int32_t layoutIndex = findResult->second;
		return CSECore::Ref<GPUDataLayout>(_entries.GetAtIndex(layoutIndex), GPUDataLayoutEntryDeleter());
	}

	return CSECore::Ref<GPUDataLayout>();
}

void GPUDataLayoutRegistry::FreeDataLayout(GPUDataLayout* layout)
{
	GPUDataLayoutEntry* entry = static_cast<GPUDataLayoutEntry*>(layout);
	_entries.Remove(entry);
}

GPUDataLayoutRegistry::GPUDataLayoutEntry::GPUDataLayoutEntry()
	: GPUDataLayout(), _source(nullptr)
{

}

GPUDataLayoutRegistry::GPUDataLayoutEntry::GPUDataLayoutEntry(const GPUDataLayout& layout, GPUDataLayoutRegistry* source)
	: GPUDataLayout(layout), _source(source)
{

}

GPUDataLayoutRegistry::GPUDataLayoutEntry::~GPUDataLayoutEntry()
{

}

void GPUDataLayoutRegistry::GPUDataLayoutEntry::operator=(const GPUDataLayoutEntry& other)
{
	GPUDataLayout::operator=(other);
	_source = other._source;
}

}