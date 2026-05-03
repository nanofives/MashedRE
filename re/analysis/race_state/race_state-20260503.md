# race_state session — 2026-05-03

Session ID: race_state-20260503  
Slot: Mashed_pool10 (read-only)  
SHA-256 anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E ✓

All 8 RVAs from SCRIBE_QUEUE entry `race_state-20260503-0600`.  
All 8 addresses are unrecognized by Ghidra (DataDB); analyzed via listing bytes.  
U-1009, U-1010, U-1012 resolved. U-1013, U-1014 remain open (Frida required).

## Functions mapped

| RVA | Size (bytes) | Summary |
|-----|-------------|---------|
| 0x0042c280 | 27 | Stub: calls FUN_0042bf30 with 6 hardcoded args (0x27f, 0xff210000, 0, 0, 0, 0) |
| 0x0042c2d0 | 6 | Getter: return DAT_0067ecb4 |
| 0x0042c2e0 | 6 | Getter: return DAT_0067ecb8 |
| 0x0042c2f0 | 10 | Setter: DAT_0067ecb8 = param_1 |
| 0x00432080 | 417 | Race trigger scan: checks player flags per mode; fires on condition |
| 0x004331a0 | 152 | Race init/reset: arms trigger, inits 4×210.0f floats, zeros state |
| 0x00448700 | 43 | Loop stub: calls FUN_004464c0(0x00897fe0) ×100; stores 2 params |
| 0x004927c0 | — | Unrecognized (DataDB); U-1013/U-1014 open; Frida required |

## Tiny getters/setters (0x0042c280, 0x0042c2d0, 0x0042c2e0, 0x0042c2f0)

### FUN_0042c280 (0x0042c280) — 6-param stub

```
PUSH 0; PUSH 0; PUSH 0; PUSH 0
PUSH 0xff210000
PUSH 0x0000027f
CALL FUN_0042bf30   ; 0x0042bf30
ADD ESP, 0x18       ; cleanup 6 dwords
RET
```

Hardcoded params (cdecl order, last pushed = first param):  
- param_1 = 0x0000027f (= 639 decimal)  
- param_2 = 0xff210000  
- param_3..6 = 0

FUN_0042bf30 not yet analyzed; S-1066.

### FUN_0042c2d0 (0x0042c2d0) — DAT_0067ecb4 getter

```
MOV EAX, [DAT_0067ecb4]
RET
```

### FUN_0042c2e0 (0x0042c2e0) — DAT_0067ecb8 getter

```
MOV EAX, [DAT_0067ecb8]
RET
```

### FUN_0042c2f0 (0x0042c2f0) — DAT_0067ecb8 setter

```
MOV EAX, [ESP+4]           ; param_1
MOV [DAT_0067ecb8], EAX
RET
```

DAT_0067ecb4 and DAT_0067ecb8 are adjacent race-state integers (4 bytes each).

## FUN_00432080 (0x00432080) — race trigger scan (417 bytes)

### Structure

```
EAX = [DAT_0067eca4]
EBX = 0
[DAT_0067f19c] = 0       ; reset trigger output
if EAX != 0: goto EXIT_ZERO  ; already triggered — return 0

mode = [DAT_007f1a0c]
switch(mode):
  case 1:    goto PATH_1   ; single player at DAT_007f1a14
  case 2:    goto PATH_2   ; 4-slot race loop
  case 0x1000: goto EXIT_ZERO
  default:   fall-through  ; 10-player unrolled scan

UNROLLED: checks 10 players at base 0x007f1041 + i*0x4c (flag1)
          and 0x007f1501 + i*0x4c (flag2)
          for each: if flag1 != 0 && flag2 == 0: [DAT_0067eca4] = 1

PATH_1: player_idx = [DAT_007f1a14]
        if idx == -1: EXIT_ZERO
        if player_A[idx*0x4c] != 0 && player_B[idx*0x4c] == 0:
            [DAT_0067eca4] = 1; goto TRIGGER

PATH_2: loop ESI over 0x007f1a14..0x007f1a54 (stride 0x10, 4 iters)
        EDI = slot counter (0..3)
        call FUN_0040e470(EDI)     ; 0x0040e470
        if returns 1:
            idx = [ESI]
            if idx in [0..11]:
                call FUN_00496900(idx)   ; 0x00496900
                if returns 0: player_A[idx*0x4c] = 1
        (note: DAT_0067eca4 set inside PATH_2 when condition met)
        if [DAT_0067eca4] != 1: EXIT_ZERO

TRIGGER (shared commit block):
        [DAT_0067ecac] = param_1   ; store external handle (resolves U-1009)
        [DAT_0067e844] = 0
        [DAT_0067e9f8] = 0
        [DAT_0067e914] = 0
        [DAT_0067f19c] = 2          ; SET TRIGGER OUTPUT = 2 (resolves U-1009)
        return 1

EXIT_ZERO:
        XOR EAX, EAX; return 0
```

