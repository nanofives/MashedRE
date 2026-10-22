# RESULT dcstream (2026-10-22): the render-stream pair, one body each

Branch `work/dcstream` (worktree `.worktrees/dcstream`), base `f0cf8fa6b`. Not merged. Shared `original/mashed_re_dev.asi` not
redeployed. Evidence off git: `D:/MashedWT/dcstream_data` (`leaf_ab/`, `arms/`, `t13/`, build logs).

## 1. Verdicts

| entry | finding | fix | evidence | verdict |
|---|---|---|---|---|
| `0x004cbd30` RwStreamRead | CROSS-TARGET. exe `D3d9Render/RwWorldStream.cpp` ported type 3 only (others returned 0 without err 0xe). .asi `Render/TextureLoader_q6.cpp` dispatched four types but emitted no error at all (file short read: err 5 / 0x8000001a; memory clamp: err 5; default: err 0xe) | ONE body `Render/RwStreamCore.cpp:RwStreamCoreRead` in both rsp files, read from the disassembly (section 2). Per-target host thunks only under `#ifdef MASHED_STANDALONE`: error pair, `FUN_00550950` read, `FUN_00550af0` EOF check | in-process A/B 0 / 0 (section 3), sweep 0 / 0 | **folded, knob-free** |
| `0x004cc050` RwStreamSkip | CROSS-TARGET. .asi copy: callback result INVERTED (`ok == 0` -> stream; the original returns the stream on NON-zero, neg/sbb/and at `0x004cc118`), no `n == 0` early-out (`0x004cc05a`, before the type is read), a memory overrun left pos unchanged (the original writes pos = size, `0x004cc0db`), no errors. exe copy matched the memory path, no file / callback cases | same TU, `RwStreamCoreSkip`; host thunks add `FUN_005509b0` seek | same | **folded, knob-free** |

Allowlist `re/tools/dual_copy_allowlist.txt`: 11 -> 9 findings (both lines removed), rva-lint `9 known, NEW=0`, asm-lint OK.
`RwWorldStream.cpp` keeps `Rwl_StreamRead` / `StreamSkip` as call-throughs (no RVA anchor); the four-case copy and its two
`RH_ScopedInstall` lines left `TextureLoader_q6.cpp` (now installed from `RwStreamCore.cpp`, same export names
`RwStreamRead` / `RwStreamSkip`, registry `rw_stream_read` / `rw_stream_skip` unchanged).

**Exe reachability:** the exe's only stream producer is `RwWorldLoad_OpenMemory` (type 3), and its only caller
`RwWorldLoad_FromBytes` has NO caller in `mashedmod/src` (grep 2026-10-22). The exe path is dead code on the default build, so
the exe file thunks (`return 0` / seek fails) are never reached. Gates were still run because the TU set changed (code layout,
memory sim-trajectory-is-code-layout-sensitive).

## 2. The original (`original/MASHED.exe.unpatched`, `re/tools/disasm_fn.py`)

Jump tables (read from the image): `0x004cbe70` = {`0x004cbd4e`, `0x004cbd4e`, `0x004cbdc8`, `0x004cbe28`}, `0x004cc14c` =
{`0x004cc07c`, `0x004cc07c`, `0x004cc0cd`, `0x004cc10d`}; index = type - 1, > 3 unsigned = default. Every error is
`{1, FUN_004d7ff0(code)}` passed to `FUN_004d8480` (`0x004d7ff0` returns its argument; `0x004d8480` stores into
`*DAT_007d3ff8 + DAT_007d6c5c` only while that slot holds {0, 0x80000000}, `0x004d848d` / `0x004d849c`).

- Read. File: `FUN_00550950(dst, 1, size, [s+0xc])` `0x004cbd62`; got != size -> `FUN_00550af0(h)` non-zero -> err 5, else
  err 0x8000001a; returns got. Memory: avail = [s+0x10] - [s+0xc]; size > avail -> size = avail, err 5 (`0x004cbdd8`); rep movsd
  / rep movsb from [s+0x14] + pos; pos += size. Callback: `[s+0x10]([s+0x1c], dst, size)`, its value returned. Default: err 0xe, 0.
- Skip. n == 0 -> stream (`0x004cc05a`). File: `FUN_005509b0(h, n, 1)` zero -> stream, else (EOF check non-zero -> err 5) 0.
  Memory: np = n + pos (32-bit); np > size -> pos = size, err 5, 0; else pos = np, stream. Callback: `[s+0x18]([s+0x1c], n)`
  non-zero -> stream. Default: err 0xe, 0.

## 3. In-process A/B on the original (dev .asi `d3be8272`, hooks live, private root `D:/MashedWT/dcstream_root`)

