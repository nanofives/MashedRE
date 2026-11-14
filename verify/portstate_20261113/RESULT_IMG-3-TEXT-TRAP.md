# RESULT IMG-3-TEXT-TRAP: trap on the original code range on the MASHED_ORIG_TOP path (L-149 a, step 3)

Row `IMG-3-TEXT-TRAP` (D4-IMG, run 44). Worker cmwfdpcza29oklh1nefid12xn, lead cmwf9zx6d1ljulh1nd88fk6k9. Row kind: top-down.
**Branch `orch/IMG-3-TEXT-TRAP` was created from `orch/IMG-2-LOADER` @ `85ab71228`** (not from `orch/integration`):
IMG-2 was enqueued but not yet in `orch/integration` when this row started. **Merge IMG-1 and IMG-2 first (or with this).**
Spec: `verify/portstate_20261113/RESULT_PORTSTATE-IMAGE.md` section 4.

## Verdict: GREEN

- Knob `MASHED_CODE_TRAP` (default OFF), read only on WinMain's `MASHED_ORIG_TOP` branch. `1` = report the first trap
  and exit with the original VA as exit code (the `PortTopCodePtr.cpp` CodePtrStop convention). `2` = a diagnostic
  addition to the spec: report every new (target, return address) whose call site decodes, simulate the return
  (eax 0) and go on, so one boot lists every unbound pointer it reaches. Default path unchanged (section 3).
- **Self-test: PASS, 0 of 6 checks failed**, on both builds and every trap arm: 459/459 pages PAGE_NOACCESS, and one
  probe per report kind through the real handler (read, write, execute, execute-null, breakpoint on a carved page),
  each with the right kind, VA and, for the calls, the slot the call read.
- **ddkobj check: IDENTICAL 1192/1219, DIFFERENT 12, not-in-map 15, units linked 1167/1443, refusals 4
  (wrapper-redefines-globals=4)**. Same tally and same DIFFERENT list as the IMG-2 base.
- **Image-ON knob boot with the trap: the AppDestroy AV of IMG-2 becomes a named report**: `execute original
  0x004568A0 slot 0x005F99B0`, exit code 0x004568A0, from the matched 0x0045b930 (`_Orig_0045b930`), on both builds.
  Mode 2 reaches all 9 entries of the table at 0x005f99b0 (stride 0x40), then `WinMain 0x00492370 returned 0 after
  3000 frames`, rc 0. **Zero `read` / `write` reports** in 3000 frames + teardown (spec section 4's [UNCERTAIN] on
  `.text` read as data, for this boot: nothing read it).

## 1. What changed

| File | Change |
|---|---|
| `mashedmod/src/mashed_re/Compat/CodeTrap.{h,cpp}` (new, exe only, `exe_sources.rsp`, `RVA-LINT: ORIGTOP-ONLY`) | arm, vectored handler, call-site decoder, self-test, held log |
| `mashedmod/src/mashed_re/exe_main.cpp` | WinMain ORIG_TOP branch: `CodeTrap_Arm()` after `PortTopCodePtr_Apply`. `CodeTrap_Log(kLogPath)` after the log truncate, next to `PortTopCodePtr_Log` |
| `mashedmod/src/mashed_re/Compat/StandaloneRvaThunks.cpp` | when armed: `MapRange` leaves trap-range pages alone (protects only the granule pages outside 0x00401000..0x005cbfff, so `.rdata` 0x005cc000..0x005cffff keeps its image bytes and its previous RWX), `Install` calls `CodeTrap_AllowPage` for the thunk's page(s). Not armed (every default-path run): unchanged |
| `re/tools/image/code_trap_report.py` (new) | offline reader: joins `[code-trap]` lines with `re/console/pc_funcs.csv`, `hooks.csv`, `CODEPTR_SLOTS.tsv`, `mashed_re.map` and the image's dword at the slot |

