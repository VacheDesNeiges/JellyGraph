#include "gtest/gtest.h"

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

using AllGraphs =
    ::testing::Types<jGraph::MatrixGraph<unsigned>,
                     jGraph::MatrixGraph<long long>,
                     jGraph::ListGraph<unsigned>, jGraph::ListGraph<long long>>;

TYPED_TEST_SUITE(GraphIOTests, AllGraphs);

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
