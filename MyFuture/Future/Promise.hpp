//
//  Promise.hpp
//  Folly_Learn
//
//  Created by huanao on 2019/1/3.
//  Copyright © 2019年 HuanaoGroup. All rights reserved.
//
#pragma once

#include <functional>
#include "Core.hpp"
#include "Future.hpp"

template<typename T> class Future;

template <typename T>
class Promise
{
public:
    
    
    Promise()
    {
        core_ = new Core<T>;
    }
    
    Future<T> getFuture()
    {
        return Future<T>(core_);
    }
    
    template <typename M>
    void setValue(M&& val)
    {
        fulfilTry(Try<T>(std::forward<M>(val)));
    }
    void setValue()
    {
        fulfilTry(Try<void>());
    }
    
    void fulfilTry(Try<T>&& t)
    {
        core_->setResult(std::move(t));
    }
    
    template <class F>
    void fulfil(F&& func)
    {
        fulfilTry(makeTryFunction(std::forward<F>(func)));
    }
    
private:
    Core<T>* core_;

};
