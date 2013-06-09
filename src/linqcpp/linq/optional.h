#pragma once

#include <utility>

template<typename T>
class optional
{
public:
	typedef T value_type;

private:
	bool _has_value;
	value_type _value;

public:
	optional()
		: _has_value(false)
		, _value()
	{
	}

	optional(value_type&& value)
		: _has_value(true)
		, _value(std::move(value))
	{
	}

	optional& operator=(value_type&& value)
	{
		_has_value = true;
		_value = std::move(value);
		return *this;
	}

	optional(optional&& other)
		: _has_value(other._has_value)
		, _value(std::move(other._value))
	{
	}

	optional& operator=(optional&& other)
	{
		_has_value = other._has_value;
		_value = std::move(other._value);
		return *this;
	}

	operator bool()
	{
		return _has_value;
	}

	T& value()
	{
		return _value;
	}

	T const& value() const
	{
		return _value;
	}
};