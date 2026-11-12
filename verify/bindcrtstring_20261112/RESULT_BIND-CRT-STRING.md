# RESULT BIND-CRT-STRING (2026-11-12, roadmap run 39): GREEN

The original's `strchr`, `strrchr` and `wcsncpy` are bound to the exe CRT on the MASHED_ORIG_TOP path
(`Match/PortTop_CrtString.cpp`, exe-only, `// RVA-LINT: ORIGTOP-ONLY`). 0x005a7b40 is **not** bound: its identity is
still open (section 3). This is a top-down row. No file the default path executes changed, so per WORKER_RULES
("Row kinds") the worker ran both builds, the ddkobj check, the knob boots and the self-test. G-CTRL / B are left to
the merge train. Base: `orch/integration` @ `7de0787b1`.

## 1. Bindings (3 rows, kind `crt`, `update_bindings_bindcrtstring.py` through `bindings_rows.apply`)

| rva | function | identity | exe binding |
|---|---|---|---|
| 0x004a2fd0 | strchr | libid strchr.obj **differs on bytes only**. Normalized listing equal, CF-safe, differential clean (section 2) | `MashedCrt_strchr_004a2fd0` -> `std::strchr` |
| 0x004a3480 | strrchr | libid strrchr.obj **differs on bytes only**. Normalized listing equal, CF-safe, differential clean | `MashedCrt_strrchr_004a3480` -> `std::strrchr` |
| 0x004a3bdf | wcsncpy | libid wcsncpy.obj identical (61 bytes, no relocations) | `MashedCrt_wcsncpy_004a3bdf` -> `std::wcsncpy` |

All three are pure functions of their arguments: no CRT state, no locale, no heap. BINDINGS.tsv 551 -> 554 rows
(`crt` 22 -> 25).

**Reach.** The callers in reach are `re/match/s01/00402b70.c`, `re/match/s16/004944c0.c` and
`re/match/s16/00494c80.c`. `origtop_size.py` on this branch, measured against the same tree with the three rows taken
out (`BINDINGS.tsv` at HEAD of `orch/integration`):

| measure | without | with | delta |
|---|---|---|---|
| runnable with no new family bound | 1,207 | 1,216 | **+9** |
| final (every binding family bound) | 1,381 | 1,393 | +12 |
| `fam:crt` left to bind in reach | 14 | 11 | -3 |

The committed SIZE.tsv was older than `orch/integration` (it read 1,075 runnable), so its diff also carries the rows
that landed since. The +9 above is this row alone. The PORT-OT-BIND-CRT split predicted +0 for this group on its
10 Nov base. The L-126 admitted rows (31 now) and rows landed since gave these functions callers that are runnable.

## 2. Identity of strchr / strrchr (`crt_string_identity.py`, output `identity_output.txt`)

PORT-OT-BIND-CRT left both as "differs, code-level identity needed". The libid first differences were
strchr `+0x17 lib[je 0x2b] orig[je 0x2d]` and strrchr `+0xe lib[inc ecx] orig[add ecx, 1]`
(`verify/libidentity_20261030/libidentity_functions.tsv`).

**Leg 1, normalized listing.** The script disassembles the DDK object's whole `.text`
(`tools/vc7ddk/lib/wxp/i386/libcmt.lib`). For strchr that includes the `found_bx` tail placed before the entry, at
image 0x004a2fc0. It disassembles the same instruction count from the image and rewrites `inc r` to `add r, 1` and
`dec r` to `sub r, 1`. Every branch target becomes an instruction index.

| function | image span | insns | DDK bytes | image bytes | inc/dec in DDK | in image | normalized |
|---|---|---|---|---|---|---|---|
| strchr | 0x004a2fc0..0x004a308e | 91 | 204 | 206 | 1 | 0 | **equal** |
| strrchr | 0x004a3480..0x004a34ad | 23 | 39 | 45 | 3 | 0 | **equal** |

So the only difference is the encoding of 4 increments / decrements. The `je` displacement at strchr +0x17 is the
2-byte shift that one `add edx, 1` (0x004a2fea) causes. `inc` / `dec` leave CF unchanged, and `add` / `sub` write it.
At each rewritten site the next instruction that touches CF writes it, with no CF reader or branch in between:
- strchr #14 (0x004a2fea `add edx, 1`) is followed by `cmp`.
- strrchr #7 (0x004a348e `add ecx, 1`) is followed by `neg`.
- strrchr #9 (0x004a3493 `sub edi, 1`) is followed by `repne scasb`. ecx is non-zero there, so at least one
  iteration runs and writes the flags.
- strrchr #13 (0x004a349c `add edi, 1`) is followed by `cmp`.

The two bodies are therefore equal as computations.

**Leg 2, differential run (unicorn).** The image bytes and the DDK object bytes run on the same 4,000 inputs per
function. The inputs cover random strings at start alignments 0..3, c = 0, c > 0xff, absent c and present c. The
returned pointer is compared with the C-standard result. Result: **0 mismatches** for strchr, strrchr and wcsncpy
(wcsncpy also compares the whole destination buffer). Callee-saved registers and cdecl stack balance are asserted on
every call.

## 3. 0x005a7b40: reported, not bound

