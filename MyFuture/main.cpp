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
    Promise<int> pro;
    auto future = pro.getFuture();
    auto fu = future.then<int>([](int val) -> int
                               {
                                   return 10;
                               })
    .then<std::string>([](int val) -> std::string
                       {
                           return std::to_string(val) + "呵呵🙄";
                       })
    .then<std::string>([](std::string&& val) -> std::string
                       {
                           return val + "呵呵🙄";
                       });
    pro.setValue(100);
    
    std::cout << fu.value() << std::endl;
}

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
      });
    pro.setValue(100);
    std::cout << future.value() << std::endl;
    
}



int main(int argc, const char * argv[])
{
    test2();
    return 0;
}
