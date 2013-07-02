#pragma once

#include "make_unique.h"
#include "enumerable.h"
#include "merge_enumerator.h"

namespace linq {

template <typename SourceA, typename SourceB>
class merge_enumerable : public enumerable<typename merge_enumerator<typename SourceA::enumerator_type, typename SourceB::enumerator_type>::value_type>
{
public:
	typedef merge_enumerator<typename SourceA::enumerator_type, typename SourceB::enumerator_type> enumerator_type;
	typedef typename enumerator_type::value_type value_type;

private:
	SourceA sourceA;
	SourceB sourceB;

	merge_enumerable(merge_enumerable const&); // not defined
	merge_enumerable& operator=(merge_enumerable const&); // not defined
	
public:
	merge_enumerable(merge_enumerable&& other)
		: sourceA(std::move(other.sourceA))
		, sourceB(std::move(other.sourceB))
	{
	}

	merge_enumerable(SourceA&& sourceA, SourceB&& sourceB)
		: sourceA(std::move(sourceA))
		, sourceB(std::move(sourceB))
	{
	}

	enumerator_type get_enumerator()
	{
		return enumerator_type(
			std::move(sourceA.get_enumerator()),
			std::move(sourceB.get_enumerator()));
	}

	std::unique_ptr<enumerator<value_type>> get_enumerator_ptr()
	{
		return make_unique<enumerator_type>(std::move(get_enumerator()));
	}
};

}