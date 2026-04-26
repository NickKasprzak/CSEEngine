#pragma once
#include "EventTraits.h"
#include "EventCallback.h"
#include <unordered_map>
#include <functional>

namespace CSECore
{

template<typename ...EventTypes>
class EventCallbackList
{
public:
	EventCallbackList();
	~EventCallbackList();

	template<typename EventTagType>
	void AddCallback(EventTagType tag, EventCallback<EventTypes...> callback);
	void InvokeCallback(EventTag tag, EventTypes... events);
	void RemoveCallback(EventTag tag);

private:
	std::unordered_map<EventTag, EventCallback<EventTypes...>> _callbacks;
};

template<typename ...EventTypes>
EventCallbackList<EventTypes...>::EventCallbackList()
	: _callbacks()
{

}

template<typename ...EventTypes>
EventCallbackList<EventTypes...>::~EventCallbackList()
{

}

template<typename ...EventTypes>
template<typename EventTagType>
void EventCallbackList<EventTypes...>::AddCallback(EventTagType tag, EventCallback<EventTypes...> callback)
{
	EventTag realTag = CreateEventTag(tag);

	auto findIter = _callbacks.find(realTag);
	if (findIter != _callbacks.end())
	{
		_callbacks[realTag] = callback;
		return;
	}

	_callbacks.insert({ realTag, callback });
}

template<typename ...EventTypes>
void EventCallbackList<EventTypes...>::InvokeCallback(EventTag tag, EventTypes... events)
{
	auto findIter = _callbacks.find(tag);
	if (findIter == _callbacks.end())
	{
		return;
	}

	_callbacks[tag].Invoke(events...);
}

template<typename ...EventTypes>
void EventCallbackList<EventTypes...>::RemoveCallback(EventTag tag)
{
	_callbacks.erase(tag);
}

}