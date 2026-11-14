# RESULT MATCH-ADMIT-420050

Verdict: **GREEN**. `0x00420050` is admitted to `re/match/ADMITTED.tsv` (one row appended). Row kind: match-only / text.
No build, no gates, no game run. Admission is not C4 (L-128). `hooks.csv` and `re/promotions/` are not touched.

## 1. Tree, tool, source

| item | value |
|---|---|
| Worktree / branch | `.worktrees/MATCH-ADMIT-420050`, `orch/MATCH-ADMIT-420050` from `orch/integration` @ `ba39eeb6b` |
| Merged first | `orch/MATCH-NM-41FAF0` @ `397232926` (`git -c merge.autoStash=false merge`, fast-forward) |
| `equiv.py` | last change `fea5b5483`; `git diff --stat HEAD orch/integration -- re/tools/match/` empty, so this is the merged tool |
| Source | `re/match/s05/00420050.c` blob `024fb03e1d18f4545631bdab12df37085a632ca7`, last commit `397232926` |
| Flags | `/O2 /GS /Ob1`, toolchain ddk (XP DDK cl 13.00.9176 / c2 13.00.9178) |
| Original | `original/MASHED.exe.unpatched` (= version anchor) |

## 2. Verdict (re-run on this tree)

`py -3.12 re/tools/match/equiv.py re/match/s05/00420050.c` (`admit420050/equiv_default.txt`, `--stats --json` in `admit420050/equiv.json`):

```
0x0041f8f0	EQUIV	[BYTE-IDENTICAL]
0x0041faf0	EQUIV	[BYTE-IDENTICAL]
0x0041fcc0	EQUIV	[ENC,ORDER,VALUE-FLOW]
0x00420050	EQUIV	[ORDER]
```

`0x00420050`: mode strict, rules `A6-paired-pop, A6-pinned-pop, jmp-threading, x87-barrier, x87-motion`, bytes_bad 9,
local_callees `_FUN_0041f8f0, _FUN_0041faf0, _FUN_0041fcc0`. Plain EQUIV, no `--relax-icall`.

## 3. L-147 coverage (log/orch/DECISIONS.md line 207)

L-147 answer, verbatim: "answered 2026-11-13 Mariano: (a) teach equiv.py to prove integer-across-x87 independence
(no blanket admit). [...] Admissions stay ADMITTED.tsv, never C4."

This verdict is that case: equiv.py proved it with its `x87-motion` rule (equiv_rules.md section 5), no blanket rule.
The only differing pair (from `RESULT_MATCH-EQUIV-420050-ORDER.md` section 2, unchanged by the 41FAF0 merge): integer
`mov edx,[ecx+4]` (orig `0x0042010a`) moved before x87 `fmul [esi+0x284]` (orig `0x00420104`). No shared register
(ECX/EDX against ESI), no EFLAGS, read against read. The 12-instruction x87 stream is identical in text and order, so
no x87 instruction order is admitted (WORKER_RULES L-126 line "x87 instruction order is never admitted"). Precedent
rows under L-147 (a): `0x004c4a50`, `0x00536e90` in ADMITTED.tsv.

## 4. Local callees (equiv_rules.md "Local callees")

