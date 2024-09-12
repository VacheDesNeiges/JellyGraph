#pragma once

#include "DirectedGraphPrimitives.hpp"
#include "MatrixGraph.hpp"

#include <concepts>
#include <cstdint>
#include <ranges>
#include <utility>
#include <vector>

namespace jGraph
{

template <typename T>
class DirectedMatrixGraph : public MatrixGraph<T>,
                            public DirectedGraphPrimitives<T>
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

    std::vector<unsigned> internal_getNeighbors(unsigned index) const override;

    std::vector<unsigned> internal_getOutgoingNeighbors(
        unsigned index) const override;
    std::vector<unsigned> internal_getIngoingNeighbors(
        unsigned index) const override;
};

template <typename T>
bool DirectedMatrixGraph<T>::isDirected() const
{
    return true;
}

template <typename T>
void DirectedMatrixGraph<T>::addEdge(std::pair<T, T> edge)
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

template <typename T>
void DirectedMatrixGraph<T>::removeEdge(std::pair<T, T> edge)
{
    const unsigned first =
        this->getNodeMap().convertNodeNameToIndex(edge.first);
    const unsigned second =
        this->getNodeMap().convertNodeNameToIndex(edge.second);

    if (edgeMatrix.at(first).at(second) == this->EDGE)
    {
        edgeNumber--;
        edgeMatrix[first][second] = this->NOT_EDGE;
    }
}

template <typename T>
std::vector<std::pair<T, T>> DirectedMatrixGraph<T>::getEdges() const
{
    std::vector<std::pair<T, T>> result;
    result.reserve(edgeNumber);

    for (unsigned i = 0; i < edgeMatrix.size(); i++)
    {
        for (unsigned j = 0; j < edgeMatrix.size(); j++)
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

template <typename T>
bool DirectedMatrixGraph<T>::hasEdge(std::pair<T, T> edge) const
{
    const unsigned first =
        this->getNodeMap().convertNodeNameToIndex(edge.first);
    const unsigned second =
        this->getNodeMap().convertNodeNameToIndex(edge.second);

    return edgeMatrix.at(first).at(second) == this->EDGE;
}

template <typename T>
std::vector<T> DirectedMatrixGraph<T>::getNeighbors(T key) const
{
    const auto nodeIndex = this->getNodeMap().convertNodeNameToIndex(key);
    return this->getNodeMap().convertIndexToNodeName(
        internal_getNeighbors(nodeIndex));
}

template <typename T>
std::vector<unsigned> DirectedMatrixGraph<T>::internal_getNeighbors(
    unsigned index) const
{
    std::vector<unsigned> result;
    result.reserve(edgeMatrix.size());
    for (unsigned i = 0; i < edgeMatrix.size(); i++)
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

template <typename T>
std::vector<T> DirectedMatrixGraph<T>::getOutgoingNeighbors(T key) const
{
    const auto nodeIndex = this->getNodeMap().convertIndexToNodeName(key);
    return this->getNodeMap().convertIndexToNodeName(
        internal_getOutgoingNeighbors(nodeIndex));
}

template <typename T>
std::vector<unsigned> DirectedMatrixGraph<T>::internal_getOutgoingNeighbors(
    unsigned index) const
{
    std::vector<unsigned> result;
    result.reserve(edgeMatrix.size());

    for (unsigned i = 0; i < edgeMatrix.size(); i++)
    {
        if (edgeMatrix.at(index).at(i) == this->EDGE)
            result.emplace_back(i);
    }
    result.shrink_to_fit();
    return result;
}

template <typename T>
std::vector<T> DirectedMatrixGraph<T>::getIngoingNeighbors(T key) const
{
    const auto nodeIndex = this->getNodeMap().convertIndexToNodeName(key);
    return this->getNodeMap().convertIndexToNodeName(
        internal_getIngoingNeighbors(nodeIndex));
}

template <typename T>
std::vector<unsigned> DirectedMatrixGraph<T>::internal_getIngoingNeighbors(
    unsigned index) const
{
    std::vector<unsigned> result;
    result.reserve(edgeMatrix.size());
    for (unsigned i = 0; i < edgeMatrix.size(); i++)
    {
        if (edgeMatrix.at(i).at(index) == this->EDGE)
            result.emplace_back(i);
    }
    result.shrink_to_fit();
    return result;
}

} // namespace jGraph
