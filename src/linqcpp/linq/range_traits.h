#pragma once

#include <iterator>
#include <utility>

namespace linq
{

namespace adl_range
{
	using std::begin;
	template <typename Range> auto adl_begin(Range& range) -> decltype(begin(range));
}

template <typename Range>
struct range_traits
{
	typedef decltype(adl_range::adl_begin(std::declval<Range>())) iterator_type;
};

}