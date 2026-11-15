# RESULT L156-ECX-RUNTIME-TARGETS (run 47, MATCH section, runtime row on the original)

Branch `orch/L156-ECX-RUNTIME-TARGETS`, based on `orch/integration` 198109d50. 2026-11-14. No build, no source
change, no tracker change, no gate. Runtime on the stock original (no dev asi mapped) from the private root
`D:\MashedWT\L156-ECX-RUNTIME-TARGETS_root` (`MASHED.exe` SHA-256 `b9977dab...fe87`, the ten-patch boot copy;
`MASHED.exe.unpatched` beside it = the anchor `bdcae093...3c0e`). Both launches ran under
`mashed_lock.py run --label L156-ECX-RUNTIME-TARGETS`; lock status before the first run: `free`, no PRIORITY-HOLD.
Static disassembly: capstone through `re/tools/disasm_va.py` on `MASHED.exe.unpatched`, and
`verify/l156_20261114/ecx_livein.py` (from L156-ECX-556CA0). Ghidra not used.

**Question.** Which targets does `jmp dword ptr [eax + 0x138]` at 0x00556cb0 reach at runtime, and does each write
ECX before reading it?

**Answer.** In both runs one target was observed: **0x00554940**, 16,227 hits in total. It fully writes ECX at
0x00554962 before any ECX read, and its early-return path does not touch ECX. **0x005547f0 was not observed**, so
for that target the evidence is still the static result of L156-ECX-556CA0 only.

## 1. Tool

`re/frida/probe_icall_targets.py` (new, committed here). No registry fits: `hooks_registry.py` holds A/B test
vectors for hook bodies, and no existing script logs an indirect-transfer target by site. The script spawns
the original with `dev_asi.spawn_original(hooks=False, resume=False)`, loads one Interceptor at the site
before the first resume, and in `onEnter` reads `[<reg> + <disp>]`. It counts per target and per
(target, return address, base) and keeps the first 4 register snapshots per target. The listener detaches
itself after `--max-hits` hits. With `--race` it drives Quick Battle with `nav_agent.js` (same recipe as
`nav_to_race.py`). It kills only the pid it spawned.

## 2. Call rate (hot-path check, run 1 first)

Run 1: boot to the main menu, 15 s, cap 3000 hits (`run1.log`). 0 hits/s for the first 9 s of boot, then
136, 408, 220, 184, 180, 212 hits/s at the menu. Run 2: in the race, 180 to 189 hits/s from second 19 on.
Run 2's first 1 s sample after the race load reads 10,906, but that sample spans the whole menu navigation
and race load (the poll loop starts after navigation), so it is not a per-second rate. The highest per-second
sample was 408/s, under the 1000/s line in CLAUDE.md. The cap was never reached (`detached=False` in both).
Frida detached with `application-requested` in both runs. No crash.

## 3. Observed targets

| run | scenario | hits | targets | (target <- return address, base) |
|---|---|---|---|---|
| 1 | boot + main menu, 15 s | 1,340 | 0x00554940 only | `<- 0x00427ed2`, base `0x0d469b5c` (first 4 samples) |
| 2 | boot, menu, Quick Battle (Arctic, `nav_agent.js` recipe), 40 s in race (phase 0) | 14,887 | 0x00554940 only | `<- 0x00427ed2` 8,866; `<- 0x00428276` 5,967; `<- 0x00427fc6` 54; all base `0x0d629ad4` |

Call sites of those return addresses (`MASHED.exe.unpatched`, function bounds from `re/console/pc_funcs.csv`):

```
0x00427ecd  e8 ce ed 12 00   call 0x556ca0     ; FUN_00427e00 (0x00427e00..0x00427ef5), returns to 0x00427ed2
0x00427fc1  e8 da ec 12 00   call 0x556ca0     ; FUN_00427f00 (0x00427f00..0x00427fe8), returns to 0x00427fc6
0x00428271  e8 2a ea 12 00   call 0x556ca0     ; FUN_00428140 (0x00428140..0x00428293), returns to 0x00428276
```

