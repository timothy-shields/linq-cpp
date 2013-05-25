#pragma once

#include <utility>

#include "enumerable.h"
#include "concat_traits.h"
#include "concat_enumerator.h"

template <typename Source>
class concat_enumerable : public enumerable<typename concat_traits<Source>::inner_value_type>
{
public:
	typedef concat_enumerator<Source> enumerator_type;
	
private:
	Source source;
	
public:
	concat_enumerable(Source&& source)
		: source(source)
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