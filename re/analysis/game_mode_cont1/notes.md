# game_mode_cont1 — session GGGG

Session: game_mode_cont1-20260503  
Pool: Mashed_pool4 (read-only)  
Parent: session AAAA (game_mode) → landed RVA 0x0043dfd0 (mapped in timer_d2 as C0, 63 callees).  
Strategy: callees of 0x0043dfd0 as new subset; also checked callers.

## Caller of 0x0043dfd0

Single caller: FUN_00492d30 (0x00492d30–0x00492e39, 265B). Not decompiled this session — D-4868.

## Subset: callees of 0x0043dfd0

63 callees total. 8 already in hooks.csv (0x0045b350 C1, 0x004a2c48 C1, 0x0042f500 C2, 0x00430760 C0-deferred, 0x00430830 C0-deferred, 0x0040ad20 C0-deferred, 0x0042bf30 C1, 0x0040e470 C1).  
Tiny callees (≤10B) skipped: 0x00402f40, 0x0040ad30, 0x0040e170, 0x0040e360, 0x00413f90, 0x00431b30, 0x0042b950, 0x00472640.  
Soft cap (15) hit; remaining 28 deferred to D-4840..D-4867.

---

## Functions covered (cap_count=15)

### 0x0043c000 — FUN_0043c000 (1450B)
Timer tick dispatcher. Called every frame from 0x0043dfd0.  
Guard at top: `if (_DAT_0067e840 == DAT_005d757c)` resets all 19 timer slots — if slot != 1, clears it to 0.

**Timer slot layout** (19 slots in 0x0067e7a8–0x0067e838):  
- Slot 0: state=DAT_0067e7a8, counter=DAT_0067e7ac; action=FUN_004332a0 (S-1640)  
- Slot 1: state=DAT_0067e7b0, counter=DAT_0067e7b4 (byte); no action  
- Slot 2: state=DAT_0067e7b8, counter=DAT_0067e7bc; no action  
- Slot 3: state=DAT_0067e7c0, counter=DAT_0067e7c4; no action  
- Slot 4: state=DAT_0067e7c8, counter=DAT_0067e7cc; action=FUN_0042c960 (S-1642)  
- Slot 5: state=DAT_0067e7d0, counter=DAT_0067e7d4; no action  
- Slot 6: state=DAT_0067e7d8, counter=DAT_0067e7dc; action=FUN_0042f7b0 (S-1641)  
- Slot 7: state=DAT_0067e7e0, counter=DAT_0067e7e4; action=FUN_0042fa00 (S-1643)  
- Slot 8: state=DAT_0067e7e8, counter=DAT_0067e7ec; no action  
- Slot 9: state=DAT_0067e7f0, counter=DAT_0067e7f4 (byte); no action  
- Slot 10: state=DAT_0067e7f8, counter=DAT_0067e7fc (byte); no action  
- Slot 11: state=DAT_0067e800, counter=DAT_0067e804; no action  
- Slot 12: state=DAT_0067e808, counter=DAT_0067e80c; no action  
- Slot 13: state=DAT_0067e810, counter=DAT_0067e814; no action  
- Slot 14: state=DAT_0067e818, counter=DAT_0067e81c; no action  
- Slot 15: state=DAT_0067e820, counter=DAT_0067e824 (byte); no action  
- Slot 16: state=DAT_0067e828, counter=DAT_0067e82c; no action  
- Slot 17: state=DAT_0067e830, counter=DAT_0067e834 (byte); no action  
- Slot 18: state=DAT_0067e838, counter=DAT_0067e83c (byte); no action  

Per-slot dispatch: state==1 → record `FUN_004a2c48()` (frame counter) into counter slot + call action; state==2 → record counter only; else → clear counter to 0.  
U-1647: What game event does each timer slot correspond to? Only slots 0, 4, 6, 7 have action callbacks — semantics unknown.  
U-1648: DAT_0067e840 vs DAT_005d757c gate — what race/game state value does 005d757c represent?

### 0x0043d2a0 — FUN_0043d2a0(int param_1, int param_2) (1302B)
Screen/menu push-pop. param_1 = screen type ID, param_2 = direction (0=push, 1=pop-with-history, 2=reload).

**Screen ID dispatch (param_1):**
- 0x1a (26): copy 2 DAT fields (007f0ef4=007f0eec, 007f0ef8=007f0ef0)
- 0x13 (19): copy 5 DAT fields (007f0f14–007f0f24 from 007f0f00–007f0f10)
- 0x20 (32): DAT_008990d8 = FUN_0040ad20()
- 0x1c (28): loop copy 007f105c table (stride 0x13 ints) into 00899160
- 0x1f (31) or 0x21 (33): copy 4 DAT fields (007f0f44–007f0f50 from 007f0f30–007f0f3c)
- 0x01 (1): FUN_00414120 + FUN_00422b30 + FUN_0040b810 + DAT_0067ea70=1

Navigation stack: DAT_0067e9f8 = stack depth (int).  
Stack entry at `base + depth * 0x10`: 0x0067ed78[depth*0x10] = screen ptr from PTR_DAT_005f7638[param_1].  
Stack entry at +4: param_1 (screen type code).  
Also writes FUN_00432800(depth) to initialize options.  