Mechanism (spec section 4): range 0x00401000..0x005cbfff (459 pages; `.text` and `_rwcseg`), inside the B17 pad
(`g_b17_low_arena_pad`), `VirtualProtect` to PAGE_NOACCESS (the log shows the previous protection 0x8). Handler
`AddVectoredExceptionHandler(1, ...)` takes an AV whose address is in the range (kind from `ExceptionInformation[0]`:
8 execute, 1 write, 0 read), an execute AV below 0x10000 (`execute-null`: the image zeroes the IAT 0x005cc000..0x005cc2e8,
spec section 3), and a breakpoint whose address is in the range (a 0xCC byte of a carved page). Everything else returns
EXCEPTION_CONTINUE_SEARCH. Carved B16 pages on this boot: 0x0042B000, 0x0042D000, 0x00472000, 0x00473000 (B16 runs on
ORIG_TOP: `Milestone B16 ... thunks=6/6`, spec risk 5 answered for this boot).

Call-site decoder (execute kinds; the registers at the fault are those right after the call): `E8 rel32`, `FF 15 abs32`,
`FF /2` with register / SIB base and disp8 / disp32, and `call reg` with the load of that register just before it
(`A1` / `8B` abs32, `8B` reg+disp or SIB, skipping `test r,r` and a short jcc; a SIB index clobbered by the load is
recovered from a `mov r,[abs32]` right before). A form is accepted only when the computed slot still holds the target.
The default build's call site is `mov eax,[0x0038d798]; mov eax,[edi+eax]; test eax,eax; je; call eax` and the object
build's `mov edi,0x005f99b0; ... mov eax,[edi]; test eax,eax; je; call eax`. Both decode to the same slots.

One cosmetic note: the self-test's own carve of 0x005CB000 logs "for a B16 thunk". It is the breakpoint probe's carve,
restored right after (zeros, PAGE_NOACCESS, `page restored 1`).

## 2. Checks

