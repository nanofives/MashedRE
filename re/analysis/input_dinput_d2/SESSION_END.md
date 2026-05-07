# Session MMMMM — input_dinput depth-2

**Date:** 2026-05-06  
**Slot:** Mashed_pool6  
**Bucket:** re/analysis/input_dinput_d2/  
**ID ranges:** U=2287..2306, D=6760..6819, S=2280..2299  

---

## hooks.csv additions (subsystem=input, confidence=C1, status=mapped)

| RVA | notes | file |
|-----|-------|------|
| 004955b0 | bool wrapper for FUN_00495530 (DI init) | re/analysis/input_dinput_d2/004955b0.md |
| 00495520 | getter: returns DAT_00771e78 (IDirectInput8A ptr) | re/analysis/input_dinput_d2/00495520.md |
| 00495580 | IDirectInput8::Release wrapper; logs "Releasing object" | re/analysis/input_dinput_d2/00495580.md |
| 004955d0 | drive enum / disc detection; adjacent to DI cluster; subsystem uncertain [U-2304b] | re/analysis/input_dinput_d2/004955d0.md |
| 00495150 | main input init: QPC+DI8Create+EnumJoypads+KbdDevice+ButtonRemap; returns DAT_007719e4==0; U-2293..2298 | re/analysis/input_dinput_d2/00495150.md |
| 00495120 | QueryPerformanceFrequency→DAT_00771e70/74 | re/analysis/input_dinput_d2/00495120.md |
| 00499710 | getter: returns DAT_007e9584 (HWND) | re/analysis/input_dinput_d2/00499710.md |
| 004960a0 | joypad init wrapper; stores HWND+0x10; sets deadzone 0x3EB33333≈0.35f | re/analysis/input_dinput_d2/004960a0.md |
| 004963b0 | keyboard device setup wrapper; stores IDirectInput8 ptr → FUN_00496320 | re/analysis/input_dinput_d2/004963b0.md |
| 00496040 | IDirectInput8::EnumDevices(DI8DEVCLASS_GAMECTRL,callback,count,ATTACHED); U-2287,U-2299 | re/analysis/input_dinput_d2/00496040.md |
| 00496320 | IDirectInput8A::CreateDevice(GUID_SysKeyboard)+SetDataFormat(c_dfDIKeyboard)+SetCooperativeLevel(NONEXCLUSIVE\|FOREGROUND); U-2288 | re/analysis/input_dinput_d2/00496320.md |
| 00498510 | button remap config; 6 player config blocks @006147e8..006157e8 stride 0x400; dialog 0x67; U-2293,U-2296; S-2282..2284 | re/analysis/input_dinput_d2/00498510.md |
| 00496010 | joypad subsystem reset: releases all+zeroes 0x1120 bytes+clears counts | re/analysis/input_dinput_d2/00496010.md |
| 00495e80 | joypad device shutdown all: Unacquire+Release per joypad; stride 0x448; U-2297 | re/analysis/input_dinput_d2/00495e80.md |
| 00495270 | *param_1 = FUN_00499710() (HWND store helper) | re/analysis/input_dinput_d2/00495270.md |
| 00495790 | getter: returns DAT_00772fac (joypad count) | re/analysis/input_dinput_d2/00495790.md |
| 004960e0 | Acquire(param_1!=0)/Unacquire(param_1==0) for keyboard device DAT_00772fb8 | re/analysis/input_dinput_d2/004960e0.md |
| 00496100 | keyboard state reader: GetDeviceState(256 bytes)→32-byte bitmap; Acquire-retry on failure; U-2298 | re/analysis/input_dinput_d2/00496100.md |
| 00496370 | keyboard device Release: Unacquire+Release+null; logs "_lpDIDKeyboard" | re/analysis/input_dinput_d2/00496370.md |
| 004963e0 | file log write: fputs(param_1, DAT_00772fbc) | re/analysis/input_dinput_d2/004963e0.md |
| 00496400 | file log printf: vsprintf→fputs to log file; 512-byte buf | re/analysis/input_dinput_d2/00496400.md |
| 00496470 | close log file: fclose(DAT_00772fbc)+null | re/analysis/input_dinput_d2/00496470.md |
| 00496490 | open log file "mashed.log"; writes timestamp header; U-2302 | re/analysis/input_dinput_d2/00496490.md |
| 00496530 | per-player joypad state update; EBX=player; copies prev state; reads 9 buttons; axis modes 1/2; player-1 special U-2304; U-2289 | re/analysis/input_dinput_d2/00496530.md |
| 004967e0 | main input poll: poll joypads+keyboard; extract key flags; active joypad count; U-2301,U-2305; S-2280..2281 | re/analysis/input_dinput_d2/004967e0.md |
| 00497310 | get button state (device-agnostic): type-1=joystick/type-2=keyboard; param_2=slot 9..12 analog; U-2291,U-2301 | re/analysis/input_dinput_d2/00497310.md |
| 004957a0 | axis calibration getter: param_1=device, param_2=axis; reads min/max/flags; stride 0x89 per device; U-2292 | re/analysis/input_dinput_d2/004957a0.md |
| 00495fe0 | joypad data poll loop: calls FUN_00495870 for each joypad | re/analysis/input_dinput_d2/00495fe0.md |
| 00495870 | per-joypad raw state: Poll+GetDeviceState(80 bytes DIJOYSTATE); POV→cardinal; deadzone 0.35f; unit-circle clamp; 8-frame axis smoothing; ESI register convention; U-2290,U-2291,U-2300,U-2303 | re/analysis/input_dinput_d2/00495870.md |
| 00497450 | joypad present check: param_1<4 && device type[player*0x80]!=0 | re/analysis/input_dinput_d2/00497450.md |

