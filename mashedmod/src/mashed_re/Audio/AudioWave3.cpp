// Mashed RE — Audio subsystem Wave 3 mixed-candidate cluster.
// Session: wave3-s4   Branch: c3-sweep/wave3-s4
//
// Two audio utility functions from the RWS loader depth-3 callees.
// Both are C1 mechanical descriptions; authored here at C2 quality.
//
// Analysis notes:
//   re/analysis/audio_rws_loader_d3/005ade60.md
//   re/analysis/audio_rws_loader_d3/005a7b00.md

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// Callee prototypes (called via absolute game VA, no thunk needed for C2).
// ---------------------------------------------------------------------------

// 0x005a7a40  FUN_005a7a40 — pool-object list searcher.
// Searches the doubly-linked list at param_1+0x0c; returns matching obj ptr or NULL.
typedef int* (__cdecl* FnAudioPoolSearch)(int* list_head, int key);

// 0x005addd0  FUN_005addd0 — doubly-linked list insert into DAT_009146c0 pool.
// param_1 = list head; param_2 = new node.
typedef void (__cdecl* FnListInsert)(int* list_head, int* node);

static inline int* Call005a7a40(int* list_head, int key) {
    return reinterpret_cast<FnAudioPoolSearch>(0x005a7a40)(list_head, key);
}
static inline void Call005addd0(int* list_head, int* node) {
    reinterpret_cast<FnListInsert>(0x005addd0)(list_head, node);
}

// ─────────────────────────────────────────────────────────────────────────────
// 0x005ade60  FUN_005ade60  AudioCircularListSearch  (C1 → C2 candidate)
//
// Forward-scanning circular linked-list index search.
//
// param_1 = list head pointer.
//           *(param_1+4) = first node (sentinel: loops back to param_1).
// param_2 = search key (compared against node+8).
// Returns: 0-based index of the first matching node, or -1 if not found.
//
// List layout (confirmed from audio_rws_loader_d3/005ade60.md):
//   list_head + 0x00: (unused sentinel data)
//   list_head + 0x04: first node ptr
//   node      + 0x04: next node ptr
//   node      + 0x08: data value (key)
//
// Called by FUN_005a7a40 with arg1 = audio_obj+0x0c (embedded list head).
//
// ASM (0x005ade60..0x005ade8a):
//   005ade60: XOR  EDI, EDI          ; iVar2 = 0
//   005ade62: MOV  EAX, [param_1+4]  ; iVar1 = *(param_1+4) first node
//   005ade65: CMP  EAX, param_1      ; if node == head → not found
//   005ade67: JE   return_neg1
//   005ade69: CMP  [EAX+8], param_2  ; if node+8 == key → found
//   005ade6c: JE   return_idx
//   005ade6e: MOV  EAX, [EAX+4]     ; advance to next node
//   005ade71: INC  EDI               ; iVar2++
//   005ade72: JMP  check
//   return_neg1: MOV EAX, -1; RET
//   return_idx:  MOV EAX, EDI; RET
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl AudioCircularListSearch(int param_1, int param_2) {
    int iVar2 = 0;
    int iVar1 = *reinterpret_cast<int*>(param_1 + 4); // first node = *(head+4)
    while (true) {
        if (iVar1 == param_1) return -1;                           // wrapped back to head: not found
        if (*reinterpret_cast<int*>(iVar1 + 8) == param_2) break; // node+8 == key: found
        iVar1 = *reinterpret_cast<int*>(iVar1 + 4);               // advance: node = node->next
        iVar2++;
    }
    return iVar2; // 0-based index of matching node
}

RH_ScopedInstall(AudioCircularListSearch, 0x005ade60);

// ─────────────────────────────────────────────────────────────────────────────
// 0x005a7b00  FUN_005a7b00  AudioContextInsert  (C1 → C2 candidate)
//
// Audio context node insert with duplicate-check.
//
// Checks whether param_2 is already in the circular list at param_1+0x0c.
// If NOT already present (FUN_005a7a40 returns NULL), inserts param_2 via
// FUN_005addd0(param_1+0x0c, param_2).
// Returns param_1 on success (node inserted or already present).
//
// NOTE: The analysis note (audio_rws_loader_d3/005a7b00.md) states the function
// returns param_1 in both cases. The early-exit path (already present) appears
// to return NULL or 0 — but the note says "returns param_1 on success"; the
// already-present path was not fully traced in that session. Carrying the
// [UNCERTAIN U-5010] marker: the return value on the already-present path.
//
// Callees (from analysis note):
//   0x005a7a40  FUN_005a7a40  audio  C1  pool-object list searcher
//   0x005addd0  FUN_005addd0  audio  C1  doubly-linked list insert
//
// ASM (0x005a7b00..0x005a7b2e, ~47 bytes):
//   005a7b00: PUSH  param_2
//   005a7b01: LEA   EAX, [param_1+0x0c]   ; list head embedded at +0x0c
//   005a7b04: PUSH  EAX
//   005a7b05: CALL  FUN_005a7a40           ; search
//   005a7b0a: ADD   ESP, 8
//   005a7b0d: TEST  EAX, EAX
//   005a7b0f: JNZ   already_present        ; non-null = found = skip insert
//   005a7b11: PUSH  param_2
//   005a7b12: LEA   EAX, [param_1+0x0c]
//   005a7b15: PUSH  EAX
//   005a7b16: CALL  FUN_005addd0           ; insert
//   005a7b1b: ADD   ESP, 8
//   already_present:
//   005a7b1e: MOV   EAX, param_1
//   005a7b21: RET
// ─────────────────────────────────────────────────────────────────────────────
extern "C" __declspec(dllexport) int __cdecl AudioContextInsert(int param_1, int param_2) {
    // Check if param_2 is already in the list at param_1+0x0c
    int* list_head = reinterpret_cast<int*>(param_1 + 0x0c);
    int* found = Call005a7a40(list_head, param_2);
    if (found == nullptr) {
        // Not yet present: insert
        Call005addd0(list_head, reinterpret_cast<int*>(param_2));
    }
    // [UNCERTAIN U-5010] return value on already-present path:
    // analysis note says "returns param_1"; both paths fall through to MOV EAX, param_1.
    return param_1;
}

RH_ScopedInstall(AudioContextInsert, 0x005a7b00);
