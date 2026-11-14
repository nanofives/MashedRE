# RESULT L156-ECX-556CA0 (run 46, MATCH section, static evidence row)

Branch `orch/L156-ECX-556CA0`, based on `orch/integration` 2c7f97e41. 2026-11-14. No build, no gate, nothing bound,
nothing decided. Image: `original/MASHED.exe.unpatched` (the SHA-256 anchor copy). Method: capstone (i386) through
`re/tools/disasm_va.py`, `re/tools/findoffset.py`, and a CFG walk written for this row, `ecx_livein.py` (this folder,
output `ecx_livein.txt`). Ghidra was not used.

**Question.** Does the tail-jump at 0x00556ca0 read ECX before writing it, counting the code it jumps to
through `[DAT_0067d838 + 0x138]` (U-5674)?

**Answer.** No, on every path from entry, for 0x00556ca0 itself and for both functions the original stores at
`+0x138` of the object kept in DAT_0067d838: 0x005547f0 and 0x00554940. ECX is fully written in each of them before
any instruction reads ECX, CX, CL or CH. Section 4 lists the limits.

## 1. Which function sits at [DAT_0067d838 + 0x138]

Chain from the only store that writes a value (U-5674: 0x00427d4a; the other writer, 0x00427632, stores 0):

```
0x00427d41  lea eax, [esp + 0x28]
0x00427d45  call 0x427880
0x00427d4a  mov dword ptr [0x67d838], eax          ; DAT_0067d838 = return of 0x00427880

0x00427880 .. 0x004278c2 (FUN_00427880)
0x00427889  call 0x4cc230
0x0042788e  mov esi, eax
0x00427895  je 0x4278be                            ; esi == 0 -> returns edi = 0
0x0042789f  call 0x4cc5e0
0x004278a9  je 0x4278be                            ; eax == 0 -> returns edi = 0
0x004278ab  push esi
0x004278ac  call 0x554390
0x004278b4  mov edi, eax
0x004278be  mov eax, edi                           ; return = return of 0x00554390, or 0

0x00554390 .. 0x005547e5 (FUN_00554390)
0x00554397  call 0x5551d0
0x0055439c  mov ebx, eax
0x005543a0  jne 0x5543aa                           ; ebx == 0 -> ret with eax = 0 (0x005543a2..0x005543a9)
  exit A:
0x0055466c  mov dword ptr [ebx + 0x138], 0x5547f0
0x00554676  mov eax, ebx
0x0055467d  ret
  exit B:
0x005547b8  mov dword ptr [ebx + 0x138], 0x554940
0x005547c2  jmp 0x5547cf
0x005547dc  mov eax, ebx
0x005547e5  ret
  exit C:
0x005547c4  push ebx / 0x005547c5 call 0x555830 / 0x005547cd xor ebx, ebx   ; returns 0
```

0x005551d0 (the allocator 0x00554390 calls first) also stores 0x00554940 there:
`0x00555261 mov dword ptr [esi + 0x138], 0x554940` (bytes `c7 86 38 01 00 00 40 49 55 00`), then
`0x0055526d mov eax, esi`, `0x00555271 ret`.

`findoffset.py --writes 0x138` (linear sweep of `.text`) lists 24 stores to a `+0x138` displacement. The only ones
that store a code address as an immediate are these four, and each stores 0x005547f0 or 0x00554940:

| store | function | value |
|---|---|---|
| 0x0055466c | 0x00554390 | 0x005547f0 |
| 0x005547b8 | 0x00554390 | 0x00554940 |
| 0x00555261 | 0x005551d0 | 0x00554940 |
| 0x00556c7b | 0x00556780 | 0x005547f0 |

The register stores to `+0x138` are in 0x00482c10, 0x0049d490, 0x00560260, 0x005bfe60, 0x005bfed0, 0x005c01f0 and
at 0x005c0b96 (no pc_funcs.csv entry). None of them appears on the chain above. See section 4 for this limit.

## 2. Disassembly excerpts, ECX only

### 0x00556ca0 (all 22 bytes)
```
0x00556ca0  8b 54 24 0c         mov edx, dword ptr [esp + 0xc]
0x00556ca4  8b 44 24 04         mov eax, dword ptr [esp + 4]
0x00556ca8  89 54 24 0c         mov dword ptr [esp + 0xc], edx
0x00556cac  89 44 24 04         mov dword ptr [esp + 4], eax
0x00556cb0  ff a0 38 01 00 00   jmp dword ptr [eax + 0x138]
```
No instruction reads or writes ECX. ECX at the jump equals ECX at entry.

