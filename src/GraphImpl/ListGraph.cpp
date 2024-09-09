#include "ListGraph.hpp"
#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>

namespace jGraph
{

ListGraph::ListGraph(unsigned numNodes)
{
    nodes.reserve(numNodes);
    nameIndexMap.reserve(numNodes);
    for (unsigned i = 0; i < numNodes; i++)
    {
        nameIndexMap.addByName(i);
        nodes.emplace_back();
    }
}

void ListGraph::addNode(unsigned nodeName)
{
    if (nameIndexMap.addByName(nodeName))
    {
        nodes.emplace_back();
    }
}

unsigned ListGraph::getNumberOfNodes() const
{
    return static_cast<unsigned>(nodes.size());
}

unsigned ListGraph::getNumberOfEdges() const
{
    return edgeNumber;
}

void ListGraph::removeNode(unsigned nodeName)
{
    if (nameIndexMap.contains(nodeName))
    {
        const auto index = nameIndexMap.getByName(nodeName);
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

void ListGraph::addEdge(std::pair<unsigned, unsigned> edge)
{
    const auto [first, second] = edge;
    if (!nameIndexMap.contains(first))
        addNode(first);

    if (!nameIndexMap.contains(second))
        addNode(second);

    const auto firstIndex = nameIndexMap.getByName(first);
    const auto secondIndex = nameIndexMap.getByName(second);

    if (std::ranges::find(nodes.at(firstIndex), secondIndex) ==
        nodes.at(firstIndex).end())
    {
        nodes.at(firstIndex).push_back(secondIndex);
        nodes.at(secondIndex).push_back(firstIndex);
        edgeNumber++;
    }
}

void ListGraph::removeEdge(std::pair<unsigned, unsigned> edge)
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

std::vector<unsigned> ListGraph::getNodes() const
{
    std::vector<unsigned> result;
    result.reserve(getNumberOfNodes());

    for (unsigned i = 0; i < nodes.size(); i++)
    {
        result.emplace_back(nameIndexMap.getByIndex(i));
    }
    return result;
}

std::vector<std::pair<unsigned, unsigned>> ListGraph::getEdges() const
{
    std::vector<std::pair<unsigned, unsigned>> result;
    result.reserve(getNumberOfEdges());

    for (unsigned i = 0; i < nodes.size(); i++)
    {
        for (unsigned j = 0; j < nodes.at(i).size(); j++)
        {
            const auto firstIndex = nameIndexMap.getByIndex(i);
            const auto secondIndex = nameIndexMap.getByIndex(nodes.at(i).at(j));
            if (firstIndex < secondIndex)
                result.emplace_back(firstIndex, secondIndex);
        }
    }

    return result;
}

std::vector<unsigned> ListGraph::getNeighbors(unsigned key) const
{
    std::vector<unsigned> result;
    const auto index = nameIndexMap.getByName(key);
    result.reserve(nodes.at(index).size());
    for (const auto neighbor : nodes.at(index))
    {
        result.emplace_back(nameIndexMap.getByIndex(neighbor));
    }

    return result;
}

bool ListGraph::hasEdge(std::pair<unsigned, unsigned> edge) const
{
    return std::ranges::find(nodes.at(edge.first), edge.second) !=
           nodes.at(edge.first).end();
}

} // namespace jGraph
