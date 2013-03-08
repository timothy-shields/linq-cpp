#pragma once

#include "dllapi.h"

#include <memory>
#include <functional>
#include <vector>
#include <set>
#include <map>
#include <string>

#include "Comparer.h"
#include "Functional.h"
#include "PairingHeap.h"

template<typename T>
class PairingHeap;

template<typename T>
class Enumerator
{
private:
	const std::function<bool()> moveNext;
	const std::function<T()> current;

	Enumerator(std::function<bool()> moveNext)
		: moveNext(moveNext)
	{
	}

public:
	Enumerator(std::function<bool()> moveNext, std::function<T()> current)
		: moveNext(moveNext)
		, current(current)
	{
	}

	bool MoveNext()
	{
		return moveNext();
	}

	T Current()
	{
		return current();
	}

	static std::shared_ptr<Enumerator<T>> Empty()
	{
		return std::shared_ptr<Enumerator<T>>(new Enumerator<T>([](){ return false; }));
	}
};

template<typename T>
class Enumerable
{
private:
	std::shared_ptr<const std::function<std::shared_ptr<Enumerator<T>>()>> getEnumerator;

public:
	std::shared_ptr<Enumerator<T>> GetEnumerator()
	{
		return (*getEnumerator)();
	}

	Enumerable()
	{
		getEnumerator = std::make_shared<const std::function<std::shared_ptr<Enumerator<T>>()>>([](){ return Enumerator<T>::Empty(); });
	}

	Enumerable(std::function<std::shared_ptr<Enumerator<T>>()> getEnumerator)
		: getEnumerator(std::make_shared<const std::function<std::shared_ptr<Enumerator<T>>()>>(getEnumerator))
	{
	}
	
