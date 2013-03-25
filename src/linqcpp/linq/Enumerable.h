// Copyright 2013 Timothy Shields
//
// Licensed under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in
// compliance with the License. You may obtain a copy
// of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in
// writing, software distributed under the License is
// distributed on an "AS IS" BASIS, WITHOUT WARRANTIES
// OR CONDITIONS OF ANY KIND, either express or
// implied. See the License for the specific language
// governing permissions and limitations under the
// License.

#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <string>
#include <tuple>
#include <iostream>
#include <sstream>

namespace linq {

enum class Ordering { Ascending, Descending };

template<typename T>
class TEnumerator
{
private:
	const std::function<bool()> moveNext;
	const std::function<T()> current;

public:
	TEnumerator(std::function<bool()>&& moveNext, std::function<T()>&& current)
		: moveNext(std::forward<std::function<bool()>>(moveNext))
		, current(std::forward<std::function<T()>>(current))
	{
	}

	//First call to MoveNext moves from one before the first item onto it
	//Last call to MoveNext moves from the last item to one past it
	bool MoveNext()
	{
		return moveNext();
	}

	//Should be able to call Current as many times as wanted per single call to MoveNext
	T Current()
	{
		return current();
	}
};

class Enumerable;

template<typename T>
class TEnumerable
{
private:
	friend Enumerable;
	std::shared_ptr<const std::function<std::shared_ptr<TEnumerator<T>>()>> getEnumerator;

public:
	typedef T value_type;

	std::shared_ptr<TEnumerator<T>> GetEnumerator()
	{
		return (*getEnumerator)();
	}

	TEnumerable()
	{
	}

	TEnumerable(std::function<std::shared_ptr<TEnumerator<T>>()>&& getEnumerator)
		: getEnumerator(std::make_shared<const std::function<std::shared_ptr<TEnumerator<T>>()>>(std::forward<std::function<std::shared_ptr<TEnumerator<T>>()>>(getEnumerator)))
	{
	}

	TEnumerable<T> ToInclusive(T end)
	{
		return TakeWhile(std::bind2nd(std::less_equal<T>(), end));
	}

	TEnumerable<T> ToExclusive(T end)
	{
		return TakeWhile(std::bind2nd(std::less<T>(), end));
	}

	//TAction: T -> void
	template<typename TAction>
	TEnumerable<T> Do(TAction action)
	{
		auto _getEnumerator(getEnumerator);
		return TEnumerable<T>([=]()
		{
			auto enumerator = (*_getEnumerator)();
			return std::make_shared<TEnumerator<T>>(
				[=]()
				{
					if (enumerator->MoveNext())
					{
						action(enumerator->Current());
						return true;
					}
					else
					{
						return false;
					}
				},
				[=](){ return enumerator->Current(); });
		});
	}

	//TAction: T, int -> void
	template<typename TAction>
	TEnumerable<T> DoIndexed(TAction action)
	{
		struct State
		{
			State(std::shared_ptr<TEnumerator<T>> enumerator)
				: enumerator(enumerator)
				, i(0)
			{
			}
			std::shared_ptr<TEnumerator<T>> enumerator;
			int i;
		};
		auto _getEnumerator(getEnumerator);
		return TEnumerable<T>([=]()
		{
			auto state = std::make_shared<State>((*_getEnumerator)());
			return std::make_shared<TEnumerator<T>>(
				[=]()
				{
					if (state->enumerator->MoveNext())
					{
						action(state->enumerator->Current(), state->i);
						return true;
					}
					else
					{
						return false;
					}
				},
				[=](){ return state->enumerator->Current(); });
		});
	}

	//TSelector: T -> TResult
	template<typename TSelector>
	auto Select(TSelector selector) -> TEnumerable<decltype(selector(std::declval<T>()))>
	{
		typedef decltype(selector(std::declval<T>())) TResult;
		struct State
		{
			State(std::shared_ptr<TEnumerator<T>> enumerator)
				: enumerator(enumerator)
			{
			}
			std::shared_ptr<TEnumerator<T>> enumerator;
			TResult value;
		};
		auto _getEnumerator(getEnumerator);
		return TEnumerable<TResult>([=]()
		{
			auto state = std::make_shared<State>((*_getEnumerator)());
			return std::make_shared<TEnumerator<TResult>>(
				[=]()
				{
					if (state->enumerator->MoveNext())
					{
						state->value = selector(state->enumerator->Current());
						return true;
					}
					else
					{
						return false;
					}
				},
				[=](){ return state->value; });
		});
	}

