#include "ListGraph.hpp"
#include "MatrixGraph.hpp"
#include <algorithm>
#include <array>
#include <gtest/gtest.h>
#include <string>

TEST(GraphConstructors, rangeOfNodes)
{
    const std::string initString = "Hello world!!";
    const std::array<char, 9> charsInString{'H', 'e', 'l', 'o', ' ',
                                            'w', 'r', 'd', '!'};

    jGraph::MatrixGraph<char> g1(initString);
    jGraph::ListGraph<char> g2(initString);

    const auto g1Nodes = g1.getNodes();
    const auto g2Nodes = g2.getNodes();

    ASSERT_EQ(g1Nodes.size(), 9);
    ASSERT_EQ(g2Nodes.size(), 9);

    ASSERT_TRUE(std::ranges::is_permutation(g1Nodes, charsInString));
    ASSERT_TRUE(std::ranges::is_permutation(g2Nodes, charsInString));
}
