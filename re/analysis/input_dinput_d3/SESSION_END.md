# Session input_dinput_d3 — input_dinput depth-3

**Date:** 2026-05-06  
**Slot:** Mashed_pool13  
**Session ID:** 61ca630a2543482ab819a27b4ff5974b  
**Bucket:** re/analysis/input_dinput_d3/  
**ID ranges:** U=2587..2590, D=7660, S=(none new)  
**Parent:** input_dinput_d2-cont1 (D=6760..6766)  

---

## Drain result: D-6760..D-6766

| D | RVA | Result | Confidence |
|---|-----|--------|------------|
| D-6760 | 0x004972b0 FUN_004972b0 | **Analyzed** — per-frame joypad axis snapshot + keyboard read | C1 |
| D-6761 | 0x0045b350 FUN_0045b350 | **Already C1** — render subsystem (rw_engine_init); S-2281 cleared | — |
| D-6762 | 0x00499720 FUN_00499720 | **Analyzed** — HINSTANCE getter (5 bytes) | C1 |
| D-6763 | 0x00495830 FUN_00495830 | **Analyzed** — joypad struct string copier | C1 |
| D-6764 | 0x004971b0 FUN_004971b0 | **Analyzed** — controller config file loader | C1 |
| D-6765 | 0x004a2c48 FUN_004a2c48 | **Already C1** — FPU float→int64 rounding utility; S-2285 cleared | — |
| D-6766 | 0x00495ee0 LAB_00495ee0 | **DEFERRED** — no function at address (read-only slot); needs function_create on RW slot; → D-7660 | — |

**In-session bonus:** FUN_00497190 (0x00497190) — config filename formatter ("contcfg%d.bin" via sprintf); C1; callee of FUN_004971b0.

---

## hooks.csv additions (subsystem=input, confidence=C1, status=new)

| RVA | notes | file |
|-----|-------|------|
| 004972b0 | per-frame joypad axis snapshot + keyboard bitmap clear/read; calls FUN_00495790+FUN_004957a0(per-slot)+FUN_004b6520(zero 0x20b)+FUN_00496100(kbd); resolves U-2305 U-2301 | re/analysis/input_dinput_d3/004972b0.md |
| 00499720 | 5b getter; returns DAT_007e9580 (HINSTANCE; adjacent to HWND@007e9584); called by FUN_00498510 for LoadStringA | re/analysis/input_dinput_d3/00499720.md |
| 00495830 | strcpy joypad_struct[param_1] (base DAT_00771eb4 stride 0x448) → param_2; bounds-check < DAT_00772fac; returns 1/0; U-2589 | re/analysis/input_dinput_d3/00495830.md |
| 004971b0 | controller config loader; "contcfg%d.bin" via FUN_00497190; fopen FUN_004a4541(DAT_005cf010); fread 0x200b→DAT_007e95c0+EAX×0x80; EAX=slot (register conv); U-2587 U-2588 U-2590 | re/analysis/input_dinput_d3/004971b0.md |
| 00497190 | sprintf "contcfg%d.bin"→DAT_007730e4 via FUN_004a2b60; returns ptr; implicit EAX slot (U-2588) | re/analysis/input_dinput_d3/00497190.md |

---

## UNCERTAINTIES.md additions (U-2587..2590)

| ID | type | location | observation | evidence_needed |
|----|------|----------|-------------|-----------------|
| U-2587 | structural | FUN_004971b0 0x004971b0 | _fread reads 0x200 bytes into DAT_007e95c0+EAX×0x80; slot stride 0x80 too small for 0x200b read; buffer block layout unclear | decomp callers; read DAT_007e95c0 layout |
| U-2588 | structural | FUN_004971b0 + FUN_00497190 | EAX as implicit slot index — register convention; no stack parameter; caller must set EAX | listing_code_unit_at caller sites of 0x004971b0 |
| U-2589 | structural | FUN_00495830 0x00495830 | String starts at offset 0 of joypad struct (DAT_00771eb4+param_1×0x448); conflicts with COM ptr expected at offset 0 per shutdown function | listing_data_at 0x00771eb4 |
| U-2590 | semantic | FUN_004971b0 0x004971b0 | DAT_005cf010 — fopen mode string; expected "rb" | memory_read 0x005cf010 4 bytes |

