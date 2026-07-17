# B5e solver island — vtable / function-pointer dispatch resolution

Date: 2026-07-15. Source: Mashed_pool0 `MASHED.exe` (read-only), image base 0x400000.
Scope: the 23 island functions with indirect calls (see task list); companion machine-readable
table: `island_vtable_targets.tsv` (one row per table slot; 104 rows, 78 unique target RVAs).

Method note: every table address, install site, and slot value below was read from the binary
(`memory_read`) or from Ghidra reference records (`reference_to` / instruction listing). Sizes
marked `span-est` are ADDRESS-DELTA UPPER BOUNDS to the next known entry/function start (the
targets are not defined as functions in the Ghidra project — they are exactly the static-walk
orphans this task was hunting); they are not verified function extents.

---

## 1. Dispatch systems found (4 real table families + 1 internal + 1 callback lane)

### 1.1 RWP volume descriptor tables (`*(shape+0x5c)` dispatch) — 9 tables

Every shape carries a pointer at +0x5c to a per-volume-type descriptor:
`+0x00` dword type id (read as short by FUN_0057a660 @0x57a68x, FUN_0057a9a0 @0x57a9ca),
`+0x04..+0x28` ten function-pointer slots, `+0x2c/+0x30` zero, `+0x34` ptr to SCM `$Id:` string.
Descriptors sit in `.data` 0x5e4f50..0x5e5e50 between `RwpXxx.c` SCM stamps (dumped 0x5e4e00..0x5e5f00).

Island call sites and the slot they hit:
- `+0x08`: FUN_0055bae0 @0055bae0 line 19
- `+0x10`: FUN_0055bd70 @0055bd7x, FUN_0055bd80 @0055bd9x, FUN_00575c60 (via obj[0x17]=+0x5c, call `(**(code**)(iVar6+0x10))`), FUN_00575fe0 (same pattern; this function also INSTALLS the Triangle descriptor at 0x00576344)
- `+0x14`: FUN_0055c000 (GJK support map)
- `+0x18`: FUN_0055c230
- `+0x1c`: FUN_00578610 (two sites: `(**(code**)(iVar7+0x1c))` line 92 and `(**(code**)(*(int*)(uStack_128+0x5c)+0x1c))` line 95)
- `+0x20`: FUN_0055c2d0
- `+0x24`: FUN_00561390 line 106, FUN_005729a0 lines 100/104

| type | id | table base | installed at (write site) | +04 | +08 | +0c | +10 | +14 | +18 | +1c | +20 | +24 | +28 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| Sphere | 2 | 0x005e4f50 | 0x0055c545 (FUN_0055c540) | 4d8560 | 55c560 | 55c580 | 55c5d0 | 55c670 | 55c680 | 4d8560 | 55c6c0 | 57b9a0 | 55c6e0 |
| Box | 5 | 0x005e4fe0 | 0x0055c815 (FUN_0055c810) | 4d8560 | 55c840 | 55c890 | 55cbf0 | 55cd40 | 55cdd0 | 55d1b0 | 55d2e0 | 55d270 | 55d330 |
| Cylinder | 6 | 0x005e51c8 | 0x0055e445 (FUN_0055e440) | 4d8560 | 55e470 | 55e4a0 | 55e510 | 55e6b0 | 55e7f0 | 5c9d00 | 55ebc0 | 55eb90 | 55ec20 |
| Trilist | 9 | 0x005e52a8 | 0x00562611 (FUN_00562600) | 562640 | 562660 | 562770 | 5628a0 | 562a10 | 5c9d00 | 5c9d00 | 562a60 | 562a20 | 5c9d00 |
| Capsule | 3 | 0x005e5338 | 0x00562fd5 (FUN_00562fd0) | 4d8560 | 563000 | 563020 | 563090 | 563140 | 563180 | 563270 | 5632a0 | 57b9a0 | 5632d0 |
| Grid | 10 | 0x005e54c0 | NONE (0 refs — dead) | 5668e0 | 5669b0 | 5669e0 | 566a60 | 562a10 | 5c9d00 | 5c9d00 | 5c9d00 | 566aa0 | 5c9d00 |
| Aggregate | 8 | 0x005e5900 | NONE (0 refs — dead) | 574110 | 574130 | 574170 | 5741e0 | 574220 | 5c9d00 | 5c9d00 | 5742a0 | 574230 | 5742b0 |
| Triangle | 4 | 0x005e5db0 | 0x00576344 (inside island FUN_00575fe0) | 4d8560 | 57ae30 | 57af90 | 57b000 | 57b340 | 57b420 | 57b750 | 57b9b0 | 57b9a0 | 57ba90 |
| Null | 0 | 0x005e5e50 | 0x0057c1b5 (FUN_0057c1b0) | 4d8560 | 5c9d00 | 57c1d0 | 5c9d00 | 5c9d00 | 5c9d00 | 5c9d00 | 5c9d00 | 57b9a0 | 5c9d00 |

