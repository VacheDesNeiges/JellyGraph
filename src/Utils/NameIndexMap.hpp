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
    { keyType <=> keyType } -> std::convertible_to<std::strong_ordering>;
};

template <typename T>
    requires ValidKeyType<T>

class NameIndexMap
{

  public:
    bool addByName(T name)
    {
        if (nameToIndex.contains(name))
            return false;

        nameToIndex[name] = static_cast<unsigned>(indexToName.size());
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

    void removeByIndex(unsigned index)
    {
        if (!(indexToName.size() < index))
            return;

        const auto name = indexToName.atw(index);
        nameToIndex.erase(name);
        indexToName.erase(indexToName.begin() + index);
    }

    T getByIndex(unsigned index) const
    {
        return indexToName.at(index);
    }

    unsigned getByName(T name) const
    {
        return nameToIndex.at(name);
    }

    void reserve(size_t size)
    {
        indexToName.reserve(size);
        nameToIndex.reserve(size);
    }

  private:
    std::vector<T> indexToName;
    std::unordered_map<T, unsigned> nameToIndex;
};

} // namespace jGraph::internals
