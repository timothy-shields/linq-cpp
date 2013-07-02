#pragma once

#include <utility>

#include "enumerator.h"

namespace linq {

template <typename T>
class return_enumerator : public enumerator<T&>
{
public:
	typedef T& value_type;

private:
	value_type value;
	
public:
	return_enumerator()
	{
	}

	return_enumerator(return_enumerator&& other)
		: value(std::move(other.value))
	{
	}

	return_enumerator& operator=(return_enumerator&& other)
	{
		value = std::move(other.value);
	}

	return_enumerator(value_type value)
		: value(value)
	{
	}

	bool move_first()
	{
		return true;
	}
	
	bool move_next()
	{
		return false;
	}
	
	value_type current()
	{
		return value;
	}
};

}