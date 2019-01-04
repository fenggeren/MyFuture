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


// 根据参数不同调用不同

template <typename T, typename F>
class CoreCallbackState
{
    using DF = std::decay_t<F>;
    template <typename R, std::size_t S>
    using IfArgsSizeIs = std::enable_if_t<R::Arg::ArgsSize::value == S, int>;
public:
    CoreCallbackState(Promise<T>&& promise, F&& func)
    :func_(std::forward<F>(func)), promise_(std::move(promise))
    {
    }
    
    template <typename U, typename R, IfArgsSizeIs<R,0> = 0>
    auto invoke(Try<U>& t, R)
    {
        return std::forward<F>(func_)();
    }
    
    template <typename U, typename R, IfArgsSizeIs<R,1> = 0>
    auto invoke(Try<U>& t, R)
    {
//        using Arg0 = typename R::Arg::ArgList::FirstArg;
        return std::forward<F>(func_)(std::move(t.value()));
    }

//    template <typename U, typename R>
//    auto invoke(U&& u, R)
//    {
//        return std::forward<F>(func_)(std::forward<U>(u));
//    }
    
    void setTry(Try<T>&& t)
    {
        stealPromise().fulfilTry(std::move(t));
    }
    
    
    Promise<T> stealPromise()
    {
        func_.~DF();
        return std::move(promise_);
    }
    
private:
    DF func_;
    Promise<T> promise_{Promise<T>::makeEmpty()};
};

template <typename T, typename F>
auto makeCoreCallbackState(Promise<T>&& p, F&& f)
{
    return CoreCallbackState<T, F>(std::move(p), std::forward<F>(f));
}

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
    
    ~Future()
    {
        if (core_)
        {
            core_ = nullptr;
        }
    }
    
public:
    
    template <typename Executor>
    Future<T> via(Executor* executor)&&
    {
        core_->setExecutor(executor);
        return std::move(*this);
    }

    
public:
    
    template <typename F>
    typename std::enable_if_t<
    !std::is_same<T, void>::value,
    Future<std::result_of_t<F(typename AliasIfVoid<T>::type&&)>>>
    thenValue(F&& func) &&
    {
        using R = std::result_of_t<F(T)>;
        Promise<R> p;
        auto fu = p.getFuture();
        setCallback([f=std::move(func), pro=std::move(p)]
        (Try<T>&& t) mutable
        {
            pro.fulfil([&]{
                return f(std::move(t.value()));
            });
        });
        
        return fu;
    }
    
    template <typename F>
    typename std::enable_if_t<
    std::is_same<T, void>::value,
    Future<std::result_of_t<F()>>>
    thenValue(F&& func) &&
    {
        using R = std::result_of_t<F()>;
        Promise<R> p;
        auto fu = p.getFuture();
        setCallback([f=std::move(func), pro=std::move(p)]
        (Try<T>&& t) mutable
        {
            pro.fulfil([&]{
                return f();
            });
        });
        
        return fu;
    }
#if 0
    template <typename F>
    Future<typename FutureThenCallbackResult<T, F>::value_type>
    then(F&& func) &&
    {
        using FC = FutureThenCallbackResult<T, F>;
        using R = typename FC::value_type;
        Promise<R> p;
        auto fu = p.getFuture();
        
        setCallback([state=makeCoreCallbackState(std::move(p),
                                             std::forward<F>(func))]
                    (Try<T>&& t) mutable
                    {
                        state.setTry(
                        makeTryFunction(
                        [&]
                        {
                            return state.invoke(t, FC{});
                        }));
                    });
        
        return fu;
    }
#endif
    
    T value()
    {
        return core_->getTry().value();
    }
    
protected:
    
    // [](Try<T>&&t){ return t.value(); }
    template <typename F, typename R>
    typename std::enable_if<!R::ReturnsFuture::value, typename R::Return>::type
    thenImplementation(F&& func, R)
    {
        static_assert(R::Arg::ArgsSize::value <= 1,
                      "Then must take zero/one argument");
        using B = typename R::ReturnsFuture::Inner;
        
        Promise<B> p;
        auto f = p.getFuture();
        
        setCallback([f=std::move(func), pro=std::move(p)]
                    (Try<T>&& t) mutable
                    {
                        // 根据 R::Arg::ArgsSize::value的数量是否为0
                        // invoke f不同
                        
                        pro.fulfil([&]{
                            return f(std::move(t.value()));
                        });
                    });
        
        return f;
    }
    
    // [](Try<T>&&t){ return makeFuture<T>(t); }
    template <typename F, typename R>
    typename std::enable_if<R::ReturnsFuture::value, typename R::Return>::type
    thenImplementation(F&& func, R)
    {
        
    }
    
//    template <typename R>
//    auto
//    then(std::function<R(T)>&& func) -> Future<R>
//    {
//        Promise<R> p;
//        auto future = p.getFuture();
//
//        auto lambda = [f=std::move(func), pro=std::move(p)](T val) mutable
//        {
//            auto r = f(val);
//            pro.setValue(std::move(r));
//        };
//
////        setCallback(std::move(lambda));
//
//        return future;
//    }
    
    template <typename F>
    void setCallback(F&& func)
    {
        core_->setCallback(std::move(func));
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

template <class T>
Future<T> makeFuture(Try<T> t)
{
    return Future<T>(Core<T>::make(std::move(t)));
}


template <class T>
Future<typename std::decay<T>::type>
makeFuture(T&& t)
{
    return makeFuture(Try<typename std::decay<T>::type>(std::forward<T>(t)));
}

Future<Unit> makeFuture()
{
    return makeFuture(Unit{});
}







