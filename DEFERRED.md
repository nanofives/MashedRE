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
| D-1240 | 0x004335f0 FUN_004335f0 | depth-2 callee of FUN_0043bf30 (flag 0x0067e7a8 path); not recursed in hud_frontend-20260502-1944 | hud_frontend-cont1 session | frontend |
| D-1241 | 0x0043a610 FUN_0043a610 | depth-2 callee of FUN_0043bf30 (flag 0x0067e7d8 path); not recursed | hud_frontend-cont1 session | frontend |
| D-1242 | 0x0042f0c0 FUN_0042f0c0 | depth-2 callee of FUN_0043bf30 (flag 0x0067e7b0 path); not recursed | hud_frontend-cont1 session | frontend |
| D-1243 | 0x0043af10 FUN_0043af10 | depth-2 callee of FUN_0043bf30 (flag 0x0067e7f0 path); not recursed | hud_frontend-cont1 session | frontend |
| D-1244 | 0x00434720 FUN_00434720 | depth-2 callee of FUN_0043bf30 (flag 0x0067e7c8 path); not recursed | hud_frontend-cont1 session | frontend |
| D-1245 | 0x00430b90 FUN_00430b90 | depth-2 callee of FUN_0043bf30 (flag 0x0067e7f8 path); not recursed | hud_frontend-cont1 session | frontend |
| D-1246 | 0x00431240 FUN_00431240 | depth-2 callee of FUN_0043bf30 (flag 0x0067e838 path); not recursed | hud_frontend-cont1 session | frontend |
| D-1247 | 0x004314b0 FUN_004314b0 | depth-2 callee of FUN_0043bf30 (flag 0x0067e830 path); not recursed | hud_frontend-cont1 session | frontend |
| D-1248 | 0x00431710 FUN_00431710 | depth-2 callee of FUN_0043bf30 (flag 0x0067e820 path); not recursed | hud_frontend-cont1 session | frontend |
| D-1249 | 0x0043aa30 FUN_0043aa30 | depth-2 callee of FUN_0043bf30 (flag 0x0067e7e0 path); not recursed | hud_frontend-cont1 session | frontend |
| D-1250 | 0x0042fb70 FUN_0042fb70 | depth-2 callee of FUN_0043bf30 (flag 0x0067e7e8 path); not recursed | hud_frontend-cont1 session | frontend |
| D-1251 | 0x0042fe90 FUN_0042fe90 | depth-2 callee of FUN_0043bf30 (flag 0x0067e810 path); not recursed | hud_frontend-cont1 session | frontend |
| D-1252 | 0x00430120 FUN_00430120 | depth-2 callee of FUN_0043bf30 (flag 0x0067e818 path); not recursed | hud_frontend-cont1 session | frontend |
| D-1253 | 0x00439210 FUN_00439210 | depth-2 callee of FUN_0043bf30 (flag 0x0067e7b8 path); not recursed | hud_frontend-cont1 session | frontend |

## Cleared (delivered or rejected)

