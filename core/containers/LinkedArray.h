#pragma once
#include "../CSEAssert.h"

namespace CSECore
{

template<typename Type, size_t BlockElementCount>
class LinkedArray
{
	struct Entry
	{
		Type data;
		Entry* prev;
		Entry* next;
	};

public:

	class Iterator
	{
	public:
		Iterator(Entry* ptr);
		~Iterator();

		Type& operator*();
		Type* operator->();

		Iterator& operator++(int);
		Iterator& operator--(int);

		bool operator==(const Iterator& other);
		bool operator!=(const Iterator& other);

	private:
		Entry* _ptr;
	};

	LinkedArray();
	~LinkedArray();

	Type* Add(const Type& data);
	void Remove(Type* data);
	void Remove(const Type& data);

	Iterator Begin();
	Iterator End();

private:
	Entry* _dataBlock;
	Entry* _activeListHead;
	Entry* _activeListTail;
	Entry* _freeListHead;
	size_t _count;

	void _allocateBlock();
};

template<typename Type, size_t BlockElementCount>
LinkedArray<Type, BlockElementCount>::LinkedArray()
	: _dataBlock(nullptr), 
	_activeListHead(nullptr),
	_activeListTail(nullptr),
	_freeListHead(nullptr)
{

}

template<typename Type, size_t BlockElementCount>
LinkedArray<Type, BlockElementCount>::~LinkedArray()
{

}

template<typename Type, size_t BlockElementCount>
Type* LinkedArray<Type, BlockElementCount>::Add(const Type& data)
{
	CSE_ASSERT(_count != BlockElementCount, "Can't add to LinkedArray as it has reached full capacity. Replace with chained group allocation later.");

	if (_dataBlock == nullptr)
	{
		_allocateBlock();
	}

	// Get new entry from head of free list and assign data
	Entry* newEntry = _freeListHead;
	newEntry->data = data;

	// Bump free list head to next entry
	Entry* newFreeListHead = _freeListHead->next;
	if (newFreeListHead != nullptr)
	{
		newFreeListHead->prev = nullptr;
	}
	_freeListHead = newFreeListHead;

	// Initialize active list if not done so
	if (_activeListHead == nullptr && _activeListTail == nullptr)
	{
		_activeListHead = newEntry;
		_activeListTail = newEntry;
		newEntry->prev = nullptr;
		newEntry->next = nullptr;
	}

	// Append new element to end after the tail otherwise
	else if (_activeListHead != nullptr && _activeListTail != nullptr)
	{
		newEntry->prev = _activeListTail;
		newEntry->next = nullptr;
		_activeListTail->next = newEntry;
		_activeListTail = newEntry;
	}

	++_count;
	return &newEntry->data;
}

template<typename Type, size_t BlockElementCount>
void LinkedArray<Type, BlockElementCount>::Remove(Type* data)
{
	// do bounds check for given address and its next+prev addresses
	// after casting the Type* to an Entry*

	Entry* entry = reinterpret_cast<Entry*>(data);
	CSE_ASSERT(_dataBlock < entry && entry < _dataBlock + BlockElementCount, "Given data block is out of bounds.");

	// Patch up free list
	Entry* prev = entry->prev;
	Entry* next = entry->next;

	if (prev != nullptr)
	{
		prev->next = next;
	}

	if (next != nullptr)
	{
		next->prev = prev;
	}

	// Patch active list head or tail if needbe
	if (entry == _activeListHead)
	{
		_activeListHead = next;
	}

	if (entry == _activeListTail)
	{
		_activeListTail = prev;
	}

	// Make entry the new freelist head
	Entry* oldFreeListHead = _freeListHead;
	oldFreeListHead->prev = entry;
	entry->next = oldFreeListHead;
	entry->prev = nullptr;
	_freeListHead = entry;
}

template<typename Type, size_t BlockElementCount>
void LinkedArray<Type, BlockElementCount>::Remove(const Type& data)
{
	// iterate through list until a match is found
	// pass match into Remove(Type* data)
}

template<typename Type, size_t BlockElementCount>
void LinkedArray<Type, BlockElementCount>::_allocateBlock()
{
	if (_dataBlock != nullptr)
	{
		delete _dataBlock;
	}

	_dataBlock = new Entry[BlockElementCount];
	_activeListHead = nullptr;
	_activeListTail = nullptr;
	_count = 0;

	// Initialize free list
	_freeListHead = _dataBlock;
	for (int i = 0; i < BlockElementCount; i++)
	{
		if (i != 0)
		{
			_dataBlock[i].prev = &_dataBlock[i - 1];
		}

		if (i != BlockElementCount - 1)
		{
			_dataBlock[i].next = &_dataBlock[i + 1];
		}
	}
}

template<typename Type, size_t BlockElementCount>
typename LinkedArray<Type, BlockElementCount>::Iterator LinkedArray<Type, BlockElementCount>::Begin()
{
	return Iterator(_activeListHead);
}

template<typename Type, size_t BlockElementCount>
typename LinkedArray<Type, BlockElementCount>::Iterator LinkedArray<Type, BlockElementCount>::End()
{
	return Iterator(nullptr);
}

template<typename Type, size_t BlockElementCount>
LinkedArray<Type, BlockElementCount>::Iterator::Iterator(Entry* ptr)
	: _ptr(ptr)
{

}

template<typename Type, size_t BlockElementCount>
LinkedArray<Type, BlockElementCount>::Iterator::~Iterator()
{

}

template<typename Type, size_t BlockElementCount>
Type& LinkedArray<Type, BlockElementCount>::Iterator::operator*()
{
	CSE_ASSERT(_ptr != nullptr, "Can't dereference a LinkedArray iterator with a nullptr");
	return _ptr->data;
}

template<typename Type, size_t BlockElementCount>
Type* LinkedArray<Type, BlockElementCount>::Iterator::operator->()
{
	CSE_ASSERT(_ptr != nullptr, "Can't access a LinkedArray iterator value with a nullptr");
	return &_ptr->data;
}

template<typename Type, size_t BlockElementCount>
typename LinkedArray<Type, BlockElementCount>::Iterator& LinkedArray<Type, BlockElementCount>::Iterator::operator++(int)
{
	_ptr = _ptr->next;
	return *this;
}

template<typename Type, size_t BlockElementCount>
typename LinkedArray<Type, BlockElementCount>::Iterator& LinkedArray<Type, BlockElementCount>::Iterator::operator--(int)
{
	_ptr = _ptr->prev;
	return *this;
}

template<typename Type, size_t BlockElementCount>
bool LinkedArray<Type, BlockElementCount>::Iterator::operator==(const Iterator& other)
{
	return _ptr == other._ptr;
}

template<typename Type, size_t BlockElementCount>
bool LinkedArray<Type, BlockElementCount>::Iterator::operator!=(const Iterator& other)
{
	return _ptr != other._ptr;
}

}