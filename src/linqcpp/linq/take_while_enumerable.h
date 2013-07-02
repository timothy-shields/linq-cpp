#pragma once

#include "make_unique.h"
#include "enumerable.h"
#include "take_while_enumerator.h"

namespace linq {

template <typename Source, typename Predicate>
class take_while_enumerable : public enumerable<typename Source::value_type>
{
public:
	typedef take_while_enumerator<typename Source::enumerator_type, Predicate> enumerator_type;
	typedef typename enumerator_type::value_type value_type;

private:
	Source source;
	Predicate predicate;

	take_while_enumerable(take_while_enumerable const&); // not defined
	take_while_enumerable& operator=(take_while_enumerable const&); // not defined
	
public:
	take_while_enumerable(take_while_enumerable&& other)
		: source(std::move(other.source))
		, predicate(std::move(other.predicate))
	{
	}

	take_while_enumerable(Source&& source, Predicate const& predicate)
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