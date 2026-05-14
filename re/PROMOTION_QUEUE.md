# Promotion Queue

Rows queued for the parallel-fanout C3 promotion sweep. Mirrors the format of
`re/SCRIBE_QUEUE.md`. Each row is appended by a `promote-c3-batch` session at
end-of-session; the user (or a sweep, if needed) merges branches and confirms
the integration build.

Format per row:

```
YYYY-MM-DD  c3-batch-<id>-s<N>  rvas=0x<a>,0x<b>,...  branch=c3/batch-<id>-s<N>  evidence=log/diff_<hook>.csv;...  note=<one-line>
```

The sweep (or user-driven merge) moves rows from "Queued" to "Merged".

## Queued

```
2026-05-14  c3-batch-o-s88  rvas=0x0046c7b0,0x0046dbe0,0x00468b40,0x0046c770,0x0046c6d0,0x0046d700,0x00417730  branch=c3/batch-o-s88  evidence=log/diff_vehicle_slot_getter.csv;log/diff_vehicle_race_position_get.csv;log/diff_vehicle_contact_history_lookup.csv;log/diff_vehicle_destruction_state_getter.csv;log/diff_vehicle_entity_slot_read.csv;log/diff_vehicle_vec3_at_9c8_get.csv;log/diff_vehicle_race_angle_get.csv  note=7 vehicle leaf C2->C3; harness extended for out3_idx/idx_out2 arg types; 0x0046cbb0 refused (callers C1+structural U open); all 7 A/B GREEN path2 PASS
2026-05-13  c3-batch-o-s89  rvas=0x0042f6a0,0x005c9d00,0x0041f1c0,0x0041f090  branch=c3/batch-o-s89  evidence=log/diff_get_race_sub_mode.csv;log/diff_get_race_end_trigger.csv;log/diff_get_event_flag.csv;log/diff_get_player_state_bits.csv  note=4 util pure-function C2->C3 (0x00413f90 refused: callers C1 only); harness extended for void/int/ptr2_out arg types
2026-05-13  c3-batch-o-s85  rvas=0x005aea10,0x005aea40,0x005aec00,0x005aee20,0x005aec30  branch=c3/batch-o-s85  evidence=log/diff_audio_aligned_alloc.csv;log/diff_audio_aligned_free.csv;log/diff_audio_byte_reverse.csv;log/diff_audio_bit_scan_forward.csv;log/diff_audio_byte_swap_buffer.csv  note=5 audio leaf C2->C3; harness extended for alloc_check/free_via_alloc/bytes_inplace/bytes_inplace_3/uint32_scalar; all diffs GREEN
2026-05-14  c3-batch-o-s86  rvas=0x004c3730,0x004c3880,0x004c3bf0,0x004c3c60,0x004c5010  branch=c3/batch-o-s86  evidence=log/diff_rw_v3d_transform_point.csv;log/diff_rw_v3d_transform_vector.csv;log/diff_vec2_length.csv;log/diff_vec2_normalize.csv;log/diff_rw_matrix_scale.csv  note=5 render math C2->C3; harness extended for transform_point/transform_vector/vec2_ptr/vec2_normalize/matrix_scale; all diffs GREEN (59 total cases)
2026-05-14  c3-batch-a-s1  rvas=0x00422b30,0x0040b810  branch=c3/batch-a-s1  evidence=log/diff_timer_array_clear.csv;log/diff_timer_globals_reset.csv  note=2 timer_d2_cont1 leaf C2->C3; 0x0042af50 refused (no caller at C2+, Frida A/B on file); 0x00429aa0 refused (all callees C1); 0x0040e470 drift C1->C2
2026-05-14  c3-batch-a-s2  rvas=0x0042b930  branch=c3/batch-a-s2  evidence=log/diff_menu_alpha_get.csv  note=1 frontend_menus_a small leaf C2->C3; 0x0042ac50 refused (EAX ABI); 0x0042ac00/0x0042aa00 deferred (runtime cannot reach menu state)
2026-05-14  c3-batch-a-s3  rvas=0x0042ac90,0x0042bb60,0x0042aff0,0x0042b180  branch=c3/batch-a-s3  evidence=log/diff_menu_entry_get.csv;log/diff_menu_team_balance.csv;log/diff_menu_button_detect_a.csv;log/diff_menu_button_detect_b.csv  note=4 frontend_menus_a medium C2->C3; 1 drift-promote (0x0040e470 C1->C2); MenuTeamBalance/DetectA/DetectB GREEN 10/10; MenuEntryGet identical-crash both sides (deferred re-verify when runtime unblocked)
2026-05-14  c3-batch-a-s4  rvas=0x00430b60,0x0042f6b0,0x00430910  branch=c3/batch-a-s4  evidence=log/diff_MenuSlotCount.csv;log/diff_MenuModeSync.csv;log/diff_MenuOptionSlotGet.csv  note=3 frontend_c0_promote leaves C2->C3; 0x0042f020 refused (EAX ABI unsupported in harness)
2026-05-14  c3-batch-a-s5  rvas=0x004323c0,0x0046dc00,0x00492340  branch=c3/batch-a-s5  evidence=log/diff_MenuCursorBack.csv;log/diff_EntityFieldSet.csv;log/diff_CarSlotInit.csv  note=3/4 frontend_c0+game_mode_cont2 C2->C3; 0x004307a0 escalated (callees 0x004a2c48/0x00429a70/80/90 confirmed C1, needs drift-promote batch)
2026-05-14  c3-batch-a-s6  rvas=0x0042b770,0x0042b310,0x0042b960,0x0042b9e0  branch=c3/batch-a-s6  evidence=log/diff_menu_button_detect_e.csv;log/diff_menu_button_detect_c.csv;log/diff_car_slot_init_1p.csv;log/diff_car_slot_assign.csv  note=4/5 frontend_menus_a+game_mode C2->C3; 3 drift-promotes (0x0040e470/80/0x00431b80 C1->C2); 0x00431d00 skipped (FUN_00431b80 infinite loop ESI=0 at quiescent state, deferred until runtime unblocked)
```

## Merged

```
```