| ID | Title | Outcome | Date |
|----|-------|---------|------|
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
| D-1000 | 0x0049ec10 FUN_0049ec10 | depth-2 callee of FUN_00493c00 (0x00493c00) and FUN_00494ac0 (0x00494ac0); called with (&DAT_005cfaac, 0, param_2, param_3); not decomped (outside subset) | video_mci-cont1 | util |
| D-1001 | 0x004a3b84 FUN_004a3b84 | depth-2 callee of FUN_00493f00 (0x00493f00); called as vsnprintf-like formatter (buf, 0x3ff, fmt, va_list); not decomped (outside subset) | video_mci-cont1 | util |
| D-1002 | LAB_00493ac0 (0x00493ac0) and LAB_00493b40 (0x00493b40) | code labels set into PTR_FUN_006147dc by FUN_00493b50; return UINT code page; Ghidra did not identify as named functions; decomp requires listing_disassemble or function_create first | video_mci-cont1 | util |
| D-1180 | 0x00426cd0,0x0042a530,0x0042a8d0,0x0042f510,0x00462950,0x004671a0,0x004715a0,0x00478660,0x00479330,0x0047a020,0x0047a0f0,0x0047c0b0,0x0047c0f0,0x00480340,0x00491780,0x004924c0,0x00495280,0x004952f0,0x004987b0,0x004c1b10 from session track_loader-20260502-1943 bucket track_loader — pick up as bucket track_loader-cont1; 4 from declared subset cap-stop + 16 from pre-split; same depth, no further recursion | track_loader-cont1 | render |
| D-1300 | 14 depth-2 callees of FUN_004548e0 (DepthCharge init): 0x0042a5d0,0x004548a0,0x004b3f90,0x004b3fc0,0x004b5190,0x004b5240,0x004b6520,0x004c0b30,0x004c1040,0x004c39b0,0x004c5c80,0x004e69a0,0x004e6ab0,0x004e7e30 | depth-2; not recursed per session rules | powerups-cont1; same depth, no further recursion | vehicle |
| D-1301 | 10 depth-2 callees of FUN_00456760 (GatlingGun init): 0x0040bb30,0x0042a5d0,0x00474d60,0x00476cb0,0x004770c0,0x004b3fc0,0x004b5190,0x004b5240,0x004c5c80,0x004e6ab0,0x004e6e00 | depth-2; not recursed per session rules | powerups-cont1 | vehicle |
| D-1302 | 9 depth-2 callees of FUN_004587a0 (PowerUpIcons init): 0x0040bb30,0x004770c0,0x004b3b70,0x004b3d80,0x004b3e40,0x004b6520,0x004b65a0,0x004b65b0,0x004c5c80 | depth-2; not recursed per session rules | powerups-cont1 | vehicle |
| D-1303 | 9 depth-2 callees of FUN_00459290 (Laser/Lazer init): 0x0040bb30,0x00474d60,0x00476c10,0x00476cb0,0x004770c0,0x004b3bf0,0x004b3d80,0x004c57a0,0x004c5c80 | depth-2; not recursed per session rules | powerups-cont1 | vehicle |

