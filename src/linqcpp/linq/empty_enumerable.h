#pragma once

#include "empty_enumerator.h"

template <typename T>
class empty_enumerable
{
public:
	typedef empty_enumerator<T> enumerator_type;
	typedef typename enumerator_type::value_type value_type;

public:
	enumerator_type get_enumerator()
	{
		return enumerator_type();
	}
};