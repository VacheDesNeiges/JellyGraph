#pragma once

#include "DirectedGraphPrimitives.hpp"
#include "MatrixGraph.hpp"
#include "UnderlyingIndexType.hpp"

#include <concepts>
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
    DirectedMatrixGraph() = default;
    DirectedMatrixGraph(unsigned numNodes);
    DirectedMatrixGraph(std::initializer_list<T> nodes);

    template <std::ranges::range R>
        requires std::convertible_to<std::ranges::range_value_t<R>, T>
    DirectedMatrixGraph(const R &nodes);

    [[nodiscard]] bool isDirected() const override;

    void addEdge(std::pair<T, T> edge) override;
    void addEdge(std::span<std::pair<T, T>> edges) override;
    void removeEdge(std::pair<T, T> edge) override;

    [[nodiscard]] std::vector<std::pair<T, T>> getEdges() const override;

    [[nodiscard]] std::vector<T> getNeighbors(T key) const override;
    [[nodiscard]] std::vector<T> getOutgoingNeighbors(T key) const override;
    [[nodiscard]] std::vector<T> getIngoingNeighbors(T key) const override;

    bool hasEdge(std::pair<T, T> edge) const override;

  private:
    [[nodiscard]] std::vector<IndexType> internal_getNeighbors(
        IndexType index) const override;

    [[nodiscard]] std::vector<IndexType> internal_getOutgoingNeighbors(
        IndexType index) const override;
    [[nodiscard]] std::vector<IndexType> internal_getIngoingNeighbors(
        IndexType index) const override;
};

