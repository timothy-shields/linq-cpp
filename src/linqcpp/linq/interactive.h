#pragma once

#include <utility>
#include <vector>

#include "enumerable.h"
#include "captured_enumerable.h"
#include "memoize_enumerable.h"
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

	interactive<concat_enumerable<enumerable_type>> concat()
	{
		return concat_enumerable<enumerable_type>(std::move(source));
	}

	template <typename Selector>
	auto select_many(Selector selector) -> interactive<decltype(select(selector).concat())>
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
	auto take_until(Predicate const& predicate) ->
		interactive<decltype(
			take_while(negate_predicate<value_type>(predicate))
		)>
	{
		return take_while(negate_predicate<value_type>(predicate));
	}

	auto take(std::size_t count) ->
		interactive<decltype(
			take_while(counter_predicate<value_type>(count))
		)>
	{
		return take_while(counter_predicate<value_type>(count));
	}

	template <typename Predicate>
	interactive<skip_while_enumerable<enumerable_type, Predicate>> skip_while(Predicate const& predicate)
	{
		return skip_while_enumerable<enumerable_type, Predicate>(std::move(source), predicate);
	}

	template <typename Predicate>
	auto skip_until(Predicate const& predicate) ->
		interactive<decltype(
			skip_while(negate_predicate<value_type>(predicate))
		)>
	{
		return skip_while(negate_predicate<value_type>(predicate));
	}

	auto skip(std::size_t count) ->
		interactive<decltype(
			skip_while(counter_predicate<value_type>(count))
		)>
	{
		return skip_while(counter_predicate<value_type>(count));
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
		for_each([&](value_type const& value){ vector.emplace_back(value); });
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