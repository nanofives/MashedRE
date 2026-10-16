# Matched-pose per-draw lists, baseline (b26d9f3c) vs rebased deletion

d1r1 recorder (MASHED_DBG_D3DREC) replaying original-side traces, run_replay3d.py. events = D3D9 draws recorded in the frame (librw's own draws included).

| track | shot | events | draws base | draws post | verdict | bmp base | bmp post | verdict |
|---|---|---:|---|---|---|---|---|---|
| arctic | stand_row_01051 | 308 | 7683033ada | 7683033ada | SAME | 6684aa9674 | 6684aa9674 | SAME |
| arctic | stand_row_01091 | 308 | 4d07dc2fad | 4d07dc2fad | SAME | 8aae5eacd1 | 8aae5eacd1 | SAME |
| arctic | stand_row_01126 | 308 | 72b6c26445 | 72b6c26445 | SAME | 532e4fad73 | 532e4fad73 | SAME |
| city | stand_row_00761 | 643 | 964ff51552 | 964ff51552 | SAME | ea7c4b637a | ea7c4b637a | SAME |
| city | stand_row_00801 | 643 | 0c72085245 | 0c72085245 | SAME | f609476911 | f609476911 | SAME |
| city | stand_row_00836 | 643 | 4c17ce358f | 4c17ce358f | SAME | 5a16ce4fbf | 5a16ce4fbf | SAME |
| training | stand_row_00884 | 469 | 63ae463973 | 63ae463973 | SAME | dff2c5a5be | dff2c5a5be | SAME |
| training | stand_row_00924 | 469 | aaac1c09f3 | aaac1c09f3 | SAME | efcf95e043 | efcf95e043 | SAME |
| training | stand_row_00959 | 469 | 7d5603169f | 7d5603169f | SAME | 07814e5dc9 | 07814e5dc9 | SAME |

draw lists identical 9/9; backbuffer BMPs identical 9/9.
