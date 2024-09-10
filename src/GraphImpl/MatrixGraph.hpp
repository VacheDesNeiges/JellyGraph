#pragma once

#include "GraphAlgorithms.hpp"
#include "GraphMeasures.hpp"
#include "GraphPrimitives.hpp"
#include <cassert>
#include <cstdint>
#include <linux/limits.h>
#include <utility>
#include <vector>

namespace jGraph
{

template <typename T>
class MatrixGraph : public GraphAlgorithms<T>,
                    public GraphMeasures<T>,
                    public virtual GraphPrimitives<T>
{
  public:
    MatrixGraph() = default;
    MatrixGraph(unsigned numNodes);

    void addNode(T nodeName) override;
    void removeNode(T nodeName) override;
    void addEdge(std::pair<T, T> edge) override;
    void removeEdge(std::pair<T, T> edge) override;

    unsigned getNumberOfNodes() const override;
    unsigned getNumberOfEdges() const override;

    std::vector<T> getNodes() const override;
    std::vector<std::pair<T, T>> getEdges() const override;
    std::vector<T> getNeighbors(T key) const override;

    bool hasEdge(std::pair<T, T> edge) const override;

  private:
    unsigned edgeNumber = 0;

    std::vector<std::vector<uint8_t>> edgeMatrix;
    static constexpr uint8_t NOT_EDGE = 0;
    static constexpr uint8_t EDGE = 1;

    std::vector<unsigned> internal_getNodes() const override;
    std::vector<unsigned> internal_getNeighbors(unsigned index) const override;
};

template <typename T>

MatrixGraph<T>::MatrixGraph(unsigned numNodes)
{
    this->getNodeMap().reserve(numNodes);
    for (unsigned i = 0; i < numNodes; i++)
    {
        this->getNodeMap().addByName(i);
    }

    edgeMatrix.reserve(numNodes);
    for (size_t i = 0; i < numNodes; i++)
    {
        edgeMatrix.emplace_back();
        edgeMatrix.back().reserve(numNodes);
    }
}

template <typename T>

void MatrixGraph<T>::addNode(T nodeName)
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

template <typename T>

void MatrixGraph<T>::removeNode(T nodeName)
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

template <typename T>

void MatrixGraph<T>::addEdge(std::pair<T, T> edge)
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

template <typename T>

void MatrixGraph<T>::removeEdge(std::pair<T, T> edge)
{
    const unsigned first =
        this->getNodeMap().convertNodeNameToIndex(edge.first);
    const unsigned second =
        this->getNodeMap().convertNodeNameToIndex(edge.second);

    if (edgeMatrix.at(first).at(second) == EDGE)
        edgeNumber--;

    edgeMatrix[first][second] = NOT_EDGE;
    edgeMatrix[second][first] = NOT_EDGE;
}

template <typename T>

bool MatrixGraph<T>::hasEdge(std::pair<T, T> edge) const
{
    return edgeMatrix.at(edge.first).at(edge.second);
}

template <typename T>

unsigned MatrixGraph<T>::getNumberOfNodes() const
{
    return static_cast<unsigned>(edgeMatrix.size());
}

template <typename T>

std::vector<T> MatrixGraph<T>::getNodes() const
{
    return this->getNodeMap().convertIndexToNodeName(internal_getNodes());
}

template <typename T>

std::vector<unsigned> MatrixGraph<T>::internal_getNodes() const
{
    std::vector<unsigned> result;
    result.reserve(edgeMatrix.size());
    for (unsigned i = 0; i < edgeMatrix.size(); i++)
    {
        result.emplace_back(i);
    }
    return result;
}

template <typename T>

std::vector<std::pair<T, T>> MatrixGraph<T>::getEdges() const
{
    std::vector<std::pair<T, T>> result;
    result.reserve(edgeNumber);

    for (unsigned i = 0; i < edgeMatrix.size(); i++)
    {
        for (unsigned j = 0; j < edgeMatrix.size(); j++)
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

template <typename T>

std::vector<T> MatrixGraph<T>::getNeighbors(T key) const
{
    const auto nodeIndex = this->getNodeMap().convertNodeNameToIndex(key);
    return this->getNodeMap().convertIndexToNodeName(
        internal_getNeighbors(nodeIndex));
}

template <typename T>

std::vector<unsigned> MatrixGraph<T>::internal_getNeighbors(
    unsigned index) const
{
    std::vector<unsigned> result;
    result.reserve(edgeMatrix.size());

    for (unsigned i = 0; i < edgeMatrix.size(); i++)
    {
        if (edgeMatrix.at(index).at(i) == EDGE)
            result.emplace_back(i);
    }
    result.shrink_to_fit();
    return result;
}

template <typename T>

unsigned MatrixGraph<T>::getNumberOfEdges() const
{
    return edgeNumber;
}

} // namespace jGraph
