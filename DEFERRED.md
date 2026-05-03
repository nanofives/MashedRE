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
| D-2740 | 0x00430760 FUN_00430760 | depth-3 callee of FUN_004335f0/FUN_00439210; not recursed in hud_frontend_d2 | hud_frontend_d3 sweep session | frontend |
| D-2741 | 0x0042fab0 FUN_0042fab0 | depth-3 callee of FUN_004335f0/FUN_0043a610/FUN_00434720/FUN_0043aa30; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2742 | 0x0042bcb0 FUN_0042bcb0 | depth-3 callee of FUN_004335f0/FUN_0043a610/FUN_0043aa30; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2743 | 0x004282a0 FUN_004282a0 | depth-3 callee: text size/width; multiple callers in hud_frontend_d2; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2744 | 0x00427ad0 FUN_00427ad0 | depth-3 callee of FUN_004335f0; icon draw 7-param variant; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2745 | 0x0042f8d0 FUN_0042f8d0 | depth-3 callee of FUN_0043a610/FUN_00434720/FUN_0043aa30; background rect draw; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2746 | 0x0042ac00 FUN_0042ac00 | depth-3 callee: player count / slot index getter; multiple callers; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2747 | 0x00473870 FUN_00473870 | depth-3 callee: sprite draw 7-param; multiple callers; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2748 | 0x004368e0 FUN_004368e0 | depth-3 callee of FUN_0043af10/FUN_00439210; player alpha/color setup; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2749 | 0x0042ac50 FUN_0042ac50 | depth-3 callee: layout Y base getter; multiple callers; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2750 | 0x0042a940 FUN_0042a940 | depth-3 callee of FUN_0043af10; powerup sprite index by selection; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2751 | 0x00430830 FUN_00430830 | depth-3 callee of FUN_0043af10; split-screen track check; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2752 | 0x00458630 FUN_00458630 | depth-3 callee of FUN_0043af10; powerup sprite lookup by type; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2753 | 0x0040bb70 FUN_0040bb70 | depth-3 callee of FUN_0043af10; sprite lookup by name variant A; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2754 | 0x0040bb90 FUN_0040bb90 | depth-3 callee of FUN_0043af10; sprite lookup by name variant B; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2755 | 0x0040b620 FUN_0040b620 | depth-3 callee of FUN_00434720; player mode/sort variant; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2756 | 0x0040b460 FUN_0040b460 | depth-3 callee of FUN_00434720; player data fetch; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2757 | 0x0040e3a0 FUN_0040e3a0 | depth-3 callee of FUN_00434720; player name/string data; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2758 | 0x00430b30 FUN_00430b30 | depth-3 callee of FUN_00434720; lap time getter (3 out-params); not recursed | hud_frontend_d3 sweep session | frontend |
| D-2759 | 0x0042d290 FUN_0042d290 | depth-3 callee of FUN_00434720; lap time formatter; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2760 | 0x0042d300 FUN_0042d300 | depth-3 callee of FUN_00434720; lap time comparator; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2761 | 0x00429870 FUN_00429870 | depth-3 callee of FUN_00434720; race data getter A; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2762 | 0x00429a30 FUN_00429a30 | depth-3 callee of FUN_00434720; race data getter B; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2763 | 0x00429a80 FUN_00429a80 | depth-3 callee of FUN_00434720; race data getter C; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2764 | 0x00429a90 FUN_00429a90 | depth-3 callee of FUN_00434720; race data getter D; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2765 | 0x00429a70 FUN_00429a70 | depth-3 callee of FUN_00434720; race data getter E; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2766 | 0x004736c0 FUN_004736c0 | depth-3 callee of FUN_00434720; line/border renderer; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2767 | 0x00428320 FUN_00428320 | depth-3 callee of FUN_00434720; text width measurement (item-based); not recursed | hud_frontend_d3 sweep session | frontend |
| D-2768 | 0x0040b7a0 FUN_0040b7a0 | depth-3 callee of FUN_00434720; hotkey string getter; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2769 | 0x0040b7b0 FUN_0040b7b0 | depth-3 callee of FUN_00434720; per-player hotkey getter; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2770 | 0x0040b6b0 FUN_0040b6b0 | depth-3 callee of FUN_00434720; vehicle data getter A; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2771 | 0x0040b6c0 FUN_0040b6c0 | depth-3 callee of FUN_00434720; vehicle data getter B; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2772 | 0x00474e60 FUN_00474e60 | depth-3 callee of FUN_00434720; float-to-x87-angle converter (fsin/fcos prep); not recursed | hud_frontend_d3 sweep session | frontend |
| D-2773 | 0x0040ad20 FUN_0040ad20 | depth-3 callee of FUN_00431240; option state bool; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2774 | 0x00436810 FUN_00436810 | depth-3 callee of FUN_00439210; local player slot occupancy check; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2775 | 0x0042ebe0 FUN_0042ebe0 | depth-3 callee of FUN_00439210; AI/remote slot occupancy check; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2776 | 0x0042ee40 FUN_0042ee40 | depth-3 callee of FUN_00439210; vehicle sprite getter (slot type); not recursed | hud_frontend_d3 sweep session | frontend |
| D-2777 | 0x004391b0 FUN_004391b0 | depth-3 callee of FUN_00439210; powerup/overlay sprite draw; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2778 | 0x0042ef40 FUN_0042ef40 | depth-3 callee of FUN_00439210; vehicle lock-state check; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2779 | 0x00430a10 FUN_00430a10 | depth-3 callee of FUN_00439210; get player at slot type 0; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2780 | 0x00430a60 FUN_00430a60 | depth-3 callee of FUN_00439210; get player at slot type 1; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2781 | 0x00430ab0 FUN_00430ab0 | depth-3 callee of FUN_00439210; get player at slot type 2; not recursed | hud_frontend_d3 sweep session | frontend |
| D-2782 | 0x0042ee00 FUN_0042ee00 | depth-3 callee of FUN_00439210; vehicle icon by unlock state; not recursed | hud_frontend_d3 sweep session | frontend |

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
| ~~D-1000~~ | ~~0x0049ec10 FUN_0049ec10~~ | ~~analyzed C1 session video_mci_d2-20260503~~ | ~~video_mci-cont1~~ | ~~util~~ |
| ~~D-1001~~ | ~~0x004a3b84 FUN_004a3b84~~ | ~~analyzed C1 session video_mci_d2-20260503~~ | ~~video_mci-cont1~~ | ~~util~~ |
| ~~D-1002~~ | ~~LAB_00493ac0 + LAB_00493b40~~ | ~~analyzed C1 session video_mci_d2-20260503; S-0373/S-0374 resolved~~ | ~~video_mci-cont1~~ | ~~util~~ |
| D-4060 | 0x0049dd60 FUN_0049dd60 | depth-3 callee of FUN_0049ec10 (0x0049ec10); called as FUN_0049dd60(param_2,param_3,param_4,param_5); likely base/ancestor ctor taking 4 args; not decomped; S-1380 | video_mci_d2-cont1 | util |
| ~~D-1180~~ | ~~drained by track_loader_d2-20260503-0302~~ | ~~all 20 RVAs processed (4 already mapped, 16 newly mapped)~~ | ~~track_loader-cont1~~ | ~~render~~ |
| ~~D-1300~~ | ~~14 depth-2 callees of FUN_004548e0 (DepthCharge init)~~ | ~~drained by powerups_d2-20260503; all 14 RVAs mapped; new stubs S-1440..S-1453; D-4240..D-4243~~ | ~~powerups_d2~~ | ~~vehicle~~ |
| ~~D-1301~~ | ~~10 depth-2 callees of FUN_00456760 (GatlingGun init)~~ | ~~drained by powerups_d2-20260503; all unique RVAs mapped (7 shared with D-1300 already handled)~~ | ~~powerups_d2~~ | ~~vehicle~~ |
| ~~D-1302~~ | ~~9 depth-2 callees of FUN_004587a0 (PowerUpIcons init)~~ | ~~drained by powerups_d2-20260503; all unique RVAs mapped~~ | ~~powerups_d2~~ | ~~vehicle~~ |
| ~~D-1303~~ | ~~9 depth-2 callees of FUN_00459290 (Laser/Lazer init)~~ | ~~drained by powerups_d2-20260503; all unique RVAs mapped~~ | ~~powerups_d2~~ | ~~vehicle~~ |

