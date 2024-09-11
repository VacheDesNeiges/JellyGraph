#pragma once
#include "GraphPrimitives.hpp"
#include <stack>
#include <utility>
#include <vector>

namespace jGraph
{

template <typename T>
class GraphAlgorithms : public virtual GraphPrimitives<T>
{
  public:
    bool isConnected() const;
    unsigned numberOfComponents() const;
    std::vector<std::vector<T>> components() const;

  private:
    std::vector<std::vector<unsigned>> internal_components() const;
};

template <typename T>
bool GraphAlgorithms<T>::isConnected() const
{
    return components().size() == 1;
}

template <typename T>
std::vector<std::vector<T>> GraphAlgorithms<T>::components() const
{
    std::vector<std::vector<T>> result;
    const auto components = internal_components();
    result.reserve(components.size());
    for (const auto &component : components)
    {
        result.emplace_back(
            GraphPrimitives<T>::getNodeMap().convertIndexToNodeName(component));
    }
    return result;
}

template <typename T>
std::vector<std::vector<unsigned>> GraphAlgorithms<T>::internal_components()
    const
{
    std::vector<std::vector<unsigned>> components;

    const auto nodes = this->internal_getNodes();
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
            for (const auto neighbor : this->internal_getNeighbors(node))
            {
                stack.push(neighbor);
            }
        }
        components.push_back(currentComponent);
    }
    return components;
}

}; // namespace jGraph
