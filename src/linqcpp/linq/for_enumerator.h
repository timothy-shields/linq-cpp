#pragma once

#include <utility>

#include "enumerator.h"

template <typename T, typename Condition, typename Next>
class for_enumerator : public enumerator<T>
{
private:
	bool first;
	value_type value;
	Condition condition;
	Next next;
	
public:
	for_enumerator(for_enumerator&& other)
		: first(other.first)
		, value(std::move(other.value))
		, condition(std::move(other.condition))
		, next(std::move(other.next))
	{
	}

	for_enumerator(value_type start, Condition condition, Next next)
		: first(true)
		, value(start)
		, condition(condition)
		, next(next)
	{
	}
	
	bool move_next()
	{
		if (first)
		{
			first = false;
		}
		else
		{
			value = next(value);
		}
		return condition(value);
	}
	
	value_type current() { return value; }
};