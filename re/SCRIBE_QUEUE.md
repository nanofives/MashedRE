# Scribe Queue

Buckets queued for the parallel-fanout sweep. See `re/SESSION_RULES.md` § "Parallel-fanout scribe-queue pattern" for the protocol.

Format per row:

```
YYYY-MM-DD  <SESSION_SHORT_ID>  bucket=<bucket>  rvas=<comma-separated list>
```

The sweep session moves rows from "Queued" to "Drained" as it processes them. Drained rows are kept for audit. Queued rows are never deleted — only moved.

## Queued

```
2026-05-07  sky_weather_d2-20260507  bucket=sky_weather_d2  rvas=0x00499d90,0x00489240,0x0047a020,0x00410b30,0x004917a0,0x004917c0,0x00491860,0x00491900,0x004919b0,0x00491a10,0x00491a70,0x00451060  S-2820..S-2828  U-2827..U-2829  D-8380..D-8385  pool=Mashed_pool13
2026-05-03  replay_record-20260503  bucket=replay_record  rvas=0x00411350,0x00411530,0x00411580,0x004115c0,0x004114e0,0x00411600,0x00411750,0x004117b0,0x00411870,0x00411ae0,0x00411ce0,0x00411d60,0x00411d90,0x00411170,0x00482930,0x004829d0,0x00482c10,0x00483d10  S-1560..S-1573  U-1567..U-1569  D-4600..D-4607  pool=Mashed_pool10  HOLD=missing-per-rva-files (sweep-20260503-1853 skipped: only notes.md present)
2026-05-07  race_results_d2-20260507-1904  bucket=race_results_d2  rvas=0x004189f0,0x004215c0,0x0045ba00,0x0046c5c0,0x0046c790  S-2860..S-2861  U-2867..U-2872  D-8500..D-8501  D-3760..D-3768-cleared  pool=Mashed_pool2
```

## Drained

