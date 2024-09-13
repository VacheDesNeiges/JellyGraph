#include "DirectedMatrixGraph.hpp"
#include "gtest/gtest.h"
#include <algorithm>
#include <cstdint>

template <typename T>
class DirectedGraphPrimitivesTests : public ::testing::Test
{
  public:
    T graph;
};

using DirectedGraphImplementations =
    ::testing::Types<jGraph::DirectedMatrixGraph<unsigned>,
                     jGraph::DirectedMatrixGraph<long long, uint8_t>>;

TYPED_TEST_SUITE(DirectedGraphPrimitivesTests, DirectedGraphImplementations);

TYPED_TEST(DirectedGraphPrimitivesTests, addEdge)
{
    this->graph.addNode(0);
    this->graph.addNode(1);

    ASSERT_EQ(this->graph.getNumberOfEdges(), 0);
    this->graph.addEdge({0, 1});
    ASSERT_EQ(this->graph.getNumberOfEdges(), 1);
    this->graph.addEdge({1, 0});
    ASSERT_EQ(this->graph.getNumberOfEdges(), 2);
}

TYPED_TEST(DirectedGraphPrimitivesTests, removeEdge)
{
    this->graph.addNode(0);
    this->graph.addNode(1);

    this->graph.addEdge({0, 1});
    this->graph.addEdge({1, 0});

    ASSERT_EQ(this->graph.getNumberOfEdges(), 2);
    this->graph.removeEdge({0, 1});
    ASSERT_EQ(this->graph.getNumberOfEdges(), 1);
    const auto edges = this->graph.getEdges();
    using EdgeType = typename decltype(edges)::value_type;
    ASSERT_TRUE(std::ranges::find(edges, EdgeType{1, 0}) != edges.end());
}
