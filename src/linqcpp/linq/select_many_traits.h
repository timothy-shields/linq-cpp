#pragma once

template <typename Source, typename Selector>
struct select_many_traits
{
	typedef typename Source::value_type source_value_type;
	typedef std::declval<Selector>()(std::declval<source_value_type>()) result_type;
	typedef typename result_type::enumerator_type 
	typedef typename result_type::value_type value_type;
};