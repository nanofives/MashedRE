// Mashed RE - FUN_0042ad90 (0x0042ad90) kv-stream search, ONE body for both targets (dualcopy4, 2026-10-16, ROADMAP D4).
//
// The RVA had two bodies: Util/PromoLoop_sessionB.cpp:Find42ad90 (.asi; C3, round 162, registry key find_42ad90, installed
// through the naked EDX/EBX/EDI shim Find42ad90_RegAbi) and Frontend/MenuNavSM.cpp:KvLookup (exe only, a separate transcription
// used by the standalone's menu navigation over its own tables). They walk the same flat u32 stream with the same result for every
// input: the match test never sees the terminator in either (Find42ad90 checks the first element, then breaks on the element it
// advances to; KvLookup checks the terminator before the match on each pass), a null table is -1 in both, and a negative occurrence
// never equals the non-negative match counter. KvLookup is now a call into this body.
//
// Original (0x0042ad90, registers EDX = array, EBX = key, EDI = n; nothing on the stack):
//   0x0042ad90 TEST EDX,EDX -> -1 if null;  walk dwords until 0xff070000;  at the (n+1)-th element == key return the NEXT dword;
//   otherwise -1.  0x0042ada6 CMP EAX,EBX (key)   0x0042adaa CMP EDI,ESI (n)   0x0042adaf / 0x0042adc0 [EDX + ECX*4 + 4] (next).
// The exported C body keeps the STACK ABI because the registry driver (arg_type edx_ebx_edi_find) calls it that way.
// Anchored to MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E (original\MASHED.exe.unpatched).
#include <cstdint>

// MATCH-S06: the body of 0x0042ad90 (Find42ad90) moved to Match/MatchS06_0042ad90.cpp,
// which compiles the byte-matched source re/match/s06/0042ad90.c.
