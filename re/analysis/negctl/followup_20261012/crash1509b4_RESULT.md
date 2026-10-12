# crash1509b4: original AV at `MASHED.exe+0x1509b4` at race start

Investigation only. No port change, no tracker edit, `original/MASHED.exe` untouched. Date 2026-10-12, branch
`work/crash1509b4` (base `4d6f7038`). `original/MASHED.exe` = the ten-patch boot recipe, SHA-256 `B9977DAB...FE87`
(same as every group R run). All runs are the `stock` arm (`MASHED_RE_NO_AUTO_HOOK=1`, nothing loaded).

## Verdict

**The crash is not caused by the Vehicle setting `0x0067ea94`.** It is a file-sharing race on the track `.piz`:
the original opens the piz with `CreateFileA(.., dwShareMode = 0, ..)`, and when another process has the same
file open during that call, the piz is not mounted, the `COURSE.LUA` open returns NULL, and the Lua loader seeks
the NULL handle without a check. Reproduced deterministically (5 of 5 boots with a concurrent reader, 0 of 15
without), including the real multi-session case: two original instances starting the same track load together.

**Standalone: avoid, do not reproduce.** The trigger is an outside process holding the file, not anything a player
does in the game. The standalone already avoids it (§6).

## 1. Recipe (from `c4_a1_r_RESULT.md` §8 and `evidence/c4_pb2/explore/c4_pb2_explore5.txt`)

Reached **through the menu**, not a memory write: Team Play route (title -> GTS -> Multi Player -> Team Play ->
colour select -> team select -> screens 15, 6, 18), then a blind cursor on screen 18 landed on row 4 and one RIGHT
(control 10) changed `0x0067ea94` 0 -> 1. The crash came on the Play Game confirm, inside the first 1.2 s of the
track load. explore5 recorded `pc` and fault address only (`0x5509b4`, read @ `0x38`), no registers or stack, and
its JSON went with the removed `c4a1-r` worktree. No WER dump exists for it (`%LOCALAPPDATA%\CrashDumps` holds one
MASHED dump, 2026-10-11 15:15, a different run).

I reproduced the same route through the menu with the same harness (`re/frida/c4_pb2_scenario.py`, extended, see
Files). The edit is the game's own menu handler, so no forced input is involved.

## 2. Runs (all own PIDs, killed by PID only. Exit `0xDEADBEEF` = harness kill after the window, `0xC0000005` = AV)

| arm | what differs | boots | AV at `0x005509b4` |
|---|---|---|---|
| `veh1..5` | explore5 route, Vehicle 0 -> 1, shared `original/` | 5 | 0 (all 5 reach Race Results) |
| `ctl1..2` | same route, cursor on row 4, no edit (Vehicle 0) | 2 | 0 |
| `dzveh1..3` | as veh, profile 3 device dword zeroed (see below) | 3 | 0 |
| `dzctl1..2` | as ctl, profile 3 device dword zeroed | 2 | 0 |
| `rootctl1` | private install copy, Vehicle 0, no reader | 1 | 0 |
| `rootprobe1` | private copy, lock probe only | 1 | 0 |
| `roothold1..2` | private copy, Vehicle 0, harness holds every track piz open read-only from the Play Game confirm | 2 | **2** |
| `rootholdveh1` | as roothold, Vehicle 0 -> 1 | 1 | **1** |
| `pair1`, `pair2` | two stock instances on the private copy, Vehicle 0, Play Game confirms released by a file barrier at the same instant | 2 pairs | **1 instance per pair** (pair1a, pair2b), the other reaches Race Results |
| `desync1` | as pair, side b confirms 5 s later | 1 pair | 0 |

Single-instance boots without a reader: 15, 0 crashes (Vehicle = 1 in 8 of them). Boots with a concurrent reader:
5, 5 crashes. Vehicle = 0 crashes as readily as 1 (roothold1/2, pair1a, pair2b).

**Profile-3 colour, ruled out.** explore3..6 logged colour choices `[1,2,3,-1]`, every later run (and mine)
`[1,2,3,0]`. `FUN_004332a0` writes -1 for a profile `FUN_00497450` reports absent, and `FUN_00497450` returns
`[0x007e96fc + p*0x200] != 0` (`0x0049745f`). This machine now has devices `[1,1,1,2]` (3 joypads + keyboard),
so profile 3 is present. The `dz*` arms zero `0x007e96fc + 3*0x200` at the title (an investigation input, not
evidence for any promotion), which reproduces `[1,2,3,-1]` exactly. 0 of 5 crashed. The device count only explains
why explore3..6 differ from later runs.

