#pragma once

#include "enumerator.h"

template <typename T>
class captured_enumerator : public enumerator<T>
{
private:
	std::unique_ptr<enumerator<T>> source_ptr;

	captured_enumerator(captured_enumerator const&); // not defined
	captured_enumerator& operator=(captured_enumerator const&); // not defined

public:
	captured_enumerator()
		: source_ptr()
	{
	}

	captured_enumerator(captured_enumerator&& other)
		: source_ptr(std::move(other.source_ptr))
	{
	}

	captured_enumerator& operator=(captured_enumerator&& other)
	{
		source_ptr = std::move(other.source_ptr);
		return *this;
	}

	captured_enumerator(std::unique_ptr<enumerator<T>>&& source_ptr)
		: source_ptr(std::move(source_ptr))
	{
	}

	bool move_next()
	{
		return source_ptr->move_next();
	}

	value_type current()
	{
		return source_ptr->current();
	}
};
