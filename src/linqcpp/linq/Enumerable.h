#pragma once

#include <utility>
#include <memory>

#include "enumerator.h"

template<class T, class U>
std::unique_ptr<T> make_unique(U&& u)
{
    return std::unique_ptr<T>(new T(std::forward<U>(u)));
}

// Concept Enumerable<T>
// o Implies:
//   o MoveConstructible: http://en.cppreference.com/w/cpp/concept/MoveConstructible
//   o MoveAssignable: http://en.cppreference.com/w/cpp/concept/MoveAssignable
// o typename value_type
//   o value_type == T
// o typename enumerator_type
//   o Implements concept Enumerator<T>
// o enumerator_type get_enumerator()
//   o Lifetime of x.get_enumerator() should never exceed that of x

// Implements concept
template <typename T>
class enumerable
{
public:
	typedef T value_type;
	virtual std::unique_ptr<enumerator<T>> get_enumerator_ptr() = 0;
};