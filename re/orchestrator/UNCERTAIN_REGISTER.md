# [UNCERTAIN] register (Mariano 2026-11-16, run-51 policy)

An [UNCERTAIN] claim found by a worker gets ONE line here (claim, where it rests, the test that would settle it, the RESULT path).
It becomes a ledger row only when it blocks a milestone (T1, T2, the audio init lane, or a gate). L0 promotes a line to a row with
`roadmap.ps1 add` when that happens and strikes the line. NO-GUESSING is unchanged: the claim stays marked [UNCERTAIN] in its RESULT.

Rows parked here on 2026-11-16 (ledger stage blocked, note "parked: UNCERTAIN_REGISTER"):

| Row | Claim / gap | Test | Evidence |
|---|---|---|---|
| EQUIV-CALLER-0045AC20-FRIDA-RETS | 0x0045ac20's only caller is the table call (static scans) | Frida return-address sample in a race boot | verify/equivcaller_20261115/ |
| HB-TRAIN-MATCHTABLES-BUILD-RUN | object-build line on a pure match-table pass (self-test only) | L0 reads the first such train log | verify/ (HB-TRAIN-MATCHTABLES-BUILD) |
| HB-MASHEDLOCK-LEGACY-MARKER-DUALWRITE | old-code worktrees miss new-dir markers after the 1 h window | only if a long-lived old worktree appears | verify/hb_20261115/ |
| RO-CTL-FOCUSWORD | 0x0077391c flips at tick 31 in one run | name the writer with --watch | verify/ro_20261115/RESULT_RO-CTL-WARP-N.md |
| RWC-IMGREAD-DEPTH | frame-route PNG/BMP wrappers build 32-bit images, original depth unknown | original-side read of +0x0c/+0x18 of the returned RwImage | verify/rwcframetitlelogo_20261115/ |
| RO-CTL-PAD-68BC (+ -READWATCH) | 10 pad words of the 0x0045a736 copy have no reader | 5 rotated rw-watch runs | verify/ro_20261115/RESULT_RO-MASKS-REGEN.md |
| MATCH-ARITY-GETTER-ARGS | 5 sites where a zero-arg getter sits between the original's pushes | read each site's VS2022 object | verify/MATCH-ARITY-TRIAGE-1/ |
| MATCH-ARITY-TAILJMP-PASSTHRU | 1-param bodies tail-jumping into 3/4-param bodies | same | verify/MATCH-ARITY-TRIAGE-1/ |
| RWC-FRAME-CROSSFADE-PHASE | 2 extra crossfade draws in 2 of 4 dumps | log DAT_007f1010 per frame | verify/rwcframemovie_20261116/ |
| RWC-FRAME-MOVIE-TEARDOWN | AppDestroy closes the movie through 0x00494460 | log the close path | verify/rwcframemovie_20261116/ |
| GEN-RWCORE-PROVLIST-DRIFT | generator re-adds 94 dropped PROVISIONAL_RWC rows | make the generator read the drop list | verify/rwcframeclump_20261113/ |
| MATCH-551A50-BIND-CALLER | 0x00551a50 linked but unreferenced | bind a caller | verify/MATCH-TAG-FIX-005BF690/ |
| MATCH-43DFD0-RAISE-1-EDX | 0x0043dfd0 keeps edx live across call 0x0045b350 (0x0043e49b -> read 0x0043e4a3); the DDK reproduces it only when the TU defines the callee (naked RET). How the original TU knew the callee register use (same TU, LTCG) is unknown | check the other 32 callers of 0x0045b350 for a caller-saved register live across the call; then a /GL + /LTCG two-TU DDK compile | verify/match_20261116/RESULT_MATCH-43DFD0-RAISE-1.md |
| MATCH-43DFD0-RAISE-2 | slice-2 rewrites of re/match/s08/0043dfd0.c (case-10 order, 5 team-slot loops, base pointer) preserve behaviour | equiv.py new vs 7630e53ca source, or the PROVISIONAL knob-path menu walk ON vs OFF | verify/match_20261116/RESULT_MATCH-43DFD0-RAISE-2.md |
| MATCH-43DFD0-RAISE-3 | original source of 0x0043dfd0 lists the switch 2/3 cases in layout order (0x1f,0x21,0x20,4,0x13,0x1e,0xa,0x11,0x1c,0x12,0x18,6/7); rests on DDK c2 laying out case blocks in source order | byte match of the region, or a DDK test unit with permuted case order | verify/match_20261116/RESULT_MATCH-43DFD0-RAISE-3.md |
| MATCH-43DFD0-FRAME | hoisting `piVar13 = piVar18` above `if (0 < iVar8)` in the 5 team-slot loops of re/match/s08/0043dfd0.c preserves behaviour (piVar13 is read only inside those loops, and every later read at source lines 1267/1268/1282/1343/1363 follows its own assignment); rests on reading the source | equiv.py new vs 75a7be877 source, or the PROVISIONAL knob-path menu walk ON vs OFF | verify/match_20261116/RESULT_MATCH-43DFD0-FRAME.md |
| MATCH-43DFD0-DISPATCH | rewriting the uVar10 if-tree of re/match/s08/0043dfd0.c (base lines 522-1497) as `switch (uVar10)` preserves behaviour: every case value reaches the same body, every former `goto LAB_0043fc37` is the `default`, ff2c/ff2e falls into the ff71 body as `goto LAB_0043f1fd` did, ff37/ff39/ff4f break as before; rests on reading the source | equiv.py new vs 0048e45cb source, or the PROVISIONAL knob-path menu walk ON vs OFF | verify/match_20261116/RESULT_MATCH-43DFD0-DISPATCH.md |
| MATCH-43DFD0-STORES | the three edits to re/match/s08/0043dfd0.c (switch-1 case groups reordered, each group still ends in break or goto; 0x007f0f44..0x007f0f50 read into locals before the 0x1f/0x21 tests; case-8 tests read the 0x0067ed3c word in place of iVar8, which holds the same value because nothing between writes 0x0067ed3c or 0x0067e9f8) preserve behaviour; rests on reading the source | equiv.py new vs 0048e45cb source, or the PROVISIONAL knob-path menu walk ON vs OFF | verify/match_20261116/RESULT_MATCH-43DFD0-STORES.md |
