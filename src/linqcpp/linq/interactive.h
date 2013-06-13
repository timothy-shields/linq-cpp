#pragma once

#include <utility>
#include <vector>

#include "enumerable.h"
#include "captured_enumerable.h"
#include "memoize_enumerable.h"
#include "from_enumerable.h"
#include "empty_enumerable.h"
#include "return_enumerable.h"
#include "for_enumerable.h"
#include "generate_enumerable.h"
#include "select_enumerable.h"
#include "concat_enumerable.h"
#include "where_enumerable.h"
#include "take_while_enumerable.h"
#include "skip_while_enumerable.h"
#include "counter_predicate.h"
#include "negated_predicate.h"
#include "static_cast_selector.h"

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

	interactive(interactive const& other); // not defined
	interactive& operator=(interactive const& other); // not defined
	
public:
	interactive(interactive&& other)
		: source(std::move(other.source))
	{
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

	interactive<memoize_enumerable<enumerable_type>> memoize()
	{
		return memoize_enumerable<enumerable_type>(std::move(source));
	}

	template <typename Selector>
	interactive<select_enumerable<enumerable_type, Selector>> select(Selector const& selector)
	{
		return select_enumerable<enumerable_type, Selector>(std::move(source), selector);
	}

	template <typename T>
	auto static_cast_() -> decltype(select(static_cast_selector<value_type, T>()))
	{
		return select(static_cast_selector<value_type, T>());
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
	interactive<where_enumerable<enumerable_type, Predicate>> where(Predicate const& predicate)
	{
		return where_enumerable<enumerable_type, Predicate>(std::move(source), predicate);
	}

	template <typename Predicate>
	interactive<take_while_enumerable<enumerable_type, Predicate>> take_while(Predicate const& predicate)
	{
		return take_while_enumerable<enumerable_type, Predicate>(std::move(source), predicate);
	}

	template <typename Predicate>
	auto take_until(Predicate const& predicate) -> decltype(take_while(negate_predicate<value_type>(predicate)))
	{
		return take_while(negate_predicate<value_type>(predicate));
	}

	auto take(std::size_t count) -> decltype(take_while(counter_predicate<value_type>(count)))
	{
		return take_while(counter_predicate<value_type>(count));
	}

	template <typename Predicate>
	interactive<skip_while_enumerable<enumerable_type, Predicate>> skip_while(Predicate const& predicate)
	{
		return skip_while_enumerable<enumerable_type, Predicate>(std::move(source), predicate);
	}

	template <typename Predicate>
	auto skip_until(Predicate const& predicate) -> decltype(skip_while(negate_predicate<value_type>(predicate)))
	{
		return skip_while(negate_predicate<value_type>(predicate));
	}

	auto skip(std::size_t count) -> decltype(skip_while(counter_predicate<value_type>(count)))
	{
		return skip_while(counter_predicate<value_type>(count));
	}

	template <typename T, typename BinaryOperation>
	T aggregate(T seed, BinaryOperation const& func)
	{
		T value = seed;
		auto e = source.get_enumerator();
		while (e.move_next())
			value = func(value, e.current());
		return value;
	}

	template <typename T, typename BinaryOperation>
	T aggregate(BinaryOperation const& func)
	{
		T value;
		auto e = source.get_enumerator();
		if (!e.move_next())
			throw std::logic_error("Should never call aggregate on an empty enumerable.");
		value = e.current();
		while (e.move_next())
			value = func(value, e.current());
		return value;
	}

	value_type sum()
	{
		return aggregate(static_cast<value_type>(0), std::plus<value_type>());
	}

	value_type product()
	{
		return aggregate(static_cast<value_type>(1), std::multiplies<value_type>());
	}

	value_type min()
	{
		return aggregate(std::min<value_type>);
	}

	template <typename Selector>
	auto min(Selector const& selector) -> decltype(select(selector).min())
	{
		return select(selector).min();
	}

	value_type max()
	{
		return aggregate(std::max<value_type>);
	}

	template <typename Selector>
	auto max(Selector const& selector) -> decltype(select(selector).max())
	{
		return select(selector).max();
	}

	template <typename Predicate>
	bool all(Predicate const& predicate)
	{
		auto e = source.get_enumerator();
		while (e.move_next())
		{
			if (!predicate(e.current()))
				return false;
		}
		return true;
	}

	bool any()
	{
		auto e = source.get_enumerator();
		return e.move_next();
	}

	template <typename Predicate>
	bool any(Predicate const& predicate)
	{
		auto e = source.get_enumerator();
		while (e.move_next())
		{
			if (predicate(e.current()))
				return true;
		}
		return false;
	}

	value_type first()
	{
		auto e = source.get_enumerator();
		if (e.move_next())
			return e.current();
		else
			throw std::logic_error("Should never call first on an empty enumerable.");
	}

	value_type first_or_default(value_type default_value = value_type())
	{
		auto e = source.get_enumerator();
		if (e.move_next())
			return e.current();
		else
			return default_value;
	}

	std::size_t count()
	{
		std::size_t n = 0;
		auto e = source.get_enumerator();
		while (e.move_next())
			++n;
		return n;
	}

	template <typename Predicate>
	std::size_t count_if(Predicate const& predicate)
	{
		return where(predicate).count();
	}

	template <typename Action>
	void for_each(Action const& action)
	{
		auto e = source.get_enumerator();
		while (e.move_next())
			action(e.current());
	}

	std::vector<value_type> to_vector()
	{
		std::vector<value_type> vector;
		into_vector(vector);
		return vector;
	}

	void into_vector(std::vector<value_type>& vector)
	{
		for_each([&](value_type const& value){ vector.emplace_back(value); });
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

	template <typename Iterator>
	static interactive<from_enumerable<Iterator>> from(Iterator const& begin, Iterator const& end)
	{
		return from_enumerable<Iterator>(begin, end);
	}

	template <typename value_type>
	static interactive<empty_enumerable<value_type>> empty()
	{
		return empty_enumerable<value_type>();
	}

	template <typename value_type>
	static interactive<return_enumerable<value_type>> return_(value_type value)
	{
		return return_enumerable<value_type>(value);
	}

	template <typename value_type, typename Condition, typename Next>
	static interactive<for_enumerable<value_type, Condition, Next>> for_(value_type start, Condition condition, Next next)
	{
		return for_enumerable<value_type, Condition, Next>(start, condition, next);
	}

	template <typename Generator>
	static interactive<generate_enumerable<Generator>> generate(Generator generator)
	{
		return generate_enumerable<Generator>(generator);
	}
};