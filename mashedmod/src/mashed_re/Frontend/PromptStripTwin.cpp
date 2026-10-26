// Mashed RE — hook-side twin of FUN_00432b30 (prompt-strip glyph row
// builder; the bottom-bar back/forward/L-R indicator row appended to the
// 0x00898ac0 record array by FUN_0043d2a0 Phase 7). Transcribed VERBATIM
// from re/analysis/standalone_menu_sm/FUN_00432b30_full.asm + the five
// dumped jump tables (0x433060/88/b0/d8/100/128/150/178).
//
// True ABI: EAX = mode (0/2/other head branch), ESI = rec-index ptr,
// stack = (screen_kind, b920_compare). The exported naked thunk marshals
// to the cdecl body. Diff: re/frida/menu_prompt_diff.py (nav-driven table
// compare with JMP patch-in per sample).
//
// D4-WSH-EXEFILE (2026-10-26): compiled into BOTH targets. The jump-table
// dispatch (PromptStripTag + PromptStripResolve) is the one body for the
// exe's Frontend/MenuNavSM.cpp PromptStripAppend too. Target-specific: the two
// getter calls (the .asi calls the original's FUN_0042a9c0 / FUN_0042b920, the
// exe the ported ModeCodeLookup / ConstantGetter22), and everything that
// touches the live 0x00898ac0 table (the mode head, FUN_0042ad10, the stores),
// which stays .asi-only; the exe's head lives in its Nav caller.
#include <cstdint>

#include "../Core/HookSystem.h"
#include "PromptStrip.h"

#ifdef MASHED_STANDALONE
extern "C" std::uint32_t __cdecl ModeCodeLookup();     // FUN_0042a9c0 (Frontend/BatchAA_s4.cpp)
extern "C" std::uint32_t __cdecl ConstantGetter22();   // FUN_0042b920 (Frontend/MenuNearLeaves_s6.cpp)
#endif

namespace {

#ifdef MASHED_STANDALONE
inline int oA9C0() { return static_cast<int>(ModeCodeLookup()); }
inline int oB920() { return static_cast<int>(ConstantGetter22()); }
#else
template <typename T>
inline T& At(std::uintptr_t a) { return *reinterpret_cast<T*>(a); }

constexpr std::uintptr_t kDepth = 0x0067e9f8;
// record fields by index (stride 0x34)
inline std::int32_t* RecType(int i)  { return &At<std::int32_t>(0x00898ac4 + unsigned(i) * 0x34); }
inline std::int32_t* RecSlide(int i) { return &At<std::int32_t>(0x00898ad0 + unsigned(i) * 0x34); }
inline std::int32_t* RecSec(int i)   { return &At<std::int32_t>(0x00898ae8 + unsigned(i) * 0x34); }
inline std::int32_t* RecPrim(int i)  { return &At<std::int32_t>(0x00898aec + unsigned(i) * 0x34); }

// FUN_0042add0: kv iterator (EBX = tag key, EDI = occurrence) -> EAX
static int Add0_FF080000() {
    int r;
    __asm { mov ebx, 0xff080000
            xor edi, edi
            mov ecx, 0x0042add0
            call ecx
            mov r, eax }
    return r;
}
// FUN_0042ad10: glyph slot init — EAX = rec-index ptr, EDX = tag,
// stack (0x40, 0x1ac, 0), caller-cleaned.
static void Ad10(void* idx_ptr, std::uint32_t tag) {
    __asm { push 0
            push 0x1ac
            push 0x40
            mov eax, idx_ptr
            mov edx, tag
            mov ecx, 0x0042ad10
            call ecx
            add esp, 0xc }
}
using IntFn = int(__cdecl*)();
auto* const oB920 = reinterpret_cast<IntFn>(0x0042b920);
auto* const oA9C0 = reinterpret_cast<IntFn>(0x0042a9c0);   // ModeCodeLookup C3
#endif  // MASHED_STANDALONE

// finisher codes
enum Fin { kZero, kFrozen, kDefault };
struct Cell { int prim; bool a9c0; bool dbl48; Fin fin; };
// prim -2 = call a9c0; -3 = 0x48 then 0x13 (double write, net 0x13)
constexpr Cell C(int p, Fin f) { return Cell{p, false, false, f}; }
constexpr Cell CA(Fin f)       { return Cell{0, true, false, f}; }
constexpr Cell CD(Fin f)       { return Cell{0, false, true, f}; }
constexpr Cell FRZ()           { return Cell{-1, false, false, kFrozen}; }
constexpr Cell DEF()           { return Cell{-1, false, false, kDefault}; }

struct KindRow {
    std::uint32_t tag;
    int sec;           // -2 = a9c0(); -3 = 0x48-then-0x13; -4 = 0x42 w/ b920 gate
    Cell cells[10];
};
// Jump tables transcribed literally (index = relation-1; out-of-range = DEF).
// Ground truth: table dwords read from MASHED.exe at 0x433060 (outer, 10) +
// 0x433088/b0/d8/100/128/150/178 (inner, 10 each); every cell body walked
// linearly in FUN_00432b30_full.asm (writes to +0x898ac4/+0x898ad0/
// +0x898ae8/+0x898aec, ebx=-1, ebp=0). 2026-06-11 re-derivation fixed four
// rows the first transcription got wrong (key5/6/8 frozen-cell position,
// key10 cell[1]).
static const KindRow kRows[10] = {
    /*key1 */ {0xff100000u, -4, {FRZ(), C(0x43,kZero), DEF(), C(0x225,kZero), CD(kZero), C(0x58,kZero), DEF(), CA(kZero), DEF(), C(0x133,kZero)}},
    /*key2 */ {0xff100000u, 0x43, {C(0x42,kZero), FRZ(), DEF(), C(0x225,kZero), CD(kZero), C(0x58,kZero), DEF(), CA(kZero), DEF(), C(0x133,kZero)}},
    /*key3 */ {0, 0, {DEF(),DEF(),DEF(),DEF(),DEF(),DEF(),DEF(),DEF(),DEF(),DEF()}},   // ja -> ret (no row)
    /*key4 */ {0xff100000u, 0x225, {C(0x42,kZero), C(0x43,kZero), DEF(), FRZ(), CD(kZero), C(0x58,kZero), DEF(), CA(kZero), DEF(), C(0x133,kZero)}},
    /*key5 */ {0xff100000u, -3, {C(0x42,kZero), C(0x43,kZero), DEF(), C(0x225,kZero), CD(kZero), C(0x58,kZero), DEF(), FRZ(), DEF(), C(0x133,kZero)}},
    /*key6 */ {0xff110000u, 0x58, {C(0x42,kZero), C(0x43,kZero), DEF(), C(0x225,kZero), FRZ(), C(0x58,kZero), DEF(), CA(kZero), DEF(), C(0x133,kZero)}},
    /*key7 */ {0, 0, {DEF(),DEF(),DEF(),DEF(),DEF(),DEF(),DEF(),DEF(),DEF(),DEF()}},
    /*key8 */ {0xff230000u, -2, {C(0x42,kZero), C(0x43,kZero), DEF(), C(0x225,kZero), CD(kZero), FRZ(), DEF(), CA(kZero), DEF(), C(0x133,kZero)}},
    /*key9 */ {0, 0, {DEF(),DEF(),DEF(),DEF(),DEF(),DEF(),DEF(),DEF(),DEF(),DEF()}},
    /*key10*/ {0xff100000u, 0x133, {C(0x42,kZero), C(0x43,kZero), DEF(), C(0x225,kZero), CD(kZero), C(0x58,kZero), DEF(), CA(kZero), DEF(), FRZ()}},
};

}  // namespace

