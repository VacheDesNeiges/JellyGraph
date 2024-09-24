#pragma once

#include "DefaultTypes.hpp"
#include <concepts>
#include <span>
#include <tuple>
#include <utility>
#include <vector>

namespace jGraph
{

template <typename T, typename IndexType = internals::underlyingGraphIndex_t,
          typename WeightType = internals::underlyingGraphWeight_t>

    requires std::totally_ordered<WeightType>

class WeightedGraphPrimitives
{
  public:
    virtual void addEdge(std::pair<T, T> edge, WeightType weight) = 0;
    virtual void addEdge(std::tuple<T, T, WeightType> edge) = 0;
    virtual void addEdge(std::span<std::tuple<T, T, WeightType>> edges) = 0;

    virtual void setWeight(std::pair<T, T> edge) = 0;
    virtual void setWeight(std::span<std::pair<T, T>> edges) = 0;

    virtual WeightType getWeight(std::pair<T, T> edge) = 0;
    virtual std::vector<WeightType> getWeight(
        std::span<std::pair<T, T>> edges) = 0;
};

} // namespace jGraph
