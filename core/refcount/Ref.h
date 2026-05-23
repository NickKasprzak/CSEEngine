#pragma once
#include "../CSEAssert.h"

namespace CSECore
{

template<typename Type>
class Ref
{
public:
	Ref();
	Ref(const Ref<Type>& other);
	~Ref();

	Type& operator*();
	Type* operator->();

	void operator=(const Ref<Type>& other);

	Type* GetRawPointer();

private:
	Type* _data;
	bool _owning;

	Ref(Type* data, bool owning);

	template <typename T>
	friend Ref<T> MakeOwningRef(T* data);

	template <typename T>
	friend Ref<T> MakeNonOwningRef(T* data);
};

template<typename Type>
Ref<Type>::Ref()
	: _data(nullptr), _owning(false)
{

}

template<typename Type>
Ref<Type>::Ref(Type* data, bool owning)
	: _data(data), _owning(owning)
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
	if (_data == nullptr)
	{
		return;
	}

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
void Ref<Type>::operator=(const Ref<Type>& other)
{
	Ref::~Ref();

	_data = other._data;
	_owning = other._owning;
	_data->IncrementRefCount();
}

template<typename Type>
Type* Ref<Type>::GetRawPointer()
{
	return _data;
}

template<typename T>
Ref<T> MakeEmptyRef()
{
	return Ref<T>();
}

template <typename T>
Ref<T> MakeOwningRef(T* data)
{
	return Ref<T>(data, true);
}

template <typename T>
Ref<T> MakeNonOwningRef(T* data)
{
	return Ref<T>(data, false);
}

}