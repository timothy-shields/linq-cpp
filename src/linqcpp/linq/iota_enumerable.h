#pragma once

#include "make_unique.h"
#include "enumerable.h"
#include "iota_enumerator.h"

template <typename T>
class iota_enumerable : public enumerable<T>
{
public:
	typedef iota_enumerator<T> enumerator_type;

private:
	value_type start;

public:
	iota_enumerable(iota_enumerable&& other)
		: start(std::move(other.start))
	{
	}

	iota_enumerable(value_type start)
		: start(start)
	{
	}
	
	enumerator_type get_enumerator()
	{
		return enumerator_type(start);
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};