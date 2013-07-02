#pragma once

#include <type_traits>

#include "enumerator.h"
#include "memoize_traits.h"

namespace linq {

template <typename Source>
class memoize_enumerator : public enumerator<typename memoize_traits<Source>::value_type>
{
public:
	typedef typename memoize_traits<Source>::value_type value_type;

	static_assert(
		is_enumerator<Source>::value,
		"Failed assert: Source meets the Enumerator<T> requirements");

	static_assert(
		std::is_default_constructible<value_type>::value,
		"Failed assert: Source::value_type meets the DefaultConstructible requirements");

private:
	Source source;
	typename Source::value_type value;

	memoize_enumerator(const memoize_enumerator&); // not defined
	memoize_enumerator& operator=(const memoize_enumerator&); // not defined

public:
	memoize_enumerator(memoize_enumerator&& other)
		: source(std::move(other.source))
		, value(std::move(other.value))
	{
	}

	memoize_enumerator(Source&& source)
		: source(std::move(source))
		, value()
	{
	}

	bool move_first()
	{
		if (source.move_first())
		{
			value = source.current();
			return true;
		}
		else
		{
			return false;
		}
	}

	bool move_next()
	{
		if (source.move_next())
		{
			value = source.current();
			return true;
		}
		else
		{
			return false;
		}
	}

	value_type current()
	{
		return value;
	}
};

}