	TEnumerable<std::pair<T, int>> Index()
	{
		return Enumerable::Zip(*this, Enumerable::Sequence<int>(), std::make_pair<T, int>);
	}

	//TSelector: T, int -> TResult
	template<typename TSelector>
	auto SelectIndexed(TSelector selector) -> TEnumerable<decltype(selector(std::declval<T>()))>
	{
		return Enumerable::Zip(*this, Enumerable::Sequence<int>(), selector);
	}

	template<typename TResult>
	TEnumerable<TResult> StaticCast()
	{
		return Select([](T x){ return static_cast<TResult>(x); });
	}

	template<typename TResult>
	TEnumerable<TResult> DynamicCast()
	{
		return Select([](T x){ return dynamic_cast<TResult>(x); });
	}

	//Assumption: T = TEnumerable<*>
	T SelectMany()
	{
		return SelectMany([](T x){ return x; });
	}

	//TSelector: T -> TEnumerable<TResult>
	template<typename TSelector>
	auto SelectMany(TSelector selector) -> decltype(selector(std::declval<T>()))
	{
		typedef decltype(selector(std::declval<T>())) TResultEnumerable;
		typedef typename TResultEnumerable::value_type TResult;
		struct State
		{
			State(std::shared_ptr<TEnumerator<T>> outerEnumerator)
				: outerEnumerator(outerEnumerator)
			{
			}
			std::shared_ptr<TEnumerator<T>> outerEnumerator;
			std::shared_ptr<TEnumerator<TResult>> innerEnumerator;
			TResult value;
			
		};
		auto _getEnumerator(getEnumerator);
		return TEnumerable<TResult>([=]()
		{
			auto state = std::make_shared<State>((*_getEnumerator)());
			return std::make_shared<TEnumerator<TResult>>(
				[=]()
				{
					while (true)
					{
						if (state->innerEnumerator && state->innerEnumerator->MoveNext())
						{
							state->value = state->innerEnumerator->Current();
							return true;
						}
						else if (state->outerEnumerator->MoveNext())
						{
							state->innerEnumerator = selector(state->outerEnumerator->Current()).GetEnumerator();
						}
						else
						{
							return false;
						}
					}
				},
				[=](){ return state->value; });
		});
	}

	//TPredicate: T -> bool
	template<typename TPredicate>
	TEnumerable<T> Where(TPredicate predicate)
	{
		auto _getEnumerator(getEnumerator);
		return TEnumerable<T>([=]()
		{
			auto enumerator = (*_getEnumerator)();
			return std::make_shared<TEnumerator<T>>(
				[=]()
				{
					while (true)
					{
						if (!enumerator->MoveNext())
						{
							return false;
						}
						if (predicate(enumerator->Current()))
						{
							return true;
						}
					}
				},
				[=](){ return enumerator->Current(); });
		});
	}

	//TPredicate: T, int -> bool
	template<typename TPredicate>
	TEnumerable<T> WhereIndexed(TPredicate predicate)
	{
		struct State
		{
			State(std::shared_ptr<TEnumerator<T>> enumerator)
				: enumerator(enumerator)
				, i(0)
			{
			}
			std::shared_ptr<TEnumerator<T>> enumerator;
			int i;
		};
		auto _getEnumerator(getEnumerator);
		return TEnumerable<T>([=]()
		{
			auto state = std::make_shared<State>((*_getEnumerator)());
			return std::make_shared<TEnumerator<T>>(
				[=]()
				{
					while (true)
					{
						if (!state->enumerator->MoveNext())
						{
							return false;
						}
						if (predicate(state->enumerator->Current(), state->i))
						{
							state->i++;
							return true;
						}
					}
				},
				[=](){ return state->enumerator->Current(); });
		});
	}