Calls FUN_0042ad90() (S-1646) twice to get sVar2/sVar3 (player port IDs, -1 if absent).  
Builds display header at DAT_00898acc (color 0xff000000, w=256.0, h=192.0, alpha 0x3f19999a).  
Then loops calling FUN_0042ad90() building slide-in display table (stride 0xd ints at DAT_00898ad8+).  
U-1649: PTR_DAT_005f7638 — screen-pointer lookup table, one entry per screen type code; size unknown.  
U-1650: DAT_0067e844 — set to param_2 at end; likely "last transition direction" flag.

### 0x00432800 — FUN_00432800(int param_1) (648B)
Menu option initializer for navigation stack entry param_1.  
- Sets 12 availability flags at DAT_0067ed84[param_1*0x10+0..0xB] to 1.
- Dispatches on current screen ID `DAT_0067ed7c[param_1*0x10]`:
  - case 1 (single-player): if DAT_007f0f2c==0 clear DAT_0067ed90; clear DAT_007f0fe8; FUN_0040e480(0..3, 0)
  - case 2: if DAT_007f0ad4==0 clear DAT_0067ed8c
  - case 3 (time trial): DAT_0067ea74=1; FUN_0040e480(0..3, 0)
  - case 8: FUN_00492d10() — if ==1 skip, else clear DAT_0067ed8c
  - case 10: conditional clear DAT_0067ed88/ed8c based on DAT_0067ea8c (2 or 3)
  - case 0x12 (18): advanced multiplayer checks (DAT_0067ea88, FUN_00430b60, DAT_0067f17c, split-screen, player-count)
  - case 0x18 (24): FUN_00430830(1) check
  - case 0x1c (28): lobby setup — 4-slot availability via thunk_FUN_00497450; car IDs 0x236/0x237/0x238/0x239 at DAT_00898aa0..00898b84
- After switch: scans availability flags to set initial cursor DAT_0067ed80[param_1*0x10].

### 0x004325c0 — FUN_004325c0() (563B)
Menu slide animation tick. Returns 1 when all animation slots are settled.  
Iterates stride-0xd int array starting at DAT_00898ac0 (pointer base) + 0x10 (piVar6 starts at DAT_00898ad0).  
Terminates when piVar6 > 0x8990e7.  
Per-entry: piVar6[-4] = color/type tag.  

**Type tags:**
- -0xfc0000 (0xff040000): countdown animation — decrements counter at piVar6[0]; wraps at 0→0x1ff or calls FUN_0042ac50 + position recalc; mode piVar6[-3]==2 → freeze-done
- -0xef0000 (0xff110000), -0xee0000 (0xff120000), -0xed0000 (0xff130000), -0xdd0000 (0xff230000), -0xf00000 (0xff100000): slide-in variants; counter increments; at 399+ → freeze at 0x1ff
- Others: float position update via DAT_007f1004 * _DAT_005cd8fc; clamp at _DAT_005cd09c

local_8 flag: 0 if any non-frozen active slot exists. Return value = local_8.

### 0x0042b310 — FUN_0042b310() (547B)
### 0x0042b540 — FUN_0042b540() (547B)
### 0x0042aff0 — FUN_0042aff0() (387B)
### 0x0042b180 — FUN_0042b180() (387B)
### 0x0042b770 — FUN_0042b770() (280B)

**Input change detector family.** Per-player record at 0x7f1044, stride 0x4c = 76 bytes, 8 slots (0..7).  
Slot i: player car ID from DAT_007f1a14[i*4]; car state from FUN_0040e470(i) — skip if ==2 (dead/spectating).  
Secondary "processed" array at 0x7f1504, same stride.

| Function | Byte offset | Processed offset | Timer | Float acc |
|----------|-------------|-----------------|-------|-----------|
| 0x0042b310 | +0 (007f1044) | +0 (007f1504) | no | — |
| 0x0042b540 | +1 (007f1045) | +1 (007f1505) | no | — |
| 0x0042aff0 | +2 (007f1046) | +2 (007f1506) | yes (screens 6–7) | _DAT_0067f1b0 |
| 0x0042b180 | +3 (007f1047) | +3 (007f1507) | yes (screens 6–7) | _DAT_0067f1b4 |
| 0x0042b770 | +5 (007f1049) | +5 (007f1509) | no | — |

Bytes 2,3 add a hold-repeat timer: accumulates DAT_007f1004 into float acc when screen 5 < id < 8; triggers at threshold _DAT_005ccac0; resets to 0.2.  
All return 1 if any active player has field[byte] set AND processed[byte] not set; 0 otherwise.  
U-1651: Byte offsets 4 and 6+ not seen in this session — how many button-change bits exist in the 0x4c record?

### 0x0042b9e0 — FUN_0042b9e0() (376B)
Car-selection confirm / player-slot assigner.

