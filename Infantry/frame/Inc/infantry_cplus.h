/**
 * @file Infantry/Src
 * @author CharlesHsu
 * @date 11/8/2023
 */
#pragma once

#include "plugins/logger/Inc/infantry_log.h"
#include "infantry_config.h"
#include "rtthread.h"
#include <cstdlib>

#if INFANTRY_CONSOLE_MEM == INFANTRY_ON
#define inew new (__FILE__, __LINE__)

void *operator new(size_t size, const char *file, int line) noexcept;

void *operator new[](size_t size, const char *file, int line) noexcept;

void *operator new(size_t size);

void *operator new[](size_t size);

void operator delete(void *ptr) noexcept;

void operator delete[](void *ptr) noexcept;

#else
#define inew new

void *operator new(size_t size);

void *operator new[](size_t size);

void operator delete(void *ptr) noexcept;

void operator delete[](void *ptr) noexcept;

#endif

extern "C" void __cxa_pure_virtual();
