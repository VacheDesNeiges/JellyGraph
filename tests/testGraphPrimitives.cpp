#include "GraphPrimitives.hpp"
#include "ListGraph.hpp"
#include "MatrixGraph.hpp"

#include "gtest/gtest.h"
#include <type_traits>
#include <utility>

template <typename T>
concept hasGraphPrimitives = std::is_base_of_v<jGraph::GraphPrimitives, T>;

template <typename T>
    requires hasGraphPrimitives<T>
class GraphPrimitivesTests : public ::testing::Test
{
  public:
    T graph;

    constexpr static unsigned numNode = 5;
    T numGraph{numNode};
};

using GraphImplementations =
    ::testing::Types<jGraph::MatrixGraph, jGraph::ListGraph>;

TYPED_TEST_SUITE(GraphPrimitivesTests, GraphImplementations);

TYPED_TEST(GraphPrimitivesTests, initEmptyGraph)
{
    ASSERT_EQ(this->graph.getNumberOfNodes(), 0);
}

TYPED_TEST(GraphPrimitivesTests, initWithNumNode)
{
    ASSERT_EQ(this->numGraph.getNumberOfNodes(), this->numNode);
}

TYPED_TEST(GraphPrimitivesTests, addNode)
{
    this->graph.addNode(0);
    ASSERT_EQ(this->graph.getNumberOfNodes(), 1);
    this->graph.addNode(4);
    ASSERT_EQ(this->graph.getNumberOfNodes(), 2);
}

TYPED_TEST(GraphPrimitivesTests, removeNode)
{
    this->graph.addNode(0);
    this->graph.addNode(1);
    this->graph.removeNode(0);
    ASSERT_EQ(this->graph.getNumberOfNodes(), 1);
}

TYPED_TEST(GraphPrimitivesTests, addEdge)
{
    this->graph.addNode(0);
    this->graph.addNode(1);
    this->graph.addNode(2);

    ASSERT_EQ(this->graph.getNumberOfEdges(), 0);
    this->graph.addEdge({0, 1});
    ASSERT_EQ(this->graph.getNumberOfEdges(), 1);
    this->graph.addEdge({1, 2});
    ASSERT_EQ(this->graph.getNumberOfEdges(), 2);
    this->graph.addEdge({1, 2});
    ASSERT_EQ(this->graph.getNumberOfEdges(), 2);
}

TYPED_TEST(GraphPrimitivesTests, hasEdge)
{
    this->graph.addNode(0);
    this->graph.addNode(1);
    this->graph.addNode(2);
    this->graph.addEdge({0, 1});

    ASSERT_TRUE(this->graph.hasEdge({0, 1}));
    ASSERT_TRUE(this->graph.hasEdge({1, 0}));
    ASSERT_FALSE(this->graph.hasEdge({2, 1}));
}

TYPED_TEST(GraphPrimitivesTests, removeEdge)
{
    this->graph.addNode(0);
    this->graph.addNode(1);
    this->graph.addEdge({0, 1});

    ASSERT_EQ(this->graph.getNumberOfEdges(), 1);
    this->graph.removeEdge({1, 0});
    ASSERT_EQ(this->graph.getNumberOfEdges(), 0);

    ASSERT_FALSE(this->graph.hasEdge({0, 1}));
    ASSERT_FALSE(this->graph.hasEdge({1, 0}));
}

TYPED_TEST(GraphPrimitivesTests, addingEdgeCanAddNode)
{
    this->graph.addEdge({0, 1});
    ASSERT_EQ(this->graph.getNumberOfNodes(), 2);
    this->graph.addEdge({1, 2});
    ASSERT_EQ(this->graph.getNumberOfNodes(), 3);
    this->graph.addEdge({0, 2});
    ASSERT_EQ(this->graph.getNumberOfNodes(), 3);
}

TYPED_TEST(GraphPrimitivesTests, getNodes)
{
    this->graph.addNode(1);
    this->graph.addEdge({4, 3});
    const auto vec = this->graph.getNodes();
    ASSERT_EQ(vec.size(), 3);
    ASSERT_TRUE(std::ranges::find(vec, 1) != vec.end());
    ASSERT_TRUE(std::ranges::find(vec, 3) != vec.end());
    ASSERT_TRUE(std::ranges::find(vec, 4) != vec.end());
}

TYPED_TEST(GraphPrimitivesTests, getEdges)
{

    this->graph.addEdge({2, 4});
    this->graph.addEdge({4, 1});
    this->graph.addEdge({3, 1});

    const auto vec = this->graph.getEdges();

    ASSERT_EQ(vec.size(), 3);
    ASSERT_TRUE(std::ranges::find(vec, std::make_pair(2U, 4U)) != vec.end());
    ASSERT_TRUE(std::ranges::find(vec, std::make_pair(1U, 4U)) != vec.end());
    ASSERT_TRUE(std::ranges::find(vec, std::make_pair(1U, 3U)) != vec.end());
    ASSERT_FALSE(std::ranges::find(vec, std::make_pair(3U, 1U)) != vec.end());
}

TYPED_TEST(GraphPrimitivesTests, getNeighbors)
{
    this->graph.addEdge({2, 4});
    this->graph.addEdge({4, 1});

    const auto neighborsOf4 = this->graph.getNeighbors(4);
    const auto neighborsOf2 = this->graph.getNeighbors(2);

    ASSERT_EQ(neighborsOf4.size(), 2);
    ASSERT_EQ(neighborsOf2.size(), 1);

    ASSERT_TRUE(std::ranges::find(neighborsOf4, 2) != neighborsOf4.end());
    ASSERT_TRUE(std::ranges::find(neighborsOf4, 1) != neighborsOf4.end());

    ASSERT_TRUE(std::ranges::find(neighborsOf2, 4) != neighborsOf2.end());
    ASSERT_FALSE(std::ranges::find(neighborsOf2, 1) != neighborsOf2.end());
}
