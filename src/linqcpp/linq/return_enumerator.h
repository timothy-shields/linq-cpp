#pragma once

#include <utility>

template <typename T>
class return_enumerator
{
public:
	typedef T value_type;
	
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
	
	const value_type& current() const { return value; }
	value_type& current() { return value; }
};