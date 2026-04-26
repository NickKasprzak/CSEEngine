#pragma once
#include "eventsystem/EventQueue.h"
#include "eventsystem/EventDistributor.h"
#include "eventsystem/EventCallback.h"
#include "Any.h"
#include "Logger.h"
#include "CSEAssert.h"

#include "Utility.h"

enum EventTags
{
	TYPE_ONE,
	TYPE_TWO,
	TYPE_THREE
};

struct MyEvent
{
	int num1;
	int num2;
	int num3;
};

class EventRecipient
{
public:
	EventRecipient()
		: _sum(0)
	{

	}

	~EventRecipient()
	{

	}

	void HandleMyEventTypeOne(MyEvent event)
	{
		_sum += event.num1;
	}

	void HandleMyEventTypeTwo(MyEvent event)
	{
		_sum += (event.num2 * 2);
	}

	void HandleMyEventTypeThree(MyEvent event)
	{
		_sum += (event.num3 * 3);
	}

	int GetSum()
	{
		return _sum;
	}

private:
	int _sum;
};

void DistributorTest()
{
	/*
	* Create a subject and an event queue. Said
	* event queue will be "owned" by the subject
	* by invoking callbacks for events it needs
	* to process.
	*/
	EventRecipient subject;
	CSECore::EventQueue<CSECore::AnyEvent> eventQueue;
	
	/*
	* Create a callback list to assign callbacks
	* to invoke for specific event types.
	* 
	* This event list gets bound to the event queue
	* for use in processing any incoming events.
	*/
	CSECore::EventCallbackList<CSECore::AnyEvent> callbacks;

	/*
	* The callback list requires the creation of
	* a specific callback type to store the subject
	* of the callback and said callback itself.
	* 
	* Each callback also needs an associated EventTag
	* so the callback list knows what callback to invoke
	* when an event comes in based on said tag.
	*/
	CSECore::EventCallback<CSECore::AnyEvent> callback1;
	callback1.BindCallback<EventRecipient, MyEvent, &EventRecipient::HandleMyEventTypeOne>(&subject);
	callbacks.AddCallback(EventTags::TYPE_ONE, callback1);

	CSECore::EventCallback<CSECore::AnyEvent> callback2;
	callback2.BindCallback<EventRecipient, MyEvent, &EventRecipient::HandleMyEventTypeTwo>(&subject);
	callbacks.AddCallback(EventTags::TYPE_TWO, callback2);

	CSECore::EventCallback<CSECore::AnyEvent> callback3;
	callback3.BindCallback<EventRecipient, MyEvent, &EventRecipient::HandleMyEventTypeThree>(&subject);
	callbacks.AddCallback(EventTags::TYPE_THREE, callback3);

	eventQueue.BindCallbackList(callbacks);

	/*
	* Each event queue can be given to a distributor.
	* A distributor accepts events and passes each event
	* to any queue registered to it.
	* 
	* Registered event queues are also registered using
	* tags, allowing the distributor to only pass events
	* to queues that need to listen for specific event types.
	*/
	CSECore::EventDistributor<CSECore::AnyEvent> distributor;
	distributor.AddEventQueue(EventTags::TYPE_ONE, &eventQueue);
	distributor.AddEventQueue(EventTags::TYPE_TWO, &eventQueue);
	distributor.AddEventQueue(EventTags::TYPE_THREE, &eventQueue);

	MyEvent event;
	event.num1 = 10;
	event.num2 = 100;
	event.num3 = 1000;
	
	distributor.PushEvent(EventTags::TYPE_ONE, event);
	distributor.PushEvent(EventTags::TYPE_TWO, event);
	distributor.PushEvent(EventTags::TYPE_THREE, event);

	/*
	* Once a queue has its events given to it by a
	* distributor or some other means, it can then
	* process them all and invoke any registered
	* callbacks for the subject.
	*/
	eventQueue.Process();

	CSE_LOGI("Subject sum is: " << subject.GetSum() << ".");

	/*
	* An event queue needs to be removed from an
	* event distributor before it goes out of scope.
	* 
	* Additionally, a subject has to have its callbacks
	* removed from an event queue before it goes out
	* of scope.
	* 
	* This should be coordinated safely using RAII
	* or some other pattern.
	*/

	distributor.RemoveEventQueue(EventTags::TYPE_ONE, &eventQueue);
	distributor.RemoveEventQueue(EventTags::TYPE_TWO, &eventQueue);
	distributor.RemoveEventQueue(EventTags::TYPE_THREE, &eventQueue);

	eventQueue.ClearCallbackList();

	/*
	* To demonstrate the events will no longer
	* invoke a callback on the subject, the
	* events are pushed and processed again.
	*/

	distributor.PushEvent(EventTags::TYPE_ONE, event);
	distributor.PushEvent(EventTags::TYPE_TWO, event);
	distributor.PushEvent(EventTags::TYPE_THREE, event);
	eventQueue.Process();

	CSE_LOGI("Subject sum is: " << subject.GetSum() << ". This should be the same as before.");
}

int main()
{
	DistributorTest();
	return 0;
}