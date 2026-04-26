#pragma once
#include "EventQueue.h"
#include "../containers/SkipArray.h"
#include <unordered_map>

namespace CSECore
{

template<typename ...EventTypes>
class EventDistributor
{
	class QueueInfo
	{
	public:
		std::unordered_map<EventQueue<EventTypes...>*, int32_t> queueToIndex;
		SkipArray<EventQueue<EventTypes...>*, 64> queues;

		QueueInfo() : queueToIndex(), queues() {};
		~QueueInfo() {};
	};

public:
	EventDistributor();
	~EventDistributor();

	template<typename EventTagType>
	void AddEventQueue(EventTagType tagType, EventQueue<EventTypes...>* queue);
	template<typename EventTagType>
	void RemoveEventQueue(EventTagType tagType, EventQueue<EventTypes...>* queue);

	template<typename EventTagType>
	void PushEvent(EventTagType tagType, const EventTypes&... events);

private:
	std::unordered_map<EventTag, QueueInfo> _eventQueueMap;
};

template<typename ...EventTypes>
EventDistributor<EventTypes...>::EventDistributor()
	: _eventQueueMap()
{

}

template<typename ...EventTypes>
EventDistributor<EventTypes...>::~EventDistributor()
{

}

template<typename ...EventTypes>
template<typename EventTagType>
void EventDistributor<EventTypes...>::AddEventQueue(EventTagType tagType, EventQueue<EventTypes...>* queue)
{
	EventTag tag = CreateEventTag(tagType);

	// Add tag/queues pairing if not added
	if (_eventQueueMap.find(tag) == _eventQueueMap.end())
	{
		_eventQueueMap.insert(std::make_pair(tag, QueueInfo()));
	}

	QueueInfo& queueInfo = _eventQueueMap[tag];

	/*
	* Skip addition if queue has already been
	* registered for this event type
	*/
	if (queueInfo.queueToIndex.find(queue) != queueInfo.queueToIndex.end())
	{
		return;
	}

	int32_t index = queueInfo.queues.Add(queue);
	CSE_ASSERT(index != -1, "Failed to add an event queue to an event distributor.");

	queueInfo.queueToIndex.insert(std::make_pair(queue, index));
}

template<typename ...EventTypes>
template<typename EventTagType>
void EventDistributor<EventTypes...>::RemoveEventQueue(EventTagType tagType, EventQueue<EventTypes...>* queue)
{
	EventTag tag = CreateEventTag(tagType);

	// Break early if event tag isn't registered
	if (_eventQueueMap.find(tag) == _eventQueueMap.end())
	{
		return;
	}

	QueueInfo& queueInfo = _eventQueueMap[tag];

	/*
	* Break early again if given queue isn't
	* registered for this event type
	*/
	if (queueInfo.queueToIndex.find(queue) == queueInfo.queueToIndex.end())
	{
		return;
	}

	int32_t index = queueInfo.queueToIndex[queue];
	queueInfo.queueToIndex.erase(queue);
	queueInfo.queues.RemoveAtIndex(index);

	/*
	* Remove the QueueInfo associated with the
	* tag if it doesn't contain anything
	*/
	if (queueInfo.queueToIndex.size() == 0)
	{
		_eventQueueMap.erase(tag);
	}
}

template<typename ...EventTypes>
template<typename EventTagType>
void EventDistributor<EventTypes...>::PushEvent(EventTagType tagType, const EventTypes&... events)\
{
	EventTag tag = CreateEventTag(tagType);

	/*
	* If this distributor doesn't have
	* any queues registered to recieve
	* events of the given tag, drop the
	* event.
	*/
	if (_eventQueueMap.find(tag) == _eventQueueMap.end())
	{
		return;
	}

	/*
	* Iterate through each queue registered
	* to recieve events with the given tag
	* and push the given event to them
	*/
	QueueInfo& queueInfo = _eventQueueMap[tag];
	
	auto iter = queueInfo.queues.Begin();
	auto end = queueInfo.queues.End();
	while (iter != end)
	{
		(*iter)->PushEvent(tagType, events...);
		iter++;
	}
}

}