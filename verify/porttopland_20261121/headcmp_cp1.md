| | head (log/orch/run54_headcheck) | here |
|---|---|---|
| identical | 1976 | 2321 |
| different | 32 | 71 |
| not_in_map | 27 | 29 |
| functions | 2035 | 2421 |
| units | 2231 | 2579 |
| units_linked | 1966 | 2308 |
| provisional_units_skipped | 253 | 254 |
| refusals | {'fail:externalized-static-register-convention': 8, 'fail:wrapper-redefines-globals': 4} | {'fail:externalized-static-register-convention': 13, 'fail:wrapper-redefines-globals': 4} |
| dual body rvas | 32 | 32 |

Dual body: left none, new none.

### Newly DIFFERENT (39)

| rva | at head | src | diff bytes / size | class |
|---|---|---|---|---|
| 0x00401690 | not checked | re/match/s01/00401690.c | 713 / 1280 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x00408590 | not checked | re/match/s01/00408590.c | 12 / 128 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x00408610 | not checked | re/match/s01/00408610.c | 44 / 1088 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x0040c690 | not checked | re/match/racepath/0040c690.c | 642 / 880 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x0040e590 | not checked | re/match/t01g/0040e590.c | 841 / 2384 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x00410d10 | not checked | re/match/s02/00410d10.c | 98 / 1104 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x00411ae0 | not checked | re/match/racepath/00411ae0.c | 17 / 512 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x0041d930 | not checked | re/match/s04/0041d930.c | 14 / 336 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x0041fcc0 | not checked | re/match/s05/00420050.c | 7 / 336 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x00420050 | not checked | re/match/s05/00420050.c | 9 / 480 | ADMITTED (re/match/ADMITTED.tsv) |
| 0x004212b0 | not checked | re/match/s05/004212b0.c | 540 / 688 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x00421d20 | not checked | re/match/s05/00421d20.c | 134 / 944 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x00422ba0 | not checked | re/match/s05/00422ba0.c | 22 / 1072 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x00424b80 | not checked | re/match/s05/00424b80.c | 26 / 816 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x004252c0 | not checked | re/match/s05/004252c0.c | 606 / 1920 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x00425ca0 | not checked | re/match/s05/00425ca0.c | 32 / 176 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x00429310 | not checked | re/match/s06/00429310.c | 16 / 656 | ADMITTED (re/match/ADMITTED.tsv) |
| 0x00429bd0 | not checked | re/match/s06/00429bd0.c | 8 / 576 | ADMITTED (re/match/ADMITTED.tsv) |
| 0x00474de0 | not checked | re/match/s11/00474de0.c | 12 / 128 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x004770a0 | not checked | re/match/s12/004770a0.c | 1 / 32 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x00477e60 | not checked | re/match/s12/00477e60.c | 3 / 160 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x00477f50 | not checked | re/match/s12/00477f50.c | 29 / 224 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x0047cea0 | not checked | re/match/s13/0047cea0.c | 3 / 128 | ADMITTED (re/match/ADMITTED.tsv) |
| 0x004818d0 | not checked | re/match/s13/004818d0.c | 6 / 352 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x00484de0 | not checked | re/match/s14/00484de0.c | 8 / 400 | ADMITTED (re/match/ADMITTED.tsv) |
| 0x00484f70 | not checked | re/match/s14/00484f70.c | 238 / 272 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x00485ef0 | not checked | re/match/s14/00485ef0.c | 540 / 656 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x004864f0 | not checked | re/match/s14/004864f0.c | 9 / 288 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x00486830 | not checked | re/match/racepath/00486830.c | 1402 / 1680 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x00487020 | not checked | re/match/s14/00487020.c | 7 / 288 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x0048a4e0 | not checked | re/match/s14/0048a4e0.c | 2 / 240 | ADMITTED (re/match/ADMITTED.tsv) |
| 0x0048ab70 | not checked | re/match/racepath/0048ab70.c | 39 / 480 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x0048b7c0 | not checked | re/match/s14/0048b7c0.c | 347 / 416 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x0048d360 | not checked | re/match/s15/0048d360.c | 143 / 464 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x0048d540 | not checked | re/match/s15/0048d540.c | 143 / 464 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x00490500 | not checked | re/match/s15/00490500.c | 1986 / 2384 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x004b61c0 | not checked | re/match/s18/004b61c0.c | 428 / 704 | ADMITTED (re/match/ADMITTED.tsv) |
| 0x004c1b40 | not checked | re/match/s19/004c1b40.c | 5 / 112 | replay-C4 body (hooks.csv c4=frida-replay) |
| 0x0055b650 | not checked | re/match/rwl05/0055b650.c | 212 / 256 | replay-C4 body (hooks.csv c4=frida-replay) |

Tally: {'replay-C4': 32, 'ADMITTED': 7}

### Listed / refused unit -> 100% neighbour on its VS2022 body

| unit (src) | status | rvas in the unit with c4=match (fell back) | listed / provisional rvas in the unit | at head |
|---|---|---|---|---|
| re/match/s16/00498810.c | fail:externalized-static-register-convention | 0x00498860 | 0x00498810 | same |
| re/match/s03/h2_00414180.c | fail:wrapper-redefines-globals | 0x00414180 | - | same |
| re/match/s06/00427e00.c | fail:externalized-static-register-convention | 0x00427e00 | - | same |
| re/match/s06/00427f00.c | fail:externalized-static-register-convention | 0x00427f00 | - | same |
| re/match/s06/00427ff0.c | fail:externalized-static-register-convention | 0x00427ff0 | - | same |
| re/match/s06/00428140.c | fail:externalized-static-register-convention | 0x00428140 | - | same |
| re/match/s01/00404e80.c | fail:wrapper-redefines-globals | 0x00404e80 0x00404ee0 | - | same |
| re/match/s01/00404e50.c | fail:wrapper-redefines-globals | 0x00404e50 0x00404f50 0x00404f80 | - | same |
| re/match/s09/00458920.c | fail:externalized-static-register-convention | 0x00458920 | - | same |
| re/match/s01/00404e00.c | fail:wrapper-redefines-globals | 0x00404e00 0x00404e20 | - | same |
| re/match/s16/00495fe0.c | fail:externalized-static-register-convention | 0x00495fe0 | 0x00495870 | same |
| re/match/s04/0041c090.c | fail:externalized-static-register-convention | 0x0041b7a0 0x0041c090 | 0x0041b7a0 | NEW |
| re/match/s04/0041c410.c | fail:externalized-static-register-convention | 0x0041cc50 | 0x0041c410 | NEW |
| re/match/s04/0041d830.c | fail:externalized-static-register-convention | 0x0041d830 | 0x0041ce00 | NEW |
| re/match/s01/004036a0.c | fail:externalized-static-register-convention | 0x00403910 | 0x004036a0 | NEW |
| re/match/s16/00496c10.c | fail:externalized-static-register-convention | 0x00496c10 | 0x004969a0 | NEW |
