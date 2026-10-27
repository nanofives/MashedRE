# RESULT D4-WSB-BVH-WRITESET: write-set compare for the 9 BVH rows that stayed C3

Verdict: ****BLOCKED** on machine state, not on the code. The static legs and the default gates are done. Every runtime leg needs
the original `MASHED.exe` to boot. On LIVING-PC since about 02:15 (2026-10-27) it access-violates at `0x005baf48` before the
menu, for this step and for the sibling D4-WSB-NARROW-FIX stock and no-`.asi` arms alike. No row changes level in this step
and no tracker is touched.

| leg | state |
|---|---|
| harness swap (6 wrappers `Run` -> `RunTracked`) | done, `9577eac4d` |
| default gates on this exe | **unchanged**: G-CTRL `5943c5f1/364350ae/52052890`, B `3a03670c/e85ea6fb/53778664` |
| `0x00564c80` arg-3 diff | static: unobservable (the callee masks it), v6 comparator masks it, racediff re-run **blocked** |
| `0x00565260` EAX diff | static: unobservable at all 3 call sites, racediff (v5 void rule) re-run **blocked** |
| `0x00565200` out-memory | static: the `RunRegion` 0x1c region covers all 6 stores |
| write-set shadow, 9 rows | **blocked** (9 of 9 boots crash before the menu) |
| census racediff `0x00563e70` / `0x00565200` | tool ready (v6), **blocked** |
| BROAD / SOLV blind spot | mapped, 9 rows (section below) |**

Branch `orch/D4-WSB-BVH-WRITESET` off `orch/integration` @ `1158f5c1c`. PREREG `PREREG_D4-WSB-BVH-WRITESET.md`
(`eec2fd746`, committed alone before any arm). Prior evidence `verify/wsbbvh_20261026/RESULT_D4-WSB-C4-BVH.md`.

## What changed

Harness only. The shadow wrappers of `0x0055fea0`, `0x0055ff90`, `0x00561040`, `0x00561390` (RwpSolverCore23.cpp),
`0x00564040` (RwpSolverLeaves1.cpp) and `0x00564c80` (RwpSolverCore4.cpp) call `ShadowAB::RunTracked` instead of
`ShadowAB::Run`. Unarmed both call only `_impl`. The wrappers are linked into the exe as well, so the default gates
were re-run on this branch's exe (below).

Tool v6 (`make_tool2.py` -> `c4_racediff_v6.py`, from the v5 tool with 4 asserted replacements): the declared-width
arg mask for `0x005646c0` arg 3, an `asiloaded` rpc, and a census-scenario driver (`C4_SCENARIO=census`).

