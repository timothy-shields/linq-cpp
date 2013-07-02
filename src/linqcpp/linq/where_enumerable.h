#pragma once

#include "make_unique.h"
#include "enumerable.h"
#include "where_enumerator.h"

namespace linq {

template <typename Source, typename Predicate>
class where_enumerable : public enumerable<typename Source::value_type>
{
public:
	typedef where_enumerator<typename Source::enumerator_type, Predicate> enumerator_type;
	typedef typename enumerator_type::value_type value_type;

	static_assert(
		is_enumerable<Source>::value,
		"Failed assert: Source meets the Enumerable<T> requirements");

private:
	Source source;
	Predicate predicate;

	where_enumerable(where_enumerable const&); // not defined
	where_enumerable& operator=(where_enumerable const&); // not defined
	
public:
	where_enumerable(where_enumerable&& other)
		: source(std::move(other.source))
		, predicate(std::move(other.predicate))
	{
	}

	where_enumerable(Source&& source, Predicate const& predicate)
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

}