// 0x00432b30 jump-table dispatch (from 0x00432b97): key -> the row's tag, or 0
// when no row is appended (key out of 1..10: ja 0x43305b; keys 3/7/9: ja default).
extern "C" std::uint32_t __cdecl PromptStripTag(int key) {
    const unsigned k = static_cast<unsigned>(key) - 1u;
    if (k > 9u) return 0;
    return kRows[k].tag;
}

// The inner dispatch: (key, b920_cmp, rel) -> the four stores FUN_00432b30
// makes after FUN_0042ad10. Call only when PromptStripTag(key) != 0.
extern "C" void __cdecl PromptStripResolve(int key, int b920_cmp, int rel,
                                           PromptStripRow* out) {
    const KindRow& row = kRows[static_cast<unsigned>(key) - 1u];
    if (row.sec == -2)      out->sec = oA9C0();
    else if (row.sec == -3) { out->sec = 0x48; out->sec = 0x13; }
    else if (row.sec == -4) {
        out->sec = 0x42;                           // 0x432bbf
        if (b920_cmp == oB920()) out->sec = -1;
    } else {
        out->sec = row.sec;
    }
    const unsigned r = static_cast<unsigned>(rel) - 1u;
    const Cell cell = (r > 9u) ? DEF() : row.cells[r];
    if (cell.a9c0)        out->prim = oA9C0();
    else if (cell.dbl48)  { out->prim = 0x48; out->prim = 0x13; }
    else                  out->prim = cell.prim;
    switch (cell.fin) {
        case kZero:    out->type = 0;       out->slide = 0x1ff; break;
        case kFrozen:  out->type = 0x1000;  out->slide = 0x1ff; break;
        case kDefault: out->type = 1;       out->slide = 0;     break;
    }
}

#ifndef MASHED_STANDALONE
// cdecl body; thunk below marshals the true ABI.
extern "C" void __cdecl PromptStripTwinBody(int mode_eax, int* rec_index,
                                            int key, int b920_cmp) {
    int rel = 0;                                   // EDI
    if (mode_eax != 0) {
        if (mode_eax == 2) {
            key = 0;                               // [esp+0x10] = 0 (0x432b45)
            rel = Add0_FF080000();
        } else {
            ++At<std::int32_t>(kDepth);            // 0x432b52
            rel = Add0_FF080000();
            if (rel == -1) rel = 0;
            --At<std::int32_t>(kDepth);
        }
    } else if (At<std::int32_t>(kDepth) != 0) {    // 0x432b70
        rel = Add0_FF080000();
        if (rel == -1) rel = 0;
    }
    const std::uint32_t tag = PromptStripTag(key);
    if (tag == 0) return;
    Ad10(rec_index, tag);
    const int idx = *rec_index;
    PromptStripRow row;
    PromptStripResolve(key, b920_cmp, rel, &row);
    *RecSec(idx)   = row.sec;
    *RecPrim(idx)  = row.prim;
    *RecType(idx)  = row.type;
    *RecSlide(idx) = row.slide;
    ++(*rec_index);
}

// true-ABI entry: EAX = mode, ESI = rec-index ptr, stack (key, b920_cmp).
// Exported so menu_prompt_diff.py can JMP-patch 0x00432b30 at it per sample.
extern "C" __declspec(dllexport, naked) void PromptStripTwin(void) {
    __asm {
        push dword ptr [esp + 8]    // b920_cmp
        push dword ptr [esp + 8]    // key (orig [esp+4]; +8 after prior push)
        push esi
        push eax
        call PromptStripTwinBody
        add esp, 0x10
        ret
    }
}

RH_ScopedInstall(PromptStripTwin, 0x00432b30);
#endif  // !MASHED_STANDALONE