---

## UNCERTAINTIES.md additions (U-2287..2306)

| ID | type | location | observation | evidence_needed |
|----|------|----------|-------------|-----------------|
| U-2287 | structural | FUN_00496040 0x00496040 | decompiler shows `FUN_00495520(4, callback, ...)` but FUN_00495520 is 0-arg getter; actual call is vtable[4]=EnumDevices on IDirectInput8 ptr | listing_code_units_list 0x00496040..0x00496099 |
| U-2288 | structural | FUN_00496320 0x00496320 | SetCooperativeLevel hwnd shown as &DAT_005d0974 (same as data format addr); likely decompiler artifact from missed param_1 | listing_code_unit_at 0x00496320..0x0049636d |
| U-2289 | structural | FUN_00496530 0x00496530 | unaff_EBX used as player index; register convention not stack parameter | listing_code_unit_at caller site in FUN_004967e0 |
| U-2290 | structural | FUN_00495870 0x00495870 | unaff_ESI used as device data pointer; register convention | listing_code_unit_at caller site in FUN_00495fe0 |
| U-2291 | semantic | FUN_00495870 + FUN_00497310 | DAT_005d757c used as axis snap-to-zero float; expected 0.0f | memory_read 0x005d757c 4 bytes |
| U-2292 | semantic | FUN_004957a0 0x004957a0 | stride 0x89=137 per device in axis calibration arrays; meaning unclear | check DIJOYSTATE2 axis layout; reference_to 0x00772150 |
| U-2293 | semantic | FUN_00495150 + FUN_00498510 | DAT_006147c0 passed as first arg to FUN_00498510; type not confirmed | reference_to 0x006147c0 |
| U-2294 | semantic | FUN_00495150 0x00495150 | return = (DAT_007719e4 == 0); whether this is an error flag or init-complete flag not confirmed | reference_from 0x007719e4 |
| U-2295 | semantic | FUN_00495150 0x00495150 | DAT_007719e8 enum range 0..5 mapped to device-type indices; semantic meaning of each not confirmed | all write-sites of 0x007719e8 |
| U-2296 | structural | FUN_00498510 0x00498510 | 6 player config blocks @0x006147e8..0x006157e8 stride 0x400; layout of each block not confirmed | listing around 0x006147e8; reference_to blocks |
| U-2297 | structural | FUN_00495e80 0x00495e80 | joypad entry stride 0x448 (0x112*4) confirmed by decomp but struct layout not confirmed | listing_data_at 0x00771e88 |
| U-2298 | structural | FUN_00496100 0x00496100 | Acquire-retry path: re-acquires on any GetDeviceState failure; standard DI focus-loss pattern but not confirmed by specific error code | none needed — pattern confirmed |
| U-2299 | structural | FUN_00496040 0x00496040 | LAB_00495ee0 is the EnumDevices callback; first instruction MOV EAX,[ESP+8] confirmed; Ghidra has not defined it as a function | function_create 0x00495ee0 or listing of body |
| U-2300 | semantic | FUN_00495870 0x00495870 | Acquire retry loop: while result == -0x7FF8FFE2; this HRESULT value not matched to a named DIERR_ constant | check dinput.h for value 0x80070022 area |
| U-2301 | semantic | FUN_00497310 0x00497310 | DAT_0077313c used as keyboard bitmap in type-2 path; DAT_00773058 is written by FUN_00496100; relationship/copy unclear | reference_to 0x0077313c |
| U-2302 | semantic | FUN_00496490 0x00496490 | DAT_005d0004: fopen mode string for "mashed.log"; expected "wt" or "at" | memory_read 0x005d0004 4 bytes |
| U-2303 | semantic | FUN_00495870 0x00495870 | _DAT_005cd050: float multiplier applied to 8-frame axis sum (smoothing divisor, expected 1/8 = 0.125f) | memory_read 0x005cd050 4 bytes |
| U-2304 | semantic | FUN_00496530 0x00496530 | DAT_008989c0/c4/a0/a4: player-1-only state; purpose unclear (possibly MIDI or secondary analog) | reference_to 0x008989c0 |
| U-2305 | semantic | FUN_004967e0 0x004967e0 | FUN_004972b0 and FUN_0045b350 called at start of each frame poll; purpose unknown | decomp of both functions |
| U-2306 | semantic | FUN_004955d0 0x004955d0 | function physically adjacent to DI cluster but performs drive/disc detection; subsystem classification uncertain | callers of 0x004955d0 |

