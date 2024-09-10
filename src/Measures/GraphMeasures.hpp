#pragma once
#include "GraphPrimitives.hpp"

class GraphMeasures : public virtual GraphPrimitives
{
  public:
    float averageNeighborDegree() const;
};
