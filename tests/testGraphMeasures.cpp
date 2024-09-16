#include "DirectedListGraph.hpp"
#include "DirectedMatrixGraph.hpp"
#include "ListGraph.hpp"
#include "MatrixGraph.hpp"
#include "gtest/gtest.h"

template <typename T>
class GraphMeasuresTests : public ::testing::Test
{
  public:
    T graph;
};
using AllGraphs = ::testing::Types<
    jGraph::MatrixGraph<unsigned>, jGraph::MatrixGraph<double>,
    jGraph::ListGraph<unsigned>, jGraph::ListGraph<double>,
    jGraph::DirectedMatrixGraph<unsigned>, jGraph::DirectedMatrixGraph<double>,
    jGraph::DirectedListGraph<unsigned>, jGraph::DirectedListGraph<double>>;
TYPED_TEST_SUITE(GraphMeasuresTests, AllGraphs);

template <typename T>
class SimpleGraphMeasuresTests : public ::testing::Test
{
  public:
    T graph;
};
using SimpleGraphs = ::testing::Types<
    jGraph::MatrixGraph<unsigned, short>, jGraph::MatrixGraph<long long>,
    jGraph::ListGraph<unsigned>, jGraph::ListGraph<long long, short>>;
TYPED_TEST_SUITE(SimpleGraphMeasuresTests, SimpleGraphs);

template <typename T>
class DirectedGraphMeasuresTests : public ::testing::Test
{
  public:
    T graph;
};
using DirectedGraphs =
    ::testing::Types<jGraph::DirectedMatrixGraph<unsigned, short>,
                     jGraph::DirectedMatrixGraph<long long>,
                     jGraph::DirectedListGraph<unsigned, short>,
                     jGraph::DirectedListGraph<long long>>;
TYPED_TEST_SUITE(DirectedGraphMeasuresTests, DirectedGraphs);

TYPED_TEST(GraphMeasuresTests, degree)
{
    this->graph.addEdge({0, 1});
    this->graph.addEdge({0, 2});
    this->graph.addEdge({0, 3});
    this->graph.addNode(4);

    ASSERT_EQ(this->graph.degree(0), 3);
    ASSERT_EQ(this->graph.degree(1), 1);
    ASSERT_EQ(this->graph.degree(2), 1);
    ASSERT_EQ(this->graph.degree(3), 1);
    ASSERT_EQ(this->graph.degree(4), 0);
}

TYPED_TEST(GraphMeasuresTests, averageNeighborDegree)
{
    this->graph.addEdge({0, 1});
    this->graph.addEdge({2, 3});
    ASSERT_EQ(this->graph.averageNeighborDegree(), 1);
}

TYPED_TEST(SimpleGraphMeasuresTests, density)
{
    ASSERT_EQ(this->graph.density(), 0);

    this->graph.addNode(0);
    ASSERT_EQ(this->graph.density(), 0);
    this->graph.addNode(1);
    ASSERT_EQ(this->graph.density(), 0);

    this->graph.addEdge({0, 1});
    ASSERT_EQ(this->graph.density(), 1);

    this->graph.addEdge({2, 3});
    this->graph.addEdge({0, 3});
    ASSERT_EQ(this->graph.density(), 0.5);
}

TYPED_TEST(DirectedGraphMeasuresTests, density)
{
    ASSERT_EQ(this->graph.density(), 0);
    this->graph.addNode(0);
    ASSERT_EQ(this->graph.density(), 0);
    this->graph.addEdge({0, 1});
    ASSERT_EQ(this->graph.density(), 0.5);
    this->graph.addEdge({1, 0});
    ASSERT_EQ(this->graph.density(), 1);
    this->graph.addEdge({2, 0});
    ASSERT_EQ(this->graph.density(), 0.5);
}
