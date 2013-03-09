#pragma once

#include "dllapi.h"

#include <memory>
#include <functional>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <tuple>

#include "Comparer.h"
#include "Functional.h"
#include "PairingHeap.h"

namespace linq {

template<typename T>
class PairingHeap;

template<typename T>
class TEnumerator
{
private:
	const std::function<bool()> moveNext;
	const std::function<T()> current;

public:
	TEnumerator(std::function<bool()> moveNext, std::function<T()> current)
		: moveNext(moveNext)
		, current(current)
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
	friend Enumerable;
private:
	std::shared_ptr<const std::function<std::shared_ptr<TEnumerator<T>>()>> getEnumerator;

public:
	std::shared_ptr<TEnumerator<T>> GetEnumerator()
	{
		return (*getEnumerator)();
	}

	TEnumerable()
	{
	}

	TEnumerable(std::function<std::shared_ptr<TEnumerator<T>>()> getEnumerator)
		: getEnumerator(std::make_shared<const std::function<std::shared_ptr<TEnumerator<T>>()>>(getEnumerator))
	{
	}

	TEnumerable<T> ToInclusive(T end) {
		return TakeWhile([=](T item){ return item <= end; });
	}

	TEnumerable<T> ToExclusive(T end) {
		return TakeWhile([=](T item){ return item < end; });
	}

	//TSelector: T -> TResult
	template<typename TResult, typename TSelector>
	TEnumerable<TResult> Select(TSelector selector)
	{
		struct State
		{
			State() { }
			std::shared_ptr<TEnumerator<T>> enumerator;
			TResult value;
		};

		auto _getEnumerator(getEnumerator);

		return TEnumerable<TResult>
		(
			[=]()
			{
				auto state = std::make_shared<State>();
				state->enumerator = (*_getEnumerator)();

				return std::make_shared<TEnumerator<TResult>>
				(
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
					[=]()
					{
						return state->value;
					}
				);
			}
		);
	}

	TEnumerable<std::tuple<T, int>> Index()
	{
		return Enumerable::Zip<std::tuple<T, int>>(*this, Enumerable::From(0), [](T x, int i){ return std::make_tuple(x, i); });
	}

	//TSelector: T, int -> TResult
	template<typename TResult, typename TSelector>
	TEnumerable<TResult> SelectIndexed(TSelector selector)
	{
		return Enumerable::Zip(*this, Enumerable::From(0), selector);
	}

	template<typename TResult>
	TEnumerable<TResult> StaticCast()
	{
		return Select<TResult>([](T item)
		{
			return static_cast<TResult>(item);
		});
	}

	template<typename TResult>
	TEnumerable<TResult> DynamicCast()
	{
		return Select<TResult>([](T item)
		{
			return dynamic_cast<TResult>(item);
		});
	}

	//Assumption: T = TEnumerable<TResult>
	template<typename TResult>
	TEnumerable<TResult> SelectMany()
	{
		return SelectMany(Functional::Identity<T>());
	}

	//TSelector: T -> TEnumerable<TResult>
	template<typename TResult, typename TSelector>
	TEnumerable<TResult> SelectMany(TSelector selector)
	{
		struct State
		{
			std::shared_ptr<TEnumerator<T>> outerEnumerator;
			std::shared_ptr<TEnumerator<TResult>> innerEnumerator;
			TResult value;
			State() { }
		};

		auto _getEnumerator(getEnumerator);

		return TEnumerable<TResult>
		(
			[=]()
			{
				auto state = std::make_shared<State>();
				state->outerEnumerator = (*_getEnumerator)();

				return std::make_shared<TEnumerator<TResult>>
				(
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
					[=]()
					{
						return state->value;
					}
				);
			}
		);
	}

	//TPredicate: T -> bool
	template<typename TPredicate>
	TEnumerable<T> Where(TPredicate predicate)
	{
		struct State
		{
			State() { }
			std::shared_ptr<TEnumerator<T>> enumerator;
		};

		auto _getEnumerator(getEnumerator);

		return TEnumerable<T>
		(
			[=]()
			{
				auto state = std::make_shared<State>();
				state->enumerator = (*_getEnumerator)();

				return std::make_shared<TEnumerator<T>>
				(
					[=]()
					{
						while (true)
						{
							if (!state->enumerator->MoveNext())
							{
								return false;
							}
							if (predicate(state->enumerator->Current()))
							{
								return true;
							}
						}
					},
					[=]()
					{
						return state->enumerator->Current();
					}
				);
			}
		);
	}