- `0x0041f8f0`: EQUIV [BYTE-IDENTICAL]. relocheck OK (checked 26).
- `0x0041faf0`: EQUIV [BYTE-IDENTICAL]. relocheck OK (checked 18, unchecked `__ftol`). Byte-matched by MATCH-NM-41FAF0.
- `0x0041fcc0`: EQUIV [ENC,ORDER,VALUE-FLOW], mode frame, rules `A1-exposed, A5-arity, call-args-all, x87-barrier, x87-motion`.
  - **ORDER**: `pop esi` (ours `+0xf1`, orig `0x0041fdbf`) crosses `fmul [0x5d757c]`, `faddp st(1)`, `fld [0x5d757c]`.
    None reads ESI or ESP. The x87 stream is identical (13 instructions). Integer motion across x87, L-147 (a).
  - **ENC and VALUE-FLOW**: located with `admit420050/attr_trace.py`, a read-only wrapper that prints every class
    the post-proof `attribute()` (equiv.py line 2953) adds, with its residue (`admit420050/attr_41fcc0.txt`).
    Both come from one instruction pair: original `lea eax,[eax+0x5f6088]` at `0x0041fd21` (bytes `8d 80 88 60 5f 00`) and ours
    `lea eax,[eax]` with a pending relocation (object bytes `8d 80 00 00 00 00`). `pad()` (line 2977) tests the
    object displacement before relocation, sees `lea eax,[eax+0]` and files ours as padding, so `pa != pb` adds ENC
    (line 2996) and the original's `lea` remains an unmatched insert, which adds VALUE-FLOW (line 3067). In
    `match.py --show` (`nm41faf0/match_show_41fcc0.txt` line 24) the pair is `=` and relocheck checks that operand OK
    (the only relocheck BAD rows are `+0xf4` / `+0xfc` `__real@00000000`, offset-aligned reads one byte off after
    the `pop esi` shift: the original bytes at both operands, `0x5d757c`, are `00000000` = our `__real@00000000`).
    So ENC and VALUE-FLOW here are labels on a relocated operand, not a semantic difference. Both are proven
    classes under equiv_rules.md section 3 (VALUE-FLOW: "Proven, but none of the six syntactic classes explains the
    difference"), and both already appear in admitted rows (`0x00407be0`, `0x0042aa00`, `0x004c4a50`).
  - Following the precedent of `0x00417180` and `0x004332a0`, the local callees are recorded in the
    `local_callees` column, not as separate rows.

## 5. Caller scan

| caller | where | after the call | verdict |
|---|---|---|---|
| `0x00410b8b` (in `FUN_00410b30`) | original, the only `call`/`jmp` rel32 to `0x00420050` in executable sections; no absolute dword reference to `0x00420050` anywhere in the image (`admit420050/orig_callers.txt`) | `add esp,8` (EFLAGS overwritten), `inc esi; cmp esi,4; jl 0x410b81`. Loop path: `mov edx,[0x7d3ff8]`, `mov eax,[edx]`. Exit path: `mov ecx,[0x7d3ff8]`, then pushes. EAX/ECX/EDX written before read on both paths. ESI is callee-saved (a proof observable). | ignores |
| `re/match/s02/00410b30.c:87` | `FUN_00420050(i, *DAT_007d3ff8);` in a `for (i = 0; i < 4; i++)` loop, return value unused (declared void at line 49) | same as above (that source is the 100% match of `0x00410b30`) | ignores |
| `mashedmod/src` | 21 hits for `00420050`, all comments (RwRaceSubmit, RwRaceFx, RwCarFx, TrackRenderer, TrackLoaderMicros_p3, TrackHookSlots). `FUN_00410b30` there is `RwCoreMissing_00410b30` (RwCore_Missing.cpp:13), a stop stub | no compiled call | n/a |

The verdict is plain EQUIV, so EAX/ECX/EDX are already proven equal at return. The scan finds no unknown caller.

## 6. Cross-check (equiv_dyn)

`py -3.12 re/tools/match/equiv_dyn.py re/match/s05/00420050.c --only 0x00420050` : `SAME same=32 inc=0 diff=0`;
with `--seeded`: `SAME same=32 inc=0 diff=0` (`admit420050/dyn_default.txt`, `dyn_seeded.txt`).

## 7. ADMITTED.tsv row

```
0x00420050  FUN_00420050  re/match/s05/00420050.c  /O2 /GS /Ob1  397232926  fea5b5483  EQUIV  ORDER
A6-paired-pop,A6-pinned-pop,jmp-threading,x87-barrier,x87-motion  <caller scan>  <local callees>
same=32 inc=0 diff=0  same=32 inc=0 diff=0  no compiled caller on the path yet [...] Admitted under L-147 (a), not C4
verify/match_20261113/RESULT_MATCH-ADMIT-420050.md
```

## 8. Files

`verify/match_20261113/admit420050/`: `equiv_default.txt`, `equiv.json`, `equiv.err` (stats), `attr_trace.py`,
`attr_41fcc0.txt`, `relocheck_file.txt`, `orig_callers.txt`, `dyn_default.txt`, `dyn_seeded.txt`. No raw data
outside the repo. No processes left running.
