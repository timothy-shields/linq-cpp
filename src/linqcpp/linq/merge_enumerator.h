#pragma once

#include <type_traits>

#include "enumerator.h"

namespace linq {

template <typename SourceA, typename SourceB>
class merge_enumerator : public enumerator<typename SourceA::value_type>
{
public:
	typedef typename SourceA::value_type value_type;

	static_assert(
		is_enumerator<SourceA>::value,
		"Failed assert: SourceA meets the Enumerator<T> requirements");

	static_assert(
		is_enumerator<SourceB>::value,
		"Failed assert: SourceB meets the Enumerator<T> requirements");

	static_assert(
		std::is_same<typename SourceA::value_type, typename SourceB::value_type>::value,
		"Failed assert: SourceA::value_type is the same as SourceB::value_type");

private:
	SourceA sourceA;
	SourceB sourceB;
	bool haveA;
	bool haveB;
	bool currentA;
	
	merge_enumerator(merge_enumerator const&); // not defined
	merge_enumerator& operator=(merge_enumerator const&); // not defined

public:
	merge_enumerator()
	{
	}

	merge_enumerator(merge_enumerator&& other)
		: sourceA(std::move(other.sourceA))
		, sourceB(std::move(other.sourceB))
		, haveA(other.haveA)
		, haveB(other.haveB)
		, currentA(other.currentA)
	{
	}

	merge_enumerator(SourceA&& sourceA, SourceB&& sourceB)
		: sourceA(std::move(sourceA))
		, sourceB(std::move(sourceB))
		, haveA(true)
		, haveB(true)
		, currentA(true)
	{
	}
	
	bool move_first()
	{
		haveA = sourceA.move_first();
		haveB = sourceB.move_first();
		if (haveA && haveB)
		{
			currentA = !(sourceB.current() < sourceA.current());
			return true;
		}
		else if (haveA)
		{
			currentA = true;
			return true;
		}
		else if (haveB)
		{
			currentA = false;
			return true;
		}
		else
		{
			return false;
		}
	}

	bool move_next()
	{
		if (haveA && haveB)
		{
			if (currentA)
			{
				haveA = sourceA.move_next();
			}
			else
			{
				haveB = sourceB.move_next();
			}
			currentA = !(sourceB.current() < sourceA.current());
			return true;
		}
		else if (haveA)
		{
			haveA = sourceA.move_next();
			currentA = true;
			return haveA;
		}
		else if (haveB)
		{
			haveB = sourceB.move_next();
			currentA = false;
			return haveB;
		}
		else
		{
			return false;
		}
	}
	
	value_type current()
	{
		if (currentA)
		{
			return sourceA.current();
		}
		else
		{
			return sourceB.current();
		}
	}
};

}