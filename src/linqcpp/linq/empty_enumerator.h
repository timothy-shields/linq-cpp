#pragma once

#include <stdexcept>

#include "enumerator.h"

template <typename T>
class empty_enumerator : public enumerator<T>
{
private:
	empty_enumerator(const empty_enumerator&); // not defined
	empty_enumerator& operator=(const empty_enumerator&); // not defined

public:
	bool move_next() { return false; }
	value_type current() { throw std::logic_error("Should never call current on empty_enumerator"); }
};