#pragma once
#include "../Any.h"
#include <cstdint>
#include <type_traits>

namespace CSECore
{

typedef uint32_t EventTag;
typedef Any<64> AnyEvent;

namespace CSECore_Private
{

typedef uint16_t EventTagID;
typedef uint16_t EventTagValue;

struct EventTagData
{
	union
	{
		struct TagComponents
		{
			EventTagID id;
			EventTagValue value;
		} components;

		EventTag tag;
	};
};

inline EventTagID GenerateEventTagID()
{
	static EventTagID counter = 0;
	EventTagID next = counter;
	++counter;
	return next;
}

template<typename EventTagType>
inline EventTagID GetEventTagID()
{
	static EventTagID id = GenerateEventTagID();
	return id;
}

}

template<typename EventTagType, typename std::enable_if<std::is_enum_v<EventTagType>>::type* = nullptr>
static EventTag CreateEventTag(EventTagType value)
{
	CSECore_Private::EventTagData tagData;
	tagData.components.id = CSECore_Private::GetEventTagID<EventTagType>();
	tagData.components.value = static_cast<uint16_t>(value);

	return tagData.tag;
}

}