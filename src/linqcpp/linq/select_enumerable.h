#pragma once

#include <type_traits>

#include "make_unique.h"
#include "enumerable.h"
#include "select_enumerator.h"

namespace linq {

template <typename Source, typename Selector>
class select_enumerable : public enumerable<typename select_enumerator<typename Source::enumerator_type, Selector>::value_type>
{
public:
	typedef select_enumerator<typename Source::enumerator_type, Selector> enumerator_type;
	typedef typename enumerator_type::value_type value_type;

	static_assert(
		is_enumerable<Source>::value,
		"Failed assert: Source meets the Enumerable<T> requirements");

private:
	Source source;
	Selector selector;

	select_enumerable(select_enumerable const&); // not defined
	select_enumerable& operator=(select_enumerable const&); // not defined
	
public:
	select_enumerable(select_enumerable&& other)
		: source(std::move(other.source))
		, selector(std::move(other.selector))
	{
	}

	select_enumerable(Source&& source, Selector const& selector)
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

}