#pragma once

#include <utility>

#include "enumerator.h"
#include "concat_traits.h"

template <typename Source>
class concat_enumerator : public enumerator<typename concat_traits<Source>::inner_value_type>
{
private:
	typedef typename concat_traits<Source>::inner_enumerator_type inner_enumerator_type;

	Source source;
	bool first;
	inner_enumerator_type inner_enumerator;
	
public:
	concat_enumerator(Source&& source)
		: source(source)
		, first(true)
	{
	}
	
	bool move_next()
	{
		while (true)
		{
			if (!first && inner_enumerator.move_next())
			{
				return true;
			}
			else if (source.move_next())
			{
				inner_enumerator = std::move(source.current().get_enumerator());
			}
			else
			{
				return false;
			}
		}
	}
	
	const value_type& current() const { return inner.current(); }
	value_type& current() { return inner.current(); }
};
