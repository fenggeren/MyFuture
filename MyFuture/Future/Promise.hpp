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
    
    void setValue(T&& val)
    {
        core_->setResult(std::forward<T>(val));
    }
    
private:
    Core<T>* core_;

};
