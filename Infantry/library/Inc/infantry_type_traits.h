/**
 * @file Infantry/library/Inc
 * @author CharlesHsu
 * @date 12/25/2023
 */

#pragma once

#include <type_traits>

namespace infantry {
    // c++17 前 std 不提供 void_t
    template<typename...>
    using void_t = void;

    // 检查成员变量存在性
    template<typename, typename T, typename = void>
    struct has_member_variable : std::false_type {
    };

    template<typename C, typename T>
    struct has_member_variable<C, T, void_t<decltype(std::declval<C>().*std::declval<T>())>> : std::true_type {
    };

    // 检查成员函数存在性
    template<typename, typename, typename = void>
    struct has_member_function : std::false_type {
    };

    template<typename C, typename F>
    struct has_member_function<C, F, void_t<decltype(&C::F)>> : std::true_type {
    };

    // 检查成员变量存在性及其类型
    template<typename C, typename T, T C::*Ptr, typename = void>
    struct has_member_variable_of_type : std::false_type {
    };

    template<typename C, typename T, T C::*Ptr>
    struct has_member_variable_of_type<C, T, Ptr, void_t<decltype(Ptr)>> : std::true_type {
    };

    // 辅助类型，用于封装成员函数签名
    template<typename Ret, typename... Args>
    struct member_function_signature {
        using type = Ret(Args...);
    };

    /// 检查成员函数存在性及其返回类型的模板
    template<typename C, typename Signature, typename = void>
    struct has_member_function_of_type : std::false_type {
    };

    template<typename C, typename Ret, typename... Args>
    struct has_member_function_of_type<C, member_function_signature<Ret, Args...>,
            void_t<decltype(static_cast<Ret (C::*)(Args...)>(&C::operator()))>> : std::true_type {
    };

#define HAS_MEMBER_VARIABLE(Class, MemberName) has_member_variable<Class, decltype(&Class::MemberName)>::value
#define HAS_MEMBER_FUNCTION(Class, MemberFunc) has_member_function<Class, decltype(&Class::MemberFunc)>::value

#define HAS_MEMBER_VARIABLE_OF_TYPE(Class, Type, MemberName) has_member_variable_of_type<Class, Type, &Class::MemberName>::value
#define HAS_MEMBER_FUNCTION_OF_TYPE(Class, ReturnType, ...) has_member_function_of_type<Class, member_function_signature<ReturnType, __VA_ARGS__>>::value

#define IS_SAME_TYPE(TypeA, TypeB) std::is_same<TypeA, TypeB>::value_type
}

