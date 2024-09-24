#include "gtest/gtest.h"

#include "DirectedListGraph.hpp"
#include "DirectedMatrixGraph.hpp"
#include "GraphSerialization.hpp"
#include "ListGraph.hpp"
#include "MatrixGraph.hpp"

#include <fstream>

template <typename T>
class GraphIOTests : public ::testing::Test
{
  public:
    T graph;
};

using AllGraphs = ::testing::Types<
    jGraph::MatrixGraph<unsigned>, jGraph::MatrixGraph<long long>,
    jGraph::ListGraph<unsigned>, jGraph::ListGraph<long long>,
    jGraph::DirectedListGraph<long long>, jGraph::DirectedMatrixGraph<double>>;

TYPED_TEST_SUITE(GraphIOTests, AllGraphs);

template <typename T>
class DirectedGraphIOTests : public ::testing::Test
{
  public:
    T graph;
};

using DirectedGraphs = ::testing::Types<
    jGraph::DirectedListGraph<long long>, jGraph::DirectedListGraph<float>,
    jGraph::DirectedMatrixGraph<unsigned>, jGraph::DirectedMatrixGraph<double>>;

TYPED_TEST_SUITE(DirectedGraphIOTests, DirectedGraphs);

template <typename T>
class SimpleGraphIOTests : public ::testing::Test
{
  public:
    T graph;
};

using SimpleGraphs =
    ::testing::Types<jGraph::ListGraph<long long>, jGraph::ListGraph<float>,
                     jGraph::MatrixGraph<unsigned>,
                     jGraph::MatrixGraph<double>>;

TYPED_TEST_SUITE(SimpleGraphIOTests, SimpleGraphs);

TYPED_TEST(GraphIOTests, loadDimacsFile)
{
    std::ifstream file("tests/Graphs/DimacsGraphs/valid.dimacs");
    this->graph.loadFromFile(file, jGraph::DIMACS);
    ASSERT_EQ(this->graph.getNumberOfNodes(), 5);
    ASSERT_EQ(this->graph.getNumberOfEdges(), 6);
    ASSERT_TRUE(this->graph.hasEdge({1, 2}));
    ASSERT_TRUE(this->graph.hasEdge({2, 3}));
    ASSERT_FALSE(this->graph.hasEdge({1, 3}));
}

TYPED_TEST(SimpleGraphIOTests, loadFromLargeDimacsFile)
{

    std::ifstream fileStream("tests/Graphs/DimacsGraphs/mediumSize.gr");
    this->graph.loadFromFile(fileStream, jGraph::DIMACS);

    ASSERT_EQ(this->graph.getNumberOfNodes(), 3353);
    ASSERT_EQ(this->graph.getNumberOfEdges(), 4831);
}

TYPED_TEST(DirectedGraphIOTests, loadFromLargeDimacsFile)
{

    std::ifstream fileStream("tests/Graphs/DimacsGraphs/mediumSize.gr");
    this->graph.loadFromFile(fileStream, jGraph::DIMACS);

    ASSERT_EQ(this->graph.getNumberOfNodes(), 3353);
    ASSERT_EQ(this->graph.getNumberOfEdges(), 8859);
}
