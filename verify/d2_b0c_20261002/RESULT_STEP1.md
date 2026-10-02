# RESULT — D2 attempt 17, STEP 1: what `+0xb0c` is, mechanically

Static only. No interpretation beyond what the instructions literally encode.
Instruments: `re/tools/findoffset.py` (direct displacement sweep, resynchronising),
a capstone **folded-base** sweep written for this step and **self-checked** (§1.2),
`re/tools/disasm_va.py`, `re/tools/memread.py`. All against
`original/MASHED.exe.unpatched` (SHA-256 anchor `BDCAE093...`, image base `0x00400000`).

---

## 1 The sweep, and its self-check

### 1.1 Direct form `[reg + 0xb0c]`

`py -3.12 re/tools/findoffset.py 0xb0c` -> **4 accesses in `.text`**:

```
  r  0x004676de  [esi]  fsub dword ptr [esi + 0xb0c]
  W  0x0046bc84  [ecx]  mov  dword ptr [ecx + 0xb0c], edx
  r  0x00470724  [edi]  fstp dword ptr [edi + 0xb0c]       <- a STORE (x87 pop-store)
  W  0x0047072c  [edi]  mov  dword ptr [edi + 0xb0c], ebx
```

`findoffset.py` labels by capstone's access flags; `fstp` is a store and is counted as a
writer below.

### 1.2 Folded form, and the known-answer self-check

Attempt 15 established that `&veh[i] + off` can encode with the base folded into the
displacement: `i*0xd04 + (0x008815a0 + off)`. A first sweep for that form returned **0
hits**, and **its known-answer self-check on `+0xbf8` also returned 0** — so the negative
was discarded, not reported. Cause: `capstone.disasm()` is a generator that stops at the
first undecodable byte, exactly the failure `findoffset.py`'s own comment documents. The
sweep was rewritten with a resynchronising loop and re-run.

**Known-answer check (PASS):** sweeping `+0xbf8` recovers attempt 15's published sites
verbatim, including the release writer

```
  W FOLDED  0x0046d7a2  mov dword ptr [eax + 0x882198], 2       <- base 0x008815a0 slot 0
  W FOLDED  0x0046d7cf  mov dword ptr [eax + 0x882198], 1
  r FOLDED  0x0046c742  mov eax, dword ptr [eax + 0x882198]
```

Only then was the `+0xb0c` result read. Coverage: **622 511 instructions** over
`.text 0x00401000..0x005cb000`.

**`+0xb0c` folded, base `0x008815a0` (the base attempt 16's axis-probe self-check
confirmed, `rec == 0x8815a0`), folded address `0x008820ac` — 1 hit:**

```
  r FOLDED  0x0046d6b6  mov eax, dword ptr [eax + 0x8820ac]
```

Three further hits at `0x008820b0` (`0x0046c6e6`, `0x0046c716`, `0x0046c71c`) are
`0x008820b0 - 0x008815a0 = 0xb10`, i.e. **`+0xb10`, not `+0xb0c`**. They are `+0xb0c` only
under the record base `0x008815a4`, which the live self-check contradicts.
`re/analysis/structs/vehicle_damage.md:35` derives `+0xB0C` from that `0x008815a4` base and
is therefore **off by one dword** for this field. Recorded, not acted on.

**Dword-index form:** `0xb0c / 4 = 0x2c3`. Immediate `0x2c3` in `push/mov/cmp/add/sub/lea`:
**0 hits** (same self-checked sweep).

**Second instrument:** the `GrepDecompAll.java` decompiler-text grep that attempt 15 added
was **not** needed to reach a hit here — the folded sweep returned a positive, and
`re/analysis/` already carries the decompiled citations for both consumers
(`A6a_FUN_00467650_decomp_20260824.txt:76`, `D3_AI_PORT_2026-09-26.md:97`). It would be the
required second instrument only for a **negative** claim, and no negative is claimed in §3.

---

## 2 The writers, and the transcribed expression

### 2.1 `A4 FUN_00470670` — the per-frame writer. `0x004706d9 .. 0x0047072c`

Record base is `EDI` (`0x00470679 mov edi, eax`; `in_EAX` in the decompilation).

