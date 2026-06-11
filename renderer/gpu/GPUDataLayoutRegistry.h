#pragma once
#include "GPUDataLayout.h"
#include "containers/SkipArray.h"
#include <unordered_map>

namespace CSERenderer
{

class GPUDataLayoutRegistry
{
private:
	class GPUDataLayoutEntryDeleter;

	class GPUDataLayoutEntry : public GPUDataLayout
	{
	public:
		GPUDataLayoutEntry();
		GPUDataLayoutEntry(const GPUDataLayout& layout, GPUDataLayoutRegistry* source);
		~GPUDataLayoutEntry();

		void operator=(const GPUDataLayoutEntry& other);

	private:
		friend struct GPUDataLayoutEntryDeleter;

		GPUDataLayoutRegistry* _source;
	};

	struct GPUDataLayoutEntryDeleter
	{
		void operator()(GPUDataLayout* layout)
		{
			GPUDataLayoutEntry* entry = static_cast<GPUDataLayoutEntry*>(layout);
			entry->_source->FreeDataLayout(entry);
		}
	};

public:
	GPUDataLayoutRegistry();
	~GPUDataLayoutRegistry();

	CSECore::Ref<GPUDataLayout> AddDataLayout(const GPUDataLayout& layout);
	CSECore::Ref<GPUDataLayout> GetDataLayout(uint32_t layoutNameHash);
	void FreeDataLayout(GPUDataLayout* layout);

private:
	CSECore::SkipArray<GPUDataLayoutEntry, 256> _entries;
	std::unordered_map<uint32_t, int32_t> _nameHashToIndex;
};

}