Shared stock entries: `0x004d8560` (5-byte defined leaf), `0x005c9d00` (2-byte defined leaf; RET-class no-op).
Grid (id 10) and Aggregate (id 8) descriptors have ZERO references (data refs and instruction-text
search for `0x5e54c0` / `0x5e5900` both empty) — dead volume types in this build; excluded from the
must-port set but listed in the tsv.

### 1.2 RWP object-type table `rwpOBJTYPEBODY` (`*obj` dispatch on registered bodies)

Objects walked by FUN_0055a1f0 (calls `obj[0]+0x10` @0055a1f0 line 223, `obj[0]+0x14` line 189),
FUN_0055e050 (`obj[0]+0x18`), FUN_00561390 (`obj[0]+0x1c` at lines 198 and 320, then FUN_0055e050)
are RWP BODY objects: created by FUN_0057c300, which allocates 0x34 bytes and writes
`MOV [ESI],0x62403c` at **0x0057c31a**. Table at **0x0062403c**: `+0x04`=0x62405c → string
"rwpOBJTYPEBODY" (0x0062405c), `+0x08`=0xffffffff, and:

| slot | static value | live value | note |
|---|---|---|---|
| +0x10 | 0x0057c3a0 | 0x0057c3a0 | called by FUN_0055a1f0 (wake gate, returns int) |
| +0x14 | 0x0057c3f0 | 0x0057c3f0 | called by FUN_0055a1f0 |
| +0x18 | 0x0057c590 | 0x0057c590 | called by FUN_0055e050 (fills 32-byte stack buf) |
| +0x1c | 0x0057c5a0 | **0x0057c2b0** | `PTR_LAB_00624058 = FUN_0057c2b0` overwritten in FUN_0057c270 (the RpAtomicRegisterPlugin(4,0x901,…) registrar that also sets plugin offset DAT_007dc8d8); called by FUN_00561390 |

Registration path into the island's manager arrays: FUN_0055ab30 external callers =
FUN_0047d180, FUN_0047d240, FUN_00480100 (game-side build/warp chain; each does
`iVar2 = FUN_0057c210((&DAT_006c71d8)[i])` = atomic + plugin offset DAT_007dc8d8, then
`FUN_0055ab30(*(iVar2+0x24), iVar2, FUN_0057c420(...))`). EXTERNAL-SUPPLIED entry, but the
dispatched-on objects are RWP bodies with the single table above.

Second object type `rwpOBJTYPERAGDOLL`, table at **0x00632b68** (+0x10..+0x1c =
0x5a5950 / 0x5a5b30 / 0x5a5e50 / 0x5a5e60): **0 references to the table base — never installed
in this build**; excluded from the must-port set, listed in tsv for completeness.

### 1.3 Scene-constructor callback fields (FUN_00560260's `code*` params)

FUN_00560260 (sole caller FUN_00561040, in island) receives `param_11 = *(scene+0xf4)`,
`param_12 = *(scene+0xf8)`, `param_13 = *(scene+0x404)` (call at FUN_00561040.c line 90-94).
These fields are installed by the scene constructor FUN_0055f800 (body 0x55f800..0x55fdcd):

- 0x0055fd8e: `[ESI+0x404] = 0x0056b310` (always)
- branch on `CMP [ESI+0x58],2` @0x0055fd84:
  - equal: 0x0055fd9c `[ESI+0xf4] = 0x0056a450`; 0x0055fda6 `[ESI+0xf8] = 0x0056adb0`
  - else:  0x0055fdb5 `[ESI+0xf4] = 0x00569140`; 0x0055fdbf `[ESI+0xf8] = 0x005697f0`

`(*param_11)()` @FUN_00560260.c line 384; `(*param_13)(…)` lines 445/486. param_12 (+0xf8) is
paired in the constructor and passed at the same call — treated as a function pointer target set
too ([UNCERTAIN] whether FUN_00560260 invokes it on a path the grep missed; include in port set).

### 1.4 RwMatrix module dispatch (FUN_004c4600 / FUN_004c52f0)

