#pragma once

#include "make_unique.h"
#include "enumerable.h"
#include "memoize_enumerator.h"
#include "memoize_traits.h"

namespace linq {

template <typename Source>
class memoize_enumerable : public enumerable<typename memoize_traits<Source>::value_type>
{
public:
	typedef memoize_enumerator<typename Source::enumerator_type> enumerator_type;
	typedef typename enumerator_type::value_type value_type;

private:
	Source source;

	memoize_enumerable(memoize_enumerable const &); // not defined
	memoize_enumerable& operator=(memoize_enumerable const&); // not defined

public:
	memoize_enumerable(memoize_enumerable&& other)
		: source(std::move(other.source))
	{
	}

	memoize_enumerable(Source&& source)
		: source(std::move(source))
	{
	}

	enumerator_type get_enumerator()
	{
		enumerator_type e(std::move(source.get_enumerator()));
		return e;
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};

}