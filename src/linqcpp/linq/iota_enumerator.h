#pragma once

#include <utility>

#include "enumerator.h"

template <typename T>
class iota_enumerator : public enumerator<T>
{
private:
	bool first;
	value_type value;
	
public:
	iota_enumerator(iota_enumerator&& other)
		: first(other.first)
		, value(std::move(other.value))
	{
	}

	iota_enumerator(value_type start)
		: first(true)
		, value(start)
	{
	}

	bool move_first()
	{
		return true;
	}
	
	bool move_next()
	{
		++value;
		return true;
	}
	
	value_type current()
	{
		return value;
	}
};