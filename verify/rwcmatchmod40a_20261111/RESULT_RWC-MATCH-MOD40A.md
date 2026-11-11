# RESULT RWC-MATCH-MOD40A: the RW module 0x40a constructor chain (L-124 (C))

Verdict: **GREEN, partial.** Of the three named functions, 0x004d4dd0 is byte-identical under rw9044 and C4 by match.
0x004d4380 and 0x004d4f90 are typed rewrites that follow the original branch by branch. They stay below 100%, and
equiv.py reports NOT-PROVEN for both, so they are **provisional-body candidates** (L-126), not admitted and not C4.
The typed 0x004d4380 builds the 0x004d4bf0 sort context `{pipeline, next = 0}` as one struct, the way the original
does. The raw form wrote `next` to a separate local that the compiler can drop as a dead store. That uninitialised
read is the fault RWC-CORE-RUN died on (section 3).

Branch `orch/RWC-MATCH-MOD40A`, based on 9e36314ac (MATCH-RW-COMPILER-9044: toolchain rw9044). `orch/integration`
(df08d4cea) did not contain 9e36314ac when this row started. Match-only row: no build, no gates.

## 1. Per-function status (for the RW-CORE lead)

| rva | size | best score | toolchain | other toolchain | equiv.py | status | residual |
|---|---|---|---|---|---|---|---|
| 0x004d4dd0 | 291 B | **100.0%**, byte-identical, relocheck OK (6/0) | rw9044 | ddk 77.2% | not needed | **C4 by match** (c4_reclass) | none |
| 0x004d4f90 | 508 B | 47.8% (regs 63.2, ops 84.6, 175/182 ins) | rw9044 | ddk 28.6% | NOT-PROVEN (call-memory at the 0x004d5190 call) | provisional-body candidate | register allocation: the original keeps `pipeline` in ebp and spills the node-offset IV to [esp+0x14]. This source gets the IV in ebp and `pipeline` in memory. The va cursor `&nodeDef0` escapes into the argument area, so equiv falls back to strict memory and the frame slot differences decide it |
| 0x004d4380 | 1,412 B (0x00..0x583) | 37.5% (regs 68.9, ops 86.9, 490/518 ins) | rw9044 | ddk 35.1% | NOT-PROVEN (call-regargs at the 0x004d4910 call, 0x004d44c5) | provisional-body candidate | register rotation (original pipeline ebx / zero edi / numNodes esi, this source esi / ebx / edi), the 0x22 and 0x1c error tails cross-jumped here but kept apart in the original, order of the scratch-size arithmetic |
| 0x004d4bf0 | | 100% (unchanged, s21) | ddk and rw9044 both 100% | | | C4 already | none |
| 0x004e18f0 | | 100% (unchanged) | ddk and rw9044 both 100% | | | | none |

Before this row: 0x004d4380 17.2% (raw Ghidra form), 0x004d4f90 28.3%, 0x004d4dd0 63.2%, all under ddk.

Toolchain per function. The chain is one object (module 0x40a, consecutive RVAs). Its neighbours 0x004d4250 /
0x004d42b0 / 0x004d4310 (s21) became byte-identical only under rw9044 in MATCH-RW-COMPILER-9044 and carry
`// TOOLCHAIN rw9044`. Every source here scores higher under rw9044 than under ddk, and 0x004d4dd0 matches only under
rw9044. All three sources now carry `// TOOLCHAIN rw9044`.

equiv.py: tool commit 9e36314ac (the worktree base), output `equiv.txt`. `orch/EQUIV-CALLEE-POPS` (A6, __stdcall COM
pops) is **not** on orch/integration, so the run used the version on this base. Neither residual involves a
__stdcall callee: 0x004d4f90 stops on frame memory at a cdecl call, 0x004d4380 on register state at a cdecl call.

C++ check: each source compiles as C++ inside `extern "C" { #include ... }` with VS2022 `cl /TP /Zs` and no error,
which is how `gen_rwcore_tus.py` (RWC-CORE-RUN) builds them. The new signatures are typed structs instead of
`int *`. Under `extern "C"` there is no mangling, so the existing `#define` callee bindings still link.

## 2. What each typed source does (checked against the original disassembly)

