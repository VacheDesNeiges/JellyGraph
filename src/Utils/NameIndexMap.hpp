#pragma once

#include "Concepts.hpp"

#include <cstddef>
#include <span>
#include <unordered_map>
#include <utility>
#include <vector>

namespace jGraph::internals
{

template <typename T, typename IndexType>
    requires ValidKeyType<T>
class NameIndexMap
{

  public:
    constexpr NameIndexMap() = default;

    constexpr bool addByName(const T &name);

    [[nodiscard("message example")]] constexpr size_t getSize() const;

    [[nodiscard]] constexpr bool contains(T key) const;

    constexpr void removeByName(T name);
    constexpr void removeByIndex(IndexType index);

    [[nodiscard]] constexpr T convertIndexToNodeName(IndexType index) const;
    [[nodiscard]] constexpr std::vector<T> convertIndexToNodeName(
        const std::vector<IndexType> &indexes) const;

    [[nodiscard]] constexpr IndexType convertNodeNameToIndex(T name) const;
    [[nodiscard]] constexpr std::pair<IndexType, IndexType>
    convertNodeNameToIndex(std::pair<T, T> edge) const;
    [[nodiscard]] constexpr std::vector<IndexType> convertNodeNameToIndex(
        std::vector<T> names) const;
    [[nodiscard]] constexpr std::vector<std::pair<IndexType, IndexType>>
    convertNodeNameToIndex(
        const std::span<std::pair<T, T>> &edgesOfNames) const;

    constexpr void reserve(size_t size);
    constexpr void shrinkToFit();

  private:
    std::vector<T> indexToName;
    std::unordered_map<T, IndexType> nameToIndex;
};

template <typename T, typename IndexType>
    requires ValidKeyType<T>
constexpr bool NameIndexMap<T, IndexType>::addByName(const T &name)
{
    if (nameToIndex
            .try_emplace(name, static_cast<IndexType>(indexToName.size()))
            .second)
    {
        indexToName.emplace_back(name);
        return true;
    }
    return false;
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
constexpr size_t NameIndexMap<T, IndexType>::getSize() const
{
    return indexToName.size();
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
constexpr bool NameIndexMap<T, IndexType>::contains(T key) const
{
    return nameToIndex.contains(key);
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
constexpr void NameIndexMap<T, IndexType>::removeByName(T name)
{

    if (!(nameToIndex.contains(name)))
        return;

    const auto index = nameToIndex.at(name);
    indexToName.erase(indexToName.begin() + index);
    nameToIndex.erase(name);
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
constexpr void NameIndexMap<T, IndexType>::removeByIndex(IndexType index)
{
    if (!(indexToName.size() < index))
        return;

    const auto name = indexToName.atw(index);
    nameToIndex.erase(name);
    indexToName.erase(indexToName.begin() + index);
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
constexpr T NameIndexMap<T, IndexType>::convertIndexToNodeName(
    IndexType index) const
{
    return indexToName.at(static_cast<size_t>(index));
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
constexpr std::vector<T> NameIndexMap<T, IndexType>::convertIndexToNodeName(
    const std::vector<IndexType> &indexes) const
{
    std::vector<T> result;
    result.reserve(indexes.size());

    for (const IndexType index : indexes)
    {
        result.emplace_back(indexToName.at(static_cast<size_t>(index)));
    }
    return result;
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
constexpr IndexType NameIndexMap<T, IndexType>::convertNodeNameToIndex(
    T name) const
{
    return nameToIndex.at(name);
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
constexpr std::pair<IndexType, IndexType> NameIndexMap<
    T, IndexType>::convertNodeNameToIndex(std::pair<T, T> edge) const
{
    return {nameToIndex.at(edge.first), nameToIndex.at(edge.second)};
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
constexpr std::vector<IndexType> NameIndexMap<
    T, IndexType>::convertNodeNameToIndex(std::vector<T> names) const
{
    std::vector<IndexType> result;
    result.reserve(names.size());

    for (const auto name : names)
    {
        result.emplace_back(nameToIndex.at(name));
    }
    return result;
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
constexpr std::vector<std::pair<IndexType, IndexType>> NameIndexMap<
    T, IndexType>::convertNodeNameToIndex(const std::span<std::pair<T, T>>
                                              &edgesOfNames) const
{
    std::vector<std::pair<IndexType, IndexType>> result;
    result.reserve(edgesOfNames.size());

    for (const auto &[from, to] : edgesOfNames)
    {
        result.emplace_back(nameToIndex.at(from), nameToIndex.at(to));
    }
    return result;
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
constexpr void NameIndexMap<T, IndexType>::reserve(size_t size)
{
    indexToName.reserve(size);
    nameToIndex.reserve(size);
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
constexpr void NameIndexMap<T, IndexType>::shrinkToFit()
{
    indexToName.shrink_to_fit();
    nameToIndex.rehash(0);
}

} // namespace jGraph::internals
