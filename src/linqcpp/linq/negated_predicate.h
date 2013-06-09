#pragma once

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

template <typename T, typename Predicate>
negated_predicate<T, Predicate> negate_predicate(Predicate const& predicate)
{
	return negated_predicate<T, Predicate>(predicate);
}