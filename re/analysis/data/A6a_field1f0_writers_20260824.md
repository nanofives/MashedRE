# A6a — Writers & meaning of vehicle-record field +0x1f0

- Date: 2026-08-24
- Program: MASHED.exe (anchor BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E)
- Pool slot: Mashed_pool13, session 9a2bdd0f6d48419a9758556fa940c0f6, read_only
- NO-GUESSING: every offset/constant below cites the Ghidra RVA where it appears.

## 1. Record base and the field's absolute address

- Vehicle record base (slot 0) = `&DAT_008815a0`; per-slot stride = `0xd04`; init = `FUN_0046b540`
  (`iVar5 = (int)param_1 * 0xd04`, and `(&DAT_008815a4)[param_1 * 0x341]` at 0x0046b540).
- Therefore record+0x1f0 for slot 0 == absolute `0x00881790`.
- `FUN_0046b540` (the per-vehicle init) writes MANY fields but **NONE at +0x1f0** (verified in its
  full decomp — no `&DAT_00881790 + iVar5` store).

## 2. The field is PER-WHEEL (stride 0xc4)

Two accessor helpers compute `(vehIdx*0x11 + wheel)*0xc4 + 0x881790` and read a dword:
- `FUN_0046d2e0` @ 0x0046d304  `MOV EAX,[EAX + 0x881790]` (getter, bounds veh<0x10, wheel<4)
- `FUN_0046d320` @ 0x0046d344  `MOV EAX,[EAX + 0x881790]` (getter, arg order swapped)
`0x11*0xc4 = 0xd04` (one record); `+wheel*0xc4 + 0x1f0`. So +0x1f0 is the **wheel[0] slot** of a
per-wheel field replicated at +0x1f0 + wheel*0xc4. `reference_to 0x00881790` returns ONLY these two
reads — no absolute-address writer exists; the writer walks a wheel pointer.

Wheel-block bases seen for the SAME field:
- physics `FUN_00467650`: `ESI+0x1a4` base, color at `piVar12+0x4c`  (0x1f0-0x1a4=0x4c)
- contact `FUN_0046f6c0`/`FUN_0046cc40`: `record+0x194` base, color at `+0x5c`  (0x1f0-0x194=0x5c)
- tyre-scalar `FUN_0046ddb0`: `record+0x1f4` base, color at `[-1]`  (0x1f4-4=0x1f0)

## 3. Readers (all compare +0x1f0 as an INT vs hardcoded packed constants)

`search_constants` for the magics returns ONLY `CMP` sites (readers), never an immediate store:
- 0xFF961E5A (-0x69e1a6): CMP EAX at 0x461f3c,0x4620b3,0x4640ea,0x467a9d,0x467bad,0x4686e3,0x46e7f3
- 0xFFA08080 (-0x5f7f80): CMP EAX at 0x4640e1,0x467a55,0x4686c7,0x46e7d5 (+ CMP ESI at 0x40c7eb)
- `search_bytes 5a1e96ff` and `8080a0ff` in data sections → 0 hits (constants live only as code immediates).
- Physics `FUN_00467650`: `MOV EAX,[ESI+0x1f0]` at 0x00467a4f / 0x00467ba7 / 0x004686ad, each feeding
  the CMP-vs-magic grip gate.
- Classifier `FUN_00461e90(selector, packedColor, out1, out2)` switches on a 0..0x27 selector
  (MOVZX EAX,[EAX+0x4623ac]; JMP [EAX*4+0x462374] @ 0x461e9d/0x461ea4) then CMPs the color arg
  against packed 0xAARRGGBB values (0xffb48080,0xffc88080,0xff804000,0xffdc8080, …) and emits small
  enums 0/1/2. Callers: `FUN_00463c80`, `FUN_004642f0` — both read the color via `FUN_0046d2e0`
  then classify. `FUN_004642f0` uses the enum to index an impact-SFX descriptor table
  (`local_8 * 0x90 + 0x80 + DAT_0088e670`).
- Tyre-scalar `FUN_0046ddb0`: reads `pfVar14[-1]` (=+0x1f0) and the `fVar4 == -NAN` chain (raw-bit
  compares) picks a wheel scalar 0.25/0.2/0.1/0.01.

=> MEANING: +0x1f0 is a per-wheel **surface material colour / id** (packed, high byte 0xFF),
recognised by hardcoded constants to select surface-specific grip / tyre / scrape-SFX behaviour.

## 4. THE WRITER (record field +0x1f0)

