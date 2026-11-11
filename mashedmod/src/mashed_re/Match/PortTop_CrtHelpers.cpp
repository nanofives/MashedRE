// BIND-CRT-HELPERS (L-111 step 2, split of PORT-OT-BIND-CRT): the six MSVC compiler helpers the original's matched
// bodies call, bound on the MASHED_ORIG_TOP path to the exe's own CRT helpers. Each thunk is calling-convention glue,
// not a body: the DDK helper and the VS2022 helper share the ABI (the compiler emits both the same way), so the thunk
// is a single jmp. Identity of each original RVA = libid against the XP DDK libcmt.lib
// (verify/libidentity_20261030/libidentity_functions.tsv):
//   _ftol2    rva 004a2c48    ftol2.obj  identical-unlisted   in: ST0 (popped)            out: EDX:EAX, truncation
//   _alldiv   rva 004a4170   lldiv.obj  identical-unlisted   in: 2 x int64 on the stack  out: EDX:EAX, ret 0x10
//   _allmul   rva 004a4220   llmul.obj  identical-unlisted   same
//   _aulldiv  rva 004a4e40  ulldiv.obj identical            same
//   _aullrem  rva 004aa060  ullrem.obj identical            same
//   _chkstk   rva 004a3440   chkstk.obj identical-unlisted   in: EAX = bytes             out: ESP lowered by EAX, pages probed
// Equivalence of the thunked VS2022 helpers with the DDK bodies (bytes from MASHED.exe.unpatched, run in place):
// verify/bindcrthelpers_20261111/ (unit test over edge values, x87 control words PC_24 / PC_53 / PC_64 with every
// rounding mode for _ftol2). A matched source binds a helper with `#define FUN_004a2c48 MashedCrt_ftol2_004a2c48` in its
// TU; the source text is not edited, so its match.py status is unchanged.
// EXE-ONLY: the .asi runs inside MASHED.exe and calls the originals directly.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
extern "C" {
void __cdecl _ftol2(void);
void __cdecl _alldiv(void);
void __cdecl _allmul(void);
void __cdecl _aulldiv(void);
void __cdecl _aullrem(void);
void __cdecl _chkstk(void);

__declspec(naked) unsigned __int64 __cdecl MashedCrt_ftol2_004a2c48(void) { __asm jmp _ftol2 }
__declspec(naked) __int64 __stdcall MashedCrt_alldiv_004a4170(unsigned, unsigned, unsigned, unsigned) { __asm jmp _alldiv }
__declspec(naked) __int64 __stdcall MashedCrt_allmul_004a4220(unsigned, unsigned, unsigned, unsigned) { __asm jmp _allmul }
__declspec(naked) unsigned __int64 __stdcall MashedCrt_aulldiv_004a4e40(unsigned, unsigned, unsigned, unsigned) { __asm jmp _aulldiv }
__declspec(naked) unsigned __int64 __stdcall MashedCrt_aullrem_004aa060(unsigned, unsigned, unsigned, unsigned) { __asm jmp _aullrem }
__declspec(naked) void __cdecl MashedCrt_chkstk_004a3440(void) { __asm jmp _chkstk }
}  // extern "C"
#endif  // MASHED_STANDALONE
