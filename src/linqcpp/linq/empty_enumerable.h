#pragma once

#include "enumerable.h"
#include "empty_enumerator.h"

template <typename T>
class empty_enumerable : public enumerable<T>
{
public:
	typedef empty_enumerator<T> enumerator_type;

private:
	empty_enumerable(const empty_enumerable&); // not defined
	empty_enumerable& operator=(const empty_enumerable&); // not defined

public:
	enumerator_type get_enumerator()
	{
		return enumerator_type();
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};