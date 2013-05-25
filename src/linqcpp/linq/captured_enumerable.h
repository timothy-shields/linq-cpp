#pragma once

#include "enumerable.h"
#include "captured_enumerator.h"

template <typename T>
class captured_enumerable : public enumerable<T>
{
public:
	typedef captured_enumerator<T> enumerator_type;

private:
	std::shared_ptr<enumerable<T>> source_ptr;

	captured_enumerable(const captured_enumerable&); // not defined
	captured_enumerable& operator=(const captured_enumerable&); // not defined

public:
	captured_enumerable(captured_enumerable&& other)
		: source_ptr(std::move(other.source_ptr))
	{
	}

	captured_enumerable(const std::shared_ptr<enumerable<T>>& source_ptr)
		: source_ptr(source_ptr)
	{
	}
	
	enumerator_type get_enumerator()
	{
		return enumerator_type(std::move(get_enumerator_ptr()));
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return source_ptr->get_enumerator_ptr();
	}
};
