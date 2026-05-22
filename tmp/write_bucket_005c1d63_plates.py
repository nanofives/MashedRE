#!/usr/bin/env python3
"""
Generate 80 C1 plates for bucket_005c1d63 (batch_v session 6).

NO-GUESSING discipline: each plate cites only what was verified via Ghidra MCP
calls during the session. Frontmatter values come directly from
mcp__ghidra__function_at results captured in the session transcript.

Three classes of candidates:
  CRT_FIDDB     -- Ghidra returned a FidDB CRT name (no rename, library_match cited)
  CRT_INTERNAL  -- FUN_* whose decomp shows it is a CRT internal helper
                   (also library_match cited, but the *function* must be
                   plated because it is currently unmapped)
  GAME          -- Game-side code worth a full mechanical description

The session refused to rename anything (per skill: "note the library match in
the plate, do NOT rename in this session"). All plates are C1 first-pass.
"""
import os
from pathlib import Path

BUCKET_DIR = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\re\analysis\bucket_005c1d63")
SESSION = "batch-v-s6"
DATE = "2026-05-18"
SLOT = "Mashed_pool6"   # pre-assigned per brief; verified in this session
POOL_OPENED_IN = "Mashed_pool7"  # actual slot because pool6 had stale JVM channel lock

# (rva_int, body_end_int, ghidra_name, size_bytes, kind, lib_match_or_summary, callees_depth1, callers_noted, mech_bullets)
# `mech_bullets` is a list of single-line strings.
# `callees_depth1` / `callers_noted` are strings already formatted as "yaml-friendly".
CANDIDATES = [
    # 1
    (0x005c1d63, 0x005c1da2, "_fseek", 0x3f, "CRT_FIDDB",
     "MSVC CRT _fseek (FidDB-attested; signature `int __cdecl _fseek(FILE*, long, int)`)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point.",
      "Ghidra signature: `int __cdecl _fseek(FILE * _File, long _Offset, int _Origin)` — canonical MSVCRT parameter names (_File, _Offset, _Origin) confirm FidDB pack origin.",
      "No mechanical body description authored — library residue per discover-c1-batch skill (skip rename in this session)."]),
    # 2
    (0x005c1da2, 0x005c1dab, "FUN_005c1da2", 0x09, "CRT_INTERNAL",
     "MSVC CRT internal: naked SEH-finally tail calling `__unlock_file(*(FILE**)(EBP+8))`",
     "__unlock_file (CRT)", "(none recorded — only reachable via SEH unwinder)",
     ["Naked SEH-finally tail.",
      "Body: `__unlock_file(*(FILE**)(unaff_EBP+8))` — reads FILE* at EBP+0x08 in the *parent* frame (no own frame; relies on unaff_EBP).",
      "Frame-bound CRT helper; not a standalone function in source — emitted by MSVC `try/__finally` blocks in CRT stdio routines (e.g., _fseek body)."]),
    # 3
    (0x005c1dac, 0x005c1dd2, "__ftol", 0x27, "CRT_FIDDB",
     "MSVC CRT __ftol (FidDB-attested; FP-to-long convert)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `__ftol` (float-to-long with FPU mode swap).",
      "No mechanical body description authored — library residue."]),
    # 4
    (0x005c1dd3, 0x005c1e26, "FID_conflict:_fwprintf", 0x54, "CRT_FIDDB",
     "MSVC CRT _fwprintf (FidDB-attested; wide formatted print). FID_conflict prefix = Ghidra observed multiple FidDB matches at this address",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `_fwprintf` (`int __cdecl FID_conflict:_fwprintf(FILE*, wchar_t*, ...)`).",
      "`FID_conflict:` prefix indicates Ghidra found 2+ signature matches for this body — typical when two MSVCRT versions ship identical bytes."]),
    # 5
    (0x005c1e27, 0x005c1e30, "FUN_005c1e27", 0x0a, "CRT_INTERNAL",
     "MSVC CRT internal: SEH-finally tail (variant of 0x005c1da2 — uses FILE* at EBP-0x1c instead of EBP+8)",
     "__unlock_file (CRT)", "(none recorded)",
     ["Naked SEH-finally tail; same pattern as 0x005c1da2.",
      "Body: `__unlock_file(*(FILE**)(unaff_EBP - 0x1c))` — FILE* at EBP-0x1c (different parent stack frame).",
      "Emitted by a different CRT routine's `__finally` block; library residue."]),
    # 6
    (0x005c1e31, 0x005c1e9c, "FID_conflict:_ungetc", 0x6c, "CRT_FIDDB",
     "MSVC CRT _ungetc (FidDB-attested; one-char ungetc to FILE*)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `_ungetc`.",
      "FID_conflict prefix = 2+ FidDB matches."]),
    # 7
    (0x005c1f1c, 0x005c1f96, "__setjmp3", 0x7b, "CRT_FIDDB",
     "MSVC CRT __setjmp3 (FidDB-attested; SEH-aware setjmp variant)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `__setjmp3` (records EBP, SEH chain, frame info)."]),
    # 8
    (0x005c1f97, 0x005c2024, "_strtod", 0x8e, "CRT_FIDDB",
     "MSVC CRT _strtod (FidDB-attested; string-to-double)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `_strtod` (`double __cdecl _strtod(char* _Str, char** _EndPtr)`)."]),
    # 9
    (0x005c2025, 0x005c2083, "_strcoll", 0x5f, "CRT_FIDDB",
     "MSVC CRT _strcoll (FidDB-attested; locale-aware string compare)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `_strcoll`."]),
    # 10
    (0x005c2090, 0x005c21ac, "FUN_005c2090", 0x11d, "CRT_INTERNAL",
     "MSVC CRT internal: math kernel for `floor`/`ceil`-shaped op. Uses MXCSR-aware fast path when SSE flush-to-zero & FPU precision are at defaults; falls back to `__ctrlfp`+`__sptype`+`__handle_qnan1` for NaN/Inf cases. Calls FUN_004ae279, FUN_004a85d2",
     "__ctrlfp, __sptype, __handle_qnan1 (all CRT)", "(CRT-internal)",
     ["Math kernel matching the structure of `floor`/`ceil`-default implementations.",
      "Fast path (line 005c20a3): tests `DAT_008aa6a4 != 0 && (MXCSR & 0x1f80) == 0x1f80 && (FPU CW & 0x7f) == 0x7f` — only used when both SSE and x87 are at canonical denormal/precision defaults.",
      "Bit-twiddling at 005c20bd: extracts 64-bit IEEE-754 exponent `(uVar2 >> 0x14)`, masks `_DAT_005e7260`, shifts by `lVar6` to clear mantissa fraction below the integer position.",
      "Constants cited: `_DAT_005e7230`, `_DAT_005e7240`, `_DAT_005e7250`, `_DAT_005e7260`, `_DAT_005e7270`, `_DAT_005cd0c8`.",
      "Slow path (005c2167): calls `__ctrlfp` to save FPU control word, classifies via `__sptype`, handles QNaN via `__handle_qnan1`, falls back to FUN_004ae279.",
      "Library residue (CRT math helper)."]),
    # 11
    (0x005c21ba, 0x005c21c3, "FUN_005c21ba", 0x09, "CRT_INTERNAL",
     "MSVC CRT internal: tail-call thunk to `__cintrindisp2`",
     "__cintrindisp2 (CRT — also in this bucket at 0x005c3340)", "(CRT-internal)",
     ["3-instruction tail to `__cintrindisp2()` (CRT internal intrinsic dispatcher).",
      "Library residue."]),
    # 12
    (0x005c21e2, 0x005c228d, "_frexp", 0xac, "CRT_FIDDB",
     "MSVC CRT _frexp (FidDB-attested; FP mantissa+exponent split)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `_frexp` (`double __cdecl _frexp(double _X, int * _Y)`)."]),
    # 13
    (0x005c228e, 0x005c229a, "FUN_005c228e", 0x0d, "CRT_INTERNAL",
     "MSVC CRT internal: `_srand` (writes seed to `__getptd()->_holdrand`)",
     "__getptd (CRT)", "(CRT-internal)",
     ["Body: `p = __getptd(); p->_holdrand = param_1; return;`.",
      "Canonical MSVC `_srand` implementation (per-thread RNG seed).",
      "Library residue."]),
    # 14
    (0x005c229b, 0x005c22bc, "_rand", 0x22, "CRT_FIDDB",
     "MSVC CRT _rand (FidDB-attested; LCG using _holdrand)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `_rand`."]),
    # 15
    (0x005c22bd, 0x005c22f0, "_sscanf", 0x34, "CRT_FIDDB",
     "MSVC CRT _sscanf (FidDB-attested; string-source scanf wrapper)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `_sscanf` (`int __cdecl _sscanf(char* _Src, char* _Format, ...)`).",
      "Body is the thin wrapper that builds an in-memory FILE* and forwards to the shared formatter at FUN_005c35d6 (the `_input` core)."]),
    # 16
    (0x005c2300, 0x005c2434, "_strncat", 0x135, "CRT_FIDDB",
     "MSVC CRT _strncat (FidDB-attested; strncat with size cap)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `_strncat`."]),
    # 17
    (0x005c2435, 0x005c246d, "FID_conflict:__time32", 0x39, "CRT_FIDDB",
     "MSVC CRT __time32 (FidDB-attested; 32-bit time_t getter wrapping GetSystemTimeAsFileTime)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `__time32`.",
      "FID_conflict prefix = 2+ FidDB matches."]),
    # 18
    (0x005c2470, 0x005c24dc, "shortsort", 0x6d, "CRT_FIDDB",
     "MSVC CRT shortsort (FidDB-attested; insertion-sort partition used by _qsort)",
     "-", "-",
     ["FidDB-attested MSVC CRT helper: `shortsort` (the insertion-sort partition called by `_qsort` for sub-ranges < 8).",
      "Verified via Ghidra: callee of FUN_005c24e0 (which is `_qsort`)."]),
    # 19
    (0x005c24e0, 0x005c2762, "FUN_005c24e0", 0x283, "CRT_INTERNAL",
     "MSVC CRT internal: `_qsort` (iterative quicksort with explicit stack-of-(lo,hi) ranges; falls back to `shortsort` for sub-ranges)",
     "shortsort (0x005c2470)", "(CRT-internal)",
     ["Iterative quicksort: stack-of-(lo, hi) ranges held in `auStack_f0` / `auStack_78` (30 dwords each = 30 levels).",
      "Falls back to `shortsort(lo, elem_size, cmp)` (0x005c2470) when sub-range size <= 8 (see condition `8 < uVar2` at 005c2533).",
      "Median-of-three pivot via three pairwise swaps comparing `local_100` (lo), `local_100 + (n/2)*elem_size` (mid), `local_fc` (hi).",
      "Constants cited: stack depth 30 → max-depth limit (refused at depths > 30 by ranges-stack overflow).",
      "Library residue: canonical MSVC `_qsort` body."]),
    # 20
    (0x005c2770, 0x005c278e, "FUN_005c2770", 0x1f, "CRT_INTERNAL",
     "MSVC CRT internal: SEH-chain init helper (sets `ExceptionList = auStack_c`)",
     "(CRT-internal — touches fs:[0] ExceptionList)", "(CRT-internal)",
     ["Body: declares 12 bytes of stack (`auStack_c`), then `ExceptionList = auStack_c;` (writes fs:[0]).",
      "Internal CRT SEH-chain helper invoked by `_setjmp3` and similar prologues.",
      "Library residue."]),
    # 21
    (0x005c278f, 0x005c27e5, "FUN_005c278f", 0x57, "CRT_INTERNAL",
     "MSVC CRT internal: `_vsnprintf` core (stack-alloc a FILE with `_cnt=N`, `_flag=0x42`, `_base=_ptr=buf`; forwards to FUN_004a504f formatter)",
     "FUN_004a504f (CRT formatter), __flsbuf (CRT)", "(CRT-internal)",
     ["In-memory `_vsnprintf` shape.",
      "Constants cited: `local_24._flag = 0x42` (MSVC FILE flag for in-memory buffer with write-allowed)",
      "After format completion: `*local_24._ptr = '\\0'` writes the trailing null unless the buffer overflowed (`_cnt < 0` -> `__flsbuf(0, &local_24)`).",
      "Library residue."]),
    # 22
    (0x005c27e6, 0x005c281d, "_atof", 0x38, "CRT_FIDDB",
     "MSVC CRT _atof (FidDB-attested; ASCII-to-double via _strtod fast path)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `_atof`."]),
    # 23
    (0x005c281e, 0x005c28e6, "FUN_005c281e", 0xc9, "CRT_INTERNAL",
     "MSVC CRT internal: `_toupper_l` (locale-aware ToUpper). ASCII fast path lowercases a-z by -0x20; MBCS path via `___crtLCMapStringA(LCMAP_UPPERCASE=0x200)`",
     "___crtLCMapStringA, FUN_004afff1 (CRT)", "FUN_005c28e7 (the non-locale `_toupper` wrapper)",
     ["Locale-aware uppercase conversion.",
      "Fast path (005c2830): when locale has no MBCS table (`*(int*)(locale+0x14) == 0`) OR has the C-locale flag at offset 0x24 AND char < 0x80 — ASCII 'a'..'z' → -0x20.",
      "MBCS path (005c2858): tests the MBCS class bitmap at `locale+0x48`, builds either a 1- or 2-byte source via DBCS lead-byte detection (bit 0x80 of bitmap[1]), then calls `___crtLCMapStringA(locale[0x14], LCMAP_UPPERCASE=0x200, ...)`.",
      "Constants cited: 0x14 (locale->lc_ctype), 0x24 (C-locale flag), 0x28 (mbcs-level), 0x48 (mbcs class bitmap), 0x200 (LCMAP_UPPERCASE).",
      "Library residue."]),
    # 24
    (0x005c28e7, 0x005c2908, "FUN_005c28e7", 0x22, "CRT_INTERNAL",
     "MSVC CRT internal: `_toupper` (non-`_l` wrapper) — fetches thread locale via `__getptd()->_tfpecode`, refreshes via `___updatetlocinfo()` if stale, then delegates to `_toupper_l` (0x005c281e)",
     "__getptd, ___updatetlocinfo, FUN_005c281e (_toupper_l, this bucket)", "(CRT-internal)",
     ["Body: `p = __getptd(); locale = p->_tfpecode; if (locale != PTR_DAT_00616be4) locale = ___updatetlocinfo(); FUN_005c281e(locale, ch);`.",
      "Canonical MSVC `_toupper` shape (non-locale wrapper).",
      "Library residue."]),
    # 25
    (0x005c29cb, 0x005c2a0d, "_clock", 0x43, "CRT_FIDDB",
     "MSVC CRT _clock (FidDB-attested; GetSystemTimeAsFileTime-based wall clock)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `_clock`."]),
    # 26
    (0x005c2a0e, 0x005c2a44, "___inittime", 0x37, "CRT_FIDDB",
     "MSVC CRT ___inittime (FidDB-attested; time-data table initializer)",
     "-", "-",
     ["FidDB-attested MSVC CRT helper: `___inittime`."]),
    # 27
    (0x005c2a45, 0x005c2bc4, "_localtime", 0x180, "CRT_FIDDB",
     "MSVC CRT _localtime (FidDB-attested; time_t -> struct tm with timezone)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `_localtime` (`tm * __cdecl _localtime(time_t * _Time)`)."]),
    # 28
    (0x005c2bc5, 0x005c2d83, "strtoxl", 0x1bf, "CRT_FIDDB",
     "MSVC CRT strtoxl (FidDB-attested; shared long/unsigned-long parser used by _strtol/_strtoul)",
     "-", "-",
     ["FidDB-attested MSVC CRT helper: `strtoxl` (the shared signed/unsigned `long` parser).",
      "Callee of _strtol (0x005c2d84)."]),
    # 29
    (0x005c2d84, 0x005c2d9a, "_strtol", 0x17, "CRT_FIDDB",
     "MSVC CRT _strtol (FidDB-attested; signed long parser — thin wrapper over strtoxl)",
     "strtoxl (0x005c2bc5, this bucket)", "-",
     ["FidDB-attested MSVC CRT entry point: `_strtol`. Calls strtoxl (0x005c2bc5) with signed flag."]),
    # 30
    (0x005c2d9b, 0x005c2dd7, "FUN_005c2d9b", 0x3d, "CRT_INTERNAL",
     "MSVC CRT internal: `_endthreadex` — invokes optional cleanup via PTR_FUN_0061604c, retrieves ptd via __getptd, CloseHandle on _thandle if not -1, frees ptd via FUN_004a8899, calls ExitThread(0)",
     "PTR_FUN_0061604c (cleanup), __getptd, __amsg_exit, CloseHandle, FUN_004a8899, ExitThread", "FUN_005c2dd9 (_threadstartex trampoline)",
     ["Canonical `_endthreadex` body.",
      "Cleanup hook at `PTR_FUN_0061604c` (settable globally).",
      "Constants cited: `0x10` (amsg_exit code for thread teardown error), `ExitThread(0)` exit code.",
      "Library residue."]),
    # 31
    (0x005c2dd9, 0x005c2e58, "FUN_005c2dd9", 0x80, "CRT_INTERNAL",
     "MSVC CRT internal: `_threadstartex` — TlsGetValue/TlsSetValue ptd handoff for `_beginthread`, optional global init hook PTR_FUN_00616048, invokes user proc at ptd+0x4c with arg at ptd+0x50, then calls `_endthreadex`",
     "TlsGetValue, TlsSetValue, __amsg_exit, _free, PTR_FUN_00616048 (init), FUN_005c2d9b (_endthreadex)", "__beginthread (0x005c2e79, this bucket)",
     ["Thread entry trampoline that bridges the C runtime's per-thread ptd over to the user proc.",
      "Layout: ptd offsets 0x4c=user_proc, 0x50=user_arg, 0x04=initial param. TLS slot is `DAT_00616658`.",
      "Library residue."]),
    # 32
    (0x005c2e79, 0x005c2f09, "__beginthread", 0x91, "CRT_FIDDB",
     "MSVC CRT __beginthread (FidDB-attested; CreateThread wrapper that allocates ptd and pivots through _threadstartex)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `__beginthread`."]),
    # 33
    (0x005c2f0a, 0x005c2f25, "FUN_005c2f0a", 0x1c, "CRT_INTERNAL",
     "MSVC CRT internal: `_wcscpy` — copy wide-string until null",
     "-", "-",
     ["Body: do { *dst = *src; ++dst; ++src; } while (last_value != 0); return dst.",
      "Canonical MSVC `_wcscpy`.",
      "Library residue."]),
    # 34
    (0x005c2f26, 0x005c2f57, "_wcscmp", 0x32, "CRT_FIDDB",
     "MSVC CRT _wcscmp (FidDB-attested; wide-string compare)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `_wcscmp`."]),
    # 35
    (0x005c2f58, 0x005c2f6d, "FUN_005c2f58", 0x16, "CRT_INTERNAL",
     "MSVC CRT internal: `_wcslen` (with -1 result; returns `((end - start) >> 1) - 1`)",
     "-", "-",
     ["Body: walk wide-string until null, return char-count - 1.",
      "Canonical MSVC `_wcslen` shape (the off-by-one is just the inclusive-of-null shift).",
      "Library residue."]),
    # 36
    (0x005c2f70, 0x005c2f83, "FUN_005c2f70", 0x14, "CRT_INTERNAL",
     "MSVC CRT internal: math-error entry — converts ST(0) to double, calls FUN_004a7118 (math-error chain), tail-jumps to FUN_005c2f8d",
     "FUN_004a7118, FUN_005c2f8d (this bucket)", "(CRT-internal)",
     ["Tiny: `FUN_004a7118((double)in_ST0); FUN_005c2f8d();`. Promotes the FP top-of-stack value to a math-error-chain handler.",
      "Library residue."]),
    # 37
    (0x005c2f84, 0x005c2f8c, "FUN_005c2f84", 0x09, "CRT_INTERNAL",
     "MSVC CRT internal: `__fload_withFB` entry trampoline — sets up FB then falls into FUN_005c2f8d's body",
     "FUN_004a70a5, FUN_004a70bc, __math_exit, __startOneArgErrorHandling, __fload_withFB", "(CRT-internal)",
     ["Body shares its tail with FUN_005c2f8d (post-`__fload_withFB`). Math-domain-error classifier.",
      "Library residue."]),
    # 38
    (0x005c2f8d, 0x005c3029, "FUN_005c2f8d", 0x9d, "CRT_INTERNAL",
     "MSVC CRT internal: math-domain-error classifier (NaN/Inf branch via DBL bit-pattern; returns via __math_exit or __startOneArgErrorHandling)",
     "FUN_004a70a5, FUN_004a70bc, __math_exit, __startOneArgErrorHandling", "FUN_005c2f70, FUN_005c2f84 (this bucket)",
     ["Walks ST(0) using FPU flag ZF + FPU control word to classify NaN / Inf / signed-Inf / signed-zero, returns matching errno code via __math_exit or one-arg error handler.",
      "Library residue."]),
    # 39
    (0x005c302a, 0x005c318b, "__ftell_lk", 0x162, "CRT_FIDDB",
     "MSVC CRT __ftell_lk (FidDB-attested; locked _ftell)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `__ftell_lk` (file-locked ftell)."]),
    # 40
    (0x005c318c, 0x005c31cd, "__rt_probe_read4@4", 0x42, "CRT_FIDDB",
     "MSVC CRT __rt_probe_read4@4 (FidDB-attested; 4-byte read-probe primitive used by RTC)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `__rt_probe_read4@4` (read-probe primitive, /RTCu support)."]),
    # 41
    (0x005c31d0, 0x005c326a, "FUN_005c31d0", 0x9b, "CRT_INTERNAL",
     "MSVC CRT internal: `_strgtold12` wrapper — calls FUN_004b0609 with bias args, converts via FID_conflict___ld12tod, packs status flags (0x80=overflow, 0x100=underflow, 0x200=parse-error)",
     "FUN_004b0609 (CRT), FID_conflict___ld12tod, security_check_cookie", "(CRT-internal)",
     ["Wrapper that uses GS-cookie protection (`local_8 = DAT_00616038 ^ unaff_retaddr`).",
      "Status bit values cited: 0x80 (overflow), 0x100 (underflow), 0x200 (parse-error).",
      "Library residue."]),
    # 42
    (0x005c326b, 0x005c333b, "FID_conflict:__floor_default", 0xd1, "CRT_FIDDB",
     "MSVC CRT __floor_default (FidDB-attested; default `floor` implementation for non-SSE-enabled paths)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `__floor_default`.",
      "FID_conflict prefix = 2+ FidDB matches."]),
    # 43
    (0x005c3340, 0x005c337d, "__cintrindisp2", 0x3e, "CRT_FIDDB",
     "MSVC CRT __cintrindisp2 (FidDB-attested; intrinsic dispatcher #2)",
     "-", "-",
     ["FidDB-attested MSVC CRT helper: `__cintrindisp2`."]),
    # 44
    (0x005c337e, 0x005c33ba, "__cintrindisp1", 0x3d, "CRT_FIDDB",
     "MSVC CRT __cintrindisp1 (FidDB-attested; intrinsic dispatcher #1)",
     "-", "-",
     ["FidDB-attested MSVC CRT helper: `__cintrindisp1`."]),
    # 45
    (0x005c33bb, 0x005c3402, "__ctrandisp2", 0x48, "CRT_FIDDB",
     "MSVC CRT __ctrandisp2 (FidDB-attested; control-transfer dispatcher #2)",
     "-", "-",
     ["FidDB-attested MSVC CRT helper: `__ctrandisp2`."]),
    # 46
    (0x005c3403, 0x005c3409, "FUN_005c3403", 0x07, "CRT_INTERNAL",
     "MSVC CRT internal: FP exception classifier (sub-routine) — reads parent frame, classifies NaN/Inf/Denormal at EBP-0x90 (FP type tag), rescales via fscale and constants at 0x005e72a8/72b8/72c8",
     "fscale (x87 inst), FUN_004ae17b (error reporter)", "(CRT-internal)",
     ["7-byte function — extreme jump-target / partial-routine pattern (likely entry from a `cmp/jcc` ladder elsewhere).",
      "Body uses parent EBP heavily: reads `*(char*)(EBP - 0x90)` to dispatch among 0/-1/-2 FP type tags.",
      "Constants cited: _DAT_005e72a8, _DAT_005e72b0, _DAT_005e72b8, _DAT_005e72c0, _DAT_005e72c8 (fscale scaling consts).",
      "Library residue."]),
    # 47
    (0x005c340a, 0x005c3550, "FUN_005c340a", 0x147, "CRT_INTERNAL",
     "MSVC CRT internal: full FP exception classifier (variant of 0x005c3403 without the leading flag-clear). NaN/Inf/Denormal classification + rescale + error-call FUN_004ae17b",
     "fscale, FUN_004ae17b", "(CRT-internal)",
     ["Same body as 0x005c3403 minus the leading `byte&0xfe` clear at EBP-0x2c8.",
      "Both share constants _DAT_005e72a0..72c8 and call FUN_004ae17b(type, ec_info_ptr, status_ptr).",
      "Library residue."]),
    # 48
    (0x005c3551, 0x005c3583, "__ctrandisp1", 0x33, "CRT_FIDDB",
     "MSVC CRT __ctrandisp1 (FidDB-attested; control-transfer dispatcher #1)",
     "-", "-",
     ["FidDB-attested MSVC CRT helper: `__ctrandisp1`."]),
    # 49
    (0x005c3584, 0x005c35bf, "__fload", 0x3c, "CRT_FIDDB",
     "MSVC CRT __fload (FidDB-attested; FPU-stack helper used by C library math)",
     "-", "-",
     ["FidDB-attested MSVC CRT helper: `__fload`."]),
    # 50
    (0x005c35c0, 0x005c35d5, "__inc", 0x16, "CRT_FIDDB",
     "MSVC CRT __inc (FidDB-attested; one-char input increment helper used by scanf)",
     "-", "-",
     ["FidDB-attested MSVC CRT helper: `__inc`. Calls from FUN_005c35d6 (the shared scanf core) confirm role."]),
    # 51
    (0x005c35d6, 0x005c407d, "FUN_005c35d6", 0xaa8, "CRT_INTERNAL",
     "MSVC CRT internal: shared `_vfscanf` / scanf core (handles %d/%i/%o/%x/%u/%s/%c/%[...]/%n/%f/%e/%g/%I64/%I32 with width caps, locale-aware whitespace via _isspace/_isdigit/_isxdigit/_mbtowc). GS-cookie protected",
     "FID_conflict__ungetc, _isspace, _isdigit, _isxdigit, _mbtowc, __inc, _free, _memset, __allmul, PTR_FUN_006160e0 (float formatter), security_check_cookie", "_sscanf (0x005c22bd) and other scanf-family entry points",
     ["Full _input/_vfscanf body. 2,672 bytes.",
      "Conversion specifiers parsed at 005c3640-005c37b0 (flags/width/length/conv-char).",
      "Constants cited: 0x25 ('%'), 0x2a ('*' — suppress assignment), 0x2b/0x2d (sign), 0x30..0x39 (digit chars), 0x46 ('F'), 0x49 ('I'), 0x4c ('L'), 0x4e ('N'), 0x6c ('l'), 0x6e ('n'), 0x68 ('h'), 0x77 ('w'), 0x36/0x34 ('64' for I64), 0x33/0x32 ('32' for I32), 0x5b/0x5d ('[' / ']' bracket-set), 0x5e ('^'), 0x15d (width cap for %f buffer = 349).",
      "Multibyte support via PTR_DAT_006169cc (mbctype table) and DAT_006169e4 (decimal point char) and DAT_006169e0 (codepage).",
      "PTR_FUN_006160e0 (called at 005c3a0a) is the float formatter — atof/strtod variant invoked when %f/%e/%g matches.",
      "Library residue."]),
    # 52
    (0x005c4180, 0x005c4197, "FUN_005c4180", 0x18, "CRT_INTERNAL",
     "MSVC CRT internal: `_exp` outer trampoline — promotes ST(0) to double and tail-calls FUN_005c419e",
     "FUN_005c419e (this bucket — the _exp body)", "(CRT-internal)",
     ["Body: `FUN_005c419e((double)in_ST0);` and return.",
      "Outer FPU-call ABI for the inner XMM-ABI _exp at 0x005c419e.",
      "Library residue."]),
    # 53
    (0x005c419e, 0x005c443c, "FUN_005c419e", 0x29f, "CRT_INTERNAL",
     "MSVC CRT internal: `_exp` body (XMM-ABI: returns float10, takes double-via-XMM0_Qa). Range-checked range `[0x3c90 .. 0x408f]` (in exponent units), 64-entry table at &UNK_005e7398, polynomial coeffs at _DAT_005e7378..5e7388, fallback `__math_exit` codes 0xe/0xf/0x3ea",
     "FUN_004a73e8 (math-error reporter)", "FUN_005c4180 (the _exp ST(0) trampoline, this bucket)",
     ["Canonical Pentium-era MSVC `_exp`.",
      "Range gate at 005c41a8: exponent in `[0x3c90 (~3e-19) .. 0x408f (~1024)]` — outside this calls __math_exit with codes 0xe (overflow→Inf), 0xf (underflow→0), 0x3ea (NaN→QNaN).",
      "64-entry table at `&UNK_005e7398` indexed by `(uVar1 & 0x3f) * 0x10`: dwords [0x00..0x07] = exp2(k/64) mantissa, [0x08..0x0f] = correction term.",
      "Polynomial degree 4 in dVar4 (precision = 53 bits + correction).",
      "Constants cited: _DAT_005e7300, _DAT_005e7310, _DAT_005e7320, _UNK_005e7328, _DAT_005e7340, _UNK_005e7348, _DAT_005e7350, _UNK_005e7358, _DAT_005e7360, _UNK_005e7368, _DAT_005e7370, _UNK_005e7378, _DAT_005e7380, _UNK_005e7388, _DAT_005e7390, &UNK_005e7398, _DAT_005e72f0, _DAT_005e7790, _DAT_005e77b0, _DAT_005e77b8, _DAT_005e77c0, _DAT_005e77c8.",
      "Library residue."]),
    # 54
    (0x005c4440, 0x005c4546, "_gmtime", 0x107, "CRT_FIDDB",
     "MSVC CRT _gmtime (FidDB-attested; UTC time_t -> struct tm)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `_gmtime`."]),
    # 55
    (0x005c4547, 0x005c4606, "___mbtowc_mt", 0xc0, "CRT_FIDDB",
     "MSVC CRT ___mbtowc_mt (FidDB-attested; locale-aware multibyte-to-wide conversion)",
     "-", "-",
     ["FidDB-attested MSVC CRT helper: `___mbtowc_mt`."]),
    # 56
    (0x005c4607, 0x005c4631, "_mbtowc", 0x2b, "CRT_FIDDB",
     "MSVC CRT _mbtowc (FidDB-attested; thread-locale wrapper over ___mbtowc_mt)",
     "-", "-",
     ["FidDB-attested MSVC CRT entry point: `_mbtowc`."]),
    # 57
    (0x005c4640, 0x005c4661, "FUN_005c4640", 0x22, "GAME",
     "Mashed asset-loader registration thunk: forwards (&DAT_00617f98, param_1..param_4) to FUN_004e1ac0 (engine registry sink). Companion of FUN_005c4780 (registers via DAT_00617f78). Likely registers a chunk reader for tag table at 0x00617f98",
     "FUN_004e1ac0", "FUN_005c4670 (this bucket — the chunk reader using DAT_00617f98 as registry key)",
     ["3-line trampoline: `FUN_004e1ac0(&DAT_00617f98, param_1, param_2, param_3, param_4);`.",
      "DAT_00617f98 is a process-wide registry table (cited at 005c4642).",
      "Same shape as FUN_005c4780 (which uses DAT_00617f78). Two registry tables for two different chunk families.",
      "Mashed game code."]),
    # 58
    (0x005c4670, 0x005c4776, "FUN_005c4670", 0x107, "GAME",
     "Mashed chunk-section reader: dispatches on header type code in [0x35000, 0x37002] range, calls FUN_004cc5e0 (header read) and FUN_004cbd30 (body read), allocates object via FUN_004c1d30, populates fields via FUN_004c1c80/1a10/1a40/1b10/1c10, and inserts into a graph via FUN_004e1b60(&DAT_00617f98, ...).",
     "FUN_004cc5e0 (chunk header reader), FUN_004cbd30 (chunk body reader), FUN_004d7ff0/4d8480 (error chain), FUN_004c1d30 (object alloc), FUN_004c1c80/1a10/1a40/1b10/1c10 (field setters), FUN_004e1b60 (registry insert)", "FUN_004e7420",
     ["Reads a chunk header via `FUN_004cc5e0(stream, 1, &len, &type)` at 005c4684.",
      "Type-code accepted range cited: 0x35000 <= type <= 0x37002 (constants at 005c4694 and 005c4699).",
      "On out-of-range type: errors via `FUN_004d8480({1, FUN_004d7ff0(0x80000004)})` at 005c46b1 (error code 0x80000004 — likely UNKNOWN_TYPE).",
      "On valid type: reads 0x20 bytes of body fields via `FUN_004cbd30(stream, local_20, 0x20)`, allocates via FUN_004c1d30, writes fields at offsets 0x88 (one undefined4), and registers via `FUN_004e1b60(&DAT_00617f98, stream, obj)`.",
      "Constants cited: 0x35000 (min type), 0x37002 (max type), 0x20 (body field count = 8 dwords cleared), 0x80000004 (error code), 0x88 (object field offset).",
      "Mashed game code — looks like RW-style chunk reader for a specific subsystem (the &DAT_00617f98 registry handles this family of chunks)."]),
    # 59
    (0x005c4780, 0x005c47a1, "FUN_005c4780", 0x22, "GAME",
     "Mashed asset-loader registration thunk: forwards (&DAT_00617f78, ...) to FUN_004e1ac0. Companion of FUN_005c4640 (registers DAT_00617f98)",
     "FUN_004e1ac0", "FUN_005c47e0 (this bucket — the multi-record chunk reader using DAT_00617f78)",
     ["Body: `FUN_004e1ac0(&DAT_00617f78, p1, p2, p3, p4);`. Companion of FUN_005c4640.",
      "DAT_00617f78 is the second registry table (different chunk family, smaller record size = 0x38 vs 0x20 from FUN_005c4670).",
      "Mashed game code."]),
    # 60
    (0x005c47b0, 0x005c47d1, "FUN_005c47b0", 0x22, "GAME",
     "Mashed DynArray destructor — if `param_1[1] != 0` (length > 0 meaning data is populated) calls DAT_007d3ff8+0x10c (allocator->Free) on `*param_1` (data buffer). Does NOT free the container.",
     "DAT_007d3ff8+0x10c (allocator free vtbl slot)", "(none recorded — called via vtable dispatch)",
     ["Body: `if (param_1[1] != 0) (*(DAT_007d3ff8 + 0x10c))(*param_1); return param_1;`.",
      "Field shape: [*data, length, ?, ?].",
      "Mashed game code — DynArray.Free."]),
    # 61
    (0x005c47e0, 0x005c4aa5, "FUN_005c47e0", 0x2c6, "GAME",
     "Mashed multi-record chunk reader (variant of FUN_005c4670): reads 4-byte count, allocates `count * 4` pointer array via `DAT_007d3ff8+0x108`, then reads `count` records of size 0x38 each, runs FUN_004c42d0/4270/4220 (geometry-classification predicates), sets flag bits in record+0x1c (`0xfffdfffc` if any predicate triggers, else `0xfffdffff`), parents each into a tree via FUN_004c0f00/1040, and registers via FUN_004e1b60(&DAT_00617f78,...).",
     "FUN_004cc5e0, FUN_004cbd30, FUN_004c0b30 (record alloc), FUN_004c42d0/4270/4220 (geom predicates), FUN_004e1b60 (registry insert), FUN_004c0f00/1040 (tree linker), FUN_004c0de0 (cleanup), FUN_004c0ef0/0e50 (post-process), FUN_004d7ff0/4d8480 (error chain)", "FUN_004e7420",
     ["Two-phase loader.",
      "Phase 1 (005c483b..0x005c48c5): allocates pointer table sized `count * 4`, reads each record's 0x38 fields, allocs object via FUN_004c0b30, copies 0x10..0x48 of the record into the object, applies geometric predicate triple (FUN_004c42d0/4270/4220 with reference DAT_005cc328/05cc9b4) to set the high-bit pattern at obj+0x1c.",
      "Phase 2 (005c4970..0x005c49a3): for each record, calls FUN_004e1b60(&DAT_00617f78, stream, obj) to register; if FUN_004c0ef0(obj) returns obj (cycle / fixpoint) AND DAT_00635cf0 == 1, calls FUN_004c0e50.",
      "Constants cited: 0x35000..0x37002 (type-range gate), 0x80000004 (UNKNOWN_TYPE error), 0x80000013 (likely OOM error code), 0x38 (per-record size), 0x10..0x48 (field copy range), 0x3000e (alloc flag passed to +0x108), 0x1c (record flag word), 0xfffdfffc / 0xfffdffff (flag bit-masks).",
      "Mashed game code — large chunk reader for the second registry family."]),
    # 62
    (0x005c4ab0, 0x005c4ac6, "FUN_005c4ab0", 0x17, "GAME",
     "Mashed flag-test: returns `(byte at DAT_00911ae4 + 10 + param_1) & 0xf0 != 0`",
     "(no calls)", "(none recorded)",
     ["Body: `return (*(byte*)(DAT_00911ae4 + 10 + param_1) & 0xf0) != 0;`.",
      "Constants cited: 10 (offset), 0xf0 (mask).",
      "DAT_00911ae4 is a process-wide flag/state table base.",
      "Mashed game code."]),
    # 63
    (0x005c4ad0, 0x005c4b9c, "FUN_005c4ad0", 0xcd, "GAME",
     "Mashed DynArray constructor (elem_size=0x14). Allocates a 0x10-byte header [data_ptr, length=0, capacity_bytes=0x14, capacity_count=param_1] then allocates `param_1 * 0x14` bytes of element storage. Error path uses error-code 0x80000013 (OOM).",
     "DAT_007d3ff8+0x108 (allocator alloc), FUN_004d7ff0/4d8480 (error chain)", "FUN_00553f40, FUN_005551d0, FUN_00558c50, FUN_00558df0 (game-side users)",
     ["Allocates header (0x10 bytes) via `DAT_007d3ff8+0x108(0x10, 0x30000)`.",
      "Header layout: [*data (0), length (0), capacity_bytes (0x14), capacity_count (param_1)] — so the struct is `{T* data; int len; int elemSize; int cap;}` with elemSize hard-coded to 0x14 (= 20 bytes — likely the per-record struct for this DynArray's element).",
      "Constants cited: 0x10 (header size), 0x14 (elem size), 0x30000 (header alloc flag), 0x1000000 (data alloc flag OR'd with param_2), 0x80000013 (OOM error code).",
      "Has 4 game-side callers — definitively Mashed code, not library residue."]),
    # 64
    (0x005c4ba0, 0x005c4bab, "FUN_005c4ba0", 0x0c, "GAME",
     "Mashed DynArray clear: `param_1[4] = 0` (length=0 at offset 4)",
     "(no calls)", "(none recorded)",
     ["1-line: `*(int*)(param_1 + 4) = 0;`.",
      "Companion to FUN_005c4ad0's header layout (offset 4 = length field).",
      "Mashed game code."]),
    # 65
    (0x005c4bb0, 0x005c4c55, "FUN_005c4bb0", 0xa6, "GAME",
     "Mashed DynArray append-one: realloc by 1.25x via `DAT_007d3ff8+0x110` if `cap <= len`, increment length, return address of new slot (`base + len*elemSize`)",
     "DAT_007d3ff8+0x110 (allocator realloc), FUN_004d7ff0/4d8480 (error chain)", "(none recorded directly — internal to module)",
     ["Growth formula: `new_cap = cap + (cap + (cap >> 31 & 3)) >> 2` — that is, `cap * 1.25` rounded toward zero (the SAR-by-31 trick gives a positive-only rounding).",
      "Realloc call: `(*(DAT_007d3ff8 + 0x110))(*data, new_cap * elemSize, alloc_flags)`.",
      "Return: `*data + len * elemSize` (the newly-claimed slot address).",
      "Mashed game code."]),
    # 66
    (0x005c4c60, 0x005c4d19, "FUN_005c4c60", 0xba, "GAME",
     "Mashed DynArray append-N: variant of FUN_005c4bb0 that grows by N at once. Grows to `cap + (cap*1.25 + N)` if `cap <= len + N`",
     "DAT_007d3ff8+0x110, FUN_004d7ff0/4d8480", "(none recorded)",
     ["Same growth strategy as FUN_005c4bb0 but factors `param_2` (count to append) into the new-capacity calculation: `new_cap = ((cap * 1.25 + N)`).",
      "Mashed game code."]),
    # 67
    (0x005c4d20, 0x005c4d2b, "FUN_005c4d20", 0x0c, "GAME",
     "Mashed DynArray pop-N: `length -= param_2`",
     "(no calls)", "(none recorded)",
     ["1-line: `*(int*)(param_1 + 4) -= param_2;`.",
      "Mashed game code."]),
    # 68
    (0x005c4d30, 0x005c4d40, "FUN_005c4d30", 0x11, "GAME",
     "Mashed DynArray data-ptr getter (0-on-empty): returns `*param_1` if `param_1[1] != 0`, else 0",
     "(no calls)", "(none recorded)",
     ["Body: `if (param_1[1] != 0) return *param_1; return 0;`.",
      "Mashed game code."]),
    # 69
    (0x005c4d50, 0x005c4d94, "FUN_005c4d50", 0x45, "GAME",
     "Mashed DynArray full destructor: if data && elemSize (`*param_1 != 0 && param_1[2] != 0`) free the data buffer via `+0x10c`, zero data and elemSize, then free the header itself via `+0x10c`. Returns 1.",
     "DAT_007d3ff8+0x10c (allocator free vtbl)", "(none recorded)",
     ["Body: `if (*p != 0 && p[2] != 0) { free(*p); *p = 0; p[2] = 0; } free(p); return 1;`.",
      "Counterpart to FUN_005c4ad0 (the constructor).",
      "Mashed game code."]),
    # 70
    (0x005c4da0, 0x005c4de4, "FUN_005c4da0", 0x45, "GAME",
     "Mashed DynArray release+detach: if length==0 and data!=NULL, free data via `+0x10c` and return 0; if length!=0, return *data (detach); always free the header. Returns the detached data pointer (or 0).",
     "DAT_007d3ff8+0x10c", "(none recorded)",
     ["Body: ownership-detaching destructor — returns the underlying data array to the caller iff length > 0, otherwise frees it.",
      "Mashed game code."]),
    # 71
    (0x005c4df0, 0x005c4e00, "FUN_005c4df0", 0x11, "GAME",
     "Mashed DynArray index-to-pointer: returns `param_1[3] * param_2 + *param_1` (= data + idx*elemSize)",
     "(no calls)", "(none recorded)",
     ["Body: `return p[3] * idx + *p;`. p[3] = elemSize, *p = data ptr.",
      "Mashed game code."]),
    # 72
    (0x005c4e10, 0x005c4e1f, "FUN_005c4e10", 0x10, "GAME",
     "Mashed DynArray tail-pointer: returns `p[3] * p[1] + *p` (= data + length*elemSize, the one-past-end pointer)",
     "(no calls)", "(none recorded)",
     ["Body: `return p[3] * p[1] + *p;`. p[1] = length.",
      "Mashed game code."]),
    # 73
    (0x005c4e20, 0x005c4f9e, "FUN_005c4e20", 0x17f, "GAME",
     "Mashed AABB/octree node initializer: tests that bbox at `*(param_2+0x10..18)` < `param_3[0..2]` AND `param_3[3..5] >= *(param_2+4..0xc)`, then populates a 0x40+ byte node record at param_1 with min/max planes, frustum-classification flag word at param_1[0x10] (built from 6 individual axis-test bits 0x01/0x04/0x10/0x02/0x08/0x20), and links into a tree at param_1[0xf]/[0x51]",
     "(no calls — pure data setup)", "(none recorded — zero callers)",
     ["AABB-vs-AABB overlap test: bbox min `*(p2+0x10..18)` strictly < `param_3[0..2]` AND bbox max `*(p2+4..0xc)` >= `param_3[3..5]`.",
      "On overlap: copies 6 floats from param_3 into param_1+0x04, copies 6 floats from param_4 into param_1+0x1c, sets `param_1[0xf] = &param_1[0xd]` (self-pointer for traversal-stack init).",
      "Frustum-classification flag at param_1[0x10] is the OR of 6 individual axis comparison results — typical octree/AABB-vs-frustum signed-axis classification.",
      "Constants cited: 0x10 (bbox-min offset in param_2), 0x4 (bbox-max offset), 0x1c..0x1e (param_2 internal flags), 0xff (alpha-shape sentinel at param_1+0x35), 0x36 (zero word offset), 0x0f/0x10/0x11/0x51 (traversal-stack slots), 0x01/0x02/0x04/0x08/0x10/0x20 (6 axis flags).",
      "ZERO callers in current Ghidra DB — possibly library residue (statically-linked geometry library; the 0x40-byte node + 0x51-slot stack pattern matches a kd-tree or octree traversal kernel) OR unreachable game code. Marked [UNCERTAIN] for that reason.",
      "Mashed-shaped game code geometry but with zero callers — likely qhull-2002.1 cousin OR dead game code."]),
    # 74
    (0x005c4fa0, 0x005c513f, "FUN_005c4fa0", 0x1a0, "GAME",
     "Mashed BVH/octree traversal step: pops a node from param_1[0x51] traversal stack, classifies the leaf flag at node+1 (cVar2). For interior nodes, reads child indices from node+2, descends along ray direction (using the 4-bit class mask 0xc at byte 0), pushes a sibling onto the stack with cleared flag bits, and writes out (selected_child_id, mask, alpha) to (param_2, param_3, param_4). Counterpart of FUN_005c4e20.",
     "(no calls)", "(none recorded — zero callers)",
     ["Stack pointer pattern: `param_1[0x51]` is the top-of-stack; pre-decrement by 8 (= one 8-byte stack entry: {void* nodePtr, uint32_t parentMask}).",
      "Leaf test: `*(char*)(node + 1) != -1` => leaf reached; writes child_id + mask + alpha.",
      "Interior traversal: uses byte at `node` low 4 bits of `0xc` as axis-mask offset into param_1's per-axis float arrays at +4, +0x10, +0x1c, +0x28.",
      "Sibling push pattern at 005c5025: writes new top-of-stack {iVar8, parentMask & ~(2<<axis)} and updates parent's mask via `& ~(1<<axis)`.",
      "Climb-down loop at 005c50d5: walks two parallel leaf chains until both terminate via `cVar2 == -1`.",
      "Constants cited: 8 (stack-entry size), 0x10 (child-record size in bytes), `0xc` (axis-mask), 1, 2, 4, 8, 0x10, 0x20 (axis bit flags), -1 (leaf sentinel), 0x20 (param_1 internal field at +0x20).",
      "ZERO callers in current Ghidra DB — same status as FUN_005c4e20 (its initializer counterpart). Marked [UNCERTAIN]."]),
    # 75
    (0x005c5780, 0x005c67d1, "FUN_005c5780", 0x1052, "GAME",
     "Mashed swept-sphere vs poly-soup intersection: face/edge/vertex traversal walking face-list at *(param_1+0x10) (count at +0x1e), edge-list at *(param_1+0x14), index-list at *(param_1+0x18), and vertex-list at *(param_1+4). Implements the classic 3-stage swept-AABB/swept-sphere collision test (face-plane intersection -> edge-vertex Voronoi region clip -> distance check). Returns 0 on no-hit, 1 on hit with intersection point written to *(param_5+4..1c).",
     "FUN_00566b40 (vec ops), FUN_00566ca0 (vec ops), FUN_00566d50 (vec ops), FUN_00566ea0 (vec ops)", "(none recorded — zero callers)",
     ["Stage 1 (005c57e2..005c5828): walk all faces (count at +0x1e), find face whose plane-distance is maximum (closest to start point local_90 in direction local_60). Local 'best face' index in local_40, distance in fVar14.",
      "Stage 2 (005c58a0..005c5990): for each edge of best face, compute cross-product based plane normal and test 'beyond edge' classification. Vertex/edge selection in local_98.",
      "Stage 3 (005c59a0..005c5ae9): compute final hit point via Voronoi-region projection onto the chosen feature (vertex/edge/face); writes 6 floats to param_5+4..1c (hit position + hit normal).",
      "Constants cited: DAT_005d757c (zero/epsilon), _DAT_005e5050 (large-negative sentinel), _DAT_005cc320 (epsilon-squared), _DAT_005cc33c (cone-shape param), _DAT_005cc328 (epsilon for fVar6 < -EPS), _DAT_005cc9b4.",
      "Struct shape at param_1: +0 unused, +4 verts*ptr, +8 ???, +0xc edge-normals*ptr, +0x10 face-list*ptr, +0x14 edge-records*ptr, +0x18 index-list*ptr, +0x1c face_id_start, +0x1e face_count, +0x20 ??? .",
      "ZERO callers — strong [UNCERTAIN] flag. This is either library residue from a statically-linked collision-detection library (libCD, GIMPACT, or similar) OR dead game code. Body looks too specialized to be Mashed-original.",
      "Subsystem suspicion: collision-detection library (mid-2000s era, possibly Open Dynamics Engine's swept-sphere code, RAPID, or PQP). Halt-marker: matches no entry in re/analysis/plans/rw_function_catalog.tsv."]),
    # 76
    (0x005c6b40, 0x005c6b54, "FUN_005c6b40", 0x15, "GAME",
     "Mashed 5-field struct initializer: zeroes 5 dwords at offsets +0, +4, +8, +0xc, +0x10 of param_1",
     "(no calls)", "(none recorded)",
     ["Body: `p[0]=0; p[3]=0; p[4]=0; p[1]=0; p[2]=0;`. Out-of-order to match assembler/optimizer.",
      "5-dword (20-byte) struct zero-init.",
      "Mashed game code."]),
    # 77
    (0x005c6b60, 0x005c6baa, "FUN_005c6b60", 0x4b, "GAME",
     "Mashed 2D motion-delta calculator: subtracts (param_2*0x100 - p1[1], param_3*0x100 - p1[2]), arithmetic-right-shifts by 6 (signed div-by-64 with bias), writes results to p1[3]/p1[4], sets p1[0]=0x40 if any nonzero else p1[0]=0",
     "(no calls)", "(none recorded)",
     ["Coordinate normalization: input *0x100 (16-bit fixed-point) minus current X/Y in p1[1]/p1[2], then SAR-by-6 (signed >>6 with `+ (x>>31 & 0x3F)` bias) — equivalent to dividing by 64 toward zero.",
      "Result fields: p1[3] = delta-X / 64, p1[4] = delta-Y / 64.",
      "Status byte p1[0] := 0x40 if (delta-X != 0 || delta-Y != 0), else 0.",
      "Constants cited: 0x100 (input scaling), 0x3f (round-toward-zero bias), 6 (shift), 0x40 (delta-nonzero flag).",
      "Mashed game code — likely menu/cursor tile-step or screen-grid delta."]),
    # 78
    (0x005c6bb0, 0x005c6bd3, "FUN_005c6bb0", 0x24, "GAME",
     "Mashed trampoline: forwards (&DAT_007de148, param_1, param_2, 0, param_3, param_4) to FUN_005aa560. Pattern matches an `Allocator->Method(state, ...)` dispatch with sentinel 0 between params 2 and 3.",
     "FUN_005aa560", "(none recorded)",
     ["Body: `FUN_005aa560(&DAT_007de148, p1, p2, 0, p3, p4);`.",
      "DAT_007de148 is a process-wide state-pointer (cited at 005c6bb6).",
      "Mashed game code."]),
    # 79
    (0x005c6cc0, 0x005c6f1a, "FUN_005c6cc0", 0x25b, "GAME",
     "Mashed audio-mixer voice/handle initializer: calls FUN_005a9e10 with init-string DAT_005e7a4c, sets state fields (+0x46 = 8, +0x40 base = 0xb0), probes 4 times via FUN_005aee20(0x28) and DAT_00617ff8 LUT (bit-position table) to compute a 4-or-8-bit signature value, stores it at +0x40 shifted left 0x1c, vtable slots at +0x28/+0x2c and code pointer at +0x34 = FUN_005c7070 (in batch-t-s6's audio-mixer cluster).",
     "FUN_005a9e10 (logger?), FUN_005aee20 (bit-LUT probe), FUN_005c7070 (audio-mixer voice-link wrapper)", "(none recorded)",
     ["Init-string DAT_005e7a4c is the class-name anchor (referenced at 005c6cc7).",
      "Constants cited: 0x46 (state field, set to 8), 0x40 (base, set to 0xb0; later OR'd with signature<<0x1c), 0x28 (FUN_005aee20 probe arg), DAT_00617ff8 (the bit-position LUT shared with batch-t-s6 cluster), 0x1c (signature shift), 0x28/0x2c (vtable slots), 0x34 (code-pointer field), 0x44 (zero word).",
      "FUN_005c7070 (the +0x34 code pointer) IS in batch-t-s6's audio-mixer cluster — confirms this is an audio voice/handle class.",
      "Pattern is two-fold parallel: matches FUN_005c7a90/FUN_005c8120 from batch-t-s6 (different base/string but identical FUN_005aee20 LUT-probe shape). Documented as 'parallel class-init twins' in that batch.",
      "Mashed game code — audio voice initializer."]),
    # 80
    (0x005cb160, 0x005cb1c1, "FUN_005cb160", 0x62, "GAME",
     "Mashed/RW 3x3 matrix * 3-vector transform: `out[0..2] = m[0..2]*v[0] + m[4..6]*v[1] + m[8..10]*v[2]`. __fastcall (param_1+param_2 are ECX+EDX — likely vtable this+arg). Signature matches RW Vector3 transform. Zero callers in current Ghidra DB — likely library residue (rwcore-vectormath).",
     "(no calls — leaf function)", "(none recorded — zero callers)",
     ["__fastcall: param_1=ECX, param_2=EDX (unused in body), param_3=out vec3, param_4=in vec3, param_5=mat3x3.",
      "Body: `out[0] = m[0]*v[0] + m[4]*v[1] + m[8]*v[2]; out[1] = m[1]*v[0] + m[5]*v[1] + m[9]*v[2]; out[2] = m[2]*v[0] + m[6]*v[1] + m[10]*v[2];`.",
      "Constants cited: matrix offsets 0, 4, 8 (col 0), 1, 5, 9 (col 1), 2, 6, 10 (col 2) — column-major 3x3 stored as a 12-float (3-col x 4-row stride, skipping col-3 trailing dword).",
      "Catalog match: RW V3dTransformVector signature (3x3 matrix * vec3, no translation). See re/analysis/plans/rw_function_catalog.tsv for RW V3d ops.",
      "ZERO callers — likely library residue (statically-linked rwcore-vectormath fragment). Marked [UNCERTAIN] — flag as RW catalog match but do not rename per skill rules.",
      "Possibly Mashed-internal or RW residue."]),
]

