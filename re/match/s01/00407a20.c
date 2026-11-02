// FLAGS: /O2 /GS /Ob1
// MATCH-TOPDOWN-T13: port-includable form of re/match/s01/raw/00407a20.c (same body; the raw file's Ghidra
// preamble is not C++-safe). Reads the dword at +0x28 of the 0x30c-byte per-car block at 0x008a9620
// (0x008a9648 + i*0x30c; 0xc3 dwords = 0x30c bytes).
#ifndef MASHED_RE_PORT
#define MGLOBAL(T, n, a) extern T n
MGLOBAL(unsigned int, DAT_008a9648, 0x008a9648);
#endif

// 0x00407a20
// MATCH: 0x00407a20 FUN_00407a20
unsigned int FUN_00407a20(int param_1)
{
  return (&DAT_008a9648)[param_1 * 0xc3];
}
