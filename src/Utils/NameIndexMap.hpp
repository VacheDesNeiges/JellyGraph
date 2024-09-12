#pragma once

#include <compare>
#include <concepts>
#include <cstddef>
#include <functional>
#include <unordered_map>
#include <vector>

namespace jGraph::internals
{

template <typename T>
concept ValidKeyType = std::copyable<T> && requires(T keyType) {
    { std::hash<T>{}(keyType) } -> std::convertible_to<std::size_t>;
    { keyType <=> keyType } -> std::convertible_to<std::partial_ordering>;
};

template <typename T, typename IndexType>
    requires ValidKeyType<T>
class NameIndexMap
{

  public:
    bool addByName(T name)
    {
        if (nameToIndex.contains(name))
            return false;

        nameToIndex[name] = static_cast<IndexType>(indexToName.size());
        indexToName.emplace_back(name);
        return true;
    }

    size_t getSize() const
    {
        return indexToName.size();
    }

    bool contains(T key) const
    {
        return nameToIndex.contains(key);
    }

    void removeByName(T name)
    {

        if (!(nameToIndex.contains(name)))
            return;

        const auto index = nameToIndex.at(name);
        indexToName.erase(indexToName.begin() + index);
        nameToIndex.erase(name);
    }

    void removeByIndex(IndexType index)
    {
        if (!(indexToName.size() < index))
            return;

        const auto name = indexToName.atw(index);
        nameToIndex.erase(name);
        indexToName.erase(indexToName.begin() + index);
    }

    T convertIndexToNodeName(IndexType index) const
    {
        return indexToName.at(index);
    }

    std::vector<T> convertIndexToNodeName(
        const std::vector<IndexType> &indexes) const
    {
        std::vector<T> result;
        result.reserve(indexes.size());

        for (const IndexType index : indexes)
        {
            result.emplace_back(indexToName.at(index));
        }
        return result;
    }

    IndexType convertNodeNameToIndex(T name) const
    {
        return nameToIndex.at(name);
    }

    std::vector<IndexType> convertNodeNameToIndex(std::vector<T> names) const
    {
        std::vector<IndexType> result;
        result.reserve(names.size());

        for (const auto name : names)
        {
            result.emplace_back(nameToIndex(name));
        }
        return result;
    }

    void reserve(size_t size)
    {
        indexToName.reserve(size);
        nameToIndex.reserve(size);
    }

  private:
    std::vector<T> indexToName;
    std::unordered_map<T, IndexType> nameToIndex;
};

} // namespace jGraph::internals