```
2026-05-07  physics_collision_d2-20260507-0106  bucket=physics_collision_d2  rvas=0x0047b9b0,0x00478cb0,0x004715a0  S-2640..S-2643  U-2647..U-2651  D-7840..D-7843  pool=Mashed_pool12  drained-by=sweep-20260507-0133; 3 plates, 3 bookmarks, 0 renames
2026-05-06  input_lua_d2-20260506-1854  bucket=input_lua_d2  rvas=0x0047b8a0,0x004b7330,0x004c0510,0x004b7480,0x004b6520  S-2400..S-2410  U-2407..U-2418  D-7120..D-7130  pool=Mashed_pool12  drained-by=sweep-20260506-2157; 5 plates, 5 bookmarks, 0 renames
2026-05-06  audio_music_d3-20260506-1930  bucket=audio_music_d3  rvas=0x005a7520,0x005a6d60  S-2560..S-2562  U-2567  D-7600..D-7602  pool=Mashed_pool12  drained-by=sweep-20260506-2157; 2 plates, 2 bookmarks, 0 renames
2026-05-06  track_loader_d4-20260506-2007  bucket=track_loader_d4  rvas=004b3c60,00558df0,004b3cc0,004b3de0,00479030,00474fb0,00474f30,0047f4c0,0047d080,0047d100,00487280,0047be80,0047bcc0,004b53b0,004c3d90,00546380  S-2480..S-2499  U-2487..U-2506  D-7360..D-7367  drained-from=queue.md/track_loader_d3-cont1  pool=Mashed_pool12  drained-by=sweep-20260506-2157; 16 plates, 16 bookmarks, 0 renames; 0x00479030 written as listing-level (no Ghidra function object — C0 plate)
2026-05-03  race_results-20260503-0655  bucket=race_results  rvas=0x0040eee0,0x00408a50,0x00408a70,0x00408ad0,0x0040b290,0x0040b6d0,0x0040d590,0x0040e470,0x00422fd0,0x0042f500,0x0042f6a0,0x00431d80,0x0046c700,0x0046c7b0  note=0x0040b6d0/0x0042f500/0x0042f6a0 plates preserved (skipped overwrite per row note)
2026-05-03  timer_d2-20260503-0656  bucket=timer_d2  rvas=0x004030d0,0x004111c0,0x0043d7c0,0x0043dfd0,0x0045d3f0,0x0045d430,0x00466b50,0x00496920,0x00496930,0x00498860,0x0049d1d0,0x004a4170,0x004a4220  S-1120..S-1139 U-1127..U-1146 D-3280..D-3283  note=0x0043dfd0 is C0 (decomp too large); U-0680 resolved by 0x0049d1d0; S+U ranges exhausted — any continuation needs extended ranges  pool=Mashed_pool15
2026-05-03  render_frame-20260503-0611  bucket=render_frame  rvas=0x004671c0  D-0880..D-0894  pool=Mashed_pool14
2026-05-03  hud_ingame_d2-20260503  bucket=hud_ingame_d2  rvas=0x004c1c80,0x00402fb0,0x00428760,0x0041c2d0,0x0041b340,0x0041bc50,0x0041c9a0,0x0041d410,0x0041de80,0x0041e630,0x00428610,0x004c0e50,0x0042b8b0,0x0042b8c0  stubs_cleared=S-0561..S-0572  deferred_added=D-2680  pool=Mashed_pool3
2026-05-03  track_loader_d2-20260503-0302  bucket=track_loader_d2  rvas=0x00426cd0,0x0042a8d0,0x0042f510,0x00462950,0x004715a0,0x00478660,0x00479330,0x0047a0f0,0x0047c0b0,0x0047c0f0,0x00480340,0x00491780,0x004924c0,0x00495280,0x004952f0,0x004c1b10  D1180_drained  S-0900..S-0919  U-0907..U-0912  D-2620..D-2645  pool=Mashed_pool13
2026-05-03  race_state-20260503-0600  bucket=race_state  rvas=0x0042c280,0x0042c2d0,0x0042c2e0,0x0042c2f0,0x00432080,0x004331a0,0x00448700,0x004927c0  U-1009/1010/1012 resolved; U-1073/1074/1075 added; S-1066..1070 D-3113..3117 registered
2026-05-03  exception_filter-20260502-2221  bucket=exception_filter  rvas=0x004af2d4,0x004af31a,0x004af32d
2026-05-03  memory_pool-20260502-2223  bucket=memory_pool  rvas=0x005208c0,0x00517250
2026-05-03  localization-20260502-2227  bucket=localization  rvas=0x004274d0,0x004274e0,0x004cc230,0x004cc160
2026-05-03  window_msgpump-20260502-2232  bucket=window_msgpump  rvas=0x00499820,0x00499690,0x00496470,0x00496490,0x004960e0
2026-05-03  timer-20260502-2221  bucket=timer  rvas=0x00495120,0x004950b0,0x00495110,0x004926c0,0x00492d30,0x005ab040,0x0049cfb0,0x0049d080,0x0049d240,0x0049d270,0x0049d9a0
2026-05-03  settings_config-20260502-2222  bucket=settings_config  rvas=0x004963e0,0x00496400,0x00498910,0x00498950,0x004989b0,0x00499400,0x004a4541
2026-05-03  render_d3d_reset-20260502-2221  bucket=render_d3d_reset  rvas=0x004c9cd0,0x004c9ad0,0x004dc970,0x004db3e0,0x004d1e30,0x004e0920,0x004d6200
2026-05-03  intro_splash-20260502-2247  bucket=intro_splash  rvas=0x00495350,0x00492d20,0x00493f70,0x00493f80,0x00493fc0,0x00493fd0,0x00494460,0x00494480,0x00494a80,0x004c19f0,0x004c1a00,0x004c1bb0,0x004c1be0
2026-05-03  render_lighting-20260502-2221  bucket=render_lighting  HALT=RpLight-anchors-absent  rvas=none  notes=no-scribe-writes-queued (skipped: no RVAs to drain)
2026-05-02  texture_loader-20260502-1900  bucket=texture_loader  rvas=0x0042a530,0x004b3d20,0x004b3d80
2026-05-02  effects_particle-20260502-2135  bucket=effects_particle  rvas=0x00490500,0x00472650,0x00472690,0x004769a0,0x004769d0,0x004769f0,0x00476a30,0x00476a40,0x00476d00,0x00476df0,0x0048ebc0,0x0048ebf0,0x0048f290,0x0048f420,0x0048fe70,0x004a2c48
2026-05-02  hud_ingame-20260502-2132  bucket=hud_ingame  rvas=0x0040dfc0,0x00403160,0x0041a3e0,0x0041b630,0x0041c0c0,0x0041c300,0x0041ccc0,0x0041d870,0x0041db80,0x0041ded0,0x0041e850,0x00426ba0,0x0042f500,0x0042f6a0
2026-05-02  camera_follow-20260502-2132  bucket=camera_follow  rvas=0x0040b090,0x0041e8c0,0x0041e9b0,0x0041e9e0,0x00426700,0x00426780,0x00426810,0x00426ab0,0x004671a0,0x00467210,0x00471ec0,0x0047c160,0x00491490
2026-05-02  audio_music-20260502-2145  bucket=audio_music  rvas=0x004623e0,0x0045da60,0x0045dd60,0x004631f0
2026-05-02  game_state-20260502-2144  bucket=game_state  rvas=0x004929d0,0x0040b430,0x0040b540,0x0040b6d0,0x0040b6e0,0x0040b700,0x0040d440,0x0040e340,0x0040e350,0x0040e450,0x0040e460,0x0042b8d0,0x0042b8e0,0x0042b8f0,0x0042b910,0x0042b930,0x0042b940,0x0042c1c0,0x0042c1d0,0x0042c220
2026-05-02  physics_collision-20260502-1900  bucket=physics_collision  rvas=0x00492e90,0x0047a020  notes=COLLISION_FN not found; BSP system mapped; Frida required for runtime path; D-1792 filed
2026-05-02  audio_dsound-20260502-1942  bucket=audio_dsound  rvas=0x005b9f30,0x005a9e10,0x005aee20,0x005ba1d0,0x005bad30
2026-05-02  render_d3d9_device-20260502-1856  bucket=render_d3d9_device  rvas=0x004c7a70,0x004c8650,0x004c8690,0x004c8740,0x004c8800,0x004c8c70,0x004c8e50,0x004cc820,0x004cc9f0,0x004dcf90,0x004dcff0,0x004dd050
2026-05-02  boot_app_init-20260502-1724  bucket=boot_app_init  rvas=0x00402750,0x00402a40,0x00492270,0x00492290,0x004924f0,0x00493540,0x00493550,0x00493560,0x00493900,0x004963e0,0x004996f0,0x00499ba0,0x00499cc0,0x004c5930,0x005c9d00
2026-05-02  boot_crt_exit-20260502-1733  bucket=boot_crt_exit  rvas=0x004a2c2f,0x004a3258,0x004a31b1,0x004a333c,0x004a40fe,0x004a415e,0x004a467e,0x004a57e4,0x004a774d,0x004a87f7,0x004aa3e4,0x004aa44f,0x004ab8d6,0x004aba4d
2026-05-02  rw_engine_teardown-20260502-1440  bucket=rw_engine_teardown  rvas=0x004938c0,0x00558470,0x00550390,0x004c2f60,0x004c3040,0x004c3270
2026-05-02  rw_engine_init-20260502-1734  bucket=rw_engine_init  rvas=0x00493710,0x0045b350,0x00493600,0x00493640,0x004951e0,0x004951f0,0x00495270,0x004963e0,0x004a2cbd,0x004c2ed0,0x004c2f00,0x004c2fb0,0x004c3040,0x004c30b0,0x004c3270,0x004c32b0,0x004c5c80,0x004c9eb0
2026-05-02  boot_crt_env-20260502-1734  bucket=boot_crt_env  rvas=0x004a45fb,0x004a460d,0x004a9410,0x004aaff0,0x004abd1a,0x004ac560,0x004ae29f,0x004af2b6,0x004affe0
2026-05-02  input_dinput-20260502-1855  bucket=input_dinput  rvas=0x00495530,0x004987b0
2026-05-02  rw_engine_teardown_d2-20260502-1854  bucket=rw_engine_teardown_d2  rvas=0x004c2c90,0x004ccf20,0x004d7ca0,0x004d8060,0x00551510
2026-05-02  rw_engine_init_d2-20260502-1905  bucket=rw_engine_init_d2  rvas=0x00472380,0x00496400,0x00498b60,0x00498bf0,0x00499400,0x00499710,0x004a2d18,0x004a504f,0x004c2c90,0x004c7a60,0x004c9f50,0x004c9f60,0x004cae90,0x004cbb60,0x004cbc60,0x004cbc70,0x004cbc80,0x004cbc90
2026-05-02  audio_rws_loader-20260502-1838  bucket=audio_rws_loader  rvas=0x005a7b60,0x004522d0,0x004cbd30,0x004cc050,0x005a79a0,0x005a7b40,0x005a7b50,0x005a7ee0,0x005ab380,0x005ab410,0x005abcf0,0x005abfa0,0x005ade10,0x005aea00,0x005aea10,0x005aea40,0x005aec00
2026-05-02  input_lua-20260502-1902  bucket=input_lua  rvas=0x0047b9b0,0x0047b860,0x0047b880,0x0047b8d0
2026-05-02  boot_crt_exit_d3-20260502-1854  bucket=boot_crt_exit_d3  rvas=0x004a2be9,0x004a2bf7,0x004a3440,0x004a34b0,0x004a4126,0x004a4728,0x004a5984,0x004a59bf,0x004a5de3,0x004a5e35,0x004a5f07,0x004a7796,0x004a77eb,0x004a787f,0x004aac76,0x004aaf72,0x004aaf90,0x004ac45c
2026-05-02  save_gamesave-20260502-1854  bucket=save_gamesave  rvas=0x00404e50,0x00404f50,0x00404f80,0x004b3b70,0x004b3bb0,0x004099e0
2026-05-02  boot_app_init_d3-20260502-1859  bucket=boot_app_init_d3  rvas=0x004014b0,0x004015a0,0x004025f0,0x004026d0,0x00402f50,0x00403640,0x00403660,0x00404830,0x0040bb30,0x0040bb50,0x0040bbb0,0x0040bd00,0x0040cf80,0x0040cfd0,0x004113b0,0x004114c0,0x00418980,0x004189e0
2026-05-02  powerups-20260502-1946  bucket=powerups  rvas=0x0045bae0,0x0045ba10,0x0042a6b0,0x00453f30,0x004548e0,0x00456760,0x004587a0,0x00459290,0x0047b9b0,0x00411f30,0x004039c0
2026-05-02  hud_frontend-20260502-1944  bucket=hud_frontend  rvas=0x0043c5b0,0x0040bb50,0x00427e00,0x00428140,0x0042aad0,0x0042aae0,0x0042b930,0x0042e3a0,0x0042e5b0,0x0043bf30,0x00472c60,0x00472dc0,0x00473540,0x004739f0,0x004a2c48
2026-05-02  ai_update-20260502-1952  bucket=ai_update  rvas=0x00418560,0x00407a40,0x0040e350,0x0040e4a0,0x00413fe0,0x00416250,0x00416a30,0x00417180,0x00417640,0x00417da0,0x00426c00
2026-05-02  video_mci-20260502-1943  bucket=video_mci  rvas=0x00493b50,0x00493bc0,0x00493c00,0x00493f00,0x004944b0,0x004944c0,0x00494ac0,0x00494c80,0x004a1790
2026-05-02  track_loader-20260502-1943  bucket=track_loader  rvas=0x00426e10,0x004053d0,0x00409680,0x00409710,0x00412050,0x0041e870,0x0041e8b0,0x0041e970,0x0041e980,0x0041e9d0,0x0041ea90,0x004235b0,0x00423630,0x00425ab0,0x004260e0,0x004262f0
```
2026-05-03  random_rng-20260503-0601  bucket=random_rng  rvas=0x00534920,0x004b44f0,0x004b4510
2026-05-03  render_frame_tree_d2-20260503-0700  bucket=render_frame_tree_d2  rvas=0x004d80d0,0x004d8280,0x004d8300,0x004c0e50  notes=FRAME_UPDATE_FN(_rwFrameSyncDirty) found; D-2080 resolved; D-3460 (FUN_004c4600 matrix_math) filed; U-1187 U-1188 U-1189 added
2026-05-03  race_state_d2-20260503-1737  bucket=race_state_d2  rvas=0x0042bf30,0x00496900,0x0042d3a0,0x004248b0  D-3113/3115/3116 analyzed; D-3117 corrected typo 004348b0→004248b0 then analyzed; D-3114 already mapped (race_results); U-1507..U-1511 added; S-1500 added; S-1066/1067 cleared S-1068 partial; no D-4420..4479 entries (all leaf functions)  pool=Mashed_pool7
2026-05-03  audio_dsound_d2-20260503-1735  bucket=audio_dsound_d2  rvas=0x005ba720,0x005ba760,0x005ba780,0x005ba7f0,0x005bac00,0x005bb000,0x005bbc10,0x005bbdb0,0x005bbf30  note=D-0941/D-0942 skipped (already C1 in audio_rws_loader_d2); U-range exhausted (U-1367..U-1386 full); pool=Mashed_pool1
2026-05-03  game_mode-20260503-1622  bucket=game_mode  rvas=0x0043dfd0  note=plate-skipped:C0 (preserved prior timer_d2 plate "decomp too large"); bookmark added; library-rename skipped (no "Library Function:" prefix); via sweep-20260503-1853
2026-05-05  vehicle_damage_d2-20260505  bucket=vehicle_damage_d2  rvas=0x0040e180,0x00410d10,0x00442df0,0x0046cbb0,0x004922e0  S-1840..S-1842  U-1847..U-1860  D-5440..D-5446  D-1548-cleared  pool=Mashed_pool2  drained-by=sweep-20260506-0458; 5 plates, 5 bookmarks, 0 renames
2026-05-03  vehicle_dynamics-20260503-0000  bucket=vehicle_dynamics  rvas=0x0046e9e0,0x0046d700,0x004c4680
2026-05-03  timer_d2_cont1-20260503-1824  bucket=timer_d2_cont1  rvas=0x0040ab40,0x0040ac80,0x0040b810,0x0040de10,0x0040e360,0x0040e370,0x00422b30,0x00429aa0,0x0042af50,0x0042b900,0x0042b950,0x0042c150,0x00431b50,0x00431b60,0x00432290,0x0045c480,0x0045d3a0,0x0045d7a0
2026-05-06  video_mci_d3-20260506-0512  bucket=video_mci_d3  rvas=0x0049dd60
2026-05-06  hud_frontend_d3-20260506-0511  bucket=hud_frontend_d3  rvas=0x0040ad20,0x0040b460,0x0040b620,0x0040b6b0,0x0040b6c0,0x0040b7a0,0x0040b7b0,0x0040bb70,0x0040bb90,0x0040e3a0,0x00427ad0,0x004282a0,0x00428320,0x00429870,0x00429a30,0x00429a70,0x00429a80,0x00429a90  S-2087,S-2089,S-2090  U-2087..U-2095  D-6160..D-6184  pool=Mashed_pool12
2026-05-06  launch_handshake-20260506  bucket=launch_handshake  rvas=0x000955d0,0x00095780  U-0070=resolved  pool=Mashed_pool15  note=queue used bare offsets; sweep-20260506-0544 corrected to MASHED.exe VAs 0x004955d0 and 0x00495780 before writing
2026-05-06  powerups_d3-20260506-0504  bucket=powerups_d3  rvas=0x00454170,0x004547c0,0x00534b60,0x004c0790,0x004c0870,0x004c0a60,0x004c0de0,0x004d8060,0x004e4440,0x004e4d90,0x004e68a0,0x004e6710,0x004e6920,0x004e6d00,0x004e6d80,0x004e6f80,0x004e6fe0  S-1920..S-1930  U-1927..U-1945  D-5680..D-5686  pool=Mashed_pool4  drained-by=sweep-20260506-0544; 17 plates, 17 bookmarks (0x004d8060 plate overwrote prior rw_engine_teardown_d2 plate)
2026-05-06  profile_career_d3-20260506-1249  bucket=profile_career_d3  rvas=0x00441990,0x00446520,0x00429a70,0x0046b4f0,0x0046d510,0x00442e00  S-2180..S-2192  U-2187..U-2197  D-6460..D-6483  pool=Mashed_pool3  drained-by=sweep-20260506-1326; 6 plates, 6 bookmarks
2026-05-06  vehicle_damage_d3-20260506-1244  bucket=vehicle_damage_d3  rvas=0x00405890,0x00408a50,0x00408a70,0x0040e340,0x0040e350,0x00417730,0x00423b20  drained-by=sweep-20260506-1326; 7 plates, 7 bookmarks; 0x00408a50 and 0x00408a70 plates overwrote prior race_results plates
2026-05-06  localization_d2-20260506  bucket=localization_d2  rvas=0x0042c300,0x0042c3c0,0x0042c7c0,0x00429660,0x0040dc80,0x0040dc90,0x00429620,0x00429300  U-2267..U-2279  D-6700..D-6703  pool=Mashed_pool5
2026-05-06  save_gamesave_d2-20260506-1508  bucket=save_gamesave_d2  rvas=0x004cbe80,0x00550b00
2026-05-06  options_menu-20260506  bucket=options_menu  rvas=0x00431d90,0x00431f30,0x0043df00  S-2380  U-2387..U-2389  D-7060  pool=Mashed_pool0  drained-by=sweep-20260506-1624; 3 plates, 3 bookmarks
2026-05-06  memory_pool_d2-20260506-1512  bucket=memory_pool_d2  rvas=0x005c1ea0  S-2360..S-2363  U=none  D-7000..D-7003  pool=Mashed_pool10  drained-by=sweep-20260506-1624; 1 plate, 1 bookmark; library_match=_longjmp (already named in Ghidra, no rename needed)
2026-05-06  intro_splash_d2-20260506  bucket=intro_splash_d2  rvas=0x004c75e0,0x00494320,0x004c7650,0x004942b0,0x004938e0,0x004c77c0,0x004c7730  S-0814..S-0820-resolved  S-2340..S-2341  U-2347..U-2361  D-2321..D-2327-cleared  D-6940..D-6941  pool=Mashed_pool9  drained-by=sweep-20260506-1624; 7 plates, 7 bookmarks
2026-05-06  audio_sfx_dispatch-20260506  bucket=audio_sfx_dispatch  rvas=0x004669b0,0x0045d460,0x004627f0,0x004625b0,0x00462dd0,0x005a6710,0x0045e040,0x00466a50  U-2487..U-2494  D-7360..D-7375  pool=Mashed_pool4
2026-05-07  split_screen_d2-20260507-1852  bucket=split_screen_d2  rvas=0x0041f8f0,0x004228f0,0x00426060,0x004260c0,0x004e4900  U-2847..U-2848  D-5620..D-5624-cleared  D-8440..D-8446  pool=Mashed_pool1
