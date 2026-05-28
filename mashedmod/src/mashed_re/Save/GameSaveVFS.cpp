// Mashed RE — Save subsystem: VFS file-exists router and autosave trigger.
// Session: c3-batch-e-s2 (2026-05-15)
//
// ─── 0x00550b00  VfsFileExists ────────────────────────────────────────────────
// ~186 bytes. Single param: char* filename.
// Branch A (DAT_007dc75c != 0, VFS manager active):
//   1. Call vtable fn at DAT_007d3ff8+0xf4 (param_1) → scan-length uVar1.
//   2. If uVar1 != 0: iterate 0..uVar1-1 scanning for ':' in param_1.
//      On colon at index i: strncpy(&stack_buf, param_1, i+1); null-terminate.
//      Walk linked list at DAT_007dc754 (head pointer, next at node[0]):
//        For each node: call (**(DAT_007d3ff8+0xf0))(&stack_buf, node[3]).
//        If result==0 (match): if node==NULL → return 0; else dispatch.
//      No match: use default obj at DAT_007dc76c → dispatch.
//   3. Dispatch (vtable[0x13])(obj, filename) → return result.
// Branch B (DAT_007dc75c == 0, VFS manager inactive):
//   If DAT_007dc768 == NULL → return 0.
//   Else call (*DAT_007dc768)(6) → return 0.
// Constants:
//   DAT_007dc75c (0x007dc75c): VFS-active flag
//   DAT_007dc754 (0x007dc754): linked-list head pointer
//   DAT_007dc76c (0x007dc76c): default filesystem object
//   DAT_007dc768 (0x007dc768): error callback (Branch B)
//   DAT_007d3ff8 (0x007d3ff8): vtable root; +0xf4 = scan fn; +0xf0 = compare fn
//   vtable index 0x13 (word) = byte offset 0x4c: file-exists method on fs object
//   stack buffer: 100 bytes (cStack_64 + auStack_63[99])
//   ':' character = 0x3a
//   error code 6 passed to DAT_007dc768 in Branch B [UNCERTAIN U-2331]
// Inline uncertainties in body:
//   [UNCERTAIN U-2330] vtable fn at +0xf4: result used as scan length; precise
//     semantics unknown (may be strlen or encoding-aware). Does not block C3.
// Cited from: re/analysis/save_gamesave_d2/00550b00.md (Mashed_pool8, 2026-05-06)

#include "../Core/HookSystem.h"
#include <cstdint>
#include <cstring>
#include <cstdlib>

// VFS globals (cited from 0x00550b00 body):
static constexpr std::uintptr_t kVfsActiveFlag    = 0x007dc75cu; // DAT_007dc75c: VFS-active flag
static constexpr std::uintptr_t kVfsListHead      = 0x007dc754u; // DAT_007dc754: linked-list head ptr
static constexpr std::uintptr_t kVfsDefaultObj    = 0x007dc76cu; // DAT_007dc76c: default filesystem object
static constexpr std::uintptr_t kVfsErrorCb       = 0x007dc768u; // DAT_007dc768: error callback (Branch B)
static constexpr std::uintptr_t kVfsVtableRoot    = 0x007d3ff8u; // DAT_007d3ff8: vtable root pointer
static constexpr std::uintptr_t kVtableScanOffset = 0xf4u;       // vtable byte-offset for scan fn
static constexpr std::uintptr_t kVtableCmpOffset  = 0xf0u;       // vtable byte-offset for compare fn
static constexpr std::uintptr_t kVtableExistsIdx  = 0x13u;       // vtable word-index for file-exists method
static constexpr char            kColonChar        = ':';         // 0x3a: drive-prefix separator
static constexpr int             kErrorArg         = 6;           // Branch B error code [UNCERTAIN U-2331]
static constexpr std::size_t     kStackBufSize     = 100u;        // cStack_64 + auStack_63[99]

// Convenience: read a uint32 at a raw address.
static inline std::uint32_t readU32(std::uintptr_t addr) {
    return *reinterpret_cast<std::uint32_t*>(addr);
}