	TEnumerable<T> Skip(int count)
	{
		struct State
		{
			State(std::shared_ptr<TEnumerator<T>> enumerator)
				: enumerator(enumerator)
				, n(0)
			{
			}
			std::shared_ptr<TEnumerator<T>> enumerator;
			int n;
		};
		auto _getEnumerator(getEnumerator);
		return TEnumerable<T>([=]()
		{
			auto state = std::make_shared<State>((*_getEnumerator)());
			return std::make_shared<TEnumerator<T>>(
				[=]()
				{
					while (state->n < count)
					{
						state->n++;
						if (!state->enumerator->MoveNext())
						{
							return false;
						}
					}
					return state->enumerator->MoveNext();
				},
				[=](){ return state->enumerator->Current(); });
		});
	}

	//TPredicate: T -> bool
	template<typename TPredicate>
	TEnumerable<T> SkipWhile(TPredicate predicate)
	{
		struct State
		{
			State(std::shared_ptr<TEnumerator<T>> enumerator)
				: enumerator(enumerator)
				, skipping(true)
			{
			}
			std::shared_ptr<TEnumerator<T>> enumerator;
			bool skipping;
		};
		auto _getEnumerator(getEnumerator);
		return TEnumerable<T>([=]()
		{
			auto state = std::make_shared<State>((*_getEnumerator)());
			return std::make_shared<TEnumerator<T>>(
				[=]()
				{
					if (state->skipping)
					{
						while (true)
						{
							if (!state->enumerator->MoveNext())
							{
								return false;
							}
							if (!predicate(state->enumerator->Current()))
							{
								state->skipping = false;
								return true;
							}
						}
					}
					else
					{
						return state->enumerator->MoveNext();
					}
				},
				[=](){ return state->enumerator->Current(); });
		});
	}

	TEnumerable<T> Take(int count)
	{
		struct State
		{
			State(std::shared_ptr<TEnumerator<T>> enumerator)
				: enumerator(enumerator)
				, n(0)
			{
			}
			std::shared_ptr<TEnumerator<T>> enumerator;
			int n;
		};
		auto _getEnumerator(getEnumerator);
		return TEnumerable<T>([=]()
		{
			auto state = std::make_shared<State>((*_getEnumerator)());
			return std::make_shared<TEnumerator<T>>(
				[=]()
				{
					if (state->n < count)
					{
						state->n++;
						return state->enumerator->MoveNext();
					}
					return false;
				},
				[=](){ return state->enumerator->Current(); });
		});
	}

	//TPredicate: T -> bool
	template<typename TPredicate>
	TEnumerable<T> TakeWhile(TPredicate predicate)
	{
		auto _getEnumerator(getEnumerator);
		return TEnumerable<T>([=]()
		{
			auto enumerator = (*_getEnumerator)();
			return std::make_shared<TEnumerator<T>>(
				[=](){ return enumerator->MoveNext() && predicate(enumerator->Current()); },
				[=](){ return enumerator->Current(); });
		});
	}

	//TGreater: T, T -> bool
	template<typename TGreater>
	TEnumerable<T> OrderAscending(TGreater greater)
	{
		struct State
		{
			State(const TGreater& _greater)
				: heap(_greater)
			{
			}
			std::priority_queue<T, std::vector<T>, TGreater> heap;
			T value;
		};
		auto _getEnumerator(getEnumerator);
		return TEnumerable<T>([=]()
		{
			auto enumerator = (*_getEnumerator)();
			auto state = std::make_shared<State>(greater);
			while (enumerator->MoveNext())
			{
				state->heap.push(enumerator->Current());
			}
			return std::make_shared<TEnumerator<T>>(
				[=]()
				{
					if (state->heap.empty())
					{
						return false;
					}
					state->value = state->heap.top();
					state->heap.pop();
					return true;
				},
				[=](){ return state->value; });
		});
	}

	//TLess: T, T -> bool
	template<typename TLess>
	TEnumerable<T> OrderDescending(TLess less)
	{
		struct State
		{
			State(const TLess& _less)
				: heap(_less)
			{
			}
			std::priority_queue<T, std::vector<T>, TLess> heap;
			T value;
		};
		auto _getEnumerator(getEnumerator);
		return TEnumerable<T>([=]()
		{
			auto enumerator = (*_getEnumerator)();
			auto state = std::make_shared<State>(less);
			while (enumerator->MoveNext())
			{
				state->heap.push(enumerator->Current());
			}
			return std::make_shared<TEnumerator<T>>(
				[=]()
				{
					if (state->heap.empty())
					{
						return false;
					}
					state->value = state->heap.top();
					state->heap.pop();
					return true;
				},
				[=](){ return state->value; });
		});
	}

