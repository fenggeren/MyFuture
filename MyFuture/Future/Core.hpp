//
//  Core.hpp
//  Folly_Learn
//
//  Created by huanao on 2019/1/3.
//  Copyright © 2019年 HuanaoGroup. All rights reserved.
//
#pragma once

#include <functional>
#include <atomic>
#include "Try.hpp"

static int num = 0;

template <typename T>
class Core
{
public:
    
    using Result = T;
    using Callback = std::function<void(Result&&)>;
    
    Core()
    :attached_(2)
    {
        num_ = num++; 
    }
    
    void setResult(Result&& result)
    {
        new (&result_)Result(std::forward<Result>(result));
        detachOne();
    }
    
    void setCallback(Callback&& cb)
    {
        new (&callback_)Callback(std::forward<Callback>(cb));
        detachOne();
    }
    
    Result&& result()
    {
        return std::move(result_);
    }
    
private:
    
    void doCallback()
    {
        callback_(result());
    }
    
    void detachOne() noexcept
    {
        attached_--;
        if (attached_ == 0)
        {
            doCallback();
        }
    }
    
private:
    std::atomic<unsigned char> attached_;
    Result result_;
    Callback callback_{nullptr};
    int num_;
};


