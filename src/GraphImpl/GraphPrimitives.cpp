#include "GraphPrimitives.hpp"
#include "NameIndexMap.hpp"

namespace jGraph
{

jGraph::internals::NameIndexMap<unsigned> &GraphPrimitives::getNodeMap()
{
    return nodeMap;
}

const jGraph::internals::NameIndexMap<unsigned> &GraphPrimitives::getNodeMap()
    const
{
    return nodeMap;
}

} // namespace jGraph
