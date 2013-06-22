#pragma once

#include <cstddef>

namespace linq {

//returns true for the first "count" calls, then returns false
template <typename T>
class counter_predicate
{
private:
	std::size_t remaining;

public:
	counter_predicate(std::size_t count)
		: remaining(count)
	{
	}

	bool operator()(T const&)
	{
		return remaining-- > 0;
	}
};

}