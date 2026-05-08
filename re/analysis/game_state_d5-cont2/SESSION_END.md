# SESSION_END — game_state_d5_cont2-20260508-1648

## Session summary

- Pool slot: Mashed_pool2 (read-only)
- Binary anchor: MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E — verified OK
- Functions analyzed: 14 (D-8802..D-8815)
- Functions skipped (pre-existing): 2 (D-8800: 0x00404fa0, D-8801: 0x00408b00)
- Confidence level: C1 for all 14

## Per-RVA files written

| RVA | DEFERRED | Size | Notes |
|---|---|---|---|
| 0x00409290 | D-8802 | 109b | pos-clamp-to-segment; calls S-3240/S-3241 |
| 0x0040b250 | D-8803 | 51b | pre-placement sentinel init (−1000 × 8 fields) |
| 0x0040b410 | D-8804 | 11b | player readiness getter from DAT_008a9520 array |
| 0x0041ede0 | D-8805 | 111b | per-player flags writer: bits 4/5 → bits 12/13 cascade |
| 0x0041ee50 | D-8806 | 88b | per-player flags writer: bits 12/13 direct |
| 0x0041ef80 | D-8807 | 54b | per-player flags writer: bit 10 only |
| 0x0041f000 | D-8808 | 36b | copies 6 dwords from per-player struct to output |
| 0x00429820 | D-8809 | 20b | zeros DAT_008991b0/b4 |
| 0x0046b1c0 | D-8810 | 809b | vehicle bounding-geometry midpoint generator; slot 0..15 guard |
| 0x0046b540 | D-8811 | 1367b | full vehicle physics init; mode-override table; spring/dist calcs |
| 0x0046c6d0 | D-8812 | 35b | vehicle physics struct getter at +0xa80 offset |
| 0x004704c0 | D-8813 | 426b | 6-arg vehicle placement: matrix + contact-sentinel reset |
| 0x0048f680 | D-8814 | 32b | zeroes field[+0] of 256-entry 56-byte-stride table at 0x76a100 |
| 0x0048f740 | D-8815 | 48b | calls 3 sibling inits + zeroes 256-entry 48-byte-stride table at 0x766f40 |

## Carry-forward (pre-existing, skip re-analysis)

| RVA | DEFERRED | File |
|---|---|---|
| 0x00404fa0 | D-8800 | re/analysis/game_state_d5-cont2/0x00404fa0.md |
| 0x00408b00 | D-8801 | re/analysis/game_state_d5-cont2/0x00408b00.md |

## New stubs (S-3240..S-3248)

| ID | RVA | Callee of | Description |
|---|---|---|---|
| S-3240 | 0x00426bc0 | 0x00409290 | no-arg; returns segment/slot count |
| S-3241 | 0x00426bd0 | 0x00409290 | (slot_idx, &min, short[2]); fills [min, max] range |
| S-3242 | 0x0047ea60 | 0x0046b540, 0x004704c0 | (slot, matrix_ptr); physics placement bind |
| S-3243 | 0x0041eda0 | 0x0046b540, 0x004704c0 | (slot, 1); entity activation |
| S-3244 | 0x0046d400 | 0x004704c0 | (param_2, param_3, param_4, param_6) → float; ground snap? |
| S-3245 | 0x0040ce80 | 0x0046b540 | (track/race handle) → vehicle class index |
| S-3246 | 0x0048f710 | 0x0048f740 | no-arg; sibling table init |
| S-3247 | 0x0048f6b0 | 0x0048f740 | no-arg; sibling table init |
| S-3248 | 0x0048f6e0 | 0x0048f740 | no-arg; sibling table init |

## New uncertainties (U-3247..U-3251)

| ID | RVA | Note |
|---|---|---|
| U-3247 | 0x00409290 | param_1 semantics: position handle vs segment index |
| U-3248 | 0x0040b250 | 4 unwritten dwords at 0x8a9510..0x8a951c between the two groups |
| U-3249 | 0x0046b1c0 | param_2 6-element layout: extents vs vertex positions |
| U-3250 | 0x0046b540 | vehicle physics struct layout (>60 fields written at stride 0xd04) |
| U-3251 | 0x004704c0 | FUN_0046d400 return value semantics (ground height / snap float) |

## Deferred rows consumed

D-8800..D-8815 — all 16 rows. Bucket game_state_d5-cont2 fully drained.

## Cap status

No functions capped (all 14 analyzed within session). Bucket D=9640..9699 (game_state_d5-cont2-cont1) not needed.
