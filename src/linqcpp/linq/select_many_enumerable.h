#pragma once

#include "enumerable.h"
#include "select_many_enumerator.h"

template <typename Source, typename Selector>
struct select_many_enumerable_traits
{
	typedef typename Source::value_type source_value_type;
	typedef std::declval<Selector>()(std::declval<source_value_type>()) result_type;
	typedef typename result_type::enumerator_type 
	typedef typename result_type::value_type value_type;
}

template <typename Source, typename Selector>
class select_many_enumerable : public enumerable<select_many_enumerable_traits<Source, Selector>::value_type>
{
public:
	typedef select_many_enumerator<typename Source::enumerator_type, Selector> enumerator_type;
	typedef typename enumerator_type::value_type value_type;
	
private:
	Source source;
	Selector selector;
	
public:
	select_many_enumerable(Source&& source, const Selector& selector)
		: source(std::move(source))
		, selector(selector)
	{
	}

	enumerator_type get_enumerator()
	{
		return enumerator_type(std::move(source.get_enumerator()), selector);
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};