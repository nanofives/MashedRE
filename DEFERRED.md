# Deferred

Things deliberately not being worked on right now. Each row has a rationale and a re-pickup condition. Pure dumping ground; no analysis, no code lives here.

A row goes into DEFERRED when:
- It's outside the current phase's scope.
- It depends on something not-yet-done and we don't want to track that as an uncertainty.
- It's a "nice to have" that doesn't block any DoD.
- A stub or uncertainty is explicitly accepted as `wontfix` for v1.

## Active

| ID | Title | Why deferred | Re-pickup when | Phase tag |
|----|-------|--------------|----------------|-----------|
| D-0001 | Depth-2 callees of entry+depth-1 subset (boot CRT chain) | Out of scope for this session; only entry+depth-1 targeted | boot subsystem sweep session opened for CRT init chain | boot |
| D-0040 | depth-3 callees of 0x00402750: 004025f0,004026d0,00402f50,00403640,00404830,0040bb30,0040bb50,0040bbb0,004113b0,00412890,00418980,0041a1e0,0041b450,0041bec0,0041c100,0041cb10,0041d6e0,0041d8b0,0041db90,0041def0,0041eaa0,00420d00,00425bc0,004274d0,004274e0,004275d0,00427ca0,00428390,004283a0,00431b40,0045b350,0045bae0,004669b0,004671a0,00471eb0,004723d0,0047ba00,00484170,004841d0,004881d0,00494c80,00495280,004952f0,00495350,00496e40,00498bc0,00498bd0,00499ce0,004b3d80,004b6540,004b6560,004c2ed0,004c2f00,004caea0,004d8560,00558240 | depth-3 of 0x00492370; not recursed | boot subsystem depth-3 sweep session | boot |
| D-0041 | depth-3 callees of 0x00402a40 (unique vs D-0040): 004014b0,004015a0,00403660,0040bd00,0040cf80,0040cfd0,004114c0,004189e0,0041a3d0,0041b660,0041c0e0,0041c2c0,0041ccf0,0041d890,0041da80,0041de70,0041e0d0,0041ffb0,00421590,00425ed0,00426ba0,00426c00,00427620,00428400,0042c2a0,0045b930,00467010,00467020,00467070,0047ba10,00484130,00489250,00494bc0,00494ef0,00494f20,00496ce0,004b4880,004b6550,005581f0 | depth-3 of 0x00492370; not recursed | boot subsystem depth-3 sweep session | boot |
| D-0042 | depth-3 callees of 0x00492270+0x00492290+0x004924f0: 00428590,004921d0,00493710,004926c0,00492770,004929d0,00492d20,00492d30,00492e90,00493480,00499690,00431ae0,00431af0,00431b00,00431b10,00431d00 | depth-3 of 0x00492370; not recursed | boot subsystem depth-3 sweep session | boot |
| D-0043 | depth-3 callees of thunk targets 0x00495150+0x004938c0+0x004954f0: 00495120,004955b0,004960a0,004963b0,00498510,00499710,004c2f60,004c3040,004c3270,00550390,00558470,00498b60,004963d0,00496010,004955c0,00498bf0 | depth-3 of 0x00492370 via thunks; not recursed | boot subsystem depth-3 sweep session | boot |
| D-0044 | depth-3 callees of 0x00493900 (internal): 00499730,004a2be9,004a3ac9,004b302f | depth-3 of 0x00492370; not recursed; S-0001+S-0002 filed for key stubs | boot subsystem depth-3 sweep session | boot |
| D-0045 | depth-3 callees of 0x00499ba0+0x004c5930: 00499890,004c5a60,004d8060 | depth-3 of 0x00492370; not recursed; S-0003+S-0004+S-0005 filed | boot subsystem depth-3 sweep session | boot |
| D-0160 | 0x004a45cf __nh_malloc | depth-2 of _malloc (boot_crt_env session) | boot_crt_env-cont1 | boot |
| D-0161 | 0x004a4660 FUN_004a4660 | depth-2 of _free (boot_crt_env session) | boot_crt_env-cont1 | boot |
| D-0162 | 0x004a787f __lock | depth-2 of _free (boot_crt_env session) | boot_crt_env-cont1 | boot |
| D-0163 | 0x004aa497 ___sbh_find_block | depth-2 of _free (boot_crt_env session) | boot_crt_env-cont1 | boot |
| D-0164 | 0x004aa4c2 ___sbh_free_block | depth-2 of _free (boot_crt_env session) | boot_crt_env-cont1 | boot |
| D-0165 | 0x004ac570 FUN_004ac570 | depth-2 of FUN_004ac560 tail-call (boot_crt_env session) | boot_crt_env-cont1 | boot |
| D-0166 | 0x004ae28f ___crtInitCritSecNoSpinCount@8 | depth-2 of ___crtInitCritSecAndSpinCount (boot_crt_env session) | boot_crt_env-cont1 | boot |
| D-0167 | 0x004af166 __setmbcp | depth-2 of ___initmbctable (boot_crt_env session) | boot_crt_env-cont1 | boot |
| D-0168 | 0x004affaf FUN_004affaf | depth-2 of FUN_004affe0 (boot_crt_env session) | boot_crt_env-cont1 | boot |
| D-0460 | 0x004ac570 FUN_004ac570 | depth-3 of FUN_004ab8d6; supersedes D-0119; not taken in boot_crt_exit_d3 (cap 18) | pick up as bucket boot_crt_exit_d3-cont1; no further recursion | boot |
| D-0461 | 0x004af32d FUN_004af32d | depth-3 of FUN_004a3258; supersedes D-0106; not taken in boot_crt_exit_d3 (cap 18) | pick up as bucket boot_crt_exit_d3-cont1; no further recursion | boot |
| D-0462 | 0x004a2bb8 report_failure | depth-4 of 0x004a2be9 __security_check_cookie; not recursed | boot_crt_exit_d3-cont1 or dedicated boot session | boot |
| D-0463 | 0x004a31e1 FUN_004a31e1 | depth-4 of 0x004a4126 __onexit; called before __onexit_lk | boot_crt_exit_d3-cont1 | boot |
| D-0464 | 0x004a407e __onexit_lk | depth-4 of 0x004a4126 __onexit; core logic of __onexit | boot_crt_exit_d3-cont1 | boot |
| D-0465 | 0x004a4158 FUN_004a4158 | depth-4 of 0x004a4126 __onexit; called after __onexit_lk | boot_crt_exit_d3-cont1 | boot |
| D-0466 | 0x004ad33b __controlfp | depth-4 of 0x004a5de3 FUN_004a5de3; called with (0x10000,0x30000) | boot_crt_exit_d3-cont1 | boot |
| D-0467 | 0x004a5df5 __ms_p5_test_fdiv | depth-4 of 0x004a5e35 __ms_p5_mp_test_fdiv; fallback FPU test | boot_crt_exit_d3-cont1 | boot |
| D-0468 | 0x004a9744 __flushall | depth-4 of 0x004a5f07 ___endstdio; unconditional flush | boot_crt_exit_d3-cont1 | boot |
| D-0469 | 0x004ad351 __fcloseall | depth-4 of 0x004a5f07 ___endstdio; conditional close via DAT_007739d4 | boot_crt_exit_d3-cont1 | boot |
| D-0470 | 0x004a7800 FUN_004a7800 | depth-4 of 0x004a787f __lock; lazy-init of lock slot | boot_crt_exit_d3-cont1 | boot |
| D-0471 | 0x004aa7da ___sbh_alloc_new_region | depth-4 of 0x004aac76 ___sbh_alloc_block; allocates new SBH region | boot_crt_exit_d3-cont1 | boot |
| D-0472 | 0x004aa891 ___sbh_alloc_new_group | depth-4 of 0x004aac76 ___sbh_alloc_block; initialises new SBH group | boot_crt_exit_d3-cont1 | boot |
| ~~D-0580~~ | ~~0x004ccce0 FUN_004ccce0~~ | RESOLVED 2026-05-03: analyzed in rw_engine_teardown_d3 session; see re/analysis/rw_engine_teardown_d3/0x004ccce0.md | — | render |
| ~~D-0581~~ | ~~0x004cc9f0 FUN_004cc9f0~~ | RESOLVED 2026-05-03: already mapped in re/analysis/render_d3d9_device/0x004cc9f0.md prior to this session | — | render |
| D-0281 | 0x004954f0 FUN_004954f0 (HardwareExitApplication) and its callees: 0x00498bf0,ShowCursor,0x00498b60,0x0045b350,thunk_FUN_00496370,0x00496010,thunk_FUN_00495580 | sister function to FUN_004938c0; called by FUN_00492370 for hardware-layer teardown; not in rw_engine_teardown subset (callee of outer shell FUN_00492370, not of RW_TEAR_FN) | pick up as bucket rw_hw_teardown; after rw_engine_teardown-cont1 finishes; confirm if D3D teardown path is here | render |
| D-0220 | 0x004c9f50 FUN_004c9f50, 0x004c9f60 FUN_004c9f60 | early-finish from rw_engine_init-20260502-1734 (cap_count reached 18 at fn 18/20); depth-1 callees of 0x00493710 RW_INIT_FN | pick up as bucket rw_engine_init-cont1; depth-1 of RW_INIT_FN; no further recursion | render |
| D-0221 | 0x004cbc60,0x004cbc70,0x004cbc80,0x004cbc90,0x00550350,0x00550390,0x005584c0,0x005c9d00 | subset cap-split from rw_engine_init-20260502-1734 (28 callees exceeded hard cap 20); depth-1 callees of 0x00493710 RW_INIT_FN (S-0060..S-0067 filed) | pick up as bucket rw_engine_init-cont1; depth-1 of RW_INIT_FN; no further recursion | render |
| D-0222 | 0x004ce790 FUN_004ce790 | depth-2 of 0x00493600 FUN_00493600; called twice with (ptr-to-global, fn-ptr, fn-ptr-or-label) pattern | rw_engine_init-cont1 sweep or rw_plugin_reg session | render |
| D-0223 | 0x00496400 FUN_00496400 | depth-2 of 0x004951f0 FUN_004951f0; called with log string args | rw_engine_init-cont1 | render |
| D-0224 | 0x00498b60 FUN_00498b60 | depth-2 of 0x004951f0 FUN_004951f0; called on failure path | rw_engine_init-cont1 | render |
| D-0225 | 0x00498bf0 FUN_00498bf0 | depth-2 of 0x004951f0 FUN_004951f0; result checked; if non-zero calls ShowCursor(0) | rw_engine_init-cont1 | render |
| D-0226 | 0x00499400 FUN_00499400 | depth-2 of 0x004951f0 FUN_004951f0; called with DAT_006147bc as arg | rw_engine_init-cont1 | render |
| D-0227 | 0x004cbb60 FUN_004cbb60 | depth-2 of 0x004951f0 FUN_004951f0; return stored to DAT_00771e58; dereffed at +0xC4 and +0xCC | rw_engine_init-cont1 | render |
| D-0228 | 0x00499710 FUN_00499710 | depth-2 of 0x00495270 FUN_00495270; return value forwarded via *param_1 to FUN_004c30b0 | rw_engine_init-cont1 | render |
| D-0229 | 0x004c2c90 FUN_004c2c90 | depth-2 of 004c2ed0+004c2f00+004c2fb0+004c3040+004c30b0; also S-0081 from rw_engine_teardown; called with (DAT_007d3ff8+0x10, id, ...) pattern; ids seen: 0,2,3,4,6,10,11,17,0xb,0x12 | rw_engine_init-cont1 | render |
| D-0230 | 0x004cf160 FUN_004cf160 | depth-2 of 0x004c2fb0 FUN_004c2fb0; receives *(DAT_007d3ff8+0x10) dereference as arg | rw_engine_init-cont1 | render |
| D-0231 | 0x004d8000 FUN_004d8000 | depth-2 of 0x004c2fb0 FUN_004c2fb0; called with (&DAT_00617fe0, DAT_007d3ff8); result checked | rw_engine_init-cont1 | render |
| D-0232 | 0x004cae90 FUN_004cae90 | depth-2 of 0x004c30b0 FUN_004c30b0; result checked; gating further execution | rw_engine_init-cont1 | render |
| D-0233 | 0x004d7ff0 FUN_004d7ff0 | depth-2 of 0x004c30b0 FUN_004c30b0; called with HRESULT-style codes 0x80000001/0x80000013/0x80000016 | rw_engine_init-cont1 | render |
| D-0234 | 0x004d8480 FUN_004d8480 | depth-2 of 0x004c30b0 FUN_004c30b0; called with &local_8 on error paths | rw_engine_init-cont1 | render |
| D-0235 | 0x004d7ca0 FUN_004d7ca0 | depth-2 of 004c3270+004c32b0; also S-0082 from rw_engine_teardown | rw_engine_init-cont1 | render |
| D-0236 | 0x004ccf20 FUN_004ccf20 | depth-2 of 004c3270+004c32b0; also S-0083 from rw_engine_teardown | rw_engine_init-cont1 | render |
| D-0237 | 0x004c7a60 thunk_FUN_004cfa00 | depth-2 of 0x004c32b0 FUN_004c32b0; called after all FUN_004d7de0 plugin-reg calls succeed | rw_engine_init-cont1 | render |
| D-0238 | 0x004cc7e0 FUN_004cc7e0 | depth-2 of 0x004c32b0 FUN_004c32b0; called with bVar17 (param_2&1 result) | rw_engine_init-cont1 | render |
| D-0239 | 0x004cce20 FUN_004cce20 | depth-2 of 0x004c32b0 FUN_004c32b0; called with param_1 | rw_engine_init-cont1 | render |
| D-0240 | 0x004d7c60 FUN_004d7c60 | depth-2 of 0x004c32b0 FUN_004c32b0; result checked | rw_engine_init-cont1 | render |
| D-0241 | 0x004d7de0 FUN_004d7de0 | depth-2 of 0x004c32b0 FUN_004c32b0; called 14× with (&DAT_00617fe0, size, id, cb1, cb2, 0) pattern; ids 0x401–0x40f + 0x412 | rw_engine_init-cont1 | render |
| D-0242 | 0x004d8560 FUN_004d8560 | depth-2 of 0x004c32b0 FUN_004c32b0; no args; result OR'd with FUN_004d7de0 results | rw_engine_init-cont1 | render |
| D-0243 | 0x004d8570 FUN_004d8570 | depth-2 of 0x004c32b0 FUN_004c32b0; result checked for non-zero before main work | rw_engine_init-cont1 | render |
| D-0244 | 0x004e5d30 FUN_004e5d30 | depth-2 of 0x00493640 FUN_00493640; first in plugin-chain | rw_engine_init-cont1 | render |
| D-0245 | 0x00543e50 FUN_00543e50 | depth-2 of 0x00493640 FUN_00493640 | rw_engine_init-cont1 | render |
| D-0246 | 0x0053eaa0 FUN_0053eaa0 | depth-2 of 0x00493640 FUN_00493640 | rw_engine_init-cont1 | render |
| D-0247 | 0x0053d0b0 FUN_0053d0b0 | depth-2 of 0x00493640 FUN_00493640 | rw_engine_init-cont1 | render |
| D-0248 | 0x00538600 FUN_00538600 | depth-2 of 0x00493640 FUN_00493640 | rw_engine_init-cont1 | render |
| D-0249 | 0x00534a80 FUN_00534a80 | depth-2 of 0x00493640 FUN_00493640 | rw_engine_init-cont1 | render |
| D-0250 | 0x00534920 FUN_00534920 | depth-2 of 0x00493640 FUN_00493640 | rw_engine_init-cont1 | render |
| D-0251 | 0x00546530 FUN_00546530 | depth-2 of 0x00493640 FUN_00493640 | rw_engine_init-cont1 | render |
| D-0252 | 0x005336d0 FUN_005336d0 | depth-2 of 0x00493640 FUN_00493640 | rw_engine_init-cont1 | render |
| D-0253 | 0x0052e310 FUN_0052e310 | depth-2 of 0x00493640 FUN_00493640 | rw_engine_init-cont1 | render |
| D-0254 | 0x00544d20 FUN_00544d20 | depth-2 of 0x00493640 FUN_00493640 | rw_engine_init-cont1 | render |
| D-0255 | 0x00549640 FUN_00549640 | depth-2 of 0x00493640 FUN_00493640 | rw_engine_init-cont1 | render |
| D-0256 | 0x005578a0 FUN_005578a0 | depth-2 of 0x00493640 FUN_00493640 | rw_engine_init-cont1 | render |
| D-0257 | 0x005515a0 FUN_005515a0 | depth-2 of 0x00493640 FUN_00493640 | rw_engine_init-cont1 | render |
| D-0258 | 0x0052d8e0 FUN_0052d8e0 | depth-2 of 0x00493640 FUN_00493640; result NOT checked | rw_engine_init-cont1 | render |
| D-0259 | 0x0057c270 FUN_0057c270 | depth-2 of 0x00493640 FUN_00493640 | rw_engine_init-cont1 | render |
| D-0260 | 0x00561ee0 FUN_00561ee0 | depth-2 of 0x00493640 FUN_00493640 | rw_engine_init-cont1 | render |
| D-0261 | 0x00472380 FUN_00472380 | depth-2 of 0x00493640 FUN_00493640; last in plugin-chain | rw_engine_init-cont1 | render |
| D-0262 | 0x004a504f FUN_004a504f | depth-2 of 0x004a2cbd FID_conflict:_wprintf; internal formatting core | rw_engine_init-cont1 | render |
| D-0263 | 0x004a2d18 FUN_004a2d18 | depth-2 of 0x004a2cbd FID_conflict:_wprintf; called after ftbuf | rw_engine_init-cont1 | render |

