#pragma once

#include "enumerable.h"
#include "from_enumerator.h"

template <typename Iterator>
class from_enumerable : public enumerable<typename from_enumerator<Iterator>::value_type>
{
public:
	typedef from_enumerator<Iterator> enumerator_type;

private:
	Iterator begin;
	Iterator end;

	from_enumerable(from_enumerable const&); // not defined
	from_enumerable& operator=(from_enumerable const&); // not defined
	
public:
	from_enumerable(from_enumerable&& other)
		: begin(std::move(other.begin))
		, end(std::move(other.end))
	{
	}

	from_enumerable(Iterator const& begin, Iterator const& end)
		: begin(begin)
		, end(end)
	{
	}

	enumerator_type get_enumerator()
	{
		return enumerator_type(begin, end);
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};