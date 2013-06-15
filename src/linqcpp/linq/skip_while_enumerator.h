#pragma once

#include "enumerator.h"

template <typename Source, typename Predicate>
class skip_while_enumerator : public enumerator<typename Source::value_type>
{
public:
	typedef typename Source::value_type value_type;

private:
	Source source;
	Predicate predicate;
	
	skip_while_enumerator(skip_while_enumerator const&); // not defined
	skip_while_enumerator& operator=(skip_while_enumerator const&); // not defined

public:
	skip_while_enumerator(skip_while_enumerator&& other)
		: source(std::move(other.source))
		, predicate(std::move(other.predicate))
	{
	}

	skip_while_enumerator(Source&& source, Predicate const& predicate)
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

		while (predicate(source.current()))
		{
			if (!source.move_next())
				return false;
		}

		return true;
	}
	
	bool move_next()
	{
		return source.move_next();
	}
	
	value_type current()
	{
		return source.current();
	}
};