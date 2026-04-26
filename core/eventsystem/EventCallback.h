#pragma once
#include "EventTraits.h"
#include "../CSEAssert.h"
#include "../Logger.h"
#include <functional>

namespace CSECore
{

template<typename ...EventTypes>
class EventCallback
{
	typedef std::function<void(void*, EventTypes...)> Callback;

	template<typename SubjectType, void (SubjectType::* Function)(EventTypes...)>
	static void CallbackTemplate(void* subject, EventTypes... events)
	{
		(static_cast<SubjectType*>(subject)->*Function)(events...);
	}

	template<typename SubjectType, void (*Function)(SubjectType*, EventTypes...)>
	static void CallbackTemplate(void* subject, EventTypes... events)
	{
		(*Function)(static_cast<SubjectType*>(subject), events...);
	}

public:
	EventCallback();
	~EventCallback();

	template<typename SubjectType, void (SubjectType::* Function)(EventTypes...)>
	void BindCallback(SubjectType* subject);
	template<typename SubjectType, void (*Function)(SubjectType*, EventTypes...)>
	void BindCallback(SubjectType* subject);

	void Invoke(EventTypes... events);

private:
	void* _boundSubject;
	Callback _boundCallback;
};

template<typename ...EventTypes>
EventCallback<EventTypes...>::EventCallback()
	: _boundSubject(nullptr), _boundCallback()
{

}

template<typename ...EventTypes>
EventCallback<EventTypes...>::~EventCallback()
{

}

template<typename ...EventTypes>
template<typename SubjectType, void (SubjectType::* Function)(EventTypes...)>
void EventCallback<EventTypes...>::BindCallback(SubjectType* subject)
{
	CSE_ASSERT(subject != nullptr, "Attempted to pass in a nullptr as a subject for member function EventCallback.");

	_boundSubject = subject;
	_boundCallback = CallbackTemplate<SubjectType, Function>;
}

template<typename ...EventTypes>
template<typename SubjectType, void (*Function)(SubjectType*, EventTypes...)>
void EventCallback<EventTypes...>::BindCallback(SubjectType* subject)
{
	CSE_ASSERT(subject != nullptr, "Attempted to pass in a nullptr as a subject for member function EventCallback.");

	_boundSubject = subject;
	_boundCallback = CallbackTemplate<SubjectType, Function>;
}

template<typename ...EventTypes>
void EventCallback<EventTypes...>::Invoke(EventTypes... events)
{
	CSE_ASSERT(_boundSubject != nullptr && _boundCallback, "Attempted to invoke an EventCallback without having anything bound.");

	(_boundCallback)(_boundSubject, events...);
}

template<>
class EventCallback<AnyEvent>
{
	typedef std::function<void(void*, AnyEvent)> Callback;

	template<typename SubjectType, typename EventType, void (SubjectType::* Function)(EventType)>
	static void AnyEventCallbackTemplate(void* subject, AnyEvent event)
	{
		if (!event.IsA<EventType>())
		{
			CSE_LOGW("AnyEvent given to EventCallback was the incorrect type.");
			return;
		}

		(static_cast<SubjectType*>(subject)->*Function)(event.CastTo<EventType>());
	}

public:
	EventCallback()
		: _boundSubject(nullptr), _boundCallback()
	{

	}

	~EventCallback()
	{

	}

	template<typename SubjectType, typename EventType, void (SubjectType::* Function)(EventType)>
	void BindCallback(SubjectType* subject)
	{
		CSE_ASSERT(subject != nullptr, "Attempted to pass in a nullptr as a subject for member function EventCallback.");

		_boundSubject = subject;
		_boundCallback = AnyEventCallbackTemplate<SubjectType, EventType, Function>;
	}

	void Invoke(AnyEvent event)
	{
		CSE_ASSERT(_boundSubject != nullptr && _boundCallback, "Attempted to invoke an EventCallback without having anything bound.");

		(_boundCallback)(_boundSubject, event);
	}

private:
	void* _boundSubject;
	Callback _boundCallback;
};

}