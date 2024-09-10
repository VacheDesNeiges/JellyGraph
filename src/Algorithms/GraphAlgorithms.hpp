#pragma once

#include "GraphPrimitives.hpp"
class GraphAlgorithms : public virtual GraphPrimitives
{
  public:
    bool isConnected() const;
    unsigned numberOfComponents() const;
    void components() const;
    void components(unsigned) const;
};