| D-0520 | 0x00550350 FUN_00550350 | remainder from D-0221; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0521 | 0x00550390 FUN_00550390 | remainder from D-0221; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0522 | 0x005584c0 FUN_005584c0 | remainder from D-0221; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0523 | 0x005c9d00 FUN_005c9d00 | remainder from D-0221; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0524 | 0x004ce790 FUN_004ce790 | from D-0222; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0525 | 0x004cf160 FUN_004cf160 | from D-0230; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0526 | 0x004d8000 FUN_004d8000 | from D-0231; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0527 | 0x004d7ff0 FUN_004d7ff0 | from D-0233; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0528 | 0x004d8480 FUN_004d8480 | from D-0234; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0529 | 0x004d7ca0 FUN_004d7ca0 | from D-0235; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0530 | 0x004ccf20 FUN_004ccf20 | from D-0236; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0531 | 0x004cc7e0 FUN_004cc7e0 | from D-0238; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0532 | 0x004cce20 FUN_004cce20 | from D-0239; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0533 | 0x004d7c60 FUN_004d7c60 | from D-0240; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0534 | 0x004d7de0 FUN_004d7de0 | from D-0241; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0535 | 0x004d8560 FUN_004d8560 | from D-0242; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0536 | 0x004d8570 FUN_004d8570 | from D-0243; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0537 | 0x004e5d30 FUN_004e5d30 | from D-0244; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0538 | 0x00543e50 FUN_00543e50 | from D-0245; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0539 | 0x0053eaa0 FUN_0053eaa0 | from D-0246; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0540 | 0x0053d0b0 FUN_0053d0b0 | from D-0247; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0541 | 0x00538600 FUN_00538600 | from D-0248; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0542 | 0x00534a80 FUN_00534a80 | from D-0249; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0543 | 0x00534920 FUN_00534920 | from D-0250; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0544 | 0x00546530 FUN_00546530 | from D-0251; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0545 | 0x005336d0 FUN_005336d0 | from D-0252; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0546 | 0x0052e310 FUN_0052e310 | from D-0253; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0547 | 0x00544d20 FUN_00544d20 | from D-0254; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0548 | 0x00549640 FUN_00549640 | from D-0255; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0549 | 0x005578a0 FUN_005578a0 | from D-0256; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0550 | 0x005515a0 FUN_005515a0 | from D-0257; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0551 | 0x0052d8e0 FUN_0052d8e0 | from D-0258; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0552 | 0x0057c270 FUN_0057c270 | from D-0259; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0553 | 0x00561ee0 FUN_00561ee0 | from D-0260; not analyzed in rw_engine_init_d2-20260502-1905 (cap=18) | pick up as bucket rw_engine_init_d2-cont1; same depth; no further recursion | render |
| D-0554 | 0x004c2d90 FUN_004c2d90 | depth-3 callee of 0x00472380 (S-0200); not recursed in rw_engine_init_d2 | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-0555 | 0x004e7d40 FUN_004e7d40 | depth-3 callee of 0x00472380 (S-0201); not recursed | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-0556 | 0x004a42c5 FUN_004a42c5 | depth-3 callee of 0x00496400 (already S-0260); not recursed | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-0557 | 0x00498950 FUN_00498950 | depth-3 callee of 0x00499400 (S-0202); not recursed | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-0558 | 0x004989b0 FUN_004989b0 | depth-3 callee of 0x00499400 (S-0203); not recursed | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-0559 | 0x00498a00 FUN_00498a00 | depth-3 callee of 0x00499400 (S-0204); not recursed | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-0560 | 0x00498c00 FUN_00498c00 | depth-3 callee of 0x00499400 (S-0205); not recursed | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-0561 | 0x00498e40 FUN_00498e40 | depth-3 callee of 0x00499400 (S-0206); not recursed | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-0562 | 0x00498ea0 FUN_00498ea0 | depth-3 callee of 0x00499400 (S-0207); not recursed | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-0563 | 0x004c2e70 FUN_004c2e70 | depth-3 callee of 0x00499400 (S-0208); not recursed | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-0564 | 0x004c2f30 FUN_004c2f30 | depth-3 callee of 0x00499400 (S-0209); not recursed | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-0565 | 0x004a4fc1 write_char | depth-3 callee of 0x004a504f (S-0210); not recursed | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-0566 | 0x004a4ff4 write_multi_char | depth-3 callee of 0x004a504f (S-0211); not recursed | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-0567 | 0x004a5018 write_string | depth-3 callee of 0x004a504f (S-0212); not recursed | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-0568 | 0x004a4da0 __aulldvrm | depth-3 callee of 0x004a504f (S-0213); not recursed | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-0569 | 0x004ad1e0 FUN_004ad1e0 | depth-3 callee of 0x004a504f (S-0214); not recursed | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-0570 | 0x004cfa00 FUN_004cfa00 | depth-3 thunk target of 0x004c7a60 (S-0215); not recursed | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-0571 | 0x004c7690 FUN_004c7690 | depth-3 callee of FUN_004cfa00 via 0x004c7a60 (S-0216); not recursed | rw_engine_init_d2-cont1 depth-3 sweep | render |
| D-6160 | 0x0042a940 FUN_0042a940 | hud_frontend_d3 early-finish (cap=18); was D-2750; powerup sprite index by selection | hud_frontend_d3-cont1 sweep session | frontend |
| D-6161 | 0x0042ac00 FUN_0042ac00 | hud_frontend_d3 early-finish; was D-2746; player count / slot index getter | hud_frontend_d3-cont1 sweep session | frontend |
| D-6162 | 0x0042ac50 FUN_0042ac50 | hud_frontend_d3 early-finish; was D-2749; layout Y base getter | hud_frontend_d3-cont1 sweep session | frontend |
| D-6163 | 0x0042bcb0 FUN_0042bcb0 | hud_frontend_d3 early-finish; was D-2742; depth-3 of FUN_004335f0/FUN_0043a610/FUN_0043aa30 | hud_frontend_d3-cont1 sweep session | frontend |
| D-6164 | 0x0042d290 FUN_0042d290 | hud_frontend_d3 early-finish; was D-2759; lap time formatter | hud_frontend_d3-cont1 sweep session | frontend |
| D-6165 | 0x0042d300 FUN_0042d300 | hud_frontend_d3 early-finish; was D-2760; lap time comparator | hud_frontend_d3-cont1 sweep session | frontend |
| D-6166 | 0x0042ebe0 FUN_0042ebe0 | hud_frontend_d3 early-finish; was D-2775; AI/remote slot occupancy check | hud_frontend_d3-cont1 sweep session | frontend |
| D-6167 | 0x0042ee00 FUN_0042ee00 | hud_frontend_d3 early-finish; was D-2782; vehicle icon by unlock state | hud_frontend_d3-cont1 sweep session | frontend |
| D-6168 | 0x0042ee40 FUN_0042ee40 | hud_frontend_d3 early-finish; was D-2776; vehicle sprite getter (slot type) | hud_frontend_d3-cont1 sweep session | frontend |
| D-6169 | 0x0042ef40 FUN_0042ef40 | hud_frontend_d3 early-finish; was D-2778; vehicle lock-state check | hud_frontend_d3-cont1 sweep session | frontend |
| D-6170 | 0x0042f8d0 FUN_0042f8d0 | hud_frontend_d3 early-finish; was D-2745; background rect draw | hud_frontend_d3-cont1 sweep session | frontend |
| D-6171 | 0x0042fab0 FUN_0042fab0 | hud_frontend_d3 early-finish; was D-2741; depth-3 of FUN_004335f0/FUN_0043a610/FUN_00434720/FUN_0043aa30 | hud_frontend_d3-cont1 sweep session | frontend |
| D-6172 | 0x00430760 FUN_00430760 | hud_frontend_d3 early-finish; was D-2740; depth-3 of FUN_004335f0/FUN_00439210 | hud_frontend_d3-cont1 sweep session | frontend |
| D-6173 | 0x00430830 FUN_00430830 | hud_frontend_d3 early-finish; was D-2751; split-screen track check | hud_frontend_d3-cont1 sweep session | frontend |
| D-6174 | 0x00430a10 FUN_00430a10 | hud_frontend_d3 early-finish; was D-2779; get player at slot type 0 | hud_frontend_d3-cont1 sweep session | frontend |
| D-6175 | 0x00430a60 FUN_00430a60 | hud_frontend_d3 early-finish; was D-2780; get player at slot type 1 | hud_frontend_d3-cont1 sweep session | frontend |
| D-6176 | 0x00430ab0 FUN_00430ab0 | hud_frontend_d3 early-finish; was D-2781; get player at slot type 2 | hud_frontend_d3-cont1 sweep session | frontend |
| D-6177 | 0x00430b30 FUN_00430b30 | hud_frontend_d3 early-finish; was D-2758; lap time getter (3 out-params) | hud_frontend_d3-cont1 sweep session | frontend |
| ~~D-6178~~ | ~~0x004368e0 FUN_004368e0~~ | RESOLVED 2026-05-11: analyzed C1 session hud_frontend_d5-20260511-1710; see re/analysis/hud_frontend_d5/0x004368e0.md | — | frontend |
| ~~D-6179~~ | ~~0x00436810 FUN_00436810~~ | RESOLVED 2026-05-11: analyzed C1 session hud_frontend_d5-20260511-1710; see re/analysis/hud_frontend_d5/0x00436810.md | — | frontend |
| ~~D-6180~~ | ~~0x004391b0 FUN_004391b0~~ | RESOLVED 2026-05-11: analyzed C1 session hud_frontend_d5-20260511-1710; see re/analysis/hud_frontend_d5/0x004391b0.md | — | frontend |
| ~~D-6181~~ | ~~0x00458630 FUN_00458630~~ | RESOLVED 2026-05-11: analyzed C1 session hud_frontend_d5-20260511-1710; see re/analysis/hud_frontend_d5/0x00458630.md | — | frontend |
| ~~D-6182~~ | ~~0x00473870 FUN_00473870~~ | RESOLVED 2026-05-11: analyzed C1 session hud_frontend_d5-20260511-1710; see re/analysis/hud_frontend_d5/0x00473870.md | — | frontend |
| ~~D-6183~~ | ~~0x004736c0 FUN_004736c0~~ | RESOLVED 2026-05-11: analyzed C1 session hud_frontend_d5-20260511-1710; see re/analysis/hud_frontend_d5/0x004736c0.md | — | frontend |
| ~~D-6184~~ | ~~0x00474e60 FUN_00474e60~~ | RESOLVED 2026-05-11: analyzed C1 session hud_frontend_d5-20260511-1710; see re/analysis/hud_frontend_d5/0x00474e60.md | — | frontend |
| D-7000 | 0x004a3f90 __global_unwind2 | depth-3 callee of _longjmp (S-2360); not recursed in memory_pool_d2 | memory_pool_d2-cont1 sweep session | boot |
| D-7001 | 0x004a3fd2 __local_unwind2 | depth-3 callee of _longjmp (S-2361); not recursed in memory_pool_d2 | memory_pool_d2-cont1 sweep session | boot |
| D-7002 | 0x004a4066 FUN_004a4066 | depth-3 callee of _longjmp (S-2362); not recursed in memory_pool_d2 | memory_pool_d2-cont1 sweep session | boot |
| D-7003 | 0x005c318c __rt_probe_read4@4 | depth-3 callee of _longjmp (S-2363); not recursed in memory_pool_d2 | memory_pool_d2-cont1 sweep session | boot |
| D-8140 | debug_overlay_d2 bucket (J6 slot: pool13, U=2747..2766, D=8140..8199, S=2740..2759) | d1 session returned hard negative — retail binary has no debug overlay code; d2 halt rule triggered (0 deferred rows < 5 threshold); slot should be reassigned | reassign J6 slot to a subsystem with active DEFERRED rows in next batch planning | util |

| D-9280 | 0x00426640,0x0040bde0,0x004891f0,0x00475e50,0x00475d30,0x00477810,0x00477a10,0x004189c0,0x0048fd70,0x0045b990,0x00413a00,0x00413a40,0x004072e0,0x00404650,0x0047b9e0,0x00448730,0x00490490,0x00403910,0x00429e10 (D-5042..D-5060) | cap-split from render_frame_d4-20260508-0332 (20-RVA hard cap reached); remaining 19 of 39 render_frame_d3-cont1 rows | pick up as bucket render_frame_d4-cont1; same depth; no further recursion | render |

## Cleared (delivered or rejected)

| ID | Title | Outcome | Date |
|----|-------|---------|------|
| D-1240 | 0x004335f0 FUN_004335f0 | analyzed C1 session hud_frontend_d2-20260503-0559; depth-3 callees D-2740..D-2782 | 2026-05-03 |
| D-1241 | 0x0043a610 FUN_0043a610 | analyzed C1 session hud_frontend_d2-20260503-0559 | 2026-05-03 |
| D-1242 | 0x0042f0c0 FUN_0042f0c0 | analyzed C1 session hud_frontend_d2-20260503-0559 | 2026-05-03 |
| D-1243 | 0x0043af10 FUN_0043af10 | analyzed C1 session hud_frontend_d2-20260503-0559 | 2026-05-03 |
| D-1244 | 0x00434720 FUN_00434720 | analyzed C1 session hud_frontend_d2-20260503-0559 | 2026-05-03 |
| D-1245 | 0x00430b90 FUN_00430b90 | analyzed C1 session hud_frontend_d2-20260503-0559 | 2026-05-03 |
| D-1246 | 0x00431240 FUN_00431240 | analyzed C1 session hud_frontend_d2-20260503-0559 | 2026-05-03 |
| D-1247 | 0x004314b0 FUN_004314b0 | analyzed C1 session hud_frontend_d2-20260503-0559 | 2026-05-03 |
| D-1248 | 0x00431710 FUN_00431710 | analyzed C1 session hud_frontend_d2-20260503-0559 | 2026-05-03 |
| D-1249 | 0x0043aa30 FUN_0043aa30 | analyzed C1 session hud_frontend_d2-20260503-0559 | 2026-05-03 |
| D-1250 | 0x0042fb70 FUN_0042fb70 | analyzed C1 session hud_frontend_d2-20260503-0559 | 2026-05-03 |
| D-1251 | 0x0042fe90 FUN_0042fe90 | analyzed C1 session hud_frontend_d2-20260503-0559 | 2026-05-03 |
| D-1252 | 0x00430120 FUN_00430120 | analyzed C1 session hud_frontend_d2-20260503-0559 | 2026-05-03 |
| D-1253 | 0x00439210 FUN_00439210 | analyzed C1 session hud_frontend_d2-20260503-0559; depth-3 callees D-2774..D-2782 | 2026-05-03 |
| D-1840 | 0x004a43b9 FUN_004a43b9 + 0x004a43d2 FUN_004a43d2 | analyzed C1 session window_msgpump_d2; depth-3 callees deferred D-2980..D-2984 | 2026-05-03 |
| D-0002 | 0x00402750 FUN_00402750 | analyzed C1 boot_app_init session boot_app_init-20260502-1724 | 2026-05-02 |
| D-0003 | 0x00402a40 FUN_00402a40 | analyzed C1 boot_app_init session boot_app_init-20260502-1724 | 2026-05-02 |
| D-0004 | 0x00492270 FUN_00492270 | analyzed C1 boot_app_init session boot_app_init-20260502-1724 | 2026-05-02 |
| D-0005 | 0x00492290 FUN_00492290 | analyzed C1 boot_app_init session boot_app_init-20260502-1724 | 2026-05-02 |
| D-0006 | 0x004924f0 FUN_004924f0 | analyzed C1 boot_app_init session boot_app_init-20260502-1724 | 2026-05-02 |
| D-0007 | 0x00493540 thunk_FUN_00495150 | analyzed C1 boot_app_init session boot_app_init-20260502-1724 | 2026-05-02 |
| D-0008 | 0x00493550 thunk_FUN_004938c0 | analyzed C1 boot_app_init session boot_app_init-20260502-1724 | 2026-05-02 |
| D-0009 | 0x00493560 thunk_FUN_004954f0 | analyzed C1 boot_app_init session boot_app_init-20260502-1724 | 2026-05-02 |
| D-0010 | 0x00493900 FUN_00493900 | analyzed C1 boot_app_init session boot_app_init-20260502-1724 | 2026-05-02 |
| D-0011 | 0x004963e0 FUN_004963e0 | analyzed C1 boot_app_init session boot_app_init-20260502-1724 | 2026-05-02 |
| D-0012 | 0x004996f0 FUN_004996f0 | analyzed C1 boot_app_init session boot_app_init-20260502-1724 | 2026-05-02 |
| D-0013 | 0x00499ba0 FUN_00499ba0 | analyzed C1 boot_app_init session boot_app_init-20260502-1724 | 2026-05-02 |
| D-0014 | 0x00499cc0 FUN_00499cc0 | analyzed C1 boot_app_init session boot_app_init-20260502-1724 | 2026-05-02 |
| D-0015 | 0x004c5930 FUN_004c5930 | analyzed C1 boot_app_init session boot_app_init-20260502-1724 | 2026-05-02 |
| D-0016 | 0x005c9d00 FUN_005c9d00 | analyzed C1 boot_app_init session boot_app_init-20260502-1724 | 2026-05-02 |
| D-0017 | 0x004a2c2f FUN_004a2c2f | analyzed C1 session boot_crt_exit-20260502-1733 | 2026-05-02 |
| D-0018 | 0x004a40fe ___onexitinit | analyzed C1 session boot_crt_exit-20260502-1733 | 2026-05-02 |
| D-0019 | 0x004a415e _atexit | analyzed C1 session boot_crt_exit-20260502-1733 | 2026-05-02 |
| D-0020 | 0x004a57e4 FUN_004a57e4 | analyzed C1 session boot_crt_exit-20260502-1733 | 2026-05-02 |
| D-0021 | 0x004a3258 FUN_004a3258 | analyzed C1 session boot_crt_exit-20260502-1733 | 2026-05-02 |
| D-0022 | 0x004a333c __exit | analyzed C1 session boot_crt_exit-20260502-1733 | 2026-05-02 |
| D-0023 | 0x004ab8d6 FUN_004ab8d6 | analyzed C1 session boot_crt_exit-20260502-1733 | 2026-05-02 |
| D-0024 | 0x004aba4d __FF_MSGBANNER | analyzed C1 session boot_crt_exit-20260502-1733 | 2026-05-02 |
| D-0025 | 0x004a31b1 ___crtExitProcess | analyzed C1 session boot_crt_exit-20260502-1733 | 2026-05-02 |
| D-0026 | 0x004a467e _calloc | analyzed C1 session boot_crt_exit-20260502-1733 | 2026-05-02 |
| D-0027 | 0x004a774d __mtinitlocks | analyzed C1 session boot_crt_exit-20260502-1733 | 2026-05-02 |
| D-0028 | 0x004a87f7 FUN_004a87f7 | analyzed C1 session boot_crt_exit-20260502-1733 | 2026-05-02 |
| D-0029 | 0x004aa3e4 ___heap_select | analyzed C1 session boot_crt_exit-20260502-1733 | 2026-05-02 |
| D-0030 | 0x004aa44f ___sbh_heap_init | analyzed C1 session boot_crt_exit-20260502-1733 | 2026-05-02 |
| D-0031 | 0x004af2b6 ___initmbctable | analyzed C1 session boot_crt_env-20260502-1734 | 2026-05-02 |
| D-0032 | 0x004affe0 FUN_004affe0 | analyzed C1 session boot_crt_env-20260502-1734 | 2026-05-02 |
| D-0033 | 0x004a45fb _malloc | analyzed C1 session boot_crt_env-20260502-1734 | 2026-05-02 |
| D-0034 | 0x004a460d _free | analyzed C1 session boot_crt_env-20260502-1734 | 2026-05-02 |
| D-0035 | 0x004a9410 _strlen | analyzed C1 session boot_crt_env-20260502-1734 | 2026-05-02 |
| D-0036 | 0x004ac560 FUN_004ac560 | analyzed C1 session boot_crt_env-20260502-1734 | 2026-05-02 |
| D-0037 | 0x004abd1a FUN_004abd1a | analyzed C1 session boot_crt_env-20260502-1734 | 2026-05-02 |
| D-0038 | 0x004aaff0 _memcpy | analyzed C1 session boot_crt_env-20260502-1734 | 2026-05-02 |
| D-0039 | 0x004ae29f ___crtInitCritSecAndSpinCount | analyzed C1 session boot_crt_env-20260502-1734 | 2026-05-02 |
| D-0220 | 0x004c9f50+0x004c9f60 | analyzed C1 session rw_engine_init_d2-20260502-1905 (both RVAs) | 2026-05-02 |
| D-0221 | 0x004cbc60+0x004cbc70+0x004cbc80+0x004cbc90 analyzed; 0x00550350+0x00550390+0x005584c0+0x005c9d00 → D-0520..D-0523 | partial pickup rw_engine_init_d2-20260502-1905 | 2026-05-02 |
| D-0223 | 0x00496400 | analyzed C1 session rw_engine_init_d2-20260502-1905 | 2026-05-02 |
| D-0224 | 0x00498b60 | analyzed C1 session rw_engine_init_d2-20260502-1905 | 2026-05-02 |
| D-0225 | 0x00498bf0 | analyzed C1 session rw_engine_init_d2-20260502-1905 | 2026-05-02 |
| D-0226 | 0x00499400 | analyzed C1 session rw_engine_init_d2-20260502-1905 | 2026-05-02 |
| D-0227 | 0x004cbb60 | analyzed C1 session rw_engine_init_d2-20260502-1905 | 2026-05-02 |
| D-0228 | 0x00499710 | analyzed C1 session rw_engine_init_d2-20260502-1905 | 2026-05-02 |
| D-0229 | 0x004c2c90 | analyzed C1 session rw_engine_init_d2-20260502-1905 | 2026-05-02 |
| D-0232 | 0x004cae90 | analyzed C1 session rw_engine_init_d2-20260502-1905 | 2026-05-02 |
| D-0237 | 0x004c7a60 | analyzed C1 session rw_engine_init_d2-20260502-1905 (thunk) | 2026-05-02 |
| D-0261 | 0x00472380 | analyzed C1 session rw_engine_init_d2-20260502-1905 | 2026-05-02 |
| D-0262 | 0x004a504f | analyzed C1 session rw_engine_init_d2-20260502-1905 | 2026-05-02 |
| D-0263 | 0x004a2d18 | analyzed C1 session rw_engine_init_d2-20260502-1905 | 2026-05-02 |
| D-0100 | 0x004a2bf7 FUN_004a2bf7 | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |
| D-0101 | 0x004a5e35 __ms_p5_mp_test_fdiv | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |
| D-0102 | 0x004a5de3 FUN_004a5de3 | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |
| D-0103 | 0x004a5f07 ___endstdio | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |
| D-0104 | 0x004a77eb FUN_004a77eb | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |
| D-0105 | 0x004a787f __lock | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |
| D-0106 | 0x004af32d FUN_004af32d | superseded by D-0461; not taken due to cap | 2026-05-02 |
| D-0107 | 0x004a4126 __onexit | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |
| D-0108 | 0x004a4728 FUN_004a4728 | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |
| D-0109 | 0x004a5984 __SEH_prolog | analyzed C1 (entry_callees already; D now closed) | 2026-05-02 |
| D-0110 | 0x004a59bf __SEH_epilog | analyzed C1 (entry_callees already; D now closed) | 2026-05-02 |
| D-0111 | 0x004aac76 ___sbh_alloc_block | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |
| D-0112 | 0x004aaf72 __callnewh | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |
| D-0113 | 0x004aaf90 _memset | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |
| D-0114 | 0x004a7796 __mtdeletelocks | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |
| D-0115 | 0x004a2be9 __security_check_cookie | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |
| D-0116 | 0x004a3440 __chkstk | analyzed C1 (entry_callees already; D now closed) | 2026-05-02 |
| D-0117 | 0x004a34b0 _strncpy | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |
| D-0118 | 0x004ac45c ___crtMessageBoxA | analyzed C1 session boot_crt_exit_d3-20260502-1854 | 2026-05-02 |
| D-0119 | 0x004ac570 FUN_004ac570 | superseded by D-0460; not taken due to cap | 2026-05-02 |
| D-0280 | 0x00551510,0x004c2c90,0x004d8060,0x004d7ca0,0x004ccf20 | analyzed C1 session rw_engine_teardown_d2-20260502-1854 (all 5 RVAs) | 2026-05-02 |
| D-6940 | 0x004d8000 FUN_004d8000 | analyzed C1 session intro_splash_d3-20260506 (S-2340 resolved; find-in-list + secondary dispatch) | 2026-05-06 |
| D-6941 | 0x004d8c40 FUN_004d8c40 | analyzed C1 session intro_splash_d3-20260506 (S-2341 resolved; doubly-linked list splice + swap + counter clear; resolves U-2360) | 2026-05-06 |

## Closed Sessions (never opened — no DEFERRED ID)

| Session | Reason | Date |
|---------|--------|------|
| input_lua_d5 | No cont1 work; input_lua_d4 consumed all 9 DEFERRED targets with cap_count=0 and left no continuation rows. Lua VM surface (alloc, state init/teardown, GC sweep, error handlers, protected calls) fully mapped through d4. Lua 5.0 confirmed. | 2026-05-08 |

## Conventions

