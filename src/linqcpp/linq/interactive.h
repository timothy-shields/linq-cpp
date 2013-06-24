#pragma once

#include <utility>
#include <vector>

#include "enumerable.h"
#include "captured_enumerable.h"
#include "memoize_enumerable.h"
#include "from_enumerable.h"
#include "empty_enumerable.h"
#include "return_enumerable.h"
#include "iota_enumerable.h"
#include "for_enumerable.h"
#include "select_enumerable.h"
#include "concat_enumerable.h"
#include "where_enumerable.h"
#include "take_while_enumerable.h"
#include "skip_while_enumerable.h"
#include "counter_predicate.h"
#include "negated_predicate.h"
#include "static_cast_selector.h"

namespace linq {

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
	interactive<select_enumerable<enumerable_type, static_cast_selector<value_type, T>>> static_cast_()
	{
		return select(static_cast_selector<value_type, T>());
	}

	interactive<concat_enumerable<enumerable_type>> concat()
	{
		return concat_enumerable<enumerable_type>(std::move(source));
	}

	template <typename Selector>
	interactive<concat_enumerable<select_enumerable<enumerable_type, Selector>>> select_many(Selector selector)
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
	interactive<take_while_enumerable<enumerable_type, negated_predicate<value_type, Predicate>>> take_until(Predicate const& predicate)
	{
		return take_while(negated_predicate<value_type, Predicate>(predicate));
	}

	interactive<take_while_enumerable<enumerable_type, counter_predicate<value_type>>> take(std::size_t count)
	{
		return take_while(counter_predicate<value_type>(count));
	}

	template <typename Predicate>
	interactive<skip_while_enumerable<enumerable_type, Predicate>> skip_while(Predicate const& predicate)
	{
		return skip_while_enumerable<enumerable_type, Predicate>(std::move(source), predicate);
	}

	template <typename Predicate>
	interactive<skip_while_enumerable<enumerable_type, negated_predicate<value_type, Predicate>>> skip_until(Predicate const& predicate)
	{
		return skip_while(negated_predicate<value_type, Predicate>(predicate));
	}

	interactive<skip_while_enumerable<enumerable_type, counter_predicate<value_type>>> skip(std::size_t count)
	{
		return skip_while(counter_predicate<value_type>(count));
	}

	template <typename T, typename BinaryOperation>
	T aggregate(T seed, BinaryOperation const& func)
	{
		T value = seed;
		auto e = source.get_enumerator();
		if (!e.move_first())
		{
			return value;
		}
		while (true)
		{
			value = func(value, e.current());
			if (!e.move_next())
			{
				break;
			}
		}
		return value;
	}

