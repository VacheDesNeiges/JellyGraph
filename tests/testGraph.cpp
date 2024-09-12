#include "DirectedMatrixGraph.hpp"
#include "ListGraph.hpp"
#include "MatrixGraph.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <string>

TEST(GraphConstructors, rangeOfNodes)
{
    const std::string initString = "Hello world!!";
    const std::array<char, 9> charsInString{'H', 'e', 'l', 'o', ' ',
                                            'w', 'r', 'd', '!'};

    const jGraph::MatrixGraph<char> g1(initString);
    const jGraph::ListGraph<char> g2(initString);

    const auto g1Nodes = g1.getNodes();
    const auto g2Nodes = g2.getNodes();

    ASSERT_EQ(g1Nodes.size(), 9);
    ASSERT_EQ(g2Nodes.size(), 9);

    ASSERT_TRUE(std::ranges::is_permutation(g1Nodes, charsInString));
    ASSERT_TRUE(std::ranges::is_permutation(g2Nodes, charsInString));
}
