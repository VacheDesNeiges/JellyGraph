#include "GraphAlgorithms.hpp"
#include "ListGraph.hpp"
#include "MatrixGraph.hpp"
#include "gtest/gtest.h"
#include <cassert>
#include <type_traits>

template <typename T>
concept hasGraphAlgorithms = std::is_base_of_v<GraphAlgorithms, T>;

template <typename T>
    requires hasGraphAlgorithms<T>

class GraphAlgorithmsTests : public ::testing::Test
{
  public:
    T graph;
};

using GraphImplementations =
    ::testing::Types<jGraph::MatrixGraph, jGraph::ListGraph>;

TYPED_TEST_SUITE(GraphAlgorithmsTests, GraphImplementations);

TYPED_TEST(GraphAlgorithmsTests, isConnected)
{
    this->graph.addEdge({0, 1});
    this->graph.addEdge({1, 2});
    ASSERT_TRUE(this->graph.isConnected());
    this->graph.addNode(3);
    ASSERT_FALSE(this->graph.isConnected());
}

TYPED_TEST(GraphAlgorithmsTests, connectedComponents)
{
    ASSERT_EQ(this->graph.components().size(), 0);
    this->graph.addEdge({0, 1});
    this->graph.addEdge({1, 2});
    this->graph.addEdge({0, 3});
    this->graph.addEdge({4, 5});
    this->graph.addEdge({6, 7});

    const auto components = this->graph.components();
    ASSERT_EQ(components.size(), 3);
}
