/*
 *           佛曰:  
 *                   写字楼里写字间，写字间里程序员；  
 *                   程序人员写程序，又拿程序换酒钱。  
 *                   酒醒只在网上坐，酒醉还来网下眠；  
 *                   酒醉酒醒日复日，网上网下年复年。  
 *                   但愿老死电脑间，不愿鞠躬老板前；  
 *                   奔驰宝马贵者趣，公交自行程序员。  
 *                   别人笑我忒疯癫，我笑自己命太贱；  
 *                   不见满街漂亮妹，哪个归得程序员？
 * 
 * @Date: 2024-01-16 20:40:50
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-17 01:53:38
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
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
        }

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
        if (HAL_FDCAN_ActivateNotification(ph_fdcan, FDCAN_IT_TX_FIFO_EMPTY, 0) != HAL_OK) {
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
        auto res = HAL_FDCAN_AddMessageToTxFifoQ(_tx_data->_ph_fdcan, &_tx_data->_header, _tx_data->_buffer);
        if (res != HAL_OK) {
            logErrorWithTag("fdcan", "Data sending failure");
            if (_tx_data->_ph_fdcan->ErrorCode | HAL_FDCAN_ERROR_FIFO_FULL) {
                logErrorWithTag("fdcan", "FDCAN_ERROR_TX_FIFO_FULL");
//                HAL_FDCAN_Stop(_tx_data->_ph_fdcan);
//                _tx_data->_ph_fdcan->Instance->TXFQS = 0x03;
//                HAL_FDCAN_Start(_tx_data->_ph_fdcan);
            }
        }
        return this;
    }

    FdcanDevice *FdcanDevice::process() {
        return this;
    }

    FdcanTxDataType::FdcanTxDataType(FDCAN_HandleTypeDef *ph_fdcan, FDCAN_TxHeaderTypeDef header, uint16_t buffer_size)
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
//        logDebugWithTag("fdcan", "phfdcan(fdcan%d) stdid(0x%x)",
//                        phfdcan == &hfdcan1 ? 1 : (phfdcan == &hfdcan2 ? 2 : (phfdcan == &hfdcan3 ? 3 : 0)),
//                        FdcanTempHeader.Identifier
//        );
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
                device_ptr->rxCallback(FdcanTempHeader, FdcanTempBuffer);
            }
        }

        if (HAL_FDCAN_ActivateNotification(phfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
            while (true) {}
        }
    }
}

void HAL_FDCAN_TxFifoEmptyCallback(FDCAN_HandleTypeDef *hfdcan) {
    if (HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_TX_FIFO_EMPTY, 0) != HAL_OK) {
        while (true) {}
    }
}

//void HAL_FDCAN_ErrorCallback(FDCAN_HandleTypeDef *hfdcan) {
//    if (hfdcan->ErrorCode | HAL_FDCAN_ERROR_NOT_STARTED) {
//        while (hfdcan->ErrorCode | HAL_FDCAN_ERROR_NOT_STARTED) {
//            HAL_FDCAN_Stop(hfdcan);
//            HAL_FDCAN_Start(hfdcan);
//            HAL_Delay(10);
//        }
//    }
//    if (hfdcan->ErrorCode | HAL_FDCAN_ERROR_FIFO_FULL) {
//        logErrorWithTag("fdcan", "FDCAN_ERROR_TX_FIFO_FULL");
//        HAL_FDCAN_Stop(hfdcan);
//        hfdcan->Instance->TXFQS = 0x03;
//        HAL_FDCAN_Start(hfdcan);
//    }
//}
