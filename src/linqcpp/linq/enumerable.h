#pragma once

#include <memory>
#include <type_traits>

#include "enumerator.h"

// Concept Enumerable<T>
// o Implies:
//   o MoveConstructible: http://en.cppreference.com/w/cpp/concept/MoveConstructible
//   o MoveAssignable: http://en.cppreference.com/w/cpp/concept/MoveAssignable
// o typename enumerator_type
//   o Implements concept Enumerator<T>
// o typename value_type
//   o value_type == T
// o enumerator_type get_enumerator()
//   o Lifetime of x.get_enumerator() should never exceed that of x

namespace linq {

// Does not (have to) implement concept Enumerable<T> - but inheriting types should
template <typename T>
class enumerable
{
public:
	typedef T value_type;
	virtual std::unique_ptr<enumerator<value_type>> get_enumerator_ptr() = 0;
};

template <typename Enumerable>
struct is_enumerable
{
	typedef Enumerable enumerable_type;
	typedef typename enumerable_type::enumerator_type enumerator_type;

	static const bool value = true;

	static_assert(
		std::is_move_constructible<enumerable_type>::value,
		"Failed assert: enumerable_type meets the MoveConstructible requirements");

	static_assert(
		std::is_move_assignable<enumerable_type>::value,
		"Failed assert: enumerable_type meets the MoveAssignable requirements");

	static_assert(
		is_enumerator<enumerator_type>::value,
		"Failed assert: enumerator_type meets the Enumerator<T> requirements");

	static_assert(
		std::is_same<typename enumerable_type::value_type, typename enumerator_type::value_type>::value,
		"Failed assert: enumerable_type::value_type is the same as enumerator_type::value_type");

};

}