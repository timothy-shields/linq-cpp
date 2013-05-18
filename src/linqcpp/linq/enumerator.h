#pragma once

// Concept Enumerator<T>
// o Implies:
//   o DefaultConstructible: http://en.cppreference.com/w/cpp/concept/DefaultConstructible
//   o MoveConstructible: http://en.cppreference.com/w/cpp/concept/MoveConstructible
//   o MoveAssignable: http://en.cppreference.com/w/cpp/concept/MoveAssignable
// o typename value_type
//   o value_type == T
// o bool move_next()
//   o Returns true if and only if there is a next value
// o const value_type& current() const
// o value_type& current()
// o Starts "one before" first value

template <typename T>
class enumerator
{
public:
	typedef T value_type;
	virtual bool move_next() = 0;
	virtual const value_type& current() const = 0;
	virtual value_type& current() = 0;
};