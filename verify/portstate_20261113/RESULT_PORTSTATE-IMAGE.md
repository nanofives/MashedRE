# RESULT PORTSTATE-IMAGE: design of the whole data image (L-121 direction b)

Row `PORTSTATE-IMAGE` (section D4, run 43). Branch `orch/PORTSTATE-IMAGE` from `orch/integration` @ `491c841da`.
Design only: no port code, no build, no game run, no gates. Worker cmwf1n0zv0m4zlh1ngz3502xu.

## Verdict: GREEN (design complete)

The image is three file-backed ranges of `original/MASHED.exe.unpatched`, 434,252 bytes, written once into the B17 pad
at boot from a staged data file. Every byte of the four per-address tables in the tree today (PortStateSeed 272
entries, RdataSeed 41, DInputSeed 2, CodePtr CP_SEED 72) equals the image at its address (checked below, 387 of 387).
The binary has no base relocations, so code pointers in the data cannot be found exactly from the file: 2,124 aligned
dwords land in `.text` and at least 330 of them are text (strings, UTF-16). The fix-up table is therefore a
positive-evidence list (today's 188 CODEPTR slots, then a Ghidra data-reference export), and every code pointer left
unfixed is caught when it is called by a trap on the original `.text` range. Recommendation for L-121: adopt (b) on
the `MASHED_ORIG_TOP` path only, behind a default-OFF knob; the default (hand) path keeps the per-address seed until T4
deletes it.

Note on the count: the row title says 259 seed entries. The tree today has **272** (`SEED_CANDIDATES.tsv` 272 rows,
`PortStateSeed.inc:6-8` `ENTRIES 272u`, `BYTES 3231u`, FNV `0x5ecb7d52`). The design does not depend on the count.

## Self-test (what was run, read-only)

Three scripts, committed here, run against `original/MASHED.exe.unpatched` (read, never written) and the committed
tables. Outputs committed beside them.

| Script | Output | What it shows |
|---|---|---|
| `image_survey.py` | `image_survey.txt` | PE header, sections, data directories, base relocations; equality of the four seed tables with the file |
| `codeptr_classify.py` | `codeptr_classify.txt` | aligned code-range dwords in `.rdata`/`.data`, IAT contents, CODEPTR_SLOTS coverage, the two B16 constants |
| `codeptr_split.py` | `codeptr_split.txt` | the 2,124 code-range dwords split by mechanical evidence (function entry, text bytes, table shape) |

Key lines (verbatim):

```
file ../../original/MASHED.exe.unpatched size 2846720 sha256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
e_lfanew 0x110 machine 0x14c nsec 6 timestamp 0x40cd80ea characteristics 0x010f (RELOCS_STRIPPED=1)
optional magic 0x10b ImageBase 0x00400000 SectionAlignment 0x1000 FileAlignment 0x1000 SizeOfImage 0x595000
base relocations HIGHLOW: 0
PortStateSeed.inc: entries 272 bytes 3231 equal-to-file 272 other 0 sections {('.rdata', 'file'): 147, ('.data', 'file'): 125}
PortTop_RdataSeed.inc: entries 41 bytes 641 equal-to-file 41 other 0 sections {('.rdata', 'file'): 41}
PortTop_DInputSeed.inc: entries 2 bytes 32 equal-to-file 2 other 0 sections {('.rdata', 'file'): 2}
PortTopCodePtr.inc CP_SEED blocks equal-to-file 72 of 72, bytes 288
PortTopCodePtr.inc CP_FIX slots whose image dword lies in .text: 188 of 188
aligned dwords into a code section: 2124
CODEPTR_SLOTS.tsv slots 188, in this scan 188, missing []
B16 const 005cd5a8 image 3acccccd written-by-B16 3acccccd equal=True
B16 const 005cc560 image 3b088889 written-by-B16 3b088889 equal=True
```

(The "reloc code slot" columns in `image_survey.txt` read 0 because the file has no relocations; they are not a
finding.)

## 1. Image layout

Section headers of `MASHED.exe.unpatched` (file offsets of each 40-byte header in brackets), `ImageBase` 0x00400000:

| Section | Header | VA range | VirtualSize | Raw size @ file | File-backed | BSS tail | Characteristics | In the image |
|---|---|---|---|---|---|---|---|---|
| `.text` | 0x208 | 0x00401000..0x005ca8e0 | 0x1c98e0 | 0x1ca000 @ 0x001000 | 0x1c98e0 | 0 | 0x60000020 (code, X/R) | **no**: trapped (section 4) |
| `_rwcseg` | 0x230 | 0x005cb000..0x005cb451 | 0x000451 | 0x001000 @ 0x1cb000 | 0x000451 | 0 | 0x60000020 (code) | **no**: trapped |
| `.rdata` | 0x258 | 0x005cc000..0x005e9044 | 0x01d044 | 0x01e000 @ 0x1cc000 | 0x01d044 | 0 | 0x40000040 (R) | **yes**, 0x1d044 bytes |
| `.data` | 0x280 | 0x005ea000..0x00914704 | 0x32a704 | 0x04d000 @ 0x1ea000 | 0x04d000 | 0x2dd704 (0x00637000..0x00914704) | 0xc0000040 (RW) | **yes**, 0x4d000 bytes; BSS stays the pad's zeros |
| `_rwdseg` | 0x2a8 | 0x00915000..0x00915008 | 0x000008 | 0x001000 @ 0x237000 | 0x000008 | 0 | 0xc0000040 | **yes**, 8 bytes (all zero in the file) |
| `.rsrc` | 0x2d0 | 0x00916000..0x00994b18 | 0x07eb18 | 0x07f000 @ 0x238000 | 0x07eb18 | 0 | 0x40000040 | **no**: the one resource used is embedded (L-64 b) |

Image total: 0x1d044 + 0x4d000 + 0x8 = **434,252 bytes**. L-99's "about 428 KB" counted `.rdata` at its raw size
0x1e000; the mapped size is VirtualSize 0x1d044, and the 0xfbc raw bytes past it are zero in the file
(`rdata tail ... all zero: True`), so the difference carries no data.

Data directories inside the image ranges (all in `.rdata`): IAT 0x005cc000 size 0x2e8 (header field at file 0x1e8),
DEBUG 0x005cc2f0 size 0x1c (0x1b8), IMPORT 0x005e7ff4 size 0xc8 (0x190). RESOURCE is `.rsrc`, outside the image.

Where it lands in the exe: the pad `g_b17_low_arena_pad[0x00A00000]` (`exe_main.cpp:294-295`) is at 0x00270ec0 in the
current map (`mashedmod/build/mashed_re.map:9376`, `0003:0003bec0`, preferred load 0x00010000, `build.bat:264`
`/BASE:0x10000 /FIXED:NO /DYNAMICBASE:NO`), so it spans 0x00270ec0..0x00c70ec0 and contains every range above. The
loader keeps the per-range bounds check `PortStateSeed.cpp:63-70` uses (`a < lo || a + len > hi` -> refuse), because
the pad start moves with the exe's own size.

## 2. The staged data file

**Name and place.** `mashed_image.bin`. Release: `original\mashed_image.bin` in the stage (beside the other original
data, listed in `manifest.json`, so the updater fetches and verifies it like any other file). Dev: written by the build
into the build output folder, beside `mashed_re.exe`. Nothing is written into the dev `original\` (WORKER_RULES
"Never touch original\"). Lookup order at boot: `MASHED_IMAGE=<path>`, then `<exe dir>\mashed_image.bin`, then
`<root>\original\mashed_image.bin` where `<root>` is the folder `exe_main.cpp:9598-9610` already resolves
(`MASHED_ROOT`, else the exe's folder).

**Format** (little endian, version 1). Bytes only: no fix-up is baked in, because the fix-up values are exe addresses
that change with every link.

| Offset | Field |
|---|---|
| 0x00 | magic `MSIM` |
| 0x04 | u32 version = 1 |
| 0x08 | 32 B SHA-256 of the source binary (must be the anchor `BDCAE093...3C0E`) |
| 0x28 | u32 range count N (3) |
| 0x2c | N x { u32 va, u32 len, u32 payload offset, u32 FNV-1a 32 of the range, char name[8] } |
| ... | payloads, each the raw file bytes of `[va, va+len)` |
| end-32 | 32 B SHA-256 of every byte before it |

The three ranges are { 0x005cc000, 0x1d044, `.rdata` }, { 0x005ea000, 0x4d000, `.data` }, { 0x00915000, 0x8,
`_rwdseg` }. The extractor takes them from the section table, not from constants, and refuses when the headers
differ from the table in section 1.

**Extractor.** One tool, `re/tools/image/extract_image.py --exe <MASHED.exe.unpatched> --out <file>`: checks the
SHA-256 anchor, reads the section table, writes the file, prints the three range FNVs. Two callers:

- `scripts/lan_release/stage_lan_release.py`: after the TOASTART/toastaudio loop (`stage_lan_release.py:87-101`), call
  the extractor on `<--install>\MASHED.exe.unpatched` and `place()` the result at `original\mashed_image.bin`, adding it
  to `wanted`. A missing or wrong-hash `.unpatched` is a hard stop (`sys.exit`), the same shape as the Frontend.piz
  marker check (`stage_lan_release.py:87-89`). The exe itself is still the only binary in `APP_FILES`
  (`stage_lan_release.py:43`); no MASHED.exe is staged (L-64 b, docstring lines 19-27 stay true).
- `mashedmod\build.bat`: a post-link step that writes `%OUT%\mashed_image.bin` when it is missing or older than the
  `.unpatched`. `publish.sh` builds with `MASHED_NO_DEPLOY=1` from a `git archive`; the stage, not the build, provides
  the release copy, so the publish path needs no change beyond the stage.

**Nothing embedded in the exe.** The exe carries the loader, the format check and the fix-up table (addresses only,
section 3). It holds no original byte. Today's generated tables do hold original bytes (3,231 + 641 + 32 + 288) and
retire with the migration (section 5).

**Load.** In `WinMain` at the call that is `PortStateSeed_Apply()` today (`exe_main.cpp:10640`), before any game code,
when the knob is ON: open, check magic, version, source SHA-256, trailer SHA-256 and each range FNV, check each range
lies inside the pad, then `memcpy` the payloads. Any failure writes nothing and logs one `[portstate-image]
REFUSED(<reason>)` line, the `REFUSED(fnv)` convention of `PortStateSeed.cpp:84`. Before the copy, the loader counts
nonzero bytes already present in the three ranges and logs the count: anything written to the pad before WinMain
(static initialisers, the B17 TLS callback) would be overwritten, and this count makes that visible. [UNCERTAIN]
whether any C++ static initialiser writes into 0x005cc000..0x00637000 today; no evidence either way was collected,
the count answers it on the first knob boot.

## 3. Code-pointer fix-up table

**Why it cannot be derived from the file alone.** `characteristics 0x010f` has IMAGE_FILE_RELOCS_STRIPPED set and the
BASERELOC directory is empty (`base relocations HIGHLOW: 0`), so the file does not say which dwords are pointers. A scan
finds 2,124 aligned dwords in the file-backed `.rdata` (893) and `.data` (1,231) whose value lies in
0x00401000..0x005ca8e0, none into `_rwcseg`. They include text: the seed entries 0x005cfc2c and 0x005cfc70 are
UTF-16 strings (`S\0O\0U\0R\0C\0E\0`, `T\0E\0X\0T\0U\0R\0E\0...`) whose dwords land in `.text`. Split
(`codeptr_split.txt`, `re/console/pc_funcs.csv` 6,498 bodies):

| Class | Count |
|---|---|
| value is a function entry | 768 (644 of them have a code-range neighbour, a table shape) |
| inside a known body, not an entry | 86, plus 88 whose bytes are also printable |
| isolated, bytes printable | 330 |
| outside every known body | 756, plus 96 printable |

The split does not decide pointer vs data. The 188 slots PORT-OT-SEED-CODEPTR proved by Ghidra label
(`verify/portorigtop_20261109/CODEPTR_SLOTS.tsv`) fall as 97 entries, 91 outside every `pc_funcs` body (LAB_ targets
such as 0x0049dd00 that `pc_funcs.csv` does not list as functions), and 41 of the 188 have printable bytes. So no
byte heuristic is sound in either direction: an entry test misses LAB_ targets, a text test drops real pointers.

**Rule: positive evidence only.** A dword is rewritten only when a committed list names it. Rewriting a dword that is
text corrupts it silently; leaving a real pointer unfixed is caught loudly by the trap when it is called (section 4),
and reads of it as data are unaffected. So the table may be incomplete but must not be wrong.

**Sources of the list, in order.**

1. Today: the 188 slots of `CODEPTR_SLOTS.tsv` (generated by `verify/portorigtop_20261109/codeptr_inventory.py`
   from Ghidra labels in `SEED2_ADDED.tsv` with action `code-pointer`). All 188 hold a `.text` value in the image.
2. Whole image: a headless export of every reference whose source is in 0x005cc000..0x00637000 or 0x00915000..0x00915008
   and whose destination is in a code section, with Ghidra's data type at the source (pointer / string / other). Run
   from a ghidra-pool slot with a script next to `re/tools/ghidra_scripts/XrefRange.java`. A source inside a
   defined string or a non-pointer type is excluded. Output: `re/tools/image/CODEPTR_IMAGE.tsv` {slot, original
   target, ghidra type, source label}. This needs Ghidra, so it is an account3 row (`re/ACCOUNT2_CAPABILITIES.md`:
   Ghidra MCP is account2's one hard blocker).
3. Trap reports (section 4): every `TRAP execute` whose return address shows a call through a data slot names that
   slot. It is added to the list with the report as evidence.

**Rewrite.** For each listed slot the exe's generated table carries {slot VA, original target VA, exe symbol}. The exe
symbol comes from `verify/porttop_20261108/BINDINGS.tsv` (matched / bound / port body for that RVA), exactly as
`PortTopCodePtr.inc` does today (`CP_FIX(slot, sym, orig)`, `PortTopCodePtr.cpp:93-101`). The loader first checks
that the image dword at the slot equals the original target (a mismatch means the list and the image disagree: refuse
that slot and count it), then writes the exe address. The read-back self-test of `PortTopCodePtr.cpp:103-131` carries
over unchanged (value equals the symbol, lies in the exe image, not in the pad).

**A pointer into an unported original `.text` address.** Two choices, the design picks (b):

- (a) today's stop thunk per target (`CP_STOP`, `PortTopCodePtr.cpp:66-69`): 178 of the 188 slots bind to one today
  (`PortTopCodePtr.inc` header: "10 slots bound ... 178 to a stop thunk").
- (b) **leave the original value in place** and let the trap report the call. Same information (target and return
  address), no generated thunk per target, and it also covers the slots the list does not know yet. A listed slot
  with no exe body is then a no-op entry kept for the report's name. The stop thunks retire with the CP table.

Exception: pointer equality. A matched body that compares a slot against `&FUN_x` compiled in the exe compares an exe
address against the original value and differs. Only bound slots are rewritten, so this can only occur for an unbound
target, where the call would trap anyway. [UNCERTAIN] whether any matched body compares a code pointer without calling
it; no scan for that pattern was run. The rewrite list makes it checkable: a `cmp` against a listed target in a
matched source is a lint rule for the implementation row.

**IAT (0x005cc000..0x005cc2e8).** 186 dwords, 177 nonzero; the file holds hint/name RVAs (sample `001e89e0 001e89be
001e89cc 001e9018`), not addresses. In the exe those values lie below the pad start 0x00270ec0, that is inside
`mashed_re.exe`'s own image (preferred base 0x00010000), so a call through an IAT slot would jump into exe bytes with
no fault. The loader therefore **zeroes the IAT range after the copy** (a call through it faults at 0 and the trap
handler reports it, decoding `FF 15 <slot>` at the return address minus 6 to name the slot). Binding the IAT to the
exe's own imports is possible from image data alone (import directory 0x005e7ff4, nine DLLs: ADVAPI32 4 @0x005cc000,
DINPUT8 1 @0x005cc014, DSOUND 1 @0x005cc01c, GDI32 2 @0x005cc024, KERNEL32 118 @0x005cc030, USER32 39 @0x005cc20c,
WINMM 5 @0x005cc2ac, d3d9 1 @0x005cc2c4, ole32 6 @0x005cc2cc) but is not proposed: matched bodies compiled into the
exe call through the exe's own IAT, and the CRT binding rows (`BIND-CRT-*`) decide what original imports resolve to.

## 4. The trap on the original code range

**Range.** 0x00401000..0x005cbfff: `.text` (0x00401000..0x005ca8e0) and `_rwcseg` (0x005cb000..0x005cb451), page
rounded; 459 pages. It stops before `.rdata` at 0x005cc000. Today these bytes are pad zeros, so a call into them runs
`00 00` (`add [eax],al`) and goes on silently or faults far from the cause. Example in the tree today: the seed writes
`PTR_FUN_005f99c4` = 0x0045b350 verbatim (`PortStateSeed.inc:824`) on the default path, where no CP fix-up runs.

**Mechanism.**

1. After the image load, `VirtualProtect` the range to `PAGE_NOACCESS` (page granularity inside the exe's committed
   `.bss`; no new reservation). This faults on execute, read and write alike, with or without DEP.
2. Carve-outs: `StandaloneThunks_MapRange` / `StandaloneThunks_Install` (`Compat/StandaloneRvaThunks.h`, the only
   writers into this range, called from `exe_main.cpp:9944-9961`: maps 0x00420000+0x60000 and 0x005c0000+0x10000,
   thunks at 0x0042b8b0, 0x0042b8c0, 0x0042d5a0, 0x00472c60, 0x00472f40, 0x004730b0) call a new
   `CodeTrap_AllowPage(page)`. A carved page becomes `PAGE_EXECUTE_READWRITE` and is filled with `0xCC` on its first
   carve, so the bytes around a thunk still trap, as `EXCEPTION_BREAKPOINT`. Note `exe_main.cpp:9945` maps
   0x005c0000..0x005cffff, which overlaps `.rdata` 0x005cc000..0x005cffff; the carve-out must clip to the trap range so
   that `.rdata` image pages are never filled.
3. A vectored handler (`AddVectoredExceptionHandler(1, ...)`) takes `STATUS_ACCESS_VIOLATION` whose fault address is in
   the range and `EXCEPTION_BREAKPOINT` whose `Eip - 1` is in the range. Everything else returns
   `EXCEPTION_CONTINUE_SEARCH`, so `__try` blocks such as `exe_main.cpp:9950-9954` keep working.

**Report** (one line, `[code-trap]`, in `mashed_re.log`, then `ExitProcess(<original VA>)`, the exit convention of
`PortTopCodePtr.cpp:61-63`):

- kind: `execute` (AV with `ExceptionInformation[0] == 8`, or a breakpoint), `read` (0), `write` (1);
- the original VA (fault address, or `Eip - 1`);
- for execute: `[esp]` as the return address, as `exe+0x<off>` against `mashed_re.map`, and, when the 6 bytes before
  it are `FF 15 <imm32>` or `FF 1x` forms with an absolute operand, that slot address (names the data slot or IAT slot
  the call went through);
- for read/write: `Eip` as `exe+0x<off>`.

Names are not resolved in-process (no symbol table shipped). The log line is joined offline with `re/console/pc_funcs.csv`
and `hooks.csv` by a small reader, which prints the function name, its confidence and its `port` column.

[UNCERTAIN] whether anything reads original `.text` bytes as data on the ORIG_TOP path (jump tables of the original live
in `.text`; matched bodies compiled into the exe use their own). The first knob boot answers it: a `read` report names
the reader.

## 5. Migration from the per-address seed

Tables the image subsumes (all byte-equal to it, self-test above): `Compat/PortStateSeed.inc` (272 entries,
`gen_portstate_seed.py`), `Match/PortTop_RdataSeed.inc` (41, `verify/porttop_20261108/gen_rdata_seed.py`),
`Match/PortTop_DInputSeed.inc` (2, `gen_dinput_seed.py`), the 72 `CP_SEED` blocks of `Compat/PortTopCodePtr.inc`, and the
two B16 scale constants written by hand at `exe_main.cpp:9951-9952`. Out of scope (L-148, frozen until T4): the hand
transcriptions of MATCH-PORTSTATE-COURSE (`Track/CourseTableState.cpp`) and every unit that references `kCdRows`,
`kTrackTable`, `kSaveBufBase` or `Tables().body`. The image does not touch them and this design proposes no work on them.

Steps, each its own row:

| Step | Content | Path | Proof |
|---|---|---|---|
| IMG-1 | `extract_image.py`, the format, the stage and build hooks, an offline check `image_check.py` (this RESULT's `image_survey.py` made permanent: every entry of the four tables equals the file image) | tools only | check prints 387/387 equal, 0 overlap with the refused IAT range |
| IMG-2 | Loader behind `MASHED_PORTSTATE_IMAGE=1` (default OFF), **only on the `MASHED_ORIG_TOP` branch** (`exe_main.cpp:10643-10651`). With it ON the per-address writes are skipped (PortStateSeed, RdataSeed, DInputSeed, CP_SEED) and the CP fix-ups run on top of the image. Top-down row: knob boot, self-test | knob path | see below |
| IMG-3 | Trap behind `MASHED_CODE_TRAP=1` (default OFF), ORIG_TOP only, with the carve-out hook | knob path | knob boot report: zero `read`/`write` lines, every `execute` line names a target already in the stop list or a new slot |
| IMG-4 | CP list from the Ghidra export (account3) | knob path | `CODEPTR_IMAGE.tsv` contains all 188 current slots; each new slot cites its Ghidra type |
| IMG-5 | Decision row: image + trap default ON on the ORIG_TOP path (L-127 class P1 if the knob-path captures and logs are unchanged ON vs OFF on OFF/ON/OFF/ON; otherwise a decision row) | knob path | as stated |

**Test that the image equals the seed on the seed's addresses.** Offline: `image_check.py` (IMG-1), today 272/272,
41/41, 2/2, 72/72. In-process on the knob boot: with the image ON and the seed write skipped, `PortStateSeed_Log`
still runs its memory-side FNV (`PortStateSeed.cpp:42`, `FnvEntries(true)` over the entry list) and must print
`mem=5ecb7d52`, equal to `MASHED_PORTSTATE_SEED_FNV1A` (`PortStateSeed.inc:8`). That proves the image put the seed's
3,231 bytes at the seed's 272 addresses without the seed writing them. The same witness is added for the Rdata and
DInput tables (FNV over their {addr, len} list, memory side).

**Default path.** Unchanged until T4. The hand path is frozen (ROADMAP v5 "Frozen until T4") and reads original
addresses that are zero today in places the seed does not cover; loading 434,252 bytes of initial values under it
would change what it reads, and is not needed since the hand path is deleted at the T4 flip.

**Retirement condition.** The four per-address tables, their generators and the CP stop thunks retire when both hold:
(1) IMG-5 has made image + trap the ORIG_TOP default and the knob boot's witnesses read equal; (2) the T4 flip has
deleted the hand path, so no path still calls `PortStateSeed_Apply`. After retirement the exe keeps only the witness
lists ({addr, len} plus the FNV, no bytes) as a regression check against a wrong image file.

## 6. Risks and open questions

1. **Writes before WinMain** into the image ranges would be overwritten. [UNCERTAIN], measured by the nonzero count
   the loader logs (section 2).
2. **Unfixed code pointers** stay original values. Called: trapped and reported (section 4). Compared without a call:
   [UNCERTAIN], lint rule proposed (section 3).
3. **A wrong slot in the list** would corrupt text silently. Mitigated by the per-slot "image value equals original
   target" check before rewrite and by taking slots only from Ghidra pointer-typed references.
4. **Values that matched bodies read differently from the seed**: none on the seed's addresses (byte-equal). Off the
   seed's addresses, the ORIG_TOP path will read nonzero where it read zero; that is the purpose, but behaviour can
   change. IMG-2 is a knob-path row and runs the top-down self-test and the menu walk.
5. **Thunk page overlap** with `.rdata` (`exe_main.cpp:9945` maps 0x005c0000+0x10000): the carve-out clips to
   0x005cbfff. On ORIG_TOP, whether B16 runs at all is [UNCERTAIN]; the carve-out works either way.
6. **IAT zeroing** turns a silent jump into exe bytes into a trap at 0. If a matched body does call through the original
   IAT, the report names it and that is a binding row.
7. **The image file goes missing or stale** on a player PC: the manifest SHA-256 covers it, and the loader refuses a
   file whose source SHA-256 is not the anchor. With the knob default OFF on the shipped (hand) path, the release
   cannot break before T4.
8. **Pad start moves** with exe size (0x00270ec0 today); the per-range bounds check refuses rather than writing past
   the pad.
9. **Account split**: IMG-4 needs Ghidra (account3). IMG-1..3 need no Ghidra.

## 7. Recommendation for the L-121 final-form decision row

Adopt **(b), whole image from the staged data file, on the `MASHED_ORIG_TOP` path only**, in the order IMG-1 to IMG-5:
default-OFF knob `MASHED_PORTSTATE_IMAGE` plus `MASHED_CODE_TRAP`, a positive-evidence code-pointer list (today's 188
slots, then the Ghidra export), unbound targets left in place for the trap instead of per-target stop thunks, and the
IAT zeroed. The default (hand) path keeps the per-address seed (L-99 a / L-101) until T4 deletes it; the seed tables
retire at T4. Reason: the four seed tables already hold 387 entries that are byte-equal to the image and grew from 100
to 272 PortStateSeed entries in five days, the image removes the list to maintain, and gating it to the knob path keeps
every default gate where it is.

Options for the row:
- (a) the above (recommended);
- (b) the same, and also load the image on the default path under a separate knob with a PREREG and a flip decision;
- (c) keep the per-address seed as the final form (L-99 a), close this row.

## Files

- `verify/portstate_20261113/RESULT_PORTSTATE-IMAGE.md` (this file)
- `verify/portstate_20261113/image_survey.py`, `image_survey.txt`
- `verify/portstate_20261113/codeptr_classify.py`, `codeptr_classify.txt`
- `verify/portstate_20261113/codeptr_split.py`, `codeptr_split.txt`

No raw data outside the tree. Input binary: `original/MASHED.exe.unpatched`, SHA-256
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E` (read only).
