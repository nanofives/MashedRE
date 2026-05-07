# DEFERRED.md
<!-- Mutate only via re-classify skill. Hand-edits drift. -->
<!-- Format: | D-ID | RVA | name | subsystem | pickup_condition | bucket |  -->

| ID | RVA | Name | Subsystem | Pickup condition | Bucket |
|---|---|---|---|---|---|
| D-2985 | 0045efe0 | FUN_0045efe0 | audio | Decompile and confirm pattern; classify per-ch SFX dispatcher | audio_sfx_dispatch-cont1 |
| D-2986 | 0045f5f0 | FUN_0045f5f0 | audio | Decompile and confirm pattern; classify per-ch SFX dispatcher | audio_sfx_dispatch-cont1 |
| D-2987 | 0045faa0 | FUN_0045faa0 | audio | Decompile and confirm pattern; classify per-ch SFX dispatcher | audio_sfx_dispatch-cont1 |
| D-2988 | 0045ff50 | FUN_0045ff50 | audio | Decompile and confirm pattern; classify per-ch SFX dispatcher | audio_sfx_dispatch-cont1 |
| D-2989 | 00460350 | FUN_00460350 | audio | 2715-byte dispatcher; likely per-vehicle engine loop; full decompile needed | audio_sfx_dispatch-cont1 |
| D-2990 | 00460df0 | FUN_00460df0 | audio | 2134-byte dispatcher; full decompile needed | audio_sfx_dispatch-cont1 |
| D-2991 | 00461650 | FUN_00461650 | audio | 2103-byte dispatcher; full decompile needed | audio_sfx_dispatch-cont1 |
| D-2992 | 00463640 | FUN_00463640 | audio | 1520-byte dispatcher; decompile and classify | audio_sfx_dispatch-cont1 |
| D-2993 | 00463c80 | FUN_00463c80 | audio | 694-byte dispatcher; decompile and classify | audio_sfx_dispatch-cont1 |
| D-2994 | 00463f40 | FUN_00463f40 | audio | 681-byte dispatcher; decompile and classify | audio_sfx_dispatch-cont1 |
| D-2995 | 00464e10 | FUN_00464e10 | audio | 1182-byte dispatcher; decompile and classify | audio_sfx_dispatch-cont1 |
| D-2996 | 00465a30 | FUN_00465a30 | audio | 227-byte; likely variant of 00465940 (4-slot resequenced SFX) | audio_sfx_dispatch-cont1 |
| D-2997 | 00465b20 | FUN_00465b20 | audio | 232-byte; likely variant of 00465940 (4-slot resequenced SFX) | audio_sfx_dispatch-cont1 |
| D-2998 | 004661f0 | FUN_004661f0 | audio | 1976-byte dispatcher; full decompile needed | audio_sfx_dispatch-cont1 |
| D-6460 | 004c1c10 | FUN_004c1c10 | util | 101B; callee of FUN_00441990 — takes *(param_1+0x84) and int 2; decompile and classify | profile_career_d4 |
| D-6461 | 004c3b30 | FUN_004c3b30 | util | 83B; sqrt-like float10 function; callee of FUN_00441990 and FUN_00446520 | profile_career_d4 |
| D-6462 | 00441760 | FUN_00441760 | util | 116B; camera apply — final call in FUN_00441990 and FUN_00442e00 and FUN_00446520 | profile_career_d4 |
| D-6463 | 0041ef60 | FUN_0041ef60 | util | 20B; called 4× per-player in FUN_00446520 entry loop; decompile | profile_career_d4 |
| D-6464 | 00442600 | FUN_00442600 | util | 316B; once-per-frame side-effect call in FUN_00446520; decompile | profile_career_d4 |
| D-6465 | 0046cb30 | FUN_0046cb30 | util | 125B; takes player index, outputs 3-float offset; callee of FUN_00446520 | profile_career_d4 |
| D-6466 | 00441820 | FUN_00441820 | util | 356B; camera path sample — takes (index, &pos_out, &float_out); callee of FUN_00446520 | profile_career_d4 |
| D-6467 | 00426bb0 | FUN_00426bb0 | util | 5B; returns int (path-length wrap count); callee of FUN_00446520 | profile_career_d4 |
| D-6468 | 00441700 | FUN_00441700 | util | 82B; death-match camera setup; callee of FUN_00446520 branch A | profile_career_d4 |
| D-6469 | 004464c0 | FUN_004464c0 | util | 91B; side-effect call in FUN_00446520 branch B; decompile | profile_career_d4 |
| D-6470 | 00442a60 | FUN_00442a60 | util | 530B; side-effect call in FUN_00446520 branch B; decompile | profile_career_d4 |
| D-6471 | 004a3384 | FUN_004a3384 | util | 8B; acos-like double→float10; callee of FUN_00446520 | profile_career_d4 |
| D-6472 | 004a3620 | FUN_004a3620 | util | 78B; atan-like float→float10; callee of FUN_00446520 | profile_career_d4 |
| D-6473 | 00405890 | FUN_00405890 | util | 25B; mode-5 spectator predicate; callee of FUN_00446520 branch A | profile_career_d4 |
| D-6474 | 00407600 | FUN_00407600 | util | 28B; returns float* position for mode-5; callee of FUN_00446520 branch A | profile_career_d4 |
| D-6475 | 0041f120 | FUN_0041f120 | util | 149B; per-player loop setup — takes (index, 0); callee of FUN_00446520 | profile_career_d4 |
| D-6476 | 004427c0 | FUN_004427c0 | util | 597B; else-branch camera handler in FUN_00446520; decompile | profile_career_d4 |
| D-6477 | 00442a20 | FUN_00442a20 | util | 50B; called if *param_1!=0 at end of FUN_00446520; decompile | profile_career_d4 |
| D-6478 | 004a3700 | FUN_004a3700 | util | 19B; returns random/time float; called 9× in jitter loop; decompile | profile_career_d4 |
| D-6479 | 004a37b0 | FUN_004a37b0 | util | 19B; time-based float getter; callee of FUN_00446520 branch A | profile_career_d4 |
| D-6480 | 004b4430 | FUN_004b4430 | util | 273B; bezier frame orientation setter; callee of FUN_00446520 branch A | profile_career_d4 |
| D-6481 | 004b4cd0 | FUN_004b4cd0 | util | 56B; bezier path query; callee of FUN_00446520 branch A | profile_career_d4 |
| D-6482 | 0045bfe0 | FUN_0045bfe0 | util | 5B; bezier locate; callee of FUN_00446520 branch A | profile_career_d4 |
| D-6483 | 0045c350 | FUN_0045c350 | util | 302B; bezier interpolate; callee of FUN_00446520 branch A | profile_career_d4 |
| D-7120 | 004ba1b0 | FUN_004ba1b0 | input | Lua allocator stub (S-2401): called with args (0,0,0x70); decompile and classify | input_lua_d3 |
| D-7121 | 004b7be0 | FUN_004b7be0 | input | Lua stub (S-2402): receives (block, label_addr, stack_slot); failure triggers FUN_004b7480; decompile | input_lua_d3 |
| D-7122 | 004ba210 | FUN_004ba210 | input | Lua stub (S-2408): called with (param_1, 1); decompile and classify | input_lua_d3 |
| D-7123 | 004b9850 | FUN_004b9850 | input | Lua stub (S-2409): called with (param_1); decompile and classify | input_lua_d3 |
| D-7124 | 004b64e0 | FUN_004b64e0 | input | Lua stub (S-2410): wrapper — receives (param_1, 0, param_2); decompile and classify | input_lua_d3 |
