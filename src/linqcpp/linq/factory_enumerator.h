#pragma once

#include <stdexcept>

#include "enumerator.h"

template <typename T>
class empty_enumerator : public enumerator<T>
{
public:
	bool move_next() { return false; }
	const value_type& current() const { throw std::logic_error("Should never call current on empty_enumerator"); }
	value_type& current() { throw std::logic_error("Should never call current on empty_enumerator"); }
};