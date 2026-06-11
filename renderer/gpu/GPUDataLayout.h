#pragma once
#include "Expected.h"
#include "Optional.h"
#include "refcount/Ref.h"
#include "refcount/RefCounted.h"
#include <cstdint>
#include <vector>
#include <string>

namespace CSERenderer
{

class GPUDataLayout;

static uint32_t RUNTIME_ARRAY_LENGTH = UINT32_MAX;

enum DataLayoutMemberType
{
	MEMBER_TYPE_NULL,
	MEMBER_TYPE_INT,
	MEMBER_TYPE_FLOAT,
	MEMBER_TYPE_VEC3,
	MEMBER_TYPE_VEC4,
	MEMBER_TYPE_MAT3,
	MEMBER_TYPE_MAT4,
	MEMBER_TYPE_STRUCT
};

struct DataLayoutMemberDescription
{
public:
	DataLayoutMemberDescription();
	DataLayoutMemberDescription(std::string& memberName,
		size_t size,
		size_t offset,
		uint32_t arrayLength,
		DataLayoutMemberType type,
		GPUDataLayout* typeLayout);
	DataLayoutMemberDescription(const DataLayoutMemberDescription& other);
	~DataLayoutMemberDescription();

	void operator=(const DataLayoutMemberDescription& other);

	uint32_t GetLayoutHash() const;
	uint32_t GetNameHash() const;
	size_t GetSize() const;
	size_t GetOffset() const;
	uint32_t GetArrayLength() const;
	DataLayoutMemberType GetType() const;
	GPUDataLayout* GetLayout() const;

	void test();

private:
	uint32_t _layoutHash;
	uint32_t _nameHash;
	size_t _size;
	size_t _offset;
	uint32_t _arrayLength;
	DataLayoutMemberType _type;
	GPUDataLayout* _typeLayout;
};

class GPUDataLayout : public CSECore::RefCounted
{
public:
	GPUDataLayout();
	GPUDataLayout(const GPUDataLayout& other);
	virtual ~GPUDataLayout();

	void operator=(const GPUDataLayout& other);

	uint32_t GetLayoutHash() const;
	uint32_t GetNameHash() const;
	size_t GetSize() const;
	size_t GetAlignment() const;
	CSECore::Expected<const DataLayoutMemberDescription*, std::string> GetMember(uint32_t nameHash) const;
	const std::vector<DataLayoutMemberDescription>& GetMembers() const;

private:
	friend class GPUDataLayoutBuilder;

	uint32_t _layoutHash;
	uint32_t _nameHash;
	size_t _size;
	size_t _alignment;
	std::vector<DataLayoutMemberDescription> _members;

	friend bool operator==(const GPUDataLayout& lhs, const GPUDataLayout& rhs)
	{
		return lhs._layoutHash == rhs._layoutHash;
	}

	friend bool operator!=(const GPUDataLayout& lhs, const GPUDataLayout& rhs)
	{
		return lhs._layoutHash != rhs._layoutHash;
	}

	GPUDataLayout(std::string& name, size_t size, size_t alignment, std::vector<DataLayoutMemberDescription>& members);
};

class GPUDataLayoutView
{
public:
	GPUDataLayoutView();
	GPUDataLayoutView(const GPUDataLayout* layout);
	~GPUDataLayoutView();

	GPUDataLayoutView GetMember(uint32_t nameHash);
	GPUDataLayoutView GetArrayIndex(uint32_t index);

	uint32_t GetRootLayoutHash();
	size_t GetOffset();
	DataLayoutMemberType GetMemberType();

private:
	uint32_t _rootLayoutHash;
	const GPUDataLayout* _layout;
	const DataLayoutMemberDescription* _memberDesc;
	DataLayoutMemberType _type;
	size_t _offset;

	GPUDataLayoutView(uint32_t rootHash, const GPUDataLayout* layout, const DataLayoutMemberDescription* memberDesc, DataLayoutMemberType type, size_t offset);
};

class GPUDataLayoutBuilder
{
public:
	GPUDataLayoutBuilder();
	~GPUDataLayoutBuilder();

	void SetName(std::string& name);
	void AppendPrimitiveMember(std::string& name, size_t size, size_t alignment, uint32_t arrayLength, DataLayoutMemberType type);
	void AppendStructMember(std::string& name, uint32_t arrayLength, GPUDataLayout* typeLayout);
	GPUDataLayout Build();

private:
	std::string _name;
	size_t _runningSize;
	size_t _runningAlignment;
	std::vector<DataLayoutMemberDescription> _members;

	bool _runtimeArrayAdded;
};

}