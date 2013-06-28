#pragma once

#include "enumerator.h"

namespace linq {

template <typename SourceA, typename SourceB>
class merge_enumerator : public enumerator<typename SourceA::value_type>
{
public:
	typedef typename SourceA::value_type value_type;

private:
	SourceA sourceA;
	SourceB sourceB;
	bool currentA;
	
	merge_enumerator(merge_enumerator const&); // not defined
	merge_enumerator& operator=(merge_enumerator const&); // not defined

public:
	merge_enumerator(merge_enumerator&& other)
		: sourceA(std::move(other.sourceA))
		, sourceB(std::move(other.sourceB))
		, currentA(other.currentA)
	{
	}

	merge_enumerator(SourceA&& sourceA, SourceB&& sourceB)
		: sourceA(std::move(sourceA))
		, sourceB(std::move(sourceB))
		, currentA(true)
	{
	}
	
	bool move_first()
	{
		bool haveA = sourceA.move_first();
		bool haveB = sourceB.move_first();
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
		if (currentA)
		{
			bool haveA = sourceA.move_next();
		}
		else
		{
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