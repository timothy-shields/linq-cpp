#pragma once

#include <utility>

#include "enumerator.h"

namespace linq {

template <typename T>
class iota_enumerator : public enumerator<T>
{
public:
	typedef T value_type;

	static_assert(
		std::is_default_constructible<value_type>::value,
		"Failed assert: iota_enumerator<T>::value_type meets the DefaultConstructible requirements");

	static_assert(
		std::is_move_constructible<value_type>::value,
		"Failed assert: iota_enumerator<T>::value_type meets the MoveConstructible requirements");

	static_assert(
		std::is_move_assignable<value_type>::value,
		"Failed assert: iota_enumerator<T>::value_type meets the MoveAssignable requirements");

private:
	bool first;
	value_type value;
	
public:
	iota_enumerator()
	{
	}

	iota_enumerator(iota_enumerator&& other)
		: first(std::move(other.first))
		, value(std::move(other.value))
	{
	}

	iota_enumerator& operator=(iota_enumerator&& other)
	{
		first = std::move(other.first);
		value = std::move(other.value);
	}

	iota_enumerator(value_type start)
		: first(true)
		, value(start)
	{
	}

	bool move_first()
	{
		return true;
	}
	
	bool move_next()
	{
		++value;
		return true;
	}
	
	value_type current()
	{
		return value;
	}
};

}