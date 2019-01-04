//
//  main.cpp
//  MyFuture
//
//  Created by huanao on 2019/1/3.
//  Copyright © 2019年 fgr. All rights reserved.
//

#include <iostream>


#include "Future.hpp"
#include "Promise.hpp"




void test()
{
//    Promise<int> pro;
//    auto future = pro.getFuture();
//    auto fu = future.then<int>([](int val) -> int
//                               {
//                                   return 10;
//                               })
//    .then<std::string>([](int val) -> std::string
//                       {
//                           return std::to_string(val) + "呵呵🙄";
//                       })
//    .then<std::string>([](std::string&& val) -> std::string
//                       {
//                           return val + "呵呵🙄";
//                       });
//    pro.setValue(100);
//    
//    std::cout << fu.value() << std::endl;
}


struct tag_empty_t{};

template <typename T>
struct AliasIfVoidt
{
    using type = typename std::conditional_t<
    std::is_same<T, void>::value, tag_empty_t, T>;
};

template <typename T>
struct CallbackF
{
    void operator()(int)
    {
        
    }
    
    template <typename F>
    std::enable_if_t<
    !std::is_same<std::result_of_t<F(typename AliasIfVoidt<T>::type)>, void>::value, int>
    finvoke(F&& f)
    {
        return 10;
    }
};





void test2()
{
    Promise<int> pro;
    auto future = pro.getFuture()
    .thenValue([](int val)
               {
                   std::cout << val << std::endl;
                   return "start: ";
               })
    .thenValue([](std::string&& val)
               {
                   std::cout << val << std::endl;
                   return val + " Middle;";
               })
    .thenValue([](std::string&& val)
               {
                   return val + " end;";
               })
    .thenValue([](std::string&& val)
               {
                   std::cout << val << std::endl;
               })
    .thenValue([]()
               {
                   return 111;
               });
    pro.setValue(100);
    std::cout << future.value() << std::endl;
}


void test3()
{
    makeFuture().thenValue([](auto&&)
    {
        return 123;
    });
}

#include <boost/asio.hpp>
#include <thread>
using namespace boost::asio;

void test4()
{
    io_context ioc1;
    io_context ioc2;
    auto guard1 = make_work_guard(ioc1);
    auto guard2 = make_work_guard(ioc2);
    std::thread([&]{
        ioc2.run();
    }).detach();
    
    makeFuture("1234")
    .thenValue([](std::string&& val)
               {
                   std::cout << val << "\n" << std::this_thread::get_id() << std::endl;
                   return val + " begin: ";
               })
    .via(&ioc1)
    .thenValue([](std::string&& val)
               {
                   std::cout << val  << "\n"<< std::this_thread::get_id() << std::endl;
                   return 1234;
               })
    .via(&ioc2)
    .thenValue([](int val)
               {
                   std::cout << val << "\n" << std::this_thread::get_id() << std::endl;
               });
    
    ioc1.run();
}


int main(int argc, const char * argv[])
{
    test4();
    
    return 0;
}
