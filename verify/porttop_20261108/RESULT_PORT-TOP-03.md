# RESULT PORT-TOP-03: RW API binding layer on librw (L-96 top-down lane, L-98, PLAN section 7)

Verdict: GREEN. Under MASHED_ORIG_TOP=1 the exe now brings RenderWare up and down through the matched original
bodies: WinMain 0x00492370 -> SubsystemInit 0x00492270 -> engine bring-up 0x00493710, and 0x00493550 ->
SoftwareTidyUpBeforeExiting 0x004938c0, with every RW core call they make bound to librw in one TU
(`LibRw/RwApiBind.cpp`). Two of TOP-02's stand-ins are gone (`HandTop_InitDevice` for 0x00492270,
`HandTop_DeviceShutdown` for 0x00493550) and the RW texdict destroy 0x004c5930 is bound to librw. The knob path boots
to the main menu, runs 3000 frames and exits 0. The 12 nav-demo captures with MASHED_DETERMINISTIC=1 are identical
knob ON vs knob OFF. Default gates: unchanged, G-CTRL `8284be46/67931834/31fe043e` and B `1d5bf4f9` x3 (the merge_train.json refs).

Base: `orch/PORT-TOP-02` @ `6cd03090b` (neither TOP-01 nor TOP-02 is on `orch/integration` yet, see "Merge state").

## The binding layer (`mashedmod/src/mashed_re/LibRw/RwApiBind.cpp`)

Exe-only TU compiled in isolation with the librw include path (`mashedmod/build.bat`, like `RwBridge.cpp`), added to the
rva-body scanner's isolated-TU list (`scripts/rva_body_scan.py`). Each function has the C signature the matched source
declares (cdecl, int-sized arguments) and cites the RW 3.x API it implements.

