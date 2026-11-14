# RESULT MATCH-EQUIV-420050-ORDER

Verdict: **GREEN** (RESULT written). Row kind: match-only / text. No build, no gates, no game run. ADMITTED.tsv not touched.

**Verdict line (merged equiv.py):** `0x00420050	EQUIV	[ORDER]`, mode `strict`, rules
`A6-paired-pop, A6-pinned-pop, jmp-threading, x87-barrier, x87-motion`, `bytes_bad` 9,
`local_callees` `_FUN_0041f8f0, _FUN_0041faf0, _FUN_0041fcc0`, steps 850 / terms 1310.

**Order check:** the ORDER class here is **one integer instruction moved across one x87 instruction**. The x87
instruction stream is the same on both sides (12 instructions, same text, same order). It is **not x87
instruction order.** It is the L-147 (a) case (answered 2026-11-13: equiv.py proves integer-vs-x87 independence,
admissions stay ADMITTED.tsv, never C4).

**Admission is still blocked, by a different fact:** local callee `0x0041faf0` (a static defined in the same
source) is `NOT-PROVEN x87-region` on the same tool, and `equiv_rules.md` "Local callees" requires every local
callee to be byte-matched or EQUIV on its own row before the caller is admitted.

## 1. Tool and source

| item | value |
|---|---|
| Worktree / branch | `.worktrees/MATCH-EQUIV-420050-ORDER`, `orch/MATCH-EQUIV-420050-ORDER` from `orch/integration` @ `ba39eeb6b` |
| Tool that produced the verdict | `re/tools/match/equiv.py` @ `fea5b5483` (EQUIV-5441F0-TIMEOUT), blob `51bb0c5cc3b3da20bdd221549e1350e78e600f61` (`verify/match_20261113/xcheck_measure_new.tsv` row 68, `changed4_stats.txt`) |
| Tool on merged `orch/integration` @ `ba39eeb6b` | same blob `51bb0c5cc3b3da20bdd221549e1350e78e600f61`; `fea5b5483` is an ancestor of `orch/integration`; `git diff --stat fea5b5483 orch/integration -- re/tools/match/` is empty |
| Re-run | done anyway on the merged tool in this worktree; same verdict, same steps / terms |
| Source | `re/match/s05/00420050.c`, last commit `e446c2dd4` (2026-11-09, MATCH-NEARMISS-2-A), blob `cd34db171677b3b4f338890c48a9f1e49f005620` |
| Flags | `/O2 /GS /Ob1`, toolchain `ddk` (XP DDK cl 13.00.9176 / c2 13.00.9178) |
| Byte score | `match.py --only 0x00420050 --show`: 99.3% (regs 99.3%, ops 99.3%), 151/151 instructions |
| Original image | `original/MASHED.exe.unpatched`, SHA-256 `bdcae093a30fbf226bdd852b9c36798a987aee33b3ae82bf7404b0336efd3c0e` (= version anchor) |

Commands (from the worktree root):

```
py -3.12 re/tools/match/equiv.py re/match/s05/00420050.c --stats --json
py -3.12 re/tools/match/equiv.py re/match/s05/00420050.c --only 0x00420050 --trace
py -3.12 re/tools/match/equiv.py re/match/s05/00420050.c --only 0x00420050 --x87-strict
py -3.12 re/tools/match/match.py re/match/s05/00420050.c --only 0x00420050 --show
py -3.12 re/tools/match/equiv_dyn.py re/match/s05/00420050.c --only 0x00420050 [--seeded]
py -3.12 verify/match_20261113/equiv420050/orig_disasm.py ../../original/MASHED.exe.unpatched
```

## 2. Order evidence (raw facts)

Region: ours `+0xa7` / orig `0x004200f7` (from `equiv_trace.txt`, "pair +0xa7 / 0x004200f7").

