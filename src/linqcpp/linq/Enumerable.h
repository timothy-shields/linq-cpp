#pragma once

#include <utility>
#include <memory>

#include "enumerator.h"

template<class T, class U>
std::unique_ptr<T> make_unique(U&& u)
{
    return std::unique_ptr<T>(new T(std::forward<U>(u)));
}

template <typename T>
class enumerable
{
public:
	typedef T value_type;
	virtual std::unique_ptr<enumerator<T>> get_enumerator_ptr() = 0;
};