1. Resets 4 player states: FUN_0040e480(0..3, 0).
2. Counts valid car slots: checks 6 values at DAT_0067eafc[entry*0x12*4] offsets piVar2[-3/0/3/6/9/0xc]. If total valid < 2 → return 1 (already valid, no more checking needed).
3. Collision check: scans DAT_0067eaf0 stride-3 array (12 entries, piVar2=0x67eaf0); if any two entries [i] == [j] and both > 0 → bVar1=true (collision) → return 0.
4. Assignment loop: clears DAT_007f1a14..007f1a44 (4 player slots, stride 0x10) to -1. Then loops assigning ascending car IDs (iVar4 starting 1) to player slots matching DAT_0067eaf0[i*3]==iVar4; calls FUN_0040e480(player, 1) to activate. Returns 0x1000 when all 4 assigned or iVar4 reaches 7, and sets DAT_007f1a0c=2.

U-1652: DAT_0067eaf0 structure — stride 3 ints, 12 entries (up to 0x67eb7c); first int = car choice ID. What do [1] and [2] hold?  
U-1653: What is the semantic of DAT_007f1a0c? Values seen: 0x1000 (max-player mode?), 2 (assigned).

### 0x0042bb60 — FUN_0042bb60() (322B)
Team balance checker.

Reads player car IDs from DAT_007f1a14/1a24/1a34/1a44 (stride 0x10). For each valid slot (≠-1): looks up team via `(&DAT_0067e938)[carId * 3] - 1` → 0=team-A, 1=team-B, other=neutral.  
Counts: iVar3=total, iVar2=teamA, iVar4=teamB.

Return value by player count:
- 2 players: 1A+1B → 0x1000; else → 0
- 3 players: 1A+2B or 2A+1B → 0x1000; 0A or 3A → see LAB_0042bc82/bc92
- 4 players: 2A+2B → 0x1000; other → 2 or 3
- <2 or unknown → -1

U-1654: DAT_0067e938 stride-3 table — index by car ID, [0]-1 = team. Size and all car-team assignments unknown.

### 0x00431b80 — FUN_00431b80(int param_1) (369B)
Car-selection cursor mover. Uses non-standard calling: `in_EAX` = implicit player index (0-based byte offset into DAT_0067ea98).

Per-player selection array: DAT_0067ea98 + in_EAX (int); values 1..6 (car IDs).  
Movement direction from `unaff_ESI` (not a named param — likely ESI from caller).

Logic: while current value collides with another player's value (DAT_0067ea98[0], [1], [2]), or matches DAT_0067e9f8+1 — increment/decrement until collision-free.  
Wrap rules (param_1 discriminates):
- unaff_ESI=+1, param_1=0: wrap 7→1 (valid range 1–6)
- unaff_ESI=+1, param_1=1: wrap 7→0 (valid range 0–6)
- unaff_ESI=-1: wrap 0/-1→6

U-1655: `unaff_ESI` — what is the actual calling convention? Caller must set ESI before call.

### 0x004324a0 — FUN_004324a0() (275B)
Race start handler.

Early-out guard: if `DAT_0067eab0==0 OR DAT_0067eabc != -0xba0000 OR screen ID not in {0x233,0x231,0x1c1,0x232}` → proceed.

SP path (`DAT_007f1a0c == 0x1000`):
- FUN_0042b930() — if returns 0x21 or 1: skip
- Else FUN_0042c1f0(): if non-zero:
  - DAT_0067eab0==0 → FUN_0042bf30(0x213, 0xff220000, 0,0,0,0)
  - DAT_0067eabc != -0xde0000 → FUN_00432450(0x213, 0xff220000, 0,0,0,0)

MP path:
- DAT_0067eab0==0 → FUN_0042c1d0() + FUN_0042c220() → if nonzero → FUN_0042c280()
- Else if DAT_0067eabc not -0xdf0000/-0xde0000 → FUN_0042c1d0() + FUN_0042c220() → FUN_00432450(0x213, 0xff210000, 0,0,0,0)

U-1656: screen ID 0x213 (531) — what race/loading screen does this represent?  
S-1648: FUN_0042c1d0 — prerequisite check called before FUN_0042c220 in MP path.  
S-1649: FUN_00432450 — parallel of FUN_0042bf30; takes same 6-param signature.

### 0x004322c0 — FUN_004322c0() (243B)
Multiplayer track selection cursor move.

Calls FUN_0042aa00(1) first (S-1646b).  
Only active if screen 5 < `DAT_0067ed3c[DAT_0067e9f8*0x40]` < 8.

Loop: increments DAT_0067f17c (track index), clamps at 0xC (max 12 tracks). Calls FUN_00430830(index) — if non-zero: track is available → return.  
Computes target index `iVar2` from mode flags (DAT_0067ea68, DAT_0067e9fc: 10→0xB, 2→0, else 1 or 3).  
Inner loop decrements DAT_0067f184 while FUN_00430910() returns 0, seeking a valid option.  
Modes 3/4/5 call FUN_0042f6b0() after each pass.  
S-1647: FUN_00430910 — per-track availability check (secondary to FUN_00430830).

### 0x0042f400 — FUN_0042f400(param_1, param_2) (243B)
3-digit decimal number formatter for text vertex buffer.

