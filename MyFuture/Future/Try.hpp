//
//  Try.hpp
//  MyFuture
//
//  Created by huanao on 2019/1/3.
//  Copyright © 2019年 fgr. All rights reserved.
//

#pragma once
#include <type_traits>

template <typename T>
class Try
{
    static_assert(!std::is_reference<T>::value, "Try may not be used with reference types");
    
    enum class Contains
    {
        VALUE, EXCEPTION, NOTHING,
    };
    
public:
    
    using element_type = T;
    
    Try() noexcept : contains_(Contains::NOTHING){}
    
    Try(T&& val):value_(val) {}
    
    explicit Try(const T& val)
    :value_(val),
    contains_(Contains::VALUE)
    {}
    
    
public:
    
    T& value() &
    {
        return value_;
    }

    T&& value() &&
    {
        return std::move(value_);
    }
    
    const T& value() const&
    {
        return value_;
    }
    const T&& value() const&&
    {
        return std::move(value_);
    }
    
    
private:
    
    Contains contains_;
    union
    {
        T value_;
        // exception?
    };
};








