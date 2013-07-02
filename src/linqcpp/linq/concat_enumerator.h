#pragma once

#include <utility>
#include <type_traits>

#include "enumerator.h"
#include "concat_traits.h"

namespace linq {

template <typename Source>
class concat_enumerator : public enumerator<typename concat_traits<Source>::inner_value_type>
{
public:
	typedef typename concat_traits<Source>::inner_value_type value_type;

	static_assert(
		is_enumerator<Source>::value,
		"Failed assert: Source meets the Enumerator<T> requirements");

	static_assert(
		is_enumerator<value_type>::value,
		"Failed assert: Source::value_type meets the Enumerator<T> requirements");

private:
	typedef typename concat_traits<Source>::inner_enumerator_type inner_enumerator_type;

	Source source;
	inner_enumerator_type inner_enumerator;

	concat_enumerator(concat_enumerator const&); // not defined
	concat_enumerator& operator=(concat_enumerator const&); // not defined
	
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

	bool move_first()
	{
		if (source.move_first())
		{
			inner_enumerator = source.current().get_enumerator();
		}
		else
		{
			return false;
		}

		while (true)
		{
			if (inner_enumerator.move_first())
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
	
	bool move_next()
	{
		if (inner_enumerator.move_next())
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
		
		while (true)
		{
			if (inner_enumerator.move_first())
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
	
	value_type current()
	{
		return inner_enumerator.current();
	}
};

}