	//TComparer: T, T -> int
	template<typename TComparer>
	TEnumerable<T> Order(Ordering ordering, TComparer comparer)
	{
		if (ordering == Ordering::Ascending)
		{
			return OrderAscending([comparer](const T& a, const T& b){ return comparer(a, b) > 0; });
		}
		else
		{
			return OrderDescending([comparer](const T& a, const T& b){ return comparer(a, b) < 0; });
		}
	}

	TEnumerable<T> Order(Ordering ordering)
	{
		if (ordering == Ordering::Ascending)
		{
			return OrderAscending(std::greater<T>());
		}
		else
		{
			return OrderDescending(std::less<T>());
		}
	}

	template<typename TKeySelector>
	TEnumerable<T> OrderBy(Ordering ordering, TKeySelector keySelector)
	{
		typedef decltype(keySelector(std::declval<T>())) TKey;
		typedef std::pair<TKey, T> TPair;
		auto pairs = Select([=](T& x) -> TPair { return std::make_pair(keySelector(x), x); });
		if (ordering == Ordering::Ascending)
		{
			pairs = pairs.OrderAscending([](const TPair& a, const TPair& b){ return a.first > b.first; });
		}
		else
		{
			pairs = pairs.OrderDescending([](const TPair& a, const TPair& b){ return a.first < b.first; });
		}
		return pairs.Select([](const TPair& x) -> T { return x.second; });
	}

	TEnumerable<T> Distinct()
	{
		auto _set = std::make_shared<std::set<T>>();
		ForEach([=](T x){ _set->insert(x); });
		return Enumerable::FromRange(_set);
	}

	template<typename TKeySelector>
	TEnumerable<T> DistinctBy(TKeySelector keySelector)
	{
		typedef decltype(keySelector(std::declval<T>())) TKey;
		auto _map = std::make_shared<std::map<TKey, T>>();
		ForEach([=](T x){ _map->insert(keySelector(x), x); });
		return Enumerable::FromRange(_map).Select([](std::pair<TKey, T>& _pair) -> T { return _pair.second; });
	}

	template<typename TKeySelector>
	auto GroupBy(TKeySelector keySelector) -> TEnumerable<std::pair<decltype(keySelector(std::declval<T>())), TEnumerable<T>>>
	{
		typedef decltype(keySelector(std::declval<T>())) TKey;
		auto _map = std::make_shared<std::map<TKey, std::shared_ptr<std::vector<T>>>>();
		ForEach([=](T x)
		{
			TKey key = keySelector(x);
			auto it = _map->find(key);
			std::shared_ptr<std::vector<T>> _vector;
			if (it != _map->end())
			{
				_vector = it->second;
			}
			else
			{
				_vector = std::make_shared<std::vector<T>>();
				_map->insert(std::make_pair(key, _vector));
			}
			_vector->push_back(x);
		});
		return Enumerable::FromRange(_map)
			.Select([](std::pair<TKey, std::shared_ptr<std::vector<T>>> _pair)
			{
				return std::make_pair(_pair.first, Enumerable::FromRange(_pair.second));
			});
	}

	bool Any()
	{
		return GetEnumerator()->MoveNext();
	}

	template<typename TPredicate>
	bool Any(TPredicate predicate)
	{
		auto enumerator = GetEnumerator();
		while (enumerator->MoveNext())
		{
			if (predicate(enumerator->Current()))
			{
				return true;
			}
		}
		return false;
	}

	template<typename TPredicate>
	bool All(TPredicate predicate)
	{
		auto enumerator = GetEnumerator();
		while (enumerator->MoveNext())
		{
			if (!predicate(enumerator->Current()))
			{
				return false;
			}
		}
		return true;
	}

	T First()
	{
		auto enumerator = GetEnumerator();
		if (enumerator->MoveNext())
		{
			return enumerator->Current();
		}
		else
		{
			throw runtime_error("TEnumerable<T>::First failed because the enumerable is empty.");
		}
	}

	//TPredicate: T -> bool
	template<typename TPredicate>
	T First(TPredicate predicate)
	{
		return Where(predicate).First();
	}

