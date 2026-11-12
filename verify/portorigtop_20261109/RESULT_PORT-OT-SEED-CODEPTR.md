# RESULT PORT-OT-SEED-CODEPTR (L-111 (a), follows PORT-OT-SEED-2): the 22 code-pointer seed entries, bound

Verdict: **GREEN.** All 22 entries PORT-OT-SEED-2 left out (`SEED2_ADDED.tsv` action `code-pointer`) are bound. That
is 188 code-pointer slots and 72 data seeds. With `MASHED_ORIG_TOP=1 MASHED_OT_CODEPTR=1` the self-test passes
260 of 260 checks on both builds. The binding sits behind a **sub-knob, `MASHED_OT_CODEPTR`, default OFF**, for one
reason: when it is ON, the knob boot reaches original code the exe has no body for (section 4). With the sub-knob off,
the `MASHED_ORIG_TOP` knob boot runs its 600 frames as before. The default path never calls the code.

Branch `orch/PORT-OT-SEED-CODEPTR` from orch/integration `275dec120`:
- `8fd9128fe`: table, generator, code. Its message says 11 bound and 177 stops. The actual split is 10 and 178, as
  logged in section 3.
- `bc9b3b352`: the sub-knob, the log hold, and the return address in the STOP line.
- this RESULT.

Gates: G-CTRL / B / 12 captures not run. This is a top-down row (WORKER_RULES, L-127): the new code runs only under
`MASHED_ORIG_TOP=1` with `MASHED_OT_CODEPTR=1`. The default exe still links one new TU and one new no-op call
(`PortTopCodePtr_Log`, which writes nothing when no line is held), so code layout moves. The merge train's default
gates cover that (memory `sim-trajectory-is-code-layout-sensitive`).

## 1. What an entry holds

`verify/portorigtop_20261109/codeptr_inventory.py` reads `original\MASHED.exe.unpatched` and walks each entry:
- **Range:** the entry's own size, extended while the next dword still points into `.text` and no other inventory
  label starts there. The cap is 64 dwords.
- **Slots:** every dword that points into `.text` (0x00401000..0x005ca8e0) is a slot.
- **Data:** every other dword inside the entry's own size is data. A data dword that points at a NUL-terminated
  printable string in a file-backed section brings that string along, as PORT-OT-SEED-2 did.

Output:
- `CODEPTR_SLOTS.tsv`: one line per slot (entry, slot, original target, target name, BINDINGS kind, exe symbol, reading
  rows).
- `mashedmod/src/mashed_re/Compat/PortTopCodePtr.inc` (generated, `--write`).

| entry | slots | what it is |
|---|---|---|
| 0x005cf9d0 / 0x005cfd30 / 0x005cfd70 / 0x005d0e74 / 0x005d0e88 | 5 / 15 / 49 / 5 / 9 | addresses 0x00494ac0 stores into the object it builds: `*param_1 = &PTR_LAB_005cfd70`, `param_1[3]`, `[4]`, `[0x38]`, `[0x39]` (re/match/s16/00494ac0.c) |
| 0x005f9998 (32 B) | 7 + 1 data dword (`9`) | the 8 dwords before 0x005f99b8. 0x005f99ac / 0x005f99b0 / 0x005f99b4 are inside it |
| 0x005f99bc, 0x005f99c0 | 1, 4 | single pointers (thunk_FUN_00456c70 = 0x00456c90; 0x0045b350 x3, 0x00456740) |
| 0x006147dc | 1 | `PTR_FUN_006147dc` = 0x00493b50 |
| 0x00632c28 .. 0x00632c4c (6 entries) | 10 | audio handler pointers (LAB_ targets 0x005a8ea0 .. 0x005aafa0) |
| 0x00634550 (408 B) | 51 + 51 data dwords + 51 strings | (string pointer, handler) pairs. The strings are 0x005e6a68 onward in .rdata |
| 0x00635194 .. 0x006351fc (6 entries) | 30 | audio handler pointers (LAB_ targets 0x005a7fe0 .. 0x005c92a0) |

There are 138 distinct targets. Only 4 have an exe body that `verify/porttop_20261108/BINDINGS.tsv` records. Those 4
cover 10 slots:

| slots | target | exe symbol |
|---|---|---|
| 0x005cfd88 | 0x004d8560 (`mov eax,1; ret`) | `UtilReturnOne4d8560` |
| 0x005cfd90, 0x005cfde4 | 0x005c9d00 (`xor eax,eax; ret`) | `GetRaceEndTrigger` (Match/PortBatch_GAMEPLAY_005c9d00.cpp) |
| 0x005cfdb8 / bc / c0, 0x005f99c0 / c4 / c8 | 0x0045b350 (bare `ret`) | `RwInitNullStub` (Match/MatchS09_0045b350.cpp) |
| 0x005f99ac | 0x00456760 | `Orig_00456760` (Match/PortTop_00456760.cpp) |

