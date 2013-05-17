#pragma once

#include "where_enumerator.h"

template <typename Source, typename Predicate>
class where_enumerable
{
public:
	typedef where_enumerator<typename Source::enumerator_type, Predicate> enumerator_type;
	typedef typename enumerator_type::value_type value_type;
	
private:
	Source source;
	Predicate predicate;
	
public:
	where_enumerable(const Source& source, const Predicate& predicate)
		: source(source)
		, predicate(predicate)
	{
	}

	where_enumerable(Source&& source, const Predicate& predicate)
		: source(std::move(source))
		, predicate(predicate)
	{
	}

	enumerator_type get_enumerator()
	{
		return enumerator_type(source.get_enumerator(), predicate);
	}
};