### 0x005547f0 (FUN_005547f0, 0x005547f0..0x0055493c), entry to the first full write
```
0x005547f0  sub esp, 0xc
0x005547f3  push edi
0x005547f4  mov edi, dword ptr [esp + 0x14]
0x005547f8  test edi, edi
0x005547fa  je 0x554936          ; path P0
0x00554800  mov eax, dword ptr [edi + 0x134]
0x00554806  push eax
0x00554807  call 0x5c4d30        ; path P1
0x0055480c  mov cl, byte ptr [edi + 0x10]
...
0x0055481b  test al, cl
...
0x00554859  mov ecx, dword ptr [eax + 4]
```
Path P0: `0x00554936 mov eax, edi / 0x00554938 pop edi / 0x00554939 add esp, 0xc / 0x0055493c ret`. No ECX access.

Callee 0x005c4d30 (all of it):
```
0x005c4d30  mov eax, dword ptr [esp + 4]
0x005c4d34  mov ecx, dword ptr [eax + 4]      ; full write, first ECX access
0x005c4d37  test ecx, ecx
0x005c4d39  je 0x5c4d3e
0x005c4d3b  mov eax, dword ptr [eax]
0x005c4d3d  ret
0x005c4d3e  xor eax, eax
0x005c4d40  ret
```

### 0x00554940 (FUN_00554940, 0x00554940..0x005551ca), entry to the first full write
```
0x00554940  sub esp, 0xc8
0x00554946  mov eax, dword ptr [0x912be0]
0x0055494b  push ebx
0x0055494c  push ebp
0x0055494d  mov ebp, dword ptr [esp + 0xd4]
0x00554954  xor ebx, ebx
0x00554956  mov dword ptr [esp + 0x28], eax
0x0055495a  cmp ebp, ebx
0x0055495c  je 0x5551c0          ; path Q0
0x00554962  mov ecx, dword ptr [ebp + 0x134]   ; full write, first ECX access
```
Path Q0: `0x005551c0 mov eax, ebp / 0x005551c2 pop ebp / 0x005551c3 pop ebx / 0x005551c4 add esp, 0xc8 / 0x005551ca ret`.
No ECX access.

## 3. Per-address ECX table (every access before the first full write, on every path from entry)

| function | path | address | instruction | ECX access |
|---|---|---|---|---|
| 0x00556ca0 | only | (none) 0x00556ca0..0x00556cb0 | | none. Leaves through `jmp [eax+0x138]` at 0x00556cb0 |
| 0x005547f0 | P0 (edi == 0) | (none) 0x005547f0..0x005547fa, 0x00554936..0x0055493c | | none, ret |
| 0x005547f0 | P1 (edi != 0) | 0x00554807 -> 0x005c4d34 | `mov ecx, dword ptr [eax + 4]` | **write, full** (inside callee 0x005c4d30, before any read) |
| 0x005547f0 | P1 | 0x0055480c | `mov cl, byte ptr [edi + 0x10]` | write, CL (ECX already full-written at 0x005c4d34) |
| 0x005547f0 | P1 | 0x0055481b | `test al, cl` | read CL (value written at 0x0055480c) |
| 0x005547f0 | P1 | 0x00554859 | `mov ecx, dword ptr [eax + 4]` | write, full |
| 0x00554940 | Q0 (ebp == 0) | (none) 0x00554940..0x0055495c, 0x005551c0..0x005551ca | | none, ret |
| 0x00554940 | Q1 (ebp != 0) | 0x00554962 | `mov ecx, dword ptr [ebp + 0x134]` | **write, full**, first ECX access |

`ecx_livein.py` reports no `READ-before-full-write` event in 0x00556ca0, 0x005547f0 (107 instructions reached,
callee 0x005c4d30 walked) or 0x00554940 (584 instructions reached, no indirect jump, no call before 0x00554962).
Every later ECX write in both targets is listed in `ecx_livein.txt`.

## 4. The three verdicts restated (source: RESULT_L156-REGCONV-EVIDENCE.md on orch/L156-REGCONV-EVIDENCE @ 4d4bda020)

That RESULT's condition for these three units was "same if the `[font+0x138]` target ignores ECX". With the two
targets in section 1, ECX at 0x00556ca0 is not read before it is fully written on any path.

