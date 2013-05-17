#pragma once

#include <type_traits>

template <typename Source, typename Selector>
class select_enumerator
{
public:
	typedef std::result_of<Selector, Source::value_type>::type value_type;

private:
	Source source;
	Selector& selector;
	value_type value;
	
public:
	select_enumerator(Source&& source, Selector& selector)
		: source(std::move(source))
		, selector(selector)
	{
	}
	
	bool move_next()
	{
		if (source.move_next())
		{
			value = selector(source.current());
			return true;
		}
		else
		{
			return false;
		}
	}
	
	const value_type& current() const { return value; }
	value_type& current() { return value; }
};