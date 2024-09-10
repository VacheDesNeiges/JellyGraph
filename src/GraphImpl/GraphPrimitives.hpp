#pragma once

#include "NameIndexMap.hpp"
#include <utility>
#include <vector>

class GraphPrimitives
{
  public:
    GraphPrimitives() = default;
    virtual ~GraphPrimitives() = default;

    GraphPrimitives(const GraphPrimitives &) = delete;
    GraphPrimitives(GraphPrimitives &&) = delete;
    GraphPrimitives &operator=(const GraphPrimitives &) = delete;
    GraphPrimitives &operator=(GraphPrimitives &&) = delete;

    virtual void addNode(unsigned) = 0;
    virtual void removeNode(unsigned) = 0;
    virtual void addEdge(std::pair<unsigned, unsigned>) = 0;
    virtual void removeEdge(std::pair<unsigned, unsigned>) = 0;

    virtual unsigned getNumberOfNodes() const = 0;
    virtual unsigned getNumberOfEdges() const = 0;

    virtual std::vector<unsigned> getNodes() const = 0;
    virtual std::vector<std::pair<unsigned, unsigned>> getEdges() const = 0;
    virtual std::vector<unsigned> getNeighbors(unsigned) const = 0;

    virtual bool hasEdge(std::pair<unsigned, unsigned>) const = 0;

  protected:
    jGraph::internals::NameIndexMap<unsigned> &getNodeMap();
    const jGraph::internals::NameIndexMap<unsigned> &getNodeMap() const;
    virtual std::vector<unsigned> internal_getNodes() const = 0;
    virtual std::vector<unsigned> internal_getNeighbors(unsigned) const = 0;

  private:
    jGraph::internals::NameIndexMap<unsigned> nodeMap;
};
