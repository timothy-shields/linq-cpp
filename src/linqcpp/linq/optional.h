#pragma once

#include <memory>

namespace linq {

//Will be replaced with std::optional

template <typename T>
typedef std::unique_ptr<T> optional;

}