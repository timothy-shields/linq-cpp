#pragma once

#include "return_enumerator.h"

template <typename T>
class return_enumerable
{
public:
	typedef return_enumerator<T> enumerator_type;
	typedef typename enumerator_type::value_type value_type;
	
private:
	value_type value;

public:
	return_enumerable(const value_type& value)
		: value(value)
	{
	}
	
	return_enumerable(value_type&& value)
		: value(std::move(value))
	{
	}
	
	enumerator_type get_enumerator()
	{
		return enumerator_type(value);
	}
};