The first three originals end in a plain `ret` (read from the binary). A virtual or table call to them leaves stack
cleanup to the caller, and the exe symbols are `__cdecl`, so they match. `Orig_00456760` is the matched source.

The other 134 targets (178 slots) have no exe body. Each one gets a **stop thunk**, `PortTopCodePtr_Stop_<rva>`, that
logs the target and the caller's return address, then ends the process with exit code = the target RVA. This is the
`RwCore_Missing` pattern: a named stop instead of a call into whatever exe code happens to sit at an original address.
The thunk never returns, so the caller's calling convention does not matter.

## 2. What changed

| file | change |
|---|---|
| `Compat/PortTopCodePtr.{h,cpp}` (new, exe only, `RVA-LINT: ORIGTOP-ONLY`) | `PortTopCodePtr_Apply`: with `MASHED_OT_CODEPTR=1`, writes the 72 seeds, then the 188 slots, into the B17 pad (`g_b17_low_arena_pad`), with a bounds check. Then it reads every write back. A slot must equal its symbol's address, inside the exe image and outside the pad. A seed must equal its table bytes. With the sub-knob off it writes nothing and logs one line. `PortTopCodePtr_Log` writes the held lines after the exe clears `mashed_re.log` |
| `Compat/PortTopCodePtr.inc` (generated) | `CP_EXTERN` / `CP_STOP` / `CP_FIX` / `CP_SEED` table |
| `exe_main.cpp` | `#include`. In WinMain's `MASHED_ORIG_TOP` branch, `PortTopCodePtr_Apply(kLogPath)` before `PortTop_WinMain`. After the log clear, `PortTopCodePtr_Log(kLogPath)` next to `PortStateSeed_Log` |
| `exe_sources.rsp` | one line, `"Compat\PortTopCodePtr.cpp"`, after `PortStateSeed.cpp` |
| `verify/portorigtop_20261109/codeptr_inventory.py`, `CODEPTR_SLOTS.tsv`, `codeptr_arms.sh` | generator, slot table, arm driver |

No `re/match` source, unit list, BINDINGS.tsv row or tracker is touched. No librw binding is added (L-124).

**Why the log is held.** WinMain applies the table before the exe truncates `mashed_re.log` (the `fopen(kLogPath, "w")`
in exe_main.cpp before `PortStateSeed_Log`). The first boot of this row lost its summary line that way. The apply-time
lines are now buffered and written after the truncation.

## 3. Builds, ddkobj and knob boots

| check | result |
|---|---|
| default build (`MASHED_NO_DEPLOY=1`, VS2022) | Build OK. rva-lint OK, 0 new (93 known, ORIGTOP-ONLY 89). Port-state seed `--check` OK |
| object build (`MASHED_DDKOBJ=1`) | Build OK. Units linked 782/829, refusals 47 (static-register-convention 38, wrapper-redefines-globals 9) |
| `ddkobj.py check` | IDENTICAL 709, DIFFERENT 80, not-in-map 3 (`BYTECHECK.tsv`). The row adds no unit and changes no matched source, so these are integration `275dec120`'s numbers |

Knob boots: `codeptr_arms.sh`, one launch inside `mashed_lock.py run` inside `test-slot`, private root
`D:/MashedWT/PORT-OT-SEED-CODEPTR_root` (junction `original` -> `D:/MashedWT/PORT-TOP-04J_rootK/original`).
Common env: `MASHED_WIN_POS=primary-bl MASHED_DET_FRAMES=600`.

| arm | exe | env | result |
|---|---|---|---|
| vs_off | VS2022 | (default path) | rc 0, 7.3 s. Same end as run 1 (`B17-SUMMARY reached-main-loop chrome=YES thunks=6/6`). The only log difference is the B17 region map's image pages (layout) |
| vs_on_knoboff | VS2022 | `MASHED_ORIG_TOP=1` | rc 0, 7.2 s, `WinMain 0x00492370 returned 0 after 600 frames`, `[OT_CODEPTR] off ... no slot written` |
| vs_on | VS2022 | `+ MASHED_OT_CODEPTR=1` | rc 0x004568A0 after 7.2 s, STOP (section 4) |
| ddk_on | object build | `MASHED_ORIG_TOP=1 MASHED_OT_CODEPTR=1` | rc 0x004568A0 after 7.6 s, the same STOP |
| ddk_on_knoboff | object build | `MASHED_ORIG_TOP=1` | rc 0, 6.7 s, 600 frames |