```
0x00470680  fld   dword ptr [edi + 0x9e4]          ; speed
0x00470686  fcomp dword ptr [0x5d757c]             ; vs DAT_005d757c = 0.0
0x004706a0  fnstsw ax
0x004706a6  test  ah, 0x44
0x004706d9  jnp   0x47072c                         ; speed != 0.0 falls through

0x004706db  fld   dword ptr [edi + 0x9d8]          ; fwd.y
0x004706e1  fmul  dword ptr [edi + 0x9b4]          ; * vel.y
0x004706e7  fld   dword ptr [edi + 0x9d4]          ; fwd.x
0x004706ed  fmul  dword ptr [edi + 0x9b0]          ; * vel.x
0x004706f3  faddp st(1)                            ; (fwd.y*vel.y + fwd.x*vel.x)
0x004706f5  fld   dword ptr [edi + 0x9dc]          ; fwd.z
0x004706fb  fmul  dword ptr [edi + 0x9b8]          ; * vel.z
0x00470701  faddp st(1)                            ; dot = (fy*vy + fx*vx) + fz*vz
0x00470703  fcom  dword ptr [0x5d757c]             ; vs 0.0
0x00470709  fnstsw ax
0x0047070b  test  ah, 5
0x0047070e  jp    0x470712                         ; skip FCHS unless dot < 0.0
0x00470710  fchs                                   ; |dot|
0x00470712  fdiv  dword ptr [edi + 0x9e4]          ; |dot| / speed
0x00470718  fsubr dword ptr [0x5cc320]             ; _DAT_005cc320 - that
0x0047071e  fmul  dword ptr [edi + 0x9e4]          ; * speed
0x00470724  fstp  dword ptr [edi + 0xb0c]          ; <- STORE

0x0047072c  mov   dword ptr [edi + 0xb0c], ebx     ; <- STORE, EBX = 0 (0x00470694 xor ebx,ebx)
```

Constants, read from the anchored binary, not from an annotation
(`memory: audit-annotated-consts-against-the-binary`):

| symbol | VA | bytes | value |
|---|---|---|---|
| `DAT_005d757c` | `0x005d757c` | `00 00 00 00` | `0.0` |
| `_DAT_005cc320` | `0x005cc320` | `00 00 80 3f` | **`1.0`** (`0x3f800000`) |

So the stored value is exactly

```
+0xb0c = 0                                        if  +0x9e4 == 0.0
+0xb0c = (1.0 - |dot| / +0x9e4) * +0x9e4          otherwise
         dot = (+0x9d8 * +0x9b4 + +0x9d4 * +0x9b0) + +0x9dc * +0x9b8
```

The seven inputs are `+0x9b0/+0x9b4/+0x9b8` (vel) `+0x9d4/+0x9d8/+0x9dc` (fwd) `+0x9e4`.

### 2.2 `0x0046bc84` — an initialisation writer, not a per-frame one

`mov dword ptr [ecx + 0xb0c], edx`, inside a run of single-dword stores of the same `EDX`
to `+0xb18 +0xb14 +0xaf0 +0xaf4` (`0x0046bbb2` stores the literal `0xbf800000 = -1.0f` to
`+0xaf8`) `+0xafc +0x9f0 +0x9f4 +0xad0 +0x490 +0x494 +0xb24 +0xb28 +0xaec +0xb0c +0xb10`,
followed by `lea esi, [ecx+0x4ac]` and a `mov dword ptr [esi], 0xffffffff` / `add esi,0x40`
loop. A record-reset block. **[UNCERTAIN]** that `EDX` is 0 here: not witnessed in this
step, and no claim in §4 depends on it.

---

## 3 The readers — both of them

| RVA | function | instruction | what it does with it |
|---|---|---|---|
| `0x004676de` | A6a `FUN_00467650` | `fsub dword ptr [esi + 0xb0c]` | `fVar5 = 1500.0 - b0c`, then clamped |
| `0x0046d6b6` | accessor `FUN_0046d6a0` | `mov eax, [eax + 0x8820ac]` | copies it to `*param_1`, returns 1 |

**No other reader exists in `.text`** in either the direct or the folded form, under the
self-checked sweep of §1.

### 3.1 The A6a channel, transcribed

