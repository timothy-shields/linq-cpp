#pragma once

#include <utility>

template <typename T, typename Condition, typename Next>
class for_enumerator
{
public:
	typedef T value_type;
	
private:
	bool first;
	value_type value;
	Condition& condition;
	Next& next;
	
public:
	for_enumerator(const value_type& start, Condition& condition, Next& next)
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
	
	const value_type& current() const { return value; }
	value_type& current() { return value; }
};