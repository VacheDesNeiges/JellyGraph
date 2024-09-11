#pragma once

#include "NameIndexMap.hpp"
#include <utility>
#include <vector>

namespace jGraph
{

template <typename T>
class GraphPrimitives
{
  public:
    GraphPrimitives() = default;
    virtual ~GraphPrimitives() = default;

    GraphPrimitives(const GraphPrimitives &) = delete;
    GraphPrimitives(GraphPrimitives &&) = delete;
    GraphPrimitives &operator=(const GraphPrimitives &) = delete;
    GraphPrimitives &operator=(GraphPrimitives &&) = delete;

    virtual void clear() = 0;
    virtual bool isDirected() const = 0;

    virtual void addNode(T) = 0;
    virtual void removeNode(T) = 0;
    virtual void addEdge(std::pair<T, T>) = 0;
    virtual void removeEdge(std::pair<T, T>) = 0;

    virtual unsigned getNumberOfNodes() const = 0;
    virtual unsigned getNumberOfEdges() const = 0;

    virtual std::vector<T> getNodes() const = 0;
    virtual std::vector<std::pair<T, T>> getEdges() const = 0;
    virtual std::vector<T> getNeighbors(T) const = 0;

    virtual bool hasEdge(std::pair<T, T>) const = 0;

  protected:
    jGraph::internals::NameIndexMap<T> &getNodeMap();
    const jGraph::internals::NameIndexMap<T> &getNodeMap() const;
    virtual std::vector<unsigned> internal_getNodes() const = 0;
    virtual std::vector<unsigned> internal_getNeighbors(unsigned) const = 0;

  private:
    jGraph::internals::NameIndexMap<T> nodeMap;
};

template <typename T>
jGraph::internals::NameIndexMap<T> &GraphPrimitives<T>::getNodeMap()
{
    return nodeMap;
}

template <typename T>
const jGraph::internals::NameIndexMap<T> &GraphPrimitives<T>::getNodeMap() const
{
    return nodeMap;
}

} // namespace jGraph
