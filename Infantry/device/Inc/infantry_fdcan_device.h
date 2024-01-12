/**
 * @file Infantry/Src
 * @author CharlesHsu
 * @date 11/11/2023
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

        FdcanTxDataType(FDCAN_TxHeaderTypeDef header, FDCAN_HandleTypeDef *ph_fdcan, uint16_t buffer_size);

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