- ID format: `D-NNNN`, monotonic, never reused.
- Re-pickup condition must be **observable** (a phase exits, a feature ships, a tool gains a capability) — not "later" or "when I feel like it."
- A DEFERRED row may reference S-NNNN or U-NNNN ids; in that case the original tracker entry stays, with a pointer to D-NNNN.
| D-0340 | 0x004d7ff0 FUN_004d7ff0 | depth-2 of FUN_004cbd30 (audio_rws_loader session); S-0101; error-code constructor | audio_rws_loader-cont1 | audio |
| D-0341 | 0x004d8480 FUN_004d8480 | depth-2 of FUN_004cbd30 (audio_rws_loader session); S-0102; error dispatcher/logger | audio_rws_loader-cont1 | audio |
| D-0342 | 0x00550950 FUN_00550950 | depth-2 of FUN_004cbd30 (audio_rws_loader session); S-0103; file-read wrapper | audio_rws_loader-cont1 | audio |
| D-0343 | 0x00550af0 FUN_00550af0 | depth-2 of FUN_004cbd30+FUN_004cc050 (audio_rws_loader session); S-0104; EOF checker | audio_rws_loader-cont1 | audio |
| D-0344 | 0x005509b0 FUN_005509b0 | depth-2 of FUN_004cc050 (audio_rws_loader session); S-0105; file-seek wrapper | audio_rws_loader-cont1 | audio |
| D-0345 | 0x005a7a40 FUN_005a7a40 | depth-2 of FUN_005a79a0 (audio_rws_loader session); S-0106; pool-list searcher | audio_rws_loader-cont1 | audio |
| D-0346 | 0x005ade90 FUN_005ade90 | depth-2 of FUN_005a79a0 (audio_rws_loader session); S-0107; resets embedded list head | audio_rws_loader-cont1 | audio |
| D-0347 | 0x005a7ea0 FUN_005a7ea0 | depth-2 of FUN_005a79a0 (audio_rws_loader session); S-0108; final dealloc of audio object | audio_rws_loader-cont1 | audio |
| D-0348 | 0x005ae0c0 FUN_005ae0c0 | depth-2 of FUN_005a7ee0 (audio_rws_loader session); S-0109; init sub-struct at audio_obj+0x24 | audio_rws_loader-cont1 | audio |
| D-0349 | 0x005ae010 FUN_005ae010 | depth-2 of FUN_005a7ee0 (audio_rws_loader session); S-0110; links audio_obj with sub-struct +0x24 | audio_rws_loader-cont1 | audio |
| D-0350 | 0x005adfe0 FUN_005adfe0 | depth-2 of FUN_005a7ee0 (audio_rws_loader session); S-0111; init sub-struct at audio_obj+0x34 | audio_rws_loader-cont1 | audio |
| D-0351 | 0x005ac740 FUN_005ac740 | depth-2 of FUN_005abcf0 (audio_rws_loader session); S-0112; cleans wave_node sub-struct +0x10/+0x2c | audio_rws_loader-cont1 | audio |
| D-0352 | 0x005a7e70 FUN_005a7e70 | depth-2 of FUN_005abcf0 (audio_rws_loader session); S-0113; unknown wave_node op | audio_rws_loader-cont1 | audio |
| D-0353 | 0x005ae030 FUN_005ae030 | depth-2 of FUN_005abcf0 (audio_rws_loader session); S-0114; unknown wave_node op | audio_rws_loader-cont1 | audio |
| D-0354 | 0x005abcb0 FUN_005abcb0 | depth-2 of FUN_005abcf0 (audio_rws_loader session); S-0115; final dealloc of wave_node | audio_rws_loader-cont1 | audio |
| D-0355 | 0x005ac210 FUN_005ac210 | depth-2 of FUN_005abfa0 (audio_rws_loader session); S-0116; wave object creator from 0x803 data | audio_rws_loader-cont1 | audio |
| D-0356 | 0x005adf30 FUN_005adf30 | depth-2 of FUN_005abfa0 (audio_rws_loader session); S-0117; format descriptor comparator | audio_rws_loader-cont1 | audio |
| D-0357 | 0x005aec30 FUN_005aec30 | depth-2 of FUN_005abfa0 (audio_rws_loader session); S-0118; audio sample buffer byte-swapper | audio_rws_loader-cont1 | audio |
| D-0358 | 0x005abd30 FUN_005abd30 | depth-2 of FUN_005abfa0 (audio_rws_loader session); S-0119; PCM chunk feeder | audio_rws_loader-cont1 | audio |
| D-0359 | 0x005abf80 FUN_005abf80 | depth-2 of FUN_005abfa0 (audio_rws_loader session); S-0120; audio drain/flush loop | audio_rws_loader-cont1 | audio |
| D-0360 | 0x005ae920 FUN_005ae920 | depth-2 of FUN_005ade10 (audio_rws_loader session); S-0121; returns node to free pool DAT_009146c0 | audio_rws_loader-cont1 | audio |
| D-0820 | 0x004b7330 FUN_004b7330 + 0x004c0510 FUN_004c0510 + 0x004b7480 FUN_004b7480 + 0x0047b8a0 FUN_0047b8a0 + 0x004b6520 FUN_004b6520 | Lua interpreter internals — possibly vendored Lua 5.x source; pick up only if Mashed needs scriptable joypad remap rebuilt rather than wrapped | DRAINED by input_lua_d2-20260506-1854 (all 5 RVAs analyzed at C1; depth-3 callees filed D-7120..D-7130); S-0300..S-0304; U-0307 U-0308 | input |
| D-0640 | 0x004c35f0 FUN_004c35f0 | depth-2 callee of _rwDeviceSystemFn (render_d3d9_device session); identified from switch-body listing scan; not decomped (outside subset cap) | render_d3d9_device-cont1 | render |
| D-0641 | 0x004c40b0 FUN_004c40b0 | depth-2 callee of _rwDeviceSystemFn (render_d3d9_device session); identified from switch-body listing scan; not decomped (outside subset cap) | render_d3d9_device-cont1 | render |
| D-0642 | 0x004cfe20 FUN_004cfe20 | depth-2 callee of _rwDeviceSystemFn (render_d3d9_device session); identified from switch-body listing scan; not decomped (outside subset cap) | render_d3d9_device-cont1 | render |
| D-0643 | 0x004d5bc0 FUN_004d5bc0 | depth-2 callee of _rwDeviceSystemFn (render_d3d9_device session); identified from switch-body listing scan; not decomped (outside subset cap) | render_d3d9_device-cont1 | render |
| D-0644 | 0x004db3e0 FUN_004db3e0 | depth-2 callee of _rwDeviceSystemFn (render_d3d9_device session); identified from switch-body listing scan; not decomped (outside subset cap) | render_d3d9_device-cont1 | render |
| D-0645 | 0x004dcd50 FUN_004dcd50 | depth-2 callee of _rwDeviceSystemFn (render_d3d9_device session); identified from switch-body listing scan; not decomped (outside subset cap) | render_d3d9_device-cont1 | render |
| D-0646 | 0x005cb404 FUN_005cb404 | depth-2 callee of _rwDeviceSystemFn (render_d3d9_device session); identified from switch-body listing scan; not decomped (outside subset cap) | render_d3d9_device-cont1 | render |
| D-0400 | Remaining 116 depth-3 RVAs from D-0040..D-0045 not reached in boot_app_init_d3 session (cap=18): 00412890,0041a1e0,0041b450,0041bec0,0041c100,0041cb10,0041d6e0,0041d8b0,0041db90,0041def0,0041eaa0,00420d00,00425bc0,004274d0,004274e0,004275d0,00427ca0,00428390,004283a0,00431b40,0045b350,0045bae0,004669b0,004671a0,00471eb0,004723d0,0047ba00,00484170,004841d0,004881d0,00494c80,00495280,004952f0,00495350,00496e40,00498bc0,00498bd0,00499ce0,004b3d80,004b6540,004b6560,004c2ed0,004c2f00,004caea0,004d8560,00558240,0041a3d0,0041b660,0041c0e0,0041c2c0,0041ccf0,0041d890,0041da80,0041de70,0041e0d0,0041ffb0,00421590,00425ed0,00426ba0,00426c00,00427620,00428400,0042c2a0,0045b930,00467010,00467020,00467070,0047ba10,00484130,00489250,00494bc0,00494ef0,00494f20,00496ce0,004b4880,004b6550,005581f0,00428590,004921d0,00493710,004926c0,00492770,004929d0,00492d20,00492d30,00492e90,00493480,00499690,00431ae0,00431af0,00431b00,00431b10,00431d00,00495120,004955b0,004960a0,004963b0,00498510,00499710,004c2f60,004c3040,004c3270,00550390,00558470,00498b60,004963d0,00496010,004955c0,00498bf0,00499730,004a2be9,004a3ac9,004b302f,00499890,004c5a60,004d8060 | cap reached in boot_app_init_d3-20260502-1859; note: 0x00412890 missed due to sort error — include as first target | boot subsystem depth-3 sweep session (boot_app_init_d3-cont1) | boot |
| D-0401 | Depth-4 callees encountered as stubs during boot_app_init_d3-20260502-1859 session (S-0160..S-0179, S-0305..S-0372 — 88 stubs total); not recursed into | depth-4; out of scope for depth-3 session | boot subsystem depth-4 sweep session (boot_app_init_d4) | boot |
| ~~D-1000~~ | ~~0x0049ec10 FUN_0049ec10~~ | ~~analyzed C1 session video_mci_d2-20260503~~ | ~~video_mci-cont1~~ | ~~util~~ |
| ~~D-1001~~ | ~~0x004a3b84 FUN_004a3b84~~ | ~~analyzed C1 session video_mci_d2-20260503~~ | ~~video_mci-cont1~~ | ~~util~~ |
| ~~D-1002~~ | ~~LAB_00493ac0 + LAB_00493b40~~ | ~~analyzed C1 session video_mci_d2-20260503; S-0373/S-0374 resolved~~ | ~~video_mci-cont1~~ | ~~util~~ |
| ~~D-4060~~ | ~~0x0049dd60 FUN_0049dd60~~ | **RESOLVED 2026-05-06** video_mci_d3-20260506-0512; analyzed in re/analysis/video_mci_d3/0x0049dd60.md | video_mci_d3 | util |
| ~~D-1180~~ | ~~drained by track_loader_d2-20260503-0302~~ | ~~all 20 RVAs processed (4 already mapped, 16 newly mapped)~~ | ~~track_loader-cont1~~ | ~~render~~ |
| ~~D-1300~~ | ~~14 depth-2 callees of FUN_004548e0 (DepthCharge init)~~ | ~~drained by powerups_d2-20260503; all 14 RVAs mapped; new stubs S-1440..S-1453; D-4240..D-4243~~ | ~~powerups_d2~~ | ~~vehicle~~ |
| ~~D-1301~~ | ~~10 depth-2 callees of FUN_00456760 (GatlingGun init)~~ | ~~drained by powerups_d2-20260503; all unique RVAs mapped (7 shared with D-1300 already handled)~~ | ~~powerups_d2~~ | ~~vehicle~~ |
| ~~D-1302~~ | ~~9 depth-2 callees of FUN_004587a0 (PowerUpIcons init)~~ | ~~drained by powerups_d2-20260503; all unique RVAs mapped~~ | ~~powerups_d2~~ | ~~vehicle~~ |
| ~~D-1303~~ | ~~9 depth-2 callees of FUN_00459290 (Laser/Lazer init)~~ | ~~drained by powerups_d2-20260503; all unique RVAs mapped~~ | ~~powerups_d2~~ | ~~vehicle~~ |

| ~~D-0940~~ | ~~0x005ba1d0 FUN_005ba1d0 body tail~~ | ~~Body extends beyond 471 code units; listing capped at 250; address range ~0x005ba4a0+ not analyzed~~ | ~~Resolved: audio_dsound_d4 session 2026-05-08; tail 0x005ba4a0–0x005ba716 fully analyzed; re/analysis/audio_dsound_d4/0x005ba1d0_tail.md~~ | ~~audio~~ |
| D-0952 | 0x005aa560 FUN_005aa560 | depth-2 callee of FUN_005ba1d0 tail via FUN_005c7990; 567 bytes (005aa560–005aa797); S-3190; pickup condition: audio_dsound_d4-cont1 | audio_dsound_d4-cont1 session | audio |
| D-0941 | 0x005adfe0 FUN_005adfe0 | depth-2 callee of FUN_005a9e10; called as FUN_005adfe0(param_1, param_3); S-0344 | audio_dsound-cont1 session | audio |
| D-0942 | 0x005ae010 FUN_005ae010 | depth-2 callee of FUN_005a9e10; called as FUN_005ae010(param_1, param_2); S-0345 | audio_dsound-cont1 session | audio |
| D-0943 | 0x005ba780 LAB_005ba780 | fn-ptr body at struct+0x34; depth-1 callee of FUN_005b9f30; unrecognized; S-0342 | audio_dsound-cont1 session | audio |
| D-0944 | 0x005ba7f0 LAB_005ba7f0 | fn-ptr body at struct+0x30; depth-1 callee of FUN_005b9f30; unrecognized; S-0343 | audio_dsound-cont1 session | audio |
| D-0945 | 0x005ba720 LAB_005ba720 | early-exit guard in LAB_005ba1d0; depth-2 of entry; S-0346 | audio_dsound-cont1 session | audio |
| D-0946 | 0x005bb000 FUN_005bb000 | pre-DirectSoundCreate8 callee in LAB_005ba1d0; depth-2 of entry; S-0347 | audio_dsound-cont1 session | audio |
| D-0947 | 0x005ba760 LAB_005ba760 | error-cleanup callee in LAB_005ba1d0; depth-2 of entry; S-0348 | audio_dsound-cont1 session | audio |
| D-0948 | 0x005bbc10 FUN_005bbc10 | format/caps query in LAB_005ba1d0 after vtable+0x18; depth-2 of entry; S-0349 | audio_dsound-cont1 session | audio |
| D-0949 | 0x005bbdb0 FUN_005bbdb0 | buffer creation; shared depth-2 callee of LAB_005ba1d0 and LAB_005bad30; S-0350 | audio_dsound-cont1 session | audio |
| D-0950 | 0x005bac00 FUN_005bac00 | conditional callee; shared depth-2 callee of LAB_005ba1d0 and LAB_005bad30; S-0351 | audio_dsound-cont1 session | audio |
| D-0951 | 0x005bbf30 FUN_005bbf30 | unconditional single-arg callee in LAB_005bad30; depth-2 of entry; S-0352 | audio_dsound-cont1 session | audio |
| D-1780 | 0x005a66d0 FUN_005a66d0 | (audio_obj_ptr, 0_or_1) stop/start; depth-2 callee of audio_music bucket; S-0620 | audio_music-cont1 session | audio |
| D-1781 | 0x005a6dc0 FUN_005a6dc0 | (sub_ptr, param_type, flag, value) param setter; depth-2 callee of audio_music bucket; S-0621 | audio_music-cont1 session | audio |
| D-1782 | 0x0045e0f0 FUN_0045e0f0 | (channel_index, float) volume setter; depth-2 callee of audio_music bucket; S-0622 | audio_music-cont1 session | audio |
| D-1783 | 0x00431b20 FUN_00431b20 | zero-arg float10 getter; depth-2 of audio_music; S-0623 | audio_music-cont1 session | audio |
| D-1784 | 0x00432290 FUN_00432290 | zero-arg int condition; music-mute gate; depth-2 of audio_music; S-0624 | audio_music-cont1 session | audio |
| D-1785 | 0x005baf00 FUN_005baf00 | (ptr, float) on DAT_0069049c; depth-2 of audio_music; S-0625 | audio_music-cont1 session | audio |
| D-1786 | 0x00431b60 FUN_00431b60 | zero-arg float10 getter; depth-2 of audio_music; S-0626 | audio_music-cont1 session | audio |
| D-1787 | 0x0042f760 FUN_0042f760 | zero-arg bool; trigger check channel DAT_00604eb0; depth-2 of audio_music; S-0627 | audio_music-cont1 session | audio |
| D-1788 | 0x0042f770 FUN_0042f770 | zero-arg bool; trigger check A channel DAT_00604e1c; depth-2 of audio_music; S-0628 | audio_music-cont1 session | audio |
| D-1789 | 0x0042f780 FUN_0042f780 | zero-arg bool; trigger check B channel DAT_00604e1c; depth-2 of audio_music; S-0629 | audio_music-cont1 session | audio |
| D-1790 | 0x00432230 FUN_00432230 | zero-arg bool; trigger check channel DAT_00605918; depth-2 of audio_music; S-0630 | audio_music-cont1 session | audio |
| D-1791 | 0x00432260 FUN_00432260 | zero-arg bool; trigger check musicloop1 (DAT_00605d24); depth-2 of audio_music; S-0631 | audio_music-cont1 session | audio |
| D-2800 | 0x00538d60 FUN_00538d60 | depth-3 callee of FUN_00538c80 case 1; geometry query handler; 0xB90 bytes; from session effects_particle_d2 bucket effects_particle_d2 | pick up as bucket effects_particle_d2-cont1; no further recursion required | render |
| D-2801 | 0x00539900 FUN_00539900 | depth-3 callee of FUN_00538c80 case 4; geometry query handler; 0x5BC bytes; from session effects_particle_d2 bucket effects_particle_d2 | pick up as bucket effects_particle_d2-cont1; no further recursion required | render |
| D-2802 | 0x00539ec0 FUN_00539ec0 | depth-3 callee of FUN_00538c80 cases 3 and 5; geometry query handler; 0x6F0 bytes; from session effects_particle_d2 bucket effects_particle_d2 | pick up as bucket effects_particle_d2-cont1; no further recursion required | render |
| D-2803 | 0x004e6100 FUN_004e6100 | depth-3 callee of FUN_00538c80 case 5; dereferences *param_2 and returns pointer to 4-DWORD block; 0x146 bytes; from session effects_particle_d2 bucket effects_particle_d2 | pick up as bucket effects_particle_d2-cont1; no further recursion required | render |
| D-1360 | 0x0042c280,0x0042c2d0,0x0042c2e0,0x0042c2f0,0x0042f500,0x0042f6a0,0x00432080,0x004331a0,0x00448700,0x004927c0,0x005c9d00 from session game_state-20260502-2144 bucket game_state — pick up as bucket game_state-cont1; same depth, no further recursion. S-0480..S-0490 |

## camera_follow-20260502-2132 depth-2 callees (D-1540..D-1599)

