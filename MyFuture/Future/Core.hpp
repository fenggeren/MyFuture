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
#include <boost/asio.hpp>
#include "Try.hpp"

static int num = 0;

using Executor = boost::asio::io_context;

template <typename T>
class Core
{
public:
    
    using Result = Try<T>;
    using Callback = std::function<void(Result&&)>;
    
    static Core* make()
    {
        return new Core();
    }
    
    static Core* make(Try<T>&& t)
    {
        return new Core(std::move(t));
    }
    
    Core()
    :attached_(2)
    {
        num_ = num++; 
    }
    explicit Core(Try<T>&& t)
    :result_(std::move(t)), attached_(1)
    {}
    
    void setExecutor(Executor* e)
    {
        executor_ = e;
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
    
    Try<T>& getTry()
    {
        return result_;
    }
    
private:
    
    void doCallback()
    {
        if (executor_)
        {
            boost::asio::dispatch(*executor_,
             [cb=std::move(callback_),
              val=std::move(result_)] () mutable
            {
                cb(std::move(val));
            });
        }
        else
        {
            callback_(std::move(result_));
        }
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
    std::atomic<Executor*> executor_{nullptr};
    int num_;
};


