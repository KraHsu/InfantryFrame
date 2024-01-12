/**
 * @file Infantry/Src
 * @brief $Brief$
 * @details $Detaile$
 * @author CharlesHsu
 * @date 11/11/2023
 */
#include "device/Inc/infantry_fdcan_device.h"
#include "fdcan.h"
/* Global variables and functions */
namespace infantry {
    FdcanRxDataType *Fdcan1DeviceRxList[16]{nullptr};
    FdcanRxDataType *Fdcan2DeviceRxList[16]{nullptr};
    FdcanRxDataType *Fdcan3DeviceRxList[16]{nullptr};
    uint8_t FdcanDeviceRxListIndex[3]{0};

    [[noreturn]] void FdcanErrorHandler(const char *msg) {
        logErrorWithTag("Fdcan", msg);
        while (true) {
            //TODO: Fdcan devices error handler
        }
    }

    [[noreturn]] void FDCAN_ErrorHandler() {
        while (true) {
            // return;
        }
    }

    void FdcanInitFilterAndStart(FDCAN_HandleTypeDef *ph_fdcan, FDCAN_FilterTypeDef *p_config) {
        if (p_config != nullptr) {
            if (HAL_FDCAN_ConfigFilter(ph_fdcan, p_config) != HAL_OK) {
                FDCAN_ErrorHandler();
            }
            HAL_FDCAN_ConfigTxDelayCompensation(
                    ph_fdcan, ph_fdcan->Init.DataPrescaler * ph_fdcan->Init.DataTimeSeg1, 0
            );
            HAL_FDCAN_EnableTxDelayCompensation(ph_fdcan);
        } else {
            FDCAN_FilterTypeDef sFilterConfig;
            sFilterConfig.IdType = FDCAN_STANDARD_ID;
            sFilterConfig.FilterIndex = 0;
            sFilterConfig.FilterType = FDCAN_FILTER_MASK;
            sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
            sFilterConfig.FilterID1 = 0x0400;
            sFilterConfig.FilterID2 = 0x0000;
            if (HAL_FDCAN_ConfigFilter(ph_fdcan, &sFilterConfig) != HAL_OK) {
                FDCAN_ErrorHandler();
            }
        }

        if (HAL_FDCAN_ConfigGlobalFilter(
                ph_fdcan, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK) {
            FDCAN_ErrorHandler();
        }
        if (HAL_FDCAN_Start(ph_fdcan) != HAL_OK) {
            FDCAN_ErrorHandler();
        }

        if (HAL_FDCAN_ActivateNotification(ph_fdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
            FDCAN_ErrorHandler();
        }
        if (HAL_FDCAN_ActivateNotification(ph_fdcan, FDCAN_IT_BUS_OFF, 0) != HAL_OK) {
            FDCAN_ErrorHandler();
        }
    }
}
/* Major classes */
namespace infantry {
    FdcanDataType::FdcanDataType(FDCAN_HandleTypeDef *ph_fdcan) : _ph_fdcan{ph_fdcan} {}

    FdcanDataType *FdcanDataType::init() {
        return this;
    }

    FdcanDevice::FdcanDevice(FdcanRxDataType *rx_data, FdcanTxDataType *tx_data)
            : _rx_data{rx_data}, _tx_data{tx_data} {}

    FdcanDevice::~FdcanDevice() {
        delete _rx_data;
        delete _tx_data;
    }

    FdcanDevice *FdcanDevice::init() {
        if (_rx_data != nullptr) {
            _rx_data->init();
        }
        if (_tx_data != nullptr) {
            _tx_data->init();
        }
        return this;
    }

    FdcanDevice *FdcanDevice::registerRxCallback() {
        if (_rx_data == nullptr) {
            FdcanErrorHandler("Cannot register a receive callback function when the receive data type is null");
        }
        if (_rx_data->_ph_fdcan == &hfdcan1) {
            Fdcan1DeviceRxList[FdcanDeviceRxListIndex[0]++] = _rx_data;
        } else if (_rx_data->_ph_fdcan == &hfdcan2) {
            Fdcan2DeviceRxList[FdcanDeviceRxListIndex[1]++] = _rx_data;
        } else if (_rx_data->_ph_fdcan == &hfdcan3) {
            Fdcan3DeviceRxList[FdcanDeviceRxListIndex[2]++] = _rx_data;
        }
        return this;
    }

    FdcanDevice *FdcanDevice::transmit() {
        if (_tx_data == nullptr) {
            FdcanErrorHandler("Data cannot be sent if the send data type is null");
        }
        _tx_data->txHook();
        if (HAL_FDCAN_AddMessageToTxFifoQ(_tx_data->_ph_fdcan, &_tx_data->_header, _tx_data->_buffer) != HAL_OK) {
            logErrorWithTag("fdcan", "Data sending failure");
        }
        return this;
    }

    FdcanDevice *FdcanDevice::process() {
        return this;
    }

    FdcanTxDataType::FdcanTxDataType(FDCAN_TxHeaderTypeDef header, FDCAN_HandleTypeDef *ph_fdcan, uint16_t buffer_size)
            : FdcanDataType(ph_fdcan), _header{header}, _buffer_size{buffer_size} {}

    FdcanTxDataType *FdcanTxDataType::init() {
        _buffer = new uint8_t[_buffer_size];
        return this;
    }

    FdcanTxDataType::~FdcanTxDataType() {
        delete[] _buffer;
    }

    FdcanRxDataType::FdcanRxDataType(FDCAN_HandleTypeDef *ph_fdcan, uint32_t std_id)
            : FdcanDataType(ph_fdcan), stdID{std_id} {}
}

/* Interrupt callback */
uint8_t FdcanTempBuffer[200];
FDCAN_RxHeaderTypeDef FdcanTempHeader;

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *phfdcan, uint32_t RxFifo0ITs) {
    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {
        HAL_FDCAN_GetRxMessage(phfdcan, FDCAN_RX_FIFO0, &FdcanTempHeader, FdcanTempBuffer);
        logDebugWithTag("fdcan", "phfdcan(fdcan%d) stdid(0x%x)",
                        phfdcan == &hfdcan1 ? 1 : (phfdcan == &hfdcan2 ? 2 : (phfdcan == &hfdcan3 ? 3 : 0)),
                        FdcanTempHeader.Identifier
        );

        infantry::FdcanRxDataType **rxList{nullptr};
        if (phfdcan == &hfdcan1) {
            rxList = infantry::Fdcan1DeviceRxList;
        } else if (phfdcan == &hfdcan2) {
            rxList = infantry::Fdcan2DeviceRxList;
        } else if (phfdcan == &hfdcan3) {
            rxList = infantry::Fdcan3DeviceRxList;
        }

        if (rxList != nullptr) {
            for (int i = 0; i < 16; i++) {
                auto device_ptr = *(rxList + i);
                if (device_ptr == nullptr) { break; }
                if (FdcanTempHeader.Identifier == device_ptr->stdID) {
                    device_ptr->rxCallback(FdcanTempHeader, FdcanTempBuffer);
                }
            }
        }

        if (HAL_FDCAN_ActivateNotification(phfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
            while (true) {}
        }
    }
}
