#pragma once

#include "make_unique.h"
#include "range_traits.h"
#include "enumerable.h"
#include "from_enumerator.h"

template <typename Range>
class from_enumerable : public enumerable<typename from_enumerator<typename linq::range_traits<Range>::iterator_type>::value_type>
{
public:
	typedef from_enumerator<typename linq::range_traits<Range>::iterator_type> enumerator_type;
	typedef typename enumerator_type::value_type value_type;

private:
	Range range;

	from_enumerable(from_enumerable const&); // not defined
	from_enumerable& operator=(from_enumerable const&); // not defined
	
public:
	from_enumerable(from_enumerable&& other)
		: range(std::move(other.range))
	{
	}

	from_enumerable(Range&& range)
		: range(std::forward<Range>(range))
	{
	}

	enumerator_type get_enumerator()
	{
		using std::begin;
		using std::end;
		return enumerator_type(begin(range), end(range));
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};

template <typename Range>
class from_enumerable<Range&> : public enumerable<typename from_enumerator<typename linq::range_traits<Range>::iterator_type>::value_type>
{
public:
	typedef from_enumerator<typename linq::range_traits<Range>::iterator_type> enumerator_type;
	typedef typename enumerator_type::value_type value_type;

private:
	Range& range;

	from_enumerable(from_enumerable const&); // not defined
	from_enumerable& operator=(from_enumerable const&); // not defined
	
public:
	from_enumerable(from_enumerable&& other)
		: range(other.range)
	{
	}

	from_enumerable(Range& range)
		: range(range)
	{
	}

	enumerator_type get_enumerator()
	{
		using std::begin;
		using std::end;
		return enumerator_type(std::move(begin(range)), std::move(end(range)));
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};