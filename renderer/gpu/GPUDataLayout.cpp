#include "GPUDataLayout.h"
#include "Logger.h"
#include "Hash.h"
#include "memory/MemoryUtils.h"

namespace CSERenderer
{


DataLayoutMemberDescription::DataLayoutMemberDescription()
	: _layoutHash(UINT32_MAX), _nameHash(UINT32_MAX), _size(0), _offset(0), _arrayLength(0), _type(MEMBER_TYPE_NULL), _typeLayout(nullptr)
{

}

DataLayoutMemberDescription::DataLayoutMemberDescription(std::string& memberName,
	size_t size,
	size_t offset,
	uint32_t arrayLength,
	DataLayoutMemberType type,
	GPUDataLayout* typeLayout)
	: _layoutHash(UINT32_MAX), _nameHash(CSECore::FNVHash(memberName)), _size(size), _offset(offset), _arrayLength(arrayLength), _type(type), _typeLayout(nullptr)
{
	CSE_ASSERT((type == MEMBER_TYPE_STRUCT && typeLayout != nullptr) || (typeLayout == nullptr),
		"Invalid parameters given for DataLayoutMemberDescription constructor.");

	if (typeLayout != nullptr)
	{
		_typeLayout = (new GPUDataLayout(*typeLayout));
	}

	uint32_t layoutHash = 0;
	layoutHash ^= _nameHash;
	layoutHash ^= CSECore::FNVHash(_size);
	layoutHash ^= CSECore::FNVHash(_offset);
	layoutHash ^= CSECore::FNVHash(_arrayLength);
	layoutHash ^= CSECore::FNVHash(_type);
	if (_typeLayout != nullptr)
	{
		layoutHash ^= _typeLayout->GetLayoutHash();
	}
}

DataLayoutMemberDescription::DataLayoutMemberDescription(const DataLayoutMemberDescription& other)
	: _layoutHash(other._layoutHash), _nameHash(other._nameHash), _size(other._size), _offset(other._offset), _arrayLength(other._arrayLength), _type(other._type), _typeLayout(nullptr)
{
	if (other._typeLayout != nullptr)
	{
		_typeLayout = (new GPUDataLayout(*other._typeLayout));
	}
}

DataLayoutMemberDescription::~DataLayoutMemberDescription()
{
	if (_typeLayout != nullptr)
	{
		delete _typeLayout;
	}
}

void DataLayoutMemberDescription::operator=(const DataLayoutMemberDescription& other)
{
	if (_typeLayout != nullptr)
	{
		delete _typeLayout;
	}

	_layoutHash = other._layoutHash;
	_nameHash = other._nameHash;
	_size = other._size;
	_offset = other._offset;
	_arrayLength = other._arrayLength;
	_type = other._type;
	
	if (other._typeLayout != nullptr)
	{
		_typeLayout = new GPUDataLayout(*other._typeLayout);
	}
}

uint32_t DataLayoutMemberDescription::GetLayoutHash() const
{
	return _layoutHash;
}

uint32_t DataLayoutMemberDescription::GetNameHash() const
{
	return _nameHash;
}

size_t DataLayoutMemberDescription::GetSize() const
{
	return _size;
}

size_t DataLayoutMemberDescription::GetOffset() const
{
	return _offset;
}

uint32_t DataLayoutMemberDescription::GetArrayLength() const
{
	return _arrayLength;
}

DataLayoutMemberType DataLayoutMemberDescription::GetType() const
{
	return _type;
}

GPUDataLayout* DataLayoutMemberDescription::GetLayout() const
{
	return _typeLayout;
}

GPUDataLayout::GPUDataLayout()
	: _layoutHash(UINT32_MAX), _nameHash(UINT32_MAX), _size(0), _alignment(0), _members()
{

}

GPUDataLayout::GPUDataLayout(std::string& name, size_t size, size_t alignment, std::vector<DataLayoutMemberDescription>& members)
	: _layoutHash(UINT32_MAX), _nameHash(CSECore::FNVHash(name)), _size(size), _alignment(alignment), _members(members)
{
	uint32_t layoutHash = 0;
	layoutHash ^= _nameHash;
	layoutHash ^= _size;
	layoutHash ^= _alignment;
	for (int i = 0; i < _members.size(); i++)
	{
		layoutHash ^= _members[i].GetLayoutHash();
	}
	_layoutHash = layoutHash;
}

GPUDataLayout::GPUDataLayout(const GPUDataLayout& other)
	: _layoutHash(other._layoutHash), _nameHash(other._nameHash), _size(other._size), _alignment(other._alignment), _members(other._members)
{

}

GPUDataLayout::~GPUDataLayout()
{

}

void GPUDataLayout::operator=(const GPUDataLayout& other)
{
	_layoutHash = other._layoutHash;
	_nameHash = other._nameHash;
	_size = other._size;
	_alignment = other._alignment;
	_members = other._members;
}

uint32_t GPUDataLayout::GetLayoutHash() const
{
	return _layoutHash;
}

uint32_t GPUDataLayout::GetNameHash() const
{
	return _nameHash;
}

size_t GPUDataLayout::GetSize() const
{
	return _size;
}

size_t GPUDataLayout::GetAlignment() const
{
	return _alignment;
}

CSECore::Expected<const DataLayoutMemberDescription*, std::string> GPUDataLayout::GetMember(uint32_t nameHash) const
{
	for (int i = 0; i < _members.size(); i++)
	{
		if (_members[i].GetNameHash() == nameHash)
		{
			return CSECore::CreateExpected<const DataLayoutMemberDescription*, std::string>(&_members[i]);
		}
	}

	return CSECore::CreateUnexpected<const DataLayoutMemberDescription*, std::string>("Member couldn't be found.");
}

const std::vector<DataLayoutMemberDescription>& GPUDataLayout::GetMembers() const
{
	return _members;
}

GPUDataLayoutView::GPUDataLayoutView()
	: _rootLayoutHash(UINT32_MAX), _layout(nullptr), _memberDesc(nullptr), _type(MEMBER_TYPE_NULL), _offset(0)
{

}

GPUDataLayoutView::GPUDataLayoutView(const GPUDataLayout* layout)
	: _rootLayoutHash(layout->GetLayoutHash()), _layout(layout), _memberDesc(nullptr), _type(MEMBER_TYPE_NULL), _offset(0)
{

}

GPUDataLayoutView::GPUDataLayoutView(uint32_t rootHash, const GPUDataLayout* layout, const DataLayoutMemberDescription* memberDesc, DataLayoutMemberType type, size_t offset)
	: _rootLayoutHash(rootHash), _layout(layout), _memberDesc(memberDesc), _type(type), _offset(offset)
{
	
}

GPUDataLayoutView::~GPUDataLayoutView()
{

}

GPUDataLayoutView GPUDataLayoutView::GetMember(uint32_t nameHash)
{
	if (_layout == nullptr)
	{
		CSE_LOGE("Can't get a data layout member if a layout isn't assigned.");
		return GPUDataLayoutView();
	}

	CSECore::Expected<const DataLayoutMemberDescription*, std::string> memberResult = _layout->GetMember(nameHash);
	if (memberResult.HasUnexpected())
	{
		CSE_LOGE(memberResult.GetUnexpected());
		return GPUDataLayoutView();
	}

	const DataLayoutMemberDescription* memDesc = memberResult.GetExpected();
	size_t offset = _offset + memDesc->GetOffset();
	DataLayoutMemberType type = memDesc->GetType();

	if (type == MEMBER_TYPE_STRUCT)
	{
		return GPUDataLayoutView(_rootLayoutHash, memDesc->GetLayout(), memDesc, type, offset);
	}

	else if (type != MEMBER_TYPE_STRUCT)
	{
		return GPUDataLayoutView(_rootLayoutHash, nullptr, memDesc, type, offset);
	}

	return GPUDataLayoutView();
}

GPUDataLayoutView GPUDataLayoutView::GetArrayIndex(uint32_t index)
{
	if (_memberDesc == nullptr)
	{
		CSE_LOGE("Can't get a specific array element if a member isn't assigned.");
		return GPUDataLayoutView();
	}

	else if (_memberDesc->GetArrayLength() < index)
	{
		CSE_LOGE("Provided array index is out of bounds.");
		return GPUDataLayoutView();
	}

	size_t offset = _offset + (_memberDesc->GetSize() * index);
	return GPUDataLayoutView(_rootLayoutHash, _layout, nullptr, _type, offset);
}

uint32_t GPUDataLayoutView::GetRootLayoutHash()
{
	return _rootLayoutHash;
}

size_t GPUDataLayoutView::GetOffset()
{
	return _offset;
}

DataLayoutMemberType GPUDataLayoutView::GetMemberType()
{
	return _type;
}

GPUDataLayoutBuilder::GPUDataLayoutBuilder()
	: _name(), _runningSize(0), _runningAlignment(0), _members(), _runtimeArrayAdded(false)
{

}

GPUDataLayoutBuilder::~GPUDataLayoutBuilder()
{

}

void GPUDataLayoutBuilder::SetName(std::string& name)
{
	_name = name;
}

void GPUDataLayoutBuilder::AppendPrimitiveMember(std::string& name, size_t size, size_t alignment, uint32_t arrayLength, DataLayoutMemberType type)
{
	CSE_ASSERT(type != MEMBER_TYPE_STRUCT && type != MEMBER_TYPE_NULL, "Given member type must not be STRUCT or NULL.");
	CSE_ASSERT(size != 0 && alignment != 0 && arrayLength != 0, "Size, alignment, and array length must all be greater than 1.");
	CSE_ASSERT(!_runtimeArrayAdded, "Can't add additional members after adding a runtime array.");

	size_t alignedOffset = CSECore::AlignOffset(_runningSize, alignment);

	if (arrayLength != RUNTIME_ARRAY_LENGTH)
	{
		_runningSize = alignedOffset + (size * arrayLength);
	}
	else if (arrayLength == RUNTIME_ARRAY_LENGTH)
	{
		_runtimeArrayAdded = true;
		_runningSize = alignedOffset + size;
	}

	if (_runningAlignment < alignment)
	{
		_runningAlignment = alignment;
	}

	_members.push_back(DataLayoutMemberDescription(name, size, alignedOffset, arrayLength, type, nullptr));
}

void GPUDataLayoutBuilder::AppendStructMember(std::string& name, uint32_t arrayLength, GPUDataLayout* typeLayout)
{
	CSE_ASSERT(typeLayout != nullptr, "Must provide a type layout for a struct member.");
	CSE_ASSERT(typeLayout->GetSize() != 0 && typeLayout->GetAlignment() != 0 && arrayLength != 0, "Size, alignment, and array length must all be greater than 1.");
	CSE_ASSERT(!_runtimeArrayAdded, "Can't add additional members after adding a runtime array.");

	size_t alignedOffset = CSECore::AlignOffset(_runningSize, typeLayout->GetAlignment());

	if (arrayLength != RUNTIME_ARRAY_LENGTH)
	{
		_runningSize = alignedOffset + (typeLayout->GetSize() * arrayLength);
	}
	else if (arrayLength == RUNTIME_ARRAY_LENGTH)
	{
		_runtimeArrayAdded = true;
		_runningSize = alignedOffset + typeLayout->GetSize();
	}

	if (_runningAlignment < typeLayout->GetAlignment())
	{
		_runningAlignment = typeLayout->GetAlignment();
	}

	_members.push_back(DataLayoutMemberDescription(name, typeLayout->GetSize(), alignedOffset, arrayLength, MEMBER_TYPE_STRUCT, typeLayout));
}

GPUDataLayout GPUDataLayoutBuilder::Build()
{
	CSE_ASSERT(_name.length() != 0, "Can't build a GPUDataLayout without giving it a name.");
	CSE_ASSERT(_members.size() != 0, "Can't build a GPUDataLayout without giving it any members.");

	size_t paddedSize = CSECore::AlignOffset(_runningSize, _runningAlignment);

	return GPUDataLayout(_name, paddedSize, _runningAlignment, _members);
}

GPUDataLayoutView GetLayoutView(GPUDataLayout* layout, std::initializer_list<LayoutViewNavNode> path)
{
	GPUDataLayoutView view(layout);
	auto arr = path.begin();
	for (int i = 0; i < path.size(); i++)
	{
		auto elem = arr[i];

		if (elem.type == NAV_MEMBER)
		{
			view = view.GetMember(elem.value);
		}
		else if (elem.type == NAV_ARRAY)
		{
			view = view.GetArrayIndex(elem.value);
		}
	}
	return view;
}

}