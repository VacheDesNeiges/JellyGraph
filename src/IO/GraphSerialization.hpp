#pragma once

#include "Concepts.hpp"
#include "Exceptions.hpp"
#include "GraphPrimitives.hpp"
#include "UnderlyingIndexType.hpp"

#include <cstddef>

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace jGraph
{

namespace internals
{

template <typename T>
struct parsedGraph
{
    std::vector<std::pair<T, T>> edges;
    std::vector<double> weights;
};
} // namespace internals

enum FileFormat : std::uint8_t
{
    DOT,
    DIMACS,
    EDGE_LIST,
};

template <typename T, typename IndexType = internals::underlyingGraphIndex_t>
class GraphSerialization : public virtual GraphPrimitives<T, IndexType>
{
  public:
    void loadFromFile(std::ifstream &file, FileFormat format);

    void saveToFile(std::string_view fileName, FileFormat format);

  private:
    constexpr void applyParsedWeights(internals::parsedGraph<T> &parsedData)
        requires internals::IsWeightedGraph<decltype(*this)>;

    void writeDimacsFile(std::string_view fileName);

    [[nodiscard]] internals::parsedGraph<T> parseDimacsFile(
        std::ifstream &file);
    void parseDimacsHeader(const std::string &currentLine,
                           internals::parsedGraph<T> &result);
    void parseDimacsEdge(const std::string &currentLine,
                         internals::parsedGraph<T> &result);

    void parseDotFile(std::ifstream &file);
    //  void parseEdgeListFile(std::ifstream &file);
};

template <typename T, typename IndexType>
void GraphSerialization<T, IndexType>::saveToFile(std::string_view fileName,
                                                  FileFormat format)
{
    switch (format)
    {
    case DIMACS:
        writeDimacsFile(fileName);
        break;
    default:
        break;
    }
}

template <typename T, typename IndexType>
void GraphSerialization<T, IndexType>::writeDimacsFile(
    std::string_view fileName)
{
    std::ofstream outputFile((std::string(fileName)));
    if (!outputFile.is_open())
    {
        throw Exception::JGraphIOException(
            "Error while writting a graph : could not open the file " +
            std::string(fileName));
    }

    const auto &edges = this->getEdges();
    outputFile << "p edge " << this->getNumberOfNodes() << " " << edges.size()
               << '\n';

    for (const auto &edge : edges)
    {
        outputFile << "e " << edge.first << " " << edge.second << "\n";
    }
    outputFile.close();
}

template <typename T, typename IndexType>
void GraphSerialization<T, IndexType>::loadFromFile(std::ifstream &file,
                                                    FileFormat format)
{
    if (!file.is_open())
    {
        throw Exception::JGraphIOException(
            "Could not open the std::ifstream passed to loadFromFile method");
    }

    internals::parsedGraph<T> parsingResult;
    switch (format)
    {
    case DIMACS:
        parsingResult = parseDimacsFile(file);
        break;
    case DOT:
        parseDotFile(file);
        std::cerr << "functionality not implemented yet";
        std::abort();
        break;
    case EDGE_LIST:
        //   parseEdgeListFile(file);
        std::cerr << "functionality not implemented yet";
        std::abort();
        break;
    default:
        throw Exception::JGraphIOException("Unknown file format");
    }
    this->addEdge(parsingResult.edges);
    if constexpr (internals::IsWeightedGraph<decltype(*this)>)
        this->applyParsedWeights(parsingResult);
}

template <typename T, typename IndexType>
constexpr void GraphSerialization<T, IndexType>::applyParsedWeights(
    internals::parsedGraph<T> &parsedData)
    requires internals::IsWeightedGraph<decltype(*this)>
{
    if (parsedData.weights.size() == 0)
    {
        this->setWeight(std::vector(parsedData.edges.size(), 1));
    }
    else if (parsedData.weights.size() == parsedData.edges.size())
    {
        this->setWeight(parsedData.weights);
    }
    else
    {
        throw Exception::JGraphIOException(
            "In parsed file, number of weights differ from number of edges "
            ":" +
            parsedData.edges.size() + " weights, " + parsedData.weights.size() +
            " edges");
    }
}

template <typename T, typename IndexType>
internals::parsedGraph<T> GraphSerialization<T, IndexType>::parseDimacsFile(
    std::ifstream &file)
{
    internals::parsedGraph<T> result;

    std::string currentLine;
    bool headerParsed = false;

    while (std::getline(file, currentLine))
    {
        if (currentLine.empty() || currentLine.at(0) == 'c')
            continue;

        if (currentLine.at(0) == 'p')
        {
            if (headerParsed)
            {
                throw Exception::internals::DimacsParsingException(
                    "Second definition of dimacs header : " + currentLine);
            }
            parseDimacsHeader(currentLine, result);
            headerParsed = true;
            continue;
        }

        if (currentLine.at(0) == 'e' || currentLine.at(0) == 'a')
        {
            if (!headerParsed)
            {
                throw Exception::internals::DimacsParsingException(
                    "Could not parse header line before edges");
            }
            parseDimacsEdge(currentLine, result);
            continue;
        }

        throw Exception::internals::DimacsParsingException(
            "Unknown line start '" + std::string(1, currentLine.at(0)) +
            "'. Valid line starts for this file format are p, c, a, or e.");
    }

    return result;
}

template <typename T, typename IndexType>
void GraphSerialization<T, IndexType>::parseDimacsHeader(
    const std::string &currentLine, internals::parsedGraph<T> &result)
{
    std::istringstream lineStream(currentLine);
    std::string lineStart;
    std::string edge;
    size_t numNodes = 0;
    size_t numEdges = 0;

    if (lineStream >> lineStart >> edge >> numNodes >> numEdges)
    {
        result.edges.reserve(numEdges);
        result.weights.reserve(numEdges);
    }
    else
    {
        throw Exception::internals::DimacsParsingException(
            "Failed to read header from line :" + currentLine);
    }
}

template <typename T, typename IndexType>
void GraphSerialization<T, IndexType>::parseDimacsEdge(
    const std::string &currentLine, internals::parsedGraph<T> &result)
{
    std::istringstream lineStream(currentLine);
    std::pair<T, T> edge;
    double weight{};

    std::string edgeIndicator;
    if (!(lineStream >> edgeIndicator) ||
        (edgeIndicator != "e" && edgeIndicator != "a"))
    {
        throw Exception::internals::DimacsParsingException(
            "error while parsing edge indicator at the start of the edge line "
            ": " +
            currentLine);
    }

    if (lineStream >> edge.first >> edge.second)
    {
        result.edges.push_back(edge);
    }
    else
    {
        std::cout << "Edge parsed :" << edge.first << " , " << edge.second
                  << '\n';
        throw Exception::internals::DimacsParsingException(
            "Failed to read edge from line: " + currentLine);
    }

    if (lineStream >> weight)
    {
        result.weights.push_back(weight);
    }
}

template <typename T, typename IndexType>
void GraphSerialization<T, IndexType>::parseDotFile(std::ifstream &file
                                                    [[maybe_unused]])
{
}

} // namespace jGraph
