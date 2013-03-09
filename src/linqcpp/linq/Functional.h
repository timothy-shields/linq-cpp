#pragma once

#include "dllapi.h"

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
	static std::function<T ()> Return(T x)
	{
		return [=](){ return x; };
	}

	template<typename T>
	static std::function<T (T)> Increment()
	{
		return [](T x){ return x + 1; };
	}

	template<typename T>
	static std::function<T (T)> Identity()
	{
		return [](T x){ return x; }
	}
};

}