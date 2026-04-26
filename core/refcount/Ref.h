#pragma once
#include "../CSEAssert.h"

namespace CSECore
{

template<typename Type>
class Ref
{
public:
	Ref(Type* data);
	Ref(Type& data);
	Ref(const Ref<Type>& other);
	~Ref();

	Type& operator*();
	Type* operator->();

	Type* GetRawPointer();

private:
	Type* _data;
	bool _owning;
};

template<typename Type>
Ref<Type>::Ref(Type* data)
	: _data(data), _owning(true)
{
	_data->IncrementRefCount();
}

template<typename Type>
Ref<Type>::Ref(Type& data)
	: _data(&data), _owning(false)
{
	_data->IncrementRefCount();
}

template<typename Type>
Ref<Type>::Ref(const Ref<Type>& other)
	: _data(other._data), _owning(other._owning)
{
	_data->IncrementRefCount();
}

template<typename Type>
Ref<Type>::~Ref()
{
	_data->DecrementRefCount();

	if (_owning && _data->GetRefCount() == 0)
	{
		delete _data;
	}
}

template<typename Type>
Type& Ref<Type>::operator*()
{
	CSE_ASSERT(_data != nullptr, "Can't dereference a Ref with a nullptr");
	return *_data;
}

template<typename Type>
Type* Ref<Type>::operator->()
{
	CSE_ASSERT(_data != nullptr, "Can't dereference a Ref with a nullptr");
	return _data;
}

template<typename Type>
Type* Ref<Type>::GetRawPointer()
{
	return _data;
}

}