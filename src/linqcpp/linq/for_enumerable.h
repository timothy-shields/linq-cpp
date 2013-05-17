#pragma once

#include "for_enumerator.h"

template <typename T, typename Condition, typename Next>
class for_enumerable
{
public:
	typedef for_enumerator<T, Condition, Next> enumerator_type;
	typedef typename enumerator_type::value_type value_type;
	
private:
	value_type start;
	Condition condition;
	Next next;

public:
	for_enumerable(const value_type& start, const Condition& condition, const Next& next)
		: start(start)
		, condition(condition)
		, next(next)
	{
	}
	
	enumerator_type get_enumerator()
	{
		return enumerator_type(start, condition, next);
	}
};