	T Last()
	{
		auto enumerator = GetEnumerator();
		if (enumerator->MoveNext())
		{
			T item = enumerator->Current();
			while (enumerator->MoveNext())
			{
				item = enumerator->Current();
			}
			return item;
		}
		else
		{
			throw runtime_error("TEnumerable<T>::Last failed because the enumerable is empty.");
		}
	}

	//TPredicate: T -> bool
	template<typename TPredicate>
	T Last(TPredicate predicate)
	{
		return Where(predicate).Last();
	}

	T Single()
	{
		auto enumerator = GetEnumerator();
		if (enumerator->MoveNext())
		{
			T item = enumerator->Current();
			if (enumerator->MoveNext())
			{
				throw runtime_error("TEnumerable<T>::Single failed because the enumerable contains more than one item.");
			}
			return item;
		}
		else
		{
			throw runtime_error("TEnumerable<T>::Single failed because the enumerable is empty.");
		}
	}

	//TPredicate: T -> bool
	template<typename TPredicate>
	T Single(TPredicate predicate)
	{
		return Where(predicate).Single();
	}

	int Count()
	{
		auto enumerator = GetEnumerator();
		int count = 0;
		while (enumerator->MoveNext())
		{
			++count;
		}
		return count;
	}

	//TPredicate: T -> bool
	template<typename TPredicate>
	int Count(TPredicate predicate)
	{
		return Where(predicate).Count();
	}

	//TAccumulator: TAccumulate, T -> TAccumulate
	template<typename TAccumulate, typename TAccumulator>
	TAccumulate Aggregate(TAccumulate seed, TAccumulator accumulator)
	{
		auto enumerator = GetEnumerator();
		TAccumulate agg = seed;
		while (enumerator->MoveNext())
		{
			agg = accumulator(agg, enumerator->Current());
		}
		return agg;
	}

	//TAccumulator: T, T -> T
	template<typename TAccumulator>
	T Aggregate(TAccumulator accumulator)
	{
		auto enumerator = GetEnumerator();
		if (!enumerator->MoveNext())
		{
			throw runtime_error("TEnumerable<T>::Aggregate failed because the enumerable is empty.");
		}
		T agg = enumerator->Current();
		while (enumerator->MoveNext())
		{
			agg = accumulator(agg, enumerator->Current());
		}
		return agg;
	}

	T Sum()
	{
		return Aggregate(static_cast<T>(0), std::plus<T>);
	}

	T Product()
	{
		return Aggregate(static_cast<T>(1), std::multiplies<T>);
	}

	template<typename TSelector>
	auto Min(TSelector selector) -> decltype(selector(std::declval<T>()))
	{
		return Select(selector).Min();
	}

	T Min()
	{
		return Aggregate(std::min<T>);
	}

	template<typename TSelector>
	auto Max(TSelector selector) -> decltype(selector(std::declval<T>()))
	{
		return Select(selector).Max();
	}

	T Max()
	{
		return Aggregate(std::max<T>);
	}

	double Average()
	{
		auto count = 0;
		auto sum = Aggregate(
			0.0,
			[&](double a, T b)
			{
				++count;
				return a + static_cast<double>(b);
			});
		if (count == 0)
		{
			throw runtime_error("TEnumerable<T>::Average failed because the enumerable is empty.");
		}
		return sum / static_cast<double>(count);
	}

	//TKeySelector: T -> TKey
	template<typename TKeySelector>
	T MinBy(TKeySelector keySelector)
	{
		auto enumerator = GetEnumerator();
		if (!enumerator->MoveNext())
		{
			throw runtime_error("TEnumerable<T>::MinBy failed because the enumerable is empty.");
		}
		auto bestValue = enumerator->Current();
		auto bestScore = keySelector(enumerator->Current());
		while (enumerator->MoveNext())
		{
			auto currentValue = enumerator->Current();
			auto currentScore = keySelector(enumerator->Current());
			if (currentScore < bestScore)
			{
				bestValue = currentValue;
				bestScore = currentScore;
			}
		}
		return bestValue;
	}

