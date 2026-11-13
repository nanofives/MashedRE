# RESULT MATCH-NM-5518F0-4CAC10 (2026-11-13)

Verdict: **GREEN**. Both near misses are byte-identical under `re/tools/match/match.py` (toolchain tag `rw9044`,
flags `/O2 /Ob1 /G5`, the recorded flags in each source header), `re/tools/match/relocheck.py` reports OK, and neither
source contains `__asm`. Kind: match-only, so no build and no gates.

Base: `orch/integration` @ 145dec4d0. Starting scores are from `re/match/rwtb/*.c` as left by MATCH-RWT-B
(`verify/match_20261112/RESULT_MATCH-RWT-B.md`, row table at line 78). The ranked list on `orch/MATCH-RWT-REFRESH`
@ 5c48ce7bc still lists the g2s figures (0x005518f0 68.8, 0x004cac10 compile-fail), which are older than the rwtb sources.

| RVA | Start | End | Bytes | relocheck | Shaping that closed it |
|---|---|---|---|---|---|
| 0x005518f0 | 96.8 (regs 100, ops 100) | 100.0 | 339 B, 0 differing unmasked | OK, checked=15 unchecked=0 | release block |
| 0x004cac10 | 92.6 (regs 93.1, ops 99.5), 201/202 ins | 100.0, 202/202 ins | 639 B, 0 differing unmasked | OK, checked=52 unchecked=0 | raster extension pointer |

## 0x005518f0

Remaining diff at 96.8: in the refcount release block the original loads `anim[5]` into eax and the count into ecx
(`mov eax,[esi+0x14]; mov ecx,[eax+0x40]; dec ecx; mov [eax+0x40],ecx; jne`). The source produced the same ops with
the registers swapped.

Ten forms tried (scratch driver `v18f0.py`):

| Form | Score |
|---|---|
| `if (--*(int *)(anim[5]+0x40) != 0)` | 85.0 (2 extra ins) |
| reuse `interp` / `dict` as pointer or count (4 forms) | 96.8 |
| unsigned count | 96.8 |
| typed struct post-decrement then re-read `cd->ref` | 85.0 |
| `__inline` destroy that returns 1/0 | 85.0 |
| **typed struct: `CD *cd = anim->cd; int r = --cd->ref; if (r) return 0;`** | **100.0** |
| `__inline` destroy that returns `r` with the same struct read | 100.0 |

Committed: the typed-struct form in place (`CD18f0` at +0x40, `AN18f0` with `cd` at +0x14), free call reads
`((AN18f0 *)anim)->cd` again. Every int-offset form kept the swap. A typed pointer to the record gave the original's
allocation.

## 0x004cac10

Remaining diff at 92.6: the original forms the raster extension as `mov eax,[eax]; add eax,edx; mov eax,[eax+0x1c]`,
the source folded it into `mov ecx,[eax]; mov eax,[ecx+edx+0x1c]` (one instruction short, so every later branch
target shifted).

Eighteen forms tried (scratch driver `vcac10.py`). Every form that loads `ext->swapChain` once into a local folded
the add (11 forms: struct local, `__inline` getter for the extension or the swap chain, `+=`, uchar pointer, parent
struct, operand order, parentheses, unsigned offset, array index, all 92.6). A function-like macro re-evaluated in
place also folded (92.6). Every form with a **named extension pointer read three times** (`if (ext->sc)`, then
`(*ext->sc)->Present(ext->sc, ...)`, the way the COM call macro expands) matched at 100.0 (6 forms: plain, const,
typed vtable, `top` local, `!= 0` test).

Committed: `RX4cac10 *ext = (RX4cac10 *)((char *)*param_1 + DAT_00911ae4)` with `sc` at +0x1c, read three times, and
`piVar1` removed.

## Promotion

`re/promotions/MATCH-NM-5518F0-4CAC10.tsv` (route `match`, both rows). Dry run
`py -3.12 re/tools/c4_reclass.py batch --tsv re/promotions/MATCH-NM-5518F0-4CAC10.tsv`:

```
[1/2] 0x005518f0 match PASS byte-identical 339 B, relocheck OK (15/0)
[2/2] 0x004cac10 match PASS byte-identical 639 B, relocheck OK (52/0)
batch: 2 pass, 0 refused, 0 skipped (already C4), 2 rows  [by route: match=2]
```

Both rows are C1 in `hooks.csv` today, so `--apply` will move them C1 -> C4 (route match, L-93). That certifies the RE
only. Not applied on the branch: the merge train applies it with `--apply --strict`.

## Notes for the port axis (not done here)

- 0x004cac10 has a `PROVISIONAL.tsv` row: the knob path runs a lifted naked-asm transcription
  (`RwCore/RwCore_Lifted_NN.cpp`, L-137 scaffolding). A byte-identical C source now exists, so a later port row can
  swap the lifted body for this source and drop the provisional row (L-144: executed RW tail stays strict, and a
  match satisfies it). Not touched here.
- 0x005518f0 has no exe body (`port` empty), so it does not gain one from this row.

Scratch drivers (not committed, session scratchpad): `v18f0.py`, `vcac10.py`.
