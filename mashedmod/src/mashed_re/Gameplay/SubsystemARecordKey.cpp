// Mashed RE - subsystem-A record key accessor by index.
// Original: 0x00407580  FUN_00407580  gameplay  C2 -> C3
// Plate: re/analysis/bucket_gameplay_00405400_00407620/0x00407580.md
//
// Leaf pure getter: returns the dword at record[index] + 0x44.
//
// Disasm at 0x00407580..0x00407590 (17 bytes), read from the listing in
// Mashed_pool9 rather than from the decompiler:
//   0x00407580  8B 44 24 04          MOV  EAX,dword ptr [ESP+4]   ; index
//   0x00407584  69 C0 EC 00 00 00    IMUL EAX,EAX,0xec            ; * record stride
//   0x0040758A  8B 80 C4 9D 63 00    MOV  EAX,dword ptr [EAX+0x639dc4]
//   0x00407590  C3                   RET
//
// The stride is a literal `IMUL ..,0xec` — 236 bytes. The plate expresses the
// same thing as `(&DAT_00639dc4)[param_1 * 0x3b]`, i.e. 0x3b dwords; 0x3b*4 =
// 0xec, so the two agree. The BYTE form is what the instruction stream does and
// is what is written here.
//
// 0x00639dc4 is not the record base, it is record[0] + 0x44 — the base is
// 0x00639d80 (plate "Constants"). The +0x44 field is the key that 0x004058e0
// and 0x00407550 linearly scan for, which is the only reason this accessor
// exists. The field's MEANING beyond "scanned as a key" is not established here
// and none is assigned.
//
// THERE IS NO BOUNDS CHECK. `index` is multiplied and dereferenced with no
// comparison against any count, so a negative or out-of-range index reads
// arbitrary memory. The port reproduces the absence of the check deliberately —
// adding one would diverge from the original on exactly the inputs where the
// difference is observable.
//
// __cdecl: the argument is read from [ESP+4] and the callee does not adjust ESP
// before RET.
#include "../Core/HookSystem.h"

// 0x00639dc4 — record[0] + 0x44 (base 0x00639d80). Stride 0xec bytes.
static const unsigned int kRecordKey0 = 0x00639dc4;
static const unsigned int kRecordStride = 0xec;

// 0x00407580
extern "C" __declspec(dllexport) int __cdecl
SubsystemARecordKey(int index)
{
    // 0x00407584/0x0040758a — IMUL by the byte stride, then a single dword load.
    return *(int*)(kRecordKey0 + (unsigned int)(index * (int)kRecordStride));
}

RH_ScopedInstall(SubsystemARecordKey, 0x00407580);
