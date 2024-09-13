#include "DirectedMatrixGraph.hpp"
#include "gtest/gtest.h"
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