Two-boot rule: met on the crash arm (roothold 2/2, plus rootholdveh1 and 2 natural pairs) and on every control.

## 3. Crash capture

Catcher: the exception-handler agent of `re/frida/poll_attach_catch_crash.py` (`AGENT_JS`, imported, no
Interceptor on the crash path), loaded into the harness's own session on the PID it spawned. I did not run
the tool standalone because it attaches to "first MASHED.exe by name", which the multi-session rules forbid.
That agent's 320-dword walk ends inside `FUN_0047b8d0`'s 0x8000-byte stack buffer, so the harness agent adds a
0x10000-byte scan for `.text` values.

Identical in all 5 crashes (roothold1/2, rootholdveh1, pair1a, pair2b):

```
type access-violation   EIP 0x005509b4   read @ 0x00000038   (main thread: tid == first enumerated thread)
EAX 0x00000000  EBX 0x0000001e  ECX 0x005cd584 ("COURSE.LUA")  EDX 0x7eff4054
ESI 0x00000000  EDI 0x00644158  EBP 0x00644378  ESP 0x001a7c70
bytes before EIP  .. 90 90 90 90 8b 44 24 04      bytes at EIP  8b 48 38 8b 50 18 83 c1 28 85 d2 74 0e c7 40 28
```

Faulting instruction: `0x005509b4 8b 48 38  mov ecx,[eax+0x38]`, EAX = arg0 loaded at `0x005509b0`
(`mov eax,[esp+4]`). EDI/EBP hold `&DAT_00644158` / `&DAT_00644378`, the two pointer args `FUN_00426e10` passes to
`FUN_0047a020("COURSE.LUA", &DAT_00644378, &DAT_00644158)`.

Stack scan (byte offset from ESP: value), roothold2, identical in pair1a:

| off | value | resolves to |
|---|---|---|
| 0 | `0x0047b91c` | return from `call 0x004b5390` at `0x0047b917` in FUN_0047b8d0 (thunk JMP -> `0x005509b0`) |
| 32812 | `0x0047b9c9` | inside FUN_0047b9b0 (`0x0047b9b0`, 39 B), its call of FUN_0047b8d0 |
| 32828 | `0x0047a0de` | inside FUN_0047a020 (`0x0047a020`, 196 B), its call of FUN_0047b9b0 |
| 32840 | `0x00440bc0` | the `FUN_00440bc0` callback argument FUN_0047a020 pushes, not a return address |
| 32856 | `0x00426f2d` | inside FUN_00426e10 (`0x00426e10`, 723 B), the `FUN_0047a020("COURSE.LUA", ..)` call |
| 33164 | `0x0040d033` | inside FUN_0040d020 |
| 33172 | `0x0040d2f5` | inside FUN_0040d270 (course load/finish handler) |
| 33200 | `0x0040d452` | inside FUN_0040d440 |
| 33212.. | `0x00492a6c`, `0x004922ae`, `0x004923de`, `0x004a4d38`, .. | scan candidates above the course load, not unwound |

## 4. Static chain (Ghidra `Mashed_pool0` read-only via `re/tools/decomp_pc.py`, capstone on `MASHED.exe.unpatched`)

Function boundaries and callers by Ghidra (`--callers`), not by `hooks.csv`:

1. **Piz open, exclusive.** `FUN_004b6710` (callers: FUN_004b6940, the piz open + header parse).
   `0x004b6710 test byte [0x007d3e50],1` / `je 0x004b672d`. Bit clear: `push 0` (`0x004b672d`, hTemplate),
   `push 0x60000000` (`0x004b672f`), `push 3` OPEN_EXISTING (`0x004b6734`), `push 0` security (`0x004b6736`),
   **`push 0` dwShareMode (`0x004b6738`)**, `push 0x80000000` GENERIC_READ (`0x004b673a`), `push eax` path,
   `call [0x005cc140]` = CreateFileA (`0x004b6740`), then `call [0x005cc0cc]` = GetLastError (`0x004b674b`).
   Returns 1 only if GetLastError == 0 and handle != -1 (`0x004b6751..0x004b675e`), else 0.
   Bit set: `call 0x004a4541` (`0x004b671f`), the `_fsopen` wrapper that `fix_fopen` patches.
   `[0x007d3e50]` is written only at `0x004b66ad` from FUN_004b6640's arg (no direct callers).
   **Which branch runs:** the CreateFileA one. `rootprobe1` shows the game's handle refusing a share-read
   `open(.., 'rb')` of `training.piz` from t+0.327 s to t+1.129 s after the Play Game confirm. A `_fsopen` "rb"
   handle would not refuse that, and the roothold crash itself requires the open to reject a read-only reader.
   The game holds the track piz for about 0.8 s during load, then closes it (psutil saw no open `.piz` in the race).