	//TPredicate: T, int -> bool
	template<typename TPredicate>
	TEnumerable<T> WhereIndexed(TPredicate predicate)
	{
		struct State
		{
			State() { }
			std::shared_ptr<TEnumerator<T>> enumerator;
			int i;
		};

		auto _getEnumerator(getEnumerator);

		return TEnumerable<T>
		(
			[=]()
			{
				auto state = std::make_shared<State>();
				state->enumerator = (*_getEnumerator)();
				state->i = 0;

				return std::make_shared<TEnumerator<T>>
				(
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
					[=]()
					{
						return state->enumerator->Current();
					}
				);
			}
		);
	}

	TEnumerable<T> Skip(int count)
	{
		struct State
		{
			State() { }
			std::shared_ptr<TEnumerator<T>> enumerator;
			int n;
		};

		auto _getEnumerator(getEnumerator);

		return TEnumerable<T>
		(
			[=]()
			{
				auto state = std::make_shared<State>();
				state->enumerator = (*_getEnumerator)();
				state->n = 0;

				return std::make_shared<TEnumerator<T>>
				(
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
					[=]()
					{
						return state->enumerator->Current();
					}
				);
			}
		);
	}

	//TPredicate: T -> bool
	template<typename TPredicate>
	TEnumerable<T> SkipWhile(TPredicate predicate)
	{
		struct State
		{
			State() { }
			std::shared_ptr<TEnumerator<T>> enumerator;
			bool skipping;
		};

		auto _getEnumerator(getEnumerator);

		return TEnumerable<T>
		(
			[=]()
			{
				auto state = std::make_shared<State>();
				state->enumerator = (*_getEnumerator)();
				state->skipping = true;

				return std::make_shared<TEnumerator<T>>
				(
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
					[=]()
					{
						return state->enumerator->Current();
					}
				);
			}
		);
	}

	TEnumerable<T> Take(int count)
	{
		struct State
		{
			State() { }
			std::shared_ptr<TEnumerator<T>> enumerator;
			int n;
		};

		auto _getEnumerator(getEnumerator);

		return TEnumerable<T>
		(
			[=]()
			{
				auto state = std::make_shared<State>();
				state->enumerator = (*_getEnumerator)();
				state->n = 0;

				return std::make_shared<TEnumerator<T>>
				(
					[=]()
					{
						if (state->n < count)
						{
							state->n++;
							return state->enumerator->MoveNext();
						}
						return false;
					},
					[=]()
					{
						return state->enumerator->Current();
					}
				);
			}
		);
	}

	//TPredicate: T -> bool
	template<typename TPredicate>
	TEnumerable<T> TakeWhile(TPredicate predicate) {
		auto _getEnumerator(getEnumerator);
		return TEnumerable<T>(
			[=]()->std::shared_ptr<TEnumerator<T>>{
				auto enumerator = (*_getEnumerator)();
				return std::make_shared<TEnumerator<T>>(
					[=]()->bool{
						return enumerator->MoveNext() && predicate(enumerator->Current());
					},
					[=]()->T{
						return enumerator->Current();
					});
			});
	}

	TEnumerable<T> Order(std::function<int (T, T)> comparer) {

		struct State
		{
			State() { }
			PairingHeap<T> heap;
			T value;
		};

		auto _getEnumerator(getEnumerator);
		return TEnumerable<T>(
			[=]()->std::shared_ptr<TEnumerator<T>>{
				auto enumerator = (*_getEnumerator)();
				auto state = std::make_shared<State>();
				while (enumerator->MoveNext())
					state->heap.Insert(enumerator->Current());
				return std::make_shared<TEnumerator<T>>(
					[=]()->bool{
						if (state->heap.IsEmpty())
							return false;
						state->value = state->heap.ExtractMin()->value;
						return true;
					},
					[=]()->T{
						return state->value;
					});
			});
	}

	TEnumerable<T> Order()
	{
		return Order(Comparer::Default<T>());
	}

