#pragma once
#include "GraphPrimitives.hpp"
#include <vector>

class GraphAlgorithms : public virtual GraphPrimitives
{
  public:
    bool isConnected() const;
    unsigned numberOfComponents() const;
    std::vector<std::vector<unsigned>> components() const;

  private:
    std::vector<std::vector<unsigned>> internal_components() const;
};
