#pragma once
#include "GraphAlgorithms.hpp"
#include "GraphMeasures.hpp"
#include "GraphPrimitives.hpp"
#include "UnderlyingIndexType.hpp"
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <ranges>
#include <utility>
#include <vector>

namespace jGraph
{

template <typename T, typename IndexType = internals::underlyingGraphIndex_t>
class ListGraph : public GraphAlgorithms<T, IndexType>,
                  public GraphMeasures<T, IndexType>,
                  public virtual GraphPrimitives<T, IndexType>
{
  public:
    ListGraph() = default;
    ListGraph(unsigned numNodes);

    template <std::ranges::range R>
        requires std::convertible_to<std::ranges::range_value_t<R>, T>
    ListGraph(const R &nodes);

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

  private:
    size_t edgeNumber = 0;
    std::vector<std::vector<IndexType>> nodes;

    std::vector<IndexType> internal_getNodes() const override;
    std::vector<IndexType> internal_getNeighbors(
        IndexType index) const override;
};

template <typename T, typename IndexType>
ListGraph<T, IndexType>::ListGraph(unsigned numNodes)
{

    nodes.reserve(numNodes);
    this->getNodeMap().reserve(numNodes);
    for (IndexType i = 0; i < static_cast<IndexType>(numNodes); i++)
    {
        this->getNodeMap().addByName(i);
        nodes.emplace_back();
    }
}

template <typename T, typename IndexType>
template <std::ranges::range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, T>

ListGraph<T, IndexType>::ListGraph(const R &rangeOfNodes)
{
    for (const auto &node : rangeOfNodes)
    {
        if (this->getNodeMap().addByName(node))
            nodes.emplace_back();
    }
}

template <typename T, typename IndexType>
void ListGraph<T, IndexType>::addNode(T nodeName)
{
    if (this->getNodeMap().addByName(nodeName))
    {
        nodes.emplace_back();
    }
}

template <typename T, typename IndexType>
size_t ListGraph<T, IndexType>::getNumberOfNodes() const
{
    return static_cast<IndexType>(nodes.size());
}

template <typename T, typename IndexType>
size_t ListGraph<T, IndexType>::getNumberOfEdges() const
{
    return edgeNumber;
}

template <typename T, typename IndexType>
void ListGraph<T, IndexType>::removeNode(T nodeName)
{
    if (!this->getNodeMap().contains(nodeName))
        return;

    const auto associatedEdgesNumber = this->getNeighbors(nodeName).size();
    edgeNumber -= associatedEdgesNumber;

    const auto index = this->getNodeMap().convertNodeNameToIndex(nodeName);
    nodes.erase(nodes.begin() + index);

    for (auto &node : nodes)
    {
        node.erase(std::ranges::remove(node, index).begin(), node.end());
        for (auto &edge : node)
        {
            if (edge > index)
            {
                edge--;
            }
        }
    }
}

template <typename T, typename IndexType>
void ListGraph<T, IndexType>::addEdge(std::pair<T, T> edge)
{
    const auto [first, second] = edge;
    if (!this->getNodeMap().contains(first))
        addNode(first);

    if (!this->getNodeMap().contains(second))
        addNode(second);

    const auto firstIndex = this->getNodeMap().convertNodeNameToIndex(first);
    const auto secondIndex = this->getNodeMap().convertNodeNameToIndex(second);

    if (std::ranges::find(nodes.at(firstIndex), secondIndex) ==
        nodes.at(firstIndex).end())
    {
        nodes.at(firstIndex).push_back(secondIndex);
        nodes.at(secondIndex).push_back(firstIndex);
        edgeNumber++;
    }
}

template <typename T, typename IndexType>
void ListGraph<T, IndexType>::removeEdge(std::pair<T, T> edge)
{
    const IndexType first =
        this->getNodeMap().convertNodeNameToIndex(edge.first);
    const IndexType second =
        this->getNodeMap().convertNodeNameToIndex(edge.second);

    auto &vec = nodes.at(first);
    vec.erase(std::ranges::remove(vec, second).begin(), vec.end());

    auto &vec2 = nodes.at(second);
    if (auto it = std::ranges::find(vec2, first); it != vec2.end())
    {
        vec2.erase(it);
        edgeNumber--;
    }
}

template <typename T, typename IndexType>
std::vector<T> ListGraph<T, IndexType>::getNodes() const
{
    return this->getNodeMap().convertIndexToNodeName(internal_getNodes());
}

template <typename T, typename IndexType>
std::vector<IndexType> ListGraph<T, IndexType>::internal_getNodes() const
{
    std::vector<IndexType> result;
    result.reserve(getNumberOfNodes());
    for (IndexType i = 0; i < static_cast<IndexType>(nodes.size()); i++)
    {
        result.emplace_back(i);
    }
    return result;
}

template <typename T, typename IndexType>
std::vector<std::pair<T, T>> ListGraph<T, IndexType>::getEdges() const
{
    std::vector<std::pair<T, T>> result;
    result.reserve(getNumberOfEdges());

    for (IndexType i = 0; i < static_cast<IndexType>(nodes.size()); i++)
    {
        for (IndexType j = 0; j < static_cast<IndexType>(nodes.at(i).size());
             j++)
        {
            const auto firstIndex =
                this->getNodeMap().convertIndexToNodeName(i);
            const auto secondIndex =
                this->getNodeMap().convertIndexToNodeName(nodes.at(i).at(j));
            if (firstIndex < secondIndex)
                result.emplace_back(firstIndex, secondIndex);
        }
    }

    return result;
}

template <typename T, typename IndexType>
std::vector<T> ListGraph<T, IndexType>::getNeighbors(T key) const
{
    return this->getNodeMap().convertIndexToNodeName(
        internal_getNeighbors(this->getNodeMap().convertNodeNameToIndex(key)));
}

template <typename T, typename IndexType>
std::vector<IndexType> ListGraph<T, IndexType>::internal_getNeighbors(
    IndexType index) const
{
    return nodes.at(index);
}

template <typename T, typename IndexType>
bool ListGraph<T, IndexType>::hasEdge(std::pair<T, T> edge) const
{
    const IndexType first =
        this->getNodeMap().convertNodeNameToIndex(edge.first);
    const IndexType second =
        this->getNodeMap().convertNodeNameToIndex(edge.second);

    return std::ranges::find(nodes.at(first), second) != nodes.at(first).end();
}

template <typename T, typename IndexType>
void ListGraph<T, IndexType>::clear()
{
    nodes.clear();
}

template <typename T, typename IndexType>
bool ListGraph<T, IndexType>::isDirected() const
{
    return false;
}

} // namespace jGraph
