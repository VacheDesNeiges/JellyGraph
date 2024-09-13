#pragma once

#include "GraphAlgorithms.hpp"
#include "GraphMeasures.hpp"
#include "GraphPrimitives.hpp"
#include "UnderlyingIndexType.hpp"

#include <cassert>
#include <concepts>
#include <cstddef>
#include <ranges>
#include <utility>
#include <vector>

namespace jGraph
{

template <typename T, typename IndexType = internals::underlyingGraphIndex_t>
class MatrixGraph : public virtual GraphPrimitives<T, IndexType>,
                    public GraphAlgorithms<T, IndexType>,
                    public GraphMeasures<T, IndexType>
{
  public:
    MatrixGraph() = default;
    MatrixGraph(unsigned numNodes);

    template <std::ranges::range R>
        requires std::convertible_to<std::ranges::range_value_t<R>, T>
    MatrixGraph(const R &nodes);

    void clear() override;
    bool isDirected() const override;

    void addNode(T nodeName) override;
    void removeNode(T nodeName) override;
    void addEdge(std::pair<T, T> edge) override;
    void removeEdge(std::pair<T, T> edge) override;

    size_t getNumberOfNodes() const override;
    size_t getNumberOfEdges() const override;

    std::vector<T> getNodes() const override;
    std::vector<std::pair<T, T>> getEdges() const override;
    std::vector<T> getNeighbors(T key) const override;

    bool hasEdge(std::pair<T, T> edge) const override;

  protected:
    static constexpr IndexType NOT_EDGE = 0;
    static constexpr IndexType EDGE = 1;

    std::vector<std::vector<IndexType>> &getEdgeMatrix();
    const std::vector<std::vector<IndexType>> &getEdgeMatrix() const;
    size_t &getEdgeNumber();
    size_t getEdgeNumber() const;

  private:
    size_t edgeNumber = 0;
    std::vector<std::vector<IndexType>> edgeMatrix;

    std::vector<IndexType> internal_getNodes() const override;
    std::vector<IndexType> internal_getNeighbors(
        IndexType index) const override;
};

template <typename T, typename IndexType>
MatrixGraph<T, IndexType>::MatrixGraph(unsigned numNodes)
{
    this->getNodeMap().reserve(numNodes);
    for (IndexType i = 0; i < static_cast<IndexType>(numNodes); i++)
    {
        this->getNodeMap().addByName(i);
    }

    edgeMatrix.reserve(numNodes);
    for (IndexType i = 0; i < static_cast<IndexType>(numNodes); i++)
    {
        edgeMatrix.emplace_back();
        edgeMatrix.back().reserve(numNodes);
    }
}

template <typename T, typename IndexType>
template <std::ranges::range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, T>
MatrixGraph<T, IndexType>::MatrixGraph(const R &rangeOfNodes)
{
    for (const auto &node : rangeOfNodes)
    {
        if (this->getNodeMap().addByName(node))
        {
            for (auto &row : edgeMatrix)
            {
                row.emplace_back(false);
            }
            edgeMatrix.emplace_back(edgeMatrix.size() + 1, false);
        }
    }
}

template <typename T, typename IndexType>
void MatrixGraph<T, IndexType>::addNode(T nodeName)
{
    if (this->getNodeMap().addByName(nodeName))
    {
        for (auto &row : edgeMatrix)
        {
            row.emplace_back(false);
        }
        edgeMatrix.emplace_back(edgeMatrix.size() + 1, false);
    }
}

template <typename T, typename IndexType>
void MatrixGraph<T, IndexType>::removeNode(T nodeName)
{
    if (this->getNodeMap().contains(nodeName))
    {
        const auto indexToDelete =
            this->getNodeMap().convertNodeNameToIndex(nodeName);
        edgeMatrix.erase(edgeMatrix.begin() + indexToDelete);
        for (auto &vec : edgeMatrix)
        {
            vec.erase(vec.begin() + indexToDelete);
        }
    }
}

template <typename T, typename IndexType>
void MatrixGraph<T, IndexType>::addEdge(std::pair<T, T> edge)
{
    const auto [first, second] = edge;
    if (!this->getNodeMap().contains(first))
        addNode(first);

    if (!this->getNodeMap().contains(second))
        addNode(second);

    const auto firstIndex = this->getNodeMap().convertNodeNameToIndex(first);
    const auto secondIndex = this->getNodeMap().convertNodeNameToIndex(second);

    if (edgeMatrix.at(firstIndex).at(secondIndex) == NOT_EDGE)
    {
        edgeNumber++;
        edgeMatrix[firstIndex][secondIndex] = EDGE;
        edgeMatrix[secondIndex][firstIndex] = EDGE;
    }
}

template <typename T, typename IndexType>
void MatrixGraph<T, IndexType>::removeEdge(std::pair<T, T> edge)
{
    const IndexType first =
        this->getNodeMap().convertNodeNameToIndex(edge.first);
    const IndexType second =
        this->getNodeMap().convertNodeNameToIndex(edge.second);

    if (edgeMatrix.at(first).at(second) == EDGE)
        edgeNumber--;

    edgeMatrix[first][second] = NOT_EDGE;
    edgeMatrix[second][first] = NOT_EDGE;
}

template <typename T, typename IndexType>
bool MatrixGraph<T, IndexType>::hasEdge(std::pair<T, T> edge) const
{
    const IndexType first =
        this->getNodeMap().convertNodeNameToIndex(edge.first);
    const IndexType second =
        this->getNodeMap().convertNodeNameToIndex(edge.second);

    return edgeMatrix.at(first).at(second) == EDGE;
}

template <typename T, typename IndexType>
size_t MatrixGraph<T, IndexType>::getNumberOfNodes() const
{
    return edgeMatrix.size();
}

template <typename T, typename IndexType>
std::vector<T> MatrixGraph<T, IndexType>::getNodes() const
{
    return this->getNodeMap().convertIndexToNodeName(internal_getNodes());
}

template <typename T, typename IndexType>
std::vector<IndexType> MatrixGraph<T, IndexType>::internal_getNodes() const
{
    std::vector<IndexType> result;
    result.reserve(edgeMatrix.size());
    for (IndexType i = 0; i < static_cast<IndexType>(edgeMatrix.size()); i++)
    {
        result.emplace_back(i);
    }
    return result;
}

template <typename T, typename IndexType>
std::vector<std::pair<T, T>> MatrixGraph<T, IndexType>::getEdges() const
{
    std::vector<std::pair<T, T>> result;
    result.reserve(edgeNumber);

    for (IndexType i = 0; i < static_cast<IndexType>(edgeMatrix.size()); i++)
    {
        for (IndexType j = 0; j < static_cast<IndexType>(edgeMatrix.size());
             j++)
        {

            if ((edgeMatrix.at(i).at(j) == EDGE) &&
                this->getNodeMap().convertIndexToNodeName(i) <
                    this->getNodeMap().convertIndexToNodeName(j))
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
std::vector<T> MatrixGraph<T, IndexType>::getNeighbors(T key) const
{
    const auto nodeIndex = this->getNodeMap().convertNodeNameToIndex(key);
    return this->getNodeMap().convertIndexToNodeName(
        internal_getNeighbors(nodeIndex));
}

template <typename T, typename IndexType>
std::vector<IndexType> MatrixGraph<T, IndexType>::internal_getNeighbors(
    IndexType index) const
{
    std::vector<IndexType> result;
    result.reserve(edgeMatrix.size());

    for (IndexType i = 0; i < static_cast<IndexType>(edgeMatrix.size()); i++)
    {
        if (edgeMatrix.at(index).at(i) == EDGE)
            result.emplace_back(i);
    }
    result.shrink_to_fit();
    return result;
}

template <typename T, typename IndexType>
size_t MatrixGraph<T, IndexType>::getNumberOfEdges() const
{
    return edgeNumber;
}

template <typename T, typename IndexType>
void MatrixGraph<T, IndexType>::clear()
{
    edgeMatrix.clear();
}

template <typename T, typename IndexType>
bool MatrixGraph<T, IndexType>::isDirected() const
{
    return false;
}

template <typename T, typename IndexType>
size_t &MatrixGraph<T, IndexType>::getEdgeNumber()
{
    return edgeNumber;
}

template <typename T, typename IndexType>
size_t MatrixGraph<T, IndexType>::getEdgeNumber() const
{
    return edgeNumber;
}

template <typename T, typename IndexType>
std::vector<std::vector<IndexType>> &MatrixGraph<T, IndexType>::getEdgeMatrix()

{
    return edgeMatrix;
}

template <typename T, typename IndexType>
const std::vector<std::vector<IndexType>> &MatrixGraph<
    T, IndexType>::getEdgeMatrix() const

{
    return edgeMatrix;
}

} // namespace jGraph