	//Anamorphism
	static Enumerable<T> Generate(T start, std::function<bool (T)> condition, std::function<T (T)> next)
	{
		struct State
		{
			bool first;
			T value;
			State() { }
		};

		return Enumerable<T>
		(
			[=]()
			{
				auto state = std::make_shared<State>();
				state->first = true;

				return std::make_shared<Enumerator<T>>
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
						return condition(state->value);
					},
					[=]()
					{
						return state->value;
					}
				);
			}
		);
	}

	//Bind
	template<typename TResult>
	Enumerable<TResult> SelectMany(std::function<Enumerable<TResult> (T)> selector)
	{
		struct SelectManyState
		{
			std::shared_ptr<Enumerator<T>> outerEnumerator;
			std::shared_ptr<Enumerator<TResult>> innerEnumerator;
			TResult value;
			SelectManyState() { }
		};

		auto _getEnumerator(getEnumerator);

		return Enumerable<TResult>
		(
			[=]()
			{
				auto state = std::make_shared<SelectManyState>();
				state->outerEnumerator = (*_getEnumerator)();

				return std::make_shared<Enumerator<TResult>>
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
					});
			});
	}

	//Catamorphism
	template<typename TAccumulate>
	TAccumulate Aggregate(TAccumulate seed, std::function<TAccumulate (TAccumulate, T)> accumulator)
	{
		auto enumerator = GetEnumerator();
		TAccumulate agg(seed);
		while (enumerator->MoveNext())
		{
			agg = accumulator(agg, enumerator->Current());
		}
		return agg;
	}

	static Enumerable<T> Empty()
	{
		return Enumerable<T>([](){ return Enumerator<T>::Empty(); });
	}

	static Enumerable<T> Return(T item)
	{
		struct State
		{
			bool first;
		};

		return Enumerable<T>
		(
			[=]()
			{
				auto state = std::make_shared<State>();
				state->first = true;

				return std::make_shared<Enumerator<T>>
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

	static Enumerable<T> Generate(T start, std::function<T (T)> next)
	{
		return Generate(start, [](T _){ return true; }, next);
	}

	static Enumerable<T> Range(T start, T count)
	{
		return Generate(start, [](T value){ return value + 1; }).Take(count);
	}

	static Enumerable<T> From(T start) {
		return Generate(start, [](T value){ return value + 1; });
	}

	Enumerable<T> ToInclusive(T end) {
		return TakeWhile([=](T item){ return item <= end; });
	}

	Enumerable<T> ToExclusive(T end) {
		return TakeWhile([=](T item){ return item < end; });
	}

	template<typename TResult>
	Enumerable<TResult> Select(std::function<TResult(T)> selector) {

		struct State {
			std::shared_ptr<Enumerator<T>> enumerator;
			TResult value;
		};

		auto _getEnumerator(getEnumerator);
		return Enumerable<TResult>(
			[=]()->std::shared_ptr<Enumerator<TResult>>{
				auto state = std::make_shared<State>();
				state->enumerator = (*_getEnumerator)();
				return std::make_shared<Enumerator<TResult>>(
					[=]()->bool{
						if (state->enumerator->MoveNext()) {
							state->value = selector(state->enumerator->Current());
							return true;
						} else {
							return false;
						}
					},
					[=]()->TResult{
						return state->value;
					});
			});
	}

	template<typename TResult>
	Enumerable<TResult> SelectIndexed(std::function<TResult(T, int)> selector) {

		struct State {
			std::shared_ptr<Enumerator<T>> enumerator;
			TResult value;
			int i;
		};

		auto _getEnumerator(getEnumerator);
		return Enumerable<TResult>(
			[=]()->std::shared_ptr<Enumerator<TResult>>{
				auto state = std::make_shared<State>();
				state->enumerator = (*_getEnumerator)();
				state->i = 0;
				return std::make_shared<Enumerator<TResult>>(
					[=]()->bool{
						if (state->enumerator->MoveNext()) {
							state->value = selector(state->enumerator->Current(), state->i);
							++(state->i);
							return true;
						} else {
							return false;
						}
					},
					[=]()->TResult{
						return state->value;
					});
			});
	}

	Enumerable<T> Where(std::function<bool(T)> predicate) {

		struct State {
			std::shared_ptr<Enumerator<T>> enumerator;
		};

		auto _getEnumerator(getEnumerator);
		return Enumerable<T>(
			[=]()->std::shared_ptr<Enumerator<T>>{
				auto state = std::make_shared<State>();
				state->enumerator = (*_getEnumerator)();
				return std::make_shared<Enumerator<T>>(
					[=]()->bool{
						while (true) {
							if (!state->enumerator->MoveNext())
								return false;
							if (predicate(state->enumerator->Current()))
								return true;
						}
					},
					[=]()->T{
						return state->enumerator->Current();
					});
			});
	}

	Enumerable<T> WhereIndexed(std::function<bool(T, int)> predicate) {

		struct State {
			std::shared_ptr<Enumerator<T>> enumerator;
			int i;
		};

		auto _getEnumerator(getEnumerator);
		return Enumerable<T>(
			[=]()->std::shared_ptr<Enumerator<T>>{
				auto state = std::make_shared<State>();
				state->enumerator = (*_getEnumerator)();
				state->i = 0;
				return std::make_shared<Enumerator<T>>(
					[=]()->bool{
						while (true) {
							if (!state->enumerator->MoveNext())
								return false;
							auto index = state->i;
							++(state->i);
							if (predicate(state->enumerator->Current(), index))
								return true;
						}
					},
					[=]()->T{
						return state->enumerator->Current();
					});
			});
	}

	Enumerable<T> Skip(int count) {
		auto _getEnumerator(getEnumerator);
		return Enumerable<T>(
			[=]()->std::shared_ptr<Enumerator<T>>{
				auto enumerator = (*_getEnumerator)();
				auto n = std::make_shared<int>(0);
				return std::make_shared<Enumerator<T>>(
					[=]()->bool{
						while (*n < count) {
							++*n;
							if (!enumerator->MoveNext())
								return false;
						}
						return enumerator->MoveNext();
					},
					[=]()->T{
						return enumerator->Current();
					});
			});
	}

	Enumerable<T> SkipWhile(std::function<bool(T)> predicate) {
		auto _getEnumerator(getEnumerator);
		return Enumerable<T>(
			[=]()->std::shared_ptr<Enumerator<T>>{
				auto enumerator = (*_getEnumerator)();
				auto skipping = std::make_shared<bool>(true);
				return std::make_shared<Enumerator<T>>(
					[=]()->bool{
						if (*skipping) {
							while (true) {
								if (!enumerator->MoveNext())
									return false;
								if (!predicate(enumerator->Current())) {
									*skipping = false;
									return true;
								}
							}
						} else {
							return enumerator->MoveNext();
						}
					},
					[=]()->T{
						return enumerator->Current();
					});
			});
	}

	Enumerable<T> Take(int count)
	{
		auto _getEnumerator(getEnumerator);
		return Enumerable<T>
		(
			[=]()
			{
				auto enumerator = (*_getEnumerator)();
				auto n = std::make_shared<int>(0);
				return std::make_shared<Enumerator<T>>
				(
					[=]()
					{
						if (*n < count)
						{
							++*n;
							return enumerator->MoveNext();
						}
						return false;
					},
					[=]()
					{
						return enumerator->Current();
					}
				);
			}
		);
	}

	Enumerable<T> TakeWhile(std::function<bool(T)> predicate) {
		auto _getEnumerator(getEnumerator);
		return Enumerable<T>(
			[=]()->std::shared_ptr<Enumerator<T>>{
				auto enumerator = (*_getEnumerator)();
				return std::make_shared<Enumerator<T>>(
					[=]()->bool{
						return enumerator->MoveNext() && predicate(enumerator->Current());
					},
					[=]()->T{
						return enumerator->Current();
					});
			});
	}

	Enumerable<T> Order(std::function<int (T, T)> comparer) {

		struct State {
			PairingHeap<T> heap;
			T value;
		};

		auto _getEnumerator(getEnumerator);
		return Enumerable<T>(
			[=]()->std::shared_ptr<Enumerator<T>>{
				auto enumerator = (*_getEnumerator)();
				auto state = std::make_shared<State>();
				while (enumerator->MoveNext())
					state->heap.Insert(enumerator->Current());
				return std::make_shared<Enumerator<T>>(
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

	Enumerable<T> Order()
	{
		return Order(Comparer::Default<T>());
	}

	template<typename TKey>
	Enumerable<T> OrderByKey(std::function<TKey (T)> keySelector) {
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

	Enumerable<T> Concat(Enumerable<T> other) {
		return Concat(*this, other);
	}

	static Enumerable<T> Concat(Enumerable<T> first, Enumerable<T> second) {
		auto _getFirstEnumerator(first.getEnumerator); //We don't want to capture "first"!
		auto _getSecondEnumerator(second.getEnumerator); //We don't want to capture "second"!
		return Enumerable<T>(
			[=]()->std::shared_ptr<Enumerator<T>>{
				auto firstEnumerator = (*_getFirstEnumerator)();
				auto secondEnumerator = (*_getSecondEnumerator)();
				auto first = std::make_shared<bool>(true);
				return std::make_shared<Enumerator<T>>(
					[=]()->bool{
						if (*first) {
							if (firstEnumerator->MoveNext())
								return true;
							else
								*first = false;
						}
						return secondEnumerator->MoveNext();
					},
					[=]()->T{
						if (*first)
							return firstEnumerator->Current();
						else
							return secondEnumerator->Current();
					});
			});
	}

	template<typename TResult>
	Enumerable<TResult> StaticCast() {
		return Select<TResult>([](T item){
			return static_cast<TResult>(item);
		});
	}

	template<typename TResult>
	Enumerable<TResult> DynamicCast() {
		return Select<TResult>([](T item){
			return dynamic_cast<TResult>(item);
		});
	}

	bool Any() {
		auto enumerator = GetEnumerator();
		return enumerator->MoveNext();
	}

	bool Any(std::function<bool(T)> predicate) {
		auto enumerator = GetEnumerator();
		while (enumerator->MoveNext())
			if (predicate(enumerator->Current()))
				return true;
		return false;
	}

	bool All(std::function<bool(T)> predicate) {
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
			throw runtime_error("Enumerable<T>::First failed because the enumerable is empty.");
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
			throw runtime_error("Enumerable<T>::First failed because the enumerable is empty.");
	}

	T Last(std::function<bool(T)> predicate) {
		return Where(predicate).Last();
	}

	T Single() {
		auto enumerator = GetEnumerator();
		if (enumerator->MoveNext()) {
			T item = enumerator->Current();
			if (enumerator->MoveNext())
				throw runtime_error("Enumerable<T>::Single failed because the enumerable contains more than one item.");
			return item;
		}
		else {
			throw runtime_error("Enumerable<T>::Single failed because the enumerable is empty.");
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

	T Aggregate(std::function<T (T, T)> accumulator) {
		auto enumerator = GetEnumerator();
		if (!enumerator->MoveNext())
			throw runtime_error("Enumerable<T>::Aggregate failed because the enumerable is empty.");
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
			throw runtime_error("Enumerable<T>::Average failed because the enumerable is empty.");
		return sum / count;
	}

	template<typename TScore>
	T MinBy(std::function<TScore(T)> score) {
		auto enumerator = GetEnumerator();
		if (!enumerator->MoveNext())
			throw runtime_error("Enumerable<T>::MinBy failed because the enumerable is empty.");
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
			throw runtime_error("Enumerable<T>::MaxBy failed because the enumerable is empty.");
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
			throw runtime_error("Enumerable<T>::Indexmin failed because the enumerable is empty.");
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
			throw runtime_error("Enumerable<T>::Indexmax failed because the enumerable is empty.");
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

	template<typename TRange>
	static Enumerable<T> FromRange(TRange& range) {
		return FromRange(range.begin(), range.end());
	}

	template<typename TIter>
	static Enumerable<T> FromRange(TIter begin, TIter end) {
		return Enumerable<T>(
			[=]()->std::shared_ptr<Enumerator<T>>{
				auto iter = std::make_shared<TIter>(begin);
				auto first = std::make_shared<bool>(true);
				return std::make_shared<Enumerator<T>>(
					[=]()->bool{
						if (!(*first))
							++*iter;
						*first = false;
						return (*iter != end);
					},
					[=]()->T{
						return **iter;
					});
			});
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