#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <set>
#include <map>
#include <string>

namespace linq {

class Functional
{
public:
	template<typename T>
	static T Increment(T x)
	{
		return x + 1;
	}

	template<typename T>
	static T Identity(T x)
	{
		return x;
	}
};

}