#pragma once

#include <utility>
#include <vector>

#include "enumerable.h"
#include "captured_enumerable.h"
#include "empty_enumerable.h"
#include "return_enumerable.h"
#include "for_enumerable.h"
#include "generate_enumerable.h"
#include "select_enumerable.h"
#include "concat_enumerable.h"
#include "where_enumerable.h"

// Implements Enumerable<T>
template <typename Enumerable>
class interactive
{
public:
	typedef Enumerable enumerable_type;
	typedef typename enumerable_type::enumerator_type enumerator_type;
	typedef typename enumerable_type::value_type value_type;
	
private:
	enumerable_type source;

	interactive(const interactive& other); // not defined
	interactive& operator=(const interactive& other); // not defined
	
public:
	interactive()
		: source()
	{
	}

	interactive(interactive&& other)
	{
		*this = std::move(other);
	}

	interactive& operator=(interactive&& other)
	{
		source = std::move(other.source);
		return *this;
	}

	interactive(enumerable_type&& source)
		: source(std::move(source))
	{
	}

	enumerator_type get_enumerator()
	{
		return source.get_enumerator();
	}

	std::shared_ptr<enumerable<value_type>> ref_count()
	{
		return std::make_shared<enumerable_type>(std::move(source));
	}

	interactive<captured_enumerable<value_type>> capture()
	{
		return captured_enumerable<value_type>(ref_count());
	}

	template <typename Selector>
	interactive<select_enumerable<enumerable_type, Selector>> select(Selector selector)
	{
		return select_enumerable<enumerable_type, Selector>(std::move(source), selector);
	}

	interactive<concat_enumerable<enumerable_type>> concat()
	{
		return concat_enumerable<enumerable_type>(std::move(source));
	}
	
	template <typename Selector>
	auto select_many(Selector selector) -> decltype(select(selector).concat())
	{
		return select(selector).concat();
	}

	template <typename Predicate>
	interactive<where_enumerable<enumerable_type, Predicate>> _where(Predicate predicate)
	{
		return where_enumerable<enumerable_type, Predicate>(std::move(source), predicate);
	}

	template <typename Action>
	void for_each(const Action& action)
	{
		auto e = source.get_enumerator();
		while (e.move_next())
			action(e.current());
	}

	std::vector<value_type> to_vector()
	{
		std::vector<value_type> vector;
		for_each([&](const value_type& value){ vector.emplace_back(value); });
		return vector;
	}
};

class ix
{
public:
	template <typename value_type>
	static interactive<captured_enumerable<value_type>> capture(std::shared_ptr<enumerable<value_type>> enumerable_ptr)
	{
		return captured_enumerable<value_type>(enumerable_ptr);
	}

	template <typename value_type>
	static interactive<empty_enumerable<value_type>> empty()
	{
		return empty_enumerable<value_type>();
	}

	template <typename value_type>
	static interactive<return_enumerable<value_type>> _return(value_type value)
	{
		return return_enumerable<value_type>(value);
	}

	template <typename value_type, typename Condition, typename Next>
	static interactive<for_enumerable<value_type, Condition, Next>> _for(value_type start, Condition condition, Next next)
	{
		return for_enumerable<value_type, Condition, Next>(start, condition, next);
	}

	template <typename Generator>
	static interactive<generate_enumerable<Generator>> generate(Generator generator)
	{
		return generate_enumerable<Generator>(generator);
	}
};