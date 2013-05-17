#pragma once

template <typename Source, typename Selector>
class select_many_enumerator
{
private:
	//should use std::result_of here - but doesn't work without variadic templates
	typedef decltype(std::declval<Selector>()(std::declval<typename Source::value_type>())) inner_interactive_type;
	typedef typename inner_interactive_type::enumerable_type inner_enumerable_type;
	typedef typename inner_enumerable_type::enumerator_type inner_enumerator_type;

public:
	typedef typename inner_enumerable_type::value_type value_type;

private:
	Source source;
	Selector& selector;
	bool first;
	inner_enumerator_type enumerator;
	
public:
	select_many_enumerator(Source&& source, Selector& selector)
		: source(std::move(source))
		, selector(selector)
		, first(true)
	{
	}
	
	bool move_next()
	{
		while (true)
		{
			if (!first && enumerator.move_next())
			{
				return true;
			}
			else if (source.move_next())
			{
				enumerator = selector(source.current()).get_enumerator();
			}
			else
			{
				return false;
			}
		}
	}
	
	const value_type& current() const { return enumerator.current(); }
	value_type& current() { return enumerator.current(); }
};