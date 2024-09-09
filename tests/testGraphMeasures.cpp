#include "GraphMeasures.hpp"
#include "ListGraph.hpp"
#include "MatrixGraph.hpp"
#include "gtest/gtest.h"
#include <type_traits>

template <typename T>
concept hasGraphMeasures = std::is_base_of_v<GraphMeasures, T>;

template <typename T>
    requires hasGraphMeasures<T>

class GraphMeasuresTests : public ::testing::Test
{
  public:
    T graph;
};

using GraphImplementations =
    ::testing::Types<jGraph::MatrixGraph, jGraph::ListGraph>;

TYPED_TEST_SUITE(GraphMeasuresTests, GraphImplementations);

TYPED_TEST(GraphMeasuresTests, averageNeighborDegree)
{
    this->graph.addEdge({0, 1});
    this->graph.addEdge({2, 3});
    ASSERT_EQ(this->graph.averageNeighborDegree(), 1);
}
