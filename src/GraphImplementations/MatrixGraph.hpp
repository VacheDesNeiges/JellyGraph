#pragma once

#include "DefaultTypes.hpp"
#include "GraphAlgorithms.hpp"
#include "GraphMeasures.hpp"
#include "GraphPrimitives.hpp"
#include "GraphSerialization.hpp"

#include <cassert>
#include <concepts>
#include <cstddef>
#include <ranges>
#include <span>
#include <utility>
#include <vector>

namespace jGraph
{

template <typename T, typename IndexType = internals::underlyingGraphIndex_t>
class MatrixGraph : public virtual GraphPrimitives<T, IndexType>,
                    public GraphAlgorithms<T, IndexType>,
                    public GraphSerialization<T, IndexType>,
                    public GraphMeasures<T, IndexType>
{
  public:
    constexpr MatrixGraph() = default;

    template <std::ranges::range R>
        requires std::convertible_to<std::ranges::range_value_t<R>, T>
    constexpr MatrixGraph(const R &nodes);

    constexpr void clear() override;
    [[nodiscard]] constexpr bool isDirected() const override;
    [[nodiscard]] constexpr bool isWeighted() const override;

    constexpr void addNode(T nodeName) override;
    constexpr void addNode(std::span<T> nodes) override;
    constexpr void removeNode(T nodeName) override;

    constexpr void addEdge(std::pair<T, T> edge) override;
    constexpr void addEdge(std::span<std::pair<T, T>> edges) override;
    constexpr void removeEdge(std::pair<T, T> edge) override;
    [[nodiscard]] constexpr bool hasEdge(std::pair<T, T> edge) const override;

    [[nodiscard]] constexpr size_t getNumberOfNodes() const override;
    [[nodiscard]] constexpr size_t getNumberOfEdges() const override;

    [[nodiscard]] constexpr std::vector<T> getNodes() const override;
    [[nodiscard]] constexpr std::vector<std::pair<T, T>> getEdges()
        const override;
    [[nodiscard]] constexpr std::vector<T> getNeighbors(T key) const override;

  protected:
    static constexpr IndexType NOT_EDGE = 0;
    static constexpr IndexType EDGE = 1;

    [[nodiscard]] constexpr std::vector<std::vector<IndexType>> &
    getEdgeMatrix();
    [[nodiscard]] constexpr const std::vector<std::vector<IndexType>> &
    getEdgeMatrix() const;
    [[nodiscard]] constexpr size_t &getEdgeNumber();
    [[nodiscard]] constexpr size_t getEdgeNumber() const;

  private:
    size_t edgeNumber = 0;
    std::vector<std::vector<IndexType>> edgeMatrix;

    constexpr std::vector<IndexType> internal_getNodes() const override;
    constexpr std::vector<IndexType> internal_getNeighbors(
        IndexType index) const override;

    void internal_assignParsedData(
        internals::parsedGraph<T> &parsedData) override;
};

template <typename T, typename IndexType>
template <std::ranges::range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, T>
constexpr MatrixGraph<T, IndexType>::MatrixGraph(const R &rangeOfNodes)
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
constexpr void MatrixGraph<T, IndexType>::addNode(T nodeName)
{
    if (this->getNodeMap().addByName(nodeName))
    {
        for (auto &row : edgeMatrix)
        {
            row.emplace_back(NOT_EDGE);
        }
        edgeMatrix.emplace_back(edgeMatrix.size() + 1, NOT_EDGE);
    }
}

template <typename T, typename IndexType>
constexpr void MatrixGraph<T, IndexType>::addNode(std::span<T> nodes)
{
    std::vector<T> nodesToAdd;
    nodesToAdd.reserve(nodes.size());
    for (const auto &node : nodes)
    {
        if (this->getNodeMap().addByName(node))
            nodesToAdd.emplace_back(node);
    }

    const auto newSize = edgeMatrix.size() + nodesToAdd.size();
    for (auto &row : edgeMatrix)
    {
        row.resize(newSize, NOT_EDGE);
    }
    edgeMatrix.resize(newSize, std::vector<IndexType>(newSize, NOT_EDGE));
}

template <typename T, typename IndexType>
constexpr void MatrixGraph<T, IndexType>::removeNode(T nodeName)
{

    if (this->getNodeMap().contains(nodeName))
    {
        const auto associatedEdgesNumber = this->getNeighbors(nodeName).size();
        edgeNumber -= associatedEdgesNumber;

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
constexpr void MatrixGraph<T, IndexType>::addEdge(std::pair<T, T> edge)
{
    const auto [first, second] = edge;
    if (!this->getNodeMap().contains(first))
        addNode(first);

    if (!this->getNodeMap().contains(second))
        addNode(second);

    const auto firstIndex =
        static_cast<size_t>(this->getNodeMap().convertNodeNameToIndex(first));
    const auto secondIndex =
        static_cast<size_t>(this->getNodeMap().convertNodeNameToIndex(second));

    if (edgeMatrix.at(firstIndex).at(secondIndex) == NOT_EDGE)
    {
        edgeNumber++;
        edgeMatrix[firstIndex][secondIndex] = EDGE;
        edgeMatrix[secondIndex][firstIndex] = EDGE;
    }
}

template <typename T, typename IndexType>
constexpr void MatrixGraph<T, IndexType>::addEdge(
    std::span<std::pair<T, T>> edges)
{
    size_t addedNodesCount = 0;
    this->getNodeMap().reserve(this->getNodeMap().getSize() +
                               (2 * edges.size()));
    for (const std::pair<T, T> &edge : edges)
    {
        for (const T &node : {edge.first, edge.second})
        {
            if (this->getNodeMap().addByName(node))
                addedNodesCount++;
        }
    }
    this->getNodeMap().shrinkToFit();

    const auto newMatrixSize = edgeMatrix.size() + addedNodesCount;
    for (auto &row : edgeMatrix)
    {
        row.resize(newMatrixSize, NOT_EDGE);
    }
    edgeMatrix.resize(newMatrixSize,
                      std::vector<IndexType>(newMatrixSize, NOT_EDGE));

    const auto edgesOfIndexes =
        this->getNodeMap().convertNodeNameToIndex(edges);

    for (const auto &[first, second] : edgesOfIndexes)
    {
        const auto firstIndex = static_cast<size_t>(first);
        const auto secondIndex = static_cast<size_t>(second);

        if (edgeMatrix.at(firstIndex).at(secondIndex) == NOT_EDGE)
        {
            edgeNumber++;
            edgeMatrix[firstIndex][secondIndex] = EDGE;
            edgeMatrix[secondIndex][firstIndex] = EDGE;
        }
    }
}

template <typename T, typename IndexType>
constexpr void MatrixGraph<T, IndexType>::removeEdge(std::pair<T, T> edge)
{
    const auto first = static_cast<size_t>(
        this->getNodeMap().convertNodeNameToIndex(edge.first));
    const auto second = static_cast<size_t>(
        this->getNodeMap().convertNodeNameToIndex(edge.second));

    if (edgeMatrix.at(first).at(second) == EDGE)
        edgeNumber--;

    edgeMatrix[first][second] = NOT_EDGE;
    edgeMatrix[second][first] = NOT_EDGE;
}

template <typename T, typename IndexType>
constexpr bool MatrixGraph<T, IndexType>::hasEdge(std::pair<T, T> edge) const
{
    const auto first = static_cast<size_t>(
        this->getNodeMap().convertNodeNameToIndex(edge.first));
    const auto second = static_cast<size_t>(
        this->getNodeMap().convertNodeNameToIndex(edge.second));

    return edgeMatrix.at(first).at(second) == EDGE;
}

template <typename T, typename IndexType>
constexpr size_t MatrixGraph<T, IndexType>::getNumberOfNodes() const
{
    return edgeMatrix.size();
}

template <typename T, typename IndexType>
constexpr std::vector<T> MatrixGraph<T, IndexType>::getNodes() const
{
    return this->getNodeMap().convertIndexToNodeName(internal_getNodes());
}

template <typename T, typename IndexType>
constexpr std::vector<IndexType> MatrixGraph<T, IndexType>::internal_getNodes()
    const
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
constexpr std::vector<std::pair<T, T>> MatrixGraph<T, IndexType>::getEdges()
    const
{
    std::vector<std::pair<T, T>> result;
    result.reserve(edgeNumber);

    for (size_t i = 0; i < edgeMatrix.size(); i++)
    {
        for (size_t j = 0; j < edgeMatrix.size(); j++)
        {

            if ((edgeMatrix.at(i).at(j) == EDGE) &&
                this->getNodeMap().convertIndexToNodeName(
                    static_cast<IndexType>(i)) <
                    this->getNodeMap().convertIndexToNodeName(
                        static_cast<IndexType>(j)))
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
constexpr std::vector<T> MatrixGraph<T, IndexType>::getNeighbors(T key) const
{
    const auto nodeIndex = this->getNodeMap().convertNodeNameToIndex(key);
    return this->getNodeMap().convertIndexToNodeName(
        internal_getNeighbors(nodeIndex));
}

template <typename T, typename IndexType>
constexpr std::vector<IndexType> MatrixGraph<
    T, IndexType>::internal_getNeighbors(IndexType index) const
{
    std::vector<IndexType> result;
    result.reserve(edgeMatrix.size());

    for (IndexType i = 0; i < static_cast<IndexType>(edgeMatrix.size()); i++)
    {
        if (edgeMatrix.at(static_cast<size_t>(index))
                .at(static_cast<size_t>(i)) == EDGE)
            result.emplace_back(i);
    }
    result.shrink_to_fit();
    return result;
}

template <typename T, typename IndexType>
constexpr size_t MatrixGraph<T, IndexType>::getNumberOfEdges() const
{
    return edgeNumber;
}

template <typename T, typename IndexType>
constexpr void MatrixGraph<T, IndexType>::clear()
{
    edgeMatrix.clear();
    edgeNumber = 0;
}

template <typename T, typename IndexType>
constexpr bool MatrixGraph<T, IndexType>::isDirected() const
{
    return false;
}

template <typename T, typename IndexType>
constexpr bool MatrixGraph<T, IndexType>::isWeighted() const
{
    return false;
}

template <typename T, typename IndexType>
constexpr size_t &MatrixGraph<T, IndexType>::getEdgeNumber()
{
    return edgeNumber;
}

template <typename T, typename IndexType>
constexpr size_t MatrixGraph<T, IndexType>::getEdgeNumber() const
{
    return edgeNumber;
}

template <typename T, typename IndexType>
constexpr std::vector<std::vector<IndexType>> &MatrixGraph<
    T, IndexType>::getEdgeMatrix()

{
    return edgeMatrix;
}

template <typename T, typename IndexType>
constexpr const std::vector<std::vector<IndexType>> &MatrixGraph<
    T, IndexType>::getEdgeMatrix() const

{
    return edgeMatrix;
}

template <typename T, typename IndexType>
void MatrixGraph<T, IndexType>::internal_assignParsedData(
    internals::parsedGraph<T> &parsedData)
{
    this->addEdge(parsedData.edges);
}

} // namespace jGraph
