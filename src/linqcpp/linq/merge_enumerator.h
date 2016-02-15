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
	bool depletedA;
	bool depletedB;
	bool currentA;
	
	merge_enumerator(merge_enumerator const&); // not defined
	merge_enumerator& operator=(merge_enumerator const&); // not defined

public:
	merge_enumerator(merge_enumerator&& other)
		: sourceA(std::move(other.sourceA))
		, sourceB(std::move(other.sourceB))
		, depletedA(other.depletedA)
		, depletedB(other.depletedB)
		, currentA(other.currentA)
	{
	}

	merge_enumerator(SourceA&& sourceA, SourceB&& sourceB)
		: sourceA(std::move(sourceA))
		, sourceB(std::move(sourceB))
		, depletedA(false)
		, depletedB(false)
		, currentA(true)
	{
	}
	
	bool move_first()
	{
		depletedA = !sourceA.move_first();
		depletedB = !sourceB.move_first();
		if (depletedA && depletedB)
		{
			return false;
		}
		else if (depletedA)
		{
			currentA = false;
			return true;
		}
		else if (depletedB)
		{
			currentA = true;
			return true;
		}
		else
		{
			currentA = !(sourceB.current() < sourceA.current());
			return true;
		}
	}

	bool move_next()
	{
		if (depletedA && depletedB)
		{
			return false;
		}
		else if (depletedA)
		{
			currentA = false;
			depletedB = !sourceB.move_next();
			return !depletedB;
		}
		else if (depletedB)
		{
			currentA = true;
			depletedA = !sourceA.move_next();
			return !depletedA;
		}
		else
		{
			if (currentA)
			{
				depletedA = !sourceA.move_next();
				if (depletedA)
				{
					currentA = false;
					return true;
				}
			}
			else
			{
				depletedB = !sourceB.move_next();
				if (depletedB)
				{
					currentA = true;
					return true;
				}
			}
			currentA = !(sourceB.current() < sourceA.current());
			return true;
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