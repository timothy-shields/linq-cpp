#pragma once

#include <utility>

#include "optional.h"
#include "enumerator.h"
#include "concat_traits.h"

template <typename Source>
class concat_enumerator : public enumerator<typename concat_traits<Source>::inner_value_type>
{
private:
	typedef typename concat_traits<Source>::inner_enumerator_type inner_enumerator_type;

	Source source;
	optional<inner_enumerator_type> inner_enumerator;

	concat_enumerator(const concat_enumerator&); // not defined
	concat_enumerator& operator=(const concat_enumerator&); // not defined
	
public:
	concat_enumerator(concat_enumerator&& other)
		: source(std::move(other.source))
		, inner_enumerator(std::move(other.inner_enumerator))
	{
	}

	concat_enumerator(Source&& source)
		: source(std::move(source))
		, inner_enumerator()
	{
	}
	
	bool move_next()
	{
		while (true)
		{
			if (inner_enumerator && inner_enumerator.value().move_next())
			{
				return true;
			}
			else if (source.move_next())
			{
				inner_enumerator = source.current().get_enumerator();
			}
			else
			{
				return false;
			}
		}
	}
	
	const value_type& current() const { return inner_enumerator.value().current(); }
	value_type& current() { return inner_enumerator.value().current(); }
};
