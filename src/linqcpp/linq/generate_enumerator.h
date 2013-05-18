#pragma once

#include <stdexcept>

#include "enumerator.h"

template <typename Generator>
class generate_enumerator : public enumerator<decltype(std::declval<Generator>())>
{
private:
	value_type value;
	Generator& generator;

public:
	generate_enumerator(Generator& generator)
		: generator(generator)
	{
	}

	bool move_next()
	{
		value = generator();
		return true;
	}

	const value_type& current() const { return value; }
	value_type& current() { return value; }
};