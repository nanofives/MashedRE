# SCRIBE_QUEUE fragment — batch_aj session 1 (aj_s1)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

## Queued

2026-06-02  aj_s1  bucket=re/analysis/bucket_input_dinput_0047b860_0049b300  confidence=C1->C2  rvas=0047b860,0047b880,0047b8a0,0047b8d0,00495520,00495790,004957a0,00495870,00495e80,00495fe0,00496040,00496100,00496320,00496530,00496910,00497450,0049b300

## Notes for the sweep

- **Count**: 17 RVAs, 17 plates authored in the bucket dir. None drift-skipped
  (all were `input/C1` per their existing first-pass plates at session start;
  none already C2+). Opened Mashed_pool0 read-only; `program_close`d; lock released.
- **Subsystem confirmation**: all 17 CONFIRMED `input` (no reclassifications).
  The prompt's working hypothesis holds and splits cleanly in two:
  - **DirectInput8 device subsystem** (0x00495xxx–0x00497xxx + the 0049b300
    thunk): joypad enumeration, per-joypad poll/cook, keyboard create+poll,
    per-player input mapping. Library CONFIRMED dinput8.dll — 0049b300 is a pure
    import thunk `JMP [0x005cc014]` to `dinput8.dll!DirectInput8Create`
    (`thunk=true`, disassembled live), sole caller FUN_00495530 (DInput init).
  - **Input-Lua glue** (0047b860/880/8a0/8d0): the open / close / call / load-file
    wrappers around the embedded Lua VM. **Cross-ref batch_aj session 2**
    (bucket_input_luajoy_004b64e0_004c06c0): aj_s2 identifies the exact callees
    these four invoke — FUN_004b7330 (lua_newstate), FUN_004b7480 (close),
    FUN_004b7a70 (load-chunk exec), FUN_004c0510 (luaopen_base). So 0047b860 =
    open VM + base lib; 0047b880 = close + null the handle; 0047b8a0 = run loaded
    chunk + bool(result==0); 0047b8d0 = fopen("rb")/fread a file into a 32K stack
    buffer then run it via 0047b8a0. Shared handle global = DAT_006bf1e0.

- **Cluster role map** (DInput subsystem):
  - getters: 0x00495520 (IDirectInput8A* DAT_00771e78), 0x00495790 (joypad count
    DAT_00772fac), 0x00496910 (active-player count DAT_00772ffc),
    0x00497450 (player-active predicate, base DAT_007e96fc, stride 0x80 dwords).
  - joypad table accessor: 0x004957a0 (per-joypad/per-axis info; 3 tables
    @0x00772150 / 0x007721d0 / 0x00772290; strides 0x89 and 0x112=2*0x89).
  - enumeration: 0x00496040 (`IDirectInput8::EnumDevices` vtbl[+0x10], class 4
    GAMECTRL, callback LAB_00495ee0, flags 1 ATTACHEDONLY) writes DAT_00772fac.
  - per-joypad poll: 0x00495870 (Poll/Acquire/GetDeviceState, button bitmap,
    POV→unit-vector, deadzone, unit-clamp, 8-sample average); 0x00495fe0
    (poll-all loop, `this` = 0x00771e88 + i*0x448); 0x00495e80 (teardown:
    Unacquire+Release+zero per joypad).
  - keyboard: 0x00496320 (CreateDevice GUID_SysKeyboard + SetDataFormat +
    SetCooperativeLevel → device DAT_00772fb8 from interface DAT_00772fb4);
    0x00496100 (GetDeviceState 256 keys → 256-bit bitmap, re-acquire on fail).
  - per-player cook: 0x00496530 (snapshot/clear per-player block @0x007f1038,
    13× FUN_00497310 reads, analog/digital axis cook ×1/255, player-1 echo).