### 0x004d4dd0 (C4)
Header {+0 state, +4 count, +8 entries (0x28 each), +0x20 block, +0x24 capacity}. When state is 0: size =
[RwGlobals + module offset 0x00911ae0 + 0x38] * 0xb4. With entries, it grows through 0x004d4910 when size > capacity,
then 0x004d4a20. Without entries, it allocates through RwGlobals+0x108 (tag 0x1030409) and reports error 0x80000013
(code, size) on NULL. Then state = 1, and per entry: call obj+4+0x10 (entry), decrement obj+0x3c, call obj+4+8 (obj)
at 0, clear entry+0xc. Levers that closed it: the error as a 2-word struct, `size > cap`, a `Vt *` local taken from
`&arr[i].obj->vt`, and the global operand order `7d3ff8 + 911ae0 + 0x38`.

### 0x004d4f90
`(pipeline, uint *firstIndex, nodeDef0, ...)`. Errors: NULL pipeline 0x80000016 (0x004d5135), not locked 0x34
(0x004d5162), capacity 0x2a (0x004d4ff4, cross-jumped into the 0x34 tail). It counts the NULL-terminated list, then
creates each node at `nodes[oldNum + i]` through 0x004d5190. From the second node on, it links the previous node's
output 0 to the new node, with the original's inlined checks: previous node's nodeDef and numOutputs > 0
(0x004d5040..0x004d5053); new node non-NULL with nodeDef (0x004d5031..0x004d503e); locked, out != NULL, *out == -1,
`idx = (dest - nodes) / 0x28` signed, `&nodes[idx] == dest`, `idx < numNodes`, `idx != -1` (0x004d5061..0x004d50a3);
then `*out = idx` (0x004d50ad). On a failed link it calls 0x004d3e90(node, pipeline) and breaks. When every node was
added, it writes oldNum to firstIndex and returns pipeline. Otherwise it unwinds the i added nodes from the top
(0x004d5106..0x004d5129) and returns NULL.

### 0x004d4380
NULL 0x80000016 (0x004d48ad). Not locked 0x34 (0x004d48da). numNodes 0 -> clear locked, return pipeline
(0x004d47f2). The steps after that:

1. Entry check: entry >= numNodes or nodes[entry].nodeDef NULL -> error 0x24 (0x004d4883).
2. Count the distinct cluster ids over every nodeDef's 12-byte refs at +0x24 / +0x20 (0x004d43ca..0x004d4414,
   unsigned `> last` and `< min`).
3. Scratch size (0x004d4416..0x004d44be): max(maxNodes * 0xb4, the sum). Grow through 0x004d4910 when it exceeds
   +0x24 (0x004d44c0).
4. Set scratch pointers 0x007d4714 = mem + total and 0x007d4718 = 0.
5. Clear each node's count block +0x1c (b, then a).
6. Count inputs through outputs != -1.
7. Errors: entry has inputs -> 0x24; a non-entry node with no inputs -> 0x22 (0x004d45be).
8. **0x004d4bf0(&sorter, entry)** (section 3).
9. Every count block must have a == b, else error 0x1c (0x004d45e8). Then entry = 0.
10. Move the count blocks from the lock block (`nodes + maxNodes*0x28 + maxNodes*0x80`) to the top of the scratch,
    walking downwards (0x004d4612..0x004d4690).
11. Pack each node's outputs after the used nodes with `rep movsd`, NULL outputs for 0 outputs
    (0x004d4694..0x004d4703).
12. Publish 0x007d4714 / 0x007d4718 (0x004d470e / 0x004d4717).
13. 0x004e2090(pipeline) nonzero -> return NULL. Shrink through 0x004d4910(pipeline, 0x007d4718 - mem); failure ->
    0x004d4a20(pipeline, pipeline), return NULL (0x004d486f).
14. Clear +0x1c.
15. Node init in reverse: refCount++, nodeDef+8 on the first reference, nodeDef+0x10(node). On failure undo the
    nodes above (0x004d4818: done = numNodes - k - 1).
16. Config in reverse: nodeDef+0x18(node, pipeline). On failure undo all (0x004d4826).
17. Undo loop: nodeDef+0x14(node), then refCount--, nodeDef+0xc at 0 (0x004d482b..0x004d486d).
18. Success: locked = 0, return pipeline.