| check | result |
|---|---|
| default build (`MASHED_NO_DEPLOY=1`) | `=== Build OK ===`, rva-lint OK (237 known, 0 new), asm-lint OK, portstate-seed OK (272, 3231 B, fnv 0x5ecb7d52). `mashed_re.exe` sha256 `e31086f3...7ee0cf` |
| object build (`MASHED_DDKOBJ=1`, NO_DEPLOY) | `=== Build OK ===`, `mashed_re_ddk.exe` sha256 `facc2c19...972459` |
| `py.exe -3.12 re/tools/ddkobj/ddkobj.py check` | IDENTICAL 1192/1219, DIFFERENT 12 `[0x00401690 0x004332a0 0x0043c5b0 0x0045a3a0 0x00475a00 0x004929d0 0x00493710 0x00493900 0x00498510 0x00498810 0x004c0c20 0x004c0f00]`, not-in-map 15, units linked 1167/1443, refusals 4 (wrapper-redefines-globals=4). Unchanged against the base |
| self-test (in-process, every trap arm, both builds) | PASS, 0 of 6 failed |
| real `original\` | `original/mashed_image.bin` absent; the private root got IMG-2's image file by copy |
| G-CTRL / B / 12x4 captures / menu walk | not run: top-down row, no menu code changed. The merge train runs the default gates |

## 3. Knob boots (`MASHED_DET_FRAMES=3000`, private root `D:/MashedWT/IMG-3-TEXT-TRAP_root`, test-slot outside, `mashed_lock.py run --label IMG-3-TEXT-TRAP` inside)

| arm | exe | env | rc | `[code-trap]` lines |
|---|---|---|---|---|
| def_off | default | none | 0 | 0 |
| def_off_trap1 | default | CODE_TRAP=1 + PORTSTATE_IMAGE=1, no ORIG_TOP | 0 | 0 |
| def_on_img0 | default | ORIG_TOP | 0, WinMain returned 0 after 3000 frames | 0 |
| def_on_img1 | default | ORIG_TOP + IMAGE | 0xC0000005 in AppDestroy (IMG-2's finding, reproduced) | 0 |
| def_on_img1_trap1 | default | ORIG_TOP + IMAGE + CODE_TRAP=1 | **0x004568A0** | armed, self-test PASS, 4 carves, `execute original 0x004568A0 slot 0x005F99B0 ... exit` |
| def_on_img1_trap2 | default | ORIG_TOP + IMAGE + CODE_TRAP=2 | 0, WinMain returned 0 after 3000 frames | 9 execute reports (section 4), 0 read/write |
| ddk_on_img1_trap1 | object | ORIG_TOP + IMAGE + CODE_TRAP=1 | **0x004568A0** | same report, site `_Orig_0045b930+0x38` |
| ddk_on_img1_trap2 | object | ORIG_TOP + IMAGE + CODE_TRAP=2 | 0, WinMain returned 0 after 3000 frames | same 9 reports |

**Default path unchanged.** `def_off_trap1` (masked for addresses, thread id and movie frame counts) is line for line
equal to IMG-2's `def_off_img1` (same env, IMG-2's exe). Against `def_off` (no env) it differs only in the B17
region-map dump (65 vs 43 `COMMIT IMAGE` lines), and IMG-2's own `def_off_img1` shows the same 65, so that dump
difference is not from this row. [UNCERTAIN] why the region map differs with the image env var set on the default
path: no per-page comparison was run.

**Image + trap against image OFF.** Sorted, masked (addresses, heap pointers, wall-clock movie frame counts, the trap /
image / seed / region-map lines removed), `def_on_img1_trap2` equals `def_on_img0` with 0 differing lines, and
`ddk_on_img1_trap2` equals `def_on_img1_trap2` with 0. The trap1 arms equal `def_on_img1` (without the trap) up to their
last line. The only sorted difference left (2 RW_BIND lines) was a filter artifact: a heap pointer containing "B17"
dropped a line. Unfiltered counts are equal (15 `mask 0x40` lines and 6 `12 vertices` lines in both).

The mode-2 continuation after the nine simulated returns is valid for this site: `0x0045b930` calls each entry with no
arguments (`re/match/s09/0045b930.c`), and the boot then runs AppDestroy to `WinMain 0x00492370 returned 0`.

## 4. Unbound code pointers reached by the image-ON boot (input for IMG-4)

From `def_on_img1_trap2` / `ddk_on_img1_trap2` through `re/tools/image/code_trap_report.py`
(`img3_trap_report_default.tsv`, `img3_trap_report_ddkobj.tsv`). All nine are called from the matched powerup teardown
0x0045b930 (`_Orig_0045b930`), slot = 0x005f99b0 + 0x40*i, i < `DAT_005f9bd8` = 9 (image dword). Each slot's image
dword equals the reported target (column `image_dword`), and the nine targets equal the offline read of
`mashed_image.bin` at those slots.

| i | slot | target (original VA) | in CODEPTR_SLOTS.tsv | pc_funcs.csv / hooks.csv |
|---|---|---|---|---|
| 0 | 0x005f99b0 | 0x004568a0 | yes (CP_FIX, stop thunk under MASHED_OT_CODEPTR=1) | FUN_004568a0, C4, port none |
| 1 | 0x005f99f0 | 0x004540c0 | no | DepthChargePerFrameTeardown, C4, port none |
| 2 | 0x005f9a30 | 0x004555d0 | no | no entry: gap between FUN_00455400 (end 0x004555c0) and FUN_00455610; no hooks.csv row |
| 3 | 0x005f9a70 | 0x00457b10 | no | no entry: gap between FUN_004577f0 (end 0x004577f8) and FUN_00457ba0; no row |
| 4 | 0x005f9ab0 | 0x0045a790 | no | no entry: gap between FUN_0045a6b0 (end 0x0045a784) and FUN_0045a950; no row |
| 5 | 0x005f9af0 | 0x0045b2d0 | no | no entry: gap between FUN_0045ae80 (end 0x0045b1f5) and FUN_0045b350; no row |
| 6 | 0x005f9b30 | 0x00454af0 | no | no entry: gap between FUN_00454a30 (end 0x00454a3e) and FUN_00454bd0; no row |
| 7 | 0x005f9b70 | 0x00456ca0 | no | no entry: gap between thunk_FUN_00456c70 (end 0x00456c94) and FUN_00456ce0; no row |
| 8 | 0x005f9bb0 | 0x004530b0 | no | no entry: gap between FUN_00452f70 (end 0x004530a6) and FUN_00453100; no row |

Seven targets (i = 2..8) are not function entries in `re/console/pc_funcs.csv` and have no `hooks.csv` row. They need
a function created at that address (the Ghidra-missed-function route, `decomp_pc.py --create`) before a body can be
matched or bound. No other unbound pointer was reached by this boot: after these nine the boot returned 0.

Not bound here: the spec gives binding to IMG-4 (the CP list) and the body rows after it.

## 5. Raw files (not committed), sha256

```
e31086f3473e3f649980fde19b6b67aa822ab77f73d37a0a1641221ab77ee0cf  D:/MashedWT/IMG-3-TEXT-TRAP_data/default/mashed_re.exe
9304f75a734482bf4512a8081e76bde4916efb99a67f88a4dd1c10e3bc686eac  D:/MashedWT/IMG-3-TEXT-TRAP_data/default/mashed_re.map
facc2c19389dae34ea98f23427569f4615e6cdc04e2194fee91456d7d1972459  D:/MashedWT/IMG-3-TEXT-TRAP_data/ddkobj/mashed_re.exe
81cc77758ffe93ac0b681eb1fed3d63912d25e7d059503af6d4f871dea9afcd8  D:/MashedWT/IMG-3-TEXT-TRAP_root/original/mashed_image.bin
a93a232cd4d91795a926ed365fb122a3ca19a202ff9d73674328f8b46e400eae  D:/MashedWT/IMG-3-TEXT-TRAP_data/boots/def_off.log
f0c7f34545e64a5ebfb8940f88e713fe21c3ac24006eb48cd32d26ce66155c41  D:/MashedWT/IMG-3-TEXT-TRAP_data/boots/def_off_trap1.log
a37ebb9b3cedc76daabd8844090e7dd680152a4bcc1d7846f91b22b1e15957a0  D:/MashedWT/IMG-3-TEXT-TRAP_data/boots/def_on_img0.log
eec13258866e71fa95656aff4d60ce90653a93da618d1a6fd1323582d2aa875e  D:/MashedWT/IMG-3-TEXT-TRAP_data/boots/def_on_img1.log
b6368e9a679bbeec32a3ff711626fec6958f1620c1776157088c1b5a84f5431c  D:/MashedWT/IMG-3-TEXT-TRAP_data/boots/def_on_img1_trap1.log
79f30482c632b5518d9bb210d8b9a4eed7f1a7b6797bae12a5c5c8f235b4b83d  D:/MashedWT/IMG-3-TEXT-TRAP_data/boots/def_on_img1_trap2.log
f2c0fc629ba9bcb0f545bb72589457caecdd633696252623e88fd9d34f27d7d4  D:/MashedWT/IMG-3-TEXT-TRAP_data/boots/ddk_on_img1_trap1.log
b08547d9054fed10c896ca3c7dcd109e28d8d506812c11fa3e4e730f9d412d80  D:/MashedWT/IMG-3-TEXT-TRAP_data/boots/ddk_on_img1_trap2.log
a19cbe89c89fd1e840658c5d6b20453fca8a2863c9527fd3afc8cbb5853e376f  D:/MashedWT/IMG-3-TEXT-TRAP_data/boots.sh
f03f6d9f5b6041b70b71694168308b005e96d42d4f41bef3c5eae3ff98a851de  D:/MashedWT/IMG-3-TEXT-TRAP_data/boots.out
06b8b9d629d37bc7b93f6325a9a21e43a641b5863b87ed37fdfe4433301622cf  D:/MashedWT/IMG-3-TEXT-TRAP_data/build_default.log
0013bc5e9f2447273ebf6d5d98c62585b13784942456e2a34efcbb55c083aac2  D:/MashedWT/IMG-3-TEXT-TRAP_data/build_ddkobj.log
ad2bd7990bf4f01e47adb2b5459938654ca92ea707a0e2dd0153fa90bc6e0943  D:/MashedWT/IMG-3-TEXT-TRAP_data/ddkobj_check.txt
```

`D:/MashedWT/IMG-3-TEXT-TRAP_data/boots_v1/` holds the first pass (same arms, decoder without the `call reg` load
forms: same targets and exit codes, slots reported as `?`).

No promotions, no tracker edit, no new librw binding, no decision row.

## Next

- IMG-4: add the nine slots 0x005f99b0 + 0x40*i (i 0..8) to the CP list with these targets; i = 2..8 first need
  functions created at 0x004555d0, 0x00457b10, 0x0045a790, 0x0045b2d0, 0x00454af0, 0x00456ca0, 0x004530b0.
- IMG-5: mode 2 gives the per-boot list for the knob-path captures; walk the menus with `MASHED_CODE_TRAP=2` to collect
  the pointers the menu screens reach.
- Merge order: IMG-1-EXTRACT, IMG-2-LOADER, then this branch.
