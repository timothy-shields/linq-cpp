#pragma once

#include "select_enumerator.h"

template <typename Source, typename Selector>
class select_enumerable
{
public:
	typedef select_enumerator<Source, Selector> enumerator_type;
	typedef enumerator_type::value_type value_type;
	
private:
	Source source;
	Selector selector;
	
public:
	select_enumerable(const Source& source, const Selector& selector)
		: source(source)
		, selector(selector)
	{
	}
	select_enumerable(Source&& source, const Selector& selector)
		: source(std::move(source))
		, selector(selector)
	{
	}
	enumerator_type get_enumerator()
	{
		return enumerator_type(source.get_enumerator(), selector);
	}
};