#pragma once

#include "DefaultTypes.hpp"
#include "GraphAlgorithms.hpp"
#include "GraphMeasures.hpp"
#include "GraphPrimitives.hpp"
#include "GraphSerialization.hpp"
#include "WeightedGraphPrimitives.hpp"
#include <cstddef>
#include <span>
#include <utility>
#include <vector>
namespace jGraph
{

template <typename T, typename IndexType = internals::underlyingGraphIndex_t,
          typename WeightType = internals::underlyingGraphWeight_t>

class WeightedListGraph
    : public WeightedGraphPrimitives<T, IndexType, WeightType>,
      public GraphMeasures<T, IndexType>,
      public GraphAlgorithms<T, IndexType>,
      public GraphSerialization<T, IndexType>,
      public virtual GraphPrimitives<T, IndexType>
{
  public:
    constexpr WeightedListGraph() = default;

    constexpr void clear() override;
    [[nodiscard]] constexpr bool isDirected() const override;
    [[nodiscard]] constexpr bool isWeighted() const override;

    constexpr void addNode(T nodeName) override;
    constexpr void addNode(std::span<T> newNodes) override;

  private:
    size_t edgeNumber = 0;
    std::vector<std::vector<std::pair<IndexType, WeightedListGraph>>>
        adjacencyList;
};

template <typename T, typename IndexType, typename WeightType>
constexpr void WeightedListGraph<T, IndexType, WeightType>::clear()
{
    adjacencyList.clear();
    edgeNumber = 0;
}

template <typename T, typename IndexType, typename WeightType>
constexpr bool WeightedListGraph<T, IndexType, WeightType>::isWeighted() const
{
    return true;
}

template <typename T, typename IndexType, typename WeightType>
constexpr bool WeightedListGraph<T, IndexType, WeightType>::isDirected() const
{
    return false;
}

template <typename T, typename IndexType, typename WeightType>
constexpr void WeightedListGraph<T, IndexType, WeightType>::addNode(T nodeName)
{
    if (this->getNodeMap().addByName(nodeName))
        adjacencyList.emplace_back();
}

template <typename T, typename IndexType, typename WeightType>
constexpr void WeightedListGraph<T, IndexType, WeightType>::addNode(
    std::span<T> newNodes)
{
    std::vector<T> nodesToAdd;
    nodesToAdd.reserve(newNodes.size());

    for (const auto &node : newNodes)
    {
        if (this->getNodeMap().addByName(node))
            adjacencyList.emplace_back();
    }

    const auto newSize = adjacencyList.size() + nodesToAdd.size();
    adjacencyList.resize(newSize);
}

} // namespace jGraph