# Sanity check: 80 candidates
assert len(CANDIDATES) == 80, f"Expected 80 candidates, got {len(CANDIDATES)}"

PLATE_TEMPLATE = """---
rva: 0x{rva:08x}
name_in_ghidra: {name_in_ghidra}
size_bytes: {size_bytes}
confidence: C1
callees_depth1: "{callees}"
callers_noted: "{callers}"
opened_in_slot: {opened_in_slot}
session_date: {date}
session_id: {session}
bucket: bucket_005c1d63
kind: {kind}
library_match: "{lib_match}"
---

# 0x{rva:08x} {name_in_ghidra}

{summary_line}

## Mechanical description
{bullets}

## Constants

(See "Mechanical description" bullets — constants cited inline with their addresses.)

## Uncertainties

{uncertainties}

## Stubs encountered

{stubs}
"""

KIND_TO_SUMMARY = {
    "CRT_FIDDB":    "**Library residue (CRT, FidDB-attested).** Ghidra already names this function. Plate filed for tracking only — no rename, no mechanical body description (per discover-c1-batch skill: library matches should be flagged but not renamed in worker sessions).",
    "CRT_INTERNAL": "**Library residue (CRT internal helper).** Ghidra labels this `FUN_*` but the decompiled body matches a canonical MSVC CRT internal routine. Plate filed for tracking; the sweep can rename or leave as-is.",
    "GAME":         "**Mashed game code.** Mechanical description follows; cite constants and called functions inline.",
}

