#include "MatrixGraph.hpp"
#include <cassert>
#include <utility>
#include <vector>

namespace jGraph
{

MatrixGraph::MatrixGraph(unsigned numNodes)
{
    getNodeMap().reserve(numNodes);
    for (unsigned i = 0; i < numNodes; i++)
    {
        getNodeMap().addByName(i);
    }

    edgeMatrix.reserve(numNodes);
    for (size_t i = 0; i < numNodes; i++)
    {
        edgeMatrix.emplace_back();
        edgeMatrix.back().reserve(numNodes);
    }
}

void MatrixGraph::addNode(unsigned nodeName)
{
    if (getNodeMap().addByName(nodeName))
    {
        for (auto &row : edgeMatrix)
        {
            row.emplace_back(false);
        }
        edgeMatrix.emplace_back(edgeMatrix.size() + 1, false);
    }
}

void MatrixGraph::removeNode(unsigned nodeName)
{
    if (getNodeMap().contains(nodeName))
    {
        const auto indexToDelete =
            getNodeMap().convertNodeNameToIndex(nodeName);
        edgeMatrix.erase(edgeMatrix.begin() + indexToDelete);
        for (auto &vec : edgeMatrix)
        {
            vec.erase(vec.begin() + indexToDelete);
        }
    }
}

void MatrixGraph::addEdge(std::pair<unsigned, unsigned> edge)
{
    const auto [first, second] = edge;
    if (!getNodeMap().contains(first))
        addNode(first);

    if (!getNodeMap().contains(second))
        addNode(second);

    const auto firstIndex = getNodeMap().convertNodeNameToIndex(first);
    const auto secondIndex = getNodeMap().convertNodeNameToIndex(second);

    if (!edgeMatrix.at(firstIndex).at(secondIndex))
    {
        edgeNumber++;
        edgeMatrix[firstIndex][secondIndex] = true;
        edgeMatrix[secondIndex][firstIndex] = true;
    }
}

void MatrixGraph::removeEdge(std::pair<unsigned, unsigned> edge)
{
    assert(edge.first < edgeMatrix.size() && edge.second < edgeMatrix.size());
    if (edgeMatrix.at(edge.first).at(edge.second))
        edgeNumber--;

    edgeMatrix[edge.first][edge.second] = false;
    edgeMatrix[edge.second][edge.first] = false;
}

bool MatrixGraph::hasEdge(std::pair<unsigned, unsigned> edge) const
{
    return edgeMatrix.at(edge.first).at(edge.second);
}

unsigned MatrixGraph::getNumberOfNodes() const
{
    return static_cast<unsigned>(edgeMatrix.size());
}

std::vector<unsigned> MatrixGraph::getNodes() const
{
    return getNodeMap().convertIndexToNodeName(internal_getNodes());
}

std::vector<unsigned> MatrixGraph::internal_getNodes() const
{
    std::vector<unsigned> result;
    result.reserve(edgeMatrix.size());
    for (unsigned i = 0; i < edgeMatrix.size(); i++)
    {
        result.emplace_back(i);
    }
    return result;
}

std::vector<std::pair<unsigned, unsigned>> MatrixGraph::getEdges() const
{
    std::vector<std::pair<unsigned, unsigned>> result;
    result.reserve(edgeNumber);

    for (unsigned i = 0; i < edgeMatrix.size(); i++)
    {
        for (unsigned j = 0; j < edgeMatrix.size(); j++)
        {

            if (edgeMatrix.at(i).at(j) &&
                getNodeMap().convertIndexToNodeName(i) <
                    getNodeMap().convertIndexToNodeName(j))
            {
                result.emplace_back(getNodeMap().convertIndexToNodeName(i),
                                    getNodeMap().convertIndexToNodeName(j));
            }
        }
    }
    return result;
}

std::vector<unsigned> MatrixGraph::getNeighbors(unsigned key) const
{
    const auto nodeIndex = getNodeMap().convertNodeNameToIndex(key);
    return getNodeMap().convertIndexToNodeName(
        internal_getNeighbors(nodeIndex));
}

std::vector<unsigned> MatrixGraph::internal_getNeighbors(unsigned index) const
{
    std::vector<unsigned> result;
    result.reserve(edgeMatrix.size());

    for (unsigned i = 0; i < edgeMatrix.size(); i++)
    {
        if (edgeMatrix.at(index).at(i))
            result.emplace_back(i);
    }
    result.shrink_to_fit();
    return result;
}

unsigned MatrixGraph::getNumberOfEdges() const
{
    return edgeNumber;
}

} // namespace jGraph
