#pragma once

#include <stdexcept>

// Concept Enumerator<T>
// o Implies:
//   o DefaultConstructible: http://en.cppreference.com/w/cpp/concept/DefaultConstructible
//   o MoveConstructible: http://en.cppreference.com/w/cpp/concept/MoveConstructible
//   o MoveAssignable: http://en.cppreference.com/w/cpp/concept/MoveAssignable
// o typename value_type
//   o value_type == T
// o bool move_next()
//   o Returns true if and only if there is a next value
// o value_type current()
// o Starts "one before" first value

// Implements concept Enumerator<T> - as should inheriting types
template <typename T>
class enumerator
{
public:
	typedef T value_type;
	virtual bool move_next() = 0;
	virtual value_type current() = 0;
};

template <typename Enumerator>
void move_next_or_throw(Enumerator& e)
{
	if (!e.move_next())
		throw new std::logic_error("move_next returned false");
}