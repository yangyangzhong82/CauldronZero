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

    // 辅助宏，用于简化反射信息的定义
    // 使用方法：
    // struct MyStruct {
    //     int a;
    //     std::string b;
    // };
    // REFLECT_MEMBERS(MyStruct, a, b)
#define REFLECT_MEMBERS(Struct, ...)                                                                                   \
    namespace CauldronZero::reflection                                                                                 \
    {                                                                                                                  \
        template <>                                                                                                   \
        struct Members<Struct>                                                                                        \
        {                                                                                                              \
            static constexpr auto value = std::make_tuple(                                                             \
                FOR_EACH(GENERATE_MEMBER_INFO, Struct, __VA_ARGS__)                                                    \
            );                                                                                                         \
        };                                                                                                             \
    }

    // --- 宏的内部实现细节 ---
#define EXPAND(x) x
#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, NAME, ...) NAME

#define FOR_EACH(action, type, ...)                                                                                    \
    EXPAND(GET_MACRO(__VA_ARGS__, FOR_EACH_16, FOR_EACH_15, FOR_EACH_14, FOR_EACH_13, FOR_EACH_12, FOR_EACH_11,          \
                     FOR_EACH_10, FOR_EACH_9, FOR_EACH_8, FOR_EACH_7, FOR_EACH_6, FOR_EACH_5, FOR_EACH_4, FOR_EACH_3,   \
                     FOR_EACH_2, FOR_EACH_1)(action, type, __VA_ARGS__))

#define GENERATE_MEMBER_INFO(type, member) CauldronZero::reflection::Member<type, decltype(type::member)> { &type::member, #member }

#define FOR_EACH_1(action, type, x) action(type, x)
#define FOR_EACH_2(action, type, x, ...) action(type, x), EXPAND(FOR_EACH_1(action, type, __VA_ARGS__))
#define FOR_EACH_3(action, type, x, ...) action(type, x), EXPAND(FOR_EACH_2(action, type, __VA_ARGS__))
#define FOR_EACH_4(action, type, x, ...) action(type, x), EXPAND(FOR_EACH_3(action, type, __VA_ARGS__))
#define FOR_EACH_5(action, type, x, ...) action(type, x), EXPAND(FOR_EACH_4(action, type, __VA_ARGS__))
#define FOR_EACH_6(action, type, x, ...) action(type, x), EXPAND(FOR_EACH_5(action, type, __VA_ARGS__))
#define FOR_EACH_7(action, type, x, ...) action(type, x), EXPAND(FOR_EACH_6(action, type, __VA_ARGS__))
#define FOR_EACH_8(action, type, x, ...) action(type, x), EXPAND(FOR_EACH_7(action, type, __VA_ARGS__))
#define FOR_EACH_9(action, type, x, ...) action(type, x), EXPAND(FOR_EACH_8(action, type, __VA_ARGS__))
#define FOR_EACH_10(action, type, x, ...) action(type, x), EXPAND(FOR_EACH_9(action, type, __VA_ARGS__))
#define FOR_EACH_11(action, type, x, ...) action(type, x), EXPAND(FOR_EACH_10(action, type, __VA_ARGS__))
#define FOR_EACH_12(action, type, x, ...) action(type, x), EXPAND(FOR_EACH_11(action, type, __VA_ARGS__))
#define FOR_EACH_13(action, type, x, ...) action(type, x), EXPAND(FOR_EACH_12(action, type, __VA_ARGS__))
#define FOR_EACH_14(action, type, x, ...) action(type, x), EXPAND(FOR_EACH_13(action, type, __VA_ARGS__))
#define FOR_EACH_15(action, type, x, ...) action(type, x), EXPAND(FOR_EACH_14(action, type, __VA_ARGS__))
#define FOR_EACH_16(action, type, x, ...) action(type, x), EXPAND(FOR_EACH_15(action, type, __VA_ARGS__))

} // namespace CauldronZero::reflection