2. **Course load.** FUN_0040d270 -> FUN_0040d020 -> FUN_00426e10: builds `"toastart/tracks/" + name + ".piz"`
   (`0x005cd280` = ".piz"), `FUN_00495280` pushes it, then `FUN_0042a530(&local_110,"COURSE.LUA")` whose return
   (0 = "Unable to find %s in piz") is **ignored**, then `FUN_0047a020("COURSE.LUA", ..)`, and likewise for
   `LAPDATA.LUA` via `FUN_0047a0f0`.
3. **Lua file loader, unguarded.** FUN_0047b9b0 -> FUN_0047b8d0:
   `h = thunk_FUN_005507b0(name, "rb")` (`0x005cf010` = "rb"), then `thunk_FUN_005509b0(h,0,2)` at `0x0047b917`
   with no NULL test. `FUN_005507b0` (VFS open) returns 0 when no mounted file system resolves the name, which
   is the case when the piz open in step 1 failed.
4. **Fault.** FUN_005509b0 (called at `0x004b5390` thunk, `0x004cc084`, `0x005ab1ef`, `0x005ab2e3`,
   `0x005ab95b`, and via the thunk from `0x0047b917`/`0x0047b929`) dereferences `[arg0+0x38]` at `0x005509b4`.
   With arg0 = 0 that is the read @ `0x38`.

Where the bad value comes from: arg0 = the return of `FUN_005507b0("COURSE.LUA","rb")` = 0, because
`FUN_004b6710` got `ERROR_SHARING_VIOLATION` from CreateFileA while another process had the track piz open.

What `0x0067ea94` actually does on this path: in mode 6 `FUN_0040d110` and `FUN_0040d040` index the course table
by `FUN_00431d70()` (`mov eax,[0x0067ea94]`) to pick the vehicle group (`(id/6)*6`). FUN_00426e10's course
argument does not come from it. That is consistent with Vehicle = 1 never crashing on its own (0 of 8).

## 5. Boot patches and the shim

None implicated.
- `fix_fopen` patches `FUN_004a4541`, reached only on the `[0x007d3e50] & 1` branch (`0x004b671f`). The live branch
  is CreateFileA (§4.1). The NULL comes from a failed CreateFileA, not from the patch's `< 0x10000 -> 0` rewrite.
- `disable_log` only silences the "Unable to find %s in piz" / "Opening piz file" prints. It changes no value.
- `no_focus_pause`, `fix_camera_res`, `show_windowed`, the d3d9 shim and the 640x480 backbuffer do not touch file
  I/O. `skip_*` / `fix_joypad` sites are not on the chain.
- The patched `original/MASHED.exe` and `MASHED.exe.unpatched` are byte-identical over `0x004b6710..0x004b6764`,
  `0x0047b8d0..0x0047b978`, `0x005509b0..0x00550a15`, `0x00426e10..0x004270e3`, `0x005507b0..0x00550905` and
  `0x00550580..0x0055066d` (compared 2026-10-12). All disassembly above is from `MASHED.exe.unpatched`.

## 6. Standalone decision: avoid

- The trigger is an outside process reading the track piz during the original's ~0.8 s exclusive open window: a
  second game instance, or any scanner, indexer or backup tool. A single player in a single instance never hits it
  (15 of 15 boots clean). Reproducing it would mean shipping a crash keyed on other programs, which the project's
  fork-fix stance does not require. No faithful-behaviour observable is lost by not crashing.
- The standalone already avoids it. `Piz/PizReader.cpp:40` opens with `std::fopen(path, "rb")` (shared read),
  reads the whole file into memory and returns `false` with `last_error_ = "fopen failed"` on failure. None of
  `0x004b6710`, `0x0047b8d0`, `0x00426e10` is ported into the exe (`exe_sources.rsp`). The `.asi`'s
  `Compat/PizWin32Bypass.cpp` replacement for `0x004b6710` keeps `dwShareMode = 0` but is MASS-DISABLED
  (`:363`), so it does not affect hook-live runs either.
- If the standalone ever ports the Lua loader byte-faithfully, keep a NULL check after the open (documented
  deviation, cite `0x0047b917`).

## 7. Harness consequence (the part that matters for day-to-day work)

Every worktree's `original\` is a junction to the same install, so **two sessions loading the same track at the
same moment can AV one another's original-side run at `0x005509b4`.** pair1/pair2 show this directly. The likely
explanation for explore5 is that another session's MASHED was loading `training.piz` at that moment.
`[UNCERTAIN]`: explore5 recorded no registers or stack, and no record exists of which other MASHED processes ran at
that instant. The signature (EIP, fault address, load phase) matches exactly.
Group R's ctl1 load-time AV at `MASHED.exe+0xe66db` (read @ `0x8`) happened in the same phase and is a candidate
for the same class. `[UNCERTAIN]`: not reproduced or examined here.

