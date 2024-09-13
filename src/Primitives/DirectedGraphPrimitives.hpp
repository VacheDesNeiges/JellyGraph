#pragma once

#include "UnderlyingIndexType.hpp"
#include <vector>

namespace jGraph
{

template <typename T, typename IndexType = internals::underlyingGraphIndex_t>
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

    [[nodiscard]] virtual std::vector<T> getOutgoingNeighbors(T) const = 0;
    [[nodiscard]] virtual std::vector<T> getIngoingNeighbors(T) const = 0;

  protected:
    [[nodiscard]] virtual std::vector<IndexType> internal_getOutgoingNeighbors(
        IndexType) const = 0;
    [[nodiscard]] virtual std::vector<IndexType> internal_getIngoingNeighbors(
        IndexType) const = 0;
};

} // namespace jGraph