### U-1009 resolution

- DAT_0067f19c = 2: confirmed set in TRIGGER block at 0x004321e6 (`C7 05 9c f1 67 00 02 00 00 00`)
- DAT_0067ecac = param_1: confirmed at 0x004321cc (`A3 ac ec 67 00` — but param_1 read from [ESP+0x0c] at 0x004321c8)
- "Value 2" = trigger has fired. The value is the trigger-output state, not a player count.
- Handle type at DAT_0067ecac: stored as 32-bit int from param_1; semantic type still unknown → U-1073

### Player struct layout observed

Two parallel byte arrays, stride 0x4c per player (confirmed by IMUL EAX, 0x4c at 0x00432194):
- Array A: base 0x007f1041 (flag1)  
- Array B: base 0x007f1501 (flag2)  
- 10 entries (unrolled scan checks addresses through 0x007f11bd/0x007f167d)  
- Distance A→B = 0x007f1501 − 0x007f1041 = 0x4c0 = 10 × 0x4c  

### Deferred callees

- FUN_0040e470 (0x0040e470): per-slot check function; called with slot index; returns 1 when slot is active; D-3114
- FUN_00496900 (0x00496900): called with player index int; returns 0 on success; role unknown; D-3115 [NEW]
- FUN_0042d3a0 (0x0042d3a0): called at trigger commit by both FUN_00432080 and FUN_004331a0; shared pre-commit sub; D-3116

## FUN_004331a0 (0x004331a0) — race init/reset (152 bytes, ends 0x00433237)

### Structure

```
EAX = [DAT_0067eca4]
ESI = 0
if EAX != 0: POP ESI; RET   ; already armed — skip

PUSH EDI
[DAT_0067eca4] = 1           ; arm trigger
CALL FUN_0042d3a0            ; 0x0042d3a0 — shared pre-commit sub

EAX = [ESP+0x0c]             ; param_1 (after 2 pushes: ESI + EDI)
ECX = 0x43520000             ; = 210.0f (IEEE 754 single)
[DAT_0067ebb0] = ECX         ; four race timer floats, all 210.0f
[DAT_0067ebb4] = ECX
[DAT_0067ebb8] = ECX
[DAT_0067ebbc] = ECX

[DAT_0067ecac] = EAX         ; store param_1

EAX = 0
[DAT_0067ebc8] = 0
[DAT_0067ec20] = 0
[DAT_0067ebcc] = 0
[DAT_0067e844] = 0
[DAT_0067e9f8] = 0
[DAT_0067e914] = 0
[DAT_0067ec24] = 0
[DAT_0067ea6c] = 0

ECX = 0x14; EDI = 0x0067ebd0
REP STOSD                    ; fill 0x14 dwords (80 bytes) at 0x0067ebd0..0x0067ec1f with 0

[DAT_0067ea08] = 0

CALL FUN_004348b0            ; 0x004348b0 — pre-tail init; D-3117

POP EDI; POP ESI
JMP FUN_00424920             ; 0x00424920 — tail call (see U-1011)
```

### U-1010 resolution

0x43520000 = 210.0 (IEEE 754 single-precision confirmed).  
Four floats at 0x0067ebb0, 0x0067ebb4, 0x0067ebb8, 0x0067ebbc all initialized to 210.0f.  
Semantic meaning (lap timer default? time limit?) still unknown — they are written here and presumably read by the race timer subsystem. → U-1075 [NEW]

### State globals zeroed at race init

| Address | Notes |
|---------|-------|
| 0x0067ebc8 | unknown |
| 0x0067ec20 | unknown |
| 0x0067ebcc | unknown |
| 0x0067e844 | also zeroed by FUN_00432080 TRIGGER block |
| 0x0067e9f8 | also zeroed by FUN_00432080 TRIGGER block |
| 0x0067e914 | also zeroed by FUN_00432080 TRIGGER block |
| 0x0067ec24 | unknown |
| 0x0067ea6c | unknown |
| 0x0067ea08 | unknown |
| 0x0067ebd0..0x0067ec1f | 80 bytes zeroed via REP STOSD (0x14 dwords) |

