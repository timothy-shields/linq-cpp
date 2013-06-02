#pragma once

#include "make_unique.h"
#include "enumerable.h"
#include "for_enumerator.h"

template <typename T, typename Condition, typename Next>
class for_enumerable : public enumerable<T>
{
public:
	typedef for_enumerator<T, Condition, Next> enumerator_type;

private:
	value_type start;
	Condition condition;
	Next next;

public:
	for_enumerable(for_enumerable&& other)
		: start(std::move(other.start))
		, condition(std::move(other.condition))
		, next(std::move(other.next))
	{
	}

	for_enumerable(value_type start, Condition condition, Next next)
		: start(start)
		, condition(condition)
		, next(next)
	{
	}
	
	enumerator_type get_enumerator()
	{
		return enumerator_type(start, condition, next);
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};