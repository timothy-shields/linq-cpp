#pragma once

#include "enumerable.h"
#include "return_enumerator.h"

template <typename T>
class return_enumerable : public enumerable<T>
{
public:
	typedef return_enumerator<T> enumerator_type;

private:
	value_type value;

public:
	return_enumerable(return_enumerable&& other)
		: value(std::move(other.value))
	{
	}

	return_enumerable(value_type&& value)
		: value(std::move(value))
	{
	}

	return_enumerable(value_type const& value)
		: value(value)
	{
	}
	
	enumerator_type get_enumerator()
	{
		return enumerator_type(value);
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};