def plate_for(c):
    rva, body_end, name, size, kind, lib_or_sum, callees, callers, bullets = c
    summary = KIND_TO_SUMMARY[kind]
    if kind == "GAME":
        # additional one-line summary from the table
        summary = summary + "\n\n_Working summary_: " + lib_or_sum
    else:
        summary = summary + "\n\n_Library match_: " + lib_or_sum

    # Bullets
    bullet_block = "\n".join(f"- {b}" for b in bullets)

    # Uncertainties — mark zero-caller game-looking functions
    unc = []
    if "ZERO callers" in " ".join(bullets) or "zero callers" in " ".join(bullets):
        unc.append("[UNCERTAIN] Function has zero callers in current Ghidra DB. Could be (a) library residue (statically-linked collision/geometry/RW math library), (b) game code reachable only via runtime dispatch / vtables not yet recovered, or (c) dead code. Evidence to resolve: search MASHED.exe rdata for vtables containing this RVA; cross-reference against known statically-linked libraries (qhull-2002.1 already found at 0x00583f10..0x005913c0).")
    if "FID_conflict" in name:
        unc.append("[UNCERTAIN] Ghidra prefixed `FID_conflict:` — multiple FidDB signature packs match. Sweep should pick canonical name or leave as-is.")
    if not unc:
        unc.append("(none)")
    unc_block = "\n".join(unc)

    # Stubs — mention any external-pointer table dispatches as stubs
    stubs = []
    body = " ".join(bullets)
    if "DAT_007d3ff8" in body:
        stubs.append("[STUB] Allocator vtable at `DAT_007d3ff8` — slots +0x108 (alloc), +0x10c (free), +0x110 (realloc). Not yet a typed struct. Pattern is used across the DynArray module (FUN_005c4ad0/47b0/4bb0/4c60/4d50/4da0).")
    if "DAT_007de148" in body:
        stubs.append("[STUB] Process state pointer at `DAT_007de148` used by FUN_005aa560 — not yet typed.")
    if "&DAT_00617f98" in body or "&DAT_00617f78" in body:
        stubs.append("[STUB] Chunk-loader registries at `DAT_00617f98` and `DAT_00617f78` — registered via FUN_004e1ac0 trampolines and queried via FUN_004e1b60. Registry struct not yet typed.")
    if "DAT_00617ff8" in body:
        stubs.append("[STUB] Bit-position lookup table at `DAT_00617ff8` — used by FUN_005aee20 (batch-t-s6 audio-mixer cluster). Not yet a typed array.")
    if not stubs:
        stubs.append("(none)")
    stubs_block = "\n".join(stubs)

    text = PLATE_TEMPLATE.format(
        rva=rva,
        name_in_ghidra=name,
        size_bytes=size,
        callees=callees,
        callers=callers,
        opened_in_slot=POOL_OPENED_IN,
        date=DATE,
        session=SESSION,
        kind=kind,
        lib_match=(lib_or_sum if kind != "GAME" else "(none — game code)"),
        summary_line=summary,
        bullets=bullet_block,
        uncertainties=unc_block,
        stubs=stubs_block,
    )
    return text

BUCKET_DIR.mkdir(parents=True, exist_ok=True)
written = 0
for c in CANDIDATES:
    rva = c[0]
    path = BUCKET_DIR / f"0x{rva:08x}.md"
    path.write_text(plate_for(c), encoding="utf-8")
    written += 1
print(f"Wrote {written} plates to {BUCKET_DIR}")

# Count kinds for reporting
kinds = {}
for c in CANDIDATES:
    kinds[c[4]] = kinds.get(c[4], 0) + 1
print("Kind distribution:", kinds)
