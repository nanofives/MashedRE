# D2 attempt 20 — STEP 1: the original's per-wheel state machine, transcribed by RVA

Source: `original/MASHED.exe.unpatched`
(SHA-256 `BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`), disassembled with
`re/tools/disasm_fn.py` (capstone, i386). Raw listings committed alongside this file:

- `disasm_head_0046f6c0_0046f870.txt` — prologue, init loop, queries, classifier call, `bVar4`
- `disasm_statemachine_0046f850_0046fa40.txt` — the `bVar4` accumulate + the 4-wheel state machine
- `disasm_combine_0046fa26_0046fd60.txt` — the grounded-count combine, the 4-wheel drop, the
  `bVar16 == 3` promotion

Port side compared: `mashedmod/src/mashed_re/Collision/WheelContactSolver.cpp:140-207`
(plus `:218-233` and `:241-259`, which the replay in STEP 2 needs).

**VERDICT: no transcription defect. Every condition, every constant, every field offset and
every control-flow edge in `WheelContactSolver.cpp:140-207` matches the anchored binary.**
The constants are confirmed by direct byte read, not by re-using the port's own annotations
(memory `audit-annotated-consts-against-the-binary`).

---

## 1 States' meanings, stated mechanically only

The per-wheel record stride is `0xc4` bytes. Wheel `w` (0..3) uses:

