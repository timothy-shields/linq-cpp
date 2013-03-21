#pragma once

#include <functional>
#include <vector>

namespace linq {

class Comparer
{
private:
	Comparer() { }
public:
	template<typename T>
	static std::function<int (T, T)> Default()
	{
		return [](T a, T b)
		{
			if (a < b)
			{
				return -1;
			}
			else if (b < a)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		};
	}

	template<typename T, typename TKey>
	static std::function<int (T, T)> Default(std::function<TKey (T)> keySelector)
	{
		return [=](T a, T b)
		{
			auto aKey = keySelector(a);
			auto bKey = keySelector(b);

			if (aKey < bKey)
			{
				return -1;
			}
			else if (bKey < aKey)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		};
	}

	template<typename T>
	static std::function<int (T, T)> Dictionary(std::vector<std::function<int (T, T)>> comparers)
	{
		return [=](T a, T b)
		{
			for (auto iter = comparers.begin(); iter != comparers.end(); ++iter)
			{
				auto c = (*iter)(a, b);
				if (c != 0)
				{
					return c;
				}
			}
			return 0;
		};
	}

	template<typename T>
	static std::function<int (T, T)> Reverse(std::function<int (T, T)> comparer)
	{
		return [=](T a, T b)
		{
			return comparer(b, a);
		};
	}
};

}