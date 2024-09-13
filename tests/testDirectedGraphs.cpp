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

TYPED_TEST(DirectedGraphPrimitivesTests, hasEdge)
{
    this->graph.addEdge({0, 1});
    ASSERT_TRUE(this->graph.hasEdge({0, 1}));
    ASSERT_FALSE(this->graph.hasEdge({1, 0}));
}

TYPED_TEST(DirectedGraphPrimitivesTests, getEdges)
{
    this->graph.addEdge({0, 1});
    this->graph.addEdge({1, 2});
    this->graph.addEdge({4, 3});

    const auto edges = this->graph.getEdges();

    ASSERT_EQ(edges.size(), 3);

    using EdgeType = typename decltype(edges)::value_type;
    ASSERT_TRUE(std::ranges::find(edges, EdgeType{0, 1}) != edges.end());
    ASSERT_TRUE(std::ranges::find(edges, EdgeType{1, 2}) != edges.end());
    ASSERT_TRUE(std::ranges::find(edges, EdgeType{4, 3}) != edges.end());
    ASSERT_FALSE(std::ranges::find(edges, EdgeType{3, 4}) != edges.end());
}

TYPED_TEST(DirectedGraphPrimitivesTests, removeNodeWithEdges)
{
    this->graph.addEdge({0, 1});
    this->graph.addEdge({1, 2});
    this->graph.addEdge({0, 2});

    ASSERT_EQ(this->graph.getNumberOfEdges(), 3);
    this->graph.removeNode(2);
    ASSERT_EQ(this->graph.getNumberOfEdges(), 1);
}

TYPED_TEST(DirectedGraphPrimitivesTests, IngoingNeighbors)
{
    this->graph.addEdge({0, 1});
    this->graph.addEdge({1, 2});
    ASSERT_EQ(this->graph.getIngoingNeighbors(1).size(), 1);
}
TYPED_TEST(DirectedGraphPrimitivesTests, outgoingNeighbors)
{
    this->graph.addEdge({0, 1});
    this->graph.addEdge({1, 2});
    ASSERT_EQ(this->graph.getOutgoingNeighbors(1).size(), 1);
}