template <typename T, typename IndexType>
DirectedMatrixGraph<T, IndexType>::DirectedMatrixGraph(
    std::initializer_list<T> nodes)
{
    for (const auto &node : nodes)
    {
        if (this->getNodeMap().addByName(node))
        {
            for (auto &row : this->getthis->getEdgeMatrix()())
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
DirectedMatrixGraph<T, IndexType>::DirectedMatrixGraph(const R &rangeOfNodes)
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
bool DirectedMatrixGraph<T, IndexType>::isDirected() const
{
    return true;
}

template <typename T, typename IndexType>
void DirectedMatrixGraph<T, IndexType>::addEdge(std::pair<T, T> edge)
{
    const auto [first, second] = edge;

    if (!this->getNodeMap().contains(first))
        this->addNode(first);
    if (!this->getNodeMap().contains(second))
        this->addNode(second);

    const auto firstIndex = this->getNodeMap().convertNodeNameToIndex(first);
    const auto secondIndex = this->getNodeMap().convertNodeNameToIndex(second);

    if (this->getEdgeMatrix().at(firstIndex).at(secondIndex) == this->NOT_EDGE)
    {
        this->getEdgeNumber()++;
        this->getEdgeMatrix()[firstIndex][secondIndex] = this->EDGE;
    }
}

template <typename T, typename IndexType>
void DirectedMatrixGraph<T, IndexType>::addEdge(
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

        if (this->getEdgeMatrix().at(firstIndex).at(secondIndex) ==
            this->NOT_EDGE)
        {
            this->getEdgeNumber()++;
            this->getEdgeMatrix()[firstIndex][secondIndex] = this->EDGE;
        }
    }
}

template <typename T, typename IndexType>
void DirectedMatrixGraph<T, IndexType>::removeEdge(std::pair<T, T> edge)
{
    const IndexType first =
        this->getNodeMap().convertNodeNameToIndex(edge.first);
    const IndexType second =
        this->getNodeMap().convertNodeNameToIndex(edge.second);

    if (this->getEdgeMatrix().at(first).at(second) == this->EDGE)
    {
        this->getEdgeNumber()--;
        this->getEdgeMatrix()[first][second] = this->NOT_EDGE;
    }
}

template <typename T, typename IndexType>
std::vector<std::pair<T, T>> DirectedMatrixGraph<T, IndexType>::getEdges() const
{
    std::vector<std::pair<T, T>> result;
    result.reserve(this->getEdgeNumber());

    for (IndexType i = 0;
         i < static_cast<IndexType>(this->getEdgeMatrix().size()); i++)
    {
        for (IndexType j = 0;
             j < static_cast<IndexType>(this->getEdgeMatrix().size()); j++)
        {
            if (this->getEdgeMatrix().at(i).at(j) == this->EDGE)
            {
                result.emplace_back(
                    this->getNodeMap().convertIndexToNodeName(i),
                    this->getNodeMap().convertIndexToNodeName(j));
            }
        }
    }
    return result;
}

template <typename T, typename IndexType>
bool DirectedMatrixGraph<T, IndexType>::hasEdge(std::pair<T, T> edge) const
{
    const IndexType first =
        this->getNodeMap().convertNodeNameToIndex(edge.first);
    const IndexType second =
        this->getNodeMap().convertNodeNameToIndex(edge.second);

    return this->getEdgeMatrix().at(first).at(second) == this->EDGE;
}

template <typename T, typename IndexType>
std::vector<T> DirectedMatrixGraph<T, IndexType>::getNeighbors(T key) const
{
    const auto nodeIndex = this->getNodeMap().convertNodeNameToIndex(key);
    return this->getNodeMap().convertIndexToNodeName(
        internal_getNeighbors(nodeIndex));
}

template <typename T, typename IndexType>
std::vector<IndexType> DirectedMatrixGraph<T, IndexType>::internal_getNeighbors(
    IndexType index) const
{
    std::vector<IndexType> result;
    result.reserve(this->getEdgeMatrix().size());
    for (IndexType i = 0;
         i < static_cast<IndexType>(this->getEdgeMatrix().size()); i++)
    {
        if (this->getEdgeMatrix().at(index).at(i) == this->EDGE ||
            this->getEdgeMatrix().at(i).at(index) == this->EDGE)
        {
            result.emplace_back(i);
        }
    }
    result.shrink_to_fit();
    return result;
}

template <typename T, typename IndexType>
std::vector<T> DirectedMatrixGraph<T, IndexType>::getOutgoingNeighbors(
    T key) const
{
    const auto nodeIndex = this->getNodeMap().convertNodeNameToIndex(key);
    return this->getNodeMap().convertIndexToNodeName(
        internal_getOutgoingNeighbors(nodeIndex));
}

template <typename T, typename IndexType>
std::vector<IndexType> DirectedMatrixGraph<
    T, IndexType>::internal_getOutgoingNeighbors(IndexType index) const
{
    std::vector<IndexType> result;
    result.reserve(this->getEdgeMatrix().size());

    for (IndexType i = 0;
         i < static_cast<IndexType>(this->getEdgeMatrix().size()); i++)
    {
        if (this->getEdgeMatrix().at(index).at(i) == this->EDGE)
            result.emplace_back(i);
    }
    result.shrink_to_fit();
    return result;
}

template <typename T, typename IndexType>
std::vector<T> DirectedMatrixGraph<T, IndexType>::getIngoingNeighbors(
    T key) const
{
    const auto nodeIndex = this->getNodeMap().convertNodeNameToIndex(key);
    return this->getNodeMap().convertIndexToNodeName(
        internal_getIngoingNeighbors(nodeIndex));
}

template <typename T, typename IndexType>
std::vector<IndexType> DirectedMatrixGraph<
    T, IndexType>::internal_getIngoingNeighbors(IndexType index) const
{
    std::vector<IndexType> result;
    result.reserve(this->getEdgeMatrix().size());
    for (IndexType i = 0;
         i < static_cast<IndexType>(this->getEdgeMatrix().size()); i++)
    {
        if (this->getEdgeMatrix().at(i).at(index) == this->EDGE)
            result.emplace_back(i);
    }
    result.shrink_to_fit();
    return result;
}

} // namespace jGraph
