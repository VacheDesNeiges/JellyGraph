#pragma once

#include "DirectedGraphPrimitives.hpp"
#include "MatrixGraph.hpp"
#include "UnderlyingIndexType.hpp"

#include <concepts>
#include <cstdint>
#include <ranges>
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

    template <std::ranges::range R>
        requires std::convertible_to<std::ranges::range_value_t<R>, T>
    DirectedMatrixGraph(const R &nodes);

    bool isDirected() const override;

    void addEdge(std::pair<T, T> edge) override;
    void removeEdge(std::pair<T, T> edge) override;

    std::vector<std::pair<T, T>> getEdges() const override;

    std::vector<T> getNeighbors(T key) const override;
    std::vector<T> getOutgoingNeighbors(T key) const override;
    std::vector<T> getIngoingNeighbors(T key) const override;

    bool hasEdge(std::pair<T, T> edge) const override;

  private:
    unsigned edgeNumber = 0;
    std::vector<std::vector<uint8_t>> edgeMatrix;

    std::vector<IndexType> internal_getNeighbors(
        IndexType index) const override;

    std::vector<IndexType> internal_getOutgoingNeighbors(
        IndexType index) const override;
    std::vector<IndexType> internal_getIngoingNeighbors(
        IndexType index) const override;
};

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

    if (edgeMatrix.at(firstIndex).at(secondIndex) == this->NOT_EDGE)
    {
        edgeNumber++;
        edgeMatrix[firstIndex][secondIndex] = this->EDGE;
    }
}

template <typename T, typename IndexType>
void DirectedMatrixGraph<T, IndexType>::removeEdge(std::pair<T, T> edge)
{
    const IndexType first =
        this->getNodeMap().convertNodeNameToIndex(edge.first);
    const IndexType second =
        this->getNodeMap().convertNodeNameToIndex(edge.second);

    if (edgeMatrix.at(first).at(second) == this->EDGE)
    {
        edgeNumber--;
        edgeMatrix[first][second] = this->NOT_EDGE;
    }
}

template <typename T, typename IndexType>
std::vector<std::pair<T, T>> DirectedMatrixGraph<T, IndexType>::getEdges() const
{
    std::vector<std::pair<T, T>> result;
    result.reserve(edgeNumber);

    for (IndexType i = 0; i < static_cast<IndexType>(edgeMatrix.size()); i++)
    {
        for (IndexType j = 0; j < static_cast<IndexType>(edgeMatrix.size());
             j++)
        {
            if (edgeMatrix.at(i).at(j) == this->EDGE)
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

    return edgeMatrix.at(first).at(second) == this->EDGE;
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
    result.reserve(edgeMatrix.size());
    for (IndexType i = 0; i < static_cast<IndexType>(edgeMatrix.size()); i++)
    {
        if (edgeMatrix.at(index).at(i) == this->EDGE ||
            edgeMatrix.at(i).at(index) == this->EDGE)
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
    const auto nodeIndex = this->getNodeMap().convertIndexToNodeName(key);
    return this->getNodeMap().convertIndexToNodeName(
        internal_getOutgoingNeighbors(nodeIndex));
}

template <typename T, typename IndexType>
std::vector<IndexType> DirectedMatrixGraph<
    T, IndexType>::internal_getOutgoingNeighbors(IndexType index) const
{
    std::vector<IndexType> result;
    result.reserve(edgeMatrix.size());

    for (IndexType i = 0; i < static_cast<IndexType>(edgeMatrix.size()); i++)
    {
        if (edgeMatrix.at(index).at(i) == this->EDGE)
            result.emplace_back(i);
    }
    result.shrink_to_fit();
    return result;
}

template <typename T, typename IndexType>
std::vector<T> DirectedMatrixGraph<T, IndexType>::getIngoingNeighbors(
    T key) const
{
    const auto nodeIndex = this->getNodeMap().convertIndexToNodeName(key);
    return this->getNodeMap().convertIndexToNodeName(
        internal_getIngoingNeighbors(nodeIndex));
}

template <typename T, typename IndexType>
std::vector<IndexType> DirectedMatrixGraph<
    T, IndexType>::internal_getIngoingNeighbors(IndexType index) const
{
    std::vector<IndexType> result;
    result.reserve(edgeMatrix.size());
    for (IndexType i = 0; i < static_cast<IndexType>(edgeMatrix.size()); i++)
    {
        if (edgeMatrix.at(i).at(index) == this->EDGE)
            result.emplace_back(i);
    }
    result.shrink_to_fit();
    return result;
}

} // namespace jGraph
