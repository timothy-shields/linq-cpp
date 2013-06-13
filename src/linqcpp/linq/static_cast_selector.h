#pragma once

template <typename Source, typename Target>
class static_cast_selector
{
public:
	Target operator()(Source& value)
	{
		return static_cast<Target>(value);
	}
};