#pragma once
#include "GraphAlgorithms.hpp"
#include "GraphMeasures.hpp"
#include "GraphPrimitives.hpp"
#include <utility>
#include <vector>

namespace jGraph
{

class ListGraph : public GraphAlgorithms,
                  public GraphMeasures,
                  public virtual GraphPrimitives
{
  public:
    ListGraph() = default;
    ListGraph(unsigned numNodes);

    void addNode(unsigned nodeName) override;
    void removeNode(unsigned nodeName) override;
    void addEdge(std::pair<unsigned, unsigned> edge) override;
    void removeEdge(std::pair<unsigned, unsigned> edge) override;

    unsigned getNumberOfNodes() const override;
    unsigned getNumberOfEdges() const override;

    std::vector<unsigned> getNodes() const override;
    std::vector<std::pair<unsigned, unsigned>> getEdges() const override;
    std::vector<unsigned> getNeighbors(unsigned key) const override;

    bool hasEdge(std::pair<unsigned, unsigned> edge) const override;

  private:
    unsigned edgeNumber = 0;
    std::vector<std::vector<unsigned>> nodes;

    std::vector<unsigned> internal_getNodes() const override;
    std::vector<unsigned> internal_getNeighbors(unsigned index) const override;
};

} // namespace jGraph
