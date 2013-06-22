#pragma once

#include "enumerator.h"

namespace linq {

template <typename Source, typename Selector>
class select_enumerator : public enumerator<typename std::result_of<Selector(typename Source::value_type)>::type>
{
public:
	typedef typename std::result_of<Selector(typename Source::value_type)>::type value_type;

private:
	Source source;
	Selector selector;
	
	select_enumerator(select_enumerator const&); // not defined
	select_enumerator& operator=(select_enumerator const&); // not defined

public:
	select_enumerator(select_enumerator&& other)
		: source(std::move(other.source))
		, selector(std::move(other.selector))
	{
	}

	select_enumerator(Source&& source, Selector const& selector)
		: source(std::move(source))
		, selector(selector)
	{
	}
	
	bool move_first()
	{
		return source.move_first();
	}

	bool move_next()
	{
		return source.move_next();
	}
	
	value_type current()
	{
		return selector(source.current());
	}
};

}