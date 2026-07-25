# KV Batch-B call-frame recovery (2026-07-25)

Resolves the blocker recorded in `mashedmod/src/mashed_re/Collision/RwpVtableKV1.cpp`'s header:
`FUN_0056adb0`, `FUN_0056aae0` and `FUN_0056ac40` all build **stack-constructed outgoing
frames** that Ghidra mis-models (calls printed with no/too-few arguments; the "locals"
`local_14..local_40` / `piStack_dc` / `piStack_d8` and the `uStack_e0 = 0x56af69` *return
address* are in fact the frame). Documented pitfall: `feedback_ghidra_prebranch_args`.

Method: `memory_read` on pool0 (read_only) + offline capstone. No master write. Every claim
below is an instruction at a cited address.

---

## 1. `FUN_0056ac40` — true arity is **15 dwords**, not 4

EBP-framed with 16-byte stack alignment:

```
0056ac40  push ebp
0056ac41  mov  ebp, esp
0056ac43  and  esp, 0xfffffff0        <- 16-byte align (matters for the movaps/SSE body)
0056ac46  sub  esp, 0x38
0056ac49  mov  eax, [ebp + 0x40]      <- deepest arg read
...
0056ad9a  mov  ecx, [ebp + 8]         <- param_1 (int*  out)
0056ad9d  mov  eax, [ebp + 0xc]       <- param_2 (uint* out)
0056ada0  mov  [ecx], edi
0056ada2  mov  [eax], edx
```

With `push ebp; mov ebp,esp`, `[ebp+8]` = entry `[esp+4]` = arg1. So args run `[ebp+0x08]`
(arg1) .. `[ebp+0x40]` (**arg15**). Ghidra's deep-slot names map cleanly:

| Ghidra | machine | arg # |
|---|---|---|
| `param_1` | `[ebp+0x08]` | 1 |
| `param_2` | `[ebp+0x0c]` | 2 |
| `param_3` | `[ebp+0x10]` | 3 |
| `param_4` | `[ebp+0x14]` | 4 |
| `in_stack_00000024` | `[ebp+0x28]` (`mov edx,[ebp+0x28]` @0056ac6e) | 9 |
| `in_stack_00000030` | `[ebp+0x34]` (`mov eax,[ebp+0x34]` @0056ac68) | 12 |
| `in_stack_0000003c` | `[ebp+0x40]` (`mov eax,[ebp+0x40]` @0056ac49) | 15 |

→ port signature: take a by-value `struct Kv_Ac40Frame { int w[15]; }`, indices
`w[0]`=param_1 … `w[8]`=in_stack_00000024, `w[11]`=in_stack_00000030, `w[14]`=in_stack_0000003c
(index = offset/4 - 1, the same rule proven for `0x0056a450`).

**Return is `float10` in ST0** — declare returning `float`/`long double`, never `void`
(`feedback_x87_st0_float10_return_fnptr`).

---

## 2. `FUN_0056adb0` → `FUN_0056ac40`, call site #1 (`call 0x56ac40` @ `0x0056af64`)

MSVC builds the 15 dwords as **1 push + four 3-dword blocks + 2 pushes**
(1 + 12 + 2 = 15 ✓). Verbatim:

```
0056aedf  mov  eax, [ebp + 0x10c]
0056aee5  mov  edx, [esp + 0x60]
0056aee9  push eax                     ; --> arg15
0056aeea  mov  eax, [esp + 0x68]
0056aeee  sub  esp, 0xc                ; block D (args 12,13,14)
0056aef1  mov  ecx, esp
0056aef3  sub  esp, 0xc                ; block C (args 9,10,11)
0056aef6  mov  [ecx], edx              ;   D+0 = [esp+0x60]
0056aef8  mov  edx, [esp + 0x84]
0056aeff  mov  [ecx + 4], eax          ;   D+4 = [esp+0x68]
0056af02  mov  eax, esp
0056af04  sub  esp, 0xc                ; block B (args 6,7,8)
0056af07  mov  [ecx + 8], edx          ;   D+8 = [esp+0x84]
0056af0a  mov  ecx, [esp + 0x94]
0056af11  mov  edx, [esp + 0x98]
0056af18  mov  [eax], ecx              ;   C+0 = [esp+0x94]
0056af1a  mov  ecx, [esp + 0x9c]
0056af21  mov  [eax + 4], edx          ;   C+4 = [esp+0x98]
0056af24  mov  edx, esp
0056af26  sub  esp, 0xc                ; block A (args 3,4,5)
0056af29  mov  [eax + 8], ecx          ;   C+8 = [esp+0x9c]
0056af2c  mov  eax, [esp + 0xbc]
0056af33  mov  ecx, [esp + 0xc0]
0056af3a  mov  [edx], edi              ;   B+0 = EDI
0056af3c  mov  [edx + 4], eax          ;   B+4 = [esp+0xbc]
0056af3f  mov  eax, [esp + 0xb0]
0056af46  mov  [edx + 8], ecx          ;   B+8 = [esp+0xc0]
0056af49  mov  ecx, [esp + 0xb4]
0056af50  mov  edx, esp
0056af52  mov  [edx], esi              ;   A+0 = ESI
0056af54  mov  [edx + 4], eax          ;   A+4 = [esp+0xb0]
0056af57  lea  eax, [esp + 0x4c]
0056af5b  mov  [edx + 8], ecx          ;   A+8 = [esp+0xb4]
0056af5e  lea  edx, [esp + 0x50]
0056af62  push edx                     ; --> arg2 = &local_84
0056af63  push eax                     ; --> arg1 = &local_88
0056af64  call 0x56ac40
```

