#pragma once
#include "GraphPrimitives.hpp"

namespace jGraph
{

class GraphMeasures : public virtual GraphPrimitives
{
  public:
    float averageNeighborDegree() const;
};

} // namespace jGraph
