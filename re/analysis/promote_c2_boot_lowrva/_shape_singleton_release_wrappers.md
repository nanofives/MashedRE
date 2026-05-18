---
shape_name: singleton_release_wrapper
candidates: 4
session_bucket: promote_c2_boot_lowrva
session_date: 2026-05-18
---

# Shape: singleton-release-wrapper

## Definition

A 12-byte function with this exact body:

```
void FUN_XXXXXXXX(void)
{
    FUN_004e6e00(DAT_YYYYYYYY);
    return;
}
```

Single instruction calls `FUN_004e6e00` (canonical RW resource release helper) on a fixed global handle, then returns.

## Members in this batch

| RVA       | Released global   | Note |
|-----------|-------------------|------|
| 0x0041a3d0 | DAT_0063c620      | BTBPanel dff handle (cf. 0x0041a1e0 init plate, +0x10 of BTBPanel_Record) |
| 0x0041c2c0 | DAT_0063cdb4      | endpoint cluster singleton (init not in this batch) |
| 0x0041da80 | DAT_0063d57c      | teameppanel cluster tail singleton (init not in this batch) |
| 0x0041de70 | DAT_0063d5e0      | mid-cluster singleton (init not in this batch) |

## Adjacent but NOT in this shape

- `0x00425ed0` releases `DAT_00644150` via **`FUN_004c5930`** (TXD-shape helper), not `FUN_004e6e00` → different shape group ("singleton-TXD-release-wrapper").

## Caller pattern

All 4 in-shape members are called from `FUN_00402a40` (boot teardown, this batch). They appear in this order in the teardown:
1. `FUN_0041a3d0()` — BTBPanel
2. `FUN_0041de70()` — mid-cluster
3. `FUN_0041c2c0()` — endpoint
4. `FUN_0041da80()` — teameppanel tail

Suggests they are siblings in a teardown dispatch table, each owning one singleton.

## Reimplementation note for greenfield port

These 4 wrappers can collapse to a single template function or even inline at their call sites:

```cpp
inline void ReleaseSingleton(void*& handle) {
    if (handle) {                       // FUN_004e6e00 itself handles null per convention (TBD: verify)
        FUN_004e6e00(handle);
        handle = nullptr;
    }
}
```

NOTE: the original wrappers do NOT zero the global after release. If `FUN_004e6e00` is tolerant of double-free, the absence of the re-zero may be intentional (single-shot teardown only at shutdown). If not, the original has a latent double-free risk on re-entry.

[UNCERTAIN] `FUN_004e6e00`'s null-handling and idempotency.
