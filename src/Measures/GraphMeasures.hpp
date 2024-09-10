#pragma once
#include "GraphPrimitives.hpp"

namespace jGraph
{

template <typename T>

class GraphMeasures : public virtual GraphPrimitives<T>
{
  public:
    float averageNeighborDegree() const;
};

template <typename T>

float GraphMeasures<T>::averageNeighborDegree() const
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

} // namespace jGraph
