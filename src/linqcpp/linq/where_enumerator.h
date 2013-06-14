#pragma once

#include "enumerator.h"

template <typename Source, typename Predicate>
class where_enumerator : public enumerator<typename Source::value_type>
{
private:
	Source source;
	Predicate predicate;
	
	where_enumerator(where_enumerator const&); // not defined
	where_enumerator& operator=(where_enumerator const&); // not defined

public:
	where_enumerator(where_enumerator&& other)
		: source(std::move(other.source))
		, predicate(other.predicate)
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

		while (true)
		{
			if (predicate(source.current()))
			{
				return true;
			}
			else if (!source.move_next())
			{
				return false;
			}
		}
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