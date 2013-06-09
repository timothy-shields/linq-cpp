#pragma once

#include "enumerator.h"

template <typename Source>
class memoize_enumerator : public enumerator<typename Source::value_type>
{
private:
	Source source;
	value_type value;

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

	value_type& current()
	{
		return value;
	}
};