#include "DirectedMatrixGraph.hpp"
#include "ListGraph.hpp"
#include "MatrixGraph.hpp"

#include "gtest/gtest.h"

#include <algorithm>
#include <type_traits>
#include <utility>
#include <vector>

template <typename T>
class GraphPrimitivesTests : public ::testing::Test
{
  public:
    T graph;
};

template <typename T>
class SimpleGraphPrimitivesTests : public ::testing::Test
{
  public:
    T graph;
};

template <typename T>
class DirectedGraphPrimitivesTests : public ::testing::Test
{
  public:
    T graph;
};

using AllGraphs = ::testing::Types<
    jGraph::MatrixGraph<unsigned, short>, jGraph::MatrixGraph<long long>,
    jGraph::ListGraph<unsigned>, jGraph::ListGraph<long long, short>,
    jGraph::DirectedMatrixGraph<unsigned>,
    jGraph::DirectedMatrixGraph<long long, short>>;

using SimpleGraphs = ::testing::Types<
    jGraph::MatrixGraph<unsigned, short>, jGraph::MatrixGraph<long long>,
    jGraph::ListGraph<unsigned>, jGraph::ListGraph<long long, short>>;

using DirectedGraphs =
    ::testing::Types<jGraph::DirectedMatrixGraph<unsigned, short>,
                     jGraph::DirectedMatrixGraph<long long>>;

TYPED_TEST_SUITE(GraphPrimitivesTests, AllGraphs);
TYPED_TEST_SUITE(SimpleGraphPrimitivesTests, SimpleGraphs);
TYPED_TEST_SUITE(DirectedGraphPrimitivesTests, DirectedGraphs);

TYPED_TEST(GraphPrimitivesTests, initEmptyGraph)
{
    ASSERT_EQ(this->graph.getNumberOfNodes(), 0);
}

TYPED_TEST(GraphPrimitivesTests, addNode)
{
    this->graph.addNode(0);
    ASSERT_EQ(this->graph.getNumberOfNodes(), 1);
    this->graph.addNode(4);
    ASSERT_EQ(this->graph.getNumberOfNodes(), 2);
}

TYPED_TEST(GraphPrimitivesTests, addNodeFromSpan)
{
    this->graph.addNode(0);
    using nodeType =
        typename std::remove_reference_t<decltype(this->graph.getNodes().at(
            0))>;
    std::vector<nodeType> newNodes{0, 1, 2, 3, 4};

    this->graph.addNode(newNodes);
    ASSERT_EQ(this->graph.getNumberOfNodes(), 5);
}

TYPED_TEST(GraphPrimitivesTests, removeNode)
{
    this->graph.addNode(0);
    this->graph.addNode(1);
    this->graph.removeNode(0);
    ASSERT_EQ(this->graph.getNumberOfNodes(), 1);
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

TYPED_TEST(GraphPrimitivesTests, addEdgeFromSpan)
{
    this->graph.addNode(0);
    this->graph.addNode(1);
    this->graph.addNode(2);

    this->graph.addEdge({0, 1});

    ASSERT_EQ(this->graph.getNumberOfEdges(), 1);
    using nodeType =
        typename std::remove_reference_t<decltype(this->graph.getNodes().at(
            0))>;
    std::vector<std::pair<nodeType, nodeType>> newEdges{{0, 1}, {2, 3}};

    this->graph.addEdge(newEdges);
    ASSERT_EQ(this->graph.getNumberOfNodes(), 4);
    ASSERT_EQ(this->graph.getNumberOfEdges(), 2);
}

TYPED_TEST(SimpleGraphPrimitivesTests, hasEdge)
{
    this->graph.addNode(0);
    this->graph.addNode(1);
    this->graph.addNode(2);
    this->graph.addEdge({0, 1});

    ASSERT_TRUE(this->graph.hasEdge({0, 1}));
    ASSERT_TRUE(this->graph.hasEdge({1, 0}));
    ASSERT_FALSE(this->graph.hasEdge({2, 1}));
}

TYPED_TEST(DirectedGraphPrimitivesTests, hasEdge)
{
    this->graph.addEdge({0, 1});
    this->graph.addNode(2);
    ASSERT_TRUE(this->graph.hasEdge({0, 1}));
    ASSERT_FALSE(this->graph.hasEdge({1, 0}));
    ASSERT_FALSE(this->graph.hasEdge({0, 2}));
}

TYPED_TEST(SimpleGraphPrimitivesTests, removeEdge)
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

TYPED_TEST(DirectedGraphPrimitivesTests, removeEdge)
{
    this->graph.addEdge({0, 1});
    this->graph.addEdge({1, 0});
    ASSERT_EQ(this->graph.getNumberOfEdges(), 2);
    this->graph.removeEdge({1, 0});
    ASSERT_EQ(this->graph.getNumberOfEdges(), 1);
    ASSERT_FALSE(this->graph.hasEdge({1, 0}));
    ASSERT_TRUE(this->graph.hasEdge({0, 1}));
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

TYPED_TEST(GraphPrimitivesTests, removingNodeCorrectlyUpdatesEdgeNumber)
{
    this->graph.addEdge({0, 1});
    this->graph.addEdge({1, 2});
    this->graph.removeNode(1);

    ASSERT_EQ(this->graph.getNumberOfEdges(), 0);
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

TYPED_TEST(SimpleGraphPrimitivesTests, getEdges)
{

    this->graph.addEdge({2, 4});
    this->graph.addEdge({4, 1});
    this->graph.addEdge({3, 1});

    const auto vec = this->graph.getEdges();

    ASSERT_EQ(vec.size(), 3);

    using EdgeType = typename decltype(vec)::value_type;

    ASSERT_TRUE(std::ranges::find(vec, EdgeType{2, 4}) != vec.end());
    ASSERT_TRUE(std::ranges::find(vec, EdgeType{1, 4}) != vec.end());
    ASSERT_TRUE(std::ranges::find(vec, EdgeType{1, 3}) != vec.end());
    ASSERT_FALSE(std::ranges::find(vec, EdgeType{3, 1}) != vec.end());
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
