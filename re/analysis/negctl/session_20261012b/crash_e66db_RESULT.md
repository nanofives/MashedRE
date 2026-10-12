# CRASH-E66DB: group R ctl1 AV at `MASHED.exe+0xe66db` during track load

Investigation only. No port change, no tracker edit, `original/MASHED.exe` untouched. Date 2026-10-12, branch
`work/crashcam` (base `8da9ecf2`). Binary under test: `original/MASHED.exe`, the ten-patch boot recipe, SHA-256
`B9977DAB...FE87` (same as ctl1). All disassembly is from `MASHED.exe.unpatched`. The patched exe is
byte-identical to it over every range cited here (compared 2026-10-12: `0x004e66d0..0x004e6709`,
`0x004b3f20..0x004b3ff7`, `0x00420420..0x0042086b`, `0x0042a5d0..0x0042a637`, `0x0040d110..0x0040d250`,
`0x0040d270..0x0040d429`).

## Verdict: SAME COLLISION CLASS as U-9677, different victim file

The ctl1 crash is the original's exclusive piz open failing while another handle holds the file, the same
mechanism as U-9677. The file is the **vehicle** piz, not the track piz, so the NULL surfaces in a different
unguarded consumer: the DFF clump loader returns NULL and `RpClumpForAllAtomics` dereferences it.

