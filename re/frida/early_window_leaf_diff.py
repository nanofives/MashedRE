#!/usr/bin/env py -3.12
"""early_window_leaf_diff.py — diff a PURE-LEAF hook without booting MASHED to menu.

WHY THIS EXISTS
---------------
The normal lane (`run_diff.py`) spawns MASHED and waits for the engine dispatch
LUT to ready before calling functions. That requires MASHED to survive D3D9
device init and reach (near) the menu. When the host display is wedged (monitors
asleep / topology change -> CreateDevice AVs 0xC0000005 at ~3 s, see the
"No-display D3D9 wedge" memory note), the LUT root never populates and run_diff
times out — blocking ALL diffs.

But a PURE LEAF — a function whose output depends only on its arguments plus the
single global the harness explicitly seeds — does not need engine state at all.
This tool diffs such leaves in the pre-crash window:

  1. subprocess-spawn MASHED with MASHED_RE_NO_AUTO_HOOK=1 (so the .asi, when
     present, does NOT install inline-JMP hooks -> the original stays original).
  2. frida.attach to the running (pre-crash) process.
  3. LoadLibraryW the .asi ourselves (loader lock is free in a RUNNING process —
     a SUSPENDED spawn deadlocks here) and GetProcAddress the reimpl export.
  4. Assert the original's first byte != 0xE9 (guards against a false-GREEN where
     a hook patched original -> reimpl).
  5. Seed + call original (fixed RVA) and reimpl (export), compare, per arg_type.
  6. Kill before WinMain's D3D9 init ever runs -> the crash never happens.

This is logically identical to run_diff's path1 (call orig + reimpl directly,
hook bypassed) for state-independent leaves; only the attach moment differs,
which cannot change a pure leaf's behavior.

VALIDATION (2026-06-13, recorded in PROMOTION_LOOP_LEDGER.md):
  - positive: reproduces GREEN for global_67f19c_get (round 31) and
    set_77196c_1 (round 29) — matches the trusted menu-attach verdicts.
  - negative: cross-wiring orig=global_67f19c_get vs reimpl=Global67f1a0Get
    yields RED on every case -> the harness has real discriminating power.

SCOPE / LIMITS
--------------
ONLY valid for arg_types whose evidence is state-INDEPENDENT (the harness fully
controls the inputs): read_global (uint32 OR float ret), void_setter_observe,
scalars_to_scattered_globals, and int_scalar leaves that are pure functions of
their argument (no absolute-table read of live state, no pointer-deref of the
arg). State-DEPENDENT functions (scenario:'race', live arrays/tables) are NOT
promotable this way — they still need run_diff against a booted game. Refuses
any other arg_type.

Exit 0 = GREEN (promotable), 1 = RED / harness error.
"""
import frida, os, sys, time, subprocess

# this file lives at <ROOT>/re/frida/ — go up three levels to the repo root
ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(ROOT, "re", "frida"))
import hooks_registry as HR

EXE = os.path.join(ROOT, "original", "MASHED.exe")
ASI = os.path.join(ROOT, "original", "mashed_re_dev.asi")
LOG = os.path.join(ROOT, "log")

PURE_LEAF_ARGTYPES = {
    'read_global', 'void_setter_observe', 'scalars_to_scattered_globals', 'int_scalar',
    'int2_scalar',                # MECHANISM: `uint32 fn(a0: uint32, a1: uint32)` 2 stack args; no seed or global setup; observable = return value only - false-GREEN hazard (internal side-effects not diffed); test values directly from path1_tests t=[a0,a1]; no configurable offsets or buffers; broadly applicable to any pure two-integer scalar-returning function.
    'deref_field_write',          # MECHANISM: fn(ptr p1, uint32 p2) on stack; harness allocates SEPARATE A+inner buffer pairs per side (orig gets A1/I1, reimpl gets A2/I2), links `A[cfg.outer_off]->inner`; calls; observes `inner[cfg.inner_off]` as u32; cfg: `outer_off`/`inner_off`; broadly applicable - both offsets configurable; independent buffers per side prevent cross-contamination while still letting the written scalar value be directly compared.
    'deref_table_read',           # MECHANISM: fn(ptr p1, uint32 i) on stack; harness allocates one array of `cfg.span` dwords (default 16) seeded 0xC0DE0000|k, wraps it in a single A pointer reused by both sides; ONLY observable is the u32 return value; cfg: `span`; broadly applicable to any `return (*p)[i]` shape - the shared A pointer means the absolute pointer value is identical on both sides, so a stored-pointer comparison is valid.
    'const_return',               # MECHANISM: Zero-arg call on stack; NO seeding at all; ONLY observable is the u32 return value (`>>> 0`). False-GREEN hazard: any side-write effects go completely undetected. Broad: any zero-arg function whose sole diffable output is a constant integer return; no cfg parameters beyond the RVA. Do not use if the function writes globals or has observable state changes.
    'global_field_read',          # MECHANISM: Zero-arg call on stack; seeds *(cfg.tgt) -> harness buffer, writes test value at buffer+cfg.field_off; ONLY observable is the u32 return value (false-GREEN hazard: any side-write effects go undetected). Parameterised by cfg.tgt (global pointer address) + cfg.field_off. Broad: any u32 getter of the form `return *(*(tgt)+off)` regardless of named context.
    'float_table_read',           # MECHANISM: Single uint32 stack arg fn(i); seeds optional cfg.seed_table={base,span,stride} with distinct 0xC0DE0000|k bit-patterns on first test iteration only; observes ONLY the float return - deliberately NOT coerced with >>> 0 to preserve float semantics. cfg.seed_table (optional). Return is the ONLY observable; false-GREEN hazard if reimpl returns the right float value for the wrong offset.
    'eax_implicit_void',          # MECHANISM: Void fn with implicit this in EAX; BOTH orig and reimpl are driven via `mov eax,buf; jmp target` trampoline (same shared 0x100 buf), so reimpl MUST also read EAX (naked/__thiscall); harness fills sentinel 0xFFFFFFFF, observes cfg.observe offsets after call; no return compared. cfg.observe, cfg.rva. ABI is NOT adapted between sides - reimpl must read EAX itself.
    'pool_insert_snapshot',       # MECHANISM: 2 stack args (pointer mgr, uint32 key=t); harness allocates mgr (0x18 bytes)/slots (Nx4)/pool (Nx0x10) ONCE, SAME buffers for both sides so stored abs pointers compare equal; mgr[0:2]=count=0, mgr[2:4]=cfg.capacity=N, mgr[0xc]=&pool, mgr[0x10]=&slots; resets before each call; observes full mgr+slots+pool state; cfg.capacity (default 4); pool item stride (0x10) and mgr layout (count@0, cap@2, pool@0xc, slots@0x10) are hardcoded.
    'pool_remove_snapshot',       # MECHANISM: Stack args (ptr mgr, u32 key); harness pre-builds pool by calling cfg.insert_rva(mgr, build_keys[k]) for each key, then calls target(mgr, t) to remove; shared mgr/slots/pool buffers (0x18+Nx4+Nx0x10 bytes) reset via build() before each side; observes full mgr+slots+pool dword dump + u16 return; cfg.capacity, cfg.insert_rva, cfg.build_keys configurable.
    'table_clear',                # MECHANISM: 1 stack u32 idx; seeds cfg.tgt[idx*4]=0xFFFFFFFF; observes cfg.tgt[idx*4] after call - the ONLY observable is always 0 on success, making this a FALSE GREEN hazard: a reimpl that writes 0 to the right slot unconditionally passes even if idx routing is wrong. cfg.tgt parameterises the base address.
    'ptr_fields_clear',           # MECHANISM: Single stack pointer arg fn(ptr); harness allocates 0x100 buf, fills sentinel 0xFFFFFFFF, calls fn(buf), snapshots cfg.observe[k].off dwords; any field the fn fails to zero stays 0xFFFFFFFF -> RED. No return value compared. cfg.observe. Applies to any void fn(ptr) that zeros a subset of struct fields at configurable offsets.
    'stack_pop_snapshot',         # MECHANISM: Stack arg (ptr st); st layout {top@0 S32, cap@4 U32, buf@8 ptr}; harness seeds buf from cfg.init_buf[] and sets top=t (edge-case driver, e.g. empty-stack boundary); shared st/buf reset via reset() before each side; observes st.top + buf[0..N-1] as u32s + u32 return; cfg.capacity and cfg.init_buf configurable; applies to any array-stack pop of this {top,cap,buf} layout.
    'stack_push_snapshot',        # MECHANISM: Stack args (ptr st, u32 value=t); st layout {top@0 S32, cap@4, buf@8}; harness zeros buf and sets top=cfg.init_top; shared st/buf reset via reset() before each side; observes st.top + buf[0..N-1] as u32s + u32 return; cfg.capacity and cfg.init_top configurable; applies to any array-stack push of this {top,cap,buf} layout.
    'ptr_table_field_read',       # MECHANISM: Stack arg (u32 idx); seeds *(cfg.tgt)=tableBuf, tableBuf[idxx4]=&entry, entry[cfg.field_off]=(0xC0DE0000|idx); observes RETURN VALUE ONLY (the double-deref field read) - false-GREEN hazard if reimpl has side effects not visible through the return; cfg.tgt, cfg.field_off, cfg.capacity configurable.
    'indexed_table_set',          # MECHANISM: Stack call fn(cfg.set_idx, val) with index FIXED to cfg.set_idx and test input t as val; seeds slot at cfg.tgt+cfg.set_idx*cfg.stride with 0xFFFFFFFF; ONLY observable is the written slot dword (return NOT observed). NARROW: index never varies across tests - a stride/addressing bug that only manifests at non-tested indices is invisible; use multiple registry entries with different cfg.set_idx values to cover index space.
    'range_init',                 # MECHANISM: Zero-arg call on stack; seeds cfg.len bytes at absolute cfg.tgt with 0xEEEEEEEE sentinel; observes the full dword range after call. Return value NOT observed (false-GREEN hazard if fn also returns a meaningful value). Parameterised by cfg.tgt (base) + cfg.len (byte count, multiple of 4). Broad: any void fn() that initialises a contiguous absolute global region with constants.
    'cond_global_set',            # MECHANISM: void fn(uint32 v) on stack; seeds `*cfg.tgt` from `t[0]`, calls with `t[1]`, observes `*cfg.tgt`; ONLY observable is the global u32; cfg: `tgt`; logic `if(v==0 || *tgt==0) *tgt=v`; test pairs [seed, arg] are chosen to exercise all three branches (v==0 write, *tgt==0 write, both-nonzero no-op); broadly applicable to any conditional scalar-global setter of this predicate shape.
    'ptr_out_table_get',          # MECHANISM: Stack call fn(out_ptr, idx); seeds absolute table at cfg.tgt+idx*cfg.stride with cfg.span distinct dwords (in-range idx only); allocates SEPARATE out buffers per side; observes cfg.span dwords from out_ptr PLUS u32 return (in-range: n dwords written + ret=1; out-of-range: nothing written + ret=0). Params: cfg.tgt, cfg.stride, cfg.span (output dword count), cfg.bound. cfg.reseed_per_side re-seeds before EACH side - REQUIRED if the fn WRITES the block it copies out (else the original's result is left in place and a port that skipped the work reads it and passes). Broad: any bounds-checked table-lookup filling an out-buffer with a success/fail return.
    'idx2_table_get',             # MECHANISM: 3 stack args fn(out_ptr,i1,i2); out_ptr freshly allocated per side; seeds abs slot at cfg.tgt+(i1*cfg.mult+i2)*cfg.stride with distinct marker; observes *out AND return value - both compared, false-GREEN only possible if both match. cfg.tgt, cfg.mult, cfg.stride, cfg.bound, cfg.bound2; composite-multiply and both bounds are configurable.
    'cond_table_get',             # MECHANISM: fn(uint32 idx) stack; harness seeds rec=cfg.tgt+idx*cfg.stride with distinct dwords at cfg.off0/cfg.off1 and flag-word at cfg.offf; observes ONLY u32 return value (false-GREEN hazard - no field snapshot); cfg: tgt, stride, off0, off1, offf; test=[idx,flag] seeds both slots plus flag so both conditional branches are exercised non-degenerately.
    'ptr_compute_get',            # MECHANISM: Stack args (ptr out, u32 idx); seeds cfg.idxtbl[idxxstride]=(0x100|idx) (distinct per idx); fresh harness-allocated out-buffer per side (not shared); observes *out (computed address) + u32 return (1 in-range, 0 out-of-bound); cfg.tgt, cfg.idxtbl, cfg.stride, cfg.tscale, cfg.bound all configurable; applies to any indexed ptr-compute getter of this two-arg shape.
    'eq_predicate_get',           # MECHANISM: fn(p1 u32, p2 u32 on stack); seeds cfg.gate with gv + cfg.tgt[p1*cfg.stride]/[p2*cfg.stride] with equal or unequal markers per test t=[p1,p2,eq,gv]; calls Orig/Reim(p1,p2); ONLY observable is return value (0/1). Parameterised by cfg.tgt (table base), cfg.stride, cfg.gate; gatemax is baked in the reimpl. Applies to any gated tbl[p1*stride]==tbl[p2*stride] predicate; tests exercise gate-fail, equal, and unequal branches.
    'table_ret_ptrout',           # MECHANISM: `u32 fn(idx: uint32, out: pointer)` 2 stack args; seeds REAL base+idx*stride at cfg.off0 and cfg.off1 with distinct sentinels; fresh harness out-buf per side; observes *out (u32) AND return value joined "out|ret=rv"; configurable: cfg.tgt, cfg.stride, cfg.off0 (->out), cfg.off1 (->ret); fits any strided-table getter that writes one field to an out-ptr and returns a different field.
    'arg_scattered_globals',      # MECHANISM: void fn(uint32 arg) - one stack scalar; fills all abs globals in cfg.observe with 0xFFFFFFFF sentinel, calls fn(arg), reads them all back as a joined |string; return is not captured. Params: cfg.observe[].addr. Broad: any arg-driven switch/branch setter that scatters distinct writes across abs globals depending on arg; non-degenerate when distinct arg values produce distinct per-global write patterns across the test set.
    'global_indexed_float',       # MECHANISM: zero-arg call; seeds *(int*)cfg.gate=t (index) and *(float*)(cfg.tgt+t*cfg.stride) to a finite non-NaN pattern; observes ONLY the return float (no side-effect check - false-GREEN hazard for a reimpl that ignores the gate or uses the wrong index); parameterised by cfg.gate, cfg.tgt, cfg.stride.
    'vec16_copy_set',             # MECHANISM: u32 fn(uint32 idx, ptr in) on stack; harness fills `in` (`cfg.span` dwords, default 16) with distinct markers; resets `cfg.tgt+idx*cfg.stride` (2xspan dwords) to sentinel 0xEEEEEEEE; calls; observes 2xspan dwords plus return; cfg: `tgt`/`stride`/`span`/`bound`; broadly applicable - span configurable, covers any fn copying n dwords from `in` into two back-to-back strided dest regions.
    'container_record_set',       # MECHANISM: Stack call fn(cont [, inA | float | inA,inB]) per cfg.shape ('p'/'f'/'pp'); ALL buffers (cont, rec, inA, inB) allocated once and shared between both sides -> stored-pointer comparisons across orig/reim remain valid (no false-RED from pointer divergence); container[0]=rec+0x100, container[8]=cfg.idx; target addr=base+cfg.idx*0x30; snapshots cfg.writes offsets from addr. Params: cfg.idx, cfg.shape, cfg.writes. Broad: any fn writing args into a record addressed through a container-pointer+fixed-index pair.
    'indexed_vec_set',            # MECHANISM: `void fn(idx: uint32, in: pointer)` 2 stack args; harness allocates in-buf with sentinel pattern (0xC0DE...), sentinel-fills destination slice 0xEEEEEEEE at REAL base+idx*stride, calls fn(idx,in), snapshots N=cfg.span dwords (default 3) at destination; return not captured; configurable: cfg.tgt, cfg.stride, cfg.span; fits any fn copying N dwords from in* to base[idx*stride].
    'indexed_bit_toggle',         # MECHANISM: Two stack args (uint32 idx, uint32 set); seeds abs flag word cfg.tgt+idx*cfg.stride+cfg.field_off with known prior value (test-vector seed); calls; observes that flag word after call; bit mask is baked into the reimpl (not in cfg); cfg.tgt, cfg.stride, cfg.field_off configurable; set=0 and set=1 both exercisable via test vector.
    'gated_int_predicate',        # MECHANISM: One stack u32 arg; cfg.gate global seeded from t[1]. Calls fn(t[0]); observes RETURN ONLY (u32 - FALSE-GREEN hazard: must seed gate open AND closed + in/out-of-membership arg values). Broad: any single-arg switch-membership predicate gated on a global int (gate==gateval -> membership(arg) else 0); cfg.gate only parameter.
    'global4_bool_out',           # MECHANISM: `void fn(out*)` 1 ptr stack arg; seeds N REAL globals at cfg.tgt from cfg.seedvecs[t] (truth-mix, N=cfg.span default 4), allocates fresh harness out-buf per side, sentinel-fills 0xEEEEEEEE, calls fn(out), snapshots N dwords; return NOT captured - false-GREEN hazard; configurable: cfg.tgt, cfg.span, cfg.seedvecs; fits any void fn(out*) mapping N globals through a predicate.
    'linear_scan_find',           # MECHANISM: One stack arg (uint32 key); seeds cfg.gate global to cfg.count, fills cfg.count slots at cfg.tgt+k*cfg.stride with distinct non-key markers, places key at test-vector placeAt position; observes signed int return (matched index >=0 or -1) ONLY; count is read from the gate global (not a function argument); cfg.tgt, cfg.stride, cfg.count, cfg.gate configurable; both hit and miss cases exercisable.
    'gated_record_eq2',           # MECHANISM: No-arg fn; seeds gate global (cfg.gate) to gidx, then cfg.tgt+gidx*cfg.stride+off0/off1 to s0/s1 from t=[gidx,s0,s1]; observes ONLY u32 return; comparison constants v0/v1 are baked into the reimpl (absent from cfg) - a true-return test must set s0=v0,s1=v1. cfg.gate, cfg.tgt, cfg.stride, cfg.off0, cfg.off1. NARROW: v0/v1 constants and gate global are hardwired per reimpl.
    'indexed_const2_set',         # MECHANISM: One stack arg (uint32 idx); seeds two abs slots cfg.tgt+idx*cfg.stride+off0 and +off1 to sentinel 0xEEEEEEEE; calls; snapshots both slots as u32 pair; written constants v0/v1 are baked into the reimpl (not in cfg); cfg.tgt, cfg.stride, cfg.off0, cfg.off1 configurable; applies to any fn writing two fixed constants into indexed struct fields.
    'global_switch_member',       # MECHANISM: zero-arg; seeds *(int*)cfg.gate=t; observes ONLY the return u32 (switch-membership predicate result); false-GREEN hazard: a reimpl that ignores the gate or returns a constant passes unless the test set includes the gate-fail case; parameterised by cfg.gate only.
    'gated_args_to_globals',      # MECHANISM: Six stack u32 args (t[0..5]); gate global (cfg.gate) seeded to 0 to enable write path, aux (cfg.aux) seeded from t[6]; cfg.observe[] absolute globals filled sentinel 0xFFFFFFFF. Calls fn(t[0]..t[5]); observes cfg.observe list + cfg.aux. Broad: any gated (gate==0) scatter of <=6 scalar args to absolute globals with optional aux side-effect; cfg.observe count is arbitrary.
    'void_global_transition',     # MECHANISM: void fn() - no args; seeds `*cfg.tgt` from `t[0]`, calls, observes `*cfg.tgt` after call; ONLY observable is the global u32; cfg: `tgt`; `from`/`to` transition values are baked into the reimpl and not in cfg; test must supply both a matching and a non-matching seed to exercise both branches of the guard.
    'two_global_predicate',       # MECHANISM: `u32 fn()` no stack args; seeds *(u32*)cfg.gate=t[0], *(u32*)cfg.tgt=t[1]; both re-seeded before each side; observable = return value only - false-GREEN hazard; configurable: cfg.gate, cfg.tgt; fits any no-arg predicate returning a membership or comparison result derived from exactly two global words.
    'index_then_ptr_array',       # MECHANISM: `u32 fn(a0)` or `fn(a0,a1)` stack args (cfg.mult selects); seeds *(int*)(cfg.tgt+comp*4)=idxval (t[-1], may be -1); fn returns 0 if idx==-1 else live read from real .rdata at cfg.basePtr+idx*4; observable = return value only; false-GREEN hazard when idxval==-1; cfg.tgt (writable idx-table), cfg.basePtr (real .rdata), cfg.mult configurable.
    'flag_multibit',              # MECHANISM: 3 or 4 stack uint32 args fn(idx,b1,b2[,b3]); harness seeds flag word at cfg.tgt+idx*cfg.stride with t[last] sentinel, calls, snapshots same slot after RMW bit logic (|=/&=~). cfg.tgt, cfg.stride, cfg.nargs4 (3- vs 4-arg toggle). Fully parameterised - not tied to any specific bit pattern or stride.
    'float_threshold_predicate',  # MECHANISM: One stack u32 arg (idx); seeds cfg.tgt+idx*cfg.stride record float = t[1] bits (threshold at cfg.gate is live .rdata - NOT seeded; tests must straddle real threshold for non-degen). Calls fn(idx); observes RETURN ONLY (u32 0/1 - FALSE-GREEN hazard). Parameterized by cfg.tgt, cfg.stride; test t=[idx, recordbits].
    'deref_struct_set',           # MECHANISM: Stack call fn(p [, scalars]); p = fresh 0x400 buffer seeded with cfg.seed_byte (nonzero exercises RMW-OR paths); test input t supplies 0-3 uint32 scalar args (cfg.nscalar); snapshots cfg.observe offsets from p plus optional cfg.abs_observe absolute globals (reset to 0x5e5e5e5e before each side). Separate buffers per side - if fn stores p itself as a field value, addresses diverge (false-RED). Params: cfg.observe, cfg.nscalar, cfg.seed_byte, cfg.abs_observe.
    'cond_deref_get',             # MECHANISM: 1 stack ptr arg to harness-allocated 0x40 buffer, zeroed then p+cfg.gate_off seeded with t[0] and p+cfg.val_off seeded with t[1]; observes RETURN VALUE ONLY (u32); gate_off and val_off are configurable (`cfg.gate_off`, `cfg.val_off`) making this applicable to any struct-gated field-read at arbitrary offset pairs; no globals, no callee.
    'table_bool_predicate',       # MECHANISM: Single uint32 stack arg fn(i); seeds slot at cfg.tgt+idx*cfg.stride+cfg.off0 with t[1] (slotval); observes ONLY the u32 return value - bound guard is baked into the reimpl, not seeded. cfg.tgt, cfg.stride, cfg.off0. test=[idx,slotval] exercises zero and nonzero slot values plus in/out-of-bound idx. Return is the sole observable; false-GREEN hazard if guard is wrong.
    'global_swap',                # MECHANISM: 1 stack uint32 arg; seeds *(u32*)cfg.tgt=t[0]; calls fn(t[1]); observes compound ret|g= string (old return value + new global contents); both ret and global verified so a write-only or return-only reimpl goes RED; parameterised by cfg.tgt only; applies broadly to any atomic-swap-of-a-global pattern.
    'byte_args_to_globals',       # MECHANISM: 1-3 stack uint8 args (count driven by `cfg.observe` array length); seeds each absolute address in cfg.observe with sentinel 0xEE before each call, then reads each back as u8 after; both sides write the same absolute globals so values compare directly; return value not observed; `cfg.observe=[{addr}...]` selects which globals to watch; no harness-allocated output buffers.
    'indexed_float_sq',           # MECHANISM: Two stack args (uint32 idx, float f); seeds abs slot cfg.tgt+idx*cfg.stride to 0xFFFFFFFF; calls; observes slot as float after call (expects f*f); return value not observed; cfg.tgt and cfg.stride configurable; applies to any fn that stores a float square into an indexed abs slot.
    'double_deref_vec3_get',      # MECHANISM: 2 stack args fn(i, out*); out freshly allocated per side; seeds 2-level ptr chain cfg.tgt[i*stride]->buf1, buf1[rec_off]->buf2, buf2[out_off+k*4]=markers; observes out[0..span-1] +/- ret via optional cfg.ret_tbl/cfg.ret_stride. cfg.tgt, cfg.stride, cfg.rec_off, cfg.out_off, cfg.span (default 3, configurable beyond vec3), cfg.ret_tbl, cfg.ret_stride.
    'global_float_predicate',     # MECHANISM: No args (void fn()); seeds cfg.gate(int)=t[0], cfg.thr(float bits)=t[1], cfg.tgt->buf with buf[cfg.rec_off]=t[2] float bits. Calls fn(); observes RETURN ONLY (u32 0/1 - FALSE-GREEN hazard: gate=0 always yields 0; non-zero gate with values straddling threshold needed for non-degen). Broad: any no-arg gated double-deref float comparison; cfg.gate/thr/tgt/rec_off all parameterized.
    'double_deref_ptr_get',       # MECHANISM: fn(ptr out, uint32 idx) stack; harness allocs buf, seeds marker at buf+cfg.rec_off, chains cfg.tgt+idx*cfg.stride->buf; calls fn(out, idx); observes *out ONLY (no return; false-GREEN hazard); cfg: tgt, stride, rec_off, add (addend to deref'd value); applies broadly to any fn with pattern *out=*(*(base+i*S)+off)+add regardless of what the outer table represents.
    'deref_float_field_rmw',      # MECHANISM: fn(ptr p, float f) stack; harness allocs 0x80 buf, seeds float at buf+cfg.field_off to cfg.seedf, calls fn(buf, t); observes float at buf+cfg.field_off ONLY (no return); cfg: field_off, seedf; operator (-= or +=) baked into reimpl not the harness; applies to any struct-field float RMW taking (ptr, float) with configurable field offset and initial seed value.
    'any_slot_nonzero',           # MECHANISM: u32 fn() - no args; seeds all abs globals from cfg.observe to zero, optionally writes 0xC0DE at obs[t].addr (t=-1 = all-zero path). Observes ONLY the return value - no post-call snapshot of the globals taken, FALSE-GREEN hazard if the function also writes any of them as a side effect. Params: cfg.observe[].addr. Broad: any zero-arg predicate that scans a cfg-supplied list of abs globals and returns 1 on any nonzero.
    'arg_table_linear_search',    # MECHANISM: int fn(uint32 key, pointer table, uint32 count) - three stack args; harness allocates the table (countxcfg.stride_dw dwords) and fills it with 0x7F000000|i markers, then places key at t[1]-th entry. Observes ONLY return value (matched index or -1) - FALSE-GREEN hazard if fn has write side effects. Params: cfg.stride_dw; key/placeAt/count vary per test via t. Broad: table is harness-alloc and pointer-passed, not an abs address; any stride-configurable linear-key scan fits.
    'global_float_step',          # MECHANISM: 1 stack float arg (target); seeds *(float*)cfg.tgt to t[0] bit-pattern before each call; observes *(float*)cfg.tgt after; step constant is baked into the reimpl, not in cfg; parameterised by cfg.tgt only.
    'struct_const_init',          # MECHANISM: Stack call fn([0x12345678,] p [, scalars]) where p = fresh 0x400 buffer filled with 0xFFFFFFFF per side; cfg.passthrough_arg=true puts hardcoded 0x12345678 first and folds the return into the comparison; cfg.nscalar adds 0-3 trailing uint32 scalar args after p; snapshots cfg.observe offsets. SEPARATE buffers per side - if fn stores p's address as a field value those addresses diverge (false-RED). Params: cfg.observe, cfg.passthrough_arg, cfg.nscalar.
    'idx2_table_get_outlast',     # MECHANISM: 2 uint32 args + 1 out-pointer; seeds *(u32*)(cfg.tgt+(i1*cfg.mult+i2)*cfg.stride) with marker when in bounds; fresh out-buffer per side; observes *out|ret compound string; both checked so a write-only or return-only reimpl goes RED; parameterised by cfg.tgt, cfg.mult, cfg.stride, cfg.bound, cfg.bound2.
    'copy_arg_to_globals',        # MECHANISM: fn(ptr p) stack; harness allocs n-dword contiguous buf seeded with 0xC0DE0000|k markers, calls fn(buf), then reads cfg.observe[k].addr globals; observables are the destination globals ONLY, NOT a return value; cfg: observe[]{addr}; applies to any fn that scatter-copies from a contiguous input ptr into a fixed list of known global addresses.
    'deref_byte_flag',            # MECHANISM: fn(ptr p, u32 set) stack; harness allocs 0x40 buf, seeds byte at buf+cfg.field_off with t[1]&0xff, calls fn(buf, t[0]); observes mutated byte at buf+cfg.field_off ONLY (no return); cfg: field_off, bit; set!=0->b|=bit else b&=~bit; applies to any (struct*, bool) byte-flag RMW with configurable field offset and bit mask.
    'indexed_masked_get_out',     # MECHANISM: Two stack args (uint32 idx, pointer out [SEPARATE harness-alloc'd buffers per side - no shared-pointer hazard]); seeds abs slot cfg.tgt+idx*cfg.stride with test dword; observes out[0] after call (masked value); cfg.tgt, cfg.stride, cfg.mask all configurable - mask is NOT baked into reimpl; applies to any masked array-get with an out-pointer, regardless of the masking constant.
    'deref_p1field_glob_set',     # MECHANISM: Stack call fn(p1 [, ptr|scalar|scalar2]) per cfg.arg2_kind; resolves target base as `*(*(p1+cfg.p1_off) + *(cfg.glob))` with cfg.glob forced to 0 -> base = harness buffer reached through atab; all buffers (base, atab, p1, p2) shared between sides -> stored-pointer comparisons valid; snapshots cfg.observe offsets from base. Scalar args are hardcoded constants (not from t). Return NOT observed. Params: cfg.observe, cfg.p1_off, cfg.arg2_kind, cfg.glob.
    'global_table_linear_search', # MECHANISM: 1 stack int arg (key=t[0]); seeds abs table at cfg.tgt (cfg.count records x cfg.stride bytes each) with distinct non-key sentinels, places key at t[1]; observes ONLY the return int (found-index or -1); false-GREEN hazard if stride/count differ; parameterised by cfg.tgt, cfg.stride, cfg.count.
    'global_ptr_strided_clear',   # MECHANISM: zero-arg; plants *(u32*)cfg.glob=&harness-buf (pre-filled 0xEEEEEEEE); observes cfg.stride-stepped dword snapshot over cfg.len bytes after the call; shared buf pointer both sides so stored-pointer comparisons stay equal; parameterised by cfg.glob, cfg.len, cfg.stride.
    'struct_to_out_build',        # MECHANISM: 2 stack ptrs (out*, p2); seeds p2 dwords per cfg.seed array [{off,bits}]; uses SEPARATE out bufs per side (no shared-pointer equality hazard); observes out[0..cfg.span-1] dwords; broadly applicable to any read-p2-write-out function - cfg.span and cfg.seed fully parameterise field count and input layout.
    'store_be32',                 # MECHANISM: Stack args (ptr buf, u32 v=t); single shared harness buffer reset to 0xEEEEEEEE before each side's call; observes buf[0..3] as four individual bytes (verifies big-endian byte order); return value NOT observed (void function); no cfg.* parameterisation; applies to any fn(ptr, u32) that stores a u32 in big-endian byte order at the pointed location.
    'load_be32',                  # MECHANISM: One stack ptr arg; harness allocs buf, writes test dword t (little-endian) into it, passes buf as p. ONLY observable: return value (u32); no side-effect check. No cfg fields beyond test array t. Covers any fn that reads p[0..3] and returns them as a big-endian u32. False-GREEN hazard: return-value-only leaves a wrong byte-order implementation undetectable if both sides happen to return the same value.
    'arg_to_global_ret',          # MECHANISM: u32 fn(uint32 v) - one stack scalar; seeds abs global at cfg.tgt with sentinel 0xEEEEEEEE; observes cfg.tgt written value AND return value joined as |string - both outputs are captured so no FALSE-GREEN hazard from return-only. Params: cfg.tgt. Broad: any single-scalar-arg function that both stores a value derived from v into one abs global and returns a value from the same computation.
    'indexed_global_field_read',  # MECHANISM: `u32 fn()` no stack args; seeds *(u32*)cfg.tgt=&scratch_buf, *(u32*)cfg.glob=gidx (cfg.idx, default 0x40), buf[gidx+cfg.field_off]=test_value; observable = return value only - false-GREEN hazard; wrong cfg.glob reads idx=0 -> different slot -> RED; configurable: cfg.tgt, cfg.glob, cfg.field_off, cfg.idx; fits any no-arg reader through two-global double-deref at configurable offset.
    'indexed_global_field_write', # MECHANISM: `int fn(v: uint32)` 1 uint32 stack arg; seeds *(u32*)cfg.tgt=&scratch_buf, *(u32*)cfg.glob=gi4 (cfg.idx, default 0x40), zeroes buf; calls fn(v), observes buf[gi4+cfg.field_off] AND return value joined "store|ret"; configurable: cfg.tgt, cfg.glob, cfg.field_off, cfg.idx; fits any fn that stores v through base+index-global indirection at a configurable field offset.
    'thiscall_struct_from_table', # MECHANISM: Orig=`__thiscall fn(ECX=sbuf)` (cfg.conv_orig='thiscall'), Reim=`__cdecl fn(sbuf)` on stack; same 0x400-B scratch buf both sides; seeds sbuf[cfg.idx_off]=gi5 and REAL global table cfg.tbl at gi5*tbl_stride+k*4 (seed_tbl_n dwords, varied by t); snapshots cfg.observe_offs from sbuf; configurable: cfg.idx_off, cfg.idx, cfg.tbl, cfg.tbl_stride, cfg.seed_tbl_n, cfg.observe_offs; fits any __thiscall that reads table-index from this[off] and writes derived fields back.
    'eax_ecx_insert',            # MECHANISM: any fn taking TWO REGISTER args in EAX+ECX (and optionally EDX via edx_val) - NOT only inserts. bufA/bufC are allocated ONCE and SHARED across both sides, so a pointer STORED from one reg into the other's buffer compares equal (this is what makes `mov [ecx],eax` shapes work). Original use case: cross-link insert. trampoline sets EAX+ECX, seed both bufs (eax_seed/ecx_seed [{off,val}]), call, snapshot eax_observe/ecx_observe offsets in both + ret. reimpl naked __asm reading EAX+ECX. test ignored (single call)
    'ptr_buffer_op',              # MECHANISM: `void fn(p*)` 1 ptr stack arg; harness allocates buf (cfg.buf_dwords*4 B, default 0xC00*4), sentinel-fills 0xA5A5A5A5 before each side, passes same allocated region to both Orig and Reim; snapshots cfg.observe_offs dwords from buf; return not captured; configurable: cfg.buf_dwords, cfg.observe_offs; fits any void-fn-of-ptr doing memset or abs-memcpy over a caller-supplied buffer.
    'reg_scalar_compute',         # MECHANISM: Original invoked via per-side trampoline (mov EAX=a; mov ECX=c; mov EDX=d; jmp cfg.rva) with no stack args; reimpl called cdecl(a, c[, d]) on stack; no memory seeded or observed; observes RETURN VALUE (EAX coerced to u32) ONLY - false-GREEN if side effects differ between sides; applies to any EAX/ECX[/EDX]-in, EAX-out register-convention function; test vector t=[a,c] or [a,c,d].
    'eax_struct_stack_out',       # MECHANISM: fn(EAX=struct via `mov eax,sbuf; jmp`, [esp+4]=out ptr delivered as the NativeFunction's single stack arg); ORIG and REIMPL both wrapped via mkTQ (shared sbuf+obuf - stored self-relative pointers compare equal across both sides); seeds sbuf via cfg.eax_seed [{off,val}]; observes obuf at cfg.out_observe offsets. Broadly applicable to any EAX-struct + single stack out-ptr fn; parameterised by cfg.eax_seed and cfg.out_observe.
    'abstable_ptr_zero',          # MECHANISM: void fn(uint idx) - one stack scalar; seeds abs ptr-table at cfg.tgt+cfg.idx*4 -> harness-alloc scratch (cfg.buf_dwordsx4 bytes) sentinel-filled 0xA5A5A5A5; same scratch address written into the table on both sides so stored-pointer reads compare equal. Observes cfg.observe_offs offsets in scratch. Params: cfg.tgt, cfg.idx, cfg.buf_dwords, cfg.observe_offs. Broad: any fn loading a pointer from an abs table by index and operating on the pointed-to buffer.
    'idx_table_out',              # MECHANISM: Two stack args (uint32 idx, pointer out [harness-alloc'd, shared across both sides]); if cfg.tgt is set, seeds table entry cfg.tgt+idx*cfg.stride with distinct value (0xC0DE0000|idx) before call, else table is truly static .rdata; observes out[0] only - return value not observed; cfg.tgt (optional, null = static table), cfg.stride configurable.
    'nested_struct_op',           # MECHANISM: 1 stack arg (pointer p); harness allocates p (0x400 bytes) and sub (0x8000 bytes) ONCE, SAME buffers reused for Orig and Reim so stored abs pointers compare equal; p zeroed then seeded via cfg.p_seed=[{off,val}], p[cfg.link_off]=&sub (sentinel-filled 0xA5A5A5A5); observes cfg.observe_p offsets in p + cfg.observe_sub offsets in sub; cfg.link_off/p_seed/observe_p/observe_sub; broadly applicable to any fn(ptr) that RMWs p and writes into a linked sub-buffer.
    'idx_src_abs_memcpy',         # MECHANISM: Two stack args (uint32 idx, pointer srcbuf [harness-alloc'd, dwords 0xC0DE0000|k]); seeds abs dest cfg.tgt+idx*cfg.stride to sentinel 0x5e5e, calls, observes cfg.copy_dwords dwords from abs dest; cfg.tgt, cfg.stride, cfg.copy_dwords configurable; NULL-src branch not tested; applies to any bounded memcpy writing into an indexed abs-address table.
    'dll_unlink',                 # MECHANISM: fn(ptr list, ptr arg1=N1+0xc) stack; SHARED L/N0/N1/N2/S bufs allocated once, reset via build() before each side; removes N1 from a 3-node DLL; observes N0[0].next and N2[4].prev as pointer strings; no return; NARROW: layout (next@+0, prev@+4, head@list+8, sentinel@list+0xc) hardcoded from the target DLL, no cfg tuning; test ignored; faithful __cdecl port.
    'circular_dll_search',        # MECHANISM: 2 stack ptr args (container p, key = target-object ptr); harness builds 3-object circular list with nodes at obj+0x18, list head at p+0x10, sentinel=p+0x10; both sides share same harness-allocated objects so returned pointer is directly comparable; observes RETURN VALUE ONLY (u32 - false-GREEN hazard); all layout offsets hardcoded (+0x10 head, +0x18 node-within-object); no cfg knobs; plain `__cdecl` port.
    'dll_get_nth',                # MECHANISM: fn(ptr p, ptr cont, uint32 idx) stack; SHARED bufs build a 5-node DLL (node embedded at obj+0x2c, p[0x20]=head, p[0x24]=tail, cont[8]=count); walks forward from head or backward from tail based on idx vs count/2; observes ONLY u32 return = node-0x2c (false-GREEN hazard - READ-ONLY, no mutation snapshot); NARROW: all offsets hardcoded, no cfg tuning; faithful __cdecl port; test=idx.
    'indexed_global_2lvl',        # MECHANISM: No stack args (void fn()); seeds cfg.tgt global -> shared 0x2000 harness buffer, cfg.glob global -> cfg.idx (default 0x40), buf[idx+cfg.mid_off]=cfg.edx_val, buf[edx_val*4+idx]=test; observes return u32 ONLY; both sides read the same buffer via the seeded global (shared-pointer safe); cfg.tgt, cfg.glob, cfg.mid_off, cfg.idx, cfg.edx_val all configurable.
    'indexed_bound_array_get',    # MECHANISM: One stack arg (uint32 idx, fixed to cfg.idx default 5); seeds cfg.glob=0xFFFF (large bound so bound-check passes), cfg.tgt global -> harness container buf, container[cfg.field_off] -> harness array buf, arrbuf[idx*4]=test value; observes return u32 ONLY - false-GREEN hazard; cfg.tgt, cfg.glob, cfg.field_off, cfg.idx configurable; bound-fail (return 0) path untested.
    'abs_ranges_setter',          # MECHANISM: Stack call fn(scalars) with 1-3 uint32 args (count driven by cfg.nscalar via the test vector shape); resets each {addr,dwords} block in cfg.abs_ranges to zero before each side; snapshots all those dwords after. Return NOT observed. Broad: covers any void fn(scalars) writing to multiple disjoint absolute global regions; cfg.abs_ranges accepts a list of non-contiguous {addr,dwords} blocks so wide scatter-writes across unrelated tables can be captured in one registry entry.
    'esi_global_search',         # MECHANISM: PARAMETERISED scan, not a fixed recipe - cfg.tgt base, cfg.glob count, cfg.stride, cfg.key_off key field (default 0). It SEEDS the table, so it does not depend on the live array being populated. u32 fn(ESI=key): linear-search for entry[+cfg.key_off]==key; return an index-derived pointer or 0. ORIG called via `mov esi,key; jmp` trampoline; reimpl is __cdecl(key) reading the same globals (compares result, not ABI). Seed count=4, zero 4 entries, table[idx*stride]=key, key=0xC0DE0000|idx. test=idx (0..3 -> distinct matched addr -> non-degen)
    'indexed_global_idiv',        # MECHANISM: One stack arg (uint32 arg, fixed to cfg.idx default 0); seeds abs slot cfg.tgt+cfg.idx*cfg.stride with signed divisor (test value); calls; observes return u32 ONLY - false-GREEN hazard; numerator (num) and clamp constants baked into reimpl; cfg.tgt, cfg.stride, cfg.idx configurable; index never varies across tests, only the divisor does.
    'float_vec3_lerp_out',        # MECHANISM: Four stack args (out*, a*, b*, float t); same out/a/b bufs reused both sides (reset between), so no stored-pointer ambiguity. Seeds a/b vec3 from cfg.seed_a/seed_b (u32 float-bits), t from cfg.t_bits. Calls fn(out,a,b,tf); observes out[0..2] as u32 bit-patterns. Reimpl must be verbatim naked __asm (bit-identical x87 - C float rounds differently). Parameterized by cfg.seed_a/seed_b/t_bits.
    'float_2ptr_ret',             # MECHANISM: 2 stack ptr args fn(a*,b*) to freshly allocated float bufs; seeds a[0..2] and b[0..2] as floats from cfg.seed_pairs[t]={a,b}; observes ONLY the float return (ST0) captured as u32 bit pattern to avoid NaN issues. cfg.seed_pairs. Reimpl must be verbatim naked __asm for bit-identical x87. Return is the ONLY observable - false-GREEN hazard if x87 rounding diverges.
    'float_planes6_predicate',    # MECHANISM: Two stack ptrs (obj*, point*); harness allocs both fresh each side. Seeds 6 plane xyzw floats at obj+0x94 stride 0x14 per plane and point xyzw at pt from cfg.seed_sets[t]={point:[4],planes:[6x4]}. Calls fn(obj,pt); observes RETURN ONLY (u32 0/1 - FALSE-GREEN hazard). Stride 0x14 and offset 0x94 hardcoded; reimpl verbatim naked __asm (bit-identical x87).
    'eax_edi_out',                # MECHANISM: fn(EAX=v, EDI=out*); ORIG via `mov eax,v; mov edi,outbuf; jmp` trampoline, NativeFunction(void,[]); REIMPL __cdecl(v,out); test=v (varied); observes out[0..2] (3 dwords - span hardcoded, no cfg param). Broadly applicable to any int-in-EAX fn writing <=3 dwords through EDI; set-comment notes intended use for magic-multiply-equals-C-integer-division shapes.
    'grid_getter_multiout',       # MECHANISM: 2 uint32 args (cfg.grid.i, cfg.grid.j) + 3 out-pointer args; seeds bound globals cfg.grid.b1/b2 to max-int and three parallel abs tables (cfg.grid.out1_t/out2_t/out3_t) at 2D indices via cfg.grid.mul1/mul3/s12/s3; observes out1[0,4]|out2[0,4]|out3[0]|ret; fully parameterised via the cfg.grid object.
    'struct_ctor_big',            # MECHANISM: One stack ptr arg; harness-allocates one shared buffer (default 0x600 dwords, `cfg.buf_dwords` overrides), sentinel-fills 0xA5A5A5A5 before each call; snapshots dwords at byte-offsets in `cfg.observe` ({off} entries); return value NOT observed (false-GREEN hazard if ctor returns meaningful data); shared buffer makes self-relative pointer writes (p+const stored at p+X) compare equal between sides - applies to any deterministic void ctor too large for struct_const_init's fixed 0x400 buffer; test vector unused.
    'indexed_abs_dualout',        # MECHANISM: u32 fn(uint32 i, ptr out1, ptr out2) on stack; seeds `cfg.tbl1[i*cfg.stride]` and `cfg.tbl2[i*cfg.stride]` directly in absolute memory (BSS: committed, writable, shared across both sides); fresh independent out1/out2 allocs per call; observes `out1[0]|out2[0]|ret`; cfg: `tbl1`/`tbl2`/`stride`/`bound`; broadly applicable - two fully configurable source tables with dual independent out-ptrs, varied i for non-degeneracy.
    'dll_remove_count',           # MECHANISM: fn(ptr list, ptr node) stack; SHARED lst+nd+A+B bufs so relinked pointers compare equal across sides; seeds list[0]=7 (count), list[4]=list+4 (empty sentinel, skips pure-read search loop), node[0x20]=B, node[0x24]=A; observes list[0]|A[0]|B[4] (count decrement + two pointer rewrites); no return; NARROW: link offsets (0x20/0x24) and count@list[0] hardcoded; test ignored.
    'dll_insert_head',            # MECHANISM: fn(ptr list, ptr node) stack; SHARED lst+nd bufs so all stored pointer values compare equal across sides; seeds empty list (*(list+8)=list+8) and node[4]=0 (skips conditional unlink branch); observes 5 link writes: nd[4]|nd[8]|nd[0xc]|list[8]|list[0xc]; no return value observed; NARROW: all layout offsets hardcoded, no cfg tuning; test ignored; faithful __cdecl port.
    'global_ptrtable_match',      # MECHANISM: 2 stack args (uint32 key, pointer arg2); seeds 4-slot ptr table at cfg.tbl (slot 2 -> &entry, entry[0xc]=1, entry[0x28]=KEY=0xABCD01); arg2[4]=2/3 selects match/no-match test; observes ONLY return (0/1); NARROW: table size 4, tbl slot 2, entry offsets 0xc/0x28, KEY all hardcoded; only cfg.tbl is parameterised.
    'global_rec_clear_ret',       # MECHANISM: 2 stack uint32 args (ARG1=0xDEAD01 hardcoded, arg2=cfg.idx byte offset); seeds *(u32*)cfg.glob=&harness-buf with buf[off+0xc]=nonzero or 0 per test; observes buf[off+8]|buf[off+0xc]|ret; parameterised by cfg.glob and cfg.idx; ARG1 hardcoded so only the flag-and-return shape (not the arg1 value) is generic.
    'abs_scan_flag',              # MECHANISM: void fn() - no args; seeds cfg.span dwords at cfg.glob to zero and flag at cfg.tgt to 0x11; test0 places 1 at cfg.glob+cfg.idx (byte offset). Observes ONLY cfg.tgt post-call - return is not captured, FALSE-GREEN hazard if any other write is the real output. Params: cfg.glob, cfg.tgt, cfg.span, cfg.idx. Broad: zero-arg abs-range scanner that writes a single flag dword when any slot is nonzero.
    'global_2level_list_search',  # MECHANISM: One stack u32 arg (key); shared bufs cont/node/entry (both sides) so address comparisons in traversal compare equal. Seeds *cfg.glob=cont, cont[4]=node, node[0]=entry, entry[8]=KEY, entry[0xc]=RESULT. Calls fn(key); observes RETURN ONLY (u32; found->RESULT, miss->-1 - FALSE-GREEN if only one case tested). Traversal offsets (g[4], node[0/8], entry[8/0xc]) hardcoded; cfg.glob only parameter.
    'arg_flag_branch_getter',     # MECHANISM: u32 fn(pointer arg) - one stack ptr; seeds arg[0x20]=c, arg[0x1c]=flag byte, arg[0]=&p, p[0x40]=f from cfg.seed_sets[t]; arg and p buffers are SHARED across both sides so branches that return arg-struct-derived addresses compare equal rather than false-RED. Observes ONLY return value. Params: cfg.seed_sets[t]={c,flag,f}. Broad: any multi-branch getter over a single pointer struct where some branches return scalar fields and others return pointers into the same struct.
    'global_dll_insert_head',     # MECHANISM: One stack ptr arg; shared bufs arg (0x80) and S (0x40) - SHARED both sides so DLL ptr stores (node[0]=old-head, *glob=node, S[4]=&node[0]) compare equal. Seeds *cfg.glob=S, arg[0x34]=0xF (=node[0xc]). Calls fn(arg); observes arg+0x28[0|4], S[4], *cfg.glob, arg+0x34, ret. NARROW: node at hardcoded arg+0x28, link fields [0/4/0xc], ~1 clear mask baked in; cfg.glob only parameter.
    'global_fieldoff_clear',      # MECHANISM: One stack ptr arg; shared bufs arg (0x80) and entry (0x40) - SHARED both sides. Seeds *cfg.glob=V=0x10, arg[0x10]=entry+entry[0]=1+entry[4]=0x77 (t0) or arg[0x10]=null (t1). Calls fn(arg); observes arg[0x48], entry[0], entry[4], ret. V=0x10 and all field offsets hardcoded. NARROW: entry layout and arg[0x48] destination baked in; cfg.glob only parameter; partner to global_fieldoff_set.
    'multi_state_list_setter',    # MECHANISM: One stack arg (ptr p); test t = state; harness seeds p[0x48]=state, p[0x50]=0x11 sentinel; state 1 also seeds p[0x14]=&A, p[0x18]=&B. Shared p/A/B bufs so stored list pointers compare equal across both sides. Snapshots p[0x50]|p[0x14]|p[0x18]|A[4]|B[0]; tests {1,2,3,0}. No cfg fields. NARROW: field offsets (p[0x48]/0x14/0x18/0x50, A[4], B[0]) and state constants baked into reimpl.
    'byte_counter_struct',        # MECHANISM: 1 stack ptr arg to harness-allocated 0x20 struct; seeds p[0]=b0, p[1]=b1, p[3]=b3 from `cfg.seed_sets[t]={b0,b1,b3}`, all other bytes zeroed; observes p[0] and p[1] as u8 after call; return value not observed; no absolute globals; byte-slot layout (offsets 0, 1, 3) hardcoded but modular-wrap counter semantics are general for any function with this 3-byte struct shape.
    'arg_default_memcpy_abs',     # MECHANISM: void fn(pointer src) - one stack ptr; test0: src=harness-alloc buf filled 0xC0DE0000|k markers; test1: src=NULL so fn falls back to its internally-hardcoded abs default source. Observes cfg.copy_dwords dwords at abs dest cfg.tgt. Params: cfg.tgt, cfg.copy_dwords; the abs default source address is NOT a cfg key - it is embedded in the function code (both sides hit the same process address when src=NULL, so no seeding asymmetry).
    'byte_idx_table_bitclear',    # MECHANISM: 1 stack ptr arg to harness-allocated control struct; seeds p[0]=b0, p[1]=b1, p[3]=b3 from `cfg.seed_sets[t]={b0,b1,b3}`, p[4]=&tbl; pre-writes 0xFF at tbl[(b1+b0 mod b3)*0x14]; observes tbl[that offset] (expects 0xF7 = 0xFF & ~8) and p[1] (incremented by 1); stride 0x14 and bit-clear mask ~8 are hardcoded; return value not observed; no absolute globals.
    'struct_table5_search',       # MECHANISM: NARROW: Two stack ptr args (p1, p2); count read from HARDCODED offset p1+0x1d0, table ptr from HARDCODED p1+0x1d4; 5-byte entries (dword key @+0, byte val @+4) scanned backward; p2 dereferenced as dword search key; ONLY observable is return value (u8 val or 0) - false-GREEN if callee writes side-effects elsewhere; count hardcoded 4 in harness; no cfg fields; structurally bound to one fixed struct layout.
    'circular_list_search_node',  # MECHANISM: 2 stack args (list-head ptr, uint32 key); harness builds 3-object circular list with node=obj+0x4c, key stored at node-0x44, sentinel=list itself; both sides share harness-allocated objects so returned pointer is directly comparable; observes RETURN VALUE ONLY (u32, node-0x4c on match or 0 on miss); all offsets (0x4c, 0x44) hardcoded; no cfg knobs.
    'global_fieldoff_set',        # MECHANISM: One stack ptr arg; shared bufs arg (0x80) and entry (0x40) - SHARED both sides. Seeds *cfg.glob=V=0x10, arg[0x48]=0x66; t0: entry[0]=0 (write path); t1: null entry; t2: entry[0]=5 (occupied/early-ret). Calls fn(arg); observes entry[4], arg[0x48], entry[0], ret. V=0x10, all offsets, and the constant 0x557b70 written to arg[0x48] are hardcoded. NARROW: cfg.glob only parameter; partner to global_fieldoff_clear.
    'eax_dest_memcpy_init',       # MECHANISM: NARROW: fn(EAX=dest via `mov eax,dest; jmp`, four stack args: src ptr/arg2 ptr/arg3 u32/arg4 u32); ORIG as NativeFunction(void,[ptr,ptr,u32,u32]); REIMPL __cdecl(dest,src,arg2,arg3,arg4); seeds src 16-dword markers + *arg2 scalar; observes dest[0..0x3c]+dest[0x40,0x48,0x4c,0x50,0x54,0x58]; ARG3/ARG4/obs offsets hardcoded in harness - not reusable for other struct-init shapes.
    'struct_div_mod_compute',     # MECHANISM: 5 stack args (ptr arg1, u32 val, u32 arg3=1, u32 arg4=2, out-ptr arg5); seeds arg1[0x18]->divTbl (div at arg4*0x28+0x20), arg1[0x10]->baseTbl (BASE at arg3*0x20+0x1c), arg1[0x20]=MULT; observes *arg5 (remainder) | ret (base+MULT*quotient formula); cfg.seed_sets drives {val, div}; arg3/arg4 values and all table offsets hardcoded.
    'ring_copy_5ab980',          # void fn(arg): esi=arg[0xc]-*0x7dd610; cnt=*0x7dd614-esi; if(cnt>=arg[0x14]) cnt=arg[0x14]; memcpy(arg[0x18], 0x7dce08+esi, cnt); arg[0x18]+=cnt; arg[0x14]-=cnt. seed g610/g614 + ring markers + arg fields; snapshot dest dwords|arg[0x18]|arg[0x14]. non-degen
    'struct_init_3arg_sub',       # MECHANISM: 3 stack args (ptr a, u32 b, ptr dest); seeds a[4] and b as constants, dest[0x60]->harness sub-buf; observes dest dwords at offsets [0..0x28] + sub dwords at [0x38,0x3c,0x40,0x44,0x48,0x50]; no cfg fields parameterise field offsets - all hardcoded in snapI3 - NARROW to this struct layout.
    'flag_branch_struct_2way',    # MECHANISM: Two stack args (ptr p, u32 arg2); shared bufs p/f/sub/s. t=0: f[0x50]&8 set -> sub[0x88]=0, sub[0x8c]=arg2. t=1: flag clear -> val=(s[0x38]>>3)*s[0x39]*arg2; p[0x8c/0x90]=val, p[0x88]=arg2, p[0x28]|=0x400. Observes sub[0x88|0x8c], p[0x8c|0x90|0x88|0x28]. NARROW: all offsets (0x94/0x84/0x11c/0x50/0x38/0x39/etc.) baked in; no cfg.* parameters.
    'abs_region_zeroer',          # MECHANISM: `void fn()` no stack args; sentinel-fills 6 HARDCODED abs addresses (cfg.glob+{0, 0x1c}, cfg.glob+0x8c+0x1c, cfg.glob+5*0x8c+0x1c, cfg.glob+0x64, and cfg.tgt) with 0xA5A5A5A5; calls fn(), snapshots same 6 addresses; NARROW: stride (0x8c) and observed-offset set are hardcoded in the handler body - not configurable; written for one specific abs-record-array layout.
    'array_fill_2way',            # MECHANISM: void fn(pointer p, pointer src) - two stack ptrs; harness-allocs arr1 and arr2, seeds p[0]=&arr1, p[4]=&arr2, p[0xc]=count HARDCODED 3; src=3-dword marker vec3 (0xC0DE0001/2/3); expects fn to copy src vec3 into arr1[i] and zero-fill arr2[i] per element, stride HARDCODED 12. Observes countx3 dwords from each array. Params: none in cfg - count (3) and stride (12) are hardcoded in handler. NARROW: reuse for a different count or element stride requires a body edit.
    'abs_table_state_setter',     # MECHANISM: u32 fn(uint i, uint arg2) - two stack scalars; abs table at cfg.glob with stride HARDCODED 0x50 in harness body (no cfg.stride key); drives 4 HARDCODED test specs covering OOB/arg2==0/state==3/else; seeds rec[0x20]=pre, rec[0x1c]=0xEE. Observes rec[0x20]|rec[0x1c]|return. Params: cfg.glob only. NARROW: stride (0x50) and all four test vectors are hardcoded in the handler; reuse requires a body edit.
    'esi_edx_predicate',          # MECHANISM: NARROW: fn(ESI=s, EDX=e) - ORIG via `mov esi,bufS; mov edx,bufE; jmp` trampoline; REIMPL __cdecl(s,e); seeds s[0x10]/s[0x14]+e[0x10]/e[0x14] for match(t0->1) or no-match(t1->0); ONLY observable is return value (0/1). Field offsets 0x10 and 0x14 hardcoded in the harness - not reusable for other struct layouts without modifying the branch body.
    'edx_ebx_edi_find',           # MECHANISM: fn(EDX=arr ptr, EBX=key, EDI=n); ORIG via call-trampoline saving/restoring callee-saved EBX+EDI; REIMPL __cdecl(arr,key,n); arr harness-allocated with KEY=0x1234 (two entries) + TERM=0xff070000 (all hardcoded - no cfg params for key or terminator); walks to find (n+1)-th entry==key, returns following dword; ONLY observable is return value. More general than name implies: any 0xff070000-terminated dword-pair array scan fits.
    'ebx_edi_global_find',        # MECHANISM: fn(EBX=key, EDI=n); ORIG via call-trampoline saving/restoring callee-saved EBX+EDI (push edi,ebx...pop ebx,edi); REIMPL __cdecl(key,n); arr fetched via *(cfg.tgt + *(cfg.glob)*0x40) - cfg.glob and cfg.tgt parameterise the two-level lookup; harness seeds *(glob)=0 + *(tgt)=&arr; arr entries and TERM=0xff070000 hardcoded; walks for (n+1)-th match, returns following dword; ONLY observable is return value.
    'strided_color_fill',         # MECHANISM: NARROW: no stack args; seeds *0x771530=fresh buf + 4 BGRA bytes at 0x616030; observes 3 entries at buf+0x1d+k*0x20 (k=0,1,895); all addresses (0x771530, 0x616030), stride (0x20), count (896), and intra-entry offset (0x1d) hardcoded - no cfg fields parameterise any of them.
    'bitmap_alloc_slot',          # MECHANISM: void fn() - no args; seeds ABS bitmap at HARDCODED 0x6bf198 (32 bytes all-0xFF, first-clear bit=K) and ABS record array at HARDCODED 0x693198+Kx0x2c0 with sentinel 0xEE at fields [0x2b0..0x2bc]; K  in  {5,0} per test. Observes rec[0x2b0|0x2b4|0x2b8|0x2bc], bitmap byte at BMP+(K>>3), and return. Params: none - both abs addresses, stride (0x2c0), and all observed field offsets hardcoded. NARROW: tied to two specific abs addresses and one struct layout.
    'state_list_insert',         # void fn(p, _, state_src): state=*state_src; sub=p[0x20]; if(state==1 && sub[0x28]!=3) sub[0x28]=8; elif(state==3 && sub[0x28]!=5) sub[0x28]=4; sub[0x20]=state; old=p[0x14]; if(old){*(p[0x18])=old; *(old+4)=p[0x18];} node=p[0x24]+0xc; nx=*node; p[0x18]=node; p[0x14]=nx; *(nx+4)=&p[0x14]; *node=&p[0x14]. test state 1/3 (p[0x14]=0 empty list). shared bufs; snapshot sub+list ptrs. non-degen
    'multi_deref_global_set',     # MECHANISM: Two stack args (ptr p1, ptr p2); harness seeds abs 0x7dc57c=0 and builds nested chain p1[4]->E->{obj, X->Ncell->N}; t0 p2=&val (Ncell->N), t1 p2=0 (fn reads fallback from abs 0x613290). Snapshots obj[0xc4]|N[4]|obj[0x40]. No cfg fields. NARROW: both abs globals (0x7dc57c, 0x613290) and the full chain layout (p1[4]/E[0]/E[0x18]/X[0x20]) are hardcoded in harness.
    'list_node_const_init',       # MECHANISM: Two stack args (ptr p, ptr arg2); harness walks p[0x18]->s, reads s[0x24]=count + s[0x20]=arr (node-ptr array); per node writes *arg2->node[4] and float literals 1.0f/1.0f/0.5f->node[0xc/0x10/0x14] (no arithmetic). Snapshots node[4,0xc,0x10,0x14] x count. No cfg fields. NARROW: chain offsets and float constants are baked into the reimpl; harness count is hardcoded 3.
    'bounded_struct_push',        # MECHANISM: 3 stack ptr args (state-struct p, vec3 arg2, dword arg3), all harness-allocated; seeds p[0]=buf, p[4]=cap=4, p[8]=top (0 for push path, 4=cap for full path), p[0x54]=sentinel, arg2=3-dword marker, arg3=1-dword marker; observes buf[top*0x30+0..0x1c], p[8], p[0x54]; stride 0x30 and cap 4 are hardcoded; NARROW to this stride/cap combination; return value not observed.
    'trie_walk',                  # MECHANISM: u32 fn(ptr node, uint32 key, uint32 depth) on stack; harness builds a fresh 2-level trie (child at `node[nibble*4+0x1c]`, nibble=`key&0xf` per level); ONLY observable is u32 return `(leaf&~0xff)|leaf[0x18]`; no cfg parameterisation - trie offsets, test keys (0x21/0x35), and the return formula are hardcoded in the handler body.
    'struct_delta_flag_init',     # MECHANISM: 5 stack ptrs (out, a, b, c, d); seeds a/b/c/d each as 2 floats (xy pairs); observes out fields at hardcoded offsets [0x10,0x14,0x28,0x2c,0x40,0x44,0x58,0x5c,0x64] + u32 return (fcomp-driven flag); reimpl is verbatim naked __asm; no cfg fields parameterise offsets - NARROW to this exact struct layout.
    'table_accum_clamp',          # MECHANISM: 3 stack args (u32 a1, int32* p2, int32* p3); seeds *p2=v2, *p3=v3 per cfg.seed_sets[t]={a1,v2,v3}; tables tblA/tblB at hardcoded .rdata addresses (0x634498/0x634478) left REAL and identical on both sides; observes *p2|*p3 after accumulate+clamp; cfg.seed_sets is the only parameterisation.
    'fastcall_float_clamp',       # MECHANISM: __fastcall fn(ECX=idx, EDX=base ptr, [esp+4]=val float); ORIG via `mov ecx,idx; mov edx,base; push valBits; call; add esp,4; ret` trampoline; REIMPL naked __cdecl(idx,base,val) with exact x87 (fld/fadd/fst/fcomp); seeds base[idx*4]=cur float per cfg.seed_sets[t]={idx,cur,val}; observes base[idx*4] as u32 bits after call. 50.0f clamp cap and 80-bit x87 compare baked in reimpl - applies to exactly this clamp shape.
    'list_walk_self_write',       # MECHANISM: Two stack args (ptr p, uint32 value); harness builds len-node linked list where last node is self-pointing; relies on abs global 0x911ae4 being .bss-zero at spawn so write target = terminal_node_addr, landing at terminal[0]. Snapshots terminal[0] only (no return). cfg.seed_sets[t]={len,value}. NARROW: abs global 0x911ae4 is hardcoded in harness and verbatim __asm reimpl.
    'eax_ecx_float_hash',         # MECHANISM: fn(EAX=a, ECX=b) -> float; ORIG via `mov eax,a; mov ecx,b; call; ret` trampoline (ST0 return), NativeFunction(float,[]); REIMPL naked __cdecl(a,b)->float; test vectors from cfg.seed_pairs[t]=[a,b]; ONLY observable is float return compared as u32 bit pattern - false-GREEN if reimpl writes wrong float. Broadly applicable to any two-int-register -> float fn whose body reads only EAX+ECX.
    'case_insensitive_ncmp',      # MECHANISM: 3 stack args (char* s1, char* s2, int n), both string buffers harness-allocated from `cfg.seed_sets[t]={s1,s2,n}`; observes RETURN VALUE ONLY (int - false-GREEN hazard: no side-effects to check); no globals, no out-pointers; reimpl is verbatim naked `__asm`; PURE_LEAF comment notes asymmetric toupper (s1 folds 0x5b-0x60, s2 skips), so test vectors must include that range to expose divergence.
    'aabb_sphere_overlap',        # MECHANISM: `int fn(box*, sphere*)` 2 ptr stack args; harness allocates two bufs, seeds 6 floats into b1 (min[0,4,8] max[0xc,0x10,0x14]) and 4 floats into b2 (center[0,4,8] radius[0xc]) from cfg.seed_sets[t]; observable = return value only; no global seeding or writes; layout FIXED - byte offsets not configurable; NARROW in layout but cfg.seed_sets is freely configurable per-handler.
    'circular_str_search_ci',     # MECHANISM: 2 stack ptr args (list* arg1, char* query); harness builds 3-node circular list (alpha/beta/gamma) with node[0]=next, node+8=key string, sentinel=arg1+8; query from `cfg.seed_sets[t]={q}`; observes RETURN VALUE ONLY (pointer - node-8 on match, 0 on miss); both sides share harness-allocated nodes so returned pointer is directly comparable; reimpl is verbatim naked `__asm`; all layout offsets hardcoded; folding is +0xe0 not `&0xdf`.
    'byte_format_hexdump',        # MECHANISM: void fn(ptr a1, ptr out, ptr a3) on stack; seeds 4 bytes at `a1[0x11c]` from `seed_sets[t].bytes`; a3 is an optional payload pointer toggled by `seed_sets[t].payload`; observes `out[0..0x70]` as a hex string; cfg: `seed_sets` ({bytes:[4], payload:bool}); NARROW: struct offset 0x11c and output window 0x70 are hardcoded - handler written for one specific formatter layout.
    'pool_freelist_init',         # MECHANISM: 1 stack arg (pointer pool P); NARROW: harness seeds P[0x14]=&buf (zeroed harness buffer), P[0x16c]=n; calls fn(P); observes P[0x18/0x1c/0x20/0x168/0x170/0x194/0x198] + freelist link field B[kx0x24+0x1c] for 5 nodes; reimpl is verbatim naked __asm; cfg.seed_sets[t].n is the only configurable field; all pool struct field offsets and node stride (0x24) + link offset (0x1c) are hardcoded.
    'bitmap_blit',                # MECHANISM: 2 stack ptr args (dst, src struct headers), both harness-allocated; seeded from `cfg.seed_sets[t]={rows,width_bits,channels,dstride,sstride,pal_bits,palette}` with inline pixel/pal buffer ptrs embedded in the structs; observes first 0x40 B of dst-pixel buffer and dst-pal buffer (both harness-owned, so pointers equal across sides); return value not observed; reimpl is verbatim naked `__asm`; all field offsets hardcoded (+4=channels, +8=rows, +0xc=width_bits, +0x10/0x14/0x18 for stride/px/pal).
    'record_array_filter_update', # MECHANISM: Stack args (ptr A1, ptr A2, u32 a3, u32 a4, u32 a5, u32 a6); shared A1 (4 records of 0x10 bytes starting at A1+0x18, fields at R+0/+4/+8/+0xc) and A2 (A2[4]=N=4, A2[8]=10) reset via seedR() before each side; scalar args a3..a6 from cfg.seed_sets[t]; observes A1[0] + per-record R[0] and R[0xc]; applies to any filter+update over this fixed record layout.
    'heap_alloc_aligned',         # MECHANISM: 3 stack args (heap-descriptor pointer, size, align) from cfg.seed_sets[t]; harness builds a single-block heap (block H with sentinel, used=0x10) in shared buffers; observes return-as-H-offset|block0-next; shared bufs both sides so returned pointer compares equal; parameterised by cfg.seed_sets (size, align per test vector).
    'near_leaf_abs_table',        # MECHANISM: One uint32 stack arg; resets cfg.tbl_count records (stride cfg.tbl_stride) at abs cfg.tbl_base applying cfg.seed_sets[t].preset per record; calls fn(arg); snapshots cfg.observe offsets at sampled entries [0, count/2, count-1]. No return value observed. cfg fields: tbl_base, tbl_stride, tbl_count, observe, seed_sets[t]={arg,preset}. BROAD: fully parameterized for any fn driving a C3 callee that writes into an abs strided record table.
    'near_leaf_seed_ret',         # MECHANISM: 0 args; harness zeroes abs table cfg.tbl_base (cfg.tbl_count entries x cfg.tbl_stride bytes) then applies seed_sets[t].preset=[[idx,val],...]; calls fn(); ONLY OBSERVABLE IS RETURN VALUE (u32) - false-GREEN hazard if fn also writes globals; cfg.tbl_base/tbl_stride/tbl_count/seed_sets[t].preset; applies to any zero-arg NEAR-LEAF reading a seedable abs table via a C3 callee.
    'near_leaf_memcmp16',         # MECHANISM: Three stack args (uint32 unused=0, ptr H, ptr arg3); H[0]=&bufP; fills bufP and arg3 with sequential bytes; if !sp.eq sets arg3[sp.diffat]=0xFF; calls fn(0,H,arg3). ONLY observable: return value (0/1); false-GREEN hazard. cfg fields: seed_sets[t]={eq,diffat}. NARROW: arg2-deref-once pattern and 16-byte comparison length are hardcoded in harness; no cfg addresses; no side-effect check.
    'near_leaf_arr_to_table',     # MECHANISM: One stack ptr arg; fills arg[0..n-1] from cfg.seed_sets[t].vals (n=vals.length, dynamic), zeros abs tbl_base[ixstride]; calls fn(arg); snapshots tbl_base[ixstride] per i. No return value observed. cfg fields: tbl_base, tbl_stride, seed_sets[t]={vals:[...]}. BROAD: any fn iterating a contiguous ptr-arg array and scattering each element into an abs strided table via a C3 callee; entry count driven by vals.length.
    'near_leaf_dot_plane',        # MECHANISM: Two stack ptr args (a1, a2); seeds a1[0x20]=idx + a1[0xa8]=a8 + a2[0x20..0x28]=point vec3 + abs rec at cfg.tbl_base+idxxcfg.tbl_stride [0..8]=normal vec3; calls fn(a1,a2); snapshots a1[0xac]|a1[0xa8]. cfg fields: tbl_base, tbl_stride, seed_sets[t]={idx,normal[3],point[3],a8}. Table address/stride are cfg-parameterized; field offsets within a1 and a2 are hardcoded in harness.
    'near_leaf_seed_globals',     # MECHANISM: No-arg fn; before each call seeds N absolute globals from cfg.seed_sets[t].globals=[[addr,val],...]; ONLY observable is the u32 return value - no side-effect globals or out-buffers are captured. cfg.seed_sets drives all seeding. Applies to any near-leaf that reads seeded globals and returns a computed int/bool.
    'near_leaf_seed_arg_obs',     # MECHANISM: 1 stack arg (uint32 arg); harness seeds arbitrary absolute globals from seed_sets[t].globals=[[addr,val],...], calls fn(arg); ONLY OBSERVABLE IS ONE u32 at cfg.obs_addr - false-GREEN hazard if fn writes additional locations; cfg.obs_addr/seed_sets[t].{globals,arg}; applies to any single-uint32-arg NEAR-LEAF with one configurable observable output global.
    'near_leaf_ptr_array_search', # MECHANISM: 2 stack args (uint32 key, uint32 gate); harness allocates cfg.count structs seeded struct[k][0]=0x10000+k and struct[at_idx][0]=key, then writes *cfg.glob=arr; ONLY OBSERVABLE IS RETURN VALUE (u32) - false-GREEN hazard if fn has side effects beyond the return; cfg.count/cfg.glob/seed_sets[t].{gate,key,at_idx}; applies to any fn(key,gate) scanning a configurable-size pointer array at a fixed global.
    'near_leaf_seed_multi_obs',   # MECHANISM: Zero-arg call on stack; seeds per-test absolute globals from cfg.seed_sets[t].globals [[addr,val],...]; observes cfg.observe_addrs list of absolute globals (hex) after call. Return value NOT observed (false-GREEN hazard). Broad: any void zero-arg fn whose complete observable effect is writes to absolute globals; name "near_leaf" is misleading - applies to any depth as long as no pointer args are needed.
    'near_leaf_record_builder',   # MECHANISM: 2 stack args (uint32 A=rec_idx, uint32 B=value); seeds rec at cfg.rec_base+Axcfg.rec_stride (fields v0/v1/sentinel at [0/4/8]) plus two abs table slots at cfg.tbl_base+Axcfg.tbl_stride and (A+0xa)xcfg.tbl_stride; observes rec[8]|slot_A|slot_(A+0xa); both sides invoke the SAME real C3 setter callee for table writes; cfg.rec_base/rec_stride/tbl_base/tbl_stride/seed_sets[t].{A,B,v0,v1}.
    'near_leaf_accum_table',      # MECHANISM: Three stack args (uint32 a1, float val, uint32 a3); seeds float slot at abs cfg.tbl_base + a1xcfg.rec_stride + a3x4 with sp.seed; calls fn(a1,val,a3); snapshots that one slot as u32 bits. No return value observed. cfg fields: tbl_base, rec_stride, seed_sets[t]={a1,val,a3,seed}. BROAD for any fn forwarding to a C3 fastcall callee that accumulates a float into a 2D-indexed abs table slot; only one slot is observed.
    'near_leaf_struct_array_predicate',# MECHANISM: 0 args; harness allocates cfg.count structs of cfg.struct_size bytes each and writes harness pointers or nulls into abs pointer array ptr(cfg.glob); seed_sets[t].entries[k]={null:bool}|{fields:[[off,val],...]} seeds each slot; ONLY OBSERVABLE IS RETURN VALUE (u32 predicate) - false-GREEN hazard; cfg.glob/count/struct_size/seed_sets; struct field layout fully configurable via fields[].
    'near_leaf_global_str_search',# MECHANISM: One stack ptr arg (char* query); harness builds 3-node circular list (node[0]=next, node[8]=string; entries 'alpha'/'beta'/'gamma') and seeds *cfg.glob=list-head; calls fn(q). ONLY observable: returned pointer (shared bufs, so address comparison is valid). cfg fields: glob, seed_sets[t]={q}. BROAD for any fn delegating to a circular-list string-search callee with the list root at *cfg.glob.
    'near_leaf_list_search',      # MECHANISM: u32 fn(uint32 key) on stack; seeds linked list directly into live game global `ptr(cfg.glob)+0x10` (real struct mutated in place, not sandboxed); node layout `node[0]=payload, node[8]=key, node[0x30]=next`; `cfg.seed_sets[t]` provides empty/query/nodes cases; ONLY observable is u32 return; cfg: `glob`/`seed_sets`; valid in the pre-crash window because the global struct is idle; mutates game memory - not safe post-engine-init.
    'near_leaf_memset2',          # MECHANISM: 2 stack args (pointer dest, uint32 count); harness pre-fills a 0x80-byte buffer with 0xCC, calls fn(dest, count), snapshots dest[0:0x20] as hex - first count bytes become 0x00, rest stay 0xCC, zero/0xCC boundary shifts with count (non-degenerate); fn is void, no return-value observable; cfg.seed_sets[t].count; applies to any NEAR-LEAF wrapping a C3 memset callee with this two-arg shape.
    'near_leaf_seed_outbuf',      # MECHANISM: Stack args from cfg.seed_sets[t].args; abs table slots seeded from cfg.seed_sets[t].seeds=[[addr,val]...]; harness allocates out-buffer (cfg.out_size bytes) inserted at cfg.out_argpos; observes cfg.out_observe offsets in outbuf +/- cfg.fold_ret for return; both sides call same live C3 getter callees so verbatim naked port compares equal. Call shape from cfg.argkinds (default ['pointer']).
    'struct_list_float_set',      # MECHANISM: 2 stack args (struct*, float vol); builds harness 1-node self-circular list (head=struct+0xc, sentinel=struct+0xc) + optional secondary at struct+0x11c; seeds vol varies per test; observes struct[0x38]|node[0x14]|secondary[0x30]; field offsets (0x38,0x0c,0x14,0x40,0x11c,0x30) all hardcoded - NARROW to this layout.
    'seed_indirect_ctx_obs',      # MECHANISM: No-arg call (nargs=[]); seeds cfg.ptr_array[cfg.depth_idxx4]=&ctxbuf (fresh harness buffer, prevents the null-deref AV noted in comment), *cfg.depth_global=depth_idx, optional ctxbuf[cfg.ctx_seed_off] and cfg.seed_globals abs globals; observes return + ctxbuf[cfg.observe_offs] + cfg.observe_globals; all addresses and offsets configurable; applies to any zero-arg function that fetches its context via a pointer-array indexed by a depth global.
    'indexed_float_sum2',         # MECHANISM: One stack arg (uint32 idx); seeds two adjacent floats at cfg.tgt+idx*cfg.stride as [idx+1.0, idx*0.5]; observes float return value ONLY - false-GREEN hazard if reimpl returns matching value coincidentally; cfg.tgt and cfg.stride configurable; general for any fn summing two consecutive floats at an indexed abs slot.
    'double_indexed_float_mul',   # MECHANISM: fn(int idx) stack with idx=0 hardcoded by harness; seeds cfg.aTbl=0, cfg.bTbl=t, cfg.fTbl+t*4=float(t+1.0); observes ONLY float return (false-GREEN hazard); cfg: aTbl, bTbl, fTbl; multiplier constant K baked into reimpl; NARROW: only the idx=0 path exercised so stride S between table entries is never varied; test=e (bTbl seed, varied for non-degen).
    'struct_tag_equals',          # MECHANISM: 2 stack ptrs (a, b); seeds two harness-allocated 0x80 bufs identically (tag at [0]), then optionally perturbs b[scen.diff]=0xDEADBEEF per cfg.scenarios[t]={tag,diff}; observes return value ONLY - FALSE GREEN hazard: a reimpl returning the same int without correct dispatch logic passes undetected.
    'indexed_float_accum16',      # MECHANISM: Three stack args (pointer out [harness-alloc'd], uint32 i, uint32 j); seeds 16 floats at cfg.tbl_base+i*iStride+j*jStride+regionOff-4 (fill+k pattern) when in-bounds (i<0x10, j<4); observes return int + out dword as "ret:hex"; K multiplier baked into reimpl; cfg.tbl_base, cfg.iStride, cfg.jStride, cfg.regionOff, cfg.scenarios drive all params; bounds-fail (ret 0) case included.
    'bounded_table_signselect_clamp',# MECHANISM: 2 stack uint32 args (idx, val); seeds real absolute tables cfg.t1Tbl/t2Tbl/t3Tbl from `cfg.scenarios[t]={idx,val,byte,slot}`; t3Stride is configurable via cfg.t3Stride; observes both the return value AND t3Tbl[idx*t3Stride] as s32; out-of-bounds idx returns 0 with no table write (hazard: sign-select path not exercised on that test); seeds live .bss globals that persist across tests.
    'seed_globals_arg_multiobs',  # MECHANISM: Stack call fn(sp.arg) with single uint32 arg drawn from cfg.seed_sets[t].arg; seeds per-test absolute globals from sp.globals [[addr,val],...] in the same seed_sets entry; observes cfg.observe_addrs list of absolute globals (hex) after call. Return NOT observed (false-GREEN hazard). Broad: any void fn(int) whose observable effects are entirely writes to absolute globals; differs from near_leaf_seed_multi_obs only in the single varying int arg.
    'succ_approx_quantize',       # MECHANISM: 3 stack args (u32 arg1, int32* p2, int32* p3); seeds p2=s.cur, p3=s.idx, cfg.rangeTbl[s.idx*2]=s.range per cfg.scenarios[t]; deltaTbl at absolute .rdata addr left REAL (seeding it diverged Orig/Reim - harness artifact noted in-code); observes *p2|*p3; cfg.rangeTbl address and cfg.scenarios parameterise.
    'multi_array_scatter',        # MECHANISM: One stack arg (ptr st); harness builds 7 harness-alloc arrays in st[0x10..0x28] and seeds 10 HARDCODED abs globals (0x692528-0x692534) with distinct markers; calls fn(st); snapshots each array at [counter*stride] + st[0xc]. cfg.scenarios[t]={counter,bound}. NARROW: source global addresses and per-array strides [12,8,4,64,4,16,32] are fully hardcoded; only counter/bound are parametric.
    'dll_head_insert',            # MECHANISM: fn(ptr p) stack; node=p[0xa0]; gate=(node[3]&3)==0 (skip insert if flag bits set); seeds *cfg.glob=G (list head), G[0xbc]=H (old head); on insert rewrites node[3]|=3, p[3]|=0xc, node[8]/node[0xc]/G[0xbc]/H[4]; SHARED bufs so written addresses compare equal across sides; cfg: glob, scenarios (flag byte); NARROW: node offset 0xa0 and sentinel layout (G[0xbc]) hardcoded.
    'idx2_record_condset',        # MECHANISM: 3 stack int args (i, j, v) from cfg.scenarios[t]; computes off=(j+5i)*cfg.recStride (multiplier 5 hardcoded in harness); seeds *(float*)(cfg.baseA+off)=s.cur and *(u32*)(cfg.baseB+off)=sentinel; observes both slots as raw u32 hex; parameterised by cfg.baseA, cfg.baseB, cfg.recStride, cfg.scenarios.
    'quad_buffer_build',          # MECHANISM: Stack args (ptr out, u32 maxsize, ptr rec); seeds rec[0x14]=cnt, rec[0x18]=&arr, arr[0x14+kx0x28]=&Ps[k], Ps[k][0xd]=subs[k]; shared out buffer reset to 0xEEEEEEEE via seedB() before each side; observes s32 return + out at fixed offsets [0,4,12,64,68,76]; parameterised by cfg.scenarios[t] supplying subs[] and maxsize; applies to any 2-pass quad-buffer builder taking (out, maxsize, rec).
    'eax_out_2float',             # MECHANISM: fn(EAX=out* via `mov eax,obuf; jmp`, float a1+a2 on stack); ORIG and REIMPL both wrapped via mkE2 trampoline (NativeFunction(void,[float,float])) so both write into the same obuf (reset to 0xCCCCCCCC between sides); scenarios from cfg.scenarios[t]={a1,a2}; observes obuf[0..4] (2 dwords hardcoded). Broadly applicable to any EAX-implicit-out + two float stack-arg fn.
    'dll_merge_swap',             # MECHANISM: fn() zero stack args; seeds *cfg.glob_a=mybuf, *cfg.glob_b=0; mybuf[0x20]=B, mybuf[0x24]=A, mybuf[8]=sentinel; B self-loops (B[0]=B, empty-B path -> early-exit swap); observes mybuf[0x20]|mybuf[0x24]|mybuf[8]; cfg: glob_a, glob_b, scenarios (A/B role-swap for non-degen); NARROW: list-head offsets 0x20/0x24/0x08 in mybuf hardcoded; verbatim naked reimpl.
    'find_node_struct_copy',      # MECHANISM: Two stack ptrs (p1, p2); shared bufs both sides so stored-ptr writes compare equal. Seeds p1 pattern (cfg.scenarios[t].{pat,pat2}), node[8]=p1[8], node[0]=0x10b. Calls fn(p1,p2); observes node[0|0x198|0x19c]+ret. NARROW: key+tag offsets, 0x67-dword primary copy + p1[0x16c]*9-dword secondary copy (->node+0x19c) hardcoded; cfg.scenarios sets patterns only.
    'nested_list_search',         # MECHANISM: 1 stack arg (uint32 key); NARROW: harness builds a two-level circular list at *cfg.glob with all offsets hardcoded - outer next @+0, inner head @outer-0xc, inner sentinel @outer-0x10, inner next @+4, inner payload ptr @+8, key field @payload+0xc; ONLY OBSERVABLE IS RETURN VALUE (u32); cfg.glob/scenarios[t].{pval,key}; narrow because no link offsets are configurable.
    'pixel_max_alpha',            # MECHANISM: 1 stack arg (pointer struct s); NARROW: builds struct at hardcoded offsets s[0xc]=mode, s[8]=rows, s[4]=cols, s[0x10]=stride, s[0x14]/s[0x18]=base ptr; base buffer seeded RGB byte-pattern, alpha bytes (per-pixel offset +3) preset 0xEE sentinel; observes return value + base[3]|base[7]|base[0x43] (alpha of 3 pixels); scenarios[t].{mode,rows,cols,stride}; narrow to this exact struct layout and 4-byte RGBA pixel format.
    'engine_register_funcs',      # MECHANISM: fn() - no args; seeds *cfg.glob=a 0x140-byte buffer (sentinel 0xCCCCCCCC), calls ORIG+REIMPL as void(); observes ret + buffer slots at cfg.observe_offs. Broadly applicable to any no-arg fn that stores fixed constants/funcptrs into (*cfg.glob)+offsets; cfg.observe_offs must cover all written slots - uncovered slots are a false-GREEN hazard.
    'eax_struct_deref_write',     # MECHANISM: NARROW: fn(EAX=s via `mov eax,sbuf; jmp`), no stack args; guards s[0x1b4]!=s[0x1b8] (early-exit); seeds 12 ptr-fields at hardcoded offs [0x108-0x12c, 0x174, 0x178] all->P1, then P1[0x18]->P2, P2[0x20]->P3, *P3->P4 (4-level deref chain); reads cfg.tbl[s[0x1b4]*4]; observes s[0x1b8]+P4[4]; cfg.scenarios[t]={idx,prev}. All deref offsets fixed.
    'particle_pool_alloc',        # MECHANISM: 2 stack args (pointer a1, uint32 a2); NARROW: seeds abs pool at cfg.glob as hardcoded 10 slots x stride 0x24 (slot[0]=in-use flag, slot[0x1c]=priority); a1 is harness-allocated [0x111,0x222,0x333], a2 hardcoded 0x444; scenarios[t].{used:[idxs],pris:[...]} selects which slots are pre-occupied; observes slot[+0]|slot[+4] for slots 0/1/9; narrow: 10-slot count, 0x24 stride, field offsets, and test arg values all hardcoded.
    'thunk_node_write',           # MECHANISM: void fn(ptr p, uint32 a2, uint32 a3) on stack; seeds `*cfg.glob=table`, `table[0]=node`, `p[0x14]=0`; calls; observes `node[0xa4]|node[0xa8]|node[0x40]`; cfg: `glob` only; NARROW: offsets 0xa4/0xa8/0x40/0x14 and OR-mask 0x10000000 are hardcoded in the handler - not parameterised by any cfg field.
    'thunk_field_copy',           # MECHANISM: 2 stack ptrs (p, out); seeds p[0x18]->sp, sp[0x24]=count, sp[0x20]->src (pattern from cfg.scenarios[t]={pat,count}); uses SEPARATE per-side out bufs; observes out[0..count-1] dwords; NEAR-LEAF adjustor thunk; count and pattern configurable via cfg.scenarios; field offsets (0x18,0x24,0x20) hardcoded.
    'thunk_cond_or',              # MECHANISM: 3 stack args (ptr p, u32 a2, u32 a3); seeds p[0x18]->sp (shared harness buf), sp[8]=sc.seed per cfg.scenarios[t]={a2,a3,seed}; observes ret|sp[8] after: if(a3) s[8]|=a2; return s[8]; NEAR-LEAF adjustor thunk; field offsets (0x18 chain, s[8]) hardcoded; cfg.scenarios is the only parameterisation.
    'thunk_list_count',           # MECHANISM: uint fn(ptr p) on stack; harness builds fresh n-node circular list at `p+0xc` (next at `node+4`, sentinel=head) from `scenarios[t].n`; calls; ONLY observable is the u32 return value; cfg: `scenarios[t].n`; broadly applicable to any circular list count reading a `p+0xc`/+4-linked sentinel-headed chain, including on zero nodes.
    'thunk_float_sub',            # MECHANISM: void fn(uint32 idx, float fval) on stack; seeds `cfg.tbl+idx*cfg.stride+cfg.field_off` with float from `scenario.seed`, calls, observes same slot as u32 hex; cfg: `tbl`/`stride`/`field_off`; `scenarios[t].{idx,seed,fval}`; broadly applicable to any strided float-field in-place subtract - all three coordinates are cfg-driven, not hardcoded.
    'bounded_thunk_orflag',       # MECHANISM: 2 stack uint32 args (idx, a2); harness plants a ptr at `cfg.tbl[5]` (index hardcoded to 5 in harness regardless of sc.idx) to a harness-allocated s buffer; seeds s[2]=sc.s2 from `cfg.scenarios[t]={idx,a2,s2}`; observes s[2] byte only, return value not captured; sc.idx=5 exercises the or-flag path, other sc.idx values test bounds-exit or null-slot; cfg.tbl is a real absolute table address; NARROW: slot index 5 hardcoded.
    'bitfield_range_set',         # MECHANISM: void fn(uint8** pbuf, uint startbit, uint nbits, int fill) - 4 stack args; harness allocates a 24-byte buf (N HARDCODED), seeds all bytes to cfg.scenarios[t].seed, writes buf's address through a shared pbuf (same pbuf on both sides so stored-pointer reads compare equal). Observes all 24 bytes as a hex string. Params: cfg.scenarios[t]={startbit,nbits,fill,seed}. Broad: any bit-range setter taking a uint8** to a byte buffer; N=24 is the only hardcoded constraint.
    'esi_struct_init',            # MECHANISM: Void fn with ptr in ESI; orig driven via `mov esi,buf; jmp rva` trampoline (no stack args); reimpl called as __cdecl(buf) - no naked asm required; same buf reseeded from cfg.scenarios[t].seed byte across cfg.bufsize (default 0x6c) bytes; observes all dwords as hex string. cfg.scenarios, cfg.bufsize, cfg.rva. ABI asymmetry is intentional and handled by the harness.
}

SRC = r"""
rpc.exports.diff = function(cfg) {
  const LL  = new NativeFunction(Module.getGlobalExportByName('LoadLibraryW'),  'pointer', ['pointer']);
  const GPA = new NativeFunction(Module.getGlobalExportByName('GetProcAddress'),'pointer', ['pointer','pointer']);
  const hm = LL(Memory.allocUtf16String(cfg.asi));
  if (hm.isNull()) return { error: 'LoadLibraryW failed' };
  const reim = GPA(hm, Memory.allocAnsiString(cfg.export));
  if (reim.isNull()) return { error: 'GetProcAddress failed for ' + cfg.export };
  const b0 = ptr(cfg.rva).readU8();
  if (b0 === 0xE9) return { error: 'ORIGINAL PATCHED (b0=0xE9) — NO_AUTO_HOOK failed; aborting' };
  const nargs = (cfg.at === 'int2_scalar') ? ['uint32','uint32']
              : (cfg.at === 'deref_field_write') ? ['pointer','uint32']
              : (cfg.at === 'deref_table_read')  ? ['pointer','uint32']
              : (cfg.at === 'pool_insert_snapshot') ? ['pointer','uint32']
              : (cfg.at === 'pool_remove_snapshot') ? ['pointer','uint32']
              : (cfg.at === 'table_clear') ? ['uint32']
              : (cfg.at === 'ptr_fields_clear') ? ['pointer']
              : (cfg.at === 'stack_pop_snapshot') ? ['pointer']
              : (cfg.at === 'stack_push_snapshot') ? ['pointer','uint32']
              : (cfg.at === 'ptr_table_field_read') ? ['uint32']
              : (cfg.at === 'ptr_out_table_get') ? ['pointer','uint32']
              : (cfg.at === 'idx2_table_get') ? ['pointer','uint32','uint32']
              : (cfg.at === 'ptr_compute_get') ? ['pointer','uint32']
              : (cfg.at === 'table_ret_ptrout') ? ['uint32','pointer']
              : (cfg.at === 'arg_scattered_globals') ? ['uint32']
              : (cfg.at === 'vec16_copy_set') ? ['uint32','pointer']
              : (cfg.at === 'indexed_vec_set') ? ['uint32','pointer']
              : (cfg.at === 'indexed_bit_toggle') ? ['uint32','uint32']
              : (cfg.at === 'gated_int_predicate') ? ['uint32']
              : (cfg.at === 'global4_bool_out') ? ['pointer']
              : (cfg.at === 'linear_scan_find') ? ['uint32']
              : (cfg.at === 'indexed_const2_set') ? ['uint32']
              : (cfg.at === 'gated_args_to_globals') ? ['uint32','uint32','uint32','uint32','uint32','uint32']
              : (cfg.at === 'index_then_ptr_array') ? (cfg.mult ? ['uint32','uint32'] : ['uint32'])
              : (cfg.at === 'flag_multibit') ? (cfg.nargs4 ? ['uint32','uint32','uint32','uint32'] : ['uint32','uint32','uint32'])
              : (cfg.at === 'float_threshold_predicate') ? ['uint32']
              : (cfg.at === 'deref_struct_set') ? (['pointer'].concat(new Array(cfg.nscalar | 0).fill('uint32')))
              : (cfg.at === 'cond_deref_get') ? ['pointer']
              : (cfg.at === 'table_bool_predicate') ? ['uint32']
              : (cfg.at === 'global_swap') ? ['uint32']
              : (cfg.at === 'byte_args_to_globals') ? (cfg.observe.map(function(){return 'uint8';}))
              : (cfg.at === 'indexed_float_sq') ? ['uint32','float']
              : (cfg.at === 'double_deref_vec3_get') ? ['uint32','pointer']
              : (cfg.at === 'global_float_predicate') ? []
              : (cfg.at === 'double_deref_ptr_get') ? ['pointer','uint32']
              : (cfg.at === 'deref_float_field_rmw') ? ['pointer','float']
              : (cfg.at === 'any_slot_nonzero') ? []
              : (cfg.at === 'arg_table_linear_search') ? ['uint32','pointer','uint32']
              : (cfg.at === 'global_float_step') ? ['float']
              : (cfg.at === 'struct_const_init') ? (cfg.passthrough_arg ? ['uint32','pointer'] : ['pointer'].concat(new Array(cfg.nscalar | 0).fill('uint32')))
              : (cfg.at === 'idx2_table_get_outlast') ? ['uint32','uint32','pointer']
              : (cfg.at === 'copy_arg_to_globals') ? ['pointer']
              : (cfg.at === 'deref_byte_flag') ? ['pointer','uint32']
              : (cfg.at === 'indexed_masked_get_out') ? ['uint32','pointer']
              : (cfg.at === 'deref_p1field_glob_set') ? (cfg.arg2_kind === 'ptr' ? ['pointer','pointer'] : cfg.arg2_kind === 'scalar' ? ['pointer','uint32'] : cfg.arg2_kind === 'scalar2' ? ['pointer','uint32','uint32'] : ['pointer'])
              : (cfg.at === 'global_table_linear_search') ? ['uint32']
              : (cfg.at === 'global_ptr_strided_clear') ? []
              : (cfg.at === 'struct_to_out_build') ? ['pointer','pointer']
              : (cfg.at === 'store_be32') ? ['pointer','uint32']
              : (cfg.at === 'load_be32') ? ['pointer']
              : (cfg.at === 'arg_to_global_ret') ? ['uint32']
              : (cfg.at === 'indexed_global_field_read') ? []
              : (cfg.at === 'indexed_global_field_write') ? ['uint32']
              : (cfg.at === 'ptr_buffer_op') ? ['pointer']
              : (cfg.at === 'reg_scalar_compute') ? ['uint32', 'uint32']
              : (cfg.at === 'abstable_ptr_zero') ? ['uint32']
              : (cfg.at === 'idx_table_out') ? ['uint32', 'pointer']
              : (cfg.at === 'nested_struct_op') ? ['pointer']
              : (cfg.at === 'idx_src_abs_memcpy') ? ['uint32', 'pointer']
              : (cfg.at === 'dll_unlink') ? ['pointer', 'pointer']
              : (cfg.at === 'circular_dll_search') ? ['pointer', 'pointer']
              : (cfg.at === 'dll_get_nth') ? ['pointer', 'pointer', 'uint32']
              : (cfg.at === 'indexed_global_2lvl') ? []
              : (cfg.at === 'indexed_bound_array_get') ? ['uint32']
              : (cfg.at === 'abs_ranges_setter') ? ((cfg.nscalar | 0) === 1 ? ['uint32'] : (cfg.nscalar | 0) === 3 ? ['uint32','uint32','uint32'] : ['uint32','uint32'])
              : (cfg.at === 'esi_global_search') ? ['uint32']
              : (cfg.at === 'indexed_global_idiv') ? ['uint32']
              : (cfg.at === 'float_vec3_lerp_out') ? ['pointer','pointer','pointer','float']
              : (cfg.at === 'float_2ptr_ret') ? ['pointer','pointer']
              : (cfg.at === 'float_planes6_predicate') ? ['pointer','pointer']
              : (cfg.at === 'eax_edi_out') ? ['uint32','pointer']
              : (cfg.at === 'grid_getter_multiout') ? ['uint32','uint32','pointer','pointer','pointer']
              : (cfg.at === 'struct_ctor_big') ? ['pointer']
              : (cfg.at === 'indexed_abs_dualout') ? ['uint32','pointer','pointer']
              : (cfg.at === 'dll_remove_count') ? ['pointer','pointer']
              : (cfg.at === 'dll_insert_head') ? ['pointer','pointer']
              : (cfg.at === 'global_ptrtable_match') ? ['uint32','pointer']
              : (cfg.at === 'global_rec_clear_ret') ? ['uint32','uint32']
              : (cfg.at === 'abs_scan_flag') ? []
              : (cfg.at === 'global_2level_list_search') ? ['uint32']
              : (cfg.at === 'arg_flag_branch_getter') ? ['pointer']
              : (cfg.at === 'global_dll_insert_head') ? ['pointer']
              : (cfg.at === 'global_fieldoff_clear') ? ['pointer']
              : (cfg.at === 'multi_state_list_setter') ? ['pointer']
              : (cfg.at === 'byte_counter_struct') ? ['pointer']
              : (cfg.at === 'arg_default_memcpy_abs') ? ['pointer']
              : (cfg.at === 'byte_idx_table_bitclear') ? ['pointer']
              : (cfg.at === 'struct_table5_search') ? ['pointer','pointer']
              : (cfg.at === 'circular_list_search_node') ? ['pointer','uint32']
              : (cfg.at === 'global_fieldoff_set') ? ['pointer']
              : (cfg.at === 'eax_dest_memcpy_init') ? ['pointer','pointer','pointer','uint32','uint32']
              : (cfg.at === 'struct_div_mod_compute') ? ['pointer','uint32','uint32','uint32','pointer']
              : (cfg.at === 'ring_copy_5ab980') ? ['pointer']
              : (cfg.at === 'struct_init_3arg_sub') ? ['pointer','uint32','pointer']
              : (cfg.at === 'flag_branch_struct_2way') ? ['pointer','uint32']
              : (cfg.at === 'abs_region_zeroer') ? []
              : (cfg.at === 'array_fill_2way') ? ['pointer','pointer']
              : (cfg.at === 'abs_table_state_setter') ? ['uint32','uint32']
              : (cfg.at === 'esi_edx_predicate') ? ['pointer','pointer']
              : (cfg.at === 'edx_ebx_edi_find') ? ['pointer','uint32','uint32']
              : (cfg.at === 'ebx_edi_global_find') ? ['uint32','uint32']
              : (cfg.at === 'strided_color_fill') ? []
              : (cfg.at === 'bitmap_alloc_slot') ? []
              : (cfg.at === 'state_list_insert') ? ['pointer','uint32','pointer']
              : (cfg.at === 'multi_deref_global_set') ? ['pointer','pointer']
              : (cfg.at === 'list_node_const_init') ? ['pointer','pointer']
              : (cfg.at === 'bounded_struct_push') ? ['pointer','pointer','pointer']
              : (cfg.at === 'trie_walk') ? ['pointer','uint32','uint32']
              : (cfg.at === 'struct_delta_flag_init') ? ['pointer','pointer','pointer','pointer','pointer']
              : (cfg.at === 'table_accum_clamp') ? ['uint32','pointer','pointer']
              : (cfg.at === 'fastcall_float_clamp') ? ['uint32','pointer','float']
              : (cfg.at === 'list_walk_self_write') ? ['pointer','uint32']
              : (cfg.at === 'eax_ecx_float_hash') ? ['uint32','uint32']
              : (cfg.at === 'case_insensitive_ncmp') ? ['pointer','pointer','uint32']
              : (cfg.at === 'aabb_sphere_overlap') ? ['pointer','pointer']
              : (cfg.at === 'circular_str_search_ci') ? ['pointer','pointer']
              : (cfg.at === 'byte_format_hexdump') ? ['pointer','pointer','pointer']
              : (cfg.at === 'pool_freelist_init') ? ['pointer']
              : (cfg.at === 'bitmap_blit') ? ['pointer','pointer']
              : (cfg.at === 'record_array_filter_update') ? ['pointer','pointer','uint32','uint32','uint32','uint32']
              : (cfg.at === 'heap_alloc_aligned') ? ['pointer','uint32','uint32']
              : (cfg.at === 'near_leaf_abs_table') ? ['uint32']
              : (cfg.at === 'near_leaf_seed_ret') ? []
              : (cfg.at === 'near_leaf_memcmp16') ? ['uint32','pointer','pointer']
              : (cfg.at === 'near_leaf_arr_to_table') ? ['pointer']
              : (cfg.at === 'near_leaf_dot_plane') ? ['pointer','pointer']
              : (cfg.at === 'near_leaf_seed_globals') ? []
              : (cfg.at === 'near_leaf_seed_arg_obs') ? ['uint32']
              : (cfg.at === 'near_leaf_ptr_array_search') ? ['uint32','uint32']
              : (cfg.at === 'near_leaf_seed_multi_obs') ? []
              : (cfg.at === 'near_leaf_record_builder') ? ['uint32','uint32']
              : (cfg.at === 'near_leaf_accum_table') ? ['uint32','float','uint32']
              : (cfg.at === 'near_leaf_struct_array_predicate') ? []
              : (cfg.at === 'near_leaf_global_str_search') ? ['pointer']
              : (cfg.at === 'near_leaf_list_search') ? ['uint32']
              : (cfg.at === 'near_leaf_memset2') ? ['pointer','uint32']
              : (cfg.at === 'struct_list_float_set') ? ['pointer','float']
              : (cfg.at === 'seed_indirect_ctx_obs') ? []
              : (cfg.at === 'indexed_float_sum2') ? ['uint32']
              : (cfg.at === 'double_indexed_float_mul') ? ['uint32']
              : (cfg.at === 'struct_tag_equals') ? ['pointer','pointer']
              : (cfg.at === 'indexed_float_accum16') ? ['pointer','uint32','uint32']
              : (cfg.at === 'bounded_table_signselect_clamp') ? ['uint32','uint32']
              : (cfg.at === 'seed_globals_arg_multiobs') ? ['uint32']
              : (cfg.at === 'succ_approx_quantize') ? ['uint32','pointer','pointer']
              : (cfg.at === 'multi_array_scatter') ? ['pointer']
              : (cfg.at === 'dll_head_insert') ? ['pointer']
              : (cfg.at === 'find_node_struct_copy') ? ['pointer','pointer']
              : (cfg.at === 'nested_list_search') ? ['uint32']
              : (cfg.at === 'pixel_max_alpha') ? ['pointer']
              : (cfg.at === 'engine_register_funcs') ? []
              : (cfg.at === 'particle_pool_alloc') ? ['pointer','uint32']
              : (cfg.at === 'thunk_node_write') ? ['pointer','uint32','uint32']
              : (cfg.at === 'thunk_field_copy') ? ['pointer','pointer']
              : (cfg.at === 'thunk_cond_or') ? ['pointer','uint32','uint32']
              : (cfg.at === 'thunk_list_count') ? ['pointer']
              : (cfg.at === 'thunk_float_sub') ? ['uint32','float']
              : (cfg.at === 'bounded_thunk_orflag') ? ['uint32','uint32']
              : (cfg.at === 'bitfield_range_set') ? ['pointer','uint32','uint32','uint32']
              : (cfg.at === 'esi_struct_init') ? ['pointer']
              : (cfg.at === 'near_leaf_seed_outbuf') ? (cfg.argkinds || ['pointer'])
              : (cfg.at === 'idx2_record_condset') ? ['uint32','uint32','uint32']
              : (cfg.at === 'quad_buffer_build') ? ['pointer','uint32','pointer']
              : (cfg.at === 'container_record_set') ? (cfg.shape === 'pp' ? ['pointer','pointer','pointer'] : cfg.shape === 'f' ? ['pointer','float'] : ['pointer','pointer'])
              : (cfg.at === 'eq_predicate_get') ? ['uint32','uint32']
              : (cfg.at === 'cond_table_get') ? ['uint32']
              : (cfg.at === 'cond_global_set') ? ['uint32']
              : (cfg.at === 'indexed_table_set') ? ['uint32','uint32']
              : (cfg.at === 'thiscall_struct_from_table') ? ['pointer']
              : (cfg.at === 'void_setter_observe' || cfg.at === 'int_scalar' || cfg.at === 'float_table_read') ? ['uint32'] : [];
  const _keep = [];
  // Per-side calling convention. The original may be __thiscall (this in ECX);
  // the reimpl is exported as plain __cdecl(void* self) (clean undecorated name,
  // no __fastcall @name@N mangling). The diff compares the OBSERVABLE effect
  // (struct fields / globals written), not the call mechanism, so mixed
  // conventions are sound. Defaults preserve the all-mscdecl behaviour.
  const Orig = new NativeFunction(ptr(cfg.rva), cfg.ret, nargs, cfg.conv_orig || 'mscdecl');
  const Reim = new NativeFunction(reim,         cfg.ret, nargs, cfg.conv_reim || 'mscdecl');
  const norm = function (v) { return (cfg.ret === 'float') ? v : (v >>> 0); };
  const res = [];
  for (let i = 0; i < cfg.tests.length; i++) {
    const t = cfg.tests[i]; let o = null, r = null, eo = null, er = null;
    if (cfg.at === 'read_global') {
      try { ptr(cfg.tgt).writeU32(t >>> 0); o = norm(Orig()); } catch (e) { eo = e.message; }
      try { ptr(cfg.tgt).writeU32(t >>> 0); r = norm(Reim()); } catch (e) { er = e.message; }
    } else if (cfg.at === 'scalars_to_scattered_globals') {
      // Observe ALL written globals: fill each with a sentinel, call, read each back,
      // join into one comparison string (covers multi-store setters fully).
      const obs = cfg.observe;
      const readAll = function () { return obs.map(function (x) { return ptr(x.addr).readU32() >>> 0; }).join('|'); };
      const fillAll = function () { obs.forEach(function (x) { ptr(x.addr).writeU32(0xFFFFFFFF); }); };
      try { fillAll(); Orig(); o = readAll(); } catch (e) { eo = e.message; }
      try { fillAll(); Reim(); r = readAll(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'void_setter_observe') {
      try { Orig(t >>> 0); o = ptr(cfg.tgt).readU32() >>> 0; } catch (e) { eo = e.message; }
      try { Reim(t >>> 0); r = ptr(cfg.tgt).readU32() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'int_scalar') {
      // Optional table-seed: write distinct values into an absolute table the
      // function indexes, so a state-zero table is diffed NON-degenerately and a
      // wrong stride/base in the reimpl is caught. Seed once on the first test.
      if (cfg.seed_table && i === 0) {
        const base = ptr(cfg.seed_table.base), st = cfg.seed_table.stride | 0;
        const span = cfg.seed_table.span | 0;
        for (let k = 0; k < span; k++) base.add(k * st).writeU32((0xC0DE0000 | k) >>> 0);
      }
      try { o = Orig(t >>> 0) >>> 0; } catch (e) { eo = e.message; }
      try { r = Reim(t >>> 0) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'int2_scalar') {
      try { o = Orig(t[0] >>> 0, t[1] >>> 0) >>> 0; } catch (e) { eo = e.message; }
      try { r = Reim(t[0] >>> 0, t[1] >>> 0) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'deref_field_write') {
      // *(*(p1+outer_off)+inner_off) = p2. Fresh A+inner buffers per side; check inner[inner_off].
      const oo = cfg.outer_off | 0, io = cfg.inner_off | 0;
      const A1 = Memory.alloc(0x80), I1 = Memory.alloc(0x80); _keep.push(A1, I1);
      for (let z = 0; z < 0x80; z += 4) { A1.add(z).writeU32(0); I1.add(z).writeU32(0); }
      A1.add(oo).writePointer(I1);
      try { Orig(A1, t >>> 0); o = I1.add(io).readU32() >>> 0; } catch (e) { eo = e.message; }
      const A2 = Memory.alloc(0x80), I2 = Memory.alloc(0x80); _keep.push(A2, I2);
      for (let z = 0; z < 0x80; z += 4) { A2.add(z).writeU32(0); I2.add(z).writeU32(0); }
      A2.add(oo).writePointer(I2);
      try { Reim(A2, t >>> 0); r = I2.add(io).readU32() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'const_return') {
      try { o = Orig() >>> 0; } catch (e) { eo = e.message; }
      try { r = Reim() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'pool_insert_snapshot') {
      // Pool/list insert into a manager. Same buffers for both sides so absolute
      // link pointers are directly comparable; reset to a fresh state, call with
      // the test key, snapshot manager+slots+pool, compare. Non-degenerate: the
      // inserted node stores `key` (distinct per test).
      const N = (cfg.capacity | 0) || 4;
      const mgr = Memory.alloc(0x18), slots = Memory.alloc(N * 4), pool = Memory.alloc(N * 0x10);
      _keep.push(mgr, slots, pool);
      const reset = function () {
        for (let z = 0; z < 0x18; z += 4) mgr.add(z).writeU32(0);
        mgr.writeU16(0); mgr.add(2).writeU16(N);
        mgr.add(0xc).writePointer(pool); mgr.add(0x10).writePointer(slots);
        for (let z = 0; z < N * 4; z += 4) slots.add(z).writeU32(0);
        for (let z = 0; z < N * 0x10; z += 4) pool.add(z).writeU32(0);
      };
      const snap = function () {
        const p = [];
        for (let z = 0; z < 0x18; z += 4) p.push(mgr.add(z).readU32() >>> 0);
        for (let z = 0; z < N * 4; z += 4) p.push(slots.add(z).readU32() >>> 0);
        for (let z = 0; z < N * 0x10; z += 4) p.push(pool.add(z).readU32() >>> 0);
        return p.join(',');
      };
      try { reset(); Orig(mgr, t >>> 0); o = snap(); } catch (e) { eo = e.message; }
      try { reset(); Reim(mgr, t >>> 0); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'pool_remove_snapshot') {
      // Build a list with the ORIGINAL insert (cfg.insert_rva), then remove the
      // test key; snapshot full state + return value. Same buffers both sides.
      const N = (cfg.capacity | 0) || 4;
      const mgr = Memory.alloc(0x18), slots = Memory.alloc(N * 4), pool = Memory.alloc(N * 0x10);
      _keep.push(mgr, slots, pool);
      const Insert = new NativeFunction(ptr(cfg.insert_rva), 'uint16', ['pointer', 'uint32'], 'mscdecl');
      const bks = cfg.build_keys;
      const build = function () {
        for (let z = 0; z < 0x18; z += 4) mgr.add(z).writeU32(0);
        mgr.writeU16(0); mgr.add(2).writeU16(N);
        mgr.add(0xc).writePointer(pool); mgr.add(0x10).writePointer(slots);
        for (let z = 0; z < N * 4; z += 4) slots.add(z).writeU32(0);
        for (let z = 0; z < N * 0x10; z += 4) pool.add(z).writeU32(0);
        for (let k = 0; k < bks.length; k++) Insert(mgr, bks[k] >>> 0);
      };
      const snap = function () {
        const p = [];
        for (let z = 0; z < 0x18; z += 4) p.push(mgr.add(z).readU32() >>> 0);
        for (let z = 0; z < N * 4; z += 4) p.push(slots.add(z).readU32() >>> 0);
        for (let z = 0; z < N * 0x10; z += 4) p.push(pool.add(z).readU32() >>> 0);
        return p.join(',');
      };
      try { build(); const ro = Orig(mgr, t >>> 0) >>> 0; o = snap() + '|ret=' + ro; } catch (e) { eo = e.message; }
      try { build(); const rr = Reim(mgr, t >>> 0) >>> 0; r = snap() + '|ret=' + rr; } catch (e) { er = e.message; }
    } else if (cfg.at === 'table_clear') {
      // void fn(i): zero [tgt + i*4]. Seed sentinel, call, read back.
      const base = ptr(cfg.tgt), idx = t >>> 0;
      try { base.add(idx * 4).writeU32(0xFFFFFFFF); Orig(idx); o = base.add(idx * 4).readU32() >>> 0; } catch (e) { eo = e.message; }
      try { base.add(idx * 4).writeU32(0xFFFFFFFF); Reim(idx); r = base.add(idx * 4).readU32() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'ptr_fields_clear') {
      // void fn(ptr): zero struct fields. Fill buffer with sentinel, call, check observe offsets.
      const obs = cfg.observe;
      const buf = Memory.alloc(0x100); _keep.push(buf);
      const fill = function () { for (let z = 0; z < 0x100; z += 4) buf.add(z).writeU32(0xFFFFFFFF); };
      const rd = function () { return obs.map(function (x) { return buf.add(x.off | 0).readU32() >>> 0; }).join('|'); };
      try { fill(); Orig(buf); o = rd(); } catch (e) { eo = e.message; }
      try { fill(); Reim(buf); r = rd(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'ptr_out_table_get') {
      // u32 fn(out_ptr, idx): if(idx>=bound) return 0; out[0..n-1]=*(u32*)(base+idx*stride+j*4); return 1.
      // Seed the absolute table slots for this idx (distinct -> non-degenerate); out buffers fresh per
      // side. In-range idx writes n dwords + returns 1; out-of-range writes nothing + returns 0.
      const base = cfg.tgt, stride = cfg.stride | 0, n = (cfg.span | 0) || 1, bound = cfg.bound | 0;
      const idx = t >>> 0;
      // cfg.reseed_per_side (default false): re-seed the table slots before EACH side
      // instead of once per test. REQUIRED when the function itself WRITES the block it
      // then copies out - e.g. 0x0046d510 runs a matrix transform into 0x881f74 at
      // 0x0046d53a and copies the result from there. Seeded once, the original's transform
      // leaves its result in place, so a port that SKIPPED the transform and merely copied
      // the block would read that result and compare equal: a false GREEN one level down
      // from the out3_idx one this row was just demoted for. Defaulting to false keeps every
      // existing caller byte-identical (they are pure reads and mutate nothing).
      const seedTbl = function () {
        if (idx >= bound) return;
        for (let j = 0; j < n; j++)
          ptr(base).add(idx * stride + j * 4).writeU32((0xC0DE0000 | ((idx << 4) | j)) >>> 0);
      };
      seedTbl();
      const outO = Memory.alloc(0x40), outR = Memory.alloc(0x40); _keep.push(outO, outR);
      const rd = function (b) { const p = []; for (let j = 0; j < n; j++) p.push(b.add(j * 4).readU32() >>> 0); return p.join(','); };
      try { for (let j = 0; j < 16; j++) outO.add(j * 4).writeU32(0); const ro = Orig(outO, idx) >>> 0; o = rd(outO) + '|ret=' + ro; } catch (e) { eo = e.message; }
      if (cfg.reseed_per_side) seedTbl();
      try { for (let j = 0; j < 16; j++) outR.add(j * 4).writeU32(0); const rr = Reim(outR, idx) >>> 0; r = rd(outR) + '|ret=' + rr; } catch (e) { er = e.message; }
    } else if (cfg.at === 'idx2_table_get') {
      // u32 fn(out_ptr, i1, i2): if(i1>=bound || i2>=bound2) return 0; *out=*(u32*)(base+(i1*mult+i2)*stride); return 1.
      // test t=[i1,i2]. seed the composite slot (distinct -> non-degenerate); fresh out per side.
      const base = cfg.tgt, mult = cfg.mult | 0, stride = cfg.stride | 0, b1 = cfg.bound | 0, b2 = cfg.bound2 | 0;
      const i1 = t[0] >>> 0, i2 = t[1] >>> 0;
      if (i1 < b1 && i2 < b2) {
        ptr(base).add((i1 * mult + i2) * stride).writeU32((0xC0DE0000 | ((i1 << 8) | i2)) >>> 0);
      }
      const outO = Memory.alloc(0x10), outR = Memory.alloc(0x10); _keep.push(outO, outR);
      try { outO.writeU32(0); const ro = Orig(outO, i1, i2) >>> 0; o = (outO.readU32() >>> 0) + '|ret=' + ro; } catch (e) { eo = e.message; }
      try { outR.writeU32(0); const rr = Reim(outR, i1, i2) >>> 0; r = (outR.readU32() >>> 0) + '|ret=' + rr; } catch (e) { er = e.message; }
    } else if (cfg.at === 'float_threshold_predicate') {
      // u32 fn(idx): return (*(float*)(base+idx*stride) < *(float*)gate) ? 1 : 0.
      // gate (threshold) is READ-ONLY .rdata -> seed ONLY the record float; use records straddling the
      // real fixed threshold so the result varies (non-degenerate). test t=[idx, recordbits].
      const base = cfg.tgt, stride = cfg.stride | 0, idx = t[0] >>> 0, recb = t[1] >>> 0;
      const seed = function () { ptr(base).add(idx * stride).writeU32(recb); };
      try { seed(); o = Orig(idx) >>> 0; } catch (e) { eo = e.message; }
      try { seed(); r = Reim(idx) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'double_deref_ptr_get') {
      // void fn(out*, idx): rec=*(u32*)(tgt+idx*stride); *out = *(u32*)(rec+rec_off) + add.
      const base = cfg.tgt, stride = cfg.stride | 0, ro = cfg.rec_off | 0, add = cfg.add | 0, idx = t >>> 0;
      const buf = Memory.alloc(0x40); _keep.push(buf); buf.add(ro).writeU32((0xC0DE0000 | idx) >>> 0);
      ptr(base).add(idx * stride).writePointer(buf);
      const outO = Memory.alloc(4), outR = Memory.alloc(4); _keep.push(outO, outR);
      try { outO.writeU32(0); Orig(outO, idx); o = outO.readU32() >>> 0; } catch (e) { eo = e.message; }
      try { outR.writeU32(0); Reim(outR, idx); r = outR.readU32() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'deref_float_field_rmw') {
      // void fn(ptr p, float f): *(float*)(p+field_off) {-=|+=} f. seed field=seedf, call, read field.
      const off = cfg.field_off | 0, seedf = cfg.seedf, f = t;
      const buf = Memory.alloc(0x80); _keep.push(buf);
      const seed = function () { buf.add(off).writeFloat(seedf); };
      try { seed(); Orig(buf, f); o = buf.add(off).readFloat(); } catch (e) { eo = e.message; }
      try { seed(); Reim(buf, f); r = buf.add(off).readFloat(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'double_deref_vec3_get') {
      // void fn(i, out*): rec=*(u32*)(tgt+i*stride); tp=*(u32*)(rec+rec_off); out[k]=*(u32*)(tp+out_off+k*4).
      // seed table[i]->buf1, buf1[rec_off]->buf2, buf2[out_off+k*4]=distinct; fresh out per side.
      const base = cfg.tgt, stride = cfg.stride | 0, ro = cfg.rec_off | 0, oo = cfg.out_off | 0, n = (cfg.span | 0) || 3, idx = t >>> 0;
      const buf1 = Memory.alloc(0x100), buf2 = Memory.alloc(0x100); _keep.push(buf1, buf2);
      for (let z = 0; z < 0x100; z += 4) { buf1.add(z).writeU32(0); buf2.add(z).writeU32(0); }
      buf1.add(ro).writePointer(buf2);
      for (let k = 0; k < n; k++) buf2.add(oo + k * 4).writeU32((0xC0DE0000 | ((idx << 4) | k)) >>> 0);
      ptr(base).add(idx * stride).writePointer(buf1);
      // optional 2nd parallel table drives a bool return (e.g. table2[idx*ret_stride]==0)
      if (cfg.ret_tbl) ptr(cfg.ret_tbl).add(idx * (cfg.ret_stride | 0)).writeU32((idx & 1) ? 0 : 0xC0DE);
      const outO = Memory.alloc(0x40), outR = Memory.alloc(0x40); _keep.push(outO, outR);
      const rd = function (b) { const p = []; for (let k = 0; k < n; k++) p.push(b.add(k * 4).readU32() >>> 0); return p.join(','); };
      try { for (let k = 0; k < n; k++) outO.add(k * 4).writeU32(0); const ro = Orig(idx, outO); o = rd(outO) + (cfg.ret_tbl ? ('|ret=' + (ro >>> 0)) : ''); } catch (e) { eo = e.message; }
      try { for (let k = 0; k < n; k++) outR.add(k * 4).writeU32(0); const rr = Reim(idx, outR); r = rd(outR) + (cfg.ret_tbl ? ('|ret=' + (rr >>> 0)) : ''); } catch (e) { er = e.message; }
    } else if (cfg.at === 'arg_to_global_ret') {
      // u32 fn(v): reimpl writes *tgt and returns (deterministic fn of v). seed tgt sentinel, call, check tgt+ret.
      const g = ptr(cfg.tgt), v = t >>> 0;
      try { g.writeU32(0xEEEEEEEE); const ro = Orig(v) >>> 0; o = (g.readU32() >>> 0) + '|ret=' + ro; } catch (e) { eo = e.message; }
      try { g.writeU32(0xEEEEEEEE); const rr = Reim(v) >>> 0; r = (g.readU32() >>> 0) + '|ret=' + rr; } catch (e) { er = e.message; }
    } else if (cfg.at === 'store_be32') {
      // void fn(ptr p, u32 v): p[0..3] = big-endian bytes of v. test = v.
      const buf = Memory.alloc(0x20); _keep.push(buf);
      const rd = function () { return [buf.readU8(), buf.add(1).readU8(), buf.add(2).readU8(), buf.add(3).readU8()].join(','); };
      try { buf.writeU32(0xEEEEEEEE); Orig(buf, t >>> 0); o = rd(); } catch (e) { eo = e.message; }
      try { buf.writeU32(0xEEEEEEEE); Reim(buf, t >>> 0); r = rd(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'load_be32') {
      // int fn(ptr p): return big-endian u32 from p[0..3]. test = dword written to p.
      const buf = Memory.alloc(0x20); _keep.push(buf); buf.writeU32(t >>> 0);
      try { o = Orig(buf) >>> 0; } catch (e) { eo = e.message; }
      try { buf.writeU32(t >>> 0); r = Reim(buf) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'struct_to_out_build') {
      // void fn(out*, p2): reads p2 fields (seeded per cfg.seed [{off,bits}]), writes out[0..span-1].
      const span = (cfg.span | 0) || 16, seed = cfg.seed || [];
      const p2 = Memory.alloc(0x100), outO = Memory.alloc(0x80), outR = Memory.alloc(0x80); _keep.push(p2, outO, outR);
      const setup = function () { for (let z = 0; z < 0x100; z += 4) p2.add(z).writeU32(0); seed.forEach(function (s) { p2.add(s.off | 0).writeU32(s.bits >>> 0); }); };
      const rd = function (b) { const p = []; for (let k = 0; k < span; k++) p.push(b.add(k * 4).readU32() >>> 0); return p.join(','); };
      try { setup(); for (let k = 0; k < span; k++) outO.add(k * 4).writeU32(0xEEEEEEEE); Orig(outO, p2); o = rd(outO); } catch (e) { eo = e.message; }
      try { setup(); for (let k = 0; k < span; k++) outR.add(k * 4).writeU32(0xEEEEEEEE); Reim(outR, p2); r = rd(outR); } catch (e) { er = e.message; }
    } else if (cfg.at === 'global_table_linear_search') {
      // int fn(key): if(key<=0) return -1; for i<count: if(*(int*)(tgt+i*stride)==key) return i; return -1.
      const base = cfg.tgt, stride = cfg.stride | 0, count = cfg.count | 0, key = t[0] | 0, placeAt = t[1] | 0;
      for (let i = 0; i < count; i++) ptr(base).add(i * stride).writeU32((0x7F000000 | i) >>> 0);
      if (placeAt >= 0 && placeAt < count) ptr(base).add(placeAt * stride).writeU32(key >>> 0);
      try { o = '' + (Orig(key) | 0); } catch (e) { eo = e.message; }
      try { r = '' + (Reim(key) | 0); } catch (e) { er = e.message; }
    } else if (cfg.at === 'global_ptr_strided_clear') {
      // void fn(): for i in [0,len) step stride: *(u32*)(*(u32*)glob + i)=0. seed *glob=&buf, snapshot strided.
      const len = cfg.len | 0, stride = cfg.stride | 0;
      const buf = Memory.alloc(len + 0x20); _keep.push(buf);
      const setup = function () { ptr(cfg.glob).writePointer(buf); for (let z = 0; z < len; z += 4) buf.add(z).writeU32(0xEEEEEEEE); };
      const snap = function () { const p = []; for (let i = 0; i < len; i += stride) p.push(buf.add(i).readU32() >>> 0); return p.join(','); };
      try { setup(); Orig(); o = snap(); } catch (e) { eo = e.message; }
      try { setup(); Reim(); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'deref_p1field_glob_set') {
      // fn(p1[, p2/v]): base = *(u32*)(*(u32*)(p1+p1_off) + *(u32*)glob). seed glob=0 so
      // base = (*(p1+p1_off))[0]: p1[p1_off]->atab, atab[0]->basebuf. reimpl writes basebuf fields.
      const obs = cfg.observe, p1off = cfg.p1_off | 0, kind = cfg.arg2_kind, n = cfg.arg2_dwords | 0;
      ptr(cfg.glob).writeU32(0);
      const base = Memory.alloc(0x400), atab = Memory.alloc(0x40), p1 = Memory.alloc(0x40);
      _keep.push(base, atab, p1);
      let p2 = null; if (kind === 'ptr') { p2 = Memory.alloc(n * 4 + 0x10); _keep.push(p2); }
      const setup = function () {
        for (let z = 0; z < 0x400; z += 4) base.add(z).writeU32(0xEEEEEEEE);
        atab.writePointer(base); p1.add(p1off).writePointer(atab);
        if (kind === 'ptr') for (let k = 0; k < n; k++) p2.add(k * 4).writeU32((0xC0DE0000 | k) >>> 0);
      };
      const snap = function () { return obs.map(function (x) { return base.add(x.off | 0).readU32() >>> 0; }).join(','); };
      const call = function (fn) { if (kind === 'ptr') fn(p1, p2); else if (kind === 'scalar') fn(p1, 0xC0DE0001); else if (kind === 'scalar2') fn(p1, 0xC0DE0001, 0xC0DE0002); else fn(p1); };
      try { setup(); call(Orig); o = snap(); } catch (e) { eo = e.message; }
      try { setup(); call(Reim); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'copy_arg_to_globals') {
      // void fn(ptr p): copy len(observe) dwords from p to observe[k].addr globals.
      const obs = cfg.observe, n = obs.length;
      const buf = Memory.alloc(n * 4 + 0x20); _keep.push(buf);
      for (let k = 0; k < n; k++) buf.add(k * 4).writeU32((0xC0DE0000 | k) >>> 0);
      const fill = function () { obs.forEach(function (x) { ptr(x.addr).writeU32(0xEEEEEEEE); }); };
      const rd = function () { return obs.map(function (x) { return ptr(x.addr).readU32() >>> 0; }).join(','); };
      try { fill(); Orig(buf); o = rd(); } catch (e) { eo = e.message; }
      try { fill(); Reim(buf); r = rd(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'deref_byte_flag') {
      // void fn(ptr p, set): b=*(u8*)(p+field_off); set?b|=bit:b&=~bit; store. test=[set,seedbyte].
      const off = cfg.field_off | 0, bit = cfg.bit | 0, set = t[0] >>> 0, seed = t[1] & 0xff;
      const buf = Memory.alloc(0x40); _keep.push(buf);
      try { buf.add(off).writeU8(seed); Orig(buf, set); o = buf.add(off).readU8(); } catch (e) { eo = e.message; }
      try { buf.add(off).writeU8(seed); Reim(buf, set); r = buf.add(off).readU8(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'indexed_masked_get_out') {
      // void fn(i, out*): if(out) *out = *(u32*)(tgt+i*stride) & mask. seed slot, call, read out.
      const base = cfg.tgt, stride = cfg.stride | 0, mask = cfg.mask >>> 0, idx = t[0] >>> 0, sv = t[1] >>> 0;
      ptr(base).add(idx * stride).writeU32(sv);
      const outO = Memory.alloc(4), outR = Memory.alloc(4); _keep.push(outO, outR);
      try { outO.writeU32(0); Orig(idx, outO); o = outO.readU32() >>> 0; } catch (e) { eo = e.message; }
      try { outR.writeU32(0); Reim(idx, outR); r = outR.readU32() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'idx2_table_get_outlast') {
      // u32 fn(i1, i2, out*): if(i1>=bound||i2>=bound2) return 0; *out=*(u32*)(tgt+(i1*mult+i2)*stride); return 1.
      const base = cfg.tgt, mult = cfg.mult | 0, stride = cfg.stride | 0, b1 = cfg.bound | 0, b2 = cfg.bound2 | 0;
      const i1 = t[0] >>> 0, i2 = t[1] >>> 0;
      if (i1 < b1 && i2 < b2) ptr(base).add((i1 * mult + i2) * stride).writeU32((0xC0DE0000 | ((i1 << 8) | i2)) >>> 0);
      const outO = Memory.alloc(0x10), outR = Memory.alloc(0x10); _keep.push(outO, outR);
      try { outO.writeU32(0); const ro = Orig(i1, i2, outO) >>> 0; o = (outO.readU32() >>> 0) + '|ret=' + ro; } catch (e) { eo = e.message; }
      try { outR.writeU32(0); const rr = Reim(i1, i2, outR) >>> 0; r = (outR.readU32() >>> 0) + '|ret=' + rr; } catch (e) { er = e.message; }
    } else if (cfg.at === 'struct_const_init') {
      // [u32] fn([passthrough,] ptr p): writes deterministic values into p's fields. alloc 0x400
      // sentinel buffer, call (with optional leading passthrough arg), snapshot observe offsets +
      // (if passthrough) the return. Same init both sides -> orig/reim outputs compared directly.
      const obs = cfg.observe, hasarg = cfg.passthrough_arg, ns = cfg.nscalar | 0;
      const mk = function () { const b = Memory.alloc(0x400); _keep.push(b);
                               for (let z = 0; z < 0x400; z += 4) b.add(z).writeU32(0xFFFFFFFF); return b; };
      const snap = function (b) { return obs.map(function (x) { return b.add(x.off | 0).readU32() >>> 0; }).join(','); };
      const call = function (fn, b) {  // p first, then nscalar trailing scalars
        if (hasarg) return fn(0x12345678, b);
        if (ns === 1) return fn(b, 0xC0DE0001); if (ns === 2) return fn(b, 0xC0DE0001, 0xC0DE0002);
        if (ns === 3) return fn(b, 0xC0DE0001, 0xC0DE0002, 0xC0DE0003); return fn(b);
      };
      try { const b = mk(); const ro = call(Orig, b); o = snap(b) + (hasarg ? ('|ret=' + (ro >>> 0)) : ''); } catch (e) { eo = e.message; }
      try { const b = mk(); const rr = call(Reim, b); r = snap(b) + (hasarg ? ('|ret=' + (rr >>> 0)) : ''); } catch (e) { er = e.message; }
    } else if (cfg.at === 'arg_table_linear_search') {
      // int fn(key, table*, count): for i<count: if(table[i*stride_dw]==key) return i; return -1.
      // test t=[key, placeAt, count]: alloc table, fill distinct non-key markers, place key at placeAt.
      const sdw = cfg.stride_dw | 0, key = t[0] >>> 0, placeAt = t[1] | 0, count = t[2] | 0;
      const tbl = Memory.alloc(count * sdw * 4 + 0x40); _keep.push(tbl);
      for (let i = 0; i < count; i++) tbl.add(i * sdw * 4).writeU32((0x7F000000 | i) >>> 0);
      if (placeAt >= 0 && placeAt < count) tbl.add(placeAt * sdw * 4).writeU32(key);
      try { o = '' + (Orig(key, tbl, count) | 0); } catch (e) { eo = e.message; }
      try { r = '' + (Reim(key, tbl, count) | 0); } catch (e) { er = e.message; }
    } else if (cfg.at === 'global_float_step') {
      // void fn(float target): if(*tgt<target) *tgt+=step; if(target<*tgt) *tgt-=step. seed *tgt, call, read.
      const g = ptr(cfg.tgt), seedbits = t[0] >>> 0, target = t[1];
      try { g.writeU32(seedbits); Orig(target); o = g.readFloat(); } catch (e) { eo = e.message; }
      try { g.writeU32(seedbits); Reim(target); r = g.readFloat(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'any_slot_nonzero') {
      // u32 fn(): return any(observe[k].addr nonzero)?1:0. test = index to set nonzero (-1 = all zero).
      const obs = cfg.observe, set = t | 0;
      const seed = function () { obs.forEach(function (x) { ptr(x.addr).writeU32(0); }); if (set >= 0) ptr(obs[set].addr).writeU32(0xC0DE); };
      try { seed(); o = Orig() >>> 0; } catch (e) { eo = e.message; }
      try { seed(); r = Reim() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'global_float_predicate') {
      // u32 fn(): if(*(int*)gate==0) return 0; return (*(float*)thr <= *(float*)(*(u32*)tgt + rec_off))?1:0.
      // test t=[gateval,thrbits,valbits]: seed gate(int), thr(float bits), tgt->buf with [rec_off]=val.
      const gatev = t[0] >>> 0, thrb = t[1] >>> 0, valb = t[2] >>> 0, ro = cfg.rec_off | 0;
      const buf = Memory.alloc(0x40); _keep.push(buf); buf.add(ro).writeU32(valb);
      const seed = function () { ptr(cfg.gate).writeU32(gatev); ptr(cfg.thr).writeU32(thrb); ptr(cfg.tgt).writePointer(buf); };
      try { seed(); o = Orig() >>> 0; } catch (e) { eo = e.message; }
      try { seed(); r = Reim() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'global_swap') {
      // u32 fn(v): old=*tgt; *tgt=v; return old. test t=[oldseed,v]: seed, call, check ret + *tgt.
      const g = ptr(cfg.tgt), oldseed = t[0] >>> 0, v = t[1] >>> 0;
      try { g.writeU32(oldseed); o = (Orig(v) >>> 0) + '|g=' + (g.readU32() >>> 0); } catch (e) { eo = e.message; }
      try { g.writeU32(oldseed); r = (Reim(v) >>> 0) + '|g=' + (g.readU32() >>> 0); } catch (e) { er = e.message; }
    } else if (cfg.at === 'byte_args_to_globals') {
      // void fn(u8...): write byte args to byte globals (observe addrs). fill sentinel, call, read back.
      const obs = cfg.observe, args = (Array.isArray(t) ? t : [t]).map(function (x) { return x & 0xff; });
      const fill = function () { obs.forEach(function (x) { ptr(x.addr).writeU8(0xEE); }); };
      const rd = function () { return obs.map(function (x) { return ptr(x.addr).readU8(); }).join(','); };
      const call = function (fn) { if (args.length === 3) fn(args[0], args[1], args[2]); else if (args.length === 2) fn(args[0], args[1]); else fn(args[0]); };
      try { fill(); call(Orig); o = rd(); } catch (e) { eo = e.message; }
      try { fill(); call(Reim); r = rd(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'indexed_float_sq') {
      // void fn(i, float f): *(float*)(tgt+i*stride) = f*f. call, read slot as float, compare.
      const base = cfg.tgt, stride = cfg.stride | 0, idx = t[0] >>> 0, f = t[1];
      const slot = ptr(base).add(idx * stride);
      try { slot.writeU32(0xFFFFFFFF); Orig(idx, f); o = slot.readFloat(); } catch (e) { eo = e.message; }
      try { slot.writeU32(0xFFFFFFFF); Reim(idx, f); r = slot.readFloat(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'cond_deref_get') {
      // u32 fn(ptr p): if(*(u32*)(p+gate_off)) return *(u32*)(p+val_off); else 0.
      // test t=[gateval,val]: seed both fields, call, compare return. gate=0 -> 0; gate!=0 -> val.
      const go = cfg.gate_off | 0, vo = cfg.val_off | 0, gate = t[0] >>> 0, val = t[1] >>> 0;
      const mk = function () { const b = Memory.alloc(0x40); _keep.push(b);
                               for (let z = 0; z < 0x40; z += 4) b.add(z).writeU32(0);
                               b.add(go).writeU32(gate); b.add(vo).writeU32(val); return b; };
      try { o = Orig(mk()) >>> 0; } catch (e) { eo = e.message; }
      try { r = Reim(mk()) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'table_bool_predicate') {
      // u32 fn(i): if(bound>=0 && (int)i<=bound) return 0; return (*(u32*)(tgt+i*stride+off0) {==|!=} 0)?1:0.
      // test t=[idx,slotval]: seed the slot, call, compare. Vary idx (in/out of bound) + slotval (0/nonzero).
      const base = cfg.tgt, stride = cfg.stride | 0, off = cfg.off0 | 0, idx = t[0] >>> 0, sv = t[1] >>> 0;
      ptr(base).add(idx * stride + off).writeU32(sv);
      try { o = Orig(idx) >>> 0; } catch (e) { eo = e.message; }
      try { r = Reim(idx) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'deref_struct_set') {
      // void fn(ptr p, scalar...): writes deterministic values into fields of p. Alloc a
      // 0x400 buffer, seed every byte (cfg.seed_byte — non-zero exercises RMW-OR paths),
      // pass as p with nscalar uint32 args, snapshot the observe offsets. Same init both
      // sides; absolute addresses are not stored so the snapshots are directly comparable.
      const obs = cfg.observe, ns = cfg.nscalar | 0, seed = (cfg.seed_byte | 0) & 0xff;
      const A = Array.isArray(t) ? t : [t];
      const a0 = (A[0] || 0) >>> 0, a1 = (A[1] || 0) >>> 0, a2 = (A[2] || 0) >>> 0;
      const mk = function () { const b = Memory.alloc(0x400); _keep.push(b);
                               for (let z = 0; z < 0x400; z++) b.add(z).writeU8(seed); return b; };
      // Optional abs_observe: absolute globals the fn writes (e.g. abs tables indexed
      // by the scalar args). Reset to sentinel before each call, snapshot after.
      const absO = cfg.abs_observe || [];
      const snap = function (b) {
        const f = obs.map(function (x) { return b.add(x.off | 0).readU32() >>> 0; });
        const g = absO.map(function (a) { return ptr(a).readU32() >>> 0; });
        return f.join(',') + (absO.length ? ' G[' + g.join(',') + ']' : '');
      };
      const resetAbs = function () { absO.forEach(function (a) { ptr(a).writeU32(0x5e5e5e5e); }); };
      const call = function (fn, b) { if (ns === 1) fn(b, a0); else if (ns === 2) fn(b, a0, a1);
                                      else if (ns === 3) fn(b, a0, a1, a2); else fn(b); };
      try { const b = mk(); resetAbs(); call(Orig, b); o = snap(b); } catch (e) { eo = e.message; }
      try { const b = mk(); resetAbs(); call(Reim, b); r = snap(b); } catch (e) { er = e.message; }
    } else if (cfg.at === 'index_then_ptr_array') {
      // fn(args): comp=mult?a0*mult+a1:a0; idx=*(int*)(base_idx+comp*4); if(idx==-1) return 0; return *(u32*)(basePtr+idx*4).
      // basePtr is REAL .rdata (string-pointer table) -> idxval must be a small in-range index; idx=-1 -> 0.
      const baseIdx = cfg.tgt, basePtr = cfg.basePtr, mult = cfg.mult | 0;
      const a0 = t[0] >>> 0, a1 = (mult ? (t[1] >>> 0) : 0), idxval = t[t.length - 1] | 0;
      const comp = mult ? (a0 * mult + a1) : a0;
      const seed = function () { ptr(baseIdx).add(comp * 4).writeS32(idxval); };
      const call = function (fn) { return (mult ? fn(a0, a1) : fn(a0)) >>> 0; };
      try { seed(); o = call(Orig); } catch (e) { eo = e.message; }
      try { seed(); r = call(Reim); } catch (e) { er = e.message; }
    } else if (cfg.at === 'flag_multibit') {
      // void fn(idx,b1,b2[,b3]): RMW flag word at base+idx*stride via the reimpl's bit logic.
      // seed flag with a known prior value, call, snapshot. test t=[idx,b1,b2(,b3),seed].
      const base = cfg.tgt, stride = cfg.stride | 0, idx = t[0] | 0, is4 = cfg.nargs4, seed = t[t.length - 1] >>> 0;
      const slot = ptr(base).add(idx * stride);
      const call = function (fn) { if (is4) fn(idx, t[1] >>> 0, t[2] >>> 0, t[3] >>> 0); else fn(idx, t[1] >>> 0, t[2] >>> 0); };
      try { slot.writeU32(seed); call(Orig); o = slot.readU32() >>> 0; } catch (e) { eo = e.message; }
      try { slot.writeU32(seed); call(Reim); r = slot.readU32() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'void_global_transition') {
      // void fn(): if(*(int*)tgt==from) *(int*)tgt=to. test t=[seed]: seed *tgt, call, snapshot.
      const g = ptr(cfg.tgt);
      try { g.writeU32(t[0] >>> 0); Orig(); o = g.readU32() >>> 0; } catch (e) { eo = e.message; }
      try { g.writeU32(t[0] >>> 0); Reim(); r = g.readU32() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'two_global_predicate') {
      // u32 fn(): reads gate global + tgt global, returns membership. test t=[g1seed,g2seed]: seed both, compare.
      const g1 = t[0] >>> 0, g2 = t[1] >>> 0;
      ptr(cfg.gate).writeU32(g1); ptr(cfg.tgt).writeU32(g2);
      try { o = Orig() >>> 0; } catch (e) { eo = e.message; }
      try { ptr(cfg.gate).writeU32(g1); ptr(cfg.tgt).writeU32(g2); r = Reim() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'global_switch_member') {
      // u32 fn(): reads *(int*)gate, returns switch-membership. seed gate to test value, compare.
      ptr(cfg.gate).writeU32(t >>> 0);
      try { o = Orig() >>> 0; } catch (e) { eo = e.message; }
      try { ptr(cfg.gate).writeU32(t >>> 0); r = Reim() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'gated_args_to_globals') {
      // void fn(p1..p6): if(*(int*)gate==0){ write args+consts to observe globals; conditional on aux }.
      // seed gate=0 (write path) + aux per test; observe result globals. test t=[p1..p6,auxseed].
      const obs = cfg.observe;
      const fillAll = function () { obs.forEach(function (x) { ptr(x.addr).writeU32(0xFFFFFFFF); }); };
      const readAll = function () { return obs.map(function (x) { return ptr(x.addr).readU32() >>> 0; }).join('|'); };
      const aux = t[6] >>> 0;
      const setup = function () { fillAll(); ptr(cfg.gate).writeU32(0); ptr(cfg.aux).writeU32(aux); };
      try { setup(); Orig(t[0] >>> 0, t[1] >>> 0, t[2] >>> 0, t[3] >>> 0, t[4] >>> 0, t[5] >>> 0); o = readAll() + '|aux=' + (ptr(cfg.aux).readU32() >>> 0); } catch (e) { eo = e.message; }
      try { setup(); Reim(t[0] >>> 0, t[1] >>> 0, t[2] >>> 0, t[3] >>> 0, t[4] >>> 0, t[5] >>> 0); r = readAll() + '|aux=' + (ptr(cfg.aux).readU32() >>> 0); } catch (e) { er = e.message; }
    } else if (cfg.at === 'indexed_const2_set') {
      // void fn(idx): *(u32*)(base+idx*stride+off0)=v0; *(u32*)(base+idx*stride+off1)=v1.
      // seed both slots with sentinel, call(idx), snapshot -> non-degenerate vs sentinel. consts in reimpl.
      const base = cfg.tgt, stride = cfg.stride | 0, off0 = cfg.off0 | 0, off1 = cfg.off1 | 0, idx = t >>> 0;
      const rec = ptr(base).add(idx * stride);
      const snap = function () { return (rec.add(off0).readU32() >>> 0) + ',' + (rec.add(off1).readU32() >>> 0); };
      try { rec.add(off0).writeU32(0xEEEEEEEE); rec.add(off1).writeU32(0xEEEEEEEE); Orig(idx); o = snap(); } catch (e) { eo = e.message; }
      try { rec.add(off0).writeU32(0xEEEEEEEE); rec.add(off1).writeU32(0xEEEEEEEE); Reim(idx); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'gated_record_eq2') {
      // u32 fn(): g=*(int*)gate; rec=base+g*stride; return (*(rec+off0)==v0 && *(rec+off1)==v1)?1:0.
      // test t=[gidx,s0,s1]: seed gate index + the 2 slots -> exercises true (both match) + false branches.
      const base = cfg.tgt, stride = cfg.stride | 0, off0 = cfg.off0 | 0, off1 = cfg.off1 | 0;
      const gidx = t[0] >>> 0, s0 = t[1] >>> 0, s1 = t[2] >>> 0;
      const seed = function () { ptr(cfg.gate).writeU32(gidx); const rec = ptr(base).add(gidx * stride); rec.add(off0).writeU32(s0); rec.add(off1).writeU32(s1); };
      try { seed(); o = Orig() >>> 0; } catch (e) { eo = e.message; }
      try { seed(); r = Reim() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'linear_scan_find') {
      // int fn(key): for k in [0,count): if(*(int*)(base+k*stride)==key) return k; return -1.
      // seed count@gate; fill count slots with distinct non-matching markers; place key at placeAt (if in range).
      // test t=[key,placeAt]: placeAt in range -> expect placeAt; out of range -> -1. ret int (signed -1 keeps).
      const base = cfg.tgt, stride = cfg.stride | 0, cnt = cfg.count | 0;
      const key = t[0] >>> 0, placeAt = t[1] | 0;
      ptr(cfg.gate).writeU32(cnt);
      for (let k = 0; k < cnt; k++) ptr(base).add(k * stride).writeU32((0x7F000000 | k) >>> 0);
      if (placeAt >= 0 && placeAt < cnt) ptr(base).add(placeAt * stride).writeU32(key);
      try { o = '' + (Orig(key) | 0); } catch (e) { eo = e.message; }
      try { r = '' + (Reim(key) | 0); } catch (e) { er = e.message; }
    } else if (cfg.at === 'global4_bool_out') {
      // void fn(out): reads N globals at base[k], writes out[k]=predicate(base[k])?1:0.
      // test t indexes cfg.seedvecs (each a length-N seed vector mixing predicate true/false). out fresh per side.
      const base = cfg.tgt, n = (cfg.span | 0) || 4, sv = cfg.seedvecs[t >>> 0];
      for (let k = 0; k < n; k++) ptr(base).add(k * 4).writeU32(sv[k] >>> 0);
      const out = Memory.alloc(0x20); _keep.push(out);
      const snap = function () { const p = []; for (let k = 0; k < n; k++) p.push(out.add(k * 4).readU32() >>> 0); return p.join(','); };
      try { for (let k = 0; k < n; k++) out.add(k * 4).writeU32(0xEEEEEEEE); Orig(out); o = snap(); } catch (e) { eo = e.message; }
      try { for (let k = 0; k < n; k++) ptr(base).add(k * 4).writeU32(sv[k] >>> 0); for (let k = 0; k < n; k++) out.add(k * 4).writeU32(0xEEEEEEEE); Reim(out); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'indexed_bit_toggle') {
      // void fn(idx, set): flag=*(u32*)(base+idx*stride+field_off); set?flag|=bit:flag&=~bit; store.
      // test t=[idx,set,seed]: seed the flag word with a known prior value -> set/clear both exercised.
      const base = cfg.tgt, stride = cfg.stride | 0, foff = cfg.field_off | 0;
      const idx = t[0] >>> 0, set = t[1] >>> 0, seed = t[2] >>> 0;
      const slot = ptr(base).add(idx * stride + foff);
      try { slot.writeU32(seed); Orig(idx, set); o = slot.readU32() >>> 0; } catch (e) { eo = e.message; }
      try { slot.writeU32(seed); Reim(idx, set); r = slot.readU32() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'gated_int_predicate') {
      // u32 fn(arg): if(*(int*)gate==gateval) <switch membership over arg> else 0.
      // test t=[arg,gateseed]: seed the gate global -> exercises in-set/out-of-set AND gate-fail.
      const arg = t[0] >>> 0, gv = t[1] >>> 0;
      ptr(cfg.gate).writeU32(gv);
      try { o = Orig(arg) >>> 0; } catch (e) { eo = e.message; }
      try { ptr(cfg.gate).writeU32(gv); r = Reim(arg) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'indexed_vec_set') {
      // void fn(idx, in): addr=base+idx*stride; if(in) write n dwords from in to addr+j*4 else zero them.
      // tests the write path (non-null in) bit-identically; the null-zero branch shares the same addresses.
      const base = cfg.tgt, stride = cfg.stride | 0, n = (cfg.span | 0) || 3, idx = t >>> 0;
      const inb = Memory.alloc(0x20); _keep.push(inb);
      for (let j = 0; j < n; j++) inb.add(j * 4).writeU32((0xC0DE0000 | ((idx << 4) | j)) >>> 0);
      const addr = ptr(base).add(idx * stride);
      const reset = function () { for (let j = 0; j < n; j++) addr.add(j * 4).writeU32(0xEEEEEEEE); };
      const snap = function () { const p = []; for (let j = 0; j < n; j++) p.push(addr.add(j * 4).readU32() >>> 0); return p.join(','); };
      try { reset(); Orig(idx, inb); o = snap(); } catch (e) { eo = e.message; }
      try { reset(); Reim(idx, inb); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'container_record_set') {
      // void fn(container, <args>): base=container[0], idx=container[2]; addr=base+idx*0x30;
      // writes args into addr+off (off may be negative). shape: 'p'=(cont,inA), 'f'=(cont,floatval), 'pp'=(cont,inA,inB).
      // base = rec+0x100 so negative offsets stay in-bounds; same buffers both sides.
      const idx = cfg.idx | 0, shape = cfg.shape, writes = cfg.writes;
      const cont = Memory.alloc(0x10), rec = Memory.alloc(0x200); _keep.push(cont, rec);
      const base = rec.add(0x100), addr = base.add(idx * 0x30);
      const setup = function () { for (let z = 0; z < 0x200; z += 4) rec.add(z).writeU32(0xEEEEEEEE); cont.add(0).writePointer(base); cont.add(8).writeU32(idx); };
      const snap = function () { return writes.map(function (off) { return addr.add(off | 0).readU32() >>> 0; }).join(','); };
      if (shape === 'f') {
        const fval = 1.5 + (t >>> 0) * 0.25;
        try { setup(); Orig(cont, fval); o = snap(); } catch (e) { eo = e.message; }
        try { setup(); Reim(cont, fval); r = snap(); } catch (e) { er = e.message; }
      } else {
        const inA = Memory.alloc(0x10), inB = Memory.alloc(0x10); _keep.push(inA, inB);
        inA.writeU32((0xC0DE0000 | ((t << 4) | 1)) >>> 0); inA.add(4).writeU32((0xC0DE0000 | ((t << 4) | 2)) >>> 0);
        inB.writeU32((0xC0DE0000 | ((t << 4) | 3)) >>> 0); inB.add(4).writeU32((0xC0DE0000 | ((t << 4) | 4)) >>> 0);
        if (shape === 'pp') {
          try { setup(); Orig(cont, inA, inB); o = snap(); } catch (e) { eo = e.message; }
          try { setup(); Reim(cont, inA, inB); r = snap(); } catch (e) { er = e.message; }
        } else {
          try { setup(); Orig(cont, inA); o = snap(); } catch (e) { eo = e.message; }
          try { setup(); Reim(cont, inA); r = snap(); } catch (e) { er = e.message; }
        }
      }
    } else if (cfg.at === 'global_indexed_float') {
      // float fn(): idx=*(int*)gate; return *(float*)(base+idx*stride). seed idx + a FINITE
      // non-NaN float bit pattern at the slot -> distinct per idx. ret 'float' -> no >>>0 coercion.
      const idx = t >>> 0;
      ptr(cfg.gate).writeU32(idx);
      ptr(cfg.tgt).add(idx * (cfg.stride | 0)).writeU32((0x40000000 | (idx << 4)) >>> 0);
      try { o = Orig(); } catch (e) { eo = e.message; }
      try { r = Reim(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'vec16_copy_set') {
      // u32 fn(idx, in): if(idx>=bound) return 0; copy n dwords from in to TWO contiguous regions
      // at base+idx*stride (region2 immediately follows region1). alloc in distinct, snapshot 2n dwords.
      const base = cfg.tgt, stride = cfg.stride | 0, n = (cfg.span | 0) || 16, bound = cfg.bound | 0, idx = t >>> 0;
      const inb = Memory.alloc(n * 4); _keep.push(inb);
      for (let j = 0; j < n; j++) inb.add(j * 4).writeU32((0xC0DE0000 | ((idx << 8) | j)) >>> 0);
      const r1 = ptr(base).add(idx * stride);
      const reset = function () { for (let j = 0; j < n * 2; j++) r1.add(j * 4).writeU32(0xEEEEEEEE); };
      const snap = function () { const p = []; for (let j = 0; j < n * 2; j++) p.push(r1.add(j * 4).readU32() >>> 0); return p.join(','); };
      try { reset(); const ro = Orig(idx, inb) >>> 0; o = snap() + '|ret=' + ro; } catch (e) { eo = e.message; }
      try { reset(); const rr = Reim(idx, inb) >>> 0; r = snap() + '|ret=' + rr; } catch (e) { er = e.message; }
    } else if (cfg.at === 'arg_scattered_globals') {
      // void fn(arg): fill observed globals with sentinel, call(arg), read them back; vary arg.
      // A switch/branch setter writes DISTINCT globals per arg -> non-degenerate across the test set.
      const obs = cfg.observe, arg = t >>> 0;
      const fill = function () { obs.forEach(function (x) { ptr(x.addr).writeU32(0xFFFFFFFF); }); };
      const readAll = function () { return obs.map(function (x) { return ptr(x.addr).readU32() >>> 0; }).join('|'); };
      try { fill(); Orig(arg); o = readAll(); } catch (e) { eo = e.message; }
      try { fill(); Reim(arg); r = readAll(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'table_ret_ptrout') {
      // u32 fn(idx, out): addr=base+idx*stride; if(out) *out=*(u32*)(addr+off0); return *(u32*)(addr+off1).
      // seed both slots distinct -> *out and ret both non-degenerate; fresh out per side.
      const base = cfg.tgt, stride = cfg.stride | 0, offA = cfg.off0 | 0, offB = cfg.off1 | 0, idx = t >>> 0;
      const slot = ptr(base).add(idx * stride);
      slot.add(offA).writeU32((0xC0DE0000 | idx) >>> 0);
      slot.add(offB).writeU32((0xC0DE1000 | idx) >>> 0);
      const outO = Memory.alloc(0x10), outR = Memory.alloc(0x10); _keep.push(outO, outR);
      try { outO.writeU32(0); const ro = Orig(idx, outO) >>> 0; o = (outO.readU32() >>> 0) + '|ret=' + ro; } catch (e) { eo = e.message; }
      try { outR.writeU32(0); const rr = Reim(idx, outR) >>> 0; r = (outR.readU32() >>> 0) + '|ret=' + rr; } catch (e) { er = e.message; }
    } else if (cfg.at === 'cond_table_get') {
      // u32 fn(idx): rec=base+idx*stride; return *(rec+offf) ? *(rec+off1) : *(rec+off0).
      // test t=[idx,flag]: seed slot0/slot1 distinct + flag -> exercises BOTH branches non-degenerately.
      const base = cfg.tgt, stride = cfg.stride | 0, idx = t[0] >>> 0, flag = t[1] >>> 0;
      const rec = ptr(base).add(idx * stride);
      rec.add(cfg.off0 | 0).writeU32((0xC0DE0000 | idx) >>> 0);
      rec.add(cfg.off1 | 0).writeU32((0xC0DE1000 | idx) >>> 0);
      rec.add(cfg.offf | 0).writeU32(flag);
      try { o = Orig(idx) >>> 0; } catch (e) { eo = e.message; }
      try { r = Reim(idx) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'ptr_compute_get') {
      // u32 fn(out,idx): if(idx>=bound) return 0; t=*(u32*)(idxtbl+idx*stride); *out=base+idx*stride+t*tscale; return 1.
      // seed idxtbl slot with a small distinct t -> *out varies per idx (non-degenerate). fresh out per side.
      const base = cfg.tgt, idxtbl = cfg.idxtbl, stride = cfg.stride | 0, tscale = cfg.tscale | 0, bound = cfg.bound | 0, idx = t >>> 0;
      if (idx < bound) ptr(idxtbl).add(idx * stride).writeU32((0x100 | idx) >>> 0);
      const outO = Memory.alloc(0x10), outR = Memory.alloc(0x10); _keep.push(outO, outR);
      try { outO.writeU32(0); const ro = Orig(outO, idx) >>> 0; o = (outO.readU32() >>> 0) + '|ret=' + ro; } catch (e) { eo = e.message; }
      try { outR.writeU32(0); const rr = Reim(outR, idx) >>> 0; r = (outR.readU32() >>> 0) + '|ret=' + rr; } catch (e) { er = e.message; }
    } else if (cfg.at === 'eq_predicate_get') {
      // u32 fn(p1,p2): if(*(int*)gate<gatemax && p2>=0) return tbl[p1*stride]==tbl[p2*stride]?1:0; return 0.
      // test t=[p1,p2,eq,gateval]: gate value + equal/unequal table slots exercise all branches.
      const base = cfg.tgt, stride = cfg.stride | 0, gate = cfg.gate;
      const p1 = t[0] >>> 0, p2 = t[1] >>> 0, eq = t[2] | 0, gv = (t.length > 3 ? t[3] | 0 : 0);
      ptr(gate).writeU32(gv >>> 0);
      if ((p1 | 0) >= 0 && p1 < 0x100) ptr(base).add(p1 * stride).writeU32(0xC0DE0001 >>> 0);
      if ((p2 | 0) >= 0 && p2 < 0x100) ptr(base).add(p2 * stride).writeU32((eq ? 0xC0DE0001 : 0xC0DE0002) >>> 0);
      try { o = Orig(p1, p2) >>> 0; } catch (e) { eo = e.message; }
      try { r = Reim(p1, p2) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'cond_global_set') {
      // void fn(v): if (v==0 || *tgt==0) *tgt=v. test t=[seed,arg]: seed *tgt, call(arg),
      // snapshot *tgt. The seed/arg pairs exercise all 3 branches (v==0 write, global==0
      // write, both-nonzero no-write) -> non-degenerate. Reset between sides.
      const seed = t[0] >>> 0, arg = t[1] >>> 0, g = ptr(cfg.tgt);
      try { g.writeU32(seed); Orig(arg); o = g.readU32() >>> 0; } catch (e) { eo = e.message; }
      try { g.writeU32(seed); Reim(arg); r = g.readU32() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'range_init') {
      // void fn(): writes a contiguous global range. Fill sentinel, call, snapshot range.
      const base = ptr(cfg.tgt), len = cfg.len | 0;
      const fill = function () { for (let z = 0; z < len; z += 4) base.add(z).writeU32(0xEEEEEEEE); };
      const snap = function () { const p = []; for (let z = 0; z < len; z += 4) p.push(base.add(z).readU32() >>> 0); return p.join(','); };
      try { fill(); Orig(); o = snap(); } catch (e) { eo = e.message; }
      try { fill(); Reim(); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'indexed_table_set') {
      // void fn(i, val): *(tgt + i*stride) = val. Fix i=set_idx, vary val.
      const base = ptr(cfg.tgt), stride = cfg.stride | 0, idx = cfg.set_idx | 0, val = t >>> 0;
      const slot = base.add(idx * stride);
      try { slot.writeU32(0xFFFFFFFF); Orig(idx, val); o = slot.readU32() >>> 0; } catch (e) { eo = e.message; }
      try { slot.writeU32(0xFFFFFFFF); Reim(idx, val); r = slot.readU32() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'ptr_table_field_read') {
      // return *(*(tgt)[i] + field_off). Seed *(tgt)=tableBuf, tableBuf[i]=&entry,
      // entry[field_off]=distinct value -> non-degenerate.
      const N = (cfg.capacity | 0) || 8, fo = cfg.field_off | 0, idx = t >>> 0;
      const tableBuf = Memory.alloc(N * 4), entry = Memory.alloc(0x40); _keep.push(tableBuf, entry);
      const sv = (0xC0DE0000 | (idx & 0xffff)) >>> 0;
      const setup = function () {
        for (let z = 0; z < 0x40; z += 4) entry.add(z).writeU32(0);
        entry.add(fo).writeU32(sv);
        tableBuf.add(idx * 4).writePointer(entry);
        ptr(cfg.tgt).writePointer(tableBuf);
      };
      try { setup(); o = Orig(idx) >>> 0; } catch (e) { eo = e.message; }
      try { setup(); r = Reim(idx) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'stack_pop_snapshot') {
      // array-stack {top@0, cap@4, buf@8}. test value t = initial top (exercises edge).
      const N = (cfg.capacity | 0) || 4;
      const st = Memory.alloc(0xc), buf = Memory.alloc(N * 4); _keep.push(st, buf);
      const ib = cfg.init_buf, top0 = (t | 0);
      const reset = function () { st.writeS32(top0); st.add(4).writeU32(N); st.add(8).writePointer(buf); for (let k = 0; k < N; k++) buf.add(k * 4).writeU32(ib[k] >>> 0); };
      const snap = function () { const p = [st.readS32()]; for (let k = 0; k < N; k++) p.push(buf.add(k * 4).readU32() >>> 0); return p.join(','); };
      try { reset(); const ro = Orig(st) >>> 0; o = snap() + '|ret=' + ro; } catch (e) { eo = e.message; }
      try { reset(); const rr = Reim(st) >>> 0; r = snap() + '|ret=' + rr; } catch (e) { er = e.message; }
    } else if (cfg.at === 'stack_push_snapshot') {
      // array-stack push. test value t = value pushed (distinct -> non-degenerate).
      const N = (cfg.capacity | 0) || 4;
      const st = Memory.alloc(0xc), buf = Memory.alloc(N * 4); _keep.push(st, buf);
      const top0 = (cfg.init_top | 0);
      const reset = function () { st.writeS32(top0); st.add(4).writeU32(N); st.add(8).writePointer(buf); for (let k = 0; k < N; k++) buf.add(k * 4).writeU32(0); };
      const snap = function () { const p = [st.readS32()]; for (let k = 0; k < N; k++) p.push(buf.add(k * 4).readU32() >>> 0); return p.join(','); };
      try { reset(); const ro = Orig(st, t >>> 0) >>> 0; o = snap() + '|ret=' + ro; } catch (e) { eo = e.message; }
      try { reset(); const rr = Reim(st, t >>> 0) >>> 0; r = snap() + '|ret=' + rr; } catch (e) { er = e.message; }
    } else if (cfg.at === 'eax_implicit_void') {
      // The function uses EAX as an implicit `this`. Build a tiny trampoline
      // `mov eax, buf ; jmp target` (B8 imm32 / E9 rel32), call it (no args), and
      // check the observed buffer fields. Fill with a sentinel first so a wrong
      // reimpl that leaves any field unwritten -> RED.
      const obs = cfg.observe;
      const ebuf = Memory.alloc(0x100); _keep.push(ebuf);
      const mkTramp = function (target) {
        const tr = Memory.alloc(Process.pageSize); _keep.push(tr);
        tr.writeU8(0xB8); tr.add(1).writePointer(ebuf); tr.add(5).writeU8(0xE9);
        tr.add(6).writeS32(target.sub(tr.add(10)).toInt32());
        Memory.protect(tr, 16, 'rwx');
        return new NativeFunction(tr, 'void', [], 'mscdecl');
      };
      const fillAll = function () { for (let z = 0; z < 0x100; z += 4) ebuf.add(z).writeU32(0xFFFFFFFF); };
      const readObs = function () { return obs.map(function (x) { return ebuf.add(x.off | 0).readU32() >>> 0; }).join('|'); };
      try { fillAll(); mkTramp(ptr(cfg.rva))(); o = readObs(); } catch (e) { eo = e.message; }
      try { fillAll(); mkTramp(reim)(); r = readObs(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'float_table_read') {
      // return *(float*)(base+i*stride). Seed the table bits (read as float, distinct
      // -> non-degenerate). ret is float so DO NOT coerce with >>> 0.
      const st = cfg.seed_table;
      if (i === 0 && st) { const b = ptr(st.base); for (let k = 0; k < (st.span | 0); k++) b.add(k * (st.stride | 0)).writeU32((0xC0DE0000 | k) >>> 0); }
      try { o = Orig(t >>> 0); } catch (e) { eo = e.message; }
      try { r = Reim(t >>> 0); } catch (e) { er = e.message; }
    } else if (cfg.at === 'global_field_read') {
      // return *(*(tgt)+field_off). Point the global at a seeded buffer; the test
      // value lands at +field_off (distinct per test -> non-degenerate).
      const fo = cfg.field_off | 0;
      const buf = Memory.alloc(0x100); _keep.push(buf);
      for (let z = 0; z < 0x100; z += 4) buf.add(z).writeU32(0);
      buf.add(fo).writeU32(t >>> 0);
      ptr(cfg.tgt).writePointer(buf);
      try { o = Orig() >>> 0; } catch (e) { eo = e.message; }
      try { ptr(cfg.tgt).writePointer(buf); buf.add(fo).writeU32(t >>> 0); r = Reim() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'indexed_global_field_read') {
      // return *(u32*)(*(u32*)tgt + *(u32*)glob + field_off). Seed base global at a
      // scratch buffer, index global at a fixed nonzero (cfg.idx, default 0x40), and
      // place the test value at buf[idx+field_off]. Verifies the base-ptr global, the
      // index global address, and the field offset all together (a wrong index-global
      // address reads idx=0 -> different slot -> RED). test = value (varied).
      const fo2 = cfg.field_off | 0, gidx = (cfg.idx | 0) || 0x40;
      const gbuf = Memory.alloc(0x800); _keep.push(gbuf);
      for (let z = 0; z < 0x800; z += 4) gbuf.add(z).writeU32(0);
      const seedG = function () {
        ptr(cfg.glob).writeU32(gidx >>> 0);
        gbuf.add(gidx + fo2).writeU32(t >>> 0);
        ptr(cfg.tgt).writePointer(gbuf);
      };
      try { seedG(); o = Orig() >>> 0; } catch (e) { eo = e.message; }
      try { seedG(); r = Reim() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'indexed_global_field_write') {
      // int fn(v): *(u32*)(*(u32*)tgt + *(u32*)glob + field_off) = v; return <const or v>.
      // seed base global -> scratch buf, index global -> fixed nonzero (cfg.idx), call
      // fn(v), observe the written slot AND the return joined. Verifies base ptr +
      // index global + field_off + store value + return. test = v (varied).
      const fo4 = cfg.field_off | 0, gi4 = (cfg.idx | 0) || 0x40;
      const wbuf = Memory.alloc(0x800); _keep.push(wbuf);
      const runW = function (CALL) {
        for (let z = 0; z < 0x800; z += 4) wbuf.add(z).writeU32(0);
        ptr(cfg.glob).writeU32(gi4 >>> 0);
        ptr(cfg.tgt).writePointer(wbuf);
        const rv = CALL(t >>> 0) >>> 0;
        return (wbuf.add(gi4 + fo4).readU32() >>> 0) + '|' + rv;
      };
      try { o = runW(Orig); } catch (e) { eo = e.message; }
      try { r = runW(Reim); } catch (e) { er = e.message; }
    } else if (cfg.at === 'indexed_global_2lvl') {
      // base=*(u32*)tgt; idx=*(u32*)glob; edx=*(u32*)(base+idx+mid_off);
      // return *(u32*)(base+edx*4+idx). Seed base global -> scratch buffer, index
      // global -> fixed nonzero (cfg.idx, default 0x40), write edx_val (default 7) at
      // buf[idx+mid_off], and the test value at buf[edx_val*4+idx]. A wrong base/index
      // global, mid_off, or *4 scale in the reimpl reads a 0-sentinel slot -> RED.
      const mo = cfg.mid_off | 0, gi5 = (cfg.idx | 0) || 0x40, ev = (cfg.edx_val | 0) || 7;
      const g2buf = Memory.alloc(0x2000); _keep.push(g2buf);
      const seed2 = function () {
        for (let z = 0; z < 0x2000; z += 4) g2buf.add(z).writeU32(0);
        ptr(cfg.glob).writeU32(gi5 >>> 0);
        ptr(cfg.tgt).writePointer(g2buf);
        g2buf.add(gi5 + mo).writeU32(ev >>> 0);
        g2buf.add(ev * 4 + gi5).writeU32(t >>> 0);
      };
      try { seed2(); o = Orig() >>> 0; } catch (e) { eo = e.message; }
      try { seed2(); r = Reim() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'indexed_bound_array_get') {
      // u32 fn(i): if(i > *(u32*)glob) return 0; cont=*(u32*)tgt; arr=*(u32*)(cont+field_off);
      // return *(u32*)(arr+i*4). Seed glob=large bound (so the fixed index passes), tgt->cont
      // buffer, cont[field_off]->arr buffer, arr[idx*4]=test. Call fn(idx). A wrong bound/field
      // offset/container global reads a 0-sentinel slot or fails the bound -> RED. test=value.
      const ao6 = cfg.field_off | 0, gi6 = (cfg.idx | 0) || 5;
      const cbuf = Memory.alloc(0x100), abuf = Memory.alloc(0x400); _keep.push(cbuf, abuf);
      const seedB = function () {
        for (let z = 0; z < 0x100; z += 4) cbuf.add(z).writeU32(0);
        for (let z = 0; z < 0x400; z += 4) abuf.add(z).writeU32(0);
        ptr(cfg.glob).writeU32(0xFFFF);
        ptr(cfg.tgt).writePointer(cbuf);
        cbuf.add(ao6).writePointer(abuf);
        abuf.add(gi6 * 4).writeU32(t >>> 0);
      };
      try { seedB(); o = Orig(gi6 >>> 0) >>> 0; } catch (e) { eo = e.message; }
      try { seedB(); r = Reim(gi6 >>> 0) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'abs_ranges_setter') {
      // void fn(scalars...): writes to absolute globals. Reset cfg.abs_ranges to 0,
      // call fn(test scalars), snapshot the same ranges, compare. A wrong base/stride/
      // offset in the reimpl writes a different slot -> snapshot differs -> RED. The
      // scalar args (i, v) vary per test so the written slots/values differ -> non-degen.
      const ranges = cfg.abs_ranges || [];
      const aa = Array.isArray(t) ? t : [t];
      const resetR = function () { ranges.forEach(function (rg) { for (let z = 0; z < rg.dwords; z++) ptr(rg.addr).add(z * 4).writeU32(0); }); };
      const snapR = function () { const p = []; ranges.forEach(function (rg) { for (let z = 0; z < rg.dwords; z++) p.push(ptr(rg.addr).add(z * 4).readU32() >>> 0); }); return p.join(','); };
      const callF = function (F) {
        if (aa.length === 1) return F(aa[0] >>> 0);
        if (aa.length === 3) return F(aa[0] >>> 0, aa[1] >>> 0, aa[2] >>> 0);
        return F(aa[0] >>> 0, aa[1] >>> 0);
      };
      try { resetR(); callF(Orig); o = snapR(); } catch (e) { eo = e.message; }
      try { resetR(); callF(Reim); r = snapR(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'esi_global_search') {
      // u32 fn(ESI=key): linear-search a global table for entry[+0]==key, return an
      // index-derived pointer (or 0). ORIG is reg-arg (ESI) -> `mov esi,key; jmp target`
      // trampoline. REIMPL is __cdecl(key) reading the SAME globals -> compares the
      // computed pointer, not the ABI. Seed count=4, zero 4 entries, set table[idx*stride]
      // [+0]=key (key=0xC0DE0000|idx, distinct & nonzero). test=idx -> distinct matched addr.
      const sstride = cfg.stride | 0, sidx = t | 0;
      const skey = (0xC0DE0000 | sidx) >>> 0;
      const seedS = function () {
        for (let z = 0; z < 4 * sstride; z += 4) ptr(cfg.tgt).add(z).writeU32(0);
        // cfg.key_off: byte offset of the KEY FIELD within an entry, default 0.
        // Added orch-iter21 for 0x00407550, which compares at entry+0x44
        // (CMP dword ptr [EAX+0x44],ESI at 0x00407561) rather than entry+0.
        // Defaulting to 0 leaves every existing caller byte-identical. The zero
        // loop above already clears 4*stride bytes, so it covers any in-entry
        // offset without change.
        ptr(cfg.tgt).add(sidx * sstride + (cfg.key_off | 0)).writeU32(skey);
        ptr(cfg.glob).writeU32(4);
      };
      const mkEsi = function (target) {
        const tr = Memory.alloc(Process.pageSize); _keep.push(tr);
        tr.writeU8(0xBE); tr.add(1).writeU32(skey);            // mov esi, key
        tr.add(5).writeU8(0xE9);                                // jmp target
        tr.add(6).writeS32(target.sub(tr.add(10)).toInt32());
        Memory.protect(tr, 16, 'rwx');
        return new NativeFunction(tr, 'uint32', [], 'mscdecl');
      };
      try { seedS(); o = mkEsi(ptr(cfg.rva))() >>> 0; } catch (e) { eo = e.message; }
      try { seedS(); r = Reim(skey >>> 0) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'indexed_global_idiv') {
      // u32 fn(arg): d = *(int*)(tgt + arg*stride); q = num / d (signed idiv); clamp. Seed
      // d = test value at the slot (fixed arg = cfg.idx, default 0), call fn(idx), compare
      // ret. test = divisor (varied -> distinct quotients -> non-degen). Reimpl __cdecl(arg)
      // does signed C division, which truncates toward zero exactly like x86 idiv.
      const divarg = (cfg.idx | 0), divslot = (cfg.tgt >>> 0) + divarg * (cfg.stride | 0);
      const seedDv = function () { ptr(divslot).writeS32(t | 0); };
      try { seedDv(); o = Orig(divarg >>> 0) >>> 0; } catch (e) { eo = e.message; }
      try { seedDv(); r = Reim(divarg >>> 0) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'float_vec3_lerp_out') {
      // void fn(out*, a*, b*, float t): out = a + t*(b-a) per component (pure x87). Seed
      // a,b vec3 + t, call, snapshot out[0..2] as u32 bit patterns. Reimpl is verbatim
      // naked __asm -> bit-identical x87. seed-controlled -> non-degenerate.
      const sa = cfg.seed_a || [0, 0, 0], sb = cfg.seed_b || [0, 0, 0];
      const tb = Memory.alloc(4); tb.writeU32((cfg.t_bits >>> 0)); const tf = tb.readFloat();
      const out1 = Memory.alloc(0x20), av = Memory.alloc(0x20), bv = Memory.alloc(0x20);
      _keep.push(out1, av, bv, tb);
      const runL = function (CALL) {
        for (let k = 0; k < 8; k++) out1.add(k * 4).writeU32(0);
        for (let k = 0; k < 3; k++) { av.add(k * 4).writeU32(sa[k] >>> 0); bv.add(k * 4).writeU32(sb[k] >>> 0); }
        CALL(out1, av, bv, tf);
        return [out1.readU32() >>> 0, out1.add(4).readU32() >>> 0, out1.add(8).readU32() >>> 0].join('|');
      };
      try { o = runL(Orig); } catch (e) { eo = e.message; }
      try { r = runL(Reim); } catch (e) { er = e.message; }
    } else if (cfg.at === 'float_2ptr_ret') {
      // float fn(a*, b*): pure float of two vec3 args (dot product + clamp etc.), returns
      // ST0. Seed a,b from cfg.seed_pairs[t], call, compare the float return as a u32 bit
      // pattern (avoids NaN compare issues). Reimpl is verbatim naked __asm (bit-identical).
      const sp = (cfg.seed_pairs || [])[t | 0] || { a: [0, 0, 0], b: [0, 0, 0] };
      const av2 = Memory.alloc(0x20), bv2 = Memory.alloc(0x20), tmp = Memory.alloc(4);
      _keep.push(av2, bv2, tmp);
      const runR = function (CALL) {
        for (let k = 0; k < 4; k++) { av2.add(k * 4).writeU32(0); bv2.add(k * 4).writeU32(0); }
        for (let k = 0; k < 3; k++) { av2.add(k * 4).writeFloat(sp.a[k]); bv2.add(k * 4).writeFloat(sp.b[k]); }
        const fv = CALL(av2, bv2);
        tmp.writeFloat(fv); return tmp.readU32() >>> 0;
      };
      try { o = runR(Orig); } catch (e) { eo = e.message; }
      try { r = runR(Reim); } catch (e) { er = e.message; }
    } else if (cfg.at === 'float_planes6_predicate') {
      // u32 fn(obj*, point*): 6 planes at obj+0x94 stride 0x14; return 1 if the point is
      // inside all 6 (dot(plane.xyz,point)-plane.w <= -point.w) else 0. Seed obj+point from
      // cfg.seed_sets[t]; distinct plane.w exercises the 0x14 stride + full loop. Reimpl
      // is verbatim naked __asm (bit-identical). Results vary 0/1 -> non-degenerate.
      const ss = (cfg.seed_sets || [])[t | 0] || { point: [0, 0, 0, 0], planes: [] };
      const obj = Memory.alloc(0x200), pt = Memory.alloc(0x20); _keep.push(obj, pt);
      const runP6 = function (CALL) {
        for (let z = 0; z < 0x200; z += 4) obj.add(z).writeU32(0);
        for (let z = 0; z < 0x20; z += 4) pt.add(z).writeU32(0);
        for (let k = 0; k < 4; k++) pt.add(k * 4).writeFloat((ss.point || [])[k] || 0);
        for (let p = 0; p < 6; p++) {
          const pl = (ss.planes || [])[p] || [0, 0, 0, 0];
          for (let k = 0; k < 4; k++) obj.add(0x94 + p * 0x14 + k * 4).writeFloat(pl[k] || 0);
        }
        return CALL(obj, pt) >>> 0;
      };
      try { o = runP6(Orig); } catch (e) { eo = e.message; }
      try { r = runP6(Reim); } catch (e) { er = e.message; }
    } else if (cfg.at === 'eax_edi_out') {
      // void fn(EAX=v, EDI=out*): writes out[0..2] derived from v. ORIG via reg trampoline
      // (mov eax,v; mov edi,outbuf; jmp). REIMPL is __cdecl(v, out) -> compares out[0..2],
      // not the ABI. test=v (varied -> distinct splits -> non-degen).
      const v = t | 0;
      const ob1 = Memory.alloc(0x40), ob2 = Memory.alloc(0x40); _keep.push(ob1, ob2);
      const mkED = function (target, outp) {
        const tr = Memory.alloc(Process.pageSize); _keep.push(tr);
        tr.writeU8(0xB8); tr.add(1).writeS32(v);                 // mov eax, v
        tr.add(5).writeU8(0xBF); tr.add(6).writePointer(outp);   // mov edi, outp
        tr.add(10).writeU8(0xE9); tr.add(11).writeS32(target.sub(tr.add(15)).toInt32()); // jmp
        Memory.protect(tr, 32, 'rwx');
        return new NativeFunction(tr, 'void', [], 'mscdecl');
      };
      const rdE = function (buf) { const a = []; for (let k = 0; k < 3; k++) a.push(buf.add(k * 4).readU32() >>> 0); return a.join('|'); };
      try { for (let z = 0; z < 0x40; z += 4) ob1.add(z).writeU32(0); mkED(ptr(cfg.rva), ob1)(); o = rdE(ob1); } catch (e) { eo = e.message; }
      try { for (let z = 0; z < 0x40; z += 4) ob2.add(z).writeU32(0); Reim(v >>> 0, ob2); r = rdE(ob2); } catch (e) { er = e.message; }
    } else if (cfg.at === 'grid_getter_multiout') {
      // u32 fn(i,j,out1,_,_,out2,out3): bounds + 2D-indexed parallel-table getter. Seed
      // bounds large + the indexed slots (per cfg.grid), call with 3 out-bufs, compare
      // outs+ret. test=marker base (varied -> distinct seeded values -> non-degen).
      const g = cfg.grid, gi = g.i | 0, gj = g.j | 0;
      const idx = gi * (g.mul1 | 0) + gj, idx3 = gi * (g.mul3 | 0) + gj, mk = t >>> 0;
      const o1 = Memory.alloc(0x20), o2 = Memory.alloc(0x20), o3 = Memory.alloc(0x20); _keep.push(o1, o2, o3);
      const z = function (b) { for (let q = 0; q < 0x20; q += 4) b.add(q).writeU32(0); };
      const seedG = function () {
        ptr(g.b1).writeU32(0x7fffffff); ptr(g.b2).writeU32(0x7fffffff);
        g.out1_t.forEach(function (a, k) { ptr(a).add(idx * (g.s12 | 0)).writeU32((mk + 0x10 + k) >>> 0); });
        g.out2_t.forEach(function (a, k) { ptr(a).add(idx * (g.s12 | 0)).writeU32((mk + 0x20 + k) >>> 0); });
        g.out3_t.forEach(function (a, k) { ptr(a).add(idx3 * (g.s3 | 0)).writeU32((mk + 0x30 + k) >>> 0); });
      };
      const rd = function () { return [o1.readU32(), o1.add(4).readU32(), o2.readU32(), o2.add(4).readU32(), o3.readU32()].map(function (x) { return x >>> 0; }).join('|'); };
      try { seedG(); z(o1); z(o2); z(o3); const ro = Orig(gi >>> 0, gj >>> 0, o1, o2, o3) >>> 0; o = rd() + '|' + ro; } catch (e) { eo = e.message; }
      try { seedG(); z(o1); z(o2); z(o3); const rr = Reim(gi >>> 0, gj >>> 0, o1, o2, o3) >>> 0; r = rd() + '|' + rr; } catch (e) { er = e.message; }
    } else if (cfg.at === 'struct_ctor_big') {
      // void fn(p): deterministic constructor. ONE shared sentinel buffer so self-relative
      // pointer writes (p+const) compare equal; reset between sides; snapshot observe offsets.
      const obs = cfg.observe, bd = (cfg.buf_dwords | 0) || 0x600;
      const cb = Memory.alloc(bd * 4); _keep.push(cb);
      const fillC = function () { for (let z = 0; z < bd; z++) cb.add(z * 4).writeU32(0xA5A5A5A5); };
      const snapC = function () { return obs.map(function (x) { return cb.add(x.off | 0).readU32() >>> 0; }).join(','); };
      try { fillC(); Orig(cb); o = snapC(); } catch (e) { eo = e.message; }
      try { fillC(); Reim(cb); r = snapC(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'indexed_abs_dualout') {
      // u32 fn(i, out1*, out2*): bounded dual-out getter from two absolute tables. Seed
      // tbl1[i*stride] and tbl2[i*stride] with distinct markers (works on .bss — committed,
      // zero, writable), call, compare out1|out2|ret. test=in-bounds i (varied -> non-degen).
      const ii = t | 0, std = cfg.stride | 0, mk = (0xC0DE0000 | ii) >>> 0;
      const o1 = Memory.alloc(0x10), o2 = Memory.alloc(0x10); _keep.push(o1, o2);
      const seedD = function () { ptr(cfg.tbl1).add(ii * std).writeU32(mk); ptr(cfg.tbl2).add(ii * std).writeU32((mk ^ 0xFFFF) >>> 0); o1.writeU32(0); o2.writeU32(0); };
      const rdD = function (rv) { return (o1.readU32() >>> 0) + '|' + (o2.readU32() >>> 0) + '|' + (rv >>> 0); };
      try { seedD(); const ro = Orig(ii >>> 0, o1, o2) >>> 0; o = rdD(ro); } catch (e) { eo = e.message; }
      try { seedD(); const rr = Reim(ii >>> 0, o1, o2) >>> 0; r = rdD(rr); } catch (e) { er = e.message; }
    } else if (cfg.at === 'dll_remove_count') {
      // void fn(list, node): decrement list[0] and unlink node via its [0x20]/[0x24] link
      // pointers (A=node[0x24],B=node[0x20]; *A=B; *(B+4)=A). Empty list (list[4]=list+4)
      // skips the pure-read search loop. Shared buffers both sides so the relinked pointers
      // compare equal; snapshot list[0]|A[0]|B[4]. test ignored.
      const lst = Memory.alloc(0x40), nd = Memory.alloc(0x40), A = Memory.alloc(0x40), Bn = Memory.alloc(0x40);
      _keep.push(lst, nd, A, Bn);
      const buildR = function () {
        [lst, nd, A, Bn].forEach(function (bf) { for (let z = 0; z < 0x40; z += 4) bf.add(z).writeU32(0); });
        lst.writeU32(7);
        lst.add(4).writePointer(lst.add(4));   // empty list -> skip search loop
        nd.add(0x20).writePointer(Bn);
        nd.add(0x24).writePointer(A);
      };
      const snapR = function () { return (lst.readU32() >>> 0) + '|' + (A.readU32() >>> 0) + '|' + (Bn.add(4).readU32() >>> 0); };
      try { buildR(); Orig(lst, nd); o = snapR(); } catch (e) { eo = e.message; }
      try { buildR(); Reim(lst, nd); r = snapR(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'dll_insert_head') {
      // void fn(list, node): intrusive insert-at-head. Test with an empty list
      // (*(list+8)=list+8) and node[4]=0 (skip the unlink branch). Shared buffers both
      // sides so the written addresses compare equal; snapshot the 5 link writes.
      const lst2 = Memory.alloc(0x40), nd2 = Memory.alloc(0x40); _keep.push(lst2, nd2);
      const buildI = function () {
        for (let z = 0; z < 0x40; z += 4) { lst2.add(z).writeU32(0); nd2.add(z).writeU32(0); }
        lst2.add(8).writePointer(lst2.add(8));   // empty: *(list+8)=list+8
      };
      const snapI = function () { return [nd2.add(4).readU32(), nd2.add(8).readU32(), nd2.add(0xc).readU32(), lst2.add(8).readU32(), lst2.add(0xc).readU32()].map(function (x) { return x >>> 0; }).join('|'); };
      try { buildI(); Orig(lst2, nd2); o = snapI(); } catch (e) { eo = e.message; }
      try { buildI(); Reim(lst2, nd2); r = snapI(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'global_ptrtable_match') {
      // u32 fn(arg1, arg2): scan a 4-entry global ptr table for an entry matching arg1 +
      // arg2[4]==idx. Seed tbl[2]=&entry (.bss), entry[0xc]=1, entry[0x28]=KEY. test 0 ->
      // arg2[4]=2 (match -> 1); test 1 -> arg2[4]=3 (no match -> 0). non-degen via 1/0.
      const tbl = ptr(cfg.tbl), entry = Memory.alloc(0x40), a2 = Memory.alloc(0x40); _keep.push(entry, a2);
      const KEY = 0xABCD01, match = (t | 0) === 0;
      const setupM = function () {
        for (let k = 0; k < 4; k++) tbl.add(k * 4).writeU32(0);
        for (let z = 0; z < 0x40; z += 4) { entry.add(z).writeU32(0); a2.add(z).writeU32(0); }
        tbl.add(2 * 4).writePointer(entry);
        entry.add(0xc).writeU32(1);
        entry.add(0x28).writeU32(KEY);
        a2.add(4).writeU32(match ? 2 : 3);
      };
      try { setupM(); o = Orig(KEY >>> 0, a2) >>> 0; } catch (e) { eo = e.message; }
      try { setupM(); r = Reim(KEY >>> 0, a2) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'global_rec_clear_ret') {
      // u32 fn(arg1, arg2): rec=*(glob)+arg2; if(rec[0xc]){ rec[0xc]=0; rec[8]=0; return arg1; } return 0.
      // seed *glob=&buf; buf[off+0xc]= test0:nonzero (-> zeroed + ret arg1) / test1:0 (-> ret 0).
      // observe buf[off+8]|buf[off+0xc]|ret. non-degen via ret + the zeroing (a non-zeroing reimpl
      // leaves the seeded values -> RED).
      const off = cfg.idx | 0, ARG1 = 0xDEAD01, linked = (t | 0) === 0;
      const rbuf = Memory.alloc(0x200); _keep.push(rbuf);
      const setupR = function () {
        for (let z = 0; z < 0x200; z += 4) rbuf.add(z).writeU32(0);
        ptr(cfg.glob).writePointer(rbuf);
        if (linked) { rbuf.add(off + 0xc).writeU32(0x55); rbuf.add(off + 8).writeU32(0x66); }
      };
      const rdR = function (rv) { return (rbuf.add(off + 8).readU32() >>> 0) + '|' + (rbuf.add(off + 0xc).readU32() >>> 0) + '|' + (rv >>> 0); };
      try { setupR(); const ro = Orig(ARG1 >>> 0, off >>> 0) >>> 0; o = rdR(ro); } catch (e) { eo = e.message; }
      try { setupR(); const rr = Reim(ARG1 >>> 0, off >>> 0) >>> 0; r = rdR(rr); } catch (e) { er = e.message; }
    } else if (cfg.at === 'abs_scan_flag') {
      // void fn(): scan an abs dword range; set flag=0xff if any nonzero. Reset the range
      // (glob, span dwords) + flag(tgt)=0x11 sentinel; test0 seeds one nonzero at glob+idx
      // (-> flag 0xff), test1 all-zero (-> flag stays 0x11). observe flag. non-degen.
      const flag = ptr(cfg.tgt), lo = ptr(cfg.glob), span = cfg.span | 0, so = cfg.idx | 0;
      const seedF = function (nz) { for (let k = 0; k < span; k++) lo.add(k * 4).writeU32(0); flag.writeU32(0x11); if (nz) lo.add(so).writeU32(1); };
      const nzc = (t | 0) === 0;
      try { seedF(nzc); Orig(); o = flag.readU32() >>> 0; } catch (e) { eo = e.message; }
      try { seedF(nzc); Reim(); r = flag.readU32() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'global_2level_list_search') {
      // int fn(key): walk outer list node=g[4] via node[8]; each node has an entry e=node[0];
      // if e && e[8]==key return e[0xc]; else -1. Seed *glob=&cont, cont[4]=&node, node[0]=&entry,
      // entry[8]=KEY, entry[0xc]=RESULT, node[8]=0. test0 key=KEY (->RESULT), test1 key=KEY^1 (->-1).
      const cont = Memory.alloc(0x40), node = Memory.alloc(0x40), entry = Memory.alloc(0x40);
      _keep.push(cont, node, entry);
      const KEY = 0x1234, RESULT = 0xBEEF99;
      const setupL = function () {
        [cont, node, entry].forEach(function (b) { for (let z = 0; z < 0x40; z += 4) b.add(z).writeU32(0); });
        ptr(cfg.glob).writePointer(cont);
        cont.add(4).writePointer(node);
        node.writePointer(entry);
        node.add(8).writeU32(0);
        entry.add(8).writeU32(KEY);
        entry.add(0xc).writeU32(RESULT);
      };
      const kk = (t | 0) === 0 ? KEY : (KEY ^ 1);
      try { setupL(); o = Orig(kk >>> 0) >>> 0; } catch (e) { eo = e.message; }
      try { setupL(); r = Reim(kk >>> 0) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'arg_flag_branch_getter') {
      // u32 fn(arg): pure 4-branch getter. Seed arg[0x20]=c, arg[0x1c]=flag, arg[0]=&p,
      // p[0x40]=f per cfg.seed_sets[t]. SHARED arg+p buffers so the arg+0x2c / arg+0x28
      // branches (which return the buffer address) compare equal between sides.
      const ss = (cfg.seed_sets || [])[t | 0] || { c: 0, flag: 0, f: 0 };
      const argb = Memory.alloc(0x40), pb = Memory.alloc(0x40); _keep.push(argb, pb);
      const setupB = function () {
        for (let z = 0; z < 0x40; z += 4) { argb.add(z).writeU32(0); pb.add(z).writeU32(0); }
        argb.add(0x20).writeU32(ss.c >>> 0);
        argb.add(0x1c).writeU8(ss.flag & 0xff);
        argb.writePointer(pb);
        pb.add(0x40).writeU32(ss.f >>> 0);
      };
      try { setupB(); o = Orig(argb) >>> 0; } catch (e) { eo = e.message; }
      try { setupB(); r = Reim(argb) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'global_dll_insert_head') {
      // u32 fn(arg): insert node=arg+0x28 at head of the global list at glob; node[0xc]&=~1.
      // seed *glob=&S, arg[0x34]=0xF; shared arg+S bufs; snapshot the 5 writes + ret.
      const argg = Memory.alloc(0x80), S = Memory.alloc(0x40); _keep.push(argg, S);
      const setupG = function () {
        for (let z = 0; z < 0x80; z += 4) argg.add(z).writeU32(0);
        for (let z = 0; z < 0x40; z += 4) S.add(z).writeU32(0);
        ptr(cfg.glob).writePointer(S);
        argg.add(0x34).writeU32(0xF);
      };
      const snapG = function (rv) { return [argg.add(0x28).readU32(), argg.add(0x2c).readU32(), S.add(4).readU32(), ptr(cfg.glob).readU32(), argg.add(0x34).readU32(), rv >>> 0].map(function (x) { return x >>> 0; }).join('|'); };
      try { setupG(); const ro = Orig(argg) >>> 0; o = snapG(ro); } catch (e) { eo = e.message; }
      try { setupG(); const rr = Reim(argg) >>> 0; r = snapG(rr); } catch (e) { er = e.message; }
    } else if (cfg.at === 'global_fieldoff_clear') {
      // u32 fn(arg): V=*(glob); entry=*(arg+V); if(!entry) return 0; if(!entry[0]) return arg;
      // if(entry[4]) arg[0x48]=entry[4]; entry[4]=0; entry[0]=0; return arg. seed *glob=V,
      // arg[V]=&entry (test0) or 0 (test1); entry[0]=1, entry[4]=0x77. shared arg+entry bufs.
      const V = 0x10, argc = Memory.alloc(0x80), ent = Memory.alloc(0x40); _keep.push(argc, ent);
      const linked = (t | 0) === 0;
      const setupC = function () {
        for (let z = 0; z < 0x80; z += 4) argc.add(z).writeU32(0);
        for (let z = 0; z < 0x40; z += 4) ent.add(z).writeU32(0);
        ptr(cfg.glob).writeU32(V);
        if (linked) { argc.add(V).writePointer(ent); ent.writeU32(1); ent.add(4).writeU32(0x77); }
      };
      const snapC2 = function (rv) { return [argc.add(0x48).readU32(), ent.readU32(), ent.add(4).readU32(), rv >>> 0].map(function (x) { return x >>> 0; }).join('|'); };
      try { setupC(); const ro = Orig(argc) >>> 0; o = snapC2(ro); } catch (e) { eo = e.message; }
      try { setupC(); const rr = Reim(argc) >>> 0; r = snapC2(rr); } catch (e) { er = e.message; }
    } else if (cfg.at === 'multi_state_list_setter') {
      // void fn(p): 4-state dispatch (state=p[0x48]); state1 unlinks p[0x14]/p[0x18] + p[0x50]=3;
      // state2 p[0x50]=6; state3 p[0x50]=5; else no change. Seed per test state; shared p+A+B
      // bufs (so state1 list pointers compare equal); snapshot p[0x50]|p[0x14]|p[0x18]|A[4]|B[0].
      const st = t | 0;
      const ps = Memory.alloc(0x80), A = Memory.alloc(0x40), B = Memory.alloc(0x40); _keep.push(ps, A, B);
      const setupS = function () {
        for (let z = 0; z < 0x80; z += 4) ps.add(z).writeU32(0);
        for (let z = 0; z < 0x40; z += 4) { A.add(z).writeU32(0); B.add(z).writeU32(0); }
        ps.add(0x48).writeU32(st);
        ps.add(0x50).writeU32(0x11);
        if (st === 1) { ps.add(0x14).writePointer(A); ps.add(0x18).writePointer(B); }
      };
      const snapS = function () { return [ps.add(0x50).readU32(), ps.add(0x14).readU32(), ps.add(0x18).readU32(), A.add(4).readU32(), B.readU32()].map(function (x) { return x >>> 0; }).join('|'); };
      try { setupS(); Orig(ps); o = snapS(); } catch (e) { eo = e.message; }
      try { setupS(); Reim(ps); r = snapS(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'byte_counter_struct') {
      // void fn(p): a=p[0]+1; if(a>=p[3]) a-=p[3]; p[0]=a; p[1]=p[1]-1. Seed p[0],p[1],p[3]
      // per cfg.seed_sets[t]={b0,b1,b3}; observe p[0]|p[1]. non-degen via varied seeds + wrap.
      const sp = (cfg.seed_sets || [])[t | 0] || { b0: 0, b1: 0, b3: 0 };
      const pc = Memory.alloc(0x20); _keep.push(pc);
      const setupBC = function () { for (let z = 0; z < 0x20; z += 4) pc.add(z).writeU32(0); pc.writeU8(sp.b0 & 0xff); pc.add(1).writeU8(sp.b1 & 0xff); pc.add(3).writeU8(sp.b3 & 0xff); };
      const snapBC = function () { return (pc.readU8()) + '|' + (pc.add(1).readU8()); };
      try { setupBC(); Orig(pc); o = snapBC(); } catch (e) { eo = e.message; }
      try { setupBC(); Reim(pc); r = snapBC(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'arg_default_memcpy_abs') {
      // void fn(src): if(!src) src=glob(default); memcpy(tgt, src, copy_dwords*4). test0:
      // src=&buf(markers) -> dest=markers; test1: src=0 -> dest=copy of the default source.
      const dw = cfg.copy_dwords | 0, dest = ptr(cfg.tgt);
      const mbuf = Memory.alloc(dw * 4 + 0x20); _keep.push(mbuf);
      const usebuf = (t | 0) === 0;
      const setupM2 = function () {
        for (let k = 0; k < dw; k++) mbuf.add(k * 4).writeU32((0xC0DE0000 | k) >>> 0);
        for (let k = 0; k < dw; k++) dest.add(k * 4).writeU32(0xA5A5A5A5);
      };
      const snapM2 = function () { const a = []; for (let k = 0; k < dw; k++) a.push(dest.add(k * 4).readU32() >>> 0); return a.join('|'); };
      const src = usebuf ? mbuf : ptr(0);
      try { setupM2(); Orig(src); o = snapM2(); } catch (e) { eo = e.message; }
      try { setupM2(); Reim(src); r = snapM2(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'byte_idx_table_bitclear') {
      // void fn(p): off=p[1]+p[0] (mod p[3]); p[1]++; *(p[4]+off*0x14) &= ~8. Main path
      // (b1!=b3). Seed p + tbl, place 0xFF at tbl[off*0x14]; observe that slot (->0xF7) + p[1].
      const sp = (cfg.seed_sets || [])[t | 0] || { b0: 0, b1: 0, b3: 0 };
      const pbc = Memory.alloc(0x20), tbl = Memory.alloc(0x400); _keep.push(pbc, tbl);
      let off = sp.b1 + sp.b0; if (off >= sp.b3) off -= sp.b3;
      const tblOff = off * 0x14;
      const setupBT = function () {
        for (let z = 0; z < 0x20; z += 4) pbc.add(z).writeU32(0);
        for (let z = 0; z < 0x400; z += 4) tbl.add(z).writeU32(0);
        pbc.writeU8(sp.b0 & 0xff); pbc.add(1).writeU8(sp.b1 & 0xff); pbc.add(3).writeU8(sp.b3 & 0xff);
        pbc.add(4).writePointer(tbl);
        tbl.add(tblOff).writeU32(0xFF);
      };
      const snapBT = function () { return (tbl.add(tblOff).readU32() >>> 0) + '|' + (pbc.add(1).readU8()); };
      try { setupBT(); Orig(pbc); o = snapBT(); } catch (e) { eo = e.message; }
      try { setupBT(); Reim(pbc); r = snapBT(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'struct_table5_search') {
      // u32 fn(p1,p2): count=p1[0x1d0]; tbl=p1[0x1d4]; backward search 5-byte entries (dword
      // key + byte val) for *p2; return entry[4] or 0. seed count=4, distinct keys/vals; p2 key.
      const count = 4;
      const p1 = Memory.alloc(0x200), tbl = Memory.alloc(0x80), p2 = Memory.alloc(0x10); _keep.push(p1, tbl, p2);
      const match = (t | 0) === 0;
      const setupT5 = function () {
        for (let z = 0; z < 0x200; z += 4) p1.add(z).writeU32(0);
        for (let z = 0; z < 0x80; z += 4) tbl.add(z).writeU32(0);
        p1.add(0x1d0).writeU32(count);
        p1.add(0x1d4).writePointer(tbl);
        for (let i = 0; i < count; i++) { tbl.add(i * 5).writeU32((0x1000 + i) >>> 0); tbl.add(i * 5 + 4).writeU8(0x20 + i); }
        p2.writeU32(match ? 0x1002 : 0x9999);
      };
      try { setupT5(); o = Orig(p1, p2) >>> 0; } catch (e) { eo = e.message; }
      try { setupT5(); r = Reim(p1, p2) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'circular_list_search_node') {
      // u32 fn(list, key): walk circular list (sentinel=list, *list=first, node[0]=next);
      // if *(node-0x44)==key return node-0x4c else next; return 0. Build 3-object circular
      // list (node=obj+0x4c, key at obj+8). test0 key matches obj1, test1 no match.
      const N = 3, listb = Memory.alloc(0x10); _keep.push(listb);
      const objs = []; for (let i = 0; i < N; i++) { const ob = Memory.alloc(0x80); _keep.push(ob); objs.push(ob); }
      const nodes = objs.map(function (ob) { return ob.add(0x4c); });
      const matchC = (t | 0) === 0;
      const buildC = function () {
        for (let i = 0; i < N; i++) for (let z = 0; z < 0x80; z += 4) objs[i].add(z).writeU32(0);
        listb.writePointer(nodes[0]);
        for (let i = 0; i < N; i++) {
          nodes[i].writePointer(i < N - 1 ? nodes[i + 1] : listb);
          nodes[i].add(-0x44).writeU32((0x100 + i) >>> 0);
        }
      };
      const keyC = matchC ? 0x101 : 0x999;
      try { buildC(); o = Orig(listb, keyC >>> 0) >>> 0; } catch (e) { eo = e.message; }
      try { buildC(); r = Reim(listb, keyC >>> 0) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'global_fieldoff_set') {
      // u32 fn(arg): V=*(glob); entry=*(arg+V); if(!entry) return 0; if(entry[0]) return arg;
      // entry[4]=arg[0x48]; arg[0x48]=0x557b70; entry[0]=1; return arg. t0 set, t1 null, t2 early.
      const V = 0x10, args = Memory.alloc(0x80), ents = Memory.alloc(0x40); _keep.push(args, ents);
      const tc = t | 0;
      const setupFS = function () {
        for (let z = 0; z < 0x80; z += 4) args.add(z).writeU32(0);
        for (let z = 0; z < 0x40; z += 4) ents.add(z).writeU32(0);
        ptr(cfg.glob).writeU32(V);
        args.add(0x48).writeU32(0x66);
        if (tc !== 1) { args.add(V).writePointer(ents); if (tc === 2) ents.writeU32(5); }
      };
      const snapFS = function (rv) { return [ents.add(4).readU32(), args.add(0x48).readU32(), ents.readU32(), rv >>> 0].map(function (x) { return x >>> 0; }).join('|'); };
      try { setupFS(); const ro = Orig(args) >>> 0; o = snapFS(ro); } catch (e) { eo = e.message; }
      try { setupFS(); const rr = Reim(args) >>> 0; r = snapFS(rr); } catch (e) { er = e.message; }
    } else if (cfg.at === 'eax_dest_memcpy_init') {
      // void fn(EAX=dest, src, arg2*, arg3, arg4): struct init (16-dword copy from src +
      // scalar/const field sets + dest[0x40]=*arg2). ORIG via `mov eax,dest; jmp` trampoline
      // (4 stack args); REIMPL __cdecl(dest,src,arg2,arg3,arg4). Shared dest/src/a2 bufs.
      const dest = Memory.alloc(0x80), srcb = Memory.alloc(0x40), a2 = Memory.alloc(0x10); _keep.push(dest, srcb, a2);
      const ARG3 = 0x33330000, ARG4 = 0x44440000, A2V = 0x40400000;
      const setupE = function () {
        for (let z = 0; z < 0x80; z += 4) dest.add(z).writeU32(0xA5A5A5A5);
        for (let k = 0; k < 16; k++) srcb.add(k * 4).writeU32((0xC0DE0000 | k) >>> 0);
        a2.writeU32(A2V);
      };
      const mkTd = function (target) {
        const tr = Memory.alloc(Process.pageSize); _keep.push(tr);
        tr.writeU8(0xB8); tr.add(1).writePointer(dest);   // mov eax, dest
        tr.add(5).writeU8(0xE9); tr.add(6).writeS32(target.sub(tr.add(10)).toInt32());  // jmp
        Memory.protect(tr, 16, 'rwx');
        return new NativeFunction(tr, 'void', ['pointer', 'pointer', 'uint32', 'uint32'], 'mscdecl');
      };
      const snapE = function () { const a = []; for (let k = 0; k < 16; k++) a.push(dest.add(k * 4).readU32() >>> 0);[0x40, 0x48, 0x4c, 0x50, 0x54, 0x58].forEach(function (o2) { a.push(dest.add(o2).readU32() >>> 0); }); return a.join('|'); };
      try { setupE(); mkTd(ptr(cfg.rva))(srcb, a2, ARG3, ARG4); o = snapE(); } catch (e) { eo = e.message; }
      try { setupE(); Reim(dest, srcb, a2, ARG3, ARG4); r = snapE(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'struct_div_mod_compute') {
      // u32 fn(arg1,arg2,arg3,arg4,arg5): div from arg1[0x18] table; q=arg2/div; *arg5=arg2%div;
      // return arg1[0x10]-table base + arg1[0x20]*q + rem. arg3=1,arg4=2 fixed. seed per seed_sets.
      const sp = (cfg.seed_sets || [])[t | 0] || { val: 100, div: 7 };
      const arg3 = 1, arg4 = 2, MULT = 0x10, BASE = 0x1000;
      const a1 = Memory.alloc(0x40), t1 = Memory.alloc(0x200), t2 = Memory.alloc(0x200), a5 = Memory.alloc(0x10);
      _keep.push(a1, t1, t2, a5);
      const setupDM = function () {
        for (let z = 0; z < 0x40; z += 4) a1.add(z).writeU32(0);
        for (let z = 0; z < 0x200; z += 4) { t1.add(z).writeU32(0); t2.add(z).writeU32(0); }
        a1.add(0x18).writePointer(t1); a1.add(0x10).writePointer(t2); a1.add(0x20).writeU32(MULT);
        t1.add(arg4 * 0x28 + 0x20).writeU32(sp.div >>> 0);
        t2.add(arg3 * 0x20 + 0x1c).writeU32(BASE);
        a5.writeU32(0);
      };
      const rdDM = function (rv) { return (a5.readU32() >>> 0) + '|' + (rv >>> 0); };
      try { setupDM(); const ro = Orig(a1, sp.val >>> 0, arg3, arg4, a5) >>> 0; o = rdDM(ro); } catch (e) { eo = e.message; }
      try { setupDM(); const rr = Reim(a1, sp.val >>> 0, arg3, arg4, a5) >>> 0; r = rdDM(rr); } catch (e) { er = e.message; }
    } else if (cfg.at === 'ring_copy_5ab980') {
      // void fn(arg): ring-buffer copy from 0x7dce08 to arg[0x18]; advances arg[0x18], decrements
      // arg[0x14]. Seed g610/g614, ring markers, arg fields; snapshot dest + the two updated fields.
      const G610 = 0x7dd610, G614 = 0x7dd614, RING = 0x7dce08;
      const dest = Memory.alloc(0x40), argr = Memory.alloc(0x40); _keep.push(dest, argr);
      const setupRC = function () {
        for (let z = 0; z < 0x40; z += 4) { dest.add(z).writeU32(0xA5A5A5A5); argr.add(z).writeU32(0); }
        ptr(G610).writeU32(0x100); ptr(G614).writeU32(0x40);
        for (let k = 0; k < 8; k++) ptr(RING).add(k * 4).writeU32((0xC0DE0000 | k) >>> 0);
        argr.add(0xc).writeU32(0x100);
        argr.add(0x14).writeU32(0x20);
        argr.add(0x18).writePointer(dest);
      };
      const snapRC = function () { const a = []; for (let k = 0; k < 8; k++) a.push(dest.add(k * 4).readU32() >>> 0); a.push(argr.add(0x18).readU32() >>> 0); a.push(argr.add(0x14).readU32() >>> 0); return a.join('|'); };
      try { setupRC(); Orig(argr); o = snapRC(); } catch (e) { eo = e.message; }
      try { setupRC(); Reim(argr); r = snapRC(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'struct_init_3arg_sub') {
      // void fn(a, b, dest): struct init (dest fields incl float consts + a nested sub=dest[0x60]).
      // seed a[4], b, dest[0x60]=&sub; snapshot dest + sub fields.
      const A4 = 0x1234, B = 0xABCD;
      const ai = Memory.alloc(0x40), dst = Memory.alloc(0x80), sub = Memory.alloc(0x80); _keep.push(ai, dst, sub);
      const setupI3 = function () {
        for (let z = 0; z < 0x40; z += 4) ai.add(z).writeU32(0);
        for (let z = 0; z < 0x80; z += 4) { dst.add(z).writeU32(0xA5A5A5A5); sub.add(z).writeU32(0xA5A5A5A5); }
        ai.add(4).writeU32(A4);
        dst.add(0x60).writePointer(sub);
      };
      const snapI3 = function () {
        const dofs = [0, 4, 8, 0xc, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28];
        const a2 = dofs.map(function (x) { return dst.add(x).readU32() >>> 0; });
        [0x38, 0x3c, 0x40, 0x44, 0x48, 0x50].forEach(function (x) { a2.push(sub.add(x).readU32() >>> 0); });
        return a2.join('|');
      };
      try { setupI3(); Orig(ai, B, dst); o = snapI3(); } catch (e) { eo = e.message; }
      try { setupI3(); Reim(ai, B, dst); r = snapI3(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'flag_branch_struct_2way') {
      // void fn(p, arg2): branch on p[0x94][0x50]&8 -> sub-write vs s-derived compute. test0 set,
      // test1 clear. seed p[0x94]=&f, p[0x11c]=&sub, p[0x84]=&s, s[0x38]=0x40, s[0x39]=3.
      const p = Memory.alloc(0x140), f = Memory.alloc(0x60), sub = Memory.alloc(0x100), s = Memory.alloc(0x40); _keep.push(p, f, sub, s);
      const flagset = (t | 0) === 0, ARG2 = flagset ? 0x77 : 5;
      const setupFB = function () {
        for (let z = 0; z < 0x140; z += 4) p.add(z).writeU32(0);
        for (let z = 0; z < 0x60; z += 4) f.add(z).writeU32(0);
        for (let z = 0; z < 0x100; z += 4) sub.add(z).writeU32(0);
        for (let z = 0; z < 0x40; z += 4) s.add(z).writeU32(0);
        p.add(0x94).writePointer(f); p.add(0x11c).writePointer(sub); p.add(0x84).writePointer(s);
        f.add(0x50).writeU8(flagset ? 8 : 0);
        s.add(0x38).writeU8(0x40); s.add(0x39).writeU8(3);
      };
      const snapFB = function () { return [sub.add(0x88).readU32(), sub.add(0x8c).readU32(), p.add(0x8c).readU32(), p.add(0x90).readU32(), p.add(0x88).readU32(), p.add(0x28).readU32()].map(function (x) { return x >>> 0; }).join('|'); };
      try { setupFB(); Orig(p, ARG2); o = snapFB(); } catch (e) { eo = e.message; }
      try { setupFB(); Reim(p, ARG2); r = snapFB(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'abs_region_zeroer') {
      // void fn(): strided record-array zeroer that also writes record index (word) to [+0x1c].
      // Pure writer -> sentinel-fill only the observed offsets, call, snapshot, compare.
      const base = cfg.glob >>> 0, stride = 0x8c;
      const addrs = [base, base + 0x1c, base + stride + 0x1c, base + 5 * stride + 0x1c, base + 0x64, cfg.tgt >>> 0];
      const fillZ = function () { addrs.forEach(function (a) { ptr(a).writeU32(0xA5A5A5A5); }); };
      const snapZ = function () { return addrs.map(function (a) { return ptr(a).readU32() >>> 0; }).join('|'); };
      try { fillZ(); Orig(); o = snapZ(); } catch (e) { eo = e.message; }
      try { fillZ(); Reim(); r = snapZ(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'array_fill_2way') {
      // void fn(p, src): count=p[0xc]; fill arr1=p[0] with src vec3 + arr2=p[4] with {0,0,0}
      // per element (stride 12). seed count=3, arr ptrs, src markers; snapshot both arrays.
      const count = 3;
      const p = Memory.alloc(0x20), arr1 = Memory.alloc(0x80), arr2 = Memory.alloc(0x80), src = Memory.alloc(0x20);
      _keep.push(p, arr1, arr2, src);
      const setupA = function () {
        for (let z = 0; z < 0x20; z += 4) p.add(z).writeU32(0);
        for (let z = 0; z < 0x80; z += 4) { arr1.add(z).writeU32(0xA5A5A5A5); arr2.add(z).writeU32(0xA5A5A5A5); }
        p.add(0xc).writeU32(count);
        p.writePointer(arr1); p.add(4).writePointer(arr2);
        src.writeU32(0xC0DE0001); src.add(4).writeU32(0xC0DE0002); src.add(8).writeU32(0xC0DE0003);
      };
      const snapA = function () {
        const a = [];
        for (let i = 0; i < count; i++) for (let k = 0; k < 3; k++) a.push(arr1.add(i * 12 + k * 4).readU32() >>> 0);
        for (let i = 0; i < count; i++) for (let k = 0; k < 3; k++) a.push(arr2.add(i * 12 + k * 4).readU32() >>> 0);
        return a.join('|');
      };
      try { setupA(); Orig(p, src); o = snapA(); } catch (e) { eo = e.message; }
      try { setupA(); Reim(p, src); r = snapA(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'abs_table_state_setter') {
      // u32 fn(i, arg2): bounded abs-table state setter. tests cover arg2==0 / rec[0x20]==3 /
      // else / OOB branches. seed rec[0x20]=pre, rec[0x1c]=0xEE; snapshot rec[0x20]|rec[0x1c]|ret.
      const base = cfg.glob >>> 0, stride = 0x50;
      const specs = [{ i: 2, a2: 0, pre: 9 }, { i: 3, a2: 1, pre: 3 }, { i: 4, a2: 1, pre: 7 }, { i: 0x20, a2: 1, pre: 5 }];
      const sp = specs[t | 0];
      const rec = (base + sp.i * stride) >>> 0;
      const setupTS = function () { ptr(rec).add(0x20).writeU32(sp.pre); ptr(rec).add(0x1c).writeU32(0xEE); };
      const snapTS = function (rv) { return (ptr(rec).add(0x20).readU32() >>> 0) + '|' + (ptr(rec).add(0x1c).readU32() >>> 0) + '|' + (rv >>> 0); };
      try { setupTS(); const ro = Orig(sp.i >>> 0, sp.a2 >>> 0) >>> 0; o = snapTS(ro); } catch (e) { eo = e.message; }
      try { setupTS(); const rr = Reim(sp.i >>> 0, sp.a2 >>> 0) >>> 0; r = snapTS(rr); } catch (e) { er = e.message; }
    } else if (cfg.at === 'esi_edx_predicate') {
      // u32 fn(ESI=s, EDX=e): field-match predicate. ORIG via `mov esi,s; mov edx,e; jmp`
      // trampoline; REIMPL __cdecl(s,e). seed s/e fields for match (t0->1) or no-match (t1->0).
      const bufS = Memory.alloc(0x40), bufE = Memory.alloc(0x40); _keep.push(bufS, bufE);
      const match = (t | 0) === 0;
      const setupEP = function () {
        for (let z = 0; z < 0x40; z += 4) { bufS.add(z).writeU32(0); bufE.add(z).writeU32(0); }
        bufS.add(0x10).writeU32(0x111); bufS.add(0x14).writeU32(0x222);
        if (match) { bufE.add(0x10).writeU32(0x111); bufE.add(0x14).writeU32(0x222); }
        else { bufE.add(0x10).writeU32(0x999); bufE.add(0x14).writeU32(0x888); }
      };
      const mkEP = function (target) {
        const tr = Memory.alloc(Process.pageSize); _keep.push(tr);
        tr.writeU8(0xBE); tr.add(1).writePointer(bufS);             // mov esi, bufS
        tr.add(5).writeU8(0xBA); tr.add(6).writePointer(bufE);      // mov edx, bufE
        tr.add(10).writeU8(0xE9); tr.add(11).writeS32(target.sub(tr.add(15)).toInt32()); // jmp
        Memory.protect(tr, 16, 'rwx');
        return new NativeFunction(tr, 'uint32', [], 'mscdecl');
      };
      try { setupEP(); o = mkEP(ptr(cfg.rva))() >>> 0; } catch (e) { eo = e.message; }
      try { setupEP(); r = Reim(bufS, bufE) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'edx_ebx_edi_find') {
      // u32 fn(EDX=arr, EBX=key, EDI=n): find (n+1)-th key in arr, return following element.
      // ORIG via a call-trampoline that saves/restores callee-saved ebx,edi. REIMPL __cdecl.
      const arr = Memory.alloc(0x80); _keep.push(arr);
      const KEY = 0x1234, TERM = 0xff070000;
      const buildArr = function () {
        arr.writeU32(KEY); arr.add(4).writeU32(0xAAAA);
        arr.add(8).writeU32(0x9999); arr.add(0xc).writeU32(0x8888);
        arr.add(0x10).writeU32(KEY); arr.add(0x14).writeU32(0xBBBB);
        arr.add(0x18).writeU32(TERM);
      };
      const nval = t | 0;
      const mkF = function (target, n) {
        const tr = Memory.alloc(Process.pageSize); _keep.push(tr);
        let p = 0;
        tr.add(p).writeU8(0x57); p += 1;                                  // push edi
        tr.add(p).writeU8(0x53); p += 1;                                  // push ebx
        tr.add(p).writeU8(0xBA); tr.add(p + 1).writePointer(arr); p += 5; // mov edx, arr
        tr.add(p).writeU8(0xBB); tr.add(p + 1).writeU32(KEY); p += 5;     // mov ebx, key
        tr.add(p).writeU8(0xBF); tr.add(p + 1).writeU32(n >>> 0); p += 5; // mov edi, n
        tr.add(p).writeU8(0xE8); tr.add(p + 1).writeS32(target.sub(tr.add(p + 5)).toInt32()); p += 5; // call target
        tr.add(p).writeU8(0x5B); p += 1;                                  // pop ebx
        tr.add(p).writeU8(0x5F); p += 1;                                  // pop edi
        tr.add(p).writeU8(0xC3); p += 1;                                  // ret
        Memory.protect(tr, 32, 'rwx');
        return new NativeFunction(tr, 'uint32', [], 'mscdecl');
      };
      try { buildArr(); o = mkF(ptr(cfg.rva), nval)() >>> 0; } catch (e) { eo = e.message; }
      try { buildArr(); r = Reim(arr, KEY >>> 0, nval >>> 0) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'ebx_edi_global_find') {
      // u32 fn(EBX=key, EDI=n): idx=*(glob); arr=*(tgt+idx*0x40); same walk. seed idx=0, arr ptr,
      // build arr. ORIG via call-trampoline (ebx,edi only). REIMPL __cdecl(key,n).
      const arrg = Memory.alloc(0x80); _keep.push(arrg);
      const KEY = 0x1234, TERM = 0xff070000;
      const buildG = function () {
        arrg.writeU32(KEY); arrg.add(4).writeU32(0xAAAA); arrg.add(8).writeU32(0x9999); arrg.add(0xc).writeU32(0x8888);
        arrg.add(0x10).writeU32(KEY); arrg.add(0x14).writeU32(0xBBBB); arrg.add(0x18).writeU32(TERM);
        ptr(cfg.glob).writeU32(0);
        ptr(cfg.tgt).writePointer(arrg);
      };
      const nval2 = t | 0;
      const mkG = function (target, n) {
        const tr = Memory.alloc(Process.pageSize); _keep.push(tr);
        let p = 0;
        tr.add(p).writeU8(0x57); p += 1; tr.add(p).writeU8(0x53); p += 1;          // push edi, ebx
        tr.add(p).writeU8(0xBB); tr.add(p + 1).writeU32(KEY); p += 5;              // mov ebx, key
        tr.add(p).writeU8(0xBF); tr.add(p + 1).writeU32(n >>> 0); p += 5;          // mov edi, n
        tr.add(p).writeU8(0xE8); tr.add(p + 1).writeS32(target.sub(tr.add(p + 5)).toInt32()); p += 5; // call
        tr.add(p).writeU8(0x5B); p += 1; tr.add(p).writeU8(0x5F); p += 1; tr.add(p).writeU8(0xC3); p += 1; // pop ebx,edi,ret
        Memory.protect(tr, 32, 'rwx');
        return new NativeFunction(tr, 'uint32', [], 'mscdecl');
      };
      try { buildG(); o = mkG(ptr(cfg.rva), nval2)() >>> 0; } catch (e) { eo = e.message; }
      try { buildG(); r = Reim(KEY >>> 0, nval2 >>> 0) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'strided_color_fill') {
      // void fn(): fills a strided buffer (base=*0x771530+0x1d, 896 entries, stride 0x20) with a
      // BGRA-swizzled global color. seed base ptr + 4 color bytes; observe entries 0,1,895.
      const C0 = 0x771530, COL = 0x616030;
      const buf = Memory.alloc(0x7000); _keep.push(buf);
      const setupCF = function () {
        for (let z = 0; z < 0x7000; z += 4) buf.add(z).writeU32(0xA5A5A5A5);
        ptr(C0).writePointer(buf);
        ptr(COL).writeU8(0x11); ptr(COL + 1).writeU8(0x22); ptr(COL + 2).writeU8(0x33); ptr(COL + 3).writeU8(0x44);
      };
      const ent = function (i) { const a = 0x1d + i * 0x20; return [buf.add(a - 1).readU8(), buf.add(a).readU8(), buf.add(a + 1).readU8(), buf.add(a + 2).readU8()].join(','); };
      const snapCF = function () { return ent(0) + '|' + ent(1) + '|' + ent(895); };
      try { setupCF(); Orig(); o = snapCF(); } catch (e) { eo = e.message; }
      try { setupCF(); Reim(); r = snapCF(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'bitmap_alloc_slot') {
      // u32 fn(): allocate first free bit in bitmap 0x6bf198; init rec=0x693198+idx*0x2c0; set
      // bit; return idx+1. seed bitmap so first-clear bit = K; snapshot rec fields + bitmap byte + ret.
      const BMP = 0x6bf198, REC = 0x693198, Ks = [5, 0], K = Ks[t | 0];
      const setupBA = function () {
        for (let b = 0; b < 32; b++) ptr(BMP).add(b).writeU8(0xFF);
        ptr(BMP).add(K >> 3).writeU8(((1 << (K & 7)) - 1) & 0xff);  // first clear = K
        const rec = REC + K * 0x2c0;
        [0x2b0, 0x2b4, 0x2b8, 0x2bc].forEach(function (o2) { ptr(rec).add(o2).writeU32(0xEE); });
      };
      const snapBA = function (rv) {
        const rec = REC + K * 0x2c0;
        return [ptr(rec).add(0x2b0).readU32() >>> 0, ptr(rec).add(0x2b4).readU32() >>> 0, ptr(rec).add(0x2b8).readU32() >>> 0, ptr(rec).add(0x2bc).readU32() >>> 0, ptr(BMP).add(K >> 3).readU8(), rv >>> 0].join('|');
      };
      try { setupBA(); const ro = Orig() >>> 0; o = snapBA(ro); } catch (e) { eo = e.message; }
      try { setupBA(); const rr = Reim() >>> 0; r = snapBA(rr); } catch (e) { er = e.message; }
    } else if (cfg.at === 'state_list_insert') {
      // void fn(p, _, state_src): state dispatch on sub=p[0x20] then intrusive-list insert of
      // node=p[0x24]+0xc into the &p[0x14] list (empty-list path: p[0x14]=0). test state 1/3.
      const p = Memory.alloc(0x40), sub = Memory.alloc(0x40), srcnode = Memory.alloc(0x40), nx = Memory.alloc(0x40), st = Memory.alloc(0x10);
      _keep.push(p, sub, srcnode, nx, st);
      const state = (t | 0) === 0 ? 1 : 3;
      const setupSL = function () {
        [p, sub, srcnode, nx].forEach(function (b) { for (let z = 0; z < 0x40; z += 4) b.add(z).writeU32(0); });
        p.add(0x20).writePointer(sub);
        p.add(0x24).writePointer(srcnode);
        p.add(0x14).writeU32(0);
        sub.add(0x28).writeU32(9);
        srcnode.add(0xc).writePointer(nx);
        st.writeU32(state);
      };
      const snapSL = function () { return [sub.add(0x20).readU32(), sub.add(0x28).readU32(), p.add(0x14).readU32(), p.add(0x18).readU32(), srcnode.add(0xc).readU32(), nx.add(4).readU32()].map(function (x) { return x >>> 0; }).join('|'); };
      try { setupSL(); Orig(p, 0, st); o = snapSL(); } catch (e) { eo = e.message; }
      try { setupSL(); Reim(p, 0, st); r = snapSL(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'multi_deref_global_set') {
      // void fn(p1, p2): multi-deref global setter (g=0 seeded). Nested chain p1[4]=&E,
      // E[0]=&obj, E[0x18]=&X, X[0x20]=&Ncell, Ncell=&N(or 0). t0 p2=&val n!=0; t1 p2=0/n=0.
      const G = 0x7dc57c, DEF = 0x613290;
      const p1 = Memory.alloc(0x40), E = Memory.alloc(0x40), obj = Memory.alloc(0x100), X = Memory.alloc(0x40), Ncell = Memory.alloc(0x40), N = Memory.alloc(0x40), valbuf = Memory.alloc(0x40);
      _keep.push(p1, E, obj, X, Ncell, N, valbuf);
      const useN = (t | 0) === 0;
      const setupMD = function () {
        [p1, E, X, Ncell, N, valbuf].forEach(function (b) { for (let z = 0; z < 0x40; z += 4) b.add(z).writeU32(0); });
        for (let z = 0; z < 0x100; z += 4) obj.add(z).writeU32(0xA5A5A5A5);
        ptr(G).writeU32(0);
        p1.add(4).writePointer(E);
        E.writePointer(obj);
        E.add(0x18).writePointer(X);
        X.add(0x20).writePointer(Ncell);
        Ncell.writePointer(useN ? N : ptr(0));
        obj.add(0x40).writeU32(0x100);
        if (useN) valbuf.writeU32(0x1234); else ptr(DEF).writeU32(0x5678);
      };
      const p2 = useN ? valbuf : ptr(0);
      const snapMD = function () { return [obj.add(0xc4).readU32() >>> 0, N.add(4).readU32() >>> 0, obj.add(0x40).readU32() >>> 0].join('|'); };
      try { setupMD(); Orig(p1, p2); o = snapMD(); } catch (e) { eo = e.message; }
      try { setupMD(); Reim(p1, p2); r = snapMD(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'list_node_const_init') {
      // void fn(p, arg2): writes *arg2 + float consts (1.0,1.0,0.5) into each of count nodes
      // (arr=s[0x20] array of ptrs, count=s[0x24], s=p[0x18]). All stores (C reimpl bit-identical).
      const count = 3, VAL = 0xCAFE;
      const p = Memory.alloc(0x40), s = Memory.alloc(0x40), arr = Memory.alloc(0x40), valbuf = Memory.alloc(0x40);
      const nodes = []; for (let i = 0; i < count; i++) { const nb = Memory.alloc(0x40); _keep.push(nb); nodes.push(nb); }
      _keep.push(p, s, arr, valbuf);
      const setupLN = function () {
        [p, s, arr, valbuf].forEach(function (b) { for (let z = 0; z < 0x40; z += 4) b.add(z).writeU32(0); });
        nodes.forEach(function (nb) { for (let z = 0; z < 0x40; z += 4) nb.add(z).writeU32(0xA5A5A5A5); });
        p.add(0x18).writePointer(s);
        s.add(0x24).writeU32(count);
        s.add(0x20).writePointer(arr);
        nodes.forEach(function (nb, i) { arr.add(i * 4).writePointer(nb); });
        valbuf.writeU32(VAL);
      };
      const snapLN = function () { const a = []; nodes.forEach(function (nb) {[4, 0xc, 0x10, 0x14].forEach(function (o2) { a.push(nb.add(o2).readU32() >>> 0); }); }); return a.join('|'); };
      try { setupLN(); Orig(p, valbuf); o = snapLN(); } catch (e) { eo = e.message; }
      try { setupLN(); Reim(p, valbuf); r = snapLN(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'bounded_struct_push') {
      // void fn(p, arg2, arg3): bounded SoA push (stride 0x30): rec=buf+top*0x30+4 <- arg2 vec3;
      // buf[off+0..3] <- arg3; buf[off+0x1c]=0; top++; p[0x54]=1. t0 push, t1 full.
      const p = Memory.alloc(0x80), buf = Memory.alloc(0x200), arg2 = Memory.alloc(0x40), arg3 = Memory.alloc(0x40);
      _keep.push(p, buf, arg2, arg3);
      const top = (t | 0) === 1 ? 4 : 0;
      const setupBP = function () {
        for (let z = 0; z < 0x80; z += 4) p.add(z).writeU32(0);
        for (let z = 0; z < 0x200; z += 4) buf.add(z).writeU32(0xA5A5A5A5);
        for (let z = 0; z < 0x40; z += 4) { arg2.add(z).writeU32(0); arg3.add(z).writeU32(0); }
        p.writePointer(buf); p.add(4).writeU32(4); p.add(8).writeU32(top); p.add(0x54).writeU32(0xEE);
        arg2.writeU32(0xC0DE0001); arg2.add(4).writeU32(0xC0DE0002); arg2.add(8).writeU32(0xC0DE0003);
        arg3.writeU32(0x44332211);
      };
      const snapBP = function () { const off = top * 0x30; return [buf.add(off).readU32(), buf.add(off + 4).readU32(), buf.add(off + 8).readU32(), buf.add(off + 0xc).readU32(), buf.add(off + 0x1c).readU32(), p.add(8).readU32(), p.add(0x54).readU32()].map(function (x) { return x >>> 0; }).join('|'); };
      try { setupBP(); Orig(p, arg2, arg3); o = snapBP(); } catch (e) { eo = e.message; }
      try { setupBP(); Reim(p, arg2, arg3); r = snapBP(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'trie_walk') {
      // u32 fn(node, key, depth): descend a trie (idx=key&0xf per level, node[idx*4+0x1c]);
      // return (node&~0xff)|node[0x18]. build depth-2 trie w/ 2 keyed paths.
      const root = Memory.alloc(0x80), mid1 = Memory.alloc(0x80), mid2 = Memory.alloc(0x80), leaf1 = Memory.alloc(0x80), leaf2 = Memory.alloc(0x80);
      _keep.push(root, mid1, mid2, leaf1, leaf2);
      const keys = [0x21, 0x35], key = keys[t | 0], depth = 2;
      const buildT = function () {
        [root, mid1, mid2, leaf1, leaf2].forEach(function (b) { for (let z = 0; z < 0x80; z += 4) b.add(z).writeU32(0); });
        root.add(1 * 4 + 0x1c).writePointer(mid1); mid1.add(2 * 4 + 0x1c).writePointer(leaf1); leaf1.add(0x18).writeU8(0x77);
        root.add(5 * 4 + 0x1c).writePointer(mid2); mid2.add(3 * 4 + 0x1c).writePointer(leaf2); leaf2.add(0x18).writeU8(0x88);
      };
      try { buildT(); o = Orig(root, key >>> 0, depth) >>> 0; } catch (e) { eo = e.message; }
      try { buildT(); r = Reim(root, key >>> 0, depth) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'struct_delta_flag_init') {
      // u32 fn(out,a,b,c,d): copies + single-fsub deltas + an fcomp-driven flag. seed a/b/c/d
      // xy floats; snapshot out fields + flag + ret. reimpl is verbatim naked __asm.
      const out = Memory.alloc(0x80), a = Memory.alloc(0x20), b = Memory.alloc(0x20), c = Memory.alloc(0x20), d = Memory.alloc(0x20);
      _keep.push(out, a, b, c, d);
      const setupDF = function () {
        for (let z = 0; z < 0x80; z += 4) out.add(z).writeU32(0xA5A5A5A5);
        a.writeFloat(0.5); a.add(4).writeFloat(0.25);
        b.writeFloat(3.0); b.add(4).writeFloat(4.0);
        c.writeFloat(5.0); c.add(4).writeFloat(7.0);
        d.writeFloat(1.0); d.add(4).writeFloat(2.0);
        out.add(0x64).writeU32(0);
      };
      const snapDF = function (rv) { return [0x10, 0x14, 0x28, 0x2c, 0x40, 0x44, 0x58, 0x5c, 0x64].map(function (o2) { return out.add(o2).readU32() >>> 0; }).concat([rv >>> 0]).join('|'); };
      try { setupDF(); const ro = Orig(out, a, b, c, d) >>> 0; o = snapDF(ro); } catch (e) { eo = e.message; }
      try { setupDF(); const rr = Reim(out, a, b, c, d) >>> 0; r = snapDF(rr); } catch (e) { er = e.message; }
    } else if (cfg.at === 'near_leaf_list_search') {
      // u32 f(key): C3 list-search(cfg.glob, key) -> node[0] or -1. seed_sets[t]={empty,query,nodes}.
      const sp = (cfg.seed_sets || [])[t | 0] || { empty: true, query: 0, nodes: [] };
      const struc = ptr(cfg.glob), nds = sp.nodes || [], N = nds.length;
      const nodes = []; for (let k = 0; k < N; k++) { const b = Memory.alloc(0x40); _keep.push(b); nodes.push(b); }
      const seedL = function () {
        if (sp.empty || N === 0) { struc.add(0x10).writePointer(ptr(0)); return; }
        nds.forEach(function (nd, k) {
          nodes[k].writeU32(nd[1] >>> 0); nodes[k].add(8).writeU32(nd[0] >>> 0);
          nodes[k].add(0x30).writePointer(k < N - 1 ? nodes[k + 1] : ptr(0));
        });
        struc.add(0x10).writePointer(nodes[0]);
      };
      try { seedL(); o = Orig(sp.query >>> 0) >>> 0; } catch (e) { eo = e.message; }
      try { seedL(); r = Reim(sp.query >>> 0) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'near_leaf_memset2') {
      // void f(dest, count): NEAR-LEAF zero-fill via C3 memset callee.
      // Pre-fill a 0x80 buffer with 0xCC, call f(dest, count), snapshot a fixed
      // 0x20 window. First `count` bytes -> 0x00, the rest stay 0xCC -> the
      // boundary varies per count (NON-DEGENERATE) and proves a bounded fill.
      const sp = (cfg.seed_sets || [])[t | 0] || { count: 0 };
      const dst = Memory.alloc(0x80); _keep.push(dst);
      const seedM = function () { for (let z = 0; z < 0x80; z++) dst.add(z).writeU8(0xCC); };
      const snap = function () {
        const u = new Uint8Array(dst.readByteArray(0x20)); let s = '';
        for (let k = 0; k < u.length; k++) s += ('0' + u[k].toString(16)).slice(-2);
        return s;
      };
      try { seedM(); Orig(dst, sp.count >>> 0); o = snap(); } catch (e) { eo = e.message; }
      try { seedM(); Reim(dst, sp.count >>> 0); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'struct_list_float_set') {
      // void f(struct*, float vol): struct+0x38=vol; walk circular list at struct+0xc
      // (sentinel=struct+0xc), each node sets node+0x14|=0x40; if struct+0x11c!=0,
      // secondary+0x30 = vol raw bits. Build a 1-node self-circular list + secondary.
      const S = Memory.alloc(0x140), N = Memory.alloc(0x40), SEC = Memory.alloc(0x40);
      _keep.push(S, N, SEC);
      const vol = 0.5 + (t >>> 0) * 0.25;
      const seedS = function () {
        for (let z = 0; z < 0x140; z += 4) S.add(z).writeU32(0);
        for (let z = 0; z < 0x40; z += 4) { N.add(z).writeU32(0); SEC.add(z).writeU32(0); }
        N.add(0).writePointer(S.add(0x0c));                         // node->next = sentinel (1 iter)
        N.add(0x14).writeU32((0xA0000 | ((t >>> 0) << 8)) >>> 0);    // flags seed (varies per t)
        N.add(0x1c).writeU32(0);                                    // no-op FLD/FSTP source (0.0f)
        S.add(0x0c).writePointer(N);                               // list head
        S.add(0x11c).writePointer(SEC);                           // secondary present
      };
      const snap = function () {
        return (S.add(0x38).readU32() >>> 0).toString(16) + '|' +
               (N.add(0x14).readU32() >>> 0).toString(16) + '|' +
               (SEC.add(0x30).readU32() >>> 0).toString(16);
      };
      try { seedS(); Orig(S, vol); o = snap(); } catch (e) { eo = e.message; }
      try { seedS(); Reim(S, vol); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'seed_indirect_ctx_obs') {
      // u32 f(void): ctx = ptr_array[depth_global]; f writes fixed values into
      // ctx[offsets] (+ OR a flags field) and zeros direct globals. Seed the
      // INDIRECT ctx pointer to a fresh buffer (prior AV was a null deref).
      const ctxbuf = Memory.alloc(0x80); _keep.push(ctxbuf);
      const idx = cfg.depth_idx | 0;
      const seedFld = cfg.ctx_seed_off;
      const seedVal = (0xC0DE0000 | ((t >>> 0) << 8)) >>> 0;
      const seedC = function () {
        for (let z = 0; z < 0x80; z += 4) ctxbuf.add(z).writeU32(0xEEEEEEEE);
        if (seedFld !== undefined && seedFld !== null) ctxbuf.add(seedFld | 0).writeU32(seedVal);
        ptr(cfg.ptr_array).add(idx * 4).writePointer(ctxbuf);
        ptr(cfg.depth_global).writeU32(idx);
        (cfg.seed_globals || []).forEach(function (g) { ptr(g[0]).writeU32(g[1] >>> 0); });
      };
      const snap = function () {
        let s = (cfg.observe_offs || []).map(function (o) { return (ctxbuf.add(o | 0).readU32() >>> 0).toString(16); }).join('|');
        if (cfg.observe_globals) s += '#' + cfg.observe_globals.map(function (a) { return (ptr(a).readU32() >>> 0).toString(16); }).join('|');
        return s;
      };
      try { seedC(); o = (Orig() >>> 0).toString(16) + ':' + snap(); } catch (e) { eo = e.message; }
      try { seedC(); r = (Reim() >>> 0).toString(16) + ':' + snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'indexed_float_sum2') {
      // float f(int idx): p=(float*)(tgt + idx*stride); return p[0]+p[1]. Seed two
      // distinct floats per idx; ret 'float' -> compare JS numbers (exact here).
      const base = ptr(cfg.tgt), stride = cfg.stride | 0, idx = t >>> 0;
      const seedI = function () {
        const slot = base.add(idx * stride);
        slot.writeFloat(idx + 1.0);
        slot.add(4).writeFloat(idx * 0.5);
      };
      try { seedI(); o = Orig(idx); } catch (e) { eo = e.message; }
      try { seedI(); r = Reim(idx); } catch (e) { er = e.message; }
    } else if (cfg.at === 'double_indexed_float_mul') {
      // float f(int idx): c=*(int*)(idx*S+aTbl); d=*(int*)(idx*S+bTbl); e=d+c*4;
      // return *(float*)(fTbl+e*4) * *(float*)K. Seed idx=0 path: aTbl=0, bTbl=e=t,
      // fTbl+t*4=float(t+1). ret 'float' -> compare JS numbers.
      const e = t >>> 0;
      const seedD = function () {
        ptr(cfg.aTbl).writeU32(0);
        ptr(cfg.bTbl).writeU32(e);
        ptr(cfg.fTbl).add(e * 4).writeFloat(e + 1.0);
      };
      try { seedD(); o = Orig(0); } catch (ex) { eo = ex.message; }
      try { seedD(); r = Reim(0); } catch (ex) { er = ex.message; }
    } else if (cfg.at === 'struct_tag_equals') {
      // int f(a,b): tagged-union dword equality. Two 0x80 bufs filled identical,
      // tag at [0], optional one-field perturbation in b per scenario.
      const scen = (cfg.scenarios || [])[t] || { tag: 0, diff: -1 };
      const A = Memory.alloc(0x80), B = Memory.alloc(0x80); _keep.push(A, B);
      const setup = function () {
        for (let z = 0; z < 0x80; z += 4) { A.add(z).writeU32((z + 0x100) >>> 0); B.add(z).writeU32((z + 0x100) >>> 0); }
        A.writeU32(scen.tag >>> 0); B.writeU32(scen.tag >>> 0);
        if (scen.diff >= 0) B.add(scen.diff | 0).writeU32(0xDEADBEEF);
      };
      try { setup(); o = Orig(A, B) >>> 0; } catch (e) { eo = e.message; }
      try { setup(); r = Reim(A, B) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'indexed_float_accum16') {
      // int f(float* out, uint i, uint j): bounds-checked 16-float sum at
      // tbl_base + i*iStride + j*jStride + regionOff (start = region - 4).
      const scen = (cfg.scenarios || [])[t] || { i: 0, j: 0, fill: 1 };
      const out = Memory.alloc(4); _keep.push(out);
      const inb = (scen.i >>> 0) < 0x10 && (scen.j >>> 0) < 4;
      const region = ptr(cfg.tbl_base).add((scen.i >>> 0) * (cfg.iStride | 0))
                                      .add((scen.j >>> 0) * (cfg.jStride | 0))
                                      .add(cfg.regionOff | 0);
      const seedA = function () {
        out.writeU32(0x7f7f7f7f);
        if (inb) for (let k = 0; k < 16; k++) region.add(-4 + k * 4).writeFloat((scen.fill | 0) + k);
      };
      const snap = function (ret) { return (ret >>> 0) + ':' + (out.readU32() >>> 0).toString(16); };
      try { seedA(); o = snap(Orig(out, scen.i >>> 0, scen.j >>> 0)); } catch (e) { eo = e.message; }
      try { seedA(); r = snap(Reim(out, scen.i >>> 0, scen.j >>> 0)); } catch (e) { er = e.message; }
    } else if (cfg.at === 'bounded_table_signselect_clamp') {
      // int f(uint idx, int val): bounded table update; sign selected by a byte vs C.
      const s = (cfg.scenarios || [])[t] || { idx: 0, val: 0, byte: 0, slot: 0 };
      const idx = s.idx >>> 0;
      const inb = idx < 0x10;
      const t3 = ptr(cfg.t3Tbl).add(idx * (cfg.t3Stride | 0));
      const seedX = function () {
        if (inb) {
          ptr(cfg.t1Tbl).add(idx * 16).writeU32(0);
          ptr(cfg.t2Tbl).writeU8(s.byte & 0xff);
          t3.writeS32(s.slot | 0);
        }
      };
      const snap = function (ret) { return (ret >>> 0) + ':' + (inb ? (t3.readS32() | 0) : 'na'); };
      try { seedX(); o = snap(Orig(idx, s.val >>> 0)); } catch (e) { eo = e.message; }
      try { seedX(); r = snap(Reim(idx, s.val >>> 0)); } catch (e) { er = e.message; }
    } else if (cfg.at === 'seed_globals_arg_multiobs') {
      // void f(int arg): seed input globals, call(arg), observe a list of globals.
      const sp = (cfg.seed_sets || [])[t | 0] || { arg: 0, globals: [] };
      const obs = cfg.observe_addrs || [];
      const seedG = function () { (sp.globals || []).forEach(function (g) { ptr(g[0]).writeU32(g[1] >>> 0); }); };
      const snap = function () { return obs.map(function (a) { return (ptr(a).readU32() >>> 0).toString(16); }).join('|'); };
      try { seedG(); Orig(sp.arg >>> 0); o = snap(); } catch (e) { eo = e.message; }
      try { seedG(); Reim(sp.arg >>> 0); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'succ_approx_quantize') {
      // void f(int arg1, int* p2, int* p3): successive-approx quantizer.
      const s = (cfg.scenarios || [])[t] || { arg1: 0, cur: 0, idx: 0, range: 0x400 };
      const p2 = Memory.alloc(4), p3 = Memory.alloc(4); _keep.push(p2, p3);
      const seedQ = function () {
        p2.writeS32(s.cur | 0);
        p3.writeS32(s.idx | 0);
        ptr(cfg.rangeTbl).add((s.idx | 0) * 2).writeS16(s.range | 0);
        // deltaTbl (0x634478) left at REAL values: both sides read it identically
        // (seeding it earlier diverged Orig vs Reim - harness artifact).
      };
      const snap = function () { return (p2.readS32() | 0) + '|' + (p3.readS32() | 0); };
      try { seedQ(); Orig(s.arg1 >>> 0, p2, p3); o = snap(); } catch (e) { eo = e.message; }
      try { seedQ(); Reim(s.arg1 >>> 0, p2, p3); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'multi_array_scatter') {
      // void f(struct* p): scatter REAL source globals into 7 arrays indexed by counter.
      const s = (cfg.scenarios || [])[t] || { counter: 0, bound: 4 };
      const cnt = s.counter | 0;
      const st = Memory.alloc(0x40); _keep.push(st);
      const strides = [12, 8, 4, 64, 4, 16, 32];   // for fields 0x10..0x28
      const bufs = [];
      for (let k = 0; k < 7; k++) { const b = Memory.alloc(0x400); _keep.push(b); bufs.push(b); }
      // seed the REAL source globals with distinct values -> proves source addresses
      // (runtime values are 0, so without this a wrong source addr would pass).
      const srcSeed = [
        [0x692528,0x11110000],[0x69252c,0x22220000],[0x692530,0x33330000],
        [0x6924dc,0x44440000],[0x6924e0,0x55550000],[0x692554,0x66660000],
        [0x6924e8,0x77770000],[0x6924d8,0x40000000],
        [0x692598,0x88880000],[0x692534,0x99990000],
      ];
      const seedM = function () {
        for (let z = 0; z < 0x40; z += 4) st.add(z).writeU32(0);
        st.add(8).writeU32(s.bound >>> 0);
        st.add(0xc).writeU32(cnt >>> 0);
        for (let k = 0; k < 7; k++) {
          for (let z = 0; z < 0x400; z += 4) bufs[k].add(z).writeU32(0xEEEEEEEE);
          st.add(0x10 + k * 4).writePointer(bufs[k]);
        }
        srcSeed.forEach(function (g) { ptr(g[0]).writeU32(g[1] >>> 0); });
      };
      const snap = function () {
        let parts = [];
        for (let k = 0; k < 7; k++) parts.push((bufs[k].add(cnt * strides[k]).readU32() >>> 0).toString(16));
        parts.push((st.add(0xc).readU32() >>> 0).toString(16));
        return parts.join('|');
      };
      try { seedM(); Orig(st); o = snap(); } catch (e) { eo = e.message; }
      try { seedM(); Reim(st); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'dll_head_insert') {
      // void f(struct* p): intrusive DLL head-insert gated on node[3]&3.
      const s = (cfg.scenarios || [])[t] || { flag: 0 };
      const P = Memory.alloc(0x100), node = Memory.alloc(0x40), G = Memory.alloc(0x100), H = Memory.alloc(0x40);
      _keep.push(P, node, G, H);
      const seedD = function () {
        for (let z = 0; z < 0x100; z += 4) P.add(z).writeU32(0);
        for (let z = 0; z < 0x40; z += 4) { node.add(z).writeU32(0xEEEEEEEE); H.add(z).writeU32(0xEEEEEEEE); }
        for (let z = 0; z < 0x100; z += 4) G.add(z).writeU32(0xEEEEEEEE);
        node.add(3).writeU8(s.flag & 0xff);   // byte 3 (clears the 0xEE in that byte)
        P.add(3).writeU8(0);
        P.add(0xa0).writePointer(node);
        G.add(0xbc).writePointer(H);           // old head = H
        ptr(cfg.glob).writePointer(G);         // *0x7d3ff8 = G
      };
      const snap = function () {
        return [node.add(8).readU32()>>>0, node.add(0xc).readU32()>>>0, node.add(3).readU8(),
                P.add(3).readU8(), G.add(0xbc).readU32()>>>0, H.add(4).readU32()>>>0]
               .map(function(x){return (x>>>0).toString(16);}).join('|');
      };
      try { seedD(); Orig(P); o = snap(); } catch (e) { eo = e.message; }
      try { seedD(); Reim(P); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'idx2_record_condset') {
      // void f(int i, int j, int v): off=(j+5i)*recStride; record A(float)@baseA, B(int)@baseB.
      const s = (cfg.scenarios || [])[t] || { i: 0, j: 0, v: 0, cur: 0 };
      const off = ((s.j | 0) + 5 * (s.i | 0)) * (cfg.recStride | 0);
      const A = ptr(cfg.baseA).add(off), B = ptr(cfg.baseB).add(off);
      const seedR = function () { A.writeFloat(s.cur); B.writeU32(0xCAFE0000 >>> 0); };
      const snap = function () { return (A.readU32() >>> 0).toString(16) + '|' + (B.readU32() >>> 0).toString(16); };
      try { seedR(); Orig(s.i >>> 0, s.j >>> 0, s.v >>> 0); o = snap(); } catch (e) { eo = e.message; }
      try { seedR(); Reim(s.i >>> 0, s.j >>> 0, s.v >>> 0); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'quad_buffer_build') {
      // int f(void* out, uint maxsize, struct* rec): 2-pass quad-buffer builder.
      const s = (cfg.scenarios || [])[t] || { subs: [1], maxsize: 1000 };
      const subs = s.subs || [1], cnt = subs.length;
      const out = Memory.alloc(0x800), rec = Memory.alloc(0x40), arr = Memory.alloc(0x200);
      const Ps = []; for (let k = 0; k < cnt; k++) { const P = Memory.alloc(0x20); _keep.push(P); Ps.push(P); }
      _keep.push(out, rec, arr);
      const seedB = function () {
        for (let z = 0; z < 0x800; z += 4) out.add(z).writeU32(0xEEEEEEEE);
        rec.add(0x14).writeU32(cnt >>> 0);
        rec.add(0x18).writePointer(arr);
        for (let k = 0; k < cnt; k++) {
          arr.add(0x14 + k * 0x28).writePointer(Ps[k]);
          Ps[k].add(0xd).writeU8(subs[k] & 0xff);
        }
      };
      const snap = function (ret) {
        return (ret | 0) + ':' + [0, 4, 12, 64, 68, 76].map(function (o2) { return (out.add(o2).readU32() >>> 0).toString(16); }).join('|');
      };
      try { seedB(); o = snap(Orig(out, s.maxsize >>> 0, rec)); } catch (e) { eo = e.message; }
      try { seedB(); r = snap(Reim(out, s.maxsize >>> 0, rec)); } catch (e) { er = e.message; }
    } else if (cfg.at === 'near_leaf_global_str_search') {
      // void* f(query): C3 circular-list search(*cfg.glob, query). build 3-node list. seed_sets[t]={q}.
      const ss = (cfg.seed_sets || [])[t | 0] || { q: '' };
      const A = Memory.alloc(0x40), n0 = Memory.alloc(0x40), n1 = Memory.alloc(0x40), n2 = Memory.alloc(0x40), q = Memory.alloc(0x40);
      _keep.push(A, n0, n1, n2, q);
      const buildS = function () {
        [A, n0, n1, n2, q].forEach(function (b) { for (let z = 0; z < 0x40; z += 4) b.add(z).writeU32(0); });
        A.add(8).writePointer(n0);
        n0.writePointer(n1); n0.add(8).writeUtf8String('alpha');
        n1.writePointer(n2); n1.add(8).writeUtf8String('beta');
        n2.writePointer(A.add(8)); n2.add(8).writeUtf8String('gamma');
        q.writeUtf8String(ss.q);
        ptr(cfg.glob).writePointer(A);
      };
      try { buildS(); o = '' + Orig(q); } catch (e) { eo = e.message; }
      try { buildS(); r = '' + Reim(q); } catch (e) { er = e.message; }
    } else if (cfg.at === 'near_leaf_struct_array_predicate') {
      // int f(void): predicate over a pointer array at cfg.glob. seed_sets[t]={entries:[...]}.
      const sp = (cfg.seed_sets || [])[t | 0] || { entries: [] };
      const N = cfg.count | 0, ss = cfg.struct_size | 0;
      const structs = []; for (let k = 0; k < N; k++) { const b = Memory.alloc(ss); _keep.push(b); structs.push(b); }
      const arr = ptr(cfg.glob);
      const seedP = function () {
        (sp.entries || []).forEach(function (ent, k) {
          if (ent.null) { arr.add(k * 4).writePointer(ptr(0)); }
          else {
            (ent.fields || []).forEach(function (fv) { structs[k].add(fv[0]).writeU32(fv[1] >>> 0); });
            arr.add(k * 4).writePointer(structs[k]);
          }
        });
      };
      try { seedP(); o = Orig() >>> 0; } catch (e) { eo = e.message; }
      try { seedP(); r = Reim() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'near_leaf_accum_table') {
      // void f(a1, float val, a3): accumulate into float table slot via C3 fastcall callee.
      const sp = (cfg.seed_sets || [])[t | 0] || { a1: 0, val: 0.0, a3: 0, seed: 0.0 };
      const slot = ptr(cfg.tbl_base).add((sp.a1 | 0) * (cfg.rec_stride | 0)).add((sp.a3 | 0) * 4);
      const seedF = function () { slot.writeFloat(sp.seed); };
      try { seedF(); Orig(sp.a1 >>> 0, sp.val, sp.a3 >>> 0); o = slot.readU32() >>> 0; } catch (e) { eo = e.message; }
      try { seedF(); Reim(sp.a1 >>> 0, sp.val, sp.a3 >>> 0); r = slot.readU32() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'near_leaf_record_builder') {
      // void f(A=rec_idx, B=value): record at rec_base+A*rec_stride; rec[8]=B; table writes via C3
      // setter at base=A and A+0xa. seed_sets[t]={A,B,v0,v1}.
      const sp = (cfg.seed_sets || [])[t | 0] || { A: 0, B: 0, v0: 0, v1: 0 };
      const rec = ptr(cfg.rec_base).add((sp.A | 0) * (cfg.rec_stride | 0));
      const tb = ptr(cfg.tbl_base), ts = cfg.tbl_stride | 0;
      const s0 = tb.add((sp.A | 0) * ts), s1 = tb.add(((sp.A | 0) + 0xa) * ts);
      const seedB = function () {
        rec.writeU32(sp.v0 >>> 0); rec.add(4).writeU32(sp.v1 >>> 0); rec.add(8).writeU32(0xDEAD0000 >>> 0);
        s0.writeU32(0); s1.writeU32(0);
      };
      const snapB = function () { return (rec.add(8).readU32() >>> 0).toString(16) + '|' + (s0.readU32() >>> 0).toString(16) + '|' + (s1.readU32() >>> 0).toString(16); };
      try { seedB(); Orig(sp.A >>> 0, sp.B >>> 0); o = snapB(); } catch (e) { eo = e.message; }
      try { seedB(); Reim(sp.A >>> 0, sp.B >>> 0); r = snapB(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'near_leaf_seed_multi_obs') {
      // void f(void): seed globals, call, snapshot several absolute globals. cfg.observe_addrs.
      const sp = (cfg.seed_sets || [])[t | 0] || { globals: [] };
      const obs = cfg.observe_addrs || [];
      const seedM = function () { (sp.globals || []).forEach(function (g) { ptr(g[0]).writeU32(g[1] >>> 0); }); };
      const snap = function () { return obs.map(function (a) { return (ptr(a).readU32() >>> 0).toString(16); }).join('|'); };
      try { seedM(); Orig(); o = snap(); } catch (e) { eo = e.message; }
      try { seedM(); Reim(); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'near_leaf_ptr_array_search') {
      // int f(key, gate): search arr=*cfg.glob (cfg.count ptrs) for *arr[i]==key. seed_sets[t]={gate,key,at_idx}.
      const sp = (cfg.seed_sets || [])[t | 0] || { gate: 1, key: 0, at_idx: -1 };
      const N = cfg.count | 0;
      const arr = Memory.alloc(N * 4 + 4); const structs = [];
      for (let k = 0; k < N; k++) { const b = Memory.alloc(0x10); _keep.push(b); structs.push(b); }
      _keep.push(arr);
      const seedS = function () {
        for (let k = 0; k < N; k++) { structs[k].writeU32((0x10000 + k) >>> 0); arr.add(k * 4).writePointer(structs[k]); }
        if ((sp.at_idx | 0) >= 0) structs[sp.at_idx | 0].writeU32(sp.key >>> 0);
        ptr(cfg.glob).writePointer(arr);
      };
      try { seedS(); o = Orig(sp.key >>> 0, sp.gate >>> 0) >>> 0; } catch (e) { eo = e.message; }
      try { seedS(); r = Reim(sp.key >>> 0, sp.gate >>> 0) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'near_leaf_seed_arg_obs') {
      // void f(arg): seed globals, call with arg, observe an absolute global. cfg.obs_addr.
      const sp = (cfg.seed_sets || [])[t | 0] || { globals: [], arg: 0 };
      const seedO = function () { (sp.globals || []).forEach(function (g) { ptr(g[0]).writeU32(g[1] >>> 0); }); };
      try { seedO(); Orig(sp.arg >>> 0); o = ptr(cfg.obs_addr).readU32() >>> 0; } catch (e) { eo = e.message; }
      try { seedO(); Reim(sp.arg >>> 0); r = ptr(cfg.obs_addr).readU32() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'near_leaf_seed_outbuf') {
      // NEAR-LEAF that reads seedable absolute table(s) via C3 getter callee(s) and either
      // writes a small out-buffer arg and/or returns a derived int. Generic:
      //   cfg.seed_sets[t] = { seeds:[[addr,val],...], args:[int,...] }
      //   cfg.out_size     = bytes to alloc for the out buffer (0 => no out buffer)
      //   cfg.out_argpos   = position of the out-ptr in the call's arg list (only if out_size>0)
      //   cfg.out_observe  = [offsets] to snapshot from the out buffer
      //   cfg.ret          = true to also fold the function's return into the compared value
      // Both Orig and Reim call the SAME real C3 getter callee(s) at their absolute RVAs,
      // so a faithful verbatim-naked port is identical by construction; the per-scenario
      // table seeds make the observed value vary -> non-degenerate.
      const sp = (cfg.seed_sets || [])[t | 0] || { seeds: [], args: [] };
      const outSize = cfg.out_size | 0, outPos = (cfg.out_argpos == null ? -1 : (cfg.out_argpos | 0));
      const obs = cfg.out_observe || [];
      const intArgs = (sp.args || []).map(function (a) { return a >>> 0; });
      const seedT = function () { (sp.seeds || []).forEach(function (s) { ptr(s[0]).writeU32(s[1] >>> 0); }); };
      const run = function (fn) {
        let ob = null;
        const args = intArgs.slice();
        if (outSize > 0) {
          ob = Memory.alloc(outSize); _keep.push(ob);
          for (let z = 0; z < outSize; z++) ob.add(z).writeU8(0xEE);
          args.splice(outPos, 0, ob);  // insert out-ptr at the declared position
        }
        const rv = fn.apply(null, args);
        let s = '';
        if (ob) s += obs.map(function (x) { return (ob.add(x | 0).readU32() >>> 0).toString(16); }).join(',');
        if (cfg.fold_ret) s += '|R=' + ((rv >>> 0).toString(16));
        return s;
      };
      try { seedT(); o = run(Orig); } catch (e) { eo = e.message; }
      try { seedT(); r = run(Reim); } catch (e) { er = e.message; }
    } else if (cfg.at === 'near_leaf_seed_globals') {
      // u32 f(void): near-leaf reading pure global getters. seed_sets[t]={globals:[[addr,val],...]}.
      const sp = (cfg.seed_sets || [])[t | 0] || { globals: [] };
      const seedG = function () { (sp.globals || []).forEach(function (g) { ptr(g[0]).writeU32(g[1] >>> 0); }); };
      try { seedG(); o = Orig() >>> 0; } catch (e) { eo = e.message; }
      try { seedG(); r = Reim() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'near_leaf_dot_plane') {
      // void f(arg1, arg2): dot product over a pure-callee table record. seed_sets[t]={idx,normal,point,a8}.
      const sp = (cfg.seed_sets || [])[t | 0] || { idx: 0, normal: [0, 0, 0], point: [0, 0, 0], a8: 0 };
      const base = ptr(cfg.tbl_base), stride = cfg.tbl_stride | 0;
      const a1 = Memory.alloc(0x100), a2 = Memory.alloc(0x40); _keep.push(a1, a2);
      const rec = base.add((sp.idx | 0) * stride);
      const seedD = function () {
        for (let z = 0; z < 0x100; z += 4) a1.add(z).writeU32(0);
        for (let z = 0; z < 0x40; z += 4) a2.add(z).writeU32(0);
        a1.add(0x20).writeU32(sp.idx >>> 0); a1.add(0xa8).writeU32(sp.a8 >>> 0);
        a2.add(0x20).writeFloat(sp.point[0]); a2.add(0x24).writeFloat(sp.point[1]); a2.add(0x28).writeFloat(sp.point[2]);
        rec.writeFloat(sp.normal[0]); rec.add(4).writeFloat(sp.normal[1]); rec.add(8).writeFloat(sp.normal[2]);
      };
      const snapD = function () { return (a1.add(0xac).readU32() >>> 0).toString(16) + '|' + (a1.add(0xa8).readU32() >>> 0); };
      try { seedD(); Orig(a1, a2); o = snapD(); } catch (e) { eo = e.message; }
      try { seedD(); Reim(a1, a2); r = snapD(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'near_leaf_arr_to_table') {
      // void f(int* arg): writes arg[i] into abs table[base+i*stride] via a C3 setter.
      // cfg.tbl_base/tbl_stride. seed_sets[t]={vals:[...]}.
      const sp = (cfg.seed_sets || [])[t | 0] || { vals: [] };
      const base = ptr(cfg.tbl_base), stride = cfg.tbl_stride | 0, n = sp.vals.length;
      const arg = Memory.alloc(0x40); _keep.push(arg);
      const seedA = function () {
        for (let z = 0; z < 0x40; z += 4) arg.add(z).writeU32(0);
        sp.vals.forEach(function (v, i) { arg.add(i * 4).writeU32(v >>> 0); });
        for (let i = 0; i < n; i++) base.add(i * stride).writeU32(0);
      };
      const snap = function () { let s = ''; for (let i = 0; i < n; i++) s += (base.add(i * stride).readU32() >>> 0).toString(16) + ','; return s; };
      try { seedA(); Orig(arg); o = snap(); } catch (e) { eo = e.message; }
      try { seedA(); Reim(arg); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'near_leaf_memcmp16') {
      // int f(_, arg2, arg3): !memcmp16(*arg2, arg3) via C3 callee. seed_sets[t]={eq, diffat}.
      const sp = (cfg.seed_sets || [])[t | 0] || { eq: true };
      const H = Memory.alloc(8), bufP = Memory.alloc(0x20), bufQ = Memory.alloc(0x20); _keep.push(H, bufP, bufQ);
      const seedC = function () {
        for (let z = 0; z < 0x20; z++) { bufP.add(z).writeU8((z + 1) & 0xff); bufQ.add(z).writeU8((z + 1) & 0xff); }
        if (!sp.eq) bufQ.add(sp.diffat | 0).writeU8(0xFF);
        H.writePointer(bufP);
      };
      try { seedC(); o = Orig(0, H, bufQ) >>> 0; } catch (e) { eo = e.message; }
      try { seedC(); r = Reim(0, H, bufQ) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'near_leaf_seed_ret') {
      // u32 f(void): near-leaf reading a seedable abs table via a C3 callee, returns derived int.
      // cfg.tbl_base/tbl_stride/tbl_count. seed_sets[t]={preset:[[idx,val],...]}.
      const sp = (cfg.seed_sets || [])[t | 0] || { preset: [] };
      const base = ptr(cfg.tbl_base), stride = cfg.tbl_stride | 0, count = cfg.tbl_count | 0;
      const seedR = function () {
        for (let i = 0; i < count; i++) base.add(i * stride).writeU32(0);
        (sp.preset || []).forEach(function (pv) { base.add(pv[0] * stride).writeU32(pv[1] >>> 0); });
      };
      try { seedR(); o = Orig() >>> 0; } catch (e) { eo = e.message; }
      try { seedR(); r = Reim() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'near_leaf_abs_table') {
      // void f(arg): near-leaf parent looping a C3 callee that writes an absolute table.
      // cfg.tbl_base/tbl_stride/tbl_count/observe. seed_sets[t]={arg, preset:[[off,val],...]}.
      const sp = (cfg.seed_sets || [])[t | 0] || { arg: 0, preset: [] };
      const base = ptr(cfg.tbl_base), stride = cfg.tbl_stride | 0, count = cfg.tbl_count | 0;
      const obs = cfg.observe || [];
      const seedTbl = function () {
        for (let i = 0; i < count; i++) {
          const rec = base.add(i * stride);
          for (let z = 0; z < stride; z += 4) rec.add(z).writeU32(0);
          (sp.preset || []).forEach(function (pv) { rec.add(pv[0]).writeU32(pv[1] >>> 0); });
        }
      };
      const snap = function () {
        let s = ''; [0, count >> 1, count - 1].forEach(function (ri) {
          const rec = base.add(ri * stride);
          obs.forEach(function (o2) { s += (rec.add(o2).readU32() >>> 0).toString(16) + ','; });
          s += '|';
        }); return s;
      };
      try { seedTbl(); Orig(sp.arg >>> 0); o = snap(); } catch (e) { eo = e.message; }
      try { seedTbl(); Reim(sp.arg >>> 0); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'heap_alloc_aligned') {
      // void* f(heap* arg1, size, align): aligned first-fit over one block. seed_sets[t]={size,align}.
      const sp = (cfg.seed_sets || [])[t | 0] || { size: 0x20, align: 0x10 };
      const H = Memory.alloc(0x400), A = Memory.alloc(0x40); _keep.push(H, A);
      const seedH = function () {
        for (let z = 0; z < 0x400; z += 4) H.add(z).writeU32(0);
        for (let z = 0; z < 0x40; z += 4) A.add(z).writeU32(0);
        A.add(8).writePointer(H);                 // first block = H
        A.add(0xc).writePointer(H.add(0x100));    // sentinel = end of block region
        H.writePointer(H.add(0x100));             // block0.next/end = sentinel
        H.add(8).writeU32(0x10);                  // block0.used = 0x10
      };
      const snapH = function (ret) {
        const rv = (ret.isNull()) ? '0' : '+' + ret.sub(H).toInt32().toString(16);
        const b0n = H.readPointer(); const b0 = b0n.isNull() ? '0' : (b0n.compare(H) >= 0 && b0n.compare(H.add(0x400)) < 0 ? '+' + b0n.sub(H).toInt32().toString(16) : b0n.toString());
        return rv + '|' + b0;
      };
      try { seedH(); o = snapH(Orig(A, sp.size >>> 0, sp.align >>> 0)); } catch (e) { eo = e.message; }
      try { seedH(); r = snapH(Reim(A, sp.size >>> 0, sp.align >>> 0)); } catch (e) { er = e.message; }
    } else if (cfg.at === 'record_array_filter_update') {
      // void f(arg1, arg2, arg3, arg4, arg5, arg6): record-array filter+update. fixed 4-record
      // array [A,B,C]; seed_sets[t]={arg3,arg4,arg5,arg6}. snapshot *arg1 + per-record (A,D).
      const sp = (cfg.seed_sets || [])[t | 0] || {};
      const N = 4;
      const recs = [[2, 5, 1], [3, 5, 2], [2, 9, 1], [7, 5, 3]];
      const A1 = Memory.alloc(0x200), A2 = Memory.alloc(0x40); _keep.push(A1, A2);
      const seedR = function () {
        for (let z = 0; z < 0x200; z += 4) A1.add(z).writeU32(0);
        for (let z = 0; z < 0x40; z += 4) A2.add(z).writeU32(0);
        A2.add(4).writeU32(N); A2.add(8).writeU32(10);
        for (let i = 0; i < N; i++) {
          const R = A1.add(0x18 + i * 0x10);
          R.writeU32(recs[i][0] >>> 0); R.add(4).writeU32(recs[i][1] >>> 0); R.add(8).writeU32(recs[i][2] >>> 0); R.add(0xc).writeU32(0);
        }
      };
      const snapR = function () {
        let s = (A1.readU32() >>> 0).toString(16);
        for (let i = 0; i < N; i++) { const R = A1.add(0x18 + i * 0x10); s += '|' + (R.readU32() >>> 0).toString(16) + ',' + (R.add(0xc).readU32() >>> 0).toString(16); }
        return s;
      };
      const a3 = sp.arg3 >>> 0, a4 = sp.arg4 >>> 0, a5 = sp.arg5 >>> 0, a6 = sp.arg6 >>> 0;
      try { seedR(); Orig(A1, A2, a3, a4, a5, a6); o = snapR(); } catch (e) { eo = e.message; }
      try { seedR(); Reim(A1, A2, a3, a4, a5, a6); r = snapR(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'bitmap_blit') {
      // int f(dst* arg1, src* arg2): palette + per-row pixel copy. seed_sets[t]={rows,width_bits,
      // channels,dstride,sstride,pal_bits,palette}. snapshot dst pixels[0x40] + dst palette[0x40].
      const sp = (cfg.seed_sets || [])[t | 0] || {};
      const D = Memory.alloc(0x40), S = Memory.alloc(0x40);
      const dpx = Memory.alloc(0x200), spx = Memory.alloc(0x200), dpal = Memory.alloc(0x200), spal = Memory.alloc(0x200);
      _keep.push(D, S, dpx, spx, dpal, spal);
      const seedTX = function () {
        [D, S].forEach(function (b) { for (let z = 0; z < 0x40; z += 4) b.add(z).writeU32(0); });
        for (let z = 0; z < 0x200; z++) { dpx.add(z).writeU8(0); dpal.add(z).writeU8(0); spx.add(z).writeU8((z * 7 + 3) & 0xff); spal.add(z).writeU8((z * 5 + 0x80) & 0xff); }
        D.add(4).writeU32((sp.channels | 0) >>> 0);
        D.add(8).writeU32((sp.rows | 0) >>> 0);
        D.add(0xc).writeU32((sp.width_bits | 0) >>> 0);
        D.add(0x10).writeU32((sp.dstride | 0) >>> 0);
        D.add(0x14).writePointer(dpx);
        D.add(0x18).writePointer(sp.palette ? dpal : ptr(0));
        S.add(0xc).writeU32((sp.pal_bits | 0) >>> 0);
        S.add(0x10).writeU32((sp.sstride | 0) >>> 0);
        S.add(0x14).writePointer(spx);
        S.add(0x18).writePointer(sp.palette ? spal : ptr(0));
      };
      const snapTX = function () {
        const hx = function (buf) { const u = new Uint8Array(buf.readByteArray(0x40)); let s = ''; for (let k = 0; k < u.length; k++) s += ('0' + u[k].toString(16)).slice(-2); return s; };
        return hx(dpx) + '#' + hx(dpal);
      };
      try { seedTX(); Orig(D, S); o = snapTX(); } catch (e) { eo = e.message; }
      try { seedTX(); Reim(D, S); r = snapTX(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'pool_freelist_init') {
      // void f(pool* arg1): zero buffer + build circular freelist. seed_sets[t]={n}.
      const sp = (cfg.seed_sets || [])[t | 0] || { n: 0 };
      const P = Memory.alloc(0x200), B = Memory.alloc(0x100); _keep.push(P, B);
      const seedP = function () {
        for (let z = 0; z < 0x200; z += 4) P.add(z).writeU32(0);
        for (let z = 0; z < 0x100; z += 4) B.add(z).writeU32(0);
        P.add(0x16c).writeU32(sp.n >>> 0);
        P.add(0x14).writePointer(B);
      };
      const snapP = function () {
        const fields = [0x18, 0x1c, 0x20, 0x168, 0x170, 0x194, 0x198].map(function (o2) { return P.add(o2).readU32() >>> 0; });
        const links = [0x00, 0x24, 0x48, 0x6c, 0x90].map(function (o2) { return B.add(o2 + 0x1c).readU32() >>> 0; });
        return fields.concat(links).join('|');
      };
      try { seedP(); Orig(P); o = snapP(); } catch (e) { eo = e.message; }
      try { seedP(); Reim(P); r = snapP(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'byte_format_hexdump') {
      // void f(struct* arg1, char* out, void* arg3): 4 bytes at arg1[0x11c] -> formatted string.
      // seed_sets[t]={bytes:[4], payload:bool}. snapshot out[0x70] as hex.
      const ss = (cfg.seed_sets || [])[t | 0] || { bytes: [0, 0, 0, 0], payload: false };
      const a1 = Memory.alloc(0x140), out = Memory.alloc(0x100), pay = Memory.alloc(0x40);
      _keep.push(a1, out, pay);
      const seedF = function () {
        for (let z = 0; z < 0x140; z += 4) a1.add(z).writeU32(0);
        for (let z = 0; z < 0x100; z += 4) out.add(z).writeU32(0);
        for (let z = 0; z < 0x40; z++) pay.add(z).writeU8((0x10 + z) & 0xff);
        for (let k = 0; k < 4; k++) a1.add(0x11c + k).writeU8(ss.bytes[k] & 0xff);
      };
      const a3 = ss.payload ? pay : ptr(0);
      const snap = function () {
        const u = new Uint8Array(out.readByteArray(0x70));
        let s = ''; for (let k = 0; k < u.length; k++) s += ('0' + u[k].toString(16)).slice(-2);
        return s;
      };
      try { seedF(); Orig(a1, out, a3); o = snap(); } catch (e) { eo = e.message; }
      try { seedF(); Reim(a1, out, a3); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'circular_str_search_ci') {
      // void* f(list* arg1, char* query): case-insensitive search over a circular list.
      // build a 3-node list (alpha/beta/gamma); node[0]=next, node+8=key. seed_sets[t]={q}.
      const ss = (cfg.seed_sets || [])[t | 0] || { q: '' };
      const A = Memory.alloc(0x40), n0 = Memory.alloc(0x40), n1 = Memory.alloc(0x40), n2 = Memory.alloc(0x40), q = Memory.alloc(0x40);
      _keep.push(A, n0, n1, n2, q);
      const buildS = function () {
        [A, n0, n1, n2, q].forEach(function (b) { for (let z = 0; z < 0x40; z += 4) b.add(z).writeU32(0); });
        A.add(8).writePointer(n0);
        n0.writePointer(n1); n0.add(8).writeUtf8String('alpha');
        n1.writePointer(n2); n1.add(8).writeUtf8String('beta');
        n2.writePointer(A.add(8)); n2.add(8).writeUtf8String('gamma');
        q.writeUtf8String(ss.q);
      };
      try { buildS(); o = '' + Orig(A, q); } catch (e) { eo = e.message; }
      try { buildS(); r = '' + Reim(A, q); } catch (e) { er = e.message; }
    } else if (cfg.at === 'aabb_sphere_overlap') {
      // int f(box* arg1, sphere* arg2): box min[0,4,8] max[0xc,0x10,0x14]; sphere center[0,4,8] r[0xc].
      // plain __cdecl, no callee/global -> direct Orig/Reim. seed_sets[t]={box:[6],sph:[4]}.
      const ss = (cfg.seed_sets || [])[t | 0] || { box: [0, 0, 0, 0, 0, 0], sph: [0, 0, 0, 0] };
      const b1 = Memory.alloc(0x40), b2 = Memory.alloc(0x40); _keep.push(b1, b2);
      const seedAB = function () {
        for (let z = 0; z < 0x40; z += 4) { b1.add(z).writeU32(0); b2.add(z).writeU32(0); }
        for (let k = 0; k < 6; k++) b1.add(k * 4).writeFloat(ss.box[k]);
        for (let k = 0; k < 4; k++) b2.add(k * 4).writeFloat(ss.sph[k]);
      };
      try { seedAB(); o = (Orig(b1, b2) | 0); } catch (e) { eo = e.message; }
      try { seedAB(); r = (Reim(b1, b2) | 0); } catch (e) { er = e.message; }
    } else if (cfg.at === 'case_insensitive_ncmp') {
      // int fn(char* s1, char* s2, int n): bounded case-insensitive compare (custom toupper).
      // plain __cdecl, no callee -> direct Orig/Reim. seed_sets[t]={s1,s2,n}.
      const ss = (cfg.seed_sets || [])[t | 0] || { s1: '', s2: '', n: 0 };
      const b1 = Memory.alloc(64), b2 = Memory.alloc(64); _keep.push(b1, b2);
      const seedCC = function () {
        for (let z = 0; z < 64; z += 4) { b1.add(z).writeU32(0); b2.add(z).writeU32(0); }
        b1.writeUtf8String(ss.s1); b2.writeUtf8String(ss.s2);
      };
      try { seedCC(); o = (Orig(b1, b2, ss.n >>> 0) | 0); } catch (e) { eo = e.message; }
      try { seedCC(); r = (Reim(b1, b2, ss.n >>> 0) | 0); } catch (e) { er = e.message; }
    } else if (cfg.at === 'eax_ecx_float_hash') {
      // float fn(EAX=a, ECX=b): integer noise-hash -> float. ORIG via mov eax/ecx + call
      // trampoline (returns float in st0); REIMPL naked __cdecl(a,b)->float. seed_pairs[t]=[a,b].
      const pr = (cfg.seed_pairs || [])[t | 0] || [0, 0];
      const scrH = Memory.alloc(8); _keep.push(scrH);
      const bits = function (fv) { scrH.writeFloat(fv); return scrH.readU32() >>> 0; };
      const mkH = function (target) {
        const tr = Memory.alloc(Process.pageSize); _keep.push(tr);
        let p = 0;
        tr.add(p).writeU8(0xB8); tr.add(p + 1).writeU32(pr[0] >>> 0); p += 5; // mov eax, a
        tr.add(p).writeU8(0xB9); tr.add(p + 1).writeU32(pr[1] >>> 0); p += 5; // mov ecx, b
        tr.add(p).writeU8(0xE8); tr.add(p + 1).writeS32(target.sub(tr.add(p + 5)).toInt32()); p += 5; // call
        tr.add(p).writeU8(0xC3); p += 1;                                      // ret
        Memory.protect(tr, 24, 'rwx');
        return new NativeFunction(tr, 'float', [], 'mscdecl');
      };
      try { o = bits(mkH(ptr(cfg.rva))()); } catch (e) { eo = e.message; }
      try { r = bits(Reim(pr[0] >>> 0, pr[1] >>> 0)); } catch (e) { er = e.message; }
    } else if (cfg.at === 'list_walk_self_write') {
      // void fn(p, value): node=*p; while(node!=*node) node=*node; *(*0x911ae4 + node)=value.
      // global is .bss-zero at spawn -> writes value to terminal[0]. seed_sets[t]={len,value}.
      const sp = (cfg.seed_sets || [])[t | 0] || { len: 1, value: 0 };
      const LEN = Math.max(1, sp.len | 0);
      const pW = Memory.alloc(0x10); _keep.push(pW);
      const nodesW = []; for (let k = 0; k < LEN; k++) { const b = Memory.alloc(0x40); _keep.push(b); nodesW.push(b); }
      const buildW = function () {
        for (let k = 0; k < LEN; k++) {
          for (let z = 0; z < 0x40; z += 4) nodesW[k].add(z).writeU32(0);
          nodesW[k].writePointer(k < LEN - 1 ? nodesW[k + 1] : nodesW[k]); // terminal self-points
        }
        pW.writePointer(nodesW[0]);
      };
      const termW = function () { return nodesW[LEN - 1].readU32() >>> 0; };
      try { buildW(); Orig(pW, sp.value >>> 0); o = termW(); } catch (e) { eo = e.message; }
      try { buildW(); Reim(pW, sp.value >>> 0); r = termW(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'fastcall_float_clamp') {
      // void __fastcall fn(ECX=idx, EDX=base, [esp+4]=val): base[idx] = min(val+base[idx], 50.0f)
      // with the compare on the 80-bit sum. ORIG via mov ecx/edx + push valbits + call trampoline;
      // REIMPL is naked __cdecl(idx,base,val) doing the exact x87. seed_sets[t]={idx,cur,val}.
      const sp = (cfg.seed_sets || [])[t | 0] || { idx: 0, cur: 0.0, val: 0.0 };
      const base = Memory.alloc(0x40), scr = Memory.alloc(8); _keep.push(base, scr);
      const seedFC = function () {
        for (let z = 0; z < 0x40; z += 4) base.add(z).writeU32(0);
        base.add((sp.idx | 0) * 4).writeFloat(sp.cur);
      };
      scr.writeFloat(sp.val); const valBits = scr.readU32() >>> 0;
      const mkFC = function (target) {
        const tr = Memory.alloc(Process.pageSize); _keep.push(tr);
        let p = 0;
        tr.add(p).writeU8(0xB9); tr.add(p + 1).writeU32((sp.idx | 0) >>> 0); p += 5; // mov ecx, idx
        tr.add(p).writeU8(0xBA); tr.add(p + 1).writePointer(base); p += 5;           // mov edx, base
        tr.add(p).writeU8(0x68); tr.add(p + 1).writeU32(valBits); p += 5;            // push valBits
        tr.add(p).writeU8(0xE8); tr.add(p + 1).writeS32(target.sub(tr.add(p + 5)).toInt32()); p += 5; // call target
        tr.add(p).writeU8(0x83); tr.add(p + 1).writeU8(0xC4); tr.add(p + 2).writeU8(0x04); p += 3;    // add esp, 4
        tr.add(p).writeU8(0xC3); p += 1;                                             // ret
        Memory.protect(tr, 32, 'rwx');
        return new NativeFunction(tr, 'void', [], 'mscdecl');
      };
      try { seedFC(); mkFC(ptr(cfg.rva))(); o = base.add((sp.idx | 0) * 4).readU32() >>> 0; } catch (e) { eo = e.message; }
      try { seedFC(); Reim(sp.idx >>> 0, base, sp.val); r = base.add((sp.idx | 0) * 4).readU32() >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'table_accum_clamp') {
      // void fn(a1, p2, p3): p2/p3 are int32 counters. idx=*p3 -> tblA (0x634498) signed
      // word; scaled by (2*(a1&7)+1), >>3 (logical), added/subbed into *p2 by (a1&8) and
      // clamped [-32768,32767]; then *p3 += tblB(0x634478) signed word @ a1, clamp[0,88].
      // Tables are absolute .rdata read identically by both sides. seed_sets[t]={a1,v2,v3}.
      const sp = (cfg.seed_sets || [])[t | 0] || { a1: 0, v2: 0, v3: 0 };
      const p2 = Memory.alloc(0x40), p3 = Memory.alloc(0x40); _keep.push(p2, p3);
      const runAC = function (CALL) {
        for (let z = 0; z < 0x40; z += 4) { p2.add(z).writeU32(0); p3.add(z).writeU32(0); }
        p2.writeS32(sp.v2 | 0); p3.writeS32(sp.v3 | 0);
        CALL(sp.a1 >>> 0, p2, p3);
        return (p2.readS32() | 0) + '|' + (p3.readS32() | 0);
      };
      try { o = runAC(Orig); } catch (e) { eo = e.message; }
      try { r = runAC(Reim); } catch (e) { er = e.message; }
    } else if (cfg.at === 'dll_get_nth') {
      // u32 fn(p, cont, idx): DLL get Nth element. count=cont[8]; if idx<count/2 walk
      // forward from p[0x20] (head) idx times via node[0]; else backward from p[0x24]
      // (tail) (count-1-idx) times via node[4]; return node-0x2c. Build a 5-node DLL in
      // fixed object buffers (node embedded at object+0x2c); test = idx. Read-only.
      const NN = 5;
      const pG = Memory.alloc(0x40), contG = Memory.alloc(0x40);
      const objG = []; for (let k = 0; k < NN; k++) { const b = Memory.alloc(0x40); _keep.push(b); objG.push(b); }
      _keep.push(pG, contG);
      const nodeG = objG.map(function (b) { return b.add(0x2c); });
      const buildG = function () {
        contG.add(8).writeU32(NN);
        pG.add(0x20).writePointer(nodeG[0]); pG.add(0x24).writePointer(nodeG[NN - 1]);
        for (let k = 0; k < NN; k++) {
          nodeG[k].writePointer(k < NN - 1 ? nodeG[k + 1] : ptr(0));        // next
          nodeG[k].add(4).writePointer(k > 0 ? nodeG[k - 1] : ptr(0));      // prev
        }
      };
      const runG = function (CALL) { buildG(); return CALL(pG, contG, t >>> 0) >>> 0; };
      try { o = runG(Orig); } catch (e) { eo = e.message; }
      try { r = runG(Reim); } catch (e) { er = e.message; }
    } else if (cfg.at === 'circular_dll_search') {
      // u32 fn(p, key): circular list head at p[0x10], sentinel = p+0x10, node[0]=next,
      // object = node-0x18; returns the object whose addr == key, else 0. Build a 3-object
      // circular list (node embedded at object+0x18). test 0 -> search obj1 (found), else
      // a bogus key (not found -> 0). Read-only, so a simple A/B suffices.
      const pC = Memory.alloc(0x40);
      const o0 = Memory.alloc(0x40), o1 = Memory.alloc(0x40), o2 = Memory.alloc(0x40);
      _keep.push(pC, o0, o1, o2);
      const n0 = o0.add(0x18), n1 = o1.add(0x18), n2 = o2.add(0x18), sentC = pC.add(0x10);
      const buildC = function () { pC.add(0x10).writePointer(n0); n0.writePointer(n1); n1.writePointer(n2); n2.writePointer(sentC); };
      const keyC = ((t >>> 0) === 0) ? o1 : pC.add(0x200);
      const runC = function (CALL) { buildC(); return CALL(pC, keyC) >>> 0; };
      try { o = runC(Orig); } catch (e) { eo = e.message; }
      try { r = runC(Reim); } catch (e) { er = e.message; }
    } else if (cfg.at === 'dll_unlink') {
      // Doubly-linked-list unlink. Layout (from 0x5ae550): node[0]=next, node[4]=prev,
      // list[8]=head node, list[0xc]=sentinel; the call removes the node whose
      // (node+0xc) == arg1. Build a fresh 3-node DLL (N0<->N1<->N2, sentinel S at both
      // ends) in FIXED buffers each side, remove the middle (N1), snapshot the relinked
      // pointers (N0.next, N2.prev) -> both should become &N2 / &N0 (changed from &N1).
      const L = Memory.alloc(0x20), S = Memory.alloc(0x10);
      const N0 = Memory.alloc(0x10), N1 = Memory.alloc(0x10), N2 = Memory.alloc(0x10);
      _keep.push(L, S, N0, N1, N2);
      const build = function () {
        L.add(8).writePointer(N0); L.add(0xc).writePointer(S);
        N0.writePointer(N1); N0.add(4).writePointer(S);   // N0: next=N1, prev=S
        N1.writePointer(N2); N1.add(4).writePointer(N0);  // N1: next=N2, prev=N0
        N2.writePointer(S);  N2.add(4).writePointer(N1);  // N2: next=S,  prev=N1
      };
      const arg1 = N1.add(0xc);
      const runD = function (CALL) {
        build(); CALL(L, arg1);
        return N0.readPointer().toString() + ',' + N2.add(4).readPointer().toString();
      };
      try { o = runD(Orig); } catch (e) { eo = e.message; }
      try { r = runD(Reim); } catch (e) { er = e.message; }
    } else if (cfg.at === 'idx_src_abs_memcpy') {
      // void fn(idx, src): if(src) memcpy(tgt + idx*stride, src, copy_dwords*4).
      // Seed src with distinct values, reset the abs dest, call fn(idx, srcbuf),
      // observe the abs dest -> distinct copied values (verifies dest addr+stride+count).
      const cdw = (cfg.copy_dwords | 0) || 0x10, strM = (cfg.stride | 0) || 0;
      const destM = ptr(cfg.tgt).add((t >>> 0) * strM);
      const srcM = Memory.alloc(cdw * 4); _keep.push(srcM);
      const runM = function (CALL) {
        for (let k = 0; k < cdw; k++) { srcM.add(k * 4).writeU32((0xC0DE0000 | k) >>> 0); destM.add(k * 4).writeU32(0x5e5e5e5e); }
        CALL(t >>> 0, srcM);
        const out = []; for (let k = 0; k < cdw; k++) out.push(destM.add(k * 4).readU32() >>> 0);
        return out.join(',');
      };
      try { o = runM(Orig); } catch (e) { eo = e.message; }
      try { r = runM(Reim); } catch (e) { er = e.message; }
    } else if (cfg.at === 'nested_struct_op') {
      // void fn(ptr p): p[link_off] points to a sub-buffer; fn RMWs p's own fields
      // and writes into the sub-buffer. Alloc p + sub, link p[link_off]=&sub, seed
      // p fields (cfg.p_seed [{off,val}]), fill sub with sentinel, call fn(p),
      // observe cfg.observe_p (in p) + cfg.observe_sub (in sub). Reimpl __cdecl(p).
      const lo = cfg.link_off | 0, pseed = cfg.p_seed || [];
      const opN = cfg.observe_p || [], osN = cfg.observe_sub || [];
      const pN = Memory.alloc(0x400), subN = Memory.alloc(0x8000); _keep.push(pN, subN);
      const runN = function (CALL) {
        for (let z = 0; z < 0x400; z += 4) pN.add(z).writeU32(0);
        for (let z = 0; z < 0x8000; z += 4) subN.add(z).writeU32(0xA5A5A5A5);
        pseed.forEach(function (s) { pN.add(s.off | 0).writeU32(s.val >>> 0); });
        pN.add(lo).writePointer(subN);
        CALL(pN);
        const a = opN.map(function (o2) { return pN.add(o2 | 0).readU32() >>> 0; });
        const b = osN.map(function (o2) { return subN.add(o2 | 0).readU32() >>> 0; });
        return 'P[' + a.join(',') + '] S[' + b.join(',') + ']';
      };
      try { o = runN(Orig); } catch (e) { eo = e.message; }
      try { r = runN(Reim); } catch (e) { er = e.message; }
    } else if (cfg.at === 'idx_table_out') {
      // void fn(idx, out*): *out = value from a static abs table indexed by idx.
      // Call with an out buffer, observe out[0]. Varying idx across tests reads
      // different (static) table entries -> non-degenerate. Reimpl is __cdecl(idx,out).
      const obI = Memory.alloc(0x40); _keep.push(obI);
      const tblI = cfg.tgt ? ptr(cfg.tgt) : null, strI = (cfg.stride | 0) || 8;
      const seedI = (0xC0DE0000 | (t & 0xffff)) >>> 0;   // varied per idx -> verifies address+stride
      const runI = function (CALL) {
        if (tblI) tblI.add((t >>> 0) * strI).writeU32(seedI);
        obI.writeU32(0xA5A5A5A5); CALL(t >>> 0, obI); return obI.readU32() >>> 0;
      };
      try { o = runI(Orig); } catch (e) { eo = e.message; }
      try { r = runI(Reim); } catch (e) { er = e.message; }
    } else if (cfg.at === 'abstable_ptr_zero') {
      // void fn(idx): ptr = *(u32*)(abstable + idx*4); zero/op a buffer at ptr.
      // Seed abstable[idx] = &scratch, fill scratch sentinel, call fn(idx), observe
      // scratch at observe_offs. Reimpl reads the SAME abs-table entry -> same scratch.
      const tbZ = ptr(cfg.tgt), idxZ = (cfg.idx | 0), bdZ = (cfg.buf_dwords | 0) || 0x1000;
      const offsZ = cfg.observe_offs || [0x0];
      const scratchZ = Memory.alloc(bdZ * 4); _keep.push(scratchZ);
      const runZ = function (CALL) {
        for (let z = 0; z < bdZ; z++) scratchZ.add(z * 4).writeU32(0xA5A5A5A5);
        tbZ.add(idxZ * 4).writePointer(scratchZ);
        CALL(idxZ);
        return offsZ.map(function (o2) { return scratchZ.add(o2 | 0).readU32() >>> 0; }).join('|');
      };
      try { o = runZ(Orig); } catch (e) { eo = e.message; }
      try { r = runZ(Reim); } catch (e) { er = e.message; }
    } else if (cfg.at === 'eax_struct_stack_out') {
      // void fn(EAX=struct ptr, [esp+4]=out ptr). Trampoline `mov eax,sbuf; jmp
      // target`, NativeFunction(void,['pointer']) called with obuf -> obuf lands at
      // [esp+4]. Seed sbuf fields (eax_seed), call, observe obuf (out_observe).
      const eseedQ = cfg.eax_seed || [], oobsQ = cfg.out_observe || [0x0];
      const sbufQ = Memory.alloc(0x100), obufQ = Memory.alloc(0x100); _keep.push(sbufQ, obufQ);
      const mkTQ = function (target) {
        const tr = Memory.alloc(Process.pageSize); _keep.push(tr);
        tr.writeU8(0xB8); tr.add(1).writePointer(sbufQ);   // mov eax, sbuf
        tr.add(5).writeU8(0xE9); tr.add(6).writeS32(target.sub(tr.add(10)).toInt32()); // jmp
        Memory.protect(tr, 16, 'rwx');
        return new NativeFunction(tr, 'void', ['pointer'], 'mscdecl');
      };
      const runQ = function (CALL) {
        for (let z = 0; z < 0x100; z += 4) { sbufQ.add(z).writeU32(0); obufQ.add(z).writeU32(0xCCCCCCCC); }
        eseedQ.forEach(function (s) { sbufQ.add(s.off | 0).writeU32(s.val >>> 0); });
        CALL(obufQ);
        return oobsQ.map(function (o2) { return obufQ.add(o2 | 0).readU32() >>> 0; }).join('|');
      };
      try { o = runQ(mkTQ(ptr(cfg.rva))); } catch (e) { eo = e.message; }
      try { r = runQ(mkTQ(reim)); } catch (e) { er = e.message; }
    } else if (cfg.at === 'eax_out_2float') {
      // void fn(EAX=out ptr, float a1, float a2). Trampoline mov eax,outbuf; jmp target.
      const obuf2 = Memory.alloc(0x40); _keep.push(obuf2);
      const mkE2 = function (target) {
        const tr = Memory.alloc(Process.pageSize); _keep.push(tr);
        tr.writeU8(0xB8); tr.add(1).writePointer(obuf2);   // mov eax, obuf
        tr.add(5).writeU8(0xE9); tr.add(6).writeS32(target.sub(tr.add(10)).toInt32()); // jmp
        Memory.protect(tr, 16, 'rwx');
        return new NativeFunction(tr, 'void', ['float', 'float'], 'mscdecl');
      };
      const s = (cfg.scenarios || [])[t] || { a1: 1.0, a2: 1.0 };
      const runE = function (CALL) {
        for (let z = 0; z < 0x40; z += 4) obuf2.add(z).writeU32(0xCCCCCCCC);
        CALL(s.a1, s.a2);
        return (obuf2.readU32() >>> 0).toString(16) + '|' + (obuf2.add(4).readU32() >>> 0).toString(16);
      };
      try { o = runE(mkE2(ptr(cfg.rva))); } catch (e) { eo = e.message; }
      try { r = runE(mkE2(reim)); } catch (e) { er = e.message; }
    } else if (cfg.at === 'dll_merge_swap') {
      // void f(void): circular-list merge+swap on table base=*glob_a + *glob_b.
      // Test the empty-B early-exit swap path (safe). Role-swap A/B for non-degen.
      const s = (cfg.scenarios || [])[t] || { swap: false };
      const mybuf = Memory.alloc(0x40), n1 = Memory.alloc(0x20), n2 = Memory.alloc(0x20);
      _keep.push(mybuf, n1, n2);
      const A = s.swap ? n2 : n1, B = s.swap ? n1 : n2;
      const seedS = function () {
        for (let z = 0; z < 0x40; z += 4) mybuf.add(z).writeU32(0);
        ptr(cfg.glob_a).writePointer(mybuf);
        ptr(cfg.glob_b).writeU32(0);
        mybuf.add(0x20).writePointer(B);
        mybuf.add(0x24).writePointer(A);
        mybuf.add(8).writeU32(0x12345678);   // sentinel -> should be cleared to 0
        B.writePointer(B);                    // empty B (Bnode[0]==B) -> early-exit
      };
      const snap = function () {
        return (mybuf.add(0x20).readU32() >>> 0).toString(16) + '|' +
               (mybuf.add(0x24).readU32() >>> 0).toString(16) + '|' +
               (mybuf.add(8).readU32() >>> 0).toString(16);
      };
      try { seedS(); Orig(); o = snap(); } catch (e) { eo = e.message; }
      try { seedS(); Reim(); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'find_node_struct_copy') {
      // int f(struct* p1, void** p2): find node in p2's list, copy p1 into it.
      const s = (cfg.scenarios || [])[t] || { pat: 0xA0000000, pat2: 0xB0000000 };
      const p1 = Memory.alloc(0x400), node = Memory.alloc(0x400), src2 = Memory.alloc(0x80), p2 = Memory.alloc(4);
      _keep.push(p1, node, src2, p2);
      const seedN = function () {
        for (let z = 0; z < 0x400; z += 4) p1.add(z).writeU32(((s.pat >>> 0) | (z >>> 2)) >>> 0);
        p1.add(0x16c).writeU32(1);                 // count -> 2nd copy = 9 dwords
        p1.add(0x14).writePointer(src2);           // src2 ptr
        for (let z = 0; z < 0x80; z += 4) src2.add(z).writeU32(((s.pat2 >>> 0) | (z >>> 2)) >>> 0);
        for (let z = 0; z < 0x400; z += 4) node.add(z).writeU32(0xEEEEEEEE);
        node.add(8).writeU32(p1.add(8).readU32() >>> 0);  // node[8] = p1[8] -> match
        node.writeU32(0x10b);                      // node[0] = 0x10b -> found first iter
        p2.writePointer(node);
      };
      const snap = function (ret) {
        return (ret >>> 0) + ':' + [0, 0x66 * 4, 0x19c].map(function (o2) { return (node.add(o2).readU32() >>> 0).toString(16); }).join('|');
      };
      try { seedN(); o = snap(Orig(p1, p2)); } catch (e) { eo = e.message; }
      try { seedN(); r = snap(Reim(p1, p2)); } catch (e) { er = e.message; }
    } else if (cfg.at === 'nested_list_search') {
      // uint f(int key): nested circular-list search. outer node O1=outerBuf+0x20.
      const s = (cfg.scenarios || [])[t] || { pval: 0, key: 0 };
      const outerBuf = Memory.alloc(0x80), I1 = Memory.alloc(0x40), P1 = Memory.alloc(0x40);
      _keep.push(outerBuf, I1, P1);
      const O1 = outerBuf.add(0x20);
      const sentinel = ptr(cfg.glob);
      const seedL = function () {
        for (let z = 0; z < 0x80; z += 4) outerBuf.add(z).writeU32(0);
        sentinel.writePointer(O1);                 // *glob = O1 (outer head)
        O1.writePointer(sentinel);                 // O1[0] = sentinel (1-node outer, circular)
        O1.sub(0xc).writePointer(I1);              // inner head
        // inner sentinel = O1-0x10 (outerBuf+0x10); link I1 -> sentinel (1-node inner)
        for (let z = 0; z < 0x40; z += 4) { I1.add(z).writeU32(0); P1.add(z).writeU32(0); }
        I1.add(8).writePointer(P1);                // inner payload ptr
        I1.add(4).writePointer(O1.sub(0x10));      // next inner = sentinel
        P1.add(0xc).writeU32(s.pval >>> 0);        // payload[0xc] = pval
      };
      try { seedL(); o = (Orig(s.key >>> 0) >>> 0).toString(16); } catch (e) { eo = e.message; }
      try { seedL(); r = (Reim(s.key >>> 0) >>> 0).toString(16); } catch (e) { er = e.message; }
    } else if (cfg.at === 'pixel_max_alpha') {
      // int f(struct* s): per-pixel alpha = max(R,G,B).
      const sc = (cfg.scenarios || [])[t] || { mode: 0, rows: 0, cols: 0, stride: 0x40 };
      const st = Memory.alloc(0x40), base = Memory.alloc(0x400); _keep.push(st, base);
      const seedP = function () {
        for (let z = 0; z < 0x40; z += 4) st.add(z).writeU32(0);
        for (let z = 0; z < 0x400; z++) base.add(z).writeU8((z * 7 + 3) & 0xff);   // RGB pattern
        for (let z = 3; z < 0x400; z += 4) base.add(z).writeU8(0xEE);               // alpha = sentinel
        st.add(0xc).writeU32(sc.mode >>> 0);
        st.add(8).writeU32(sc.rows >>> 0);
        st.add(4).writeU32(sc.cols >>> 0);
        st.add(0x10).writeU32(sc.stride >>> 0);
        st.add(0x14).writePointer(base);   // mode 0x20 base
        st.add(0x18).writePointer(base);   // mode 4/8 base
      };
      const snap = function (ret) {
        return (ret >>> 0).toString(16) + ':' +
               [3, 7, 0x43].map(function (o2) { return (base.add(o2).readU8()).toString(16); }).join('|');
      };
      try { seedP(); o = snap(Orig(st)); } catch (e) { eo = e.message; }
      try { seedP(); r = snap(Reim(st)); } catch (e) { er = e.message; }
    } else if (cfg.at === 'engine_register_funcs') {
      // int f(void): straight-line stores of fixed funcptrs into (*glob)+offsets; ret 1.
      const stE = Memory.alloc(0x140); _keep.push(stE);
      const obs = cfg.observe_offs || [];
      const seedE = function () { for (let z = 0; z < 0x140; z += 4) stE.add(z).writeU32(0xCCCCCCCC); ptr(cfg.glob).writePointer(stE); };
      const snap = function (ret) { return (ret >>> 0) + ':' + obs.map(function (o2) { return (stE.add(o2 | 0).readU32() >>> 0).toString(16); }).join('|'); };
      try { seedE(); o = snap(Orig()); } catch (e) { eo = e.message; }
      try { seedE(); r = snap(Reim()); } catch (e) { er = e.message; }
    } else if (cfg.at === 'eax_struct_deref_write') {
      // void f(EAX=s): index-gated propagation of table[idx] into 12 deref chains.
      const s = (cfg.scenarios || [])[t] || { idx: 0, prev: -1 };
      const sbuf = Memory.alloc(0x200);
      const P1 = Memory.alloc(0x40), P2 = Memory.alloc(0x40), P3 = Memory.alloc(0x40), P4 = Memory.alloc(0x40);
      _keep.push(sbuf, P1, P2, P3, P4);
      const offs = [0x108, 0x10c, 0x110, 0x114, 0x118, 0x11c, 0x120, 0x124, 0x128, 0x12c, 0x178, 0x174];
      const mkT = function (target) {
        const tr = Memory.alloc(Process.pageSize); _keep.push(tr);
        tr.writeU8(0xB8); tr.add(1).writePointer(sbuf);   // mov eax, sbuf
        tr.add(5).writeU8(0xE9); tr.add(6).writeS32(target.sub(tr.add(10)).toInt32()); // jmp
        Memory.protect(tr, 16, 'rwx');
        return new NativeFunction(tr, 'void', [], 'mscdecl');
      };
      const seedX = function () {
        for (let z = 0; z < 0x200; z += 4) sbuf.add(z).writeU32(0);
        offs.forEach(function (o2) { sbuf.add(o2).writePointer(P1); });
        P1.add(0x18).writePointer(P2);
        P2.add(0x20).writePointer(P3);
        P3.writePointer(P4);                    // *P3 = P4
        P4.add(4).writeU32(0xEEEEEEEE);         // target field sentinel
        sbuf.add(0x1b4).writeS32(s.idx | 0);
        sbuf.add(0x1b8).writeS32(s.prev | 0);
        ptr(cfg.tbl).add((s.idx | 0) * 4).writeU32((0xD00D0000 | (s.idx & 0xffff)) >>> 0);   // table[idx]
      };
      const runX = function (CALL) {
        seedX(); CALL();
        return (sbuf.add(0x1b8).readU32() >>> 0).toString(16) + '|' + (P4.add(4).readU32() >>> 0).toString(16);
      };
      try { o = runX(mkT(ptr(cfg.rva))); } catch (e) { eo = e.message; }
      try { r = runX(mkT(reim)); } catch (e) { er = e.message; }
    } else if (cfg.at === 'particle_pool_alloc') {
      // void f(int* a1, int a2): particle-pool slot allocator at cfg.glob (10 slots stride 0x24).
      const s = (cfg.scenarios || [])[t] || { used: [], pris: [] };
      const pool = ptr(cfg.glob);
      const a1 = Memory.alloc(0x10); _keep.push(a1);
      const seedPP = function () {
        for (let z = 0; z < 10 * 0x24; z += 4) pool.add(z).writeU32(0xEEEEEEEE);
        for (let i = 0; i < 10; i++) pool.add(i * 0x24).writeU32(0);          // all free
        (s.used || []).forEach(function (i) { pool.add(i * 0x24).writeU32(1); });   // mark used
        (s.pris || []).forEach(function (pv, i) { pool.add(i * 0x24 + 0x1c).writeU32(pv >>> 0); });
        a1.writeU32(0x111); a1.add(4).writeU32(0x222); a1.add(8).writeU32(0x333);
      };
      const snap = function () {
        let p = [];
        [0, 1, 9].forEach(function (i) { p.push((pool.add(i * 0x24).readU32() >>> 0).toString(16)); p.push((pool.add(i * 0x24 + 4).readU32() >>> 0).toString(16)); });
        return p.join('|');
      };
      try { seedPP(); Orig(a1, 0x444); o = snap(); } catch (e) { eo = e.message; }
      try { seedPP(); Reim(a1, 0x444); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'thunk_node_write') {
      // void f(p, a2, a3): adjustor thunk -> C3; node=(*glob)[p[0x14]]; node[0xa4]=a2; node[0xa8]=a3; node[0x40]|=0x10000000.
      const sc = (cfg.scenarios || [])[t] || { a2: 0, a3: 0 };
      const p = Memory.alloc(0x40), table = Memory.alloc(0x40), node = Memory.alloc(0x100);
      _keep.push(p, table, node);
      const seedT = function () {
        for (let z = 0; z < 0x40; z += 4) p.add(z).writeU32(0);
        p.add(0x14).writeU32(0);                 // index/offset 0
        ptr(cfg.glob).writePointer(table);       // *0x7dc57c = table
        table.writePointer(node);                // table[0] = node
        for (let z = 0; z < 0x100; z += 4) node.add(z).writeU32(0xEEEEEEEE);
        node.add(0x40).writeU32(5);              // seed for the OR
      };
      const snap = function () {
        return (node.add(0xa4).readU32() >>> 0).toString(16) + '|' +
               (node.add(0xa8).readU32() >>> 0).toString(16) + '|' +
               (node.add(0x40).readU32() >>> 0).toString(16);
      };
      try { seedT(); Orig(p, sc.a2 >>> 0, sc.a3 >>> 0); o = snap(); } catch (e) { eo = e.message; }
      try { seedT(); Reim(p, sc.a2 >>> 0, sc.a3 >>> 0); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'thunk_field_copy') {
      // void f(p, out): adjustor thunk -> C3 copy. s=p[0x18]; copy s[0x24] dwords from *(s[0x20]) to out.
      const sc = (cfg.scenarios || [])[t] || { pat: 0xA0000000, count: 4 };
      const cnt = sc.count | 0;
      const p = Memory.alloc(0x40), sp = Memory.alloc(0x40), src = Memory.alloc(0x80), out = Memory.alloc(0x80);
      _keep.push(p, sp, src, out);
      const seedC = function () {
        for (let z = 0; z < 0x40; z += 4) { p.add(z).writeU32(0); sp.add(z).writeU32(0); }
        for (let z = 0; z < 0x80; z += 4) out.add(z).writeU32(0xEEEEEEEE);
        p.add(0x18).writePointer(sp);
        sp.add(0x24).writeU32(cnt >>> 0);
        sp.add(0x20).writePointer(src);
        for (let k = 0; k < cnt; k++) src.add(k * 4).writeU32(((sc.pat >>> 0) | k) >>> 0);
      };
      const snap = function () { let a = []; for (let k = 0; k < cnt; k++) a.push((out.add(k * 4).readU32() >>> 0).toString(16)); return a.join('|'); };
      try { seedC(); Orig(p, out); o = snap(); } catch (e) { eo = e.message; }
      try { seedC(); Reim(p, out); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'thunk_cond_or') {
      // uint f(p, a2, a3): adjustor thunk -> C3 cond-or. s=p[0x18]; if(a3) s[8]|=a2; return s[8].
      const sc = (cfg.scenarios || [])[t] || { a2: 0, a3: 0, seed: 0 };
      const p = Memory.alloc(0x40), sp = Memory.alloc(0x40); _keep.push(p, sp);
      const seedC = function () { for (let z = 0; z < 0x40; z += 4) { p.add(z).writeU32(0); sp.add(z).writeU32(0); } p.add(0x18).writePointer(sp); sp.add(8).writeU32(sc.seed >>> 0); };
      const snap = function (ret) { return (ret >>> 0).toString(16) + '|' + (sp.add(8).readU32() >>> 0).toString(16); };
      try { seedC(); o = snap(Orig(p, sc.a2 >>> 0, sc.a3 >>> 0)); } catch (e) { eo = e.message; }
      try { seedC(); r = snap(Reim(p, sc.a2 >>> 0, sc.a3 >>> 0)); } catch (e) { er = e.message; }
    } else if (cfg.at === 'thunk_list_count') {
      // uint f(p): adjustor thunk -> C3 count circular list at (p+0xc) (linked +4, sentinel=head).
      const sc = (cfg.scenarios || [])[t] || { n: 0 };
      const n = sc.n | 0;
      const p = Memory.alloc(0x40); _keep.push(p);
      const d = p.add(0xc);
      const nodes = []; for (let k = 0; k < n; k++) { const nd = Memory.alloc(0x20); _keep.push(nd); nodes.push(nd); }
      const seedL = function () {
        for (let z = 0; z < 0x40; z += 4) p.add(z).writeU32(0);
        if (n === 0) { d.add(4).writePointer(d); }
        else {
          d.add(4).writePointer(nodes[0]);
          for (let k = 0; k < n; k++) nodes[k].add(4).writePointer(k < n - 1 ? nodes[k + 1] : d);
        }
      };
      try { seedL(); o = (Orig(p) >>> 0).toString(); } catch (e) { eo = e.message; }
      try { seedL(); r = (Reim(p) >>> 0).toString(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'thunk_float_sub') {
      // void f(idx, float fval): adjustor thunk -> C3 *(float*)(tbl+idx*stride+field_off) -= fval.
      const sc = (cfg.scenarios || [])[t] || { idx: 0, seed: 0, fval: 0 };
      const entry = ptr(cfg.tbl).add((sc.idx | 0) * (cfg.stride | 0)).add(cfg.field_off | 0);
      const seedF = function () { entry.writeFloat(sc.seed); };
      try { seedF(); Orig(sc.idx >>> 0, sc.fval); o = (entry.readU32() >>> 0).toString(16); } catch (e) { eo = e.message; }
      try { seedF(); Reim(sc.idx >>> 0, sc.fval); r = (entry.readU32() >>> 0).toString(16); } catch (e) { er = e.message; }
    } else if (cfg.at === 'bounded_thunk_orflag') {
      // int f(idx, a2): bounds-checked adjustor thunk -> C3; s=tbl[idx]; if(a2) s[2]|=4.
      const sc = (cfg.scenarios || [])[t] || { idx: 5, a2: 0, s2: 0 };
      const s = Memory.alloc(0x20); _keep.push(s);
      const slot = ptr(cfg.tbl).add(5 * 4);  // fixed table[5] -> s
      const seedB = function () { slot.writePointer(s); for (let z = 0; z < 0x20; z += 4) s.add(z).writeU32(0); s.add(2).writeU8(sc.s2 & 0xff); };
      const snap = function () { return (s.add(2).readU8()).toString(16); };
      try { seedB(); Orig(sc.idx >>> 0, sc.a2 >>> 0); o = snap(); } catch (e) { eo = e.message; }
      try { seedB(); Reim(sc.idx >>> 0, sc.a2 >>> 0); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'bitfield_range_set') {
      // void f(uint8** pbuf, uint startbit, uint nbits, int fill): buf=*pbuf; set bits
      // [startbit,startbit+nbits) of bit-array buf to (fill!=0). 24-byte buf seeded to
      // cfg.scenarios[t].seed; same pbuf re-seeded both sides. observe all 24 bytes hex.
      const sc = (cfg.scenarios || [])[t] || { startbit: 0, nbits: 8, fill: 1, seed: 0x00 };
      const N = 24;
      const pbuf = Memory.alloc(4), buf = Memory.alloc(N); _keep.push(pbuf, buf);
      pbuf.writePointer(buf);
      const seedB = function () { for (let z = 0; z < N; z++) buf.add(z).writeU8(sc.seed & 0xff); };
      const snap = function () { let h = ''; for (let z = 0; z < N; z++) { const b = buf.add(z).readU8(); h += (b < 16 ? '0' : '') + b.toString(16); } return h; };
      try { seedB(); Orig(pbuf, sc.startbit >>> 0, sc.nbits >>> 0, sc.fill >>> 0); o = snap(); } catch (e) { eo = e.message; }
      try { seedB(); Reim(pbuf, sc.startbit >>> 0, sc.nbits >>> 0, sc.fill >>> 0); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'esi_struct_init') {
      // void f(ESI=p): struct initializer with pointer in register ESI. Orig driven via an
      // esi-trampoline (mov esi,buf; jmp rva); Reim is plain __cdecl(void* p). Same buf
      // re-seeded both sides; observe the full buffer as hex dwords.
      const sc = (cfg.scenarios || [])[t] || { seed: 0xCC };
      const N = (cfg.bufsize | 0) || 0x6c;
      const buf = Memory.alloc(N); _keep.push(buf);
      const seedB = function () { for (let z = 0; z < N; z++) buf.add(z).writeU8(sc.seed & 0xff); };
      const snap = function () { let h = ''; for (let z = 0; z < N; z += 4) h += ('00000000' + (buf.add(z).readU32() >>> 0).toString(16)).slice(-8); return h; };
      const tr = Memory.alloc(Process.pageSize); _keep.push(tr);
      tr.writeU8(0xBE); tr.add(1).writePointer(buf);                                       // mov esi, buf
      tr.add(5).writeU8(0xE9); tr.add(6).writeS32(ptr(cfg.rva).sub(tr.add(10)).toInt32()); // jmp rva
      Memory.protect(tr, 16, 'rwx');
      const OrigT = new NativeFunction(tr, 'void', []);
      try { seedB(); OrigT(); o = snap(); } catch (e) { eo = e.message; }
      try { seedB(); Reim(buf); r = snap(); } catch (e) { er = e.message; }
    } else if (cfg.at === 'reg_scalar_compute') {
      // fn with scalar register args: trampoline `mov eax,a; mov ecx,c; mov edx,d;
      // jmp target` per test t=[a,c(,d)], NativeFunction returns ret (EAX). Varying
      // a/c across tests exercises all branches. Reimpl is naked __asm reading regs.
      const tv = (typeof t === 'object' && t !== null) ? t : [t];
      const va = tv[0] | 0, vc = (tv.length > 1 ? tv[1] : 0) | 0, vd = (tv.length > 2 ? tv[2] : 0) | 0;
      const mkS = function (target) {
        const tr = Memory.alloc(Process.pageSize); _keep.push(tr);
        tr.writeU8(0xB8); tr.add(1).writeS32(va);              // mov eax, a
        tr.add(5).writeU8(0xB9); tr.add(6).writeS32(vc);       // mov ecx, c
        tr.add(10).writeU8(0xBA); tr.add(11).writeS32(vd);     // mov edx, d
        tr.add(15).writeU8(0xE9); tr.add(16).writeS32(target.sub(tr.add(20)).toInt32()); // jmp
        Memory.protect(tr, 32, 'rwx');
        return new NativeFunction(tr, 'uint32', [], 'mscdecl');
      };
      // PER-SIDE CONVENTION: original is reg-arg (EAX/ECX[/EDX]) -> reg-trampoline.
      // Reimpl is plain __cdecl(a,c[,d]) (stack args) -> the standard Reim handle
      // (nargs = ['uint32',...]). Compares the computed RESULT, not the ABI, so a
      // naked value-return (ret-imbalance-prone) is avoided.
      try { o = mkS(ptr(cfg.rva))() >>> 0; } catch (e) { eo = e.message; }
      try { r = (tv.length > 2 ? Reim(va, vc, vd) : Reim(va, vc)) >>> 0; } catch (e) { er = e.message; }
    } else if (cfg.at === 'ptr_buffer_op') {
      // void fn(ptr p): memset / memcpy-from-abs over a buffer at p. Alloc a
      // buffer (cfg.buf_dwords), fill a sentinel, call fn(buf), snapshot
      // cfg.observe_offs. Reimpl is __cdecl(p) so it sees the SAME buffer.
      const offs = cfg.observe_offs || [];
      const bd = (cfg.buf_dwords | 0) || 0xC00;
      const pbuf = Memory.alloc(bd * 4); _keep.push(pbuf);
      const runP = function (CALL) {
        for (let z = 0; z < bd; z++) pbuf.add(z * 4).writeU32(0xA5A5A5A5);
        CALL(pbuf);
        return offs.map(function (o2) { return pbuf.add(o2 | 0).readU32() >>> 0; }).join('|');
      };
      try { o = runP(Orig); } catch (e) { eo = e.message; }
      try { r = runP(Reim); } catch (e) { er = e.message; }
    } else if (cfg.at === 'eax_ecx_insert') {
      // fn(EAX=container, ECX=item): cross-link insert. Build a trampoline
      // `mov eax,bufA ; mov ecx,bufC ; jmp target` and call it (no stack args).
      // Seed both bufs (cfg.eax_seed / cfg.ecx_seed = [{off,val}]; default 0),
      // snapshot cfg.eax_observe / cfg.ecx_observe offsets in both + ret. Reimpl is
      // naked __asm reading EAX+ECX, so it observes the SAME two buffers.
      const eobs = cfg.eax_observe || [], cobs = cfg.ecx_observe || [];
      const eseed = cfg.eax_seed || [], cseed = cfg.ecx_seed || [];
      const aobs = cfg.abs_observe || [];   // absolute globals the fn writes (e.g. EDX-indexed tables)
      const hasEdx = (cfg.edx_val !== undefined && cfg.edx_val !== null);
      const bufA = Memory.alloc(0x400), bufC = Memory.alloc(0x400); _keep.push(bufA, bufC);
      const mkT2 = function (target) {
        const tr = Memory.alloc(Process.pageSize); _keep.push(tr);
        tr.writeU8(0xB8); tr.add(1).writePointer(bufA);        // mov eax, bufA
        tr.add(5).writeU8(0xB9); tr.add(6).writePointer(bufC); // mov ecx, bufC
        let p = 10;
        if (hasEdx) { tr.add(p).writeU8(0xBA); tr.add(p + 1).writeS32(cfg.edx_val | 0); p += 5; } // mov edx, imm32
        tr.add(p).writeU8(0xE9);                                // jmp target
        tr.add(p + 1).writeS32(target.sub(tr.add(p + 5)).toInt32());
        Memory.protect(tr, 32, 'rwx');
        return new NativeFunction(tr, 'uint32', [], 'mscdecl');
      };
      const seedBoth = function () {
        for (let z = 0; z < 0x400; z += 4) { bufA.add(z).writeU32(0); bufC.add(z).writeU32(0); }
        eseed.forEach(function (s) { bufA.add(s.off | 0).writeU32(s.val >>> 0); });
        cseed.forEach(function (s) { bufC.add(s.off | 0).writeU32(s.val >>> 0); });
        aobs.forEach(function (a2) { ptr(a2).writeU32(0); });   // reset abs globals before each call
      };
      const snap = function (rv) {
        const a = eobs.map(function (o2) { return bufA.add(o2 | 0).readU32() >>> 0; });
        const c = cobs.map(function (o2) { return bufC.add(o2 | 0).readU32() >>> 0; });
        const g = aobs.map(function (a2) { return ptr(a2).readU32() >>> 0; });
        return 'A[' + a.join(',') + '] C[' + c.join(',') + '] G[' + g.join(',') + '] ret=' + (rv >>> 0);
      };
      try { seedBoth(); const rv = mkT2(ptr(cfg.rva))(); o = snap(rv); } catch (e) { eo = e.message; }
      try { seedBoth(); const rv = mkT2(reim)(); r = snap(rv); } catch (e) { er = e.message; }
    } else if (cfg.at === 'thiscall_struct_from_table') {
      // __thiscall void/int fn(this): idx=this[idx_off]; read record at
      // tbl+idx*tbl_stride; write/derive into this fields. Seed a scratch `this`
      // (idx_off=cfg.idx) + the global table record (seed_tbl_n dwords, varied by
      // test), call, snapshot cfg.observe_offs. Reimpl is __cdecl(self) so it sees
      // the SAME this buffer (passed on the stack); original gets it in ECX.
      const io5 = cfg.idx_off | 0, gi5 = (cfg.idx | 0) || 5, ts5 = cfg.tbl_stride | 0;
      const sn5 = cfg.seed_tbl_n | 0, offs5 = cfg.observe_offs || [];
      const tbl5 = ptr(cfg.tbl);
      const sbuf = Memory.alloc(0x400); _keep.push(sbuf);
      const runT = function (CALL) {
        for (let z = 0; z < 0x400; z += 4) sbuf.add(z).writeU32(0);
        sbuf.add(io5).writeU32(gi5 >>> 0);
        for (let k = 0; k < sn5; k++) tbl5.add(gi5 * ts5 + k * 4).writeU32((t ^ (0x100 * k)) >>> 0);
        CALL(sbuf);
        return offs5.map(function (o2) { return sbuf.add(o2).readU32() >>> 0; }).join('|');
      };
      try { o = runT(Orig); } catch (e) { eo = e.message; }
      try { r = runT(Reim); } catch (e) { er = e.message; }
    } else if (cfg.at === 'deref_table_read') {
      // return (*p1)[i]. Seed an array behind p1 with distinct values; non-degenerate.
      const span = (cfg.span | 0) || 16;
      const arr = Memory.alloc(span * 4), A = Memory.alloc(4); _keep.push(arr, A);
      for (let k = 0; k < span; k++) arr.add(k * 4).writeU32((0xC0DE0000 | k) >>> 0);
      A.writePointer(arr);
      try { o = Orig(A, t >>> 0) >>> 0; } catch (e) { eo = e.message; }
      try { r = Reim(A, t >>> 0) >>> 0; } catch (e) { er = e.message; }
    }
    res.push({ i: i, t: '' + t, o: (o === null ? null : '' + o), r: (r === null ? null : '' + r),
               match: (eo === null && er === null && o !== null && o === r), eo: eo, er: er });
  }
  return { b0: '0x' + b0.toString(16), reim: reim.toString(), results: res };
};
"""


def run(name):
    h = HR.HOOKS[name]
    at = h['arg_type']
    if at not in PURE_LEAF_ARGTYPES:
        print(f"REFUSED: arg_type {at!r} is not a state-independent pure-leaf type "
              f"({sorted(PURE_LEAF_ARGTYPES)}). Use run_diff.py against a booted game.")
        return None
    cfg = {'rva': h['rva'], 'export': h['export'], 'ret': h['signature']['ret'], 'at': at,
           'tgt': h.get('target_global'), 'tests': h.get('path1_tests', []),
           'observe': h.get('observe'), 'seed_table': h.get('seed_table'),
           'outer_off': h.get('outer_off'), 'inner_off': h.get('inner_off'),
           'span': h.get('span'), 'field_off': h.get('field_off'),
           'capacity': h.get('capacity'), 'insert_rva': h.get('insert_rva'),
           'build_keys': h.get('build_keys'), 'init_buf': h.get('init_buf'),
           'init_top': h.get('init_top'), 'stride': h.get('stride'),
           'set_idx': h.get('set_idx'), 'len': h.get('len'), 'bound': h.get('bound'),
           'mult': h.get('mult'), 'bound2': h.get('bound2'),
           'off0': h.get('off0'), 'off1': h.get('off1'), 'offf': h.get('offf'),
           'idxtbl': h.get('idxtbl'), 'tscale': h.get('tscale'),
           'gate': h.get('gate'), 'gatemax': h.get('gatemax'),
           'idx': h.get('idx'), 'shape': h.get('shape'), 'writes': h.get('writes'),
           'bit': h.get('bit'), 'gateval': h.get('gateval'), 'seedvecs': h.get('seedvecs'),
           'count': h.get('count'), 'aux': h.get('aux'),
           'basePtr': h.get('basePtr'), 'nargs4': h.get('nargs4'),
           'nscalar': h.get('nscalar'), 'seed_byte': h.get('seed_byte'),
           'gate_off': h.get('gate_off'), 'val_off': h.get('val_off'),
           'rec_off': h.get('rec_off'), 'out_off': h.get('out_off'), 'thr': h.get('thr'),
           'add': h.get('add'), 'seedf': h.get('seedf'),
           'ret_tbl': h.get('ret_tbl'), 'ret_stride': h.get('ret_stride'),
           'stride_dw': h.get('stride_dw'), 'passthrough_arg': h.get('passthrough_arg'),
           'mask': h.get('mask'), 'glob': h.get('glob'), 'p1_off': h.get('p1_off'),
           'arg2_kind': h.get('arg2_kind'), 'arg2_dwords': h.get('arg2_dwords'),
           'seed': h.get('seed'),
           'idx_off': h.get('idx_off'), 'tbl': h.get('tbl'),
           'tbl_stride': h.get('tbl_stride'), 'seed_tbl_n': h.get('seed_tbl_n'),
           'tbl_base': h.get('tbl_base'), 'tbl_count': h.get('tbl_count'),
           'obs_addr': h.get('obs_addr'), 'observe_addrs': h.get('observe_addrs'),
           'rec_base': h.get('rec_base'), 'rec_stride': h.get('rec_stride'),
           'struct_size': h.get('struct_size'),
           'observe_offs': h.get('observe_offs'),
           'conv_orig': h.get('conv_orig'), 'conv_reim': h.get('conv_reim'),
           'eax_seed': h.get('eax_seed'), 'ecx_seed': h.get('ecx_seed'),
           'eax_observe': h.get('eax_observe'), 'ecx_observe': h.get('ecx_observe'),
           'edx_val': h.get('edx_val'), 'abs_observe': h.get('abs_observe'),
           'mid_off': h.get('mid_off'), 'abs_ranges': h.get('abs_ranges'),
           'seed_a': h.get('seed_a'), 'seed_b': h.get('seed_b'), 't_bits': h.get('t_bits'),
           'seed_pairs': h.get('seed_pairs'), 'seed_sets': h.get('seed_sets'),
           'grid': h.get('grid'), 'tbl1': h.get('tbl1'), 'tbl2': h.get('tbl2'),
           'buf_dwords': h.get('buf_dwords'), 'out_observe': h.get('out_observe'),
           'link_off': h.get('link_off'), 'p_seed': h.get('p_seed'),
           'observe_p': h.get('observe_p'), 'observe_sub': h.get('observe_sub'),
           'copy_dwords': h.get('copy_dwords'),
           'ptr_array': h.get('ptr_array'), 'depth_global': h.get('depth_global'),
           'depth_idx': h.get('depth_idx'), 'ctx_seed_off': h.get('ctx_seed_off'),
           'observe_globals': h.get('observe_globals'), 'seed_globals': h.get('seed_globals'),
           'aTbl': h.get('aTbl'), 'bTbl': h.get('bTbl'), 'fTbl': h.get('fTbl'),
           'scenarios': h.get('scenarios'), 'bufsize': h.get('bufsize'),
           'iStride': h.get('iStride'), 'jStride': h.get('jStride'), 'regionOff': h.get('regionOff'),
           't1Tbl': h.get('t1Tbl'), 't2Tbl': h.get('t2Tbl'), 't3Tbl': h.get('t3Tbl'), 't3Stride': h.get('t3Stride'),
           'rangeTbl': h.get('rangeTbl'), 'deltaTbl': h.get('deltaTbl'),
           'baseA': h.get('baseA'), 'baseB': h.get('baseB'), 'recStride': h.get('recStride'),
           'glob_a': h.get('glob_a'), 'glob_b': h.get('glob_b'),
           'out_size': h.get('out_size'), 'out_argpos': h.get('out_argpos'),
           'argkinds': h.get('argkinds'), 'fold_ret': h.get('fold_ret'),
           'asi': ASI}
    # SUSPENDED-SPAWN MODE (2026-06-14): frida.spawn leaves the process suspended at
    # the entry point. We force-call the leaf on Frida's own thread via rpc and NEVER
    # resume — so MASHED's main thread never runs its CRT/RenderWare/d3d9 boot path.
    # This bypasses the GPU-thrash wedge (which faults only once the main thread
    # touches the wedge-corrupted heap during boot), so pure-leaf diffs work even when
    # a full boot crashes pre-window. Pure leaves don't allocate heap, so a possibly-
    # corrupted loader heap doesn't affect them. Falls back to Popen+attach if spawn
    # is unavailable.
    device = frida.get_local_device()
    pid = None
    session = None
    try:
        pid = device.spawn([EXE], cwd=os.path.join(ROOT, 'original'),
                           env={**os.environ, 'MASHED_RE_NO_AUTO_HOOK': '1'})
        session = device.attach(pid)
    except Exception as e:
        print(f"  spawn/attach failed: {e}")
        if pid is not None:
            try: device.kill(pid)
            except Exception: pass
        return None
    out = None
    try:
        sc = session.create_script(SRC); sc.load()
        out = sc.exports_sync.diff(cfg)   # runs on Frida thread; main stays suspended
    except Exception as e:
        print("  script error:", e)
    finally:
        try: device.kill(pid)
        except Exception: pass
    if not out:
        return None
    if out.get('error'):
        print("  HARNESS ERROR:", out['error']); return None
    rs = out['results']
    mism = [x for x in rs if not x['match']]
    print(f"  b0={out['b0']} reim={out['reim']} cases={len(rs)} mismatches={len(mism)}")
    # write evidence CSV (early-window lane)
    os.makedirs(LOG, exist_ok=True)
    csv = os.path.join(LOG, f"diff_{name}.csv")
    with open(csv, 'w') as f:
        f.write("idx,input,original,reimpl,match\n")
        for x in rs:
            f.write(f"{x['i']},{x['t']},{x['o']},{x['r']},{x['match']}\n")
    print(f"  evidence: {csv}")
    for m in mism[:4]:
        print("   MISM", m)
    return len(mism) == 0 and len(rs) > 0


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("usage: early_window_leaf_diff.py <hook_name>  (PURE LEAVES ONLY)")
        sys.exit(2)
    n = sys.argv[1]
    print(f"hook: {n}  (early-window pure-leaf diff — no menu boot required)")
    ok = run(n)
    print("VERDICT:", "GREEN" if ok else ("RED" if ok is False else "ERROR/REFUSED"))
    sys.exit(0 if ok else 1)
