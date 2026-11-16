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
| RO-REF-FIXED-LOAD (k site) | the boot 0x00428af4 QPC call is made inside FUN_00429290's call 0x004967e0 (counts move with the callback, no record holds both returns) | boot call log with a deeper stack window (--call-log-boot, 32 dwords) | verify/ro_20261116/RESULT_RO-REF-FIXED-LOAD.md |
| RO-REF-FIXED-LOAD (f1) | 50 ms sleep holds did not bound piz read time because the run started under the memory cap | sleep-hold runs on an idle machine | verify/ro_20261116/RESULT_RO-REF-FIXED-LOAD.md |
| RO-REF-FIXED-LOAD (Interceptor AV) | Interceptor.attach at 0x004b6894 plus a JS GetOverlappedResult call clobbers edi (AV 0x004b6b40) | one run with an empty onEnter at 0x004b6894 | verify/ro_20261116/RESULT_RO-REF-FIXED-LOAD.md |
| RO-REF-NAME (exe read wait) | on the MASHED_ORIG_TOP exe the piz reads go through PIZ_FS (Match/PortTop_00402750_Pieces.cpp PortTop_PizFs_SetIdle_004b6560 records the 0x00429290 callback and never calls it), so the exe makes 0 of the original's 93 pre-tick-1 poll callbacks and its load length and k cannot equal 108 / 222 by a read wait; the arm may need the callback chain replayed per read | one exe replay arm with the MASHED_REPLAY_TRACE clock count read against the reference meta.json (race.dn_warp, bootlen.qpc) | verify/ro_20261116/RESULT_RO-REF-NAME.md |