Self-test line, identical on vs_on and ddk_on:

```
[OT_CODEPTR] applied: 188 slots (10 bound to an exe body, 178 to a stop thunk), 72 data seeds (288 B), 0 outside the
pad (table: 188 slots, 72 seeds); self-test PASS (0 of 260 checks failed)
```

## 4. Finding: with the binding ON, the first unbound target reached is 0x004568a0

```
vs_on   [OT_CODEPTR] STOP: a bound code pointer to original 0x004568A0 was called; the exe has no body for it.
        Return address 00177B5E (exe+0x167B5E) -> _Orig_0045b930+0x24E   (mapsym.py, mashed_re.map)
ddk_on  ... Return address 00015348 (exe+0x5348) -> _Orig_0045b930+0x38 (ddk_s09_0045b930_PortTop_0045b930.obj)
```

- The caller is the matched 0x0045b930 (`re/match/s09/0045b930.c:55-56`): `if (PTR_FUN_005f99b0[i].m_fn != 0)
  PTR_FUN_005f99b0[i].m_fn();`, over 0x40-byte entries at 0x005f99b0 (count `DAT_005f9bd8`).
- Slot 0x005f99b0 is entry 0's `m_fn`. Its file-backed value is 0x004568a0, and it sits inside the 32-byte
  `DAT_005f9998` entry of the inventory.
- **Without this row the slot reads 0**, so the matched body skips the call and the knob boot runs 600 frames. With
  the slot bound, the body makes the call the original makes, and the exe has no body for 0x004568a0.
- The other entries of the 0x005f99b0 table (0x005f99f0 onward) are not in the PORT-OT-SEED-2 inventory and stay
  unseeded.

So `MASHED_OT_CODEPTR` stays default OFF. Turning it on inside the `MASHED_ORIG_TOP` path changes the knob boot's
outcome, so it is not a P1 flip. It needs bodies first: 0x004568a0 is the next one, and the other 133 stop-thunk targets
are listed in `CODEPTR_SLOTS.tsv` (empty `exe_symbol`).

## 5. Evidence (raw data in `D:/MashedWT/PORT-OT-SEED-CODEPTR_data`, not committed)

| file | SHA-256 |
|---|---|
| mashedmod/build/mashed_re.exe (bc9b3b352) | 990b98790eebc28795a87437ce88c17c90df6583b7d4d0e5d7096b1d6533558e |
| mashedmod/build/ddkobj/mashed_re_ddk.exe | 2f8d858c156273410876def66e1fc7540f84dfeb42067d459da7302539988ddd |
| build_vs.log | 1853f36b0bf8227d592290e747fc08d7eab6924caf2b61b87f4e0f44a74acb25 |
| build_ddk.log | 53cbf647a1f6408bb8f7f542cd0e444c7563cded6dd442d9a7514fcd51795038 |
| BYTECHECK.tsv | 141b77bc8d5f0dbd816d4cc8236d01902189068b7b4a027181d3890539db083d |
| arms.log | 2fb562cf20bd8fbdfe4ef9757ab01167dd6b9fe994c04c25d9707c7383f48036 |
| vs_off.log | 0992850b496247bad6b5b4320a5a70544621d4f09c95c50858e55d18398e6e82 |
| vs_on_knoboff.log | f8b78f23cfd8326e601eae5d24206622f0f104cb5653842899dbf6a12eec0cd8 |
| vs_on.log | 454fa89b918eba1e0bb90cf2596301f8c61bc552b09ad0ba58e438b079cfe4c6 |
| ddk_on.log | 4a817b2f8d49dfc44e8dfe6b25cc4640340befa1a5c63ae4d400b2ccffb62fa5 |
| ddk_on_knoboff.log | 6091c8fe2c74538c8798f81284857947669fe9a1bb4a9e6f90611f09749fea06 |
| Compat/PortTopCodePtr.inc | eccb0e3ac36c44c0c2908a1d6de2dfd265a8857f6695c59999e5c1400137a2f3 |
| CODEPTR_SLOTS.tsv | 9a0f29f2cda9f0151f96123436709d13989c05455b1d5583976a66e8b5b71139 |

`run1_void/` holds the first run (`8fd9128fe`, binding unconditional on `MASHED_ORIG_TOP`). The same STOP showed there,
and its summary line was lost to the log truncation. It also holds a second launch whose lock wait timed out after
1800 s behind PROV-WRITESET-114, with no boot.
