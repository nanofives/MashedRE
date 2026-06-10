# SCRIBE_QUEUE fragment — batch_ah session 1 (ah_s1)

Author-only promote-c2 pass. Bucket plates are the C2 deliverable; central
finalize (ghidra-sweep) writes hooks.csv / trackers and commits.

## Queued

2026-06-01  ah_s1  bucket=re/analysis/bucket_audio_0042f760_00465b20  confidence=C1->C2  rvas=0042f760,0042f770,0042f780,00431b20,00432230,00432260,0045c6b0,0045c720,0045c7f0,0045c820,0045cc50,0045cc80,0045cd30,0045cfe0,0045d250,0045d460,0045da60,0045dc80,0045dd60,0045e040,0045e0f0,0045e460,0045e990,0045efe0,0045f5f0,0045faa0,0045ff50,00460270,00460350,00460df0,00461650,004623e0,004624c0,00462520,004625b0,004627b0,004627f0,00462dd0,00462ec0,00462f50,00463150,004631f0,004633d0,00463590,00463640,00463c80,00463f40,004642f0,004644a0,004645c0,00464670,004647f0,004648b0,00464a50,00464ca0,00464e10,004652b0,00465940,00465a30,00465b20

## Notes for the sweep

- **Count**: 60 RVAs, 60 plates authored in the bucket dir. None drift-skipped
  (all were C1 in hooks.csv at session start; none already C2+).
- **Subsystem confirmation**: all 60 CONFIRMED `audio` (no reclassifications).
  Every function touches the audio voice pool (`DAT_0068f640` 8-byte virtual
  voices), the per-frame channel-slot base (`DAT_0088e670`), RenderWare RWS-audio
  library calls (`FUN_005a6xxx`/`FUN_005a7xxx`/`FUN_005a8xxx`/`FUN_005b8xxx`/
  `FUN_005baxxx`), the music channel (`DAT_00605db8`), and/or the master
  audio-disable flag `DAT_0068f4c8`. The "audio" hypothesis in the prompt holds
  for the whole bucket.
- **Vendored-library callees** (RtFSManager / RWS-audio engine) are referenced
  but NOT deep-plated: FUN_005a5f00/005a6110/005a6280/005a66d0/005a6710/005a6d60/
  005a6dc0/005a71f0/005a79a0/005a7aa0/005a7af0/005a7b60/005a7f70/005a89a0/005a8960/
  005a8890/005a8e70/005a9e40/005aa060/005aaa00/005b8570/005b9e30/005baf00/005baf40/
  005baf60/005bbb20 and RtFSManager FUN_00550430/00550be0/00551190. These are
  RenderWare audio-toolkit entry points (library band); plates tag them as
  audio-lib callees per-call, consistent with existing DEFERRED rows
  (D-7380/D-7383/D-7392) and U-2488/U-2489/U-2491.
- **New uncertainties filed this session** (range U-6400..U-6499):
  - U-6400 — FUN_0045c6b0 consumes `extraout_EDX` as the per-player index, set by
    callee FUN_0045c640; the EDX-output contract of FUN_0045c640 is unverified
    (callee not decomped this pass).
  - U-6401 — FUN_0045cd30's `FUN_0045c820` call site consumes a 64-bit EDX:EAX
    value (`>>0x20` = pointer, low = slot) but FUN_0045c820's decomp shows only an
    EAX int return and never loads EDX with a pointer; EDX-output contract
    unverified.
  Both are data/calling-convention semantics, NON-BLOCKING for C2 of the
  bit-identical leaves.
- **No new S-IDs** minted. Several plates note they resolve pre-existing stubs
  (S-0620..S-0631, S-0622..S-0625, etc.) per the existing hooks.csv rows — leave
  stub resolution to the central sweep.
- **DEFERRED touchpoints** carried (not resolved): D-8260 (0045cd30 nested
  route-type switch), D-7380 (0045cd... device-state), D-7383 (0045/00462ec0
  DestroyStreams), D-7392 (004624c0 RtFSManagerRegister), D-10346/10349/10352/
  10353 (0045dc80/00462520/004647f0/004648b0).
- Files left UNTRACKED (bucket dir + this fragment + .pool_slot_ah_s1). No git
  commit, no re-classify, no build, no Frida — per author-only mission.
