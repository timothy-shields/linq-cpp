#pragma once

#include <utility>

#include "enumerable.h"

// Source:
// o Implements concept Enumerable<std::shared_ptr<enumerable<T>>>

template <typename Source>
struct concat_traits
{
	typedef typename Source::value_type outer_value_type;
	typedef typename outer_value_type::element_type inner_enumerable_type;
	typedef typename inner_enumerable_type::enumerator_type inner_enumerator_type;
	typedef typename inner_enumerable_type::value_type inner_value_type;
};

template <typename Source>
class concat_enumerable : public enumerable<typename concat_traits<Source>::inner_value_type>
{
public:
	typedef concat_enumerator<Source> enumerator_type;
	
private:
	Source source;
	
public:
	concat_enumerable(Source&& source)
		: source(std::move(source))
	{
	}

	enumerator_type get_enumerator()
	{
		return enumerator_type(std::move(source.get_enumerator()));
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};

template <typename Source>
class concat_enumerator : public enumerator<typename concat_traits<Source>::inner_value_type>
{
private:
	typedef typename concat_traits<Source>::inner_enumerable_type inner_enumerable_type;
	typedef typename concat_traits<Source>::inner_enumerator_type inner_enumerator_type;

	Source source;
	bool first;
	inner_enumerator_type inner_enumerator;
	
public:
	concat_enumerator(Source&& source)
		: source(std::move(source))
		, first(true)
	{
	}
	
	bool move_next()
	{
		while (true)
		{
			if (!first && inner_enumerator.move_next())
			{
				return true;
			}
			else if (source.move_next())
			{
				inner_enumerator = std::move(source.current().get_enumerator());
			}
			else
			{
				return false;
			}
		}
	}
	
	value_type current() { return inner.current(); }
};