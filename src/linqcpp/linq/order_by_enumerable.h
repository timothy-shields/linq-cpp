#pragma once

#include "make_unique.h"
#include "enumerable.h"
#include "order_by_enumerator.h"

namespace linq {

template <typename Source, typename Compare>
class order_by_enumerable : public enumerable<typename std::remove_reference<typename Source::value_type>::type>
{
public:
	typedef order_by_enumerator<typename Source::enumerator_type, Compare> enumerator_type;
	typedef typename std::remove_reference<typename Source::value_type>::type value_type;
	typedef Source source_type;
	typedef Compare compare_type;


	Source source;
	Compare compare;
private:
	order_by_enumerable(order_by_enumerable const&); // not defined
	order_by_enumerable& operator=(order_by_enumerable const&); // not defined

public:
	order_by_enumerable(order_by_enumerable&& other)
		: source(std::move(other.source))
		, compare(std::move(other.compare))
	{
	}

	order_by_enumerable(Source&& source, Compare const& compare)
		: source(std::move(source))
		, compare(compare)
	{
	}

	enumerator_type get_enumerator()
	{
		return enumerator_type(source.get_enumerator(), compare);
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};

}
