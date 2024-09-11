#include "ListGraph.hpp"
#include "MatrixGraph.hpp"

#include "gtest/gtest.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <utility>
#include <vector>

template <typename T>
class GraphAlgorithmsTests : public ::testing::Test
{
  public:
    T graph;
};

using GraphImplementations =
    ::testing::Types<jGraph::MatrixGraph<unsigned>,
                     jGraph::MatrixGraph<long long>,
                     jGraph::ListGraph<unsigned>, jGraph::ListGraph<long long>>;

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

    const std::array<std::pair<const unsigned, const unsigned>, 5> edges{
        {{0, 1}, {1, 2}, {0, 3}, {4, 5}, {6, 7}}};

    for (const auto &edge : edges)
    {
        this->graph.addEdge(edge);
    }

    const auto components = this->graph.components();
    ASSERT_EQ(components.size(), 3);

    const std::vector<std::vector<unsigned>> expectedComponents = {
        {0, 1, 2, 3}, {4, 5}, {6, 7}};

    for (const auto &expectedComponent : expectedComponents)
    {
        auto it = std::ranges::find_if(
            components, [&expectedComponent](const auto &vec) {
                return std::ranges::is_permutation(vec, expectedComponent);
            });
        ASSERT_TRUE(it != components.end());
    }
}
