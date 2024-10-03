#pragma once

#include "DefaultTypes.hpp"
#include "GraphAlgorithms.hpp"
#include "GraphMeasures.hpp"
#include "GraphPrimitives.hpp"
#include "WeightedGraphPrimitives.hpp"

#include <algorithm>
#include <cstddef>
#include <optional>
#include <span>
#include <tuple>
#include <utility>
#include <vector>

namespace jGraph
{

template <typename T, typename IndexType = internals::underlyingGraphIndex_t,
          typename WeightType = internals::underlyingGraphWeight_t>

class WeightedListGraph
    : public GraphMeasures<T, IndexType>,
      public GraphAlgorithms<T, IndexType>,
      //   public GraphSerialization<T, IndexType>,
      public virtual GraphPrimitives<T, IndexType>,
      public virtual WeightedGraphPrimitives<T, IndexType, WeightType>
{
  public:
    constexpr WeightedListGraph() = default;

    constexpr void clear() override;
    [[nodiscard]] constexpr bool isDirected() const override;
    [[nodiscard]] constexpr bool isWeighted() const override;

    constexpr void addNode(T nodeName) override;
    constexpr void addNode(std::span<T> newNodes) override;
    constexpr void removeNode(T nodeName) override;

    constexpr void addEdge(std::pair<T, T> edge) override;
    constexpr void addEdge(std::span<std::pair<T, T>> edges) override;

    constexpr void addWeightedEdge(std::pair<T, T> edge,
                                   WeightType weight) override;

    constexpr void addWeightedEdge(std::span<std::pair<T, T>> edges,
                                   WeightType weight) override;

    constexpr void addWeightedEdge(std::span<std::pair<T, T>> edges,
                                   std::span<WeightType> weights) override;

    constexpr void addWeightedEdge(std::tuple<T, T, WeightType> edge) override;

    constexpr void setWeight(std::pair<T, T> edge, WeightType weight) override;

    constexpr void addWeightedEdge(
        std::span<std::tuple<T, T, WeightType>> edges) override;

    constexpr void removeEdge(std::pair<T, T> edge) override;
    constexpr bool hasEdge(std::pair<T, T> edge) const override;

    [[nodiscard]] constexpr size_t getNumberOfNodes() const override;
    [[nodiscard]] constexpr size_t getNumberOfEdges() const override;

    constexpr std::optional<WeightType> getWeight(
        std::pair<T, T> edge) const override;

    constexpr std::vector<std::optional<WeightType>> getWeight(
        std::span<std::pair<T, T>> edges) const override;

    constexpr std::vector<T> getNodes() const override;
    constexpr std::vector<std::pair<T, T>> getEdges() const override;
    constexpr std::vector<T> getNeighbors(T key) const override;

  private:
    size_t edgeNumber = 0;
    std::vector<std::vector<std::pair<IndexType, WeightType>>> adjacencyList;

    [[nodiscard]] constexpr bool internal_insertEdge(
        std::pair<IndexType, IndexType> edge);
    [[nodiscard]] constexpr bool internal_insertEdge(
        std::pair<IndexType, IndexType> edge, WeightType weight);

    constexpr std::vector<IndexType> internal_getNodes() const override;
    constexpr std::vector<IndexType> internal_getNeighbors(
        IndexType index) const override;

    //  void internal_assignParsedData(
    //      internals::parsedGraph<T, WeightType> &parsedData) override;
};

template <typename T, typename IndexType, typename WeightType>
constexpr void WeightedListGraph<T, IndexType, WeightType>::clear()
{
    adjacencyList.clear();
    edgeNumber = 0;
}

template <typename T, typename IndexType, typename WeightType>
constexpr bool WeightedListGraph<T, IndexType, WeightType>::isWeighted() const
{
    return true;
}

template <typename T, typename IndexType, typename WeightType>
constexpr bool WeightedListGraph<T, IndexType, WeightType>::isDirected() const
{
    return false;
}

template <typename T, typename IndexType, typename WeightType>
constexpr void WeightedListGraph<T, IndexType, WeightType>::addNode(T nodeName)
{
    if (this->getNodeMap().addByName(nodeName))
        adjacencyList.emplace_back();
}

template <typename T, typename IndexType, typename WeightType>
constexpr void WeightedListGraph<T, IndexType, WeightType>::addNode(
    std::span<T> newNodes)
{
    std::vector<T> nodesToAdd;
    nodesToAdd.reserve(newNodes.size());

    for (const auto &node : newNodes)
    {
        if (this->getNodeMap().addByName(node))
            adjacencyList.emplace_back();
    }

    const auto newSize = adjacencyList.size() + nodesToAdd.size();
    adjacencyList.resize(newSize);
}

template <typename T, typename IndexType, typename WeightType>
constexpr void WeightedListGraph<T, IndexType, WeightType>::removeNode(
    T nodeName)
{
    if (!this->getNodeMap().contains(nodeName))
        return;

    const auto associatedEdgesNumber = this->getNeighbors(nodeName).size();
    edgeNumber -= associatedEdgesNumber;

    const auto index = this->getNodeMap().convertNodeNameToIndex(nodeName);
    adjacencyList.erase(adjacencyList.begin() + index);

    for (auto &node : adjacencyList)
    {
        node.erase(std::ranges::remove_if(
                       node,
                       [index](std::pair<IndexType, WeightType> &edge) {
                           return edge.first == index;
                       })
                       .begin(),
                   node.end());

        for (auto &[edge, _] : node)
        {
            if (edge > index)
                edge--;
        }
    }
}

template <typename T, typename IndexType, typename WeightType>
constexpr void WeightedListGraph<T, IndexType, WeightType>::addEdge(
    std::pair<T, T> edge)
{
    addWeightedEdge(edge, 1);
}

template <typename T, typename IndexType, typename WeightType>
constexpr void WeightedListGraph<T, IndexType, WeightType>::addEdge(
    std::span<std::pair<T, T>> edges)
{
    addWeightedEdge(edges, 1);
}

template <typename T, typename IndexType, typename WeightType>
constexpr void WeightedListGraph<T, IndexType, WeightType>::removeEdge(
    std::pair<T, T> edge)
{
    const auto indexEdge = this->getNodeMap().convertNodeNameToIndex(edge);
    const auto first = static_cast<size_t>(indexEdge.first);
    const auto second = static_cast<size_t>(indexEdge.second);

    auto &vec = adjacencyList.at(first);
    vec.erase(std::ranges::remove_if(vec, [&](const auto &pair) {
                  return pair.first == second;
              }).begin());

    auto &vec2 = adjacencyList.at(second);
    if (auto it = std::ranges::find_if(
            vec2, [&](const auto &pair) { return pair.first == first; });
        it != vec2.end())
    {
        vec2.erase(it);
        edgeNumber--;
    }
}

template <typename T, typename IndexType, typename WeightType>
constexpr void WeightedListGraph<T, IndexType, WeightType>::addWeightedEdge(
    std::pair<T, T> edge, WeightType weight)
{
    if (!this->getNodeMap().contains(edge.first))
        addNode(edge.first);
    if (!this->getNodeMap().contains(edge.second))
        addNode(edge.second);

    const auto indexEdge = this->getNodeMap().convertNodeNameToIndex(edge);

    if (internal_insertEdge(indexEdge, weight))
        edgeNumber++;
}

template <typename T, typename IndexType, typename WeightType>
constexpr void WeightedListGraph<T, IndexType, WeightType>::addWeightedEdge(
    std::span<std::pair<T, T>> edges, WeightType weight)
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
    adjacencyList.resize(adjacencyList.size() + addedNodesCount);

