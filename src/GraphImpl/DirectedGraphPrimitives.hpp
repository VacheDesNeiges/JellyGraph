#pragma once

#include <vector>

namespace jGraph
{

template <typename T>
class DirectedGraphPrimitives
{
  public:
    DirectedGraphPrimitives() = default;
    virtual ~DirectedGraphPrimitives() = default;

    DirectedGraphPrimitives(const DirectedGraphPrimitives &) = delete;
    DirectedGraphPrimitives(DirectedGraphPrimitives &&) = delete;
    DirectedGraphPrimitives &operator=(const DirectedGraphPrimitives &) =
        delete;
    DirectedGraphPrimitives &operator=(DirectedGraphPrimitives &&) = delete;

    virtual std::vector<T> getOutgoingNeighbors(T) const = 0;
    virtual std::vector<T> getIngoingNeighbors(T) const = 0;

  protected:
    virtual std::vector<unsigned> internal_getOutgoingNeighbors(
        unsigned) const = 0;
    virtual std::vector<unsigned> internal_getIngoingNeighbors(
        unsigned) const = 0;
};

} // namespace jGraph