### Deferred callees

- FUN_0042d3a0 (0x0042d3a0): shared with FUN_00432080; D-3116
- FUN_004348b0 (0x004348b0): called before tail; role unknown; D-3117
- FUN_00424920 (0x00424920): 607-byte tail call; U-1011 already open; S-1003

## FUN_00448700 (0x00448700) — 100-loop stub (43 bytes)

### Structure

```
PUSH ESI
ESI = 0x64          ; 100 (loop counter)
loop:
  PUSH 0x00897fe0
  CALL FUN_004464c0   ; 0x004464c0 — S-1004 (already known deferred)
  ADD ESP, 4
  DEC ESI
  JNZ loop

EAX = [ESP+8]       ; param_1 (after PUSH ESI)
ECX = [ESP+0xc]     ; param_2
[DAT_00897ffc] = EAX
[DAT_00898000] = ECX
POP ESI
RET
```

### U-1012 resolution

- 100-iteration loop: confirmed (ESI = 0x64 = 100)
- Constant arg to FUN_004464c0: 0x00897fe0 (a pointer into the ~0x00898000 block)
- After loop: stores param_1 → DAT_00897ffc; param_2 → DAT_00898000
- "May be flush/drain loop or fixed-count array iteration": from bytes this is a flush/reset loop (100 identical calls), not array iteration
- FUN_004464c0 still deferred (S-1004); its role determines the flush semantics

## FUN_004927c0 (0x004927c0) — race completion handler [NO NEW ANALYSIS]

Not analyzed this session. Ghidra has no function record (DataDB confirmed).  
Existing uncertainties U-1013 and U-1014 remain open:

- U-1013: DAT_00771980 step counter (0..12), local_30[12] pattern table — needs Frida-trace during completed race
- U-1014: Mass-fill of 156 words at DAT_007f0a40 with value 2; meaning unknown — needs Frida

## Uncertainties resolved

| ID | Resolution |
|----|-----------|
| U-1009 | RESOLVED: DAT_0067f19c=2 confirmed set at 0x004321e6; DAT_0067ecac=param_1 confirmed at 0x004321cc; value 2 = "trigger fired". Handle type still unknown → U-1073 |
| U-1010 | RESOLVED: 210.0f (0x43520000) is race init default for 4 floats at 0x0067ebb0..bc in FUN_004331a0; semantic meaning deferred → U-1075 |
| U-1012 | RESOLVED: exact 100-iteration loop calling FUN_004464c0(0x00897fe0); param_1→DAT_00897ffc; param_2→DAT_00898000 |

## New uncertainties registered

- U-1073: DAT_0067ecac handle type — stored as 32-bit int from param_1 of FUN_00432080; actual type (object ptr? index? flags?) not determined
- U-1074: FUN_0040e470 return semantics — returns 1 under what condition? called with slot-index 0..3
- U-1075: 210.0f floats at 0x0067ebb0..bc — semantic meaning (timer default? lap time limit? countdown?) not determined

## Stubs registered

- S-1066: FUN_0042c280 needs FUN_0042bf30 (6-param target unknown)
- S-1067: FUN_00432080 needs FUN_0040e470 + FUN_0042d3a0
- S-1068: FUN_004331a0 needs FUN_0042d3a0 + FUN_004348b0 + FUN_00424920
- S-1069: FUN_00448700 needs FUN_004464c0 (S-1004 already open)
- S-1070: FUN_004927c0 blocked by U-1013/U-1014 (Frida required)

## Deferred callees

- D-3113: FUN_0042bf30 — called by FUN_0042c280 with 6 hardcoded args; role unknown
- D-3114: FUN_0040e470 — per-slot player check; returns 1 when slot is active; called from FUN_00432080 PATH_2 and FUN_0042c220
- D-3115: FUN_00496900 — called with player index; returns 0 on success; deeper race-state query
- D-3116: FUN_0042d3a0 — shared pre-commit sub; called by FUN_00432080 and FUN_004331a0
- D-3117: FUN_004348b0 — pre-tail init in FUN_004331a0; role unknown
