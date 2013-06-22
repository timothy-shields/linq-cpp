#pragma once

#include <stdexcept>

#include "enumerator.h"

namespace linq {

template <typename T>
class empty_enumerator : public enumerator<T>
{
public:
	typedef T value_type;

	bool move_first()
	{
		return false;
	}
	
	bool move_next()
	{
		static_assert(false, "Should never call move_next on empty_enumerator");
		throw std::logic_error("Should never call move_next on empty_enumerator");
	}

	value_type current()
	{
		static_assert(false, "Should never call current on empty_enumerator");
		throw std::logic_error("Should never call current on empty_enumerator");
	}
};

}