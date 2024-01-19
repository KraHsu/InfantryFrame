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

#define LOG_TAG "cplus"

#include "frame/Inc/infantry_cplus.h"
#include "infantry_config.h"
#include "rtthread.h"

#if INFANTRY_CONSOLE_MEM == INFANTRY_ON

void *operator new(size_t size, const char *file, int line) noexcept {
    if (size == 0) {
        return nullptr;
    }
    void *ptr = rt_malloc(size);
    if (ptr) {
        logVerbose(
                "new size(%d) at (%p) in line(%d) of file(%s)", size, ptr, line, file
        );
    } else {
        logVerbose(
                "Memory allocation failure at (%p) in line(%d) of file(%s)", ptr, line, file
        );
    }
    return ptr;
}

void *operator new[](size_t size, const char *file, int line) noexcept {
    if (size == 0) {
        return nullptr;
    }
    void *ptr = rt_malloc(size);
    if (ptr) {
        logVerbose(
                "new[%d] at (%p) in line(%d) of file(%s)", size, ptr, line, file
        );
    } else {
        logVerbose(
                "Memory allocation failure at (%p) in line(%d) of file(%s)", ptr, line, file
        );
    }
    return ptr;
}

void *operator new(size_t size) {
    if (size == 0) {
        return nullptr;
    }
    void *ptr = rt_malloc(size);
    if (ptr) {
        logVerbose("new size(%d) at (%p)", size, ptr);
    } else {
        logVerbose(
                "Memory allocation failure at (%p) in line(%d) of file(%s)", ptr
        );
    }
    return ptr;
}

void *operator new[](size_t size) {
    if (size == 0) {
        return nullptr;
    }
    void *ptr = rt_malloc(size);
    if (ptr) {
        logVerbose("new[%d] at (%p)", size, ptr);
    } else {
        logVerbose(
                "Memory allocation failure at (%p) in line(%d) of file(%s)", ptr
        );
    }
    return ptr;
}

void operator delete(void *ptr) noexcept {
    logVerbose("delete (%p)", ptr);
    rt_free(ptr);
}

void operator delete[](void *ptr) noexcept {
    logVerbose("delete (%p)", ptr);
    rt_free(ptr);
}

#else

void *operator new(size_t size) {
    if (size == 0) {
        return nullptr;
    }
    return rt_malloc(size);
}

void *operator new[](size_t size) {
    if (size == 0) {
        return nullptr;
    }
    return rt_malloc(size);
}

void operator delete(void *ptr) noexcept { rt_free(ptr); }

void operator delete[](void *ptr) noexcept { rt_free(ptr); }

#endif

void __cxa_pure_virtual() {
    logError("Illegal to call a pure virtual function.");
}
