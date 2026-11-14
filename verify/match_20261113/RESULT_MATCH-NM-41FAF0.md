# RESULT MATCH-NM-41FAF0

Verdict: **GREEN**. `0x0041faf0` is **100.0% BYTE-IDENTICAL** under `match.py --bytes` (XP DDK cl 13.00.9176 /
c2 13.00.9178, `/O2 /GS /Ob1`, C), `relocheck.py` OK (checked 18, unchecked 1 = `__ftol`), with a plain C body (no
naked function, no inline `__asm`). Row kind: match-only. No build, no gates, no game run. ADMITTED.tsv not touched.

Consequence for `0x00420050`: on the merged `equiv.py` all three local callees now pass (`0x0041f8f0` and
`0x0041faf0` BYTE-IDENTICAL, `0x0041fcc0` EQUIV), so the local-callee blocker named in
`RESULT_MATCH-EQUIV-420050-ORDER.md` section 3 is cleared. The caller is unchanged at 99.3%, EQUIV [ORDER].

## 1. Tool, source, image

| item | value |
|---|---|
| Worktree / branch | `.worktrees/MATCH-NM-41FAF0`, `orch/MATCH-NM-41FAF0` from `orch/integration` @ `ba39eeb6b` |
| `equiv.py` | blob `51bb0c5cc3b3da20bdd221549e1350e78e600f61` (last change `fea5b5483`), `git diff --stat orch/integration -- re/tools/match/` empty, so this is the merged tool |
| Sources | `re/match/s05/0041faf0.c` blob `dd2b1f0e0b238d5a5293a8ad0d16d6b754349346`; `re/match/s05/00420050.c` blob `024fb03e1d18f4545631bdab12df37085a632ca7` (same three edits in its static copy of `FUN_0041faf0`) |
| Flags | `/O2 /GS /Ob1`, toolchain `ddk` |
| Original image | `original/MASHED.exe.unpatched`, SHA-256 `bdcae093...cd3c0e` (= version anchor) |

## 2. Score and the levers

Start: 90.8% (regs 90.8%, ops 93.3%), 119/119 instructions (`nm41faf0/match_show_base.txt`). Three differing
regions. Each lever was measured alone with `nm41faf0/try_variants.py` (literal text replacement on the source, then
`match.py --only 0x0041faf0`).

| step | edit | score | region it closed |
|---|---|---|---|
| 1 | `dot = (float)((double)fr->m.at.z * b.at.z + (double)fr->m.at.x * b.at.x + (double)fr->m.at.y * b.at.y);` | 90.8 -> 95.8 | dot product, orig `0x0041fc4e` .. `0x0041fc68`: the original loads the camera side first in y, x, z order (`fld [eax+0x34]`, `fmul [esp+0x40]`, `fld [eax+0x30]`, `fmul [esp+0x3c]`, `faddp`, `fld [eax+0x38]`, `fmul [esp+0x44]`, `faddp`) |
| 2 | `d.z = (float)((double)a.pos.z - b.pos.z);` | 95.8 -> 98.3 | b.pos += up * (s.y * 0.5f) block: `fmul [0x5cc32c]` now issued before `push 1`, as in the original |
| 3 | `d.x = (a.pos.x - b.pos.x);` (parenthesised) | 98.3 -> 100.0 | d.y: `fld [esp+0xb0]` now issued before `push eax`, as in the original (this was the `x87 instruction 7 ... [esp+0xb4] / [esp+0xb0]` of the NOT-PROVEN verdict: the same slot, read one push later) |

Measured facts behind lever 1 (`v_dot.py`, `v_dbl.py`): with float products, all 8 parenthesisations tested,
stepwise `+=` forms and the operand-swapped form all give 90.8%, the same code. With double products the 6 term
orders give 92.4 to 95.8, and only z, x, y gives the original's sequence. Casting the whole product,
`(double)(a * b)`, gives 90.8 (the products stay float). Lever 3: the parenthesis is a no-op in C semantics. The
DDK front end keeps it as a tree node, and that node changes the c2 schedule. Other forms tried for the d.y slot
(`v_h3.py`, `v_h4.py`: double or long double on d.x / d.y, negated forms, statement order, pointer temps, call
argument forms) stayed at or below 98.3.

Both permuter runs (`match_permute.py`, 691 compiles from 95.8%, then a run from 98.3% stopped once the hand
lever reached 100) found nothing (`nm41faf0/permute.log`, `permute2.log`).

Final (`nm41faf0/match_final.txt`, `relocheck.txt`):
```
0x0041faf0 _FUN_0041faf0: 100.0% (regs 100.0%, ops 100.0%)  119/119 ins  flags=/O2 /GS /Ob1 tc=ddk
   bytes: 458 B whole symbol, 0 differing unmasked bytes -> BYTE-IDENTICAL (relocations masked)
0x0041faf0	OK	checked=18	unchecked=1	0041faf0.c	  unchecked: __ftol
```
Same result for the copy in `00420050.c` (`match_00420050_file.txt`, `relocheck_00420050_file.txt`). Before the
edits that copy was relocheck BAD (`+0xdb const __real@3f000000`, from the shifted layout). Now it is OK.
`equiv_dyn.py` on `0x0041faf0`: SAME same=32 inc=0 diff=0.

## 3. Promotion file

