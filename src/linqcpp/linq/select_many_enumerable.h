#pragma once

#include "select_many_enumerator.h"

template <typename Source, typename Selector>
class select_many_enumerable
{
public:
	typedef select_many_enumerator<typename Source::enumerator_type, Selector> enumerator_type;
	typedef typename enumerator_type::value_type value_type;
	
private:
	Source source;
	Selector selector;
	
public:
	select_many_enumerable(const Source& source, const Selector& selector)
		: source(source)
		, selector(selector)
	{
	}

	select_many_enumerable(Source&& source, const Selector& selector)
		: source(std::move(source))
		, selector(selector)
	{
	}

	enumerator_type get_enumerator()
	{
		return enumerator_type(source.get_enumerator(), selector);
	}
};