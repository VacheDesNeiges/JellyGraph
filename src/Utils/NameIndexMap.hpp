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
    bool addByName(T name);

    [[nodiscard]] size_t getSize() const;

    [[nodiscard]] bool contains(T key) const;

    void removeByName(T name);
    void removeByIndex(IndexType index);

    [[nodiscard]] T convertIndexToNodeName(IndexType index) const;
    [[nodiscard]] std::vector<T> convertIndexToNodeName(
        const std::vector<IndexType> &indexes) const;

    [[nodiscard]] IndexType convertNodeNameToIndex(T name) const;
    [[nodiscard]] std::vector<IndexType> convertNodeNameToIndex(
        std::vector<T> names) const;

    void reserve(size_t size);

  private:
    std::vector<T> indexToName;
    std::unordered_map<T, IndexType> nameToIndex;
};

template <typename T, typename IndexType>
    requires ValidKeyType<T>
bool NameIndexMap<T, IndexType>::addByName(T name)
{
    if (nameToIndex.contains(name))
        return false;

    nameToIndex[name] = static_cast<IndexType>(indexToName.size());
    indexToName.emplace_back(name);
    return true;
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
size_t NameIndexMap<T, IndexType>::getSize() const
{
    return indexToName.size();
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
bool NameIndexMap<T, IndexType>::contains(T key) const
{
    return nameToIndex.contains(key);
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
void NameIndexMap<T, IndexType>::removeByName(T name)
{

    if (!(nameToIndex.contains(name)))
        return;

    const auto index = nameToIndex.at(name);
    indexToName.erase(indexToName.begin() + index);
    nameToIndex.erase(name);
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
void NameIndexMap<T, IndexType>::removeByIndex(IndexType index)
{
    if (!(indexToName.size() < index))
        return;

    const auto name = indexToName.atw(index);
    nameToIndex.erase(name);
    indexToName.erase(indexToName.begin() + index);
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
T NameIndexMap<T, IndexType>::convertIndexToNodeName(IndexType index) const
{
    return indexToName.at(index);
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
std::vector<T> NameIndexMap<T, IndexType>::convertIndexToNodeName(
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

template <typename T, typename IndexType>
    requires ValidKeyType<T>
IndexType NameIndexMap<T, IndexType>::convertNodeNameToIndex(T name) const
{
    return nameToIndex.at(name);
}

template <typename T, typename IndexType>
    requires ValidKeyType<T>
std::vector<IndexType> NameIndexMap<T, IndexType>::convertNodeNameToIndex(
    std::vector<T> names) const
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
void NameIndexMap<T, IndexType>::reserve(size_t size)
{
    indexToName.reserve(size);
    nameToIndex.reserve(size);
}

} // namespace jGraph::internals