- **Live-data evidence read this session (Mashed_pool0)**:
  - GUID @0x005d09ec = bytes `61 2b 1d 6f a0 d5 cf 11 bf c7 44 45 53 54 00 00`
    = `{6F1D2B61-D5A0-11CF-BFC7-444553540000}` = **GUID_SysKeyboard**.
  - fopen mode @0x005cf010 = `72 62 00 00` = `"rb\0"`.
  - 00495870 floats: 0x005cfee8 = 0x3e088889 (axis scale ≈ 0.133333),
    0x005cc320 = 1.0f (bias + unit-clamp), 0x005cd050 = 0.125f (1/8 average),
    0x005d757c = 0.0f (deadzone default).
  - 00496530 scale 0x005ceb90 = 0x3b808081 = **1/255.0f**.
  - IAT slot @0x005cc014 = dinput8.dll!DirectInput8Create.

- **Corrections to prior C1 plates** (sweep should prefer these C2 values):
  1. 00496320 cooperative-level flags `6` = `DISCL_FOREGROUND (0x4) |
     DISCL_NONEXCLUSIVE (0x2)` — prior plate mislabeled 0x4 as BACKGROUND
     (DISCL_BACKGROUND is 0x8).
  2. 00495870 re-acquire test constant `-0x7ff8ffe2` = `0x8007001E` =
     **DIERR_INPUTLOST** (HRESULT_FROM_WIN32(ERROR_READ_FAULT 0x1E)) — prior plate
     said DIERR_NOTACQUIRED.
  3. 00495870 axis scale is ≈0.133333 (0x3e088889), NOT the prior-guessed 1/16384;
     the moving-average factor IS 0.125 (1/8), prior guess confirmed.
  4. 00495870 direct callees are only FUN_004b6480 (+ __security_check_cookie
     injection); prior plate's FUN_004987b0 entry was incorrect.
  5. 004957a0 axis-count bound DAT_00771e80 is statically 0x00000000
     (runtime-populated); prior plate's "= 0x10" is a runtime observation, not
     statically citable.

- **Resolved this session**: prior U-T3-027 (how FUN_00495870's ESI/`this`
  advances) — RESOLVED by disassembling 00495fe0: `MOV ESI,0x771e88` then
  `ADD ESI,0x448` per iteration. Recorded in the 00495fe0 plate.

- **Uncertainties carried** (all data-semantic / decompiler-artifact,
  NON-BLOCKING for C2 of these control-flow-complete reads): U-0308, U-0309
  (0047b860/8d0 Lua-handle + unbounded fread vs 0x8000 buffer), U-2407 (004b7a70
  identity — being pinned by aj_s2), U-T3-026 (003495870 axis-range — scale value
  now known, the device range-property source is the open question),
  U-T3-028/U-T3-029 (00496040 decompiler call-conflation + EnumDevices callback
  LAB_00495ee0 identity), U-T3-030 (00496320 SetCooperativeLevel HWND arg),
  U-T3-031 (00496530 per-player struct layout). **One NEW uncertainty needs a
  U-ID** (NOT minted — sweep owns numbering): the 004957a0 static-vs-runtime
  DAT_00771e80 discrepancy + meaning of the 0x89/0x112 strides.

- **Stubs noted** (NOT minted — sweep owns numbering): pre-existing S-0300..S-0304
  (Lua leaf callees), S-2400 (004b7a70), S-T3-008 (004b6520). **needs-S-ID** for:
  FUN_004b6480 (zero/memset helper, shared 00495870/00496100), FUN_004987b0
  (printf-style logger, shared across cluster), LAB_00495ee0 @0x00495ee0
  (EnumDevices callback — writes joypad count + per-joypad records; recommend its
  own follow-up bucket), FUN_00497310 (per-binding input read, 13× in 00496530),
  FUN_004a2c48 (float→int convert, 00496530 player-1 packing).

- **needs_function_create = NONE.** All 17 RVAs returned non-null `function_at`;
  0049b300 already recognized as `thunk=true` named `DirectInput8Create`.

- **No new U-IDs / S-IDs / arg_types minted. No Frida, no build, no re-classify,
  no hooks.csv write.** Per author-only mission: only the bucket dir, this
  fragment, and `.pool_slot_aj_s1` were created.
