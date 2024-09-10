#include "GraphAlgorithms.hpp"
#include <stack>
#include <vector>

bool GraphAlgorithms::isConnected() const
{
    return components().size() == 1;
}

std::vector<std::vector<unsigned>> GraphAlgorithms::components() const
{
    std::vector<std::vector<unsigned>> result;
    const auto components = internal_components();
    result.reserve(components.size());
    for (const auto &component : components)
    {
        result.emplace_back(getNodeMap().convertIndexToNodeName(component));
    }
    return result;
}

std::vector<std::vector<unsigned>> GraphAlgorithms::internal_components() const
{
    std::vector<std::vector<unsigned>> components;

    const auto nodes = internal_getNodes();
    std::vector<bool> visited(nodes.size(), false);
    std::vector<unsigned> currentComponent;

    std::vector<unsigned> underlyingStack;
    underlyingStack.reserve(nodes.size());
    std::stack<unsigned, std::vector<unsigned>> stack(
        std::move(underlyingStack));

    for (const auto nodeIndex : nodes)
    {
        if (visited.at(nodeIndex))
            continue;

        currentComponent.clear();
        stack.push(nodeIndex);
        while (!stack.empty())
        {
            const auto node = stack.top();
            stack.pop();
            if (visited.at(node))
                continue;

            visited.at(node) = true;
            currentComponent.push_back(node);
            for (const auto neighbor : internal_getNeighbors(node))
            {
                stack.push(neighbor);
            }
        }
        components.push_back(currentComponent);
    }
    return components;
}
