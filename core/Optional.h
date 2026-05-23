#pragma once
#include "Any.h"
#include "CSEAssert.h"

namespace CSECore
{

template<typename Type>
class Optional
{
public:
	Optional()
		: _data(), _hasValue(false)
	{

	}

	Optional(Type& data)
		: _data(), _hasValue(false)
	{
		_data.Construct(data);
		_hasValue = true;
	}

	Optional(const Optional& other)
		: _data(), _hasValue(false)
	{
		if (other._hasValue)
		{
			_data.Construct(*other._data.Cast<Type>());
			_hasValue = true;
		}
	}

	~Optional()
	{
		if (_hasValue)
		{
			_data.Destruct<Type>();
			_hasValue = false;
		}
	}

	void operator=(const Optional& other)
	{
		if (_hasValue)
		{
			_data.Destruct<Type>();
			_hasValue = false;
		}

		if (other._hasValue)
		{
			_data.Construct(*other._data.Cast<Type>());
			_hasValue = true;
		}
	}

	bool HasValue() const
	{
		return _hasValue;
	}

	Type& GetValue()
	{
		CSE_ASSERT(_hasValue, "Attempted to get a value from an Optional that doesn't have one.");
		return *_value.CastToPtr<Type>();
	}

	const Type& GetValue() const
	{
		CSE_ASSERT(_hasValue, "Attempted to get a value from an Optional that doesn't have one.");
		return *_value.CastToPtr<Type>();
	}

private:
	Any<sizeof(Type)> _value;
	bool _hasValue;
};

}