	//TKeySelector: T -> TKey
	template<typename TKeySelector>
	T MaxBy(TKeySelector keySelector)
	{
		auto enumerator = GetEnumerator();
		if (!enumerator->MoveNext())
		{
			throw runtime_error("TEnumerable<T>::MaxBy failed because the enumerable is empty.");
		}
		auto bestValue = enumerator->Current();
		auto bestScore = keySelector(enumerator->Current());
		while (enumerator->MoveNext())
		{
			auto currentValue = enumerator->Current();
			auto currentScore = keySelector(enumerator->Current());
			if (bestScore < currentScore)
			{
				bestValue = currentValue;
				bestScore = currentScore;
			}
		}
		return bestValue;
	}

	//TKeySelector: T -> TKey
	template<typename TKeySelector>
	int MinIndex(TKeySelector keySelector)
	{
		auto enumerator = GetEnumerator();
		if (!enumerator->MoveNext())
		{
			throw runtime_error("TEnumerable<T>::MinIndex failed because the enumerable is empty.");
		}
		auto bestIndex = 0;
		auto bestScore = keySelector(enumerator->Current());
		auto currentIndex = 0;
		while (enumerator->MoveNext())
		{
			++currentIndex;
			auto currentScore = keySelector(enumerator->Current());
			if (currentScore < bestScore)
			{
				bestIndex = currentIndex;
				bestScore = currentScore;
			}
		}
		return bestIndex;
	}

	int MinIndex()
	{
		return MinIndex([](T x){ return x; });
	}

	//TKeySelector: T -> TKey
	template<typename TKeySelector>
	int MaxIndex(TKeySelector keySelector)
	{
		auto enumerator = GetEnumerator();
		if (!enumerator->MoveNext())
		{
			throw runtime_error("TEnumerable<T>::MaxIndex failed because the enumerable is empty.");
		}
		auto bestIndex = 0;
		auto bestScore = keySelector(enumerator->Current());
		auto currentIndex = 0;
		while (enumerator->MoveNext())
		{
			++currentIndex;
			auto currentScore = keySelector(enumerator->Current());
			if (bestScore < currentScore)
			{
				bestIndex = currentIndex;
				bestScore = currentScore;
			}
		}
		return bestIndex;
	}

	int MaxIndex()
	{
		return MaxIndex([](T x){ return x; });
	}

	//TAction: T -> void
	template<typename TAction>
	void ForEach(TAction action)
	{
		auto enumerator = GetEnumerator();
		while (enumerator->MoveNext())
		{
			action(enumerator->Current());
		}
	}

	//TAction: T, int -> void
	template<typename TAction>
	void ForEachIndexed(TAction action)
	{
		auto enumerator = GetEnumerator();
		auto i = 0;
		while (enumerator->MoveNext())
		{
			action(enumerator->Current(), i);
			++i;
		}
	}

	void IntoVector(std::vector<T>& _vector)
	{
		ForEach([&](T x){ _vector.push_back(x); });
	}

	std::vector<T> ToVector()
	{
		std::vector<T> _vector;
		IntoVector(_vector);
		return _vector;
	}

	void IntoSet(std::set<T>& _set)
	{
		ForEach([&](T x){ _set.insert(x); });
	}

	std::set<T> ToSet()
	{
		std::set<T> _set;
		IntoSet(_set);
		return _set;
	}

	//TKeySelector: T -> TKey
	//TValueSelector: T -> TValue
	template<typename TKey, typename TValue, typename TKeySelector, typename TValueSelector>
	void IntoMap(std::map<TKey, TValue>& _map, TKeySelector keySelector, TValueSelector valueSelector)
	{
		ForEach([&](T x){ _map.insert(make_pair(keySelector(x), valueSelector(x))); });
	}

	//TKeySelector: T -> TKey
	//TValueSelector: T -> TValue
	template<typename TKey, typename TValue, typename TKeySelector, typename TValueSelector>
	std::map<TKey, TValue> ToMap(TKeySelector keySelector, TValueSelector valueSelector)
	{
		std::map<TKey, T> _map;
		IntoMap(_map, keySelector, valueSelector);
		return _map;
	}

	//TKeySelector: T -> TKey
	template<typename TKey, typename TKeySelector>
	void IntoMap(std::map<TKey, T> _map, TKeySelector keySelector)
	{
		ForEach([&](T x){ _map.insert(make_pair(keySelector(x), x)); });
	}

	//TKeySelector: T -> TKey
	template<typename TKey, typename TKeySelector>
	std::map<TKey, T> ToMap(TKeySelector keySelector)
	{
		std::map<TKey, T> _map;
		IntoMap(_map, keySelector);
		return _map;
	}