| ID | RVA | Ghidra name | Caller | Reason deferred | Re-pickup condition |
|---|---|---|---|---|---|
| D-1540 | 0x004756e0 | FUN_004756e0 | FUN_00426700 | depth-2 callee; camera path node per-tick updater; purpose unknown | reverse FUN_00426700 fully first |
| D-1541 | 0x00475010 | FUN_00475010 | FUN_00426780 | depth-2 callee; camera path entry time-delta processor; purpose unknown | reverse FUN_00426780 fully first |
| D-1542 | 0x004c4d20 | FUN_004c4d20 | FUN_00426810 | depth-2 callee; called in lerp path; candidate RW math or position helper | reverse FUN_00426810 fully first |
| D-1543 | 0x004c3dc0 | FUN_004c3dc0 | FUN_00426810 | depth-2 callee; called in lerp path | reverse FUN_00426810 fully first |
| D-1544 | 0x004c39b0 | FUN_004c39b0 | FUN_00426810 | depth-2 callee; called in lerp path | reverse FUN_00426810 fully first |
| D-1545 | 0x004a2c48 | FUN_004a2c48 | FUN_00426810 | depth-2 callee; called in lerp path | reverse FUN_00426810 fully first |
| D-1546 | 0x004924c0 | FUN_004924c0 | FUN_00426810 | depth-2 callee; called in lerp path | reverse FUN_00426810 fully first |
| D-1547 | 0x004c1b10 | FUN_004c1b10 | FUN_00426810 | depth-2 callee; receives interpolated Y/Z; candidate position setter | reverse FUN_00426810 fully first |
| D-1548 | ~~0x0040e180 FUN_0040e180~~ | ~~FUN_00471ec0~~ | CLEARED vehicle_damage_d2-20260505; C1/mapped CollisionPair_FindMaxDist; no dependency on FUN_00471ec0 confirmed | — | — |
| D-1549 | 0x00407a40 | FUN_00407a40 | FUN_00471ec0 | depth-2 callee; returns frame counter / tick A from timing struct | reverse FUN_00471ec0 fully first |
| D-1550 | 0x00407a20 | FUN_00407a20 | FUN_00471ec0 | depth-2 callee; returns tick B from timing struct | reverse FUN_00471ec0 fully first |
| D-1551 | 0x0047ce80 | FUN_0047ce80 | FUN_00471ec0 | depth-2 callee; returns ID from target object (inner loop B) | reverse FUN_00471ec0 fully first |
| D-1552 | 0x0047ce00 | FUN_0047ce00 | FUN_00471ec0 | depth-2 callee; returns flags from target object (inner loop B) | reverse FUN_00471ec0 fully first |
| D-1553 | 0x0047d130 | FUN_0047d130 | FUN_00471ec0 | depth-2 callee; returns inner ref from target object (dispatch case 2) | reverse FUN_00471ec0 fully first |
| D-1554 | 0x0057c210 | FUN_0057c210 | FUN_00471ec0 | depth-2 callee; returns animation object from ref (dispatch case 2) | reverse FUN_00471ec0 fully first |
| D-1555 | 0x004c0ed0 | FUN_004c0ed0 | FUN_0047c160 | depth-2 callee; takes vehicle sub-ptr *(param_1+4); result+0x30 passed to FUN_0047bb10; candidate RW frame accessor | reverse FUN_0047c160 fully first |
| D-1556 | 0x004c1b40 | FUN_004c1b40 | FUN_0047c160 | depth-2 callee; spatial containment test: vehicle vs camera path node range | reverse FUN_0047c160 fully first |
| D-1557 | 0x00491340 | FUN_00491340 | FUN_00491490 | depth-2 callee; camera mode A (DAT_007f108b==0 path); purpose unknown | reverse FUN_00491490 fully first |
| D-1558 | 0x004910c0 | FUN_004910c0 | FUN_00491490 | depth-2 callee; camera mode B (DAT_007f108b!=0 path); purpose unknown | reverse FUN_00491490 fully first |
| D-1559 | 0x0042b930 | FUN_0042b930 | FUN_004671a0,FUN_00467210 | depth-2 callee; returns state discriminant (3 = alt mode); S-0549 filed | reverse FUN_004671a0 or FUN_00467210 fully first |
| D-1560 | 0x0042f510 | FUN_0042f510 | FUN_004671a0,FUN_00467210 | depth-2 callee; alt vehicle getter (state==3 && param_1!=-1); S-0550 filed | reverse FUN_004671a0 or FUN_00467210 fully first |
| D-1792 | COLLISION_FN | unknown RVA | RWP37Active vtable | Runtime BSP collision test for vehicle-vs-track; statically linked in RWP37Active without Ghidra symbols; not reachable via static call graph from game loop | Frida hardware watchpoint on read to DAT_006bf1cc (0x006bf1cc) during live race; capture full call stack |
| D-1793 | 0x0047b9b0 | FUN_0047b9b0 | FUN_0047a020 | Lua script executor that runs COURSE.LUA with BSP C functions; protocol unknown | decomp FUN_0047b9b0; U-0644 |
| D-1794 | 0x00478cb0 | FUN_00478cb0 | FUN_0047a020 | BSP struct A initializer; struct layout unknown | decomp FUN_00478cb0; U-0643 |
| D-1795 | 0x004715a0 | FUN_004715a0 | FUN_00426e10 | Physics scenario linker: populates scenario struct at 0x0086ece0 from BSP primitive table 0x0086cac0; calls FUN_0047ce40 per prim | reverse FUN_0047ce40; needs vehicle bucket |
| D-1960 | 0x004a92de 0x004af400 | terminate _ValidateExecute | depth-2 callees of FUN_004af2d4 (terminate + _ValidateExecute subtrees); both are CRT library functions compiled into the binary — out of scope for game reimplementation | pick up only if CRT reimplementation is required; bucket exception_filter-cont1 |
| D-2140 | RESOLVED render_pipeline_d2-20260503 | reference_to(0x005cf820) → FUN_00493710 already mapped (C1, rw_engine_init); plugin chain FUN_00493640 18-call already mapped (C1); custom node 0x26990004 found in FUN_00472380 (C1, rw_engine_init_d2); no additional PIPELINE_SETUP_FN exists | closed; no further action needed |
| D-2080 | render_frame_tree FRAME_UPDATE_FN | Session render_frame_tree-20260502-2221 halted; RwFrameUpdateObjects/RwFrameTransform/RwFrameForAllChildren/RwFrameSetIdentity/_rwFrameSyncDirty absent as imports and symbols (RW statically linked; FidDB did not match); FRAME_UPDATE_FN not identified | Re-run after FidDB re-analysis on Mashed.gpr with a RW 3.x FidDB, or locate FRAME_UPDATE_FN via Frida structural scan (frame-tree walker pattern); re-open as bucket render_frame_tree-cont1 | render |
| D-2440 | custom allocator implementations at obj+0x1ec | FUN_005208c0 dispatches to a function pointer at allocator-object+0x1ec; concrete functions placed there are the pool/arena allocators; not reachable via static call graph from FUN_005208c0 alone | Frida: hook FUN_005208c0, log param_1+0x1ec at each call; capture all concrete function pointers; then decomp each via MCP | bucket memory_pool-cont1 |
| D-2320 | 0x004967e0 | FUN_004967e0 | FUN_00492d20 | depth-2 from INTRO_FN; 283 bytes; sole callee of frame-tick shim | decomp FUN_004967e0; pick up as intro_splash-cont1 |
| D-2500 | 0x005507b0 0x00550bc0 | FUN_005507b0 FUN_00550bc0 | depth-2 callees of FUN_004cc230 (stream-open); PIZ-open and file-open; from session localization-20260502-2227 bucket localization | pick up as bucket localization-cont1; no further recursion |
| D-2501 | 0x00550910 | FUN_00550910 | depth-2 callee of FUN_004cc160 (stream-close); file-close; from session localization-20260502-2227 bucket localization | pick up as bucket localization-cont1; no further recursion |
| D-2502 | 0x004625b0 0x004669b0 0x004a2b60 | FUN_004625b0 FUN_004669b0 FUN_004a2b60 | sibling audio-language functions called by FUN_00402750; FUN_004669b0 calls FUN_004625b0(DAT_007f0f60) to build per-language audio path strings; out of scope for localization subset; from session localization-20260502-2227 | pick up as bucket localization-cont1; frontend subsystem |
| D-2321 | 0x004c75e0 | FUN_004c75e0 | FUN_00493fd0 | depth-2 from INTRO_FN; 26 bytes; fills viewport short[2] arrays | decomp FUN_004c75e0; pick up as intro_splash-cont1 |
 save |