`verify/dcstream_20261022/ab_leaf.sh` (`MASHED_LEAF_SELFTEST=1`, `scenario_launch.py --hooks 0x004cbd30,0x004cc050 --cars 4`).
Pure class (memory, unknown type, zero skip): original twice through a trampoline (`noo`), then the shared body (`ndiff`),
9 stream words + destination bytes + return. File class: original once, rewind with `FUN_005509b0(h, -count, 1)`, shared body.
Callback class: record / replay stubs in the stream's fn / user slots. Summary lines `leaf_ab_summary.txt`.

| run | reads: pure / file (noo / ndiff / unrun) | skips: pure / file | callback |
|---|---|---|---|
| `t0` 40 s (first harness) | 2883 0/0, 42685 0/**1**/0 | 3 0/0, 13 0/0 | 0 |
| `t0b` 60 s | 2883 0/0, 42685 0/0/3 | 3 0/0, 13 0/0 | 0 |
| `wz` 60 s `--track 11` | 2883 0/0, 47293 0/0/3 | 3 0/0, 13 0/0 | 0 |
| `sweep` 30 s, `MASHED_LEAF_SELFTEST_SWEEP=1` | as t0b; **sweep 116 cases, noo 0, ndiff 0, 57 with an error emitted** | | |

`t0`'s one ndiff was the harness: call 1177, a 0x10000 file read the original returned short (0x835a); after the rewind the shared
body's identical `FUN_00550950` call returned 0x10000. Both bodies return that count unchanged, so the file layer answered
differently, not the body. Since `t0b` a short file read and a failed skip are not re-run (`unrun`, the original's result kept).
Those 3 short reads (0x835a of 0x10000, two 0 of 12) are therefore covered only by the disassembly reading, not by an A/B.

Sweep (synthetic, path1-grade): memory streams size 64 at pos 0 / 5 / 60 / 64 / 70 (70 > size: unclamped), reads n 0..0xfffffff0,
skips including the 32-bit wrap (pos 5, n 0xfffffffb -> pos 0, success); types 0 / 5 / 0x80000000 / 0xffffffff (read, skip,
zero skip); callback streams on deterministic stubs (read returns 0 / 3 / 8 / 0xffffffff, skip 0 / 1 / 0x80000000). Each case resets
the error slot to {0, 0x80000000} and compares it after. By construction the old .asi skip fails the overrun cases (pos and slot)
and every non-zero callback skip (return); this is an analytic statement, no mutant was run.

**Coverage gaps:** no callback stream (type 4) occurred in natural play; skips are 16 calls per run, all early. Both are covered
only by the sweep.

## 4. Gates on the exe (recipe F), final exe `f2703ca2`, knob-free, vs the rpct references

| gate | reference (rpct) | dcstream |
|---|---|---|
| G-CTRL, 2 runs | `bacfc747 / 8dd1c004 / 6f9932b6` | `H_DEF-1`, `H_DEF-2` both equal (`arm_hashes.txt`) |
| B K0 / K1 / K26 | `f9deb9b9 / d3601fcc / 9da9a26a` | equal |
| (b) | 9/9 | 9/9 (`vsum_b_DEF.txt`) |
| (e) | 3/3, launch 1425.7 / 2052.5 / 2055.0 | 3/3, same (`e_check_DEF.txt`) |
| D2, 3 runs alone | PASS 3/3 | PASS 3/3, slip 0.1900 / 0.2550 x3, drvmed 1964.25 / 1965.35 / 1965.35, H1 398/400 (`score_d2_DEF.txt`) |
| 13 tracks | 39 frames, 878 rows, 0 RED | score table identical to rpct's, gates PASS (`tracks_def_score.txt`) |

## 5. Trackers (NOT applied; proposal for Mariano)

`0x004cbd30`, `0x004cc050`: C2 -> **C3**, `file` = `exe_file` = `mashedmod/src/mashed_re/Render/RwStreamCore.cpp`, frida_diff
= `D:/MashedWT/dcstream_data/leaf_ab`. The 2026-09-29 demotion reason (two divergent copies) is gone. C4 is not proposed:
the exe never executes the body (section 1; memory strict-c4-needs-the-body-to-execute), and the callback class and the short-read
branches have only synthetic evidence. The analysis plates `re/analysis/texture_loader_d3/0x004cbd30.md` / `0x004cc050.md`
describe the old four-case copy without the error emits; they are superseded by section 2.

## 6. Left for decisions (unchanged from the kickoff)

- Math pairs: `0x004c39b0`, `0x004c4680`, `0x00532b80`, `0x004c4d20` (knob default OFF + PREREG route).
- Deliberate splits `0x004282a0`, `0x004177b0`; `0x00410510` residue is input plumbing.
- `0x00408ad0` stays allowlisted for the `MASHED_NO_RACEPCT_SLOT` stand-in.
After this lane no allowlisted pair is left that folds without a decision.
