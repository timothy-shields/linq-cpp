#pragma once

#include "enumerator.h"

template <typename Source, typename Predicate>
class skip_while_enumerator : public enumerator<typename Source::value_type>
{
private:
	Source source;
	Predicate predicate;
	bool first;
	
	skip_while_enumerator(skip_while_enumerator const&); // not defined
	skip_while_enumerator& operator=(skip_while_enumerator const&); // not defined

public:
	skip_while_enumerator(skip_while_enumerator&& other)
		: source(std::move(other.source))
		, predicate(std::move(other.predicate))
		, first(other.first)
	{
	}

	skip_while_enumerator(Source&& source, Predicate const& predicate)
		: source(std::move(source))
		, predicate(predicate)
		, first(true)
	{
	}
	
	bool move_next()
	{
		if (first)
		{
			first = false;
			while (source.move_next())
			{
				if (!predicate(source.current()))
					return true;
			}
			return false;
		}
		else
		{
			return source.move_next();
		}
	}
	
	value_type current()
	{
		return source.current();
	}
};