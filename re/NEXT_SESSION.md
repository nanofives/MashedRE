# Next session — kickoff prompt

## ⇒ CURRENT STATE (2026-09-12, uncertainty-drain + 8 promotion rounds) — READ THIS FIRST

Branch `race/first-frame-parity`, tree clean, **27 commits** this session. Zero worktrees,
zero Ghidra pool locks, zero stray processes. Anchor verified.

| measure | value | how to re-derive |
|---|---:|---|
| C4 / C3 / C2 / C1 | 184 / **1029** / 3865 / 821 | `Import-Csv hooks.csv \| Group-Object confidence` |
| gating uncertainties | **0** | `Blocks` cell exactly `C2->C3` or `C3`, Active section only |
| open uncertainty rows | 3,031 | rows `^\| *U-[0-9]+` between the Active and Resolved headers |
| promotion rounds run | 256 | ledger `rounds_run` |
| non-canonical column count | 131 | pre-existing baseline, unchanged all session |

## ⇒ THE TWO THINGS MOST LIKELY TO BE MISREAD

**1. "0 gating" does NOT mean "everything is answered."** 46 rows were gating this morning.
17 were **resolved on evidence**; **29 were DE-GATED** and are still open, carrying their
evidence and next command. The de-gate rests on `re/CONFIDENCE.md`'s own C2→C3 wording — a
field may be named *or* explicitly marked `[UNCERTAIN]` with the marker recorded — so a
properly recorded row is the rubric's sanctioned alternative to a name. Owner-approved.
The test applied to each: *would a byte-for-byte verbatim transcription be wrong without
this answer?* For all 29, no.

**2. If you hit a function you cannot transcribe, that is a NEW finding.** Open a new row;
do not assume an old one covers it.

## What landed

### Uncertainty drain: 46 → 0 gating
17 resolved with citations. The ones worth knowing:
- **U-4780** — takes **five** stack args, not three; `[E+0xc]` proven never addressed.
- **U-4700** — the three constants are DirectShow GUIDs (`MEDIATYPE_Video`,
  `MEDIASUBTYPE_RGB24`, `FORMAT_VideoInfo`), named from the **local Windows SDK**
  `uuids.h`, matched at `+0x00/+0x10/+0x2c` = `AM_MEDIA_TYPE` per `strmif.h`.
- **U-4713** — a full 4×4 carrying translation, not a normal matrix: exactly four `fchs`
  negate the whole first row *including* `-pos.x`.
- **U-4701** — `0x00494b65` **is not a function**; it is the `jne` target inside `0x00494b50`.
- **U-5654** — the `-3` is one uniform 3-pixel inset on both axes (float `3.0` at `0x005cc31c`).

**Three corrections to the existing record**, which matter more than the wins:
- Ghidra's *"could not recover jumptable at `0x0041dec0`"* is a **misclassification** — it is
  `jmp dword ptr [eax+0x48]`, a vtable tail call. That was the only genuine transcription
  gate among the 46 and it existed because nobody disassembled the address.
- **U-4313's fourth write site does not exist.** `0x0043f8d5 mov [ebx+8],edx` never lands on
  `0x007f1a1c`; EBX is never loaded with that constant in the range.
- **U-4583's `DAT_006668f8`** is a digit transposition of `DAT_007668f8`.

### 18 promotions across rounds 249–256
`RwEngineRegisterPlugin`, `DriverSystemDispatch`, `RwErrorModuleDtor`, four r252 leaves,
three r253, `Mat4x3InvertOrthonormal`, two r255 walkers, five r256 forwarders.
**One deliberate non-promotion:** `0x004f10e0` is GREEN 8/8 with path2 PASS and **stays C2** —
both callers are anonymous (`FUN_004e4300` C1; `FUN_004e41e0` has no hooks.csv row), so
promoting would be an island promotion. Evidence banked; unblock by raising either caller.

### Harness work (all SWEEP-CRITICAL)
- **`observe_bufs`** on `stub_dispatch_observe` (path1) — three arms of `0x004c2c90` all
  return 1 and differ only through an out-pointer, so return-only observation could not tell
  a correct port from a swapped one.
- **path2 buffer-arg support** — `bgra_encode` / `ptr_seed_observe` / `stub_dispatch_observe`
  added to the verify template, the missing CONFIG forwarding added to `run_verify_hook.py`,
  and an `orch-iter21` test-shape branch narrowed because it was **shadowing** the new
  handlers. Unblocked 15 registry entries; all 17 `arg_layout` entries re-run, no regressions.
- **`re/tools/caller_screen.py`** — applies the C2→C3 caller rule *before* any code is
  written. On the r256 slice: 227 promotable, 69 caller-blocked.

## ⇒ STANDING RULE LEARNED THE HARD WAY: a new arg_type has FOUR homes
`diff_template.js`, `verify_hook_install_template.js`, **and** the config builders in
`run_diff.py` **and** `run_verify_hook.py`. Both builders are **whitelists that drop unknown
keys silently**. Miss the template and path1 goes GREEN while path2 dies `bad argument
count`; miss the builder and the handler runs against an EMPTY config. This bit three
separate ways in one round. Also check `callFn`'s branch ORDER — a branch keyed off *test
shape* rather than arg_type will shadow later handlers (same lesson as U-9067).

