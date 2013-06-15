#pragma once

#include "enumerator.h"

template <typename Source, typename Predicate>
class take_while_enumerator : public enumerator<typename Source::value_type>
{
public:
	typedef typename Source::value_type value_type;

private:
	Source source;
	Predicate predicate;
	
	take_while_enumerator(take_while_enumerator const&); // not defined
	take_while_enumerator& operator=(take_while_enumerator const&); // not defined

public:
	take_while_enumerator(take_while_enumerator&& other)
		: source(std::move(other.source))
		, predicate(std::move(other.predicate))
	{
	}

	take_while_enumerator(Source&& source, Predicate const& predicate)
		: source(std::move(source))
		, predicate(predicate)
	{
	}
	
	bool move_first()
	{
		return source.move_first() && predicate(source.current());
	}

	bool move_next()
	{
		return source.move_next() && predicate(source.current());
	}
	
	value_type current()
	{
		return source.current();
	}
};