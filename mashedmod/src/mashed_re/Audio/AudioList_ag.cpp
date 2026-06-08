// Mashed RE - c3_batch_ag harness-ext: intrusive circular doubly-linked list ops
// (generic container primitive reused by the audio module; see U-6906).
// Header layout: [0]=count, sentinel node embedded @header+4 (sentinel.next@+0,
// sentinel.prev/tail@+8). Object layout: link node @object+0x20 (node.next@+0x20,
// node.prev-holder@+0x24), so object = node-0x20; compared field @object+0x18.
// All four are pure leaves (no callees).  Decomp pool13 2026-06-08 (bit-faithful;
// matches re/analysis/bucket_audio_005b2220_005b8570/0x005b35{80,a0,70,b0... }.md).
#include "../Core/HookSystem.h"
#include <cstdint>

// 0x005b3580  FUN_005b3580  (18 bytes)  void(u32* hdr)
// Init: count=0; tail(header[2])=&sentinel; sentinel.next=&sentinel (empty ring).
extern "C" __declspec(dllexport) void __cdecl AudioListInit(uint32_t* param_1) {
    uint32_t* puVar1 = param_1 + 1;
    param_1[0] = 0;
    param_1[2] = reinterpret_cast<uint32_t>(puVar1);
    *puVar1 = reinterpret_cast<uint32_t>(puVar1);
}

RH_ScopedInstall(AudioListInit, 0x005b3580);  // c3_batch_ag harness-ext 2026-06-08

// 0x005b35a0  FUN_005b35a0  (50 bytes)  void(int* hdr, int obj)
// Push-back: link obj's embedded node (@obj+0x20 next / +0x24 prev) before the
// sentinel (append at tail of the circular list); increment count.
extern "C" __declspec(dllexport) void __cdecl AudioListPushBack(int* param_1, int param_2) {
    int* piVar1 = param_1 + 1;
    int* piVar2 = reinterpret_cast<int*>(param_2 + 0x20);
    *reinterpret_cast<uint32_t*>(param_2 + 0x24) = 0;
    *piVar2 = 0;
    *piVar2 = *piVar1;
    *reinterpret_cast<int**>(param_2 + 0x24) = piVar1;
    *reinterpret_cast<int**>(*piVar1 + 4) = piVar2;
    *piVar1 = reinterpret_cast<int>(piVar2);
    *param_1 = *param_1 + 1;
}

RH_ScopedInstall(AudioListPushBack, 0x005b35a0);  // c3_batch_ag harness-ext 2026-06-08

// 0x005b3670  FUN_005b3670  (49 bytes)  int*(int hdr, int key)
// Find-by-field: returns the first object whose field @object+0x18 (node[-2])
// equals key; NULL if none. Walks node.next (offset 0) until the sentinel @hdr+4.
extern "C" __declspec(dllexport) int* __cdecl AudioListFind(int param_1, int param_2) {
    int* piVar2 = *reinterpret_cast<int**>(param_1 + 4);
    int* piVar1 = piVar2 + -8;
    if (piVar2[-2] != param_2) {
        while (piVar2 = reinterpret_cast<int*>(*piVar2), piVar2 != reinterpret_cast<int*>(param_1 + 4)) {
            if (piVar2[-2] == param_2) {
                return piVar2 + -8;
            }
        }
        piVar1 = reinterpret_cast<int*>(0x0);
    }
    return piVar1;
}

RH_ScopedInstall(AudioListFind, 0x005b3670);  // c3_batch_ag harness-ext 2026-06-08

// 0x005b36b0  FUN_005b36b0  (56 bytes)  int*(int* hdr, int pos)
// Index-by-position: returns the pos-th object (0-based), NULL if out-of-range.
extern "C" __declspec(dllexport) int* __cdecl AudioListAt(int* param_1, int param_2) {
    if ((param_2 <= *param_1) && (*param_1 != 0)) {
        int* piVar1 = reinterpret_cast<int*>(param_1[1]);
        int iVar2 = 0;
        if (param_2 < 1) {
            return piVar1 + -8;
        }
        while (piVar1 = reinterpret_cast<int*>(*piVar1), piVar1 != param_1 + 1) {
            iVar2 = iVar2 + 1;
            if (param_2 <= iVar2) {
                return piVar1 + -8;
            }
        }
    }
    return reinterpret_cast<int*>(0x0);
}

RH_ScopedInstall(AudioListAt, 0x005b36b0);  // c3_batch_ag harness-ext 2026-06-08