| original | | ours | |
|---|---|---|---|
| `0x00420100` | `fld dword ptr [esp + 0xc]` | `+0xb0` | `fld dword ptr [esp + 0xc]` |
| `0x00420104` | `fmul dword ptr [esi + 0x284]` | `+0xb4` | `mov edx, dword ptr [ecx + 4]` |
| `0x0042010a` | `mov edx, dword ptr [ecx + 4]` | `+0xb7` | `fmul dword ptr [esi + 0x284]` |
| `0x0042010d` | `mov dword ptr [esp + 0x10], edx` | `+0xbd` | `mov dword ptr [esp + 0x10], edx` |

- This is the only differing pair in the function: `match_show.txt` has exactly two `!` rows (lines 58 and 60),
  both this `fmul`. All other 149 instructions are `=` in place. `bytes_bad` 9 = 3 (`mov`, `8b5104`) + 6
  (`fmul`, `d88e84020000`).
- Moved instruction: `mov edx, dword ptr [ecx + 4]` (orig `0x0042010a`, ours `+0xb4`). It is an integer
  instruction. It reads ECX and the dword at `[ecx + 4]`, writes EDX. No flags.
- Crossed instruction: `fmul dword ptr [esi + 0x284]` (orig `0x00420104`, ours `+0xb7`), one x87 instruction. It
  reads ESI and the dword at `[esi + 0x284]`, writes ST(0). No integer register written, no EFLAGS.
- Register overlap: none (ECX/EDX against ESI). Memory: read against read. The tool's x87-motion check
  (`equiv_rules.md` section 5, L-147 (a)) accepted it; rule tag `x87-motion` is in the verdict.
- x87 stream, whole function (`x87_orig.txt`, `x87_ours.txt`, extracted from the `match.py --show` columns):
  12 instructions each, `diff` empty, both files SHA-256 `884581bb...0fb0`. Sequence: `fld [esp+0xc]`,
  `fmul [esi+0x284]`, `fstp [esp+0x1c]`, `fld [esp+0x10]`, `fmul [esi+0x284]`, `fstp [esp+0x28]`,
  `fld [esp+0x1c]`, `fmul [esi+0x284]`, `fstp [esp+0x2c]`, `fld [esp+0x20]`, `fmul [esi+0x284]`, `fstp [esp+0x30]`
  (original addresses `0x00420100` .. `0x0042014f`).
- `--x87-strict` (whole x87 region identical text): `NOT-PROVEN x87-region: region ours +0xa7 / orig 0x004200f7
  differs at instruction 4: mov edx,m4[ecx+0x4] | fmul m4[esi+0x284]`. Expected: strict refuses any motion, and
  the instruction it names is the same `mov` / `fmul` pair.
- Dynamic cross-check `equiv_dyn.py`: default `SAME same=32 inc=0 diff=0`, `--seeded` `SAME same=32 inc=0 diff=0`
  (`dyn_default.tsv`, `dyn_seeded.tsv`). This exercises the caller with its local callees as compiled.

Answer to the three questions: (1) instructions that differ in order: `mov edx,[ecx+4]` (orig `0x0042010a`) and
`fmul [esi+0x284]` (orig `0x00420104`) swap places, nothing else. (2) It is **not** x87 instruction order only:
the x87 order is unchanged; an integer load moved across one x87 instruction. (3) No x87 instruction changed
position relative to another x87 instruction anywhere in the function.

## 3. Local callees (the admission blocker)

Same `--json` run, same tool:

| callee | verdict | classes / kind | note |
|---|---|---|---|
| `0x0041f8f0` | EQUIV (strict) | none (byte level 100% per the source header) | rules `A4-indirect, A6-paired-pop, A7-stack-image` |
| `0x0041faf0` | **NOT-PROVEN `x87-region`** | | `region ours +0x55 / orig 0x0041fb45 x87 instruction 7 differs: fld m4[esp+0xb4] / fld m4[esp+0xb0]`, `bytes_bad` 59; source header: 90.8% |
| `0x0041fcc0` | EQUIV (frame) | `ENC, ORDER, VALUE-FLOW` | rules `A1-exposed, A5-arity, call-args-all, x87-barrier, x87-motion`; its own ORDER was not checked by this row |

