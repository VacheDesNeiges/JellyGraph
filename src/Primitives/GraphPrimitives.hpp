#pragma once

#include "NameIndexMap.hpp"
#include "UnderlyingIndexType.hpp"

#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

namespace jGraph
{

template <typename T>
concept Integral = std::is_integral_v<T>;

template <typename T, typename IndexType = internals::underlyingGraphIndex_t>
    requires Integral<IndexType>
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
    [[nodiscard]] virtual bool isDirected() const = 0;

    virtual void addNode(T) = 0;
    virtual void addNode(std::initializer_list<T> nodes);
    virtual void addNode(std::span<T> nodes) = 0;

    virtual void removeNode(T) = 0;

    virtual void addEdge(std::pair<T, T>) = 0;
    virtual void addEdge(std::initializer_list<std::pair<T, T>> edges);
    virtual void addEdge(std::span<std::pair<T, T>> edges) = 0;

    template <std::ranges::range R>
        requires std::convertible_to<std::ranges::range_value_t<R>, T>
    void addEdge(const R &edges)
    {
        for (const auto &edge : edges)
        {
            addEdge(edge);
        }
    }

    virtual void removeEdge(std::pair<T, T>) = 0;

    [[nodiscard]] virtual size_t getNumberOfNodes() const = 0;
    [[nodiscard]] virtual size_t getNumberOfEdges() const = 0;

    [[nodiscard]] virtual std::vector<T> getNodes() const = 0;
    [[nodiscard]] virtual std::vector<std::pair<T, T>> getEdges() const = 0;
    [[nodiscard]] virtual std::vector<T> getNeighbors(T) const = 0;

    [[nodiscard]] virtual bool hasEdge(std::pair<T, T>) const = 0;

  protected:
    [[nodiscard]] jGraph::internals::NameIndexMap<T, IndexType> &getNodeMap();
    [[nodiscard]] const jGraph::internals::NameIndexMap<T, IndexType> &
    getNodeMap() const;
    [[nodiscard]] virtual std::vector<IndexType> internal_getNodes() const = 0;
    [[nodiscard]] virtual std::vector<IndexType> internal_getNeighbors(
        IndexType) const = 0;

  private:
    jGraph::internals::NameIndexMap<T, IndexType> nodeMap;
};

template <typename T, typename IndexType>
    requires Integral<IndexType>
jGraph::internals::NameIndexMap<T, IndexType> &GraphPrimitives<
    T, IndexType>::getNodeMap()
{
    return nodeMap;
}

template <typename T, typename IndexType>
    requires Integral<IndexType>
const jGraph::internals::NameIndexMap<T, IndexType> &GraphPrimitives<
    T, IndexType>::getNodeMap() const
{
    return nodeMap;
}

template <typename T, typename IndexType>
    requires Integral<IndexType>
void GraphPrimitives<T, IndexType>::addNode(std::initializer_list<T> nodes)
{
    for (const auto &node : nodes)
    {
        addNode(node);
    }
}

template <typename T, typename IndexType>
    requires Integral<IndexType>
void GraphPrimitives<T, IndexType>::addEdge(
    std::initializer_list<std::pair<T, T>> edges)
{
    for (const auto &edge : edges)
    {
        addEdge(edge);
    }
}

} // namespace jGraph