- FUN_00427780(param_1) → iVar2 (pointer to text vertex struct)
- FUN_004a2b60(&local_c, &DAT_005cd794, param_2) — snprintf-style int→string into local_c (format string at 005cd794, likely `"%d"` or similar)
- Measures strlen of result (iVar4)
- Writes 3 shorts into iVar2+2/+4/+6: right-aligned digits, left-padded with '0' (0x30) if string < 3 chars

---

## Deferred callees (D-range 4840–4868)

| D-ID | RVA | Bytes | Reason |
|------|-----|-------|--------|
| D-4840 | 0x004323c0 | 135 | sub-cap |
| D-4841 | 0x0042ae10 | 156 | sub-cap |
| D-4842 | 0x0042aeb0 | 156 | sub-cap |
| D-4843 | 0x0042af50 | 156 | sub-cap |
| D-4844 | 0x00430910 | 137 | sub-cap |
| D-4845 | 0x00429aa0 | 134 | sub-cap |
| D-4846 | 0x0042aa00 | 168 | sub-cap |
| D-4847 | 0x0042ac90 | 126 | sub-cap |
| D-4848 | 0x0040b810 | 124 | sub-cap |
| D-4849 | 0x0042b960 | 115 | sub-cap |
| D-4850 | 0x0042f6b0 | 115 | sub-cap |
| D-4851 | 0x004307a0 | 116 | sub-cap |
| D-4852 | 0x0042f020 | 98 | sub-cap |
| D-4853 | 0x00431d00 | 97 | sub-cap |
| D-4854 | 0x004309b0 | 52 | sub-cap |
| D-4855 | 0x00492340 | 46 | sub-cap |
| D-4856 | 0x00430b60 | 47 | sub-cap |
| D-4857 | 0x00409930 | 30 | sub-cap |
| D-4858 | 0x00409900 | 43 | sub-cap |
| D-4859 | 0x004098b0 | 27 | sub-cap |
| D-4860 | 0x004298c0 | 27 | sub-cap |
| D-4861 | 0x0040acd0 | 48 | sub-cap |
| D-4862 | 0x00414120 | 80 | sub-cap |
| D-4863 | 0x00422b30 | 16 | sub-cap |
| D-4864 | 0x0046dc00 | 20 | sub-cap |
| D-4865 | 0x00495080 | 37 | sub-cap |
| D-4866 | 0x00494f30 | 15 | sub-cap |
| D-4867 | 0x0040e480 | 18 | sub-cap |
| D-4868 | 0x00492d30 | 265 | caller of parent (0x0043dfd0) |

Next session bucket: game_mode_cont2

<!-- shape-decomp-injected 2026-05-29 -->
## Mechanical descriptions (decomp transcripts)

Verbatim Ghidra DecompInterface output (Mashed_pool13 clone of MASHED.exe).

### 0x0042f400 — FUN_0042f400

```c
/* WARNING: Function: __security_check_cookie replaced with injection: security_check_cookie */



void FUN_0042f400(undefined4 param_1,undefined4 param_2)



{

  char cVar1;

  int iVar2;

  char *pcVar3;

  int iVar4;

  uint uVar5;

  int iVar6;

  uint unaff_retaddr;

  undefined4 local_c;

  uint local_4;

  

  local_4 = DAT_00616038 ^ unaff_retaddr;

  iVar2 = FUN_00427780(param_1);

  FUN_004a2b60(&local_c,&DAT_005cd794,param_2);

  pcVar3 = (char *)&local_c;

  do {

    cVar1 = *pcVar3;

    pcVar3 = pcVar3 + 1;

  } while (cVar1 != '\0');

  iVar4 = (int)pcVar3 - ((int)&local_c + 1);

  iVar6 = 0;

  uVar5 = local_c;

  do {

    if (iVar6 == 0) {

      if (iVar4 == 3) {

        uVar5 = (uint)(ushort)(short)(char)local_c;

      }

      else {

        uVar5 = 0x30;

      }

      *(short *)(iVar2 + 2) = (short)uVar5;

    }

    else if (iVar6 == 1) {

      if (iVar4 == 3) {

        uVar5 = (uint)(ushort)(short)local_c._1_1_;

        *(short *)(iVar2 + 4) = (short)local_c._1_1_;

      }

      else if (iVar4 == 2) {

        uVar5 = (uint)(ushort)(short)(char)local_c;

        *(short *)(iVar2 + 4) = (short)(char)local_c;

      }

      else {

        if (iVar4 == 1) {

          uVar5 = 0x30;

        }

        *(short *)(iVar2 + 4) = (short)uVar5;

      }

    }

    else if (iVar6 == 2) {

      if (iVar4 == 3) {

        uVar5 = (uint)(ushort)(short)local_c._2_1_;

        *(short *)(iVar2 + 6) = (short)local_c._2_1_;

      }

      else if (iVar4 == 2) {

        uVar5 = (uint)(ushort)(short)local_c._1_1_;

        *(short *)(iVar2 + 6) = (short)local_c._1_1_;

      }

      else {

        if (iVar4 == 1) {

          uVar5 = (uint)(ushort)(short)(char)local_c;

        }

        *(short *)(iVar2 + 6) = (short)uVar5;

      }

    }

    iVar6 = iVar6 + 1;

  } while (iVar6 < 3);

  return;

}
```