| D-4180 | 0x00408a50 FUN_00408a50 | depth-3 callee of FUN_00414570/FUN_00414a70/FUN_00415880; called as FUN_00408a50(vehicle_idx) → race-progress float; used as per-vehicle ordering metric | ai_update_d2-cont1 | ai |
| D-4181 | 0x00442cc0 FUN_00442cc0 | depth-3 callee of FUN_004148b0/FUN_00414c30/FUN_00415020/FUN_00415220; called as FUN_00442cc0(vehicle_idx) → progress variant distinct from FUN_0046d6d0; == 0.0 means last-place | ai_update_d2-cont1 | ai |
| D-4182 | 0x00414300 FUN_00414300 | depth-3 callee of FUN_00414a70/FUN_00414c30; called as FUN_00414300(v,own_pos,param2,target_pos,scale,param3) → steer/target calculator; return non-0 on success | ai_update_d2-cont1 | ai |
| D-4183 | 0x00484c70 FUN_00484c70 | depth-3 callee of FUN_00414c30; called as FUN_00484c70(&count) → array ptr; count=local_5c; iterates world objects (obstacles/triggers) stride 0x8C | ai_update_d2-cont1 | ai |
| D-4184 | 0x00443d10 FUN_00443d10 | depth-3 callee of FUN_00415d00/FUN_00416060; called as FUN_00443d10(x,z) → char track-tile type; 0=wall 3=boundary; used in both ray-march checks | ai_update_d2-cont1 | ai |
| D-4185 | 0x00443dc0 FUN_00443dc0 | depth-3 callee of FUN_004161e0; called as FUN_00443dc0(spline,&pos,out_idx,vehicle_idx,1,0); initialises closest spline-point index | ai_update_d2-cont1 | ai |
| D-4186 | 0x00417730 FUN_00417730 | depth-3 callee of FUN_00417cf0; called as FUN_00417730(vehicle_idx) → "race angle" float; used for angular-difference gating in mode-8 variant targeting | ai_update_d2-cont1 | ai |
| D-4187 | 0x0046cc10 FUN_0046cc10 | depth-3 callee of FUN_00415880; called as FUN_0046cc10(&local_34,vehicle_idx) → state code; == 4 required for ram-from-behind latch | ai_update_d2-cont1 | ai |
| D-4188 | 0x00443300 FUN_00443300 | depth-3 callee of FUN_00443440; called as FUN_00443300(spline,idx,t,&out_xz) → interpolated XZ point on spline segment; foundational for spline-progress calculation | ai_update_d2-cont1 | ai |
| D-4189 | 0x0046c7b0 FUN_0046c7b0 | depth-3 callee of FUN_00414570/FUN_00415880; called as FUN_0046c7b0(vehicle_idx) → non-zero if vehicle alive/present; distinct from FUN_0040e470 active-state check | ai_update_d2-cont1 | ai |
| D-4190 | 0x0046cbb0 FUN_0046cbb0 | depth-3 callee of FUN_00414570/FUN_00415880; called as FUN_0046cbb0(vehicle_idx,&local_30,local_24) → local_30=0 if not in spinout; gate for targeting | ai_update_d2-cont1 | ai |
| D-4191 | 0x004233e0 FUN_004233e0 | depth-3 callee of FUN_00443440; called as FUN_004233e0(dx,dz) → float10 heading angle; used to compute tangent angle for curvature calculation | ai_update_d2-cont1 | ai |
| D-4192 | 0x004c3df0 FUN_004c3df0 | depth-3 callee of FUN_0046d510; called as FUN_004c3df0(vel_field,matrix,1,struct_base) → transforms velocity float3 by DAT_00614708 matrix | ai_update_d2-cont1 | ai |
| D-4193 | 0x00415200 FUN_00415200 | depth-3 callee of FUN_00415220; guard in powerup-activation cases 7/9/0xb/0x10/0x11; non-zero required to proceed | ai_update_d2-cont1 | ai |
| D-4194 | 0x0045a0f0 FUN_0045a0f0 | depth-3 callee of FUN_00415220; called as FUN_0045a0f0(fVar2) → vehicle check; case 9 armed-state clears if non-zero; case 0xb gate | ai_update_d2-cont1 | ai |
| D-4195 | 0x00415190 FUN_00415190 | depth-3 callee of FUN_00415220; called as FUN_00415190(fVar2,float_min,float_max) → 0/1; track range check using float bounds; gates cases 10/0xb/0xc/0x10/0x11 | ai_update_d2-cont1 | ai |
| D-4196 | 0x00455b40 FUN_00455b40 | depth-3 callee of FUN_00415220; called as FUN_00455b40(fVar2) → non-zero required in case 0xb path | ai_update_d2-cont1 | ai |
| D-4197 | 0x0041f030 FUN_0041f030 | depth-3 callee of FUN_00414c30; called as FUN_0041f030(param_4,local_30) in type-0x15 trigger path; role unknown | ai_update_d2-cont1 | ai |
| D-4198 | 0x0048a630 FUN_0048a630 | depth-3 callee of FUN_00414c30; called as FUN_0048a630(local_20,local_30) → 0/1 collision/proximity check for type-0x15 trigger activation | ai_update_d2-cont1 | ai |
| D-4199 | 0x00414490 FUN_00414490 | depth-3 callee of FUN_00414c30; called as FUN_00414490(v,own,param2,obj_pos,scale,param3) → alt targeting in bomb-type path; returns 1 on hit then checks height diff | ai_update_d2-cont1 | ai |
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
| D-2620 | 0x0042a640 FUN_0042a640 | depth-3 callee of FUN_00479330 (track_loader_d2 session); S-0912; BSP/world file loader; used for main BSP + collision BSPs + AI data BSPs + anim BSPs | track_loader_d2-cont1 | render |
| D-2621 | 0x0042a5d0 FUN_0042a5d0 | depth-3 callee of FUN_00479330 (track_loader_d2 session); S-0913; DFF/clump file loader; loads scene models + sky dome DFFs | track_loader_d2-cont1 | render |
| D-2622 | 0x0042a740 FUN_0042a740 | depth-3 callee of FUN_00479330 (track_loader_d2 session); S-0914; UVA animation file loader; (name, &ver, &len); up to 8 per track | track_loader_d2-cont1 | render |
| D-2623 | 0x0042a7f0 FUN_0042a7f0 | depth-3 callee of FUN_00479330 (track_loader_d2 session); S-0915; spline file loader; up to 0x10 per track | track_loader_d2-cont1 | render |
| D-2624 | 0x0042a860 FUN_0042a860 | depth-3 callee of FUN_00479330 (track_loader_d2 session); S-0916; animation path/general anim file loader; camera path + 8 general anims | track_loader_d2-cont1 | render |
| D-2625 | 0x00478200 FUN_00478200 | depth-3 callee of FUN_00479330 (track_loader_d2 session); S-0918; course load error handler; called with code 0 (BSP fail) or code 2 (collision BSP fail) | track_loader_d2-cont1 | render |
| D-2626 | 0x004790e0 FUN_004790e0 | depth-3 callee of FUN_00479330 (track_loader_d2 session); course post-load setup; called once after all assets loaded | track_loader_d2-cont1 | render |
| D-2627 | 0x00474fd0 FUN_00474fd0 | depth-3 callee of FUN_00479330 (track_loader_d2 session); S-0917; sky dome frame builder; called per sky dome clump and per animation clump node | track_loader_d2-cont1 | render |
| D-2628 | 0x00426060 FUN_00426060 | depth-3 callee of FUN_00480340 (track_loader_d2 session); pre-physics init step 1; called at "Initialize CALLED" preamble; returns uVar1 used as physics world handle | track_loader_d2-cont1 | render |
| D-2629 | 0x00426070 FUN_00426070 | depth-3 callee of FUN_00480340 (track_loader_d2 session); pre-physics init step 2; called immediately after FUN_00426060 | track_loader_d2-cont1 | render |
| D-2630 | 0x0047f840 FUN_0047f840 | depth-3 callee of FUN_00480340 (track_loader_d2 session); physics world pre-init; called in preamble before scene-object loop | track_loader_d2-cont1 | render |
| D-2631 | 0x0047f940 FUN_0047f940 | depth-3 callee of FUN_00480340 (track_loader_d2 session); cylinder physics body creator; (dff, mts, DAT_006ce274, uVar1, idx, handle) | track_loader_d2-cont1 | render |
| D-2632 | 0x0047fc40 FUN_0047fc40 | depth-3 callee of FUN_00480340 (track_loader_d2 session); box physics body creator; same 6-arg signature as FUN_0047f940 | track_loader_d2-cont1 | render |
| D-2633 | 0x0047fe00 FUN_0047fe00 | depth-3 callee of FUN_00480340 (track_loader_d2 session); physics body type 3 (mesh?); (dff, mts, DAT_006ce274, uVar1, idx) — 5 args | track_loader_d2-cont1 | render |
| D-2634 | 0x0047ff70 FUN_0047ff70 | depth-3 callee of FUN_00480340 (track_loader_d2 session); physics body type 4 (mesh?); same 5-arg signature | track_loader_d2-cont1 | render |
| D-2635 | 0x00480100 FUN_00480100 | depth-3 callee of FUN_00480340 (track_loader_d2 session); scene physics post-init; called after scene-object loop; "numscenePobjects=%d" | track_loader_d2-cont1 | render |
| D-2636 | 0x0047ce40 FUN_0047ce40 | depth-3 callee of FUN_004715a0 (track_loader_d2 session); S-0907; AI polygon index resolver; called once per sub-polygon per AI node rebuild | track_loader_d2-cont1 | render |
| D-2637 | 0x004b5030 FUN_004b5030 | depth-3 callee of FUN_00478660 (track_loader_d2 session); S-0908; reads polygon by colour-key from AI binary blob; (blob, &key, buf, 2) | track_loader_d2-cont1 | render |
| D-2638 | 0x004785e0 FUN_004785e0 | depth-3 callee of FUN_00478660 (track_loader_d2 session); S-0910; AI polygon-start flag setter; called with (1) per polygon | track_loader_d2-cont1 | render |
| D-2639 | 0x004783f0 FUN_004783f0 | depth-3 callee of FUN_00478660 (track_loader_d2 session); S-0911; AI polygon array finalizer; (param_1) after all polygons processed | track_loader_d2-cont1 | render |
| D-2640 | 0x0047bf70 FUN_0047bf70 | depth-3 callee of FUN_0047c0f0 (track_loader_d2 session); S-0919; builds 0x56-dword collision sector record from RW world sector handle | track_loader_d2-cont1 | render |
| D-2641 | 0x00491590 FUN_00491590 | depth-3 callee of FUN_00491780 (track_loader_d2 session); actual dispatch body gated by DAT_00771534; purpose unknown | track_loader_d2-cont1 | render |
| D-2642 | 0x004cc5e0 FUN_004cc5e0 | depth-3 callee of FUN_00462950 (track_loader_d2 session); S-0900; locale audio descriptor query; (descriptor, 0x809, 0, 0) | track_loader_d2-cont1 | render |
| D-2643 | 0x0045de80 FUN_0045de80 | depth-3 callee of FUN_00462950 (track_loader_d2 session); S-0904; track audio secondary setup; (audio_dir, param_1) | track_loader_d2-cont1 | render |
| D-2644 | 0x0045e2a0 FUN_0045e2a0 | depth-3 callee of FUN_00462950 (track_loader_d2 session); S-0905; track-0 audio variant init | track_loader_d2-cont1 | render |
| D-2645 | 0x0045e160 FUN_0045e160 | depth-3 callee of FUN_00462950 (track_loader_d2 session); S-0906; track-0x24 (index 36) audio variant init | track_loader_d2-cont1 | render |
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
| D-0880 | 0x00426670 FUN_00426670 | render_frame-20260503-0611 | depth-2 callee of FUN_00492e90 (RENDER_FRAME_FN); called with primary camera; role: camera setup or world geometry render entry point; called before FUN_004c1a00 (BeginUpdate) | render_frame-cont1 | render |
| D-0881 | 0x0040de30 FUN_0040de30 | render_frame-20260503-0611 | depth-2 callee of FUN_00492e90; called with overlay camera; renders minimap/overhead geometry | render_frame-cont1 | render |
| D-0882 | 0x0040df20 FUN_0040df20 | render_frame-20260503-0611 | depth-2 callee of FUN_00492e90; called with overlay camera at end of overlay block; role unknown | render_frame-cont1 | render |
| D-0883 | 0x0040df60 FUN_0040df60 | render_frame-20260503-0611 | depth-2 callee of FUN_00492e90; inside BeginUpdate block of primary camera; render sub-pass | render_frame-cont1 | render |
| D-0884 | 0x00404320 FUN_00404320 | render_frame-20260503-0611 | depth-2 callee of FUN_00492e90; inside BeginUpdate block; render sub-pass | render_frame-cont1 | render |
| D-0885 | 0x00410b30 FUN_00410b30 | render_frame-20260503-0611 | depth-2 callee of FUN_00492e90; inside BeginUpdate block of primary camera; per-frame HUD render | render_frame-cont1 | render |
| D-0886 | 0x00426030 FUN_00426030 | render_frame-20260503-0611 | depth-2 callee of FUN_00492e90; called before BeginUpdate with no camera arg; world render setup | render_frame-cont1 | render |
| D-0887 | 0x004266b0 FUN_004266b0 | render_frame-20260503-0611 | depth-2 callee of FUN_00492e90; called with primary camera at end of render block; camera post-render cleanup | render_frame-cont1 | render |
| D-0888 | 0x00492440 FUN_00492440 | render_frame-20260503-0611 | depth-2 callee of FUN_00492e90; iterates array 0x00868320..0x869ca0 (step 0x44, ~100 entries); per-frame object update loop | render_frame-cont1 | render |
| D-0889 | 0x00492e60 FUN_00492e60 | render_frame-20260503-0611 | depth-2 callee of FUN_00492e90; called in fade/race-end states (1 and 4); role unknown; very small (0x2b bytes) | render_frame-cont1 | render |
| D-0890 | 0x00433f40 FUN_00433f40 | render_frame-20260503-0611 | depth-2 callee of FUN_00492e90; called in race-end states (cases 1 and 4); results or finish screen render | render_frame-cont1 | render |
| D-0891 | 0x00403050 FUN_00403050 | render_frame-20260503-0611 | depth-2 callee of FUN_00492e90; called in state 1 when sub-state==0x21; role unknown | render_frame-cont1 | render |
| D-0892 | 0x0042d390 FUN_0042d390 | render_frame-20260503-0611 | depth-2 callee of FUN_00492e90; called in state 3 (race); returns int checked <3; game state sub-query | render_frame-cont1 | render |
| D-0893 | 0x0042f530 FUN_0042f530 | render_frame-20260503-0611 | depth-2 callee of FUN_00492e90; state 3 race loop; int checked !=0; game state/music query | render_frame-cont1 | render |
| D-0894 | 0x0042a9f0 FUN_0042a9f0 | render_frame-20260503-0611 | depth-2 callee of FUN_00492e90; used in state 6 to check >200; 5-byte function; game state query | render_frame-cont1 | render |
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
| D-3760 | 0x004a2c48 FUN_004a2c48 | depth-2 callee of FUN_00408a70; called at entry; return val written to 4 per-car struct fields; S-1280; from session race_results-20260503-0655 | pick up as bucket race_results-cont1; same depth; no further recursion | frontend |
| D-3761 | 0x0040e340 FUN_0040e340 | depth-2 callee of FUN_0040b290; active-car counter; S-1281; from session race_results-20260503-0655 | pick up as bucket race_results-cont1; same depth; no further recursion | frontend |
| D-3762 | 0x0040e350 FUN_0040e350 | depth-2 callee of FUN_0040b290; game-state query vs {1..5}; S-1282; from session race_results-20260503-0655 | pick up as bucket race_results-cont1; same depth; no further recursion | frontend |
| D-3763 | 0x0040e370 FUN_0040e370 | depth-2 callee of FUN_0040b290; per-car active check; S-1283; from session race_results-20260503-0655 | pick up as bucket race_results-cont1; same depth; no further recursion | frontend |
| D-3764 | 0x004189f0 thunk_FUN_00419760 | depth-2 callee of FUN_00422fd0; thunk→FUN_00419760; (car,1) when state==1 and mode!=7; S-1284; from session race_results-20260503-0655 | pick up as bucket race_results-cont1; same depth; no further recursion | frontend |
| D-3765 | 0x004215c0 FUN_004215c0 | depth-2 callee of FUN_00422fd0; called (car,50.0f,0) and (car,50.0f,1); S-1285; from session race_results-20260503-0655 | pick up as bucket race_results-cont1; same depth; no further recursion | frontend |
| D-3766 | 0x0045ba00 FUN_0045ba00 | depth-2 callee of FUN_00422fd0; called (car,2); S-1286; from session race_results-20260503-0655 | pick up as bucket race_results-cont1; same depth; no further recursion | frontend |
| D-3767 | 0x0046c5c0 FUN_0046c5c0 | depth-2 callee of FUN_00422fd0; first in eliminate sequence; S-1287; from session race_results-20260503-0655 | pick up as bucket race_results-cont1; same depth; no further recursion | frontend |
| D-3768 | 0x0046c790 FUN_0046c790 | depth-2 callee of FUN_00422fd0; second in eliminate sequence; S-1288; from session race_results-20260503-0655 | pick up as bucket race_results-cont1; same depth; no further recursion | frontend |
| D-3281 | (bulk) 0x0040ab40,0x0040ac80,0x0040b810,0x0040de10,0x0040e360,0x00422b30,0x00429aa0,0x0042af50,0x0042b950,0x0042c150,0x0046dc00,0x00493570,0x00493580 | util/multi | from session timer_d2-20260503-0656 (FUN_0043d7c0 secondary callees); S range exhausted; pick up per-subsystem or in timer_d2-cont1 | timer_d2 |
| D-3282 | 0x0043dfd0 | FUN_0043dfd0 | util | from session timer_d2-20260503-0656; decomp 63977 chars / 30839 tokens exceeded read buffer; C0 only; requires dedicated large-function session (Opus recommended); 63 callees unprocessed | timer_d2 |
| D-3700 | 0x004c5890 FUN_004c5890 | RwTexDictionaryCreate candidate; allocates with type-hint 0x30016; sets *result=6; links into doubly-linked list at DAT_007d4054; 151 bytes | texture_loader_d2-cont1 session | render |
| D-3701 | 0x004cee90 FUN_004cee90 | native raster reader; reads struct chunk(type=1) then 0x10 bytes; allocates with type-hint 0x30018; fills width/height/type/pixels fields; reads pixel rows + palette; 319 bytes | texture_loader_d2-cont1 session | render |
| D-3702 | 0x004c77c0 FUN_004c77c0 | RwRasterCreate candidate; called as (width, height, depth, flags); 153 bytes; used in 0x0054f8d0 and 0x0054fd60 for hardware raster allocation | texture_loader_d2-cont1 session | render |
| D-3703 | 0x00550130 FUN_00550130 | called after FindChunk(0x06) in 0x0054f8d0; returns struct ptr; caller reads +0x50/+0x0c/+0x10; 532 bytes; purpose unclear | texture_loader_d2-cont1 session | render |
| D-3704 | 0x004cbd30 FUN_004cbd30 | stream read bytes candidate (RwStreamRead); called as (stream, dest, size); returns bytes read; 317 bytes | texture_loader_d2-cont1 session | render |
| D-3705 | 0x004cc400 FUN_004cc400 | chunk header reader; called by FUN_004cc5e0 as (stream, &type, &size, &version, 0); assumed 12-byte header; needs verify | texture_loader_d2-cont1 session | render |
| D-3706 | 0x004cc050 FUN_004cc050 | chunk body skip; called by FUN_004cc5e0 as (stream, size); advances stream position | texture_loader_d2-cont1 session | render |
| D-3707 | 0x004cefd0 FUN_004cefd0 | raster post-load processor; called in 0x004cee90 and 0x0054f8d0 if (*raster & 2) == 0; 447 bytes; meaning unclear | texture_loader_d2-cont1 session | render |
| D-3708 | 0x004c5bc0 FUN_004c5bc0 | RwTexDictionaryAddTexture candidate; called as (dict, texture) in both 0x004cf7d0 and 0x0054fd60; 57 bytes | texture_loader_d2-cont1 session | render |
| D-3709 | 0x004e1b60 FUN_004e1b60 | event dispatcher; called with (event_obj, stream, data_ptr); event objects DAT_00618138/DAT_00618150; 298 bytes; U-1269 | texture_loader_d2-cont1 session | render |
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
| D-4000 | 0x005aabe0 FUN_005aabe0 | depth-3 callee of FUN_005ba720/FUN_005ba760; result gates CoInitialize; S-1360 | audio_dsound_d2-cont1 or audio sweep session | audio |
| D-4001 | 0x005bc860 FUN_005bc860 | depth-3 callee of FUN_005ba720; called after CoInitialize; S-1361 | audio_dsound_d2-cont1 or audio sweep session | audio |
| D-4002 | 0x005bc880 FUN_005bc880 | depth-3 callee of FUN_005ba760; 11-byte body; called when check<2; S-1362 | audio_dsound_d2-cont1 or audio sweep session | audio |
| D-4003 | 0x005baf40 FUN_005baf40 | depth-3 callee of LAB_005ba780; called (EDI,0x0) after FUN_005bb000; S-1363 | audio_dsound_d2-cont1 or audio sweep session | audio |
| D-4004 | 0x005aaa00 FUN_005aaa00 | depth-3 callee of LAB_005ba780; called (EAX,0,0) when [ESI+0xf4]!=0; S-1364 | audio_dsound_d2-cont1 or audio sweep session | audio |
| D-4005 | 0x005aab70 FUN_005aab70 | depth-3 callee of LAB_005ba7f0; called at entry (EDI, 0x1); S-1365 | audio_dsound_d2-cont1 or audio sweep session | audio |
| D-4006 | 0x005baa60 FUN_005baa60 | depth-3 callee of LAB_005ba7f0 + FUN_005bac00; motion-state processing or similar; S-1366 | audio_dsound_d2-cont1 or audio sweep session | audio |
| D-4007 | 0x005bc750 FUN_005bc750 | depth-3 callee of LAB_005ba7f0; called when [EDI+0x78] bit 0x8 set and bits 0x7 clear; S-1367 | audio_dsound_d2-cont1 or audio sweep session | audio |
| D-4008 | 0x005aea50 FUN_005aea50 | depth-3 callee of FUN_005bb000; allocator (1, size, 0x3080c); streaming buffer alloc; S-1368 | audio_dsound_d2-cont1 or audio sweep session | audio |
| D-4009 | 0x005aeea0 FUN_005aeea0 | depth-3 callee of FUN_005bb000; event/handle init (param+0x3d, 0, 1); S-1369 | audio_dsound_d2-cont1 or audio sweep session | audio |
| D-4010 | 0x005aef00 FUN_005aef00 | depth-3 callee of FUN_005bb000; thread creation (param+0x3e, LAB_005bb380, 0xf, 0x1000); S-1370 | audio_dsound_d2-cont1 or audio sweep session | audio |
| D-4011 | 0x005aef30 FUN_005aef30 | depth-3 callee of FUN_005bb000; thread start/bind (param+0x3e, param+0x27); S-1371 | audio_dsound_d2-cont1 or audio sweep session | audio |
| D-4012 | 0x005bc470 FUN_005bc470 | depth-3 callee of FUN_005bb000; streaming format path (DAT_005d098c, format, param+0x2a); S-1372 | audio_dsound_d2-cont1 or audio sweep session | audio |
| D-4013 | 0x005bc640 FUN_005bc640 | depth-3 callee of FUN_005bb000; streaming teardown helper (param+0x2a); S-1373 | audio_dsound_d2-cont1 or audio sweep session | audio |
| D-4014 | 0x005bbd50 FUN_005bbd50 | depth-3 callee of FUN_005bbc10; returns COM interface from (param_1, param_5); S-1374 | audio_dsound_d2-cont1 or audio sweep session | audio |
| D-4015 | 0x005bbed0 FUN_005bbed0 | depth-3 callee of FUN_005bbdb0; post-CreateSoundBuffer; takes (param_6, param_3, unaff_retaddr); S-1375 | audio_dsound_d2-cont1 or audio sweep session | audio |
| D-4240 | 0x004547c0 FUN_004547c0 | depth-3 callee of FUN_004548a0; per-entry activator for DepthCharge struct-A (stride 0x2c, 0x00688240..0x006882f0); ESI-implicit; role unknown; S-1441 | powerups_d2-cont1 | vehicle |
| D-4241 | 0x00454170 FUN_00454170 | depth-3 callee of FUN_004548a0; per-entry activator for DepthCharge struct-B (stride 0x44, 0x00688020..0x00688240); ESI-implicit; role unknown; S-1441 | powerups_d2-cont1 | vehicle |
| D-4242 | 0x00534b60 FUN_00534b60 | depth-3 callee of FUN_004770c0; particle/effect system allocate: (count, rw_flags, 0); result stored at param_1[1]; S-1444 | powerups_d2-cont1 | vehicle |
| D-4243 | 0x004e6d80+0x004c0870+0x004d8060+0x004e6920+0x004e6fe0+0x004e4d90+0x004e68a0+0x004c0790+0x004c0a60+0x004e6710+0x004c0de0+0x004e6f80+0x004e6d00+0x004e4440 | depth-3 callees of FUN_004e6ab0 (RW hierarchy instantiate); all RW hierarchy-management internals; not recursed | powerups_d2-cont1 | vehicle |
