# DEFERRED.md
<!-- Mutate only via re-classify skill. Hand-edits drift. -->
<!-- Format: | D-ID | RVA | name | subsystem | pickup_condition | bucket |  -->

| ID | RVA | Name | Subsystem | Pickup condition | Bucket |
|---|---|---|---|---|---|
| D-8680 | 004b7b00 | FUN_004b7b00 | input | error handler (lua_State, error_string); from FUN_004ba1b0 on size>0xfffffffc; decompile and classify | input_lua_d4 |
| D-8681 | 004b7ba0 | FUN_004b7ba0 | input | error handler (lua_State, int=4); from FUN_004ba1b0 on failed realloc; decompile and classify | input_lua_d4 |
| D-8682 | 004b7c70 | FUN_004b7c70 | input | longjmp error-path cleanup called from FUN_004b7be0; decompile and classify | input_lua_d4 |
| D-8683 | 004ba3e0 | FUN_004ba3e0 | input | lua_State sub-init 1 (param_1, param_2); from FUN_004ba210; decompile and classify | input_lua_d4 |
| D-8684 | 004ba470 | FUN_004ba470 | input | lua_State sub-init 2 (param_1); from FUN_004ba210; decompile and classify | input_lua_d4 |
| D-8685 | 004ba310 | FUN_004ba310 | input | lua_State sub-init 3 (param_1, param_2); from FUN_004ba210; decompile and classify | input_lua_d4 |
| D-8686 | 004ba2d0 | FUN_004ba2d0 | input | lua_State sub-init 4 (param_1); from FUN_004ba210; decompile and classify | input_lua_d4 |
| D-8687 | 004ba250 | FUN_004ba250 | input | lua_State sub-init 5 (param_1); from FUN_004ba210; decompile and classify | input_lua_d4 |
| D-8688 | 004ba290 | FUN_004ba290 | input | lua_State sub-init 6 (param_1); from FUN_004ba210; decompile and classify | input_lua_d4 |
| D-8800 | 00404fa0 | FUN_00404fa0 | util | spline matrix eval; 1068 bytes; callee of FUN_00405460; pick up as game_state_d5-cont2 | game_state_d5-cont2 |
| D-8801 | 00408b00 | FUN_00408b00 | util | grid-pos lookup; 0x1D2 bytes; callee of FUN_0040e590; pick up as game_state_d5-cont2 | game_state_d5-cont2 |
| D-8802 | 00409290 | FUN_00409290 | util | pos-handle writer; 0x6D bytes; callee of FUN_0040e590; pick up as game_state_d5-cont2 | game_state_d5-cont2 |
| D-8803 | 0040b250 | FUN_0040b250 | util | no-arg pre-placement; 0x33 bytes; callee of FUN_0040e590; pick up as game_state_d5-cont2 | game_state_d5-cont2 |
| D-8804 | 0040b410 | FUN_0040b410 | util | player readiness getter; 0xB bytes; callee of FUN_0040e590; pick up as game_state_d5-cont2 | game_state_d5-cont2 |
| D-8805 | 0041ede0 | FUN_0041ede0 | util | zeroing-loop call A; 0x6F bytes; callee of FUN_0040e590; pick up as game_state_d5-cont2 | game_state_d5-cont2 |
| D-8806 | 0041ee50 | FUN_0041ee50 | util | zeroing-loop call B; 0x58 bytes; callee of FUN_0040e590; pick up as game_state_d5-cont2 | game_state_d5-cont2 |
| D-8807 | 0041ef80 | FUN_0041ef80 | util | zeroing-loop call C; 0x36 bytes; callee of FUN_0040e590; pick up as game_state_d5-cont2 | game_state_d5-cont2 |
| D-8808 | 0041f000 | FUN_0041f000 | util | score-array call; 0x24 bytes; callee of FUN_0040e590; pick up as game_state_d5-cont2 | game_state_d5-cont2 |
| D-8809 | 00429820 | FUN_00429820 | util | no-arg init call; 0x14 bytes; callee of FUN_0040e590; pick up as game_state_d5-cont2 | game_state_d5-cont2 |
| D-8810 | 0046b1c0 | FUN_0046b1c0 | util | score-array call B; 0x329 bytes; callee of FUN_0040e590; pick up as game_state_d5-cont2 | game_state_d5-cont2 |
| D-8811 | 0046b540 | FUN_0046b540 | util | post-b1c0 call; 0x157 bytes; callee of FUN_0040e590; pick up as game_state_d5-cont2 | game_state_d5-cont2 |
| D-8812 | 0046c6d0 | FUN_0046c6d0 | util | score writer; 0x23 bytes; callee of FUN_0040e590; pick up as game_state_d5-cont2 | game_state_d5-cont2 |
| D-8813 | 004704c0 | FUN_004704c0 | util | 6-arg vehicle-placement; 0x1AA bytes; callee of FUN_0040e590; pick up as game_state_d5-cont2 | game_state_d5-cont2 |
| D-8814 | 0048f680 | FUN_0048f680 | util | no-arg init A; 0x20 bytes; callee of FUN_0040e590; pick up as game_state_d5-cont2 | game_state_d5-cont2 |
| D-8815 | 0048f740 | FUN_0048f740 | util | no-arg init B; 0x30 bytes; callee of FUN_0040e590; pick up as game_state_d5-cont2 | game_state_d5-cont2 |
| D-8864 | 00427ca0 | FUN_00427ca0 | boot | called by FUN_00402750 no args; result passed to FUN_004671a0 call; decompile and classify | boot_app_init_d2-cont1 |
| D-8880 | 00471eb0 | thunk_FUN_00471df0 | boot | thunk; called by FUN_00402750; decompile target FUN_00471df0 and classify | boot_app_init_d2-cont1 |
| D-8881 | 00425bc0 | FUN_00425bc0 | boot | called by FUN_00402750 after perm.piz load; decompile and classify | boot_app_init_d2-cont1 |
| D-8882 | 00558240 | FUN_00558240 | boot | called by FUN_00402750 with two stack ptr args; result stored in DAT_007f09e4; decompile and classify | boot_app_init_d2-cont1 |
| D-8883 | 004841d0 | FUN_004841d0 | boot | called by FUN_00402750; if non-zero result calls FUN_004671a0 chain; decompile and classify | boot_app_init_d2-cont1 |
| D-8884 | 00484170 | FUN_00484170 | boot | called by FUN_00402750 with FUN_004671a0 result; stores in DAT_007f0a30; decompile and classify | boot_app_init_d2-cont1 |
| D-8885 | 004723d0 | FUN_004723d0 | boot | called by FUN_00402750, no args; decompile and classify | boot_app_init_d2-cont1 |
| D-8886 | 00494ef0 | thunk_FUN_00493f70 | boot | thunk; first call in FUN_00402a40 teardown; decompile target FUN_00493f70 and classify | boot_app_init_d2-cont1 |
| D-8887 | 00494f20 | thunk_FUN_00494460 | boot | thunk; called if 00494ef0 returns non-zero in FUN_00402a40; decompile target FUN_00494460 and classify | boot_app_init_d2-cont1 |
| D-8888 | 00494bc0 | FUN_00494bc0 | boot | called by FUN_00402a40, no args; decompile and classify | boot_app_init_d2-cont1 |
| D-8889 | 00496ce0 | FUN_00496ce0 | boot | called by FUN_00402a40, no args; decompile and classify | boot_app_init_d2-cont1 |
| D-8890 | 0041a3d0 | FUN_0041a3d0 | boot | called by FUN_00402a40, no args; game-obj finalizer group; decompile and classify | boot_app_init_d2-cont1 |
| D-8891 | 0041de70 | FUN_0041de70 | boot | called by FUN_00402a40, no args; game-obj finalizer group; decompile and classify | boot_app_init_d2-cont1 |
| D-8892 | 0041c2c0 | FUN_0041c2c0 | boot | called by FUN_00402a40, no args; game-obj finalizer group; decompile and classify | boot_app_init_d2-cont1 |
| D-8893 | 0041da80 | FUN_0041da80 | boot | called by FUN_00402a40, no args; game-obj finalizer group; decompile and classify | boot_app_init_d2-cont1 |
| D-8894 | 0041e0d0 | FUN_0041e0d0 | boot | called by FUN_00402a40, no args; game-obj finalizer group; decompile and classify | boot_app_init_d2-cont1 |
| D-8895 | 0041d890 | FUN_0041d890 | boot | called by FUN_00402a40, no args; game-obj finalizer group; decompile and classify | boot_app_init_d2-cont1 |
| D-8896 | 0041ccf0 | FUN_0041ccf0 | boot | called by FUN_00402a40, no args; game-obj finalizer group; decompile and classify | boot_app_init_d2-cont1 |
| D-8897 | 0041c0e0 | FUN_0041c0e0 | boot | called by FUN_00402a40, no args; game-obj finalizer group; decompile and classify | boot_app_init_d2-cont1 |
| D-8898 | 0041b660 | FUN_0041b660 | boot | called by FUN_00402a40, no args; game-obj finalizer group; decompile and classify | boot_app_init_d2-cont1 |
| D-8899 | 00489250 | FUN_00489250 | boot | called by FUN_00402a40, no args; decompile and classify | boot_app_init_d2-cont1 |
| D-8900 | 0045b930 | FUN_0045b930 | boot | called by FUN_00402a40, no args; decompile and classify | boot_app_init_d2-cont1 |
| D-8901 | 0041ffb0 | FUN_0041ffb0 | boot | called by FUN_00402a40, no args; decompile and classify | boot_app_init_d2-cont1 |
| D-8902 | 00421590 | thunk_FUN_004210f0 | boot | thunk; called by FUN_00402a40, no args; decompile target FUN_004210f0 and classify | boot_app_init_d2-cont1 |
| D-8903 | 00484130 | FUN_00484130 | boot | called by FUN_00402a40 with DAT_007f0a30; decompile and classify | boot_app_init_d2-cont1 |
| D-8904 | 005581f0 | FUN_005581f0 | boot | called by FUN_00402a40 with DAT_007f09e4; decompile and classify | boot_app_init_d2-cont1 |
| D-8905 | 00467020 | FUN_00467020 | boot | called by FUN_00402a40 with FUN_004671a0 result; decompile and classify | boot_app_init_d2-cont1 |
| D-8906 | 00467070 | FUN_00467070 | boot | called by FUN_00402a40, no args; decompile and classify | boot_app_init_d2-cont1 |
| D-8907 | 004b4880 | FUN_004b4880 | boot | called by FUN_00402a40 twice with DAT_0067f18c then DAT_0067f190; decompile and classify | boot_app_init_d2-cont1 |
| D-8908 | 0042c2a0 | FUN_0042c2a0 | boot | called by FUN_00402a40, no args; decompile and classify | boot_app_init_d2-cont1 |
| D-8909 | 00427620 | FUN_00427620 | boot | called by FUN_00402a40, no args; decompile and classify | boot_app_init_d2-cont1 |
| D-8910 | 0047ba10 | thunk_FUN_00496970 | boot | thunk; called by FUN_00402a40; decompile target FUN_00496970 and classify | boot_app_init_d2-cont1 |
| D-8911 | 00425ed0 | FUN_00425ed0 | boot | called by FUN_00402a40, no args; decompile and classify | boot_app_init_d2-cont1 |
| D-8912 | 004b6550 | thunk_FUN_004b6700 | boot | thunk; called by FUN_00402a40; decompile target FUN_004b6700 and classify | boot_app_init_d2-cont1 |
| D-8913 | 00467010 | FUN_00467010 | boot | called by FUN_00402a40, no args; decompile and classify | boot_app_init_d2-cont1 |
| D-8914 | 00428400 | FUN_00428400 | boot | called by FUN_00402a40, no args; decompile and classify | boot_app_init_d2-cont1 |
| D-8915 | 004955c0 | thunk_FUN_00495580 | boot | thunk; target callee of 0x00493560 (cleanup/teardown path); decompile target FUN_00495580 and classify | boot_app_init_d2-cont1 |
| D-8916 | 004963d0 | thunk_FUN_00496370 | boot | thunk; target callee of 0x00493560 (cleanup/teardown path); decompile target FUN_00496370 and classify | boot_app_init_d2-cont1 |
| D-8917 | 00427620 | FontText_HudShutdown | hud | C3 refused: callee gate fails — FUN_00555830 C1, FUN_00556e40 C1, FUN_00556cd0 C1, FUN_00552b90 C1; pickup when all four callees reach C2+ | c3-batch-g-s13 |
| D-8921 | 0042aad0 | MenuDimSet | frontend | C3 caller-gate: FUN_0043c5b0 (only known caller) is C1; unblock when 0x0043c5b0 reaches C2+ OR another C2+ caller is identified; reimpl+hook+leaf-frida-evidence ready in MenuChrome.cpp | c3-batch-g-s1-followup |
| D-8918 | 0042aae0 | MenuIm2DQuad | frontend | C3 caller-gate: FUN_0043c5b0 (only known caller) is C1; unblock when 0x0043c5b0 reaches C2+ OR another C2+ caller is identified; reimpl+hook+path1-GREEN-10/10 ready in MenuChrome.cpp | c3-batch-g-s1-followup |
| D-8919 | 0042e3a0 | FUN_0042e3a0 | frontend | C3 callee-gate: callees 0x00427e00, 0x00472f40, 0x004730b0, 0x00472c60 all C1; unblock when all callees_depth1 reach C2+ | c3-batch-g-s1-followup |
| D-8920 | 0042e5b0 | FUN_0042e5b0 | frontend | C3 callee-gate: callees 0x00473c20, 0x00474890, 0x00473ee0 not in hooks.csv (untracked), 0x004a2c48 C1; unblock when all callees_depth1 reach C2+ | c3-batch-g-s1-followup |
| D-10697 | 0042d290 | FUN_0042d290 | frontend | c3-batch-g-s3 REFUSED: callee FUN_004a2b60 is C1 (sprintf wrapper at re/analysis/rw_engine_init_d3/004a2b60.md); C3 gate requires at-least-one-callee C2+. Promote FUN_004a2b60 to C2 (variadic sprintf wrapper, mechanical decomp) then re-queue 0x0042d290 as C2->C3 candidate | c3-batch-g-s3 |
| D-10698 | 0042d300 | FUN_0042d300 | frontend | c3-batch-g-s3 REFUSED: HARNESS GAP. Sig is void(int,int,uint32*,int*,int*,float*) — 2 in-ints + 4 out-pointers (sign-code u32, minutes int, seconds int, residual float). diff_template.js has no matching arg_type (closest is int_with_out_ptr=1+1, idx_out2=1+2, int_ptr2_out=1+2). Pickup: add new arg_type "int2_out_signMSF" (alloc 4 ptrs into single 16-byte buffer; report packed [sign,min,sec,frac] hex) to diff_template.js, then re-queue. Aligns with HARNESS-GAP family D-10601 cited in batch-g spec | c3-batch-g-s3 |
| D-10699 | 0042f8d0 | FUN_0042f8d0 | frontend | c3-batch-g-s3 REFUSED: callee FUN_00472c60 is C1 (RwIm2D filled-quad draw); C3 gate requires at-least-one-callee C2+. Also sig in batch-g manifest claimed uint32() but actual sig is void(float x1, float y1, float x2, float y2) — 4-float bordered-rect draw via 5x FUN_00472c60 calls + 2 global border-margin reads (_DAT_005cc574, _DAT_005cc35c). Pickup: promote FUN_00472c60 to C2 (re-read re/analysis/hud_frontend/0x00472c60.md, file analysis note with shape+globals), then re-queue 0x0042f8d0 with corrected vec4_floats arg_type | c3-batch-g-s3 |
| D-10700 | 0042f0c0 | FUN_0042f0c0 | frontend | c3-batch-g-s3 REFUSED: 800-byte options-row renderer with 8 callees; failures: (a) FUN_00473870 is C0 deferred — blocks C3 gate; (b) FUN_0040bb50 has conflicting C2(frontend) AND C1(boot) rows in hooks.csv — tracker drift must resolve first; (c) vtable indirection via DAT_007d3ff8+0x20 dispatcher with no resolved target; (d) sig in batch-g manifest claimed uint32(int) but actual sig is void(void) reading globals DAT_0067e7b0/b4/9f8/ea8c/etc — would need read_global+observe_writes harness. Pickup chain: promote 0x00473870 to C2, resolve 0x0040bb50 dup-row, resolve vtable DAT_007d3ff8 dispatcher, then re-queue as C2->C3 with corrected sig | c3-batch-g-s3 |
