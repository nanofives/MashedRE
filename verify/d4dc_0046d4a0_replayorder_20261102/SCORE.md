| Gate | Arm | Result | Detail |
|---|---|---|---|
| G-RP-OFF | `RP_FIX_OFF` | PASS | G-M3-REPLAY rows=3888 exact=3888 entry_bad=0 calls_bad=0 | arms bump=60 collect=60 no-op=3522 vanish=246 | MISMATCH lines 0 |
| G-RP-ON | `RP_FIX_ON` | PASS | G-M3-REPLAY rows=3888 exact=3888 entry_bad=0 calls_bad=0 | arms bump=60 collect=60 no-op=3522 vanish=246 | MISMATCH lines 0 |
| G-RP-FLIP | `RP_FLIP` | PASS | G-M3-REPLAY rows=3888 exact=3888 entry_bad=0 calls_bad=0 | arms bump=60 collect=60 no-op=3522 vanish=246 | MISMATCH lines 0 |
| G-RP-FLIP53 (diag) | `RP_FLIP53` | PASS | G-M3-REPLAY rows=3888 exact=3888 entry_bad=0 calls_bad=0 | arms bump=60 collect=60 no-op=3522 vanish=246 | MISMATCH lines 0 |
| G-READS-ON (OFF arm) | `RP_FIX_OFF` | PASS | rows=60 one_ret!=1_or_ptr0=0 distinct(one_ptr-idx*0xd04)=1 00d26940 |
| G-READS-ON (ON arm) | `RP_FIX_ON` | PASS | rows=60 one_ret!=1_or_ptr0=0 distinct(one_ptr-idx*0xd04)=1 00d26940 |
| G-DEF | `H_DEF + B_DEF_K0/K1/K26` | PASS | G-CTRL 8284be46/ba407a16/31fe043e (ref 8284be46/ba407a16/31fe043e)  B 1d5bf4f9/1d5bf4f9/1d5bf4f9 (ref 1d5bf4f9/1d5bf4f9/1d5bf4f9) |
