// Mashed RE - subsystem-A record lookup by key.
// Original: 0x00407550  FUN_00407550  gameplay  C2 -> C3
// Plate: re/analysis/bucket_gameplay_00405400_00407620/0x00407550.md
//
// Linear scan of the record array for the entry whose +0x44 key matches; returns
// that record's address, or 0 on miss / empty table.
//
// Disasm at 0x00407550..0x00407572 (35 bytes), read from the listing
// (Mashed_pool11) rather than from the decompiler:
//   0x00407550  MOV   EDX,dword ptr [0x0063a5d0]   ; record count
//   0x00407556  XOR   ECX,ECX                      ; i = 0
//   0x00407558  TEST  EDX,EDX
//   0x0040755A  JLE   0x00407570                   ; count <= 0 -> return 0
//   0x0040755C  MOV   EAX,0x639d80                 ; record base
//   0x00407561  CMP   dword ptr [EAX + 0x44],ESI   ; key field at +0x44
//   0x00407564  JZ    0x00407572                   ; hit -> return EAX
//   0x00407566  INC   ECX
//   0x00407567  ADD   EAX,0xec                     ; stride
//   0x0040756C  CMP   ECX,EDX
//   0x0040756E  JL    0x00407561
//   0x00407570  XOR   EAX,EAX                      ; miss -> 0
//   0x00407572  RET
//
// THE SEARCH KEY ARRIVES IN ESI. Nothing is read from the stack, and Ghidra
// types the function `undefined * FUN_00407550(void)` with the key surfacing as
// `unaff_ESI` — the same class of invisible argument that made 0x00482900 look
// zero-arg. The count is a SIGNED test (TEST/JLE), so a negative count returns 0
// rather than scanning; that is reproduced.
//
// UNLIKE the register-store helper at 0x004b6b00, THIS PORT IS PLAIN __cdecl and
// deliberately so. The `esi_global_search` A/B drives the ORIGINAL through a
// `mov esi,key ; jmp target` trampoline while calling the reimpl as
// `__cdecl(key)`, and compares the RETURNED POINTER — behaviour, not ABI. A
// naked ESI port would work too but buys nothing the comparison can see.
//
// Pairs with 0x00407580 SubsystemARecordKey (C3), which returns record[i]'s +0x44
// key: this function is the inverse lookup over the same array — same base
// 0x00639d80, same 0xec stride, same key field.
//
// [UNCERTAIN] 0x0063a5d0 is also cited as a "rule-5 collect counter" in
// scenario_launch.py. Whether that is the same counter or an address collision in
// the notes is unresolved and is NOT relied on here — the count global is taken
// from the listing at 0x00407550.
//
// NO BOUNDS CHECK BEYOND THE COUNT: the scan trusts 0x0063a5d0 completely and
// walks 0xec bytes per step from a fixed base. The port reproduces that.
#include "../Core/HookSystem.h"

// 0x00639d80 record base; 0x0063a5d0 record count; stride 0xec; key at +0x44.
static const unsigned int kRecordBase  = 0x00639d80;
static const unsigned int kRecordCount = 0x0063a5d0;
static const unsigned int kRecordStride = 0xec;
static const unsigned int kKeyOffset    = 0x44;

// 0x00407550
extern "C" __declspec(dllexport) unsigned int __cdecl
SubsystemARecordFind(unsigned int key)
{
    // 0x00407550/0x00407558 — signed count test; <= 0 falls straight to the miss.
    const int count = *(int*)kRecordCount;
    if (count <= 0)
        return 0;                                   // 0x00407570

    unsigned char* rec = (unsigned char*)kRecordBase;   // 0x0040755c
    for (int i = 0; i < count; i++)                     // 0x0040756c/0x0040756e
    {
        // 0x00407561 — compare the +0x44 key field, NOT the record's first dword.
        if (*(unsigned int*)(rec + kKeyOffset) == key)
            return (unsigned int)rec;               // 0x00407564 -> 0x00407572
        rec += kRecordStride;                       // 0x00407567
    }
    return 0;                                       // 0x00407570
}

RH_ScopedInstall(SubsystemARecordFind, 0x00407550);
