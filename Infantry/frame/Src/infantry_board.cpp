/*
 *                                |~~~~~~~|
 *                                |       |
 *                                |       |
 *                                |       |
 *                                |       |
 *                                |       |
 *     |~.\\\_\~~~~~~~~~~~~~~xx~~~         ~~~~~~~~~~~~~~~~~~~~~/_//;~|
 *     |  \  o \_         ,XXXXX),                         _..-~ o /  |
 *     |    ~~\  ~-.     XXXXX`)))),                 _.--~~   .-~~~   |
 *      ~~~~~~~`\   ~\~~~XXX' _/ ';))     |~~~~~~..-~     _.-~ ~~~~~~~
 *               `\   ~~--`_\~\, ;;;\)__.---.~~~      _.-~
 *                 ~-.       `:;;/;; \          _..-~~
 *                    ~-._      `''        /-~-~
 *                        `\              /  /
 *                          |         ,   | |
 *                           |  '        /  |
 *                            \/;          |
 *                             ;;          |
 *                             `;   .       |
 *                             |~~~-----.....|
 *                            | \             \
 *                           | /\~~--...__    |
 *                           (|  `\       __-\|
 *                           ||    \_   /~    |
 *                           |)     \~-'      |
 *                            |      | \      '
 *                            |      |  \    :
 *                             \     |  |    |
 *                              |    )  (    )
 *                               \  /;  /\  |
 *                               |    |/   |
 *                               |    |   |
 *                                \  .'  ||
 *                                |  |  | |
 *                                (  | |  |
 *                                |   \ \ |
 *                                || o `.)|
 *                                |`\\) |
 *                                |       |
 *                                |       |
 * 
 * @Date: 2024-01-16 20:40:50
 * @LastEditors: KraHsu && 1191393280@qq.com
 * @LastEditTime: 2024-01-17 01:55:18
 * Copyright (c) 2024 by KraHsu, All Rights Reserved. 
 */

#include "rthw.h"
#include "rtthread.h"

#include "SEGGER_RTT.h"
#include "arm_compat.h"

#include "infantry_def.h"

#define STM32_SRAM1_START              (0x20000000)
#define STM32_SRAM1_END                (STM32_SRAM1_START + 128 * 1024)   // 结束地址 = 0x20000000（基址） + 20K(RAM大小)

extern int Image$$RW_IRAM1$$ZI$$Limit;                   // RW_IRAM1，需与链接脚本中运行时域名相对应
#define HEAP_BEGIN      ((void *)&Image$$RW_IRAM1$$ZI$$Limit)
#define HEAP_END                       STM32_SRAM1_END

StaticCallType static_call_start_  __attribute__((used, section(".static_call.0.start")));
StaticCallType static_call_end_  __attribute__((used, section(".static_call.1.end")));

SetupCallType board_setup_call_start_  __attribute__((used, section(".setup_call.0.start")));
SetupCallType setup_call_start_  __attribute__((used, section(".setup_call.1.start")));
SetupCallType setup_call_end_  __attribute__((used, section(".setup_call.5.end")));

extern "C" {
void rt_hw_board_init();
char rt_hw_console_getchar();
_Noreturn void Error_Handler();
}

void rt_hw_board_init() {
    for (SetupCallType *fn = &board_setup_call_start_ + 1; fn < &setup_call_start_; fn++) {
        if (fn->entry) {
            fn->entry();
        }
    }

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init((void *) HEAP_BEGIN, (void *) HEAP_END);
#endif
}

int main() {
    for (SetupCallType *fn = &setup_call_start_ + 1; fn < &setup_call_end_; fn++) {
        if (fn->entry) {
            fn->entry();
        }
    }

    for (StaticCallType *fn = &static_call_start_ + 1; fn < &static_call_end_; fn++) {
        if (fn->entry) {
            rt_thread_init(
                    fn->thread, fn->name, fn->entry, RT_NULL, fn->stack_start, fn->stack_size, fn->priority, fn->tick
            );
            rt_thread_startup(fn->thread);
        }
    }

    return 0;
}

#ifdef RT_USING_CONSOLE

//static UART_HandleTypeDef UartHandle;
//
//static int uart_init() {
//    UartHandle.Instance = INFANTRY_SHELL_UART;
//    UartHandle.Init.BaudRate = 115200;
//    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
//    UartHandle.Init.StopBits = UART_STOPBITS_1;
//    UartHandle.Init.Parity = UART_PARITY_NONE;
//    UartHandle.Init.Mode = UART_MODE_TX_RX;
//    UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
//
//    if (HAL_UART_Init(&UartHandle) != HAL_OK) {
//        while (true) {}
//    }
//    return 0;
//}
//
//INIT_BOARD_EXPORT(uart_init);

void rt_hw_console_output(const char *str) {
    SEGGER_RTT_WriteString(0, str);
}

#endif

#ifdef RT_USING_FINSH

char rt_hw_console_getchar() {
    /* Note: the initial value of ch must < 0 */
    int ch = -1;

    char get = SEGGER_RTT_GetKey();

    if (get != 0) {
        return get;
    } else {
        rt_thread_mdelay(10);
    }
    return ch;
}

#endif

_Noreturn void Error_Handler() {
    __disable_irq();
    while (true) {
        rt_thread_mdelay(500);
    }
}

