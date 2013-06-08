#pragma once

#include "enumerator.h"

template <typename Source, typename Selector>
class select_enumerator : public enumerator<decltype(std::declval<Selector>()(std::declval<typename Source::value_type>()))>
{
private:
	Source source;
	Selector selector;
	
	select_enumerator(const select_enumerator&); // not defined
	select_enumerator& operator=(const select_enumerator&); // not defined

public:
	select_enumerator(select_enumerator&& other)
		: source(std::move(other.source))
		, selector(std::move(other.selector))
	{
	}

	select_enumerator(Source&& source, Selector selector)
		: source(std::move(source))
		, selector(selector)
	{
	}
	
	bool move_next() { return source.move_next(); }
	
	value_type current() { return selector(source.current()); }
};