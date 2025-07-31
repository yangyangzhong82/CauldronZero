#pragma once

#include "Reflection.h"
#include <nlohmann/json.hpp>
#include <optional>

#include "TypeTraits.h"

namespace CauldronZero::reflection
{

    // 前向声明
    template <typename T>
    std::optional<nlohmann::json> serialize(const T& obj);

    namespace detail
    {
        // 辅助函数，用于序列化单个成员
        template <typename T>
        void serialize_member(nlohmann::json& j, const T& obj, const char* name, const auto& member_ptr)
        {
            using MemberType = std::remove_cvref_t<decltype(obj.*member_ptr)>;

            if constexpr (Reflectable<MemberType>)
            {
                // 递归序列化嵌套的 Reflectable 对象
                j[name] = serialize(obj.*member_ptr).value_or(nlohmann::json{});
            }
            else if constexpr (detail::is_vector<MemberType>::value)
            {
                // 处理 vector
                nlohmann::json arr = nlohmann::json::array();
                for (const auto& item : obj.*member_ptr) {
                    if constexpr (Reflectable<std::remove_cvref_t<decltype(item)>>) {
                        arr.push_back(serialize(item).value_or(nlohmann::json{}));
                    } else {
                        arr.push_back(item);
                    }
                }
                j[name] = arr;
            }
            else if constexpr (detail::is_map<MemberType>::value)
            {
                // 处理 map
                nlohmann::json map_obj = nlohmann::json::object();
                for (const auto& [key, value] : obj.*member_ptr) {
                    if constexpr (Reflectable<std::remove_cvref_t<decltype(value)>>) {
                        map_obj[key] = serialize(value).value_or(nlohmann::json{});
                    } else {
                        map_obj[key] = value;
                    }
                }
                j[name] = map_obj;
            }
            else
            {
                // 处理普通类型
                j[name] = obj.*member_ptr;
            }
        }

        // 遍历元组并序列化每个成员
        template <typename T, std::size_t... I>
        void serialize_members(nlohmann::json& j, const T& obj, std::index_sequence<I...>)
        {
            (serialize_member(j, obj, std::get<I>(Members<T>::value).name, std::get<I>(Members<T>::value).ptr), ...);
        }
    } // namespace detail

    // 公共的 serialize 函数
    template <typename T>
    std::optional<nlohmann::json> serialize(const T& obj)
    {
        if constexpr (Reflectable<T>)
        {
            nlohmann::json j = nlohmann::json::object();
            constexpr auto members = Members<T>::value;
            detail::serialize_members(j, obj, std::make_index_sequence<std::tuple_size_v<decltype(members)>>{});
            return j;
        }
        else
        {
            return std::nullopt; // 不是 Reflectable 类型
        }
    }

} // namespace CauldronZero::reflection
