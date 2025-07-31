#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <tuple>
#include <type_traits>

namespace CauldronZero::reflection
{

    // 用于存储成员变量指针和其名称的结构体
    template <typename T, typename U>
    struct Member
    {
        U T::*ptr;
        const char* name;
    };

    // 用于获取结构体成员的元编程辅助
    template <typename T>
    struct Members;

    // Reflectable concept: 检查一个类型是否定义了 Members<T>::value
    template <typename T>
    concept Reflectable = requires
    {
        Members<T>::value;
    };

    // --- 宏的内部实现细节 ---
    // 宏展开的辅助工具
#define CZ_REFLECT_EXPAND(x) x
#define CZ_REFLECT_GET_MACRO(                                                                                          \
    _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25,  \
    _26, _27, _28, _29, _30, _31, _32, NAME, ...                                                                        \
) NAME
#define CZ_REFLECT_FOR_EACH(action, type, ...)                                                                         \
    CZ_REFLECT_EXPAND(CZ_REFLECT_GET_MACRO(                                                                             \
        __VA_ARGS__, CZ_REFLECT_FOR_EACH_32, CZ_REFLECT_FOR_EACH_31, CZ_REFLECT_FOR_EACH_30, CZ_REFLECT_FOR_EACH_29,     \
        CZ_REFLECT_FOR_EACH_28, CZ_REFLECT_FOR_EACH_27, CZ_REFLECT_FOR_EACH_26, CZ_REFLECT_FOR_EACH_25,                 \
        CZ_REFLECT_FOR_EACH_24, CZ_REFLECT_FOR_EACH_23, CZ_REFLECT_FOR_EACH_22, CZ_REFLECT_FOR_EACH_21,                 \
        CZ_REFLECT_FOR_EACH_20, CZ_REFLECT_FOR_EACH_19, CZ_REFLECT_FOR_EACH_18, CZ_REFLECT_FOR_EACH_17,                 \
        CZ_REFLECT_FOR_EACH_16, CZ_REFLECT_FOR_EACH_15, CZ_REFLECT_FOR_EACH_14, CZ_REFLECT_FOR_EACH_13,                 \
        CZ_REFLECT_FOR_EACH_12, CZ_REFLECT_FOR_EACH_11, CZ_REFLECT_FOR_EACH_10, CZ_REFLECT_FOR_EACH_9,                  \
        CZ_REFLECT_FOR_EACH_8, CZ_REFLECT_FOR_EACH_7, CZ_REFLECT_FOR_EACH_6, CZ_REFLECT_FOR_EACH_5,                     \
        CZ_REFLECT_FOR_EACH_4, CZ_REFLECT_FOR_EACH_3, CZ_REFLECT_FOR_EACH_2, CZ_REFLECT_FOR_EACH_1                      \
    )(action, type, __VA_ARGS__))

#define CZ_REFLECT_GENERATE_MEMBER_INFO(type, member)                                                                  \
    ::CauldronZero::reflection::Member<type, decltype(type::member)>{&type::member, #member}

#define CZ_REFLECT_FOR_EACH_1(action, type, x)  action(type, x)
#define CZ_REFLECT_FOR_EACH_2(action, type, x, ...)                                                                    \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_1(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_3(action, type, x, ...)                                                                    \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_2(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_4(action, type, x, ...)                                                                    \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_3(action, type, __VA_ARGS__))
// ... (为更多参数添加定义，最多支持32个)
#define CZ_REFLECT_FOR_EACH_5(action, type, x, ...)                                                                    \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_4(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_6(action, type, x, ...)                                                                    \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_5(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_7(action, type, x, ...)                                                                    \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_6(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_8(action, type, x, ...)                                                                    \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_7(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_9(action, type, x, ...)                                                                    \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_8(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_10(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_9(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_11(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_10(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_12(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_11(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_13(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_12(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_14(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_13(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_15(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_14(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_16(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_15(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_17(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_16(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_18(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_17(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_19(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_18(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_20(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_19(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_21(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_20(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_22(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_21(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_23(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_22(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_24(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_23(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_25(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_24(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_26(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_25(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_27(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_26(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_28(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_27(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_29(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_28(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_30(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_29(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_31(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_30(action, type, __VA_ARGS__))
#define CZ_REFLECT_FOR_EACH_32(action, type, x, ...)                                                                   \
    action(type, x), CZ_REFLECT_EXPAND(CZ_REFLECT_FOR_EACH_31(action, type, __VA_ARGS__))


} // namespace CauldronZero::reflection

// 辅助宏，用于简化反射信息的定义
// 这个宏必须在全局命名空间或者目标结构体所在的命名空间中使用
#define REFLECT_MEMBERS(Struct, ...)                                                                                   \
    namespace CauldronZero::reflection                                                                                 \
    {                                                                                                                  \
        template <>                                                                                                    \
        struct Members<Struct>                                                                                         \
        {                                                                                                              \
            static constexpr auto value = std::make_tuple(                                                             \
                CZ_REFLECT_FOR_EACH(CZ_REFLECT_GENERATE_MEMBER_INFO, Struct, __VA_ARGS__)                               \
            );                                                                                                         \
        };                                                                                                             \
    }
