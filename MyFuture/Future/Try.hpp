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
    
    explicit Try(T&& val):value_(val), contains_(Contains::VALUE) {}
    
    explicit Try(const T& val)
    :value_(val),
    contains_(Contains::VALUE)
    {}
    
    Try(Try<T>&& t)
    :contains_(t.contains_)
    {
        if (contains_ == Contains::VALUE)
        {
            new (&value_)T(std::move(t.value_));
        }
        else if (contains_ == Contains::EXCEPTION)
        {
            //...
        }
    }
    Try& operator=(Try<T>&& t)
    {
        this->~Try();
        contains_ = t.contains_;
        if (contains_ == Contains::VALUE)
        {
            new (&value_)T(std::move(t.value_));
        }
        else if (contains_ == Contains::EXCEPTION)
        {
            //...
        }
        return *this;
    }
    
    ~Try()
    {
        if (contains_ == Contains::VALUE)
        {
            value_.~T();
        }
        else if (contains_ == Contains::EXCEPTION)
        {
            //...
        }
    }
    
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

template <>
class Try<void>
{
public:
    
    using element_type = void;
    
    Try() noexcept : hasValue_(true) {}
    
    
private:
    bool hasValue_;
};


template <typename F>
typename std::enable_if<
!std::is_same<typename std::result_of_t<F()>, void>::value,
Try<typename std::result_of_t<F()>>>::type
makeTryFunction(F&& f)
{
    using ResultType = typename std::result_of_t<F()>;
    return Try<ResultType>(f());
}

template <typename F>
typename std::enable_if<
std::is_same<typename std::result_of_t<F()>, void>::value,
Try<typename std::result_of_t<F()>>>::type
makeTryFunction(F&& f)
{
    f();
    return Try<void>();
}




