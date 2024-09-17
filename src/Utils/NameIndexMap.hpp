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
    constexpr NameIndexMap() = default;

    constexpr bool addByName(T name);

    [[nodiscard]] constexpr size_t getSize() const;

    [[nodiscard]] constexpr bool contains(T key) const;

    constexpr void removeByName(T name);
    constexpr void removeByIndex(IndexType index);

    [[nodiscard]] constexpr T convertIndexToNodeName(IndexType index) const;
    [[nodiscard]] constexpr std::vector<T> convertIndexToNodeName(
        const std::vector<IndexType> &indexes) const;

    [[nodiscard]] constexpr IndexType convertNodeNameToIndex(T name) const;
    [[nodiscard]] constexpr std::vector<IndexType> convertNodeNameToIndex(
        std::vector<T> names) const;

    constexpr void reserve(size_t size);

  private:
    std::vector<T> indexToName;
    std::unordered_map<T, IndexType> nameToIndex;
};

template <typename T, typename IndexType>
    requires ValidKeyType<T>
constexpr bool NameIndexMap<T, IndexType>::addByName(T name)
{
    if (nameToIndex.contains(name))
        return false;

    nameToIndex[name] = static_cast<IndexType>(indexToName.size());
    indexToName.emplace_back(name);
    return true;
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
constexpr std::vector<IndexType> NameIndexMap<
    T, IndexType>::convertNodeNameToIndex(std::vector<T> names) const
{
    std::vector<IndexType> result;
    result.reserve(names.size());

    for (const auto name : names)
    {
        result.emplace_back(nameToIndex(name));
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

} // namespace jGraph::internals
