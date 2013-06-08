#pragma once

#include <utility>

#include "enumerator.h"

template <typename T>
class return_enumerator : public enumerator<T>
{
private:
	bool first;
	value_type& value;
	
public:
	return_enumerator(value_type& value)
		: first(true)
		, value(value)
	{
	}
	
	bool move_next()
	{
		if (first)
		{
			first = false;
			return true;
		}
		else
		{
			return false;
		}
	}
	
	value_type current() { return value; }
};