The site's `eax` is `[esp+4]` at 0x00556ca4, the first stack argument of 0x00556ca0. In each sample `eax`
equals `arg1` (`run1_rate.json` samples). This run did not read DAT_0067d838, so it does not show that the
base equals `[DAT_0067d838]`. [UNCERTAIN] that the observed base is the object held in DAT_0067d838. The missing
evidence is a read of `[0x0067d838]` next to the base.

**Not observed:** target 0x005547f0, and any call from FUN_00427ff0 (0x00427ff0..0x0042813d) to 0x00556ca0.

## 4. ECX at the observed target (raw bytes, `MASHED.exe.unpatched`)

```
0x00556ca0  8b 54 24 0c         mov edx, dword ptr [esp + 0xc]
0x00556ca4  8b 44 24 04         mov eax, dword ptr [esp + 4]
0x00556ca8  89 54 24 0c         mov dword ptr [esp + 0xc], edx
0x00556cac  89 44 24 04         mov dword ptr [esp + 4], eax
0x00556cb0  ff a0 38 01 00 00   jmp dword ptr [eax + 0x138]       ; no ECX access at the site

0x00554940  81 ec c8 00 00 00   sub esp, 0xc8
0x00554946  a1 e0 2b 91 00      mov eax, dword ptr [0x912be0]
0x0055494b  53                  push ebx
0x0055494c  55                  push ebp
0x0055494d  8b ac 24 d4 00 00 00 mov ebp, dword ptr [esp + 0xd4]
0x00554954  33 db               xor ebx, ebx
0x00554956  89 44 24 28         mov dword ptr [esp + 0x28], eax
0x0055495a  3b eb               cmp ebp, ebx
0x0055495c  0f 84 5e 08 00 00   je 0x5551c0                        ; ebp == 0 path
0x00554962  8b 8d 34 01 00 00   mov ecx, dword ptr [ebp + 0x134]   ; first ECX access = full write
```

The `ebp == 0` path is `0x005551c0 mov eax, ebp / pop ebp / pop ebx / add esp, 0xc8 / ret` with no ECX access
(L156-ECX-556CA0 section 2). `ecx_livein.py 0x554940 --depth 5` rerun here: 584 instructions reached, first ECX
event `0x00554962 FULL-WRITE`, no `READ-before-full-write` event. ECX at the site's entry differs across samples
(`0x1afc00`, `0x1afc18`) and the target overwrites it at 0x00554962 on the non-null path.

## 5. What this does to L156-ECX-556CA0's [UNCERTAIN] (the lead decides, no tracker edited)

* **Writer set of +0x138.** The runtime shows one target, 0x00554940, inside the static set
  {0x005547f0, 0x00554940}, for one object per run, in two scenarios (menu, one Quick Battle race on Arctic).
  This is consistent with the static claim for those scenarios. It does **not** clear the item. Not covered:
  other tracks, modes and screens, the 0x005547f0 target (static evidence only), FUN_00427ff0's call (never
  reached), and a write watch on `+0x138`. Recommendation: narrow the item to these gaps, do not close it.
* /GS-, stubbed callees, PK_458570 and ECX after the dispatch returns: not touched by this row, unchanged.

## 6. Files

Committed: `re/frida/probe_icall_targets.py`, this RESULT.
Rerun: `MASHED_ROOT=<root> py -3.12 <orch>/re/orchestrator/mashed_lock.py run --label <id> -- py -3.12
re/frida/probe_icall_targets.py --site 0x00556cb0 --reg eax --disp 0x138 --seconds 40 --max-hits 60000 --race --out <json>`.

Raw data (`D:/MashedWT/L156-ECX-RUNTIME-TARGETS_data`, not committed):

| file | SHA-256 |
|---|---|
| run1.log | 6fbe3012fa5c90b4929619e9551d1d562d4bcff071348d4deba543495eac3988 |
| run1_rate.json | 56c50fefe5555a3c0bd31c40dc45f9a29ee08aa99813549346f5e39c7c038850 |
| run2.log | 6705c96b54b25858938a08c5db5c9e7a291e81ad3b2ab0bb3df15e26796fb8de |
| run2_race.json | bdc5d1e47b973a763c350ceb01b21b20f26828e0024b1375812308967a697285 |