| unit | verdict (unchanged) | restated with this row |
|---|---|---|
| 0x00427f00 | **NOT-PROVEN** | The vector compare's only difference (call 7 to 0x00556ca0, live-in ECX) is in a register that neither 0x00556ca0 nor 0x005547f0 / 0x00554940 reads before writing. With ECX at 0x00556ca0 excluded the compare is same 300/300 at CW 0x037f and 0x007f. Still NOT-PROVEN: equiv.py cannot pair the bodies (`call-sequence 7 vs 8 calls`). |
| 0x00427ff0 | **NOT-PROVEN** | Same reading for call 11 to 0x00556ca0. Excluded: same 300/300 at both CWs. equiv.py: `call-sequence 12 vs 13 calls`. |
| 0x00428140 | **NOT-PROVEN** | Same reading for call 8 to 0x00556ca0. Excluded: same 300/300 at both CWs. equiv.py: `call-regargs ... call 0x4277a0`. |

No unit becomes EQUIV or DIFFERS. This row decides nothing about L-156.

## 5. [UNCERTAIN]

* **Writer set of +0x138.** The target set {0x005547f0, 0x00554940} comes from a linear displacement sweep
  (`findoffset.py`) plus the chain 0x00427d4a -> 0x00427880 -> 0x00554390 -> 0x005551d0. Not covered: writes through
  a computed base, block copies (`rep movs`, memcpy) and the register stores in section 1 that come after creation.
  Missing evidence: a write watch on `[DAT_0067d838]+0x138` during a run (`--watch-write`), or a reader check of
  those seven register-store sites.
* **ECX after the dispatch returns.** This row covers ECX into 0x00556ca0. Whether each unit reads ECX after the
  call returns is not covered here. The vector compare covered it for the paths its vectors reached.
* **/GS-.** match.py's vc19 toolchain compiles with `/GS-`. The exe builds these TUs with the default `/GS`.
  [UNCERTAIN] that the exe's /GS code changes no value (unchanged from the source RESULT, section 3).
* **Stubbed callees.** Callees outside unit + helpers are stubbed the same way on both sides of the vector compare.
  The exe binds some to port bodies (FUN_00427780, FUN_00552840, raw forwarders for 0x00552d10 / 0x00552750 /
  0x00552d70 / 0x00556ca0 / 0x00556e90). Not tested against the original (unchanged).
* **PK_458570 (0x00458920's helper).** The exe's adapter calls the naked PK_458570, and the harness ran the
  original 0x00458570 on both sides. [UNCERTAIN] that PK_458570 is byte-equal to 0x00458570 (unchanged).
* **Random-vector coverage** of the source RESULT: paths its inputs did not reach are not covered (unchanged).

## 6. Files

`ecx_livein.py` (CFG walk: per-instruction "ECX fully written on every path" state, AND-merged, direct callees
walked up to depth 5, xor/sub reg,reg treated as write only), `ecx_livein.txt` (its output for 0x00556ca0, 0x005547f0,
0x00554940). Rerun: `py -3.12 verify/l156_20261114/ecx_livein.py 0x556ca0 0x5547f0 0x554940 --depth 5`.
No source, tracker or `original/` file changed.

## 7. DECISIONS.md

log/ is gitignored (`.gitignore:16`), so the entry was appended to the L-156 row of the live
`log/orch/DECISIONS.md` in the main checkout, as the source row did. The same text:

> **Evidence 2026-11-14 (L156-ECX-556CA0, verify/l156_20261114/RESULT_L156-ECX-556CA0.md on orch/L156-ECX-556CA0):**
> 0x00556ca0 does not read ECX (5 instructions, ends `jmp [eax+0x138]` at 0x00556cb0). DAT_0067d838 = return of
> 0x00554390 (via 0x00427d4a <- 0x00427880 <- 0x004278ac), whose +0x138 is 0x005547f0 (0x0055466c) or 0x00554940
> (0x005547b8, 0x00555261). 0x005547f0 fully writes ECX at 0x005c4d34 (callee 0x005c4d30) and 0x00554940 at 0x00554962,
> each before any ECX read, and the early-ret paths do not touch ECX. So the ECX difference at 0x00556ca0 for
> 0x00427f00 / 0x00427ff0 / 0x00428140 is not read on these targets. Verdicts stay NOT-PROVEN. [UNCERTAIN]: writer set of
> +0x138 (linear sweep, register stores not traced), /GS-, stubbed callees, PK_458570.