`AudioCtxSwap5a7b40`, filed `third-party-library[msvc-crt]`. Its body at 0x005a7b40..0x005a7b4f is
`mov ecx,[esp+4]`, `mov eax,[0x007dcabc]`, `mov [0x007dcabc],ecx`, `ret`. libid matches it to two unrelated objects,
fpinit.obj `___setfflag` and user.obj `?__set_inconsistency@@YAP6AXXZP6AXXZ@Z` (16 bytes, identical-unlisted both).
A byte scan of `.text` finds:
- 0x007dcabc is referenced at 0x005a7855, 0x005a7894, 0x005a7907, 0x005a7a19 and 0x005a7a2c, besides the body.
- The function is called with E8 from 0x005a7c9b, 0x005a7cc8, 0x005a7d23, 0x005a7dd1 and 0x005a7e1a.

All of these lie in 0x005a78xx..0x005a7exx, outside the CRT block that holds the other 31 CRT functions in reach
(0x004a2c48..0x004aa060). This does not decide between the two library identities, or a non-library function with
the same 16 bytes. [UNCERTAIN] its identity: the missing evidence is the identity of the functions at
0x005a7855..0x005a7e1a that share 0x007dcabc. Per the brief it stays unbound. A follow-up would be its own row.

## 4. Self-test (`run_selftest.bat`, output `selftest_output.txt`): **6/6 PASS**

`string_selftest.cpp` compiles `Match/PortTop_CrtString.cpp` unchanged (MASHED_STANDALONE, VS2022 x86 /MT like the
exe). It copies the original's own machine code for the three functions from `original/MASHED.exe.unpatched` into
executable memory. strchr includes its `found_bx` tail. None of the three has a relocation or an absolute address.
The test calls them natively next to the bindings:

| # | check | cases |
|---|---|---|
| 1 | strchr image == binding (random strings, alignments 0..3, c = 0 / > 0xff / absent / present / negative) | 100,000 |
| 2 | strrchr image == binding (same inputs) | 100,000 |
| 3 | wcsncpy image == binding (return value + whole destination buffer, count below / at / past the length) | 20,000 |
| 4 | empty string: c = 0 gives the terminator, absent c gives NULL | 6 |
| 5 | c > 0xff truncates to char | 4 |
| 6 | c = 0 returns the terminator | 4 |

## 5. Builds, object build, boots

| check | result |
|---|---|
| default build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, .asi not deployed. mashed_re.exe sha256 `18dcb1383d80368bdb0678f3b6f11482f9ac0db9dda0a8327fd2af9fbd056017`. The new TU gives one C4996 deprecation warning on `wcsncpy` (warning only) |
| object build (`MASHED_DDKOBJ=1`, NO_DEPLOY) | `=== Build OK ===`, refusals 47 (static-register-convention 38, wrapper-redefines-globals 9), the same as on integration. None is in this row: the row adds no matched unit. sha256 `ce3e7a73be8bfa6768f8f7f9c5f1eab896cfb4e4b05d47e4e7b0add14656638c` |
| `ddkobj.py check` | IDENTICAL 714/796, DIFFERENT 79, not-in-map 3. Equal to the latest integration figure (`verify/provbodies_20261112/RESULT_PROVISIONAL-BODIES.md:33`, PROV-WRITESET-114 714/796, 79) |
| knob boots, `MASHED_ORIG_TOP=1`, 3000 frames, private root `D:/MashedWT/BIND-CRT-STRING_root`, under `mashed_lock.py run` | **4/4 rc 0**, `WinMain 0x00492370 returned 0 after 3000 frames`: default RWMOD off / on, object build RWMOD off / on |
| self-test | **6/6 PASS** (section 4) |
| port column | `scripts/backfill_port.py` refused with FATAL: `exe_file` is stale on 242 rows on the integration base (first 004c2f60). It wrote nothing. Not this row's rows: every `crt` binding stays `none` (L-67). `backfill_exe_file.py` was not run, because it would rewrite 242 rows that belong to other rows |

| captures / G-CTRL / B | not run: top-down row (WORKER_RULES "Row kinds"), no menu code changed. The merge train runs the default gates |
| rva-lint | OK, 96 known, 0 new |

Raw files (not committed), sha256:
```
6609a2ec72ac22ddbfeff10f923a912cb48ca000cf085a7a7cd3f1adeec6121d  D:/MashedWT/BIND-CRT-STRING_data/boots/ddk_on_rw0.log
2d2b7282a0b1edbe8df5bf7adf505186696f04294c0b418239ec5f886692b26a  D:/MashedWT/BIND-CRT-STRING_data/boots/ddk_on_rw1.log
010d7cb1cc7ee75125ef87f19067d4d53488561ea27e69dc2355cbe66d9aa8de  D:/MashedWT/BIND-CRT-STRING_data/boots/def_on_rw0.log
03daf033144b99b8ea0156b3f829c0735bbf2c91a534015267ad82b9025fe4ff  D:/MashedWT/BIND-CRT-STRING_data/boots/def_on_rw1.log
f348004e70c11b9701e89aea249bc271e22573b23770b604246d716cb45b677d  D:/MashedWT/BIND-CRT-STRING_data/build_default.log
1a54c00c7b377d43121d23a288bea5b2b400fa97027d2323e377397c5fcf8a5a  D:/MashedWT/BIND-CRT-STRING_data/build_ddkobj.log
6feab3be54796854511c094a9b9387642e8c2f181b267837678ea84ed92e57de  D:/MashedWT/BIND-CRT-STRING_data/ddkobj_check.txt
```
