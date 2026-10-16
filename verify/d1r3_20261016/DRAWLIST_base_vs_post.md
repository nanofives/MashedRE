# Matched-pose per-draw lists, baseline (ec3301f3) vs rebased deletion

d1r1 recorder (MASHED_DBG_D3DREC) replaying original-side traces, run_replay3d.py. events = D3D9 draws recorded in the frame (librw's own draws included).

| track | shot | events | draws base | draws post | verdict | bmp base | bmp post | verdict |
|---|---|---:|---|---|---|---|---|---|
| arctic | stand_row_01051 | 308 | b60d5f315a | b60d5f315a | SAME | b62ea3808c | b62ea3808c | SAME |
| arctic | stand_row_01091 | 308 | 3528021392 | 3528021392 | SAME | de0ca6410c | de0ca6410c | SAME |
| arctic | stand_row_01126 | 308 | 44cdf289c9 | 44cdf289c9 | SAME | 602deca49d | 602deca49d | SAME |
| city | stand_row_00761 | 643 | 0ebbe72688 | 0ebbe72688 | SAME | d2d743e80c | d2d743e80c | SAME |
| city | stand_row_00801 | 643 | c3d5b3793c | c3d5b3793c | SAME | 819ebd05b9 | 819ebd05b9 | SAME |
| city | stand_row_00836 | 643 | f09bdd2dda | f09bdd2dda | SAME | 6dbdb8c550 | 6dbdb8c550 | SAME |
| training | stand_row_00884 | 469 | 81af68cb34 | 81af68cb34 | SAME | 293326c8e9 | 293326c8e9 | SAME |
| training | stand_row_00924 | 469 | 3f094c8f1e | 3f094c8f1e | SAME | cd44ba6b89 | cd44ba6b89 | SAME |
| training | stand_row_00959 | 469 | cc790e187f | cc790e187f | SAME | 380028dccd | 380028dccd | SAME |

draw lists identical 9/9; backbuffer BMPs identical 9/9.
