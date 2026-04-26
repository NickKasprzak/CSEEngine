#pragma once
#include <cstdint>

namespace CSECore
{

class RefCounted
{
public:
	RefCounted();
	virtual ~RefCounted();

	void IncrementRefCount();
	void DecrementRefCount();
	uint32_t GetRefCount();

private:
	uint32_t _refCount;
};

inline RefCounted::RefCounted()
	: _refCount(0)
{

}

inline RefCounted::~RefCounted()
{

}

inline void RefCounted::IncrementRefCount()
{
	++_refCount;
}

inline void RefCounted::DecrementRefCount()
{
	if (_refCount != 0)
	{
		--_refCount;
	}
}

inline uint32_t RefCounted::GetRefCount()
{
	return _refCount;
}

}