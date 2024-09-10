#pragma once
#include "GraphAlgorithms.hpp"
#include "GraphMeasures.hpp"
#include "GraphPrimitives.hpp"
#include <algorithm>
#include <utility>
#include <vector>

namespace jGraph
{

template <typename T>
class ListGraph : public GraphAlgorithms<T>,
                  public GraphMeasures<T>,
                  public virtual GraphPrimitives<T>
{
  public:
    ListGraph() = default;
    ListGraph(unsigned numNodes);

    void clear() override;

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
    std::vector<std::vector<unsigned>> nodes;

    std::vector<unsigned> internal_getNodes() const override;
    std::vector<unsigned> internal_getNeighbors(unsigned index) const override;
};

template <typename T>
ListGraph<T>::ListGraph(unsigned numNodes)
{

    nodes.reserve(numNodes);
    this->getNodeMap().reserve(numNodes);
    for (unsigned i = 0; i < numNodes; i++)
    {
        this->getNodeMap().addByName(i);
        nodes.emplace_back();
    }
}

template <typename T>
void ListGraph<T>::addNode(T nodeName)
{
    if (this->getNodeMap().addByName(nodeName))
    {
        nodes.emplace_back();
    }
}

template <typename T>
unsigned ListGraph<T>::getNumberOfNodes() const
{
    return static_cast<unsigned>(nodes.size());
}

template <typename T>
unsigned ListGraph<T>::getNumberOfEdges() const
{
    return edgeNumber;
}

template <typename T>
void ListGraph<T>::removeNode(T nodeName)
{
    if (this->getNodeMap().contains(nodeName))
    {
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
}

template <typename T>
void ListGraph<T>::addEdge(std::pair<T, T> edge)
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

template <typename T>
void ListGraph<T>::removeEdge(std::pair<T, T> edge)
{
    const auto [first, second] = edge;
    auto &vec = nodes.at(first);
    vec.erase(std::ranges::remove(vec, second).begin(), vec.end());

    auto &vec2 = nodes.at(second);
    if (auto it = std::ranges::find(vec2, first); it != vec2.end())
    {
        vec2.erase(it);
        edgeNumber--;
    }
}

template <typename T>
std::vector<T> ListGraph<T>::getNodes() const
{
    return this->getNodeMap().convertIndexToNodeName(internal_getNodes());
}

template <typename T>
std::vector<unsigned> ListGraph<T>::internal_getNodes() const
{
    std::vector<unsigned> result;
    result.reserve(getNumberOfNodes());
    for (unsigned i = 0; i < nodes.size(); i++)
    {
        result.emplace_back(i);
    }
    return result;
}

template <typename T>
std::vector<std::pair<T, T>> ListGraph<T>::getEdges() const
{
    std::vector<std::pair<T, T>> result;
    result.reserve(getNumberOfEdges());

    for (unsigned i = 0; i < nodes.size(); i++)
    {
        for (unsigned j = 0; j < nodes.at(i).size(); j++)
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

template <typename T>
std::vector<T> ListGraph<T>::getNeighbors(T key) const
{
    return this->getNodeMap().convertIndexToNodeName(
        internal_getNeighbors(this->getNodeMap().convertNodeNameToIndex(key)));
}

template <typename T>
std::vector<unsigned> ListGraph<T>::internal_getNeighbors(unsigned index) const
{
    return nodes.at(index);
}

template <typename T>
bool ListGraph<T>::hasEdge(std::pair<T, T> edge) const
{
    return std::ranges::find(nodes.at(edge.first), edge.second) !=
           nodes.at(edge.first).end();
}

template <typename T>
void ListGraph<T>::clear()
{
    nodes.clear();
}

} // namespace jGraph