---

## STUBS.md additions (S-2280..2286)

| ID | RVA | caller | subsystem | status | date | notes |
|----|-----|--------|-----------|--------|------|-------|
| S-2280 | 0x004972b0 | 0x004967e0 FUN_004967e0 | input | unresolved | 2026-05-06 | 3rd call in every input poll frame; purpose unknown; D-6760 |
| S-2281 | 0x0045b350 | 0x004967e0 FUN_004967e0 | input | unresolved | 2026-05-06 | 2nd call in frame poll; cross-subsystem RVA range; D-6761 |
| S-2282 | 0x00499720 | 0x00498510 FUN_00498510 | input | unresolved | 2026-05-06 | HINSTANCE getter for LoadStringA; D-6762 |
| S-2283 | 0x00495830 | 0x00498510 FUN_00498510 | input | unresolved | 2026-05-06 | fills per-slot default button mappings; D-6763 |
| S-2284 | 0x004971b0 | 0x00498510 FUN_00498510 | input | unresolved | 2026-05-06 | compares/loads saved button config per slot; D-6764 |
| S-2285 | 0x004a2c48 | FUN_00497310 + FUN_00496530 | input | unresolved | 2026-05-06 | button state byte reader (analog threshold path); D-6765 |
| S-2286 | 0x00495ee0 | 0x00496040 FUN_00496040 | input | unresolved | 2026-05-06 | EnumDevices callback (Ghidra label LAB_00495ee0; body starts MOV EAX,[ESP+8]); D-6766 |

---

## DEFERRED.md additions (D-6760..6766)

| ID | RVA | reason | pickup_condition | bucket |
|----|-----|--------|-----------------|--------|
| D-6760 | 0x004972b0 FUN_004972b0 | purpose unknown; called pre-keyboard each frame | after frame-loop subsystem identified | input_dinput_d2-cont1 |
| D-6761 | 0x0045b350 FUN_0045b350 | cross-subsystem; different RVA range | after subsystem of 0x0045xxxx range identified | input_dinput_d2-cont1 |
| D-6762 | 0x00499720 FUN_00499720 | HINSTANCE getter; trivial but not analyzed | next input_dinput sweep | input_dinput_d2-cont1 |
| D-6763 | 0x00495830 FUN_00495830 | default button mapping setup per player slot | next input_dinput sweep | input_dinput_d2-cont1 |
| D-6764 | 0x004971b0 FUN_004971b0 | saved config comparison per slot | next input_dinput sweep | input_dinput_d2-cont1 |
| D-6765 | 0x004a2c48 FUN_004a2c48 | button state byte reader | next input_dinput sweep | input_dinput_d2-cont1 |
| D-6766 | 0x00495ee0 (LAB) | EnumDevices callback; needs function_create first | define function at 0x00495ee0 then analyze | input_dinput_d2-cont1 |

---

## Depth-3 queue

Queue entry: `input_dinput_d2-cont1` — 7 deferred items (D-6760..6766). Functions: FUN_004972b0, FUN_0045b350, FUN_00499720, FUN_00495830, FUN_004971b0, FUN_004a2c48, LAB_00495ee0.

---

## Key findings

1. **Full DirectInput8 init chain confirmed**: QPC freq → IDirectInput8Create → EnumDevices(joypads) → CreateDevice(keyboard) → SetDataFormat(c_dfDIKeyboard) → SetCooperativeLevel.
2. **GUID_SysKeyboard** confirmed at 0x005d09ec; **IID_IDirectInput8A** confirmed at 0x005d0a8c; **c_dfDIKeyboard** confirmed at 0x005d0974.
3. **Frame poll** (FUN_004967e0): polls all joypads then reads keyboard bitmap; max 4 players.
4. **Register conventions**: ESI (joypad device struct ptr in FUN_00495870) and EBX (player index in FUN_00496530) — non-standard calling conventions; callers must set these registers.
5. **Per-joypad struct**: base at DAT_00771e88, stride 0x448 bytes; first field = IDirectInputDevice8*; POV decoded to 8 cardinal/diagonal unit vectors; 8-frame axis smoothing loop.
6. **Button remapping dialog** at template ID 0x67 — shown when FUN_00498510(1, ...) called.
7. **Log file**: "mashed.log" opened by FUN_00496490; all dinput diagnostic output goes here.
8. **Deadzone**: global threshold 0x3EB33333 ≈ 0.35f, set by FUN_004960a0 into DAT_00772fa8.
