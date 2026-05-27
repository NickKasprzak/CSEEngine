#pragma once
#include "../CSEAssert.h"
#include <cstdint>
#include <algorithm>

namespace CSECore
{

template<typename Type, size_t BlockElementCount>
class SkipArray
{
private:
	static constexpr int32_t NULL_INDEX = INT32_MAX;

	struct Entry
	{
		union EntryUnion
		{
			struct Active
			{
				Type data;
				int32_t skipNext;
				int32_t skipPrev;
			} active;

			struct Inactive
			{
				int32_t blockSize;
				int32_t nextIndex;
				int32_t prevIndex;
			} inactive;

			~EntryUnion() {}
		} entryUnion;

		bool isActive;

		~Entry() {}
	};

	class EntryMemoryBlock
	{
	public:
		EntryMemoryBlock();
		~EntryMemoryBlock()
		{
			if (HasAllocation())
			{
				DeallocateMemory();
			}
		}

		void AllocateMemory();
		void DeallocateMemory();
		bool HasAllocation();

		Entry& operator[](uint32_t index);

	private:
		void* _memory;
	};

public:

	class Iterator
	{
	public:
		Iterator(Entry* src, int32_t startIndex);
		~Iterator();

		Type& operator*();
		Type* operator->();

		Iterator& operator++(int);
		Iterator& operator--(int);

		Type* GetPointer();

	private:
		Entry* _src;
		int32_t _index;

		friend bool operator==(const Iterator& lhs, const Iterator& rhs)
		{
			return (lhs._src == rhs._src) && (lhs._index == rhs._index);
		}

		friend bool operator!=(const Iterator& lhs, const Iterator& rhs)
		{
			return (lhs._src != rhs._src) && (lhs._index != rhs._index);
		}
	};

	SkipArray();
	~SkipArray();

	template<typename U = Type, 
		typename std::enable_if<std::is_copy_assignable_v<U>, int>::type = 0>
	int32_t Add(const Type& data);
	int32_t Add(Type&& data);
	void Remove(Type* data);
	void Remove(const Type& data);
	void RemoveAtIndex(int32_t index);
	void Clear();

	Type* GetAtIndex(int32_t index);

	Iterator Begin();
	Iterator End();

private:
	EntryMemoryBlock _memory;
	int32_t _activeListHead;
	int32_t _activeListTail;
	int32_t _freeListHead;
	int32_t _activeCount;

	void _allocateMemory();

	int32_t _allocateFromFreeList();
	void _deallocateToFreeList(int32_t index);
	void _addFreeListBlock(int32_t index);
	void _removeFreeListBlock(int32_t index);

	void _appendFreeListBlock(int32_t index);
	void _mergeFreeListBlocks(int32_t expandedIndex, int32_t absorbedIndex);
	void _growFreeListBlock(int32_t index, int32_t range);
	void _shrinkFreeListBlock(int32_t index, int32_t range);

