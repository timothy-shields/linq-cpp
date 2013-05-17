#pragma once

template <typename T>
class empty_enumerator
{
public:
	typedef T value_type;
	bool move_next() { return false; }
};