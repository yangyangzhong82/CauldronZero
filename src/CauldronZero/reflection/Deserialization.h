#pragma once

#include "Reflection.h"
#include <nlohmann/json.hpp>
#include <optional>

#include "TypeTraits.h"

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
                const auto& json_val = j.at(name);

                if constexpr (Reflectable<MemberType>)
                {
                    // 递归反序列化嵌套的 Reflectable 对象
                    deserialize(obj.*member_ptr, json_val);
                }
                else if constexpr (detail::is_vector<MemberType>::value)
                {
                    // 处理 vector
                    if (json_val.is_array()) {
                        auto& vec = obj.*member_ptr;
                        vec.clear();
                        using ItemType = typename MemberType::value_type;
                        for (const auto& item_json : json_val) {
                            ItemType item;
                            if constexpr (Reflectable<ItemType>) {
                                deserialize(item, item_json);
                            } else {
                                item_json.get_to(item);
                            }
                            vec.push_back(std::move(item));
                        }
                    }
                }
                else if constexpr (detail::is_map<MemberType>::value)
                {
                    // 处理 map
                    if (json_val.is_object()) {
                        auto& map_obj = obj.*member_ptr;
                        map_obj.clear();
                        using ValueType = typename MemberType::mapped_type;
                        for (auto it = json_val.begin(); it != json_val.end(); ++it) {
                            ValueType value;
                            if constexpr (Reflectable<ValueType>) {
                                deserialize(value, it.value());
                            } else {
                                it.value().get_to(value);
                            }
                            map_obj.emplace(it.key(), std::move(value));
                        }
                    }
                }
                else
                {
                    // 处理普通类型
                    json_val.get_to(obj.*member_ptr);
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
