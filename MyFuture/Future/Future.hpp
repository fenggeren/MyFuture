//
//  Future.hpp
//  Folly_Learn
//
//  Created by huanao on 2019/1/3.
//  Copyright © 2019年 HuanaoGroup. All rights reserved.
//

#pragma once
 
#include <functional>
#include <type_traits>
#include "Promise.hpp"
#include "Core.hpp"
#include "FutureTraits.hpp"
#include "Try.hpp"

template <typename T>
class Future
{
public:
    
    Future(Core<T>* core)
    :core_(core)
    {
        
    }
    
    Future()
    {
        
    }
    
    template <typename F>
    auto thenValue(F&& func) &&
    {
        using R = std::result_of_t<F(T)>;
        Promise<R> p;
        auto fu = p.getFuture();
        auto lambda = [f=std::move(func), pro=std::move(p)]
        (T&& val) mutable
        {
            auto r = f(std::forward<T>(val));
            pro.setValue(std::move(r));
        };
        
        setCallback(std::move(lambda));
        
        return fu;
    }
    
    template <typename R>
    auto
    then(std::function<R(T)>&& func) -> Future<R>
    {
        Promise<R> p;
        auto future = p.getFuture();
        
        auto lambda = [f=std::move(func), pro=std::move(p)](T val) mutable
        {
            auto r = f(val);
            pro.setValue(std::move(r));
        };
        
        setCallback(std::move(lambda));
        
        return future;
    }
    
    void setCallback(std::function<void(T)>&& func)
    {
        core_->setCallback(std::forward<decltype(func)>(func));
    }
    
    T value()
    {
        return core_->result();
    }
    
private:
    
    template <typename F, typename R>
    auto thenImplementation(F&& func, R)
    {
        using B = typename R::Result;
        
        Promise<B> p;
        auto f = p.getFuture();
        setCallback([p=std::move(p), f=std::forward<F>(func)]
                    (T&& val) mutable
                    {
                        
                    });
        return f;
    }
    
private:
    Core<T>* core_;
};