    for (const auto &edge : edges)
    {
        const auto indexEdge = this->getNodeMap().convertNodeNameToIndex(edge);
        if (internal_insertEdge(indexEdge, weight))
            edgeNumber++;
    }
}

template <typename T, typename IndexType, typename WeightType>
constexpr void WeightedListGraph<T, IndexType, WeightType>::addWeightedEdge(
    std::span<std::pair<T, T>> edges, std::span<WeightType> weights)
{
    size_t addedNodesCount = 0;
    this->getNodeMap().reserve(this->getNodeMap().getSize() +
                               (2 * edges.size()));

    for (const auto &edge : edges)
    {
        for (const T &node : {edge.first, edge.second})
        {
            if (this->getNodeMap().addByName(node))
                addedNodesCount++;
        }
    }
    this->getNodeMap().shrinkToFit();
    adjacencyList.resize(adjacencyList.size() + addedNodesCount);

    for (size_t i = 0; i < edges.size(); i++)
    {
        const auto indexEdge =
            this->getNodeMap().convertNodeNameToIndex(edges[i]);
        if (internal_insertEdge(indexEdge, weights[i]))
            edgeNumber++;
    }
}

template <typename T, typename IndexType, typename WeightType>
constexpr void WeightedListGraph<T, IndexType, WeightType>::addWeightedEdge(
    std::tuple<T, T, WeightType> edge)
{
    if (!this->getNodeMap().contains(std::get<0>(edge)))
        addNode(std::get<0>(edge));
    if (!this->getNodeMap().contains(std::get<1>(edge)))
        addNode(std::get<1>(edge));

    const auto indexEdge = this->getNodeMap().convertNodeNameToIndex(
        std::pair(std::get<0>(edge), std::get<1>(edge)));

    if (internal_insertEdge(indexEdge, std::get<2>(edge)))
        edgeNumber++;
}

