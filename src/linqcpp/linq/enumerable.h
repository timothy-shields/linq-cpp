#pragma once

#include <memory>

#include "enumerator.h"

// Concept Enumerable<enumerator_type>
// o Implies:
//   o MoveConstructible: http://en.cppreference.com/w/cpp/concept/MoveConstructible
//   o MoveAssignable: http://en.cppreference.com/w/cpp/concept/MoveAssignable
// o typename enumerator_type
//   o Implements concept Enumerator<T>
// o typename value_type
//   o value_type == T
// o enumerator_type get_enumerator()
//   o Lifetime of x.get_enumerator() should never exceed that of x

// Does not implement concept Enumerable<T> - but inheriting types should
template <typename T>
class enumerable
{
public:
	typedef T value_type;
	virtual std::unique_ptr<enumerator<T>> get_enumerator_ptr() = 0;
};