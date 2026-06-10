# Scribe Queue fragment — batch_ah Session 5 (audio C1→C2 promote-c2, AUTHOR-ONLY)

Generated 2026-06-01. Pool slot: Mashed_pool13 (read-only clone; .pool_slot_ah_s5).
For central finalize: cat this row into re/SCRIBE_QUEUE.md "## Queued" section.

## Queued

2026-06-01  batch-ah-s5  bucket=re/analysis/bucket_audio_005af070_005b2190  rvas=005af070,005af170,005af180,005af200,005af230,005af260,005af2d0,005af300,005af430,005af470,005af510,005af600,005af690,005af700,005af740,005af7a0,005af7d0,005af860,005af8f0,005afa00,005afcf0,005b0360,005b03e0,005b04b0,005b0700,005b0740,005b0970,005b09c0,005b0a90,005b0b10,005b0b40,005b0b60,005b0b90,005b0bb0,005b0c70,005b0ca0,005b0cf0,005b0dc0,005b0df0,005b0e70,005b0ec0,005b0f10,005b0f40,005b0f90,005b1030,005b1080,005b10a0,005b10d0,005b10e0,005b1110,005b1140,005b1160,005b1180,005b11d0,005b11f0,005b1260,005b1500,005b15d0,005b2080,005b2190  note=60/60 plated C1->C2; 0 drift-skips (all 60 were C1 at author time); 0 LAB_/function_create needed (every RVA had a boundary, incl. the two thunks); 0 stop-and-ask (function_at + decomp_function resolved every RVA).

## SUBSYSTEM RECLASSIFICATIONS (subsystem_observed vs hooks.csv)

NONE. The "audio" hypothesis held for all 60 RVAs — no reclassification required. This
band (0x005af070..0x005b2190) is a single self-contained **RenderWare audio sequencer /
RWS-stream** subsystem. Confirmation anchors (cited, not inferred):
  - FUN_005b1260 embeds the literal `s_rws_1_00634280` (".rws" stream-type string) and
    rewrites `'/'→'\\'` in a path before probing the RW manager
    (`(*(DAT_007d3ff8+0xf0))(h, &PTR_DAT_00634288)` / `(.., s_rws_1)`) — a RenderWare
    audio-stream (`.rws`) loader.
  - Master already carries [C1 2026-05-19] head-comments naming several: FUN_005b2080
    "Audio-stream tick / pump", FUN_005b2190 "Audio-source command submitter",
    FUN_005b1500 "Destructor for the FUN_005b1260 object".

## Structural map (for the sweep — optional master renames)

- **Object ctor / lifecycle**: FUN_005af070 (ctor, two intrusive lists @+0x18/+0x20,
  dispatch pair @+0x28/+0x2c, 1.0f @+0x34, state @+0x08), FUN_005b0b90 (descriptor
  default-init).
- **List walkers / visitors**: FUN_005af180 (+0x20 list w/ type filter FUN_005adf30 vs
  DAT_005e6ce0), FUN_005af200 (+0x38 chain), FUN_005af230 (+0x18 list), FUN_005af260
  (orchestrator, visitor LAB_005af2a0), FUN_005af700 (O(n/2) list-index).
- **State aggregators**: FUN_005af300, FUN_005af430, FUN_005af470 (return-code lattice
  {0,2,3,4,5,6,7,8,9}); FUN_005af2d0 (3-property snapshot).
- **Tick / command dispatch**: FUN_005af510 (flag-word dispatcher; bits 4/8/0x10/0x20/
  0x40/0x80) → handlers FUN_005af600/af690/af7a0/af7d0/af740; FUN_005af8f0 (start-up
  state ladder 2→3→4→5, teardown→6).
- **Command-record builder API** (raise the SAME bits FUN_005af510 consumes):
  FUN_005b0c70 (bit4), FUN_005b0ca0 (bit8), FUN_005b0cf0 (bit0x10, strided table marker),
  FUN_005b0dc0 (bit0x80), FUN_005b0f10 (wildcard record default-init).
- **Child build / collection**: FUN_005afa00 (source resolve + active-child ctor —
  decompiler type-prop unsettled), FUN_005afcf0 (large voice-build, ~1.6KB),
  FUN_005b0360 / FUN_005b03e0 (match) / FUN_005b04b0 / FUN_005b0740 (recursive resolver)
  / FUN_005b0700 (min-key pick); collection node alloc/free FUN_005b0a90 / FUN_005b0b10 /
  FUN_005b0b40 / FUN_005b0b60 (link @+0x4c); FUN_005af860 / FUN_005b0970 (submit via
  FUN_005aa560); FUN_005b0bb0 (default channel×group gain matrix, 1.0f).