The device: in the original, RwEngineStart creates the D3D9 device (RW's D3D9 driver) and RwEngineStop releases it. In
the exe the device is the exe's own (`InitD3D9`) and librw adopts it (deps/librw/MASHED_PATCHES.md P1). So the
RwEngineStart binding creates the exe device (`PortTop_HandCreateDevice`, the device half of `HandTop_InitDevice`) and
starts librw on it, and the RwEngineStop binding stops librw then releases the exe device (`ShutdownD3D9`).
To place each librw phase at the original's call site, `RwBridge.cpp EngineStartAdopted` is split into
`EnginePhaseInit / EnginePhasePlugins / EnginePhaseOpen / EnginePhaseStartAdopted`, and `RwRaceSubmit.cpp
RaceSubmit_Init / RaceSubmit_Shutdown` into prologue / epilogue / scene halves
(`RaceSubmit_InitOnStartedEngine`, `RaceSubmit_ShutdownSceneOnly`). The default path calls the same pieces in the old
order with the same log lines.

### Bound to librw (kind `bound` in BINDINGS.tsv)

| rva | RW 3.x API | exe symbol | librw | on the knob path |
|---|---|---|---|---|
| `0x004c32b0` | RwEngineInit | `MashedRw_EngineInit_004c32b0` | `rw::Engine::init()` | yes (0x00493710) |
| `0x004c30b0` | RwEngineOpen(params.displayID) | `MashedRw_EngineOpen_004c30b0` | `rw::Engine::open({window})` + last subsystem | yes |
| `0x004c2fb0` | RwEngineStart | `MashedRw_EngineStart_004c2fb0` | exe `InitD3D9`, `setAdoptedDevice`, `rw::Engine::start`, race renderer camera / state block | yes |
| `0x004c2f60` | RwEngineStop | `MashedRw_EngineStop_004c2f60` | race renderer scene teardown, `rw::Engine::stop`, exe device release | yes (0x004938c0) |
| `0x004c3040` | RwEngineClose | `MashedRw_EngineClose_004c3040` | `rw::Engine::close` | yes |
| `0x004c3270` | RwEngineTerm | `MashedRw_EngineTerm_004c3270` | `rw::Engine::term` | yes |
| `0x004c2f00` | RwEngineGetCurrentMode | `MashedRw_EngineGetCurrentMode_004c2f00` | `rw::Engine::getCurrentVideoMode` | yes |
| `0x004c2ed0` | RwEngineGetModeInfo | `MashedRw_EngineGetModeInfo_004c2ed0` | `rw::Engine::getVideoModeInfo` (refRate / format written 0: librw's VideoMode has 4 fields) | yes |
| `0x004c5c80` | RwTexDictionarySetCurrent | `MashedRw_TexDictionarySetCurrent_004c5c80` | `rw::TexDictionary::setCurrent` (0x00493710 passes 0) | yes |
| `0x004c5930` | RwTexDictionaryDestroy | `MashedRw_TexDictionaryDestroy_004c5930` | `rw::TexDictionary::destroy` (covers its callees `0x004c5a60`, `0x004d8060`) | bound, not reached (DAT_0077195c stays 0) |
| `0x004c5c00` | RwTexDictionaryFindNamedTexture | `MashedRw_TexDictionaryFindNamedTexture_004c5c00` | `rw::TexDictionary::find` | no (DisplayInit callee, for PORT-TOP-04) |

Two exe RW bodies exist but are not the binding: `RwEngineGetCurrentMode` (`MatchS19_004c2f00.cpp`) forwards raw to
`0x004c2c90`; the matched RW core bodies read RwGlobals at `0x007d3ff8`, which the exe maps zero-filled.

### Shims: no librw equivalent (kind `shim`)

| rva | what the original does | shim |
|---|---|---|
| `0x00550350` RtFSManagerOpen(-1) | RenderWare file-system toolkit | returns 1 (librw has no RtFSManager; the exe opens files through the CRT and its piz reader) |
| `0x00550390` RtFSManagerClose | closes every installed file system (`re/match/s24/00550390.c`) | no-op |
| `0x004c9f50` | stores its argument at `0x007d4134` (1) | keeps the value |
| `0x004c9eb0` DeviceModeBestBelowSet | stores `0x3c` at `0x006181c4`, lowers it to the best adapter refresh rate | keeps the value (the exe device is windowed) |
| `0x004c9f60` | stores its argument at `0x007d413c`, pushes it to the device (vtable `+0x50`) or sets 4 flags | keeps the value |
| `0x004cbc60/70/80/90` | set / get the dwords at `0x007d4598` / `0x007d459c`; 0x00493710 installs `0x00493700` / `0x004936f0` | keep / return the values; librw is not told (the two installed functions have no exe body and no exe code reads the globals) |
| `0x005584c0` / `0x00558470` | allocate / free two RW-heap buffers (`0x1c000`, `0x3000`) into `0x007dc8b8` / `0x007dc8bc` | return 1 / no-op (no exe reader) |

No UNCERTAINTIES row was filed: each shim is a port decision with its reason in the source comment, not a hole in what
the original does.

### RW rows left as `rw` (37)

Not on the knob path: their callers are still stand-ins (AppInitialiseOnBootup `0x00402750`, AppDestroy `0x00402a40`,
frame render `0x00492e90`) or under them, and for most of them the RW 3.x identity is not confirmed from the matched
source (example: `0x004e45b0`, tracked as RwFrameRemoveChild, takes two arguments and indexes a plugin offset at
`0x007d7174`). Each row's note in BINDINGS.tsv says so. Exceptions with exe bodies: `0x004c19f0` (FrameDispatch.cpp),
`0x004cbd30` (RwWorldStream.cpp / RwStreamCore.cpp), `0x00534b60` (RfxEmitter.cpp, called). `0x004c57a0` is bound by
PORT-TOP-01 (`MashedRw_MatrixCreate_004c57a0`, `rw::Matrix::create`) on `orch/PORT-TOP-01`; on this branch it is still
the raw forwarder inside the guarded MainLoopInit until that branch merges.

## What the knob path runs now

`Match/PortTop_00493710.cpp` (new, exe-only) compiles `re/match/s15/00493710.c` (MATCH-S15 h8, best 94.2%, the only
source of this RVA) as `Orig_00493710`. Its game callees bind in the PLAN 7 order:

| rva | kind | exe symbol |
|---|---|---|
| `0x005c9d00` | port | `GetRaceEndTrigger` |
| `0x004963e0` | stand-in | `PortTop_CfgLogError` (TOP-02) |
| `0x00495780` HardwareInstallFileSystem | missing | no-op (`0x004955d0` unported, no RtFSManager) |
| `0x00495270` HWNDGet | matched | `HWNDGet` (`MatchS16_00495270.cpp`, now in the exe list) -> `0x00499710` `Global7e9584Get` (matched, now in the exe list) |
| `0x00493640` RenderwareAttachPlugins | stand-in | `MashedRw_AttachPlugins_00493640` = librw plugin registration, between Engine::init and open |
| `0x004951f0` HardwareSetVideoMode | stand-in | returns 1; InitD3D9 picks the windowed mode in the RwEngineStart binding |
| `0x0045b350` RwInitNullStub | matched | `RwInitNullStub` (`MatchS09_0045b350.cpp`, now in the exe list) |
| `0x004a2cbd` printf | crt | line to `mashed_re.log` |
| `0x00493600` | missing | returns 1 (`0x004ce790` and its 4 function arguments unported) |
| `0x004936f0`, `0x00493700` | missing | pointer arguments only, never called |

Wrappers given a `MASHED_STANDALONE` branch (the .asi keeps its raw-address calls): `MatchS15_00492270.cpp`
(0x00493710 -> `PortTop_Bound_00493710`, 0x004921d0 / 0x00428590 -> missing stubs), `MatchS15_00493550.cpp`
(0x004938c0 -> `SoftwareTidyUpBeforeExiting` by name), `MatchS15_004938c0.cpp` (its 5 RW calls -> RwApiBind).
DisplayInit 0x004921d0 and ViewportInit 0x00428590 are not run (missing stubs): their exe bodies forward 12 and 5
callees to raw addresses. That is PORT-TOP-04 work, as is the AppInit / AppDestroy subtree.
The `.rdata` seed (`PortTop_RdataSeed.inc`) gains the 9 strings 0x00493710 reads (`gen_rdata_seed.py` over
00492370.c, 00493900.c, 00499ba0.c, 00493710.c: 25 rows).

## Knob-ON boot and captures

Exe: this branch's build `3b15c75f...` (commit `020014800`), private roots `D:/MashedWT/PORT-TOP-03_rootK` (knob runs)
and `D:/MashedWT/PORT-TOP-03_root` (gates), every run under `mashed_lock.py run --label PORT-TOP-03`, own PIDs only.
Driver `verify/porttop_20261108/knob_boot.py` (now takes `PORTTOP_OUT`).

1. `MASHED_ORIG_TOP=1 MASHED_DET_FRAMES=3000` (`knob_on_1`): exit 0 after 3000 frames, 53.6 s. Trace, in order:
   "Calling RwEngineInit" -> `rw::Engine::init ok`, "Calling RtFSManagerOpen" -> shim, HardwareInstallFileSystem
   missing, "Calling RenderwareAttachPlugins" -> librw plugins, "Calling RwEngineOpen" -> `rw::Engine::open ok`
   (the 0x00499ba0 window), "Calling HardwareSetVideoMode" -> stand-in, "Calling RwEngineStart" -> exe device adopted,
   `rw::Engine::start ok`, the two original printf lines (`Using video mode 0 : 3840x2160`: librw reports the adapter's
   desktop mode as mode 0), `0x00493710 returned 1`, `SubsystemInit (matched) returned 1`, AppInitialiseOnBootup
   stand-in, MainLoopInit faults in its guard (PORT-TOP-01, unchanged), 3000 frames, then "Calling
   SoftwareTidyUpBeforeExiting" -> RtFSManagerClose shim, `RwEngineStop -> rw::Engine::stop, exe device released`,
   `RwEngineClose`, `RwEngineTerm`, "Calling HardwareExitApplication", `WinMain 0x00492370 returned 0 after 3000 frames`.
2. Nav demo, `MASHED_NAV_DEMO=1 MASHED_DETERMINISTIC=1`, the 13 NAV_DEMO steps on every run (`done ok=1 depth=0
   screen=1 cur=2`), exit 0. Four runs, `mashed_re_settings.bin` restored from one baseline before each:
   knob ON, knob OFF, knob OFF, knob ON. **All 12 captures byte-identical across the four runs**
   (`walk_02_mainmenu.bmp` sha256 `24249b64...`, the same file as TOP-02's).
   - Method note: a first set without the reset differed in a 10x10 px area of `walk_10_sound` / `walk_11_sound_adjusted`
     (x 447..476, y 279..288) in all three pairs, knob OFF vs knob OFF included. The nav demo adjusts the sound volume
     and the exe saves it to `mashed_re_settings.bin` in the root, so each run started one slider step further (shift of
     10 px per run, in run order). Not the knob. TOP-02 used two roots, so its pairs did not see it.
   - Not done: a draw-list diff on the menu (PLAN 7 lists it for this row). The captures are pixel-identical, which covers
     the composition at the 12 settled screens.

## Default gates (knob OFF)

Exe `3b15c75f` (commit `020014800`), `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` under
`test-slot` + `mashed_lock.py run --label PORT-TOP-03`, root `D:/MashedWT/PORT-TOP-03_root`. All four arms exit 0 and
self-exited (H_DEF 249 s, each B arm 22 s).

| gate | this branch | merge_train.json ref |
|---|---|---|
| G-CTRL | `8284be46/67931834/31fe043e` | `8284be46/67931834/31fe043e` |
| B | `1d5bf4f9/1d5bf4f9/1d5bf4f9` | `1d5bf4f9` x3 |

The first gate attempt did not run: the session moved license mid-wait and the background job was lost, and the
retry stopped on a missing output folder (run_arms.py does not create `A4_OUT`). The third attempt is the one above.

Build: `mashedmod\build.bat` with MASHED_NO_DEPLOY=1, both targets OK. rva-lint: "OK -- no NEW duplicate bodies (1
known)". asm-lint OK.

## Port column (backfill_exe_file.py then backfill_port.py)

7 rows changed, none dropped: `0x00492270 SubsystemInit`, `0x00493550 thunk_EngineStopDispatch`, `0x004938c0
SoftwareTidyUpBeforeExiting` (body -> called), `0x00493710` (none -> called, new exe TU), `0x0045b350 RwInitNullStub`,
`0x00495270 HWNDGet`, `0x00499710 Global7e9584Get` (none -> called, shared wrappers now in the exe). `called` is the static
reach from the exe WinMain through the knob branch; at runtime these bodies execute only with MASHED_ORIG_TOP=1.
No tracker leg (no C-level moves).

## Merge state

`orch/PORT-TOP-01` and `orch/PORT-TOP-02` were both in the merge train and not on `orch/integration` when this branch
was cut and when it was handed in, so the `merge orch/integration` step was not possible yet. Expected conflicts when
TOP-01 lands: none in code expected (TOP-01 appends to the end of `RwBridge.cpp` and adds an include; this branch edits
the middle), and `BINDINGS.tsv`, which TOP-01 creates as its own 9-line file at the same path (add/add against TOP-02's
260 rows, before this branch). Resolution: keep this branch's table and set `0x004c57a0` to kind `bound`, symbol
`MashedRw_MatrixCreate_004c57a0`.

## Next stand-ins, in order

1. PORT-TOP-01 merged: drop the MainLoopInit SEH guard; `0x004c57a0` becomes bound.
2. PORT-TOP-04: DisplayInit 0x004921d0 (bind its 12 callees; RW ones are already in RwApiBind: 0x004c2f00, 0x004c2ed0,
   0x004c5c00; it sets DAT_0077195c, which makes WinMain reach the bound 0x004c5930), ViewportInit 0x00428590, then
   AppInitialiseOnBootup 0x00402750 / AppDestroy 0x00402a40. Their RW calls (the 37 `rw` rows) get bound as their
   callers come off stand-ins, each identity confirmed from its matched source first.
3. HardwareInstallFileSystem 0x00495780, 0x00493600 and HardwareSetVideoMode 0x004951f0 stay stand-in / missing while the
   exe has no RtFSManager, no image-format registry and its own device selection.

## Files, raw data

Committed: `RESULT_PORT-TOP-03.md` (this file), `BINDINGS.tsv` (updated), `update_bindings_top03.py` (the update,
idempotent), `knob_boot.py` (PORTTOP_OUT). Raw data in `D:/MashedWT/PORT-TOP-03_data`:

| file | sha256 |
|---|---|
| mashed_re_gated.exe (this branch's build, commit 020014800) | 3b15c75fcfc08c395e503a831371283470bdb606401005320d1abc8cd40dacc0 |
| build2.log | e7dcb9410f4d403e572fbb7cdbb35aab881041edeea1e5c3a016385292156a01 |
| knob_on_1.log | 928efba3eccb2dc69b8d556d9ef261fcfa306ffcd8dbc94df85e4db89afa22ec |
| navdemo_knobdetB.log / navdemo_knobdetB2.log | 6c2a675dc1e9c649ca639761aa00f8c940246c66464be78cb2c25e71fbf9f14d / c4da291d07a64799605be7407f9633bcb1a762ab9333e956de479305f697f61f |
| navdemo_handdetB.log / navdemo_handdetB2.log | bbba3c3903005d0048c41428efd7f87237a3494b8c3c9c1b86cc9b81866dd05c / 1cbc29294b9b1efd352d2feec1078e4212100ee2aa0dfbbc161c1c5564464195 |
| navdemo_*B*/walk_02_mainmenu.bmp (all four) | 24249b644e3e8057271f3f1c252514c7820abec3f3da15d316f39dbf156a9066 |
| settings_baseline.bin | efd5da5862222448af3ee58c255d583f88b5df28e26ae8ec2fe5a837572c1590 |
| navdemo_knobdet.log / navdemo_handdet.log / navdemo_handdet2.log (no-reset set) | 543b4280... / 17a1a1b8... / c9257255... |
| gates_default.log | b8ee02f10a51f4122c8ec82f97ee21e72331b15eac0bdfaa46323eb6f9a1d03e |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | 6793183492e0c0822570ad71254f9fdf27568b77c81a25c56347f1616863d58c |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv (= K1 = K26) | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 |
