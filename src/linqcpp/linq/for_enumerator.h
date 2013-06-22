#pragma once

#include <utility>

#include "enumerator.h"

namespace linq {

template <typename T, typename Condition, typename Next>
class for_enumerator : public enumerator<T>
{
public:
	typedef T value_type;

private:
	value_type value;
	Condition condition;
	Next next;
	
public:
	for_enumerator(for_enumerator&& other)
		: value(std::move(other.value))
		, condition(std::move(other.condition))
		, next(std::move(other.next))
	{
	}

	for_enumerator(value_type start, Condition condition, Next next)
		: value(start)
		, condition(condition)
		, next(next)
	{
	}

	bool move_first()
	{
		return condition(value);
	}
	
	bool move_next()
	{
		value = next(value);
		return condition(value);
	}
	
	value_type current()
	{
		return value;
	}
};

}