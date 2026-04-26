#pragma once
#include "Any.h"
#include "Assert.h"
#include <algorithm>

namespace CSECore
{

/*
* Used to explicitly define a ctor param as Unexpected
* to avoid issues where the Expected and Unexpected type
* are the same
*/

template<typename UnexpectedType>
class UnexpectedWrapper
{
public:
	UnexpectedWrapper(const UnexpectedType& unexpectedData)
		: data(unexpectedData)
	{

	}
	
	~UnexpectedWrapper()
	{

	}

	UnexpectedType data;
};

template<typename ExpectedType, typename UnexpectedType>
class Expected
{
public:
	Expected()
		: _value(), _hasExpected(false), _hasUnexpected(false)
	{

	}

	Expected(const ExpectedType& expected)
		: _value(expected), _hasExpected(true), _hasUnexpected(false)
	{

	}

	Expected(const UnexpectedWrapper<UnexpectedType>& unexpected)
		: _value(unexpected.data), _hasExpected(false), _hasUnexpected(true)
	{

	}

	Expected& operator=(const Expected& other)
	{
		_value = other._value;
		_hasExpected = other._hasExpected;
		_hasUnexpected = other._hasUnexpected;
		return *this;
	}

	bool HasExpected() const
	{
		return _hasExpected;
	}

	bool HasUnexpected() const
	{
		return _hasUnexpected;
	}

	ExpectedType GetExpected() const
	{
		CSE_ASSERT(_hasExpected, "Attempted to get an expected value from an Expected that doesn't have one.");
		return _value.CastTo<ExpectedType>();
	}

	UnexpectedType GetUnexpected() const
	{
		CSE_ASSERT(_hasUnexpected, "Attempted to get an unexpected value from an Expected that doesn't have one.");
		return _value.CastTo<UnexpectedType>();
	}

private:
	Any<(std::max)(sizeof(ExpectedType), sizeof(UnexpectedType))> _value;
	bool _hasExpected;
	bool _hasUnexpected;
};

template<typename ExpectedType, typename UnexpectedType>
Expected<ExpectedType, UnexpectedType> CreateExpected(const ExpectedType& value)
{
	return Expected<ExpectedType, UnexpectedType>(value);
}

template<typename ExpectedType, typename UnexpectedType>
Expected<ExpectedType, UnexpectedType> CreateUnexpected(const UnexpectedType& value)
{
	return Expected<ExpectedType, UnexpectedType>(UnexpectedWrapper<UnexpectedType>(value));
}

}