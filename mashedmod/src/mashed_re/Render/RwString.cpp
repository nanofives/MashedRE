// Mashed RE - RenderWare string-size helper.
// Original: 0x004d8770  FUN_004d8770  RwStringGetSizeAligned  render  C2 -> C3
// Plate: re/analysis/render_6_c1_to_c2_s4/0x004d8770.md   (34 bytes)
//
// Returns the 4-byte-aligned on-stream size of a C string: strlen(s) rounded UP
// to the next multiple of 4, with room for the NUL. A NULL argument is
// substituted with the engine's empty-string constant at DAT_005d8d70.
//   ret = (strlen(s ? s : &DAT_005d8d70) + 4) & 0xfffffffc
//
// The strlen is NOT called directly: it goes through the RenderWare engine's
// string-function table, DAT_007d3ff8 slot +0xf4. That slot is installed by
// FUN_004d8570, which literally assigns `*(code **)(DAT_007d3ff8 + 0xf4) =
// _strlen` — so the target is confirmed, not inferred. The port dispatches
// through the same slot rather than calling the CRT strlen, because that is what
// the original does; a direct call would not be a verbatim transcription.
//
// RETURN TYPE (the brief flagged this RETURN_UNVERIFIED): resolved. The caller
// FUN_004cf4a0 consumes the result arithmetically -
//   iVar4 = FUN_004d8770(piVar1);            // texture name  (RwTexture +0x10)
//   iVar5 = FUN_004d8770(piVar2 + 0xc);      // mask name     (RwTexture +0x30)
//   FUN_004cc580(param_2, 6, iVar4 + iVar5 + 0x34 + iVar6, 0x37002, 10)
// i.e. both returns are summed into a stream chunk size. The uint return is
// therefore load-bearing, not incidental.
//
// Disasm at 0x004d8770..0x004d8791 (34 bytes; integer-only, no x87 -> plain C is
// bit-identical):
//   0x004d8770  8B 44 24 04         mov  eax, [esp+4]       ; param_1
//   0x004d8774  85 C0               test eax, eax
//   0x004d8776  75 05               jnz  0x004d877d
//   0x004d8778  B8 70 8D 5D 00      mov  eax, 0x5d8d70      ; &DAT_005d8d70
//   0x004d877d  50                  push eax
//   0x004d877e  A1 F8 3F 7D 00      mov  eax, [0x007d3ff8]  ; engine str-fn table
//   0x004d8783  FF 90 F4 00 00 00   call [eax+0xf4]         ; = _strlen
//   0x004d8789  83 C0 04            add  eax, 4
//   0x004d878c  83 C4 04            add  esp, 4             ; callee is __cdecl
//   0x004d878f  24 FC               and  al,  0xfc          ; == and eax,0xfffffffc
//   0x004d8791  C3                  ret                     ; __cdecl, 1 arg
//
// NOTE on `and al,0xfc` at 0x004d878f: it is an 8-bit AND, but 0xfc clears only
// bits 0..1, and those bits live entirely inside AL. The upper 24 bits of EAX are
// untouched and need no masking, so it is exactly equivalent to
// `and eax, 0xfffffffc`. Ghidra's `& 0xfffffffc` is correct here, not a misread.
#include "../Core/HookSystem.h"

// engine string-function table root; slot +0xf4 == _strlen (set by FUN_004d8570)
static unsigned int* const s_pRwStringFuncs = (unsigned int*)0x007d3ff8;
// fallback empty string substituted for a NULL argument
static const char* const s_pRwNullString = (const char*)0x005d8d70;

typedef unsigned int(__cdecl* RwStrlenFn)(const char*);

// 0x004d8770
extern "C" __declspec(dllexport) unsigned int __cdecl
RwStringGetSizeAligned(const char* param_1)
{
    if (param_1 == nullptr)
        param_1 = s_pRwNullString;

    const RwStrlenFn strlenFn =
        *(RwStrlenFn*)(*s_pRwStringFuncs + 0xf4);

    return (strlenFn(param_1) + 4u) & 0xfffffffcu;
}

RH_ScopedInstall(RwStringGetSizeAligned, 0x004d8770);
