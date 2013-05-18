#pragma once

#include "enumerable.h"

template <typename Enumerable>
class captured_enumerable : public enumerable<typename Enumerable::value_type>
{
public:
	typedef typename Enumerable::enumerator_type enumerator_type;

private:
	std::shared_ptr<Enumerable> enumerable_ptr;

public:
	captured_enumerable(const std::shared_ptr<Enumerable>& enumerable_ptr)
		: enumerable_ptr(enumerable_ptr)
	{
	}

	captured_enumerable(std::shared_ptr<Enumerable>&& enumerable_ptr)
		: enumerable_ptr(std::move(enumerable_ptr))
	{
	}
	
	enumerator_type get_enumerator()
	{
		return enumerable_ptr->get_enumerator();
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};