	//Assumption: T = std::pair<TKey, TValue>
	template<typename TKey, typename TValue>
	void IntoMap(std::map<TKey, TValue>& _map)
	{
		ForEach([&](T x){ _map.insert(x); });
	}

	//Assumption: T = std::pair<TKey, TValue>
	template<typename TKey, typename TValue>
	std::map<TKey, TValue> ToMap()
	{
		std::map<TKey, T> _map;
		IntoMap(_map);
		return _map;
	}

	//TWriter: std::stringstream&, T -> void
	template<typename TWriter>
	std::string ToString(std::string separator, TWriter writer)
	{
		std::stringstream stream;
		bool first = true;
		ForEach([&](T item)
		{
			if (first)
			{
				first = false;
			}
			else
			{
				stream << separator;
			}
			writer(stream, item);
		});
		return stream.str();
	}

	std::string ToString(std::string separator)
	{
		return ToString(separator, [](std::stringstream& stream, T item){ stream << item; });
	}

	std::string ToString()
	{
		return ToString(", ");
	}	
};

class Enumerable
{
private:
	Enumerable() { }

public:
	
	template<typename TRange>
	static auto FromRange(TRange& range) -> TEnumerable<typename TRange::value_type>
	{
		typedef TRange::value_type T;
		typedef TRange::const_iterator TIter;
		struct State
		{
			State(const TIter& iter, bool first)
				: iter(iter)
				, first(first)
			{
			}
			TIter iter;
			bool first;
		};
		return TEnumerable<T>([&range]()
		{
			auto state = std::make_shared<State>(range.begin(), true);
			return std::make_shared<TEnumerator<T>>(
				[state, &range]()
				{
					if (state->first)
					{
						state->first = false;
					}
					else
					{
						state->iter++;
					}
					return state->iter != range.end();
				},
				[state](){ return *state->iter; });
		});
	}

	template<typename TRange>
	static auto FromRange(std::shared_ptr<TRange> range) -> TEnumerable<typename TRange::value_type>
	{
		typedef TRange::value_type T;
		typedef TRange::const_iterator TIter;
		struct State
		{
			State(const TIter& iter, bool first)
				: iter(iter)
				, first(first)
			{
			}
			TIter iter;
			bool first;
		};
		return TEnumerable<T>([=]()
		{
			auto state = std::make_shared<State>(range->begin(), true);
			return std::make_shared<TEnumerator<T>>(
				[=]()
				{
					if (!(state->first))
					{
						state->iter++;
					}
					state->first = false;
					return state->iter != range->end();
				},
				[=](){ return *state->iter; }
			);
		});
	}

	//Factory function creates an enumerable that will only ever be enumerated once
	//TFactory: void -> TEnumerable<T>
	template<typename TFactory>
	static auto Factory(TFactory factory) -> decltype(factory())
	{
		return decltype(factory())([=](){ return (*factory().getEnumerator)(); });
	}

	template<typename T>
	static TEnumerable<T> Repeat(T x)
	{
		return TEnumerable<T>([x]()
		{
			return std::make_shared<TEnumerator<T>>(
				[](){ return true; },
				[x](){ return x; });
		});
	}

	template<typename T>
	static TEnumerable<T> Empty()
	{
		return TEnumerable<T>([]()
		{
			return std::make_shared<TEnumerator<T>>(
				[](){ return false; },
				[](){ throw std::runtime_error("Current should never be called on TEnumerable<T>::Empty."); });
		});
	}

	template<typename T>
	static TEnumerable<T> Return(T item)
	{
		struct State
		{
			State()
				: first(true)
			{
			}
			bool first;
		};
		return TEnumerable<T>([=]()
		{
			auto state = std::make_shared<State>();
			return std::make_shared<TEnumerator<T>>(
				[=]()
				{
					if (state->first)
					{
						state->first = false;
						return true;
					}
					else
					{
						return false;
					}
				},
				[=](){ return item; });
		});
	}
	
	//TFactory: void -> T
	template<typename TFactory>
	static auto Generate(TFactory factory) -> TEnumerable<decltype(factory())>
	{
		typedef decltype(factory()) T;
		struct State
		{
			State() { }
			T value;
		};
		return TEnumerable<T>([=]()
		{
			auto state = std::make_shared<State>();
			return std::make_shared<TEnumerator<T>>(
				[=]()
				{
					state->value = factory();
					return true;
				},
				[=](){ return state->value; });
		});
	}

