# RESULT MATCH-RACEPATH-BIG2 (two big race-path functions by byte match), 2026-11-09

**Verdict: GREEN (progress, no close). 0x0046f6c0 44.3% -> 50.7% insn, 0x00469df0 30.1% -> 63.1% insn. Neither
is byte-identical, so nothing was promoted.**

Branch `orch/MATCH-RACEPATH-BIG2`, base `orch/integration` @ 3fbf77214. Every score comes from
`re/tools/match/match.py` (XP DDK cl 13.00.9176 / c2 13.00.9178) with each file's own line-1 FLAGS
(`/O2 /GS /Ob1`, unchanged). No `__asm`, no naked body, no pragma added. No mashedmod/ edit, no game run, no Ghidra
use, no tool code change. `grep -rn "0046f6c0.c\|00469df0.c" mashedmod/src`: 0 includes (only comments in
`D3d9Render/TrackRenderer.cpp:5996` and `Vehicle/VehiclePhysicsRun.cpp:1198` name the RVA), so the port does not
compile either source and the default gates are unaffected (not run).

| rva | before | after | source |
|---|---|---|---|
| 0x0046f6c0 | insn 44.3% (regs 53.4, ops 90.0), 885/893 ins, 3,544 B, 2,820 bad B | insn 50.7% (regs 58.5, ops 96.4), 895/893 ins, 3,582 B, 3,016 bad B | `re/match/s11/0046f6c0.c` |
| 0x0046c5f0 (same file) | 100%, 210 B BYTE-IDENTICAL, relocheck OK | unchanged: 100%, BYTE-IDENTICAL, relocheck OK (4 checked) | same |
| 0x00469df0 | insn 30.1% (regs 33.3, ops 70.7), 1081/1219 ins, 4,322 B, 3,652 bad B | insn 63.1% (regs 66.6, ops 92.4), 1204/1219 ins, 4,951 B, 3,695 bad B | `re/match/s11/00469df0.c` |

Bad bytes did not fall with the insn %: both symbols are now longer than before (closer to the original length),
so more bytes are compared, and the first difference is still at +0x2 (`sub esp, imm32`, the frame size). Until the
frame matches, every `[esp+x]` operand differs, so the byte count is not a progress measure yet. relocheck reports
BAD on both unclosed rows (operand offsets misaligned), as it did before. Commits: e7261788a (0x0046f6c0),
7af72a4ee, 27a3b99ae, b19cb0bc9, 897b7d93e (0x00469df0), plus the header-comment and evidence commit.

## 0x00469df0: structure first

