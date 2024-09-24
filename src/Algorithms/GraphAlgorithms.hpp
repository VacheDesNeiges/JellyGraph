#pragma once

#include "DefaultTypes.hpp"
#include "GraphPrimitives.hpp"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cstdio>
#include <limits>
#include <optional>
#include <queue>
#include <stack>
#include <utility>
#include <vector>

namespace jGraph
{

template <typename T, typename IndexType = internals::underlyingGraphIndex_t>
class GraphAlgorithms : public virtual GraphPrimitives<T, IndexType>
{
  public:
    [[nodiscard]] constexpr bool isConnected() const;
    [[nodiscard]] constexpr size_t numberOfComponents() const;
    [[nodiscard]] constexpr std::vector<T> componentOfNode(T node) const;
    [[nodiscard]] constexpr std::vector<std::vector<T>> components() const;

    [[nodiscard]] constexpr std::vector<std::vector<std::pair<T, T>>> djikstra(
        std::pair<T, T> pathExtremities) const;
    [[nodiscard]] constexpr std::vector<std::pair<T, T>> djikstra(
        T startingNode) const;

  private:
    [[nodiscard]] constexpr std::vector<IndexType> internal_componentOfNode(
        IndexType node) const;
    [[nodiscard]] constexpr std::vector<std::vector<IndexType>>
    internal_components() const;
    [[nodiscard]] constexpr std::vector<std::pair<IndexType, IndexType>>
    internal_djikstra(IndexType startingNode) const;
};

template <typename T, typename IndexType>
constexpr bool GraphAlgorithms<T, IndexType>::isConnected() const
{
    return components().size() == 1;
}

template <typename T, typename IndexType>
constexpr std::vector<T> GraphAlgorithms<T, IndexType>::componentOfNode(
    T node) const
{
    const auto nodeIndex = this->getNodeMap().convertNodeNameToIndex(node);

    return this->getNodeMap().convertIndexToNodeName(
        internal_componentOfNode(nodeIndex));
}

template <typename T, typename IndexType>
constexpr std::vector<IndexType> GraphAlgorithms<
    T, IndexType>::internal_componentOfNode(IndexType node) const
{
    std::vector<IndexType> result;
    std::vector<bool> visited(this->getNumberOfNodes(), false);

    std::vector<IndexType> underlyingStack;
    underlyingStack.reserve(this->getNumberOfNodes());
    std::stack<IndexType, std::vector<IndexType>> stack(
        std::move(underlyingStack));

    stack.push(node);
    while (!stack.empty())
    {
        const auto currentNode = stack.top();
        stack.pop();
        if (visited.at(currentNode))
            continue;

        visited.at(currentNode) = true;
        result.push_back(currentNode);
        for (const auto neighbor : this->internal_getNeighbors(currentNode))
        {
            if (!visited.at(neighbor))
                stack.push(neighbor);
        }
    }
    return result;
}

template <typename T, typename IndexType>
constexpr std::vector<std::vector<T>> GraphAlgorithms<
    T, IndexType>::components() const
{
    std::vector<std::vector<T>> result;
    const auto components = internal_components();
    result.reserve(components.size());
    for (const auto &component : components)
    {
        result.emplace_back(
            GraphPrimitives<T>::getNodeMap().convertIndexToNodeName(component));
    }
    return result;
}

template <typename T, typename IndexType>
constexpr std::vector<std::vector<IndexType>> GraphAlgorithms<
    T, IndexType>::internal_components() const
{
    std::vector<std::vector<IndexType>> components;

    const auto nodes = this->internal_getNodes();
    std::vector<bool> visited(nodes.size(), false);
    std::vector<IndexType> currentComponent;

    std::vector<IndexType> underlyingStack;
    underlyingStack.reserve(nodes.size());
    std::stack<IndexType, std::vector<IndexType>> stack(
        std::move(underlyingStack));

    for (const auto nodeIndex : nodes)
    {
        if (visited.at(nodeIndex))
            continue;

        currentComponent.clear();
        stack.push(nodeIndex);
        while (!stack.empty())
        {
            const auto node = stack.top();
            stack.pop();
            if (visited.at(node))
                continue;

            visited.at(node) = true;
            currentComponent.push_back(node);
            for (const auto neighbor : this->internal_getNeighbors(node))
            {
                if (!visited.at(neighbor))
                    stack.push(neighbor);
            }
        }
        components.push_back(currentComponent);
    }
    return components;
}

template <typename T, typename IndexType>
constexpr std::vector<std::pair<T, T>> GraphAlgorithms<T, IndexType>::djikstra(
    T startingNode) const
{
    IndexType startingNodeIndex =
        this->getNodeMap().convertNodeNameToIndex(startingNode);
    const auto resultAsIndexes = internal_djikstra(startingNodeIndex);

    std::vector<std::pair<T, T>> result;
    result.reserve(resultAsIndexes.size());

    for (const auto &[fromNode, node] : resultAsIndexes)
    {
        const auto first = this->getNodeMap().convertIndexToNodeName(fromNode);
        const auto second = this->getNodeMap().convertIndexToNodeName(node);
        result.emplace_back({first, second});
    }
    return result;
}

template <typename T, typename IndexType>
constexpr std::vector<std::pair<IndexType, IndexType>> GraphAlgorithms<
    T, IndexType>::internal_djikstra(IndexType startingNode) const
{
    // Data structures necessary for the algorithm
    // ------------------------------------------------------------------------
    const auto numNodes = this->getNumberOfNodes();
    std::vector<std::optional<IndexType>> previousNodes(numNodes, std::nullopt);
    std::vector<double> distances(numNodes,
                                  std::numeric_limits<double>::infinity());
    distances.at(startingNode) = 0;

    auto comparaisonFunction = [](std::pair<IndexType, double> lhs,
                                  std::pair<IndexType, double> rhs) {
        return lhs.second > rhs.second;
    };

    std::priority_queue<std::pair<IndexType, double>,
                        std::vector<std::pair<IndexType, double>>,
                        decltype(comparaisonFunction)>
        nodesPriorityQueue(comparaisonFunction);
    // Algorithm
    // ------------------------------------------------------------------------
    nodesPriorityQueue.emplace({startingNode, 0});
    while (!nodesPriorityQueue.empty())
    {
        const auto [currentBestNode, currentBestDistance] =
            nodesPriorityQueue.top();
        nodesPriorityQueue.pop();

        if (currentBestDistance > distances[currentBestNode])
            continue;

        for (const IndexType examinedNeighbor :
             this->internal_getNeighbors(currentBestNode))
        {
            const double edgeWeight =
                this->getWeight(currentBestNode, examinedNeighbor);
            assert(edgeWeight >= 0);

            double examinedDistance = distances[currentBestNode] + edgeWeight;

            if (examinedDistance < distances.at(examinedNeighbor))
            {
                distances[examinedNeighbor] = examinedDistance;
                previousNodes[examinedNeighbor] = currentBestNode;
                nodesPriorityQueue.emplace(examinedNeighbor, examinedDistance);
            }
        }
    }
    // Result construction
    // ------------------------------------------------------------------------
    std::vector<std::pair<IndexType, IndexType>> result;
    for (const auto &node : this->internal_getNodes())
    {
        if (previousNodes[node])
            result.emplace_back(previousNodes[node].value(), node);
    }
    return result;
}

}; // namespace jGraph