template <typename T, typename IndexType, typename WeightType>
constexpr void WeightedListGraph<T, IndexType, WeightType>::addWeightedEdge(
    std::span<std::tuple<T, T, WeightType>> edges)
{
    for (const auto &edge : edges)
    {
        if (!this->getNodeMap().contains(std::get<0>(edge)))
            addNode(std::get<0>(edge));
        if (!this->getNodeMap().contains(std::get<1>(edge)))
            addNode(std::get<1>(edge));

        const auto indexEdge = this->getNodeMap().convertNodeNameToIndex(
            std::pair(std::get<0>(edge), std::get<1>(edge)));

        if (internal_insertEdge(indexEdge, std::get<2>(edge)))
            edgeNumber++;
    }
}

template <typename T, typename IndexType, typename WeightType>
constexpr std::vector<T> WeightedListGraph<T, IndexType, WeightType>::getNodes()
    const
{
    return this->getNodeMap().convertIndexToNodeName(internal_getNodes());
}

template <typename T, typename IndexType, typename WeightType>
constexpr std::vector<IndexType> WeightedListGraph<
    T, IndexType, WeightType>::internal_getNodes() const
{
    std::vector<IndexType> result;
    result.reserve(getNumberOfNodes());
    for (IndexType i = 0; i < static_cast<IndexType>(adjacencyList.size()); i++)
    {
        result.emplace_back(i);
    }
    return result;
}

template <typename T, typename IndexType, typename WeightType>
constexpr std::vector<std::pair<T, T>> WeightedListGraph<
    T, IndexType, WeightType>::getEdges() const
{
    std::vector<std::pair<T, T>> result;
    result.reserve(getNumberOfEdges());
    for (size_t i = 0; i < adjacencyList.size(); i++)
    {
        for (size_t j = 0; j < adjacencyList.at(i).size(); j++)
        {
            const auto firstIndex = this->getNodeMap().convertIndexToNodeName(
                static_cast<IndexType>(i));
            const auto secondIndex = this->getNodeMap().convertIndexToNodeName(
                static_cast<IndexType>(adjacencyList.at(i).at(j).first));
            if (firstIndex < secondIndex)
                result.emplace_back(firstIndex, secondIndex);
        }
    }
    return result;
}

template <typename T, typename IndexType, typename WeightType>
constexpr size_t WeightedListGraph<T, IndexType, WeightType>::getNumberOfNodes()
    const
{
    return adjacencyList.size();
}
template <typename T, typename IndexType, typename WeightType>
constexpr size_t WeightedListGraph<T, IndexType, WeightType>::getNumberOfEdges()
    const
{
    return edgeNumber;
}

template <typename T, typename IndexType, typename WeightType>
constexpr void WeightedListGraph<T, IndexType, WeightType>::setWeight(
    std::pair<T, T> edge, WeightType weight)
{
    const auto indexEdge = this->getNodeMap().convertNodeNameToIndex(edge);

    const auto foundNode = std::ranges::find_if(
        adjacencyList.at(static_cast<size_t>(indexEdge.first)),
        [indexEdge](const auto &pair) {
            return pair.first == indexEdge.second;
        });
    if (foundNode != adjacencyList.at(static_cast<size_t>(edge.first)).end())
    {
        foundNode->second = weight;
    }
    const auto foundsecondNode = std::ranges::find_if(
        adjacencyList.at(static_cast<size_t>(indexEdge.second)),
        [indexEdge](const auto &pair) {
            return pair.first == indexEdge.first;
        });
    if (foundsecondNode !=
        adjacencyList.at(static_cast<size_t>(edge.second)).end())
    {
        foundsecondNode->second = weight;
    }
}