	int32_t _addImpl(const Type& data, std::true_type);
	int32_t _addImpl(const Type& data, std::false_type);
};

template<typename Type, size_t BlockElementCount>
SkipArray<Type, BlockElementCount>::SkipArray()
	: _memory(), 
	_activeListHead(NULL_INDEX), 
	_activeListTail(NULL_INDEX), 
	_freeListHead(0),
	_activeCount(0)
{

}

template<typename Type, size_t BlockElementCount>
SkipArray<Type, BlockElementCount>::~SkipArray()
{
	if (!_memory.HasAllocation()) { return; }

	if (_activeListHead != NULL_INDEX)
	{
		// Iterate through and deconstruct the active list
		int32_t index = _activeListHead;
		int32_t nextSkip = 0;
		while (nextSkip != NULL_INDEX)
		{
			index += nextSkip;
			_memory[index].entryUnion.active.data.Type::~Type();
			nextSkip = _memory[index].entryUnion.active.skipNext;
		}
	}

	_memory.DeallocateMemory();
	_activeListHead = NULL_INDEX;
	_activeListTail = NULL_INDEX;
	_freeListHead = 0;
	_activeCount = 0;
}

template<typename Type, size_t BlockElementCount>
template<typename U, typename std::enable_if<std::is_copy_assignable_v<U>, int>::type>
int32_t SkipArray<Type, BlockElementCount>::Add(const Type& data)
{
	return _addImpl(data, std::is_copy_assignable<Type>::type{});
}

template<typename Type, size_t BlockElementCount>
int32_t SkipArray<Type, BlockElementCount>::Add(Type&& data)
{
	if (!_memory.HasAllocation())
	{
		_allocateMemory();
	}

	int32_t entryIndex = _allocateFromFreeList();
	if (entryIndex == NULL_INDEX)
	{
		return -1;
	}

	Type* entryData = new (&_memory[entryIndex].entryUnion.active.data) Type();
	*entryData = std::move(data);
	return entryIndex;
}

template<typename Type, size_t BlockElementCount>
void SkipArray<Type, BlockElementCount>::Remove(Type* data)
{
	if (!_memory.HasAllocation())
	{
		return;
	}

	uintptr_t memUIntptr = reinterpret_cast<uintptr_t>(&_memory[0]);
	uintptr_t dataUIntptr = reinterpret_cast<uintptr_t>(data);
	CSE_ASSERT(memUIntptr <= dataUIntptr && dataUIntptr < memUIntptr + (sizeof(Entry) * BlockElementCount), "Given address is out of bounds");
	CSE_ASSERT(dataUIntptr % sizeof(Entry) == 0, "Given address does not align with an Entry.");

	int32_t removeIndex = (dataUIntptr - memUIntptr) / sizeof(Entry);
	_deallocateToFreeList(removeIndex);
}

template<typename Type, size_t BlockElementCount>
void SkipArray<Type, BlockElementCount>::Remove(const Type& data)
{
	if (!_memory.HasAllocation())
	{
		return;
	}

	Iterator iter = Begin();
	Iterator end = End();
	while (iter != end && *iter != data)
	{
		iter++;
	}

	if (iter != end)
	{
		Remove(iter.GetPointer());
	}
}

template<typename Type, size_t BlockElementCount>
void SkipArray<Type, BlockElementCount>::RemoveAtIndex(int32_t index)
{
	CSE_ASSERT(index >= 0 && index < BlockElementCount, "Given index for removal is out of bounds.");

	_deallocateToFreeList(index);
}

template<typename Type, size_t BlockElementCount>
void SkipArray<Type, BlockElementCount>::Clear()
{
	SkipArray<Type, BlockElementCount>::~SkipArray<Type, BlockElementCount>();
}

template<typename Type, size_t BlockElementCount>
Type* SkipArray<Type, BlockElementCount>::GetAtIndex(int32_t index)
{
	CSE_ASSERT(index >= 0 && index < BlockElementCount, "Given index for access is out of bounds.");

	if (!_memory[index].isActive)
	{
		return nullptr;
	}

	return &_memory[index].entryUnion.active.data;
}

template<typename Type, size_t BlockElementCount>
typename SkipArray<Type, BlockElementCount>::Iterator SkipArray<Type, BlockElementCount>::Begin()
{
	return Iterator(&_memory[0], _activeListHead);
}

template<typename Type, size_t BlockElementCount>
typename SkipArray<Type, BlockElementCount>::Iterator SkipArray<Type, BlockElementCount>::End()
{
	return Iterator(&_memory[0], NULL_INDEX);
}

template<typename Type, size_t BlockElementCount>
void SkipArray<Type, BlockElementCount>::_allocateMemory()
{
	if (_memory.HasAllocation())
	{
		SkipArray::~SkipArray();
	}

	_memory.AllocateMemory();
	_activeListHead = NULL_INDEX;
	_freeListHead = 0;
	_activeCount = 0;

	_memory[0].entryUnion.inactive.blockSize = BlockElementCount;
	_memory[0].entryUnion.inactive.nextIndex = NULL_INDEX;
	_memory[0].entryUnion.inactive.prevIndex = NULL_INDEX;
	_memory[0].isActive = false;

	_memory[BlockElementCount - 1].entryUnion.inactive.blockSize = BlockElementCount - 1;
}

template<typename Type, size_t BlockElementCount>
int32_t SkipArray<Type, BlockElementCount>::_allocateFromFreeList()
{
	int32_t index = _freeListHead;
	if (index == NULL_INDEX)
	{
		return NULL_INDEX;
	}

	/*
	* Calculate element's skip fields.
	* 
	* Previous element will always be
	* one index prior to the block due
	* to free blocks always starting
	* after an active element.
	* 
	* Next element will always be after
	* the free block's inactive range.
	* 
	* Both will be NULL_INDEX if either
	* are OOB
	*/
	
	int32_t freeBlockSize = _memory[_freeListHead].entryUnion.inactive.blockSize;
	int32_t nextIndex = index + freeBlockSize;
	int32_t prevIndex = index - 1;

	int32_t skipNext = (nextIndex >= BlockElementCount - 1) ? NULL_INDEX : nextIndex - index;
	int32_t skipPrev = (prevIndex < 0) ? NULL_INDEX : prevIndex - index;

	/*
	* Update the free list by shrinking
	* the block from its head by an element
	*/
	_shrinkFreeListBlock(index, 1);

	/*
	* Insert the element into the active list
	* by updating the element and the elements
	* it can skip to with the proper skip fields
	*/
	_memory[index].entryUnion.active.skipNext = skipNext;
	_memory[index].entryUnion.active.skipPrev = skipPrev;
	_memory[index].isActive = true;

	if (skipNext != NULL_INDEX)
	{
		_memory[nextIndex].entryUnion.active.skipPrev = -skipNext;
	}

	if (skipPrev != NULL_INDEX)
	{
		_memory[prevIndex].entryUnion.active.skipNext = -skipPrev;
	}

	/*
	* Roll back active list head to
	* this element's index if its
	* earlier than the current head
	*/
	if (index < _activeListHead)
	{
		_activeListHead = index;
	}

	return index;
}

template<typename Type, size_t BlockElementCount>
void SkipArray<Type, BlockElementCount>::_deallocateToFreeList(int32_t index)
{
	if (!_memory[index].isActive)
	{
		return;
	}

	/*
	* Deconstruct the data in the
	* entry at the given index and
	* deactivate it.
	*/
	_memory[index].entryUnion.active.data.Type::~Type();
	_memory[index].isActive = false;

	/*
	* Patch up the active list by
	* updating the entries this element
	* can skip to with the proper skip
	* fields in its absense
	*/
	int32_t skipNext = _memory[index].entryUnion.active.skipNext;
	int32_t skipPrev = _memory[index].entryUnion.active.skipPrev;
	
	int32_t nextIndex = (skipNext == NULL_INDEX) ? NULL_INDEX : index + skipNext;
	int32_t prevIndex = (skipPrev == NULL_INDEX) ? NULL_INDEX : index + skipPrev;

	if (nextIndex != NULL_INDEX)
	{
		_memory[nextIndex].entryUnion.active.skipPrev = (prevIndex == NULL_INDEX) ? NULL_INDEX : prevIndex - nextIndex;
	}

	if (prevIndex != NULL_INDEX)
	{
		_memory[prevIndex].entryUnion.active.skipNext = (nextIndex == NULL_INDEX) ? NULL_INDEX : nextIndex - prevIndex;
	}

	if (index == _activeListHead)
	{
		_activeListHead = (prevIndex != NULL_INDEX) ? (prevIndex) : nextIndex;
	}

	/*
	* Attempt to add the given
	* block to the free list
	*/
	_addFreeListBlock(index);
}

template<typename Type, size_t BlockElementCount>
void SkipArray<Type, BlockElementCount>::_addFreeListBlock(int32_t index)
{
	// This all assumes that the block given is active

	/*
	* Determine whether there are active blocks
	* to the left and right of the active block
	* being removed and calculate their indices.
	*/
	bool activeOnLeft = (index <= 0) ? false : _memory[index - 1].isActive;
	bool activeOnRight = (index >= BlockElementCount - 1) ? false : _memory[index + 1].isActive;

	int32_t freeBlockLeft = (activeOnLeft || index <= 0) ? NULL_INDEX : index + _memory[index].entryUnion.active.skipPrev + 1;
	int32_t freeBlockRight = (activeOnRight || index >= BlockElementCount - 1) ? NULL_INDEX : index + 1;

	/*
	* Clear out the active entry to effectively
	* make it inactive.
	*/
	_memory[index].entryUnion.inactive.blockSize = 1;
	_memory[index].entryUnion.inactive.nextIndex = NULL_INDEX;
	_memory[index].entryUnion.inactive.prevIndex = NULL_INDEX;
	_memory[index].isActive = false;

	/*
	* If the element being removed has a free block
	* on its left and right, both blocks will have
	* to be merged to absorb the one being removed.
	*/
	if ((!activeOnLeft && freeBlockLeft != NULL_INDEX) && (!activeOnRight && freeBlockRight != NULL_INDEX))
	{
		_mergeFreeListBlocks(freeBlockLeft, freeBlockRight);
		return;
	}

	/*
	* If the element being removed has an active
	* element to its left but no active to its right,
	* the free block to its right will have to grow
	* back an element to absorb the one being removed.
	*/
	else if (activeOnLeft && (!activeOnRight && freeBlockRight != NULL_INDEX))
	{
		_growFreeListBlock(freeBlockRight, -1);
		return;
	}

	/*
	* If the element being removed has an active
	* element to its right but no active to its left,
	* it free block to its left will have to grow
	* forward an element to absorb the one being removed.
	*/
	else if ((!activeOnLeft && freeBlockLeft != NULL_INDEX) && activeOnRight)
	{
		_growFreeListBlock(freeBlockLeft, 1);
		return;
	}

	/*
	* If theres no room to grow any existing
	* free blocks, then the element being
	* removed will form a new one.
	*/
	_appendFreeListBlock(index);
}

template<typename Type, size_t BlockElementCount>
void SkipArray<Type, BlockElementCount>::_removeFreeListBlock(int32_t index)
{
	int32_t nextBlock = _memory[index].entryUnion.inactive.nextIndex;
	int32_t prevBlock = _memory[index].entryUnion.inactive.prevIndex;
		
	/*
	* Remove a block from the free list
	* by deferring any references to it
	* onto other blocks.
	*/

	if (_freeListHead == index)
	{
		_freeListHead = prevBlock;
	}

	if (nextBlock != NULL_INDEX)
	{
		_memory[nextBlock].entryUnion.inactive.prevIndex = prevBlock;
	}

	if (prevBlock != NULL_INDEX)
	{
		_memory[prevBlock].entryUnion.inactive.nextIndex = nextBlock;
	}
}

template<typename Type, size_t BlockElementCount>
void SkipArray<Type, BlockElementCount>::_appendFreeListBlock(int32_t index)
{
	if (_freeListHead == NULL_INDEX)
	{
		_freeListHead = index;
		return;
	}

	int32_t oldFreeHead = _freeListHead;
	_memory[oldFreeHead].entryUnion.inactive.nextIndex = index;

	_memory[index].entryUnion.inactive.nextIndex = NULL_INDEX;
	_memory[index].entryUnion.inactive.prevIndex = oldFreeHead;
	_memory[index].entryUnion.inactive.blockSize = 1;
	_freeListHead = index;
}

template<typename Type, size_t BlockElementCount>
void SkipArray<Type, BlockElementCount>::_mergeFreeListBlocks(int32_t index1, int32_t index2)
{
	/*
	* This process assumes the two blocks
	* being merged are contiguous in memory.
	*/
	
	/*
	* Pick whichever of the two indices occurs
	* earlier in the array to expand.
	*/
	int32_t expandedIndex = (std::min)(index1, index2);
	int32_t absorbedIndex = (std::max)(index1, index2);

	/*
	* Patch up free list by connecting the
	* absorbed block's next and previous blocks
	*/
	int32_t absorbedPrev = _memory[absorbedIndex].entryUnion.inactive.prevIndex;
	int32_t absorbedNext = _memory[absorbedIndex].entryUnion.inactive.nextIndex;
	
	if (absorbedPrev != NULL_INDEX)
	{
		_memory[absorbedPrev].entryUnion.inactive.nextIndex = absorbedNext;
	}

	if (absorbedNext != NULL_INDEX)
	{
		_memory[absorbedNext].entryUnion.inactive.prevIndex = absorbedPrev;
	}

	if (_freeListHead == absorbedIndex)
	{
		_freeListHead = absorbedPrev;
	}

	/*
	* Update the expanded block with its
	* new size after the merge.
	*/
	int32_t absorbedSize = _memory[absorbedIndex].entryUnion.inactive.blockSize;
	int32_t newSize = (absorbedIndex - expandedIndex) + absorbedSize;
	_memory[expandedIndex].entryUnion.inactive.blockSize = newSize;
}

template<typename Type, size_t BlockElementCount>
void SkipArray<Type, BlockElementCount>::_growFreeListBlock(int32_t index, int32_t range)
{
	int32_t blockSize = _memory[index].entryUnion.inactive.blockSize;
	int32_t nextBlock = _memory[index].entryUnion.inactive.nextIndex;
	int32_t prevBlock = _memory[index].entryUnion.inactive.prevIndex;

	/*
	* Given range can be both positive
	* and negative. Its signedness indicates
	* what direction it should grow in.
	* 
	* Positive indicates forward, meaning the
	* tail moves up
	* 
	* Negative indicates backward, meaning the
	* head moves back
	*/
	int32_t newBlockSize = blockSize + abs(range);
	int32_t newBlockIndex = (range < 0) ? (index - range) : index;

	_memory[newBlockIndex].entryUnion.inactive.blockSize = newBlockSize;
	_memory[newBlockIndex].entryUnion.inactive.nextIndex = nextBlock;
	_memory[newBlockIndex].entryUnion.inactive.prevIndex = prevBlock;

	if (_freeListHead == index)
	{
		_freeListHead = newBlockIndex;
	}

	if (nextBlock != NULL_INDEX)
	{
		_memory[nextBlock].entryUnion.inactive.prevIndex = newBlockIndex;
	}

	if (prevBlock != NULL_INDEX)
	{
		_memory[prevBlock].entryUnion.inactive.nextIndex = newBlockIndex;
	}
}

template<typename Type, size_t BlockElementCount>
void SkipArray<Type, BlockElementCount>::_shrinkFreeListBlock(int32_t index, int32_t range)
{
	int32_t blockSize = _memory[index].entryUnion.inactive.blockSize;
	int32_t nextBlock = _memory[index].entryUnion.inactive.nextIndex;
	int32_t prevBlock = _memory[index].entryUnion.inactive.prevIndex;

	/*
	* Given range can be both positive
	* and negative. Its signedness indicates
	* what direction is should shrink in.
	* 
	* Positive indicates forward, meaning the
	* head moves up
	* 
	* Negative indicates backward, meaning the
	* tail moves back
	*/
	int32_t newBlockSize = blockSize - abs(range);
	int32_t newBlockIndex = (range < 0) ? (index) : index + range;

	/*
	* If shrinking the block would result in
	* it being reduced to 0, remove it from the
	* free list.
	*/
	if (newBlockSize <= 0)
	{
		_removeFreeListBlock(index);
		return;
	}

	/*
	* Otherwise, update the block itself
	* and redirect anything referencing it
	* to its new location.
	*/
	_memory[newBlockIndex].entryUnion.inactive.blockSize = newBlockSize;
	_memory[newBlockIndex].entryUnion.inactive.nextIndex = nextBlock;
	_memory[newBlockIndex].entryUnion.inactive.prevIndex = prevBlock;

	if (_freeListHead == index)
	{
		_freeListHead = newBlockIndex;
	}

	if (nextBlock != NULL_INDEX)
	{
		_memory[nextBlock].entryUnion.inactive.prevIndex = newBlockIndex;
	}

	if (prevBlock != NULL_INDEX)
	{
		_memory[prevBlock].entryUnion.inactive.nextIndex = newBlockIndex;
	}
}

template<typename Type, size_t BlockElementCount>
int32_t SkipArray<Type, BlockElementCount>::_addImpl(const Type& data, std::true_type)
{
	if (!_memory.HasAllocation())
	{
		_allocateMemory();
	}

	int32_t entryIndex = _allocateFromFreeList();
	if (entryIndex == NULL_INDEX)
	{
		return -1;
	}

	Type* entryData = new (&_memory[entryIndex].entryUnion.active.data) Type();
	*entryData = data;
	return entryIndex;
}

template<typename Type, size_t BlockElementCount>
int32_t SkipArray<Type, BlockElementCount>::_addImpl(const Type& data, std::false_type)
{
	static_assert(std::is_copy_constructible_v<Type>, "Type given must be copy assignable to be used in copy Add.");
	return -1;
}

template<typename Type, size_t BlockElementCount>
SkipArray<Type, BlockElementCount>::Iterator::Iterator(Entry* src, int32_t startIndex)
	: _src(src), _index(startIndex)
{

}

template<typename Type, size_t BlockElementCount>
SkipArray<Type, BlockElementCount>::Iterator::~Iterator()
{

}

template<typename Type, size_t BlockElementCount>
Type& SkipArray<Type, BlockElementCount>::Iterator::operator*()
{
	return _src[_index].entryUnion.active.data;
}

template<typename Type, size_t BlockElementCount>
Type* SkipArray<Type, BlockElementCount>::Iterator::operator->()
{
	return &_src[_index].entryUnion.active.data;
}

template<typename Type, size_t BlockElementCount>
typename SkipArray<Type, BlockElementCount>::Iterator& SkipArray<Type, BlockElementCount>::Iterator::operator++(int)
{
	if (_src[_index].entryUnion.active.skipNext == NULL_INDEX)
	{
		_index = NULL_INDEX;
		return *this;
	}

	_index += _src[_index].entryUnion.active.skipNext;
	return *this;
}

template<typename Type, size_t BlockElementCount>
typename SkipArray<Type, BlockElementCount>::Iterator& SkipArray<Type, BlockElementCount>::Iterator::operator--(int)
{
	if (_src[_index].entryUnion.active.skipPrev == NULL_INDEX)
	{
		_index = NULL_INDEX;
		return *this;
	}

	_index += _src[_index].entryUnion.active.skipPrev;
	return *this;
}

template<typename Type, size_t BlockElementCount>
Type* SkipArray<Type, BlockElementCount>::Iterator::GetPointer()
{
	return &_src[_index].entryUnion.active.data;
}

template<typename Type, size_t BlockElementCount>
SkipArray<Type, BlockElementCount>::EntryMemoryBlock::EntryMemoryBlock()
	: _memory(nullptr)
{

}



template<typename Type, size_t BlockElementCount>
void SkipArray<Type, BlockElementCount>::EntryMemoryBlock::AllocateMemory()
{
	_memory = malloc(sizeof(SkipArray::Entry) * BlockElementCount);
}

template<typename Type, size_t BlockElementCount>
void SkipArray<Type, BlockElementCount>::EntryMemoryBlock::DeallocateMemory()
{
	if (_memory != nullptr)
	{
		free(_memory);
		_memory = nullptr;
	}
}

template<typename Type, size_t BlockElementCount>
bool SkipArray<Type, BlockElementCount>::EntryMemoryBlock::HasAllocation()
{
	return _memory != nullptr;
}

template<typename Type, size_t BlockElementCount>
typename SkipArray<Type, BlockElementCount>::Entry& SkipArray<Type, BlockElementCount>::EntryMemoryBlock::operator[](uint32_t index)
{
	CSE_ASSERT(index < BlockElementCount, "Index given is out of range.");
	return reinterpret_cast<Entry*>(_memory)[index];
}

}