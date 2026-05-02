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
| D-0700 | 0x004a42c5 FUN_004a42c5 | depth-2 of FUN_004987b0; varargs string formatter; not recursed per session rules; S-0260 | input_dinput-cont1 session | input |
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
| D-0580 | 0x004ccce0 FUN_004ccce0 | depth-3 of 0x004d7ca0 FUN_004d7ca0; called with (DAT_007d6c50, &LAB_004d7d70, DAT_007d6c50); not recursed from rw_engine_teardown_d2 | rw_engine_teardown_d3 or dedicated session | render |
| D-0581 | 0x004cc9f0 FUN_004cc9f0 | depth-3 of 0x004d7ca0 FUN_004d7ca0; called with DAT_007d6c50 as single arg; not recursed from rw_engine_teardown_d2 | rw_engine_teardown_d3 or dedicated session | render |
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

## Cleared (delivered or rejected)

| ID | Title | Outcome | Date |
|----|-------|---------|------|
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
| D-0820 | 0x004b7330 FUN_004b7330 + 0x004c0510 FUN_004c0510 + 0x004b7480 FUN_004b7480 + 0x0047b8a0 FUN_0047b8a0 + 0x004b6520 FUN_004b6520 | Lua interpreter internals — possibly vendored Lua 5.x source; pick up only if Mashed needs scriptable joypad remap rebuilt rather than wrapped | input_lua_internals bucket; re-pickup condition: greenfield port phase where Lua scripting must be re-implemented (powerups, KTCScript, course LAP/COURSE data); S-0300..S-0304; U-0307 U-0308 | input |
| D-0640 | 0x004c35f0 FUN_004c35f0 | depth-2 callee of _rwDeviceSystemFn (render_d3d9_device session); identified from switch-body listing scan; not decomped (outside subset cap) | render_d3d9_device-cont1 | render |
| D-0641 | 0x004c40b0 FUN_004c40b0 | depth-2 callee of _rwDeviceSystemFn (render_d3d9_device session); identified from switch-body listing scan; not decomped (outside subset cap) | render_d3d9_device-cont1 | render |
| D-0642 | 0x004cfe20 FUN_004cfe20 | depth-2 callee of _rwDeviceSystemFn (render_d3d9_device session); identified from switch-body listing scan; not decomped (outside subset cap) | render_d3d9_device-cont1 | render |
| D-0643 | 0x004d5bc0 FUN_004d5bc0 | depth-2 callee of _rwDeviceSystemFn (render_d3d9_device session); identified from switch-body listing scan; not decomped (outside subset cap) | render_d3d9_device-cont1 | render |
| D-0644 | 0x004db3e0 FUN_004db3e0 | depth-2 callee of _rwDeviceSystemFn (render_d3d9_device session); identified from switch-body listing scan; not decomped (outside subset cap) | render_d3d9_device-cont1 | render |
| D-0645 | 0x004dcd50 FUN_004dcd50 | depth-2 callee of _rwDeviceSystemFn (render_d3d9_device session); identified from switch-body listing scan; not decomped (outside subset cap) | render_d3d9_device-cont1 | render |
| D-0646 | 0x005cb404 FUN_005cb404 | depth-2 callee of _rwDeviceSystemFn (render_d3d9_device session); identified from switch-body listing scan; not decomped (outside subset cap) | render_d3d9_device-cont1 | render |
| D-0400 | Remaining 116 depth-3 RVAs from D-0040..D-0045 not reached in boot_app_init_d3 session (cap=18): 00412890,0041a1e0,0041b450,0041bec0,0041c100,0041cb10,0041d6e0,0041d8b0,0041db90,0041def0,0041eaa0,00420d00,00425bc0,004274d0,004274e0,004275d0,00427ca0,00428390,004283a0,00431b40,0045b350,0045bae0,004669b0,004671a0,00471eb0,004723d0,0047ba00,00484170,004841d0,004881d0,00494c80,00495280,004952f0,00495350,00496e40,00498bc0,00498bd0,00499ce0,004b3d80,004b6540,004b6560,004c2ed0,004c2f00,004caea0,004d8560,00558240,0041a3d0,0041b660,0041c0e0,0041c2c0,0041ccf0,0041d890,0041da80,0041de70,0041e0d0,0041ffb0,00421590,00425ed0,00426ba0,00426c00,00427620,00428400,0042c2a0,0045b930,00467010,00467020,00467070,0047ba10,00484130,00489250,00494bc0,00494ef0,00494f20,00496ce0,004b4880,004b6550,005581f0,00428590,004921d0,00493710,004926c0,00492770,004929d0,00492d20,00492d30,00492e90,00493480,00499690,00431ae0,00431af0,00431b00,00431b10,00431d00,00495120,004955b0,004960a0,004963b0,00498510,00499710,004c2f60,004c3040,004c3270,00550390,00558470,00498b60,004963d0,00496010,004955c0,00498bf0,00499730,004a2be9,004a3ac9,004b302f,00499890,004c5a60,004d8060 | cap reached in boot_app_init_d3-20260502-1859; note: 0x00412890 missed due to sort error — include as first target | boot subsystem depth-3 sweep session (boot_app_init_d3-cont1) | boot |
| D-0401 | Depth-4 callees encountered as stubs during boot_app_init_d3-20260502-1859 session (S-0160..S-0179, S-0305..S-0372 — 88 stubs total); not recursed into | depth-4; out of scope for depth-3 session | boot subsystem depth-4 sweep session (boot_app_init_d4) | boot |
