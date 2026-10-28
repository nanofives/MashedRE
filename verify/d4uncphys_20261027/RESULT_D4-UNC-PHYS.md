# RESULT D4-UNC-PHYS (2026-10-27)

Step: physics standalone-input and coverage holes U-9783..U-9786. Branch `orch/D4-UNC-PHYS`, cut from
`orch/integration` @ `8f4e96366`.

**Verdict: BLOCKED for every live leg. U-9783 is narrowed statically. U-9784, U-9785 and U-9786 are unchanged.**
No source change, no knob, no PREREG, no gates run. The original does not boot on this machine right now (section 1),
and every remaining leg needs an original-side boot.

## 1. Blocker: the original AVs at boot with no audio endpoint

All four boots of `MASHED.exe` this session exited before the menu (`game exited while waiting for menu (phase 1)`
or `could not attach`). That held from the private root `D:\MashedWT\D4-UNC-PHYS_root` (with the original's HKCU
compat layer copied onto its path) and from the shared `original\`. A direct `Start-Process` of the private root
exits `0xC0000005` after under 12 s.

- With hooks (`mashed_re_dev.asi` auto-hook): fault in the .asi at `+0x289c8`, a read at `0x78`, `eax = 0`
  (`MASHED.exe.3540.dmp`).
- With `MASHED_RE_NO_AUTO_HOOK=1`: fault at `0x005baf48` in `MASHED.exe`, `mov dl, byte ptr [eax + 0x78]`, `eax = 0`,
  same `esp`/`ebp` as the first dump (`MASHED.exe.2184.dmp`). `0x005baf40` is `AudioRendererField3cSet`
  (`Audio/AudioLeaves_ab3.cpp:11`), so it was called with a NULL renderer.
- `Get-PnpDevice -Class AudioEndpoint` lists 2 endpoints, both `Philips FTV (NVIDIA High Definition Audio)`, both
  status `Unknown`. The machine has no active audio output.

The machine audio endpoint is the cause. The binary, the shim and the hook set are not. Re-run condition: an active
audio endpoint (the HDMI display on, or any other output device present). This is not U-9677 (no piz signature).

## 2. U-9783: the handling-type chain is a static image table

Disassembly of `MASHED.exe.unpatched` (capstone) and `decomp_pc.py --datarefs` (pool slot 0, read-only):

| Item | Fact | Cite |
|---|---|---|
| Selector | `v = FUN_00430790()` = `[0x0067f17c]`; `if FUN_0042f6a0() == 10` (`[0x0067e9fc]`) then `v = FUN_00431d70()` = `[0x0067ea94]` | `0x0046b56c..0x0046b582` |
| Type | `FUN_0040ce80(v)`: `ecx = [0x005f2770]`, `edx = [ecx + v*4]`, `eax = [edx + 4]` | `0x0040ce80..0x0040ce90` |
| `[0x005f2770]` | `.data` dword, value `0x005f2728`, file-backed (`.data` raw size `0x4d000`). **0 direct writes**, 40 direct reads | datarefs_u9783.txt |
| `0x005f2728[0..12]` | 13 pointers to 5-dword records (stride `0x14`, `0x005f2570..0x005f2673`). `[0]` = `0x005f2570`, **0 direct writes** | datarefs_u9783.txt |
| Record `+4` words | 13 addresses, **0 direct writes**, each one | datarefs_u9783.txt |
| Computed stores | a taint scan of all 40 read sites (60 instructions forward, through `[ptr + i*4]` loads) finds **one** store through the derived pointer: `0x0040e48e` `mov [edx + ecx*4 + 0x34], eax` (FUN_0040e480, the slot-state cell, already known). None reaches `+0x00..+0x30` or a record | taint_5f2770.txt |

So the "object array" in U-9783 is not a runtime object. It is the image's own initialised data, and nothing in the
code writes the record pointers or the `+4` words. The limit of this witness: the scan is linear and stops at calls,
so a store through a record pointer passed as an argument would be missed. A live peek is the second witness
(section 4).

Record `+4` value per `v`, joined to the table A3 walks at `0x00613140` (5 dwords per row: key, param, spring,
k1*1000, k2*1000, `-1`-terminated, 13 rows):

| v | type | `_DAT_00613108` | `0x00613114` | `0x00613130` | `0x0061313c` |
|---|---|---|---|---|---|
| 0 | 6 | **105** | 40000 | 1.0 | 1.5 |
| 1 | 30 | 100 | 40000 | 1.1 | 1.5 |
| 2 | 48 | 100 | 40000 | 1.0 | 1.4 |
| 3 | 18 | 100 | 42000 | 1.0 | 1.5 |
| 4 | 24 | 100 | 38000 | 1.0 | 1.5 |
| 5 | 0 | 100 | 40000 | 1.0 | 1.5 |
| 6 | 36 | 100 | 40000 | 0.9 | 1.5 |
| 7 | 42 | 100 | 40000 | 1.0 | 1.6 |
| 8 | 54 | 100 | 45000 | 1.0 | 1.3 |
| 9 | 60 | 100 | 40000 | 1.0 | 1.4 |
| 10 | 12 | **95** | 40000 | 1.0 | 1.5 |
| 11 | 66 | 100 | 45000 | 1.0 | 1.4 |
| 12 | 72 | 100 | 40000 | 1.0 | 1.5 |

The 13 types are a permutation of the 13 table keys (0, 6, .., 72). The standalone publishes only `_DAT_00613108`
(`g_handlingTorque`, `Vehicle/VehicleInit.cpp:128-134`). For it, the fixed type 6 is right for `v = 0`, and wrong
(105 instead of 100, or 95 at `v = 10`) for every other `v`.

**What `v` is, in Quick Race (mode 10).** `[0x0067ea94]` has 5 writers: `0x00431d16` (FUN_00431d00, stores 0) and
four in the menu handler FUN_0043dfd0 (`0x00440268`, `0x00440335`, `0x004407b8`, `0x0044088b`). At `0x0044079d..0x004407b8`
the handler steps it +1 with wrap `> 0xc -> 0`, at `0x0044024d..0x00440268` -1 with wrap `< 0 -> 0xc`, skipping any
index where `FUN_00430830(i) == 0`. FUN_00430830 in mode 10 returns `(&DAT_007f0a44)[i * 0xc]`, the per-track row
of the `0x007f0a40` array. So in mode 10 `v` is a 0..12 index filtered by that per-track row, the same 13-entry
space as the record table. `scenario_launch.py`'s `--track` help already names `0x005f2728` as the engine-track
pointer table. The engine track `[0x0063ba7c]` has one writer, `0x0040e174` (FUN_0040e170, stores `eax`). Its
callers were not read this session.

**Consequence for the harness [UNCERTAIN, unmeasured].** `scenario_launch.py` pokes `0x0063ba7c` and `0x0067f17c`
(`re/frida/scenario_launch.py:4243-4244`) and runs mode 10, but never writes `0x0067ea94`. Its initial `.data` value
is 0. If nothing else writes it before A3, then a harness original race on track N runs A3 with `v = 0`, type 6,
105, on every track. The standalone's fixed 6 would then match the harness's 13-track references, while a
menu-driven Quick Race on track N != 0 would use 100 (95 on `v = 10`). Two peeks settle it (section 4).

## 3. U-9784, U-9785, U-9786

Not advanced. Each needs an original-side boot:
- U-9784: `--watch-write 0x0063ba8c` over recipe F's countdown, next to a port log of `Fi_GameMode()`.
- U-9785: a random-surface track traced on both sides. `MASHED_FI_RAND=1` (default OFF,
  `Vehicle/ForceIntegratorStubs.cpp:74-81`) already draws from the port's ring. What is open is which stream is right.
- U-9786: natural-air and random-surface captures with A6b / A5 hooked.

## 4. Next (once an audio endpoint is present)

1. Harness peek, no hooks (`MASHED_RE_NO_AUTO_HOOK=1`), `--peek 005f2770:u,0067f17c:i,0067e9fc:i,0067ea94:i,0063ba7c:i,00613108:f,00613114:f,00613130:f,0061313c:f`
   on `--track 0`, `3`, `10`, `11`. Prediction from section 2 if nothing writes `0x0067ea94`: `0067ea94 = 0` and
   `00613108 = 105.0` on all four.
2. The same with `0x0067ea94 = N` written before spawn. `--poke-u32` cannot do it: it writes `--poke-delay`
   seconds into the hold (`re/frida/scenario_launch.py:4571-4573`), after A3 has run. The pre-spawn writes are at
   `:4243-4244`, so this needs a harness option that adds `0x0067ea94` there. Prediction: 105 / 100 / 95 / 100 and
   spring 40000 / 42000 / 40000 / 45000 for N = 0 / 3 / 10 / 11.
3. If (1) confirms, the 13-track references were taken at type 6 on every track. A faithful standalone selector
   (`v` from the pad's `0x0067ea94` / `0x0067f17c` plus the 13-value table above) is then gate-neutral on the harness
   scenario, and differs only for a menu-driven track choice. Ship it as a default-OFF knob with a PREREG. Flipping
   it is a decision row.
4. U-9784 / U-9785 / U-9786 as in section 3.

## Files

Committed (small extracts):
- `verify/d4uncphys_20261027/datarefs_u9783.txt`: `decomp_pc.py --datarefs` for `0x005f2770`, `0x005f2728`,
  `0x0067ea94`, `0x0067f17c` and the 13 record `+4` words (12 KB).
- `verify/d4uncphys_20261027/taint_5f2770.py` / `taint_5f2770.txt`: the store scan and its output.

Raw (not committed), SHA-256:
- `D:/MashedWT/D4-UNC-PHYS_data/datarefs_u9783.txt` `a26c8993523a88828b64300a2a66b7ac3e41fdb2f880a6be46d713281eff2ac8`
- `D:/MashedWT/D4-UNC-PHYS_data/datarefs_ba7c.txt` `78e502c5786cc6cb28e75c04af9400f0f65eb1f5da9e6993a9b4a09862ea565c`
- `D:/MashedWT/D4-UNC-PHYS_data/decomp_430830.txt` `d86df6856aa096ee4f7d479b38191c65d9a15ee254cb351ad596ad82f4b73ba6`
- `%LOCALAPPDATA%/CrashDumps/MASHED.exe.3540.dmp` (hooked boot) `9490c9737fd03d594a6ca023a851dea52fb1884015235d4fb6c7835c9038d135`
- `%LOCALAPPDATA%/CrashDumps/MASHED.exe.2184.dmp` (no-hook boot) `1e94a8a6052b2481b82c14631e5b57df9b81df511cf6967e8385509f5ad414b4`
