#pragma once

namespace linq {

template <typename Source>
struct memoize_traits
{
	typedef typename std::add_lvalue_reference<typename Source::value_type>::type value_type;
};

}