// 0x00550b00
extern "C" __declspec(dllexport) int __cdecl VfsFileExists(char* param_1) {
    // Load VFS-active flag.
    if (readU32(kVfsActiveFlag) == 0u) {
        // Branch B: VFS manager inactive.
        const std::uintptr_t cb = readU32(kVfsErrorCb);
        if (cb == 0u) {
            return 0;
        }
        // Call error callback with code 6 [UNCERTAIN U-2331].
        reinterpret_cast<int(__cdecl*)(int)>(cb)(kErrorArg);
        return 0;
    }

    // Branch A: VFS manager active.
    // Load vtable root.
    const std::uintptr_t vtable_root = readU32(kVfsVtableRoot);

    // Call scan fn at vtable root + 0xf4 [UNCERTAIN U-2330]: get scan length.
    const auto scan_fn = *reinterpret_cast<int(__cdecl**)(char*)>(vtable_root + kVtableScanOffset);
    const int uVar1 = scan_fn(param_1);

    // Pointer to the dispatch target object (default: DAT_007dc76c value).
    std::uint32_t* puVar4 = reinterpret_cast<std::uint32_t*>(readU32(kVfsDefaultObj));

    if (uVar1 != 0) {
        // Scan param_1[0..uVar1-1] for ':'.
        char stack_buf[kStackBufSize];
        bool colon_found = false;

        for (int i = 0; i < uVar1; ++i) {
            if (param_1[i] == kColonChar) {
                // Copy i+1 bytes of prefix to stack buffer, null-terminate after colon.
                std::strncpy(stack_buf, param_1, static_cast<std::size_t>(i + 1));
                stack_buf[i] = '\0'; // auStack_63[uVar5] = 0 (null at colon position)

                // Walk linked list at DAT_007dc754.
                const auto cmp_fn = *reinterpret_cast<int(__cdecl**)(char*, std::uint32_t*)>(
                    vtable_root + kVtableCmpOffset);
                puVar4 = reinterpret_cast<std::uint32_t*>(readU32(kVfsListHead));
                while (puVar4 != nullptr) {
                    // node[3] = puVar4[3]: node's string field at offset 0x0c.
                    if (cmp_fn(stack_buf, reinterpret_cast<std::uint32_t*>(puVar4[3])) == 0) {
                        // Match found.
                        if (puVar4 == nullptr) {
                            return 0;
                        }
                        goto dispatch;
                    }
                    // Follow next-pointer at node[0].
                    puVar4 = reinterpret_cast<std::uint32_t*>(puVar4[0]);
                }
                colon_found = true;
                break;
            }
        }
        (void)colon_found;
        // After loop (no match found in list): fall through to default obj dispatch.
        puVar4 = reinterpret_cast<std::uint32_t*>(readU32(kVfsDefaultObj));
    }

    if (puVar4 == nullptr) {
        return 0;
    }

dispatch:
    // Original: (*(code*)puVar4[0x13])(puVar4, filename) — the exists fn-ptr is a
    // DIRECT function-pointer FIELD at object word-index 0x13 (byte 0x4c), a C-style
    // object, NOT a C++ vtable.
    // 2026-05-27 FIX: the prior reimpl derefed puVar4[0] as a "vtable" first (a
    // spurious extra indirection) and called *(*(puVar4)+0x4c) — a garbage ptr —
    // so VfsFileExists wrongly returned 0 ("not found"). That broke the font-TXD
    // piz search (VfsFileExists("fgdc20.txd")=0 vs original=1) -> no TXD load ->
    // NULL font ctx -> ~94s title crash. Now reads puVar4[0x13] directly, matching
    // the original. See re/analysis/menu_crash_scoping/REPORT.md.
    const auto exists_fn = reinterpret_cast<int(__cdecl*)(std::uint32_t*, char*)>(
        static_cast<std::uintptr_t>(puVar4[kVtableExistsIdx]));
    return exists_fn(puVar4, param_1);
}

RH_ScopedInstall(VfsFileExists, 0x00550b00);  // re-enabled 2026-05-24 batch-save-f


// ─── 0x004099a0  AutosaveTrigger ──────────────────────────────────────────────
// ~54 bytes. void(void). Initiates async autosave when autosave context is set.
// Called after championship track completion.
//
// Guard: if DAT_008a95ac == 0 → return (autosave disabled/no context).
// If non-NULL:
//   wprintf("starting autosave\n") — string at 0x005cc7dc.
//   DAT_008a9584 = 4  — autosave state: "pending write" [UNCERTAIN U-1550]
//   DAT_008a9594 = 1  — write-request flag.
//   DAT_008a95b0 = 0  — autosave offset/counter reset.
// Returns.
// Constants (all cited from 0x004099a0 body):
//   0x008a95ac: autosave-context pointer (NULL = disabled)
//   0x005cc7dc: "starting autosave\n" wide string
//   0x008a9584: autosave state global; set to 4 [UNCERTAIN U-1550]
//   0x008a9594: write-request flag; set to 1
//   0x008a95b0: autosave offset counter; reset to 0
// Cited from: re/analysis/profile_career/004099a0.md (Mashed_pool9, 2026-05-03)

#include <cwchar>

static constexpr std::uintptr_t kAutosaveCtxPtr  = 0x008a95acu; // autosave-context ptr (guard)
static constexpr std::uintptr_t kAutosaveState   = 0x008a9584u; // autosave state global
static constexpr std::uintptr_t kWriteReqFlag    = 0x008a9594u; // write-request flag
static constexpr std::uintptr_t kAutosaveOffset  = 0x008a95b0u; // autosave offset counter
static constexpr std::uintptr_t kAutosaveStr     = 0x005cc7dcu; // "starting autosave\n" (wide)

// 0x004099a0
extern "C" __declspec(dllexport) void __cdecl AutosaveTrigger() {
    // Guard: check autosave context pointer.
    if (readU32(kAutosaveCtxPtr) == 0u) {
        return;
    }

    // wprintf("starting autosave\n") — string at 0x005cc7dc.
    std::wprintf(reinterpret_cast<const wchar_t*>(kAutosaveStr));

    // Set autosave state to 4 [UNCERTAIN U-1550].
    *reinterpret_cast<std::uint32_t*>(kAutosaveState)  = 4u;   // 0x004099b7
    // Set write-request flag to 1.
    *reinterpret_cast<std::uint32_t*>(kWriteReqFlag)   = 1u;   // 0x004099c0
    // Reset autosave offset counter to 0.
    *reinterpret_cast<std::uint32_t*>(kAutosaveOffset) = 0u;   // 0x004099c9
}

RH_ScopedInstall(AutosaveTrigger, 0x004099a0);  // re-enabled 2026-05-24 batch-save-b
