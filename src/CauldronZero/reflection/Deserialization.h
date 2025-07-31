#pragma once

#include "Reflection.h"
#include <nlohmann/json.hpp>
#include <optional>

namespace CauldronZero::reflection
{

    // 前向声明
    template <typename T>
    bool deserialize(T& obj, const nlohmann::json& j);

    namespace detail
    {
        // 辅助函数，用于反序列化单个成员
        template <typename T>
        void deserialize_member(T& obj, const nlohmann::json& j, const char* name, auto member_ptr)
        {
            if (j.contains(name))
            {
                using MemberType = std::remove_cvref_t<decltype(obj.*member_ptr)>;
                if constexpr (Reflectable<MemberType>)
                {
                    // 递归反序列化嵌套的 Reflectable 对象
                    deserialize(obj.*member_ptr, j.at(name));
                }
                else
                {
                    // 使用 nlohmann::json 的 get_to 以处理类型转换
                    j.at(name).get_to(obj.*member_ptr);
                }
            }
        }

        // 遍历元组并反序列化每个成员
        template <typename T, std::size_t... I>
        void deserialize_members(T& obj, const nlohmann::json& j, std::index_sequence<I...>)
        {
            (deserialize_member(obj, j, std::get<I>(Members<T>::value).name, std::get<I>(Members<T>::value).ptr), ...);
        }
    } // namespace detail

    // 公共的 deserialize 函数
    template <typename T>
    bool deserialize(T& obj, const nlohmann::json& j)
    {
        if constexpr (Reflectable<T>)
        {
            if (!j.is_object())
            {
                return false;
            }
            constexpr auto members = Members<T>::value;
            detail::deserialize_members(obj, j, std::make_index_sequence<std::tuple_size_v<decltype(members)>>{});
            return true;
        }
        else
        {
            return false; // 不是 Reflectable 类型
        }
    }

} // namespace CauldronZero::reflection
