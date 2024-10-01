#pragma once

#include <concepts>
#include <cstddef>
#include <cstdio>
#include <functional>
#include <type_traits>

namespace jGraph::internals
{

template <typename T>
concept Integral = std::is_integral_v<T>;

template <typename T>
concept ValidKeyType =
    std::copyable<T> && std::totally_ordered<T> && requires(T keyType) {
        { std::hash<T>{}(keyType) } -> std::convertible_to<std::size_t>;
    };

} // namespace jGraph::internals