	template<typename TKey>
	TEnumerable<T> OrderByKey(std::function<TKey (T)> keySelector) {
		return
			Select<std::pair<TKey, T>>([=](T item){
				return std::make_pair(keySelector(item), item);
			})
			.Order(
				Comparer<std::pair<TKey, T>>::Default<TKey>([](std::pair<TKey, T> p){
					return p.first;
				}))
			.Select<T>([](std::pair<TKey, T> pair){
				return pair.second;
			});
	}

	bool Any()
	{
		auto enumerator = GetEnumerator();
		return enumerator->MoveNext();
	}

	template<typename TPredicate>
	bool Any(TPredicate predicate)
	{
		auto enumerator = GetEnumerator();
		while (enumerator->MoveNext())
			if (predicate(enumerator->Current()))
				return true;
		return false;
	}

	template<typename TPredicate>
	bool All(TPredicate predicate)
	{
		auto enumerator = GetEnumerator();
		while (enumerator->MoveNext())
			if (!predicate(enumerator->Current()))
				return false;
		return true;
	}

	T First() {
		auto enumerator = GetEnumerator();
		if (enumerator->MoveNext())
			return enumerator->Current();
		else
			throw runtime_error("TEnumerable<T>::First failed because the enumerable is empty.");
	}

	T First(std::function<bool(T)> predicate) {
		return Where(predicate).First();
	}

	T Last() {
		auto enumerator = GetEnumerator();
		if (enumerator->MoveNext()) {
			T item = enumerator->Current();
			while (enumerator->MoveNext()) {
				item = enumerator->Current();
			}
			return item;
		}
		else
			throw runtime_error("TEnumerable<T>::First failed because the enumerable is empty.");
	}

	T Last(std::function<bool(T)> predicate) {
		return Where(predicate).Last();
	}

	T Single() {
		auto enumerator = GetEnumerator();
		if (enumerator->MoveNext()) {
			T item = enumerator->Current();
			if (enumerator->MoveNext())
				throw runtime_error("TEnumerable<T>::Single failed because the enumerable contains more than one item.");
			return item;
		}
		else {
			throw runtime_error("TEnumerable<T>::Single failed because the enumerable is empty.");
		}
	}

	T Single(std::function<bool(T)> predicate) {
		return Where(predicate).Single();
	}

	int Count() {
		auto enumerator = GetEnumerator();
		int count = 0;
		while (enumerator->MoveNext())
			++count;
		return count;
	}

	int Count(std::function<bool(T)> predicate) {
		return Where(predicate).Count();
	}

	template<typename TAccumulate>
	TAccumulate Aggregate(TAccumulate seed, std::function<TAccumulate (TAccumulate, T)> accumulator)
	{
		auto enumerator = GetEnumerator();
		TAccumulate agg = seed;
		while (enumerator->MoveNext())
		{
			agg = accumulator(agg, enumerator->Current());
		}
		return agg;
	}

	T Aggregate(std::function<T (T, T)> accumulator) {
		auto enumerator = GetEnumerator();
		if (!enumerator->MoveNext())
			throw runtime_error("TEnumerable<T>::Aggregate failed because the enumerable is empty.");
		T agg = enumerator->Current();
		while (enumerator->MoveNext())
			agg = accumulator(agg, enumerator->Current());
		return agg;
	}

	T Sum() {
		return Aggregate<T>(0, [](T a, T b){ return a + b; });
	}

	T Min() {
		return Aggregate([](T a, T b){ return std::min(a, b); });
	}

	T Max() {
		return Aggregate([](T a, T b){ return std::max(a, b); });
	}

	double Average() {
		auto count = 0;
		auto sum = Aggregate<double>(0,
			[&](double a, T b)->double{
				++count;
				return a + b;
			});
		if (count == 0)
			throw runtime_error("TEnumerable<T>::Average failed because the enumerable is empty.");
		return sum / count;
	}

	template<typename TScore>
	T MinBy(std::function<TScore(T)> score) {
		auto enumerator = GetEnumerator();
		if (!enumerator->MoveNext())
			throw runtime_error("TEnumerable<T>::MinBy failed because the enumerable is empty.");
		auto bestValue = enumerator->Current();
		auto bestScore = score(enumerator->Current());
		while (enumerator->MoveNext()) {
			auto currentValue = enumerator->Current();
			auto currentScore = score(enumerator->Current());
			if (currentScore < bestScore) {
				bestValue = currentValue;
				bestScore = currentScore;
			}
		}
		return bestValue;
	}

