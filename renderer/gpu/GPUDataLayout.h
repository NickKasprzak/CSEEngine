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

enum DataLayoutMemberType
{
	MEMBER_TYPE_NULL,
	MEMBER_TYPE_INT,
	MEMBER_TYPE_FLOAT,
	MEMBER_TYPE_VEC3,
	MEMBER_TYPE_VEC4,
	MEMBER_TYPE_MAT3,
	MEMBER_TYPE_MAT4,
	MEMBER_TYPE_STRUCT,
	MEMBER_TYPE_ARRAY
};

struct DataLayoutMemberDescription
{
	std::string memberName;
	uint32_t memberHashID;
	size_t offset;
	size_t size;
	DataLayoutMemberType type;
	GPUDataLayout* typeLayout;

	DataLayoutMemberDescription();
	DataLayoutMemberDescription(std::string& memberName,
		size_t offset,
		size_t size,
		DataLayoutMemberType type,
		GPUDataLayout* typeLayout);
	DataLayoutMemberDescription(const DataLayoutMemberDescription& other);
	~DataLayoutMemberDescription();

	void operator=(const DataLayoutMemberDescription& other);
};

class GPUDataLayout
{
public:
	GPUDataLayout();
	GPUDataLayout(std::string& name, std::vector<DataLayoutMemberDescription>& members);
	GPUDataLayout(const GPUDataLayout& other);
	~GPUDataLayout();

	void operator=(const GPUDataLayout& other);

	const std::string& GetName() const;
	uint32_t GetHashID() const;
	size_t GetSize() const;
	CSECore::Expected<const DataLayoutMemberDescription*, std::string> GetMember(const std::string& memberName) const;
	const std::vector<DataLayoutMemberDescription>& GetMembers() const;

private:
	std::string _name;
	uint32_t _hashID;
	size_t _size;
	std::vector<DataLayoutMemberDescription> _members;

	friend bool operator==(const GPUDataLayout& lhs, const GPUDataLayout& rhs)
	{
		return lhs._hashID == rhs._hashID;
	}

	friend bool operator!=(const GPUDataLayout& lhs, const GPUDataLayout& rhs)
	{
		return lhs._hashID != rhs._hashID;
	}
};

class GPUDataLayoutBuilder
{
public:
	GPUDataLayoutBuilder();
	~GPUDataLayoutBuilder();

	void SetName(std::string& name);
	void AppendMember(std::string& name, size_t size, size_t offset, DataLayoutMemberType type, GPUDataLayout* typeLayout);
	GPUDataLayout Build();

private:
	std::string _name;
	std::vector<DataLayoutMemberDescription> _members;
};

class GPUDataLayoutRef : public CSECore::RefCounted
{
public:
	GPUDataLayoutRef();
	GPUDataLayoutRef(const GPUDataLayout& layout);
	~GPUDataLayoutRef();

	void operator=(const GPUDataLayoutRef& other);

	const GPUDataLayout& GetLayout();

private:
	GPUDataLayout _layout;
};

}