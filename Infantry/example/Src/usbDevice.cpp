/*
 *                                                     __----~~~~~~~~~~~------___
 *                                    .  .   ~~//====......          __--~ ~~
 *                    -.            \_|//     |||\\  ~~~~~~::::... /~
 *                 ___-==_       _-~o~  \/    |||  \\            _/~~-
 *         __---~~~.==~||\=_    -_--~/_-~|-   |\\   \\        _/~
 *     _-~~     .=~    |  \\-_    '-~7  /-   /  ||    \      /
 *   .~       .~       |   \\ -_    /  /-   /   ||      \   /
 *  /  ____  /         |     \\ ~-_/  /|- _/   .||       \ /
 *  |~~    ~~|--~~~~--_ \     ~==-/   | \~--===~~        .\
 *           '         ~-|      /|    |-~\~~       __--~~
 *                       |-~~-_/ |    |   ~\_   _-~            /\
 *                            /  \     \__   \/~                \__
 *                        _--~ _/ | .-~~____--~-/                  ~~==.
 *                       ((->/~   '.|||' -_|    ~~-/ ,              . _||
 *                                  -_     ~\      ~~---l__i__i__i--~~_/
 *                                  _-~-__   ~)  \--______________--~~
 *                                //.-~~~-~_--~- |-------~~~~~~~~
 *                                       //.-~~~--\
 *                       ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *                               神兽保佑            永无BUG
 *
 * @Date: 2024-01-16 20:40:50
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-23 18:54:08
 * Copyright (c) 2024 by KraHsu, All Rights Reserved.
 */

#include "usbDevice.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

namespace USBDevice {
    static void (*callback)(uint8_t *rxBuf, uint32_t *len) = nullptr;

    static int8_t rxCallbackWrap(uint8_t *Buf, uint32_t *Len) {
        if (callback != nullptr) {
            callback(Buf, Len);
        }
        USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
        USBD_CDC_ReceivePacket(&hUsbDeviceFS);
        return (USBD_OK);
    }

    void setRxCallback(void (*rxCallback)(uint8_t *rxBuf, uint32_t *len)) {
        USBD_Interface_fops_FS.Receive = rxCallbackWrap;
        callback = rxCallback;
    }

    uint8_t send(uint8_t *Buf, uint16_t Len) {
        return CDC_Transmit_FS(Buf, Len);
    };

    void init() {
        MX_USB_Device_Init();
    };
}