`(**(code **)(DAT_007d4028 + 8 + DAT_007d3ff8))(…)` = RW module-globals pattern:
DAT_007d3ff8 = engine-instance module-state base (written 0x004c3073/0x004c30c1/0x004c312b/
0x004c31c7/0x004c31e9/0x004c32b7 — RwEngine open band), DAT_007d4028 = matrix module offset
(single WRITE at 0x004c4483, matrix-module open at LAB_004c4470). The `+8` slot is written
exactly once: **0x004c44f1 `MOV [EAX+ECX*1+0x8],0x4c40e0`**. No other writer of that slot exists
among all 17 DAT_007d4028 readers (all in 0x4c40xx..0x4c57xx). Target = **0x004c40e0**
(default matrix-multiply; not defined as a function in Ghidra; span-est 160 bytes to LAB_004c4180).

### 1.5 FUN_00546b10 — internal static selection (no new targets)

`pcVar2` is set only to FUN_00546c50 (line 27) or FUN_00546cb0 (line 34); both already in the
137-island set. Resolved, nothing new.

### 1.6 Non-calls among the 23 flagged functions

FUN_0055fe50, FUN_00561280, FUN_0056f350, FUN_0057a660, FUN_0057a9a0, FUN_004c51a0-band:
their "indirect" text is data-pointer arithmetic or wrappers that reach the shape table via
FUN_0055bd70 / FUN_0055c000 — no additional dispatch sites beyond 1.1.

---

## 2. NEW functions beyond the 137 (dispatch-target set)

**All 78 unique target RVAs are outside the 137-function island** (compared against
`island_dag.tsv`). Of these, **48 unique targets are reachable from island call-site slots**
(volume slots +0x08/+0x10/+0x14/+0x18/+0x1c/+0x20/+0x24, body slots +0x10..+0x1c incl. the
runtime 0x57c2b0 override, scene callbacks, matrix +0x8), totaling **~18.0 KB**
(span-estimate upper bound; defined-function sizes exact). Full 78-target total ~26.1 KB.
Per-row size/slot/table data: `island_vtable_targets.tsv`.

Ghidra-defined targets (exact size, static callees checked):
| RVA | size | callees outside island |
|---|---|---|
| FUN_004d8560 | 0x5 | (leaf) |
| FUN_005c9d00 | 0x2 | (leaf) |
| FUN_0055cd40 | 0x81 | none |
| FUN_00562660 | 0x10d | FUN_004e5fc0, FUN_00565cd0 |
| FUN_005668e0 | 0xc2 | FUN_0055bad0, FUN_00564190 (Grid — dead table) |
| FUN_0057ae30 | 0x15b | none |
| FUN_0057c2b0 | 0x24 | FUN_004c0e50, FUN_0057c440 |
| FUN_0056a450 | 0x343 | none |
| FUN_0056adb0 | 0x559 | FUN_0056aae0, FUN_0056ac40 |

The remaining ~69 targets are NOT defined as functions in the Ghidra project (raw undisassembled
bytes — e.g. 0x57c3a0 is plain code starting `PUSH EBX/PUSH ESI` but undefined). Their static
callees cannot be enumerated read-only; sizes in the tsv are `span-est` upper bounds. A
ghidra-sweep leg (write session) should seed-disassemble these 69 and re-run callee closure.

---

## 3. [UNCERTAIN] items

1. **Undefined-target extents/callees**: ~69 targets lack Ghidra function bodies. Missing
   evidence: disassembly (needs a WRITE session on the master/sweep project). tsv rows flagged
   `span-est`.
2. **param_12 (+0xf8, targets 0x56adb0/0x5697f0)**: passed to FUN_00560260 alongside the two
   proven `code*` params and installed pairwise in the ctor; the decomp grep shows no `(*param_12)`
   call line. Missing evidence: full re-read of FUN_00560260 decomp for a masked call site.
3. **Descriptor tail fields** (+0x34 onward: floats/flags such as Triangle +0x38 bytes
   `00 00 02 03`, flag byte read at desc+0x40 by FUN_0055bd80/FUN_00575c60): values dumped, field
   semantics not assigned (raw in memory dump 0x5e4e00..0x5e5f00).
4. **Game-side class vtables 0x5cf9e8 / 0x5cfd70 / 0x5d0d48 / 0x5d0dc0 / 0x5d0eb0 / 0x5d0f74 /
   0x5d0f94** (installed at obj[0] by ctors 0x493c1c/0x494adc/0x49cca7/0x49dda4/0x49ebc0/
   0x49f0bd/0x49f210): examined while hunting the body vtable; **no evidence these objects enter
   the island's manager arrays** (bodies proved to use 0x62403c instead). Recorded here only so
   the next session doesn't re-chase them; NOT in the target set.
5. **Batch continuation-address table** near 0x5e5ab8 (`ffffffff 7e495700 84495700 …` = code
   addresses 0x57497e/0x574984/0x5749fd/0x574a03 inside the RwpBatch band): observed in the
   region dump; no island decomp line ties a read to it. Missing evidence: a reference from an
   island function.