- **vtable dispatch primitives**: FUN_005b10a0 (table @+0x04) + thunk_FUN_005b10a0
  (0x005b10d0), FUN_005b10e0 (table @+0x08); wrappers FUN_005b1110 (get) / FUN_005b1140
  (set) / FUN_005b1030 (dual notify); FUN_005b1080 ({obj,fn,arg} invoker).
- **Ring buffer**: FUN_005b1160 (init), FUN_005b1180 (read-advance, stride 0x14),
  FUN_005b11d0 (head-advance), FUN_005b11f0 (object ctor w/ default handlers
  LAB_005b1240/LAB_005b1250).
- **RWS stream object**: FUN_005b1260 (ctor/open, ~0x190 B), FUN_005b1500 (dtor,
  idempotent bit 0x1000) + thunk_FUN_005b1500 (0x005b15d0), FUN_005b2080 (tick/pump),
  FUN_005b2190 (command submit). Status word @+0x188; event callback LAB_005b15e0.
- **Child-state callbacks** (0 direct callers; vtable-reached): FUN_005b0df0, FUN_005b0e70,
  FUN_005b0ec0, FUN_005b0f40 (release of +0x28 via FUN_005ad2e0), FUN_005b0f90 (state +
  list re-home).

## U-IDs filed (range U-6800..U-6899)

U-6800 (af070 +0x18/+0x20 list roles), U-6801 (af070 state consts / LAB_005af140),
U-6802 (af180 child layout + DAT_005e6ce0 tag), U-6803 (af260 visitor LAB_005af2a0),
U-6804 (af300 state lattice — the umbrella semantic hole), U-6805 (af470 lattice + unused
2nd arg), U-6806 (af510 command flag-word layout), U-6807 (af600 visitor LAB_005af670 +
record layout), U-6808 (af690 strided record / pending-bit), U-6809 (af740 visitor
LAB_005af780 + +0x3c atomic), U-6810 (af7a0 +0x30 / param value), U-6811 (af7d0 strided
source table units), U-6812 (af860 staged descriptor frame), U-6813 (af8f0 start-up
states), U-6814 (afa00 unsettled types / query-node layout), U-6815 (afcf0 collection
node layout + status bits), U-6816 (b0360 entry descriptor), U-6817 (b03e0 5-clause key/
range record), U-6818 (b04b0 multi-level pointer artifacts), U-6819 (b0700 key meaning),
U-6820 (b0740 substitution/mapping records), U-6821 (b0970 void-return vs caller / cb
LAB_005b0a70), U-6822 (b09c0 quantiser units), U-6823 (b0a90 void-return + field
semantics + variable arity), U-6824 (b0bb0 gain-matrix identity), U-6825 (b0c70 command
record fields), U-6826 (b0cf0 strided entry fields), U-6827 (b0df0 sub-object +
DAT_005e6de0), U-6828 (b0e70 sub-object state machine / +0x2c notifier), U-6829 (b0ec0
+0x48/+0x50 states), U-6830 (b0f10 wildcard record fields), U-6831 (b0f90 re-home
semantics), U-6832 (b1030 selector-1 meaning), U-6833 (b10a0 table entry {fn,u16} layout),
U-6834 (b1110 per-mode return code), U-6835 (b1140 per-mode set), U-6836 (b1160 ring
element stride/payload), U-6837 (b1180 read==cap null-store path), U-6838 (b11f0 default
handlers LAB_005b1240/1250 + layout), U-6839 (b1260 ~0x190 RWS object layout), U-6840
(b1500 teardown field identities), U-6841 (b2080 channel-state tables/cursors), U-6842
(b2190 command-op meanings).

All 43 are DATA / STRUCT-SEMANTIC uncertainties — non-blocking for C2 (mechanical
behaviour is recorded verbatim with cited offsets/constants per the NO-GUESSING rule).

## S-IDs filed (range S-5400..S-5499)

None — every callee resolved to a known FUN_ address (in-band or adjacent), a compiler
intrinsic (ROUND), a Win32 import (InterlockedExchange/Increment, CloseHandle), or a
data-driven callback / RW-manager vtable dispatch (`DAT_007d3ff8+0xf0/0xf4`). No STUBS.

## Notes for central finalize

- No drift-skips: all 60 were C1 in hooks.csv at author time; promote C1 -> C2, keep
  subsystem=audio for all 60.
- Two thunk rows (005b10d0 thunk_FUN_005b10a0, 005b15d0 thunk_FUN_005b1500) are genuine
  5-byte JMP thunks — promote as thunks (no separate body work needed).
- Optional master renames per the structural map above are at sweep discretion (master
  only); none are required for the C2 promotion.
- Decompiler caveats worth carrying forward: FUN_005afa00 / FUN_005b04b0 emit
  "Type propagation not settling" (multi-level pointer locals); FUN_005b0970 /
  FUN_005b0a90 are recovered as `void` but callers consume EAX (they return the
  allocated/submitted object) — captured in U-6821 / U-6823.