### 0x004322c0 — FUN_004322c0

```c
void FUN_004322c0(void)



{

  int iVar1;

  int iVar2;

  int iVar3;

  int iVar4;

  int iVar5;

  int iVar6;

  

  FUN_0042aa00(1);

  if ((5 < *(int *)(&DAT_0067ed3c + DAT_0067e9f8 * 0x40)) &&

     (iVar4 = DAT_0067f184, iVar6 = DAT_0067e9fc, *(int *)(&DAT_0067ed3c + DAT_0067e9f8 * 0x40) < 8)

     ) {

    while( true ) {

      DAT_0067f17c = DAT_0067f17c + 1;

      if (0xc < DAT_0067f17c) {

        DAT_0067f17c = 0xc;

        DAT_0067f1a4 = 0;

      }

      iVar1 = DAT_0067f17c;

      iVar2 = FUN_00430830(DAT_0067f17c);

      if (iVar2 != 0) {

        return;

      }

      iVar2 = (uint)(DAT_0067ea68 != 0) * 2 + 1;

      if (iVar6 == 10) {

        iVar2 = 0xb;

      }

      else if (iVar6 == 2) {

        iVar2 = 0;

      }

      if (iVar4 == iVar2) break;

      do {

        iVar4 = iVar4 + -1;

        iVar5 = iVar2;

        DAT_0067f184 = iVar2;

        if (iVar4 < iVar2) break;

        DAT_0067f184 = iVar4;

        iVar3 = FUN_00430910();

        iVar5 = iVar4;

      } while (iVar3 == 0);

      if (((iVar6 == 3) || (iVar6 == 4)) || (iVar6 == 5)) {

        FUN_0042f6b0();

        iVar6 = DAT_0067e9fc;

      }

      DAT_0067f17c = iVar1 + -1;

      iVar4 = iVar5;

    }

    DAT_0067f17c = iVar1 + -1;

    DAT_0067f1a4 = 0;

  }

  return;

}
```

### 0x004324a0 — FUN_004324a0

```c
void FUN_004324a0(void)



{

  int iVar1;

  

  if (((DAT_0067eab0 == 0) || (DAT_0067eabc != -0xba0000)) ||

     ((DAT_0067eab4 != 0x233 &&

      (((DAT_0067eab4 != 0x231 && (DAT_0067eab4 != 0x1c1)) && (DAT_0067eab4 != 0x232)))))) {

    if (DAT_007f1a0c == 0x1000) {

      iVar1 = FUN_0042b930();

      if ((iVar1 != 0x21) && (iVar1 != 1)) {

        iVar1 = FUN_0042c1f0();

        if (iVar1 != 0) {

          if (DAT_0067eab0 == 0) {

            FUN_0042bf30(0x213,0xff220000,0,0,0,0);

            return;

          }

          if (DAT_0067eabc != -0xde0000) {

            FUN_00432450(0x213,0xff220000,0,0,0,0);

          }

        }

      }

    }

    else if (DAT_0067eab0 == 0) {

      FUN_0042c1d0();

      iVar1 = FUN_0042c220();

      if (iVar1 != 0) {

        FUN_0042c280();

        return;

      }

    }

    else if ((DAT_0067eabc != -0xdf0000) && (DAT_0067eabc != -0xde0000)) {

      FUN_0042c1d0();

      iVar1 = FUN_0042c220();

      if (iVar1 != 0) {

        FUN_00432450(0x213,0xff210000,0,0,0,0);

        return;

      }

    }

  }

  return;

}
```

### 0x004325c0 — FUN_004325c0

