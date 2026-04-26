#pragma once
#include "CSEAssert.h"
#include <memory>
#include <cstddef>

namespace CSECore
{

namespace CSECore_Private
{

struct StoredTypeCallbacks
{
	void (*copyConstruct)(void* src, void* dst);
	void (*destruct)(void* dst);
};

inline bool operator==(const StoredTypeCallbacks& lhs, const StoredTypeCallbacks& rhs)
{
	return (lhs.copyConstruct == rhs.copyConstruct) && (lhs.destruct == rhs.destruct);
}

template<typename Type>
static void CopyConstructCallback(void* src, void* dst)
{
	new (dst) Type(*static_cast<Type*>(src));
}

template<typename Type>
static void DestructCallback(void* dst)
{
	(static_cast<Type*>(dst))->Type::~Type();
}

}

template<size_t BufferSize>
class Any
{
	typedef CSECore_Private::StoredTypeCallbacks StoredTypeCallbacks;

public:
	Any()
		: _buffer(), _callbacks(), _hasValue(false)
	{

	}

	template<typename Type>
	Any(const Type& type)
		: _buffer(), _callbacks(), _hasValue(false)
	{
		_construct<Type>(type);
	}

	Any(const Any& other)
		: _buffer(), _callbacks(), _hasValue(false)
	{
		_copyConstruct(other);
	}

	~Any()
	{
		if (_hasValue)
		{
			_destruct();
		}
	}

	template<typename Type>
	Any& operator=(const Type& type)
	{
		if (_hasValue)
		{
			_destruct();
		}

		_construct<Type>(type);
		return *this;
	}

	Any& operator=(const Any& other)
	{
		if (_hasValue)
		{
			_destruct();
		}

		_copyConstruct(other);
		return *this;
	}

	template<typename Type>
	Type CastTo() const
	{
		CSE_ASSERT(_hasValue, "Cannot cast Any if no type exists.");
		CSE_ASSERT(_callbacks == _getTypeCallbacks<Type>(), "Cannot cast Any to an incompatible type.");

		return *static_cast<Type*>(_getBuffer());
	}

	template<typename Type>
	bool IsA() const
	{
		return _callbacks == _getTypeCallbacks<Type>();
	}

private:
	template<typename Type>
	const StoredTypeCallbacks _getTypeCallbacks() const
	{
		StoredTypeCallbacks callbacks;
		callbacks.copyConstruct = CSECore_Private::CopyConstructCallback<Type>;
		callbacks.destruct = CSECore_Private::DestructCallback<Type>;
		return callbacks;
	}

	template<typename Type>
	void _construct(const Type& type)
	{
		static_assert(sizeof(Type) <= BufferSize);
		static_assert(std::is_copy_constructible_v<Type>);
		CSE_ASSERT(!_hasValue, "Attempted to construct in Any when a value exists.");

		new (_getBuffer()) Type(type);
		_callbacks = _getTypeCallbacks<Type>();
		_hasValue = true;
	}

	void _copyConstruct(const Any& other)
	{
		CSE_ASSERT(!_hasValue, "Attempted to copyConstruct construct another Any when a value exists.");

		if (other._callbacks.copyConstruct != nullptr)
		{
			(*other._callbacks.copyConstruct)(other._getBuffer(), _getBuffer());
		}
		_callbacks = other._callbacks;
		_hasValue = other._hasValue;
	}

	void _destruct()
	{
		CSE_ASSERT(_hasValue, "Attempted to destruct in Any when no value exists.");

		(*_callbacks.destruct)(_getBuffer());
		_hasValue = false;
	}

	void* _getBuffer() const
	{
		return const_cast<void*>(static_cast<const void*>(_buffer));
	}

	alignas(std::max_align_t) char _buffer[BufferSize];
	StoredTypeCallbacks _callbacks;
	bool _hasValue;
};

}