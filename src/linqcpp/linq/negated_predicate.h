#pragma once

namespace linq {

template <typename T, typename Predicate>
class negated_predicate
{
private:
	Predicate predicate;
public:
	negated_predicate(Predicate const& predicate)
		: predicate(predicate)
	{
	}

	bool operator()(T const& value)
	{
		return !predicate(value);
	}
};

}