| D-1120 | 0x00408af0 FUN_00408af0 | depth-2 callee of FUN_00416250 (ai_update session); called as FUN_00408af0(param_2) → float array pointer used in mode-2 steering dot-product | ai_update-cont1 | ai |
| D-1121 | 0x00414570 FUN_00414570 | depth-2 callee of FUN_00416250; called as FUN_00414570(spline,&local_24,&local_2c,vehicle_idx) → iVar5; highest-priority targeting decision | ai_update-cont1 | ai |
| D-1122 | 0x004148b0 FUN_004148b0 | depth-2 callee of FUN_00416250; called in game-mode-6 branch when local_48==0; same 4-arg targeting signature | ai_update-cont1 | ai |
| D-1123 | 0x00414a70 FUN_00414a70 | depth-2 callee of FUN_00416250; iVar5==2 causes immediate brake-return (param_3[4]=0,param_3[5]=0xff); iVar5==1 sets mode-3 | ai_update-cont1 | ai |
| D-1124 | 0x00414c30 FUN_00414c30 | depth-2 callee of FUN_00416250; iVar5==2 AND FUN_00416060!=0 → mode-7 (partial throttle 0x40); iVar5==1 → mode-3 | ai_update-cont1 | ai |
| D-1125 | 0x00414f00 FUN_00414f00 | depth-2 callee of FUN_00416250; gated on FUN_00426c00==0x21; on hit → mode-10 | ai_update-cont1 | ai |
| D-1126 | 0x00415020 FUN_00415020 | depth-2 callee of FUN_00416250; called in game-mode-6 gate; takes param_2 only; on != 0 → mode-5 | ai_update-cont1 | ai |
| D-1127 | 0x004150e0 FUN_004150e0 | depth-2 callee of FUN_00416250; takes (&local_1c,param_2); iVar5==1 → mode-9 (lateral brake adjustments) | ai_update-cont1 | ai |
| D-1128 | 0x00415220 FUN_00415220 | depth-2 callee of FUN_00416250; 5-arg: (spline,&local_24,&local_2c,vehicle_idx,uVar6); vehicle-target AI; on hit → mode-8; gated by DAT_0088fc88[v*0x2d] != 0 | ai_update-cont1 | ai |
| D-1129 | 0x00415880 FUN_00415880 | depth-2 callee of FUN_00416250; priority-2 targeting; gated by FUN_00415d00==0 to accept | ai_update-cont1 | ai |
| D-1130 | 0x00415d00 FUN_00415d00 | depth-2 callee of FUN_00416250; called as FUN_00415d00(param_2) → suppresses mode-2 acceptance when != 0 | ai_update-cont1 | ai |
| D-1131 | 0x00415e20 FUN_00415e20 | depth-2 callee of FUN_00416250; called as FUN_00415e20(param_2,local_24,local_20) → steering float (ST0); drives steer output bytes and speed-limit comparisons | ai_update-cont1 | ai |
| D-1132 | 0x00416060 FUN_00416060 | depth-2 callee of FUN_00416250; called as FUN_00416060(&local_1c,&local_2c) → gate check used across all targeting decisions; on == 0, candidate target is discarded | ai_update-cont1 | ai |
| D-1133 | 0x004161e0 FUN_004161e0 | depth-2 callee of FUN_00416250; called as FUN_004161e0(spline,&local_24,param_2); initialises target point index local_24 | ai_update-cont1 | ai |
| D-1134 | 0x00443080 FUN_00443080 | depth-2 callee of FUN_00416250; called in game-mode-6 gate as FUN_00443080() → must return 0 to enable mode-6 path | ai_update-cont1 | ai |
| D-1135 | 0x00443440 FUN_00443440 | depth-2 callee of FUN_00416250; called as FUN_00443440(spline,&local_1c,0x41200000,&local_40,0) → spline distance/progress float; 0x41200000=float 10.0 | ai_update-cont1 | ai |
| D-1136 | 0x0046d4a0 FUN_0046d4a0 | depth-2 callee of FUN_00416250; called as FUN_0046d4a0(&local_30,param_2) → per-vehicle pointer into local_30; local_30+0x30 and +0x38 read as vehicle state | ai_update-cont1 | ai |
| D-1137 | 0x0046d510 FUN_0046d510 | depth-2 callee of FUN_00416250; called as FUN_0046d510(&local_14,param_2) → 3-float vector into local_14/local_10/local_c; used in mode-2 dot-product | ai_update-cont1 | ai |
| D-1138 | 0x0046d6a0 FUN_0046d6a0 | depth-2 callee of FUN_00416250; called as FUN_0046d6a0(&local_38,param_2) → float into local_38; compared against speed/change thresholds for brake override | ai_update-cont1 | ai |
| D-1139 | 0x0046d6d0 FUN_0046d6d0 | depth-2 callee of FUN_00416250; called as FUN_0046d6d0(&local_3c,param_2) → float into local_3c; compared against _DAT_005cd0dc/_005cd0d8 in mode-9 branch | ai_update-cont1 | ai |

| D-1140 | 0x00414030 FUN_00414030 | depth-2 callee of FUN_00417180; called as FUN_00414030(0xffffffff) on AI line-bank switches; return value via ECX/EDX; S-0403 | ai_update-cont1 | ai |
| D-1141 | 0x00472650 FUN_00472650 | depth-2 callee of FUN_00417180; called as FUN_00472650(0, 0x3f800000); returns float10; compared against _DAT_005cc32c for random spline variation; S-0404 | ai_update-cont1 | ai |

