#pragma once

#include "make_unique.h"
#include "enumerable.h"
#include "empty_enumerator.h"

template <typename T>
class empty_enumerable : public enumerable<T>
{
public:
	typedef empty_enumerator<T> enumerator_type;
	typedef typename enumerator_type::value_type value_type;

	enumerator_type get_enumerator()
	{
		return enumerator_type();
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};