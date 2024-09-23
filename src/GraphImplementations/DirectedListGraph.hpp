#pragma once

#include "DirectedGraphPrimitives.hpp"
#include "ListGraph.hpp"
#include "UnderlyingIndexType.hpp"

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <span>
#include <unordered_set>
#include <utility>
#include <vector>

namespace jGraph
{

template <typename T, typename IndexType = internals::underlyingGraphIndex_t>
class DirectedListGraph : public ListGraph<T, IndexType>,
                          public DirectedGraphPrimitives<T, IndexType>
{
  public:
    constexpr DirectedListGraph() = default;
    constexpr DirectedListGraph(std::initializer_list<T> newNodes);

    [[nodiscard]] constexpr bool isDirected() const override;

    constexpr void addEdge(std::pair<T, T> edge) override;
    constexpr void addEdge(std::span<std::pair<T, T>> edges) override;

    constexpr void removeEdge(std::pair<T, T> edge) override;

    [[nodiscard]] constexpr std::vector<std::pair<T, T>> getEdges()
        const override;

    [[nodiscard]] constexpr std::vector<T> getNeighbors(T key) const override;
    [[nodiscard]] constexpr std::vector<T> getOutgoingNeighbors(
        T key) const override;
    [[nodiscard]] constexpr std::vector<T> getIngoingNeighbors(
        T key) const override;

  private:
    [[nodiscard]] constexpr std::vector<IndexType> internal_getNeighbors(
        IndexType index) const override;
    [[nodiscard]] constexpr std::vector<IndexType>
    internal_getOutgoingNeighbors(IndexType index) const override;
    [[nodiscard]] constexpr std::vector<IndexType> internal_getIngoingNeighbors(
        IndexType index) const override;
};

template <typename T, typename IndexType>
constexpr DirectedListGraph<T, IndexType>::DirectedListGraph(
    std::initializer_list<T> newNodes)
{
    for (const auto &node : newNodes)
    {
        if (this->getNodeMap().addByName(node))
            this->getAdjacencyList().emplace_back();
    }
}

template <typename T, typename IndexType>
constexpr bool DirectedListGraph<T, IndexType>::isDirected() const
{
    return true;
}

template <typename T, typename IndexType>
constexpr void DirectedListGraph<T, IndexType>::addEdge(std::pair<T, T> edge)
{
    const auto [first, second] = edge;
    if (!this->getNodeMap().contains(first))
        this->addNode(first);

    if (!this->getNodeMap().contains(second))
        this->addNode(second);

    const auto firstIndex = this->getNodeMap().convertNodeNameToIndex(first);
    const auto secondIndex = this->getNodeMap().convertNodeNameToIndex(second);

    if (std::ranges::find(
            this->getAdjacencyList().at(static_cast<size_t>(firstIndex)),
            secondIndex) ==
        this->getAdjacencyList().at(static_cast<size_t>(firstIndex)).end())
    {
        this->getEdgeNumber()++;
        this->getAdjacencyList()
            .at(static_cast<size_t>(firstIndex))
            .push_back(secondIndex);
    }
}

template <typename T, typename IndexType>
constexpr void DirectedListGraph<T, IndexType>::addEdge(
    std::span<std::pair<T, T>> edges)
{
    for (const std::pair<T, T> &edge : edges)
    {
        for (const T &node : {edge.first, edge.second})
        {
            if (!this->getNodeMap().contains(node))
                this->addNode(node);
        }
        const auto firstIndex =
            this->getNodeMap().convertNodeNameToIndex(edge.first);
        const auto secondIndex =
            this->getNodeMap().convertNodeNameToIndex(edge.second);

        if (std::ranges::find(
                this->getAdjacencyList().at(static_cast<size_t>(firstIndex)),
                secondIndex) ==
            this->getAdjacencyList().at(static_cast<size_t>(firstIndex)).end())
        {
            this->getEdgeNumber()++;
            this->getAdjacencyList()
                .at(static_cast<size_t>(firstIndex))
                .push_back(secondIndex);
        }
    }
}

template <typename T, typename IndexType>
constexpr void DirectedListGraph<T, IndexType>::removeEdge(std::pair<T, T> edge)
{
    const auto first = static_cast<size_t>(
        this->getNodeMap().convertNodeNameToIndex(edge.first));
    const auto second = static_cast<size_t>(
        this->getNodeMap().convertNodeNameToIndex(edge.second));

    auto &edgesOfFirst = this->getAdjacencyList().at(first);
    bool edgeRemoved = (std::erase(edgesOfFirst, second) == 1);
    if (edgeRemoved)
        this->getEdgeNumber()--;
}

template <typename T, typename IndexType>
constexpr std::vector<std::pair<T, T>> DirectedListGraph<
    T, IndexType>::getEdges() const
{
    std::vector<std::pair<T, T>> result;
    result.reserve(this->getNumberOfEdges());

    const auto numNodes = this->getNumberOfNodes();
    for (size_t i = 0; i < numNodes; i++)
    {
        for (const auto &neighbor : this->getAdjacencyList().at(i))
        {
            const auto firstIndex = this->getNodeMap().convertIndexToNodeName(
                static_cast<IndexType>(i));
            const auto secondIndex =
                this->getNodeMap().convertIndexToNodeName(neighbor);

            result.emplace_back(firstIndex, secondIndex);
        }
    }
    return result;
}

template <typename T, typename IndexType>
constexpr std::vector<T> DirectedListGraph<T, IndexType>::getNeighbors(
    T key) const
{
    return this->getNodeMap().convertIndexToNodeName(
        internal_getNeighbors(this->getNodeMap().convertNodeNameToIndex(key)));
}

template <typename T, typename IndexType>
constexpr std::vector<IndexType> DirectedListGraph<
    T, IndexType>::internal_getNeighbors(IndexType index) const
{
    std::unordered_set<IndexType> resultSet;

    auto nodes = this->getAdjacencyList();
    for (size_t i = 0; i < nodes.size(); i++)
    {
        if (static_cast<IndexType>(i) == index)
            continue;

        if (std::ranges::find(nodes.at(i), index) != nodes.at(i).end())
            resultSet.emplace(static_cast<IndexType>(i));
    }

    for (const auto outgoingNeighbor : nodes.at(static_cast<size_t>(index)))
    {
        resultSet.emplace(outgoingNeighbor);
    }

    std::vector<IndexType> result(resultSet.begin(), resultSet.end());
    return result;
}

template <typename T, typename IndexType>
constexpr std::vector<T> DirectedListGraph<T, IndexType>::getOutgoingNeighbors(
    T key) const
{
    return this->getNodeMap().convertIndexToNodeName(
        internal_getOutgoingNeighbors(
            this->getNodeMap().convertNodeNameToIndex(key)));
}

template <typename T, typename IndexType>
constexpr std::vector<IndexType> DirectedListGraph<
    T, IndexType>::internal_getOutgoingNeighbors(IndexType index) const
{
    return this->getAdjacencyList().at(static_cast<size_t>(index));
}

template <typename T, typename IndexType>
constexpr std::vector<T> DirectedListGraph<T, IndexType>::getIngoingNeighbors(
    T key) const
{
    return this->getNodeMap().convertIndexToNodeName(
        internal_getIngoingNeighbors(
            this->getNodeMap().convertNodeNameToIndex(key)));
}

template <typename T, typename IndexType>
constexpr std::vector<IndexType> DirectedListGraph<
    T, IndexType>::internal_getIngoingNeighbors(IndexType index) const
{
    std::vector<IndexType> result;
    result.reserve(this->getNumberOfNodes());

    auto nodes = this->getAdjacencyList();
    for (size_t i = 0; i < nodes.size(); i++)
    {
        if (static_cast<IndexType>(i) == index)
            continue;

        if (std::ranges::find(nodes.at(i), index) != nodes.at(i).end())
            result.emplace_back(static_cast<IndexType>(i));
    }
    result.shrink_to_fit();
    return result;
}

} // namespace jGraph
