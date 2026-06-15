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
    'int2_scalar',  # pure function of TWO int args (no memory) — tests are [p1,p2] pairs
    'deref_field_write',  # fn(ptr p1, u32 p2): *(*(p1+outer_off)+inner_off)=p2 — harness allocs+links buffers
    'deref_table_read',   # fn(ptr p1, u32 i): return (*p1)[i] — harness allocs+seeds an array behind p1
    'const_return',       # fn(): return <fixed constant> — no input, no state; call + compare
    'global_field_read',  # fn(): return *(*(global)+field_off) — point global at a seeded buffer
    'float_table_read',   # fn(i): return *(float*)(base+i*stride) — seed_table bits read as float
    'eax_implicit_void',  # void fn() with `this` in EAX — trampoline sets EAX=buf, check observed fields
    'pool_insert_snapshot',  # fn(mgr_ptr, key): pool/list insert — reset+call+full-state snapshot diff
    'pool_remove_snapshot',  # fn(mgr_ptr, key): pool/list remove — build list via orig insert, then remove, snapshot
    'table_clear',           # void fn(i): zero an absolute table slot [target_global + i*4]
    'ptr_fields_clear',      # void fn(ptr): zero fields of a struct arg; check observe offsets
    'stack_pop_snapshot',    # fn(stk): array-stack pop {top,cap,buf}; reset+call+snapshot+ret
    'stack_push_snapshot',   # fn(stk,val): array-stack push; reset+call+snapshot+ret
    'ptr_table_field_read',  # fn(i): return *(*(tgt))[i] + field_off  (pointer-to-table + field)
    'indexed_table_set',     # void fn(i,val): *(tgt + i*stride) = val (fixed i=set_idx, vary val)
    'range_init',            # void fn(): writes a contiguous global range [tgt, tgt+len) — snapshot whole range
    'cond_global_set',       # void fn(v): if (v==0 || *tgt==0) *tgt=v — tests are [seed,arg] pairs (seed *tgt first)
    'ptr_out_table_get',     # u32 fn(out_ptr,idx): if(idx>=bound) return 0; out[0..n-1]=*(u32*)(base+idx*stride+j*4); return 1
    'idx2_table_get',        # u32 fn(out_ptr,i1,i2): if(i1>=bound||i2>=bound2) return 0; *out=*(u32*)(base+(i1*mult+i2)*stride); return 1
    'cond_table_get',        # u32 fn(idx): rec=base+idx*stride; return *(rec+offf) ? *(rec+off1) : *(rec+off0)
    'ptr_compute_get',       # u32 fn(out,idx): if(idx>=bound) return 0; t=*(u32*)(idxtbl+idx*stride); *out=base+idx*stride+t*tscale; return 1
    'eq_predicate_get',      # u32 fn(p1,p2): if(*(int*)gate<gatemax && p2>=0) return tbl[p1*stride]==tbl[p2*stride]?1:0; return 0
    'table_ret_ptrout',      # u32 fn(idx,out): addr=base+idx*stride; if(out) *out=*(u32*)(addr+off0); return *(u32*)(addr+off1)
    'arg_scattered_globals', # void fn(arg): observe globals after call, vary arg (switch/branch setter -> distinct globals per arg)
    'global_indexed_float',  # float fn(): idx=*(int*)gate; return *(float*)(base+idx*stride) — single FLD, x87-safe
    'vec16_copy_set',        # u32 fn(idx,in): if(idx>=bound) return 0; copy n dwords from in to TWO contiguous regions at base+idx*stride
    'container_record_set',  # void fn(container,..): base=container[0],idx=container[2]; addr=base+idx*0x30; write args into addr+offs. shape p/f/pp
    'indexed_vec_set',       # void fn(idx,in): addr=base+idx*stride; if(in) write n dwords from in to addr+j*4 else zero them
    'indexed_bit_toggle',    # void fn(idx,set): flag=*(u32*)(base+idx*stride+field_off); set?flag|=bit:flag&=~bit; store. test=[idx,set,seed]
    'gated_int_predicate',   # u32 fn(arg): if(*(int*)gate==gateval) <switch membership> else 0. test=[arg,gateseed]
    'global4_bool_out',      # void fn(out): reads N globals at base[k], writes out[k]=predicate(base[k])?1:0. test indexes cfg.seedvecs
    'linear_scan_find',      # int fn(key): for k in [0,count@gate): if(*(int*)(base+k*stride)==key) return k; return -1. test=[key,placeAt]
    'gated_record_eq2',      # u32 fn(): g=*(int*)gate; rec=base+g*stride; return (*(rec+off0)==v0 && *(rec+off1)==v1)?1:0. test=[gidx,s0,s1]
    'indexed_const2_set',    # void fn(idx): *(u32*)(base+idx*stride+off0)=v0; *(u32*)(base+idx*stride+off1)=v1 (consts baked in reimpl)
    'global_switch_member',  # u32 fn(): reads *(int*)gate, returns switch-membership (1/0). seed gate; test = gate value
    'gated_args_to_globals', # void fn(p1..p6): if(*(int*)gate==0){ write args+consts to observe globals; conditional on aux }. test=[p1..p6,auxseed]
    'void_global_transition',# void fn(): if(*(int*)tgt==from) *(int*)tgt=to. test=[seed] -> seed *tgt, call, snapshot
    'two_global_predicate',  # u32 fn(): reads gate global + tgt global, returns membership (1/0). test=[g1seed,g2seed]
    'index_then_ptr_array',  # fn(args): comp=mult?a0*mult+a1:a0; idx=*(int*)(base_idx+comp*4); if(idx==-1) return 0; return *(u32*)(basePtr+idx*4). test=[a0(,a1),idxval]
    'flag_multibit',         # void fn(idx,b1,b2[,b3]): RMW flag word at base+idx*stride via reimpl bit logic. test=[idx,b1,b2(,b3),seed]
    'float_threshold_predicate', # u32 fn(idx): return (*(float*)(base+idx*stride) < *(float*)gate) ? 1 : 0. test=[idx,recordbits,threshbits]
    'deref_struct_set',          # void fn(ptr p, scalar...): writes deterministic values into fields of p. alloc+seed buffer, pass as p, call with nscalar args, snapshot observe offsets. test=[a0(,a1,a2)]
    'cond_deref_get',            # u32 fn(ptr p): if(*(u32*)(p+gate_off)) return *(u32*)(p+val_off); else 0. test=[gateval,val]
    'table_bool_predicate',      # u32 fn(i): if(bound>=0 && (int)i<=bound) return 0; return (*(u32*)(tgt+i*stride+off0) {==|!=} 0)?1:0. test=[idx,slotval]
    'global_swap',               # u32 fn(v): old=*tgt; *tgt=v; return old. test=[oldseed,v]
    'byte_args_to_globals',      # void fn(u8 a,u8 b,..): write byte args to byte globals (observe addrs). test=[a,b,..]
    'indexed_float_sq',          # void fn(i,float f): *(float*)(tgt+i*stride)=f*f. test=[idx,fval]
    'double_deref_vec3_get',     # void fn(i,out*): rec=*(tgt+i*stride); t=*(rec+rec_off); out[k]=*(t+out_off+k*4) for k<span. test=idx
    'global_float_predicate',    # u32 fn(): if(*(int*)gate==0) return 0; return (*(float*)thr <= *(float*)(*(tgt)+rec_off))?1:0. test=[gateval,thrbits,valbits]
    'double_deref_ptr_get',      # void fn(out*,idx): rec=*(tgt+idx*stride); *out=*(rec+rec_off)+add. test=idx
    'deref_float_field_rmw',     # void fn(ptr p,float f): *(float*)(p+field_off) -= f (seedf initial). test=fval
    'any_slot_nonzero',          # u32 fn(): return any(observe[k].addr nonzero)?1:0. test=index to set nonzero (-1=all zero)
    'arg_table_linear_search',   # int fn(key, table*, count): for i<count if(table[i*stride_dw]==key) return i; return -1. test=[key,placeAt,count]
    'global_float_step',         # void fn(float target): if(*tgt<target) *tgt+=step; if(target<*tgt) *tgt-=step. test=[seedbits,targetfloat]
    'struct_const_init',         # [u32] fn([passthrough,] ptr p): writes deterministic consts/computed to p's fields. alloc 0x400 buf, snapshot observe offsets (+ret if passthrough). test ignored
    'idx2_table_get_outlast',    # u32 fn(i1,i2,out*): if(i1>=bound||i2>=bound2) return 0; *out=*(u32*)(tgt+(i1*mult+i2)*stride); return 1. test=[i1,i2]
    'copy_arg_to_globals',       # void fn(ptr p): copy len(observe) dwords from p to observe[k].addr globals. test ignored
    'deref_byte_flag',           # void fn(ptr p, set): b=*(u8*)(p+field_off); set?b|=bit:b&=~bit; store. test=[set,seedbyte]
    'indexed_masked_get_out',    # void fn(i, out*): if(out) *out=*(u32*)(tgt+i*stride) & mask. test=[idx,slotval]
    'deref_p1field_glob_set',    # fn(p1[, p2/v]): base=*(u32*)(*(u32*)(p1+p1_off)+*(u32*)glob); reimpl writes base fields. seed glob=0, p1->atab->base, optional p2 in-ptr(n) or scalar; snapshot base observe. test ignored
    'global_table_linear_search',# int fn(key): if(key<=0) return -1; for i<count if(*(int*)(tgt+i*stride)==key) return i; return -1. test=[key,placeAt]
    'global_ptr_strided_clear',  # void fn(): for i in [0,len) step stride: *(u32*)(*(u32*)glob + i)=0. seed *glob=&buf, snapshot strided
    'struct_to_out_build',       # void fn(out*, p2): reads p2 fields (seeded per cfg.seed [{off,bits}]), writes out[0..span-1]. snapshot out. test ignored
    'store_be32',                # void fn(ptr p, u32 v): p[0..3] = big-endian bytes of v. test = v
    'load_be32',                 # int fn(ptr p): return big-endian u32 from p[0..3]. test = dword to seed
    'arg_to_global_ret',         # u32 fn(v): reimpl writes *tgt + returns (deterministic fn of v). seed tgt sentinel, call, check tgt+ret. test=v
    'indexed_global_field_read', # u32 fn(): return *(u32*)(*(u32*)tgt + *(u32*)glob + field_off). seed tgt->buf, glob->idx(nonzero), place test at buf[idx+field_off]. verifies base ptr + index global + field_off. test=value
    'indexed_global_field_write',# int fn(v): *(u32*)(*(u32*)tgt + *(u32*)glob + field_off)=v; return const/v. seed tgt->buf, glob->idx; call; observe slot|ret. test=v
    'thiscall_struct_from_table',# __thiscall fn(this): idx=this[idx_off]; record=tbl+idx*tbl_stride; derive into this fields. orig=thiscall(ECX), reimpl=__cdecl(self). seed this+table, snapshot observe_offs. test=value
    'eax_ecx_insert',            # fn(EAX=container, ECX=item): cross-link insert. trampoline sets EAX+ECX, seed both bufs (eax_seed/ecx_seed [{off,val}]), call, snapshot eax_observe/ecx_observe offsets in both + ret. reimpl naked __asm reading EAX+ECX. test ignored (single call)
    'ptr_buffer_op',             # void fn(ptr p): memset/memcpy-from-abs over a buffer at p. alloc buf_dwords*4, fill sentinel 0xA5A5A5A5, call fn(buf), snapshot observe_offs. test ignored
    'reg_scalar_compute',        # fn with SCALAR register args (EAX[,ECX,EDX]); returns value in EAX. trampoline sets regs per test [a,c(,d)], compare ret. varies inputs (hits all branches). reimpl naked __asm
    'eax_struct_stack_out',      # void fn(EAX=struct ptr, [esp+4]=out ptr): compute from struct fields into *out. trampoline `mov eax,sbuf; jmp`, NativeFunction(void,['pointer']) called w/ obuf. seed eax_seed into sbuf, observe out_observe in obuf. reimpl naked __asm
    'abstable_ptr_zero',         # void fn(idx): ptr=*(u32*)(abstable+idx*4); operate on a buffer at ptr. seed abstable[idx]=&scratch, fill sentinel, call fn(idx), observe scratch at observe_offs. test ignored
    'idx_table_out',             # void fn(idx, out*): *out = (value from static abs table indexed by idx). call w/ outbuf, observe out[0]. test=idx (varied -> non-degen from static table)
    'nested_struct_op',          # void fn(ptr p): p has a nested ptr p[link_off]=&sub; fn RMWs p fields + writes into sub. alloc p+sub, link, seed p_seed, fill sub sentinel, call, observe observe_p (in p) + observe_sub (in sub). reimpl __cdecl(p). test ignored
    'idx_src_abs_memcpy',        # void fn(idx, src): if(src) memcpy(tgt+idx*stride, src, copy_dwords*4). seed src distinct, reset abs dest, call, observe abs dest. test=idx. reimpl __cdecl(idx,src)
    'dll_unlink',                # void fn(list, node+0xc): doubly-linked-list unlink (node[0]=next,node[4]=prev,list[8]=head,list[0xc]=sentinel). build 3-node DLL fresh each side, remove middle, snapshot the relinked pointers, compare. faithful __cdecl port
    'circular_dll_search',       # u32 fn(p, key): circular list at p[0x10] (sentinel=p+0x10), node[0]=next, object=node-0x18; returns object whose addr==key, else 0. build 3-obj circular list, test=0 -> find obj1, else bogus. READ-ONLY (simple A/B). faithful __cdecl port
    'dll_get_nth',               # u32 fn(p, cont, idx): DLL get Nth (fwd from p[0x20]=head if idx<count/2 else bwd from p[0x24]=tail; count=cont[8]); node[0]=next,node[4]=prev; returns node-0x2c. build 5-node DLL, test=idx. READ-ONLY. faithful __cdecl port
    'indexed_global_2lvl',       # u32 fn(): base=*(u32*)tgt; idx=*(u32*)glob; edx=*(u32*)(base+idx+mid_off); return *(u32*)(base+edx*4+idx). seed tgt->buf, glob->idx(nonzero), place edx_val at buf[idx+mid_off], place test at buf[edx_val*4+idx]. verifies base+index global+mid_off+the *4 scale. test=value (varied)
    'indexed_bound_array_get',   # u32 fn(i): if(i>*(u32*)glob) return 0; cont=*(u32*)tgt; arr=*(u32*)(cont+field_off); return *(u32*)(arr+i*4). seed glob=large bound, tgt->contbuf, contbuf[field_off]->arrbuf, arrbuf[idx*4]=test; call fn(idx). verifies bound global + container ptr + field_off + array index. test=value (varied)
    'abs_ranges_setter',         # void fn(scalars...): writes to ABSOLUTE globals (no ptr args). Reset cfg.abs_ranges [{addr,dwords}] to 0, call fn(test scalars), snapshot the same ranges, compare. nscalar from cfg. test=[a0(,a1,a2)] (varied -> non-degen). reimpl __cdecl reads/writes the absolute globals directly
    'esi_global_search',         # u32 fn(ESI=key): linear-search a global table (count at glob, base tgt, stride) for entry[+0]==key; return an index-derived pointer or 0. ORIG called via `mov esi,key; jmp` trampoline; reimpl is __cdecl(key) reading the same globals (compares result, not ABI). Seed count=4, zero 4 entries, table[idx*stride]=key, key=0xC0DE0000|idx. test=idx (0..3 -> distinct matched addr -> non-degen)
    'indexed_global_idiv',       # u32 fn(arg): d=*(int*)(tgt+arg*stride); q=num/d (signed idiv); clamp. Seed d=test value at slot (fixed arg=idx), call fn(idx), compare ret. test=divisor (varied -> distinct quotients -> non-degen). reimpl __cdecl(arg) does signed C division (matches idiv trunc-toward-zero). avoid d=0
    'float_vec3_lerp_out',       # void fn(out*, a*, b*, float t): out[k] = a[k] + t*(b[k]-a[k]) per component (pure x87). Seed a,b vec3 (cfg.seed_a/seed_b = 3 u32 float-bit-patterns) + t (cfg.t_bits), call, observe out[0..2] as u32 bit patterns. Reimpl is VERBATIM naked __asm (bit-identical x87; a C float reimpl would round differently). seed-controlled -> non-degen by construction
    'float_2ptr_ret',            # float fn(a*, b*): pure float of two vec3 args (dot/clamp/etc.), returns ST0. Seed a,b from cfg.seed_pairs[t]={a:[3 floats],b:[3 floats]} (plain numbers), call, compare float return as u32 bit pattern. Reimpl is VERBATIM naked __asm (bit-identical x87). test=index into seed_pairs (varied -> non-degen). signature ret MUST be 'float'
    'float_planes6_predicate',   # u32 fn(obj*, point*): 6 planes at obj+0x94 stride 0x14 (x,y,z,w); for each test dot(plane.xyz,point.xyz)-plane.w vs -point.w; return 1 if all pass else 0. Seed from cfg.seed_sets[t]={point:[4 floats], planes:[6 lists of 4 floats]}; distinct plane.w verifies the 0x14 stride. Reimpl is VERBATIM naked __asm. test=index (results vary 0/1 -> non-degen)
    'eax_edi_out',               # void fn(EAX=v, EDI=out*): writes out[0..2] computed from v. ORIG via `mov eax,v; mov edi,outbuf; jmp` trampoline; REIMPL __cdecl(v, out) (compares out[0..2], not ABI). test=v (varied -> distinct outputs -> non-degen). use for integer fns whose magic-multiply == plain C integer division
    'grid_getter_multiout',      # u32 fn(i,j,out1,_,_,out2,out3): bounds-check i<*b1, j<*b2; idx=i*mul1+j; out1[0..1]=out1_t[*][idx*s12], out2[0..1]=out2_t[*][idx*s12], out3[0]=out3_t[*][(i*mul3+j)*s3]; return 1. Seed bounds large + the indexed table slots per cfg.grid; call with 3 out-bufs; observe outs+ret. test=marker (varied -> non-degen)
    'struct_ctor_big',           # void fn(p): deterministic constructor writing consts/self-relative-ptrs/zeros to p's fields (no live reads). Uses ONE shared sentinel buffer (cfg.buf_dwords) so self-relative pointer writes (p+const) compare equal between sides; snapshot cfg.observe offsets. For ctors too big for struct_const_init's 0x400 buf. test ignored
    'indexed_abs_dualout',       # u32 fn(i, out1*, out2*): if(i>=bound[immediate]) return 0; *out1=*(tbl1+i*stride); *out2=*(tbl2+i*stride); return 1. Seed the two abs table slots (incl .bss, which is committed/writable+zero) at i*stride, call, observe out1|out2|ret. test=in-bounds i (varied -> seeded markers differ -> non-degen)
    'dll_remove_count',          # void fn(list, node): list[0]--; A=node[0x24]; B=node[0x20]; *A=B; *(B+4)=A. (A pure-read search loop precedes it but converges either way; skipped by an EMPTY list list[4]=list+4.) Build list+node+A+B (shared bufs both sides so pointers compare equal), call, snapshot list[0]|A[0]|B[4]. test ignored
    'dll_insert_head',           # void fn(list, node): if(node[4]) unlink; node[4]=list; node[0xc]=list+8; old=*(list+8); node[8]=old; *(old+4)=&node[8]; *(list+8)=&node[8] (intrusive insert-at-head; links point to the &node[8] field). Test: empty list (*(list+8)=list+8) + node[4]=0 (skip unlink). Shared bufs; snapshot node[4]|node[8]|node[0xc]|*(list+8)|*(list+0xc). test ignored
    'global_ptrtable_match',     # u32 fn(arg1, arg2*): for idx in 0..3: e=*(tbl+idx*4); if(e && e[0xc]==1 && e[0x28]==arg1 && arg2[4]==idx) return 1; return 0. Seed tbl[2]=&entry (.bss), entry[0xc]=1, entry[0x28]=KEY, arg2[4]= (test0: 2 -> match -> 1 ; test1: 3 -> no match -> 0). non-degen via 1/0
    'global_rec_clear_ret',      # u32 fn(arg1, arg2): rec=*(glob)+arg2; if(rec[0xc]){ rec[0xc]=0; rec[8]=0; return arg1; } return 0. seed *glob=&buf, buf[idx+0xc]= (test0: nonzero -> zeroes + return arg1 ; test1: 0 -> return 0). observe buf[idx+8]|buf[idx+0xc]|ret. non-degen via ret + the zeroing
    'abs_scan_flag',             # void fn(): scans an absolute dword range; if any nonzero, sets flag global=0xff (else leaves it). Reset scan range (glob, span dwords) to 0 + flag(tgt) to a 0x11 sentinel; test0 seeds one nonzero at glob+idx (-> flag 0xff), test1 all-zero (-> flag stays 0x11). observe flag. non-degen 0xff vs 0x11
    'global_2level_list_search', # int fn(key): g=*(glob); node=g[4]; while(node){ e=node[0]; if(e && e[8]==key) return e[0xc]; node=node[8]; } return -1. seed *glob=&cont, cont[4]=&node, node[0]=&entry, entry[8]=KEY, entry[0xc]=RESULT, node[8]=0. test0 key=KEY (->RESULT), test1 key=KEY^1 (->-1). non-degen
    'arg_flag_branch_getter',    # u32 fn(arg*): pure getter; if(arg[0x20]){ p=arg[0]; f=p[0x40]&0xfffffff; return (arg[0x1c]&2)?f+arg[0x20]+4:f+arg[0x20]; } else return (arg[0x1c]&2)?arg+0x2c:arg+0x28. Seed arg fields per cfg.seed_sets[t]={c,flag,f}; SHARED arg+p bufs (so arg+0x2c/0x28 compare equal). test=index over the 4 branches (non-degen)
    'global_dll_insert_head',    # u32 fn(arg): node=arg+0x28; node[0]=*glob; node[4]=glob(addr const); (*glob)[4]=node; *glob=node; node[0xc]&=~1; return 1. seed *glob=&S, arg[0x34](=node[0xc])=0xF; shared arg+S bufs; snapshot node[0]|node[4]|S[4]|*glob|node[0xc]|ret. test ignored
    'global_fieldoff_clear',     # u32 fn(arg): V=*(glob); entry=*(arg+V); if(!entry) return 0; if(!entry[0]) return arg; e4=entry[4]; if(e4) arg[0x48]=e4; entry[4]=0; entry[0]=0; return arg. seed *glob=V(0x10), arg[V]=&entry(test0)/0(test1), entry[0]=1,entry[4]=0x77; shared arg+entry bufs; snapshot arg[0x48]|entry[0]|entry[4]|ret. non-degen via ret(arg/0)+arg[0x48]+the clearing
    'multi_state_list_setter',   # void fn(p): state=p[0x48]; state1: if(p[0x14]){ *(p[0x18])=p[0x14]; *(p[0x14]+4)=p[0x18]; p[0x18]=0; p[0x14]=0; } p[0x50]=3; state2: p[0x50]=6; state3: p[0x50]=5; else no change. Seed p[0x48]=state(test), p[0x50]=0x11 sentinel, state1 also p[0x14]=&A,p[0x18]=&B; shared p+A+B bufs; snapshot p[0x50]|p[0x14]|p[0x18]|A[4]|B[0]. test=state in {1,2,3,0} (non-degen)
    'byte_counter_struct',       # void fn(p): a=p[0]+1; if(a>=p[3]) a-=p[3]; p[0]=a&0xff; p[1]=(p[1]-1)&0xff. seed p[0],p[1],p[3] per cfg.seed_sets[t]={b0,b1,b3}; observe p[0]|p[1]. non-degen via varied seeds incl the wrap
    'arg_default_memcpy_abs',    # void fn(src): if(!src) src=glob(default); memcpy(tgt, src, copy_dwords*4). test0: src=&buf(markers)->dest=markers; test1: src=0->dest=copy of default. reset dest sentinel; snapshot dest dwords. non-degen across the two tests
    'byte_idx_table_bitclear',   # void fn(p): if(p[1]!=p[3]){ off=p[1]+p[0]; if(off>=p[3]) off-=p[3]; p[1]++; ptr=p[4]+off*0x14; } else ptr=0; *ptr&=~8. seed p[0]/p[1]/p[3](b1!=b3), p[4]=&tbl, tbl[off*0x14]=0xFF; observe tbl[off*0x14]|p[1]. (main path only; else-branch derefs null in both.) non-degen via off+p[1]++
    'struct_table5_search',      # u32 fn(p1, p2): if(!p1&&!p2) return 0; count=p1[0x1d0]; if(count<=0) return 0; tbl=p1[0x1d4]; search entries (5 bytes: dword key @+0, byte val @+4) BACKWARD from tbl+count*5-5 for *p2; return (u8)entry[4] or 0. seed count=4, distinct keys/vals, p2 key (test0 match->val, test1 nomatch->0). non-degen
    'circular_list_search_node', # u32 fn(list, key): walk circular list (sentinel=list, *list=first, node[0]=next); each node: if *(node-0x44)==key return node-0x4c; else next; return 0. build 3-object circular list (node=obj+0x4c, key at obj+8), test0 key=match->obj addr, test1 key=nomatch->0. shared bufs. non-degen
    'global_fieldoff_set',       # u32 fn(arg): V=*(glob); entry=*(arg+V); if(!entry) return 0; if(entry[0]) return arg; entry[4]=arg[0x48]; arg[0x48]=0x557b70; entry[0]=1; return arg. tests: t0 entry[0]=0(set), t1 entry null, t2 entry[0]=5(early ret). seed arg[0x48]=0x66; shared arg+entry bufs; snapshot entry[4]|arg[0x48]|entry[0]|ret. non-degen
    'eax_dest_memcpy_init',      # void fn(EAX=dest, src, arg2*, arg3, arg4): dest[0x54]=0; dest[0x48]=0; dest[0x4c]=arg3; dest[0x58]=1; dest[0x50]=arg4; memcpy(dest,src,16 dwords); dest[0x40]=*arg2. ORIG via `mov eax,dest; jmp` trampoline (4 stack args); REIMPL __cdecl(dest,src,arg2,arg3,arg4). seed src markers + a2 val; snapshot dest[0..0x3f]+[0x40,0x48,0x4c,0x50,0x54,0x58]. non-degen
    'struct_div_mod_compute',    # u32 fn(arg1,arg2,arg3,arg4,arg5): div=*(arg1[0x18]+arg4*0x28+0x20); q=arg2/div(unsigned); rem=arg2%div; *arg5=rem; return *(arg1[0x10]+arg3*0x20+0x1c)+arg1[0x20]*q+rem. seed arg1 ptrs+tables+mult per cfg.seed_sets[t]={val,div}; arg3=1,arg4=2 fixed; snapshot *arg5|ret. non-degen via varied val/div
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
      if (idx < bound) {
        for (let j = 0; j < n; j++) ptr(base).add(idx * stride + j * 4).writeU32((0xC0DE0000 | ((idx << 4) | j)) >>> 0);
      }
      const outO = Memory.alloc(0x40), outR = Memory.alloc(0x40); _keep.push(outO, outR);
      const rd = function (b) { const p = []; for (let j = 0; j < n; j++) p.push(b.add(j * 4).readU32() >>> 0); return p.join(','); };
      try { for (let j = 0; j < 16; j++) outO.add(j * 4).writeU32(0); const ro = Orig(outO, idx) >>> 0; o = rd(outO) + '|ret=' + ro; } catch (e) { eo = e.message; }
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
        ptr(cfg.tgt).add(sidx * sstride).writeU32(skey);
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
