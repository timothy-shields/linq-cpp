#pragma once

#include "return_enumerable.h"
#include "select_enumerable.h"

template <typename Enumerable>
class interactive
{
public:
	typedef Enumerable enumerable_type;
	
private:
	enumerable_type enumerable;
	
public:
	interactive(Enumerable&& enumerable)
		: enumerable(std::move(enumerable))
	{
	}

	template <typename Selector>
	interactive<select_enumerable<enumerable_type, Selector>> select(const Selector& selector)
	{
		return select_enumerable<enumerable_type, Selector>(enumerable, selector);
	}
};

template <>
class interactive<void>
{
public:
	template <typename value_type>
	interactive<return_enumerable<value_type>> _return(const value_type& value)
	{
		return return_enumerable<value_type>(value);
	}
	
	template <typename value_type>
	interactive<return_enumerable<value_type>> _return(value_type&& value)
	{
		return return_enumerable<value_type>(value);
	}
};