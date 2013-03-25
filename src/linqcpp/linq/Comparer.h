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
	static std::function<int (const T&, const T&)> Default()
	{
		return [](const T& a, const T& b)
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
	static std::function<int (const T&, const T&)> Extend(std::function<int (const TKey&, const TKey&)> keyComparer, std::function<TKey (const T&)> keySelector)
	{
		return [=](const T& a, const T& b)
		{
			return keyComparer(keySelector(a), keySelector(b));
		};
	}

	template<typename T>
	static std::function<int (const T&, const T&)> Dictionary(std::vector<std::function<int (const T&, const T&)>> comparers)
	{
		return [=](const T& a, const T& b)
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
	static std::function<int (const T&, const T&)> Reverse(std::function<int (const T&, const T&)> comparer)
	{
		return [=](const T& a, const T& b)
		{
			return -comparer(a, b);
		};
	}

	template<typename T>
	static std::function<bool (const T&, const T&)> Less(std::function<int (const T&, const T&)> comparer)
	{
		return [=](const T& a, const T& b)
		{
			return comparer(a, b) < 0;
		};
	}
};

}