#pragma once

namespace CSECore
{

template<size_t ...sequence>
struct IntegerSequence {};

namespace CSECore_Private
{

	template<size_t elem, size_t ...seq>
	struct MakeIntegerSequenceStruct : public MakeIntegerSequenceStruct<elem - 1, elem, seq...>
	{

	};

	template<size_t ...seq>
	struct MakeIntegerSequenceStruct<0, seq...> : public IntegerSequence<0, seq...>
	{
		typedef IntegerSequence<0, seq...> type;
	};

}

template<size_t count>
using MakeIntegerSequence = typename CSECore_Private::MakeIntegerSequenceStruct<count - 1>::type;

}