## Default gates (this branch's exe, `mashed_re.exe` `7f3ce5aa...382a`)

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT=D:/MashedWT/D4-WSB-BVH-WRITESET_root`,
under the test slot and `mashed_lock.py run --label D4-WSB-BVH-WRITESET`. All four arms `exit=0 self_exited=True`.

| arm | sha8 | ref (`merge_train.json`) |
|---|---|---|
| H_DEF step / gates / ptrace | `5943c5f1` / `364350ae` / `52052890` | `5943c5f1` / `364350ae` / `52052890` |
| B_DEF_K0 / K1 / K26 | `3a03670c` / `e85ea6fb` / `53778664` | `3a03670c` / `e85ea6fb` / `53778664` |

The wrapper swap is hash-neutral on the default build. The first gate attempt hit the 40 min background limit while it
waited for a test slot, and was re-run whole (`gates_run.log`, then `gates_run2.log`).

## Static results (original binary `MASHED.exe.unpatched`, anchor `BDCAE093...3C0E`)

Script `scan_argwidth.py` (recursive descent from the entry, esp delta tracked per path, the stack probe
`0x004a3440` counted as `sub esp, eax`), output `argwidth.txt`.

1. **`0x005646c0` reads only the low 16 bits of `param_3`.** The walk (390 instructions, one `ret` at `0x00564c78`
   with delta 0, no delta conflict) finds 4 loads of the slot and no store: `0x005647e3` -> `and eax, 0xffff` at
   `0x005647ec`, `0x00564830` -> `and ecx, 0xffff` at `0x0056483d`, `0x00564aab` -> `and eax, 0xffff` at
   `0x00564ab4`, `0x00564af8` -> `and ecx, 0xffff` at `0x00564b05`. Each mask precedes any use.
2. **`0x00564c80` reads only the low 16 bits of `param_2`.** Two loads of the original value: `edx` at `0x00564c83`
   feeds `and eax, 0xffff` (`0x00564c96`) and the push at `0x00564cac` (arg 3 of `0x005646c0`, item 1), then is
   overwritten by `mov edx, 0xffff` at `0x00564d0f` with no read in between. `esi` at `0x00564e03` reaches memory only
   through `or ecx, esi` / `mov word [ebp+eax*4+0x881e], cx` (`0x00564e2d`/`0x00564e2f`, 16-bit store), `ecx` is
   overwritten at `0x00564e37`, `esi` at `0x00564e4b` + `and esi, 0x3ff` at `0x00564e54`. `0x00564eae` is a 16-bit load.
   The later slot accesses (`0x00564ed9`, `0x00564ffa` write it, `0x00564eee`..`0x005650e8` read it back) are a spill
   of other values. Ghidra's decomp of `0x00564c80` agrees: `param_2 & 0xffff`, `(ushort)param_2` x2, and the full
   value only as the `FUN_005646c0` argument.
3. **The EAX that `0x00565260` returns is dead at all 3 call sites.** `0x00559fb1` and `0x0055ab61` overwrite EAX
   before any read (`verify/wsbbvh_20261026/scan_eax_use.txt`). At `0x0055aef3`: `mov ax, [edi+0x20]` (`0x0055aeff`),
   `push eax` (`0x0055af06`) as `param_2` of `0x00564c80` (`call` at `0x0055af08`), then `mov eax, [esi+0x68]` at
   `0x0055af0d`. So only EAX's high half reaches `0x00564c80`, as `param_2`'s high half, which item 2 shows is never
   read. The v5 racediff's EAX difference on `0x00565260` (orig `0/1/5`, port `0x16/0xe`) is therefore unobservable.
4. **`0x00565200`'s `RunRegion` region covers every store.** 6 stores, all `fstp [edx+k]` with `edx = param_3`
   (`0x0056520e`), `k` = 0x10, 0x14, 0x18, 0, 4, 8; the last ends at 0x1c = the region size. No call in the body.

Consequence for `0x00564c80`'s v4 BEHAVIOR-DIFF: OFF recorded arg 3 as `Sloc` (the raw dword, high half included,
fell in the stack range), ON as `18`/`19`/`1a`/`1b`. With item 1 the high half is dead, so v6 compares that arg
masked to 16 bits on both sides (PREREG 3). No other arg or callee is masked.

## Runtime legs: BLOCKED (the original does not boot on this machine now)

Every `MASHED.exe` launch from `D:/MashedWT/D4-WSB-BVH-WRITESET_root` exits with `0xC0000005` 4-6 s after start, before the
menu (`scenario_launch.py`: `game exited while waiting for menu (phase 1)`). What was tried, one boot each:

- With and without this branch's `.asi` (moved out of the root): same AV. Not this branch's code.
- With `__COMPAT_LAYER=RunAsInvoker WIN98RTM HighDpiAware EmulateHeap`, and with the root's own HKCU AppCompat layer set to
  `~ DWM8And16BitMitigation RUNASINVOKER WIN98RTM HIGHDPIAWARE EMULATEHEAP` (the value the booting roots `savelink_root` and
  `D4-WSF-SAVE-RT_root` carry; set by this step on its own exe path only): same AV.
- With the tool sandbox disabled (`re/NEXT_SESSION.md:68`): same AV.
- The root is byte-identical to `original/` except `mashed_re_dev.asi` (265 files, same sizes, same SHA-256 on `MASHED.exe`,
  `videocfg.bin`, `gamesave.bin`, `d3d9.dll`, `dinput8.dll`, `mashed_qol.asi`).

Fault (Frida exception handler on a spawned PID, no Interceptor): `access-violation` at `MASHED.exe+0x1baf48` (`0x005baf48`),
read of `0x78`, backtrace `0x00462924` <- `0x004c7753` <- `0x004c1bf8` <- `0x00428bc5` <- `0x00496641`.
`verify/savelink_20261023/RESULT_savelink_roundtrip.md:33` records this site as `mov dl,[eax+0x78]` on a NULL first argument
(a NULL audio object).

Machine state at the time: the only audio endpoint is `Philips FTV (NVIDIA High Definition Audio)` with PnP Status `Unknown`
(`Get-PnpDevice -Class AudioEndpoint`); `Audiosrv` and `AudioEndpointBuilder` are running. [UNCERTAIN] that the missing HDMI
audio endpoint (TV off) is what leaves the audio object NULL. Evidence: the NULL-audio signature, plus the timing (the last good
original boot on this machine was 2026-10-26 23:39, the `D4-WSB-C4-BVH` shadow; NARROW-FIX crashes from 02:15 on,
`D:/MashedWT/D4-WSB-NARROW-FIX_data/stock_run.txt`, `noasi_run.txt`). Missing: a boot with an active audio render endpoint.
The standalone exe is unaffected (the gates above ran).

**Unblock:** an active audio render device on LIVING-PC (TV on, or any other output). Then, from this worktree with the
sandbox disabled, in this order:

```
export MASHED_ROOT=D:/MashedWT/D4-WSB-BVH-WRITESET_root
TS=C:/Users/maria/Desktop/Proyectos/.claude/scripts/testslot/proyectos_test_slot.py
# 1. write-set shadow, 9 rows (PREREG 2)
py -3.12 $TS py -3.12 verify/wsbbvh_20261027/run_shadow.py 0055fea0 0055ff90 00561040 00561390 00564040 00564c80 00565260 00563e70 00565200
# 2. Quick Battle racediff re-reads with v6 (PREREG 3)
py -3.12 $TS py -3.12 verify/wsbbvh_20261027/run_bvh2.py 00564c80 00565260
# 3. census racediff (PREREG 4)
C4_SCENARIO=census C4_WINDOW="1 3000" py -3.12 $TS py -3.12 verify/wsbbvh_20261027/run_bvh2.py 00563e70 00565200
```

After that, the PREREG promotion rule applies per row (a C4-CLEAN racediff AND a CLEAN write-set leg), through re-classify.
Candidates if both legs come back clean: `0x0055fea0`, `0x0055ff90`, `0x00561040`, `0x00561390`, `0x00564040` (racediff already
C4-CLEAN in `verify/wsbbvh_20261026`, only the write-set leg missing); `0x00564c80`, `0x00565260` (racediff re-run under the
static proofs above); `0x00563e70`, `0x00565200` (census racediff). The census driver (`run_census` in `c4_racediff_v6.py`) has
not executed yet, so its first run is also its pilot.

## Where the same blind spot hits BROAD and SOLV (lead ask)

`scan_blindspot.py` -> `blindspot.tsv`: every C3/C4 row in the three bands, the ShadowAB template its wrapper uses,
and the count of non-ESP stores in the original body. `blind = YES` = wrapper is `Run` (return only) and the body
stores outside its frame, so a return-only shadow A/B never compared that row's out-memory.

| band | blind rows (C3, `Run`, stores / string stores) |
|---|---|
| BROAD (D4-WSB-C4-BROAD) | `0x0055a1f0` 31/1, `0x0055ab30` 1/0, `0x0055bae0` 3/0, `0x0055e050` 45/0 |
| SOLV (D4-WSB-C4-SOLV) | `0x00568990` 9/0, `0x00568dd0` 12/0, `0x00568fd0` 30/0, `0x0056b7a0` 6/0, `0x0056ba30` 4/0 |
| BVH | none left after this branch's swap |

Also to re-scope, not blind by this rule but not fully covered either:

- BROAD has **7 C3 rows with no shadow wrapper at all** (`wrapper = none`); their C3 does not rest on a shadow A/B, so
  the C4 step must establish out-memory some other way (store scan category A, or add a RunTracked wrapper).
- `RunRegion` rows (SOLV 4, BVH 1) compare one hand-declared region only. Each needs the check done above for
  `0x00565200`: every store of the body inside that region.
- Of those 7, 5 bodies store outside their frame (`0x00558100` 4, `0x00558140` 3, `0x0055c000` 3, `0x0055c4f0` 1,
  `0x0055c810` 38). BROAD also has 2 rows **already C4** with stores and no shadow wrapper (`0x0055ac00` 2, `0x0055e200` 7).
  Their promotion evidence should be checked for an out-memory leg.
- BROAD 4 and SOLV 1 rows are `Run` with no store outside their frame: return-only is complete for them.

Re-scope suggestion for D4-WSB-C4-BROAD / SOLV: swap the 9 blind rows to `RunTracked` first (same harness change as
here, gates re-run), then run the census write-set shadow next to the racediff, as this step does.

## Files

Committed: `PREREG_D4-WSB-BVH-WRITESET.md`, `make_tool2.py`, `c4_racediff_v6.py` (generated), `run_bvh2.py`,
`run_shadow.py`, `scan_argwidth.py`, `disasm_lin.py`, `argwidth.txt`, `scan_blindspot.py`, `blindspot.tsv`,
and no runtime summary (no runtime leg produced data), this file.

Raw (not committed), `D:/MashedWT/D4-WSB-BVH-WRITESET_data/`. Failed shadow boots are in `failed_boot1/` (compat env missing)
and `failed_boot2/` plus the per-RVA folders (later attempts). Every spawned PID was killed by PID by the tools. No MASHED or
`mashed_re.exe` from this step is left running.

| file | SHA-256 |
|---|---|
| `gates/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `gates/H_DEF.gates.csv` | `364350aea89110f291569f42fd7ae0de19704a6b3c95e6f29d7e26b4af9ce7e4` |
| `gates/H_DEF.ptrace.log` | `520528908f8dc61f538902ad09a8ccb5ca46fe23d112859a436da6ace7eab757` |
| `gates/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `gates/B_DEF_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `gates/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `gates/gates_run2.log` | `25a0f7efdffcc083ddec1e49b8af95a0aa26d6da8e7ea79e04e4d825bac22f77` |
| `failed_boot2/0055fea0/shadow_transcript.txt` | `88cbec05838b224d7899c9f9020496439cba12c88a53d1476c489d93b6479d09` |
| `0055ff90/shadow_transcript.txt` | `f974cabbafea37d0a51856b7be4123530837b87866f722e2ed644e240253f485` |
| `shadow_batch2.log` | `22c2acf9d7cca84e3a47de4ab1cdf9f1a993bccec0ffe5cb08e7a2847df172d6` |

Build: `mashed_re.exe` `7f3ce5aa920230482cb3bfe6146f2a818edae3d28b26a3ffb917969a62af382a`, `mashed_re_dev.asi`
`5d18fbc80373bf23b40c347db517e788ee6a70a4cae734bcb4d2bb02b9c2503a` (copied into the private root only), `mashed_re_dev.map`
`ba20e81816abed13485db63f04f11adb89f1321ac99c80dbea37b8e1c3b786f5`. `original/mashed_re_dev.asi` not touched.