| field | wheel 0 VA offset | read/written by |
|---|---|---|
| `fv` (the state machine's scalar input, `piVar9[-1]`) | `+0x194` | written `10.0f` by the init loop (`0x0046f74x`, port `:95`), then `depth` by the classifier `0x0046cc40` |
| `state` (`piVar9[0]`) | `+0x198` | this function only, three sites (below) |
| `key` (`piVar9[0x15]`) | `+0x1ec` | written `-1` by the init loop, then `param_2[0xc]` by the classifier |

`state` takes exactly the literal values `0`, `1` and `2`, written at
`0x0046f91f` (`0`), `0x0046f92a` (`1`), `0x0046f957` / `0x0046f9b2` (`2`),
`0x0046fbb2` (`0`, the drop) and `0x0046fd17` / the three promotion stores (`1`).
**No semantic name is assigned to 0/1/2 here** — the only consumers measured are the
`bVar16` combine (`0x0046fa26..0x0046fae3`, counts `state == 1`), the accumulate gate
(latched iff the state machine took a latching edge), the drift's own `nb` counters
(`state != 1`), and the masking tail `0x0047044b..0x004704b0` (counts `state != 0`).

## 2 `bVar4` — the gate input the state machine's demotion arm uses

Constants, read from the binary:
`[0x005ce18c] = 0x3ca3d70a = 0.019999999552965164f` (`kState2Lo`),
`[0x005ceaa0] = 0xbba3d70a = -0.004999999888241291f` (`kStateCmp`).

```
0046f6d3  xor  ebp,ebp                 ; cVar13 = 0
0046f70e  mov  [esp+0x10],ebp          ; the iVar8 "default" slot = 0
...
0046f827  mov  eax,[edi+0x198]         ; WHEEL 0
0046f830  cmp  eax,1
0046f833  jne  0x46f85f                ;   state != 1 -> ecx = [esp+0x10] = 0
0046f835  fld  [edi+0x194]             ; fv
0046f83b  fcom [0x5ce18c]              ; vs 0.02
0046f843  test ah,1                    ; C0 = less
0046f846  jne  0x46f84d                ;   fv < 0.02 -> skip
0046f848  mov  ebp,1                   ;   cVar13 = 1        (fv >= 0.02)
0046f84d  fcomp [0x5ceaa0]             ; vs -0.005
0046f853  mov  ecx,1
0046f85a  test ah,0x41                 ; C3|C0 = equal|less
0046f85d  jnp  0x46f863                ;   fv <= -0.005 -> keep ecx = 1
0046f85f  mov  ecx,[esp+0x10]          ;   else ecx = 0
0046f863  ... wheels 1/2/3: identical, `inc ebp` / `inc ecx` form
0046f8e7  xor  ebx,ebx
0046f8e9  test ebp,ebp / je  0x46f8f3
0046f8ed  test ecx,ecx / jne 0x46f8f3
0046f8f1  mov  ebx,edx                 ; bVar4 = (cVar13 != 0) && (iVar8 == 0)
```

Port `:141-158`. `cVar13 = (kState2Lo <= fv) ? 1 : 0` for wheel 0 is the same as `inc ebp`
from 0 because `0x0046f6d3` zeroes it; `iVar8 = 1; if ((fv < kStateCmp) == (fv == kStateCmp))
iVar8 = 0;` is the same as `ecx = 1 / ecx = [esp+0x10]` because `0x0046f70e` zeroes that slot.
**Both "assumed zero" values are writes in the binary, cited.** `bVar4` identical.

## 3 The state machine, `0x0046f8f3..0x0046fa20` — line-by-line against `:160-199`

Loop set-up `0x0046f8f3..0x0046f8fd`: `esi = 0` (`iVar14`), `edx = 0` (`byteoff`),
`ecx = edi+0x198` (`piVar9`), `ebp = -1` (the `key` comparand). Trailer `0x0046fa11`:
`esi += 4`, `ecx += 0xc4`, `edx += 0xc`, `cmp esi,0x10 / jl 0x46f900` — 4 iterations,
`piVar9 += 0x31` ints = `0xc4` bytes. Port `:162-164`, `:198`. Match.

```
0046f900  mov  eax,[ecx]               ; state
0046f902  fld  [ecx-4]                 ; fv
0046f905  test eax,eax / je 0x46f935   ; state == 0 -> LATCH arm
; ---------- ARM A: state != 0 ----------
0046f909  fcomp [0x5ce18c]             ; vs 0.02
0046f911  test ah,0x41 / jne 0x46f91a  ; fv <= 0.02 -> straight to the key test
0046f916  test ebx,ebx / jne 0x46f91f  ; fv > 0.02 AND bVar4 -> state = 0
0046f91a  cmp  [ecx+0x54],ebp          ; key vs -1
0046f91d  jne  0x46f92a                ;   key != -1 -> state = 1, ACCUMULATE
0046f91f  mov  [ecx],0                 ;   key == -1 -> state = 0, NO accumulate
0046f925  jmp  0x46fa11
0046f92a  mov  [ecx],1
0046f930  jmp  0x46f9c0
```
Port `:178-183`:
`else if (((kState2Lo < fv) && bVar4) || (piVar9[0x15] == -1)) piVar9[0] = 0;`
`else { piVar9[0] = 1; latched = true; }` — **identical predicate**, identical latch.

```
; ---------- ARM B: state == 0 ----------
0046f935  fcom [0x5cc34c]              ; vs -2.0
0046f93d  test ah,0x41 / jne 0x46fa0f  ; fv <= -2.0 -> fstp, no latch, state stays 0
0046f946  fcomp [0x5d757c]             ; vs 0.0
0046f94e  test ah,0x41 / jp 0x46fa11   ; fv > 0.0 -> no latch, state stays 0
0046f957  mov  [ecx],2                 ; -2.0 < fv <= 0.0 -> state = 2
0046f95d..0046f985  proj = (fwd . vel) / speed        ; edi+0x9d4/9d8/9dc . edi+0x9b0/9b4/9b8, / edi+0x9e4
0046f98b  fcom [0x5d757c]  / test ah,0x44 / jnp 0x46f9be   ; proj == 0 -> fstp, fall to 0x46f9c0
0046f998  fcom [0x5cea98]  / test ah,0x41 / jne 0x46f9ba   ; proj <= -0.1 -> fstp, jmp 0x46f9c0
0046f9a5  fcomp [0x5cea90] / test ah,5   / jp  0x46f9c0    ; proj >= 0.1 -> jmp 0x46f9c0
0046f9b2  mov  [ecx],2                                     ; |proj| < 0.1 -> state = 2 (already 2)
0046f9b8  jmp  0x46f9c0
```
Constants read from the binary: `[0x005cc34c] = 0xc0000000 = -2.0f`,
**`[0x005d757c] = 0x00000000 = 0.0f`** (so the port's `kZero` comparand is correct —
this was the one comparand the port named rather than cited),
`[0x005cea98] = -0.1` (double, 8 bytes), `[0x005cea90] = 0.1` (double).

Port `:167-177`:
`if ((kSpring2 < fv) && ((fv < kZero) != (fv == kZero))) { piVar9[0] = 2; ...; latched = true; }`
`(fv < 0) XOR (fv == 0)` is `fv <= 0` because the two are mutually exclusive on an ordered
compare — the same as `0x0046f94e`'s `jp` taken only when neither C0 nor C3 is set.
The projection block's **only** state write is `mov [ecx],2` onto a state already `2`, and
**every** exit from it reaches `0x0046f9c0` (the accumulate), including the two `fstp st(0)`
paths at `0x0046f9ba` and `0x0046f9be`, which fall through. So the projection is inert for
both the state and the latch. Port `:174-176` writes `2` under a narrower condition and sets
`latched = true` unconditionally — **same observable behaviour, confirmed inert.**

Accumulate `0x0046f9c0..0x0046fa0f`: `[ecx+0x68]/[ecx+0x6c]/[ecx+0x70]` times `[ecx-4]`,
stored to `esp+esi+0x60` / `esp+esi+0x1c` / `esp+esi+0x74` and subtracted from
`[edx+0x88e620]/+0x88e624/+0x88e628`. Port `:184-197` (`vF(piVar9,0x1a/0x1b/0x1c)`,
`local_bc` / `local_100v` / `local_a8`, `g_wheelContactPos`). Match.

## 4 The tail, `0x0046fa26..0x0046fd29` — needed by STEP 2's replay

`[0x005cc574] = 0x40000000 = 2.0f` (the gate comparand, unchanged from attempt 19).

- **Combine** `0x0046fa26..0x0046fae3`: `eax` (= `bVar16`) counts `state == 1` over the four
  wheels; `fVar2` / `local_f0` / `local_110` sum `local_bc` / `local_100v` / `local_a8`
  for those wheels only; wheel 0 **assigns** rather than adds (`0x0046fa49..0x0046fa61`).
  Port `:202-207`. Match, including the `fld [0x5d757c]` zero seed.
- **Drop** `0x0046fae3..0x0046fbbd`: `cmp eax,4 / jne 0x46fbbf`; `sel = argmax_w |fv_w|`
  with the exact tie-break `sel = (a0<a1)?1:0`, then `if (mx<a2) sel=2`, then
  `if (mx<a3) sel=3`; `0x0046fbb2 mov [ecx+edi+0x198],0` with `ecx = sel*0xc4`, and
  `bVar16` becomes 3. Port `:218-230`. Match.
- **`bVar16 == 3` promotion** `0x0046fc09..0x0046fd29`: the single wheel whose state is not
  `1` is set to `1` and inherits the neighbour's normal, `fv` and `key`
  (`0x0046fd17` state, `0x0046fd1d` `fv` from `[edi+0x258]`, `0x0046fd23` `key` from
  `[edi+0x2b0]` for the wheel-0 case). Port `:241-259`. Match.
  **Consequence, recorded because STEP 2's replay depends on it: after a `bVar16 == 3`
  frame all four states are `1`.**

## 5 Where `fv` and `key` come from — the producer RVAs

Both are written twice per solver call, and only by two sites:

1. **Reset, `0x0046f6c0`'s own init loop** (port `:91-103`):
   `vF(puVar6,0) = 10.0f` -> `+0x194`, and `puVar6[0x16] = -1` -> `+0x1ec`.
   (`[literal 0x41200000] = 10.0f` at `0x0046f74x`; the `-1` is an immediate.)
2. **The classifier `0x0046cc40`** (`WheelTerrainContactClassifier`,
   `CarWorldContacts.cpp:94-149`), and **only on a NEW contact**
   (`ContactHistoryLookup(param_2, param_1) == 0`, `:125`):
   `local_90[0] = depth` -> `+0x194` = `fv` (`:129`), and
   `local_90[0x16] = param_2[0xc]` -> `+0x1ec` = `key` (`:130`),
   where `local_90 = vFP(param_1, 0x65)` = `self+0x194` with stride `0x31` ints (`:143`).

So, mechanically: **a wheel the classifier does not fill keeps `fv = 10.0f` and
`key = -1`** — and both remaining arms of U-9179 then force `state = 0`:
`fv = 10.0 > 0.02` plus `key == -1` on ARM A, and `fv = 10.0 > 0.0` on ARM B.
`re/tools/dispsweep.py 0x194 0x198 0x1ec` finds no other writer inside
`0x0046f6c0..0x004705xx` beyond the sites above plus the drop and the promotion; it is
blind to the classifier's running pointer by construction (memory
`findoffset-blind-to-computed-bases`), which is why the classifier is cited from the
decompiled body instead.

**[UNCERTAIN]** `0x0046bb47 mov [ecx+0x198],eax` writes a wheel-0-displacement state from
outside `0x0046f6c0`. It is not shown here to be on the per-frame path, and STEP 2's KA-O
gate is the instrument that will expose it if it is: an out-of-function state writer would
show up as replay disagreement on the original.

## 6 What this means for U-9179

The two arms attempt 19 left open are **not** distinguishable from static code: both are
satisfied by the same upstream condition (the classifier not filling the wheel). The
question therefore moves one step upstream, and STEP 2 is pre-registered to measure it on
both sides rather than infer it: `PREREG_STEP2.md`.
