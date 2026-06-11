#pragma once
#include "../CSEAssert.h"
#include <functional>

namespace CSECore
{

template<typename Type>
struct DefaultRefDeleter
{
	void operator()(Type* data) const
	{
		if (data->GetRefCount() == 0)
		{
			delete data;
		}
	}
};

template<typename Type>
class Ref
{
private:
	typedef std::function<void(Type* data)> DeleterCallback;

	template<typename Deleter>
	static void DeleterTemplate(Type* data)
	{
		Deleter()(data);
	}

public:
	Ref();
	Ref(Type* data);
	template<typename Deleter>
	Ref(Type* data, Deleter deleter);
	Ref(const Ref<Type>& other);
	~Ref();

	Type& operator*();
	Type* operator->();

	void operator=(const Ref<Type>& other);

	Type* GetRawPointer();

	template<typename CastType>
	CastType* GetRawCastedPointer();

private:
	Type* _data;
	DeleterCallback _deleterCallback;
};

template<typename Type>
Ref<Type>::Ref()
	: _data(nullptr), _deleterCallback()
{

}

template<typename Type>
Ref<Type>::Ref(Type* data)
	: _data(data), _deleterCallback(DeleterTemplate<DefaultRefDeleter<Type>>)
{
	_data->IncrementRefCount();
}

template<typename Type>
template<typename Deleter>
Ref<Type>::Ref(Type* data, Deleter deleter)
	: _data(data), _deleterCallback(DeleterTemplate<Deleter>)
{
	_data->IncrementRefCount();
}

template<typename Type>
Ref<Type>::Ref(const Ref<Type>& other)
	: _data(other._data), _deleterCallback(other._deleterCallback)
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
	_deleterCallback(_data);
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
	_deleterCallback = other._deleterCallback;
	_data->IncrementRefCount();
}

template<typename Type>
Type* Ref<Type>::GetRawPointer()
{
	return _data;
}

template<typename Type>
template<typename CastedType>
CastedType* Ref<Type>::GetRawCastedPointer()
{
	return static_cast<CastedType*>(_data);
}

}