| D-2322 | 0x00494320 | FUN_00494320 | FUN_00494460 | depth-2; 167 bytes; first cleanup step in video close | decomp FUN_00494320; pick up as intro_splash-cont1 |
| D-2323 | 0x004c7650 | FUN_004c7650 | FUN_00494460 | depth-2; releases video texture handle DAT_00771a18 | decomp FUN_004c7650; pick up as intro_splash-cont1 |
| D-2326 | 0x004c77c0 | FUN_004c77c0 | FUN_00494a80 | depth-2; 153 bytes; video texture allocator args(0,0,0,0x84) | decomp FUN_004c77c0; pick up as intro_splash-cont1 |
| D-1900 | 0x00428a30 FUN_00428a30 | caller of FUN_004950b0 (QPC*3M/freq); 433 bytes; not analyzed in timer session timer-20260502-2221 | pick up as bucket timer-cont1; no further recursion |
| D-1901 | 0x00493390 FUN_00493390 | caller of FUN_004950b0 (QPC*3M/freq); 238 bytes; not analyzed in timer session timer-20260502-2221 | pick up as bucket timer-cont1; no further recursion |
| D-1902 | 0x004a1570 FUN_004a1570 | GetTickCount caller; 519 bytes; 2 GetTickCount refs at 0x004a159b and 0x004a1683; likely frame-pacing not frame-delta timer | pick up as bucket timer-cont1; no further recursion |
| D-1903 | 0x005aafb5 area (no Ghidra function boundary) | QPC call at 0x005aafb5 and QPF call at 0x005aafca in unanalyzed code block; possible alternate timer-init not auto-analyzed by Ghidra | disassemble area; create function boundary; pick up as bucket timer-cont1 |
| D-1904 | _DAT_0077197c 0x0077197c and _DAT_007719a8 0x007719a8 | read sites missing from Ghidra cross-refs due to symbol-name overlap (_-prefix warning); timer output consumers unknown | search_bytes for these addresses in code; pick up as bucket timer-cont1 |
| D-2020 | 0x004cc7f0,0x004ccc50,0x004ccde0,0x004d1d70,0x004db550,0x004dc8e0,0x004e08b0 | analyzed C1 session render_d3d_reset_d2-20260503-0351 | 2026-05-03 |
| D-2021 | 0x004dc9e0 | analyzed C1 session render_d3d_reset_d2-20260503-0351 | 2026-05-03 |
| D-2022 | 0x004cb8a0,0x004cba80 | analyzed C1 session render_d3d_reset_d2-20260503-0351 | 2026-05-03 |
| D-2023 | 0x004d5480,0x004d54f0,0x004d5570,0x004d53b0 | analyzed C1 session render_d3d_reset_d2-20260503-0351 | 2026-05-03 |
| D-2327 | 0x004c7730 | FUN_004c7730 | FUN_004c1be0 | depth-2; 46 bytes; receives inner render object and HWND; likely MCI draw | decomp FUN_004c7730; pick up as intro_splash-cont1 |
| D-2980 | 0x004a9858 FUN_004a9858 | depth-3 callee of 0x004a43b9 FUN_004a43b9; time-struct ptr → struct tm* converter; body 0x004a9858..0x004a9a57 (512 bytes); S-1020 | pick up as bucket window_msgpump_d2-cont1; no further recursion | boot |
| D-2981 | 0x004a974d FUN_004a974d | depth-3 callee of 0x004a43b9 FUN_004a43b9; struct-tm-like int → char* date string formatter (asctime-style, 26 chars); body 0x004a974d..0x004a9857 (267 bytes); S-1021 | pick up as bucket window_msgpump_d2-cont1; no further recursion | boot |
| D-2982 | 0x004a4e40 __aulldiv | depth-3 callee of 0x004a43d2 FUN_004a43d2; CRT 64-bit unsigned division runtime helper; body 0x004a4e40..0x004a4ea7; S-1022 | pick up as bucket window_msgpump_d2-cont1; CRT intrinsic | boot |
| D-2983 | 0x004aa00c FUN_004aa00c | depth-3 callee of 0x004a43d2 FUN_004a43d2; zero-arg call at start of time-struct init; body 0x004aa00c..0x004aa039; S-1023 | pick up as bucket window_msgpump_d2-cont1; no further recursion | boot |
| D-2984 | 0x004aa060 __aullrem | depth-3 callee of 0x004a43d2 FUN_004a43d2; CRT 64-bit unsigned remainder runtime helper; body 0x004aa060..0x004aa0d4; S-1024 | pick up as bucket window_msgpump_d2-cont1; CRT intrinsic | boot |
| D-2200 | n/a (not found) | LIGHT_SETUP_FN not identified in render_lighting-20260502-2221 session; RpLight anchors absent from MASHED.exe (no imports, no strings, no FidDB matches); nodeD3D9SubmitNoLight.csl at 0x00618598 suggests custom D3D9 pipeline bypassing RW world lights; from session render_lighting-20260502-2221 | pick up as bucket render_lighting-cont1; try constant search for rpLIGHTAMBIENT=1/rpLIGHTDIRECTIONAL=2 arguments or trace Lights_Filename consumer from 0x0047aaa0 | n/a |
| D-2201 | 0x0047aaa0 (Lights_Filename global ptr) | Lights_Filename config key registered by switch-case config fn at ~0x0043dfd0; consumer function that reads 0x0047aaa0 and loads the light file not traced; file format unknown; from session render_lighting-20260502-2221 | pick up as bucket render_lighting-cont1; reference_to 0x0047aaa0 to find reader; decomp reader function | n/a |
| D-2324 | 0x004942b0 | FUN_004942b0 | FUN_00494480 | depth-2; 109 bytes; advance/tick call | decomp FUN_004942b0; pick up as intro_splash-cont1 |
| D-2325 | 0x004938e0 | FUN_004938e0 | FUN_00494480 | depth-2; 27 bytes; receives video texture handle | decomp FUN_004938e0; pick up as intro_splash-cont1 |
| D-3040 | 0x0042bf30 | FUN_0042bf30 | FUN_0042c280 | depth-2; 112 bytes; called with (0x27f, 0xff210000, 0, 0, 0, 0); from session race_state-20260503-0600 bucket race_state | pick up as bucket race_state-cont1; same depth; no further recursion |
| D-3041 | 0x0042d3a0 | FUN_0042d3a0 | FUN_00432080 FUN_004331a0 | depth-2; 52 bytes; called on phase-transition success; from session race_state-20260503-0600 bucket race_state | pick up as bucket race_state-cont1; same depth; no further recursion |
| D-3042 | 0x004248b0 | FUN_004248b0 | FUN_004331a0 | depth-2; 367 bytes; called at end of race-end init; from session race_state-20260503-0600 bucket race_state | pick up as bucket race_state-cont1; same depth; no further recursion |
| D-3043 | 0x00424920 | FUN_00424920 | FUN_004331a0 | depth-2; 607 bytes; called after FUN_004248b0 in race-end init; from session race_state-20260503-0600 bucket race_state | pick up as bucket race_state-cont1; same depth; no further recursion |
| D-3044 | 0x004464c0 | FUN_004464c0 | FUN_00448700 | depth-2; 91 bytes; called 100 times with &DAT_00897fe0; from session race_state-20260503-0600 bucket race_state | pick up as bucket race_state-cont1; same depth; no further recursion |
| D-2920 | 0x0042bf30 FUN_0042bf30 | depth-3 callee of FUN_0042c280; pending-action packet setter; 6 params; guards on DAT_0067eab0==0; body 0x0042bf30..0x0042bfa0 (112 bytes); S-1000; from game_state_d2 session | pick up as bucket game_state_d2-cont1; no further recursion | util |
| D-2921 | 0x0042d3a0 FUN_0042d3a0 | depth-3 callee of FUN_00432080 and FUN_004331a0; zeroes 13-entry 64-byte struct array at 0x0067ed78..0x0067f0bc; S-1001; from game_state_d2 session | pick up as bucket game_state_d2-cont1; no further recursion | util |
| D-2922 | 0x004248b0 FUN_004248b0 | depth-3 callee of FUN_004331a0; 111 bytes; called after zero-clear block in race-end init; S-1002; from game_state_d2 session | pick up as bucket game_state_d2-cont1; no further recursion | util |
| D-2923 | 0x00424920 FUN_00424920 | depth-3 callee of FUN_004331a0; 607 bytes; called after FUN_004248b0 in race-end init; S-1003; from game_state_d2 session | pick up as bucket game_state_d2-cont1; no further recursion | util |
| D-2924 | 0x004464c0 FUN_004464c0 | depth-3 callee of FUN_00448700; array iterator via DAT_00898994; stride 0xd8; type dispatch on +4 (0/1/2); called 100 times; S-1004; from game_state_d2 session | pick up as bucket game_state_d2-cont1; no further recursion | util |
| D-2680 | ~~0x00450b10 FUN_00450b10~~ | ~~FUN_00428610~~ | CLEARED hud_ingame_d3; C1/new Im2DTexturedQuad leaf; callees=0 | — | — |
| D-3100 | 0x00552840 FUN_00552840 | depth-2 callee of FUN_00427ff0 (font_text session); called with param_7; role of param_7 unknown; may be shadow-text offset or flash-state | font_text-cont1 | hud |
| D-3101 | 0x00552a60 FUN_00552a60 | depth-2 callee of FUN_00552b60 (font subsystem init seq); last call in init chain; role unknown | font_text-cont1 | hud |
| D-3102 | 0x00552c10 FUN_00552c10 | depth-2 callee of FUN_00552b60; position 6 of 7 in init chain; role unknown | font_text-cont1 | hud |
| D-3103 | 0x005540d0 FUN_005540d0 | depth-2 callee of FUN_00552b60; position 2 of 7 in init chain; role unknown | font_text-cont1 | hud |
| D-3104 | 0x005571e0 FUN_005571e0 | depth-2 callee of FUN_00552b60; position 5 of 7 in init chain; role unknown | font_text-cont1 | hud |
| D-3105 | 0x00557250 FUN_00557250 | depth-2 callee of FUN_00552b60; first call in init chain; role unknown | font_text-cont1 | hud |
| D-3106 | 0x00556d20 FUN_00556d20 | depth-2 callee of FUN_00552b60; position 3 of 7; role unknown | font_text-cont1 | hud |
| D-3107 | 0x0042a6b0 FUN_0042a6b0 | depth-2 callee of FUN_00427ca0; called with &local_1c (font descriptor struct from DAT_005cd600) + 0 + 0; returns font handle stored in uVar1; role: font create from descriptor | font_text-cont1 | hud |
| D-3108 | 0x005507b0 FUN_005507b0 | depth-2 callee of LAB_00555910 (.met parser); called during character metrics parse loop with (ESI, 0x617378); role in metrics parsing unknown | font_text-cont1 | hud |
| D-3109 | 0x004c5890 FUN_004c5890 | depth-2 callee of FUN_00555360; result stored at DAT_00912a1c and DAT_00912a20; 0 params; role unknown; acts as default font factory | font_text-cont1 | hud |
| D-3110 | 0x00555af0 FUN_00555af0 | depth-2 callee of LAB_00555910; called when METRICS1 lookup returns null (null-extension branch); 4 params including xy_coords font_ctx string_buf | font_text-cont1 | hud |
| D-3111 | 0x00555ff0 FUN_00555ff0 | depth-2 callee of LAB_00555910; called when METRICS3 lookup succeeds; same 4-param signature as FUN_00555af0 | font_text-cont1 | hud |
| D-3112 | 0x00552b90 FUN_00552b90 | depth-2 callee of FUN_00427620 (HUD text shutdown); font subsystem teardown counterpart to FUN_00552b60 | font_text-cont1 | hud |
| D-0880 | ~~0x00426670 sub_00426670~~ | ~~render_frame-20260503-0611~~ | CLEARED render_frame_d3-20260503; C1/unmapped WorldRenderDispatch_Begin; spawned D-5020 | — | — |
| D-0881 | ~~0x0040de30 sub_0040de30~~ | ~~render_frame-20260503-0611~~ | CLEARED render_frame_d3-20260503; C1/unmapped MinimapCameraOrthoSetup | — | — |
| D-0882 | ~~0x0040df20 sub_0040df20~~ | ~~render_frame-20260503-0611~~ | CLEARED render_frame_d3-20260503; C1/unmapped MinimapCameraRestore | — | — |
| D-0883 | ~~0x0040df60 sub_0040df60~~ | ~~render_frame-20260503-0611~~ | CLEARED render_frame_d3-20260503; C1/unmapped ConditionalRenderSubPass; spawned D-5024 | — | — |
| D-0884 | ~~0x00404320 sub_00404320~~ | ~~render_frame-20260503-0611~~ | CLEARED render_frame_d3-20260503; C1/unmapped PerModeRenderMachine; spawned D-5025..D-5029 | — | — |
| D-0885 | ~~0x00410b30 sub_00410b30~~ | ~~render_frame-20260503-0611~~ | CLEARED render_frame_d3-20260503; C1/unmapped InGameRenderDispatcher; spawned D-5036..D-5060 | — | — |
| D-0886 | ~~0x00426030 sub_00426030~~ | ~~render_frame-20260503-0611~~ | CLEARED render_frame_d3-20260503; C1/unmapped WorldRenderPrePass; spawned D-5022 D-5023 | — | — |
| D-0887 | ~~0x004266b0 sub_004266b0~~ | ~~render_frame-20260503-0611~~ | CLEARED render_frame_d3-20260503; C1/unmapped WorldRenderDispatch_End; spawned D-5021 | — | — |
| D-0888 | ~~0x00492440 sub_00492440~~ | ~~render_frame-20260503-0611~~ | CLEARED render_frame_d3-20260503; C1/mapped RenderStatsAccumulate (leaf) | — | — |
| D-0889 | ~~0x00492e60 sub_00492e60~~ | ~~render_frame-20260503-0611~~ | CLEARED render_frame_d3-20260503; C1/unmapped SetDefaultViewWindow | — | — |
| D-0890 | ~~0x00433f40 sub_00433f40~~ | ~~render_frame-20260503-0611~~ | CLEARED render_frame_d3-20260503; C1/unmapped RaceEndFadeOverlay; spawned D-5030..D-5034 | — | — |
| D-0891 | ~~0x00403050 FUN_00403050~~ | ~~render_frame-20260503-0611~~ | CLEARED loading_screen-20260503; promoted C0→C1 frontend; pre-race loading screen renderer | — | — |
| D-0892 | ~~0x0042d390 GetRaceStateField~~ | ~~render_frame-20260503-0611~~ | CLEARED render_frame_d3-20260503; C1/mapped trivial getter DAT_0067ea6c | — | — |
| D-0893 | ~~0x0042f530 sub_0042f530~~ | ~~render_frame-20260503-0611~~ | CLEARED render_frame_d3-20260503; C1/unmapped ViewportSetup; spawned D-5035 | — | — |
| D-0894 | ~~0x0042a9f0 GetFadeAlpha~~ | ~~render_frame-20260503-0611~~ | CLEARED render_frame_d3-20260503; C1/mapped trivial getter (byte)DAT_0067eca8 | — | — |
| D-3113 | 0x0042bf30 FUN_0042bf30 | race_state-20260503 | callee of FUN_0042c280; called with 6 hardcoded args (0x27f, 0xff210000, 0, 0, 0, 0); role unknown | race_state-cont1 | race_state |
| D-3114 | 0x0040e470 FUN_0040e470 | race_state-20260503 | per-slot player check; called with slot-index 0..3 from FUN_00432080 PATH_2 and FUN_0042c220; returns 1 when slot is active | race_state-cont1 | race_state |
| D-3115 | 0x00496900 FUN_00496900 | race_state-20260503 | called with player index int from FUN_00432080 PATH_2; returns 0 on success; deeper race-state query; role unknown | race_state-cont1 | race_state |
| D-3116 | 0x0042d3a0 FUN_0042d3a0 | race_state-20260503 | shared pre-commit sub; called by both FUN_00432080 and FUN_004331a0 before trigger commit block; role unknown | race_state-cont1 | race_state |
| D-3117 | 0x004348b0 FUN_004348b0 | race_state-20260503 | called in FUN_004331a0 before tail-call to FUN_00424920; pre-tail init role unknown | race_state-cont1 | race_state |
| D-3580 | 0x004a504f FUN_004a504f | depth-3 callee of FUN_004a42c5 (sprintf_to_buf); CRT _output format engine compiled into binary; from session settings_config_d2-20260503 bucket settings_config_d2 | pick up as bucket settings_config_d2-cont1; no further recursion | save |
| D-3581 | 0x004c2e40 FUN_004c2e40 | depth-3 callee of FUN_00498c00 (VideoModeTableInit); no-arg; returns default subsystem index; role [UNCERTAIN]; from session settings_config_d2-20260503 | pick up as bucket settings_config_d2-cont1; no further recursion | save |
| D-3582 | 0x004c2f00 FUN_004c2f00 | depth-3 callee of FUN_00498c00; no-arg; returns default mode index; [OVERLAP: also re/DEFERRED D-0040 depth-3 of boot session]; from session settings_config_d2-20260503 | pick up as bucket settings_config_d2-cont1; no further recursion | save |
| D-3583 | 0x004c2de0 FUN_004c2de0 | depth-3 callee of FUN_00498c00; no-arg; returns total display subsystem count; from session settings_config_d2-20260503 | pick up as bucket settings_config_d2-cont1; no further recursion | save |
| D-3584 | 0x004c2e10 FUN_004c2e10 | depth-3 callee of FUN_00498c00; args (dest_ptr, subsystem_idx); populates 0x50-byte subsystem-info struct; from session settings_config_d2-20260503 | pick up as bucket settings_config_d2-cont1; no further recursion | save |
| D-3585 | 0x004c2ea0 FUN_004c2ea0 | depth-3 callee of FUN_00498c00; no-arg; called after FUN_004c2e70(i); returns mode count for currently-selected subsystem; from session settings_config_d2-20260503 | pick up as bucket settings_config_d2-cont1; no further recursion | save |
| D-3400 | 0x004cc820 FUN_004cc820 | depth-3 callee of 0x004cc7f0 FUN_004cc7f0; 6-arg call: (p1,p2,p3,1,0,p4); role unknown; S-1160 | pick up as bucket render_d3d_reset_d2-cont1; no further recursion | render |
| D-3401 | 0x004dcaa0 FUN_004dcaa0 | depth-3 callee of 0x004dc9e0 FUN_004dc9e0; called as FUN_004dcaa0(0x40000, out_ptr); likely CreateVertexBuffer wrapper; S-1161 | pick up as bucket render_d3d_reset_d2-cont1; no further recursion | render |
| D-3280 | (bulk) 0x00401570,0x004046a0,0x004073b0,0x00407a60,0x0040b180,0x0040bd80,0x0040d470,0x0040dbd0,0x0040de00,0x0040e480,0x0040fc00,0x004102f0,0x004103a0,0x00410860,0x00411170,0x00414060,0x00414180,0x00414220,0x00418990,0x00419760,0x0041b4d0,0x0041b510,0x0041bf20,0x0041c010,0x0041cb80,0x0041cbc0,0x0041d730,0x0041d820,0x0041e130,0x0041eda0,0x0041f000,0x00420d40,0x00422120,0x004222c0,0x00425b10,0x00426630,0x004266f0,0x00476430,0x004425d0,0x00458bf0,0x00459560,0x0045b9d0,0x0045ba00,0x0045be90,0x0045bed0,0x0046b1c0,0x0046b540,0x0046c5c0,0x00471ac0,0x00471cf0,0x00475a60,0x00477730,0x00477b40,0x00480100,0x00484c90,0x0048f260,0x004904d0,0x004a2cbd | util/multi | from session timer_d2-20260503-0656 (FUN_004111c0 bulk callees); pick up as race_state_machine-d3 or per-subsystem sessions; race/AI/physics/render/audio callees outside timer scope | timer_d2 |
| ~~D-3760~~ | ~~0x004a2c48 FUN_004a2c48~~ | CLEARED race_results_d2-20260507-1904: already C1/mapped in hud_frontend + effects_particle | — | frontend |
| ~~D-3761~~ | ~~0x0040e340 FUN_0040e340~~ | CLEARED race_results_d2-20260507-1904: already C1/mapped in game_state + vehicle_damage_d3 | — | frontend |
| ~~D-3762~~ | ~~0x0040e350 FUN_0040e350~~ | CLEARED race_results_d2-20260507-1904: already C1/mapped in ai_update + game_state + vehicle_damage_d3 | — | frontend |
| ~~D-3763~~ | ~~0x0040e370 FUN_0040e370~~ | CLEARED race_results_d2-20260507-1904: already C1/mapped in timer_d2_cont1 | — | frontend |
| ~~D-3764~~ | ~~0x004189f0 thunk_FUN_00419760~~ | CLEARED race_results_d2-20260507-1904: C1/new re/analysis/race_results_d2/004189f0.md; thunkee D-8500 | — | frontend |
| ~~D-3765~~ | ~~0x004215c0 FUN_004215c0~~ | CLEARED race_results_d2-20260507-1904: C1/new re/analysis/race_results_d2/004215c0.md; callee D-8501 | — | frontend |
| ~~D-3766~~ | ~~0x0045ba00 FUN_0045ba00~~ | CLEARED race_results_d2-20260507-1904: C1/new re/analysis/race_results_d2/0045ba00.md | — | frontend |
| ~~D-3767~~ | ~~0x0046c5c0 FUN_0046c5c0~~ | CLEARED race_results_d2-20260507-1904: C1/new re/analysis/race_results_d2/0046c5c0.md | — | frontend |
| ~~D-3768~~ | ~~0x0046c790 FUN_0046c790~~ | CLEARED race_results_d2-20260507-1904: C1/new re/analysis/race_results_d2/0046c790.md | — | frontend |
| D-3281 | (bulk) 0x0040ab40,0x0040ac80,0x0040b810,0x0040de10,0x0040e360,0x00422b30,0x00429aa0,0x0042af50,0x0042b950,0x0042c150,0x0046dc00,0x00493570,0x00493580 | util/multi | from session timer_d2-20260503-0656 (FUN_0043d7c0 secondary callees); S range exhausted; pick up per-subsystem or in timer_d2-cont1 | timer_d2 |
| D-3282 | 0x0043dfd0 | FUN_0043dfd0 | util | from session timer_d2-20260503-0656; decomp 63977 chars / 30839 tokens exceeded read buffer; C0 only; requires dedicated large-function session (Opus recommended); 63 callees unprocessed | timer_d2 |
| D-3283 | (bulk) 0x0040e370,0x0042b900,0x00431b50,0x00431b60,0x00432290,0x0045c480,0x0045d3a0,0x0045d7a0,0x0045db50,0x0045dbe0,0x0045dc80,0x0045df70,0x0045dfc0,0x00462520,0x004627b0,0x00462dd0,0x00462ec0,0x00463590,0x00463640,0x004647f0,0x004648b0,0x00492d10,0x004d8560,0x005a89a0,0x005a89b0,0x005a89c0 | util/audio | from session timer_d2-20260503-0656 (FUN_00466b50 untracked callees); S range exhausted; pick up in audio_tick_d3 or dedicated sessions | timer_d2 |
| D-4600 | 0x00482860 FUN_00482860 | replay object internal reset (rewind to head, zero write cursor, clear started flag); 50-100 bytes; needed for full lap-cycle reset understanding | replay_record-cont1 session | vehicle |
| D-4601 | 0x00483a30 FUN_00483a30 | replay rewind to start for ghost playback; sets read cursor to head node; needed to understand how ghost replays beginning-of-lap | replay_record-cont1 session | vehicle |
| D-4602 | 0x00483a40 FUN_00483a40 | replay object destructor/free; releases linked list and struct; needed for lifecycle and greenfield allocation model | replay_record-cont1 session | vehicle |
| D-4603 | 0x0046d4a0 FUN_0046d4a0 | vehicle state getter; returns ptr to struct holding quaternion at some offset and xyz at +0x30..+0x38; needed to map vehicle struct layout for replay port | replay_record-cont1 session | vehicle |
| D-4604 | 0x00546b10 FUN_00546b10 | RW quaternion copier from vehicle struct to replay frame node +0x00; needed to identify quaternion source offset in vehicle struct | replay_record-cont1 session | vehicle |
| D-4605 | 0x00483ca0 FUN_00483ca0 | serialize replay object to binary buffer (write path for .rep files); counterpart to Replay::Load (0x00483d10); needed to complete .rep file format spec | replay_record-cont1 session | vehicle |
| D-4606 | (trio) 0x004cc230 0x004cbd30 0x004cc160 | custom stream open/read/close API used for replay .rep file I/O; confirm these are wrappers around fopen/fread/fclose or a custom VFS | replay_record-cont1 session | vehicle |
| D-4607 | 0x0041a9b0 0x0041ad00 0x0041a960 | ghost car rendering trio: setup-transform, apply-transform, renderer-init; needed for ghost car visual in greenfield port | replay_record-cont1 session | vehicle |
| D-4360 | 0x00554940 LAB_00554940 (per-char Im2D emission loop, 0x00554a5b onward, ~400 instrs remaining) | Full Im2D quad construction requires Im2D vertex struct layout; separate depth-3 scope needed | After Im2D vertex layout confirmed from librw cross-check | hud |
| D-4361 | 0x00555910 LAB_00555910 (METRICS1/2/3 section parsers, 0x00555a04+) | Parsing logic needs .met file sample for cross-check; font36.piz extraction first | After .met file extracted from font36.piz via piz skill | hud |
| D-4362 | 0x005551d0 FUN_005551d0 | U-1490: role as LAB_00555910 entry guard unknown; guards entire .met loader | Decompile in next font_text_d2-cont1 or dedicated depth-3 session | hud |
| D-4363 | (quad) 0x00552d10 0x00552df0 0x00552da0 0x00552e40 | Im2D render-state setters called from LAB_00554940; address scope outside hud core | Resolve in im2d / render_pipeline depth session | hud |
| D-4364 | 0x004c4a50 FUN_004c4a50 | Angle-axis matrix builder called from FUN_004c4d20; deeper render-math; separate scope | Resolve in render_pipeline or rw_math depth session | render |
| D-4365 | 0x00550580 FUN_00550580 | VFS file-open implementation; separate filesystem/io subsystem | Resolve in dedicated vfs / filesystem session | io |
| ~~D-4000~~ | ~~0x005aabe0 FUN_005aabe0~~ | ~~depth-3 callee of FUN_005ba720/FUN_005ba760; result gates CoInitialize; S-1360~~ | ~~drained by audio_dsound_d3-20260508; C1 plate written~~ | ~~audio~~ |
| ~~D-4001~~ | ~~0x005bc860 FUN_005bc860~~ | ~~depth-3 callee of FUN_005ba720; called after CoInitialize; S-1361~~ | ~~drained by audio_dsound_d3-20260508; C1 plate written~~ | ~~audio~~ |
| ~~D-4002~~ | ~~0x005bc880 FUN_005bc880~~ | ~~depth-3 callee of FUN_005ba760; 11-byte body; called when check<2; S-1362~~ | ~~drained by audio_dsound_d3-20260508; C1 plate written~~ | ~~audio~~ |
| ~~D-4003~~ | ~~0x005baf40 FUN_005baf40~~ | ~~depth-3 callee of LAB_005ba780; called (EDI,0x0) after FUN_005bb000; S-1363~~ | ~~drained by audio_dsound_d3-20260508; C1 plate written~~ | ~~audio~~ |
| ~~D-4004~~ | ~~0x005aaa00 FUN_005aaa00~~ | ~~depth-3 callee of LAB_005ba780; called (EAX,0,0) when [ESI+0xf4]!=0; S-1364~~ | ~~drained by audio_dsound_d3-20260508; C1 plate written~~ | ~~audio~~ |
| ~~D-4005~~ | ~~0x005aab70 FUN_005aab70~~ | ~~depth-3 callee of LAB_005ba7f0; called at entry (EDI, 0x1); S-1365~~ | ~~drained by audio_dsound_d3-20260508; C1 plate written~~ | ~~audio~~ |
| ~~D-4006~~ | ~~0x005baa60 FUN_005baa60~~ | ~~depth-3 callee of LAB_005ba7f0 + FUN_005bac00; motion-state processing or similar; S-1366~~ | ~~drained by audio_dsound_d3-20260508; C1 plate written~~ | ~~audio~~ |
| ~~D-4007~~ | ~~0x005bc750 FUN_005bc750~~ | ~~depth-3 callee of LAB_005ba7f0; called when [EDI+0x78] bit 0x8 set and bits 0x7 clear; S-1367~~ | ~~drained by audio_dsound_d3-20260508; C1 plate written~~ | ~~audio~~ |
| ~~D-4008~~ | ~~0x005aea50 FUN_005aea50~~ | ~~depth-3 callee of FUN_005bb000; allocator (1, size, 0x3080c); streaming buffer alloc; S-1368~~ | ~~already mapped by audio_rws_loader_d3 (re/analysis/audio_rws_loader_d3/005aea50.md); C1 in hooks.csv~~ | ~~audio~~ |
| ~~D-4009~~ | ~~0x005aeea0 FUN_005aeea0~~ | ~~depth-3 callee of FUN_005bb000; event/handle init (param+0x3d, 0, 1); S-1369~~ | ~~drained by audio_dsound_d3-20260508; C1 plate written~~ | ~~audio~~ |
| ~~D-4010~~ | ~~0x005aef00 FUN_005aef00~~ | ~~depth-3 callee of FUN_005bb000; thread creation (param+0x3e, LAB_005bb380, 0xf, 0x1000); S-1370~~ | ~~drained by audio_dsound_d3-20260508; C1 plate written (struct init, not thread create)~~ | ~~audio~~ |
| ~~D-4011~~ | ~~0x005aef30 FUN_005aef30~~ | ~~depth-3 callee of FUN_005bb000; thread start/bind (param+0x3e, param+0x27); S-1371~~ | ~~drained by audio_dsound_d3-20260508; C1 plate written~~ | ~~audio~~ |
| ~~D-4012~~ | ~~0x005bc470 FUN_005bc470~~ | ~~depth-3 callee of FUN_005bb000; streaming format path (DAT_005d098c, format, param+0x2a); S-1372~~ | ~~drained by audio_dsound_d3-20260508; C1 plate written~~ | ~~audio~~ |
| ~~D-4013~~ | ~~0x005bc640 FUN_005bc640~~ | ~~depth-3 callee of FUN_005bb000; streaming teardown helper (param+0x2a); S-1373~~ | ~~drained by audio_dsound_d3-20260508; C1 plate written~~ | ~~audio~~ |
| ~~D-4014~~ | ~~0x005bbd50 FUN_005bbd50~~ | ~~depth-3 callee of FUN_005bbc10; returns COM interface from (param_1, param_5); S-1374~~ | ~~drained by audio_dsound_d3-20260508; C1 plate written~~ | ~~audio~~ |
| ~~D-4015~~ | ~~0x005bbed0 FUN_005bbed0~~ | ~~depth-3 callee of FUN_005bbdb0; post-CreateSoundBuffer; takes (param_6, param_3, unaff_retaddr); S-1375~~ | ~~drained by audio_dsound_d3-20260508; C1 plate written~~ | ~~audio~~ |
| ~~D-4240~~ | ~~0x004547c0 FUN_004547c0~~ | ~~drained by powerups_d3-20260506-0504; analyzed as EDI-implicit struct-A reset~~ | ~~powerups_d2-cont1~~ | ~~vehicle~~ |
| ~~D-4241~~ | ~~0x00454170 FUN_00454170~~ | ~~drained by powerups_d3-20260506-0504; analyzed as ESI-implicit struct-B zero+teardown~~ | ~~powerups_d2-cont1~~ | ~~vehicle~~ |
| ~~D-4242~~ | ~~0x00534b60 FUN_00534b60~~ | ~~drained by powerups_d3-20260506-0504; analyzed as flags normalizer→FUN_00534d00; clears S-1444~~ | ~~powerups_d2-cont1~~ | ~~vehicle~~ |
| ~~D-4243~~ | ~~14 RW hierarchy internals~~ | ~~drained by powerups_d3-20260506-0504; all 14 analyzed (13 new C1 rows; 0x004d8060 already C1); depth-4 in D-5680..D-5686~~ | ~~powerups_d2-cont1~~ | ~~vehicle~~ |
| D-4540 | 0x0040dd60 FUN_0040dd60 | RESOLVED profile_career_d2 2026-05-05: fully reversed C1; ((DAT_0063b90c==1)?0xFFFFFFFF:0)&DAT_007f0fcc; S-1540 cleared | resolved | save |
| D-4541 | 0x00448220 FUN_00448220 | RESOLVED profile_career_d2 2026-05-05: post-race camera+unlock-code C1 mapped; deeper callees in D-5500..D-5504; U-1553 resolved; U-1870 U-1873 new | resolved-partial | frontend |
| D-4542 | 0x00410510 FUN_00410510 | RESOLVED profile_career_d2 2026-05-05: race-end evaluator C1 mapped; 820B; U-1867..U-1872; S-1860..S-1867 | resolved | save |
| D-5140 | 0x004c3b90 FUN_004c3b90 | RESOLVED vehicle_dynamics-20260506-expand: fast reciprocal-sqrt via LUT; C1 new re/analysis/vehicle_dynamics/004c3b90.md | resolved | render |
| D-5141 | 0x0046ef70 FUN_0046ef70 | RESOLVED vehicle_dynamics-20260506-expand: wheel contact spring/damper resolver; 1871b; C1 new re/analysis/vehicle_dynamics/0046ef70.md; U-2629 U-2630 U-2631; S-2623 | resolved | vehicle |
| D-4960 | 0x004d7d70 LAB_004d7d70 | depth-4 callee of FUN_004ccce0 (callback); unrecognized fn body; reads param_1+0x38 sub-struct; clears +0x10/+0x14; calls vtable+0x11c; S-1680; Ghidra has no FUN_ entry at this address | rw_engine_teardown_d3-cont1 | render |
| D-4840 | 0x004323c0 FUN_004323c0 | depth-2 callee of FUN_0043dfd0; 135 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4841 | 0x0042ae10 FUN_0042ae10 | depth-2 callee of FUN_0043dfd0; 156 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4842 | 0x0042aeb0 FUN_0042aeb0 | depth-2 callee of FUN_0043dfd0; 156 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4843 | 0x0042af50 FUN_0042af50 | depth-2 callee of FUN_0043dfd0; 156 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4844 | 0x00430910 FUN_00430910 | depth-2 callee of FUN_0043dfd0 and FUN_004322c0; 137 bytes; S-1656; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4845 | 0x00429aa0 FUN_00429aa0 | depth-2 callee of FUN_0043dfd0; 134 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4846 | 0x0042aa00 FUN_0042aa00 | depth-2 callee of FUN_0043dfd0; 168 bytes; S-1655; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4847 | 0x0042ac90 FUN_0042ac90 | depth-2 callee of FUN_0043dfd0; 126 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4848 | 0x0040b810 FUN_0040b810 | depth-2 callee of FUN_0043dfd0; 124 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4849 | 0x0042b960 FUN_0042b960 | depth-2 callee of FUN_0043dfd0; 115 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4850 | 0x0042f6b0 FUN_0042f6b0 | depth-2 callee of FUN_0043dfd0; 115 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4851 | 0x004307a0 FUN_004307a0 | depth-2 callee of FUN_0043dfd0; 116 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4852 | 0x0042f020 FUN_0042f020 | depth-2 callee of FUN_0043dfd0; 98 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4853 | 0x00431d00 FUN_00431d00 | depth-2 callee of FUN_0043dfd0; 97 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4854 | 0x004309b0 FUN_004309b0 | depth-2 callee of FUN_0043dfd0; 52 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4855 | 0x00492340 FUN_00492340 | depth-2 callee of FUN_0043dfd0; 46 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4856 | 0x00430b60 FUN_00430b60 | depth-2 callee of FUN_0043dfd0; 47 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4857 | 0x00409930 FUN_00409930 | depth-2 callee of FUN_0043dfd0; 30 bytes; sub-cap in session GGGG | game_mode_cont2 | util |
| D-4858 | 0x00409900 FUN_00409900 | depth-2 callee of FUN_0043dfd0; 43 bytes; sub-cap in session GGGG | game_mode_cont2 | util |
| D-4859 | 0x004098b0 FUN_004098b0 | depth-2 callee of FUN_0043dfd0; 27 bytes; sub-cap in session GGGG | game_mode_cont2 | util |
| D-4860 | 0x004298c0 FUN_004298c0 | depth-2 callee of FUN_0043dfd0; 27 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4861 | 0x0040acd0 FUN_0040acd0 | depth-2 callee of FUN_0043dfd0; 48 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4862 | 0x00414120 FUN_00414120 | depth-2 callee of FUN_0043dfd0; 80 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4863 | 0x00422b30 FUN_00422b30 | depth-2 callee of FUN_0043dfd0; 16 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4864 | 0x0046dc00 FUN_0046dc00 | depth-2 callee of FUN_0043dfd0; 20 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4865 | 0x00495080 FUN_00495080 | depth-2 callee of FUN_0043dfd0; 37 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4866 | 0x00494f30 FUN_00494f30 | depth-2 callee of FUN_0043dfd0; 15 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4867 | 0x0040e480 FUN_0040e480 | depth-2 callee of FUN_0043dfd0; 18 bytes; sub-cap in session GGGG | game_mode_cont2 | frontend |
| D-4868 | 0x00492d30 FUN_00492d30 | sole caller of FUN_0043dfd0; 265 bytes; not decompiled this session | game_mode_cont2 | util |
| D-5080 | 0x005aa1e0 FUN_005aa1e0 | depth-4 of FUN_005aa060 (audio_rws_loader_d3 session); S-1720; inline callback at LAB_005aa1e0; no Ghidra function body; tree-walk predicate callback | audio_rws_loader_d3-cont1 | audio |
| D-4720 | (bulk) 0x0045db50,0x0045dbe0,0x0045dc80,0x0045df70,0x0045dfc0,0x00462520,0x004627b0,0x00462dd0,0x00462ec0,0x00463590,0x00463640,0x004647f0,0x004648b0,0x0046dc00,0x00492d10,0x00493570,0x00493580,0x004d8560,0x005a89a0,0x005a89b0,0x005a89c0 | from session timer_d2_cont1-20260503-1824; early-finish at cap_count=18; pick up as bucket timer_d2_cont2; same depth-2 | timer_d2_cont2 | util/audio |
| D-4721 | 0x0043dfd0 FUN_0043dfd0 | re-defer from D-3282: decomp 63977 chars / ~30839 tokens; C0 note exists in timer_d2 bucket; requires dedicated Opus session; 63 callees unprocessed | timer_d2_opus | util |
| ~~D-5020~~ | ~~0x004e4320 FUN_004e4320~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| ~~D-5021~~ | ~~0x004e4350 FUN_004e4350~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| ~~D-5022~~ | ~~0x0041ea10 FUN_0041ea10~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| ~~D-5023~~ | ~~0x0041e8f0 FUN_0041e8f0~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| ~~D-5024~~ | ~~0x00401f10 FUN_00401f10~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| ~~D-5025~~ | ~~0x00403d30 FUN_00403d30~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| ~~D-5026~~ | ~~0x00403db0 FUN_00403db0~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| ~~D-5027~~ | ~~0x00403ed0 FUN_00403ed0~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| ~~D-5028~~ | ~~0x00403fa0 FUN_00403fa0~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| ~~D-5029~~ | ~~0x004041c0 FUN_004041c0~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| ~~D-5030~~ | ~~0x0042c010 FUN_0042c010~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| ~~D-5031~~ | ~~0x0042c090 FUN_0042c090~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| ~~D-5032~~ | ~~0x004278d0 FUN_004278d0~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| ~~D-5033~~ | ~~0x00427990 FUN_00427990~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| ~~D-5034~~ | ~~0x00427be0 FUN_00427be0~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| ~~D-5035~~ | ~~0x004c7760 FUN_004c7760~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| D-5036 | 0x004270f0 CourseRenderFrame | already mapped — recorded for completeness; callee of sub_00410b30 | — | — |
| ~~D-5037~~ | ~~0x004725c0 FUN_004725c0~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| D-5038 | ~~0x00420050 FUN_00420050~~ | ~~depth-3 callee of sub_00410b30; per-player render pass; called 4× with (i, camera)~~ | CLEARED split_screen-20260505; C1/mapped PerPlayerViewportRender | — |
| ~~D-5039~~ | ~~0x0041ebb0 FUN_0041ebb0~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| ~~D-5040~~ | ~~0x004219c0 FUN_004219c0~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| ~~D-5041~~ | ~~0x00425e40 FUN_00425e40~~ | RESOLVED 2026-05-08: analyzed C1 session render_frame_d4-20260508-0332; see re/analysis/render_frame_d4/ | — | render |
| D-5042 | 0x00426640 FUN_00426640 | depth-3 callee of sub_00410b30; called with player camera (PTR_PTR_005f2770[DAT_0063ba78]) | render_frame_d3-cont1 | render |
| D-5043 | 0x0040bde0 FUN_0040bde0 | depth-3 callee of sub_00410b30; unknown | render_frame_d3-cont1 | render |
| D-5044 | 0x004891f0 FUN_004891f0 | depth-3 callee of sub_00410b30; unknown | render_frame_d3-cont1 | render |
| D-5045 | 0x00475e50 FUN_00475e50 | depth-3 callee of sub_00410b30; unknown | render_frame_d3-cont1 | render |
| D-5046 | 0x00475d30 FUN_00475d30 | depth-3 callee of sub_00410b30; unknown | render_frame_d3-cont1 | render |
| D-5047 | 0x00477810 FUN_00477810 | depth-3 callee of sub_00410b30; unknown | render_frame_d3-cont1 | render |
| D-5048 | 0x00477a10 FUN_00477a10 | depth-3 callee of sub_00410b30; unknown | render_frame_d3-cont1 | render |
| D-5049 | 0x004189c0 FUN_004189c0 | depth-3 callee of sub_00410b30; unknown | render_frame_d3-cont1 | render |
| D-5050 | 0x0048fd70 FUN_0048fd70 | depth-3 callee of sub_00410b30; unknown | render_frame_d3-cont1 | render |
| D-5051 | 0x0045b990 FUN_0045b990 | depth-3 callee of sub_00410b30; unknown | render_frame_d3-cont1 | render |
| D-5052 | 0x00413a00 FUN_00413a00 | depth-3 callee of sub_00410b30; called with primary player camera (FUN_004671a0(0)) | render_frame_d3-cont1 | render |
| D-5053 | 0x00413a40 FUN_00413a40 | depth-3 callee of sub_00410b30; unknown | render_frame_d3-cont1 | render |
| D-5054 | 0x004072e0 FUN_004072e0 | depth-3 callee of sub_00410b30; unknown | render_frame_d3-cont1 | render |
| D-5055 | 0x00404650 FUN_00404650 | depth-3 callee of sub_00410b30; unknown | render_frame_d3-cont1 | render |
| D-5056 | 0x0047b9e0 FUN_0047b9e0 | depth-3 callee of sub_00410b30; post-effects; conditional (state≠7 && DAT_007f0f3c≠0); called with camera | render_frame_d3-cont1 | render |
| D-5057 | 0x00448730 FUN_00448730 | depth-3 callee of sub_00410b30; HUD/frontend layer | render_frame_d3-cont1 | render |
| D-5058 | 0x00490490 FUN_00490490 | depth-3 callee of sub_00410b30; unknown | render_frame_d3-cont1 | render |
| D-5059 | 0x00403910 FUN_00403910 | depth-3 callee of sub_00410b30; conditional (mode≠9 && mode≠10 && DAT_007f0f30≠0) | render_frame_d3-cont1 | render |
| D-5060 | 0x00429e10 FUN_00429e10 | depth-3 callee of sub_00410b30; race overlay when FUN_00426ba0≠0 | render_frame_d3-cont1 | render |
| D-5560 | 0x004c3b30 FUN_004c3b30 | depth-4 callee of FUN_00414300; fast sqrt via two-level lookup table at DAT_007d3ff8/ffc; used for pursuit-lead normalization | ai_update_d3-cont1 | ai |
| D-5561 | 0x00416230 FUN_00416230 | depth-4 callee of FUN_00443dc0; called as FUN_00416230(param_4,iVar8==0); writes vehicle lookahead-index commitment; ~17 bytes | ai_update_d3-cont1 | ai |
| D-5562 | 0x004b55a0 FUN_004b55a0 | depth-4 callee of FUN_00443dc0; render submit/draw call; large function 0x1A4 bytes; used for debug waypoint rendering | ai_update_d3-cont1 | render |
| D-5563 | 0x004c3bf0 FUN_004c3bf0 | depth-4 callee of FUN_00443dc0; 2D vector distance / magnitude; ~100 bytes; pair with FUN_004c3c60 normalizer | ai_update_d3-cont1 | ai |
| D-5564 | 0x004c3c60 FUN_004c3c60 | depth-4 callee of FUN_00443dc0; 2D vector normalizer; called as FUN_004c3c60(&vec,&vec) (in-place); ~249 bytes | ai_update_d3-cont1 | ai |
| ~~D-5620~~ | ~~0x0041f8f0 FUN_0041f8f0~~ | RESOLVED split_screen_d2-20260507-1852: C1; ESI-based per-player render-pass setup; see re/analysis/split_screen_d2/0041f8f0.md | — | render |
| ~~D-5621~~ | ~~0x004228f0 FUN_004228f0~~ | RESOLVED split_screen_d2-20260507-1852: C1; per-player geometry batch draw with alpha blend; see re/analysis/split_screen_d2/004228f0.md | — | render |
| ~~D-5622~~ | ~~0x00426060 FUN_00426060~~ | RESOLVED split_screen_d2-20260507-1852: C1 confirmed; returns DAT_0065742c; pre-existing analysis at re/analysis/track_loader_d3/00426060_00426070.md | — | render |
| ~~D-5623~~ | ~~0x004260c0 FUN_004260c0~~ | RESOLVED split_screen_d2-20260507-1852: C1; returns DAT_00657490; see re/analysis/split_screen_d2/004260c0.md | — | render |
| ~~D-5624~~ | ~~0x004e4900 FUN_004e4900~~ | RESOLVED split_screen_d2-20260507-1852: C1; writes float4 to param_1+0x18..+0x24, sets uniform-scale byte at +3; see re/analysis/split_screen_d2/004e4900.md | — | render |
| D-8440 | 0x004c1340 FUN_004c1340 | depth-3 of PerPlayerViewportRender via 0x0041f8f0; set-pos-to-camera (also ref'd in SkyDomeUpdatePos); split_screen_d2 | split_screen_d2-cont1 | render |
| D-8441 | 0x004b42c0 FUN_004b42c0 | depth-3 of PerPlayerViewportRender via 0x0041f8f0; called as FUN_004b42c0(cam, player_pos_ptr, &DAT_006146fc) | split_screen_d2-cont1 | render |
| D-8442 | 0x004c1680 FUN_004c1680 | depth-3 of PerPlayerViewportRender via 0x0041f8f0; 1-arg camera setup call | split_screen_d2-cont1 | render |
| D-8443 | 0x00422ac0,0x00422af0 FUN_00422ac0/af0 | depth-3 pair via 0x0041f8f0; render-state setup: 00422ac0(ESI+0x290, color_block); 00422af0(ESI+0x290, cam_handle) | split_screen_d2-cont1 | render |
| D-8444 | 0x00422570 FUN_00422570 | depth-3 via 0x0041f8f0; 3-arg call (ESI+0x290, *(*(ESI+0x26c)+4)+0x10, uVar3) | split_screen_d2-cont1 | render |
| D-8445 | 0x00426e00 FUN_00426e00 | depth-3 via 0x0041f8f0; no-arg getter; return value cast to float10 then scaled by ESI+0x288 and ESI+0x284 | split_screen_d2-cont1 | render |
| D-8446 | 0x004cd070,0x004cd2d0,0x004cd140 FUN_004cdXXX | depth-3 draw/flush cluster via 0x004228f0; 004cd070(buf,count*3,0,0x19) then 004cd2d0(3)+004cd140() on non-zero | split_screen_d2-cont1 | render |
| D-5500 | 0x00441990 FUN_00441990 | alternative result handler in FUN_00448220 when DAT_007f1a50==1; sole caller FUN_00448220; size unknown; needed to understand DAT_007f1a50 semantics (U-1870); S-1868 | profile_career_d2-cont1 | frontend |
| D-5501 | 0x00446520 FUN_00446520 | main result display state machine; called from FUN_00448220 via LAB_004483a3 on race states 6/7/-1; receives &DAT_00897fe0+param_3; needed for U-1552 (priority 0x40 in DAT_00898ab0); S-1869 | profile_career_d2-cont1 | frontend |
| D-5502 | 0x00429a70 FUN_00429a70 | race data getter C; used alongside 0x00429a80/90 in FUN_00448220 unlock-code generation; depth-2 of FUN_00434720; S-1875 | profile_career_d2-cont1 | frontend |
| D-5503 | DAT_0063b900..0x0063bb00 | race-state memory block; contains 0063ba8c (race state), 0063b90c (concluded flag), 0063b910 (700.0f), 0063b914 (winner idx), 0063b918/1c (zeros); full layout not yet mapped; needed for U-1871 U-1872 | profile_career_d2-cont1 | save |
| D-5504 | 0x0046d4a0+0x0046b4f0+0x0046d510+0x00442e00 | camera-follow callees in FUN_00448220: FUN_0046d4a0 (vehicle state ptr), FUN_0046b4f0 (vehicle data float pairs), FUN_0046d510 (velocity), FUN_00442e00 (camera render setup); all vehicle subsystem; S-1873 S-1874 | profile_career_d2-cont1 | vehicle |
| ~~D-5440~~ | ~~0x00405890 FUN_00405890~~ | CLEARED vehicle_damage_d3-20260506-1244: C1/mapped; re/analysis/vehicle_damage_d3/0x00405890.md; U-2167 U-2168 | — | vehicle |
| ~~D-5441~~ | ~~0x00408a50 FUN_00408a50~~ | CLEARED vehicle_damage_d3-20260506-1244: C1/mapped; re/analysis/vehicle_damage_d3/0x00408a50.md; prior C1 race_results+ai_update_d3 | — | vehicle |
| ~~D-5442~~ | ~~0x00408a70 FUN_00408a70~~ | CLEARED vehicle_damage_d3-20260506-1244: C1/mapped; re/analysis/vehicle_damage_d3/0x00408a70.md; U-2169 U-2170 | — | vehicle |
| ~~D-5443~~ | ~~0x0040e340 FUN_0040e340~~ | CLEARED vehicle_damage_d3-20260506-1244: C1/mapped; re/analysis/vehicle_damage_d3/0x0040e340.md; U-2171 | — | vehicle |
| ~~D-5444~~ | ~~0x00417730 FUN_00417730~~ | CLEARED vehicle_damage_d3-20260506-1244: C1/mapped; re/analysis/vehicle_damage_d3/0x00417730.md; U-2173 U-2174 | — | vehicle |
| ~~D-5445~~ | ~~0x00423b20 FUN_00423b20~~ | CLEARED vehicle_damage_d3-20260506-1244: C1/mapped; re/analysis/vehicle_damage_d3/0x00423b20.md; U-2175 U-2176 | — | vehicle |
| ~~D-5446~~ | ~~0x0040e350 FUN_0040e350~~ | CLEARED vehicle_damage_d3-20260506-1244: C1/mapped; re/analysis/vehicle_damage_d3/0x0040e350.md; S-1842 resolved; U-2172 | — | vehicle |
| D-5740 | 0x004b3c60 FUN_004b3c60 | depth-4 callee of FUN_0042a640 (track_loader_d3 session); U-1947; BSP/RpWorld stream reader | track_loader_d3-cont1 | render |
| D-5741 | 0x00558df0 FUN_00558df0 | depth-4 callee of FUN_0042a740 (track_loader_d3 session); U-1948; UVAnim chunk loader; (plugin_ptr, stream) | track_loader_d3-cont1 | render |
| D-5742 | 0x004b3cc0 FUN_004b3cc0 | depth-4 callee of FUN_0042a7f0 (track_loader_d3 session); U-1949; spline stream reader | track_loader_d3-cont1 | render |
| D-5743 | 0x004b3de0 FUN_004b3de0 | depth-4 callee of FUN_0042a860 (track_loader_d3 session); U-1950; animation stream reader | track_loader_d3-cont1 | render |
| D-5744 | 0x00479030 FUN_00479030 | depth-4 callee of FUN_004790e0 (track_loader_d3 session); U-1951; course post-load callback table | track_loader_d3-cont1 | render |
| D-5745 | 0x00474fb0 FUN_00474fb0 | depth-4 callee of FUN_00474fd0 (track_loader_d3 session); U-1952; DFF clump node iterator; (clump, callback_struct) | track_loader_d3-cont1 | render |
| D-5746 | 0x00474f30 FUN_00474f30 | depth-4 callee of FUN_00474fd0 (track_loader_d3 session); U-1953; sky dome per-node callback; processes one RpFrame | track_loader_d3-cont1 | render |
| D-5747 | 0x0047f4c0 FUN_0047f4c0 | depth-4 callee of FUN_0047f840 (track_loader_d3 session); U-1954; physics world constructor; (scale:1.0f) | track_loader_d3-cont1 | render |
| D-5748 | 0x0047d080 FUN_0047d080 | depth-4 callee of FUN_00480100 (track_loader_d3 session); U-1955; activate physics body slot; (idx, 1) | track_loader_d3-cont1 | render |
| D-5749 | 0x0047d100 FUN_0047d100 | depth-4 callee of FUN_00480100 (track_loader_d3 session); U-1956; secondary enable physics body; (idx, 1) | track_loader_d3-cont1 | render |
| D-5750 | 0x00487280 FUN_00487280 | depth-4 callee of FUN_00480100 (track_loader_d3 session); U-1957; broadphase body registration; (bvh, transform, bounds, out, slot) | track_loader_d3-cont1 | render |
| D-5751 | 0x0047be80 FUN_0047be80 | depth-4 callee of FUN_0047bf70 (track_loader_d3 session); U-1958; triangle mesh init; (tri_count) | track_loader_d3-cont1 | render |
| D-5752 | 0x0047bcc0 FUN_0047bcc0 | depth-4 callee of FUN_0047bf70 (track_loader_d3 session); U-1959; collect portal/neighbor list; (&list_ptr, chain_head) | track_loader_d3-cont1 | render |
| D-5753 | 0x004b53b0 FUN_004b53b0 | depth-4 callee of FUN_0047bf70 (track_loader_d3 session); U-1960; bounding sphere builder; (out+0x4e, bounding_data, count) | track_loader_d3-cont1 | render |
| D-5754 | 0x004c3d90 FUN_004c3d90 | depth-4 callee of FUN_0047bf70 (track_loader_d3 session); U-1961; sector bounding geometry builder; (out+0x24, mesh_chunk, vertex_info, flags) | track_loader_d3-cont1 | render |
| D-5755 | 0x00546380 FUN_00546380 | depth-4 callee of FUN_0045e2a0 (track_loader_d3 session); U-1962; audio waypoint set constructor; (count, type, points_array) | track_loader_d3-cont1 | render |
| D-6280 | LAB_00554940 remaining per-char Im2D emission loop (0x00554df0..0x00555130, ~250 instructions) | Cap: full quad UV + remaining vertices + loop-back; U-2128 U-2129 U-2130 U-2131 U-2133 | font_text_d4 | hud |
| D-6281 | LAB_00555910 METRICS1/2/3 section parsers (carried from D-4361) | Needs .met file sample for cross-check | after font36.piz extracted | hud |
| D-6282 | Glyph entry full struct (offsets +0x00, +0x10..+0x13, +0x18..+0x1f; stride 32); U-2129 | Requires listing of all remaining quad vertices in D-6280 | font_text_d4 | hud |
| D-6283 | Vertex format fields +0x0c..+0x17 (z, rhw, 8 unknown bytes); U-2130 | listing_code_units_list 0x00554b70..0x00554b82 for missing vertex writes | font_text_d4 | hud |
| D-6284 | FUN_004c4600 matrix multiply (4 args: dst, src1, src2, scale); S-2124 | Deeper render-math scope; separate session | render_pipeline or rw_math depth session | render |
| D-6285 | FUN_004c4dc0 matrix invert/copy into DAT_00912b58; S-2125 | Render math scope | render_pipeline depth session | render |
| D-6286 | FUN_004c0ed0 camera view matrix getter (cam+4 field); S-2126 | Camera/render scope | camera_follow or render_pipeline session | render |
| D-6287 | FUN_00552d70 FontMatrix_Pop (counterpart to FontMatrix_Push 0x00552d10); S-2127 | Quick: decomp_function 0x00552d70 | font_text_d4 session | hud |
| D-6288 | FUN_004cd070 FUN_004cd170 FUN_004cd140 Im2D batch calls; S-2120 S-2121 S-2122 | Im2D subsystem scope | im2d/render_pipeline depth session | hud |
| D-6289 | FUN_005c4ad0 glyph data buffer alloc (0x20 bytes, type 0x30190); S-2123 | Allocator scope; decomp_function 0x005c4ad0 | font_text_d4 session | hud |
| D-6290 | ESI prologue trace in LAB_00554940 to confirm ESI source struct; U-2131 | listing_code_units_list 0x00554940..0x00554965 | font_text_d4 session | hud |
| D-5920 | 0x004a0ef0 FUN_004a0ef0 | depth-4 callee of FUN_0049dd60; 93b __thiscall; receives (param_3, param_4, &this+0x7c, param_2); called before vtable writes; S-2000; from session video_mci_d3-20260506-0512 | video_mci_d4 | video |
| D-5921 | 0x004a1160 FUN_004a1160 | depth-4 callee of FUN_0049dd60; 27b __thiscall; called 3x on offsets 0x54/0x58/0x5c with args 0/1/1; result at 0x5c used as HANDLE in SetEvent; S-2001; from session video_mci_d3-20260506-0512 | video_mci_d4 | video |
| ~~D-5680~~ | ~~0x004c1210,0x004c15c0,0x004e43b0~~ | ~~drained by powerups_d4-20260508-1823; all 3 analyzed C1~~ | ~~powerups_d3-cont1~~ | ~~gameplay~~ |
| ~~D-5681~~ | ~~0x004e4800 FUN_004e4800~~ | ~~drained by powerups_d4-20260508-1823; analyzed C1~~ | ~~powerups_d3-cont1~~ | ~~gameplay~~ |
| ~~D-5682~~ | ~~0x00534d00 FUN_00534d00~~ | ~~drained by powerups_d4-20260508-1823; analyzed C1; depth-5 in D-10480~~ | ~~powerups_d3-cont1~~ | ~~gameplay~~ |
| ~~D-5683~~ | ~~0x004c0910 FUN_004c0910~~ | ~~drained by powerups_d4-20260508-1823; analyzed C1; depth-5 in D-10481~~ | ~~powerups_d3-cont1~~ | ~~gameplay~~ |
| ~~D-5684~~ | ~~0x004c0d70 FUN_004c0d70~~ | ~~drained by powerups_d4-20260508-1823; analyzed C1~~ | ~~powerups_d3-cont1~~ | ~~gameplay~~ |
| ~~D-5685~~ | ~~0x004e8e90,0x004e8ea0~~ | ~~drained by powerups_d4-20260508-1823; both analyzed C1; depth-5 in D-10482~~ | ~~powerups_d3-cont1~~ | ~~gameplay~~ |
| ~~D-5686~~ | ~~0x004d8bd0 FUN_004d8bd0~~ | ~~drained by powerups_d4-20260508-1823; analyzed C1; depth-5 in D-10483~~ | ~~powerups_d3-cont1~~ | ~~gameplay~~ |
| D-10480 | 0x004e8090,0x00535330 | depth-5 callees of FUN_00534d00; S-3520 S-3521; from session powerups_d4-20260508-1823 | powerups_d4-cont1 | gameplay |
| D-10481 | 0x004d8090 FUN_004d8090 | depth-5 callee of FUN_004c0910; list notify on clone; called (&DAT_00617f78 new_node original_node); S-3522; from session powerups_d4-20260508-1823 | powerups_d4-cont1 | gameplay |
| D-10482 | 0x004f0c10,0x004f3b60 | depth-5 callees of FUN_004e8ea0; S-3523 S-3524; from session powerups_d4-20260508-1823 | powerups_d4-cont1 | gameplay |
| D-10483 | 0x004e2ff0 FUN_004e2ff0 | depth-5 callee of FUN_004d8bd0; particle descriptor allocator-free; S-3525; from session powerups_d4-20260508-1823 | powerups_d4-cont1 | gameplay |
| D-2743 | 0x004282a0 FUN_004282a0 | analyzed C1 session hud_frontend_d3-20260506-0511; text width/size getter; S-2089 S-2083 S-2084 | 2026-05-06 |
| D-2744 | 0x00427ad0 FUN_00427ad0 | analyzed C1 session hud_frontend_d3-20260506-0511; 7-param icon draw wrapper; S-2083..S-2088 depth-4 filed | 2026-05-06 |
| D-2753 | 0x0040bb70 FUN_0040bb70 | analyzed C1 session hud_frontend_d3-20260506-0511; thin wrapper: FUN_004c5c00(DAT_0063b900, param_1); S-2081 | 2026-05-06 |
| D-2754 | 0x0040bb90 FUN_0040bb90 | analyzed C1 session hud_frontend_d3-20260506-0511; thin wrapper: FUN_004c5c00(DAT_0063b904, param_1); S-2081 | 2026-05-06 |
| D-2755 | 0x0040b620 FUN_0040b620 | analyzed C1 session hud_frontend_d3-20260506-0511; 4-element descending bubble-sort (permutation array); U-2089 | 2026-05-06 |
| D-2756 | 0x0040b460 FUN_0040b460 | analyzed C1 session hud_frontend_d3-20260506-0511; 4-element bubble-sort + mode override; U-2087 U-2088; S-2080 | 2026-05-06 |
| D-2757 | 0x0040e3a0 FUN_0040e3a0 | analyzed C1 session hud_frontend_d3-20260506-0511; 6-entry RGBA color table by selector; S-2082 | 2026-05-06 |
| D-2761 | 0x00429870 FUN_00429870 | analyzed C1 session hud_frontend_d3-20260506-0511; two composite time-record comparison; U-2094 | 2026-05-06 |
| D-2762 | 0x00429a30 FUN_00429a30 | analyzed C1 session hud_frontend_d3-20260506-0511; writes time-record A into 3 arrays by FUN_00430790 index; U-2095; S-2091 | 2026-05-06 |
| D-2763 | 0x00429a80 FUN_00429a80 | analyzed C1 session hud_frontend_d3-20260506-0511; indexed read from DAT_0067d98c array | 2026-05-06 |
| D-2764 | 0x00429a90 FUN_00429a90 | analyzed C1 session hud_frontend_d3-20260506-0511; indexed read from DAT_0067d994 array | 2026-05-06 |
| D-2765 | 0x00429a70 FUN_00429a70 | analyzed C1 session hud_frontend_d3-20260506-0511; indexed float read from DAT_0067d99c array | 2026-05-06 |
| D-2767 | 0x00428320 FUN_00428320 | analyzed C1 session hud_frontend_d3-20260506-0511; text width measurement variant B; U-2093 U-2092; S-2090 S-2089 | 2026-05-06 |
| D-2768 | 0x0040b7a0 FUN_0040b7a0 | analyzed C1 session hud_frontend_d3-20260506-0511; global read DAT_0063b8ec | 2026-05-06 |
| D-2769 | 0x0040b7b0 FUN_0040b7b0 | analyzed C1 session hud_frontend_d3-20260506-0511; 4-case selector array dispatch; U-2090 | 2026-05-06 |
| D-2770 | 0x0040b6b0 FUN_0040b6b0 | analyzed C1 session hud_frontend_d3-20260506-0511; indexed read from DAT_008a9530 array | 2026-05-06 |
| D-2771 | 0x0040b6c0 FUN_0040b6c0 | analyzed C1 session hud_frontend_d3-20260506-0511; indexed read from DAT_008a94f0 array | 2026-05-06 |
| D-2773 | 0x0040ad20 FUN_0040ad20 | analyzed C1 session hud_frontend_d3-20260506-0511; global read DAT_008a95ac | 2026-05-06 |
| D-2740 | 0x00430760 FUN_00430760 | not analyzed; early-finish hud_frontend_d3; re-filed as D-6172 for hud_frontend_d3-cont1 | 2026-05-06 |
| D-2741 | 0x0042fab0 FUN_0042fab0 | not analyzed; early-finish hud_frontend_d3; re-filed as D-6171 | 2026-05-06 |
| D-2742 | 0x0042bcb0 FUN_0042bcb0 | not analyzed; early-finish; re-filed as D-6163 | 2026-05-06 |
| D-2745 | 0x0042f8d0 FUN_0042f8d0 | not analyzed; early-finish; re-filed as D-6170 | 2026-05-06 |
| D-2746 | 0x0042ac00 FUN_0042ac00 | not analyzed; early-finish; re-filed as D-6161 | 2026-05-06 |
| D-2747 | 0x00473870 FUN_00473870 | not analyzed; early-finish; re-filed as D-6182 | 2026-05-06 |
| D-2748 | 0x004368e0 FUN_004368e0 | not analyzed; early-finish; re-filed as D-6178 | 2026-05-06 |
| D-2749 | 0x0042ac50 FUN_0042ac50 | not analyzed; early-finish; re-filed as D-6162 | 2026-05-06 |
| D-2750 | 0x0042a940 FUN_0042a940 | not analyzed; early-finish; re-filed as D-6160 | 2026-05-06 |
| D-2751 | 0x00430830 FUN_00430830 | not analyzed; early-finish; re-filed as D-6173 | 2026-05-06 |
| D-2752 | 0x00458630 FUN_00458630 | not analyzed; early-finish; re-filed as D-6181 | 2026-05-06 |
| D-2758 | 0x00430b30 FUN_00430b30 | not analyzed; early-finish; re-filed as D-6177 | 2026-05-06 |
| D-2759 | 0x0042d290 FUN_0042d290 | not analyzed; early-finish; re-filed as D-6164 | 2026-05-06 |
| D-2760 | 0x0042d300 FUN_0042d300 | not analyzed; early-finish; re-filed as D-6165 | 2026-05-06 |
| D-2766 | 0x004736c0 FUN_004736c0 | not analyzed; early-finish; re-filed as D-6183 | 2026-05-06 |
| D-2772 | 0x00474e60 FUN_00474e60 | not analyzed; early-finish; re-filed as D-6184 | 2026-05-06 |
| D-2774 | 0x00436810 FUN_00436810 | not analyzed; early-finish; re-filed as D-6179 | 2026-05-06 |
| D-2775 | 0x0042ebe0 FUN_0042ebe0 | not analyzed; early-finish; re-filed as D-6166 | 2026-05-06 |
| D-2776 | 0x0042ee40 FUN_0042ee40 | not analyzed; early-finish; re-filed as D-6168 | 2026-05-06 |
| D-2777 | 0x004391b0 FUN_004391b0 | not analyzed; early-finish; re-filed as D-6180 | 2026-05-06 |
| D-2778 | 0x0042ef40 FUN_0042ef40 | not analyzed; early-finish; re-filed as D-6169 | 2026-05-06 |
| D-2779 | 0x00430a10 FUN_00430a10 | not analyzed; early-finish; re-filed as D-6174 | 2026-05-06 |
| D-2780 | 0x00430a60 FUN_00430a60 | not analyzed; early-finish; re-filed as D-6175 | 2026-05-06 |
| D-2781 | 0x00430ab0 FUN_00430ab0 | not analyzed; early-finish; re-filed as D-6176 | 2026-05-06 |
| D-2782 | 0x0042ee00 FUN_0042ee00 | not analyzed; early-finish; re-filed as D-6167 | 2026-05-06 |
| D-6700 | 0x004295a0 FUN_004295a0 | caller of FUN_0040dc80 in name-label render cluster; body 004295a0..0042961c (0x7c B); bucket localization_d2-cont1; pickup condition: localization_d2-cont1 session | 2026-05-06 |
| D-6701 | 0x00429bd0 FUN_00429bd0 | caller of FUN_0040dc80 in name-label render cluster; body 00429bd0..00429e0e (0x23e B); bucket localization_d2-cont1; pickup condition: localization_d2-cont1 session | 2026-05-06 |
| D-6702 | 0x00424eb0 FUN_00424eb0 | caller of FUN_0040dc90; body 00424eb0..004252be (0x40e B); pickup condition: localization_d2-cont1 session | 2026-05-06 |
| D-6703 | 0x00442440 FUN_00442440 | caller of FUN_0040dc90; body 00442440..004425c6 (0x186 B); HUD/name-label context; pickup condition: localization_d2-cont1 session | 2026-05-06 |
| D-6880 | 0x00550980 FUN_00550980 | depth-3 callee of FUN_004cbe80 (save_gamesave_d2); 4-arg fwrite-style; S-2320; bucket save_gamesave_d2-cont1; pickup condition: save_gamesave_d2-cont1 session | 2026-05-06 |

| D-7060 | 0x0040e170 FUN_0040e170 | depth-1 callee of FUN_0043df00 (options_menu); 9 bytes; S-2380; pickup condition: options_menu-cont1 session | 2026-05-06 || D-7120 | 0x004b7a70 FUN_004b7a70 | depth-3 callee of 0x0047b8a0 (input_lua_d2); S-2400; only callee of exec wrapper; pick up as input_lua_d2-cont1 | input_lua_d2-cont1 session | input |
| D-7121 | 0x004ba1b0 FUN_004ba1b0 | depth-3 callee of 0x004b7330 and 0x004b7480 (input_lua_d2); S-2401; allocator (args 0,0,0x70 or varied); pick up as input_lua_d2-cont1 | input_lua_d2-cont1 session | input |
| D-7122 | 0x004b7be0 FUN_004b7be0 | depth-3 callee of 0x004b7330 (input_lua_d2); S-2402; receives (block, &label, &stack); fail→FUN_004b7480; pick up as input_lua_d2-cont1 | input_lua_d2-cont1 session | input |
| D-7123 | 0x004c06c0 FUN_004c06c0 | depth-3 callee of 0x004c0510 (input_lua_d2); S-2403; args (param_1, &PTR_PTR_005d8a70, 0x17); pick up as input_lua_d2-cont1 | input_lua_d2-cont1 session | input |
| D-7124 | 0x004b7200 thunk_FUN_004b7fd0 | depth-3 callee of 0x004c0510 (input_lua_d2); S-2404; thunk to FUN_004b7fd0; args (param_1, &LAB_004c0560, 0); pick up as input_lua_d2-cont1 | input_lua_d2-cont1 session | input |
| D-7125 | 0x004b9730 FUN_004b9730 | depth-3 callee of 0x004c0510 (input_lua_d2); S-2405; args (param_1, 2, &PTR_DAT_00617530); pick up as input_lua_d2-cont1 | input_lua_d2-cont1 session | input |
| D-7126 | 0x004b7140 FUN_004b7140 | depth-3 callee of 0x004c0510 (input_lua_d2); S-2406; args (param_1, 0x54442d18, 0x400921fb); pick up as input_lua_d2-cont1 | input_lua_d2-cont1 session | input |
| D-7127 | 0x004b7250 FUN_004b7250 | depth-3 callee of 0x004c0510 (input_lua_d2); S-2407; args (param_1, &DAT_00617f34); pick up as input_lua_d2-cont1 | input_lua_d2-cont1 session | input |
| D-7128 | 0x004ba210 FUN_004ba210 | depth-3 callee of 0x004b7480 (input_lua_d2); S-2408; args (param_1, 1); pick up as input_lua_d2-cont1 | input_lua_d2-cont1 session | input |
| D-7129 | 0x004b9850 FUN_004b9850 | depth-3 callee of 0x004b7480 (input_lua_d2); S-2409; args (param_1); pick up as input_lua_d2-cont1 | input_lua_d2-cont1 session | input |
| D-7130 | 0x004b64e0 FUN_004b64e0 | depth-3 callee of 0x004b6520 (input_lua_d2); S-2410; receives (param_1, 0, param_2); 57 bytes; pick up as input_lua_d2-cont1 | input_lua_d2-cont1 session | input |
| D-7300 | 0x00428450 FUN_00428450 | depth-2 callee of FUN_00428a30 and FUN_00428d30 (title_screen); ticker/overlay; args (0x20,0xffffffe0) in title vs (0x10,0x100) in lobby; S-2460; bucket title_screen-cont1 | title_screen-cont1 session | frontend |
| D-7301 | 0x004288a0 FUN_004288a0 | depth-2 callee of FUN_00428a30 (title_screen); dark/blank screen renderer when DAT_0067d84c==0; S-2461; bucket title_screen-cont1 | title_screen-cont1 session | frontend |
| D-7302 | 0x00428320 FUN_00428320 | depth-2 callee of FUN_00428a30 and FUN_00428bf0 (title_screen); text renderer; renders build date + string 0x222; S-2462; bucket title_screen-cont1 | title_screen-cont1 session | frontend |
| D-7303 | 0x0042e590 FUN_0042e590 | depth-2 callee of FUN_00428bf0 (title_screen attract renderer); sprite draw at (320.0, 260.0); sprite table structure unknown; S-2463; bucket title_screen-cont1 | title_screen-cont1 session | frontend |
| D-7304 | 0x0040d250 FUN_0040d250 | depth-2 callee of FUN_00428d30 (title_screen lobby renderer); lobby visual element (gradient/line/logo); S-2464; bucket title_screen-cont1 | title_screen-cont1 session | frontend |
| D-7305 | 0x00401ee0 FUN_00401ee0 | depth-2 callee of FUN_00428d30 (title_screen lobby renderer); lobby visual element; S-2465; bucket title_screen-cont1 | title_screen-cont1 session | frontend |
| D-7306 | 0x0042f0b0 FUN_0042f0b0 | depth-2 callee of FUN_00428d30 (title_screen lobby renderer); UI sub-renderer at (347.5, 168.0); S-2466; bucket title_screen-cont1 | title_screen-cont1 session | frontend |
| D-7600 | 0x005a7460 FUN_005a7460 | depth-4 callee of FUN_005a7520 (audio_music_d3 mode-1 branch); 0xb8 bytes; S-2560; bucket audio_music_d3-cont1 | audio_music_d3 session | audio |
| D-7601 | 0x005a7560 FUN_005a7560 | depth-4 callee of FUN_005a7520 (audio_music_d3 mode-0 branch); 0x4d bytes; S-2561; bucket audio_music_d3-cont1 | audio_music_d3 session | audio |
| D-7602 | 0x005a75b0 FUN_005a75b0 | depth-4 callee of FUN_005a7520 (audio_music_d3 mode-2 branch, bit-0@param_1+0xc gated); 0x250 bytes; S-2562; bucket audio_music_d3-cont1 | audio_music_d3 session | audio |
| D-7660 | 0x00495ee0 LAB_00495ee0 | EnumDevices callback; Ghidra sees LAB only — function body not defined; first instruction MOV EAX,[ESP+8] (U-2299); needs function_create on a read-write slot before analysis | acquire RW slot; function_create 0x00495ee0; then decomp | input |
| D-7780 | 0x0046f6c0 FUN_0046f6c0 | post-integration substep fn; 3580 bytes; called as FUN_0046f6c0(1) after each substep in FUN_004709a0; callee of VehicleCollisionBroadPhase outer loop; S-2620; bucket vehicle_dynamics_d2-cont1 | vehicle_dynamics_d2-cont1 | vehicle |
| D-7781 | 0x00469aa0 FUN_00469aa0 | collision condition check; 847 bytes; returns int used to break substep loop in FUN_004709a0; S-2621; bucket vehicle_dynamics_d2-cont1 | vehicle_dynamics_d2-cont1 | vehicle |
| D-7782 | 0x00469df0 FUN_00469df0 | collision pair detection/resolution; 5062 bytes; called when vehicles within proximity radius; args (veh_idx, substep_count); S-2622; bucket vehicle_dynamics_d2-cont1 | vehicle_dynamics_d2-cont1 | vehicle |
| D-7783 | 0x004c39b0 FUN_004c39b0 | vector normalize; 270 bytes; callee of FUN_0046ef70 (wheel contact resolver); normalizes secondary contact normal and lateral dir; S-2623; bucket vehicle_dynamics_d2-cont1 | vehicle_dynamics_d2-cont1 | render |
| D-7784 | 0x00413c70 FUN_00413c70 | 4-channel effect/audio trigger; 58 bytes; called as FUN_00413c70(i,3,0) x4 from FUN_00467300 (collision win trigger); channel=0..3 code=3; S-2624; bucket vehicle_dynamics_d2-cont1 | vehicle_dynamics_d2-cont1 | audio |
| D-7540 | 0x004c52f0 FUN_004c52f0 | depth-4 callee of FUN_004c1480 (S-2543); called as FUN_004c52f0(param_1+0x10, param_2, param_3); purpose unknown; likely RW matrix or frame operation; size unknown; bucket title_screen_d2-cont1 | title_screen_d2 session | render |
| D-9340 | 0x00412f30 FUN_00412f30 | Race::Tick depth-2 callee; arg from FUN_00467210(0); ~1744b; exceeds 1500b session cap; C0 listing-level plate at re/analysis/leaderboard_d3/0x00412f30.md | leaderboard_d4-cont1 session | util |
| D-7360 | 0x004cc230,0x004cc5e0,0x004e99b0,0x004cc160 | depth-5 of track_loader_d4: stream open/chunk-reader/processor/close layer (D-5740 D-5742 D-5743 D-5741 callers); S-2480..S-2483; bucket track_loader_d5/rw_stream_layer | track_loader_d4-20260506-2007 session done | render |
| D-7361 | 0x004cbd30,0x0055deb0,0x0055dec0,0x005c4ad0,0x005c4bb0,0x005c4d20,0x005c4d50,0x005c4df0,0x005c4e10 | depth-5 of track_loader_d4: UVAnim support layer (D-5741 callees); S-2484 S-2487..S-2494; bucket track_loader_d5/uvanim_support | track_loader_d4-20260506-2007 session done | render |
| D-7362 | 0x00545260,0x0052daf0 | depth-5 of track_loader_d4: spline/anim data processors (D-5742 D-5743 callees); S-2495 S-2496; bucket track_loader_d5/stream_processors | track_loader_d4-20260506-2007 session done | render |
| D-7363 | 0x00543dc0,0x00543da0,0x00543e30,0x004e5c70,0x004b4550,0x004e66d0,0x00552020,0x00545340 | depth-5 of track_loader_d4: post-load/callback/attribute layer (D-5744 D-5745 D-5746 D-5753 D-5755 callees); S-2497..S-2499 + range-exhausted; bucket track_loader_d5/postload_callbacks | track_loader_d4-20260506-2007 session done | render |
| D-7364 | 0x00562580,0x0057c4b0,0x005625e0,0x0055dc70,0x00562520,0x0055af40 | depth-5 of track_loader_d4: physics world init sub-layer (D-5747 callees); bucket track_loader_d5/physics_world_init | track_loader_d4-20260506-2007 session done | render |
| D-7365 | 0x0057c210,0x00559ee0,0x00559c40,0x004b5240 | depth-5 of track_loader_d4: physics body ops (D-5748 D-5749 callees); bucket track_loader_d5/physics_body_ops | track_loader_d4-20260506-2007 session done | render |
| D-7366 | 0x004260b0,0x004c0ed0,0x004c4dc0,0x00547230,0x00538c80,0x0045c110,0x0045c030,0x004726f0,0x004c39b0,0x004c45f0,0x004c3df0,0x004a3384,0x004c5010,0x004c51a0 | depth-5 of track_loader_d4: broadphase registration callees (D-5750 all 14); bucket track_loader_d5/broadphase_ops | track_loader_d4-20260506-2007 session done | render |
| D-7367 | 0x004c3b30,0x0047bc90 | depth-5 of track_loader_d4: triangle mesh sub-layer (D-5751 D-5752 callees); FUN_004c3b30=sqrtf-like; FUN_0047bc90=half-edge matcher; bucket track_loader_d5/tri_mesh_ops | track_loader_d4-20260506-2007 session done | render |
| D-7840 | 0x0047b860 FUN_0047b860 | depth-2 callee of 0047b9b0 (physics_collision_d2); 21 bytes; pre-setup; S-2640 | physics_collision_d2-cont1 | physics |
| D-7841 | 0x0047b8d0 FUN_0047b8d0 | depth-2 callee of 0047b9b0 (physics_collision_d2); 167 bytes; main Lua executor body; S-2641; U-2648 | physics_collision_d2-cont1 | physics |
| D-7842 | 0x0047b880 FUN_0047b880 | depth-2 callee of 0047b9b0 (physics_collision_d2); 24 bytes; teardown; S-2642 | physics_collision_d2-cont1 | physics |
| D-7843 | 0x0047ce40 FUN_0047ce40 | depth-2 callee of 004715a0 (physics_collision_d2); 36 bytes; volume index→handle mapper; S-2643; U-2650 | physics_collision_d2-cont1 | physics |
| D-7961 | 0x0047d3c0 FUN_0047d3c0 | VehiclePhysicsWorldStep callee; RWP37 physics world step; args (world, dt); ~large; S-2626 S-2627; vehicle_update_d3-cont1 | vehicle_update_d3 session | vehicle |
| D-7962 | 0x0046c5f0 FUN_0046c5f0 | wheel state sync; no visible params; called from VehicleWheelForceIntegrator + VehicleWheelContactSolver + VehicleWheelDrivetrainUpdate; S-2625; vehicle_update_d3-cont1 | vehicle_update_d3 session | vehicle |
| D-7963 | 0x00442ce0 FUN_00442ce0 + 0x00442c80 FUN_00442c80 + 0x0046cb30 FUN_0046cb30 | VehicleWheelForceIntegrator callees: speed modifier (S-2631) + crowding bool (S-2632) + body position getter (S-2628); vehicle_update_d3-cont1 | vehicle_update_d3 session | vehicle |
| D-7964 | 0x004c52f0 FUN_004c52f0 | RW matrix rotation setter; args (matrix, rot_matrix, 2); 145b callee of VehicleWheelContactSolver; S-2543 also references it; S-2629; vehicle_update_d3-cont1 | vehicle_update_d3 session | render |
| D-7965 | 0x0046d4d0 FUN_0046d4d0 | apply matrix to vehicle rigid body; args (vehicleIdx, matrix_ptr); callee of VehiclePhysicsWorldStep; S-2630; vehicle_update_d3-cont1 | vehicle_update_d3 session | vehicle |
| D-7966 | 0x004a3384 FUN_004a3384 + 0x0046cc40 FUN_0046cc40 | acos/atan FP approx (S-2633) + wheel contact sub-helper (S-2634); callees of VehicleAeroStabilizer+VehicleWheelContactSolver; vehicle_update_d3-cont1 | vehicle_update_d3 session | vehicle |
| D-7900 | 0x004c5800 FUN_004c5800 | RwTexDictionarySetCurrent candidate; called (NULL) to clear then (saved_ptr) to restore in texture stream reader; 25 bytes | texture_loader_d3-cont1 session | render |
| D-7901 | 0x004c5820 FUN_004c5820 | RwTexDictionaryGetCurrent candidate; getter returns current TXD ptr; result saved before stream read; 15 bytes | texture_loader_d3-cont1 session | render |
| D-7902 | 0x004c5830 FUN_004c5830 | texture context setter (RwTextureSetCurrent candidate); called with NULL/saved_ptr; 25 bytes | texture_loader_d3-cont1 session | render |
| D-7903 | 0x004c5850 FUN_004c5850 | texture context getter; result saved for restore; 15 bytes | texture_loader_d3-cont1 session | render |
| D-7904 | 0x004c5a00 FUN_004c5a00 | RwTexture create-from-stream main step; called after context save in FUN_00550130; 90 bytes | texture_loader_d3-cont1 session | render |
| D-7905 | 0x004c5ae0 FUN_004c5ae0 | RwTexture post-process step 1; called after FUN_004c5a00 in FUN_00550130; 109 bytes; purpose unknown | texture_loader_d3-cont1 session | render |
| D-7906 | 0x004c5b50 FUN_004c5b50 | RwTexture post-process step 2; called after FUN_004c5ae0 in FUN_00550130; 109 bytes; purpose unknown | texture_loader_d3-cont1 session | render |
| D-7907 | 0x004d8810 FUN_004d8810 | state check called twice in FUN_00550130; non-zero required; 608 bytes; probably subsystem-ready guard | texture_loader_d3-cont1 session | render |
| D-7908 | 0x004e1df0 FUN_004e1df0 | error path cleanup in FUN_00550130 on texture create failure; 112 bytes | texture_loader_d3-cont1 session | render |
| D-7909 | 0x004cdd60 FUN_004cdd60 | RwImageAllocatePixels candidate; stride+alloc+palette setup; 188 bytes; now in hooks.csv C1; depth-4 decomp to confirm alloc type | texture_loader_d3-cont1 session | render |
| D-8260 | 0x0045cd30 FUN_0045cd30 | caller of FUN_004b44f0 (RandomInt) ×5 call sites; 562 bytes (0x0045cd30–0x0045cf62); not in hooks.csv | random_rng_d2 | audio |
| D-8261 | 0x0045d330 FUN_0045d330 | caller of FUN_004b44f0 (RandomInt) ×1; 102 bytes (0x0045d330–0x0045d396); not in hooks.csv | random_rng_d2 | audio |
| D-8262 | 0x00421d20 FUN_00421d20 | caller of FUN_004b4510 (RandomFloat) ×11 call sites; 938 bytes (0x00421d20–0x004220ca); not in hooks.csv | random_rng_d2 | util |
| D-8263 | 0x004219f0 FUN_004219f0 | caller of FUN_004b4510 (RandomFloat) ×1; 154 bytes (0x004219f0–0x00421a8a); not in hooks.csv | random_rng_d2 | util |
| D-8264 | 0x0044e0a0 FUN_0044e0a0 | caller of FUN_004b4510 (RandomFloat) ×1; 238 bytes (0x0044e0a0–0x0044e18e); not in hooks.csv | random_rng_d2 | util |
| D-8265 | 0x004762c0 FUN_004762c0 | caller of FUN_004b4510 (RandomFloat) ×2 call sites; 92 bytes (0x004762c0–0x0047631c); not in hooks.csv | random_rng_d2 | util |
| D-8266 | 0x004764f0 FUN_004764f0 | caller of FUN_004b4510 (RandomFloat) ×5 call sites; 346 bytes (0x004764f0–0x0047664a); not in hooks.csv | random_rng_d2 | util |
| D-7910 | loading_screen_d2 bucket | depth-2 pass has no work: parent session (2026-05-03) closed with all callees (0x00402fb0, 0x00428760) already at C1; no stub/uncertainty/deferred IDs in range D-8200..8259 / U-2767..2786 / S-2760..2779 | re-open if a new caller of 0x00403050 or 0x00403160 surfaces with un-mapped callees | frontend |
| D-8500 | 0x00419760 FUN_00419760 | thunkee of 0x004189f0; 112 bytes; not in hooks.csv; called with (car_idx,1) from FUN_00422fd0 via thunk; S-2860 | pick up as bucket race_results_d2-cont1; same depth; no further recursion | frontend |
| D-8501 | 0x00420de0 FUN_00420de0 | sole callee of 0x004215c0; receives param_2 (50.0f or 0/1); 30 bytes; not in hooks.csv; S-2861 | pick up as bucket race_results_d2-cont1; same depth; no further recursion | frontend |
| D-8560 | 0x004d8430,0x004d8470,0x004c3e90,0x004c3e20,0x004d9030,0x004d9040,0x004c4470,0x004c4430,0x004c07b0,0x004c0830,0x004cbca0,0x004cbd00,0x004c1980,0x004c1940,0x004cd900,0x004cdb60,0x004c78e0,0x004c78a0,0x004c5f60,0x004c5e00,0x004d8530,0x004d8550,0x004d8fa0,0x004d9000,0x004cd850,0x004cd810,0x004d8a80,0x004d8b70 from session librw_plugin_compat-20260507-1950 bucket librw_plugin_compat — pick up as bucket librw_plugin_compat-cont1; decompile all 28 callback bodies (14 ctor + 14 dtor) to derive Mashed module struct layouts; same depth; no further recursion | render |
| D-8689 | 0x00405460 FUN_00405460 | camera spline step; 222 bytes; callee of FUN_004102f0; new callee FUN_00404fa0 (spline matrix eval, 1068b); S-2929 | game_state_d5-cont1 | util |
| D-8690 | 0x0040e590 FUN_0040e590 | race setup/car-placement coordinator; 2360 bytes; callee of FUN_004102f0; too large for this pass; calls FUN_0046baa0 and many others; S-2930 | game_state_d5-cont1 | util |
| ~~D-8689~~ | ~~0x00405460 FUN_00405460~~ | RESOLVED game_state_d5-cont1-20260508-0324: analyzed as C1; re/analysis/game_state_d5-cont1/0x00405460.md | — | util |
| ~~D-8690~~ | ~~0x0040e590 FUN_0040e590~~ | RESOLVED game_state_d5-cont1-20260508-0324: analyzed as C1; re/analysis/game_state_d5-cont1/0x0040e590.md | — | util |
| D-8800 | 0x00404fa0 FUN_00404fa0 | spline matrix eval; 1068 bytes; 3 args (64b out-buf, spline-obj ptr, float t); callee of FUN_00405460; S-2960 | pick up as bucket game_state_d5-cont2; same depth; no further recursion | util |
| D-8801 | 0x00408b00 FUN_00408b00 | grid-pos lookup; args (pos-handle, start-idx, player-count, &local_34, &local_40); 0x1D2 bytes; callee of FUN_0040e590; S-2961 | pick up as bucket game_state_d5-cont2; same depth; no further recursion | util |
| D-8802 | 0x00409290 FUN_00409290 | writes starting-pos handle to &DAT_0063ba80; 0x6D bytes; callee of FUN_0040e590; S-2962 | pick up as bucket game_state_d5-cont2; same depth; no further recursion | util |
| D-8803 | 0x0040b250 FUN_0040b250 | no-arg pre-placement call; 0x33 bytes; callee of FUN_0040e590; S-2963 | pick up as bucket game_state_d5-cont2; same depth; no further recursion | util |
| D-8804 | 0x0040b410 FUN_0040b410 | player readiness code getter; arg (player_idx); returns uint (0/1/2/0xffffffff/0xfffffffe); 0xB bytes; callee of FUN_0040e590; S-2964 | pick up as bucket game_state_d5-cont2; same depth; no further recursion | util |
| D-8805 | 0x0041ede0 FUN_0041ede0 | args (player_idx,0,0); called in zeroing loop if slot active; 0x6F bytes; callee of FUN_0040e590; S-2965 | pick up as bucket game_state_d5-cont2; same depth; no further recursion | util |
| D-8806 | 0x0041ee50 FUN_0041ee50 | args (player_idx,0,0); called after S-2965 in zeroing loop; 0x58 bytes; callee of FUN_0040e590; S-2966 | pick up as bucket game_state_d5-cont2; same depth; no further recursion | util |
| D-8807 | 0x0041ef80 FUN_0041ef80 | args (player_idx,0); first call in zeroing loop; 0x36 bytes; callee of FUN_0040e590; S-2967 | pick up as bucket game_state_d5-cont2; same depth; no further recursion | util |
| D-8808 | 0x0041f000 FUN_0041f000 | args (player_idx, score-array ptr); 0x24 bytes; callee of FUN_0040e590; S-2968 | pick up as bucket game_state_d5-cont2; same depth; no further recursion | util |
| D-8809 | 0x00429820 FUN_00429820 | no-arg; first call after mod-12 counter; 0x14 bytes; callee of FUN_0040e590; S-2969 | pick up as bucket game_state_d5-cont2; same depth; no further recursion | util |
| D-8810 | 0x0046b1c0 FUN_0046b1c0 | args (player_idx, score-array ptr); 0x329 bytes; callee of FUN_0040e590; S-2970 | pick up as bucket game_state_d5-cont2; same depth; no further recursion | util |
| D-8811 | 0x0046b540 FUN_0046b540 | args (player_idx); called after FUN_0046b1c0; 0x157 bytes; callee of FUN_0040e590; S-2971 | pick up as bucket game_state_d5-cont2; same depth; no further recursion | util |
| D-8812 | 0x0046c6d0 FUN_0046c6d0 | args (player_idx, &out_score); writes score to out ptr; 0x23 bytes; callee of FUN_0040e590; S-2972 | pick up as bucket game_state_d5-cont2; same depth; no further recursion | util |
| D-8813 | 0x004704c0 FUN_004704c0 | args (player_idx, local_34, local_30, local_2c[0], local_40, &DAT_00803324); 6-arg vehicle-placement call; 0x1AA bytes; callee of FUN_0040e590; S-2973 | pick up as bucket game_state_d5-cont2; same depth; no further recursion | util |
| D-8814 | 0x0048f680 FUN_0048f680 | no-arg; called after FUN_00429820; 0x20 bytes; callee of FUN_0040e590; S-2974 | pick up as bucket game_state_d5-cont2; same depth; no further recursion | util |
| D-8815 | 0x0048f740 FUN_0048f740 | no-arg; called after FUN_0048f680; 0x30 bytes; callee of FUN_0040e590; S-2975 | pick up as bucket game_state_d5-cont2; same depth; no further recursion | util |
| D-5020 | 0x004e4320 FUN_004e4320 | analyzed C1 session render_frame_d4-20260508-0332 | 2026-05-08 |
| D-5021 | 0x004e4350 FUN_004e4350 | analyzed C1 session render_frame_d4-20260508-0332 | 2026-05-08 |
| D-5022 | 0x0041ea10 FUN_0041ea10 | analyzed C1 session render_frame_d4-20260508-0332 | 2026-05-08 |
| D-5023 | 0x0041e8f0 FUN_0041e8f0 | analyzed C1 session render_frame_d4-20260508-0332; U-3128 filed | 2026-05-08 |
| D-5024 | 0x00401f10 FUN_00401f10 | analyzed C1 session render_frame_d4-20260508-0332; S-3120 S-3121 | 2026-05-08 |
| D-5025 | 0x00403d30 FUN_00403d30 | analyzed C1 session render_frame_d4-20260508-0332 | 2026-05-08 |
| D-5026 | 0x00403db0 FUN_00403db0 | analyzed C1 session render_frame_d4-20260508-0332 | 2026-05-08 |
| D-5027 | 0x00403ed0 FUN_00403ed0 | analyzed C1 session render_frame_d4-20260508-0332 | 2026-05-08 |
| D-5028 | 0x00403fa0 FUN_00403fa0 | analyzed C1 session render_frame_d4-20260508-0332; S-3122; U-3130 U-3131 | 2026-05-08 |
| D-5029 | 0x004041c0 FUN_004041c0 | analyzed C1 session render_frame_d4-20260508-0332; U-3132 | 2026-05-08 |
| D-5030 | 0x0042c010 FUN_0042c010 | analyzed C1 session render_frame_d4-20260508-0332; S-3123 | 2026-05-08 |
| D-5031 | 0x0042c090 FUN_0042c090 | analyzed C1 session render_frame_d4-20260508-0332; S-3124; U-3134 | 2026-05-08 |
| D-5032 | 0x004278d0 FUN_004278d0 | analyzed C1 session render_frame_d4-20260508-0332; S-3125..S-3128; U-3135 | 2026-05-08 |
| D-5033 | 0x00427990 FUN_00427990 | analyzed C1 session render_frame_d4-20260508-0332 | 2026-05-08 |
| D-5034 | 0x00427be0 FUN_00427be0 | analyzed C1 session render_frame_d4-20260508-0332; S-3129; U-3136 | 2026-05-08 |
| D-5035 | 0x004c7760 FUN_004c7760 | analyzed C1 session render_frame_d4-20260508-0332; U-3137 | 2026-05-08 |
| D-5037 | 0x004725c0 FUN_004725c0 | analyzed C1 session render_frame_d4-20260508-0332; U-3138 | 2026-05-08 |
| D-5039 | 0x0041ebb0 FUN_0041ebb0 | analyzed C1 session render_frame_d4-20260508-0332; S-3130 | 2026-05-08 |
| D-5040 | 0x004219c0 FUN_004219c0 | analyzed C1 session render_frame_d4-20260508-0332; S-3131; U-3139 | 2026-05-08 |
| D-5041 | 0x00425e40 FUN_00425e40 | analyzed C1 session render_frame_d4-20260508-0332; S-3132; U-3140 | 2026-05-08 |
| D-1060 | Cap-split remainder: 0x00480b70 FUN_00480b70 (VehicleWheelParticles) | vehicle_update subset 23 RVAs > Sonnet cap 20; analysis notes in vehicle_update_d2/00480b70.md (U-1414) | vehicle_update-cont1 session; tracker rows from this session needed | vehicle |
| D-1061 | Cap-split remainder: 0x004a2c48 FUN_004a2c48 | vehicle_update subset 23 RVAs > Sonnet cap 20; new function not previously analyzed | vehicle_update-cont1 session | vehicle |
| D-1062 | Cap-split remainder: 0x004c3ac0 FUN_004c3ac0 | vehicle_update subset 23 RVAs > Sonnet cap 20; new function; called from FUN_00470c70 substep loop to read float fields piVar14+0x268 and piVar14+0x26b | vehicle_update-cont1 session | vehicle |
| D-9520 | 0x00462950,0x004671a0,0x004715a0,0x00478660,0x00479330,0x0047a020,0x0047a0f0,0x0047c0b0,0x0047c0f0,0x00480340,0x00491780,0x004924c0,0x00495280,0x004952f0,0x004987b0,0x004c1b10 from session track_loader-20260508-0616 bucket track_loader | pick up as bucket track_loader-cont1; same depth, no further recursion. | 2026-05-08 |
| D-10300 | 0x00402f40 FUN_00402f40 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10301 | 0x004046a0 FUN_004046a0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10302 | 0x00405420 FUN_00405420 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10303 | 0x004073b0 FUN_004073b0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10304 | 0x00407a60 FUN_00407a60 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10305 | 0x0040ad30 FUN_0040ad30 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10306 | 0x0040b180 FUN_0040b180 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10307 | 0x0040bd80 FUN_0040bd80 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10308 | 0x0040dbd0 FUN_0040dbd0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10309 | 0x0040de00 FUN_0040de00 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10310 | 0x00410860 FUN_00410860 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10311 | 0x00414060 FUN_00414060 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10312 | 0x00414180 FUN_00414180 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10313 | 0x00414220 FUN_00414220 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10314 | 0x00418990 FUN_00418990 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10315 | 0x00419760 FUN_00419760 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10316 | 0x0041b4d0 FUN_0041b4d0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10317 | 0x0041b510 FUN_0041b510 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10318 | 0x0041bf20 FUN_0041bf20 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10319 | 0x0041c010 FUN_0041c010 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10320 | 0x0041cb80 FUN_0041cb80 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10321 | 0x0041cbc0 FUN_0041cbc0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10322 | 0x0041d730 FUN_0041d730 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10323 | 0x0041d820 FUN_0041d820 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10324 | 0x0041e130 FUN_0041e130 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10325 | 0x0041eda0 FUN_0041eda0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10326 | 0x0041f000 FUN_0041f000 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10327 | 0x00420d40 FUN_00420d40 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10328 | 0x00422120 FUN_00422120 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10329 | 0x004222c0 FUN_004222c0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10330 | 0x00422b10 FUN_00422b10 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10331 | 0x00425b10 FUN_00425b10 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10332 | 0x00426630 FUN_00426630 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10333 | 0x004266f0 FUN_004266f0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10334 | 0x00426c10 FUN_00426c10 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10335 | 0x00426c30 FUN_00426c30 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10336 | 0x00426c70 FUN_00426c70 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10337 | 0x004292c0 FUN_004292c0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10338 | 0x004292d0 FUN_004292d0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10339 | 0x0042aab0 FUN_0042aab0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10340 | 0x004425d0 FUN_004425d0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10341 | 0x00458bf0 FUN_00458bf0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10342 | 0x00459560 FUN_00459560 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10343 | 0x0045b9d0 FUN_0045b9d0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10344 | 0x0045be90 FUN_0045be90 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10345 | 0x0045bed0 FUN_0045bed0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10346 | 0x0045dc80 FUN_0045dc80 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10347 | 0x0045df70 FUN_0045df70 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10348 | 0x0045dfc0 FUN_0045dfc0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10349 | 0x00462520 FUN_00462520 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10350 | 0x004627b0 FUN_004627b0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10351 | 0x00462ec0 FUN_00462ec0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10352 | 0x004647f0 FUN_004647f0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10353 | 0x004648b0 FUN_004648b0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10354 | 0x0046b1c0 FUN_0046b1c0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10355 | 0x0046b540 FUN_0046b540 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10356 | 0x00471ac0 FUN_00471ac0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10357 | 0x00471cf0 FUN_00471cf0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10358 | 0x004728a0 FUN_004728a0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10359 | 0x00475a60 FUN_00475a60 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont1 session | util |
| D-10360 | 0x00476430 FUN_00476430 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont2 session | util |
| D-10361 | 0x00477730 FUN_00477730 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont2 session | util |
| D-10362 | 0x00477b40 FUN_00477b40 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont2 session | util |
| D-10363 | 0x00484c90 FUN_00484c90 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont2 session | util |
| D-10364 | 0x0048f260 FUN_0048f260 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont2 session | util |
| D-10365 | 0x004904d0 FUN_004904d0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont2 session | util |
| D-10366 | 0x00494f00 FUN_00494f00 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont2 session | util |
| D-10367 | 0x00494f10 FUN_00494f10 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont2 session | util |
| D-10368 | 0x004d8560 FUN_004d8560 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont2 session | util |
| D-10369 | 0x00550be0 FUN_00550be0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont2 session | util |
| D-10370 | 0x0055dec0 FUN_0055dec0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont2 session | util |
| D-10371 | 0x005a60b0 FUN_005a60b0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont2 session | util |
| D-10372 | 0x005a60e0 FUN_005a60e0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont2 session | util |
| D-10373 | 0x005a89a0 FUN_005a89a0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont2 session | util |
| D-10374 | 0x005a89b0 FUN_005a89b0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont2 session | util |
| D-10375 | 0x005a89c0 FUN_005a89c0 | timer_d3 session cap; 76 unmapped callees exceeded 20-slot U-range | timer_d3-cont2 session | util |
| D-10540 | 0x00422570 FUN_00422570 | c0_promotion_render_a session cap; function size 889 bytes exceeds 800-byte threshold | c0_promotion_render_a-cont1 session | render |
| D-10541 | 0x00429e10 FUN_00429e10 | c0_promotion_render_a session cap; function size 1593 bytes exceeds 800-byte threshold | c0_promotion_render_a-cont1 session | render |

| D-10542 | 0x004b6fc0 FUN_004b6fc0 | Lua C API wrapper (arg-count getter); needs input_lua session to establish Lua 5.x C API binding | input_lua session | track |
| D-10543 | 0x004b70d0 FUN_004b70d0 | Lua C API wrapper (string arg getter); needs input_lua session to establish Lua 5.x C API binding | input_lua session | track |
| D-10544 | 0x004b7090 FUN_004b7090 | Lua C API wrapper (number arg getter); needs input_lua session to establish Lua 5.x C API binding | input_lua session | track |