Resulting arg vector (arg1 = lowest address = last pushed):

| arg | value |
|---|---|
| 1 | `&local_88` (`lea [esp+0x4c]`) — receives the lane index |
| 2 | `&local_84` (`lea [esp+0x50]`) — receives the axis index |
| 3,4,5 | block A = `ESI`, `[esp+0xb0]`, `[esp+0xb4]` |
| 6,7,8 | block B = `EDI`, `[esp+0xbc]`, `[esp+0xc0]` |
| 9,10,11 | block C = `[esp+0x94]`, `[esp+0x98]`, `[esp+0x9c]` |
| 12,13,14 | block D = `[esp+0x60]`, `[esp+0x68]`, `[esp+0x84]` |
| 15 | `[ebp+0x10c]` = adb0's `in_stack_00000108` |

Confirms the decomp's `uStack_a4 = in_stack_00000108` was the arg15 store, and
`piStack_dc/piStack_d8` were args 1/2. **All `[esp+X]` displacements above are relative to
the esp AT THAT INSTRUCTION** — esp moves by the pushes/subs as the block is built, so they
must be re-based (subtract the cumulative adjustment) before mapping to adb0's own frame
slots. Do that re-basing explicitly when porting; do not copy the raw displacements.

## 2b. Call site #2 (`call 0x56ac40` @ `0x0056b168`, return addr `0x56b16d`)

Same 1+12+2 shape (the decomp's second `uStack_e0 = 0x56b16d` block), with the second body
set — the assignments differ (`local_c8 = iVar11`, `local_c4 = local_18`, `local_d4 = iVar13`
etc. in the decomp). Re-derive its blocks the same way from `0x0056b0d0..0x0056b168` before
porting; the shape is identical, the sources are not.

---

## 3. `FUN_0056aae0` → `FUN_0056a7a0` (`call 0x56a7a0` @ `0x0056abfb`) — **37 dwords**

```
0056abcb  sub  esp, 0x88               ; reserve 0x22 (34) dwords
...
0056abd5  mov  edx, [esp + 0xb4]
0056abdc  mov  edi, esp                ; edi = base of the 34-dword area
0056abde  sub  esp, 0xc                ; 3 more dwords BELOW it
0056abe1  rep movsd                    ; ECX=0x22 : [esi] -> [edi]   (args 4..37)
0056abe3  mov  esi, [esp + 0xb8]
0056abea  mov  ecx, [esp + 0xbc]
0056abf1  mov  eax, esp
0056abf3  mov  [eax], esi              ; arg1 = [esp+0xb8]
0056abf5  mov  [eax + 4], ecx          ; arg2 = [esp+0xbc]
0056abf8  mov  [eax + 8], edx          ; arg3 = [esp+0xb4]
0056abfb  call 0x56a7a0
```

→ `FUN_0056a7a0` arity = **3 + 34 = 37 dwords**. Args 1..3 are aae0's `param_3/param_4/param_5`
(matching the decomp's `FUN_0056a7a0(param_3,param_4,param_5)`), args 4..37 are a verbatim
34-dword copy of aae0's OWN incoming frame starting at `in_stack_0000003c`
(`&stack0x0000003c` in the decomp) — i.e. aae0 forwards a window of its own arguments.

`FUN_0056aae0`'s own arity: it reads up to `in_stack_000000dc` → **≥ 55 dwords**
(0xdc/4 = 55). Confirm the exact top before fixing its frame struct size.

Post-call tail (x87, note the ST0 chain is live across the `add esp` / `pop`s):
```
0056ac00  fld   dword ptr [esp + 0x180]
0056ac07  fmul  dword ptr [esp + 0x188]
0056ac19  fadd  dword ptr [esp + 0xe8]
0056ac24  fmul  dword ptr [esp + 0xd4]
0056ac31  fadd  dword ptr [eax]
0056ac33  fstp  dword ptr [eax]
```
= the decomp's `*pfVar1 = (in_stack_000000d4 * in_stack_000000dc + in_stack_000000d8) *
in_stack_000000c8 + *pfVar1;` — products/sums stay 80-bit until the single `FSTP`, so port
with `float10` intermediates and ONE store (K2/K3 idiom), not plain `float` arithmetic. This
is precisely the class that VECCAP-2 caught.

---

## 4. What is now unblocked, and what still is not

**Unblocked:** all three outgoing frames have known arity and shape, so the three functions
can be ported with by-value frame structs (the `Kv11Frame` pattern already used for
`0x0056a450`).

**Still required before writing the code:**
1. Re-base every `[esp+X]` displacement in §2 / §2b against the running esp so the sources
   map to adb0's own frame words. Mechanical but error-prone — do it in a table, not in-line.
2. Derive call-site #2's four blocks explicitly (§2b) rather than assuming they mirror #1.
3. Pin `FUN_0056aae0`'s exact incoming arity (≥55 dwords) and `FUN_0056a7a0`'s signature.
4. `FUN_0056ac40`'s body is SSE with `movaps`/`cmpneqps`/`movmskps` on a 16-byte-ALIGNED
   frame (`and esp,0xfffffff0` @0056ac43) plus the `_DAT_005e5a40/70` lane-constant band.
   Port with intrinsics and keep the alignment guarantee, or the aligned moves fault.

**Verification remains the open problem.** veccap cannot take any of these shapes today
(4 flat-buffer signature kinds only); canonical-scenario runs are blocked by the pc=0x44
`.asi` crash. So even a correct port of these three lands UNVERIFIED unless veccap gains a
pointer-graph/frame signature kind first.
