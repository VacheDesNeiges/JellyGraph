#include "MatrixGraph.hpp"
#include <cassert>
#include <utility>
#include <vector>

namespace jGraph
{

MatrixGraph::MatrixGraph(unsigned numNodes)
{
    nameIndexMap.reserve(numNodes);
    for (unsigned i = 0; i < numNodes; i++)
    {
        nameIndexMap.addByName(i);
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
    if (nameIndexMap.addByName(nodeName))
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
    if (nameIndexMap.contains(nodeName))
    {
        const auto indexToDelete = nameIndexMap.getByName(nodeName);
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
    if (!nameIndexMap.contains(first))
        addNode(first);

    if (!nameIndexMap.contains(second))
        addNode(second);

    const auto firstIndex = nameIndexMap.getByName(first);
    const auto secondIndex = nameIndexMap.getByName(second);

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
    return nameIndexMap.getByIndex(getNodesAsIndexes());
}

std::vector<unsigned> MatrixGraph::getNodesAsIndexes() const
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
                nameIndexMap.getByIndex(i) < nameIndexMap.getByIndex(j))
            {
                result.emplace_back(nameIndexMap.getByIndex(i),
                                    nameIndexMap.getByIndex(j));
            }
        }
    }
    return result;
}

std::vector<unsigned> MatrixGraph::getNeighbors(unsigned key) const
{
    return nameIndexMap.getByIndex(
        getNeighborsAsIndexes(nameIndexMap.getByName(key)));
}

std::vector<unsigned> MatrixGraph::getNeighborsAsIndexes(unsigned index) const
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