| D-1142 | 0x00452160 FUN_00452160 | depth-2 callee of FUN_00417640; called as FUN_00452160() → float array ptr (target vehicle position?); S-0405 | ai_update-cont1 | ai |
| D-1143 | 0x00452ea0 FUN_00452ea0 | depth-2 callee of FUN_00417640; called as FUN_00452ea0(param_1) → per-vehicle powerup predicate; S-0406 | ai_update-cont1 | ai |
| D-1144 | 0x00452eb0 FUN_00452eb0 | depth-2 callee of FUN_00417640; called as FUN_00452eb0() → float10; threshold gate; S-0407 | ai_update-cont1 | ai |
| D-1145 | 0x0046d570 FUN_0046d570 | depth-2 callee of FUN_00417640; called as FUN_0046d570(&local_10,param_1) → float; distinct from FUN_0046d6d0; S-0408 | ai_update-cont1 | ai |
| D-1146 | 0x004c3ac0 FUN_004c3ac0 | depth-2 callee of FUN_00417640; called as FUN_004c3ac0(&local_c) → in-place op on 3-float displacement; S-0409 | ai_update-cont1 | ai |
| D-1147 | 0x00417cf0 FUN_00417cf0 | depth-2 callee of FUN_00417da0; replaces FUN_004148b0 in mode-8 variant targeting; same 4-arg signature; S-0410 | ai_update-cont1 | ai |
| D-0940 | 0x005ba1d0 LAB_005ba1d0 body tail | Body extends beyond 471 code units; listing capped at 250; address range ~0x005ba4a0+ not analyzed | Re-run listing_code_units_list with extended range; file as D-0940 | audio |
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
| D-1720 | 0x0042a470 FUN_0042a470 | piz lookup; 5 type-codes tried; depth-2 of FUN_0042a6b0; S-0600 | texture_loader-cont1 session | render |
| D-1721 | 0x00496400 FUN_00496400 | printf-style debug logger; depth-2 of FUN_0042a6b0; S-0601 | texture_loader-cont1 session | render |
| D-1722 | 0x004cf7d0 FUN_004cf7d0 | TXD stream reader; called after FUN_004cc5e0(0x16); 513 bytes; candidate RwTexDictionaryStreamRead; S-0602 | texture_loader-cont1 session | render |
| D-1723 | 0x0054f8d0 FUN_0054f8d0 | DFF/clump stream reader; called after FUN_004cc5e0(0x23); 1153 bytes; candidate RpClumpStreamRead; S-0603 | texture_loader-cont1 session | render |
| D-1724 | 0x004cc230 FUN_004cc230 | RW stream open; called with (2,1,filename); also S-0281 S-0421; depth-2 of FUN_004b3d20 and FUN_004b3d80 | texture_loader-cont1 session | render |
| D-1725 | 0x004cc5e0 FUN_004cc5e0 | RW stream find chunk; called with (handle, 0x16 or 0x23, ...); also S-0422; depth-2 of FUN_004b3d20 and FUN_004b3d80 | texture_loader-cont1 session | render |
| D-1726 | 0x004cc160 FUN_004cc160 | RW stream close; called with (handle, 0); also S-0283 S-0424; depth-2 of FUN_004b3d20 and FUN_004b3d80 | texture_loader-cont1 session | render |
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
| D-1548 | 0x0040e180 | FUN_0040e180 | FUN_00471ec0 | depth-2 callee; populates timing struct at local_24 | reverse FUN_00471ec0 fully first |
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
| D-2140 | n/a | render_pipeline PIPELINE_SETUP_FN | session render_pipeline-20260502-2227 halted; RxPipeline/RpAtomicSetPipeline/RpMaterial*/rpATOMIC/RpMeshHeader absent as strings and imports (release-stripped RW); PIPELINE_SETUP_FN not identified | re-run as bucket render_pipeline-cont1; use reference_to(0x005cf820) — "Calling RenderwareAttachPlugins\n" — as anchor; verify 0x005cf820 address via listing_code_unit_at before citing |
| D-2080 | render_frame_tree FRAME_UPDATE_FN | Session render_frame_tree-20260502-2221 halted; RwFrameUpdateObjects/RwFrameTransform/RwFrameForAllChildren/RwFrameSetIdentity/_rwFrameSyncDirty absent as imports and symbols (RW statically linked; FidDB did not match); FRAME_UPDATE_FN not identified | Re-run after FidDB re-analysis on Mashed.gpr with a RW 3.x FidDB, or locate FRAME_UPDATE_FN via Frida structural scan (frame-tree walker pattern); re-open as bucket render_frame_tree-cont1 | render |
| D-2440 | custom allocator implementations at obj+0x1ec | FUN_005208c0 dispatches to a function pointer at allocator-object+0x1ec; concrete functions placed there are the pool/arena allocators; not reachable via static call graph from FUN_005208c0 alone | Frida: hook FUN_005208c0, log param_1+0x1ec at each call; capture all concrete function pointers; then decomp each via MCP | bucket memory_pool-cont1 |
| D-2320 | 0x004967e0 | FUN_004967e0 | FUN_00492d20 | depth-2 from INTRO_FN; 283 bytes; sole callee of frame-tick shim | decomp FUN_004967e0; pick up as intro_splash-cont1 |
| D-2500 | 0x005507b0 0x00550bc0 | FUN_005507b0 FUN_00550bc0 | depth-2 callees of FUN_004cc230 (stream-open); PIZ-open and file-open; from session localization-20260502-2227 bucket localization | pick up as bucket localization-cont1; no further recursion |
| D-2501 | 0x00550910 | FUN_00550910 | depth-2 callee of FUN_004cc160 (stream-close); file-close; from session localization-20260502-2227 bucket localization | pick up as bucket localization-cont1; no further recursion |
| D-2502 | 0x004625b0 0x004669b0 0x004a2b60 | FUN_004625b0 FUN_004669b0 FUN_004a2b60 | sibling audio-language functions called by FUN_00402750; FUN_004669b0 calls FUN_004625b0(DAT_007f0f60) to build per-language audio path strings; out of scope for localization subset; from session localization-20260502-2227 | pick up as bucket localization-cont1; frontend subsystem |
| D-2321 | 0x004c75e0 | FUN_004c75e0 | FUN_00493fd0 | depth-2 from INTRO_FN; 26 bytes; fills viewport short[2] arrays | decomp FUN_004c75e0; pick up as intro_splash-cont1 |
| D-2380 | 0x004a42c5 FUN_004a42c5 | depth-2 callee of FUN_00496400 (debug-logger); sprintf/vsprintf variant; from session settings_config-20260502-2222 bucket settings_config | pick up as bucket settings_config-cont1; no further recursion | save |
| D-2381 | 0x00498c00 FUN_00498c00 | depth-2 callee of FUN_00499400 (video-settings dispatcher); called at start; role unknown; from session settings_config-20260502-2222 bucket settings_config | pick up as bucket settings_config-cont1; no further recursion | save |
| D-2382 | 0x00498a00 FUN_00498a00 | depth-2 callee of FUN_00499400; subsystem name getter; populates 69-byte struct; from session settings_config-20260502-2222 bucket settings_config | pick up as bucket settings_config-cont1; no further recursion | save |
| D-2383 | 0x00498e40 FUN_00498e40 | depth-2 callee of FUN_00499400; called on load failure; default settings init candidate; from session settings_config-20260502-2222 bucket settings_config | pick up as bucket settings_config-cont1; no further recursion | save |
| D-2384 | 0x00498ea0 FUN_00498ea0 | depth-2 callee of FUN_00499400; called before CONFIG_SAVE_FN; role unknown; from session settings_config-20260502-2222 bucket settings_config | pick up as bucket settings_config-cont1; no further recursion | save |
| D-2385 | 0x004c2e70 FUN_004c2e70 | depth-2 callee of FUN_00499400; post-save; arg is DAT_0077340c (subsystem index); from session settings_config-20260502-2222 bucket settings_config | pick up as bucket settings_config-cont1; no further recursion | save |
| D-2386 | 0x004c2f30 FUN_004c2f30 | depth-2 callee of FUN_00499400; post-save; arg is DAT_00773200 (mode index); from session settings_config-20260502-2222 bucket settings_config | pick up as bucket settings_config-cont1; no further recursion | save |
| D-2387 | 0x004c2ed0 FUN_004c2ed0 | depth-2 callee of FUN_00499400; post-save; writes display-param struct into locals; from session settings_config-20260502-2222 bucket settings_config | pick up as bucket settings_config-cont1; no further recursion | save |
| D-2322 | 0x00494320 | FUN_00494320 | FUN_00494460 | depth-2; 167 bytes; first cleanup step in video close | decomp FUN_00494320; pick up as intro_splash-cont1 |
| D-2323 | 0x004c7650 | FUN_004c7650 | FUN_00494460 | depth-2; releases video texture handle DAT_00771a18 | decomp FUN_004c7650; pick up as intro_splash-cont1 |
| D-2326 | 0x004c77c0 | FUN_004c77c0 | FUN_00494a80 | depth-2; 153 bytes; video texture allocator args(0,0,0,0x84) | decomp FUN_004c77c0; pick up as intro_splash-cont1 |
| D-1900 | 0x00428a30 FUN_00428a30 | caller of FUN_004950b0 (QPC*3M/freq); 433 bytes; not analyzed in timer session timer-20260502-2221 | pick up as bucket timer-cont1; no further recursion |
| D-1901 | 0x00493390 FUN_00493390 | caller of FUN_004950b0 (QPC*3M/freq); 238 bytes; not analyzed in timer session timer-20260502-2221 | pick up as bucket timer-cont1; no further recursion |
| D-1902 | 0x004a1570 FUN_004a1570 | GetTickCount caller; 519 bytes; 2 GetTickCount refs at 0x004a159b and 0x004a1683; likely frame-pacing not frame-delta timer | pick up as bucket timer-cont1; no further recursion |
| D-1903 | 0x005aafb5 area (no Ghidra function boundary) | QPC call at 0x005aafb5 and QPF call at 0x005aafca in unanalyzed code block; possible alternate timer-init not auto-analyzed by Ghidra | disassemble area; create function boundary; pick up as bucket timer-cont1 |
| D-1904 | _DAT_0077197c 0x0077197c and _DAT_007719a8 0x007719a8 | read sites missing from Ghidra cross-refs due to symbol-name overlap (_-prefix warning); timer output consumers unknown | search_bytes for these addresses in code; pick up as bucket timer-cont1 |
| D-2020 | 0x004cc7f0,0x004ccc50,0x004ccde0,0x004d1d70,0x004db550,0x004dc8e0,0x004e08b0 | depth-2 callees of 004c9ad0 (pre-reset resource release); roles: resource-release helpers / VB-pool walkers / render-target teardown | pick up as bucket render_d3d_reset-cont1; no further recursion required until D3D9 pipeline hooks begin |
| D-2021 | 0x004dc9e0 | depth-2 callee of 004c9ad0 (pre-reset resource release); S-0701 filed; role unknown from callsite | pick up as bucket render_d3d_reset-cont1 |
| D-2022 | 0x004cb8a0,0x004cba80 | depth-2 callees shared by 004db3e0 and 004e0920; S-0702/S-0703 filed; lock/create VB and release-buffer wrappers | pick up as bucket render_d3d_reset-cont1 |
| D-2023 | 0x004d5480,0x004d54f0,0x004d5570,0x004d53b0 | depth-2 callees of 004d6200 (RS cache reset); S-0704..S-0707 filed; render/sampler/TSS state setter wrappers and state-restore epilogue | pick up as bucket render_d3d_reset-cont1; shared with D3D9 state-machine bucket |
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
| D-2860 | 0x005ade60 FUN_005ade60 | depth-3 of FUN_005a7a40 (audio_rws_loader_d2 session); S-0980; list searcher in embedded list at obj+0x0c; called by pool-list-searcher | audio_rws_loader_d2-cont1 | audio |
| D-2861 | 0x005aeca0 FUN_005aeca0 | depth-3 of FUN_005ae0c0 (audio_rws_loader_d2 session); S-0981; format-field pack/byte-swap helper; 3 calls from format-descriptor copy | audio_rws_loader_d2-cont1 | audio |
| D-2862 | 0x005ae080 FUN_005ae080 | depth-3 of FUN_005ae010+FUN_005ae030 (audio_rws_loader_d2 session); S-0982; sub-struct A zero-init (audio_obj+0x24) | audio_rws_loader_d2-cont1 | audio |
| D-2863 | 0x005ae050 FUN_005ae050 | depth-3 of FUN_005adfe0+FUN_005ae030 (audio_rws_loader_d2 session); S-0983; sub-struct B zero-init (audio_obj+0x34) | audio_rws_loader_d2-cont1 | audio |
| D-2864 | 0x005aa060 FUN_005aa060 | depth-3 of FUN_005ac210 (audio_rws_loader_d2 session); S-0984; creates DirectSound buffer from stream data; called when param_1 bit3 set and no external sound pointer | audio_rws_loader_d2-cont1 | audio |
| D-2865 | 0x005aa0c0 FUN_005aa0c0 | depth-3 of FUN_005ac210 (audio_rws_loader_d2 session); S-0985; async sound load callback dispatcher; called via LAB_005ac770 when param_3 != 0 but param_2 == NULL | audio_rws_loader_d2-cont1 | audio |
| D-2866 | 0x005aaac0 FUN_005aaac0 | depth-3 of FUN_005ac210 (audio_rws_loader_d2 session); S-0986; resolves local_4c/param_3 from LAB_005aaaa0; called after DSound buffer path | audio_rws_loader_d2-cont1 | audio |
| D-2867 | 0x005aba20 FUN_005aba20 | depth-3 of FUN_005ac210 (audio_rws_loader_d2 session); S-0987; wave_node field initialiser; receives (format_ptr,count,chunk_hdr,section_id,mono_flag,node); 0x182 bytes | audio_rws_loader_d2-cont1 | audio |
| D-2868 | 0x005ac7b0 FUN_005ac7b0 | depth-3 of FUN_005ac210 (audio_rws_loader_d2 session); S-0988; wave_node post-init; wires format and extra-data pointers into node; 0x2f bytes | audio_rws_loader_d2-cont1 | audio |
| D-2869 | 0x005ac900 FUN_005ac900 | depth-3 of FUN_005ac210 (audio_rws_loader_d2 session); S-0989; format-object lookup by 16-byte descriptor; called from sound-data parse path | audio_rws_loader_d2-cont1 | audio |
| D-2870 | 0x005ac980 FUN_005ac980 | depth-3 of FUN_005ac210 (audio_rws_loader_d2 session); S-0990; registers default format or assigns format to sound node; called when FUN_005adf30 returns 0 | audio_rws_loader_d2-cont1 | audio |
| D-2871 | 0x005aca80 FUN_005aca80 | depth-3 of FUN_005ac210 (audio_rws_loader_d2 session); S-0991; get RWS chunk data size from parsed chunk header; 0x20 bytes | audio_rws_loader_d2-cont1 | audio |
| D-2872 | 0x005acaa0 FUN_005acaa0 | depth-3 of FUN_005ac210 (audio_rws_loader_d2 session); S-0992; parse RWS chunk header into local struct; reads 12-byte chunk header (id+size+version) from stream; 0x62 bytes | audio_rws_loader_d2-cont1 | audio |
| D-2873 | 0x005acd10 FUN_005acd10 | depth-3 of FUN_005ac210 (audio_rws_loader_d2 session); S-0993; matches wave_node against a chunk-header struct; called to associate sound bank entry with chunk; 0x4a bytes | audio_rws_loader_d2-cont1 | audio |
| D-2874 | 0x005ac5f0 FUN_005ac5f0 | depth-3 of FUN_005abd30 (audio_rws_loader_d2 session); S-0994; wave-state validator; checks wave sub-struct at param_1+0x10 vs streaming-cursor at param_1+0x2c; 0x5b bytes | audio_rws_loader_d2-cont1 | audio |
| D-2875 | 0x005b3b30 FUN_005b3b30 | depth-3 of FUN_005abd30 (audio_rws_loader_d2 session); S-0995; compute streaming parameters from wave sub-struct; fills local_1c fields; 0x26 bytes | audio_rws_loader_d2-cont1 | audio |
| D-2876 | 0x005b3b60 FUN_005b3b60 | depth-3 of FUN_005abd30 (audio_rws_loader_d2 session); S-0996; finalize streaming descriptor; 0x1c bytes | audio_rws_loader_d2-cont1 | audio |
| D-2877 | 0x005b3b80 FUN_005b3b80 | depth-3 of FUN_005abd30 (audio_rws_loader_d2 session); S-0997; fill streaming/loop buffer with PCM data; 0x2d6 bytes; large PCM feeder | audio_rws_loader_d2-cont1 | audio |
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
| D-2680 | 0x00450b10 FUN_00450b10 | FUN_00428610 | depth-3 callee of viewport-scaled rect draw; 7-param primitive draw (texture_handle, x, y, w, h, color, uv_ptr); from hud_ingame_d2-20260503 session | hud_ingame_d2-cont1 | hud |
