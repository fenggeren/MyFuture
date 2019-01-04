//
//  Unit.hpp
//  MyFuture
//
//  Created by huanao on 2019/1/3.
//  Copyright © 2019 fgr. All rights reserved.
//

#pragma once
 
#include <type_traits>

struct EmptyConstruct
{
    
};

struct Unit
{
    constexpr bool operator==(const Unit& /*other*/) const {
        return true;
    }
    constexpr bool operator!=(const Unit& /*other*/) const {
        return false;
    }
};

template <typename T>
struct lift_unit
{
    using type = T;
};
template<>
struct lift_unit<void>
{
    using type = Unit;
};

template <typename T>
using lift_unit_t = typename lift_unit<T>::type;