	//TPredicate: T -> bool
	//TNext: T -> T
	template<typename T, typename TPredicate, typename TNext>
	static TEnumerable<T> Sequence(T start, TPredicate predicate, TNext next)
	{
		struct State
		{
			State()
				: first(true)
			{
			}
			bool first;
			T value;
		};
		return TEnumerable<T>([=]()
		{
			auto state = std::make_shared<State>();
			return std::make_shared<TEnumerator<T>>(
				[=]()
				{
					if (state->first)
					{
						state->first = false;
						state->value = start;
					}
					else
					{
						state->value = next(state->value);
					}
					return predicate(state->value);
				},
				[=](){ return state->value; });
		});
	}

	//TNext: T -> T
	template<typename T, typename TNext>
	static TEnumerable<T> Sequence(T start, TNext next)
	{
		return Sequence(start, [](T _){ return true; }, next);
	}

	template<typename T>
	static TEnumerable<T> Sequence(T start)
	{
		return Sequence<T>(start, [](T x){ return x + 1; });
	}

	template<typename T>
	static TEnumerable<T> Sequence()
	{
		return Sequence<T>(static_cast<T>(0));
	}

	template<typename T>
	static TEnumerable<T> Range(T start, T count)
	{
		return Sequence(start).Take(count);
	}

	template<typename T>
	static TEnumerable<T> Concat(TEnumerable<T> first, TEnumerable<T> second)
	{
		struct State
		{
			State(std::shared_ptr<TEnumerator<T>> firstEnumerator, std::shared_ptr<TEnumerator<T>> secondEnumerator)
				: firstEnumerator(firstEnumerator)
				, secondEnumerator(secondEnumerator)
				, first(true)
			{
			}
			std::shared_ptr<TEnumerator<T>> firstEnumerator;
			std::shared_ptr<TEnumerator<T>> secondEnumerator;
			bool first;
		};
		auto _getFirstEnumerator(first.getEnumerator);
		auto _getSecondEnumerator(second.getEnumerator);
		return TEnumerable<T>([=]()
		{
			auto state = std::make_shared<State>((*_getFirstEnumerator)(), (*_getSecondEnumerator)());
			return std::make_shared<TEnumerator<T>>(
				[=]()
				{
					if (state->first)
					{
						if (state->firstEnumerator->MoveNext())
						{
							return true;
						}
						else
						{
							state->first = false;
						}
					}
					return state->secondEnumerator->MoveNext();
				},
				[=]()
				{
					if (state->first)
					{
						return state->firstEnumerator->Current();
					}
					else
					{
						return state->secondEnumerator->Current();
					}
				});
		});
	}

	//TSelector: T1, T2 -> TResult
	template<typename T1, typename T2, typename TSelector>
	static auto Zip(TEnumerable<T1> first, TEnumerable<T2> second, TSelector selector) -> TEnumerable<decltype(selector(std::declval<T1>(), std::declval<T2>()))>
	{
		typedef decltype(selector(std::declval<T1>(), std::declval<T2>())) TResult;
		struct State
		{
			State(std::shared_ptr<TEnumerator<T1>> firstEnumerator, std::shared_ptr<TEnumerator<T2>> secondEnumerator)
				: firstEnumerator(firstEnumerator)
				, secondEnumerator(secondEnumerator)
			{
			}
			std::shared_ptr<TEnumerator<T1>> firstEnumerator;
			std::shared_ptr<TEnumerator<T2>> secondEnumerator;
		};
		auto _getFirstEnumerator(first.getEnumerator);
		auto _getSecondEnumerator(second.getEnumerator);
		return TEnumerable<TResult>([=]()
		{
			auto state = std::make_shared<State>((*_getFirstEnumerator)(), (*_getSecondEnumerator)());
			return std::make_shared<TEnumerator<TResult>>(
				[=](){ return state->firstEnumerator->MoveNext() && state->secondEnumerator->MoveNext(); },
				[=](){ return selector(state->firstEnumerator->Current(), state->secondEnumerator->Current()); });
		});
	}
};

}