`equiv_rules.md` lines 68-70: "When the source itself defines the callee, the verdict lists it under
`local_callees`. An admission must cover those bodies too: each one byte-matched, or EQUIV on its own row."
`0x0041faf0` fails that: its x87 instruction 7 has a different operand (`[esp+0xb4]` ours against `[esp+0xb0]`
original), which is an x87 operand difference, not motion. The source header of `00420050.c` says the three
callees must stay statics in this file for the caller's ESI/EDI handling to match (pilot finding 1).

## 4. Decision text for the lead (do not file unless the lead decides a question remains)

My reading: **no user decision is needed for the order part.** L-147 (a) already covers integer motion across x87
that equiv.py proves, and that is what this verdict is. The admission is blocked by an existing rule (local
callees), not by a new question. If the lead still wants it on record, text ready for
`roadmap.ps1 decision MATCH-EQUIV-420050-ORDER`:

> Question: 0x00420050 (re/match/s05/00420050.c @ e446c2dd4, /O2 /GS /Ob1, 99.3%) is EQUIV [ORDER] on the merged
> equiv.py (blob 51bb0c5c, fea5b5483, same on orch/integration ba39eeb6b). How to proceed with its admission?
> Evidence: the only difference is integer `mov edx,[ecx+4]` (orig 0x0042010a) moved before x87
> `fmul [esi+0x284]` (orig 0x00420104); no shared registers, read-read memory; the 12-instruction x87 stream is
> identical in text and order, so this is L-147 (a) integer motion, not x87 order. equiv_dyn SAME 32/0/0 default
> and seeded. Blocker: local callee 0x0041faf0 (static in the same source, required there for the caller's
> register allocation) is NOT-PROVEN x87-region (x87 instruction 7 operand [esp+0xb4] vs [esp+0xb0], 90.8%), and
> equiv_rules "Local callees" needs every local callee byte-matched or EQUIV before admission. 0x0041f8f0 is
> EQUIV strict, 0x0041fcc0 EQUIV [ENC,ORDER,VALUE-FLOW] with x87-motion.
> Options: (a) park 0x00420050's admission until 0x0041faf0 is byte-matched or EQUIV, and open a near-miss row
> for 0x0041faf0 (frame slot offset of one fld); (b) admit 0x00420050 with the port binding its 0x0041faf0 call to
> a body that is itself admitted later (not allowed by the current local-callee rule, would need a rule change);
> (c) leave all four parked.
> Recommendation: (a). No x87-order admission is involved in any option.

## 5. Files (this commit)

All under `verify/match_20261113/`, total 93 KB:

| file | SHA-256 |
|---|---|
| `equiv420050/equiv_default.json` | `11bd03f2868e1621a084774fff2aa1eb8f3693e40729211bebf5942f14498a85` |
| `equiv420050/equiv_trace.txt` | `dedfeb6fff0db6be7148727e18535a435f77759cfae2efc979cec3684849ce3c` |
| `equiv420050/equiv_x87strict.txt` | `b344173a1444c9ea49b4dfb45bf49c3adb3f6104a62d635244155b0a532ad797` |
| `equiv420050/match_show.txt` | `c1fa9f13e287e96ae82f0a9ce72270d3cdddbf0b83e1c8dd69cf49d6de0f3391` |
| `equiv420050/orig_00420050.txt` | `c854be541ca53c4ecbc35e1ecc3f6f28492424e5b25fd7d7f980cfa0e1ba8e9c` |
| `equiv420050/orig_disasm.py` | `5f2666e361da09d86149ef16a128395c1957de24097d6216d0efdd3d21a7dea2` |
| `equiv420050/x87_orig.txt`, `x87_ours.txt` | `884581bb84d0353808c60b7cf3d0667bcde6781fa00857ffb777561694243fb0` (both) |
| `equiv420050/dyn_default.tsv`, `dyn_seeded.tsv` | `ae3e13236d1d7b8a85ce8f72345d1f0e478bf3d53141d1e053030b6f782473c9` (both) |

No raw data outside the repo. No processes left running.