	template<typename TScore>
	T MaxBy(std::function<TScore(T)> score) {
		auto enumerator = GetEnumerator();
		if (!enumerator->MoveNext())
			throw runtime_error("TEnumerable<T>::MaxBy failed because the enumerable is empty.");
		auto bestValue = enumerator->Current();
		auto bestScore = score(enumerator->Current());
		while (enumerator->MoveNext()) {
			auto currentValue = enumerator->Current();
			auto currentScore = score(enumerator->Current());
			if (currentScore > bestScore) {
				bestValue = currentValue;
				bestScore = currentScore;
			}
		}
		return bestValue;
	}

	T Indexmin() {
		return Indexmin([](T item){ return item; });
	}

	template<typename TScore>
	T Indexmin(std::function<TScore(T)> score) {
		auto enumerator = GetEnumerator();
		if (!enumerator->MoveNext())
			throw runtime_error("TEnumerable<T>::Indexmin failed because the enumerable is empty.");
		auto bestIndex = 0;
		auto bestScore = score(enumerator->Current());
		auto currentIndex = 0;
		while (enumerator->MoveNext()) {
			++currentIndex;
			auto currentScore = score(enumerator->Current());
			if (currentScore < bestScore) {
				bestIndex = currentIndex;
				bestScore = currentScore;
			}
		}
		return bestIndex;
	}

	T Indexmax() {
		return Indexmax([](T item){ return item; });
	}

	template<typename TScore>
	T Indexmax(std::function<TScore(T)> score) {
		auto enumerator = GetEnumerator();
		if (!enumerator->MoveNext())
			throw runtime_error("TEnumerable<T>::Indexmax failed because the enumerable is empty.");
		auto bestIndex = 0;
		auto bestScore = score(enumerator->Current());
		auto currentIndex = 0;
		while (enumerator->MoveNext()) {
			++currentIndex;
			auto currentScore = score(enumerator->Current());
			if (currentScore > bestScore) {
				bestIndex = currentIndex;
				bestScore = currentScore;
			}
		}
		return bestIndex;
	}

	void ForEach(std::function<void(T)> action) {
		auto enumerator = GetEnumerator();
		while (enumerator->MoveNext())
			action(enumerator->Current());
	}

	void ForEachIndexed(std::function<void(T, int)> action) {
		auto enumerator = GetEnumerator();
		int i = 0;
		while (enumerator->MoveNext()) {
			action(enumerator->Current(), i);
			++i;
		}
	}

	std::vector<T> ToVector() {
		std::vector<T> _vector;
		ForEach([&](T item){
			_vector.push_back(item);
		});
		return _vector;
	}

	std::set<T> ToSet() {
		std::set<T> _set;
		ForEach([&](T item){
			_set.insert(item);
		});
		return _set;
	}

	template<typename TKey, typename TValue>
	std::map<TKey, TValue> ToMap() {
		std::map<TKey, TValue> _map;
		ForEach([&](pair<TKey, TValue> item){
			_map.insert(item);
		});
		return _map;
	}

	template<typename TKey>
	std::map<TKey, T> ToMap(std::function<TKey(T)> keySelector) {
		std::map<TKey, T> _map;
		ForEach([&](T item){
			_map.insert(make_pair(keySelector(item), item));
		});
		return _map;
	}

	template<typename TKey, typename TValue>
	std::map<TKey, TValue> ToMap(std::function<TKey(T)> keySelector, std::function<TValue(T)> valueSelector) {
		std::map<TKey, T> _map;
		ForEach([&](T item){
			_map.insert(make_pair(keySelector(item), valueSelector(item)));
		});
		return _map;
	}

	std::string ToString() {
		return ToString(", ");
	}

	std::string ToString(std::string separator) {
		return ToString(separator,
			[](T item, std::stringstream& stream){
				stream << item;
			});
	}

	std::string ToString(std::string separator, std::function<void (T, std::stringstream&)> write) {
		std::stringstream stream;
		bool first = true;
		ForEach([&](T item){
			if (first)
				first = false;
			else
				stream << separator;
			write(item, stream);
		});
		return stream.str();
	}
};

class Enumerable
{
private:
	Enumerable() { }

public:
	template<typename T, typename TRange>
	static TEnumerable<T> FromRange(TRange& range)
	{
		return FromRange(range.begin(), range.end());
	}

