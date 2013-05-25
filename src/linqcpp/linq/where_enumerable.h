#pragma once

#include "enumerable.h"
#include "where_enumerator.h"

template <typename Source, typename Predicate>
class where_enumerable : public enumerable<typename Source::value_type>
{
public:
	typedef where_enumerator<typename Source::enumerator_type, Predicate> enumerator_type;

private:
	Source source;
	Predicate predicate;
	
public:
	where_enumerable(where_enumerable&& other)
		: source(std::move(other.source))
		, predicate(std::move(other.predicate))
	{
	}

	where_enumerable(Source&& source, const Predicate& predicate)
		: source(std::move(source))
		, predicate(predicate)
	{
	}

	enumerator_type get_enumerator()
	{
		return enumerator_type(std::move(source.get_enumerator()), predicate);
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};