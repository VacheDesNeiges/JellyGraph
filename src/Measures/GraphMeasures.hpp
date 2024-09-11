#pragma once
#include "GraphPrimitives.hpp"

namespace jGraph
{

template <typename T>
class GraphMeasures : public virtual GraphPrimitives<T>
{
  public:
    unsigned degree(T node) const;
    float averageNeighborDegree() const;

    float density() const;
};

template <typename T>
unsigned GraphMeasures<T>::degree(T node) const
{
    return static_cast<unsigned>(this->getNeighbors(node).size());
}

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

template <typename T>
float GraphMeasures<T>::density() const
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
