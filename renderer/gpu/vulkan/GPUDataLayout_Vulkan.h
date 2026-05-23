#pragma once
#include "Expected.h"
#include "Optional.h"
#include <cstdint>
#include <vector>
#include <string>

namespace CSERenderer
{

class GPUDataLayout_Vulkan;

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
	GPUDataLayout_Vulkan* typeLayout;

	DataLayoutMemberDescription();
	DataLayoutMemberDescription(std::string& memberName,
		size_t offset,
		size_t size,
		DataLayoutMemberType type,
		GPUDataLayout_Vulkan* typeLayout);
	DataLayoutMemberDescription(const DataLayoutMemberDescription& other);
	~DataLayoutMemberDescription();

	void operator=(const DataLayoutMemberDescription& other);
};

class GPUDataLayout_Vulkan
{
public:
	GPUDataLayout_Vulkan();
	GPUDataLayout_Vulkan(std::string& name, std::vector<DataLayoutMemberDescription>& members);
	GPUDataLayout_Vulkan(const GPUDataLayout_Vulkan& other);
	~GPUDataLayout_Vulkan();

	void operator=(const GPUDataLayout_Vulkan& other);
	bool operator==(const GPUDataLayout_Vulkan& other);

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
};

class GPUDataLayoutBuilder_Vulkan
{
public:
	GPUDataLayoutBuilder_Vulkan();
	~GPUDataLayoutBuilder_Vulkan();

	void SetName(std::string& name);
	void AppendMember(std::string& name, size_t size, DataLayoutMemberType type, GPUDataLayout_Vulkan* typeLayout);
	void AppendMember(std::string& name, size_t size, size_t offset, DataLayoutMemberType type, GPUDataLayout_Vulkan* typeLayout);
	GPUDataLayout_Vulkan Build();

private:
	std::string _name;
	std::vector<DataLayoutMemberDescription> _members;
};

}