#pragma once
#include "GraphAlgorithms.hpp"
#include "GraphMeasures.hpp"
#include "GraphPrimitives.hpp"
#include "UnderlyingIndexType.hpp"
#include <algorithm>
#include <climits>
#include <concepts>
#include <cstddef>
#include <ranges>
#include <span>
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

    template <std::ranges::range R>
        requires std::convertible_to<std::ranges::range_value_t<R>, T>
    ListGraph(const R &nodes);

    void clear() override;
    [[nodiscard]] bool isDirected() const override;

    void addNode(T nodeName) override;
    void addNode(std::span<T> nodes) override;
    void removeNode(T nodeName) override;

    void addEdge(std::pair<T, T> edge) override;
    void addEdge(std::span<std::pair<T, T>> edges) override;
    void removeEdge(std::pair<T, T> edge) override;

    [[nodiscard]] size_t getNumberOfNodes() const override;
    [[nodiscard]] size_t getNumberOfEdges() const override;

    [[nodiscard]] std::vector<T> getNodes() const override;
    [[nodiscard]] std::vector<std::pair<T, T>> getEdges() const override;
    [[nodiscard]] std::vector<T> getNeighbors(T key) const override;

    [[nodiscard]] bool hasEdge(std::pair<T, T> edge) const override;

  protected:
    [[nodiscard]] std::vector<std::vector<IndexType>> &getAdjacencyList();
    [[nodiscard]] const std::vector<std::vector<IndexType>> &getAdjacencyList()
        const;

    [[nodiscard]] size_t &getEdgeNumber();
    [[nodiscard]] size_t getEdgeNumber() const;

  private:
    size_t edgeNumber = 0;
    std::vector<std::vector<IndexType>> adjacencyList;

    [[nodiscard]] std::vector<IndexType> internal_getNodes() const override;
    [[nodiscard]] std::vector<IndexType> internal_getNeighbors(
        IndexType index) const override;
};

template <typename T, typename IndexType>
template <std::ranges::range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, T>

ListGraph<T, IndexType>::ListGraph(const R &rangeOfNodes)
{
    for (const auto &node : rangeOfNodes)
    {
        if (this->getNodeMap().addByName(node))
            adjacencyList.emplace_back();
    }
}

template <typename T, typename IndexType>
void ListGraph<T, IndexType>::addNode(T nodeName)
{
    if (this->getNodeMap().addByName(nodeName))
    {
        adjacencyList.emplace_back();
    }
}

template <typename T, typename TypeIndex>
void ListGraph<T, TypeIndex>::addNode(std::span<T> newNodes)
{
    std::vector<T> nodesToAdd;
    nodesToAdd.reserve(newNodes.size());

    for (const auto &node : newNodes)
    {
        if (this->getNodeMap().addByName(node))
            nodesToAdd.emplace_back(node);
    }

    const auto newSize = adjacencyList.size() + nodesToAdd.size();
    adjacencyList.resize(newSize);
}

template <typename T, typename IndexType>
size_t ListGraph<T, IndexType>::getNumberOfNodes() const
{
    return adjacencyList.size();
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
    adjacencyList.erase(adjacencyList.begin() + index);

    for (auto &node : adjacencyList)
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

    if (std::ranges::find(adjacencyList.at(firstIndex), secondIndex) ==
        adjacencyList.at(firstIndex).end())
    {
        adjacencyList.at(firstIndex).push_back(secondIndex);
        adjacencyList.at(secondIndex).push_back(firstIndex);
        edgeNumber++;
    }
}

template <typename T, typename IndexType>
void ListGraph<T, IndexType>::addEdge(std::span<std::pair<T, T>> edges)
{
    for (const std::pair<T, T> &edge : edges)
    {
        for (const T &node : {edge.first, edge.second})
        {
            if (!this->getNodeMap().contains(node))
                addNode(node);
        }
        const auto &firstIndex =
            this->getNodeMap().convertNodeNameToIndex(edge.first);
        const auto &secondIndex =
            this->getNodeMap().convertNodeNameToIndex(edge.second);

        if (std::ranges::find(adjacencyList.at(firstIndex), secondIndex) ==
            adjacencyList.at(firstIndex).end())
        {
            edgeNumber++;
            adjacencyList.at(firstIndex).push_back(secondIndex);
            adjacencyList.at(secondIndex).push_back(firstIndex);
        }
    }
}

template <typename T, typename IndexType>
void ListGraph<T, IndexType>::removeEdge(std::pair<T, T> edge)
{
    const IndexType first =
        this->getNodeMap().convertNodeNameToIndex(edge.first);
    const IndexType second =
        this->getNodeMap().convertNodeNameToIndex(edge.second);

    auto &vec = adjacencyList.at(first);
    vec.erase(std::ranges::remove(vec, second).begin(), vec.end());

    auto &vec2 = adjacencyList.at(second);
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
    for (IndexType i = 0; i < static_cast<IndexType>(adjacencyList.size()); i++)
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

    for (IndexType i = 0; i < static_cast<IndexType>(adjacencyList.size()); i++)
    {
        for (IndexType j = 0;
             j < static_cast<IndexType>(adjacencyList.at(i).size()); j++)
        {
            const auto firstIndex =
                this->getNodeMap().convertIndexToNodeName(i);
            const auto secondIndex = this->getNodeMap().convertIndexToNodeName(
                adjacencyList.at(i).at(j));
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
    return adjacencyList.at(index);
}

template <typename T, typename IndexType>
bool ListGraph<T, IndexType>::hasEdge(std::pair<T, T> edge) const
{
    const IndexType first =
        this->getNodeMap().convertNodeNameToIndex(edge.first);
    const IndexType second =
        this->getNodeMap().convertNodeNameToIndex(edge.second);

    return std::ranges::find(adjacencyList.at(first), second) !=
           adjacencyList.at(first).end();
}

template <typename T, typename IndexType>
void ListGraph<T, IndexType>::clear()
{
    adjacencyList.clear();
}

template <typename T, typename IndexType>
bool ListGraph<T, IndexType>::isDirected() const
{
    return false;
}

template <typename T, typename IndexType>
size_t ListGraph<T, IndexType>::getEdgeNumber() const
{
    return edgeNumber;
}

template <typename T, typename IndexType>
size_t &ListGraph<T, IndexType>::getEdgeNumber()
{
    return edgeNumber;
}

template <typename T, typename IndexType>
const std::vector<std::vector<IndexType>> &ListGraph<
    T, IndexType>::getAdjacencyList() const
{
    return adjacencyList;
}

template <typename T, typename IndexType>
std::vector<std::vector<IndexType>> &ListGraph<T, IndexType>::getAdjacencyList()
{
    return adjacencyList;
}

} // namespace jGraph
