#pragma once

#include "Concepts.hpp"
#include "DefaultTypes.hpp"
#include "NameIndexMap.hpp"

#include <cstddef>
#include <initializer_list>
#include <span>
#include <utility>
#include <vector>

namespace jGraph
{

template <typename T, typename IndexType = internals::underlyingGraphIndex_t>
    requires internals::Integral<IndexType>
class GraphPrimitives
{
  public:
    constexpr GraphPrimitives() = default;
    constexpr virtual ~GraphPrimitives() = default;

    GraphPrimitives(const GraphPrimitives &) = delete;
    GraphPrimitives(GraphPrimitives &&) = delete;
    GraphPrimitives &operator=(const GraphPrimitives &) = delete;
    GraphPrimitives &operator=(GraphPrimitives &&) = delete;

    constexpr virtual void clear() = 0;

    [[nodiscard]] constexpr virtual bool isDirected() const = 0;
    [[nodiscard]] constexpr virtual bool isWeighted() const = 0;

    constexpr virtual void addNode(T) = 0;
    constexpr virtual void addNode(std::initializer_list<T> nodes);
    constexpr virtual void addNode(std::span<T> nodes) = 0;

    constexpr virtual void removeNode(T) = 0;

    constexpr virtual void addEdge(std::pair<T, T>) = 0;
    constexpr virtual void addEdge(
        std::initializer_list<std::pair<T, T>> edges);
    constexpr virtual void addEdge(std::span<std::pair<T, T>> edges) = 0;

    constexpr virtual void removeEdge(std::pair<T, T>) = 0;

    [[nodiscard]] constexpr virtual size_t getNumberOfNodes() const = 0;
    [[nodiscard]] constexpr virtual size_t getNumberOfEdges() const = 0;

    [[nodiscard]] constexpr virtual std::vector<T> getNodes() const = 0;
    [[nodiscard]] constexpr virtual std::vector<std::pair<T, T>> getEdges()
        const = 0;
    [[nodiscard]] constexpr virtual std::vector<T> getNeighbors(T) const = 0;

    [[nodiscard]] constexpr virtual bool hasEdge(std::pair<T, T>) const = 0;

  protected:
    [[nodiscard]] constexpr jGraph::internals::NameIndexMap<T, IndexType> &
    getNodeMap();
    [[nodiscard]] constexpr const jGraph::internals::NameIndexMap<T,
                                                                  IndexType> &
    getNodeMap() const;
    [[nodiscard]] constexpr virtual std::vector<IndexType> internal_getNodes()
        const = 0;
    [[nodiscard]] constexpr virtual std::vector<
        IndexType> internal_getNeighbors(IndexType) const = 0;

  private:
    jGraph::internals::NameIndexMap<T, IndexType> nodeMap;
};

template <typename T, typename IndexType>
    requires internals::Integral<IndexType>
constexpr jGraph::internals::NameIndexMap<T, IndexType> &GraphPrimitives<
    T, IndexType>::getNodeMap()
{
    return nodeMap;
}

template <typename T, typename IndexType>
    requires internals::Integral<IndexType>
constexpr const jGraph::internals::NameIndexMap<T, IndexType> &GraphPrimitives<
    T, IndexType>::getNodeMap() const
{
    return nodeMap;
}

template <typename T, typename IndexType>
    requires internals::Integral<IndexType>
constexpr void GraphPrimitives<T, IndexType>::addNode(
    std::initializer_list<T> nodes)
{
    for (const auto &node : nodes)
    {
        addNode(node);
    }
}

template <typename T, typename IndexType>
    requires internals::Integral<IndexType>
constexpr void GraphPrimitives<T, IndexType>::addEdge(
    std::initializer_list<std::pair<T, T>> edges)
{
    for (const auto &edge : edges)
    {
        addEdge(edge);
    }
}

} // namespace jGraph