---

## DEFERRED.md additions (D-7660)

| D | RVA | reason | pickup_condition | bucket |
|---|-----|--------|-----------------|--------|
| D-7660 | 0x00495ee0 LAB_00495ee0 | EnumDevices callback; no function defined; needs function_create on RW slot | acquire RW slot; function_create 0x00495ee0 | input_dinput_d3-cont1 |

---

## Stubs resolved by this session

| ID | RVA | action |
|----|-----|--------|
| S-2280 | 0x004972b0 | Resolved — C1 |
| S-2281 | 0x0045b350 | Already resolved (render, rw_engine_init session) |
| S-2282 | 0x00499720 | Resolved — C1 |
| S-2283 | 0x00495830 | Resolved — C1 (with U-2589) |
| S-2284 | 0x004971b0 | Resolved — C1 (with U-2587 U-2588 U-2590) |
| S-2285 | 0x004a2c48 | Already resolved (frontend/render, C1) |
| S-2286 | 0x00495ee0 | Unresolved → D-7660 |

---

## Key findings

1. **FUN_004972b0** (D-6760) is the per-frame joypad axis snapshot + keyboard read function. Called 3rd in the main input poll loop (FUN_004967e0). It iterates joypads calling FUN_004957a0 to snapshot axis data into two per-slot arrays (DAT_007730d4 stride 4, DAT_0077311c stride 8), then zeros DAT_0077313c (32-byte keyboard bitmap) and calls FUN_00496100 to read keyboard state. Resolves U-2305 and U-2301.

2. **FUN_0045b350** (D-6761) was already C1 under render (rw_engine_init session). It's a zero-body bare-return function with 33 callers across all subsystems. The input_dinput_d2 stub S-2281 was redundant.

3. **FUN_00499720** (D-6762) is a 5-byte HINSTANCE getter returning DAT_007e9580, paired with FUN_00499710 (HWND getter at DAT_007e9584). Global layout: [0x007e9580]=HINSTANCE, [0x007e9584]=HWND.

4. **FUN_00495830** (D-6763) is a bounds-checked string copy from joypad struct slot `param_1` (base DAT_00771eb4, stride 0x448) to destination `param_2`. [UNCERTAIN U-2589]: the string starts at struct offset 0, which conflicts with the expected COM pointer layout.

5. **FUN_004971b0** (D-6764) is a controller config **file loader** (not a "comparison" as the d2 stub described). It generates filename "contcfg{N}.bin" via FUN_00497190, opens it with _fsopen, reads 0x200 bytes into the controller config buffer. Uses EAX register convention for slot index. [UNCERTAIN U-2587/U-2588]: read size 0x200 vs slot stride 0x80.

6. **FUN_004a2c48** (D-6765) was already C1 as a general FPU float→int64 rounding utility. The d2 description "button state byte reader" was a context-based misnomer; 154 callers confirm it's a universal math helper.

7. **FUN_00497190** (bonus) generates "contcfg%d.bin" filename into static buffer DAT_007730e4 using implicit EAX. Confirmed sprintf via FUN_004a2b60 (C1).

8. **LAB_00495ee0** (D-6766 → D-7660): Ghidra has no function defined at 0x00495ee0. This is the EnumDevices callback identified in U-2299 (d2). Requires `function_create` on a read-write slot before analysis.

---

## Depth-4 queue

Queue entry: `input_dinput_d3-cont1` — 1 deferred item (D-7660 = 0x00495ee0, LAB_00495ee0).