```c
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */



undefined4 FUN_004325c0(void)



{

  int iVar1;

  undefined *puVar2;

  float fVar3;

  int iVar4;

  int iVar5;

  int *piVar6;

  bool bVar7;

  undefined4 local_8;

  

  local_8 = 1;

  piVar6 = &DAT_00898ad0;

  do {

    iVar1 = piVar6[-4];

    if (((iVar1 != 0) && (piVar6[-3] != 0x1000)) && (DAT_0067e914 != 0)) {

      local_8 = 0;

    }

    if (iVar1 < -0xeeffff) {

      if ((iVar1 == -0xef0000) || (iVar1 == -0x1000000)) goto LAB_0043277c;

      if (iVar1 != -0xfc0000) {

        bVar7 = iVar1 == -0xf00000;

        goto LAB_0043274f;

      }

      iVar1 = piVar6[-3];

      if ((iVar1 == 0) || (iVar1 == 2)) {

        iVar5 = FUN_004a2c48();

        iVar4 = *piVar6;

        *piVar6 = iVar4 + iVar5;

        if (iVar4 + iVar5 < 1) {

          if (iVar1 == 2) {

            *piVar6 = 0x1ff;

            piVar6[-3] = 0x1000;

          }

          else {

            iVar1 = piVar6[4];

            *piVar6 = 0;

            piVar6[-3] = 1;

            iVar4 = FUN_0042ac50();

            puVar2 = (undefined *)piVar6[8];

            piVar6[3] = (int)((float)iVar4 + (float)(iVar1 * 0x1e));

            if (puVar2 == &DAT_005f7370) {

              iVar4 = FUN_0042ac50();

              piVar6[3] = (int)(((float)iVar4 + (float)(iVar1 * 0x1c)) - _DAT_005cd900);

            }

            if (puVar2 == &DAT_005f72a0) {

              piVar6[3] = (int)((float)piVar6[3] - _DAT_005cd900);

            }

          }

        }

        goto LAB_004327d7;

      }

LAB_0043265c:

      iVar4 = FUN_004a2c48();

      iVar1 = *piVar6;

      *piVar6 = iVar1 + iVar4;

      if (399 < iVar1 + iVar4) {

        *piVar6 = 0x1ff;

        piVar6[-3] = 0x1000;

      }

    }

    else if ((iVar1 == -0xee0000) || (iVar1 == -0xed0000)) {

LAB_0043277c:

      iVar1 = piVar6[-3];

      if ((iVar1 != 0) && (iVar1 != 2)) goto LAB_0043265c;

      iVar5 = FUN_004a2c48();

      iVar4 = *piVar6;

      *piVar6 = iVar4 + iVar5;

      if (iVar4 + iVar5 < 1) {

        if (iVar1 == 2) {

          *piVar6 = 0x1ff;

          piVar6[-3] = 0x1000;

        }

        else {

          *piVar6 = 0;

          piVar6[-3] = 1;

        }

      }

    }

    else {

      bVar7 = iVar1 == -0xdd0000;

LAB_0043274f:

      if (bVar7) goto LAB_0043277c;

      fVar3 = DAT_007f1004 * _DAT_005cd8fc + (float)piVar6[-2];

      piVar6[-2] = (int)fVar3;

      if (_DAT_005cd09c <= fVar3) {

        piVar6[-2] = 0x43340000;

        piVar6[-3] = 0x1000;

      }

    }

LAB_004327d7:

    piVar6 = piVar6 + 0xd;

    if (0x8990e7 < (int)piVar6) {

      return local_8;

    }

  } while( true );

}
```

### 0x00432800 — FUN_00432800

```c
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */



void FUN_00432800(int param_1)



{

  int iVar1;

  int iVar2;

  undefined4 *puVar3;

  int iVar4;

  undefined4 *puVar5;

  

  iVar4 = param_1 * 0x40;

  puVar3 = &DAT_0067ed84 + param_1 * 0x10;

  puVar5 = puVar3;

  for (iVar2 = 0xc; iVar1 = DAT_0067ea8c, iVar2 != 0; iVar2 = iVar2 + -1) {

    *puVar5 = 1;

    puVar5 = puVar5 + 1;

  }

  switch((&DAT_0067ed7c)[param_1 * 0x10]) {

  case 1:

    if (DAT_007f0f2c == 0) {

      *(undefined4 *)(&DAT_0067ed90 + iVar4) = 0;

    }

    DAT_007f0fe8 = 0;

    iVar2 = 0;

    do {

      FUN_0040e480(iVar2,0);

      iVar2 = iVar2 + 1;

    } while (iVar2 < 4);

    break;

  case 2:

    if (DAT_007f0ad4 == 0) {

      *(undefined4 *)(&DAT_0067ed8c + iVar4) = 0;

    }

    break;

  case 3:

    DAT_0067ea74 = 1;

    iVar2 = 0;

    do {

      FUN_0040e480(iVar2,0);

      iVar2 = iVar2 + 1;

    } while (iVar2 < 4);

    break;

  case 8:

    iVar2 = FUN_00492d10();

    if (iVar2 == 1) break;

    *(undefined4 *)(&DAT_0067ed8c + iVar4) = 0;

    goto LAB_00432a17;

  case 10:

    if (DAT_0067ea8c != 2) {

      (&DAT_0067ed88)[param_1 * 0x10] = 0;

    }

    if (iVar1 != 3) {

      *(undefined4 *)(&DAT_0067ed8c + iVar4) = 0;

    }

    break;

  case 0x12:

    if (DAT_0067ea88 == 1) {

      *(undefined4 *)(&DAT_0067ed90 + iVar4) = 0;

      DAT_0067ea7c = 0;

LAB_0043297d:

      iVar2 = FUN_00430b60();

      if (iVar2 == 2) {

        *(undefined4 *)(&DAT_0067ed98 + iVar4) = 0;

        DAT_0067ea78 = 0;

      }

    }

    else {

      if (DAT_0067ea7c == 0) goto LAB_0043297d;

      *(undefined4 *)(&DAT_0067ed98 + iVar4) = 1;

    }

    iVar2 = DAT_0067f17c * 0x30;

    if (*(int *)(&DAT_007f0a58 + iVar2) == 0) {

      *(undefined4 *)(&DAT_0067ed9c + iVar4) = 0;

      DAT_0067ea88 = 0;

    }

    else {

      *(undefined4 *)(&DAT_0067ed9c + iVar4) = 1;

    }

    if (*(int *)(&DAT_007f0a50 + iVar2) == 0) {

      (&DAT_0067ed88)[param_1 * 0x10] = 0;

      DAT_0067ea74 = 0;

    }

    else {

      (&DAT_0067ed88)[param_1 * 0x10] = 1;

    }

    if (DAT_007f0fd0 == 2) {

      DAT_0067ea64 = 0;

    }

    iVar2 = FUN_00430b60();

    if ((iVar2 != 4) && (iVar2 = FUN_0042f500(), iVar2 == 0)) break;

    goto LAB_00432a17;

  case 0x18:

    iVar2 = FUN_00430830(1);

    if (iVar2 != 0) break;

LAB_00432a17:

    *(undefined4 *)(&DAT_0067ed90 + iVar4) = 0;

    break;

  case 0x1c:

    *puVar3 = 1;

    (&DAT_0067ed88)[param_1 * 0x10] = 1;

    *(undefined4 *)(&DAT_0067ed8c + iVar4) = 1;

    *(undefined4 *)(&DAT_0067ed90 + iVar4) = 1;

    iVar2 = 0;

    do {

      iVar4 = thunk_FUN_00497450(iVar2);

      if (iVar4 == 0) {

        *puVar3 = 0;

      }

      iVar2 = iVar2 + 1;

      puVar3 = puVar3 + 1;

    } while (iVar2 < 4);

    DAT_00898aa0 = 0;

    DAT_00898aa4 = 1;

    _DAT_00898aa8 = 2;

    _DAT_00898aac = 3;

    DAT_00898b1c = 0x236;

    _DAT_00898b50 = 0x237;

    _DAT_00898b84 = 0x238;

    _DAT_00898bb8 = 0x239;

  }

  iVar2 = (&DAT_0067ed80)[param_1 * 0x10];

  if (iVar2 == -1) {

    iVar2 = 0;

  }

  if ((&DAT_0067ed84)[param_1 * 0x10 + iVar2] != 1) {

    iVar4 = 0;

    if (0 < (int)(&DAT_0067edb4)[param_1 * 0x10]) {

      do {

        if ((&DAT_0067ed84)[param_1 * 0x10 + iVar2] == 1) {

          (&DAT_0067ed80)[param_1 * 0x10] = iVar2;

          return;

        }

        iVar2 = iVar2 + 1;

        if (iVar2 == (&DAT_0067edb4)[param_1 * 0x10]) {

          iVar2 = 0;

        }

        iVar4 = iVar4 + 1;

      } while (iVar4 < (int)(&DAT_0067edb4)[param_1 * 0x10]);

    }

    (&DAT_0067ed80)[param_1 * 0x10] = 0xffffffff;

  }

  return;

}
```

