/*
 * _______________#########_______________________ 
 * ______________############_____________________ 
 * ______________#############____________________ 
 * _____________##__###########___________________ 
 * ____________###__######_#####__________________ 
 * ____________###_#######___####_________________ 
 * ___________###__##########_####________________ 
 * __________####__###########_####_______________ 
 * ________#####___###########__#####_____________ 
 * _______######___###_########___#####___________ 
 * _______#####___###___########___######_________ 
 * ______######___###__###########___######_______ 
 * _____######___####_##############__######______ 
 * ____#######__#####################_#######_____ 
 * ____#######__##############################____ 
 * ___#######__######_#################_#######___ 
 * ___#######__######_######_#########___######___ 
 * ___#######____##__######___######_____######___ 
 * ___#######________######____#####_____#####____ 
 * ____######________#####_____#####_____####_____ 
 * _____#####________####______#####_____###______ 
 * ______#####______;###________###______#________ 
 * ________##_______####________####______________ 
 * 
 * @Date: 2024-01-16 20:40:50
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-17 01:53:18
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
 */


#pragma once

#include "fdcan.h"
#include "plugins/logger/Inc/infantry_log.h"

/* Forward declaration */
namespace infantry {
    class FdcanDevice;

    class FdcanDataType;

    class FdcanRxDataType;

    class FdcanTxDataType;
}
/* Global variables and functions */
namespace infantry {
    [[noreturn]] void FdcanErrorHandler(const char *msg = "");

    void FdcanInitFilterAndStart(FDCAN_HandleTypeDef *ph_fdcan, FDCAN_FilterTypeDef *p_config = nullptr);
}
/* Major classes */
namespace infantry {
    class FdcanDataType {
    protected:
        FDCAN_HandleTypeDef *_ph_fdcan{nullptr};
    public:
        friend class FdcanDevice;

        explicit FdcanDataType(FDCAN_HandleTypeDef *ph_fdcan);

        virtual ~FdcanDataType() = default;

        virtual FdcanDataType *init();
    };

    class FdcanRxDataType : public FdcanDataType {
    public:
        uint32_t stdID{0};

        FdcanRxDataType(FDCAN_HandleTypeDef *ph_fdcan, uint32_t std_id);

        virtual FdcanRxDataType *rxCallback(FDCAN_RxHeaderTypeDef header, uint8_t *buffer) = 0;
    };

    class FdcanTxDataType : public FdcanDataType {
    protected:
        uint8_t *_buffer{nullptr};
        uint16_t _buffer_size{0};
        FDCAN_TxHeaderTypeDef _header{};
    public:
        friend class FdcanDevice;

        FdcanTxDataType(FDCAN_HandleTypeDef *ph_fdcan, FDCAN_TxHeaderTypeDef header, uint16_t buffer_size);

        FdcanTxDataType *init() override;

        ~FdcanTxDataType() override;

        virtual FdcanTxDataType *txHook() = 0;
    };

    class FdcanDevice {
    protected:
        FdcanRxDataType *_rx_data{nullptr};
        FdcanTxDataType *_tx_data{nullptr};
    public:
        FdcanDevice(FdcanRxDataType *rx_data, FdcanTxDataType *tx_data);

        virtual ~FdcanDevice();

        virtual FdcanDevice *init();

        virtual FdcanDevice *registerRxCallback();

        virtual FdcanDevice *transmit();

        virtual FdcanDevice *process();
    };
} // namespace infantry
