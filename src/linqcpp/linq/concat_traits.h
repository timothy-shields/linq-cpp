#pragma once

template <typename Source>
struct concat_traits
{
	typedef typename Source::value_type outer_value_type;
	typedef typename outer_value_type::enumerator_type inner_enumerator_type;
	typedef typename outer_value_type::value_type inner_value_type;
};
