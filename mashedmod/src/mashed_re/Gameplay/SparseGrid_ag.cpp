// Mashed RE - c3_batch_ag harness-ext: sparse 8x8-cell grid writer + eraser.
// Pure leaves operating on three fixed globals (no callees):
//   DAT_007f1a9c (per-cell slot-index table, short[]), DAT_007f9a9c (slot data,
//   0x40 bytes/slot), DAT_00801a9c (high-water slot index, int).
#include "../Core/HookSystem.h"
#include <cstdint>

namespace {
inline short* CellTable() { return reinterpret_cast<short*>(0x007f1a9cu); }
inline char*  SlotData()  { return reinterpret_cast<char*>(0x007f9a9cu); }
inline int*   HighWater() { return reinterpret_cast<int*>(0x00801a9cu); }
}

// 0x00417450  FUN_00417450  (209 bytes)  uint(uint x, uint y, byte v) -> 1/0
// Maps (x,y) to a cell; lazily allocates a 0x40-byte slot (first all-free block,
// slots 1..0x1ff) on first write; stores the byte at the sub-cell. Returns 1 on
// success, 0 on out-of-range / table-full.  Decomp pool13 2026-06-08 (bit-faithful).
extern "C" __declspec(dllexport) uint32_t __cdecl SparseGridCellWrite(uint32_t x, uint32_t y, uint8_t v) {
    short* const cell = CellTable();
    char*  const data = SlotData();
    int iVar3 = (int)(x + 0x200 + ((int)(x + 0x200) >> 0x1f & 7u)) >> 3;
    if ((-1 < iVar3) && (iVar3 < 0x80)) {
        int iVar2 = (int)(y + 0x200 + ((int)(y + 0x200) >> 0x1f & 7u)) >> 3;
        if ((-1 < iVar2) && (iVar2 < 0x80)) {
            iVar2 = iVar3 * 0x80 + iVar2;
            short sVar1 = cell[iVar2];
            if (0 < sVar1) goto write;
            for (sVar1 = 1; sVar1 < 0x200; sVar1 = sVar1 + 1) {
                int j = 0;
                while (true) {
                    if (0x3f < j) {
                        if (0x1ff < sVar1) return 0;
                        cell[iVar2] = sVar1;
                        goto write;
                    }
                    if (static_cast<signed char>(data[sVar1 * 0x40 + j]) != -1) break;
                    j = j + 1;
                }
            }
            return 0;
        write:
            {
                const int slot = (int)sVar1;
                if (*HighWater() < slot) *HighWater() = slot;
                data[((y & 7) + slot * 8) * 8 + (x & 7)] = static_cast<char>(v);
                return 1;
            }
        }
    }
    return 0;
}

RH_ScopedInstall(SparseGridCellWrite, 0x00417450);  // c3_batch_ag harness-ext 2026-06-08

// 0x00417530  FUN_00417530  (265 bytes)  uint(uint x, uint y) -> 1/0
// Eraser/clear twin of the writer: clears the sub-cell byte to 0xff; if the whole
// 0x40-byte slot is then free, releases the cell's slot index back to 0xffff.
// Returns 1 on success/no-op, 0 on out-of-range.  Decomp pool13 2026-06-08.
extern "C" __declspec(dllexport) uint32_t __cdecl SparseGridCellErase(uint32_t x, uint32_t y) {
    short* const cell = CellTable();
    char*  const data = SlotData();
    int iVar4 = (int)(x + 0x200 + ((int)(x + 0x200) >> 0x1f & 7u)) >> 3;
    if ((-1 < iVar4) && (iVar4 < 0x80)) {
        int iVar3 = (int)(y + 0x200 + ((int)(y + 0x200) >> 0x1f & 7u)) >> 3;
        if ((-1 < iVar3) && (iVar3 < 0x80)) {
            iVar3 = iVar4 * 0x80 + iVar3;
            short sVar1 = cell[iVar3];
            if (sVar1 < 1) return 1;
            data[((y & 7) + sVar1 * 8) * 8 + (x & 7)] = static_cast<char>(0xff);
            bool used = false;
            const int iVar5 = sVar1 * 0x40;
            for (int row = 0; row < 0x40; row = row + 8) {
                for (int k = 0; k < 8; ++k) {
                    if (static_cast<signed char>(data[iVar5 + row + k]) != -1) used = true;
                }
            }
            if (!used) cell[iVar3] = static_cast<short>(0xffff);
            return 1;
        }
    }
    return 0;
}

RH_ScopedInstall(SparseGridCellErase, 0x00417530);  // c3_batch_ag harness-ext 2026-06-08
