#pragma once

#include "make_unique.h"
#include "enumerable.h"
#include "return_enumerator.h"

namespace linq {

template <typename T>
class return_enumerable : public enumerable<typename return_enumerator<T>::value_type>
{
public:
	typedef return_enumerator<T> enumerator_type;
	typedef typename enumerator_type::value_type value_type;

private:
	value_type value;

	return_enumerable(return_enumerable const&); // not defined
	return_enumerable& operator=(return_enumerable const&); // not defined

public:
	return_enumerable(return_enumerable&& other)
		: value(std::move(other.value))
	{
	}

	return_enumerable(value_type&& value)
		: value(std::forward<value_type>(value))
	{
	}
	
	enumerator_type get_enumerator()
	{
		return enumerator_type(value);
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};

template <typename T>
class return_enumerable<T&> : public enumerable<typename return_enumerator<T>::value_type>
{
public:
	typedef return_enumerator<T> enumerator_type;
	typedef typename enumerator_type::value_type value_type;

private:
	value_type& value;

	return_enumerable(return_enumerable const&); // not defined
	return_enumerable& operator=(return_enumerable const&); // not defined

public:
	return_enumerable(return_enumerable&& other)
		: value(other.value)
	{
	}

	return_enumerable(value_type& value)
		: value(value)
	{
	}
	
	enumerator_type get_enumerator()
	{
		return enumerator_type(value);
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};

}