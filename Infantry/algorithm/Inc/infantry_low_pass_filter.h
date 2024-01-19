/*
 *                   江城子 . 程序员之歌
 * 
 *               十年生死两茫茫，写程序，到天亮。
 *                   千行代码，Bug何处藏。
 *               纵使上线又怎样，朝令改，夕断肠。
 * 
 *               领导每天新想法，天天改，日日忙。
 *                   相顾无言，惟有泪千行。
 *               每晚灯火阑珊处，夜难寐，加班狂。
 * 
 * 
 * @Date: 2024-01-16 20:40:50
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-17 01:49:53
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
 */

#pragma once

#include "main.h"

namespace infantry
{
    class LowPassFilter
    {
    private:
        struct {
            float cut_off_frq{};
            float filt_para{};
            float last_tick{0};
            float calc_frq{};
        } _param;
        float filtered_val{};
        float filtered_last_val{};

    public:
        explicit LowPassFilter(float param);

        float calculate(float val);
    };
}
