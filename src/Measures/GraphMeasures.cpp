#include "GraphMeasures.hpp"

float GraphMeasures::averageNeighborDegree() const
{
    float result = 0;
    const auto &nodes = getNodes();

    for (const auto &node : nodes)
    {
        result += static_cast<float>(getNeighbors(node).size());
    }

    result /= static_cast<float>(nodes.size());

    return result;
}