## ⇒ AND: a name may not claim more than its comment does
A naming audit of this session's own work **withdrew or corrected 11 of 18 names**.
`RwFrameHeadSet` asserted a frame type and a head field, neither ever read.
`PizOpenDefaultMode` sat on a **particle** row. `RwRGBAToIntensityScaled` claimed a channel
order its own comment explicitly declined to claim. Grounded names were kept (a C4 or
named-library callee gives you one); otherwise `Fwd<callee>_<literal>` says enough.
**An export rename is not cosmetic — re-run both paths.** The rename script itself replaced
by dict order and substituted `RwPluginListDispatch` *inside* `RwPluginListDispatch3`; only
the re-run caught it. Sort replacement keys by descending length.

## PICK ONE

> **Options A and B were DONE 2026-09-12 (same day, later session).** U-9135: attestation NARROWED, not re-banded
> (13 render rows -> psgp on dispatcher/table evidence, 92 keep `render`, 66 hlsl rows -> psgp; new U-9136 for
> the hlsl band). U-9134: Lua ends at `0x004c0735`; 16 rows -> render, 10 -> unknown. 0 C-levels moved.
> Method that settled both: `reference_to`/`reference_from` per function, never the range. U-9136 (hlsl band) also DONE the same way: 5 -> psgp, 68 -> d3dx9-shader-compiler, range label retired. Remaining pick: **C**, or D2.

### A. **U-9135 — decide the PSGP band disposition.** (recommended, and it is a decision, not research)
103 rows are tagged `render` (first-party) while carrying a note asserting they are
statically-linked Microsoft PSGP. The attested range `0x004ec000..0x004fc9e0` holds 198 rows
that the per-row tag splits into **four contiguous, non-interleaved blocks** — render 25,
psgp 79, render 80, psgp 14 — and only 93 are tagged `d3dx9-psgp`, while 12 psgp-tagged rows
sit outside the range. The clean block structure is the evidence. Same shape as U-9134.
Either **narrow the attestation** to the two genuine psgp blocks, or **re-band** the 103 rows
under library-skip. Do not edit either field before deciding: both are range-assigned and the
wrong choice mislabels 100+ rows. Seven rows in the range are already C3 (five promoted this
session) — **all seven are in the render blocks**, so under the per-row tag they are fine.

### B. **U-9134 — audit the `0x004b4a80..0x004c4000` lua band.** Same class, one row proven
mis-banded (`004c0c20` dereferences RwGlobals). May be hiding reachable first-party work.

### C. **More promotion rounds.** The pool is deep and the loop is cheap now: no path2
friction since r252, and `caller_screen.py` removes ~30% of dead ends before authoring.

## Carried over, still needing YOUR call
- **`area/frontend` is the one unmerged branch**, deliberately (WIP `PanelSortInit` hook).
- **D-11069** — 4 duplicate-RVA rows need a `hooks.csv` schema change.
- **U-9087** — 4 C4 rows may need demotion; their install proof is a byte the original has.
- **`main` is level with HEAD**; nothing pushed to `origin` (182 commits ahead).

## Harness wishlist (measured, per the ledger's own rule)
- **`ptr_to` cannot express buf+OFFSET**, which blocks intrusive CIRCULAR lists whose
  sentinel is an interior address — `0x004c59c0` (`param_1+8`), `0x004d8280`/`0x004d8300`
  (`param_1+0x90`). NULL-terminated lists are unaffected. **Count the rows before spending a
  round on it.**
- Absolute-global seeding combined with `stub_at` — blocks `0x004c9f60` (spec in ledger L2c,
  including the `__stdcall` vtable-slot hazard at `0x004c9f85`).

## Standing rules that bit earlier sessions
- Shadow lane: single-boot verdicts are unreliable; require two boots.
- Races need `--cars 4 --hold 60`; a 1-car race never fires the contact solver.
- Never `git worktree remove --force` — use `py -3.12 scripts/diag.py wt-remove`.
- Kill only PIDs you spawned; never blanket-kill MASHED by name.
- A Python script that reads text and writes text **strips CRLF**. Read/write bytes.
  `git diff --stat` is the tell (3,240 lines "changed" = you did it).
- Do not put a literal `|` in an UNCERTAINTIES cell — it splits the row. Spell it `OR`.

## Ready-to-paste kickoff

> Resume the Mashed RE lane on `race/first-frame-parity`. Read `re/NEXT_SESSION.md` first —
> note that **gating uncertainties are 0 but 29 of the 46 were de-gated, not resolved**, and
> that **U-9135 is an open owner decision affecting 103 rows**. Then pick A (decide U-9135),
> B (audit the lua band, U-9134), or C (more promotion rounds — run
> `py -3.12 re/tools/decomp_pc.py --file rvas.txt --callers --json -o batch.json` then
> `py -3.12 re/tools/caller_screen.py batch.json` and author only from the PROMOTABLE list).
