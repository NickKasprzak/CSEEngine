#include "GPUDataLayout_Vulkan.h"
#include "Logger.h"

namespace CSERenderer
{

uint32_t CalculateMemberAlignedOffset(DataLayoutMemberType type, uint32_t currentOffset);

DataLayoutMemberDescription::DataLayoutMemberDescription()
	: memberName(), memberHashID(UINT32_MAX), offset(0), size(0), type(MEMBER_TYPE_NULL), typeLayout(nullptr)
{

}

DataLayoutMemberDescription::DataLayoutMemberDescription(std::string& memberName,
	size_t offset,
	size_t size,
	DataLayoutMemberType type,
	GPUDataLayout_Vulkan* typeLayout)
	: memberName(memberName), memberHashID(UINT32_MAX), offset(offset), size(size), type(type), typeLayout(nullptr)
{
	CSE_ASSERT(((type == MEMBER_TYPE_STRUCT || type == MEMBER_TYPE_ARRAY) && typeLayout != nullptr) || (typeLayout == nullptr),
		"Invalid parameters given for DataLayoutMemberDescription constructor.");

	// owning temporarily until layout table is created
	if (typeLayout != nullptr)
	{
		this->typeLayout = (new GPUDataLayout_Vulkan(*typeLayout));
	}

	uint32_t hash = 0;
	hash = hash ^ std::hash<std::string>{}(memberName);
	hash = hash ^ std::hash<size_t>{}(offset);
	hash = hash ^ std::hash<size_t>{}(size);
	hash = hash ^ std::hash<DataLayoutMemberType>{}(type);
	if (typeLayout != nullptr)
	{
		hash = hash ^ typeLayout->GetHashID();
	}
	memberHashID = hash;
}

DataLayoutMemberDescription::DataLayoutMemberDescription(const DataLayoutMemberDescription& other)
	: memberName(other.memberName), memberHashID(other.memberHashID), offset(other.offset), size(other.size), type(other.type), typeLayout(nullptr)
{
	if (other.typeLayout != nullptr)
	{
		this->typeLayout = (new GPUDataLayout_Vulkan(*other.typeLayout));
	}
}

DataLayoutMemberDescription::~DataLayoutMemberDescription()
{
	if (typeLayout != nullptr)
	{
		delete typeLayout;
	}
}

void DataLayoutMemberDescription::operator=(const DataLayoutMemberDescription& other)
{
	if (typeLayout != nullptr)
	{
		delete typeLayout;
	}

	memberName = other.memberName;
	memberHashID = other.memberHashID;
	offset = other.offset;
	type = other.type;
	size = other.size;
	typeLayout = new GPUDataLayout_Vulkan(*other.typeLayout);
}

GPUDataLayout_Vulkan::GPUDataLayout_Vulkan()
	: _name(), _hashID(UINT32_MAX), _size(0), _members()
{

}

GPUDataLayout_Vulkan::GPUDataLayout_Vulkan(std::string& name, std::vector<DataLayoutMemberDescription>& members)
	: _name(name), _hashID(UINT32_MAX), _size(0), _members(members)
{
	if (members.size() != 0)
	{
		_size = members[members.size() - 1].offset + members[members.size() - 1].size;
	}

	uint32_t hash = 0;
	hash = hash ^ std::hash<std::string>{}(name);
	hash = hash ^ std::hash<size_t>{}(_size);
	for (int i = 0; i < _members.size(); i++)
	{
		hash = hash ^ _members[i].memberHashID;
	}
	_hashID = hash;
}

GPUDataLayout_Vulkan::GPUDataLayout_Vulkan(const GPUDataLayout_Vulkan& other)
	: _name(other._name), _hashID(other._hashID), _size(other._size), _members(other._members)
{

}

GPUDataLayout_Vulkan::~GPUDataLayout_Vulkan()
{

}

void GPUDataLayout_Vulkan::operator=(const GPUDataLayout_Vulkan& other)
{
	_name = other._name;
	_hashID = other._hashID;
	_size = other._size;
	_members = other._members;
}

bool GPUDataLayout_Vulkan::operator==(const GPUDataLayout_Vulkan& other)
{
	return _hashID == other._hashID;
}

const std::string& GPUDataLayout_Vulkan::GetName() const
{
	return _name;
}

uint32_t GPUDataLayout_Vulkan::GetHashID() const
{
	return _hashID;
}

size_t GPUDataLayout_Vulkan::GetSize() const
{
	return _size;
}

CSECore::Expected<const DataLayoutMemberDescription*, std::string> GPUDataLayout_Vulkan::GetMember(const std::string& memberName) const
{
	for (int i = 0; i < _members.size(); i++)
	{
		if (_members[i].memberName == memberName)
		{
			return CSECore::CreateExpected<const DataLayoutMemberDescription*, std::string>(&_members[i]);
		}
	}

	return CSECore::CreateUnexpected<const DataLayoutMemberDescription*, std::string>("Member of name " + memberName + " couldn't be found.");
}

const std::vector<DataLayoutMemberDescription>& GPUDataLayout_Vulkan::GetMembers() const
{
	return _members;
}

GPUDataLayoutBuilder_Vulkan::GPUDataLayoutBuilder_Vulkan()
	: _name(), _members()
{

}

GPUDataLayoutBuilder_Vulkan::~GPUDataLayoutBuilder_Vulkan()
{

}

void GPUDataLayoutBuilder_Vulkan::SetName(std::string& name)
{
	_name = name;
}

void GPUDataLayoutBuilder_Vulkan::AppendMember(std::string& name, size_t size, DataLayoutMemberType type, GPUDataLayout_Vulkan* typeLayout)
{
	uint32_t currentOffset = (_members.size() == 0) ? 0 : _members[_members.size() - 1].offset + _members[_members.size() - 1].size;
	uint32_t offset = CalculateMemberAlignedOffset(type, currentOffset);

	_members.push_back(DataLayoutMemberDescription(name, offset, size, type, typeLayout));
}

void GPUDataLayoutBuilder_Vulkan::AppendMember(std::string& name, size_t size, size_t offset, DataLayoutMemberType type, GPUDataLayout_Vulkan* typeLayout)
{
	_members.push_back(DataLayoutMemberDescription(name, offset, size, type, typeLayout));
}

GPUDataLayout_Vulkan GPUDataLayoutBuilder_Vulkan::Build()
{
	CSE_ASSERT(_name.length() != 0, "Can't build a GPUDataLayout without giving it a name.");
	CSE_ASSERT(_members.size() != 0, "Can't build a GPUDataLayout without giving it any members.");

	return GPUDataLayout_Vulkan(_name, _members);
}

uint32_t CalculateMemberAlignedOffset(DataLayoutMemberType type, uint32_t currentOffset)
{
	uint32_t requiredAlignment = 0;
	switch (type)
	{
	case MEMBER_TYPE_INT:
		requiredAlignment = 4;
		break;
	case MEMBER_TYPE_FLOAT:
		requiredAlignment = 4;
		break;
	case MEMBER_TYPE_VEC3:
		requiredAlignment = 16;
		break;
	case MEMBER_TYPE_VEC4:
		requiredAlignment = 16;
		break;
	case MEMBER_TYPE_MAT3:
		requiredAlignment = 16; // ?
		break;
	case MEMBER_TYPE_MAT4:
		requiredAlignment = 16; // ?
		break;
	case MEMBER_TYPE_STRUCT:
		requiredAlignment = 16; // ?
		break;
	case MEMBER_TYPE_ARRAY:
		requiredAlignment = 16; // ?
		break;
	default:
		requiredAlignment = 16;
		break;
	}

	uint32_t alignOffset = (currentOffset == 0) ? 0 : requiredAlignment - (currentOffset % requiredAlignment);
	return currentOffset + alignOffset;
}

}