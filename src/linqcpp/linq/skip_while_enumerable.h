#pragma once

#include "enumerable.h"
#include "skip_while_enumerator.h"

template <typename Source, typename Predicate>
class skip_while_enumerable : public enumerable<typename Source::value_type>
{
public:
	typedef skip_while_enumerator<typename Source::enumerator_type, Predicate> enumerator_type;

private:
	Source source;
	Predicate predicate;

	skip_while_enumerable(skip_while_enumerable const&); // not defined
	skip_while_enumerable& operator=(skip_while_enumerable const&); // not defined
	
public:
	skip_while_enumerable(skip_while_enumerable&& other)
		: source(std::move(other.source))
		, predicate(std::move(other.predicate))
	{
	}

	skip_while_enumerable(Source&& source, Predicate const& predicate)
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