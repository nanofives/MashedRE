# RESULT PORT-OT-BIND-CRT (2026-11-10): STOP, split proposed. Inventory and re-measure only, no binding yet

Verdict: **the 32 CRT functions in reach are five different binding problems, not one commit**, so this row stops at the
split the brief asked for. No code, BINDINGS or tracker change. The commit holds:
- the inventory (`verify/bindcrt_20261110/crt_inventory.tsv`);
- the group lists (`verify/bindcrt_20261110/<group>.txt`);
- two measurement options added to `verify/portorigtop_20261109/origtop_size.py`:
  - `--list <class>` prints the reached members of one class, with their caller count in reach;
  - `--assume-bound <file>` treats the listed rvas as bound.

## Re-measure (today, orch/integration 10323978f)

`origtop_size.py --list fam:crt`: **32 CRT functions in reach** (SIZE.tsv had 33). Runnable C4-by-match rows with no
new family bound: **1,027** (SIZE.tsv: 996; rows landed since).

| group (bound alone) | functions | runnable | gain |
|---|---|---|---|
| compiler helpers | 6 | 1,063 | **+36** |
| stdio + time | 8 | 1,036 | +9 |
| math | 10 | 1,030 | +3 |
| heap | 4 | 1,028 | +1 |
| string + misc | 4 | 1,027 | +0 |
| **all 32** | 32 | **1,090** | **+63** (SIZE claimed +86 on its older base) |

The groups overlap: binding all 32 releases more (+63) than the sum of the groups alone (+49).

## The groups and what each needs

Identity column: libid status against the DDK libcmt.lib (`verify/libidentity_20261030/libidentity_functions.tsv`).
L-116 applies: the `third-party-library[msvc-crt*]` filing is context only.

| group | rvas (libid) | binding problem |
|---|---|---|
| **helpers** | 0x004a2c48 `_ftol2` (ftol2.obj, identical-unlisted), 0x004a3440 `_chkstk` (chkstk.obj), 0x004a4170 `__alldiv` (lldiv.obj), 0x004a4220 `__allmul` (llmul.obj), 0x004a4e40 `__aulldiv` (ulldiv.obj), 0x004aa060 `__aullrem` (ullrem.obj) | Not C calls. `_ftol2` takes ST0 and returns EDX:EAX: the 31 matched sources declare it `ulonglong FUN_004a2c48(void)` and the value arrives on the x87 stack. The 64-bit helpers are stdcall with EDX:EAX results. `_chkstk` takes EAX. Each needs a naked-asm thunk onto the exe's own helper (VS2022 `_ftol2` / `_alldiv` / ...), plus a check that no matched source depends on `_ftol2`'s DDK rounding control. Largest yield, x87 / calling-convention work: one commit. |
| **stdio + time** | 0x004a4541 `fopen` (FsopenSafe; identical-unlisted fopen/wfopen), 0x004a4368 `fclose` (identical), 0x004a49cf / 0x004a4b22 `fread` (identical-ambiguous), 0x004a4254 `fputs` (**differs**), 0x004a3b84 `_vsnprintf` (identical-unlisted), 0x004a43b9 `ctime` (identical-unlisted, 4 candidates), 0x004aa00c `_tzset` (identical) | **State ownership:** the matched sources pass and dereference a DDK `FILE` (`FILE_h4`), so the exe's `FILE*` must never reach code that reads the DDK layout. `fopen` carries boot patch `fix_fopen` (NULL on a garbage FILE*). `_tzset` writes the time-zone globals that `ctime` reads, so they need one owner. fputs (differs) and the ambiguous fread pair need code-level identity first. |
| **math** | 0x004a2d30 `pow` (identical), 0x004a3090 `ceil` (identical-ambiguous, pentium4 variant), 0x004a3620 `atan` (identical-unlisted, atan/log/log10 share the body), 0x004a371d `sin` / 0x004a37cd `cos` (**differ**), 0x004a3907 ieeemisc (identical), 0x004a39d4 (**not compared**), 0x005c1cb7 `hypot` (identical), 0x004a70d5 / 0x004a7118 common.obj (identical) | x87 results. The exe's CRT math is not bit-identical to the DDK's (sin/cos already differ from the DDK object), so binding is a value change on the knob path. It needs a ruling, or byte-matched bodies (libid route), instead of the exe CRT. |
| **heap** | 0x004a45fb `malloc`, 0x004a460d `free` (identical-unlisted), 0x004a3bda `operator delete` (identical), 0x004a3c1c `operator new` (identical-ambiguous, _wctype.obj listed) | **One owner:** allocations must not cross heaps. RwGlobals `+0x108..+0x114` already route RW allocations to librw. The four bind together, to the exe heap, and nothing allocated by the original-layout code may be freed elsewhere. Yield +1 alone. |
| **string + misc** | 0x004a2fd0 `strchr` / 0x004a3480 `strrchr` (**differ**), 0x004a3bdf `wcsncpy` (identical), 0x005a7b40 `AudioCtxSwap5a7b40` (C3; identical-unlisted to **both** fpinit.obj and user.obj) | +0 alone. strchr / strrchr need code-level identity (the DDK object differs). **0x005a7b40 is reported, not bound:** it is filed `msvc-crt` and named `AudioCtxSwap`, and its libid hits two unrelated CRT objects, so its identity is open (the L-116 pattern). |

## Proposed split (cheapest yield first)

1. **BIND-CRT-HELPERS** (+36): six naked thunks, identity by libid. Includes an `_ftol2` rounding check.
2. **BIND-CRT-STDIO** (+9): FILE / tz state ownership, code-level identity for fputs and the fread pair, and keeping the
   `fopen` boot-patch behaviour.
3. **BIND-CRT-HEAP** (+1, but needed by 2 for buffers): the four allocators together.
4. **BIND-CRT-MATH** (+3): needs a decision. Exe CRT values, or the libid / byte-matched DDK bodies?
5. **BIND-CRT-STRING** (+0): strchr / strrchr identity, wcsncpy. 0x005a7b40 reported separately.

rva-lint: OK. No build input changed.
