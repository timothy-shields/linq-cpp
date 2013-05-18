#pragma once

#include <utility>
#include <vector>

#include "captured_enumerable.h"
#include "empty_enumerable.h"
#include "return_enumerable.h"
#include "for_enumerable.h"
#include "generate_enumerable.h"
#include "select_enumerable.h"
#include "select_many_enumerable.h"
#include "where_enumerable.h"

template <typename Enumerable>
class interactive
{
public:
	typedef Enumerable enumerable_type;
	typedef typename enumerable_type::value_type value_type;
	
private:
	enumerable_type enumerable;
	
public:
	interactive(Enumerable&& enumerable)
		: enumerable(std::move(enumerable))
	{
	}

	std::shared_ptr<enumerable_type> ref_count()
	{
		return std::make_shared<enumerable_type>(std::move(enumerable));
	}

	template <typename Selector>
	interactive<select_enumerable<enumerable_type, Selector>> select(const Selector& selector)
	{
		return select_enumerable<enumerable_type, Selector>(std::move(enumerable), selector);
	}

	template <typename Selector>
	interactive<select_many_enumerable<enumerable_type, Selector>> select_many(const Selector& selector)
	{
		return select_many_enumerable<enumerable_type, Selector>(enumerable, selector);
	}

	template <typename Predicate>
	interactive<where_enumerable<enumerable_type, Predicate>> _where(const Predicate& predicate)
	{
		return where_enumerable<enumerable_type, Predicate>(std::move(enumerable), predicate);
	}

	template <typename Action>
	void for_each(const Action& action)
	{
		auto enumerator = enumerable.get_enumerator();
		while (enumerator.move_next())
			action(enumerator.current());
	}

	std::vector<value_type> to_vector()
	{
		std::vector<value_type> vector;
		for_each([&](const value_type& value){ vector.push_back(value); });
		return vector;
	}
};

template <>
class interactive<void>
{
public:
	template <typename enumerable_type>
	static interactive<captured_enumerable<enumerable_type>> capture(const std::shared_ptr<enumerable_type>& enumerable_ptr)
	{
		return captured_enumerable<enumerable_type>(enumerable_ptr);
	}

	template <typename enumerable_type>
	static interactive<captured_enumerable<enumerable_type>> capture(std::shared_ptr<enumerable_type>&& enumerable_ptr)
	{
		return captured_enumerable<enumerable_type>(std::move(enumerable_ptr));
	}

	template <typename value_type>
	static interactive<empty_enumerable<value_type>> empty()
	{
		return empty_enumerable<value_type>();
	}

	template <typename value_type>
	static interactive<return_enumerable<value_type>> _return(const value_type& value)
	{
		return return_enumerable<value_type>(value);
	}

	template <typename value_type, typename Condition, typename Next>
	static interactive<for_enumerable<value_type, Condition, Next>> _for(const value_type& start, const Condition& condition, const Next& next)
	{
		return for_enumerable<value_type, Condition, Next>(start, condition, next);
	}

	template <typename Generator>
	static interactive<generate_enumerable<Generator>> generate(const Generator& generator)
	{
		return generate_enumerable<Generator>(generator);
	}
};