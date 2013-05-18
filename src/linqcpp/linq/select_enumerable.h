#pragma once

#include "enumerable.h"
#include "select_enumerator.h"

template <typename Source, typename Selector>
class select_enumerable : public enumerable<typename select_enumerator<typename Source::enumerator_type, typename Selector>::value_type>
{
public:
	typedef select_enumerator<typename Source::enumerator_type, Selector> enumerator_type;

private:
	Source source;
	Selector selector;
	
public:
	select_enumerable(Source&& source, const Selector& selector)
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