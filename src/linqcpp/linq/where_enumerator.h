#pragma once

#include "enumerator.h"

template <typename Source, typename Predicate>
class where_enumerator : public enumerator<typename Source::value_type>
{
private:
	Source source;
	Predicate& predicate;
	
	where_enumerator(const where_enumerator&); // not defined
	where_enumerator& operator=(const where_enumerator&); // not defined

public:
	where_enumerator(where_enumerator&& other)
		: source(std::move(other.source))
		, predicate(other.predicate)
	{
	}

	where_enumerator(Source&& source, Predicate& predicate)
		: source(source)
		, predicate(predicate)
	{
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
	
	const value_type& current() const { return source.current(); }
	value_type& current() { return source.current(); }
};