Mitigations, for the orchestrator to pick (I did not implement any):
- treat a track-load AV at `0x005509b4` as a harness collision: re-run, never count it as a result (extends memory
  `shadow-lane-failure-windows`)
- runs that must not collide use a private install copy (robocopy `original\`, ~483 MB) as `C4PB2_GAMEROOT` does
- or add a cross-session lock around the Play Game confirm -> track-loaded window (`multi-session` skill)

## 8. Proposed tracker changes (orchestrator applies, via re-classify)

No C-level change on any row.

**New U-row (U-NEW):**

> U-NEW | behavioural | original AV `MASHED.exe+0x1509b4` (`0x005509b4`, read @ `0x38`) at race start. RESOLVED
> (explained) 2026-10-12: track-piz sharing race, not the Vehicle setting `0x0067ea94`. `FUN_004b6710` opens the piz
> with CreateFileA dwShareMode=0 (`0x004b6738`). A concurrent reader of the same file makes it fail, `COURSE.LUA`'s
> VFS open (`FUN_005507b0`) returns 0, and `FUN_0047b8d0` seeks it unguarded (`0x0047b917` -> `0x005509b4`). Chain
> FUN_0040d270 -> FUN_0040d020 -> FUN_00426e10 (`0x00426f2d`) -> FUN_0047a020 -> FUN_0047b9b0 -> FUN_0047b8d0. Repro
> 5/5 with a concurrent reader (harness-held file 3/3, two synchronized instances 2/2 pairs), 0/15 without (Vehicle=1
> 0/8). No boot patch or shim involved. Standalone: avoid (PizReader fopen "rb", fails cleanly). Harness: concurrent
> sessions loading one track can kill each other's original-side run. Evidence
> re/analysis/negctl/followup_20261012/crash1509b4_RESULT.md, verify/crash1509b4_20261012/.

**hooks.csv notes to append (status/C-level unchanged):**
- `0047b8d0`: `| 2026-10-12 crash1509b4: no NULL test on the VFS_Open result before the seek at 0x0047b917, AVs at
  0x005509b4 when the file is not in a mounted piz (U-NEW).`
- `004b6710`: `| 2026-10-12 crash1509b4: dwShareMode=0 pushed at 0x004b6738; live branch is CreateFileA (not the
  0x004a4541 _fsopen arm); holds the track piz ~0.8 s during load; a concurrent reader makes it return 0 (U-NEW).`
- `00426e10`: `| 2026-10-12 crash1509b4: FUN_0042a530 "COURSE.LUA" result ignored before FUN_0047a020 (U-NEW).`
- `005509b0`: subsystem `audio` is incomplete. The same function is the RtFS seek used by the Lua loader
  (`0x0047b917`/`0x0047b929` via thunk `0x004b5390`) and RwStreamSkip (`0x004cc084`). Flagged, not reclassified.

**NEXT_SESSION / c4_a1_r_RESULT side finding:** the "Vehicle 0 -> 1 crash" line should read "track-piz sharing race
with a concurrent MASHED, see U-NEW". Vehicle = 1 is safe.

Nothing is ready for path2 (no port change).

## Files

- harness (extended, all options default-off and env-gated): `re/frida/c4_pb2_scenario.py`:
  `C4PB2_S18_ROW` (screen-18 row to edit, cursor leg runs even with 0 presses), `--catch-crash` (loads
  `poll_attach_catch_crash.AGENT_JS` into the same session), a deep stack scan + thread ids in the AV message,
  `C4PB2_DEV_ZERO`, `C4PB2_GAMEROOT` (private install copy), `C4PB2_HOLD_PIZ` / `C4PB2_PROBE_PIZ` (copy only,
  asserted), `C4PB2_BARRIER` / `C4PB2_BARRIER_DELAY` (synchronized confirms).
- runners: `verify/crash1509b4_20261012/run_repro.sh`, `run_pair.sh`
- per run: `verify/crash1509b4_20261012/c4_pb2_<tag>.json` + `<tag>.txt` (23 runs. The `crash_full` field
  holds the full register/stack/module payload)
- decomp: `verify/crash1509b4_20261012/decomp_*.txt`, `seek_callsites.txt`
- private copy used by `root*`/`pair*`/`desync*`: scratchpad `gameroot/original` (exe SHA-256 `B9977DAB...FE87`),
  deleted at the end of the session
