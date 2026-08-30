#pragma once

#include "hw/types.h"
#include "hw/memory_map.h"

/*
 * Omni10 – minimal IO accessors
 * Concrete peripheral bases filled in as drivers are written.
 */

#define REG32(addr) (*(vu32*)(uintptr_t)(addr))
#define REG16(addr) (*(vu16*)(uintptr_t)(addr))
#define REG8(addr)  (*(vu8*)(uintptr_t)(addr))

/* IO window */
#define IO_BASE OMNI_IO_BASE

/*
 * Common peripheral groups (offsets relative to documented maps).
 * Exact register lists added per-driver (SDMMC, HID, I2C, …).
 */
#define OMNI_IO_PAD(offset) (IO_BASE + (offset))
