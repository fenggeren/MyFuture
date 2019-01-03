//
//  FutureTraits.h
//  MyFuture
//
//  Created by huanao on 2019/1/3.
//  Copyright © 2019年 fgr. All rights reserved.
//

#pragma once
#include <type_traits>
#include <functional>



template <typename T, typename F>
struct FutureThenCallbackTraits
{
    using Result = std::result_of_t<F(T)>;
    using Arg = T;
};