## 3. The 0x004d4bf0 fault in RWC-CORE-RUN

0x004d4bf0 reads its first argument as `{Graph *graph; int next;}` (`re/match/s21/004d4bf0.c`, 100%): it starts
with `cur = s->next` and indexes `nodes[cur]`. In the original, 0x004d4380 builds that pair on the stack right before
the count loops:

```
004d44f4  mov dword ptr [esp + 0x10], ebx      ; sorter.graph = pipeline
004d44f8  mov dword ptr [esp + 0x14], edi      ; sorter.next  = 0   (edi = 0 since 0x004d44da xor edi, edi)
...
004d458e  lea eax, [esp + 0x10]
004d4592  push edx                             ; entry index
004d4593  push eax                             ; &sorter
004d4594  call 0x4d4bf0
```

The raw source RWC-CORE-RUN ran wrote `local_14 = piVar4; local_10 = 0;` and passed `&local_14`. Nothing reads
`local_10` in that function, and only the address of `local_14` escapes, so the compiler may drop the store or place
`local_10` anywhere. 0x004d4bf0 then reads `s->next` from whatever word follows `local_14`. The typed source declares
`Sorter4380 sorter` (block scope, the slot sharing of the original), sets `sorter.pipe = pipeline; sorter.next = 0;`
and calls `FUN_004d4bf0(&sorter, pipeline->entry)`. So the typed source initialises `next` as the original does.
[UNCERTAIN] that this was the only cause of `AV ... read 6E275940`. The RWC-CORE-RUN logger recorded only the fault
and the stack. The rerun is the check.

## 4. For the RWC-CORE-RUN restart

- Regenerate the RwCore TUs from this branch's sources (`gen_rwcore_tus.py`), or rebase RWC-CORE-RUN on it.
  0x004d4dd0 moves from provisional to match100. 0x004d4380 and 0x004d4f90 stay provisional, now with the typed
  bodies. Update their `PROVISIONAL_RWC.tsv` lines to the new scores (37.5 / 47.8, toolchain rw9044).
- The object build: `ddkobj.py` honours `// TOOLCHAIN rw9044`. 0x004d4dd0 is 100%, so the object build will
  substitute it and `ddkobj.py check` must report it IDENTICAL. That is the RW-CORE row's check (not run here).
- `re/analysis/match_ledger.tsv` still lists 17.2 / 63.2 / 28.3 under ddk. This row does not write the ledger
  (MATCH-LEDGER-REFRESH does).

## 5. Promotion

`c4_reclass.py batch --tsv c4_batch.tsv`: dry run `[1/1] 0x004d4dd0 match PASS byte-identical 291 B, relocheck OK
(6/0)`, then `--apply --result` this file (`c4_apply.tsv`). hooks.csv 0x004d4dd0 C2 -> C4, CHANGELOG line written by
the tool. No other tracker edit.

## 6. Shaping log (what moved, what did not)

- 0x004d4dd0: raw g2s 63.2 (ddk) / 69.3 (rw9044) -> typed structs 99.1 -> operand order 100.
- 0x004d4f90: raw 28.3 -> typed nested 49.7 -> capacity error as the inline arm 47.8 (fewer instructions,
  175/182). No change in score from: int vs uint counters, one va_list, index variable, pointer node, for vs
  do-while, a local copy of `pipeline`, macro vs inline helpers, named out/dest locals, flags (/Oy-, /G6, /Ox, /O1,
  /Gy). The 48.x plateau is the ebp choice.
- 0x004d4380: raw 17.2 -> typed 14.5 (ops 85.7) -> else-arm nesting of the NULL / 0x34 / entry errors 31.1 -> block
  scope sorter (frame 0x14 like the original) 34.4 -> pointer-walk count loops 37.5. A reordered size computation
  raised ops to 92.0% but grew the frame to 0x18 (23.7%), so it was not kept. Stopped at the lead's timebox.

## Files

- `re/match/rwl01/004d4dd0.c`, `004d4380.c`, `004d4f90.c` (rewritten, `// TOOLCHAIN rw9044`).
- `verify/rwcmatchmod40a_20261111/`: this RESULT, `equiv.txt`, `equiv_toolcommit.txt`, `c4_batch.tsv`,
  `c4_dryrun.tsv`, `c4_apply.tsv`.
