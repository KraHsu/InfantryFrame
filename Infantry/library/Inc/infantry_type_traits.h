/*
 *                        ::
 *                       :;J7, :,                        ::;7:
 *                       ,ivYi, ,                       ;LLLFS:
 *                       :iv7Yi                       :7ri;j5PL
 *                      ,:ivYLvr                    ,ivrrirrY2X,
 *                      :;r@Wwz.7r:                :ivu@kexianli.
 *                     :iL7::,:::iiirii:ii;::::,,irvF7rvvLujL7ur
 *                    ri::,:,::i:iiiiiii:i:irrv177JX7rYXqZEkvv17
 *                 ;i:, , ::::iirrririi:i:::iiir2XXvii;L8OGJr71i
 *               :,, ,,:   ,::ir@mingyi.irii:i:::j1jri7ZBOS7ivv,
 *                  ,::,    ::rv77iiiriii:iii:i::,rvLq@huhao.Li
 *              ,,      ,, ,:ir7ir::,:::i;ir:::i:i::rSGGYri712:
 *            :::  ,v7r:: ::rrv77:, ,, ,:i7rrii:::::, ir7ri7Lri
 *           ,     2OBBOi,iiir;r::        ,irriiii::,, ,iv7Luur:
 *         ,,     i78MBBi,:,:::,:,  :7FSL: ,iriii:::i::,,:rLqXv::
 *         :      iuMMP: :,:::,:ii;2GY7OBB0viiii:i:iii:i:::iJqL;::
 *        ,     ::::i   ,,,,, ::LuBBu BBBBBErii:i:i:i:i:i:i:r77ii
 *       ,       :       , ,,:::rruBZ1MBBqi, :,,,:::,::::::iiriri:
 *      ,               ,,,,::::i:  @arqiao.       ,:,, ,:::ii;i7:
 *     :,       rjujLYLi   ,,:::::,:::::::::,,   ,:i,:,,,,,::i:iii
 *     ::      BBBBBBBBB0,    ,,::: , ,:::::: ,      ,,,, ,,:::::::
 *     i,  ,  ,8BMMBBBBBBi     ,,:,,     ,,, , ,   , , , :,::ii::i::
 *     :      iZMOMOMBBM2::::::::::,,,,     ,,,,,,:,,,::::i:irr:i:::,
 *     i   ,,:;u0MBMOG1L:::i::::::  ,,,::,   ,,, ::::::i:i:iirii:i:i:
 *     :    ,iuUuuXUkFu7i:iii:i:::, :,:,: ::::::::i:i:::::iirr7iiri::
 *     :     :rk@Yizero.i:::::, ,:ii:::::::i:::::i::,::::iirrriiiri::,
 *      :      5BMBBBBBBSr:,::rv2kuii:::iii::,:i:,, , ,,:,:i@petermu.,
 *           , :r50EZ8MBBBBGOBBBZP7::::i::,:::::,: :,:,::i;rrririiii::
 *               :jujYY7LS0ujJL7r::,::i::,::::::::::::::iirirrrrrrr:ii:
 *            ,:  :@kevensun.:,:,,,::::i:i:::::,,::::::iir;ii;7v77;ii;i,
 *            ,,,     ,,:,::::::i:iiiii:i::::,, ::::iiiir@xingjief.r;7:i,
 *         , , ,,,:,,::::::::iiiiiiiiii:,:,:::::::::iiir;ri7vL77rrirri::
 *          :,, , ::::::::i:::i:::i:i::,,,,,:,::i:i:::iir;@Secbone.ii:::
 * 
 * @Date: 2024-01-16 20:40:50
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-17 01:56:23
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
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