`FUN_0046cc40(int *param_1 /*record*/, float *param_2 /*terrain-contact entries*/)`
(called by `FUN_0046f6c0` right after the broadphase `FUN_00538c80(...,&LAB_00468b80,...)`):

```
local_90 = (float *)(param_1 + 0x65);   // = record + 0x194  (0x65*4)  → wheel base, stride 0x31 (=0xc4)
...on a valid contact (FUN_00468b40 dedup == 0)...
local_90[0x16] = param_2[0xc];   // record+0x1ec  ← contact-entry+0x30   (asm 0x0046d00b area)
local_90[0x17] = param_2[0xd];   // record+0x1f0  ← contact-entry+0x34   *** THE WRITER ***
```
Store instruction: **0x0046d011  `MOV dword ptr [EAX + 0x5c], ECX`**  (EAX = local_90 = record+0x194+wheel*0xc4;
ECX = param_2[0xd]). Value written = the terrain-contact-entry field at **+0x34** (entry stride 0x24
ints = 0x90 bytes; `param_2 = param_2 + 0x24` at loop tail).

## 5. Upstream source of contact-entry+0x34 (the colour value itself)

Narrow-phase collision callback `LAB_00468b80` (bare label at 0x00468b80, invoked by broadphase
`FUN_00538c80`; `FUN_00468b40` @0x468b40 is only a per-vehicle dedup that scans record+0xbfc):

```
0x00468b93  MOV ESI,[ESI+0x4]                       ; ESI = collision triangle-list obj (from arg ESP+0x2c)
0x00468ba2  MOV EDX,[EAX+0x18]                       ; EAX = arg ESP+0x30
0x00468ba5  MOVZX EDX, word ptr [ESI+EDX*8+0x6]      ; EDX = 16-bit MATERIAL INDEX of the hit triangle
0x00468baa  MOV ESI,[0x0088e654]                     ; world/collision ctx (set = in_EAX in FUN_0046f6c0)
0x00468bb0  MOV ESI,[ESI+0x8]
0x00468bb3  MOV ESI,[ESI+0x10]
0x00468bb6  MOV ESI,[ESI+EDX*4]                       ; index material table by matIdx
0x00468bb9  MOV ESI,[ESI+0x4]                         ; ESI = surface colour/id (material entry +0x4)
0x00468bbc  MOV [ECX+0x34],ESI                        ; write into the contact entry (+0x34)
```
=> the colour is looked up from the hit collision-triangle's **material** (a 16-bit material index →
per-world material table `[0x88e654]→+8→+0x10→[matIdx*4]→+0x4`). It is **track collision data**, not a
constant baked into MASHED.exe.

## 6. Reset / default

`FUN_0046f6c0` init loop (per tick, stride 0xc4) resets the ADJACENT field **+0x1ec** to -1
(0xffffffff): `0x0046f73f  MOV [EAX+0x58],ESI` with EAX=record+0x194, ESI=0xffffffff (OR ESI,-1 @0x46f72b).
It also sets +0x194 = 10.0 (0x41200000). It does **NOT** reset +0x1f0.
[UNCERTAIN] whether +0x1f0 is cleared to a default on a no-contact frame — no reset store to +0x1f0
was located; only the adjacent +0x1ec flag is reset. The -1 the physics compares against at +0x1f0
would come from the material table returning -1, not from this reset.

## 7. Ruled-out displacement-0x1f0 stores (different structs)

`search "0x1f0],"` whole-image → 11 stores, none a vehicle record:
- 0x0051535b/0x00518371/0x005209ec/0x005c83fc/0x005c8aff/0x005c8c8c/0x005c8ce2/0x005c8d55/0x005c90da
  = library/CRT bands (0x51/0x52/0x5c).
`search "0x1f0]"` in 0x46xxxx-0x4axxxx → the FSTP/float stores at 0x0047ab66 (`FUN_0047ab30` =
COURSE.LUA `Setup_Fog`, base `DAT_006bf1cc` fog struct) and reads in `FUN_00479330` (a +0x105d0-sized
struct) are DIFFERENT objects that merely also have a field at +0x1f0.

## Verdict for the port
+0x1f0 (per wheel, +0x1f0+wheel*0xc4) = surface material colour/id of the track collision triangle under
that wheel, copied each tick by `FUN_0046cc40` from contact-entry+0x34, itself set by `LAB_00468b80`
from the triangle's material table entry (+0x4). The standalone must obtain this from the ported
wheel-vs-track contact solver reading track collision material data. Do NOT hardcode a magic constant.
