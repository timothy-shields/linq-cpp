#pragma once

#include "enumerable.h"
#include "generate_enumerator.h"

template <typename Generator>
class generate_enumerable : public enumerable<typename generate_enumerator<Generator>::value_type>
{
public:
	typedef generate_enumerator<Generator> enumerator_type;

private:
	Generator generator;

public:
	generate_enumerable(Generator generator)
		: generator(generator)
	{
	}

	enumerator_type get_enumerator()
	{
		return enumerator_type(generator);
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};