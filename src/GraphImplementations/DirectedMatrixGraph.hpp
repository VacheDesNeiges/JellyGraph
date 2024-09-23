#pragma once

#include "DirectedGraphPrimitives.hpp"
#include "MatrixGraph.hpp"
#include "UnderlyingIndexType.hpp"

#include <concepts>
#include <cstddef>
#include <initializer_list>
#include <ranges>
#include <span>
#include <utility>
#include <vector>

namespace jGraph
{

template <typename T, typename IndexType = internals::underlyingGraphIndex_t>
class DirectedMatrixGraph : public MatrixGraph<T, IndexType>,
                            public DirectedGraphPrimitives<T, IndexType>
{
  public:
    constexpr DirectedMatrixGraph() = default;
    constexpr DirectedMatrixGraph(std::initializer_list<T> nodes);

    template <std::ranges::range R>
        requires std::convertible_to<std::ranges::range_value_t<R>, T>
    constexpr DirectedMatrixGraph(const R &nodes);

    [[nodiscard]] constexpr bool isDirected() const override;

    constexpr void addEdge(std::pair<T, T> edge) override;
    constexpr void addEdge(std::span<std::pair<T, T>> edges) override;
    constexpr void removeEdge(std::pair<T, T> edge) override;

    [[nodiscard]] constexpr std::vector<std::pair<T, T>> getEdges()
        const override;

    [[nodiscard]] constexpr std::vector<T> getOutgoingNeighbors(
        T key) const override;
    [[nodiscard]] constexpr std::vector<T> getIngoingNeighbors(
        T key) const override;

    constexpr bool hasEdge(std::pair<T, T> edge) const override;

  private:
    [[nodiscard]] constexpr std::vector<IndexType> internal_getNeighbors(
        IndexType index) const override;

    [[nodiscard]] constexpr std::vector<IndexType>
    internal_getOutgoingNeighbors(IndexType index) const override;
    [[nodiscard]] constexpr std::vector<IndexType> internal_getIngoingNeighbors(
        IndexType index) const override;
};

template <typename T, typename IndexType>
constexpr DirectedMatrixGraph<T, IndexType>::DirectedMatrixGraph(
    std::initializer_list<T> nodes)
{
    for (const auto &node : nodes)
    {
        if (this->getNodeMap().addByName(node))
        {
            for (auto &row : this->getEdgeMatrix())
            {
                row.emplace_back(false);
            }
            this->getEdgeMatrix().emplace_back(this->getEdgeMatrix().size() + 1,
                                               false);
        }
    }
}

template <typename T, typename IndexType>
template <std::ranges::range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, T>
constexpr DirectedMatrixGraph<T, IndexType>::DirectedMatrixGraph(
    const R &rangeOfNodes)
{
    for (const auto &node : rangeOfNodes)
    {
        if (this->getNodeMap().addByName(node))
        {
            for (auto &row : this->getEdgeMatrix())
            {
                row.emplace_back(false);
            }
            this->getEdgeMatrix().emplace_back(this->getEdgeMatrix().size() + 1,
                                               false);
        }
    }
}

template <typename T, typename IndexType>
constexpr bool DirectedMatrixGraph<T, IndexType>::isDirected() const
{
    return true;
}

template <typename T, typename IndexType>
constexpr void DirectedMatrixGraph<T, IndexType>::addEdge(std::pair<T, T> edge)
{
    const auto [first, second] = edge;

    if (!this->getNodeMap().contains(first))
        this->addNode(first);
    if (!this->getNodeMap().contains(second))
        this->addNode(second);

    const auto firstIndex =
        static_cast<size_t>(this->getNodeMap().convertNodeNameToIndex(first));
    const auto secondIndex =
        static_cast<size_t>(this->getNodeMap().convertNodeNameToIndex(second));

    if (this->getEdgeMatrix().at(firstIndex).at(secondIndex) == this->NOT_EDGE)
    {
        this->getEdgeNumber()++;
        this->getEdgeMatrix()[firstIndex][secondIndex] = this->EDGE;
    }
}

template <typename T, typename IndexType>
constexpr void DirectedMatrixGraph<T, IndexType>::addEdge(
    std::span<std::pair<T, T>> edges)
{
    for (const std::pair<T, T> &edge : edges)
    {
        for (const T &node : {edge.first, edge.second})
        {
            if (!this->getNodeMap().contains(node))
                this->addNode(node);
        }
        const auto firstIndex = static_cast<size_t>(
            this->getNodeMap().convertNodeNameToIndex(edge.first));
        const auto secondIndex = static_cast<size_t>(
            this->getNodeMap().convertNodeNameToIndex(edge.second));

        if (this->getEdgeMatrix().at(firstIndex).at(secondIndex) ==
            this->NOT_EDGE)
        {
            this->getEdgeNumber()++;
            this->getEdgeMatrix()[firstIndex][secondIndex] = this->EDGE;
        }
    }
}

template <typename T, typename IndexType>
constexpr void DirectedMatrixGraph<T, IndexType>::removeEdge(
    std::pair<T, T> edge)
{
    const auto first = static_cast<size_t>(
        this->getNodeMap().convertNodeNameToIndex(edge.first));
    const auto second = static_cast<size_t>(
        this->getNodeMap().convertNodeNameToIndex(edge.second));

    if (this->getEdgeMatrix().at(first).at(second) == this->EDGE)
    {
        this->getEdgeNumber()--;
        this->getEdgeMatrix()[first][second] = this->NOT_EDGE;
    }
}

template <typename T, typename IndexType>
constexpr std::vector<std::pair<T, T>> DirectedMatrixGraph<
    T, IndexType>::getEdges() const
{
    std::vector<std::pair<T, T>> result;
    result.reserve(this->getEdgeNumber());

    for (size_t i = 0; i < this->getEdgeMatrix().size(); i++)
    {
        for (size_t j = 0; j < this->getEdgeMatrix().size(); j++)
        {
            if (this->getEdgeMatrix().at(i).at(j) == this->EDGE)
            {
                result.emplace_back(this->getNodeMap().convertIndexToNodeName(
                                        static_cast<IndexType>(i)),
                                    this->getNodeMap().convertIndexToNodeName(
                                        static_cast<IndexType>(j)));
            }
        }
    }
    return result;
}

template <typename T, typename IndexType>
constexpr bool DirectedMatrixGraph<T, IndexType>::hasEdge(
    std::pair<T, T> edge) const
{
    const auto first = static_cast<size_t>(
        this->getNodeMap().convertNodeNameToIndex(edge.first));
    const auto second = static_cast<size_t>(
        this->getNodeMap().convertNodeNameToIndex(edge.second));

    return this->getEdgeMatrix().at(first).at(second) == this->EDGE;
}

template <typename T, typename IndexType>
constexpr std::vector<IndexType> DirectedMatrixGraph<
    T, IndexType>::internal_getNeighbors(IndexType index) const
{
    std::vector<IndexType> result;
    result.reserve(this->getEdgeMatrix().size());
    for (size_t i = 0; i < this->getEdgeMatrix().size(); i++)
    {
        if (this->getEdgeMatrix().at(static_cast<size_t>(index)).at(i) ==
                this->EDGE ||
            this->getEdgeMatrix().at(i).at(static_cast<size_t>(index)) ==
                this->EDGE)
        {
            result.emplace_back(static_cast<IndexType>(i));
        }
    }
    result.shrink_to_fit();
    return result;
}

template <typename T, typename IndexType>
constexpr std::vector<T> DirectedMatrixGraph<
    T, IndexType>::getOutgoingNeighbors(T key) const
{
    const auto nodeIndex = this->getNodeMap().convertNodeNameToIndex(key);
    return this->getNodeMap().convertIndexToNodeName(
        internal_getOutgoingNeighbors(nodeIndex));
}

template <typename T, typename IndexType>
constexpr std::vector<IndexType> DirectedMatrixGraph<
    T, IndexType>::internal_getOutgoingNeighbors(IndexType index) const
{
    std::vector<IndexType> result;
    result.reserve(this->getEdgeMatrix().size());

    for (size_t i = 0; i < this->getEdgeMatrix().size(); i++)
    {
        if (this->getEdgeMatrix().at(static_cast<size_t>(index)).at(i) ==
            this->EDGE)
            result.emplace_back(static_cast<IndexType>(i));
    }
    result.shrink_to_fit();
    return result;
}

template <typename T, typename IndexType>
constexpr std::vector<T> DirectedMatrixGraph<T, IndexType>::getIngoingNeighbors(
    T key) const
{
    const auto nodeIndex = this->getNodeMap().convertNodeNameToIndex(key);
    return this->getNodeMap().convertIndexToNodeName(
        internal_getIngoingNeighbors(nodeIndex));
}

template <typename T, typename IndexType>
constexpr std::vector<IndexType> DirectedMatrixGraph<
    T, IndexType>::internal_getIngoingNeighbors(IndexType index) const
{
    std::vector<IndexType> result;
    result.reserve(this->getEdgeMatrix().size());
    for (size_t i = 0; i < this->getEdgeMatrix().size(); i++)
    {
        if (this->getEdgeMatrix().at(i).at(static_cast<size_t>(index)) ==
            this->EDGE)
            result.emplace_back(i);
    }
    result.shrink_to_fit();
    return result;
}

} // namespace jGraph
