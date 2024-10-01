#pragma once

#include "DefaultTypes.hpp"
#include "GraphPrimitives.hpp"
#include <expected>
#include <optional>
#include <span>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace jGraph
{

template <typename T, typename IndexType = internals::underlyingGraphIndex_t,
          typename WeightType = internals::underlyingGraphWeight_t>

    requires std::is_arithmetic_v<WeightType>

class WeightedGraphPrimitives : public virtual GraphPrimitives<T, IndexType>
{
  public:
    constexpr virtual void addWeightedEdge(std::pair<T, T> edge,
                                           WeightType weight) = 0;

    constexpr virtual void addWeightedEdge(std::span<std::pair<T, T>>,
                                           WeightType) = 0;

    constexpr virtual void addWeightedEdge(std::span<std::pair<T, T>> edges,
                                           std::span<WeightType> weights) = 0;

    constexpr virtual void addWeightedEdge(
        std::tuple<T, T, WeightType> edge) = 0;

    constexpr virtual void addWeightedEdge(
        std::span<std::tuple<T, T, WeightType>> edges) = 0;

    constexpr virtual void setWeight(std::pair<T, T> edge,
                                     WeightType weight) = 0;

    [[nodiscard]] constexpr virtual std::optional<WeightType> getWeight(
        std::pair<T, T> edge) const = 0;

    [[nodiscard]] constexpr virtual std::vector<std::optional<WeightType>>
    getWeight(std::span<std::pair<T, T>> edges) const = 0;
};

} // namespace jGraph
