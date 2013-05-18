#pragma once

//MoveConstructable: http://en.cppreference.com/w/cpp/concept/MoveConstructible
//MoveAssignable: http://en.cppreference.com/w/cpp/concept/MoveAssignable
template <typename T>
class enumerator
{
public:
	typedef T value_type;
	virtual bool move_next() = 0;
	virtual const value_type& current() const = 0;
	virtual value_type& current() = 0;
};