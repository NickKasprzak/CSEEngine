#include "GPUDataLayout.h"
#include "Logger.h"
#include "Hash.h"

namespace CSERenderer
{

DataLayoutMemberDescription::DataLayoutMemberDescription()
	: memberName(), memberHashID(UINT32_MAX), offset(0), size(0), type(MEMBER_TYPE_NULL), typeLayout(nullptr)
{

}

DataLayoutMemberDescription::DataLayoutMemberDescription(std::string& memberName,
	size_t offset,
	size_t size,
	DataLayoutMemberType type,
	GPUDataLayout* typeLayout)
	: memberName(memberName), memberHashID(UINT32_MAX), offset(offset), size(size), type(type), typeLayout(nullptr)
{
	CSE_ASSERT(((type == MEMBER_TYPE_STRUCT || type == MEMBER_TYPE_ARRAY) && typeLayout != nullptr) || (typeLayout == nullptr),
		"Invalid parameters given for DataLayoutMemberDescription constructor.");

	// owning temporarily until layout table is created
	if (typeLayout != nullptr)
	{
		this->typeLayout = (new GPUDataLayout(*typeLayout));
	}

	uint32_t hash = 0;
	hash = hash ^ CSECore::FNVHash<std::string>(memberName);
	hash = hash ^ CSECore::FNVHash<size_t>(offset);
	hash = hash ^ CSECore::FNVHash<size_t>(size);
	hash = hash ^ CSECore::FNVHash<DataLayoutMemberType>(type);
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
		this->typeLayout = (new GPUDataLayout(*other.typeLayout));
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
	typeLayout = new GPUDataLayout(*other.typeLayout);
}

GPUDataLayout::GPUDataLayout()
	: _name(), _hashID(UINT32_MAX), _size(0), _members()
{

}

GPUDataLayout::GPUDataLayout(std::string& name, std::vector<DataLayoutMemberDescription>& members)
	: _name(name), _hashID(UINT32_MAX), _size(0), _members(members)
{
	if (members.size() != 0)
	{
		_size = members[members.size() - 1].offset + members[members.size() - 1].size;
	}

	uint32_t hash = 0;
	hash = hash ^ CSECore::FNVHash<std::string>(name);
	hash = hash ^ CSECore::FNVHash<size_t>(_size);
	for (int i = 0; i < _members.size(); i++)
	{
		hash = hash ^ _members[i].memberHashID;
	}
	_hashID = hash;
}

GPUDataLayout::GPUDataLayout(const GPUDataLayout& other)
	: _name(other._name), _hashID(other._hashID), _size(other._size), _members(other._members)
{

}

GPUDataLayout::~GPUDataLayout()
{

}

void GPUDataLayout::operator=(const GPUDataLayout& other)
{
	_name = other._name;
	_hashID = other._hashID;
	_size = other._size;
	_members = other._members;
}

const std::string& GPUDataLayout::GetName() const
{
	return _name;
}

uint32_t GPUDataLayout::GetHashID() const
{
	return _hashID;
}

size_t GPUDataLayout::GetSize() const
{
	return _size;
}

CSECore::Expected<const DataLayoutMemberDescription*, std::string> GPUDataLayout::GetMember(const std::string& memberName) const
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

const std::vector<DataLayoutMemberDescription>& GPUDataLayout::GetMembers() const
{
	return _members;
}

GPUDataLayoutBuilder::GPUDataLayoutBuilder()
	: _name(), _members()
{

}

GPUDataLayoutBuilder::~GPUDataLayoutBuilder()
{

}

void GPUDataLayoutBuilder::SetName(std::string& name)
{
	_name = name;
}

void GPUDataLayoutBuilder::AppendMember(std::string& name, size_t size, size_t offset, DataLayoutMemberType type, GPUDataLayout* typeLayout)
{
	_members.push_back(DataLayoutMemberDescription(name, offset, size, type, typeLayout));
}

GPUDataLayout GPUDataLayoutBuilder::Build()
{
	CSE_ASSERT(_name.length() != 0, "Can't build a GPUDataLayout without giving it a name.");
	CSE_ASSERT(_members.size() != 0, "Can't build a GPUDataLayout without giving it any members.");

	return GPUDataLayout(_name, _members);
}

GPUDataLayoutRef::GPUDataLayoutRef()
	: _layout()
{

}

GPUDataLayoutRef::GPUDataLayoutRef(const GPUDataLayout& layout)
	: _layout(layout)
{

}

GPUDataLayoutRef::~GPUDataLayoutRef()
{

}

void GPUDataLayoutRef::operator=(const GPUDataLayoutRef& other)
{
	_layout = other._layout;
}

const GPUDataLayout& GPUDataLayoutRef::GetLayout()
{
	return _layout;
}

}