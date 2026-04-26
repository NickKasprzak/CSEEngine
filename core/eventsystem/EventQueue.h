#pragma once
#include "EventTraits.h"
#include "EventCallbackList.h"
#include "../Utility.h"
#include <tuple>
#include <queue>

namespace CSECore
{

template<typename ...EventTypes>
class EventQueue
{
	typedef std::pair<EventTag, std::tuple<EventTypes...>> TaggedEvent;
public:
	EventQueue();
	~EventQueue();

	template<typename EventTagType>
	void PushEvent(EventTagType eventTag, EventTypes... events);
	void Process();
	void Process(EventCallbackList<EventTypes...>& callbacks);

	void BindCallbackList(const EventCallbackList<EventTypes...>& callbacks);
	void ClearCallbackList();

private:
	std::queue<TaggedEvent> _queue;
	EventCallbackList<EventTypes...> _callbacks;

	template<size_t ...seq>
	void _invokeCallback(const TaggedEvent& event, IntegerSequence<seq...> sequence);
};

template<typename ...EventTypes>
EventQueue<EventTypes...>::EventQueue()
	: _queue(), _callbacks()
{

}

template<typename ...EventType>
EventQueue<EventType...>::~EventQueue()
{

}

template<typename ...EventType>
template<typename EventTagType>
void EventQueue<EventType...>::PushEvent(EventTagType eventTag, EventType... events)
{
	EventTag realTag = CreateEventTag<EventTagType>(eventTag);

	TaggedEvent taggedEvent;
	taggedEvent.first = realTag;
	taggedEvent.second = std::make_tuple(events...);

	_queue.push(taggedEvent);
}

template<typename ...EventTypes>
void EventQueue<EventTypes...>::Process()
{
	while (!_queue.empty())
	{
		const TaggedEvent& event = _queue.front();

		_invokeCallback(event, MakeIntegerSequence<std::tuple_size_v<std::tuple<EventTypes...>>>());

		_queue.pop();
	}
}

template<typename ...EventTypes>
void EventQueue<EventTypes...>::Process(EventCallbackList<EventTypes...>& callbacks)
{
	while (!_queue.empty())
	{
		const TaggedEvent& event = _queue.front();

		CSE_ASSERT(false, "Unimplemented since it'd require another overload of _invokeCallback and I don't feel like doing it rn.");

		_queue.pop();
	}
}

template<typename ...EventTypes>
void EventQueue<EventTypes...>::BindCallbackList(const EventCallbackList<EventTypes...>& callbacks)
{
	_callbacks = callbacks;
}

template<typename ...EventTypes>
void EventQueue<EventTypes...>::ClearCallbackList()
{
	_callbacks = EventCallbackList<EventTypes...>();
}

template<typename ...EventTypes>
template<size_t ...seq>
void EventQueue<EventTypes...>::_invokeCallback(const TaggedEvent& event, IntegerSequence<seq...> sequence)
{
	_callbacks.InvokeCallback(event.first, std::get<seq>(event.second)...);
}

}