```
0x004676cd  fld   dword ptr [0x5cd0ac]        ; _DAT_005cd0ac = 1500.0 (44 bb 80 00 -> 0x44bb8000)
0x004676de  fsub  dword ptr [esi + 0xb0c]     ; fVar5 = 1500.0 - b0c
0x00467702  fcom  dword ptr [0x5ccd04]        ; _DAT_005ccd04 = 500.0 (0x43fa0000)
0x00467718  fnstsw ax
0x0046771a  test  ah, 0x41
0x0046771d  jp    0x467727                    ; keep fVar5 when fVar5 > 500.0
0x0046771f  fstp  st(0)                       ; else discard it
0x00467721  fld   dword ptr [0x5ccd04]        ; and use 500.0
0x00467727  fmul  dword ptr [0x5ce1e8]        ; fVar5 *= _DAT_005ce1e8
```

i.e. **`fVar5 = max(1500.0 - b0c, 500.0)`**. The lower clamp engages only once
`b0c >= 1000.0`.

Port: `Integrate2.cpp:145  float fVar5 = k1500 - Rf(v, 0xb0c);` — same read, same operand.

### 3.2 The accessor channel

`FUN_0046d6a0` is `(param_1 = out, param_2 = slot)`; `cmp eax, 0x10 / jb` rejects slot
`>= 0x10` with a `0` return, else `imul eax, eax, 0xd04` and the folded load. It is vtable
slot `[+0x20]` (`D3_AI_PORT_2026-09-26.md:97`, `AiStandalone.cpp:823`), consumed by the AI
speed-spike rule at `0x004167d5..0x0041680e`. Port: `AiStandalone.cpp:833`
(`const float rate0 = s_host.veh_f32(v, 0xb0c);`).

---

## 4 The port's corresponding code, and the ONE non-faithfulness found

| original | port, exe target | port, asi target |
|---|---|---|
| `0x0047072c` zero store | `VehicleControl.cpp:108` `Fb(v, 0xb0c) = 0.0f;` | `PhysicsChainHooks.cpp:296` `Ib(v, 0xb0c) = 0;` |
| `0x004706db..0x00470724` | `VehicleControl.cpp:111-115` | `PhysicsChainHooks.cpp:298-303` |
| `0x004676de` read | `Integrate2.cpp:145` | `PhysicsChainHooks.cpp:1721` (`fsub dword ptr [esi+0xb0c]`, inline asm) |
| `0x0046d6b6` read | `AiStandalone.cpp:833` | — |

`VehicleControl.cpp` is in `exe_sources.rsp` only; `PhysicsChainHooks.cpp` in
`asi_sources.rsp` only. Two bodies for one RVA, one per target — the established pattern,
and a standing drift hazard (`memory: duplicate-rva-implementations-drift`). They agree
here: `vc::kOne` is annotated `1.0f // _DAT_005cc320 (0x3f800000)` and §2.1 confirms that
annotation against the binary.

**The one difference, in BOTH port copies:** the dot-product association order.

```
ORIGINAL   0x004706db..0x00470701    dot = (fwd.y*vel.y + fwd.x*vel.x) + fwd.z*vel.z
PORT       VehicleControl.cpp:111    dot = (fwd.z*vel.z + fwd.x*vel.x) + fwd.y*vel.y
           PhysicsChainHooks.cpp:298   (same order as VehicleControl.cpp)
```

This is a real, free, exact-to-fix non-faithfulness. Its size is float-rounding only
(both sides evaluate in x87 80-bit and store float32), so it is **not** offered as an
explanation of anything. Whether it is fixed is decided after STEP 2, under the
pre-registered rules.

**No other arithmetic difference exists** between the original's expression and either
port copy.

---

## 5 What STEP 1 establishes, and what it does not

**Establishes:** `+0xb0c`'s producer is one transcribed expression over seven record
fields; the port reproduces that expression; `+0xb0c` has exactly two readers, and the A6a
one is `max(1500 - b0c, 500)`.

**Does not establish:** which input diverges, at which `d`, or by how much. That is STEP 2,
pre-registered in `PREREG_STEP2.md`, committed unrun alongside this file.

**Immediate consequence, stated as a hypothesis and NOT as a finding:** since the port's
expression is the original's, a `+0xb0c` divergence is by construction a divergence of its
inputs, so `+0xb0c` is a **symptom**. STEP 2's gates DR and CB are what decide that.