### 0x0043d2a0 — FUN_0043d2a0

```c
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */



void FUN_0043d2a0(int param_1,int param_2)



{

  undefined *puVar1;

  short sVar2;

  short sVar3;

  undefined4 *puVar4;

  undefined4 *puVar5;

  int iVar6;

  bool bVar7;

  float10 extraout_ST0;

  int local_1c;

  int local_10;

  int local_c;

  int local_8;

  int local_4;

  

  if (param_2 == 0) {

    FUN_00431f30(param_1);

  }

  else if (param_2 == 1) {

    if (-1 < DAT_0067e9f8 + -2) {

      FUN_00431f30((&DAT_0067ed7c)[(DAT_0067e9f8 + -2) * 0x10]);

    }

  }

  else if (param_2 == 2) {

    FUN_00431d90();

  }

  FUN_00472640(0xff);

  if (param_1 == 0x1a) {

    DAT_007f0ef4 = DAT_007f0eec;

    DAT_007f0ef8 = DAT_007f0ef0;

  }

  else if (param_1 == 0x13) {

    DAT_007f0f14 = DAT_007f0f00;

    DAT_007f0f24 = DAT_007f0f10;

    DAT_007f0f20 = DAT_007f0f0c;

    DAT_007f0f1c = DAT_007f0f08;

    DAT_007f0f18 = DAT_007f0f04;

  }

  else if (param_1 == 0x20) {

    DAT_008990d8 = FUN_0040ad20();

  }

  else if (param_1 == 0x1c) {

    puVar5 = &DAT_00899160;

    puVar4 = &DAT_007f105c;

    do {

      *puVar5 = *puVar4;

      puVar4 = puVar4 + 0x13;

      puVar5 = puVar5 + 1;

    } while ((int)puVar4 < 0x7f13ec);

  }

  else {

    if ((param_1 == 0x1f) || (param_1 == 0x21)) {

      DAT_007f0f44 = DAT_007f0f30;

      DAT_007f0f48 = DAT_007f0f34;

      DAT_007f0f4c = DAT_007f0f38;

      DAT_007f0f50 = DAT_007f0f3c;

    }

    if (param_1 == 1) {

      FUN_00414120();

      FUN_00422b30();

      FUN_0040b810();

      DAT_0067ea70 = 1;

    }

  }

  FUN_0042d3e0();

  iVar6 = DAT_0067e9f8;

  local_4 = 0;

  local_10 = 0;

  if (param_2 == 0) {

    (&DAT_0067ed78)[DAT_0067e9f8 * 0x10] = (&PTR_DAT_005f7638)[param_1];

    (&DAT_0067ed7c)[iVar6 * 0x10] = param_1;

    (&DAT_0067ed80)[iVar6 * 0x10] = 0;

    FUN_00432800(iVar6);

    local_1c = DAT_0067e9f8 * 0x40;

  }

  else if (param_2 == 2) {

    local_1c = DAT_0067e9f8 * 0x40;

  }

  else {

    DAT_0067e9f8 = DAT_0067e9f8 + -1;

    if (DAT_0067e9f8 < 1) {

      return;

    }

    local_1c = DAT_0067e9f8 * 0x40;

  }

  sVar2 = FUN_0042ad90();

  sVar3 = FUN_0042ad90();

  if ((sVar2 != -1) || (sVar3 != -1)) {

    DAT_00898acc = 0xff;

    DAT_00898acd = 0xff;

    DAT_00898ace = 0xff;

    DAT_00898ae8 = (int)sVar2;

    DAT_00898aec = (int)sVar3;

    DAT_00898ac0 = 0xff000000;

    DAT_00898ad8 = 0x42800000;

    DAT_00898adc = 0x42400000;

    DAT_00898ad4 = 0x3f19999a;

    DAT_00898ae4 = 0;

    if (sVar3 == -1) {

      DAT_00898ac4 = 1;

      DAT_00898ad0 = 0;

      local_10 = 1;

    }

    else {

      DAT_00898ad0 = 0x1ff;

      DAT_00898ac4 = 2;

      if (sVar2 != -1) {

        DAT_00898ac4 = 0;

      }

      local_10 = 1;

    }

  }

  if (param_2 == 0) {

    FUN_0042ac00();

    FUN_0042ac00();

  }

  else {

    FUN_0042ac00();

    FUN_0042ac00();

  }

  if (DAT_0067e9f8 == 0) {

    if (DAT_0067ed78 == &DAT_005f7370) {

      FUN_0042ac50();

    }

    else {

      FUN_0042ac50();

    }

  }

  else if (*(undefined **)((int)&DAT_0067ed78 + local_1c) == &DAT_005f7370) {

    FUN_0042ac50();

  }

  else {

    FUN_0042ac50();

  }

  iVar6 = 0;

  local_8 = 0;

  local_c = 0;

  puVar4 = &DAT_00898ad8 + local_10 * 0xd;

  do {

    sVar2 = FUN_0042ad90();

    sVar3 = FUN_0042ad90();

    if (sVar2 == -1) {

      if (sVar3 == -1) {

        if (param_2 == 0) {

          *(int *)((int)&DAT_0067edb4 + local_1c) = local_4;

        }

        else {

          local_4 = *(int *)(&DAT_0067ed74 + local_1c);

        }

        DAT_0067ece0 = local_4;

        sVar2 = FUN_0042ad90(param_1);

        FUN_00432b30((int)sVar2);

        if (param_2 == 0) {

          DAT_0067e9f8 = DAT_0067e9f8 + 1;

        }

        DAT_0067e844 = param_2;

        return;

      }

    }

    else {

      local_4 = local_4 + 1;

    }

    puVar1 = *(undefined **)((int)&DAT_0067ed78 + local_1c);

    puVar4[6] = puVar1;

    puVar4[-6] = 0xff040000;

    *puVar4 = 0x42800000;

    *(undefined1 *)(puVar4 + -3) = 0xff;

    *(undefined1 *)((int)puVar4 + -0xb) = 0xff;

    *(undefined1 *)((int)puVar4 + -10) = 0xff;

    *(undefined1 *)((int)puVar4 + -9) = 0xff;

    puVar4[-1] = 0x3f4ccccd;

    if ((((puVar1 == &DAT_005f72a0) || (puVar1 == &DAT_005f7370)) ||

        ((*(undefined **)((int)&DAT_0067ed38 + local_1c) == &DAT_005f72a0 && (param_2 == 2)))) &&

       (local_c != 0)) {

      puVar4[-1] = 0x3f147ae1;

    }

    puVar4[3] = 0;

    puVar4[-5] = 0;

    puVar4[1] = (float)((float10)local_c + extraout_ST0);

    if ((undefined *)puVar4[6] == &DAT_005f7370) {

      puVar4[1] = (float)(((float10)local_8 + extraout_ST0) - (float10)_DAT_005cd900);

    }

    if ((undefined *)puVar4[6] == &DAT_005f72a0) {

      puVar4[1] = (float)puVar4[1] - _DAT_005cd900;

    }

    puVar4[2] = iVar6;

    puVar4[4] = (int)sVar2;

    bVar7 = DAT_0067e9f8 == 0;

    puVar4[5] = (int)sVar3;

    puVar4[-2] = 0x1ff;

    if (bVar7) {

      puVar4[-5] = 1;

      puVar4[-2] = 0;

    }

    puVar4 = puVar4 + 0xd;

    local_c = local_c + 0x1e;

    iVar6 = iVar6 + 1;

    local_8 = local_8 + 0x1c;

  } while( true );

}
```
