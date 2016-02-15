#pragma once

#include "enumerator.h"
#include <vector>
#include <algorithm>

namespace linq {

template <typename Source, typename Compare>
class order_by_enumerator : public enumerator<typename std::remove_reference<typename Source::value_type>::type>
{
public:
	typedef typename std::remove_reference<typename Source::value_type>::type value_type;

private:
	std::vector<value_type> ordered_values;
	typename std::vector<value_type>::iterator curr;

	order_by_enumerator(order_by_enumerator const&); // not defined
	order_by_enumerator& operator=(order_by_enumerator const&); // not defined

public:
	order_by_enumerator(order_by_enumerator&& other)
		: ordered_values(std::move(other.ordered_values))
		,curr(std::move(other.curr))
	{
	}

	order_by_enumerator(Source&& source, Compare const& compare)
	{
		if (!source.move_first())
		{
			curr = ordered_values.end();
		}
		while (true)
		{
			ordered_values.push_back(source.current());
			if (!source.move_next())
				break;
		}

		std::sort(ordered_values.begin(), ordered_values.end(), compare);
		curr = ordered_values.begin();
	}

	bool move_first()
	{
		return curr != ordered_values.end();
	}

	bool move_next()
	{
		++curr;
		return curr != ordered_values.end();
	}

	value_type current()
	{
		return *curr;
	}
};

}
