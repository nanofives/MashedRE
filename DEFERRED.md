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
| D-0002 | 0x00402750 FUN_00402750 | depth-2 of FUN_00492370 | D-0001 re-pickup | boot |
| D-0003 | 0x00402a40 FUN_00402a40 | depth-2 of FUN_00492370 | D-0001 re-pickup | boot |
| D-0004 | 0x00492270 FUN_00492270 | depth-2 of FUN_00492370 | D-0001 re-pickup | boot |
| D-0005 | 0x00492290 FUN_00492290 | depth-2 of FUN_00492370 | D-0001 re-pickup | boot |
| D-0006 | 0x004924f0 FUN_004924f0 | depth-2 of FUN_00492370 | D-0001 re-pickup | boot |
| D-0007 | 0x00493540 thunk_FUN_00495150 (→0x00495150) | depth-2 of FUN_00492370 | D-0001 re-pickup | boot |
| D-0008 | 0x00493550 thunk_FUN_004938c0 (→0x004938c0) | depth-2 of FUN_00492370 | D-0001 re-pickup | boot |
| D-0009 | 0x00493560 thunk_FUN_004954f0 (→0x004954f0) | depth-2 of FUN_00492370 | D-0001 re-pickup | boot |
| D-0010 | 0x00493900 FUN_00493900 | depth-2 of FUN_00492370 | D-0001 re-pickup | boot |
| D-0011 | 0x004963e0 FUN_004963e0 | depth-2 of FUN_00492370 | D-0001 re-pickup | boot |
| D-0012 | 0x004996f0 FUN_004996f0 | depth-2 of FUN_00492370 | D-0001 re-pickup | boot |
| D-0013 | 0x00499ba0 FUN_00499ba0 | depth-2 of FUN_00492370 | D-0001 re-pickup | boot |
| D-0014 | 0x00499cc0 FUN_00499cc0 | depth-2 of FUN_00492370 | D-0001 re-pickup | boot |
| D-0015 | 0x004c5930 FUN_004c5930 | depth-2 of FUN_00492370 | D-0001 re-pickup | boot |
| D-0016 | 0x005c9d00 FUN_005c9d00 | depth-2 of FUN_00492370 | D-0001 re-pickup | boot |
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
|    |       |         |      |

## Conventions

- ID format: `D-NNNN`, monotonic, never reused.
- Re-pickup condition must be **observable** (a phase exits, a feature ships, a tool gains a capability) — not "later" or "when I feel like it."
- A DEFERRED row may reference S-NNNN or U-NNNN ids; in that case the original tracker entry stays, with a pointer to D-NNNN.
