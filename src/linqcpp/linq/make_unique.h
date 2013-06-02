#pragma once

#include <utility>
#include <memory>

template<typename T, typename U>
std::unique_ptr<T> make_unique(U&& u)
{
    return std::unique_ptr<T>(new T(std::forward<U>(u)));
}