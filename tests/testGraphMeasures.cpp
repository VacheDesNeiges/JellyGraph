#include "ListGraph.hpp"
#include "MatrixGraph.hpp"
#include "gtest/gtest.h"

template <typename T>
class GraphMeasuresTests : public ::testing::Test
{
  public:
    T graph;
};

using GraphImplementations =
    ::testing::Types<jGraph::MatrixGraph<unsigned>,
                     jGraph::MatrixGraph<long long>,
                     jGraph::ListGraph<unsigned>, jGraph::ListGraph<long long>>;

TYPED_TEST_SUITE(GraphMeasuresTests, GraphImplementations);

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
