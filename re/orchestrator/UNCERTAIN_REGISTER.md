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
| HUDINIT-C-FONT-TEARDOWN-STATE | frame-route AppDestroy now frees the font through RwCore_00555830 (matched); not compared with the original's teardown (font entry objs, +0x12c buffer, freelist state after 0x00555830) | original-side write/return watch on 0x00555830 at AppDestroy (handle, entry count +0x130, return) vs the port's vs_frameT/ddk_frameT | verify/hudinit_20261116/RESULT_HUDINIT-C.md |
| RO-CTL-PAD-68BC (+ -READWATCH) | 10 pad words of the 0x0045a736 copy have no reader | 5 rotated rw-watch runs | verify/ro_20261115/RESULT_RO-MASKS-REGEN.md |
| MATCH-ARITY-GETTER-ARGS | 5 sites where a zero-arg getter sits between the original's pushes | read each site's VS2022 object | verify/MATCH-ARITY-TRIAGE-1/ |
| MATCH-ARITY-TAILJMP-PASSTHRU | 1-param bodies tail-jumping into 3/4-param bodies | same | verify/MATCH-ARITY-TRIAGE-1/ |
| RWC-FRAME-CROSSFADE-PHASE | 2 extra crossfade draws in 2 of 4 dumps | log DAT_007f1010 per frame | verify/rwcframemovie_20261116/ |
| RWC-FRAME-MOVIE-TEARDOWN | AppDestroy closes the movie through 0x00494460 | log the close path | verify/rwcframemovie_20261116/ |
| GEN-RWCORE-PROVLIST-DRIFT | generator re-adds 94 dropped PROVISIONAL_RWC rows | make the generator read the drop list | verify/rwcframeclump_20261113/ |
| MATCH-551A50-BIND-CALLER | 0x00551a50 linked but unreferenced | bind a caller | verify/MATCH-TAG-FIX-005BF690/ |
| T1-STANDIN-COUNT | the 59 stand-in + 32 under-stand-in BINDINGS.tsv count in ROADMAP T1 is the v5 figure, current count unknown | regenerate the count from verify/porttop_20261108/BINDINGS.tsv and the current PROVISIONAL.tsv | verify/t1_20261116/RESULT_ROADMAP-T1-STANDIN.md |
| T1-PORTTOP08-PHASE34 | RWC-FRAME-LADDER-CHECK says the phase 3 to 4 write and the rows stay PORT-TOP-08 work, ledger shows PORT-TOP-08 merged | read the PORT-TOP-08 RESULT for the 0x0067eca4 3 to 4 write | verify/t1_20261116/RESULT_ROADMAP-T1-STANDIN.md |
| T1-RWCORERUN-STAGE | RWC-CORE-RUN note ends at run 38 (first menu frame not reached), later RWC-FRAME-* rows merged with core frame draws; whether HUDINIT-C / RASTER-555FF0 / PORT-UNBLOCK-RWGLOBALS "after RWC-CORE-RUN" prerequisite is met | read RESULT_RWC-FRAME-CLUMP knob-ON boot and check 0x007d3ff8 is filled by the core engine open | verify/t1_20261116/RESULT_ROADMAP-T1-STANDIN.md |
| RWC-JITTER-STALE-UV | the u, v bytes of 80 of 82 title draws differ original vs core (pinned or not) because they are what an earlier draw left in DAT_00898a20 (re/match/s11/00473ee0.c writes no u, v); which draw leaves them is not traced | watch-write 0x00898a34 / 0x00898a38 on both sides during one title frame and compare the writers | verify/rwcframejitter_20261116/RESULT_RWC-FRAME-JITTER-STRICT.md |
