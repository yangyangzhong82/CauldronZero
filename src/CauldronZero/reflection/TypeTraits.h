#pragma once

#include <map>
#include <vector>
#include <type_traits>

namespace CauldronZero::reflection::detail
{
    // 类型特征，用于检测是否为 std::vector
    template <typename>
    struct is_vector : std::false_type
    {
    };
    template <typename T, typename A>
    struct is_vector<std::vector<T, A>> : std::true_type
    {
    };

    // 类型特征，用于检测是否为 std::map
    template <typename>
    struct is_map : std::false_type
    {
    };
    template <typename K, typename V, typename C, typename A>
    struct is_map<std::map<K, V, C, A>> : std::true_type
    {
    };

} // namespace CauldronZero::reflection::detail
