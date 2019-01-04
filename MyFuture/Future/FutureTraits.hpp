//
//  FutureTraits.h
//  MyFuture
//
//  Created by huanao on 2019/1/3.
//  Copyright © 2019年 fgr. All rights reserved.
//

#pragma once
#include <type_traits>
#include <functional>
#include "Unit.hpp"

template <typename T> class Future;
template <typename T> class Promise;
template <typename T> class Try;

template <typename ...>
struct ArgType;

template <typename Arg, typename... Args>
struct ArgType<Arg, Args...>
{
    using FirstArg = Arg;
};
template<>
struct ArgType<>
{
    using FirstVoid = void;
};


template <typename T>
struct isFuture : std::false_type
{
    using Inner = lift_unit_t<T>;
    using Return = Inner;
};
template <typename T>
struct isFuture<Future<T>> : std::true_type
{
    using Inner = T;
    using Return = Future<Inner>;
};

template <typename T>
struct isTry : std::false_type {};

template <typename T>
struct isTry<Try<T>> : std::true_type {};





template <bool isTry_, typename F, typename... Args>
struct argResult
{
    using Function = F;
    using ArgList = ArgType<Args...>;
    using Result = std::result_of_t<F(Args...)>;
    using ArgsSize = std::integral_constant<std::size_t, sizeof...(Args)>;
    static constexpr bool isTry()
    {
        return isTry_;
    }
};


template <typename T, typename F>
struct FutureThenCallbackResult
{
    using Arg = argResult<false, F, T&&>;
    using ReturnsFuture = isFuture<typename Arg::Result>; // then方法的返回值
    using value_type = typename ReturnsFuture::Inner; // ↑的模板参数类型
    using FirstArg = typename Arg::ArgList::FirstArg;
    using Result = Future<value_type>; //typename ReturnsFuture::Return;
};




template <typename T>
struct AliasIfVoid
{
    using type = typename std::conditional_t<
    std::is_same<T, void>::value, Unit, T>;
};





