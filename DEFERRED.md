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
| D-0017 | 0x004a2c2f FUN_004a2c2f | depth-2 of FUN_004a31f3 | D-0001 re-pickup | boot |
| D-0018 | 0x004a40fe ___onexitinit | depth-2 of FUN_004a31f3 | D-0001 re-pickup | boot |
| D-0019 | 0x004a415e _atexit | depth-2 of FUN_004a31f3 | D-0001 re-pickup | boot |
| D-0020 | 0x004a57e4 FUN_004a57e4 | depth-2 of FUN_004a31f3 | D-0001 re-pickup | boot |
| D-0021 | 0x004a3258 FUN_004a3258 | depth-2 of FUN_004a332b + FUN_004a334d | D-0001 re-pickup | boot |
| D-0022 | 0x004a333c __exit | depth-2 of __amsg_exit | D-0001 re-pickup | boot |
| D-0023 | 0x004ab8d6 FUN_004ab8d6 | depth-2 of __amsg_exit + fast_error_exit | D-0001 re-pickup | boot |
| D-0024 | 0x004aba4d __FF_MSGBANNER | depth-2 of __amsg_exit + fast_error_exit | D-0001 re-pickup | boot |
| D-0025 | 0x004a31b1 ___crtExitProcess | depth-2 of fast_error_exit | D-0001 re-pickup | boot |
| D-0026 | 0x004a467e _calloc | depth-2 of FUN_004a8a04 | D-0001 re-pickup | boot |
| D-0027 | 0x004a774d __mtinitlocks | depth-2 of FUN_004a8a04 | D-0001 re-pickup | boot |
| D-0028 | 0x004a87f7 FUN_004a87f7 | depth-2 of FUN_004a8a04 | D-0001 re-pickup | boot |
| D-0029 | 0x004aa3e4 ___heap_select | depth-2 of __heap_init | D-0001 re-pickup | boot |
| D-0030 | 0x004aa44f ___sbh_heap_init | depth-2 of __heap_init | D-0001 re-pickup | boot |
| D-0031 | 0x004af2b6 ___initmbctable | depth-2 of FUN_004abbea + __setenvp + FUN_004abe86 | D-0001 re-pickup | boot |
| D-0032 | 0x004affe0 FUN_004affe0 | depth-2 of FUN_004abbea | D-0001 re-pickup | boot |
| D-0033 | 0x004a45fb _malloc | depth-2 of __setenvp + FUN_004abe86 + ___crtGetEnvironmentStringsA + FUN_004ac04a | D-0001 re-pickup | boot |
| D-0034 | 0x004a460d _free | depth-2 of __setenvp + ___crtGetEnvironmentStringsA | D-0001 re-pickup | boot |
| D-0035 | 0x004a9410 _strlen | depth-2 of __setenvp | D-0001 re-pickup | boot |
| D-0036 | 0x004ac560 FUN_004ac560 | depth-2 of __setenvp | D-0001 re-pickup | boot |
| D-0037 | 0x004abd1a FUN_004abd1a | depth-2 of FUN_004abe86 | D-0001 re-pickup | boot |
| D-0038 | 0x004aaff0 _memcpy | depth-2 of ___crtGetEnvironmentStringsA | D-0001 re-pickup | boot |
| D-0039 | 0x004ae29f ___crtInitCritSecAndSpinCount | depth-2 of FUN_004ac04a | D-0001 re-pickup | boot |

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

## Conventions

- ID format: `D-NNNN`, monotonic, never reused.
- Re-pickup condition must be **observable** (a phase exits, a feature ships, a tool gains a capability) — not "later" or "when I feel like it."
- A DEFERRED row may reference S-NNNN or U-NNNN ids; in that case the original tracker entry stays, with a pointer to D-NNNN.