	template <typename BinaryOperation>
	value_type aggregate(BinaryOperation const& func)
	{
		auto e = source.get_enumerator();
		move_first_or_throw(e);
		value_type value = e.current();
		while (e.move_next())
		{
			value = func(value, e.current());
		}
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
	typename interactive<select_enumerable<enumerable_type, Selector>>::value_type min(Selector const& selector)
	{
		return select(selector).min();
	}

	template <typename Selector, typename Compare>
	value_type min_by(Selector const& selector, Compare compare = std::less<typename std::result_of<Selector(value_type)>::type>())
	{
		typedef typename std::result_of<Selector(value_type)>::type key_type;

		auto e = source.get_enumerator();
		move_first_or_throw(e);
		value_type min_value = e.current();
		key_type min_key = selector(min_value);
		while (e.move_next())
		{
			value_type current_value = e.current();
			key_type current_key = selector(current_value);
			if (compare(current_key, min_key))
			{
				min_value = current_value;
				min_key = current_key;
			}
		}
		return min_value;
	}

	value_type max()
	{
		return aggregate(std::max<value_type>);
	}

	template <typename Selector>
	typename interactive<select_enumerable<enumerable_type, Selector>>::value_type max(Selector const& selector)
	{
		return select(selector).max();
	}

	template <typename Selector, typename Compare>
	value_type max_by(Selector const& selector, Compare compare = std::less<typename std::result_of<Selector(value_type)>::type>())
	{
		typedef typename std::result_of<Selector(value_type)>::type key_type;
		return min_by(selector, [=](key_type&& a, key_type&& b)
		{
			return compare(std::forward<key_type>(b), std::forward<key_type>(a));
		});
	}

	std::pair<value_type, value_type> minmax()
	{
		auto e = source.get_enumerator();
		move_first_or_throw(e);
		value_type min_value = e.current();
		value_type max_value = min_value;
		while (e.move_next())
		{
			value_type current_value = e.current();
			if (current_value < min_value)
			{
				min_value = current_value;
			}
			else if (max_value < current_value)
			{
				max_value = current_value;
			}
		}
		return std::make_pair(min_value, max_value);
	}

	template <typename Selector>
	std::pair<
		typename interactive<select_enumerable<enumerable_type, Selector>>::value_type,
		typename interactive<select_enumerable<enumerable_type, Selector>>::value_type>
	minmax(Selector const& selector)
	{
		return select(selector).minmax();
	}

	template <typename Selector, typename Compare>
	std::pair<value_type, value_type> minmax_by(Selector const& selector, Compare compare = std::less<typename std::result_of<Selector(value_type)>::type>())
	{
		typedef typename std::result_of<Selector(value_type)>::type key_type;

		auto e = source.get_enumerator();
		move_first_or_throw(e);
		value_type min_value = e.current();
		key_type min_key = selector(min_value);
		value_type max_value = min_value;
		key_type max_key = min_key;
		while (e.move_next())
		{
			value_type current_value = e.current();
			key_type current_key = selector(current_value);
			if (compare(current_key, min_key))
			{
				min_value = current_value;
				min_key = current_key;
			}
			else if (compare(max_key, current_key))
			{
				max_value = current_value;
				max_key = current_key;
			}
		}
		return std::make_pair(min_value, max_value);
	}

	template <typename Predicate>
	bool all(Predicate const& predicate)
	{
		auto e = source.get_enumerator();
		if (!e.move_first())
		{
			return true;
		}
		while (true)
		{
			if (!predicate(e.current()))
				return false;
			if (!e.move_next())
				return true;
		}
	}

	bool any()
	{
		auto e = source.get_enumerator();
		return e.move_first();
	}

	template <typename Predicate>
	bool any(Predicate const& predicate)
	{
		auto e = source.get_enumerator();
		if (!e.move_first())
		{
			return false;
		}
		while (true)
		{
			if (predicate(e.current()))
				return true;
			if (!e.move_next())
				return false;
		}
	}

	value_type first()
	{
		auto e = source.get_enumerator();
		move_first_or_throw(e);
		return e.current();			
	}

	value_type first_or_default(value_type default_value = value_type())
	{
		auto e = source.get_enumerator();
		if (e.move_first())
			return e.current();
		else
			return default_value;
	}

	std::size_t count()
	{
		auto e = source.get_enumerator();
		if (!e.move_first())
			return 0;
		std::size_t n = 1;
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
		if (!e.move_first())
		{
			return;
		}
		while (true)
		{
			action(e.current());
			if (!e.move_next())
				return;
		}
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

template <typename value_type>
static interactive<captured_enumerable<value_type>> capture(std::shared_ptr<enumerable<value_type>> enumerable_ptr)
{
	return captured_enumerable<value_type>(enumerable_ptr);
}

template <typename Range>
static interactive<from_enumerable<Range>> from(Range&& range)
{
	return from_enumerable<Range>(std::forward<Range>(range));
}

template <typename value_type>
static interactive<empty_enumerable<value_type>> empty()
{
	return empty_enumerable<value_type>();
}

template <typename value_type>
static interactive<return_enumerable<value_type>> return_(value_type&& value)
{
	return return_enumerable<value_type>(std::forward<value_type>(value));
}

template <typename value_type>
static interactive<iota_enumerable<value_type>> iota(value_type start)
{
	return iota_enumerable<value_type>(start);
}

template <typename value_type, typename Condition, typename Next>
static interactive<for_enumerable<value_type, Condition, Next>> for_(value_type start, Condition condition, Next next)
{
	return for_enumerable<value_type, Condition, Next>(start, condition, next);
}

}