1. **Extent.** pc_funcs gives 0x00469df0..0x0046b1b6 (5,060 B), one contiguous body. The 740 B shortfall was not a
   tail-jumped chunk: the original's last block (after the `ret`) is a second epilogue, `pop edi; fstp st(0); pop esi;
   fstp st(0); pop ebp; fstp st(0); xor eax, eax; pop ebx`, the `cnt == 0` return with the three sums still on the x87
   stack, reached by `je 0x46b1a4` right after the loop. Our source was missing code, so no extent override applies.
2. **Event record per branch (30.1 -> 37.1).** The original writes the whole record (type, time, who, hdr f00..f1c,
   the 16-entry ev loop) twice: the fall-through copy for `side[0] >= side[1]` and a second copy at the `jl 0x46afc4`
   target, both ending in the common `count++` / `return p3 == 0` (`je 0x46b18b` from the five status tests). With
   one shared tail the compiler kept `a`/`b` live to the end. With two copies `a`'s register is reused for
   `wb = a->f000` (`mov edi, dword ptr [edi]`), so the args now arrive in EAX/ECX and are copied to EDI/ESI exactly as
   in the original prologue (`mov esi, ecx` / `mov edi, eax`). This confirms the 004709a0 author's rule: a
   parameter whose register is reused later arrives in EAX/ECX.
3. **Hull loop (37.1 -> 48.1).** The original loads `c->n` into three x87 registers at the top of the loop, copies
   the twelve corner floats field by field (int moves), and builds each edge normal from three edge temporaries:
   two stored, one kept with `fst`. Written that way (`nx/ny/nz`, `ex/ey/ez`) the edge code is instruction-identical.
   The plane test computes all three differences first and then the dot (`fld n.z; fmul st(1)`, the last difference
   popped by `fstp st(0)` after `fnstsw`). Neither an inline function of scalars, explicit locals, nor nested ifs
   produce that (all forward-substituted, identical code). A difference **V3 returned by value** from an inline
   `Sub` and passed by value to an inline `Dot` does. An explicit V3 with a comma expression gives the same code.
4. **Point in `da` (48.1 -> 52.1).** The tested point sits in the slots `da` uses after the loop (0x04..0x0c in the
   original's frame), so the source reuses `da` (dead until `da.x = n3.x - pb->x`).
5. **After the loop (52.1 -> 54.4).** `if (cnt != 0) { ... } return 0;` puts the return block after the body.
   The scaled sums go into `n3` (its slots hold them until `rb.ev[0..2]`). Constant `600.0f` as a literal
   (0x005cea50: the original loads the field and multiplies by the constant, `fmul dword ptr [0x5cea50]`). Literals
   for 0x005d757c, 0x005cc320, 0x005cc32c, 0x005cc33c, 0x005cea4c, 0x005cd50c, 0x005ce268, 0x005cc328 are neutral
   (values read from the binary: 0.0, 1.0, 0.5, -1.0, 2e-07, -0.5, 0.0005, 0.01). 0x005cc568 (100.0) and 0x005ce194
   (200.0) stay globals: literals lose 3%. `FUN_004c39b0` returns float: the original pops ST0 after all six calls.
6. **Slots (54.4 -> 63.1).** Declaration order moves nothing at /O2 (three reorderings, identical bytes), but
   address-taken arrays are not slot-shared. The original keeps the hull corners at 0x70..0x98 and `da`/`db`
   contiguous at 0x04..0x18 for the whole frame. `V3 P[4]` (60.6) and `V3 dd[2]` for da/db (63.1) reproduce that.
   Tried and not kept: `N[4]` for the normals (58.0 / 58.2), `n3`/`n2` as a pair (63.7 insn but regs 66.6 -> 61.2
   and +7 bad bytes), ta/tb, ia/ib, ra as arrays (55.0 / 54.1 / 60.6).

**What remains (0x00469df0).** Frame `sub esp, 0x130` vs `0x164`: ours still overlaps `inv_m` (64 B, 0x124 in the
original) with dead loop slots. Hull-loop registers: the original has cnt in EDX and `o` in EBX, ours cnt in EBX and
`o` in EDX. `ta`/`tb`: the original copies `b->vel` x/y into x87 registers and z plus `a->vel` as int moves before
adding the spin terms. The explicit copy form (`ta.x = vb->x; ...`) reproduces that shape (ops 92.7) but scores
52.8 vs 54.4 on that base, so it is not applied. Re-test it once the frame matches. Next slice: the frame (find what
keeps `inv_m` unshared), then the ta/tb copies, then the register pair.

## 0x0046f6c0: what moved and what remains

Applied, all semantics-preserving (verified per edit: same values computed, the only effect is on locals' lifetimes
or on matching an operand form the original shows):

- `a = 0; b = 0;` hoisted to the top. The original zeroes EBP once (`xor ebp, ebp`), stores it to 0x0088e650 and to
  b's slot, keeps `a` in EBP, and spills `m` to a slot (`mov esi, dword ptr [esp + 0x18]` before the 0x004c3d90
  calls). Our `m` now goes to a slot too.
- `q.r` copied as an int (`mov ecx, dword ptr [edi + 0x4a4]` in the original).
- `i0 = 0` hoisted into the `state0 == 1` arm (`xor esi, esi` before the second test, then `jmp 0x46fd30` /
  `0x46fd35` / `0x46fd2b` into the shared index setup).
- `f > best` in the three abs-max tests (`fcomp st(1)` / `test ah, 0x41`).
- Tail loop: the tag block uses `u`. The 0.7 block computes the cross product into `axis` and scales by
  `f = car[0x9e4] * 1e-05f` afterwards (`fstp [esp+0xc]` then three `fmul dword ptr [esp + 0xc]`).
- `ang = (float)acos * 57.29578f` (`fmul dword ptr [0x5cc98c]` after `call 0x4a3384`, not a qword constant).
- The pos save around the 0x004c4d20 / 0x004c52f0 calls uses `u` (address-taken, so it stays in memory as in the
  original, not in ESI/EBX/EBP).
- Final block reuses `count`, and scales sx/S.y/S.z by `inv` before subtracting from car[0x958..0x960].
- The N dot product is written `z + (x + y)`, which evaluates `(x + y)` first as the original does. The value is
  the same as `x + y + z`.

**What remains (0x0046f6c0).** Frame `0x118` vs `0x110`. In the tail loop (top `0x470030`) the original holds 0 in
EBP (`mov dword ptr [esi - 0x70], ebp`, kept to the final `cmp dword ptr [edi + 0x198], ebp`) and reloads 1 into
ECX each iteration, while ours holds 1 in EBP and zeroes ESI after the loop. In the count==3 branch the original
reloads 1 into ECX (`mov ecx, 1` before `cmp eax, ecx`) and reuses it as `i1`. The classification loop uses a
shared `k*12` index (`[edx + 0x88e620]`, `[esp + edx + 0xac]`) where ours uses a table pointer. In the final
`c = S x D`, the original loads S.y first. The comma rvalue on either operand, operand swap, the global as an array,
and a local copy all compile to identical bytes or worse. The 0x9b4 `+ 0` copy is fld/fstp in the original and an
int move in ours: four spellings compile identically. The count<3 `N = W(0)->n` copy uses direct addressing in the
original, but the field-wise form scores lower. All of these are register-allocation or slot outcomes, not
statements.

## Method notes for the next slice

- `variants/mdiff.py` lists rows whose mnemonics differ (structure). `variants/slotuse.py` gives per-slot use counts
  for one side of a listing. It showed that MSVC 13.00 at /O2 shares slots between non-address-taken locals with
  disjoint lifetimes, and that address-taken arrays escape that sharing.
- `variants/tryv.py` (the pilot's) ran every variant. The specs are `variants/f6_v1..v6.py` and
  `variants/d9_v1..v15.py`, 40 + 55 compiles. Copies of the variant sources are not committed (scratch).
- Permuter not run: both functions were still structure-bound (ops < 97%), and statement swaps do not reach slots.

## Files

| file | what |
|---|---|
| `results.tsv` | per-function before/after, verdict, residual, compiles, sha256 (pilot columns) |
| `variants/*.py` | variant specs, `tryv.py`, `mdiff.py`, `slotmap.py`, `slotuse.py` |
