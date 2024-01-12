/**
 * @file Infantry/device/Src
 * @author CharlesHsu
 * @date 11/22/2023
 */
#pragma once

#include "infantry_def.h"
#include <list>

namespace infantry {
    template<typename RxDataType, typename TxDataType>
    class infantryDevice {
    protected:
        uint8_t *_name;
    };
}