template <typename T, typename IndexType, typename WeightType>
constexpr std::optional<WeightType> WeightedListGraph<
    T, IndexType, WeightType>::getWeight(std::pair<T, T> edge) const
{
    const auto firstNode =
        this->getNodeMap().convertNodeNameToIndex(edge.first);
    const auto secondNode =
        this->getNodeMap().convertNodeNameToIndex(edge.second);

    const auto foundNode = std::ranges::find_if(
        adjacencyList.at(static_cast<size_t>(firstNode)),
        [secondNode](const auto &pair) { return pair.first == secondNode; });

    if (foundNode != adjacencyList.at(static_cast<size_t>(firstNode)).end())
        return foundNode->second;
    return std::nullopt;
}

template <typename T, typename IndexType, typename WeightType>
constexpr std::vector<std::optional<WeightType>> WeightedListGraph<
    T, IndexType, WeightType>::getWeight(std::span<std::pair<T, T>> edges) const
{
    std::vector<std::optional<WeightType>> result;
    result.reserve(edges.size());

    for (const auto &[first, second] : edges)
    {
        const auto firstNode = this->getNodeMap().convertNodeNameToIndex(first);
        const auto secondNode =
            this->getNodeMap().convertNodeNameToIndex(second);

        const auto foundNode = std::ranges::find_if(
            adjacencyList.at(static_cast<size_t>(firstNode)),
            [secondNode](const auto &pair) {
                return pair.first == secondNode;
            });

        if (foundNode != adjacencyList.at(static_cast<size_t>(firstNode)).end())
        {
            result.emplace_back(foundNode->second);
        }
        else
        {
            result.emplace_back(std::nullopt);
        }
    }
    return result;
}

template <typename T, typename IndexType, typename WeightType>
constexpr std::vector<T> WeightedListGraph<
    T, IndexType, WeightType>::getNeighbors(T key) const
{
    return this->getNodeMap().convertIndexToNodeName(
        internal_getNeighbors(this->getNodeMap().convertNodeNameToIndex(key)));
}

template <typename T, typename IndexType, typename WeightType>
constexpr std::vector<IndexType> WeightedListGraph<
    T, IndexType, WeightType>::internal_getNeighbors(IndexType index) const
{
    std::vector<IndexType> result;
    const auto &adjaList = adjacencyList.at(static_cast<size_t>(index));

    result.reserve(adjaList.size());
    for (const auto &[neighbor, weight] : adjaList)
    {
        result.emplace_back(neighbor);
    }
    return result;
}

template <typename T, typename IndexType, typename WeightType>
constexpr bool WeightedListGraph<T, IndexType, WeightType>::hasEdge(
    std::pair<T, T> edge) const
{
    const auto [firstIndex, secondIndex] =
        this->getNodeMap().convertNodeNameToIndex(edge);
    const auto castedFirstIndex = static_cast<size_t>(firstIndex);

    return (std::ranges::find_if(
                adjacencyList.at(castedFirstIndex), [&](const auto &pair) {
                    return pair.first == secondIndex;
                }) == adjacencyList.at(castedFirstIndex).end());
}

template <typename T, typename IndexType, typename WeightType>
constexpr bool WeightedListGraph<T, IndexType, WeightType>::internal_insertEdge(
    std::pair<IndexType, IndexType> edge)
{
    return internal_insertEdge(edge, 1);
}

template <typename T, typename IndexType, typename WeightType>
constexpr bool WeightedListGraph<T, IndexType, WeightType>::internal_insertEdge(
    std::pair<IndexType, IndexType> edge, WeightType weight)
{
    if (std::ranges::find_if(
            adjacencyList.at(static_cast<size_t>(edge.first)),
            [&](const auto &pair) { return pair.first == edge.second; }) ==
        adjacencyList.at(static_cast<size_t>(edge.first)).end())
    {
        adjacencyList.at(static_cast<size_t>(edge.first))
            .emplace_back(edge.second, weight);
        adjacencyList.at(static_cast<size_t>(edge.second))
            .emplace_back(edge.first, weight);

        return true;
    }
    return false;
}

} // namespace jGraph