	template<typename T, typename TIter>
	static TEnumerable<T> FromRange(TIter begin, TIter end)
	{
		struct State
		{
			State() { }
			TIter iter;
			bool first;
		};

		return TEnumerable<T>
		(
			[=]()
			{
				auto state = std::make_shared<State>();
				state->iter = begin;
				state->first = true;
				return std::make_shared<TEnumerator<T>>
				(
					[=]()
					{
						if (!(state->first))
						{
							state->iter++;
						}
						state->first = false;
						return state->iter != end;
					},
					[=]()
					{
						return *(state->iter);
					}
				);
			}
		);
	}

	template<typename T>
	static TEnumerable<T> Empty()
	{
		return TEnumerable<T>
		(
			[]()
			{
				return std::make_shared<TEnumerator<T>>
				(
					[]()
					{
						return false;
					},
					[]()
					{
						throw std::runtime_error("Current should never be called on TEnumerable<T>::Empty.");
					}
				);
			}
		);
	}

	template<typename T>
	static TEnumerable<T> Return(T item)
	{
		struct State
		{
			State() { }
			bool first;
		};

		return TEnumerable<T>
		(
			[=]()
			{
				auto state = std::make_shared<State>();
				state->first = true;

				return std::make_shared<TEnumerator<T>>
				(
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
					[=]()
					{
						return item;
					}
				);
			}
		);
	}

	template<typename T, typename TPredicate, typename TNext>
	static TEnumerable<T> Generate(T start, TPredicate predicate, TNext next)
	{
		struct State
		{
			State() { }
			bool first;
			T value;
		};

		return TEnumerable<T>
		(
			[=]()
			{
				auto state = std::make_shared<State>();
				state->first = true;

				return std::make_shared<TEnumerator<T>>
				(
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
					[=]()
					{
						return state->value;
					}
				);
			}
		);
	}

	template<typename T, typename TNext>
	static TEnumerable<T> Generate(T start, TNext next)
	{
		return Generate(start, [](T _){ return true; }, next);
	}

	template<typename T>
	static TEnumerable<T> Range(T start, T count)
	{
		return Generate<T>(start, [](T value){ return value + 1; }).Take(count);
	}

	template<typename T>
	static TEnumerable<T> From(T start)
	{
		return Generate<T>(start, [](T value){ return value + 1; });
	}

	template<typename T>
	static TEnumerable<T> Concat(TEnumerable<T> first, TEnumerable<T> second)
	{
		struct State
		{
			State() { }
			std::shared_ptr<TEnumerator<T>> firstEnumerator;
			std::shared_ptr<TEnumerator<T>> secondEnumerator;
			bool first;
		};

		auto _getFirstEnumerator(first.getEnumerator);
		auto _getSecondEnumerator(second.getEnumerator);

		return TEnumerable<T>
		(
			[=]()
			{
				auto state = std::make_shared<State>();
				state->firstEnumerator = (*_getFirstEnumerator)();
				state->secondEnumerator = (*_getSecondEnumerator)();
				state->first = true;

				return std::make_shared<TEnumerator<T>>
				(
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
					}
				);
			}
		);
	}

	template<typename TResult, typename T1, typename T2, typename TSelector>
	static TEnumerable<TResult> Zip(TEnumerable<T1> first, TEnumerable<T2> second, TSelector selector)
	{
		struct State
		{
			State() { }
			std::shared_ptr<TEnumerator<T1>> firstEnumerator;
			std::shared_ptr<TEnumerator<T2>> secondEnumerator;
		};

		auto _getFirstEnumerator(first.getEnumerator);
		auto _getSecondEnumerator(second.getEnumerator);

		return TEnumerable<TResult>
		(
			[=]()
			{
				auto state = std::make_shared<State>();
				state->firstEnumerator = (*_getFirstEnumerator)();
				state->secondEnumerator = (*_getSecondEnumerator)();

				return std::make_shared<TEnumerator<TResult>>
				(
					[=]()
					{
						return state->firstEnumerator->MoveNext() && state->secondEnumerator->MoveNext();
					},
					[=]()
					{
						return selector(state->firstEnumerator->Current(), state->secondEnumerator->Current());
					}
				);
			}
		);
	}
};

}