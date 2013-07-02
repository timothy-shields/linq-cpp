#pragma once

#include "enumerator.h"

namespace linq {

template <typename Source, typename Predicate>
class where_enumerator : public enumerator<typename Source::value_type>
{
public:
	typedef typename Source::value_type value_type;

	static_assert(
		is_enumerator<Source>::value,
		"Failed assert: Source meets the Enumerator<T> requirements");

private:
	Source source;
	Predicate predicate;
	
	where_enumerator(where_enumerator const&); // not defined
	where_enumerator& operator=(where_enumerator const&); // not defined

public:
	where_enumerator()
	{
	}

	where_enumerator(where_enumerator&& other)
		: source(std::move(other.source))
		, predicate(std::move(other.predicate))
	{
	}

	where_enumerator(Source&& source, Predicate const& predicate)
		: source(std::move(source))
		, predicate(predicate)
	{
	}

	bool move_first()
	{
		if (!source.move_first())
		{
			return false;
		}
		else if (predicate(source.current()))
		{
			return true;
		}

		return move_next();
	}
	
	bool move_next()
	{
		while (true)
		{
			if (!source.move_next())
			{
				return false;
			}
			else if (predicate(source.current()))
			{
				return true;
			}
		}
	}
	
	value_type current()
	{
		return source.current();
	}
};

}