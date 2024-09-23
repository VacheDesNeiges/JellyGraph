#pragma once

#include <compare>
#include <concepts>
#include <cstddef>
#include <functional>
#include <vector>

namespace jGraph::internals
{

template <typename T>
concept Integral = std::is_integral_v<T>;

template <typename T>
concept IsWeightedGraph =
    requires(T graph, const std::vector<double> &weights) {
        { graph.setWeight(weights) };
    };

template <typename T>
concept ValidKeyType = std::copyable<T> && requires(T keyType) {
    { std::hash<T>{}(keyType) } -> std::convertible_to<std::size_t>;
    { keyType <=> keyType } -> std::convertible_to<std::partial_ordering>;
};
} // namespace jGraph::internals
