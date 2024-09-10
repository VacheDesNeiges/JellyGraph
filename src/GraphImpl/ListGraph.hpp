#pragma once
#include "GraphAlgorithms.hpp"
#include "GraphMeasures.hpp"
#include "GraphPrimitives.hpp"
#include "NameIndexMap.hpp"
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
    jGraph::internals::NameIndexMap<unsigned> nameIndexMap;
    unsigned edgeNumber = 0;
    std::vector<std::vector<unsigned>> nodes;

    std::vector<unsigned> getNodesAsIndexes() const override;
    std::vector<unsigned> getNeighborsAsIndexes(unsigned index) const override;
};

} // namespace jGraph
