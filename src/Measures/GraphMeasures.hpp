#pragma once
#include "DefaultTypes.hpp"
#include "GraphPrimitives.hpp"
#include <cstddef>

namespace jGraph
{

template <typename T, typename IndexType = internals::underlyingGraphIndex_t>
class GraphMeasures : public virtual GraphPrimitives<T, IndexType>
{
  public:
    [[nodiscard]] constexpr size_t degree(T node) const;
    [[nodiscard]] constexpr float averageNeighborDegree() const;
    [[nodiscard]] constexpr float density() const;
};

template <typename T, typename IndexType>
constexpr size_t GraphMeasures<T, IndexType>::degree(T node) const
{
    return this->getNeighbors(node).size();
}

template <typename T, typename IndexType>
constexpr float GraphMeasures<T, IndexType>::averageNeighborDegree() const
{
    float result = 0;
    const auto &nodes = this->getNodes();

    for (const auto &node : nodes)
    {
        result += static_cast<float>(this->getNeighbors(node).size());
    }

    result /= static_cast<float>(nodes.size());

    return result;
}

template <typename T, typename IndexType>
constexpr float GraphMeasures<T, IndexType>::density() const
{
    const auto edgeNumber = static_cast<float>(this->getNumberOfEdges());
    const auto nodeNumber = static_cast<float>(this->getNumberOfNodes());
    if ((edgeNumber == 0) || (nodeNumber <= 1))
        return 0;

    float density = edgeNumber / (nodeNumber * (nodeNumber - 1));

    if (!this->isDirected())
        density *= 2;

    return density;
}

} // namespace jGraph