`re/promotions/MATCH-NM-41FAF0.tsv`: `0x0041faf0  match  re/match/s05/0041faf0.c  verify/match_20261113/RESULT_MATCH-NM-41FAF0.md`.

Dry run `py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/MATCH-NM-41FAF0.tsv` (`c4_reclass_dry.txt`):
`[1/1] 0x0041faf0 match SKIP already C4 (no write)`. The `hooks.csv` row is already C4 by route `frida-replay`
(2026-11-11, `verify/provinsitu_20261111/RESULT_PROV-PROMOTE-44.md`, notes tag `c4=frida-replay`). So the train's
`--apply` writes nothing and adds no `c4=match` tag. The match gate itself, called directly
(`c4_reclass.gate_match`, `gate_match_direct.txt`), returns `True, 'byte-identical 458 B, relocheck OK (18/1)'`.
The lead decides whether the separate match count needs a second tag on a row that is already C4. The tool has no path for that today.

## 4. 0x00420050 admission record (for ADMITTED.tsv, not written)

| column | value |
|---|---|
| rva / name | `0x00420050` / `FUN_00420050` |
| source / flags | `re/match/s05/00420050.c` blob `024fb03e...2ca7`, `/O2 /GS /Ob1`, ddk; source commit = this branch's commit |
| tool_commit | `equiv.py` `fea5b5483` (blob `51bb0c5c`) |
| verdict | `0x00420050	EQUIV	[ORDER]`, mode strict, rules `A6-paired-pop, A6-pinned-pop, jmp-threading, x87-barrier, x87-motion`, bytes_bad 9 (`equiv_00420050.json`, `equiv_00420050_default.txt`) |
| local_callees | `0x0041f8f0` EQUIV [BYTE-IDENTICAL]; `0x0041faf0` EQUIV [BYTE-IDENTICAL]; `0x0041fcc0` EQUIV [ENC,ORDER,VALUE-FLOW] |
| xcheck | `equiv_dyn.py --only 0x00420050`: default SAME 32/0/0, `--seeded` SAME 32/0/0 (`dyn_default.txt`, `dyn_seeded.txt`) |
| ORDER class | integer `mov edx,[ecx+4]` (orig `0x0042010a`) across x87 `fmul [esi+0x284]` (orig `0x00420104`), L-147 (a), from `RESULT_MATCH-EQUIV-420050-ORDER.md` section 2. Unchanged by this row |
| caller_scan, port_binding | not done by this row |

## 5. 0x0041fcc0 ORDER re-check

Answer: **integer motion across x87 (L-147 a), not x87 order.**

- The only differing pair in `match.py --show` (`match_show_41fcc0.txt` lines 69 and 73) is `pop esi`: ours `+0xf1`,
  right after `fld [esi+0x18]`. Original `0x0041fdbf`, after `fld [0x5d757c]` at `0x0041fdb9`
  (`orig_0041fcc0.txt`).
- It crosses three x87 instructions: `fmul dword ptr [0x5d757c]` (`0x0041fdb1`), `faddp st(1)` (`0x0041fdb7`),
  `fld dword ptr [0x5d757c]` (`0x0041fdb9`). `pop esi` writes ESI and ESP and reads `[esp]`. None of the three reads
  ESI or ESP: two absolute addresses and the x87 stack. The last ESI read, `fld [esi+0x18]` (`0x0041fdae`), comes
  before the pop on both sides.
- The x87 streams are identical: 13 instructions each, same text and order once relocated absolute operands are
  normalised (ours `[0]`, original `[0x5d757c]` / `[0x5cd04c]`). Files: `x87_ours_41fcc0.txt`, `x87_orig_41fcc0.txt`.
- [UNCERTAIN] Where the ENC and VALUE-FLOW classes come from inside `0x0041fcc0`: `--trace` (`equiv_trace_41fcc0.txt`)
  lists the region pairs and does not tag a class per instruction, and `--show` has no second differing pair.
  Missing evidence: a per-class location output from `equiv.py`. `0x0041fcc0` itself stays relocheck BAD
  (`+0xf4` / `+0xfc` `__real@00000000`), which comes from the one-byte `pop esi` shift. This is the same on
  `orch/integration`, not caused by this row.

## 6. Files

All under `verify/match_20261113/nm41faf0/` (small text, committed). SHA-256 (first 16 hex):
`match_show_base.txt 1d90604167f96eaf`, `match_final.txt 8ee0773b26283de4`, `relocheck.txt f781fa2a9503affc`,
`match_00420050_file.txt 5c6376bb35d8b7fc`, `relocheck_00420050_file.txt f1740f9e145a9ddf`,
`equiv_00420050.json 4da67cd163366439`, `equiv_00420050_default.txt 56fd4c50c0a9bb18`,
`equiv_trace_41fcc0.txt 30563950d6d151aa`, `match_show_41fcc0.txt 2e0b26571b865280`, `orig_0041fcc0.txt 06e766465d3ccff6`,
`dyn_default.txt` = `dyn_seeded.txt 0d15cfead188b2f0`, `c4_reclass_dry.txt 654958964cc2d931`,
`gate_match_direct.txt 969d23f48e497ff1`, `permute.log 3c0f25bff236cda5`, `permute2.log 2e7e2df653369df0`,
scripts `try_variants.py`, `orig_disasm.py`, `v_*.py`. No raw data outside the repo. No processes left running.