| reader held during the Play Game confirm | boots | result |
|---|---|---|
| none (`vehhold1`, `vehhold2`: the hold did not arm, see "Runs") | 2 | 2 reach Race Results |
| none, lock probe only (`probe1`) | 1 | reaches Race Results |
| `TOASTART/VEHICLES/*.piz` read-only (`vehholdB1`, `vehholdB2`) | 2 | **2 AV at `0x004e66db`, read @ `0x8`** |
| `TOASTART/TRACKS/*.piz` read-only (`trkhold1`, `trkhold2`) | 2 | **2 AV at `0x005509b4`, read @ `0x38`** (U-9677's signature) |

The track-hold arm reproduces U-9677 on the ctl arm, and the vehicle-hold arm reproduces ctl1. The only
difference between a clean arm and a crashing arm is which piz files a second handle holds.

The re-run rule covers it. Proposed text for the CLAUDE.md rule is at the end.

`[UNCERTAIN]` What held the vehicle piz during ctl1. ctl1 recorded only `pc` and the fault address (no registers,
no stack, no list of other processes). Other sessions were running MASHED at the time. The signature, the load
phase and the arm all match `vehholdB1/2`. Missing evidence: a process/handle record at ctl1's crash instant.

## 1. What ctl1 was (from `re/analysis/negctl/reearn_20261011/evidence/c4_pb2/c4_pb2_ctl1.json`)

- Process: the **original** `MASHED.exe` (pid 3716), not the standalone. The arm was `ctl`:
  `MASHED_RE_NO_AUTO_HOOK=1`, then `Module.load(ctl_stride4.asi)` with `MASHED_HOOK_ONLY` = the four group R
  getters (`0x00423ee0`, `0x00423f60`, `0x00423ff0`, `0x00424070`), E9 confirmed on all four.
- Crash came after `confirm 3` on screen 18 (phase 0, sph 2), with 0 calls to any of the four getters. So no hooked
  body had run. The deployed `mashed_re_dev.asi` installs nothing under `MASHED_RE_NO_AUTO_HOOK=1`.
- Exit `0xC0000005`. Record: `{'pc': '0x4e66db', 'module': 'MASHED.exe+0xe66db', 'addr': 'read @ 0x8'}`.

My runs use the same harness, route, recipe and `ctl_stride4.asi` (SHA-256 `7D6BDB0E...7721`, identical, copied
into my scratchpad). The deployed `.asi` differs (ctl1 `C5594D6B...`, mine `76CF936A...`), which does not matter
here because neither installs hooks under `MASHED_RE_NO_AUTO_HOOK=1`. All my runs used a private install copy
(`C4PB2_GAMEROOT`, robocopy of `original/`, 482 MB) so that no other session's game could collide with them.

## 2. Crash capture (`vehholdB1`, `vehholdB2`, identical)

Catcher: `poll_attach_catch_crash.py`'s exception-handler agent loaded into the harness session on the spawned
PID (`--catch-crash`), plus the harness's 0x10000-byte stack scan. Main thread.

```
access-violation  EIP 0x004e66db  read @ 0x00000008
EAX 0x00000000  EBX 0x00000000  ECX 0x001afb1c  EDX 0x001afb20
ESI 0x00000000  EDI 0x00000008  EBP 0x007f1a1c  ESP 0x001afafc
bytes at EIP  8b 40 08 3b c7 74 1e 8b 5c 24 1c 8b 6c 24 18 8b
```

Unwound frames (each return address checked against the call before it):

| ESP+ | value | what |
|---|---|---|
| 16 | `0x004b3fef` | return from `call 0x4e66d0` at `0x004b3fea` in `FUN_004b3fc0` |
| 24 | `0x004b3f20` | the callback argument `FUN_004b3fc0` pushes (`0x004b3fd8 push 0x4b3f20`) |
| 44 | `0x00420528` | return from `call 0x4b3fc0` at `0x00420523` in `FUN_00420420` |
| 716 | `0x0040d200` | return from `call 0x420420` at `0x0040d1fb` in `FUN_0040d110` |
| 776 | `0x0040d33b` | return from `call 0x40d110` at `0x0040d336` in `FUN_0040d270` |
| 804 | `0x0040d452` | inside `FUN_0040d440` (same frame as U-9677's chain) |

Function boundaries and callers from Ghidra (`re/tools/decomp_pc.py --callers`, pool slot `Mashed_pool0`,
read-only): `0x00420528` is in `FUN_00420420` (caller `FUN_0040d110`), `0x0040d200` in `FUN_0040d110` (caller
`FUN_0040d270`), `0x0040d33b` in `FUN_0040d270` (caller `FUN_0040d440`). Other scan hits (`0x004270dc`,
`0x004952b8`, `0x004b657a`, ...) are stale dwords: for example `0x004270dc` follows the cookie check
`call 0x4a2be9` at `0x004270d7`, not a call on this chain.

`EBP = 0x007f1a1c` equals `FUN_0040d110`'s loop pointer on its first iteration (`piVar6 = &DAT_007f1a1c`), so the
fault is on player 0's car.

## 3. Static chain

1. **Course load first, vehicles second.** `FUN_0040d270` calls `FUN_0040d020` (track: `call 0x40d020` at
   `0x0040d2f0`, U-9677's path) and then `FUN_0040d110` (`0x0040d336`). The lock probe (`probe1`,
   `C4PB2_PIZ_GLOB=**/*.piz`) saw the game hold files without read sharing in this order after the confirm:
   `TRACKS/training.piz` t+0.33..1.13 s, `Common/LED.piz` 1.15, `Common/AI.piz` 1.23, `Common/Perm.piz` 1.31,
   `VEHICLES/Advantag.piz` 1.41..1.97 s, `VEHICLES/dirt.piz` 1.97..2.06 s.
2. **Vehicle piz mount.** `FUN_0040d110`: `call 0x420230` (`0x0040d1ca`, group path), `call 0x42a8d0`
   (`0x0040d1d0`), `call 0x495280` (`0x0040d1d6`). `FUN_00495280` is the same mount call U-9677's track path uses
   (crash1509b4 §4.2). Its open is `FUN_004b6710`, `CreateFileA(.., dwShareMode = 0 at 0x004b6738, ..)`
   (crash1509b4 §4.1, not re-derived here).
3. **Per-player car load.** Loop at `0x0040d1fb`: `FUN_00420420(player, car)`. It loads the TXD
   (`call 0x42a6b0` at `0x004204c0`, only when `DAT_0063d7e8 == 0`), then the DFF:
   `call 0x42a5d0` at `0x0042050f`, result stored at `[edi+0x26c]` (`0x0042051d`) and passed straight to
   `call 0x4b3fc0` at `0x00420523`. No NULL test between them.
4. **DFF loader returns NULL.** `FUN_0042a5d0` copies the name to `0x0067e6a8`, appends `[0x005f65b0]` (".dff",
   string at `0x005cd29c`) when there is no '.', calls the piz lookup `FUN_0042a530(0x0067d9a8, 0x0067e6a8)`
   (`0x0042a61b`). On 0 it returns 0 (`0x0042a625 je 0x42a634`, `0x0042a634 xor eax,eax`). Otherwise it
   tail-jumps to `0x004b3bf0` (`0x0042a62f`). `[UNCERTAIN]` which of the two exits produced the NULL in
   `vehholdB1/2`. Missing evidence: the return of `FUN_0042a530` on that call. The failed mount makes the lookup
   exit the expected one. It is not observed.
5. **Fault.** `FUN_004b3fc0(clump, out)` calls `FUN_004e66d0(clump, 0x004b3f20, &local)` (`0x004b3fea`).
   `FUN_004e66d0` is `RpClumpForAllAtomics` (hooks.csv, C2): `0x004e66d8 lea edi,[eax+8]`,
   `0x004e66db mov eax,[eax+8]`. With clump = 0 that is the read @ `0x8`.

## 4. Boot patches, the hook arm, the standalone

- No boot patch touches `0x0040d110`, `0x00420420`, `0x0042a5d0`, `0x004b3fc0` or `0x004e66d0` (byte compare above).
  `disable_log` only silences the "Unable to find %s in piz" print, as in U-9677.
- The ctl `.asi` is not implicated. The crash precedes every hooked call, and `trkhold`/`vehholdB` crash the same
  arm in the two different places that the held file predicts.
- Standalone: not examined in this session. crash1509b4 §6 records that the standalone opens piz files with a
  shared-read `std::fopen` (`Piz/PizReader.cpp:40`). A byte-faithful port of `FUN_00420420` would carry the same
  unguarded DFF-NULL path. Keep a NULL check there if it is ever ported (same stance as U-9677).

## 5. Runs (all own PIDs, never killed by name. `0xDEADBEEF` = harness kill, `0xC0000005` = AV)

| tag | env | result | PID |
|---|---|---|---|
| `probe1` | `C4PB2_PROBE_PIZ=1`, glob `**/*.piz`, 15 s | Race Results | 15592 |
| `vehhold1`, `vehhold2` | `C4PB2_HOLD_PIZ=1`, glob `VEHICLES/*.piz`, **no** `C4PB2_S18_ROW` | Race Results (hold never armed, `held_piz` absent from the JSON: the hold lives in the screen-18 leg, which this route skips without `C4PB2_S18_ROW`) | 20996, 31776 |
| `vehholdB1`, `vehholdB2` | as above + `C4PB2_S18_ROW=0` (cursor already on row 0: no press, no edit) | AV `0x004e66db` | 29236, 10992 |
| `trkhold1`, `trkhold2` | glob `TRACKS/*.piz` + `C4PB2_S18_ROW=0` | AV `0x005509b4` | 5296, 6560 |

Two-boot rule met on both crash arms and on the no-reader control.

Harness change: `re/frida/c4_pb2_scenario.py` gains `C4PB2_PIZ_GLOB` (which files `HOLD_PIZ` holds and
`PROBE_PIZ` probes, default `TRACKS/*.piz` = old behaviour) and `C4PB2_PROBE_SECS` (default 1.2 = old behaviour).

Evidence: `crashcam_evidence/c4_pb2_{probe1,vehhold1,vehhold2,vehholdB1,vehholdB2,trkhold1,trkhold2}.json`.

## 6. Side finding (not acted on)

hooks.csv row `0x004b3fc0` is band-classified `third-party-library[msvc-crt-main]` ("library_residue
band=msvc-crt-main span=0x004a0000..0x004b4000"). Its bytes call `RpClumpForAllAtomics` (`0x004b3fea`) with the
callback `0x004b3f20` and return the callback's out value (`0x004b3fef mov eax,[esp+0xc]`). That is not CRT
code. Candidate for a re-classify review. The band span may hold other non-CRT functions near `0x004b4000`.

## 7. Proposed text (the orchestrator applies it)

**CLAUDE.md, MASHED process hygiene, replace the U-9677 bullet with:**

> - **Concurrent piz loads crash the original (U-9677): re-run, don't report.** All worktrees share one `original/`
>   install, and the original opens every piz exclusively (`FUN_004b6710`, dwShareMode 0 at `0x004b6738`). When
>   another process holds the file at that moment, the open fails and a later consumer dereferences NULL. Two
>   signatures, both collisions: `MASHED.exe+0x1509b4` (read @ `0x38`, track piz, NULL `COURSE.LUA` handle) and
>   `MASHED.exe+0xe66db` (read @ `0x8`, vehicle piz, NULL DFF clump into `RpClumpForAllAtomics`). Either one in an
>   original-side load means re-run the run, never treat it as a finding. User decision 2026-10-12: re-run rule
>   only (no private install copies, no cross-session lock).

**UNCERTAINTIES.md, U-9677 (append to the resolution):**

> 2026-10-12 crashcam: the same mechanism also fires on the vehicle piz. `FUN_0040d110` mounts it
> (`0x0040d1d6 call 0x495280`), `FUN_00420420` loads each car's DFF through `FUN_0042a5d0` (`0x0042050f`) and
> passes the result unchecked to `FUN_004b3fc0` -> `RpClumpForAllAtomics` (`0x004b3fea`), AV at `0x004e66db`
> read @ `0x8`. Holding `VEHICLES/*.piz` read-only across the Play Game confirm: 2/2 AV at `0x004e66db`. Holding
> `TRACKS/*.piz`: 2/2 at `0x005509b4`. No reader: 0/3. Group R ctl1's load-time AV is this signature.
> re/analysis/negctl/session_20261012b/crash_e66db_RESULT.md.

**Memory `concurrent-track-load-crashes-original`:** add the `0x004e66db` / read @ `0x8` signature (vehicle piz).
