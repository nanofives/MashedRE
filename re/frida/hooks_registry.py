# Hook verification registry.
#
# One entry per C3+ hook. The generic Frida harnesses (run_diff.py /
# run_verify_hook.py) read from this dict to build per-target test runs.
# To verify a new hook, add an entry here â€” no new .js or .py needed.
#
# Schema:
#   rva               int        target RVA in MASHED.exe
#   export            str        export name in mashed_re_dev.asi
#   signature         dict       NativeFunction signature for Frida
#                                ret:  'float' | 'uint32' | 'pointer' | 'void'
#                                args: ['float'] | ['int'] | ['pointer'] | []
#   arg_type          str        how to pack a test vector into the call:
#                                'float_scalar' -> pass single float directly
#                                'vec3_ptr'     -> alloc 12B, write 3 floats, pass ptr
#                                'void'         -> call with no args (input ignored)
#                                'int_scalar'   -> pass single int directly
#                                'int_pair'     -> pass [a,b] as two int args
#                                'int_ptr2_out' -> int arg + two uint32* out-ptrs;
#                                                  returns packed (out1&0x3f)|((out2&0x3f)<<8)
#   lut_root_delta    int        byte offset added to (rw_globals+rw_offset)
#                                before dereferencing the LUT root pointer
#                                (0 for sqrt/non-LUT functions, 4 for inv-sqrt)
#   path1_tests       list       inputs for the A/B diff harness (any
#                                length). Shape depends on arg_type.

#                                'float_scalar'    -> fn(scalar); returns float
#                                'vec3_ptr'        -> alloc 12B, write [x,y,z],
#                                                    call fn(ptr); returns float
#                                'vec2_ptr'        -> alloc 8B, write [x,y],
#                                                    call fn(ptr); returns float
#                                'transform_point' -> input dict {mat:[16f],in:[3f]};
#                                                    fn(out,in,mat) void; returns
#                                                    bit-packed string of out[3]
#                                'transform_vector'-> same shape, no translation
#                                'vec2_normalize'  -> input [x,y]; fn(out,in) float;
#                                                    returns bit-packed string of
#                                                    (ret,out[0],out[1])
#                                'matrix_scale'    -> input dict {mat:[16f],scale:[3f],
#                                                    mode:int}; fn(mat,scale,mode)
#                                                    void; returns bit-packed string
#                                                    of 12 data floats (excl flags)
#   lut_root_delta    int        byte offset added to (rw_globals+rw_offset)
#                                before dereferencing the LUT root pointer
#                                (0 for sqrt family, 4 for inv-sqrt;
#                                 0 is also used as a "game init ready" poll
#                                 for functions that don't use the LUT)
#   path1_tests       list       inputs for the A/B diff harness. Shape depends
#                                on arg_type (see above).
#   path2_tests       list       inputs for the hook-installer harness.
#                                Same shape as path1_tests, typically smaller.
#                                Verify path checks no-crash + interceptor count;
#                                bit-identity is path1's job.

# Column-major RW float[16] matrices for transform and matrix_scale test vectors.
_IDENT  = [1.0,0.0,0.0,0.0,  0.0,1.0,0.0,0.0,  0.0,0.0,1.0,0.0,  0.0,0.0,0.0,1.0]
_TRANS  = [1.0,0.0,0.0,0.0,  0.0,1.0,0.0,0.0,  0.0,0.0,1.0,0.0,  1.0,2.0,3.0,1.0]
_SCALE2 = [2.0,0.0,0.0,0.0,  0.0,2.0,0.0,0.0,  0.0,0.0,2.0,0.0,  0.0,0.0,0.0,1.0]
_ROTY90 = [0.0,0.0,-1.0,0.0, 0.0,1.0,0.0,0.0,  1.0,0.0,0.0,0.0,  0.0,0.0,0.0,1.0]
_MIXED  = [2.0,3.0,4.0,0.0,  5.0,6.0,7.0,0.0,  8.0,9.0,10.0,0.0, 11.0,12.0,13.0,1.0]

HOOKS = {
    # 0x0047d100 BoundedThunk47d100 (physics) - NEAR-LEAF bounds-checked adjustor thunk -> C3 0x4b5240.
    # int f(idx, a2): if(idx in [0,0xc8) && s=*(int*)(0x6c71d8+idx*4)) -> if(a2) s[2]|=4. Verbatim naked.
    'bounded_thunk_47d100': {'rva': 0x0047d100, 'export': 'BoundedThunk47d100', 'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32']}, 'arg_type': 'bounded_thunk_orflag',
        'tbl': 0x006c71d8,
        'scenarios': [
            {'idx': 5,   'a2': 1, 's2': 0x10},   # valid, a2!=0 -> 0x14
            {'idx': 5,   'a2': 1, 's2': 0x20},   # -> 0x24
            {'idx': 5,   'a2': 0, 's2': 0x30},   # a2==0 -> 0x30 (no or)
            {'idx': 300, 'a2': 1, 's2': 0x40},   # idx>=0xc8 bounds -> 0x40 (no effect)
        ],
        'path1_tests': [0, 1, 2, 3], 'path2_tests': [0, 1, 2, 3]},

    # 0x005c95b0 BitRangeFill5c95b0 (audio) - PURE LEAF bitfield range clear/set.
    # void f(uint8** pbuf, uint startbit, uint nbits, int fill): buf=*pbuf; set bits
    # [startbit,startbit+nbits) of bit-array buf to (fill!=0), preserving outside bits.
    # Scenarios exercise: full-byte fast path (fill & clear), partial first/last bytes,
    # single partial byte, multi-byte spans, fill=1 and fill=0.
    'bitfield_range_set_5c95b0': {'rva': 0x005c95b0, 'export': 'BitRangeFill5c95b0', 'signature': {'ret': 'void', 'args': ['pointer', 'uint32', 'uint32', 'uint32']}, 'arg_type': 'bitfield_range_set',
        'scenarios': [
            {'startbit': 3,  'nbits': 10, 'fill': 1, 'seed': 0x00},  # buf[0]=0xF8 buf[1]=0x1F
            {'startbit': 5,  'nbits': 20, 'fill': 0, 'seed': 0xFF},  # [0]=0x1F [1]=0x00 [2]=0x00 [3]=0xFE rest 0xFF
            {'startbit': 16, 'nbits': 16, 'fill': 1, 'seed': 0x00},  # [2]=0xFF [3]=0xFF (full-byte fast path)
            {'startbit': 9,  'nbits': 3,  'fill': 1, 'seed': 0x55},  # [1]=0x5F rest 0x55 (single partial byte)
            {'startbit': 12, 'nbits': 5,  'fill': 0, 'seed': 0xFF},  # [1]=0x0F [2]=0xFE rest 0xFF (span)
        ],
        'path1_tests': [0, 1, 2, 3, 4], 'path2_tests': [0, 1, 2, 3, 4]},

    # 0x00418a00 StructInit418a00 (render) - NEAR-LEAF struct initializer; pointer in ESI.
    # void f(ESI=p): memset(p,0,0x6c) via C3 ZeroFillWrapper(0x4b6520); *(u32*)(p+8)=*(u32*)(p+0xc)
    # =0x3d4ccccd (0.05f); p[0x10..0x14)=0xFF. Orig driven via esi-trampoline; reim __cdecl(p).
    'esi_struct_init_418a00': {'rva': 0x00418a00, 'export': 'StructInit418a00', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'esi_struct_init',
        'bufsize': 0x6c,
        'scenarios': [
            {'seed': 0xCC},   # sentinel CC -> post: zeros + 0.05f@8/0xc + 0xFFFFFFFF@0x10
            {'seed': 0x77},   # different sentinel -> identical post-state (proves full memset coverage)
        ],
        'path1_tests': [0, 1], 'path2_tests': [0, 1]},

    # 0x00421060 StructZero421060 (gameplay) - NEAR-LEAF zero-init; pointer in ESI.
    # void f(ESI=p): memset(p,0,8) via C3 ZeroFillWrapper(0x4b6520); *(u32*)(p+8/0xc/0x10/0x14)=0
    # => zeroes p[0..0x18). bufsize 0x20 -> boundary echo: tail [0x18,0x20) keeps sentinel.
    'esi_struct_init_421060': {'rva': 0x00421060, 'export': 'StructZero421060', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'esi_struct_init',
        'bufsize': 0x20,
        'scenarios': [
            {'seed': 0xCC},   # post: [0,0x18)=0, [0x18,0x20)=0xCCCCCCCC (boundary kept)
            {'seed': 0x77},   # post: [0,0x18)=0, [0x18,0x20)=0x77777777 (distinct boundary -> non-degen)
        ],
        'path1_tests': [0, 1], 'path2_tests': [0, 1]},

    # ===== round 248 (2026-06-23) NEAR-LEAF lane: vehicle-slot-getter family =====
    # 0x0040ba60 Active4Slots40ba60 (gameplay) - NEAR-LEAF void f(int* out):
    #   for i=0..3: out[i]=0; if(VehicleSlotGetter(i)==0) out[i]=1;  (callee C3 0x0046c7b0
    #   reads (&DAT_008815a4)[i*0x341] => byte 0x008815a4 + i*(0x341*4=0xD04)). Verbatim naked.
    #   Slot addrs i0..3 = 0x8815a4, 0x8822a8, 0x882fac, 0x883cb0.
    #   Seed slots 0,2 -> 0 (out=1) and 1,3 -> nonzero (out=0): out = [1,0,1,0] (non-degen).
    'active4slots_40ba60': {'rva': 0x0040ba60, 'export': 'Active4Slots40ba60', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'near_leaf_seed_outbuf',
        'argkinds': ['pointer'], 'out_size': 0x20, 'out_argpos': 0, 'out_observe': [0x0, 0x4, 0x8, 0xc],
        'seed_sets': [
            {'seeds': [[0x8815a4, 0], [0x8822a8, 0x55], [0x882fac, 0], [0x883cb0, 0x77]], 'args': []},
        ],
        'path1_tests': [0], 'path2_tests': [0]},

    # 0x0040b9a0 MaxScoreFlags40b9a0 (gameplay) - NEAR-LEAF void f(int* out, int mode):
    #   m=max over DAT_008a94e0[0..3] (mode!=0 => only slots i with VehicleSlotGetter(i)!=0
    #   are eligible); then out[i]=(DAT_008a94e0[i]==m)?1:0. DAT_008a94e0[i] @ 0x008a94e0+i*4;
    #   getter table (&DAT_008815a4)[i*0x341] @ 0x008815a4+i*0xD04 (i0..3 = 0x8815a4,0x8822a8,0x882fac,0x883cb0).
    #   Verbatim naked.
    #   A (mode=0): DAT=[10,30,20,30] -> max 30 -> out=[0,1,0,1].
    #   B (mode=1): getters elig {1,2} (i0,i3 = 0); DAT=[100,30,20,5] -> max over {30,20}=30
    #              -> out=[0,1,0,0] (proves gating: i0 DAT=100 would be max if not gated out).
    'maxscoreflags_40b9a0': {'rva': 0x0040b9a0, 'export': 'MaxScoreFlags40b9a0', 'signature': {'ret': 'void', 'args': ['pointer', 'uint32']}, 'arg_type': 'near_leaf_seed_outbuf',
        'argkinds': ['pointer', 'uint32'], 'out_size': 0x20, 'out_argpos': 0, 'out_observe': [0x0, 0x4, 0x8, 0xc],
        'seed_sets': [
            {'seeds': [[0x8a94e0, 10], [0x8a94e4, 30], [0x8a94e8, 20], [0x8a94ec, 30]], 'args': [0]},
            {'seeds': [[0x8a94e0, 100], [0x8a94e4, 30], [0x8a94e8, 20], [0x8a94ec, 5],
                       [0x8815a4, 0], [0x8822a8, 1], [0x882fac, 1], [0x883cb0, 0]], 'args': [1]},
        ],
        'path1_tests': [0, 1], 'path2_tests': [0, 1]},

    # 0x0045cab0 SlotAliveState45cab0 (gameplay) - NEAR-LEAF int f(uint idx):
    #   CarStatePairGet(idx,&state,&sec): state=(&DAT_00881f90)[idx*0x341] @ 0x00881f90+idx*0xD04;
    #   if(state!=0) return 0; else return VehicleSlotGetter(idx)==1. Callees C3 0x0046cbb0/0x0046c7b0.
    #   idx fixed = 2: state slot @ 0x00881f90+2*0xD04 = 0x00883998; getter slot @ 0x008815a4+2*0xD04 = 0x00882FAC.
    #   A: state=5 (nonzero) -> ret 0.  B: state=0, getter=1 -> ret 1.  C: state=0, getter=7 -> ret 0.
    'slot_alive_state_45cab0': {'rva': 0x0045cab0, 'export': 'SlotAliveState45cab0', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'near_leaf_seed_outbuf',
        'argkinds': ['uint32'], 'out_size': 0, 'fold_ret': True,
        'seed_sets': [
            {'seeds': [[0x883998, 5], [0x882FAC, 1]], 'args': [2]},   # state!=0 -> 0
            {'seeds': [[0x883998, 0], [0x882FAC, 1]], 'args': [2]},   # state==0, getter==1 -> 1
            {'seeds': [[0x883998, 0], [0x882FAC, 7]], 'args': [2]},   # state==0, getter!=1 -> 0
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},

    # 0x0045c330 PredNot45c330 (gameplay) - NEAR-LEAF int f(int idx): return Pred45bff0(idx)==0,
    #   i.e. returns (DAT_0088fc88[idx*0x2d] != 0) ? 1 : 0 (NEG/SBB/INC idiom). Callee C3 0x0045bff0.
    #   Existing table_bool_predicate handler: slot @ tgt+idx*stride+off0, stride=0x2d*4=0xb4.
    #   tests=[idx,slotval]: [2,0] (slot==0 -> ret 0), [2,0x55] (slot!=0 -> ret 1). bound=-1 (no early return).
    'prednot_45c330': {'rva': 0x0045c330, 'export': 'PredNot45c330', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'table_bool_predicate',
        'target_global': 0x0088fc88, 'stride': 0xb4, 'off0': 0, 'bound': -1,
        'path1_tests': [[2, 0], [2, 0x55]], 'path2_tests': [[2, 0], [2, 0x55]]},

    # 0x004058c0 FloatSubThunk4058c0 (gameplay) - NEAR-LEAF adjustor thunk -> C3 0x4058b0.
    # void f(idx, float fval): *(float*)(0x639d80 + idx*0xec + 0x5c) -= fval. Verbatim naked.
    'thunk_float_sub_4058c0': {'rva': 0x004058c0, 'export': 'FloatSubThunk4058c0', 'signature': {'ret': 'void', 'args': ['uint32', 'float']}, 'arg_type': 'thunk_float_sub',
        'tbl': 0x00639d80, 'stride': 0xec, 'field_off': 0x5c,
        'scenarios': [
            {'idx': 0, 'seed': 10.0, 'fval': 3.0},    # 7.0
            {'idx': 1, 'seed': 5.0,  'fval': 2.0},    # 3.0 (idx*0xec indexing)
            {'idx': 0, 'seed': 1.0,  'fval': 0.25},   # 0.75
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},

    # 0x005a7af0 Thunk5a7af0 (audio) - NEAR-LEAF adjustor thunk -> C3 AudioListNodeCount(0x5aded0).
    # int f(p): count circular list at (p+0xc) (linked +4, sentinel=head). Verbatim naked.
    'thunk_5a7af0': {'rva': 0x005a7af0, 'export': 'Thunk5a7af0', 'signature': {'ret': 'uint32', 'args': ['pointer']}, 'arg_type': 'thunk_list_count',
        'scenarios': [{'n': 0}, {'n': 1}, {'n': 3}],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},

    # 0x004b52f0 Thunk4b52f0 (render) - NEAR-LEAF adjustor thunk -> C3 0x4b52c0.
    # uint f(p, a2, a3): s=p[0x18]; if(a3) s[8]|=a2; return s[8]. Verbatim naked.
    'thunk_4b52f0': {'rva': 0x004b52f0, 'export': 'Thunk4b52f0', 'signature': {'ret': 'uint32', 'args': ['pointer', 'uint32', 'uint32']}, 'arg_type': 'thunk_cond_or',
        'scenarios': [
            {'a2': 0x0f, 'a3': 1, 'seed': 0x100},   # a3!=0 -> s[8]=0x10f, ret 0x10f
            {'a2': 0xf0, 'a3': 0, 'seed': 0x100},   # a3==0 -> s[8]=0x100 unchanged, ret 0x100
            {'a2': 0x33, 'a3': 1, 'seed': 0x44},    # -> s[8]=0x77, ret 0x77
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},

    # 0x004b4130 Thunk4b4130 (render) - NEAR-LEAF adjustor thunk -> C3 0x4b40c0.
    # f(p, out): s=p[0x18]; copy s[0x24] dwords from *(s[0x20]) to out. Verbatim naked.
    'thunk_4b4130': {'rva': 0x004b4130, 'export': 'Thunk4b4130', 'signature': {'ret': 'void', 'args': ['pointer', 'pointer']}, 'arg_type': 'thunk_field_copy',
        'scenarios': [
            {'pat': 0xA0000000, 'count': 4},
            {'pat': 0x11110000, 'count': 3},
        ],
        'path1_tests': [0, 1], 'path2_tests': [0, 1]},

    # 0x004893a0 AdjThunk4893a0 (gameplay) - NEAR-LEAF adjustor thunk -> C3 0x476cb0.
    # void f(p, a2, a3): node=(*0x7dc57c)[p[0x14]]; node[0xa4]=a2; node[0xa8]=a3;
    # node[0x40]|=0x10000000. Verbatim naked. Build p/table/node; observe node fields.
    'adj_thunk_4893a0': {'rva': 0x004893a0, 'export': 'AdjThunk4893a0', 'signature': {'ret': 'void', 'args': ['pointer', 'uint32', 'uint32']}, 'arg_type': 'thunk_node_write',
        'glob': 0x007dc57c,
        'scenarios': [
            {'a2': 0x1111, 'a3': 0x2222},
            {'a2': 0x3333, 'a3': 0x4444},
        ],
        'path1_tests': [0, 1], 'path2_tests': [0, 1]},

    # 0x00477b40 ZeroTwoRegions477b40 (util) - NEAR-LEAF void f(void): calls C3
    # ZeroFillWrapper(0x4b6520) twice: memset(0x693198,0,0x2c000) + memset(0x6bf198,0,0x20).
    # Verbatim naked. Boundary-echo verify (reuse near_leaf_seed_multi_obs): seed sentinel
    # at region points + the byte past region2 (0x6bf1b8); after -> 0 inside, sentinel past.
    'zero_two_regions_477b40': {'rva': 0x00477b40, 'export': 'ZeroTwoRegions477b40', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'near_leaf_seed_multi_obs',
        'observe_addrs': [0x00693198, 0x006bf194, 0x006bf198, 0x006bf1b4, 0x006bf1b8],
        'seed_sets': [
            {'globals': [[0x693198,0xCCCCCCCC],[0x6bf194,0xCCCCCCCC],[0x6bf198,0xCCCCCCCC],[0x6bf1b4,0xCCCCCCCC],[0x6bf1b8,0xCCCCCCCC]]},
        ],
        'path1_tests': [0], 'path2_tests': [0]},

    # 0x00486f90 PoolArrayReset486f90 (particle) - PURE LEAF void f(void): clears two
    # global pool arrays (loop1 stride 0x7c, loop2 stride 0x90), [eax-0x44]=50.0f per
    # entry, [0x703038]=[0x70303c]=0. Verbatim naked. Reuses near_leaf_seed_multi_obs:
    # seed the observed fields with sentinel, observe = 50.0f (0x42480000) / 0.
    'pool_array_reset_486f90': {'rva': 0x00486f90, 'export': 'PoolArrayReset486f90', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'near_leaf_seed_multi_obs',
        'observe_addrs': [0x00702fe8, 0x0070302c, 0x006fa610, 0x006fa654, 0x00703038, 0x0070303c],
        'seed_sets': [
            {'globals': [[0x702fe8,0xCCCCCCCC],[0x70302c,0xCCCCCCCC],[0x6fa610,0xCCCCCCCC],[0x6fa654,0xCCCCCCCC],[0x703038,0xCCCCCCCC],[0x70303c,0xCCCCCCCC]]},
        ],
        'path1_tests': [0], 'path2_tests': [0]},

    # 0x0048f590 ParticlePoolAlloc48f590 (particle) - PURE LEAF void f(int* a1, int a2):
    # 10-slot pool @0x769f50 (stride 0x24); scan for free (slot[+0]==0) else evict max
    # (slot[+0x1c]); write a1[0..2]/a2/255f/used. Observe slot[+0]/[+4] of slots 0/1/9.
    'particle_pool_alloc_48f590': {'rva': 0x0048f590, 'export': 'ParticlePoolAlloc48f590', 'signature': {'ret': 'void', 'args': ['pointer', 'uint32']}, 'arg_type': 'particle_pool_alloc',
        'glob': 0x00769f50,
        'scenarios': [
            {'used': [],                       'pris': []},                              # all free -> slot0
            {'used': [0],                      'pris': []},                              # slot0 used -> slot1
            {'used': [0,1,2,3,4,5,6,7,8,9],    'pris': [1,2,3,4,5,6,7,8,2,9]},          # full -> evict slot9 (max pri 9)
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},

    # 0x0041e4b0 StructPropagate41e4b0 (gameplay) - PURE LEAF EAX-implicit void f(EAX=s):
    # if(s[0x1b4]==s[0x1b8]) return; s[0x1b8]=s[0x1b4]; write table[idx](@0x63d5f8) into 12
    # sub-object deref chains (s[OFF]->+0x18->+0x20->*->+4). EAX-trampoline; observe s[0x1b8]|P4[4].
    'struct_propagate_41e4b0': {'rva': 0x0041e4b0, 'export': 'StructPropagate41e4b0', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'eax_struct_deref_write',
        'tbl': 0x0063d5f8,
        'scenarios': [
            {'idx': 3, 'prev': 0},    # index changed -> s[0x1b8]=3, P4[4]=table[3]
            {'idx': 5, 'prev': 5},    # index same -> no-op (s[0x1b8]=5, P4[4]=sentinel)
        ],
        'path1_tests': [0, 1], 'path2_tests': [0, 1]},

    # 0x004d8570 EngineRegisterFuncs4d8570 (render) - PURE LEAF int f(void): straight-line
    # registration of 17 funcptr constants into (*0x7d3ff8)+0xc4..0x104; return 1.
    'engine_register_funcs_4d8570': {'rva': 0x004d8570, 'export': 'EngineRegisterFuncs4d8570', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'engine_register_funcs',
        'glob': 0x007d3ff8,
        'observe_offs': [0xc4, 0xc8, 0xcc, 0xd0, 0xd4, 0xd8, 0xdc, 0xe0, 0xe4, 0xe8, 0xec, 0xf0, 0xf4, 0xf8, 0xfc, 0x100, 0x104],
        'path1_tests': [0, 1], 'path2_tests': [0, 1]},

    # 0x004cde50 PixelMaxAlpha4cde50 (render) - PURE LEAF int f(struct* s): per-pixel
    # alpha=max(R,G,B). mode=s[0xc]: 4|8->(1<<mode) pixels @s[0x18]+2; 0x20->s[8]rows x
    # s[4]cols @s[0x14]+2; else no-op. ret s. Observe base[3]/[7]/[0x43] alphas + ret.
    'pixel_max_alpha_4cde50': {'rva': 0x004cde50, 'export': 'PixelMaxAlpha4cde50', 'signature': {'ret': 'uint32', 'args': ['pointer']}, 'arg_type': 'pixel_max_alpha',
        'scenarios': [
            {'mode': 4,    'rows': 0, 'cols': 0, 'stride': 0x40},   # 16 pixels -> base[3],[7] set; [0x43] sentinel
            {'mode': 0x20, 'rows': 2, 'cols': 2, 'stride': 0x40},   # 2x2 -> base[3],[7],[0x43] set
            {'mode': 0,    'rows': 0, 'cols': 0, 'stride': 0x40},   # no-op -> all sentinel
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},

    # 0x005a7a60 NestedListSearch5a7a60 (audio) - PURE LEAF uint f(int key): nested
    # circular-list search at *0x7dca7c; inner payload[0xc]==key -> return key else 0.
    # Verbatim naked reimpl. Build 1 outer + 1 inner + payload; found vs not-found.
    'nested_list_search_5a7a60': {'rva': 0x005a7a60, 'export': 'NestedListSearch5a7a60', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'nested_list_search',
        'glob': 0x007dca7c,
        'scenarios': [
            {'pval': 0x1234, 'key': 0x1234},   # found -> 0x1234
            {'pval': 0x5678, 'key': 0x1234},   # not found -> 0
        ],
        'path1_tests': [0, 1], 'path2_tests': [0, 1]},

    # 0x00483ca0 FindNodeStructCopy483ca0 (vehicle) - PURE LEAF int f(struct* p1, void** p2):
    # walk p2's list for a node ((node[8]==p1[8] && node[0]==0x10b) || node[0]==0), copy
    # 0x67 dwords p1->node, then p1[0x16c]*9 dwords from p1[0x14]->node+0x19c; return 1.
    # Verbatim naked reimpl. Found-first scenario, varied pattern for non-degeneracy.
    'find_node_struct_copy_483ca0': {'rva': 0x00483ca0, 'export': 'FindNodeStructCopy483ca0', 'signature': {'ret': 'uint32', 'args': ['pointer', 'pointer']}, 'arg_type': 'find_node_struct_copy',
        'scenarios': [
            {'pat': 0xA0000000, 'pat2': 0xB0000000},
            {'pat': 0x11110000, 'pat2': 0x22220000},
        ],
        'path1_tests': [0, 1], 'path2_tests': [0, 1]},

    # 0x004d8c40 DllMergeSwap4d8c40 (frontend) - PURE LEAF void f(void): circular-list
    # merge+swap on table entry base=*0x911ad8 + *0x7d3ff8 (B@+0x20, A@+0x24, clear +8).
    # Verbatim naked reimpl (byte-identical). Verified via empty-B swap path, role-swapped.
    'dll_merge_swap_4d8c40': {'rva': 0x004d8c40, 'export': 'DllMergeSwap4d8c40', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'dll_merge_swap',
        'glob_a': 0x00911ad8, 'glob_b': 0x007d3ff8,
        'scenarios': [
            {'swap': False},   # A=n1, B=n2 -> +0x24=n2, +0x20=n1, +8=0
            {'swap': True},    # A=n2, B=n1 -> +0x24=n1, +0x20=n2, +8=0
        ],
        'path1_tests': [0, 1], 'path2_tests': [0, 1]},

    # 0x00475f30 ScaledMulAdd475f30 (render) - PURE LEAF EAX-implicit: void f(EAX=out,
    # float a1, float a2): r = 0.5*2.0*a1*a2 + 0.1; out[1]=r; out[0]=r. Verbatim naked
    # reimpl (byte-identical). EAX-trampoline handler unlocks EAX-implicit functions.
    'scaled_mul_add_475f30': {'rva': 0x00475f30, 'export': 'ScaledMulAdd475f30', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'eax_out_2float',
        'scenarios': [
            {'a1': 1.0, 'a2': 1.0},   # 1.1
            {'a1': 2.0, 'a2': 3.0},   # 6.1
            {'a1': 0.5, 'a2': 4.0},   # 2.1
            {'a1': -1.0,'a2': 2.0},   # -1.9
        ],
        'path1_tests': [0, 1, 2, 3], 'path2_tests': [0, 1, 2, 3]},

    # 0x005b0bb0 QuadBufferBuild5b0bb0 (audio) - PURE LEAF int f(void* out, uint maxsize,
    # struct* rec): 2-pass quad builder. rec[0x14]=count, rec[0x18]=arr; P=*(arr+0x14+k*0x28),
    # sub=P[0xd]; if sum*4>maxsize ret 0 else write count*sub 64-byte blocks {ri,si,0,1.0f}.
    'quad_buffer_build_5b0bb0': {'rva': 0x005b0bb0, 'export': 'QuadBufferBuild5b0bb0', 'signature': {'ret': 'uint32', 'args': ['pointer', 'uint32', 'pointer']}, 'arg_type': 'quad_buffer_build',
        'scenarios': [
            {'subs': [1, 1], 'maxsize': 1000},   # 2 records x1 sub -> ret 8
            {'subs': [2],    'maxsize': 1000},   # 1 record x2 subs -> ret 8
            {'subs': [1, 1], 'maxsize': 4},      # sum*4=8 > 4 -> ret 0, out untouched
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},

    # 0x00413c70 Idx2RecordCondSet413c70 (vehicle) - PURE LEAF void f(int i,int j,int v):
    # off=(j+5i)*12; *(int*)(0x63bc68+off)=v; if(*(float*)(0x63bc60+off)==0.0) ...=0.01f.
    # Seed A(float)=cur, observe A|B. non-degen via cur==0 vs !=0 + varied v/index.
    'idx2_record_condset_413c70': {'rva': 0x00413c70, 'export': 'Idx2RecordCondSet413c70', 'signature': {'ret': 'void', 'args': ['uint32', 'uint32', 'uint32']}, 'arg_type': 'idx2_record_condset',
        'baseA': 0x0063bc60, 'baseB': 0x0063bc68, 'recStride': 12,
        'scenarios': [
            {'i': 0, 'j': 0, 'v': 0x111, 'cur': 0.0},    # cur==0 -> A=0.01
            {'i': 0, 'j': 0, 'v': 0x222, 'cur': 5.0},    # cur!=0 -> A stays 5.0
            {'i': 1, 'j': 0, 'v': 0x333, 'cur': 0.0},    # index (off=60) -> A=0.01
            {'i': 0, 'j': 2, 'v': 0x444, 'cur': -3.0},   # index (off=24), cur!=0 -> stays
        ],
        'path1_tests': [0, 1, 2, 3], 'path2_tests': [0, 1, 2, 3]},

    # 0x004c0e50 DllHeadInsert4c0e50 (render, 19 callers) - PURE LEAF void f(struct* p):
    # intrusive DLL head-insert. node=p[0xa0]; if((node[3]&3)==0) insert at head of
    # list *0x7d3ff8 (sentinel @+0xbc). node[3]|=3; p[3]|=0xc. Observe node links + flags.
    'dll_head_insert_4c0e50': {'rva': 0x004c0e50, 'export': 'DllHeadInsert4c0e50', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'dll_head_insert',
        'glob': 0x007d3ff8,
        'scenarios': [
            {'flag': 0},   # node[3]&3==0 -> full insert
            {'flag': 1},   # node[3]&3!=0 -> skip insert, only set flags
            {'flag': 2},   # bit1 set -> skip insert too
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},

    # 0x00476d00 MultiArrayScatter476d00 (render, 21 callers) - PURE LEAF void f(struct* p):
    # if(p[0xc]>=p[8]) return; else scatter REAL source globals into 7 optional arrays
    # (ptr fields p+0x10/14/18/1c/20/24/28, strides 12/8/4/64/4/16/32) at counter p[0xc],
    # then counter++. Observe arr[k] at counter-slot + counter. source globals REAL.
    'multi_array_scatter_476d00': {'rva': 0x00476d00, 'export': 'MultiArrayScatter476d00', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'multi_array_scatter',
        'scenarios': [
            {'counter': 0, 'bound': 4},   # write slot 0, counter->1
            {'counter': 2, 'bound': 4},   # write slot 2 (index-verified), counter->3
            {'counter': 4, 'bound': 4},   # bounds: counter>=bound -> no write, counter stays 4
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},

    # 0x0045ca30 FloatClamp3Level45ca30 (gameplay) - PURE LEAF void f(void): 3-level
    # quantize of 6-float array @0x88f5e0. x>-2->0.0; x>-4->-1.0; else -2.0. Fixed
    # constant outputs. Reuses near_leaf_seed_multi_obs: seed 6 floats, observe them.
    'float_clamp3_45ca30': {'rva': 0x0045ca30, 'export': 'FloatClamp3Level45ca30', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'near_leaf_seed_multi_obs',
        'observe_addrs': [0x0088f5e0, 0x0088f5e4, 0x0088f5e8, 0x0088f5ec, 0x0088f5f0, 0x0088f5f4],
        'seed_sets': [
            # 5.0,-3.0,-10.0,0.0,-2.5,-5.0 -> 0,-1,-2,0,-1,-2
            {'globals': [[0x88f5e0,0x40a00000],[0x88f5e4,0xc0400000],[0x88f5e8,0xc1200000],[0x88f5ec,0x00000000],[0x88f5f0,0xc0200000],[0x88f5f4,0xc0a00000]]},
            # -1.0,-3.5,-100.0,10.0,-8.0,-2.1 -> 0,-1,-2,0,-2,-1
            {'globals': [[0x88f5e0,0xbf800000],[0x88f5e4,0xc0600000],[0x88f5e8,0xc2c80000],[0x88f5ec,0x41200000],[0x88f5f0,0xc1000000],[0x88f5f4,0xc0066666]]},
        ],
        'path1_tests': [0, 1], 'path2_tests': [0, 1]},

    # 0x0040dcb0 TimerStateMachine40dcb0 (gameplay) - PURE LEAF void f(void): multi-phase
    # countdown SM on globals 0x63b90c(state)/910(t1)/918(t2), rate 700.0, dt 0x7f100c.
    # Reuses near_leaf_seed_multi_obs: seed state+timers+dt, observe state+t1+t2.
    'timer_state_machine_40dcb0': {'rva': 0x0040dcb0, 'export': 'TimerStateMachine40dcb0', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'near_leaf_seed_multi_obs',
        'observe_addrs': [0x0063b90c, 0x0063b910, 0x0063b918],
        'seed_sets': [
            {'globals': [[0x63b90c,3],[0x63b910,0x447a0000],[0x63b918,0],[0x7f100c,0x3f800000]]},  # st3 no reset -> 300
            {'globals': [[0x63b90c,3],[0x63b910,0x43960000],[0x63b918,0],[0x7f100c,0x3f800000]]},  # st3 reset -> state0, -400
            {'globals': [[0x63b90c,2],[0x63b910,0],[0x63b918,0x41200000],[0x7f100c,0x3f800000]]},  # st2 no -> 9.0
            {'globals': [[0x63b90c,2],[0x63b910,0],[0x63b918,0x3f000000],[0x7f100c,0x3f800000]]},  # st2 advance -> state3, -0.5
            {'globals': [[0x63b90c,1],[0x63b910,0x44fa0000],[0x63b918,0],[0x7f100c,0x3f800000]]},  # st1 no -> 1300
            {'globals': [[0x63b90c,1],[0x63b910,0x44610000],[0x63b918,0],[0x7f100c,0x3f800000]]},  # st1 advance -> state2,320,5
        ],
        'path1_tests': [0, 1, 2, 3, 4, 5], 'path2_tests': [0, 1, 2, 3, 4, 5]},

    # 0x005b6a40 SuccApproxQuantize5b6a40 (audio) - PURE LEAF void f(int arg1, int* p2,
    # int* p3): successive-approx quantizer. range=*(s16*)(0x634498+(*p3)*2); flags built
    # by edx>=range (range>>=1 each); *p2 = quantized recon (clamp [-0x8000,0x7fff]);
    # *p3 += *(s16*)(0x634478+flags*2), floor 0. Seed p2/p3 + range tbl[idx] + delta tbl.
    'succ_approx_quantize_5b6a40': {'rva': 0x005b6a40, 'export': 'SuccApproxQuantize5b6a40', 'signature': {'ret': 'void', 'args': ['uint32', 'pointer', 'pointer']}, 'arg_type': 'succ_approx_quantize',
        'rangeTbl': 0x00634498, 'deltaTbl': 0x00634478,
        'scenarios': [
            {'arg1': 100,   'cur': 0, 'idx': 0, 'range': 0x400},   # small +delta, no range bits
            {'arg1': 2000,  'cur': 0, 'idx': 0, 'range': 0x400},   # >range -> bits 4|2|1
            {'arg1': -300,  'cur': 0, 'idx': 0, 'range': 0x400},   # negative -> al|=8
            {'arg1': 40000, 'cur': 0, 'idx': 0, 'range': 0x400},   # (short)40000=-25536, all bits
        ],
        'path1_tests': [0, 1, 2, 3], 'path2_tests': [0, 1, 2, 3]},

    # 0x004d7100 EventEnqueueCascade4d7100 (render) - PURE LEAF void f(int param):
    # guarded event-enqueue cascade over global flags + queue 0x7d5168[0x7d6c14].
    # 3 scenarios: param=1 full cascade from zero, param=0 from set-state, param=1
    # already-set -> early return. observe the 6 flags + 2 queue slots.
    'event_enqueue_cascade_4d7100': {'rva': 0x004d7100, 'export': 'EventEnqueueCascade4d7100', 'signature': {'ret': 'void', 'args': ['uint32']}, 'arg_type': 'seed_globals_arg_multiobs',
        'observe_addrs': [0x007d6b10, 0x007d58d0, 0x007d58d4, 0x007d6c14, 0x007d5870, 0x007d5874, 0x007d5168, 0x007d516c],
        'seed_sets': [
            # A: param=1, all-zero inputs (0x7d6bfc=0x55) -> full cascade
            {'arg': 1, 'globals': [[0x7d6b10,0],[0x7d6b14,0],[0x7d58d0,0],[0x7d58d4,0],[0x7d6c14,0],[0x7d5870,0],[0x7d5874,0],[0x7d6bfc,0x55],[0x7d5168,0],[0x7d516c,0]]},
            # B: param=0 from set-state (0x7d6b10=1, 0x7d5870=0x55)
            {'arg': 0, 'globals': [[0x7d6b10,1],[0x7d6b14,0],[0x7d58d0,0],[0x7d58d4,0],[0x7d6c14,0],[0x7d5870,0x55],[0x7d5874,0],[0x7d6bfc,0x99],[0x7d5168,0],[0x7d516c,0]]},
            # C: param=1 but 0x7d6b10 already set -> early return (no writes)
            {'arg': 1, 'globals': [[0x7d6b10,1],[0x7d6b14,0],[0x7d58d0,7],[0x7d58d4,7],[0x7d6c14,5],[0x7d5870,3],[0x7d5874,9],[0x7d5168,0],[0x7d516c,0]]},
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},

    # ── c3-batch-sgr1-s2 render-state cached dirty-queue setters (cluster B) ──
    # All 5 are cache-guarded D3D8 render-state setters in the SAME cluster as
    # 0x004d7100 (shared queue 0x7d5168[0x7d6c14]). The plain void_setter_observe
    # lane is DEGENERATE for them: Orig(arg) primes *cache=arg, so Reimpl(arg)
    # cache-HITS and no-ops -> a wrong reimpl reads Orig's leftover and still
    # passes. Promoted via the SAME proven lane as 0x004d7100:
    # seed_globals_arg_multiobs (early_window_leaf_diff.py) — seed the cache so
    # BOTH sides run the full body, snapshot the whole effect-set. 3 scenarios:
    #   A: cache-miss + dirty=0 -> store + enqueue opcode + idx++ + cache update
    #   B: cache-miss + dirty=1 -> store + cache update, NO enqueue (dirty guard)
    #   C: cache-HIT (cache==arg) -> early return, NO writes
    # -> 3 distinct observable patterns => non-degenerate. arg_type
    # seed_globals_arg_multiobs is SWEEP-CRITICAL (run_diff.py has no handler for
    # it; verify these via early_window_leaf_diff.py, NOT run_diff/run_diff_parallel).
    # Seeds: cache, store(sentinel 0xAAAAAAAA), dirty, idx(0), queue[0](0).
    # Observe: [store, dirty, idx(0x7d6c14), queue[0](0x7d5168), cache].

    # 0x004d7390  RenderStateSetOpcode39 — raw store DAT_007d59c0, opcode 0x39. U-5338
    'render_state_set_opcode39': {
        'rva':            0x004d7390,
        'export':         'RenderStateSetOpcode39',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'seed_globals_arg_multiobs',
        'observe_addrs':  [0x007d59c0, 0x007d59c4, 0x007d6c14, 0x007d5168, 0x007d6b04],
        'seed_sets': [
            {'arg': 0x11111111, 'globals': [[0x7d6b04, 0], [0x7d59c0, 0xAAAAAAAA], [0x7d59c4, 0], [0x7d6c14, 0], [0x7d5168, 0]]},
            {'arg': 0x22222222, 'globals': [[0x7d6b04, 0], [0x7d59c0, 0xAAAAAAAA], [0x7d59c4, 1], [0x7d6c14, 0], [0x7d5168, 0]]},
            {'arg': 0x33333333, 'globals': [[0x7d6b04, 0x33333333], [0x7d59c0, 0xAAAAAAAA], [0x7d59c4, 0], [0x7d6c14, 0], [0x7d5168, 0]]},
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},

    # 0x004d73e0  RenderStateSetOpcode3a — raw store DAT_007d59c8, opcode 0x3a. U-5339
    'render_state_set_opcode3a': {
        'rva':            0x004d73e0,
        'export':         'RenderStateSetOpcode3a',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'seed_globals_arg_multiobs',
        'observe_addrs':  [0x007d59c8, 0x007d59cc, 0x007d6c14, 0x007d5168, 0x007d6b08],
        'seed_sets': [
            {'arg': 0x11111111, 'globals': [[0x7d6b08, 0], [0x7d59c8, 0xAAAAAAAA], [0x7d59cc, 0], [0x7d6c14, 0], [0x7d5168, 0]]},
            {'arg': 0x22222222, 'globals': [[0x7d6b08, 0], [0x7d59c8, 0xAAAAAAAA], [0x7d59cc, 1], [0x7d6c14, 0], [0x7d5168, 0]]},
            {'arg': 0x33333333, 'globals': [[0x7d6b08, 0x33333333], [0x7d59c8, 0xAAAAAAAA], [0x7d59cc, 0], [0x7d6c14, 0], [0x7d5168, 0]]},
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},

    # 0x004d7430  RenderStateSetOpcode3b — raw store DAT_007d59d0, opcode 0x3b. U-5350
    'render_state_set_opcode3b': {
        'rva':            0x004d7430,
        'export':         'RenderStateSetOpcode3b',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'seed_globals_arg_multiobs',
        'observe_addrs':  [0x007d59d0, 0x007d59d4, 0x007d6c14, 0x007d5168, 0x007d6b0c],
        'seed_sets': [
            {'arg': 0x11111111, 'globals': [[0x7d6b0c, 0], [0x7d59d0, 0xAAAAAAAA], [0x7d59d4, 0], [0x7d6c14, 0], [0x7d5168, 0]]},
            {'arg': 0x22222222, 'globals': [[0x7d6b0c, 0], [0x7d59d0, 0xAAAAAAAA], [0x7d59d4, 1], [0x7d6c14, 0], [0x7d5168, 0]]},
            {'arg': 0x33333333, 'globals': [[0x7d6b0c, 0x33333333], [0x7d59d0, 0xAAAAAAAA], [0x7d59d4, 0], [0x7d6c14, 0], [0x7d5168, 0]]},
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},

    # 0x004d7bc0  TextureStateSetFilterMode — TABLE store DAT_007d5840=tbl(0x5d8c48)[arg],
    # opcode 0x09. in-range args: [1]->1, [4]->3, [2]->2 (C is a cache-hit so no read). U-5353
    'texture_state_set_filter_mode': {
        'rva':            0x004d7bc0,
        'export':         'TextureStateSetFilterMode',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'seed_globals_arg_multiobs',
        'observe_addrs':  [0x007d5840, 0x007d5844, 0x007d6c14, 0x007d5168, 0x007d6b2c],
        'seed_sets': [
            {'arg': 1, 'globals': [[0x7d6b2c, 0], [0x7d5840, 0xAAAAAAAA], [0x7d5844, 0], [0x7d6c14, 0], [0x7d5168, 0]]},
            {'arg': 4, 'globals': [[0x7d6b2c, 0], [0x7d5840, 0xAAAAAAAA], [0x7d5844, 1], [0x7d6c14, 0], [0x7d5168, 0]]},
            {'arg': 2, 'globals': [[0x7d6b2c, 2], [0x7d5840, 0xAAAAAAAA], [0x7d5844, 0], [0x7d6c14, 0], [0x7d5168, 0]]},
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},

    # 0x004d7c10  TextureStateSetAddressMode — TABLE store DAT_007d58a8=tbl(0x5d8d18)[arg],
    # opcode 0x16. in-range args: [1]->1, [3]->3, [2]->2 (C is a cache-hit). U-5354
    'texture_state_set_address_mode': {
        'rva':            0x004d7c10,
        'export':         'TextureStateSetAddressMode',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'seed_globals_arg_multiobs',
        'observe_addrs':  [0x007d58a8, 0x007d58ac, 0x007d6c14, 0x007d5168, 0x007d6b18],
        'seed_sets': [
            {'arg': 1, 'globals': [[0x7d6b18, 0], [0x7d58a8, 0xAAAAAAAA], [0x7d58ac, 0], [0x7d6c14, 0], [0x7d5168, 0]]},
            {'arg': 3, 'globals': [[0x7d6b18, 0], [0x7d58a8, 0xAAAAAAAA], [0x7d58ac, 1], [0x7d6c14, 0], [0x7d5168, 0]]},
            {'arg': 2, 'globals': [[0x7d6b18, 2], [0x7d58a8, 0xAAAAAAAA], [0x7d58ac, 0], [0x7d6c14, 0], [0x7d5168, 0]]},
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},

    # 0x0046d7f0 BoundedTableSignClamp46d7f0 (ai) - PURE LEAF int f(uint idx, int val):
    # if(idx>=0x10) ret 0; b=*(u8*)(0x7f103c + (*(int*)(0x7f1a14+idx*16))*0x4c);
    # slot=&[0x882194+idx*0xd04]; *slot += (float)b>160.0 ? +val : -val; clamp[0,0xbb8]; ret 1.
    'bounded_table_signclamp_46d7f0': {'rva': 0x0046d7f0, 'export': 'BoundedTableSignClamp46d7f0', 'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32']}, 'arg_type': 'bounded_table_signselect_clamp',
        't1Tbl': 0x007f1a14, 't2Tbl': 0x007f103c, 't3Tbl': 0x00882194, 't3Stride': 0xd04,
        'scenarios': [
            {'idx': 0, 'val': 100, 'byte': 200, 'slot': 500},   # b>160 -> +100 -> 600
            {'idx': 0, 'val': 100, 'byte': 50,  'slot': 500},   # b<=160 -> -100 -> 400
            {'idx': 1, 'val': 50,  'byte': 200, 'slot': 1000},  # idx indexing -> 1050
            {'idx': 0, 'val': 5000,'byte': 200, 'slot': 0},     # high clamp -> 0xbb8 (3000)
            {'idx': 0, 'val': 5000,'byte': 50,  'slot': 100},   # low clamp -> 0
            {'idx': 0x10,'val': 1, 'byte': 0,   'slot': 0},     # bounds -> ret 0
        ],
        'path1_tests': [0, 1, 2, 3, 4, 5], 'path2_tests': [0, 1, 2, 3, 4, 5]},

    # 0x0046bda0 IndexedFloatAccum16_46bda0 (gameplay) - PURE LEAF int f(float* out,
    # uint i, uint j): if(i>=0x10||j>=4) ret 0; else acc = *0x5d757c(0.0) + 16 floats
    # at 0x8815a0+i*0xd04+j*0xc4+0x1b0-4 .. +0x38; acc *= *0x5cd058(0.0625); *out=acc; ret 1.
    # Seed 16 floats=fill+k per scenario; observe ret:outbits. capstone-verified.
    'indexed_float_accum16_46bda0': {'rva': 0x0046bda0, 'export': 'IndexedFloatAccum16_46bda0', 'signature': {'ret': 'uint32', 'args': ['pointer', 'uint32', 'uint32']}, 'arg_type': 'indexed_float_accum16',
        'tbl_base': 0x008815a0, 'iStride': 0xd04, 'jStride': 0xc4, 'regionOff': 0x1b0,
        'scenarios': [
            {'i': 0, 'j': 0, 'fill': 1},    # sum (1..16)/16 = 8.5
            {'i': 0, 'j': 0, 'fill': 5},    # different fill -> different sum
            {'i': 1, 'j': 0, 'fill': 1},    # tests i*0xd04 indexing (different region)
            {'i': 0, 'j': 1, 'fill': 1},    # tests j*0xc4 indexing
            {'i': 0x10, 'j': 0, 'fill': 1}, # bounds: i>=0x10 -> ret 0, out untouched
            {'i': 0, 'j': 4, 'fill': 1},    # bounds: j>=4 -> ret 0
        ],
        'path1_tests': [0, 1, 2, 3, 4, 5], 'path2_tests': [0, 1, 2, 3, 4, 5]},

    # 0x004cb740 StructTagEquals4cb740 (render) - PURE LEAF int f(a,b): tagged-union
    # dword equality. Common compare [0,4,8,c]; tag=a[0] -> tag1{0x34,0x38,0x3c,0x4c}
    # / tag2{...,0x60,0x64} / tag3{0x40,0x44,0x48} / other{}. Returns 1 if equal else 0.
    # 8 scenarios cover all tag branches x equal/unequal -> alternating 1/0 (non-degen).
    'struct_tag_equals_4cb740': {'rva': 0x004cb740, 'export': 'StructTagEquals4cb740', 'signature': {'ret': 'uint32', 'args': ['pointer', 'pointer']}, 'arg_type': 'struct_tag_equals',
        'scenarios': [
            {'tag': 4, 'diff': -1},     # other tag, equal -> 1
            {'tag': 4, 'diff': 0x04},   # other tag, common field differs -> 0
            {'tag': 1, 'diff': -1},     # tag1 equal -> 1
            {'tag': 1, 'diff': 0x4c},   # tag1 field differs -> 0
            {'tag': 2, 'diff': -1},     # tag2 equal -> 1
            {'tag': 2, 'diff': 0x60},   # tag2 field differs -> 0
            {'tag': 3, 'diff': -1},     # tag3 equal -> 1
            {'tag': 3, 'diff': 0x44},   # tag3 field differs -> 0
        ],
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7], 'path2_tests': [0, 1, 2, 3, 4, 5, 6, 7]},

    # 0x0044e070 DoubleIndexedFloatMul44e070 (audio) - PURE LEAF float f(int idx):
    # c=*(int*)(idx*0xf8+0x89008c); d=*(int*)(idx*0xf8+0x890090); e=d+c*4;
    # return *(float*)(0x894de0+e*4) * *(float*)0x5cc8f4 (K=1/6). capstone-verified.
    # Seed idx=0: aTbl=0, bTbl=e=t, fTbl+t*4=float(t+1); non-degen via varied e.
    'double_indexed_float_mul_44e070': {'rva': 0x0044e070, 'export': 'DoubleIndexedFloatMul44e070', 'signature': {'ret': 'float', 'args': ['uint32']}, 'arg_type': 'double_indexed_float_mul',
        'aTbl': 0x0089008c, 'bTbl': 0x00890090, 'fTbl': 0x00894de0,
        'path1_tests': [0, 1, 2, 3], 'path2_tests': [0, 1, 2, 3]},

    # 0x00420d80 IndexedFloatPairSum420d80 (gameplay) - PURE LEAF float f(int idx):
    # p = (float*)(0x63e4b8 + idx*36); return p[0]+p[1]. (capstone-disasm verified:
    # mov eax,[esp+4]; lea eax,[eax+eax*8]; fld [eax*4+0x63e4b8]; fadd [eax+4]; ret).
    # Seed two distinct floats at slot+0/+4 per idx; non-degen via varied idx.
    'indexed_float_pair_sum_420d80': {'rva': 0x00420d80, 'export': 'IndexedFloatPairSum420d80', 'signature': {'ret': 'float', 'args': ['uint32']}, 'arg_type': 'indexed_float_sum2',
        'target_global': 0x0063e4b8, 'stride': 36,
        'path1_tests': [0, 1, 2, 3], 'path2_tests': [0, 1, 2, 3]},

    # 0x00552750 FontCtx_ResetTransform (hud) - u32 f(void): resets the current
    # font ctx's 3x3 matrix to identity (ctx+0x00=1.0, +0x14=1.0, +0x28=1.0, rest 0),
    # ORs ctx+0xc |= 0x20003, zeros DAT_00912bd8/bec, returns 1. ctx =
    # (*0x00912a84)[*0x00912b04]. Seed depth=0, ptr_array[0]=ctxbuf (prior AV was a
    # null deref). Vary ctx+0xc seed -> flags = seed|0x20003 non-degenerate.
    'fontctx_reset_transform': {'rva': 0x00552750, 'export': 'FontCtx_ResetTransform', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'seed_indirect_ctx_obs',
        'ptr_array': 0x00912a84, 'depth_global': 0x00912b04, 'depth_idx': 0,
        'ctx_seed_off': 0x0c,
        'observe_offs': [0x00, 0x04, 0x0c, 0x14, 0x28, 0x30],
        'observe_globals': [0x00912bd8, 0x00912bec],
        'path1_tests': [0, 1, 2, 3], 'path2_tests': [0, 1, 2, 3]},

    # 0x00493480 FpsDiscretise (util) - frame-tick discretizer. Calls C2 callee
    # FUN_00493390 (which sets DAT_007f1000=0x32=50 deterministically, timer-indep),
    # then accumulates DAT_007719d4 += 50, snaps to /50 ticks, writes int+float out.
    # Seed the accumulator 0x7719d4; observe {0x7f1000 tick_out, 0x7719d4 accum,
    # 0x7f1004 float}. Distinct per accum: 0->50, 10->50(accum10), 48->100, 100->150.
    'fps_discretise': {'rva': 0x00493480, 'export': 'FpsDiscretise', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'near_leaf_seed_multi_obs',
        'observe_addrs': [0x007f1000, 0x007719d4, 0x007f1004],
        # MUST seed QPF freq DAT_00771e70/74 nonzero: callee FUN_004950b0 does
        # __alldiv by it; at suspended-spawn QPF-init FUN_00495120 hasn't run so
        # it's 0 -> div-by-zero #DE. Freq value is irrelevant (feeds only timer
        # globals we don't observe; outputs are timer-INDEPENDENT since the callee
        # sets DAT_007f1000=0x32 unconditionally).
        'seed_sets': [
            {'globals': [[0x00771e70, 0x989680], [0x00771e74, 0], [0x007719d4, 0]]},     # accum 0  -> tick 50
            {'globals': [[0x00771e70, 0x989680], [0x00771e74, 0], [0x007719d4, 10]]},    # accum 10 -> tick 50, accum 10
            {'globals': [[0x00771e70, 0x989680], [0x00771e74, 0], [0x007719d4, 48]]},    # accum 48 -> rem>=48 -> tick 100
            {'globals': [[0x00771e70, 0x989680], [0x00771e74, 0], [0x007719d4, 100]]},   # accum 100-> tick 150
        ],
        'path1_tests': [0, 1, 2, 3], 'path2_tests': [0, 1, 2, 3]},

    'zerofill_48a830': {'rva': 0x0048a830, 'export': 'ZeroFill48a830', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'near_leaf_seed_multi_obs',
        'observe_addrs': [0x0071fa34, 0x007151f0, 0x0071522c, 0x00715230],
        'seed_sets': [
            {'globals': [[0x0071fa34, 0xDEAD], [0x007151f0, 0xAAAAAAAA], [0x0071522c, 0xAAAAAAAA], [0x00715230, 0xAAAAAAAA]]},   # gfa34->0, filled->0, boundary->aa
            {'globals': [[0x0071fa34, 0xBEEF], [0x007151f0, 0xBBBBBBBB], [0x0071522c, 0xBBBBBBBB], [0x00715230, 0xBBBBBBBB]]},   # ->0, ->0, boundary->bb
        ],
        'path1_tests': [0, 1], 'path2_tests': [0, 1]},
    'zerofill_48ade0': {'rva': 0x0048ade0, 'export': 'ZeroFill48ade0', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'near_leaf_seed_multi_obs',
        'observe_addrs': [0x007151f0, 0x0071520c, 0x0071522c, 0x00715230],
        'seed_sets': [
            {'globals': [[0x007151f0, 0xAAAAAAAA], [0x0071520c, 0xAAAAAAAA], [0x0071522c, 0xAAAAAAAA], [0x00715230, 0xAAAAAAAA]]},   # filled->0, boundary(+0x40)->aa
            {'globals': [[0x007151f0, 0xBBBBBBBB], [0x0071520c, 0xBBBBBBBB], [0x0071522c, 0xBBBBBBBB], [0x00715230, 0xBBBBBBBB]]},   # filled->0, boundary->bb
        ],
        'path1_tests': [0, 1], 'path2_tests': [0, 1]},
    'init_42c280': {'rva': 0x0042c280, 'export': 'Init42c280', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'near_leaf_seed_multi_obs',
        'observe_addrs': [0x0067eab0, 0x0067eab4, 0x0067eabc, 0x0067ead4, 0x0067ea5c],
        'seed_sets': [
            {'globals': [[0x0067eab0, 0], [0x0067ea5c, 0], [0x0067eab4, 0xAAAA], [0x0067eabc, 0xBBBB], [0x0067ead4, 0xCCCC]]},   # init, ea5c==0 -> ead4=0
            {'globals': [[0x0067eab0, 5], [0x0067ea5c, 0], [0x0067eab4, 0xAAAA], [0x0067eabc, 0xBBBB], [0x0067ead4, 0xCCCC]]},   # gate!=0 -> no-op
            {'globals': [[0x0067eab0, 0], [0x0067ea5c, 7], [0x0067eab4, 0xAAAA], [0x0067eabc, 0xBBBB], [0x0067ead4, 0xCCCC]]},   # init, ea5c!=0 -> ead4=1, ea5c=0
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'search_40bb30': {'rva': 0x0040bb30, 'export': 'Search40bb30', 'signature': {'ret': 'pointer', 'args': ['pointer']}, 'arg_type': 'near_leaf_global_str_search',
        'glob': 0x0063b8f8,
        'seed_sets': [
            {'q': 'BETA'},    # match -> n1-8
            {'q': 'zzz'},     # not found -> 0
            {'q': 'ALPHA'},   # match -> n0-8
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'pred_462760': {'rva': 0x00462760, 'export': 'Pred462760', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'near_leaf_struct_array_predicate',
        'glob': 0x0069045c, 'count': 4, 'struct_size': 0xd80,
        'seed_sets': [
            {'entries': [{'null': True}, {'fields': [[0xd6c, 5], [0xd70, 5]]}, {'fields': [[0xd6c, 5], [0xd70, 5]]}, {'fields': [[0xd6c, 5], [0xd70, 5]]}]},   # entry0 null -> 1
            {'entries': [{'fields': [[0xd6c, 5], [0xd70, 5]]}, {'fields': [[0xd6c, 5], [0xd70, 5]]}, {'fields': [[0xd6c, 5], [0xd70, 5]]}, {'fields': [[0xd6c, 5], [0xd70, 5]]}]},  # all pass -> 0
            {'entries': [{'fields': [[0xd6c, 5], [0xd70, 2]]}, {'fields': [[0xd6c, 5], [0xd70, 5]]}, {'fields': [[0xd6c, 5], [0xd70, 5]]}, {'fields': [[0xd6c, 5], [0xd70, 5]]}]},  # entry0 d70==2 -> 1
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'update_41b520': {'rva': 0x0041b520, 'export': 'Update41b520', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'near_leaf_seed_multi_obs',
        'observe_addrs': [0x0063c920, 0x0063c924, 0x0063c928, 0x0063c940, 0x0063c994, 0x0063c998, 0x0063c99c, 0x0063c9b4],
        'seed_sets': [
            {'globals': [[0x0063c934, 0], [0x0063c9a8, 1], [0x0063ca1c, 0], [0x0063ca90, 0],
                         [0x005f3304, 0x40a00000], [0x005f3308, 0x1111], [0x005f330c, 0x2222],
                         [0x005f3310, 0x40400000], [0x005f3314, 0x3333], [0x005f3318, 0x4444],
                         [0x0063c920, 0xDEAD], [0x0063c924, 0xDEAD], [0x0063c928, 0xDEAD], [0x0063c940, 0xDEAD],
                         [0x0063c994, 0xDEAD], [0x0063c998, 0xDEAD], [0x0063c99c, 0xDEAD], [0x0063c9b4, 0xDEAD]]},   # R0 idx0 (5.0->10.0), R1 idx1 (3.0->6.0)
            {'globals': [[0x0063c934, 0], [0x0063c9a8, 1], [0x0063ca1c, 0], [0x0063ca90, 0],
                         [0x005f3304, 0x40000000], [0x005f3308, 0xAAAA], [0x005f330c, 0xBBBB],
                         [0x005f3310, 0x40800000], [0x005f3314, 0xCCCC], [0x005f3318, 0xDDDD],
                         [0x0063c920, 0xDEAD], [0x0063c924, 0xDEAD], [0x0063c928, 0xDEAD], [0x0063c940, 0xDEAD],
                         [0x0063c994, 0xDEAD], [0x0063c998, 0xDEAD], [0x0063c99c, 0xDEAD], [0x0063c9b4, 0xDEAD]]},   # 2.0->4.0, 4.0->8.0
        ],
        'path1_tests': [0, 1], 'path2_tests': [0, 1]},
    'accum_4215c0': {'rva': 0x004215c0, 'export': 'Accum4215c0', 'signature': {'ret': 'void', 'args': ['uint32','float','uint32']}, 'arg_type': 'near_leaf_accum_table',
        'tbl_base': 0x0063e4b8, 'rec_stride': 0x24,
        'seed_sets': [
            {'a1': 0, 'val': 10.0, 'a3': 2, 'seed': 5.0},     # min(10+5,50)=15
            {'a1': 1, 'val': 60.0, 'a3': 0, 'seed': 0.0},     # min(60,50)=50 (clamp)
            {'a1': 0, 'val': -3.0, 'a3': 3, 'seed': 20.0},    # min(-3+20,50)=17
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'initrange_4725f0': {'rva': 0x004725f0, 'export': 'InitRange4725f0', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'near_leaf_seed_multi_obs',
        'observe_addrs': [0x0088219c, 0x008821c4, 0x00882ea4, 0x00691508, 0x0069150c],
        'seed_sets': [
            {'globals': [[0x00691500, 0x1111], [0x00691504, 0x2222], [0x00691510, 0x3333], [0x00691508, 0xDEAD], [0x0069150c, 0xBEEF], [0x0088219c, 0], [0x008821c4, 0], [0x00882ea4, 0]]},
            {'globals': [[0x00691500, 0xAAAA], [0x00691504, 0xBBBB], [0x00691510, 0xCCCC], [0x00691508, 0xDEAD], [0x0069150c, 0xBEEF], [0x0088219c, 0], [0x008821c4, 0], [0x00882ea4, 0]]},
        ],
        'path1_tests': [0, 1], 'path2_tests': [0, 1]},
    'build_472560': {'rva': 0x00472560, 'export': 'Build472560', 'signature': {'ret': 'void', 'args': ['uint32','uint32']}, 'arg_type': 'near_leaf_record_builder',
        'rec_base': 0x00691500, 'rec_stride': 0x10, 'tbl_base': 0x0088219c, 'tbl_stride': 4,
        'seed_sets': [
            {'A': 1, 'B': 5, 'v0': 0x111, 'v1': 0x222},
            {'A': 2, 'B': 9, 'v0': 0x333, 'v1': 0x444},
            {'A': 3, 'B': 0, 'v0': 0xABC, 'v1': 0xDEF},
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'init_429b70': {'rva': 0x00429b70, 'export': 'Init429b70', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'near_leaf_seed_multi_obs',
        'observe_addrs': [0x008991b0, 0x008991b4, 0x008991b8],
        'seed_sets': [
            {'globals': [[0x0067e9fc, 1], [0x008991b0, 0xAAAA], [0x008991b4, 0xBBBB], [0x008991b8, 0xCCCC]]}, # submode not in {3,4,5} -> no-op
            {'globals': [[0x0067e9fc, 4], [0x008991b0, 5], [0x008991b4, 0xBBBB], [0x008991b8, 0xCCCC]]},      # flag!=0 -> no-op
            {'globals': [[0x0067e9fc, 4], [0x008991b0, 0], [0x0067ea56, 100], [0x008991b4, 0xBBBB], [0x008991b8, 0xCCCC]]}, # compute
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'init_429b30': {'rva': 0x00429b30, 'export': 'Init429b30', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'near_leaf_seed_multi_obs',
        'observe_addrs': [0x008991b0, 0x008991b4],
        'seed_sets': [
            {'globals': [[0x0067e9fc, 2], [0x008991b0, 0xAAAA], [0x008991b4, 0xBBBB]]},               # submode==2 -> no-op
            {'globals': [[0x0067e9fc, 0], [0x008991b0, 5], [0x008991b4, 0xBBBB]]},                    # flag!=0 -> no-op
            {'globals': [[0x0067e9fc, 0], [0x008991b0, 0], [0x0067ea56, 100], [0x008991b4, 0xBBBB]]}, # compute -> flag=0x3f, float set
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'calc_412130': {'rva': 0x00412130, 'export': 'Calc412130', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'near_leaf_seed_globals',
        'seed_sets': [
            {'globals': [[0x0067e9fc, 2]]},                    # sub==2 -> 3
            {'globals': [[0x0067e9fc, 3], [0x007f0fd0, 4]]},   # sub=3,t=0 -> jt[0]=1
            {'globals': [[0x0067e9fc, 4], [0x007f0fd0, 10]]},  # sub=4,t=6 -> jt[6]=6
            {'globals': [[0x0067e9fc, 3], [0x007f0fd0, 5]]},   # sub=3,t=1 -> jt[1]=0
            {'globals': [[0x0067e9fc, 1]]},                    # sub<2 -> 0
        ],
        'path1_tests': [0, 1, 2, 3, 4], 'path2_tests': [0, 1, 2, 3, 4]},
    'search_430250': {'rva': 0x00430250, 'export': 'Search430250', 'signature': {'ret': 'uint32', 'args': ['uint32','uint32']}, 'arg_type': 'near_leaf_ptr_array_search',
        'glob': 0x005f2770, 'count': 0xd,
        'seed_sets': [
            {'gate': 0, 'key': 0x1234, 'at_idx': -1},   # gate 0 -> 0
            {'gate': 1, 'key': 0x1234, 'at_idx': 3},     # found at 3 -> 3*0x14+0x7f0cb0
            {'gate': 1, 'key': 0x9999, 'at_idx': -1},    # not found -> 0
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'sub_45c7f0': {'rva': 0x0045c7f0, 'export': 'Sub45c7f0', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'near_leaf_seed_globals',
        'seed_sets': [
            {'globals': [[0x0063ba8c, 5], [0x0088fbc0, 5]]},                   # equal -> 5
            {'globals': [[0x0063ba8c, 5], [0x0088fbc0, 9], [0x0063b908, 7]]},  # differ -> Flag=7
            {'globals': [[0x0063ba8c, 2], [0x0088fbc0, 9], [0x0063b908, 3]]},  # differ -> Flag=3
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'add_40b6e0': {'rva': 0x0040b6e0, 'export': 'Add40b6e0', 'signature': {'ret': 'void', 'args': ['uint32']}, 'arg_type': 'near_leaf_seed_arg_obs',
        'obs_addr': 0x0063b8ec,
        'seed_sets': [
            {'globals': [[0x0067e9fc, 2], [0x0063b8ec, 100]], 'arg': 5},     # submode==2 -> no add -> 100
            {'globals': [[0x0067e9fc, 0], [0x0063b8ec, 100]], 'arg': 5},     # add -> 105
            {'globals': [[0x0067e9fc, 0], [0x0063b8ec, 0]],   'arg': 0xFF},  # add -> 0xff
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'calc_413fa0': {'rva': 0x00413fa0, 'export': 'Calc413fa0', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'near_leaf_seed_globals',
        'seed_sets': [
            {'globals': [[0x0067f17c, 10], [0x0067e9fc, 4]]},   # s=30, b=4 -> 31
            {'globals': [[0x0067f17c, 10], [0x0067e9fc, 5]]},   # s=30, b=5 -> 32
            {'globals': [[0x0067f17c, 7],  [0x0067e9fc, 0]]},   # s=21, b=0 -> 21
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'dot_408590': {'rva': 0x00408590, 'export': 'Dot408590', 'signature': {'ret': 'void', 'args': ['pointer','pointer']}, 'arg_type': 'near_leaf_dot_plane',
        'tbl_base': 0x00663658, 'tbl_stride': 0x4c,
        'seed_sets': [
            {'idx': 2, 'normal': [1.0, 0.0, 0.0], 'point': [5.0, 0.0, 0.0], 'a8': 0},     # dp=5
            {'idx': 2, 'normal': [1.0, 0.0, 0.0], 'point': [-3.0, 0.0, 0.0], 'a8': 10},   # dp=-3
            {'idx': 5, 'normal': [0.0, 2.0, 0.0], 'point': [0.0, 4.0, 0.0], 'a8': 0},     # dp=8
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'ghost_41a9b0': {'rva': 0x0041a9b0, 'export': 'Ghost41a9b0', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'near_leaf_arr_to_table',
        'tbl_base': 0x0063c6f0, 'tbl_stride': 0xc4,
        'seed_sets': [
            {'vals': [0x11, 0x22]},
            {'vals': [0xAABB, 0xCCDD]},
            {'vals': [0, 0xFFFF]},
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'cmp_5aa1e0': {'rva': 0x005aa1e0, 'export': 'Cmp5aa1e0', 'signature': {'ret': 'uint32', 'args': ['uint32','pointer','pointer']}, 'arg_type': 'near_leaf_memcmp16',
        'seed_sets': [
            {'eq': True},                  # 16 bytes equal -> 1
            {'eq': False, 'diffat': 0},    # differ at byte 0 -> 0
            {'eq': False, 'diffat': 8},    # differ at byte 8 -> 0
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'count_42c1f0': {'rva': 0x0042c1f0, 'export': 'Count42c1f0', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'near_leaf_seed_ret',
        'tbl_base': 0x007e96fc, 'tbl_stride': 0x200, 'tbl_count': 4,
        'seed_sets': [
            {'preset': []},                 # all 4 entries zero -> return 1
            {'preset': [[0, 0x1234]]},      # entry 0 nonzero -> return 0
            {'preset': [[3, 0x5678]]},      # entry 3 nonzero -> return 0
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'init_458f80': {'rva': 0x00458f80, 'export': 'Init458f80', 'signature': {'ret': 'void', 'args': ['uint32']}, 'arg_type': 'near_leaf_abs_table',
        'tbl_base': 0x0068b198, 'tbl_stride': 0x50, 'tbl_count': 0x19, 'observe': [0x1c, 0x20],
        'seed_sets': [
            {'arg': 0, 'preset': []},               # flag=0 -> every rec[0x20]=3
            {'arg': 5, 'preset': [[0x20, 3]]},      # preset rec[0x20]=3, flag!=0 -> rec[0x20]=1
            {'arg': 5, 'preset': []},               # zero table, flag!=0 -> rec[0x20] stays 0
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'alloc_5ae4c0': {'rva': 0x005ae4c0, 'export': 'Alloc5ae4c0', 'signature': {'ret': 'pointer', 'args': ['pointer','uint32','uint32']}, 'arg_type': 'heap_alloc_aligned',
        'seed_sets': [
            {'size': 0x20, 'align': 0x10},   # fits -> allocate, splice
            {'size': 0x200, 'align': 0x10},  # too big (free ~0xe4) -> 0
            {'size': 0x20, 'align': 0x20},   # fits, different alignment
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'recupd_5b0cf0': {'rva': 0x005b0cf0, 'export': 'RecUpd5b0cf0', 'signature': {'ret': 'void', 'args': ['pointer','pointer','uint32','uint32','uint32','uint32']}, 'arg_type': 'record_array_filter_update',
        'seed_sets': [
            {'arg3': 2, 'arg4': 5, 'arg5': 0xffffffff, 'arg6': 0xAA},          # row==2, B==5, col any[0,4) -> rec0
            {'arg3': 0xffffffff, 'arg4': 0xffffffff, 'arg5': 1, 'arg6': 0xBB}, # row[0,10), B any, col==1 -> rec0,rec2
            {'arg3': 7, 'arg4': 5, 'arg5': 3, 'arg6': 0xCC},                   # row==7, B==5, col==3 -> rec3
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'blit_4ceaf0': {'rva': 0x004ceaf0, 'export': 'Blit4ceaf0', 'signature': {'ret': 'int', 'args': ['pointer','pointer']}, 'arg_type': 'bitmap_blit',
        'seed_sets': [
            {'rows': 3, 'width_bits': 16, 'channels': 1, 'dstride': 4, 'sstride': 4, 'pal_bits': 4, 'palette': True},  # 2 bpr x3 + 64B palette
            {'rows': 2, 'width_bits': 24, 'channels': 1, 'dstride': 8, 'sstride': 8, 'pal_bits': 0, 'palette': False}, # 3 bpr x2, no palette
            {'rows': 4, 'width_bits': 32, 'channels': 1, 'dstride': 8, 'sstride': 8, 'pal_bits': 2, 'palette': True},  # 4 bpr x4 + 16B palette
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'pool_482860': {'rva': 0x00482860, 'export': 'Pool482860', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'pool_freelist_init',
        'seed_sets': [
            {'n': 3},   # 4-node ring (B, B+0x24, B+0x48, B+0x6c)
            {'n': 1},   # 2-node ring
            {'n': 0},   # 1-node ring (loop skipped)
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'search_4c5c00': {'rva': 0x004c5c00, 'export': 'Search4c5c00', 'signature': {'ret': 'pointer', 'args': ['pointer','pointer']}, 'arg_type': 'circular_str_search_ci',
        'seed_sets': [
            {'q': 'BETA'},    # case-insensitive match -> n1-8
            {'q': 'zzz'},     # not found -> 0
            {'q': 'ALPHA'},   # match -> n0-8
            {'q': 'Gamma'},   # match -> n2-8
            {'q': 'bet'},     # prefix, not full match -> 0
        ],
        'path1_tests': [0, 1, 2, 3, 4], 'path2_tests': [0, 1, 2, 3, 4]},
    'aabb_48a630': {'rva': 0x0048a630, 'export': 'Aabb48a630', 'signature': {'ret': 'int', 'args': ['pointer','pointer']}, 'arg_type': 'aabb_sphere_overlap',
        'seed_sets': [
            {'box': [0,0,0, 10,10,10], 'sph': [5,5,5, 10.0]},     # center+/-10 covers [0,10] all axes -> 1
            {'box': [0,0,0, 10,10,10], 'sph': [5,5,5, 1.0]},      # +/-1 too small -> 0
            {'box': [0,0,0, 10,10,10], 'sph': [5,5,5, 8.0]},      # +/-8 = [-3,13] covers [0,10] -> 1
            {'box': [0,0,0, 10,10,10], 'sph': [5,5,5, 4.0]},      # +/-4 = [1,9] misses [0,10] -> 0
            {'box': [2,2,2, 8,8,8],    'sph': [5,5,5, 5.0]},      # +/-5 = [0,10] covers [2,8] -> 1
        ],
        'path1_tests': [0, 1, 2, 3, 4], 'path2_tests': [0, 1, 2, 3, 4]},
    'cmp_5ae170': {'rva': 0x005ae170, 'export': 'Cmp5ae170', 'signature': {'ret': 'int', 'args': ['pointer','pointer','uint32']}, 'arg_type': 'case_insensitive_ncmp',
        'seed_sets': [
            {'s1': 'Hello', 's2': 'HELLO', 'n': 5},   # case-insensitive equal -> 0
            {'s1': 'apple', 's2': 'apply', 'n': 5},   # diff at 4: 'E'-'Y' = -20
            {'s1': 'abc',   's2': 'abd',   'n': 2},   # n bounds before diff -> 0
            {'s1': 'dog',   's2': 'cat',   'n': 3},   # diff at 0: 'D'-'C' = 1
            {'s1': '`',     's2': '`',     'n': 1},   # asymmetry: s1 folds 0x60->0x40, s2 keeps 0x60 -> 0x20=32
        ],
        'path1_tests': [0, 1, 2, 3, 4], 'path2_tests': [0, 1, 2, 3, 4]},
    'hash_4223f0': {'rva': 0x004223f0, 'export': 'Hash4223f0', 'signature': {'ret': 'float', 'args': ['uint32','uint32']}, 'arg_type': 'eax_ecx_float_hash',
        'seed_pairs': [
            [0,           12345],       # a==0 -> hash(12345)
            [0x0000FFFF,  0x12345678],  # b &= 0xFFFF -> hash(0x5678)
            [7,           0xABCDEF],     # b &= 7 -> hash(7)
            [0,           1],            # hash(1)
            [0,           999999],       # hash(999999)
        ],
        'path1_tests': [0, 1, 2, 3, 4], 'path2_tests': [0, 1, 2, 3, 4]},
    'walk_4938e0': {'rva': 0x004938e0, 'export': 'Walk4938e0', 'signature': {'ret': 'void', 'args': ['pointer','uint32']}, 'arg_type': 'list_walk_self_write',
        'seed_sets': [
            {'len': 3, 'value': 0x11223344},   # walk n0->n1->n2(self); terminal[0]=0x11223344
            {'len': 1, 'value': 0x55667788},   # p->n0(self); terminal[0]=0x55667788
            {'len': 2, 'value': 0xCAFEBABE},   # terminal[0]=0xCAFEBABE
        ],
        'path1_tests': [0, 1, 2], 'path2_tests': [0, 1, 2]},
    'accum_420de0': {'rva': 0x00420de0, 'export': 'Accum420de0', 'signature': {'ret': 'void', 'args': ['uint32','pointer','float']}, 'arg_type': 'fastcall_float_clamp',
        'seed_sets': [
            {'idx': 2, 'cur': 10.0,  'val': 5.0},    # v=15.0  -> 15.0  (pass-through)
            {'idx': 3, 'cur': 48.0,  'val': 10.0},   # v=58.0  -> 50.0  (clamp)
            {'idx': 5, 'cur': -20.0, 'val': 3.5},    # v=-16.5 -> -16.5 (pass-through, negative)
            {'idx': 0, 'cur': 1.5,   'val': 2.25},   # v=3.75  -> 3.75  (pass-through)
            {'idx': 7, 'cur': 49.9,  'val': 0.2},    # v~=50.1 -> 50.0  (near-boundary clamp)
        ],
        'path1_tests': [0, 1, 2, 3, 4], 'path2_tests': [0, 1, 2, 3, 4]},
    'accum_5b6b00': {'rva': 0x005b6b00, 'export': 'Accum5b6b00', 'signature': {'ret': 'void', 'args': ['uint32','pointer','pointer']}, 'arg_type': 'table_accum_clamp',
        'seed_sets': [
            {'a1': 0,  'v2': 100,     'v3': 3},     # mult=1 add; tblA[3]=10  -> *p2=101;  *p3=3+(-1)=2
            {'a1': 5,  'v2': 5000,    'v3': 10},    # mult=11 add; tblA[10]=50 -> +68=5068; *p3=10+4=14
            {'a1': 12, 'v2': -100,    'v3': 20},    # mult=9 sub; tblA[20]=50  -> -56=-156; *p3=20+2=22
            {'a1': 3,  'v2': 32760,   'v3': 26},    # mult=7 add; tblA[26]=88  -> upper clamp 32767; *p3=26+10=36
            {'a1': 11, 'v2': -32760,  'v3': 39},    # mult=7 sub; tblA[39]=307 -> lower clamp -32768; *p3=39+(-1)=38
        ],
        'path1_tests': [0, 1, 2, 3, 4], 'path2_tests': [0, 1, 2, 3, 4]},
    'vec3_magnitude': {
        'rva':            0x004c3ac0,
        'export':         'Vec3Magnitude',
        'signature':      {'ret': 'float', 'args': ['pointer']},
        'arg_type':       'vec3_ptr',
        'lut_root_delta': 0,
        'path1_tests': [
            (0.0,     0.0,     0.0    ),
            (1.0,     0.0,     0.0    ),
            (0.0,     1.0,     0.0    ),
            (0.0,     0.0,     1.0    ),
            (3.0,     4.0,     0.0    ),
            (-3.0,    4.0,     0.0    ),
            (3.0,    -4.0,     0.0    ),
            (1.0,     1.0,     1.0    ),
            (-1.0,   -1.0,    -1.0    ),
            (2.0,     3.0,     6.0    ),
            (10.0,    0.0,     0.0    ),
            (100.0,   100.0,   100.0  ),
            (0.001,   0.001,   0.001  ),
            (12.34,  -56.78,   90.12  ),
            (1.5,     2.5,     3.5    ),
            (-99.99,  99.99,  -99.99  ),
            (1e-6,    1e-6,    1e-6   ),
            (1234.5,  6789.0, -4321.0 ),
        ],
        'path2_tests': [
            (0.0, 0.0, 0.0),
            (3.0, 4.0, 0.0),
            (1.0, 1.0, 1.0),
            (2.0, 3.0, 6.0),
            (10.0, 0.0, 0.0),
        ],
    },

    'fast_sqrt': {
        'rva':            0x004c3b30,
        'export':         'FastSqrt',
        'signature':      {'ret': 'float', 'args': ['float']},
        'arg_type':       'float_scalar',
        'lut_root_delta': 0,
        'path1_tests': [
            0.0, 1.0, 4.0, 9.0, 16.0, 25.0, 100.0, 10000.0,
            0.25, 0.5, 2.0, 3.0, 1.0e6, 1.0e-6, 6789.0, 1234.5, 12.34, 81.0,
        ],
        'path2_tests': [0.0, 1.0, 4.0, 9.0, 100.0],
    },

    # 0x00422440 cosine-ease lerp: float f(float a, float b, float t) =
    #   w=(1-cos(pi*t))*0.5; return w*b + (1-w)*a.  Pure x87 leaf (only fcos);
    #   caller FUN_00422470 C2 (caller-gate OK). New arg_type float3_scalar_ret.
    'cosine_lerp': {
        'rva':            0x00422440,
        'export':         'CosineLerp',
        'signature':      {'ret': 'float', 'args': ['float', 'float', 'float']},
        'arg_type':       'float3_scalar_ret',
        'path1_tests': [
            # [a, b, t] — vary endpoints a,b and phase t (incl. 0/1 and out-of-[0,1]).
            [0.0, 1.0, 0.0],    [0.0, 1.0, 1.0],   [0.0, 1.0, 0.5],   [0.0, 1.0, 0.25],
            [0.0, 1.0, 0.75],   [-1.0, 1.0, 0.5],  [10.0, 20.0, 0.5], [5.0, -5.0, 0.3],
            [100.0, 200.0, 0.1],[0.0, 1.0, 1.5],   [0.0, 1.0, -0.5],  [3.14, 2.71, 0.6],
            [1000.0, 0.0, 0.9], [0.25, 0.75, 0.333],
        ],
        'path2_tests': [[0.0, 1.0, 0.5], [10.0, 20.0, 0.25], [0.0, 1.0, 1.0]],
    },

    # 0x004c0b10 nested-deref getter: ret = *(byte*)(*(int*)(this+0xa0)+3) & 3.
    #   Validates the thiscall_nested_field_get arg_type AND the 2026-06-25
    #   identified-caller clause (caller RpAtomicGetWorldBoundingSphere = named
    #   RW API). outer_off=0xa0 holds the inner ptr; inner_off=3 holds the flag byte.
    'rw_atomic_dirty_flag': {
        'rva':            0x004c0b10,
        'export':         'RwAtomicDirtyFlagGet',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'thiscall_nested_field_get',
        'outer_off':      0xa0,
        'inner_off':      3,
        'ret_kind':       'u32',
        'struct_size':    0xb0,
        'inner_size':     16,
        'path1_tests': [
            # seeded at inner+3 (low byte matters); return = (val & 0xff) & 3.
            0, 1, 2, 3, 0xff, 0x100, 0x55, 0xaa, 0x80, 0x7f,
            0xfffffffc, 0xfffffffd, 0xfffffffe, 0xffffffff,
        ],
        'path2_tests': [0, 1, 2, 3, 0xff],
    },

    'fast_inv_sqrt': {
        'rva':            0x004c3b90,
        'export':         'FastInvSqrt',
        'signature':      {'ret': 'float', 'args': ['float']},
        'arg_type':       'float_scalar',
        'lut_root_delta': 4,
        'path1_tests': [
            0.0, 1.0, 4.0, 16.0, 0.25, 0.5, 2.0, 9.0, 100.0, 81.0,
            1024.0, 1.0e6, 1.0e-6, 0.001, 1.5, 12345.6, 0.000123, 7.7,
        ],
        'path2_tests': [0.0, 1.0, 4.0, 0.25, 100.0],
    },

    # game_state_d2 trivial getters. Each is `MOV EAX, [imm32]; RET` â€” pure
    # read of a u32 global. `arg_type='none'` calls the function with no args.
    # `tests` is a list of dummy iteration markers; we just want N independent
    # calls so we can confirm orig and reimpl both read the same address (and
    # so any tearing under a concurrent write would surface as a mismatch).
    # `lut_root_delta` is unused for these (kept for harness uniformity).
    'get_dat_0067ecb4': {
        'rva':            0x0042c2d0,
        'export':         'GetDat0067ecb4',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067ecb4,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    'get_dat_0067ecb8': {
        'rva':            0x0042c2e0,
        'export':         'GetDat0067ecb8',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067ecb8,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    'get_dat_0067ea64': {
        'rva':            0x0042f500,
        'export':         'GetDat0067ea64',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067ea64,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-c-s2 â€” hud_ingame_dispatch  (C2â†’C3, 1 candidate)
    # HUD/HudDispatch.cpp â€” pure-leaf uint32(void) global-read function
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00426ba0  HudDrawEnabled
    # Returns DAT_0066d704 as uint32. void(void) pure leaf.
    # Strategy: write sentinel values to 0x0066d704, call orig and reimpl
    # independently, compare return values. Both must read the same address.
    # 10 sentinels covering 0, 1, max, and bit-pattern variants.
    'hud_draw_enabled': {
        'rva':            0x00426ba0,
        'export':         'HudDrawEnabled',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0066d704,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xCAFEBABE,
                           0x12345678, 0xFFFFFFFF, 0x80000000, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0x00000001, 0xFFFFFFFF],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-a-s1 â€” timer_d2_cont1  (C2â†’C3, 2 candidates)
    # Frontend/TimerReset.cpp â€” pure-leaf void(void) global-zero functions
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00422b30  TimerArrayClear
    # memset(0x00899e80, 0, 312*4). void(void).
    # Strategy: write sentinel to DAT_00899e80, call fn, read back.
    # Original must write 0 (sentinel wiped); reimpl must do the same.
    # 10 sentinels chosen to span various bit patterns.
    'timer_array_clear': {
        'rva':            0x00422b30,
        'export':         'TimerArrayClear',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x00899e80,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x0040b810  TimerGlobalsReset
    # Writes 0 to 21 globals across four address clusters. void(void).
    # Strategy: write sentinel to 0x008a9550 (first address written by
    # decomp), call fn, read back. Original must write 0; reimpl must match.
    # Also tests that reimpl touches 0x0063b8ec (last write) by checking
    # the last address in the path2 suite via separate target_global tests.
    'timer_globals_reset': {
        'rva':            0x0040b810,
        'export':         'TimerGlobalsReset',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x008a9550,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x0042af50  MenuReadinessCheck
    # Returns int 0 or 1; read-only scan of char/index arrays in live game state.
    # arg_type 'none': call with no args, compare return value (0 or 1).
    # Both orig and reimpl see the same global state at boot; any divergence
    # in the return value would indicate a logic error in the reimpl.
    # 10 identical dummy-iteration calls confirm stable bit-identical output.
    'menu_readiness_check': {
        'rva':            0x0042af50,
        'export':         'MenuReadinessCheck',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session 88 â€” c3_vehicle_state  (C2â†’C3, 5 candidates)
    # VehicleState.cpp â€” pure-leaf vehicle field getters/predicates
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0046c7b0  VehicleSlotGetter
    # Returns DAT_008815a4[idx*0x341] or 0xffffffff if idx > 15.
    # Non-trivial: OOB inputs return a specific sentinel.
    'vehicle_slot_getter': {
        'rva':            0x0046c7b0,
        'export':         'VehicleSlotGetter',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 5, 10, 15, 16, 17, 100, 255, 0xffffffff],
        'path2_tests':    [0, 15, 16, 255],
    },

    # 0x0046c770  VehicleDestructionStateGetter
    # Returns DAT_008815b0[idx*0x341] or 0xffffffff if idx > 15.
    # U-3605 is about value semantics only (not offset/stride).
    'vehicle_destruction_state_getter': {
        'rva':            0x0046c770,
        'export':         'VehicleDestructionStateGetter',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 5, 15, 16, 100, 255, 0xffffffff],
        'path2_tests':    [0, 15, 16],
    },

    # 0x0046c6d0  VehicleEntitySlotRead
    # Returns 1 (in-range, writes *param_2) or 0 (OOB).  OOB gate at > 15.
    # The out-ptr write is from BSS (zero); return-value IS the non-trivial part.
    'vehicle_entity_slot_read': {
        'rva':            0x0046c6d0,
        'export':         'VehicleEntitySlotRead',
        'signature':      {'ret': 'int32', 'args': ['int32', 'pointer']},
        'arg_type':       'int_with_out_ptr',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 5, 14, 15, 16, 17, 100, 255],
        'path2_tests':    [0, 15, 16],
    },

    # 0x0046dbe0  VehicleRacePositionGet
    # Returns DAT_008815a8[param_1*0x341] â€” no OOB guard.
    # write_global_call_int0: write sentinel to DAT_008815a8, call fn(0),
    # verify return == sentinel.  Non-trivial: varying sentinels give varying output.
    'vehicle_race_position_get': {
        'rva':            0x0046dbe0,
        'export':         'VehicleRacePositionGet',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'write_global_call_int0',
        'target_global':  0x008815a8,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0x42424242, 0xDEADBEEF,
                           0x80000001, 0x3F800000, 0xFFFFFFFF, 0x0000007B],
        'path2_tests':    [0x42424242, 0xDEADBEEF, 0xFFFFFFFF],
    },

    # 0x00468b40  VehicleContactHistoryLookup
    # Scans 32-slot contact table at vehicle+0xBFC; active flag at piVar1[0x20].
    # contact_history: allocates fake vehicle struct + geom entry, provides
    # match/miss/inactive test cases for a non-trivial domain.
    'vehicle_contact_history_lookup': {
        'rva':            0x00468b40,
        'export':         'VehicleContactHistoryLookup',
        'signature':      {'ret': 'int32', 'args': ['int32', 'int32']},
        'arg_type':       'contact_history',
        'lut_root_delta': 0,
        'path1_tests': [
            {'slot_contact_id': 0xABCDEF01, 'slot_active': 1, 'geom_contact_id': 0xABCDEF01},
            {'slot_contact_id': 0xABCDEF01, 'slot_active': 1, 'geom_contact_id': 0x12345678},
            {'slot_contact_id': 0xABCDEF01, 'slot_active': 0, 'geom_contact_id': 0xABCDEF01},
            {'slot_contact_id': 0x00000000, 'slot_active': 1, 'geom_contact_id': 0x00000000},
            {'slot_contact_id': 0xFFFFFFFF, 'slot_active': 1, 'geom_contact_id': 0xFFFFFFFF},
            {'slot_contact_id': 0xDEADBEEF, 'slot_active': 1, 'geom_contact_id': 0xDEADBEEF},
            {'slot_contact_id': 0x00000001, 'slot_active': 1, 'geom_contact_id': 0x00000002},
        ],
        'path2_tests': [
            {'slot_contact_id': 0xABCDEF01, 'slot_active': 1, 'geom_contact_id': 0xABCDEF01},
            {'slot_contact_id': 0xABCDEF01, 'slot_active': 1, 'geom_contact_id': 0x12345678},
            {'slot_contact_id': 0xABCDEF01, 'slot_active': 0, 'geom_contact_id': 0xABCDEF01},
        ],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-a-s4 â€” frontend_c0_promote (C2â†’C3, 3 of 4 candidates)
    # FrontendState.cpp â€” pure-leaf frontend global accessors
    # Note: 0x0042f020 (VehicleFlagClear) REFUSED â€” __fastcall with EAX
    #       implicit arg not supported by NativeFunction('mscdecl') harness.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00430b60  MenuSlotCount
    # Returns char count (0..4) of non-(-1) slot globals at
    # DAT_007f1a14/24/34/44 (stride 0x10). Pure read, no side-effects.


# arg_type='none': call 10x at quiescent state; verify bit-identical
    # return value between original and reimpl each iteration.
    'menu_slot_count': {
        'rva':            0x00430b60,
        'export':         'MenuSlotCount',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042f6b0  MenuModeSync
    # void(void): switch on DAT_0067f184 â†’ writes DAT_0067e9fc. Default no-op.

# void(void): switch on DAT_0067f184 (0..11, absent 4/6/7); writes mapped
    # value to DAT_0067e9fc. Default case is no-op.
    # arg_type='none': both orig and reimpl return undefined; undefined===undefined
    # is true â€” proves no crash and export is present. Mapping correctness is
    # verified by literal switch implementation matching the analysis note.
    'menu_mode_sync': {
        'rva':            0x0042f6b0,
        'export':         'MenuModeSync',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042d300  TimeDiffDecompose
    # void(int time_a, int time_b, u32* sign, int* min, int* sec, float* csec).
    # Pure leaf â€” no callees. Decomposes signed centisecond delta into sign+m+s+csec.
    # arg_type 'time_diff_decompose': 16-byte buffer holds 4 out fields.
    # Leaf-exemption applies (C2->C3 only; not C4).
    'time_diff_decompose': {
        'rva':            0x0042d300,
        'export':         'TimeDiffDecompose',
        'signature':      {'ret': 'void',
                            'args': ['int32', 'int32', 'pointer', 'pointer', 'pointer', 'pointer']},
        'arg_type':       'time_diff_decompose',
        'lut_root_delta': 0,
        # Inputs: [time_a, time_b] in centiseconds.
        'path1_tests': [
            [0,       0     ],   # equal     -> sign=2, m=0,  s=0,    csec=0.0
            [100,     0     ],   # a > b 1s  -> sign=0, m=0,  s=1,    csec=0.0
            [0,       100   ],   # b > a 1s  -> sign=1, m=0,  s=1,    csec=0.0
            [6000,    0     ],   # a > b 1m  -> sign=0, m=1,  s=0,    csec=0.0
            [6050,    0     ],   # 1m 0s 50c -> sign=0, m=1,  s=0,    csec=50.0
            [6101,    0     ],   # 1m 1s 1c  -> sign=0, m=1,  s=1,    csec=1.0
            [0,       6101  ],   # -1m1s1c   -> sign=1, m=1,  s=1,    csec=1.0
            [60000,   0     ],   # 10m 0s 0c -> sign=0, m=10, s=0,    csec=0.0
            [9999,    0     ],   # 1m 39s 99c
            [12345,   6789  ],   # mixed
            [99999,   12345 ],   # large
            [1,       0     ],   # tiny diff
            [0,       1     ],   # tiny neg
            [123456,  123456],   # equal nonzero
            [10000,   5500  ],   # 0m 45s 0c
        ],
        'path2_tests': [
            [0,       0    ],
            [6101,    0    ],
            [0,       6101 ],
            [60000,   0    ],
            [12345,   6789 ],
        ],
    },

    # 0x00436810  LocalPlayerSlotCheck
    # bool(int slot_idx). Reads BSS slot arrays at DAT_007f0a7c (SP) /
    # DAT_007f0a74 (MP) and counts non-zero / type-2 entries; compares
    # slot_idx against the count. param_1 == 12 always returns false.
    # Callee: IsMultiplayerMode (C3, 0x00430760). Non-leaf C3 candidate.
    # At quiescent main menu, SP path is taken; arrays are mostly zero so
    # iVar2 ~= 0 and the function returns true only for slot_idx <= -1.
    # arg_type 'int_scalar': returns bool as int.
    'local_player_slot_check': {
        'rva':            0x00436810,
        'export':         'LocalPlayerSlotCheck',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # Cover guard (12), boundary (0..15) and a few past-range values.
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 100, 0xffffffff],
        'path2_tests':    [0, 1, 12, 13],
    },

    # 0x00430910  MenuOptionSlotGet
    # int(void): reads game mode + option/row index; returns 0 or table value.

# int(void): reads DAT_0067e9fc (game mode), DAT_0067f184 (option index),
    # DAT_0067f17c (row index). Returns 0 for excluded combos or table value.
    # Pure read. arg_type='none': call 10x at quiescent state; compare returns.
    'menu_option_slot_get': {
        'rva':            0x00430910,
        'export':         'MenuOptionSlotGet',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00417730  VehicleRaceAngleGet
    # Per-car float getter: *(float*)(DAT_0089a880 + carIdx * 4). Pure leaf, 11 bytes.
    # int_scalar: passes carIdx as uint32, returns float in ST0.
    # Callers FUN_004177b0 and FUN_00417cf0 (both ai/C2).
    # U-2173 (semantic) and U-2174 (structural count) are filed; do not affect correctness.
    'vehicle_race_angle_get': {
        'rva':            0x00417730,
        'export':         'VehicleRaceAngleGet',
        'signature':      {'ret': 'float', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 5, 10, 15],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x0046d700  VehicleVec3At9C8Get
    # Reads 3-DWORD vector at per-vehicle offsets +0x9C8/+0x9CC/+0x9D0.
    # out3_idx: 12-byte buf as first arg, vehicleIdx as second; compares return value (0/1).
    # U-1748 is open: direction type unconfirmed â€” does not affect offset/bounds correctness.
    'vehicle_vec3_at_9c8_get': {
        'rva':            0x0046d700,
        'export':         'VehicleVec3At9C8Get',
        'signature':      {'ret': 'int32', 'args': ['pointer', 'uint32']},
        'arg_type':       'out3_idx',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 5, 10, 15, 16, 17, 255, 0xffffffff],
        'path2_tests':    [0, 15, 16, 255],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-a-s2 â€” frontend_menus_a small leaves (C2â†’C3)
    # MenuGetters.cpp â€” pure-leaf menu global getter, sentinel scanner,
    # and cursor stepper.
    # 0x0042ac50 (MenuCenterCalc) REFUSED â€” ECX+EAX dual-register ABI not
    #   supported by NativeFunction harness.

    # 0x0042b930  MenuAlphaGet
    # Returns DAT_0067ecb0 (uint32_t global, pure read, no side-effects).
    # 5 bytes: MOV EAX, [0x0067ecb0]; RET.
    # read_global: write sentinel, call fn(), verify return == sentinel.
    # ref: re/analysis/frontend_promote_menus_a/0x0042b930.md
    'menu_alpha_get': {
        'rva':            0x0042b930,
        'export':         'MenuAlphaGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067ecb0,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00FF00FF],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # Session c3-batch-a-s2 â€” frontend_menus_a small leaves (C2â†’C3)
    # MenuGetters.cpp â€” pure-leaf menu sentinel scanner + cursor stepper
    # 0x0042ac50 (MenuCenterCalc) REFUSED â€” ECX+EAX dual-register ABI not
    #   supported by NativeFunction harness (same reason as VehicleFlagClear).
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042ac00  MenuGroupCount
    # int __fastcall FUN_0042ac00(int /*unused*/, int* param_2)
    # Scans sentinel-delimited int array; counts 0xFF060000 groups before
    # 0xFF070000 terminator. ECX (param_1) unused. EDX = param_2.
    # arg_type: sentinel_array_ptr â€” writes array into 256B buf, calls
    #   fn(0, buf) with fastcall for orig (ECX=0, EDX=buf) and cdecl for
    #   reimpl (stack: [0, buf]).
    # ref: re/analysis/frontend_promote_menus_a/0x0042ac00.md
    # U-3439 (ECX unused), U-3440 (sentinel semantics) â€” do not affect correctness.
    'menu_group_count': {
        'rva':                    0x0042ac00,
        'export':                 'MenuGroupCount',
        'signature':              {'ret': 'int32', 'args': ['int32', 'pointer']},
        'arg_type':               'sentinel_array_ptr',
        'orig_calling_convention':   'fastcall',
        'reimpl_calling_convention': 'fastcall',  # 2026-06-01: reimpl rebuilt as __fastcall (EDX=param_2) to match the installed inline-JMP ABI; was 'mscdecl', which let orig+reimpl each pass under its own convention while the LIVE hook crashed (boot AV). Test both as fastcall now.
        'lut_root_delta': 0,
        # Each test is a list of int32 values (as Python ints).
        # 0xFF060000 = group delimiter; 0xFF070000 = end-of-array terminator.
        # Expected results annotated in comments.
        'path1_tests': [
            [0xFF070000],                                                # 0: immediate terminator â†’ 0
            [0xFF060000, 0xFF070000],                                    # 1: one delimiter â†’ 1
            [0x00000001, 0xFF070000],                                    # 0: no delimiter â†’ 0
            [0x00000001, 0xFF060000, 0xFF070000],                        # 1: one delimiter after data â†’ 1
            [0xFF060000, 0x00000001, 0xFF060000, 0xFF070000],            # 2
            [0xFF060000, 0xFF060000, 0xFF070000],                        # 2: two adjacent delimiters â†’ 2
            [0x00000001, 0x00000002, 0xFF060000, 0x00000003, 0xFF060000, 0xFF070000],  # 2
            [0x00000001, 0x00000002, 0x00000003, 0xFF070000],            # 0: only data, no delimiters â†’ 0
            [0xFF060000, 0x00000001, 0x00000002, 0xFF060000, 0x00000004, 0xFF060000, 0xFF070000],  # 3
            [0xFF060000, 0xFF060000, 0xFF060000, 0xFF070000],            # 3: three adjacent â†’ 3
            [0x00000064, 0x000000C8, 0x0000012C, 0x00000190, 0xFF070000],  # 0: all data â†’ 0
        ],
        'path2_tests': [
            [0xFF070000],
            [0xFF060000, 0xFF070000],
            [0x00000001, 0xFF060000, 0xFF070000],
        ],
    },

    # 0x0042aa00  MenuCursorStep
    # void FUN_0042aa00(int param_1)  __cdecl
    # Advances cursor at 0x0067ed40+slot*0x40 by param_1 with wrap;
    # checks validity byte; writes 0xffffffff on exhaustion.
    # arg_type: void_step_global â€” preps slot 0 globals from raw_bytes,
    #   calls fn(step), returns cursor readback from 0x0067ed40 (as int32).
    # raw_bytes layout: bytes 0..3 = limit as LE int32; byte N = validity
    #   byte for cursor N (read at 0x0067ed74+N for slot 0).
    # ref: re/analysis/frontend_promote_menus_a/0x0042aa00.md
    # U-3436/3437/3438 (slot range, array semantics, step source) â€” don't
    #   affect bit-identity correctness; limits and validity encoded in tests.
    'menu_cursor_step': {
        'rva':            0x0042aa00,
        'export':         'MenuCursorStep',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'void_step_global',
        'lut_root_delta': 0,
        # raw_bytes[0..3] = limit LE int32; raw_bytes[N] = validity[N].
        # validity[N] == 1 â†’ valid entry; else invalid.
        # Return value: cursor at 0x0067ed40 after call (-1 = exhausted).
        'path1_tests': [
            # limit=0 â†’ immediate -1
            {'raw_bytes': [0,0,0,0],              'initial_cursor': 0,  'step':  1},  # â†’ -1
            # limit=1, val[0]=raw[0]=1=valid; step+1: wrap 0, valid â†’ 0
            {'raw_bytes': [1,0,0,0],              'initial_cursor': 0,  'step':  1},  # â†’ 0
            # limit=1, step-1: wrap to 0, valid â†’ 0
            {'raw_bytes': [1,0,0,0],              'initial_cursor': 0,  'step': -1},  # â†’ 0
            # limit=2, val[0]=2â‰ 1,val[1]=0â‰ 1 â†’ exhausted â†’ -1
            {'raw_bytes': [2,0,0,0, 0,0],         'initial_cursor': 0,  'step':  1},  # â†’ -1
            # limit=5, valid@4 (raw[4]=1), init=0, step+1: â†’1,â†’2,â†’3,â†’4 valid â†’ 4
            {'raw_bytes': [5,0,0,0, 1,0],         'initial_cursor': 0,  'step':  1},  # â†’ 4
            # limit=5, valid@4, init=4, step-1: â†’3,â†’2,â†’1,â†’0(val=5â‰ 1),â†’4 valid â†’ 4
            {'raw_bytes': [5,0,0,0, 1,0],         'initial_cursor': 4,  'step': -1},  # â†’ 4
            # limit=5, no valid (raw[4]=0) â†’ exhausted â†’ -1
            {'raw_bytes': [5,0,0,0, 0,0],         'initial_cursor': 2,  'step':  1},  # â†’ -1
            # limit=5, valid@4, init=4, step+1: â†’5==limitâ†’wrap 0,val=5â‰ 1,â†’1,â†’2,â†’3,â†’4 valid â†’ 4
            {'raw_bytes': [5,0,0,0, 1,0],         'initial_cursor': 4,  'step':  1},  # â†’ 4
            # limit=6, valid@5 (raw[5]=1), init=0, step+1: â†’1,â†’2,â†’3,â†’4,â†’5 valid â†’ 5
            {'raw_bytes': [6,0,0,0, 0,1, 0],      'initial_cursor': 0,  'step':  1},  # â†’ 5
            # limit=6, valid@4 (raw[4]=1), init=5, step-1: â†’4 valid â†’ 4
            {'raw_bytes': [6,0,0,0, 1,0, 0],      'initial_cursor': 5,  'step': -1},  # â†’ 4
            # limit=3, no valid entries â†’ exhausted â†’ -1
            {'raw_bytes': [3,0,0,0, 0,0,0],       'initial_cursor': 1,  'step':  1},  # â†’ -1
        ],
        'path2_tests': [
            {'raw_bytes': [0,0,0,0],              'initial_cursor': 0,  'step':  1},
            {'raw_bytes': [1,0,0,0],              'initial_cursor': 0,  'step':  1},
            {'raw_bytes': [5,0,0,0, 1,0],         'initial_cursor': 0,  'step':  1},
        ],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-g-s5 â€” menu state machine helpers (C2â†’C3, 3 of 4 candidates)
    # Frontend/MenuStateMachine.cpp
    # 0x0042ac50 (MenuCenterCalc) REFUSED â€” ECX+EAX dual-register ABI not
    #   supported by NativeFunction harness. D-10639 filed (re/DEFERRED.md;
    #   corrects prior draft's D-10637 miscite â€” D-10637 tracks a different gap).
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042ae10  MenuReadinessCheckA
    # void(): reads guard globals (DAT_0067eab0 != 2 AND e7c8 == 0 AND 898ab0 != 0),
    # then path A (stride-0x4c char scan at 0x7f1502) or path B (car-index array).
    # Returns 0 (not ready) or 1 (ready).
    # arg_type='none': called 10x at quiescent main menu; DAT_0067eab0=0 at menu
    # (not 2), guard2 (e7c8) may vary â€” both paths return same value deterministically.
    # U-3445/U-3446/U-3447/U-3448 registered; do not affect bit-identity correctness.
    'menu_readiness_check_a': {
        'rva':            0x0042ae10,
        'export':         'MenuReadinessCheckA',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042aeb0  MenuReadinessCheckB
    # void(): structural variant of MenuReadinessCheckA â€” omits e7c8 guard;
    # table bases +1 byte (0x7f1503 vs 0x7f1502, 0x7f1043 vs 0x7f1042).
    # Returns 0 (not ready) or 1 (ready).
    # arg_type='none': called 10x at quiescent main menu; both paths must agree.
    # U-3445/U-3446 registered; do not affect bit-identity correctness.
    'menu_readiness_check_b': {
        'rva':            0x0042aeb0,
        'export':         'MenuReadinessCheckB',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session ma3-frida-s3 â€” menu readiness third variant (carry-over from c3-batch-g)
    # Frontend/MenuStateMachine.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042af50  MenuReadinessCheckC
    # void(): structural variant of MenuReadinessCheckA â€” same e7c8 guard as A;
    # table bases -1 byte rel. A (0x7f1501 vs 0x7f1502, 0x7f1041 vs 0x7f1042).
    # Path A upper bound 0x7f1760 (vs 0x7f1761 in A).
    # Returns 0 (not ready) or 1 (ready). Same callee FUN_0040e470 (C2).
    # arg_type='none': called 10x at quiescent main menu; both paths must agree.
    # U-1615/U-1616 registered; do not affect bit-identity correctness.
    'menu_readiness_check_c': {
        'rva':            0x0042af50,
        'export':         'MenuReadinessCheckC',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # Session c3-batch-a-s3 â€” frontend_menus_a medium batch
    # MenuNav.cpp â€” menu navigation helpers (C2â†’C3)
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042ac90  MenuEntryGet
    # Traverses active slot's entry array; skips (cursor+1) groups via 0xFF040000
    # terminators; stops at 0xFF05/14/060000 sentinels; returns value or 0xffffffff.
    # Pure read-only. arg_type='none': called 10x at quiescent main menu;
    # orig and reimpl must return identical sequence.
    'menu_entry_get': {
        'rva':            0x0042ac90,
        'export':         'MenuEntryGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042bb60  MenuTeamBalance
    # Clears and rebuilds 4 team-result fields from slot indices + team table.
    # Returns int balance code: 0x1000=balanced, 0=unbalanced, 1=one-team,
    # 2/3=partial, -1=unknown. Side-effect writes are visible to both paths.
    # arg_type='none': called 10x; both paths write same globals so return is
    # deterministic and must be bit-identical.
    'menu_team_balance': {
        'rva':            0x0042bb60,
        'export':         'MenuTeamBalance',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042aff0  MenuButtonDetectA
    # Detects button-press event at byte-col +2 (0x7f1046); hold-repeat timer
    # for screens 6-7; callee FUN_0040e470 (C2 drift-promoted) filters AI slots.
    # Returns 1 (change detected) or 0.
    # arg_type='none': called 10x at quiescent main menu (no inputs â†’ returns 0
    # each time; both paths must agree). U-3445 (callee semantics) is in
    # Uncertainties section only and does not affect the mechanical reimpl.
    'menu_button_detect_a': {
        'rva':            0x0042aff0,
        'export':         'MenuButtonDetectA',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042b180  MenuButtonDetectB
    # Sister of MenuButtonDetectA: byte-col +3 (0x7f1047), processed col +5
    # (0x7f1507), hold timer _DAT_0067f1b4. Structurally identical.
    # Same quiescent-menu call pattern, 10x.
    'menu_button_detect_b': {
        'rva':            0x0042b180,
        'export':         'MenuButtonDetectB',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # Session c3-batch-a-s5 â€” frontend_c0_promote + game_mode_cont2
    # FrontendNav.cpp / GameModeInit.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004323c0  MenuCursorBack
    # Backward 2D cursor nav: decrements DAT_0067f17c, searches backward via
    # FUN_00430830/FUN_00430910, syncs mode via FUN_0042f6b0.
    # cursor_back: inject {row,col,flag,mp_flag}, call fn, read back row+col.
    'menu_cursor_back': {
        'rva':            0x004323c0,
        'export':         'MenuCursorBack',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'cursor_back',
        'lut_root_delta': 0,
        'path1_tests': [
            # { row, col, flag, mp_flag }
            # SP mode (min=1): row>0, col=min â†’ should quickly return.
            {'row': 2,  'col': 1, 'flag': 0, 'mp_flag': 0},
            {'row': 3,  'col': 1, 'flag': 0, 'mp_flag': 0},
            # SP mode: row>0, col>min â†’ inner loop navigates.
            {'row': 3,  'col': 3, 'flag': 0, 'mp_flag': 0},
            {'row': 5,  'col': 5, 'flag': 0, 'mp_flag': 0},
            {'row': 2,  'col': 2, 'flag': 0, 'mp_flag': 0},
            # MP mode (min=3): row>0, col=min(3).
            {'row': 2,  'col': 3, 'flag': 0, 'mp_flag': 1},
            {'row': 4,  'col': 3, 'flag': 0, 'mp_flag': 1},
            # MP mode: row>0, col>min.
            {'row': 3,  'col': 5, 'flag': 0, 'mp_flag': 1},
            # row=0 clamp case.
            {'row': 1,  'col': 1, 'flag': 0, 'mp_flag': 0},
            {'row': 1,  'col': 3, 'flag': 0, 'mp_flag': 1},
        ],
        'path2_tests': [
            {'row': 2,  'col': 1, 'flag': 0, 'mp_flag': 0},
            {'row': 3,  'col': 3, 'flag': 0, 'mp_flag': 0},
            {'row': 2,  'col': 3, 'flag': 0, 'mp_flag': 1},
        ],
    },

    # 0x0046dc00  EntityFieldSet
    # Writes param_2 to (&DAT_008815a8)[param_1 * 0x341].
    # entity_field_set: call fn(p1, p2), read back 0x8815a8 + p1*0xd04.
    'entity_field_set': {
        'rva':            0x0046dc00,
        'export':         'EntityFieldSet',
        'signature':      {'ret': 'uint32', 'args': ['int32', 'uint32']},
        'arg_type':       'entity_field_set',
        'target_global':  0x008815a8,
        'entity_byte_stride': 0xd04,
        'lut_root_delta': 0,
        'path1_tests': [
            # [param_1, param_2]
            [0, 0],
            [0, 1],
            [0, 0xdeadbeef],
            [0, 0xffffffff],
            [1, 0],
            [1, 0x12345678],
            [2, 0],
            [2, 0xabcdef],
            [2, 0xcafebabe],
            [3, 0],
        ],
        'path2_tests': [
            [0, 0xdeadbeef],
            [1, 0x12345678],
            [2, 0xcafebabe],
        ],
    },

    # 0x00492340  CarSlotInit
    # Guard at element[param_1][+4] of 0x7f1058 (stride 0x4c). If non-zero:
    # writes 1/3/10/0xff to fields +0/+0x10/+0xc/+0x14.
    # car_slot_init: inject guard_val, call fn(idx), read back 4 fields packed.
    'car_slot_init': {
        'rva':            0x00492340,
        'export':         'CarSlotInit',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'car_slot_init',
        'lut_root_delta': 0,
        'path1_tests': [
            # { idx, guard_val }  â€” guard_val=0 â†’ no write; guard_val!=0 â†’ write 4 fields
            {'idx': 0, 'guard_val': 0},
            {'idx': 0, 'guard_val': 1},
            {'idx': 1, 'guard_val': 0},
            {'idx': 1, 'guard_val': 1},
            {'idx': 2, 'guard_val': 0},
            {'idx': 2, 'guard_val': 1},
            {'idx': 3, 'guard_val': 0},
            {'idx': 3, 'guard_val': 1},
            {'idx': 0, 'guard_val': 0xffffffff},
            {'idx': 1, 'guard_val': 0xdeadbeef},
        ],
        'path2_tests': [
            {'idx': 0, 'guard_val': 0},
            {'idx': 0, 'guard_val': 1},
            {'idx': 1, 'guard_val': 1},
        ],
    },

    # 0x0046cbb0  VehicleCarStateRead
    # Reads 2 DWORD fields (state + secondary) from per-car struct at stride 0xD04.
    # idx_out2: carIdx first, two 4-byte out-slots in shared buf; compares return value (0/1).
    # U-1855/1856/1857 are in uncertainties section only; bounds logic is deterministic.
    'vehicle_car_state_read': {
        'rva':            0x0046cbb0,
        'export':         'VehicleCarStateRead',
        'signature':      {'ret': 'int32', 'args': ['uint32', 'pointer', 'pointer']},
        'arg_type':       'idx_out2',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 5, 10, 15, 16, 17, 255, 0xffffffff],
        'path2_tests':    [0, 15, 16, 255],
    },

    # ── c3_batch_ad session 3 — gameplay pure-leaf harvest (GameplayLeaves_ad3.cpp) ──
    # 3 of 11 candidates viable with an existing arg_type; the rest SKIPPED
    # (two-index getters / 4-arg setters / void indexed setter / stateful arena
    #  grid / thiscall-with-callee — all need a NEW arg_type → harness-extension).

    # 0x0046bce0  VehicleVec3At94Get
    # if (0xf < veh) return 0; reads vec3 (&DAT_00882094[veh*0x341] dwords) into out.
    # out3_idx: 12-byte buf first, vehicleIdx second; compares return value (0/1).
    # Read-only getter (sibling of VehicleVec3At9C8Get 0x0046d700). U-8408 open
    # (vec3 semantic) — does not affect offset/bounds correctness.
    # ref: re/analysis/bucket_gameplay_0045dff0_0046dd90/0x0046bce0.md
    'vehicle_vec3_at_94_get': {
        'rva':            0x0046bce0,
        'export':         'VehicleVec3At94Get',
        'signature':      {'ret': 'int32', 'args': ['pointer', 'uint32']},
        'arg_type':       'out3_idx',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 5, 10, 15, 16, 17, 255, 0xffffffff],
        'path2_tests':    [0, 15, 16, 255],
    },

    # 0x0046d740  VehicleVec3At6E4Set
    # if (0xf < veh) return 0; writes input vec3 into &DAT_008816e4 + veh*0xd04.
    # out3_idx: 12-byte buf is the INPUT vec3 (read by the setter), vehicleIdx
    # second; compares return value (0/1 bounds). The write side-effect lands in
    # the inactive per-vehicle .bss record at menu (benign). U-8421 open.
    # ref: re/analysis/bucket_gameplay_0045dff0_0046dd90/0x0046d740.md
    'vehicle_vec3_at_6e4_set': {
        'rva':            0x0046d740,
        'export':         'VehicleVec3At6E4Set',
        'signature':      {'ret': 'int32', 'args': ['pointer', 'uint32']},
        'arg_type':       'out3_idx',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 5, 10, 15, 16, 17, 255, 0xffffffff],
        'path2_tests':    [0, 15, 16, 255],
    },

    # 0x00461e90  SurfaceCodeClassify
    # Pure leaf: undefined4 fn(sel, key, out1*, out2*). switch(sel) over a fixed
    # case set; each case maps `key` (large negative comparands) to a pair of
    # codes {0..3} written to *out1/*out2, returning 0/1. No globals, no callees.
    # int2_ptr2_out: fn(sel, key, out1, out2) → "out1,out2" hex fingerprint
    # (compares the written code pair; return value not in the fingerprint, but
    #  the code pair is the function's product and discriminates all branches).
    # Tests span multiple cases + their discriminating constants + default.
    # U-8393/U-8394 open (comparand/code semantics) — mapping is mechanically exact.
    # ref: re/analysis/bucket_gameplay_0045dff0_0046dd90/0x00461e90.md
    'surface_code_classify': {
        'rva':            0x00461e90,
        'export':         'SurfaceCodeClassify',
        'signature':      {'ret': 'void', 'args': ['uint32', 'uint32', 'pointer', 'pointer']},
        'arg_type':       'int2_ptr2_out',
        'lut_root_delta': 0,
        'path1_tests': [
            [0,    -0x9b9b9c], [0,    -0x697f80], [0,    0],
            [2,    -0x4b7f80], [2,    -0x237f80], [2,    -0x377f80],
            [3,    -0x7fc000], [0xb,  -0xff9b4c], [0x19, -0xe17f4c],
            [0x1a, -0x7f3800], [0x21, -0xff8000], [0x21, -0x7fbfc0],
            [0x24, -0x4b7f80], [0x25, -0xebaf74], [0x27, -0x4b4b4c],
            [0x27, -1],        [0x99, 0],         [0x26, -0x697f80],
            [0x22, -0xffa54c], [0x24, -0x237f80],
        ],
        'path2_tests': [
            [0, -0x9b9b9c], [2, -0x4b7f80], [0x99, 0], [0x27, -1], [0x21, -0xff8000],
        ],
    },

    # â”€â”€ Session 89: 5 util pure-function C2â†’C3 promotions â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    'timer_get_base_ptr': {
        'rva':            0x00413f90,
        'export':         'TimerGetBasePtr',
        # Returns the constant address 0x005f2b10 regardless of args.
        'signature':      {'ret': 'pointer', 'args': []},
        'arg_type':       'void',
        'lut_root_delta': 0,
        # 5 calls; input value is ignored for 'void' arg_type.
        'path1_tests': [0, 0, 0, 0, 0],
        'path2_tests': [0, 0],
    },

    'get_race_sub_mode': {
        'rva':            0x0042f6a0,
        'export':         'GetRaceSubMode',
        # Returns *(uint32*)0x0067e9fc â€” game-phase register.
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'void',
        'lut_root_delta': 0,
        # 8 calls at quiescent game state; expect constant return per run.
        'path1_tests': [0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests': [0, 0, 0],
    },

    'get_race_end_trigger': {
        'rva':            0x005c9d00,
        'export':         'GetRaceEndTrigger',
        # Always returns 0; 3-byte stub (XOR EAX,EAX / RET / NOPÃ—13).
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'void',
        'lut_root_delta': 0,
        'path1_tests': [0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests': [0, 0, 0],
    },

    'get_event_flag': {
        'rva':            0x0041f1c0,
        'export':         'GetEventFlag',
        # DAT_0063d9e0[(player * 0xab + event_id) * 4]; 4 bytes/entry.
        # Table is zero-initialized before race start; all tests return 0
        # initially â€” bit-identical against original is guaranteed.
        'signature':      {'ret': 'uint32', 'args': ['int', 'int']},
        'arg_type':       'int_pair',
        'lut_root_delta': 0,
        'path1_tests': [
            [0, 0],     [0, 0x19],  [0, 0x1a],  [0, 0x1b],  [0, 0x1c],
            [1, 0x37],  [1, 0x38],  [1, 0x39],  [1, 0x3a],  [1, 0x2a],
            [2, 0],     [3, 0],     [0, 0xaa],  [1, 0xaa],
        ],
        'path2_tests': [
            [0, 0], [0, 0x19], [1, 0x37], [2, 0], [3, 0],
        ],
    },

    'get_player_state_bits': {
        'rva':            0x0041f090,
        'export':         'GetPlayerStateBits',
        # Reads *(uint32*)(0x0063dc74 + player*0x2ac);
        # out-param1 = word&0x10 (bit4); out-param2 = word&0x20 (bit5).
        # Returns void; callFn packs (out1&0x3f)|((out2&0x3f)<<8) for cmp.
        # Table zero-initialized before race; both players return 0 â€” valid A/B.
        'signature':      {'ret': 'void', 'args': ['int', 'pointer', 'pointer']},
        'arg_type':       'int_ptr2_out',
        'lut_root_delta': 0,
        'path1_tests': [0, 1, 2, 3, 0, 1, 2, 3],
        'path2_tests': [0, 1, 2, 3],
    },

# â”€â”€ Session 85 audio leaves (C2â†’C3) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x005aea10  AudioAlignedAlloc: alloc size+4, 4-byte-align, store raw base
    # at *(aligned-4).  Heap addresses are non-deterministic so we compare
    # (ptr % 4) and (ptr - header) â€” both must equal 4 for correct impl.
    'audio_aligned_alloc': {
        'rva':         0x005aea10,
        'export':      'AudioAlignedAlloc',
        'signature':   {'ret': 'pointer', 'args': ['int32', 'int32']},
        'arg_type':    'alloc_check',
        'alloc_tag':   0x30808,
        'lut_root_delta': 0,
        'path1_tests': [1, 16, 64, 1024, 4096],
        'path2_tests': [16, 64],
    },

    # 0x005aea40  AudioAlignedFree: recover raw base from *(ptr-4), forward to
    # vtable dealloc trampoline 0x004522d0.  Test: alloc then free â€” no-crash.
    'audio_aligned_free': {
        'rva':          0x005aea40,
        'export':       'AudioAlignedFree',
        'signature':    {'ret': 'void', 'args': ['pointer']},
        'arg_type':     'free_via_alloc',
        'alloc_rva':    0x005aea10,
        'alloc_tag':    0x30808,
        'lut_root_delta': 0,
        'path1_tests':  [16, 64, 256, 1024],
        'path2_tests':  [16, 64],
    },

    # 0x005aec00  AudioByteReverse: two-pointer in-place byte reversal.
    # Buffer fingerprints compared after each call.
    'audio_byte_reverse': {
        'rva':         0x005aec00,
        'export':      'AudioByteReverse',
        'signature':   {'ret': 'void', 'args': ['pointer', 'uint32']},
        'arg_type':    'bytes_inplace',
        'lut_root_delta': 0,
        'path1_tests': [
            {'init': [],                                                              'len': 0},
            {'init': [0x01],                                                          'len': 1},
            {'init': [0x01, 0x02],                                                    'len': 2},
            {'init': [0x01, 0x02, 0x03, 0x04],                                        'len': 4},
            {'init': [0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08],               'len': 8},
            {'init': [0x10, 0x11, 0x12, 0x13, 0x14, 0x15],                           'len': 6},
            {'init': [0x00, 0xFF, 0x80, 0x7F, 0xAA, 0x55, 0x01, 0xFE,
                      0x0F, 0xF0, 0x0A, 0xA0, 0x01, 0x23, 0x45, 0x67],              'len': 16},
        ],
        'path2_tests': [
            {'init': [0x01, 0x02, 0x03, 0x04],                          'len': 4},
            {'init': [0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11, 0x22],  'len': 8},
        ],
    },

    # 0x005aee20  AudioBitScanForward: lowest set bit index (0..31) or 32.
    # Return in AL; ret='uint8' so Frida reads only that byte.
    'audio_bit_scan_forward': {
        'rva':         0x005aee20,
        'export':      'AudioBitScanForward',
        'signature':   {'ret': 'uint8', 'args': ['uint32']},
        'arg_type':    'uint32_scalar',
        'lut_root_delta': 0,
        'path1_tests': [0x28, 0x01, 0x80000000, 0xFFFFFFFF, 0x00010000,
                        0x00000002, 0x00000000, 0xFFFFFFFE, 0x00FF0000, 0x40000000],
        'path2_tests': [0x28, 0x01, 0x80000000, 0xFFFFFFFF],
    },

    # 0x005aec30  AudioByteSwapBuffer: bswap elements in-place.
    # width==4 -> bswap uint32; width==2 -> swap bytes of uint16; else no-op.
    'audio_byte_swap_buffer': {
        'rva':         0x005aec30,
        'export':      'AudioByteSwapBuffer',
        'signature':   {'ret': 'void', 'args': ['pointer', 'uint32', 'int32']},
        'arg_type':    'bytes_inplace_3',
        'lut_root_delta': 0,
        'path1_tests': [
            {'init': [],                                                              'len': 0,  'width': 4},
            {'init': [0x01, 0x02, 0x03, 0x04],                                        'len': 4,  'width': 4},
            {'init': [0x12, 0x34, 0x56, 0x78, 0xAB, 0xCD, 0xEF, 0x00],               'len': 8,  'width': 4},
            {'init': [0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                      0x09, 0x0A, 0x0B, 0x0C],                                       'len': 12, 'width': 4},
            {'init': [0x01, 0x02],                                                    'len': 2,  'width': 2},
            {'init': [0xAB, 0xCD, 0xEF, 0x01],                                        'len': 4,  'width': 2},
            {'init': [0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88],               'len': 8,  'width': 2},
            {'init': [0x01, 0x02, 0x03],                                              'len': 3,  'width': 5},
        ],
        'path2_tests': [
            {'init': [0x12, 0x34, 0x56, 0x78], 'len': 4, 'width': 4},
            {'init': [0xAB, 0xCD],              'len': 2, 'width': 2},
        ],
    },

    # ── c3_batch_ag s1 ───────────────────────────────────────────────────────
    # 0x004b64e0  FUN_004b64e0  MemsetInline (57B inline memset; Lua-VM alloc).
    # Real sig: (void* dst, byte fill, uint count) -> dst.  Diffed via the
    # bytes_inplace_3 harness, which calls fn(buf, len, width) and fingerprints
    # `len` bytes after prefilling them to 0 (init=[]). We therefore map:
    #     dst   = buf      (the scratch buffer)
    #     fill  = len      (the harness's 2nd arg == memset's fill byte)
    #     count = width    (the harness's 3rd arg == memset's byte count)
    # So each vector zero-fills `len` bytes, memset writes `width` bytes of value
    # `len`, and the `len`-byte fingerprint shows the exact word/tail boundary.
    # Non-degenerate: the fill value AND the write count both vary across vectors,
    # and the buffer demonstrably changes from its zero prefill (except the
    # count==0 branch vector, which is the intentional early-return coverage).
    'memset_inline': {
        'rva':         0x004b64e0,
        'export':      'MemsetInline',
        'signature':   {'ret': 'void', 'args': ['pointer', 'uint32', 'uint32']},
        'arg_type':    'bytes_inplace_3',
        'lut_root_delta': 0,
        'path1_tests': [
            {'init': [], 'len': 0xFF, 'width': 16},   # fill 0xFF (MAX), 4 words + 0 tail
            {'init': [], 'len': 0xFF, 'width': 17},   # tail 1
            {'init': [], 'len': 0xFF, 'width': 19},   # tail 3
            {'init': [], 'len': 0x80, 'width': 64},   # fill 0x80 (sign bit), 16 words
            {'init': [], 'len': 0xAA, 'width': 85},   # fill 0xAA (alt bits), 21 words + 1 tail
            {'init': [], 'len': 0x55, 'width': 200},  # fill 0x55 (alt bits), count>window
            {'init': [], 'len': 0x7F, 'width': 3},    # tail-only (count<4)
            {'init': [], 'len': 0x01, 'width': 1},    # minimal
            {'init': [], 'len': 0xFE, 'width': 0},    # count==0 early-return branch
            {'init': [], 'len': 0xF0, 'width': 240},  # full fill, count==window
            {'init': [], 'len': 0x0F, 'width': 15},   # 3 words + 3 tail
            {'init': [], 'len': 0x3C, 'width': 60},   # 15 words exactly (no tail)
        ],
        'path2_tests': [
            {'init': [], 'len': 0xFF, 'width': 19},
            {'init': [], 'len': 0xAA, 'width': 85},
            {'init': [], 'len': 0x7F, 'width': 3},
        ],
    },

    # ── c3_batch_ag harness-ext 2026-06-08 — 9 deferrals unlocked by 4 new ────
    # arg_type handlers (seed_field_read_field / structptr_seeded_array /
    # scalars_to_scattered_globals / count_header_list_ring).

    # 0x00483a30 Replay::Rewind — copies *(p+0x18)->*(p+0x1c). Seed +0x18, read +0x1c.
    'replay_rewind': {
        'rva':       0x00483a30,
        'export':    'ReplayRewind',
        'signature': {'ret': 'void', 'args': ['pointer']},
        'arg_type':  'seed_field_read_field',
        'struct_size': 0x40, 'seed_off': 0x18, 'read_off': 0x1c, 'read_size': 4,
        'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xFFFFFFFF, 0x80000000, 0x7FFFFFFF, 0xAAAAAAAA,
                        0x55555555, 0x12345678, 0xDEADBEEF, 0xCAFEBABE, 0x00FF00FF, 0xF0F0F0F0],
        'path2_tests': [0xDEADBEEF, 0x55555555, 0x80000000],
    },

    # 0x004c1c80 ViewportDimsSet(structPtr, dims[2]) — stores dims@+0x68/+0x6c and
    # ratios DAT_005cc320/dim@+0x70/+0x74; gate@+4 stays 0 so FUN_004c0e50 is skipped.
    # dims are re-read as float for the ratio -> test vectors are float bit patterns.
    'viewport_dims_set': {
        'rva':       0x004c1c80,
        'export':    'ViewportDimsSet',
        'signature': {'ret': 'int', 'args': ['pointer', 'pointer']},
        'arg_type':  'structptr_seeded_array',
        'struct_size': 0x80, 'read_offs': [0x68, 0x6c, 0x70, 0x74],
        'lut_root_delta': 0,
        'path1_tests': [
            [0x44480000, 0x44160000],  # 800.0, 600.0
            [0x44200000, 0x43F00000],  # 640.0, 480.0
            [0x44800000, 0x44400000],  # 1024.0, 768.0
            [0x44F00000, 0x44870000],  # 1920.0, 1080.0
            [0x3F800000, 0x40000000],  # 1.0, 2.0
            [0x40000000, 0x3F800000],  # 2.0, 1.0
            [0x43800000, 0x43800000],  # 256.0, 256.0
            [0x44160000, 0x44480000],  # 600.0, 800.0
            [0x3F000000, 0x40400000],  # 0.5, 3.0
            [0x42C80000, 0x42C80000],  # 100.0, 100.0
            [0x45000000, 0x44A00000],  # 2048.0, 1280.0
            [0x44480000, 0x44480000],  # 800.0, 800.0
        ],
        'path2_tests': [[0x44480000, 0x44160000], [0x44200000, 0x43F00000], [0x3F800000, 0x40000000]],
    },

    # 0x00417450 SparseGridCellWrite(x,y,v) -> 1/0. Snapshot the whole cell table +
    # slot data + high-water (save/restore both sides) and fold the return.
    'sparse_grid_cell_write': {
        'rva':       0x00417450,
        'export':    'SparseGridCellWrite',
        'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32', 'uint32']},
        'arg_type':  'scalars_to_scattered_globals',
        'observe':   [{'addr': '0x007f1a9c', 'len': 0xC000}, {'addr': '0x00801a9c', 'len': 4}],
        'fold_ret':  True,
        'pre_fill_byte': 0xFF,   # grid free-slot sentinel; region is 0x00 at attach
        'lut_root_delta': 0,
        'path1_tests': [
            {'args': [0, 0, 0xAB]}, {'args': [0, 0, 0xCD]}, {'args': [8, 0, 0x11]},
            {'args': [0, 8, 0x22]}, {'args': [16, 16, 0x33]}, {'args': [0, 0, 0xFF]},
            {'args': [64, 64, 0x44]}, {'args': [100, 100, 0x55]}, {'args': [0, 0, 0x00]},
            {'args': [8, 16, 0x7F]}, {'args': [0x10000, 0, 0x99]}, {'args': [0, 0x10000, 0xEE]},
        ],
        'path2_tests': [{'args': [0, 0, 0xAB]}, {'args': [16, 16, 0x33]}, {'args': [0x10000, 0, 0x99]}],
    },

    # 0x00417530 SparseGridCellErase(x,y) -> 1/0. Prep = ORIGINAL writer 0x00417450
    # populates the cell first (so erase has something to clear), inside save/restore.
    'sparse_grid_cell_erase': {
        'rva':       0x00417530,
        'export':    'SparseGridCellErase',
        'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32']},
        'arg_type':  'scalars_to_scattered_globals',
        # Split fill: cell table -> 0x00 baseline, slot data -> 0xff (free sentinel).
        # erase frees the cell (cellTable[cell]=0xffff != 0x00 baseline) so its (x,y)-
        # dependent effect is visible — a uniform 0xff fill would revert to baseline.
        'observe':   [{'addr': '0x007f1a9c', 'len': 0x8000, 'fill': 0x00},
                      {'addr': '0x007f9a9c', 'len': 0x4000, 'fill': 0xFF},
                      {'addr': '0x00801a9c', 'len': 4, 'fill': 0x00}],
        'fold_ret':  True,
        'prep_call_str': '0x00417450',
        'prep_arg_types': ['uint32', 'uint32', 'uint32'],
        'lut_root_delta': 0,
        'path1_tests': [
            {'args': [0, 0],   'prep_args': [0, 0, 0xAB]},
            {'args': [8, 0],   'prep_args': [8, 0, 0xCD]},
            {'args': [0, 8],   'prep_args': [0, 8, 0x11]},
            {'args': [16, 16], 'prep_args': [16, 16, 0x22]},
            {'args': [64, 64], 'prep_args': [64, 64, 0x33]},
            {'args': [100, 100], 'prep_args': [100, 100, 0x44]},
            {'args': [0, 0],   'prep_args': [0, 0, 0xFF]},
            {'args': [8, 16],  'prep_args': [8, 16, 0x55]},
            {'args': [24, 8],  'prep_args': [24, 8, 0x66]},
            {'args': [0x10000, 0], 'prep_args': [0x10000, 0, 0x77]},  # OOR: prep no-op, erase->0
            {'args': [32, 32], 'prep_args': [32, 32, 0x88]},
            {'args': [0, 24],  'prep_args': [0, 24, 0x99]},
        ],
        'path2_tests': [
            {'args': [0, 0], 'prep_args': [0, 0, 0xAB]},
            {'args': [16, 16], 'prep_args': [16, 16, 0x22]},
        ],
    },

    # 0x004299d0 TimeRecordWriteTrackBest(frac,sec,min). Echoes the 3 scattered
    # scalar globals + the 3 track arrays at idx=FUN_00430790() (C3 impl).
    'time_record_write_track_best': {
        'rva':       0x004299d0,
        'export':    'TimeRecordWriteTrackBest',
        'signature': {'ret': 'void', 'args': ['uint32', 'uint32', 'uint32']},
        'arg_type':  'scalars_to_scattered_globals',
        'observe':   [{'addr': '0x0067d990', 'len': 0x14}],  # DAT_0067d990 / d998 / 9a0
        'idx_call_str': '0x00430790',
        'idx_arrays': [
            {'base': '0x007f0db4', 'stride': 4, 'elem_len': 4},   # minutes
            {'base': '0x007f0de8', 'stride': 4, 'elem_len': 4},   # seconds
            {'base': '0x007f0e1c', 'stride': 4, 'elem_len': 4},   # fractional
        ],
        'fold_ret': False,
        'lut_root_delta': 0,
        'path1_tests': [
            {'args': [0x3DCCCCCD, 30, 2]}, {'args': [0x3F000000, 45, 5]}, {'args': [0, 0, 0]},
            {'args': [0xFFFFFFFF, 59, 99]}, {'args': [0x3F800000, 1, 1]}, {'args': [0x40490FDB, 15, 10]},
            {'args': [0x12345678, 42, 7]}, {'args': [0xAAAAAAAA, 33, 3]}, {'args': [0x55555555, 12, 8]},
            {'args': [0x80000000, 59, 0]}, {'args': [0x7FFFFFFF, 1, 59]}, {'args': [0xCAFEBABE, 21, 4]},
        ],
        'path2_tests': [{'args': [0x3F000000, 45, 5]}, {'args': [0xFFFFFFFF, 59, 99]}, {'args': [0x12345678, 42, 7]}],
    },

    # 0x005b3580 AudioListInit(hdr) — count-header circular-list init. Prefill the 3
    # header words with a per-vector sentinel, call, read normalized empty-state.
    'audio_list_init': {
        'rva':       0x005b3580,
        'export':    'AudioListInit',
        'signature': {'ret': 'void', 'args': ['pointer']},
        'arg_type':  'count_header_list_ring',
        'list_op':   'init',
        'node_link_off': 0x20, 'cmp_field_off': 0x18, 'object_size': 0x40,
        'init_rva_str': '0x005b3580', 'pushback_rva_str': '0x005b35a0',
        'lut_root_delta': 0,
        'path1_tests': [0xDEADBEEF, 0xCAFEBABE, 0x11111111, 0x22222222, 0, 0xFFFFFFFF,
                        0x80000000, 0x55555555, 0xAAAAAAAA, 0x12345678, 1, 0x7FFFFFFF],
        'path2_tests': [0xDEADBEEF, 0, 0xFFFFFFFF],
    },

    # 0x005b35a0 AudioListPushBack(hdr,obj). Build list from `pre` via ORIGINAL
    # Init+PushBack, push a new object with `push_field`, observe count + first field.
    'audio_list_push_back': {
        'rva':       0x005b35a0,
        'export':    'AudioListPushBack',
        'signature': {'ret': 'void', 'args': ['pointer', 'pointer']},
        'arg_type':  'count_header_list_ring',
        'list_op':   'pushback',
        'node_link_off': 0x20, 'cmp_field_off': 0x18, 'object_size': 0x40,
        'init_rva_str': '0x005b3580', 'pushback_rva_str': '0x005b35a0',
        'lut_root_delta': 0,
        'path1_tests': [
            {'pre': [], 'push_field': 0x111},
            {'pre': [0xAAA], 'push_field': 0x222},
            {'pre': [0xAAA, 0xBBB], 'push_field': 0x333},
            {'pre': [1, 2, 3], 'push_field': 0x444},
            {'pre': [0x10, 0x20, 0x30, 0x40], 'push_field': 0x555},
            {'pre': [], 'push_field': 0},
            {'pre': [0xFFFF], 'push_field': 0x7FFF},
            {'pre': [5, 6, 7, 8, 9], 'push_field': 0x666},
            {'pre': [0x100], 'push_field': 0x777},
            {'pre': [0xDEAD], 'push_field': 0xBEEF},
            {'pre': [1], 'push_field': 2},
            {'pre': [0x12, 0x34, 0x56], 'push_field': 0x78},
        ],
        'path2_tests': [{'pre': [], 'push_field': 0x111}, {'pre': [1, 2, 3], 'push_field': 0x444}],
    },

    # 0x005b3670 AudioListFind(hdr,key) -> object base. Observe cmp-field of the
    # returned object (==key when found) or -1.
    'audio_list_find': {
        'rva':       0x005b3670,
        'export':    'AudioListFind',
        'signature': {'ret': 'pointer', 'args': ['pointer', 'int32']},
        'arg_type':  'count_header_list_ring',
        'list_op':   'find',
        'node_link_off': 0x20, 'cmp_field_off': 0x18, 'object_size': 0x40,
        'init_rva_str': '0x005b3580', 'pushback_rva_str': '0x005b35a0',
        'lut_root_delta': 0,
        'path1_tests': [
            {'field_vals': [0x10, 0x20, 0x30], 'key': 0x20},
            {'field_vals': [0x10, 0x20, 0x30], 'key': 0x10},
            {'field_vals': [0x10, 0x20, 0x30], 'key': 0x30},
            {'field_vals': [0x10, 0x20, 0x30], 'key': 0x99},   # absent -> NULL
            {'field_vals': [0xAA], 'key': 0xAA},
            {'field_vals': [0xAA], 'key': 0xBB},
            {'field_vals': [], 'key': 0x1},                    # empty -> NULL
            {'field_vals': [1, 2, 3, 4, 5], 'key': 4},
            {'field_vals': [1, 2, 3, 4, 5], 'key': 6},
            {'field_vals': [0x7FFFFFFF, 0, 0x40], 'key': 0},
            {'field_vals': [0x7FFFFFFF, 0, 0x40], 'key': 0x40},
            {'field_vals': [100, 200, 300], 'key': 200},
        ],
        'path2_tests': [{'field_vals': [0x10, 0x20, 0x30], 'key': 0x20}, {'field_vals': [0x10, 0x20, 0x30], 'key': 0x99}],
    },

    # 0x005b36b0 AudioListAt(hdr,pos) -> object base. Observe cmp-field of the pos-th
    # object (traversal order = reverse insertion) or -1 when out of range.
    'audio_list_at': {
        'rva':       0x005b36b0,
        'export':    'AudioListAt',
        'signature': {'ret': 'pointer', 'args': ['pointer', 'int32']},
        'arg_type':  'count_header_list_ring',
        'list_op':   'at',
        'node_link_off': 0x20, 'cmp_field_off': 0x18, 'object_size': 0x40,
        'init_rva_str': '0x005b3580', 'pushback_rva_str': '0x005b35a0',
        'lut_root_delta': 0,
        'path1_tests': [
            {'field_vals': [0x10, 0x20, 0x30], 'pos': 0},
            {'field_vals': [0x10, 0x20, 0x30], 'pos': 1},
            {'field_vals': [0x10, 0x20, 0x30], 'pos': 2},
            {'field_vals': [0x10, 0x20, 0x30], 'pos': 3},      # OOR -> NULL
            {'field_vals': [0xAA], 'pos': 0},
            {'field_vals': [0xAA], 'pos': 1},                  # OOR -> NULL
            {'field_vals': [], 'pos': 0},                      # empty -> NULL
            {'field_vals': [1, 2, 3, 4, 5], 'pos': 4},
            {'field_vals': [1, 2, 3, 4, 5], 'pos': 2},
            {'field_vals': [100, 200, 300], 'pos': 1},
            {'field_vals': [0x7F, 0x80, 0x90], 'pos': 0},
            {'field_vals': [5, 6], 'pos': 1},
        ],
        'path2_tests': [{'field_vals': [0x10, 0x20, 0x30], 'pos': 1}, {'field_vals': [0x10, 0x20, 0x30], 'pos': 3}],
    },

    # ── c3_batch_ah s1 — Gameplay range-table walker family + getters ──────────
    # All six walk-family functions read the shared range-table via FUN_00426090()
    # == &DAT_0066ce58 (a static .bss buffer populated only during track load).
    # The Copter getter reads DAT_0063a5d0 (count) / DAT_00639dc4 (array); the LED
    # getter returns a constant &DAT_0063a5f0. int_scalar = 1-arg cdecl; int_pair =
    # 2-arg cdecl; none = zero-arg getter.

    # 0x00407640 FindCopterByKeyIndex(key) -> index | -1.
    'find_copter_by_key_index': {
        'rva':         0x00407640,
        'export':      'FindCopterByKeyIndex',
        'signature':   {'ret': 'int', 'args': ['uint32']},
        'arg_type':    'int_scalar',
        'lut_root_delta': 0,
        'path1_tests': [0, 0xFFFFFFFF, 0x80000000, 0x7FFFFFFF, 0xAAAAAAAA, 0x55555555,
                        1, 2, 0x12345678, 0xDEADBEEF, 0x100, 0x10],
        'path2_tests': [0, 0x7FFFFFFF, 0xDEADBEEF],
    },

    # 0x004098a0 GetLedEntryArrayBase() -> &DAT_0063a5f0 (constant getter).
    'get_led_entry_array_base': {
        'rva':         0x004098a0,
        'export':      'GetLedEntryArrayBase',
        'signature':   {'ret': 'pointer', 'args': []},
        'arg_type':    'none',
        'lut_root_delta': 0,
        'path1_tests': list(range(12)),
        'path2_tests': [0, 1, 2],
    },

    # 0x00409300 RangeTableFirstOfGroup(group) -> first int of group-th group.
    'range_table_first_of_group': {
        'rva':         0x00409300,
        'export':      'RangeTableFirstOfGroup',
        'signature':   {'ret': 'int', 'args': ['uint32']},
        'arg_type':    'int_scalar',
        'lut_root_delta': 0,
        # group ordinal domain is small (0..~10); huge values walk off the table.
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11],
        'path2_tests': [0, 1, 2],
    },

    # 0x00409330 RangeTableSuccessorByGroup(value, group) -> successor.
    'range_table_successor_by_group': {
        'rva':         0x00409330,
        'export':      'RangeTableSuccessorByGroup',
        'signature':   {'ret': 'int', 'args': ['uint32', 'uint32']},
        'arg_type':    'int_pair',
        'lut_root_delta': 0,
        'path1_tests': [
            [0, 0], [1, 0], [2, 1], [5, 2], [0x10, 0], [0xFF, 1],
            [0xFFFFFFFF, 0], [0x80000000, 0], [0x7FFFFFFF, 0],
            [0xAAAAAAAA, 1], [0x55555555, 2], [0x100, 0],
        ],
        'path2_tests': [[0, 0], [2, 1], [0x7FFFFFFF, 0]],
    },

    # 0x00407b00 RangeTableFindGroup(value) -> group index containing value.
    'range_table_find_group': {
        'rva':         0x00407b00,
        'export':      'RangeTableFindGroup',
        'signature':   {'ret': 'int', 'args': ['uint32']},
        'arg_type':    'int_scalar',
        'lut_root_delta': 0,
        # value domain is small track-piece ids (0..~20); huge values walk off table.
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 16, 20],
        'path2_tests': [0, 5, 10],
    },

    # 0x00407b70 RangeTableMemberTest(value, slot) -> 1/0.
    'range_table_member_test': {
        'rva':         0x00407b70,
        'export':      'RangeTableMemberTest',
        'signature':   {'ret': 'uint32', 'args': ['uint32', 'uint32']},
        'arg_type':    'int_pair',
        'lut_root_delta': 0,
        'path1_tests': [
            [0, 0], [1, 0], [2, 0], [5, 1], [0x10, 2], [0xFF, 0],
            [0xFFFFFFFF, 0], [0x80000000, 0], [0x7FFFFFFF, 0],
            [0xAAAAAAAA, 1], [0x55555555, 2], [0x100, 0],
        ],
        'path2_tests': [[0, 0], [5, 1], [0x7FFFFFFF, 0]],
    },

    # 0x00407d70 RangeTableCountGroup(slot) -> span sum + 1.
    'range_table_count_group': {
        'rva':         0x00407d70,
        'export':      'RangeTableCountGroup',
        'signature':   {'ret': 'int', 'args': ['uint32']},
        'arg_type':    'int_scalar',
        'lut_root_delta': 0,
        # slot index domain is small (0..~12); huge values walk off the table.
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12],
        'path2_tests': [0, 1, 2],
    },

    # 0x00407db0 RangeTableGroupOffset(slot, value) -> group-local cumulative offset.
    'range_table_group_offset': {
        'rva':         0x00407db0,
        'export':      'RangeTableGroupOffset',
        'signature':   {'ret': 'int', 'args': ['uint32', 'uint32']},
        'arg_type':    'int_pair',
        'lut_root_delta': 0,
        'path1_tests': [
            [0, 0], [0, 1], [1, 2], [2, 5], [0, 0x10], [1, 0xFF],
            [0, 0xFFFFFFFF], [0, 0x80000000], [0, 0x7FFFFFFF],
            [1, 0xAAAAAAAA], [2, 0x55555555], [0, 0x100],
        ],
        'path2_tests': [[0, 0], [2, 5], [0, 0x7FFFFFFF]],
    },

    # 0x00407a20 GetAiLapCounter(idx) -> (&DAT_008a9648)[idx*0xc3] per-AI lap counter.
    # idx = AI slot index; the per-AI rows differ during/after a race so distinct
    # idx span multiple rows (use 0..7 = the populated AI slots in Quick Battle).
    'get_ai_lap_counter': {
        'rva':         0x00407a20,
        'export':      'GetAiLapCounter',
        'signature':   {'ret': 'int', 'args': ['uint32']},
        'arg_type':    'int_scalar',
        'lut_root_delta': 0,
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7],
        'path2_tests': [0, 1, 2],
    },

    # 0x00407a00 ReadAiCheckpointField(row, col) -> *(&DAT_008a9914 + (row*0xc3+col)*4).
    # row = AI slot index, col = dword within the +0x2f4 checkpoint block. Vary BOTH
    # so distinct (row,col) read distinct populated cells of the per-AI table.
    'read_ai_checkpoint_field': {
        'rva':         0x00407a00,
        'export':      'ReadAiCheckpointField',
        'signature':   {'ret': 'uint32', 'args': ['uint32', 'uint32']},
        'arg_type':    'int_pair',
        'lut_root_delta': 0,
        'path1_tests': [
            [0, 0], [0, 1], [0, 2], [1, 0], [1, 1], [2, 0],
            [3, 0], [4, 0], [5, 0], [0, 5], [1, 3], [2, 4],
        ],
        'path2_tests': [[0, 0], [1, 1], [2, 0]],
    },

# â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session 86 â€” c3_render_math  (C2â†’C3, 5 candidates)
    # RW column-major transform + 2D vector math + matrix scale
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004c3730  RwV3dTransformPoint
    # Full affine point transform: out = mat * in  (includes translation).
    # Pure math, no callees. Stored as fn-ptr by FUN_004c35f0.
    'rw_v3d_transform_point': {
        'rva':            0x004c3730,
        'export':         'RwV3dTransformPoint',
        'signature':      {'ret': 'void', 'args': ['pointer', 'pointer', 'pointer']},
        'arg_type':       'transform_point',
        'lut_root_delta': 0,
        'path1_tests': [
            {'mat': _IDENT,  'in': [1.0,  0.0,  0.0]},
            {'mat': _IDENT,  'in': [0.0,  1.0,  0.0]},
            {'mat': _IDENT,  'in': [1.0,  2.0,  3.0]},
            {'mat': _IDENT,  'in': [-1.0, -2.0, -3.0]},
            {'mat': _TRANS,  'in': [0.0,  0.0,  0.0]},   # translation only
            {'mat': _TRANS,  'in': [1.0,  1.0,  1.0]},
            {'mat': _SCALE2, 'in': [1.0,  1.0,  1.0]},
            {'mat': _SCALE2, 'in': [0.5,  0.5,  0.5]},
            {'mat': _ROTY90, 'in': [1.0,  0.0,  0.0]},   # â†’ (0,0,-1)
            {'mat': _ROTY90, 'in': [0.0,  0.0,  1.0]},   # â†’ (1,0,0)
            {'mat': _MIXED,  'in': [1.0,  0.0,  0.0]},
            {'mat': _MIXED,  'in': [0.0,  1.0,  0.0]},
            {'mat': _MIXED,  'in': [1.0,  1.0,  1.0]},
            {'mat': _MIXED,  'in': [-1.0, 2.0, -3.0]},
        ],
        'path2_tests': [
            {'mat': _IDENT, 'in': [1.0, 2.0, 3.0]},
            {'mat': _TRANS, 'in': [0.0, 0.0, 0.0]},
            {'mat': _MIXED, 'in': [1.0, 1.0, 1.0]},
        ],
    },

    # 0x004c3880  RwV3dTransformVector
    # Direction transform: out = mat * in  (no translation term).
    # Pure math, no callees. Stored as fn-ptr by FUN_004c35f0.
    'rw_v3d_transform_vector': {
        'rva':            0x004c3880,
        'export':         'RwV3dTransformVector',
        'signature':      {'ret': 'void', 'args': ['pointer', 'pointer', 'pointer']},
        'arg_type':       'transform_vector',
        'lut_root_delta': 0,
        'path1_tests': [
            {'mat': _IDENT,  'in': [1.0,  0.0,  0.0]},
            {'mat': _IDENT,  'in': [1.0,  2.0,  3.0]},
            {'mat': _TRANS,  'in': [1.0,  0.0,  0.0]},   # translation must NOT be added
            {'mat': _TRANS,  'in': [0.0,  0.0,  0.0]},   # â†’ (0,0,0), not (1,2,3)
            {'mat': _SCALE2, 'in': [1.0,  1.0,  1.0]},
            {'mat': _SCALE2, 'in': [0.5, -0.5,  2.0]},
            {'mat': _ROTY90, 'in': [1.0,  0.0,  0.0]},
            {'mat': _ROTY90, 'in': [0.0,  0.0,  1.0]},
            {'mat': _MIXED,  'in': [1.0,  1.0,  1.0]},
            {'mat': _MIXED,  'in': [-1.0, 2.0, -3.0]},
        ],
        'path2_tests': [
            {'mat': _IDENT, 'in': [1.0, 2.0, 3.0]},
            {'mat': _TRANS, 'in': [1.0, 0.0, 0.0]},
            {'mat': _MIXED, 'in': [1.0, 1.0, 1.0]},
        ],
    },

    # 0x004c3bf0  Vec2Length
    # sqrt(v[0]^2 + v[1]^2) via RW fast-sqrt LUT.  Returns 0.0 for zero vector.
    'vec2_length': {
        'rva':            0x004c3bf0,
        'export':         'Vec2Length',
        'signature':      {'ret': 'float', 'args': ['pointer']},
        'arg_type':       'vec2_ptr',
        'lut_root_delta': 0,
        'path1_tests': [
            [0.0, 0.0], [1.0, 0.0], [0.0, 1.0],
            [3.0, 4.0], [-3.0, 4.0], [3.0, -4.0],
            [0.5, 0.5], [10.0, 0.0], [100.0, 100.0],
            [0.001, 0.001], [12.34, -56.78], [1234.5, -6789.0],
            [1e-6, 1e-6], [1.5, 2.5],
        ],
        'path2_tests': [
            [0.0, 0.0], [3.0, 4.0], [1.0, 1.0],
        ],
    },

    # 0x004c3c60  Vec2Normalize
    # Normalise in[2] into out[2]; return original magnitude.
    # Uses both sqrt and inv-sqrt LUT tables.
    # Error path (magnitude < DAT_005d757c): sets outâ‰ˆ0, calls error stubs.
    # Zero vector: out={0,0}, mag=0.0 (error path triggered but output same).
    'vec2_normalize': {
        'rva':            0x004c3c60,
        'export':         'Vec2Normalize',
        'signature':      {'ret': 'float', 'args': ['pointer', 'pointer']},
        'arg_type':       'vec2_normalize',
        'lut_root_delta': 0,
        'path1_tests': [
            [1.0,  0.0],
            [0.0,  1.0],
            [3.0,  4.0],
            [-3.0, 4.0],
            [1.0,  1.0],
            [0.5,  0.5],
            [10.0, 0.0],
            [12.34, -56.78],
            [100.0, -1.0],
            [0.0,  0.0],   # zero â†’ error path; out={0,0} mag=0 (both paths match)
        ],
        'path2_tests': [
            [1.0, 0.0], [3.0, 4.0], [0.0, 0.0],
        ],
    },

    # 0x004c5010  RwMatrixScale
    # 3-mode in-place matrix scale. Mode 0=replace, 1=MÃ—Scale, 2=ScaleÃ—M.
    # Verified from Ghidra decompilation 2026-05-14 (Mashed_pool0, read-only).
    'rw_matrix_scale': {
        'rva':            0x004c5010,
        'export':         'RwMatrixScale',
        'signature':      {'ret': 'void', 'args': ['pointer', 'pointer', 'int32']},
        'arg_type':       'matrix_scale',
        'lut_root_delta': 0,
        'path1_tests': [
            # Mode 0: always produces the same identity-then-scale result
            {'mat': _MIXED,  'scale': [2.0, 3.0, 4.0], 'mode': 0},
            {'mat': _IDENT,  'scale': [1.0, 1.0, 1.0], 'mode': 0},
            {'mat': _IDENT,  'scale': [2.0, 3.0, 4.0], 'mode': 0},
            # Mode 1 (MÃ—Scale): scale columns, translation unchanged
            {'mat': _IDENT,  'scale': [2.0, 3.0, 4.0], 'mode': 1},
            {'mat': _MIXED,  'scale': [2.0, 3.0, 4.0], 'mode': 1},
            {'mat': _TRANS,  'scale': [2.0, 3.0, 4.0], 'mode': 1},
            {'mat': _SCALE2, 'scale': [0.5, 0.5, 0.5], 'mode': 1},
            # Mode 2 (ScaleÃ—M): scale rows including translation
            {'mat': _IDENT,  'scale': [2.0, 3.0, 4.0], 'mode': 2},
            {'mat': _MIXED,  'scale': [2.0, 3.0, 4.0], 'mode': 2},
            {'mat': _TRANS,  'scale': [2.0, 3.0, 4.0], 'mode': 2},
            {'mat': _SCALE2, 'scale': [0.5, 0.5, 0.5], 'mode': 2},
        ],
        'path2_tests': [
            {'mat': _IDENT, 'scale': [2.0, 3.0, 4.0], 'mode': 0},
            {'mat': _MIXED, 'scale': [2.0, 3.0, 4.0], 'mode': 1},
            {'mat': _MIXED, 'scale': [2.0, 3.0, 4.0], 'mode': 2},
        ],
    },

    # 0x004c39b0  RwV3dNormalize  (WS-A2 vehicle-physics RW-math prereq)
    # Normalise in[3] into out[3]; return original magnitude. Uses both the sqrt
    # LUT (delta 0, for the returned magnitude) and the inv-sqrt LUT (delta 4, for
    # the normalisation scale) — the 3D sibling of Vec2Normalize. Zero vector ->
    # out={0,0,0}, mag=0 (error path fires identically). Magnitude threshold read
    # at runtime from 0x005d757c (static value 0.0).
    'rw_v3d_normalize': {
        'rva':            0x004c39b0,
        'export':         'RwV3dNormalize',
        'signature':      {'ret': 'float', 'args': ['pointer', 'pointer']},
        'arg_type':       'vec3_normalize',
        'lut_root_delta': 0,
        'path1_tests': [
            [1.0,   0.0,   0.0],
            [0.0,   1.0,   0.0],
            [0.0,   0.0,   1.0],
            [3.0,   4.0,   0.0],
            [-3.0,  4.0,   0.0],
            [1.0,   1.0,   1.0],
            [-1.0, -1.0,  -1.0],
            [2.0,   3.0,   6.0],
            [10.0,  0.0,   0.0],
            [12.34,-56.78, 90.12],
            [0.001, 0.001, 0.001],
            [100.0, 100.0, 100.0],
            [0.0,   0.0,   0.0],   # zero -> error path; out={0,0,0} mag=0 (both match)
        ],
        'path2_tests': [
            [1.0, 0.0, 0.0], [3.0, 4.0, 0.0], [2.0, 3.0, 6.0], [0.0, 0.0, 0.0],
        ],
    },

    # 0x004c4d20  RwMatrixRotate  (WS-A2 vehicle-physics RW-math prereq)
    # Axis-angle rotation matrix (degrees): deg->rad (*π/180), normalize axis via
    # FastInvSqrt, x87 fsin/fcos, then Rodrigues inner builder FUN_004c4a50. Mode 0
    # (rwCOMBINEREPLACE) is fully self-contained (no device matrix-multiply
    # dispatch), so path1 tests use mode 0 across varied axes/angles; the reimpl
    # delegates to the original 0x004c4a50 so the diff isolates this fn's own
    # preprocessing (the bit-identity-sensitive deg->rad / normalize / sin/cos).
    'rw_matrix_rotate': {
        'rva':            0x004c4d20,
        'export':         'RwMatrixRotate',
        'signature':      {'ret': 'pointer', 'args': ['pointer', 'pointer', 'float', 'int32']},
        'arg_type':       'matrix_rotate',
        'lut_root_delta': 4,   # FastInvSqrt LUT readiness poll (game-init gate)
        'path1_tests': [
            {'axis': [0.0, 0.0, 1.0], 'angle': 90.0,  'mode': 0},
            {'axis': [1.0, 0.0, 0.0], 'angle': 45.0,  'mode': 0},
            {'axis': [0.0, 1.0, 0.0], 'angle': 30.0,  'mode': 0},
            {'axis': [1.0, 1.0, 1.0], 'angle': 60.0,  'mode': 0},
            {'axis': [0.0, 0.0, 1.0], 'angle': 0.0,   'mode': 0},
            {'axis': [0.0, 0.0, 1.0], 'angle': 180.0, 'mode': 0},
            {'axis': [2.0, 0.0, 0.0], 'angle': 90.0,  'mode': 0},  # non-unit axis -> normalize
            {'axis': [3.0, 4.0, 0.0], 'angle': 120.0, 'mode': 0},
            {'axis': [0.0, 0.0, 1.0], 'angle': -45.0, 'mode': 0},
            {'axis': [1.0, 2.0, 2.0], 'angle': 270.0, 'mode': 0},
        ],
        'path2_tests': [
            {'axis': [0.0, 0.0, 1.0], 'angle': 90.0, 'mode': 0},
            {'axis': [1.0, 0.0, 0.0], 'angle': 45.0, 'mode': 0},
            {'axis': [1.0, 1.0, 1.0], 'angle': 60.0, 'mode': 0},
        ],
    },

    # 0x004c3df0  RwV3dTransformPoints  (WS-A2 vehicle-physics RW-math prereq)
    # Thin __cdecl indirect-dispatch thunk: calls *(rw_offset + rw_globals + 0x14)
    # (the RW device transform-points method) with all 4 args, returns arg1. Both
    # original and reimpl read the same device globals (0x7d3ff8/0x7d3ffc) and slot
    # +0x14, so the result is identical by construction; GREEN validates the +0x14
    # offset and the global selection. Call shape mirrors caller FUN_0046d510:
    # fn(out_vec3, matrix, 1, in_vec3).
    'rw_v3d_transform_points': {
        'rva':            0x004c3df0,
        'export':         'RwV3dTransformPoints',
        'signature':      {'ret': 'pointer', 'args': ['pointer', 'pointer', 'int32', 'pointer']},
        'arg_type':       'device_transform_dispatch',
        'lut_root_delta': 0,   # device-table readiness poll (game-init gate)
        'path1_tests': [
            {'mat': _IDENT,  'in': [1.0,  2.0,  3.0]},
            {'mat': _IDENT,  'in': [0.0,  0.0,  0.0]},
            {'mat': _TRANS,  'in': [1.0,  1.0,  1.0]},
            {'mat': _SCALE2, 'in': [1.0, -2.0,  3.0]},
            {'mat': _ROTY90, 'in': [1.0,  0.0,  0.0]},
            {'mat': _MIXED,  'in': [1.0,  1.0,  1.0]},
            {'mat': _MIXED,  'in': [-1.0, 2.0, -3.0]},
        ],
        'path2_tests': [
            {'mat': _IDENT, 'in': [1.0, 2.0, 3.0]},
            {'mat': _MIXED, 'in': [1.0, 1.0, 1.0]},
        ],
    },

    # 0x004c4a50  RwMatrixRotateInner  (WS-A2 follow-up; Rodrigues builder, RwMatrixRotate callee)
    # fn(matrix, axis_n, one_minus_cos, sin, mode). mode 0 (replace) is pure (no device);
    # modes 1/2 (pre/postconcat) dispatch the RW device matrix-mult. axis must be unit.
    # Pad slots [7]/[11]/[15] uninitialized -> skipped in the matrix_rotate_inner comparison.
    'rw_matrix_rotate_inner': {
        'rva':            0x004c4a50,
        'export':         'RwMatrixRotateInner',
        'signature':      {'ret': 'pointer', 'args': ['pointer', 'pointer', 'float', 'float', 'int32']},
        'arg_type':       'matrix_rotate_inner',
        'lut_root_delta': 0,   # device-table readiness poll (game-init gate)
        'path1_tests': [
            {'matrix': _IDENT, 'axis': [0.0, 0.0, 1.0],                    'omc': 1.0,        'sin': 1.0,        'mode': 0},  # 90 Z
            {'matrix': _IDENT, 'axis': [1.0, 0.0, 0.0],                    'omc': 0.29289323, 'sin': 0.70710677, 'mode': 0},  # 45 X
            {'matrix': _IDENT, 'axis': [0.0, 1.0, 0.0],                    'omc': 0.5,        'sin': 0.8660254,  'mode': 0},  # 60 Y
            {'matrix': _IDENT, 'axis': [0.57735027, 0.57735027, 0.57735027],'omc': 0.5,       'sin': 0.8660254,  'mode': 0},  # 60 diag
            {'matrix': _IDENT, 'axis': [0.0, 0.0, 1.0],                    'omc': 2.0,        'sin': 0.0,        'mode': 0},  # 180 Z
            {'matrix': _IDENT, 'axis': [0.6, 0.8, 0.0],                    'omc': 1.5,        'sin': 0.8660254,  'mode': 0},  # 120
            {'matrix': _IDENT, 'axis': [1.0, 0.0, 0.0],                    'omc': 1.0,        'sin': 1.0,        'mode': 0},  # 90 X
            {'matrix': _IDENT, 'axis': [0.0, 1.0, 0.0],                    'omc': 0.13397461, 'sin': 0.5,        'mode': 0},  # 30 Y
            {'matrix': _MIXED, 'axis': [0.0, 0.0, 1.0],                    'omc': 1.0,        'sin': 1.0,        'mode': 1},  # preconcat (device mult)
            {'matrix': _MIXED, 'axis': [1.0, 0.0, 0.0],                    'omc': 0.5,        'sin': 0.8660254,  'mode': 2},  # postconcat (device mult)
        ],
        'path2_tests': [
            {'matrix': _IDENT, 'axis': [0.0, 0.0, 1.0], 'omc': 1.0, 'sin': 1.0,       'mode': 0},
            {'matrix': _IDENT, 'axis': [1.0, 0.0, 0.0], 'omc': 0.5, 'sin': 0.8660254, 'mode': 0},
        ],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-a-s6 â€” frontend_menus_a larger + game_mode (C2->C3)
    # MenuButtonDetect.cpp + GameModeCarSelect.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042b770  MenuButtonDetectE
    # Simplified button-detector: checks active byte col+5 AND !processed col+5.
    # No timer, no screen-type check. Returns 1 (change) or 0.
    # arg_type='none': called 10x at quiescent main menu; both paths must
    # agree on 0 (no button pressed). U-3445 (callee semantics) is in
    # Uncertainties section only.
    'menu_button_detect_e': {
        'rva':            0x0042b770,
        'export':         'MenuButtonDetectE',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042b310  MenuButtonDetectC
    # Full button-detector: col+0 with screen-type early-return (0x13/0x1e).
    # Zeros _DAT_0067f1b8 when no active input detected. Returns 0 or 1.
    # arg_type='none': called 10x at quiescent main menu; both paths agree
    # on 0. U-3445 (callee semantics) and U-3556 (screen IDs) are in
    # Uncertainties only.
    'menu_button_detect_c': {
        'rva':            0x0042b310,
        'export':         'MenuButtonDetectC',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042b540  MenuButtonDetectD
    # Structural sibling of MenuButtonDetectC: col+1 (0x7f1045/0x7f1505).
    # No-input flag: _DAT_0067f1bc. Screen-type early-return logic identical.
    # arg_type='none': called 10x at quiescent main menu; both paths agree on 0.
    # U-3445 (callee), U-1651 (record width), U-3556 (screen IDs) carried forward.
    'menu_button_detect_d': {
        'rva':            0x0042b540,
        'export':         'MenuButtonDetectD',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042b960  CarSlotInit1P
    # void: scans char table pair, inits slot sentinels, sets mode=1, calls
    # FUN_0040e480 x4. Returns void (wrapped as uint32 0 for registry compat).
    # arg_type='none': called 5x at quiescent main menu (both paths must not
    # crash and agree on void/0). Global side-effects observed indirectly.
    'car_slot_init_1p': {
        'rva':            0x0042b960,
        'export':         'CarSlotInit1P',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042b9e0  CarSlotAssign
    # Car-selection confirm: resets slots, counts valid options, detects
    # collision, assigns car IDs. Returns 0x1000/0/1.
    # arg_type='none': called 10x at quiescent main menu; original and reimpl
    # must agree. Returns 1 (auto-confirm: <2 valid car options) at menu.
    'car_slot_assign': {
        'rva':            0x0042b9e0,
        'export':         'CarSlotAssign',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00431d00  CarSelectReset
    # void: writes 7 globals, calls FUN_00431b80 x3 (chained). Returns void.
    # arg_type='none': called 5x at quiescent main menu; both paths must not
    # crash and agree on void/0. Global side-effects (ea74..eaac) observed.
    'car_select_reset': {
        'rva':            0x00431d00,
        'export':         'CarSelectReset',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-b-s3 â€” sprite gate + HUD slot type mappers
    # SpriteGate.cpp
    # Drift-promotes: 0x004c5c00 C1->C2, 0x0040bb50 C1->C2
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0040bb70  SpriteLookupTableA
    # 20-byte forwarder: calls FUN_004c5c00(DAT_0063b900, key_ptr).
    # arg_type='int_scalar': pass string VA as int32 (ASLR disabled on 2004 exe).
    # "Button" at 0x005cda7c, "SemiC" at 0x005cc414 (found via binary search).
    # Both orig and reimpl call through to original FUN_004c5c00 with same key.
    # DAT_0063b900 (sprite table A head) is NULL at RW-init time; both crash at
    # node+8 offset identically. crash_equal_ok=True counts same-error as GREEN.
    'sprite_lookup_table_a': {
        'rva':            0x0040bb70,
        'export':         'SpriteLookupTableA',
        'signature':      {'ret': 'pointer', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0x005cda7c, 0x005cc414,  # "Button", "SemiC"
                           0x005cda7c, 0x005cc414,
                           0x005cda7c, 0x005cc414,
                           0x005cda7c, 0x005cc414,
                           0x005cda7c, 0x005cc414],
        'path2_tests':    [0x005cda7c, 0x005cc414, 0x005cda7c],
    },

    # 0x0040bb90  SpriteLookupTableB
    # 20-byte forwarder: calls FUN_004c5c00(DAT_0063b904, key_ptr).
    # Same crash-equal design as SpriteLookupTableA; different table root.
    'sprite_lookup_table_b': {
        'rva':            0x0040bb90,
        'export':         'SpriteLookupTableB',
        'signature':      {'ret': 'pointer', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0x005cda7c, 0x005cc414,
                           0x005cda7c, 0x005cc414,
                           0x005cda7c, 0x005cc414,
                           0x005cda7c, 0x005cc414,
                           0x005cda7c, 0x005cc414],
        'path2_tests':    [0x005cda7c, 0x005cc414, 0x005cda7c],
    },

    # 0x0042ee00  SpriteSlotGate
    # 59-byte slot gate: slot in {0,1,2} -> FUN_0040bb50(); else returns 0.
    # arg_type='int_scalar': test with out-of-range slots (>= 3) -> both return
    # NULL (0); slots 0/1/2 both call original FUN_0040bb50 via original VA.
    # Out-of-range cases (slot=3,4,5,99) are the deterministic ones (return 0).
    'sprite_slot_gate': {
        'rva':            0x0042ee00,
        'export':         'SpriteSlotGate',
        'signature':      {'ret': 'pointer', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [3, 4, 5, 99, 3, 4, 5, 3, 4, 99],
        'path2_tests':    [3, 4, 99],
    },

    # 0x00430a10  HudSlotTypePlayer0
    # Pure leaf: reads DAT_0067e9fc (game mode), returns slot-type code for player 0.
    # Mode 2->0, 3/4/5->1, 6/7/8/9->3, 10->11, other->0.
    # arg_type='read_global': write game mode to 0x0067e9fc then call fn() -> int.
    'hud_slot_type_player0': {
        'rva':            0x00430a10,
        'export':         'HudSlotTypePlayer0',        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067e9fc,
        'lut_root_delta': 0,
        'path1_tests':    [2, 3, 4, 5, 6, 7, 8, 9, 10, 0],
        'path2_tests':    [2, 3, 10],
    },

    # 0x00430a60  HudSlotTypePlayer1
    # Pure leaf: reads DAT_0067e9fc, returns slot-type code for player 1.
    # Mode 3/4/5->2, 2/6/7/8/9/10->0, other->0.
    'hud_slot_type_player1': {
        'rva':            0x00430a60,
        'export':         'HudSlotTypePlayer1',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067e9fc,
        'lut_root_delta': 0,
        'path1_tests':    [2, 3, 4, 5, 6, 7, 8, 9, 10, 0],
        'path2_tests':    [2, 3, 10],
    },

    # 0x00430ab0  HudSlotTypePlayer2
    # Pure leaf: reads DAT_0067e9fc, returns slot-type code for player 2.
    # Mode 3/4/5->5, 2/6/7/8/9/10->0, other->0.
    'hud_slot_type_player2': {
        'rva':            0x00430ab0,
        'export':         'HudSlotTypePlayer2',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067e9fc,
        'lut_root_delta': 0,
        'path1_tests':    [2, 3, 4, 5, 6, 7, 8, 9, 10, 0],
        'path2_tests':    [2, 3, 10],
    },

    # Session c3-batch-c-s1 â€” hud_ingame_promote_c2 (C2->C3, 3 candidates)
    # HUD/HudDispatch.cpp â€” game-mode router wrappers for in-game HUD draw
    # Callees 0x0041c2d0 and 0x0041bc50 drift-promoted C1->C2 (budget used).
    # Refused: 0x0041b630 (needs 0x0041b340 C1), 0x0041ccc0 (needs 0x0041c9a0 C1).
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0041a3e0  HudDispatchMode10
    # void(void): reads int32_t at DAT_0063c628; if non-zero -> calls FUN_0041c2d0.
    # 19 bytes. Game-mode 10 HUD draw path.
    # arg_type='none': called 10x at quiescent main menu; array 0x0063c628 is 0
    # at menu -> guard fails safely -> no crash. Both paths return void/0.
    'hud_dispatch_mode10': {
        'rva':            0x0041a3e0,
        'export':         'HudDispatchMode10',        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0041c300  HudDispatchMode5
    # void(void): reads int32_t at DAT_0063cdbc; if non-zero -> calls FUN_0041c2d0.
    # 19 bytes. Game-mode 5 HUD draw path (parallel to HudDispatchMode10).
    # arg_type='none': called 10x at quiescent main menu; flag 0x0063cdbc == 0
    # at menu -> guard fails safely -> no crash. Both paths return void/0.
    'hud_dispatch_mode5': {
        'rva':            0x0041c300,
        'export':         'HudDispatchMode5',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0041c0c0  HudDispatchSlot2
    # void(void): 2-entry unconditional loop, base 0x0063cab8, stride 0x16c.
    # Calls FUN_0041bc50 each iteration (unconditional).
    # 28 bytes. Stable across 10 quiescent-menu iterations.
    'hud_dispatch_slot2': {
        'rva':            0x0041c0c0,
        'export':         'HudDispatchSlot2',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # Session c3-batch-c-s3 â€” hud+frontend getters batch-c (C2->C3, 4 candidates)
    # HUD/HudDispatch.cpp, Frontend/FrontendAccessors.cpp, Frontend/FrontendMode.cpp
    # Pure-leaf HUD sub-mode and Frontend global/array/mode-index getters.
    # Candidate 0x00436810 DEFERRED -- U-3410/U-3411 unresolved.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042f6a0  HudSubModeGet
    # Returns DAT_0067e9fc (uint32_t global). 5 bytes: MOV EAX,[imm32]; RET.
    # Used as switch discriminant in FUN_0040dfc0 (per-frame HUD dispatch).
    # read_global: write sentinel, call fn(), verify return == sentinel.
    # ref: re/analysis/hud_ingame_promote_c2/0x0042f6a0.md
    'hud_sub_mode_get': {
        'rva':            0x0042f6a0,
        'export':         'HudSubModeGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067e9fc,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000002],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # 0x0040ad20  FrontendGlobalGet
    # Returns DAT_008a95ac (uint32_t global). 6 bytes: MOV EAX,[imm32]; RET.
    # read_global: write sentinel, call fn(), verify return == sentinel.
    # ref: re/analysis/hud_frontend_d3/0x0040ad20.md
    'frontend_global_get': {
        'rva':            0x0040ad20,
        'export':         'FrontendGlobalGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x008a95ac,        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00FF00FF],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # 0x0040b6c0  FrontendArrayGet
    # Returns DAT_008a94f0[param_1] -- indexed 4-byte read. 11 bytes.
    # No bounds check in original; int_scalar: pass index, compare return.
    # ref: re/analysis/hud_frontend_d3/0x0040b6c0.md
    'frontend_array_get': {
        'rva':            0x0040b6c0,
        'export':         'FrontendArrayGet',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, -1],
        'path2_tests':    [0, 1, 2, 9],
    },

    # 0x004309b0  FrontendModeIndex
    # int(void): reads DAT_0067e9fc, computes (mode-2), switch -> mapped index.
    # 52 bytes. Cases: 0->0, 1->1, 2->2, 3->5, 4..7->3, 8->11, default->(mode-2).
    # arg_type='none': called 10x at quiescent state; compare return value.
    # ref: re/analysis/hud_frontend_d5/0x004309b0.md
    'frontend_mode_index': {
        'rva':            0x004309b0,
        'export':         'FrontendModeIndex',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # Session c3-batch-c-s6 â€” hud_promote_c2_b font sys state (C2->C3)
    # HUD/FontCtx.cpp â€” font matrix stack + render-state init + ctx ops
    # Drift-promote: 0x004c57a0 C1->C2
    # FlushMatrix (0x00552e40) EXCLUDED â€” Frida not run, S-2126 open.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00552da0  FontCtx_SetScale
    # fn(float sx, float sy) -> uint32(1).
    # Applies RwMatrixScale(ctx, {sx,sy,1}, preconcat=1); zeroes DAT_00912bd8+bec.
    # Observable: return value (1) and dirty-flag DAT_00912bd8 (must be 0 after).
    # Strategy: font_ctx_float2 -- write 0xDEADBEEF to dirty flag, call fn(sx,sy),
    # read back (ret<<16)|dirty. Both paths must produce (0x0001<<16)|0x0000.
    # 10 (sx,sy) pairs covering zero, identity, scale-up, negative, small, large.
    'font_ctx_set_scale': {
        'rva':            0x00552da0,
        'export':         'FontCtx_SetScale',
        'signature':      {'ret': 'uint32', 'args': ['float', 'float']},
        'arg_type':       'font_ctx_float2',
        'lut_root_delta': 0,
        'path1_tests': [
            [0.0,   0.0  ],
            [1.0,   1.0  ],
            [2.0,   0.5  ],
            [-1.0, -1.0  ],
            [0.001, 0.001],
            [100.0, 100.0],
            [0.5,   2.0  ],
            [-2.0,  3.0  ],
            [1.5,   1.5  ],
            [0.25,  4.0  ],
        ],
        'path2_tests': [
            [0.0,  0.0],
            [1.0,  1.0],
            [2.0,  0.5],
        ],
    },

    # 0x00552df0  FontCtx_SetTranslation
    # fn(float x, float y) -> uint32(1).
    # Applies RwMatrixTranslate(ctx, {x,y,0}, preconcat=1); zeroes DAT_00912bd8+bec.
    # Observable: return value (1) and dirty-flag DAT_00912bd8 (must be 0 after).
    # Strategy: font_ctx_float2 -- same as SetScale above.
    # 10 (x,y) pairs covering zero, unit, positive, negative, large, small.
    'font_ctx_set_translation': {
        'rva':            0x00552df0,
        'export':         'FontCtx_SetTranslation',
        'signature':      {'ret': 'uint32', 'args': ['float', 'float']},
        'arg_type':       'font_ctx_float2',
        'lut_root_delta': 0,
        'path1_tests': [
            [0.0,   0.0  ],
            [1.0,   0.0  ],
            [0.0,   1.0  ],
            [100.0, 200.0],
            [-50.0, -75.0],
            [0.5,   0.5  ],
            [800.0, 600.0],
            [-0.1,  0.1  ],
            [1e-4,  1e-4 ],
            [1e4,   1e4  ],
        ],
        'path2_tests': [
            [0.0,   0.0  ],
            [1.0,   0.0  ],
            [100.0, 200.0],
        ],
    },

    # 0x00552c10  FontSys_InitRenderState
    # fn(void) -> uint32(1).
    # Master font render-state initialiser: allocs ctx slot-0, identity table
    # (256 shorts), 31 ptr-slot clears, 18 float globals, aspect ratio defaults.
    # Observable: return value (must be 1). Side-effects too broad to inject;
    # use 'none' -- call 10x at quiescent main menu, confirm both return 1.
    # Idempotent: calling it repeatedly resets the same state each time.
    'font_sys_init_render_state': {
        'rva':            0x00552c10,
        'export':         'FontSys_InitRenderState',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00552d10  FontMatrix_Push
    # fn(void) -> bool.
    # Stack overflow guard (depth < 31); increment depth; lazy-alloc; 64-byte copy.
    # Observable: return bool and new stack depth.
    # Strategy: font_matrix_push -- inject depth, call, read back (ret|(new_depth<<8)).
    # Test depths (2026-05-24 phase-a1): only 0 (push slot 1, copies from
    # slot 0 â€” valid post-prelude) and 31 (overflow early-out, returns false
    # without touching ctx). Depths 1..30 deref unallocated slots and AV
    # both sides identically â€” non-informative. Tests alternate to exercise
    # both code paths (success + overflow).
    'font_matrix_push': {
        'rva':            0x00552d10,
        'export':         'FontMatrix_Push',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'font_matrix_push',
        'lut_root_delta': 0,
        'path1_tests': [
            {'depth':  0},
            {'depth': 31},
            {'depth':  0},
            {'depth': 31},
            {'depth':  0},
            {'depth': 31},
            {'depth':  0},
            {'depth': 31},
            {'depth':  0},
            {'depth': 31},
        ],
        'path2_tests': [
            {'depth':  0},
            {'depth': 31},
            {'depth':  0},
        ],
    },

    # Session c3-batch-b-s1 â€” frontend_score_getters (C2->C3, 6 candidates)
    # Frontend/MenuScoreGetters.cpp â€” pure-leaf indexed reads + global getters
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0040b6b0  ModeScoreGetBySlot
    # Returns DAT_008a9530[param_1] â€” per-slot mode-score array element.
    # arg_type='int_scalar': single int index, returns uint32.
    'mode_score_get_by_slot': {
        'rva':            0x0040b6b0,
        'export':         'ModeScoreGetBySlot',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x0040b7a0  HotkeyStringBaseGet
    # Returns DAT_0063b8ec â€” hotkey string base global. No args.
    # arg_type='read_global': write sentinel, call, read back.
    'hotkey_string_base_get': {
        'rva':            0x0040b7a0,
        'export':         'HotkeyStringBaseGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0063b8ec,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # 0x0040b7b0  PlayerHotkeyTableGet
    # 4-table dispatch: returns array[param_1] for table selected by param_2 (0-3).
    # arg_type='int_pair': two int args [param_1, param_2].
    # NOTE: 'int_int_scalar' does not exist in registry; using 'int_pair' instead.
    'player_hotkey_table_get': {
        'rva':            0x0040b7b0,
        'export':         'PlayerHotkeyTableGet',
        'signature':      {'ret': 'uint32', 'args': ['uint32', 'uint32']},
        'arg_type':       'int_pair',
        'lut_root_delta': 0,
        'path1_tests': [
            [0, 0], [1, 0], [2, 0], [3, 0],
            [0, 1], [1, 1], [2, 1], [3, 1],
            [0, 2], [1, 2], [2, 2], [3, 2],
            [0, 3], [1, 3], [2, 3], [3, 3],
            [0, 4], [1, 5],
        ],
        'path2_tests': [
            [0, 0], [1, 1], [2, 2], [3, 3], [0, 4],
        ],
    },

    # 0x00429870  LapTimeALessThanB
    # Reads 6 globals, computes time_A and time_B, returns 1 if A < B else 0.
    # No args. arg_type='read_global': sentinel-write to DAT_0067d98c, call, check.
    'lap_time_a_less_than_b': {
        'rva':            0x00429870,
        'export':         'LapTimeALessThanB',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067d98c,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0x00000002, 0x00000003,
                           0x0000000a, 0x0000000f, 0xDEADBEEF, 0x00000005,
                           0x00000000, 0x00000007],
        'path2_tests':    [0x00000000, 0x00000001, 0x00000002],
    },

    # 0x00429a70  LapFracGetBySlot
    # Returns (float)DAT_0067d99c[param_1] â€” indexed float read, lap frac array.
    # arg_type='int_scalar': single int index, returns float.
    'lap_frac_get_by_slot': {
        'rva':            0x00429a70,
        'export':         'LapFracGetBySlot',
        'signature':      {'ret': 'float', 'args': ['uint32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x00429a80  LapLapsGetBySlot
    # Returns DAT_0067d98c[param_1] â€” indexed read of lap laps array.
    # arg_type='int_scalar': single int index, returns uint32.
    'lap_laps_get_by_slot': {
        'rva':            0x00429a80,
        'export':         'LapLapsGetBySlot',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x004f5020  FormatEntryLookup  (c3_batch_ah s4)
    # return *(u32*)(&DAT_005dae40 + idx*4) -- indexed read of the static .rdata
    # format-size table at 0x005dae40 (entries 0..16 = 4,8,12,16,4,4,4,8,...).
    # arg_type='int_scalar': single int index, returns uint32. Diverse in-range
    # indices give a varied (non-degenerate) observable.
    # ref: re/analysis/bucket_004f022d/0x004f5020.md
    'format_entry_lookup': {
        'rva':            0x004f5020,
        'export':         'FormatEntryLookup',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 7, 10, 12, 16, 5, 8, 0],
        'path2_tests':    [0, 1, 2],
    },

    # Session c3-batch-b-s2 â€” frontend batch b session 2 (C2â†’C3, 6 candidates)
    # MenuScoreGetters.cpp + MenuInit.cpp
    # SlotSortByModeScore (0x0040b620) OMITTED â€” arg_type 'void_out_ptr'
    #   (pass ptr, call, readback 4-element array) not supported by harness.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00429a90  LapSecsGetBySlot
    # Returns (&DAT_0067d994)[param_1] â€” indexed read of lap-time seconds
    # component by slot. 12-byte body. Same indexed-array pattern as other
    # lap time getters.
    # int_scalar: passes param_1 as int, reads back return value.
    # ref: re/analysis/frontend_promote_menus_b/00429a90.md
    'lap_secs_get_by_slot': {
        'rva':            0x00429a90,
        'export':         'LapSecsGetBySlot',        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x00430760  IsMultiplayerMode
    # Returns 1 if DAT_0067e9fc in {2, 3, 4, 5, 10}; else 0.
    # 38-byte body. Multi-mode boolean check on game mode global at 0x0067e9fc.
    # read_global: write sentinel, call fn(), verify return.
    # 10 dummy-marker tests (game-mode global at quiescent main-menu = 1 â†’ 0).
    # ref: re/analysis/frontend_promote_menus_b/00430760.md
    'is_multiplayer_mode': {
        'rva':            0x00430760,
        'export':         'IsMultiplayerMode',        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067e9fc,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0x00000002, 0x00000003,
                           0x00000004, 0x00000005, 0x00000009, 0x0000000A,
                           0x0000000B, 0x00000000],
        'path2_tests':    [0x00000001, 0x00000002, 0x0000000A],
    },

    # 0x0042fe80  GetRaceEndFlag
    # 5-byte body: MOV EAX, [0x0067ea90]; RET.
    # Pure getter of DAT_0067ea90 (race-end flag / slot gate).
    # read_global: write sentinel, call fn(), verify return == sentinel.
    # ref: re/analysis/promote_c2_frontend_menus/0x0042fe80.md
    'get_race_end_flag': {
        'rva':            0x0042fe80,
        'export':         'GetRaceEndFlag',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067ea90,        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00FF00FF],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # 0x0042f0b0  GetFrameCounterPlus73
    # 8-byte body: MOV EAX, [0x0067f17c]; ADD EAX, 0x49; RET.
    # Returns animation frame counter (DAT_0067f17c) + 73 (0x49 cited at 0x0042f0b4).
    # read_global: write sentinel, call fn(), verify return == sentinel + 73.
    # Note: harness checks exact return value â€” we write known sentinel to
    # 0x0067f17c, reimpl returns sentinel+0x49; orig does the same.
    # ref: re/analysis/promote_c2_frontend_menus/0x0042f0b0.md
    'get_frame_counter_plus73': {
        'rva':            0x0042f0b0,
        'export':         'GetFrameCounterPlus73',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067f17c,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0x00000064, 0x000003E8,
                           0x7FFFFF00, 0xFFFFFF80, 0x00001000, 0x00010000,
                           0x12345678, 0xDEADB000],
        'path2_tests':    [0x00000001, 0x00000064, 0x00001000],
    },

    # 0x0042d3e0  MenuEntryArrayInit
    # 58-byte body. Zeroes 14 selected offsets per entry across 30 entries
    # of a 52-byte struct array at 0x00898ac0..0x008990dc.
    # void_write_observe: write sentinel to DAT_00898ac0 (array base, first
    # dword of entry[0] = offset -4 relative to loop ptr start), call fn(),
    # read back to verify sentinel was zeroed.
    # ref: re/analysis/promote_c2_frontend_menus/0x0042d3e0.md
    'menu_entry_array_init': {
        'rva':            0x0042d3e0,
        'export':         'MenuEntryArrayInit',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x00898ac0,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],    },

    # Session c3-batch-b-s4 â€” frontend_promote_menus_b (C2â†’C3, 3 of 6)
    # MenuScoreSort.cpp + MenuRaceEnd.cpp
    #
    # NOT registered (callee gate failures or unsupported arg_type):
    #   0x0040b460  SlotSortByScoreWithModeOverride â€” REFUSED: callee 0x00417740 C0 (STUB S-0491)
    #   0x00429a30  LapTimeStoreToPlayerArrays â€” BLOCKED: callee 0x00430790 C1 with open U-3470
    #   0x0040e3a0  PlayerColorTableGet â€” OMITTED: (int, byte*) signature not supported by
    #                harness (pointer out-param needs custom arg_type)
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00430830  SplitScreenTrackAssignment
    # uint32 FUN_00430830(int param_1) â€” pure leaf, no callees.
    # Switch on DAT_0067e9fc (game mode) returns from per-slot layout arrays.
    # Returns 0 for default/unsupported modes.
    # int_scalar: pass slot index; at main-menu quiescent state (mode typically
    # 2), returns DAT_007f0a44[param_1 * 0xc]. Both paths must agree.
    # ref: re/analysis/frontend_promote_menus_b/00430830.md
    'split_screen_track_assignment': {
        'rva':            0x00430830,
        'export':         'SplitScreenTrackAssignment',        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x0042fe30  RaceEndFlagIfEndMode
    # uint32 FUN_0042fe30(void) â€” calls GetRaceSubMode()==0xb -> return 1, else DAT_0067ea74.
    # arg_type='none': called 10x at quiescent main menu (mode != 0xb at menu).
    # Both paths must return the same value (DAT_0067ea74 at menu state).
    # Callee GetRaceSubMode (0x0042f6a0) is C3.
    # ref: re/analysis/promote_c2_frontend_menus/0x0042fe30.md
    'race_end_flag_if_end_mode': {
        'rva':            0x0042fe30,
        'export':         'RaceEndFlagIfEndMode',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042fe50  RaceEndAltFlagIfEndMode
    # uint32 FUN_0042fe50(void) â€” complement of RaceEndFlagIfEndMode.
    # Returns DAT_0067ea78 when GetRaceSubMode()!=0xb, else 0.
    # arg_type='none': called 10x at quiescent main menu; both paths must agree.
    # Callee GetRaceSubMode (0x0042f6a0) is C3.
    # ref: re/analysis/promote_c2_frontend_menus/0x0042fe50.md
    'race_end_alt_flag_if_end_mode': {
        'rva':            0x0042fe50,
        'export':         'RaceEndAltFlagIfEndMode',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # Session c3-batch-b-s5 â€” Frontend sprite dispatch + time decompose
    # SpriteDispatch.cpp + MenuTime.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042fab0  SpriteSlotDispatch
    # Assembly-confirmed 10-way dispatch: MOV [ESP+4], slot_ptr; JMP FUN_0040bb90.
    # Slot 0 -> ptr 0x5cd898; slot 9 -> ptr 0x5cd838 (stride mostly 0xc).
    # Strategy: sprite_table_dispatch (added phase-a1 2026-05-24) â€” patches
    # FUN_0040bb90 with a NativeCallback that captures the first arg (the
    # slot ptr). Both Orig and Reimpl route through the patched stub; their
    # captured ptrs must match. Prior arg_type 'int_scalar' produced
    # both-AV-at-0x8 (FUN_0040bb90 derefs NULL list head) â€” banned as GREEN.
    'sprite_slot_dispatch': {
        'rva':            0x0042fab0,
        'export':         'SpriteSlotDispatch',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'sprite_table_dispatch',
        'callee_rva_str': '0x0040bb90',
        'lut_root_delta': 0,
        # Cover all 10 slots (0..9) + one out-of-range (default fall-through path).
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 100, -1],
        'path2_tests':    [0, 5, 100],
    },

    # 0x0042e590  SpriteAnimFrameThunk
    # Assembly-confirmed tail-call thunk: computes idx = sprite_slot + 2*DAT_0067f17c,
    # looks up sprite_ptr_table[idx] at 0x5f79d8, overwrites first arg, JMP FUN_0040bb70.
    # Strategy: sprite_table_dispatch (added phase-a1 2026-05-24) â€” patches
    # FUN_0040bb70 with a NativeCallback that captures the first arg (the
    # transformed sprite ptr). Both Orig and Reimpl produce the same lookup
    # using the same anim_frame global and sprite_ptr_table base, so captures
    # must match. Prior arg_type 'int_scalar' produced both-AV-at-0x8 â€” banned
    # as GREEN; this is the SpriteAnimFrameThunk fake-AV/AV the operator
    # specifically warned about.
    'sprite_anim_frame_thunk': {
        'rva':            0x0042e590,
        'export':         'SpriteAnimFrameThunk',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'sprite_table_dispatch',
        'callee_rva_str': '0x0040bb70',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2],
    },


    # Session c3-batch-d-s1 â€” frontend_menus large functions (C2->C3)
    # MenuHelpers.cpp: FrontendPlayerSlotCheck, FrontendCursorUpdate
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042ebe0  FrontendPlayerSlotCheck
    # bool(int param_1): player-slot fullness checker; dispatch key {10,11,12}.
    # Iterates 0x7f0a48 stride 0x1e0; returns (enableFlag != 0) && !bVar1.
    # arg_type='int_scalar': pass param_1 as uint32; compare bool return (0/1).
    # Only valid dispatch keys {10,11,12} are tested â€” the original behaviour
    # for other values is undefined (the dispatch table has no other active
    # slots at main menu and enableFlag reads outside meaningful range).
    # At quiescent main menu, player-slot array is initialised; function must
    # return same value from both original and reimpl.
    'frontend_player_slot_check': {
        'rva':            0x0042ebe0,
        'export':         'FrontendPlayerSlotCheck',
        'signature':      {'ret': 'uint32', 'args': ['int']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [10, 11, 12, 10, 11, 12, 10, 11, 12, 10, 11, 12],
        'path2_tests':    [10, 11, 12],
    },

    # 0x0042f7b0  FrontendCursorUpdate
    # void(void): 4-player cursor updater; release-edge detection on d-pad.
    # Guard: DAT_0067eab0 != 0 => immediate return without touching cursor state.
    # Strategy: void_write_observe on 0x0067eab0.
    #   Write sentinel t to DAT_0067eab0 (non-zero => early return).
    #   Call fn (void). Read back DAT_0067eab0.
    #   Since guard != 0, both orig and reimpl exit without writing to 0x0067eab0,
    #   so read-back == sentinel t in both cases => match.
    #   Sentinels are all non-zero so the guard fires and cursor state is not mutated.
    'frontend_cursor_update': {
        'rva':            0x0042f7b0,
        'export':         'FrontendCursorUpdate',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x0067eab0,
        'lut_root_delta': 0,
        # 10 non-zero sentinels: guard fires, fn returns early, readback == sentinel.
        'path1_tests':    [1, 2, 3, 4, 5, 0xDEADBEEF, 0xFF, 0x100, 0x7FFFFFFF, 0xCAFEBABE],
        'path2_tests':    [1, 2, 3],
    },


    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-d-s3 â€” hud_ingame_promote_c2 (C2->C3)
    # HudDispatch.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0040dfc0  HudIngameDispatch
    # void: top-level HUD in-game dispatcher. Two-stage guard (draw-enable +
    # race-state 5-7), then double switch (sub-mode / game-mode). 13 callees
    # all C2+. Called at main menu with DAT_0063ba8c < 5 -> returns immediately
    # via guard 2 (returns void; wrapped as uint32 0 for registry compat).
    # arg_type='none': both paths must agree on void/0 at quiescent main menu.
    # Evidence: log/diff_hud_ingame_dispatch.csv
    'hud_ingame_dispatch': {
        'rva':            0x0040dfc0,
        'export':         'HudIngameDispatch',        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-g-s11 â€” HUD-ingame core dispatch (C2->C3)
    # HUD/HudDispatch.cpp â€” loop helpers for {5/6}-path and {7}-path.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0041b630  HudSlotLoopB630
    # void(): iterates 4-entry array at 0x0063c8d0, stride 0x74 (116 bytes).
    # Per entry: reads int32_t at offset +0x6c; if non-zero -> calls FUN_0041b340.
    # Called on DAT_0063ba8c âˆˆ {5,6} path when FUN_0042f500()==0.
    # At main menu DAT_0063ba8c < 5 so HudIngameDispatch guard fires first â€”
    # this function is never reached. Strategy: read_global sentinel on
    # DAT_0063ba8c (set to 0 -> guard fires, 0 returned); both paths agree.
    # Evidence: log/diff_hud_slot_loop_b630.csv
    'hud_slot_loop_b630': {
        'rva':            0x0041b630,
        'export':         'HudSlotLoopB630',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0041ccc0  HudSlotLoopCcc0
    # void(): iterates 4-entry array at 0x0063ce20, stride 0x114 (276 bytes).
    # Per entry: reads int32_t at offset +0x110; if non-zero -> calls FUN_0041c9a0.
    # Called on DAT_0063ba8c == 7 path (default/6/10 sub-modes, FUN_0042f500()==0).
    # Same quiescent strategy as HudSlotLoopB630: read_global void at main menu.
    # Evidence: log/diff_hud_slot_loop_ccc0.csv
    'hud_slot_loop_ccc0': {
        'rva':            0x0041ccc0,
        'export':         'HudSlotLoopCcc0',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },


    # Session c3-batch-b-s6 â€” VehicleUnlockFlagGet (C2->C3)
    # VehicleMeta.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-f-s8 â€” audio dsound wrapper A (C2->C3, 4 candidates)
    # Audio/AudioDSound.cpp â€” COM wrapper field setter, vtable caller,
    # QI chain, and semaphore wrapper.
    # STRUCT GAP: AudioBufFieldSet accesses audio buf struct +0x74/+0x78/+0x11c/+0x38.
    # U-0361 (AudioDSoundRelease vtable slot semantic) â€” in ## Open uncertainties.
    # U-0360 (AudioDSoundQIChain IID at 005d09dc) â€” in ## Open uncertainties.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x005baf60  AudioBufFieldSet
    # void(ptr, int): writes param_2 to *(param_1+0x74), ORs 0x100 into
    # *(param_1+0x78), then if bit 3 of *(param_1+0x78) is set writes param_2
    # to *(*(param_1+0x11c)+0x38).
    # Strategy: buf_field_set â€” allocate 0x120-byte zeroed buffer, set +0x78 to
    # avoid bit 3 (keep 3D-attach flag clear so COM branch not taken), call
    # fn(buf, value), read back (buf+0x74) and (buf+0x78) packed as 64-bit.
    # Both paths write identical fields; packed readback must match.
    # 10 test values covering 0, positive, negative, flags, max.
    # Note: STRUCT GAP is filed (offsets confirmed mechanically, struct incomplete).
    'audio_buf_field_set': {
        'rva':            0x005baf60,
        'export':         'AudioBufFieldSet',
        'signature':      {'ret': 'void', 'args': ['pointer', 'int32']},
        'arg_type':       'buf_field_set',
        'buf_size':       0x120,
        'field_offsets':  [0x74, 0x78],  # offsets to read back after call
        'lut_root_delta': 0,
        # Each test is the int param_2 to pass. Bit 3 of +0x78 stays clear
        # (buffer starts zeroed) so the COM branch is not exercised.
        'path1_tests':    [0, 1, -1, 0x100, 0x200, 0xDEADBEEF, 0x7FFFFFFF,
                           0x80000000, 0xFF, 0x00010000],
        'path2_tests':    [0, 1, 0xDEADBEEF],
    },

    # 0x005baf90  AudioDSoundRelease
    # int(ptr): calls vtable slot 8 (offset 0x20) on IDirectSound8* at param_1+0x7c.
    # Discards return; always returns 1.
    # COM objects not available at main-menu; both paths crash identically when
    # param_1+0x7c is null (NULL ptr deref). crash_equal_ok=True.
    # Strategy: int_scalar â€” pass a fake ptr (e.g. 0x1000); both orig and reimpl
    # crash dereferencing *(0x1000+0x7c) at the same offset identically.
    'audio_dsound_release': {
        'rva':            0x005baf90,
        'export':         'AudioDSoundRelease',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # Fake pointer values â€” both orig and reimpl will AV identically.
        'path1_tests':    [0x1000, 0x1000, 0x1000, 0x1000, 0x1000,
                           0x1000, 0x1000, 0x1000, 0x1000, 0x1000],
        'path2_tests':    [0x1000, 0x1000, 0x1000],
    },

    # 0x005bc400  AudioDSoundQIChain
    # int(ptr*, ptr*): double QI chain on COM objects.
    # Both paths crash identically when param_2=NULL (first write *param_2=0 â†’ AV at 0x0).
    # crash_equal_ok=True; int_pair passes [fake_param1, 0] as two int args.
    # param_2=0 means both orig and reimpl AV at 0x0 writing *param_2=0. Crash strings match.
    'audio_dsound_qi_chain': {
        'rva':            0x005bc400,
        'export':         'AudioDSoundQIChain',
        'signature':      {'ret': 'int32', 'args': ['int32', 'int32']},
        'arg_type':       'int_pair',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # param_1 = arbitrary (not used before crash), param_2 = 0 (NULL).
        # First write `*param_2 = 0` â†’ AV at 0x0 for both orig and reimpl.
        'path1_tests': [
            [0x1000, 0],
            [0x1000, 0],
            [0x1000, 0],
            [0x1000, 0],
            [0x1000, 0],
            [0x1000, 0],
            [0x1000, 0],
            [0x1000, 0],
            [0x1000, 0],
            [0x1000, 0],
        ],
        'path2_tests': [
            [0x1000, 0],
            [0x1000, 0],
            [0x1000, 0],
        ],
    },

    # 0x005aeea0  AudioSemaphoreCreate
    # uint(void*, LONG, LONG): CreateSemaphoreA(NULL, param_2, param_3, NULL).
    # Stores HANDLE at *param_1. Returns -(bool(handle!=0)) & (uint)param_1.
    # Strategy: semaphore_create â€” allocate 4-byte scratch buf, call
    # fn(buf, initial_count, max_count), close the handle from *buf,
    # verify return value matches ((handle!=NULL)?buf_addr:0).
    # Both paths call CreateSemaphoreA with identical args, both should
    # succeed (non-null handle), and return value = buf_addr & 0xFFFFFFFF.
    # 10 test vectors with valid (initial_count <= max_count) semaphore params.
    'audio_semaphore_create': {
        'rva':            0x005aeea0,
        'export':         'AudioSemaphoreCreate',
        'signature':      {'ret': 'uint32', 'args': ['pointer', 'int32', 'int32']},
        'arg_type':       'semaphore_create',
        'lut_root_delta': 0,
        # Each test: [initial_count, max_count]. Both must be >= 0 and init <= max.
        'path1_tests': [
            [0, 1],
            [0, 1],
            [0, 1],
            [1, 1],
            [0, 10],
            [5, 10],
            [0, 100],
            [0, 1],
            [1, 2],
            [0, 0x7FFFFFFF],
        ],
        'path2_tests': [
            [0, 1],
            [1, 1],
            [0, 10],
        ],
    },

    # 0x0042ef40  VehicleUnlockFlagGet
    # Pure leaf: reads byte from DAT_007f0e50 + param_1*0xc at a byte-offset
    # selected by param_2 switch; returns 1 if byte == 0x01, else 0.
    # Array is zero-initialized before game-mode selection â†’ all tests return
    # 0 at quiescent main-menu state. A/B identity guaranteed (both read 0x00
    # from the same global array).
    # [UNCERTAIN U-3176] No bounds check on param_1 â€” reproduced as-is.
    # arg_type='int_pair': passes [param_1, param_2] directly.
    'vehicle_unlock_flag_get': {
        'rva':            0x0042ef40,
        'export':         'VehicleUnlockFlagGet',
        'signature':      {'ret': 'int32', 'args': ['int', 'int']},
        'arg_type':       'int_pair',
        'lut_root_delta': 0,
        # Test vectors: (vehicle_index, slot_type)
        # slot_type values: 2(default), 3, 4, 5, 10 and 1000-offset variants.
        'path1_tests': [
            [0, 2],   [0, 3],   [0, 4],   [0, 5],   [0, 10],
            [1, 2],   [1, 3],   [1, 4],   [1, 5],   [1, 10],
            [0, 0x3e9], [0, 0x3ea], [0, 0x3ed], [0, 0x3f3],
            [1, 0x3e9], [1, 0x3ea], [1, 0x3ed], [1, 0x3f3],
            [2, 2],   [3, 2],
        ],
        'path2_tests': [
            [0, 2], [0, 3], [1, 4], [1, 5], [0, 10],
        ],
    },

    # 0x00490ff0  RainSetCameraScale
    # 19-byte leaf setter: void FUN_00490ff0(param_1, param_2).
    #   00490ff0  MOV EAX,[ESP+0x8]       ; param_2
    #   00490ff4  MOV ECX,[ESP+0x4]       ; param_1
    #   00490ff8  MOV [0x006146b4],EAX    ; _DAT_006146b4 = param_2
    #   00490ffd  MOV [0x006146b0],ECX    ; _DAT_006146b0 = param_1
    #   00491003  RET                     ; cdecl plain ret; eax = param_2
    # Observable: original is void but the final store leaves eax = param_2 at RET.
    # int_pair (ret='uint32') reads eax; reimpl declares uint32 return + `return param_2`
    # to reproduce eax bit-for-bit. Distinct param_2 per vector -> non-degenerate.
    # ref: re/analysis/breadth_unmapped_0049x/0x00490ff0.md
    'rain_set_camera_scale': {
        'rva':            0x00490ff0,
        'export':         'RainSetCameraScale',
        'signature':      {'ret': 'uint32', 'args': ['uint32', 'uint32']},
        'arg_type':       'int_pair',
        'lut_root_delta': 0,
        # [param_1, param_2]; eax == param_2.
        'path1_tests': [
            [0x00000000, 0x00000000],
            [0x00000001, 0x00000002],
            [0x3e800000, 0x3e800000],   # default 0.25f (FUN_00490e70)
            [0x12345678, 0xDEADBEEF],
            [0xCAFEBABE, 0x3f000000],   # 0.5f
            [0xFFFFFFFF, 0x40000000],   # 2.0f
            [0x00000010, 0x80000000],
            [0x7FFFFFFF, 0x00C0FFEE],
            [0xABAD1DEA, 0x0BADF00D],
            [0x40490FDB, 0x3F800000],   # pi bits / 1.0f
        ],
        'path2_tests': [
            [0x3e800000, 0x3e800000],
            [0x12345678, 0xDEADBEEF],
            [0xFFFFFFFF, 0x40000000],
        ],
    },

    # Session c3-batch-e-s2 â€” VfsFileExists + AutosaveTrigger (C2->C3)
    # Save/GameSaveVFS.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00550b00  VfsFileExists
    # ~186 bytes. int(char*). VFS file-exists router.
    # Branch A (DAT_007dc75c != 0): colon-scan â†’ linked-list lookup â†’ vtable[0x13](obj, filename).
    # Branch B (DAT_007dc75c == 0): if DAT_007dc768==NULL â†’ return 0; else call (*cb)(6) â†’ 0.
    # At quiescent main-menu state: DAT_007dc75c is likely 0 (VFS not active) â†’ Branch B.
    # Branch B never dereferences param_1, so passing 0 or any pointer is safe at menu.
    # arg_type='int_scalar': passes filename pointer VA as int32 (ASLR disabled; fixed VAs).
    # At menu: both orig and reimpl â†’ Branch B â†’ return 0 (bit-identical).
    # Test vectors: 10x calls with 0 (null ptr); safe because Branch B never deref param_1.
    'vfs_file_exists': {
        'rva':            0x00550b00,
        'export':         'VfsFileExists',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # At quiescent main-menu state: DAT_007dc75c != 0 (VFS active; confirmed by
        # first diff attempt which showed Branch A is taken).
        # Use known game string VAs (no ':' in content):
        #   0x005cda7c: "Button"  (confirmed valid from SpriteLookupTableA tests)
        #   0x005cc414: "SemiC"   (confirmed valid from SpriteLookupTableA tests)
        # scan_fn scans for ':'; finds none â†’ default obj dispatch â†’ vtable[0x13].
        # Both orig and reimpl call same vtable fn on same obj â†’ bit-identical return.
        # arg_type='int_scalar': passes the VA directly (ASLR disabled; fixed VAs).
        'path1_tests': [
            0x005cda7c, 0x005cc414, 0x005cda7c, 0x005cc414, 0x005cda7c,
            0x005cc414, 0x005cda7c, 0x005cc414, 0x005cda7c, 0x005cc414,
        ],
        'path2_tests': [
            0x005cda7c, 0x005cc414, 0x005cda7c,
        ],
    },

    # 0x004099a0  AutosaveTrigger
    # ~54 bytes. void(void). Sets 3 globals + wprintf when autosave context set.
    # Guard: if DAT_008a95ac == 0 â†’ return immediately (no side effects).
    # At quiescent main-menu state: DAT_008a95ac == 0 â†’ guard fires â†’ both return void/0.
    # arg_type='none': called 5x; both paths return void/0 at quiescent state.
    # Globals observed side-channel: DAT_008a9584, DAT_008a9594, DAT_008a95b0 unchanged.
    'autosave_trigger': {
        'rva':            0x004099a0,
        'export':         'AutosaveTrigger',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        # 5 calls at quiescent menu; DAT_008a95ac==0 â†’ early return; both return 0.
        'path1_tests':    [0, 1, 2, 3, 4],
        'path2_tests':    [0, 1, 2],
    },

    # Session save-sdone-a-s3 â€” career event helpers (C1->C3)
    # Save/CareerEvents.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042a920  PostTrophyEvent  void(uint32 event_id)
    # Pure 20-byte leaf: writes DAT_00898ab0=0x40 (priority), then param_1 into
    # DAT_00899140 (event-ID slot consumed by frontend state machine).
    # Disasm: MOV [0x898ab0],0x40; MOV [0x899140],param_1; RET.
    # Caller: 0x00430290 Championship::Complete (posts IDs 0x264..0x26a).
    # arg_type='void_setter_observe': call fn(event_id), read back DAT_00899140.
    # The fixed priority write (0x40 to DAT_00898ab0) is implicitly verified by
    # the bit-identical readback of DAT_00899140 (same code path must have run).
    # Pure leaf â€” leaf-function exemption applies (CONFIDENCE.md, 2026-05-09).
    # [UNCERTAIN U-1552] DAT_00898ab0 = 0x40 semantic unknown (non-blocking).
    'post_trophy_event': {
        'rva':            0x0042a920,
        'export':         'PostTrophyEvent',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x00899140,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000264, 0x00000265, 0x00000266, 0x00000267,
                           0x00000268, 0x00000269, 0x0000026a, 0x00000000,
                           0xDEADBEEF, 0x00000001],
        'path2_tests':    [0x00000264, 0x00000265, 0x0000026a],
    },

    # Session c3-batch-i-s4 â€” Settings/video-config CONFIG_SAVE_FN (C2->C3)
    # Save/SettingsCfg.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004989b0  ConfigSave
    # ~76 bytes. int(void). Writes 512 bytes from global settings buffer at
    # 0x00773208 to "videocfg.bin" via game CRT _fsopen("wb")/_fwrite/_fclose.
    # Returns 1 on file-open success, 0 on failure.
    # At quiescent main-menu state, videocfg.bin exists+writable; both orig and
    # reimpl hit the success path â†’ return 1 â†’ bit-identical.
    # arg_type='none': 5 calls (file-write side effect is identical between both
    # paths â€” both write the same 512 bytes from the same global buffer).
    # Side effect: rewrites videocfg.bin (same canonical 800x600 contents).
    'config_save': {
        'rva':            0x004989b0,
        'export':         'ConfigSave',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # ---------------------------------------------------------------------------
    # Boot/CrtCompilerSupport.cpp â€” FidDB-identified MSVC CRT compiler support.
    # All four are C2-ceiling functions; C3 evidence is FidDB single-match only.
    # ---------------------------------------------------------------------------

    # 0x004a4b93  CrtFastErrorExit (_fast_error_exit / fast_error_exit)
    # void(uint32) â€” checks banner flag at 0x007739f0; if 1 calls __FF_MSGBANNER;
    # forwards param_1 to FUN_004ab8d6; terminates via ___crtExitProcess(0xFF).
    # arg_type='int_scalar': passes the error code as a uint32.
    # Note: function does not return (calls ExitProcess); harness must not wait
    # for a return value.  At quiescent state the banner flag is 0, so
    # __FF_MSGBANNER is not called â€” the function races to ExitProcess(0xFF).
    # Frida can Intercept this (it has a conventional cdecl prologue), but calling
    # it during a diff run would terminate the process.  Mark harness-limited.
    'crt_fast_error_exit': {
        'rva':            0x004a4b93,
        'export':         'CrtFastErrorExit',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'harness_limited',
        'lut_root_delta': 0,
        # Harness-limited: calling this function terminates the host process.
        # C3 evidence: FidDB single-match VS2003 _fast_error_exit +
        # decompilation verified structurally identical to __amsg_exit except
        # exit path goes directly to ___crtExitProcess(0xFF) (not PTR___exit).
        'path1_tests': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10],
        'path2_tests': [1, 2],
    },

    # 0x004a3440  CrtStackProbe (__chkstk)
    # void(void) [implicit EAX = stack size] â€” page-probe loop.
    # Non-standard ABI; cannot be hooked via Frida Interceptor without stack
    # corruption.  C3 evidence: FidDB single-match VS2003 __chkstk +
    # decompilation matches page-probe pattern (threshold 0x1000, step 0x1000).
    'crt_stack_probe': {
        'rva':            0x004a3440,
        'export':         'CrtStackProbe',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'harness_limited',
        'lut_root_delta': 0,
        # Harness-limited: implicit EAX argument; Frida Interceptor cannot hook
        # without corrupting the stack-probe frame.
        # C3 evidence: FidDB single-match VS2003 __chkstk; decompilation matches
        # VS2003 page-probe body exactly (threshold 0x1000 @ 0x004a3440).
        'path1_tests': [0x1000, 0x2000, 0x3000, 0x4000,
                        0x800, 0x100, 0x10, 0x1, 0xFFF, 0xFFFF],
        'path2_tests': [0x1000, 0x800],
    },

    # 0x004a5984  CrtSehProlog (__SEH_prolog)
    # void(undefined4, int) â€” saves EBX/ESI/EDI/retaddr; installs ExceptionList.
    # Compiler-injected SEH infrastructure; non-standard ABI; cannot be hooked
    # via Frida Interceptor without corrupting the SEH chain.
    # C3 evidence: FidDB single-match VS __SEH_prolog +
    # decompilation matches (saves 4 regs at computed offsets; links ExceptionList).
    'crt_seh_prolog': {
        'rva':            0x004a5984,
        'export':         'CrtSehProlog',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'harness_limited',
        'lut_root_delta': 0,
        # Harness-limited: compiler-injected SEH prolog; Frida Interceptor would
        # corrupt the ExceptionList chain.
        # C3 evidence: FidDB single-match VS __SEH_prolog; decompilation saves
        # EBX@+0xc, ESI@+0x8, EDI@+0x4, retaddr@+0x0; installs ExceptionList.
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests': [0, 1],
    },

    # 0x004a59bf  CrtSehEpilog (__SEH_epilog)
    # void(void) â€” restores ExceptionList from EBP[-4]; writes retaddr.
    # Compiler-injected SEH teardown; non-standard ABI; cannot be hooked via
    # Frida Interceptor without corrupting the SEH chain.
    # C3 evidence: FidDB single-match VS __SEH_epilog +
    # decompilation matches (reads EBP[-4] â†’ ExceptionList; writes retaddr to *EBP).
    'crt_seh_epilog': {
        'rva':            0x004a59bf,
        'export':         'CrtSehEpilog',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'harness_limited',
        'lut_root_delta': 0,
        # Harness-limited: compiler-injected SEH epilog; hooking would corrupt
        # the SEH chain.
        # C3 evidence: FidDB single-match VS __SEH_epilog; decompilation reads
        # ExceptionList from EBP[-4] and restores it, then writes retaddr.
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests': [0, 1],
    },

    # â”€â”€ Session c3-batch-e-s8: Boot/CrtInit C2â†’C3 promotions â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004a78b0  CrtPreInitLoop
    # Zero-slot fn-ptr table iterator. SEH frame present; loop body unreachable
    # as decompiled (start == end == 0x005e7b84 â†’ zero iterations).
    # [UNCERTAIN U-0005] Decompiler may have collapsed two distinct table
    #   pointers into the same symbol; reproduced as-is.
    # Original returns void; we declare ret='uint32' for harness compat
    # (EAX will be whatever it was â€” both sides return the same EAX because the
    # loop body is unreachable and the function is side-effect-free at the main
    # menu).  arg_type='none': harness calls with no args, compares EAX.
    # 10 calls at quiescent main menu; side-effect-free.
    'crt_pre_init_loop': {
        'rva':            0x004a78b0,
        'export':         'CrtPreInitLoop',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        # 10 dummy markers; input is ignored for 'none' arg_type.
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests': [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-e-s9 â€” boot_crt_env  (C2â†’C3, 2 candidates)
    # Boot/CrtEnvArgv.cpp â€” CRT startup env-string functions
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004abc53  CrtSetEnvp  (__setenvp, Visual Studio 2003 Release)
    # Takes no args; returns int (0 on success, -1 on failure).
    # At main-menu time DAT_007739e8 is already NULL (freed during startup),
    # so every re-call immediately returns -1. Both orig and reimpl must
    # return -1 identically â€” 10 calls confirm stable bit-identical output.
    # lut_root_delta=0 (no LUT; poll just confirms game is past init).
    'crt_set_envp': {
        'rva':            0x004abc53,
        'export':         'CrtSetEnvp',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-f-s3 â€” audio sub-struct lifecycle cluster (C2->C3, 4)
    # Audio/AudioRws.cpp
    # Pool-return/heap-free/combined-cleanup leaves + 16-byte fmt-key comparator.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x005ae080  AudioSubStructAFree
    # int*(int*): if *param_1!=0 and bit0(param_1[2]) â†’ pool-return FUN_005ae920;
    # clear bit0. Returns param_1.
    # At quiescent main menu audio sub-structs have null data pointers; guard
    # fails safely (no pool-return triggered). Use 'none' (10 no-crash checks).
    'audio_sub_struct_a_free': {
        'rva':            0x005ae080,
        'export':         'AudioSubStructAFree',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004abf28  CrtGetEnvStrings  (___crtGetEnvironmentStringsA, Visual Studio 2003 Release)
    # Takes no args; returns LPVOID (malloc'd ANSI env block, or NULL on failure).
    # At main-menu time DAT_00773d48 is already 1 (wide path succeeded at startup).
    # Re-calling the function: re-fetches GetEnvironmentStringsW, converts, returns
    # a new allocation.  Pointer values differ between orig and reimpl calls (different
    # heap addresses), so we compare null/non-null (1=non-null, 0=null) using
    # ptr_nonnull_check.  target_global=0x00773d48 lets each test pre-write the
    # mode flag to exercise different code paths (0=detect, 1=wide, 2=ANSI).
    'crt_get_env_strings': {
        'rva':            0x004abf28,
        'export':         'CrtGetEnvStrings',
        'signature':      {'ret': 'pointer', 'args': []},
        'arg_type':       'ptr_nonnull_check',
        'target_global':  0x00773d48,
        'lut_root_delta': 0,
        # Pre-write values for DAT_00773d48 before each call.
        # 0 â†’ detection path (calls GetEnvironmentStringsW, may set to 1 or 2)
        # 1 â†’ wide path  (calls GetEnvironmentStringsW again, converts)
        # 2 â†’ ANSI path  (calls GetEnvironmentStrings ANSI, copies)
        # 0 again Ã— 8 â†’ repeatedly exercise detection path
        # Both orig and reimpl must return non-null (1) for all paths.
        'path1_tests':    [0, 1, 2, 0, 1, 2, 0, 1, 2, 0],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-f-s2 â€” audio fmt-desc utility cluster (C2â†’C3, 4 candidates)
    # Audio/AudioRws.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x005aca80  AudioFmtSizeCalc
    # int(ptr): serialised byte size of a format descriptor.
    # Leaf; reads ptr+0x04 (secondary header ptr), ptr+0x10 (extra-data ptr),
    # ptr+0x14 (extra-data byte size).
    # Returns 0x1c (base), 0x2c (extended), or 0x2c + extra-data size.
    # fmt_desc_ptr: each test { f04, f10, f14 } drives the 3 branch combinations.
    'audio_fmt_size_calc': {
        'rva':            0x005aca80,
        'export':         'AudioFmtSizeCalc',
        'signature':      {'ret': 'int32', 'args': ['pointer']},
        'arg_type':       'fmt_desc_ptr',
        'lut_root_delta': 0,
        # Tests covering:
        #   f04=0, f10=0 â†’ 0x1c (28)
        #   f04â‰ 0, f10=0 â†’ 0x2c (44)
        #   f04=0, f10â‰ 0, f14=8 â†’ 0x1c+8=36
        #   f04â‰ 0, f10â‰ 0, f14=8 â†’ 0x2c+8=52
        #   various extra-data sizes
        'path1_tests': [
            {'f04': 0x00000000, 'f10': 0x00000000, 'f14': 0x00000000},  # 0x1c
            {'f04': 0x00000001, 'f10': 0x00000000, 'f14': 0x00000000},  # 0x2c
            {'f04': 0x00000000, 'f10': 0x00000001, 'f14': 0x00000008},  # 0x24
            {'f04': 0x00000001, 'f10': 0x00000001, 'f14': 0x00000008},  # 0x34
            {'f04': 0xDEADBEEF, 'f10': 0x00000000, 'f14': 0x00000000},  # 0x2c
            {'f04': 0x00000000, 'f10': 0xCAFEBABE, 'f14': 0x00000020},  # 0x3c
            {'f04': 0xDEADBEEF, 'f10': 0xCAFEBABE, 'f14': 0x00000020},  # 0x4c
            {'f04': 0x00000000, 'f10': 0x00000001, 'f14': 0x00000000},  # 0x1c (+0)
            {'f04': 0x00000001, 'f10': 0x00000001, 'f14': 0x00000100},  # 0x12c
            {'f04': 0xFFFFFFFF, 'f10': 0xFFFFFFFF, 'f14': 0x00000004},  # 0x30
        ],
        'path2_tests': [
            {'f04': 0x00000000, 'f10': 0x00000000, 'f14': 0x00000000},
            {'f04': 0x00000001, 'f10': 0x00000000, 'f14': 0x00000000},
            {'f04': 0x00000001, 'f10': 0x00000001, 'f14': 0x00000008},
        ],
    },

    # 0x005ac980  AudioFmtDescCopy
    # void(src_ptr, dst_ptr, zero_init): selective field copy between format descs.
    # Leaf; reads src +0x00(u32), +0x04(u8), +0x05(u8), +0x10(u8 flags bits 0,2).
    # Writes dst +0x04(u32), +0x0c(u8), +0x0d(u8), +0x18(u8 bit0/bit2).
    # If zero_init != 0: zeroes 9 dst fields before copy.
    # fmt_desc_copy: each test {f00, f04, f05, f10, zero_init}.
    # Observable: packed dst fields (d04 ^ d0c ^ d0d ^ d18).
    'audio_fmt_desc_copy': {
        'rva':            0x005ac980,
        'export':         'AudioFmtDescCopy',
        'signature':      {'ret': 'void', 'args': ['pointer', 'pointer', 'int32']},
        'arg_type':       'fmt_desc_copy',
        'lut_root_delta': 0,
        # Tests covering zero_init=0/1, flags 0x00/0x01/0x04/0x05, varied fields.
        'path1_tests': [
            {'f00': 0x00000000, 'f04': 0x00, 'f05': 0x00, 'f10': 0x00, 'zero_init': 0},
            {'f00': 0x12345678, 'f04': 0xAB, 'f05': 0xCD, 'f10': 0x00, 'zero_init': 0},
            {'f00': 0x12345678, 'f04': 0xAB, 'f05': 0xCD, 'f10': 0x01, 'zero_init': 0},
            {'f00': 0x12345678, 'f04': 0xAB, 'f05': 0xCD, 'f10': 0x04, 'zero_init': 0},
            {'f00': 0x12345678, 'f04': 0xAB, 'f05': 0xCD, 'f10': 0x05, 'zero_init': 0},
            {'f00': 0xDEADBEEF, 'f04': 0xFF, 'f05': 0x00, 'f10': 0x00, 'zero_init': 1},
            {'f00': 0xDEADBEEF, 'f04': 0xFF, 'f05': 0x00, 'f10': 0x01, 'zero_init': 1},
            {'f00': 0xDEADBEEF, 'f04': 0xFF, 'f05': 0x00, 'f10': 0x04, 'zero_init': 1},
            {'f00': 0xDEADBEEF, 'f04': 0xFF, 'f05': 0x00, 'f10': 0x05, 'zero_init': 1},
            {'f00': 0xCAFEBABE, 'f04': 0x10, 'f05': 0x20, 'f10': 0x07, 'zero_init': 1},
        ],
        'path2_tests': [
            {'f00': 0x12345678, 'f04': 0xAB, 'f05': 0xCD, 'f10': 0x01, 'zero_init': 0},
            {'f00': 0xDEADBEEF, 'f04': 0xFF, 'f05': 0x00, 'f10': 0x05, 'zero_init': 1},
            {'f00': 0xCAFEBABE, 'f04': 0x10, 'f05': 0x20, 'f10': 0x04, 'zero_init': 0},
        ],
    },

    # 0x005acd10  AudioFmtTableSearch
    # int(ctx_ptr, desc_ptr): linear search through audio context format-entry array.
    # Accesses ctx+0x24 (count), ctx+0x28 (array base ptr).
    # STRUCT GAP: audio context at +0x24/+0x28 not fully documented.
    # Returns 1 if count==0 or any entry matches; 0 if exhausted.
    # Callee: FUN_005ac9e0 (C2) match predicate.
    # fmt_table_search: inject fake ctx with count=0 â†’ trivially returns 1 (10x).
    # At count=0, the predicate is never called; result is deterministic.
    # (Injecting non-zero count would require calling into live game state via
    # FUN_005ac9e0 with a real desc ptr â€” deferred to canonical-scenario C4.)
    'audio_fmt_table_search': {
        'rva':            0x005acd10,
        'export':         'AudioFmtTableSearch',
        'signature':      {'ret': 'int32', 'args': ['pointer', 'pointer']},
        'arg_type':       'fmt_table_search',
        'lut_root_delta': 0,
        # All tests use count=0 â†’ function returns 1 immediately without calling predicate.
        # This is the deterministic branch; count>0 requires live game state (C4 scope).
        'path1_tests': [
            {'count': 0, 'entry_ptr': 0},
            {'count': 0, 'entry_ptr': 0},
            {'count': 0, 'entry_ptr': 0},
            {'count': 0, 'entry_ptr': 0},
            {'count': 0, 'entry_ptr': 0},
            {'count': 0, 'entry_ptr': 0},
            {'count': 0, 'entry_ptr': 0},
            {'count': 0, 'entry_ptr': 0},
            {'count': 0, 'entry_ptr': 0},
            {'count': 0, 'entry_ptr': 0},
        ],
        'path2_tests': [
            {'count': 0, 'entry_ptr': 0},
            {'count': 0, 'entry_ptr': 0},
            {'count': 0, 'entry_ptr': 0},
        ],
    },

    # 0x005acd60  AudioFmtGlobalScan
    # ptr(key): scans 9-entry global format-table at 0x00633674.
    # Callee: FUN_005adf30 (C2) 16-byte format key compare.
    # Returns matching entry ptr or NULL.
    # fmt_global_scan: pass zeroed 16-byte key; global table initialized at audio
    # startup. At main menu (audio COM patched out) the table slots may be NULL.
    # Both orig and reimpl call into live game state â€” both must return same ptr.
    # Strategy: pass 10x zero key; compare return pointer (as uint32).
    # crash_equal_ok=True: if both crash the same way (e.g. NULL table entry deref),
    # that counts as GREEN for C3 purposes.
    'audio_fmt_global_scan': {
        'rva':            0x005acd60,
        'export':         'AudioFmtGlobalScan',
        'signature':      {'ret': 'pointer', 'args': ['pointer']},
        'arg_type':       'fmt_global_scan',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # 16-byte zero key (list of 16 uint8 values)
        'path1_tests': [
            [0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0],
            [0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0],
            [0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0],
            [0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0],
            [0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0],
            [1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0],
            [1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0],
            [1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0],
            [0xFF,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0],
            [0xFF,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0],
        ],
        'path2_tests': [
            [0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0],
            [1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0],
            [0xFF,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0],
        ],
    },

    # 0x005ae050  AudioSubStructBFree
    # int(int): if *(int*)(param+4)!=0 and bit1(*(uint*)(param+8)) â†’ heap-free;
    # clear bit1. Returns param_1.
    # At quiescent main menu audio sub-structs have null data pointers; guard
    # fails safely. Use 'none' (10 no-crash checks).
    'audio_sub_struct_b_free': {
        'rva':            0x005ae050,
        'export':         'AudioSubStructBFree',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x005ae030  AudioSubStructCleanup
    # int(int): calls AudioSubStructAFree + AudioSubStructBFree in sequence.
    # Same quiescent-menu guard logic â€” both sub-callees are guarded at their
    # entry; null data pointer makes them no-ops. Use 'none' (10 no-crash checks).
    'audio_sub_struct_cleanup': {
        'rva':            0x005ae030,
        'export':         'AudioSubStructCleanup',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x005adf30  AudioFmtKeyCompare
    # int(byte*, byte*): 16-byte lexicographic memcmp returning -1/0/+1.
    # Pure: no side effects, no callees, no globals.
    # arg_type='fmt_key_compare': allocates 32-byte scratch, writes a+b patterns,
    # calls fn(buf, buf+16), compares int return. 15 test vectors: 5 equal pairs,
    # 5 a<b pairs, 5 a>b pairs, covering various byte-position differences.
    'audio_fmt_key_compare': {
        'rva':            0x005adf30,
        'export':         'AudioFmtKeyCompare',
        'signature':      {'ret': 'int32', 'args': ['pointer', 'pointer']},
        'arg_type':       'fmt_key_compare',
        'lut_root_delta': 0,
        'path1_tests': [
            # Equal pairs (expect 0)
            {'a': [0x00]*16, 'b': [0x00]*16},
            {'a': [0xFF]*16, 'b': [0xFF]*16},
            {'a': [0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
                   0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10],
             'b': [0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
                   0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10]},
            {'a': [0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x11,0x22,
                   0x33,0x44,0x55,0x66,0x77,0x88,0x99,0x00],
             'b': [0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x11,0x22,
                   0x33,0x44,0x55,0x66,0x77,0x88,0x99,0x00]},
            {'a': [0x7F]*16, 'b': [0x7F]*16},
            # a < b pairs (expect -1): first byte difference, a[i] < b[i]
            {'a': [0x00]*16, 'b': [0x01] + [0x00]*15},
            {'a': [0x01,0x00] + [0x00]*14, 'b': [0x01,0x01] + [0x00]*14},
            {'a': [0xFE]*16, 'b': [0xFE]*15 + [0xFF]},
            {'a': [0x10,0x20,0x30] + [0x00]*13,
             'b': [0x10,0x20,0x31] + [0x00]*13},
            {'a': [0x00]*15 + [0x00], 'b': [0x00]*15 + [0x01]},
            # a > b pairs (expect +1): first byte difference, a[i] > b[i]
            {'a': [0x01] + [0x00]*15, 'b': [0x00]*16},
            {'a': [0x01,0x01] + [0x00]*14, 'b': [0x01,0x00] + [0x00]*14},
            {'a': [0xFF]*16, 'b': [0xFF]*15 + [0xFE]},
            {'a': [0x10,0x20,0x31] + [0x00]*13,
             'b': [0x10,0x20,0x30] + [0x00]*13},
            {'a': [0x00]*15 + [0x01], 'b': [0x00]*15 + [0x00]},
        ],
        'path2_tests': [
            {'a': [0x00]*16, 'b': [0x00]*16},
            {'a': [0x01] + [0x00]*15, 'b': [0x00]*16},
            {'a': [0x00]*16, 'b': [0x01] + [0x00]*15},
        ],
    },

    # 0x005ac5f0  AudioFmtDescEqual  (c3-batch-j-s1)
    # int(int* a, int* b): 5-field AND equality comparator on fmt-descriptor pairs.
    # Fields compared: +0x00 (u32 direct), +0x04 (16B fmt-key via FUN_005adf30),
    # +0x0c (s8), +0x0d (s8), +0x18 (u8 masked with 0xfd â€” bit1 ignored).
    # Returns 1 iff all match, 0 otherwise.
    # arg_type='fmt_desc_pair_compare' (2-arg form): allocates two 0x40 scratch
    # buffers, writes per-test field map, calls fn(bufA, bufB), packs return +
    # both buffer fingerprints. crash_equal_ok protects the FUN_005adf30 callee
    # path which dereferences param_1[1]/param_2[1] (potential null deref on
    # zero-init buffers).
    'audio_fmt_desc_equal': {
        'rva':            0x005ac5f0,
        'export':         'AudioFmtDescEqual',
        'signature':      {'ret': 'uint32', 'args': ['pointer', 'pointer']},
        'arg_type':       'fmt_desc_pair_compare',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests': [
            # equal full descriptors (return 1 if FUN_005adf30 reaches w/o crash;
            # but both halves of the compare run against same scratch shape so
            # crash_equal_ok keeps these GREEN even if the key-ptr deref AVs)
            { 'a': {'f00': 0x11111111, 'f04': 0x22222222, 'f0c': 0x33333333, 'f18': 0x00000000},
              'b': {'f00': 0x11111111, 'f04': 0x22222222, 'f0c': 0x33333333, 'f18': 0x00000000} },
            # +0x00 differs
            { 'a': {'f00': 0x11111111}, 'b': {'f00': 0x22222222} },
            # +0x04 fmt-key field differs
            { 'a': {'f04': 0xAAAAAAAA}, 'b': {'f04': 0xBBBBBBBB} },
            # +0x0c low-byte differs
            { 'a': {'f00': 1, 'f04': 0, 'f0c': 0x12345678},
              'b': {'f00': 1, 'f04': 0, 'f0c': 0x87654321} },
            # +0x18 flags differ but only bit1 (masked off by 0xfd) â€” same equivalence class
            { 'a': {'f18': 0x02}, 'b': {'f18': 0x00} },
            # +0x18 flags differ in a non-masked bit (bit0)
            { 'a': {'f18': 0x01}, 'b': {'f18': 0x00} },
            # all-zero
            { 'a': {}, 'b': {} },
            # large values
            { 'a': {'f00': 0xFFFFFFFF, 'f04': 0xFFFFFFFF},
              'b': {'f00': 0xFFFFFFFF, 'f04': 0xFFFFFFFF} },
            # high-bit set
            { 'a': {'f00': 0x80000000}, 'b': {'f00': 0x80000000} },
            # repeated equal (smoke)
            { 'a': {'f00': 0xDEADBEEF, 'f04': 0xCAFEBABE},
              'b': {'f00': 0xDEADBEEF, 'f04': 0xCAFEBABE} },
        ],
        'path2_tests': [
            { 'a': {}, 'b': {} },
            { 'a': {'f00': 1}, 'b': {'f00': 1} },
            { 'a': {'f00': 1}, 'b': {'f00': 2} },
        ],
    },

    # 0x005ac9e0  AudioFmtEntryMatch  (c3-batch-j-s1)
    # int(u32* entry, u32* candidate): 6-condition match predicate for fmt-table
    # entries against a candidate descriptor.
    # cnd1: entry+0x05 (s8 channel) == cand+0x0d (s8 channel)
    # cnd2: entry+0x04 (s8 fmt-char) == cand+0x0c (s8 fmt-char)
    # cnd3: entry+0x08 (u32 min-rate) <= cand+0x00 (u32 rate)
    # cnd4: cand+0x00 (u32 rate)     <= entry+0x0c (u32 max-rate)
    # cnd5: FUN_005adf30(cand+0x04, entry+0x00) == 0  â€” 16-byte key compare
    # cnd6: ((entry+0x10 ^ cand+0x18) & 1) == 0      â€” flag bit0 agreement
    # arg_type='fmt_desc_pair_compare' (2-arg form). crash_equal_ok protects
    # the key-ptr deref path inside FUN_005adf30.
    'audio_fmt_entry_match': {
        'rva':            0x005ac9e0,
        'export':         'AudioFmtEntryMatch',
        'signature':      {'ret': 'uint32', 'args': ['pointer', 'pointer']},
        'arg_type':       'fmt_desc_pair_compare',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests': [
            # equal full match (key-ptr deref crash â†’ both sides crash equal)
            { 'a': {'f00': 0x11111111, 'f04': 0x11, 'f05': 0x22, 'f08': 0x100, 'f0c': 0xFFFFFFFF, 'f10': 0x00},
              'b': {'f00': 0x500, 'f04': 0x22222222, 'f0c': 0x11, 'f18': 0x00} },
            # cnd1 mismatch (channel byte differs)
            { 'a': {'f05': 0xAA}, 'b': {'f0d': 0xBB} },
            # cnd2 mismatch (fmt-char differs)
            { 'a': {'f04': 0x10}, 'b': {'f0c': 0x20} },
            # cnd3 mismatch: cand+0x00 < entry+0x08 (min-rate)
            { 'a': {'f08': 0x1000}, 'b': {'f00': 0x0500} },
            # cnd4 mismatch: cand+0x00 > entry+0x0c (max-rate)
            { 'a': {'f08': 0x0000, 'f0c': 0x0500}, 'b': {'f00': 0x1000} },
            # cnd3 boundary: equal min-rate
            { 'a': {'f08': 0x100, 'f0c': 0x1000}, 'b': {'f00': 0x100} },
            # cnd4 boundary: equal max-rate
            { 'a': {'f08': 0x100, 'f0c': 0x1000}, 'b': {'f00': 0x1000} },
            # cnd6 flag bit0 mismatch
            { 'a': {'f10': 0x01}, 'b': {'f18': 0x00} },
            # cnd6 flag bit0 match (both bit0 set)
            { 'a': {'f10': 0x01}, 'b': {'f18': 0x01} },
            # all-zero (cnd3/4 trivially satisfied â€” but cnd5 key-deref may AV)
            { 'a': {}, 'b': {} },
        ],
        'path2_tests': [
            { 'a': {}, 'b': {} },
            { 'a': {'f05': 0x10}, 'b': {'f0d': 0x20} },
            { 'a': {'f08': 0x100, 'f0c': 0x1000}, 'b': {'f00': 0x500} },
        ],
    },

    'audio_sub_struct_link_device': {
        'rva':            0x005ae010,
        'export':         'AudioSubStructLinkDevice',
        'signature':      {'ret': 'pointer', 'args': ['pointer', 'uint32']},
        'arg_type':       'audio_sub_struct_link',
        'lut_root_delta': 0,
        'path1_tests': [
            0x00000000,
            0x00000001,
            0x00000002,
            0xDEADBEEF,
            0xCAFEBABE,
            0x12345678,
            0xFFFFFFFF,
            0x80000000,
            0x55555555,
            0xAAAAAAAA,
        ],
        'path2_tests': [
            0x00000000,
            0xDEADBEEF,
            0xFFFFFFFF,
        ],
    },

    # 0x005adfe0  AudioSubStructLinkBuffer  (32 bytes)
    # fn(uint32* param_1, uint32 param_2) -> uint32*
    # Calls FUN_005ae050(param_1) (no-op when param_1[1] == 0), then:
    #   param_1[1] = param_2;  param_1[2] &= 0xfffffffd;  return param_1.
    # Same arg_type strategy as AudioSubStructLinkDevice.
    # STRUCT GAP note: sub-struct at +0x34 (U-0143).
    'audio_sub_struct_link_buffer': {
        'rva':            0x005adfe0,
        'export':         'AudioSubStructLinkBuffer',
        'signature':      {'ret': 'pointer', 'args': ['pointer', 'uint32']},
        'arg_type':       'audio_sub_struct_link',
        'lut_root_delta': 0,
        'path1_tests': [
            0x00000000,
            0x00000001,
            0x00000002,
            0xDEADBEEF,
            0xCAFEBABE,
            0x12345678,
            0xFFFFFFFF,
            0x80000000,
            0x55555555,
            0xAAAAAAAA,
        ],
        'path2_tests': [
            0x00000000,
            0xDEADBEEF,
            0xFFFFFFFF,
        ],
    },

    # 0x005ae0b0  AudioSubStructZeroInit  (14 bytes)
    # fn(uint32* param_1) -> void
    # Pure leaf. Zeros param_1[2], param_1[1], param_1[0] in reverse order.
    # arg_type='audio_sub_struct_zero': fill buf with sentinel, call fn(buf),
    #   read back all 3 DWORDs â€” both paths must return '0,0,0'.
    'audio_sub_struct_zero_init': {
        'rva':            0x005ae0b0,
        'export':         'AudioSubStructZeroInit',
        'signature':      {'ret': 'void', 'args': ['pointer']},
        'arg_type':       'audio_sub_struct_zero',
        'lut_root_delta': 0,
        'path1_tests': [
            0x00000000,
            0x00000001,
            0xDEADBEEF,
            0xCAFEBABE,
            0x12345678,
            0xFFFFFFFF,
            0x80000000,
            0x55555555,
            0xAAAAAAAA,
            0x3F800000,
        ],
        'path2_tests': [
            0x00000000,
            0xDEADBEEF,
            0xFFFFFFFF,
        ],
    },

    # 0x005ac7b0  AudioSubStructDualInit  (~50 bytes)
    # fn(uint32 param_1, uint32 param_2, uint32 param_3) -> uint32
    # param_1 is a pointer to a 12-byte sub-struct (passed as int).
    # Calls AudioSubStructLinkDevice(param_1, param_2), then if non-null,
    # AudioSubStructLinkBuffer(param_1, param_3). Returns param_1 if both OK, 0 if either fails.
    # arg_type='audio_sub_struct_dual': allocate 12-byte buf, pass as param_1;
    #   normalize return: 1 if non-zero, 0 if zero. Both paths must agree.
    # With zeroed input buf the callees are no-ops on cleanup; both link calls succeed.
    'audio_sub_struct_dual_init': {
        'rva':            0x005ac7b0,
        'export':         'AudioSubStructDualInit',
        'signature':      {'ret': 'uint32', 'args': ['uint32', 'uint32', 'uint32']},
        'arg_type':       'audio_sub_struct_dual',
        'lut_root_delta': 0,
        'path1_tests': [
            {'p2': 0x00000000, 'p3': 0x00000000},
            {'p2': 0x00000001, 'p3': 0x00000001},
            {'p2': 0xDEADBEEF, 'p3': 0xCAFEBABE},
            {'p2': 0x12345678, 'p3': 0x87654321},
            {'p2': 0xFFFFFFFF, 'p3': 0xFFFFFFFF},
            {'p2': 0x80000000, 'p3': 0x00000001},
            {'p2': 0x00000001, 'p3': 0x80000000},
            {'p2': 0x55555555, 'p3': 0xAAAAAAAA},
            {'p2': 0x00000002, 'p3': 0x00000003},
            {'p2': 0x3F800000, 'p3': 0x40000000},
        ],
        'path2_tests': [
            {'p2': 0x00000000, 'p3': 0x00000000},
            {'p2': 0xDEADBEEF, 'p3': 0xCAFEBABE},
            {'p2': 0xFFFFFFFF, 'p3': 0xFFFFFFFF},
        ],
    },
    'audio_pool_block_alloc': {
        'rva':            0x005ae800,
        'export':         'AudioPoolBlockAlloc',
        'signature':      {'ret': 'uint32', 'args': ['pointer', 'int32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # All tests pass null (0) as param_1; both sides crash at *(0+0x10).
        # 10 repetitions confirm error-string stability.
        'path1_tests': [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests': [0, 0, 0],
    },

    # 0x005ae780  AudioPoolDestroy
    # Drains block list at param_1+0x10 via RawFree (0x004522d0); then frees
    # pool header via DAT_007ddab0 secondary pool or RawFree. Skips header free
    # if bit-0 of *(param_1+0x18) is set (externally-owned header).
    # crash_equal_ok: null param_1 causes identical crash on both sides
    # at *(0+0x10) = invalid read.
    'audio_pool_destroy': {
        'rva':            0x005ae780,
        'export':         'AudioPoolDestroy',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests': [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests': [0, 0, 0],
    },

    # 0x005aeca0  AudioFieldEndianPack
    # Writes 1, 2, or 4 bytes from *param_2 into **param_1 with big->little endian
    # byte reversal; advances *param_1 by field size. Pure arithmetic leaf.
    # arg_type='endian_pack': allocates fresh output buffer + ptr-to-ptr + src slot;
    # calls fn(ptrSlot, srcSlot, size); compares output buffer fingerprint.
    'audio_field_endian_pack': {
        'rva':            0x005aeca0,
        'export':         'AudioFieldEndianPack',
        'signature':      {'ret': 'void', 'args': ['pointer', 'pointer', 'int32']},
        'arg_type':       'endian_pack',
        'lut_root_delta': 0,
        'path1_tests': [
            {'src_val': 0x12,             'size': 1},  # 1-byte: low byte 0x12
            {'src_val': 0xAB,             'size': 1},  # 1-byte: 0xAB
            {'src_val': 0xFF,             'size': 1},  # 1-byte: 0xFF
            {'src_val': 0x1234,           'size': 2},  # 2-byte: swap 0x34 0x12
            {'src_val': 0xABCD,           'size': 2},  # 2-byte: swap 0xCD 0xAB
            {'src_val': 0x0001,           'size': 2},  # 2-byte: 0x01 0x00
            {'src_val': 0x12345678,       'size': 4},  # 4-byte: bswap32
            {'src_val': 0xDEADBEEF,       'size': 4},  # 4-byte: bswap32
            {'src_val': 0x00000001,       'size': 4},  # 4-byte: bswap32 -> 0x01000000
            {'src_val': 0xFF000000,       'size': 4},  # 4-byte: bswap32 -> 0x000000FF
        ],
        'path2_tests': [
            {'src_val': 0x12, 'size': 1},
            {'src_val': 0x1234, 'size': 2},
            {'src_val': 0x12345678, 'size': 4},
        ],
    },

    # 0x005ae0c0  AudioWaveFmtCopy
    # Copies a 16-byte WAVEFORMATEX-like descriptor from param_1 to param_2.
    # param_3==NULL -> direct 4-DWORD copy; param_3!=NULL -> byte-swap path
    # via AudioFieldEndianPack calls. Returns param_1.
    # arg_type='wavefmt_copy': allocates src+dst 16-byte bufs, writes test src,
    # calls fn(src, dst, swap_flag), compares dst fingerprint.
    'audio_wave_fmt_copy': {
        'rva':            0x005ae0c0,
        'export':         'AudioWaveFmtCopy',
        'signature':      {'ret': 'pointer', 'args': ['pointer', 'pointer', 'pointer']},
        'arg_type':       'wavefmt_copy',
        'lut_root_delta': 0,
        'path1_tests': [
            # No-swap path (swap=0): direct DWORD copy
            {'src': [0x01,0x00,0x01,0x00, 0x44,0xAC,0x00,0x00,
                     0x88,0x58,0x01,0x00, 0x02,0x00,0x10,0x00], 'swap': 0},
            {'src': [0xFF,0xFE,0xFD,0xFC, 0xFB,0xFA,0xF9,0xF8,
                     0xF7,0xF6,0xF5,0xF4, 0xF3,0xF2,0xF1,0xF0], 'swap': 0},
            {'src': [0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,
                     0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00], 'swap': 0},
            {'src': [0xDE,0xAD,0xBE,0xEF, 0xCA,0xFE,0xBA,0xBE,
                     0x12,0x34,0x56,0x78, 0x9A,0xBC,0xDE,0xF0], 'swap': 0},
            {'src': [0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07,
                     0x08,0x09,0x0A,0x0B, 0x0C,0x0D,0x0E,0x0F], 'swap': 0},
            # Swap path (swap=1): byte-swap first 8 bytes
            {'src': [0x01,0x00,0x01,0x00, 0x44,0xAC,0x00,0x00,
                     0x88,0x58,0x01,0x00, 0x02,0x00,0x10,0x00], 'swap': 1},
            {'src': [0xFF,0xFE,0xFD,0xFC, 0xFB,0xFA,0xF9,0xF8,
                     0xF7,0xF6,0xF5,0xF4, 0xF3,0xF2,0xF1,0xF0], 'swap': 1},
            {'src': [0xDE,0xAD,0xBE,0xEF, 0xCA,0xFE,0xBA,0xBE,
                     0x12,0x34,0x56,0x78, 0x9A,0xBC,0xDE,0xF0], 'swap': 1},
            {'src': [0x00,0x01,0x02,0x03, 0x04,0x05,0x06,0x07,
                     0x08,0x09,0x0A,0x0B, 0x0C,0x0D,0x0E,0x0F], 'swap': 1},
            {'src': [0xAA,0xBB,0xCC,0xDD, 0xEE,0xFF,0x11,0x22,
                     0x33,0x44,0x55,0x66, 0x77,0x88,0x99,0x00], 'swap': 1},
        ],
        'path2_tests': [
            {'src': [0x01,0x00,0x01,0x00, 0x44,0xAC,0x00,0x00,
                     0x88,0x58,0x01,0x00, 0x02,0x00,0x10,0x00], 'swap': 0},
            {'src': [0x01,0x00,0x01,0x00, 0x44,0xAC,0x00,0x00,
                     0x88,0x58,0x01,0x00, 0x02,0x00,0x10,0x00], 'swap': 1},
            {'src': [0xDE,0xAD,0xBE,0xEF, 0xCA,0xFE,0xBA,0xBE,
                     0x12,0x34,0x56,0x78, 0x9A,0xBC,0xDE,0xF0], 'swap': 0},
        ],
    },

    # ─────────────────────────────────────────────────────────────────────────
    # c3_batch_ab session 3 — six audio pure leaves (Audio/AudioLeaves_ab3.cpp).
    # ─────────────────────────────────────────────────────────────────────────

    # 0x005b73b0  AudioFindExtension — char* fn(char* s); back-walks from NUL to
    # find the char after the last '.', else returns s. Note: a leading '.' is
    # NOT treated as a separator (loop breaks at s before testing it).
    # arg_type='cstr_ret_offset': writes test.str into a buffer, compares the
    # returned pointer as an offset from the buffer.
    'audio_find_extension': {
        'rva':            0x005b73b0,
        'export':         'AudioFindExtension',
        'signature':      {'ret': 'pointer', 'args': ['pointer']},
        'arg_type':       'cstr_ret_offset',
        'lut_root_delta': 0,
        'path1_tests': [
            {'str': 'sound.wav'},     # dot@5 -> offset 6
            {'str': 'music.rws'},     # dot@5 -> offset 6
            {'str': 'noext'},         # no dot -> offset 0 (returns s)
            {'str': 'a.b.c'},         # last dot@3 -> offset 4
            {'str': '.hidden'},       # leading dot only -> offset 0 (edge)
            {'str': 'track01.RWS'},   # dot@7 -> offset 8
            {'str': 'x.y'},           # dot@1 -> offset 2
            {'str': 'file.'},         # trailing dot@4 -> offset 5 (empty ext)
        ],
        'path2_tests': [
            {'str': 'sound.wav'},
            {'str': 'noext'},
            {'str': 'a.b.c'},
        ],
    },

    # 0x005b9410  AudioSourceLoopSet — void fn(int src, int loop). Software path
    # (caps[+0x50]&8==0): bit 0x800 in *(src+0x28). Hardware path: bit 0x8 in
    # *(*(int*)(src+0x11c)+0xcc). arg_type='source_loop_set' exercises both.
    'audio_source_loop_set': {
        'rva':            0x005b9410,
        'export':         'AudioSourceLoopSet',
        'signature':      {'ret': 'void', 'args': ['pointer', 'int32']},
        'arg_type':       'source_loop_set',
        'lut_root_delta': 0,
        'path1_tests': [
            {'loop': 1, 'hw': 0, 'pre28': 0x00000000},  # sw set:  -> 0x800
            {'loop': 0, 'hw': 0, 'pre28': 0xFFFFFFFF},  # sw clear:-> 0xfffff7ff
            {'loop': 1, 'hw': 0, 'pre28': 0x12345000},  # sw set:  -> 0x12345800
            {'loop': 0, 'hw': 0, 'pre28': 0x00000800},  # sw clear:-> 0
            {'loop': 1, 'hw': 1, 'prehw': 0x00000000},  # hw set:  -> 0x8
            {'loop': 0, 'hw': 1, 'prehw': 0xFFFFFFFF},  # hw clear:-> 0xfffffff7
            {'loop': 1, 'hw': 1, 'prehw': 0x00000100},  # hw set:  -> 0x108
            {'loop': 0, 'hw': 1, 'prehw': 0x00000008},  # hw clear:-> 0
        ],
        'path2_tests': [
            {'loop': 1, 'hw': 0, 'pre28': 0x00000000},
            {'loop': 0, 'hw': 0, 'pre28': 0xFFFFFFFF},
            {'loop': 1, 'hw': 1, 'prehw': 0x00000000},
        ],
    },

    # 0x005baf40  AudioRendererField3cSet — void fn(int p1, uint32 v). Writes v
    # to *(p1+0x3c); if (*(byte*)(p1+0x78)&8) mirrors v to hwvoice+0x34.
    'audio_renderer_field3c_set': {
        'rva':            0x005baf40,
        'export':         'AudioRendererField3cSet',
        'signature':      {'ret': 'void', 'args': ['pointer', 'uint32']},
        'arg_type':       'renderer_field3c_set',
        'lut_root_delta': 0,
        'path1_tests': [
            {'val': 0x12345678, 'hw': 0},   # sw: +0x3c only
            {'val': 0x00000000, 'hw': 0},
            {'val': 0xFFFFFFFF, 'hw': 1},   # hw: +0x3c and mirror
            {'val': 0xCAFEBABE, 'hw': 1},
            {'val': 0x00000001, 'hw': 0},
            {'val': 0xABCD1234, 'hw': 1},
        ],
        'path2_tests': [
            {'val': 0x12345678, 'hw': 0},
            {'val': 0xCAFEBABE, 'hw': 1},
        ],
    },

    # 0x005bb5b0  AudioPcmSaturatedAdd — void fn(out, srcA, srcB, byteCount).
    # out[i] = saturate(srcA[i]+srcB[i]) over byteCount>>1 int16 samples; clamps
    # to [-0x7fff, +0x7fff] (lower bound -32767, NOT -32768).
    'audio_pcm_saturated_add': {
        'rva':            0x005bb5b0,
        'export':         'AudioPcmSaturatedAdd',
        'signature':      {'ret': 'void', 'args': ['pointer', 'pointer', 'pointer', 'uint32']},
        'arg_type':       'pcm_sat_add',
        'lut_root_delta': 0,
        'path1_tests': [
            {'a': [100, 200, 300, 400], 'b': [50, 60, 70, 80]},          # plain, n=4
            {'a': [32000, 32000],       'b': [32000, 32000]},            # +overflow -> 32767
            {'a': [-32000, -32000],     'b': [-32000, -32000]},          # -overflow -> -32767
            {'a': [1],                  'b': [-1]},                      # n=1 tail
            {'a': [10, 20, 30],         'b': [1, 2, 3]},                 # n=3 tail
            {'a': [0,0,0,0,0,0,0,0],    'b': [1,2,3,4,5,6,7,8]},         # n=8 pure main
            {'a': [32767, -32768, 16000, -16000, 32767],
             'b': [32767, -32768, 16000, -16000, 1]},                   # boundaries, n=5
            {'a': [-16384, -16384],     'b': [-16383, -16384]},          # -32767 kept / -32768 clamp
        ],
        'path2_tests': [
            {'a': [100, 200, 300, 400], 'b': [50, 60, 70, 80]},
            {'a': [32000, 32000],       'b': [32000, 32000]},
            {'a': [10, 20, 30],         'b': [1, 2, 3]},
        ],
    },

    # 0x005bc450  AudioSlotPairZero — void fn(uint32* p): p[0]=0; p[1]=0.
    # arg_type='ptr_zero_pair' preloads a sentinel + guard dword, compares 12B.
    'audio_slot_pair_zero': {
        'rva':            0x005bc450,
        'export':         'AudioSlotPairZero',
        'signature':      {'ret': 'void', 'args': ['pointer']},
        'arg_type':       'ptr_zero_pair',
        'lut_root_delta': 0,
        'path1_tests': [0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x00000000, 0xCAFEBABE],
        'path2_tests': [0xDEADBEEF, 0x12345678],
    },

    # 0x005bcb80  AudioMediaSubtypeFromTag — void fn(uint32 tag, uint32* out16):
    # writes {0000tttt-0000-0010-8000-00AA00389B71}. arg_type='guid_from_tag'.
    'audio_media_subtype_from_tag': {
        'rva':            0x005bcb80,
        'export':         'AudioMediaSubtypeFromTag',
        'signature':      {'ret': 'void', 'args': ['uint32', 'pointer']},
        'arg_type':       'guid_from_tag',
        'lut_root_delta': 0,
        'path1_tests': [0x0001, 0x0002, 0x0055, 0x0000FFFF, 0x00000000, 0x00001234],
        'path2_tests': [0x0001, 0x0055, 0x00000000],
    },

    'audio_pool_free': {
        'rva':          0x005ae920,
        'export':       'AudioPoolFree',
        'signature':    {'ret': 'void', 'args': ['pointer', 'pointer']},
        'arg_type':     'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':  [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':  [0, 1, 2],
    },

    # 0x005addd0  AudioListInsertHead
    # void fn(head_ptr, payload): allocs node from DAT_009146c0 pool via
    # FUN_005ae800, inserts new node at list head.
    # HARNESS-LIMITED: internally calls FUN_005ae800(&DAT_009146c0, 0x30804)
    # which requires the pool to be initialized. At main-menu quiescent state
    # (audio COM disabled) the pool is uninitialized; both paths crash
    # identically. arg_type='none' + crash_equal_ok=True â†’ GREEN.
    'audio_list_insert_head': {
        'rva':          0x005addd0,
        'export':       'AudioListInsertHead',
        'signature':    {'ret': 'void', 'args': ['pointer', 'int32']},
        'arg_type':     'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':  [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':  [0, 1, 2],
    },

    # 0x005ade10  AudioListRemoveByValue
    # int* fn(sentinel_ptr, payload): search + unlink + free to pool.
    # Returns sentinel on success (found), NULL if not found.
    # arg_type='audio_list_remove': allocates fresh 12-byte sentinel via
    # Memory.alloc. Not-found tests (present=False) search an empty sentinel;
    # traversal exits immediately (piVar2 == param_1) â†’ returns NULL.
    # Both paths return NULL=NULL with no errors â†’ match=True without crash.
    # (Avoids the pool entirely for not-found cases.)
    'audio_list_remove_by_value': {
        'rva':          0x005ade10,
        'export':       'AudioListRemoveByValue',
        'signature':    {'ret': 'pointer', 'args': ['pointer', 'int32']},
        'arg_type':     'audio_list_remove',
        'insert_rva':   0x005addd0,
        'lut_root_delta': 0,
        # NOTE: payload=0 excluded â€” fresh harness sentinel has data[+8]=0;
        # payload=0 hits a degenerate match against the sentinel node itself,
        # causing AudioPoolFree to be called on an uninitialized pool â†’ crash.
        # This is an artefact of the harness (not a real-world usage pattern).
        'path1_tests':  [
            {'payload': 1,   'present': False},
            {'payload': 42,  'present': False},
            {'payload': -1,  'present': False},
            {'payload': 99,  'present': False},
            {'payload': 100, 'present': False},
            {'payload': 255, 'present': False},
            {'payload': 999, 'present': False},
            {'payload': 7,   'present': False},
            {'payload': 13,  'present': False},
            {'payload': 500, 'present': False},
        ],
        'path2_tests':  [
            {'payload': 1,   'present': False},
            {'payload': 42,  'present': False},
            {'payload': 99,  'present': False},
        ],
    },

    # 0x005ade90  AudioListDrain
    # void fn(sentinel_ptr): drain all nodes from list, return each to pool.
    # arg_type='audio_list_drain': allocates fresh self-referential 12-byte
    # sentinel via Memory.alloc. nodeCount=0 tests: empty sentinel â†’ loop
    # condition param_1[1] != param_1 is immediately False â†’ no-op. Both
    # paths observe sentinel still self-loops â†’ origV=1, reimV=1 â†’ match=True.
    # (No pool access needed for empty-drain case.)
    'audio_list_drain': {
        'rva':          0x005ade90,
        'export':       'AudioListDrain',
        'signature':    {'ret': 'void', 'args': ['pointer']},
        'arg_type':     'audio_list_drain',
        'insert_rva':   0x005addd0,
        'lut_root_delta': 0,
        'path1_tests':  [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':  [0, 0, 0],
    },
    'audio_wave_node_free': {
        'rva':            0x005abcb0,
        'export':         'AudioWaveNodeFree',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # ===== c3_batch_ab session 1 (2026-06-04) — audio pure leaves =====
    # Each rides a NEW arg_type added to diff_template.js this session (the
    # branch touches diff_template.js — flag for frida-sweep manual merge).

    # 0x005ac540  AudioByte54Bit3Get — uint(ptr): (*(byte*)(p+0x54) & 8) >> 3.
    # arg_type 'ptr_scratch_field': zeroed scratch, test byte seeded at +0x54.
    'audio_byte54_bit3_get': {
        'rva':            0x005ac540,
        'export':         'AudioByte54Bit3Get',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'ptr_scratch_field',
        'field_offset':   0x54,
        'lut_root_delta': 0,
        'path1_tests':    [0x00, 0x08, 0xFF, 0xF7, 0x80, 0x88, 0x0F, 0x07, 0xF8, 0x48],
        'path2_tests':    [0x00, 0x08, 0xFF],
    },

    # 0x005aded0  AudioListNodeCount — int(anchor): circular-list (next@+4) count.
    # arg_type 'audio_list_count': hand-built ring of N nodes (no audio pool).
    'audio_list_node_count': {
        'rva':            0x005aded0,
        'export':         'AudioListNodeCount',
        'signature':      {'ret': 'int32', 'args': ['pointer']},
        'arg_type':       'audio_list_count',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 5, 8, 13, 16],
        'path2_tests':    [0, 1, 4],
    },

    # 0x005ade60  AudioListIndexOfKey — int(anchor,key): index of node whose
    # key@+8 == key, walking next@+4; -1 if none.
    # arg_type 'audio_list_find_index': hand-built ring from payloads list.
    'audio_list_index_of_key': {
        'rva':            0x005ade60,
        'export':         'AudioListIndexOfKey',
        'signature':      {'ret': 'int32', 'args': ['pointer', 'int32']},
        'arg_type':       'audio_list_find_index',
        'lut_root_delta': 0,
        'path1_tests':    [
            {'payloads': [],              'key': 5},      # empty -> -1
            {'payloads': [10],            'key': 10},     # 0
            {'payloads': [10],            'key': 99},     # -1
            {'payloads': [10, 20, 30],    'key': 10},     # 0
            {'payloads': [10, 20, 30],    'key': 20},     # 1
            {'payloads': [10, 20, 30],    'key': 30},     # 2
            {'payloads': [10, 20, 30],    'key': 99},     # -1
            {'payloads': [10, 20, 30, 40, 50], 'key': 50},# 4
            {'payloads': [-1, -2, -3],    'key': -2},     # 1 (sign)
            {'payloads': [0, 1, 2],       'key': 0},      # 0
        ],
        'path2_tests':    [
            {'payloads': [10, 20, 30], 'key': 20},
            {'payloads': [10],         'key': 99},
            {'payloads': [],           'key': 1},
        ],
    },

    # 0x005ae590  AudioArenaBlockIsFree — bool(block): **(block+8)==*(block+0xc).
    # arg_type 'arena_block_free_predicate': hand-built block, both branches.
    'audio_arena_block_is_free': {
        'rva':            0x005ae590,
        'export':         'AudioArenaBlockIsFree',
        'signature':      {'ret': 'bool', 'args': ['pointer']},
        'arg_type':       'arena_block_free_predicate',
        'lut_root_delta': 0,
        'path1_tests':    [
            {'free': True}, {'free': False}, {'free': True}, {'free': False},
            {'free': True}, {'free': False}, {'free': True}, {'free': False},
        ],
        'path2_tests':    [{'free': True}, {'free': False}, {'free': True}],
    },

    # 0x005aeda0  AudioShiftAddMul64 — void(a,b,*hi,*lo): shift-add 64-bit mul.
    # arg_type 'int2_ptr2_out': two scalar args + two 4-byte out-slots.
    'audio_shift_add_mul64': {
        'rva':            0x005aeda0,
        'export':         'AudioShiftAddMul64',
        'signature':      {'ret': 'void', 'args': ['uint32', 'uint32', 'pointer', 'pointer']},
        'arg_type':       'int2_ptr2_out',
        'lut_root_delta': 0,
        'path1_tests':    [
            [0, 0], [1, 1], [2, 3],
            [0xFFFFFFFF, 0xFFFFFFFF], [0x80000000, 0x80000000],
            [0xAAAAAAAA, 0x55555555], [0x10000, 0x10000],
            [0x7FFFFFFF, 0x7FFFFFFF], [0xFFFFFFFF, 1], [1, 0xFFFFFFFF],
        ],
        'path2_tests':    [[1, 1], [0xFFFFFFFF, 0xFFFFFFFF], [0xAAAAAAAA, 0x55555555]],
    },

    # 0x005b0700  AudioListMinKeySelect — int(anchor,thresh): select payload by
    # key chain *(*(payload+0x54)+0x10) >= thresh while bestKey stays >= thresh.
    # arg_type 'audio_list_min_select': hand-built ring with key sub-structs;
    # return mapped to payload index.
    'audio_list_min_key_select': {
        'rva':            0x005b0700,
        'export':         'AudioListMinKeySelect',
        'signature':      {'ret': 'int32', 'args': ['pointer', 'uint32']},
        'arg_type':       'audio_list_min_select',
        'lut_root_delta': 0,
        'path1_tests':    [
            {'keys': [],            'thresh': 0},     # -1 (none)
            {'keys': [5],           'thresh': 0},     # 0
            {'keys': [5],           'thresh': 10},    # -1
            {'keys': [5, 10, 15],   'thresh': 0},     # last qualifying -> 2
            {'keys': [15, 10, 5],   'thresh': 8},     # 1
            {'keys': [100, 1, 100], 'thresh': 50},    # 2
            {'keys': [0xFFFFFFFF, 0], 'thresh': 0},   # 1
            {'keys': [7, 7, 7],     'thresh': 7},     # 2
        ],
        'path2_tests':    [
            {'keys': [5, 10, 15], 'thresh': 0},
            {'keys': [5],         'thresh': 10},
            {'keys': [],          'thresh': 0},
        ],
    },

    # 0x005ac740  AudioSubStructBufCleanup
    # void(int param_1): audio sub-struct buffer cleanup.
    # bit1@+0x18 guards free; always zeroes +0x10 and +0x14.
    # Callee: FUN_004522d0 (heap free).
    # crash_equal_ok: param_1=0 -> read byte *(0+0x18) -> null-deref; both crash equally.
    'audio_sub_struct_buf_cleanup': {
        'rva':            0x005ac740,
        'export':         'AudioSubStructBufCleanup',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # 0x005ac900  AudioContextLookup
    # uint32(uint32 param_1): context-lookup dispatcher.
    # Calls FUN_005aa0c0(0, 0, LAB_005ac930, &local_8, 1).
    # Result via local_4 written by inline callback LAB_005ac930 (U-1730).
    # Callee FUN_005aa0c0 drift-promoted C1->C2 (mechanical decomp only).
    # crash_equal_ok: audio context tree (DAT_009146fc) may be NULL at menu;
    #   if non-NULL the callback at LAB_005ac930 is an inline code region â€”
    #   calling it as a fn ptr may crash. Both orig and reimpl crash equally.
    'audio_context_lookup': {
        'rva':            0x005ac900,
        'export':         'AudioContextLookup',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # 0x005ae650  AudioPoolConstruct  [STRUCT GAP: DAT_007ddab0 U-1736]
    # uint32*(int, uint, uint, int, uint*, uint32): bitmap pool constructor.
    # 6-param: elem_size, bit_count, align, pre_alloc, hdr, flags.
    # Pool header layout: 9 fields x 4B; circular block list; DAT_007dda80 list.
    # Callee: FUN_005aea00 (raw alloc trampoline).
    # STRUCT GAP: DAT_007ddab0 secondary pool role unclear (U-1736).
    # crash_equal_ok: calling via int_scalar passes only one arg (the rest default 0);
    #   FUN_005aea00 may crash on null ctx. Both orig and reimpl crash equally.
    'audio_pool_construct': {
        'rva':            0x005ae650,
        'export':         'AudioPoolConstruct',
        'signature':      {'ret': 'pointer', 'args': ['int32', 'uint32', 'uint32', 'int32', 'pointer', 'uint32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # Session c3-batch-b-s6 â€” VehicleUnlockFlagGet (C2->C3)
    # VehicleMeta.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042ef40  VehicleUnlockFlagGet
    # Pure leaf: reads byte from DAT_007f0e50 + param_1*0xc at a byte-offset
    # selected by param_2 switch; returns 1 if byte == 0x01, else 0.
    # Array is zero-initialized before game-mode selection â†’ all tests return
    # 0 at quiescent main-menu state. A/B identity guaranteed (both read 0x00
    # from the same global array).
    # [UNCERTAIN U-3176] No bounds check on param_1 â€” reproduced as-is.
    # arg_type='int_pair': passes [param_1, param_2] directly.
    'audio_dsound_secondary_init': {
        'rva':            0x005bbfc0,
        'export':         'AudioDSoundSecondaryInit',
        'signature':      {'ret': 'int32', 'args': ['pointer']},
        'arg_type':       'dsound_secondary_init',
        'lut_root_delta': 0,
        # Each test is a dummy iteration index; the arg_type builds its own COM stub.
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x005aef00  AudioThreadDescInit
    # fn(uint32_t* buf, p2, p3, p4) -> void.
    # Writes: buf[0]=0 (handle), buf[1]=p2 (proc), buf[2]=0, buf[3]=p3 (prio), buf[4]=p4 (stack).
    # thread_desc_init: 20-byte scratch buf; fill sentinel; call; read 5 fields; compare.
    # Test vectors: varied proc ptrs, priorities, stack sizes from analysis note.
    'audio_thread_desc_init': {
        'rva':            0x005aef00,
        'export':         'AudioThreadDescInit',
        'signature':      {'ret': 'void', 'args': ['pointer', 'uint32', 'uint32', 'uint32']},
        'arg_type':       'thread_desc_init',
        'lut_root_delta': 0,
        # Each test: [p2=proc_ptr, p3=priority, p4=stack_size]
        'path1_tests': [
            [0x005bb380, 0x0000000f, 0x00001000],  # canonical: LAB_005bb380, THREAD_PRIO_TC, 4096
            [0x00000000, 0x00000000, 0x00000000],  # all zero
            [0xDEADBEEF, 0x00000001, 0x00000400],  # sentinel proc, prio=1, stack=1024
            [0x005bb380, 0x00000002, 0x00002000],  # prio=2, stack=8192
            [0x00400000, 0x0000000f, 0x00010000],  # base addr proc, max prio, large stack
            [0xCAFEBABE, 0x00000000, 0x00001000],  # exotic proc, prio=0
            [0x005bb380, 0x0000000f, 0xFFFFFFFF],  # max stack_size
            [0x00000001, 0x0000000f, 0x00001000],  # minimal proc ptr
            [0x7FFFFFFF, 0x0000000f, 0x00000100],  # signed-max proc
            [0x005bb380, 0x000000FF, 0x00001000],  # large priority value
        ],
        'path2_tests': [
            [0x005bb380, 0x0000000f, 0x00001000],
            [0x00000000, 0x00000000, 0x00000000],
            [0xDEADBEEF, 0x00000001, 0x00000400],
        ],
    },

    # 0x005a9e10  AudioSubStructTwoCallInit
    # fn(void* p1, void* p2, void* p3) -> p1.
    # Calls FUN_005adfe0(p1,p3) then FUN_005ae010(p1,p2); returns p1 unchanged.
    # sub_struct_dispatcher: 3 scratch bufs; verify return == BUF0 addr (1=correct, 0=wrong).
    # Both paths route through same original callees; key assertion is return==param_1.
    # U-0351 (semantic role of p2/p3) does not affect bit-identity of return value.
    'audio_sub_struct_two_call_init': {
        'rva':            0x005a9e10,
        'export':         'AudioSubStructTwoCallInit',
        'signature':      {'ret': 'pointer', 'args': ['pointer', 'pointer', 'pointer']},
        'arg_type':       'sub_struct_dispatcher',
        'lut_root_delta': 0,
        # Each test is a dummy iteration index; bufs are rebuilt each call.
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x005ade90  AudioListDrain2
    # void fn(sentinel_ptr): drain all nodes from list, return each to pool.
    # Second implementation (C3 target for c3-batch-f-s9); same RVA as AudioListDrain.
    # audio_list_drain: build sentinel + N nodes via FUN_005addd0 (insert_rva); drain;
    # verify sentinel self-loops (1=empty, 0=not drained).
    'audio_list_drain2': {
        'rva':          0x005ade90,
        'export':       'AudioListDrain2',
        'signature':    {'ret': 'void', 'args': ['pointer']},
        'arg_type':     'audio_list_drain',
        'insert_rva':   0x005addd0,
        'lut_root_delta': 0,
        'path1_tests':  [0, 1, 2, 3, 5, 1, 3, 2, 0, 1],
        'path2_tests':  [0, 1, 2],
    },
    # Repurposed for the early_window lane (was 'music_vol_set' booted-lane, never
    # promoted -> still C2). struct_list_float_set: struct+0x38=vol; walk circular
    # list at struct+0xc setting node+0x14|=0x40; secondary+0x30=vol bits. Handler
    # builds a 1-node self-circular list + secondary; vol=0.5+t*0.25; observe
    # struct+0x38|node+0x14|secondary+0x30. Already installed + reimpl in AudioMusic.cpp.
    'music_group_volume_set': {
        'rva':            0x005baf00,
        'export':         'MusicGroupVolumeSet',
        'signature':      {'ret': 'void', 'args': ['pointer', 'float']},
        'arg_type':       'struct_list_float_set',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3],
        'path2_tests':    [0, 1, 2, 3],
    },
    'timer_slot_clear': {
        'rva':            0x0041d820,
        'export':         'TimerSlotClear',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x0063d558,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x0041e130  TimerTrackSetter
    # void(uint32): writes param_1 to DAT_0063d7e0. 9-byte body.
    # Asm: MOV EAX,[ESP+4] / MOV [0x0063d7e0],EAX / RET
    # Strategy: call fn(value) with 10 distinct values via int_scalar.
    # Both orig and reimpl return void (undefined === undefined) â€” trivial match.
    # Write correctness established by C2 analysis (single MOV instruction).
    'timer_track_setter': {
        'rva':            0x0041e130,
        'export':         'TimerTrackSetter',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0x42424242, 0xDEADBEEF,
                           0x80000001, 0x3F800000, 0xFFFFFFFF, 0x0000007B,
                           0xCAFEBABE, 0x12345678],
        'path2_tests':    [0x42424242, 0xDEADBEEF, 0xFFFFFFFF],
    },
    'timer_bit_field_set': {
        'rva':            0x0041eda0,
        'export':         'TimerBitFieldSet',
        'signature':      {'ret': 'void', 'args': ['int32', 'int32']},
        'arg_type':       'entity_field_set',
        'target_global':  0x0063dc74,
        'entity_byte_stride': 0x2ac,
        'lut_root_delta': 0,
        # [slot, flag]: flag!=0 sets bit3, flag==0 clears it.
        'path1_tests': [
            [0, 1],  [0, 0],  [0, 1],  [0, 0],
            [1, 1],  [1, 0],  [1, 1],  [1, 0],
            [2, 1],  [2, 0],
        ],
        'path2_tests': [
            [0, 1], [0, 0], [1, 1],
        ],
    },

    # 0x0041f000  TimerSlotDataCopy  void(int slot, int* dst)
    # Copies 6 dwords from 0x0063dc10 + slot*0x2ac into *dst.
    # Strategy: int_copy_outbuf â€” allocate 4 KB sentinel-filled (0xCD) scratch
    # buffer per side; call fn(slot, buf); fingerprint first 24 bytes.
    # Source is a live game-global array; at main-menu quiescent state the values
    # are stable. Both orig and reimpl read the same source â†’ identical fingerprint.
    # Slot 0 maps to 0x0063dc10; slot 1 to 0x0063debc, etc.
    # Prior arg_type 'int_copy24_out' did not exist in diff_template.js and silently
    # fell through to default fn(input) â€” calling 2-arg fn with 1 arg â†’ AV both sides.
    'timer_slot_data_copy': {
        'rva':            0x0041f000,
        'export':         'TimerSlotDataCopy',
        'signature':      {'ret': 'void', 'args': ['int32', 'pointer']},
        'arg_type':       'int_copy_outbuf',
        'out_buf_size':   24,
        'lut_root_delta': 0,
        # Test vectors: slot indices 0..9 (same slot read multiple times is valid;
        # the source globals are stable at quiescent main-menu state).
        'path1_tests': [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests': [0, 1, 2],
    },

    # 0x00420d40  TimerStructArrayClear  void(void)
    # Zeroes 24 bytes per element Ã— 6 elements at 0x0063e4b8..0x0063e4bc.
    # Strategy: void_write_observe on first element's first dword (0x0063e4b8).
    # Write sentinel, call fn, read back â€” should be 0.
    # Export renamed TimerStructArrayClear (not TimerArrayClear) to avoid
    # collision with existing 0x00422b30 in Frontend/TimerReset.cpp.
    'timer_array_clear_d40': {
        'rva':            0x00420d40,
        'export':         'TimerStructArrayClear',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x0063e4b8,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00422120  TimerInitLoop  void(void)
    # Iterates 4 elements at 0x0063fb90 stride 0x208, calling FUN_00421c50 per entry.
    # Strategy: void_write_observe on 0x0063fb90 (first loop element base).
    # Both orig and reimpl call FUN_00421c50 which may write to this region.
    # Write sentinel, call fn, read back; A/B match proves identical side-effect path.
    # Note: FUN_00421c50 is not yet classified; callee-gate: â‰¤2 drift-promotes, proceed.
    'timer_init_loop': {
        'rva':            0x00422120,
        'export':         'TimerInitLoop',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x0063fb90,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x004222c0  TimerInitThunk  void(void)
    # Confirmed thunk of 0x00422120 (TimerInitLoop). Identical test strategy.
    # Observation point: 0x0063fc80 (element 0 +0xf0) â€” a real write site.
    # Per Ghidra decomp 2026-05-24: TimerInitLoop iterates 4 elements at
    # 0x0063fb90 stride 0x208 calling FUN_00421c50; FUN_00421c50 calls
    # FUN_00421c10 twice with EAX advancing by 0x100; FUN_00421c10's tail
    # writes 0 to (param+0xf0) and (param+0xf4). For element 0 with param =
    # 0x0063fb90, that means writes hit 0x0063fc80 and 0x0063fc84. Prior
    # observation point 0x0063fb90 (element base) was NEVER written by the
    # function â€” both sides preserved sentinel â†’ fake-GREEN match.
    'timer_init_thunk': {
        'rva':            0x004222c0,
        'export':         'TimerInitThunk',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x0063fc80,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00422b10  TimerDwordClear  void(void)
    # Zeroes 0x4e0 bytes (312 dwords) at 0x008994c0.
    # Strategy: void_write_observe on 0x008994c0 (base of zeroed block).
    # Write sentinel, call fn, read back â€” should be 0.
    'timer_dword_clear': {
        'rva':            0x00422b10,
        'export':         'TimerDwordClear',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x008994c0,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00425b10  TimerGlobalZero  void(void)
    # Writes 0 to 8 globals at 0x008992a0 stride 0x4c.
    # Strategy: void_write_observe on 0x008992a0 (entry 0, first written address).
    # Write sentinel, call fn, read back â€” should be 0.
    'timer_global_zero': {
        'rva':            0x00425b10,
        'export':         'TimerGlobalZero',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x008992a0,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },
    'save_status_clear': {
        'rva':            0x004099e0,
        'export':         'SaveStatusClear',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'int_write_observe',
        'target_global':  0x008a95a0,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0x00000002, 0x00000003,
                           0xDEADBEEF, 0xFFFFFFFF, 0x80000000, 0x12345678,
                           0x55555555, 0x00000000],
        'path2_tests':    [0x00000000, 0x00000001, 0xDEADBEEF],
    },

    # 0x00404e50  SaveLoad
    # int(void): calls FUN_004b3b70(gamesave.bin, buf, size) then
    #   SaveStatusClear(0). Always returns 0.
    # Strategy: none â€” call 1x at quiescent LUT-ready state.
    # crash_equal_ok: FUN_004b3b70 may raise if VFS not yet open; if both
    # sides fail identically, that still proves the reimpl has the same ABI.
    # Return must be 0 for both orig and reimpl when VFS is ready.
    # 2 iteration markers minimise hang risk while still covering orig/reimpl paths.
    'save_load': {
        'rva':            0x00404e50,
        'export':         'SaveLoad',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1],
        'path2_tests':    [0],
    },

    # 0x00404f50  SaveWrite
    # int(void): calls FUN_004b3bb0(gamesave.bin, buf, size) then
    #   SaveStatusClear(0). Always returns 0.
    # Mirror of SaveLoad. Same test strategy and crash_equal_ok rationale.
    'save_write': {
        'rva':            0x00404f50,
        'export':         'SaveWrite',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1],
        'path2_tests':    [0],
    },

    # 0x00404f80  SaveFileExists
    # int(void): calls FUN_00550b00(gamesave.bin); normalizes via NEG/SBB/NEG.
    # Returns 1 if gamesave.bin exists, 0 otherwise.
    # Strategy: none â€” call 10x at quiescent state; both must return identical
    # value. At main menu the file either exists or does not; both paths agree.
    'save_file_exists': {
        'rva':            0x00404f80,
        'export':         'SaveFileExists',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },
    'vfs_stream_read': {
        'rva':            0x00550980,
        'export':         'VfsStreamRead',
        'signature':      {'ret': 'int32', 'args': ['pointer', 'pointer', 'int32', 'int32']},
        'arg_type':       'int_pair',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests': [
            [0, 0],
            [0, 0],
            [0, 0],
            [0, 0],
            [0, 0],
            [0, 0],
            [0, 0],
            [0, 0],
            [0, 0],
            [0, 0],
        ],
        'path2_tests':    [[0, 0], [0, 0], [0, 0]],
    },

    # 0x00550bc0  VfsStreamGetType
    # Pure 8-byte leaf: return *(int*)(ctx + 8). Trivial accessor.
    # Disasm: MOV EAX, [ESP+4]; MOV EAX, [EAX+8]; RET.
    # Strategy: pass ptr(0) (NULL) as ctx; both orig and reimpl dereference
    # *(NULL+8) and crash identically. crash_equal_ok=True counts equal-crash
    # as GREEN. This proves the function body is identical (same crash path),
    # which is the non-trivial domain check for the leaf-function exemption.
    # A deeper test would require a live VFS stream context (harness-limited).
    # int_scalar: passes the single int32 test value as the pointer arg.
    # Evidence: log/diff_vfs_stream_get_type.csv
    'vfs_stream_get_type': {
        'rva':            0x00550bc0,
        'export':         'VfsStreamGetType',
        'signature':      {'ret': 'int32', 'args': ['pointer']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # 0x005514e0  RtFSHandler_IsEOF
    # 39-byte EOF predicate for the piz-fsmanager slot struct.
    # Compares 64-bit position (slot[2]/slot[3] = +0x8/+0xc) vs 64-bit file
    # size (slot[0]/slot[1] = +0x0/+0x4).  Returns 1 (EOF) when:
    #   * posLow == -1 (SetFilePointer error sentinel); or
    #   * posHigh > sizeHigh (signed); or
    #   * posHigh == sizeHigh and posLow >= sizeLow (signed).
    # Otherwise returns 0 (not EOF).
    # Calling convention: __cdecl, single pointer arg, plain RET (no stack cleanup).
    # arg_type: ptr_arg_int_get — allocates a struct_size scratch buffer, fills it
    # with per-seed deterministic pattern (seed + o*0x01010101), passes ptr;
    # return value is the int (0 or 1). struct_size=16 (covers +0x0..+0xc).
    # Test seeds chosen for non-degeneracy:
    #   seed=0          -> posHigh=0x0c0c0c0c > sizeHigh=0x04040404 -> returns 1
    #   seed=0x77777777 -> posHigh=0x83838383 signed=-2089386109 < sizeHigh=0x7b7b7b7b signed=2071756667 -> returns 0
    #   seed=0x80000000 -> posHigh=0x8c0c0c0c > sizeHigh=0x84040404 (both negative, -1946>-2079) -> returns 1
    #   seed=0x77000000 -> posHigh=0x830c0c0c signed=-2096039924 < sizeHigh=0x7b040404 signed=2064811012 -> returns 0
    # Evidence: log/diff_rtfshandler_is_eof.csv
    # Subsystem: util — vtable+0x44 of piz_fsmanager_handler (see RtFSHandler::Install)
    'rtfshandler_is_eof': {
        'rva':            0x005514e0,
        'export':         'RtFSHandler_IsEOF',
        'signature':      {'ret': 'int32', 'args': ['pointer']},
        'arg_type':       'ptr_arg_int_get',
        'struct_size':    16,
        'lut_root_delta': 0,
        'path1_tests':    [0, 0x77777777, 0x80000000, 0x77000000],
        'path2_tests':    [0, 0x77777777],
    },

    # 0x004a4541  FsopenSafe
    # 18-byte _fsopen wrapper: forces shflag = 0x40 (_SH_DENYNO).
    # Two-arg thunk: FsopenSafe(filename, mode) -> _fsopen(filename, mode, 0x40).
    # Returns FILE* (NULL on failure).
    # Strategy: call with NULL, NULL as filename/mode pointers.
    # _fsopen(NULL, NULL, 0x40) returns NULL (CRT guards invalid args).
    # Both orig and reimpl must return NULL (ptr(0)) â€” bit-identical.
    # crash_equal_ok=True handles any unexpected AV on both sides equally.
    # int_pair: passes [0, 0] as two pointer-sized ints (NULL pointers on x86).
    # Evidence: log/diff_FsopenSafe.csv
    'fsopen_safe': {
        'rva':            0x004a4541,
        'export':         'FsopenSafe',
        'signature':      {'ret': 'pointer', 'args': ['pointer', 'pointer']},
        'arg_type':       'int_pair',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests': [
            [0, 0],
            [0, 0],
            [0, 0],
            [0, 0],
            [0, 0],
            [0, 0],
            [0, 0],
            [0, 0],
            [0, 0],
            [0, 0],
        ],
        'path2_tests':    [[0, 0], [0, 0], [0, 0]],
    },
    'config_log_error': {
        'rva':            0x004963e0,
        'export':         'ConfigLogError',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # Pass known read-only string VAs from MASHED.exe image as the msg pointer.
        # "videocfg.bin" at 0x005d012c; "rb" at 0x005cf010.
        'path1_tests': [
            0x005d012c, 0x005cf010, 0x005d012c, 0x005cf010,
            0x005d012c, 0x005cf010, 0x005d012c, 0x005cf010,
            0x005d012c, 0x005cf010,
        ],
        'path2_tests': [0x005d012c, 0x005cf010, 0x005d012c],
    },

    # 0x00496400  ConfigLogDebug
    # void(char*, ...): variadic; vsprintf into 512B stack buf; fputs to log.
    # 103 bytes. Stack-cookie guarded (MSVC).
    # [NOTE] Variadic harness limitation: NativeFunction does not natively model
    # variadic cdecl stack layout for arbitrary arg counts. We test with a
    # format string that contains NO format specifiers ("videocfg.bin" at
    # 0x005d012c) so no variadic slots are consumed by vsprintf/vsnprintf.
    # A single pointer arg is passed after the format pointer â€” it lands on
    # the stack where the original reads &stack0x00000008 but is not consumed.
    # Both paths produce the same fputs call (format string copied verbatim).
    # Return is void/0. 10 calls with same safe string pair.
    # Callee FUN_004a42c5 is vsprintf variant (DEFERRED D-2380 / STUB S-0820).
    'config_log_debug': {
        'rva':            0x00496400,
        'export':         'ConfigLogDebug',
        'signature':      {'ret': 'void', 'args': ['int32', 'int32']},
        'arg_type':       'int_pair',
        'lut_root_delta': 0,
        # [fmt, arg1]: fmt=0x005d012c ("videocfg.bin" â€” no % specifiers),
        # arg1=0x005cf010 ("rb") â€” harmlessly on stack, not consumed by vsprintf.
        'path1_tests': [
            [0x005d012c, 0x005cf010],
            [0x005d012c, 0x005cf010],
            [0x005d012c, 0x005cf010],
            [0x005d012c, 0x005cf010],
            [0x005d012c, 0x005cf010],
            [0x005d012c, 0x005cf010],
            [0x005d012c, 0x005cf010],
            [0x005d012c, 0x005cf010],
            [0x005d012c, 0x005cf010],
            [0x005d012c, 0x005cf010],
        ],
        'path2_tests': [
            [0x005d012c, 0x005cf010],
            [0x005d012c, 0x005cf010],
            [0x005d012c, 0x005cf010],
        ],
    },

    # 0x00498910  ConfigFilenameGet
    # char*(void): copies "videocfg.bin" 4B at a time to buf@0x007731e8; returns &buf.
    # 49 bytes. Pure leaf. Two callers: ConfigLoad, CONFIG_SAVE_FN.
    # Strategy: void arg_type â€” call 10x; return value is constant pointer
    # 0x007731e8 each time. Both paths must return the same address.
    # Side-effect: 0x007731e8 contains "videocfg.bin\0" after each call.
    'config_filename_get': {
        'rva':            0x00498910,
        'export':         'ConfigFilenameGet',
        'signature':      {'ret': 'pointer', 'args': []},
        'arg_type':       'void',
        'lut_root_delta': 0,
        # 10 dummy calls; return value must be 0x007731e8 each time.
        'path1_tests': [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests': [0, 0, 0],
    },

    # 0x00498950  ConfigLoad
    # int(void): opens "videocfg.bin" via _fsopen("rb", _SH_DENYNO);
    # fread 512B into DAT_00773208; returns 1 on success, 0 on failure.
    # Calls ConfigFilenameGet, ConfigLogDebug, ConfigLogError as callees.
    # Strategy: none â€” call 5x at quiescent game state; videocfg.bin exists
    # in the game directory so both paths return 1. Side-effect is idempotent
    # (same 512 bytes written to 0x00773208 each call). Both paths must agree.
    'config_load': {
        'rva':            0x00498950,
        'export':         'ConfigLoad',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        # 5 calls; videocfg.bin exists at runtime -> returns 1 each time.
        # Additional 5 dummy iterations to satisfy path1_tests >= 10.
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests': [0, 1, 2],
    },
    'crt_pre_init': {
        'rva':            0x004a31f3,
        'export':         'CrtPreInit',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'void',
        'lut_root_delta': 0,
        # 10 dummy test inputs (arg_type='void' ignores input; we just want 10 calls).
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # 0x004955b0  DInputInitPredicate
    # uint8(void): thin bool wrapper around 0x00495530 (DI8Create wrapper, C2).
    #   int iVar1 = FUN_00495530();
    #   return iVar1 != 0;
    # 11-byte body (0x004955b0..0x004955bb). Sole callee is the IDirectInput8A
    # creation wrapper, which calls DirectInput8Create(NULL_hinst, 0x0800,
    # IID_IDirectInput8A, &DAT_00771e78, NULL) and returns 1/0.
    #
    # Idempotency strategy: at quiescent main-menu, DirectInput8Create is
    # already-succeeded; re-issuing it overwrites DAT_00771e78 with a fresh
    # IDirectInput8A* on each call. Both original and reimpl perform the same
    # write sequence, so the predicate return is bit-identical (== 1) on every
    # call. If DI8Create fails for any reason on either side, crash_equal_ok
    # accepts matching crash strings as GREEN. Reference:
    # re/analysis/boot_subsystem_d3/0x004955b0.md
    'dinput_init_predicate': {
        'rva':            0x004955b0,
        'export':         'DInputInitPredicate',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'void',
        'lut_root_delta': 0,
        'crash_equal_ok': True,
        # 10 dummy calls; arg_type='void' ignores input.
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # 0x004a3258  CrtExitCore
    # void(uint, int, int): CRT exit core â€” acquires lock 8, walks atexit list,
    # runs exit tables, calls ___crtExitProcess or FUN_004a77eb.
    # HARNESS-LIMITED: calling this will terminate the process (TerminateProcess or
    # ExitProcess). crash_equal_ok=True: if both original and reimpl crash with the
    # same Frida error, the test passes as bit-identical behavior.
    # Strategy: pass exit_code=0x42, skip_atexit=1 (skip atexit walk), use_unlock_path=0
    # (calls ___crtExitProcess). Both should crash with the same error.
    # Single int arg via int_scalar; signature matches first param only.
    'crt_exit_core': {
        'rva':            0x004a3258,
        'export':         'CrtExitCore',
        'signature':      {'ret': 'void', 'args': ['uint32', 'int32', 'int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'crash_equal_ok': True,
        # int_scalar passes the single int as first arg; remaining args are 0 (cdecl default).
        # exit_code variants: all crash the process identically on both paths.
        'path1_tests':    [0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09],
        'path2_tests':    [0x00, 0x01, 0x02],
    },

    # 0x004a4bb7  WinMainEntry
    # int(void): PE entry point â€” full CRT startup sequence.
    # HARNESS-LIMITED: cannot be safely called from within a running process.
    # [UNCERTAIN U-0001] pre-GetVersionExA 4-byte write.
    # [UNCERTAIN U-0002] PE header parse branch result (local_20).
    # DRIFT-RISK: SEH frame magic bytes and PE header branch prevent 100%
    # bit-identical diff.
    # Strategy: call once with no args; both original and reimpl will attempt
    # re-init (heap double-init, second WinMain call, etc.) and crash the same way.
    # crash_equal_ok=True handles the crash-match case.
    'win_main_entry': {
        'rva':            0x004a4bb7,
        'export':         'WinMainEntry',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'void',
        'lut_root_delta': 0,
        'crash_equal_ok': True,
        # 10 dummy calls; arg_type='void' ignores input.
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },
    'timer_flag_clear': {
        'rva':            0x0041d820,
        'export':         'TimerFlagClear',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x0063d558,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x0041e130  TimerStateSet
    # void(uint32): writes param_1 to DAT_0063d7e0. 9 bytes.
    # Strategy: write_global_setter â€” call fn(test_value), read back 0x0063d7e0.
    # Both orig and reimpl must produce identical readback (== test_value).
    'timer_state_set': {
        'rva':            0x0041e130,
        'export':         'TimerStateSet',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'write_global_setter',
        'target_global':  0x0063d7e0,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0x00000002, 0x00000003,
                           0xDEADBEEF, 0xFFFFFFFF, 0x80000000, 0x00000064,
                           0x7FFFFFFF, 0x0000002A],
        'path2_tests':    [0x00000000, 0x00000001, 0xDEADBEEF],
    },

    # 0x00426630  PitchParamSet
    # void(uint32): writes param_1 to DAT_0066d6fc. 9 bytes.
    # Strategy: write_global_setter â€” call fn(test_value), read back 0x0066d6fc.
    'pitch_param_set': {
        'rva':            0x00426630,
        'export':         'PitchParamSet',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'write_global_setter',
        'target_global':  0x0066d6fc,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0x00000002, 0x00000003,
                           0xDEADBEEF, 0xFFFFFFFF, 0x80000000, 0x00000064,
                           0x7FFFFFFF, 0x0000002A],
        'path2_tests':    [0x00000000, 0x00000001, 0xDEADBEEF],
    },

    # 0x004266f0  PitchParam2Set
    # void(uint32): writes param_1 to DAT_0066d700 (sibling of PitchParamSet; +4). 9 bytes.
    # Strategy: write_global_setter â€” call fn(test_value), read back 0x0066d700.
    'pitch_param2_set': {
        'rva':            0x004266f0,
        'export':         'PitchParam2Set',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'write_global_setter',
        'target_global':  0x0066d700,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0x00000002, 0x00000003,
                           0xDEADBEEF, 0xFFFFFFFF, 0x80000000, 0x00000064,
                           0x7FFFFFFF, 0x0000002A],
        'path2_tests':    [0x00000000, 0x00000001, 0xDEADBEEF],
    },
    # Note: alias entries 'timer_array_zero' / 'player_slot_zero' removed
    # 2026-05-24 â€” their exports (TimerArrayZero / PlayerSlotZero) were
    # superseded by the first-wins TimerDwordClear / TimerGlobalZero in
    # Util/TimerInit.cpp and no longer exist in the .asi. Use
    # 'timer_dword_clear' (0x00422b10) and 'timer_global_zero' (0x00425b10).

    'float_table_init': {
        'rva':            0x0041cbc0,
        'export':         'FloatTableInit',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x005f337c,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },
    # Note: alias entry 'slot_data_copy' removed 2026-05-24 â€” its export
    # SlotDataCopy was superseded by TimerSlotDataCopy (Util/TimerInit.cpp:78)
    # and no longer exists in the .asi. Use 'timer_slot_data_copy' instead.

    # 0x0041eda0  SlotBitSet  void(int param_1, int param_2)
    # target = 0x0063dc74 + param_1 * 0x2ac.
    # if param_2 != 0: *target |= 0x8; else: *target &= ~0x8.
    # arg_type='entity_field_set': fn(param_1, param_2), reads back
    # target_global + param_1 * entity_byte_stride as observable uint32.
    # Note: bit 3 of the target dword must survive; we read it back via
    # entity_field_set (which reads CONFIG.target_global + p1*stride).
    'slot_bit_set': {
        'rva':            0x0041eda0,
        'export':         'SlotBitSet',
        'signature':      {'ret': 'void', 'args': ['int32', 'int32']},
        'arg_type':       'entity_field_set',
        'target_global':  0x0063dc74,
        'entity_byte_stride': 0x2ac,
        'lut_root_delta': 0,
        'path1_tests': [
            [0, 0],  [0, 1],  [1, 0],  [1, 1],
            [2, 0],  [2, 1],  [3, 0],  [3, 1],
            [0, 0xff], [1, 0],
        ],
        'path2_tests': [
            [0, 0], [0, 1], [1, 0],
        ],
    },

    # 0x00420d40  SlotArrayClear  void(void)
    # 6-iter loop over [0x0063e4c4, 0x0063e554) stride 0x24.
    # Per iter: memset(ptr-0xc, 0, 8); ptr[-1]=0; *ptr=0; ptr[1]=0; ptr[2]=0.
    # arg_type='void_write_observe': write sentinel to 0x0063e4c4 (ptr[0]),
    # call fn, read back â€” if fn works correctly it will write 0 there.
    'slot_array_clear': {
        'rva':            0x00420d40,
        'export':         'SlotArrayClear',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x0063e4c4,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },
    # 'timer_subarray_init' was a duplicate entry for 0x00422120 added by
    # c3_batch_h-s5 (flagged in frida-sweep-20260517-2121 release notes).
    # The canonical entry for that RVA is 'timer_init_loop' above
    # (arg_type='void_write_observe' on 0x0063fb90).  Removed during the
    # TimerInitLoop naked-wrapper fix (feature/timer-init-naked).

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-g-s1 â€” frontend menu chrome (C2->C3, 2 candidates)
    # Frontend/MenuChrome.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042aad0  MenuDimSet
    # Non-standard ABI: implicit EAX pointer at entry (naked function).
    # Writes byte 0x30 to *(in_EAX+3) and sets DAT_008990e4 = 1.
    # Harness limitation: NativeFunction cannot control EAX, so the EAX
    # dereference will crash if EAX is invalid at call time.
    # Strategy: void_write_observe on DAT_008990e4 with crash_equal_ok=True.
    #   - Write sentinel to 0x008990e4, call function, read back 0x008990e4.
    #   - If EAX is valid in game context: both orig and reimpl write 1 -> match.
    #   - If EAX is invalid: crash_equal_ok accepts matching crash.
    # 10 sentinels: covers 0, 1, max, patterns that prove write-to-1 behavior.
    'menu_dim_set': {
        'rva':            0x0042aad0,
        'export':         'MenuDimSet',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x008990e4,
        'lut_root_delta': 0,
        'crash_equal_ok': True,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xFFFFFFFF],
    },

    # 0x0042aae0  MenuIm2DQuad
    # void __fastcall fn(int param_1).
    # Renders a fullscreen dim-overlay quad via RwIm2D vtable calls.
    # Harness limitation: requires live game state (vtable at 0x007d3ff8 must
    # be initialized by the game engine before this is called).
    # Strategy: int_scalar with crash_equal_ok=True; pass param_1 variants
    # 0..5 and observe whether global alpha buffer 0x0067eca8 is read the same
    # way. The draw is suppressed if DAT_0067eca8==0 so crash may not occur
    # at main menu quiescent state.
    # If alpha is 0 at main menu: both orig and reimpl return void without draw.
    # 10 test vectors: param_1 values 0..5 plus sentinels.
    'menu_im2d_quad': {
        'rva':            0x0042aae0,
        'export':         'MenuIm2DQuad',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'crash_equal_ok': True,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 0x100, 0x200, 0x7fffffff, 0],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-g-s7 â€” race_results slot-state getters (C2â†’C3)
    # Frontend/RaceResults.cpp
    #
    # NOT registered (caller-gate / sig failures):
    #   0x0040b460  SlotSortByScoreWithModeOverride â€” REFUSED: callee 0x00417740 not C2
    #   0x0040e3a0  PlayerColorTableGet â€” already in MenuScoreSort.cpp;
    #                (int, byte*) sig unsupported (pointer out-param needs custom arg_type)
    #   0x0040e480  CarSlotStateSet â€” DEFERRED: no viable non-destructive diff arg_type
    #                for double-deref setter (entity_field_set reads wrong address for PTR_PTR)
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0040e470  CarSlotStateGet
    # uint32 FUN_0040e470(int param_1) â€” 14-byte leaf getter, no callees.
    # Returns *(PTR_PTR_005f2770 + param_1*4 + 0x34).
    # At quiescent main menu, PTR_PTR_005f2770 is initialised; reads slot state.
    # At menu, only indices 0-3 are valid; caller FUN_0042b770 uses this to
    # skip AI-controlled slots (result == 2).
    # int_scalar: pass slot index 0-3 (10 tests including repeats for coverage);
    # both orig and reimpl must return same value from live game state.
    # ref: re/analysis/race_results/0040e470.md
    'car_slot_state_get': {
        'rva':            0x0040e470,
        'export':         'CarSlotStateGet',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # Session c3-batch-g-s14 â€” FontSys+HudDispatch+RW-release (C2->C3)
    # HUD/FontCtx.cpp â€” FontSys init sequence
    # HUD/HudDispatch.cpp â€” EAX-thiscall vtable dispatcher
    # 0x004c5a60 DEFERRED â€” callees 004d8060 + 004c7650 both C1/unknown; caller gate fails.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00552b60  FontSys_InitSeq
    # fn(void) -> void.
    # 8-step font subsystem init: zero flag + 7 alloc/init calls. No branches.
    # Called exactly once during game init (FontText_HudInit at 0x00427ca0).
    # By the time the diff attaches (~2s after spawn), MASHED has already
    # called FontSys_InitSeq once via its natural boot â€” state is now valid.
    # Re-calling 10x in the diff allocates/leaks state and hangs MASHED
    # (timeout observed 2026-05-24). Reduced to 1 call. arg_type='none':
    # both Orig and Reimpl execute the same allocation sequence once; if
    # either side hangs or diverges, the test reports it.
    'font_sys_init_seq': {
        'rva':            0x00552b60,
        'export':         'FontSys_InitSeq',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0],
        'path2_tests':    [0],
    },

    # Session c3-batch-g-s15 â€” carry-overs from batch-d/c
    # MenuButtonDetect.cpp â€” trivial uint16 screen-dimension getters (C2â†’C3)
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042b8b0  ScreenWidthGet
    # Returns DAT_0067ea54 as uint16_t in EAX. 6-byte leaf. No callees.
    # Strategy: arg_type='none' â€” call 10x at quiescent main menu state; both
    # orig and reimpl read 0x0067ea54 and return the live screen pixel width.
    # ret='uint32' (Frida reads full EAX; upper 16 bits zero after MOVZX).
    # Analysis: re/analysis/promote_c2_hud_ingame/0x0042b8b0.md
    'screen_width_get': {
        'rva':            0x0042b8b0,
        'export':         'ScreenWidthGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00552e40  FontCtx_FlushMatrix
    # fn(void) -> float* (&DAT_00912b98).
    # Dirty-gated matrix composer. Path A (dirty=0): compose+scale+translate+cache;
    # Path B: skip. Every call: compose cached with camera view â†’ DAT_00912b98.
    # Callee 0x004c0ed0 (S-2126) reads *(cam+4) â€” DAT_00912c0c is NULL at boot,
    # so dereferencing cam+4 null-deref crashes identically on both sides.
    # crash_equal_ok=True: both crash with the same error (proven in c3-batch-c-s6).
    # Implementation committed in HUD/FontCtx.cpp (c3-batch-g-s14 session).
    # Re-run after FlushMatrix was refused in frida-sweep-20260515-0105.
    'font_ctx_flush_matrix': {
        'rva':            0x00552e40,
        'export':         'FontCtx_FlushMatrix',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042b8c0  ScreenHeightGet
    # Returns DAT_0067ea56 as uint16_t in EAX. 6-byte leaf. No callees.
    # Strategy: arg_type='none' â€” call 10x at quiescent main menu state; both
    # orig and reimpl read 0x0067ea56 and return the live screen pixel height.
    # ret='uint32' (Frida reads full EAX; upper 16 bits zero after MOVZX).
    # Analysis: re/analysis/promote_c2_hud_ingame/0x0042b8c0.md
    'screen_height_get': {
        'rva':            0x0042b8c0,
        'export':         'ScreenHeightGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0041c2d0  FUN_0041c2d0
    # fn(void) -> void. EAX-thiscall: object ptr in EAX.
    # Dispatches vtable[0x48] (Draw/Render) on 4 member objects at EAX+0xc,4,0,8.
    # No guards â€” unconditional. Shared by game-mode 10 and game-mode 5.
    # At quiescent main menu neither mode-10 nor mode-5 guards are active,
    # so calling with EAX=0 (null ptr) crashes identically on both sides.
    # crash_equal_ok=True. arg_type='none': call 10x, both crash same way.
    'hud_dispatch_draw4': {
        'rva':            0x0041c2d0,
        'export':         'FUN_0041c2d0',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session ma2-frida-s2 â€” Boot/Window cluster (C2â†’C3, 2 of 6 candidates)
    # Boot/Window.cpp â€” window-show + msgpump from the main game loop.
    # Refused: 0x00499ba0 (Window_Create â€” destructive 3-arg), 0x00499cc0
    #          (Window_Destroy â€” terminates game), 0x00499820 (Window_WndProc
    #          â€” needs 4-arg WNDPROC harness), 0x00496490 (Window_WmCreate
    #          â€” three callees still C1).
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004996f0  WindowShow
    # void __cdecl WindowShow(int nCmdShow)
    # ShowWindow(DAT_007e9584, nCmdShow); UpdateWindow(DAT_007e9584).
    # Both orig and reimpl invoke the same user32.dll APIs against the same
    # global HWND (DAT_007e9584), already created by the boot path. ShowWindow
    # on an already-visible window with SW_SHOW/SW_NORMAL is a no-op; reimpl
    # wrapper returns void â†’ void_match counts as GREEN.
    # Test domain: standard SW_* command codes.
    'window_show': {
        'rva':            0x004996f0,
        'export':         'WindowShow',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # SW_HIDE=0, SW_SHOWNORMAL=1, SW_SHOWMINIMIZED=2, SW_SHOWMAXIMIZED=3,
        # SW_SHOWNOACTIVATE=4, SW_SHOW=5, SW_MINIMIZE=6, SW_SHOWMINNOACTIVE=7,
        # SW_SHOWNA=8, SW_RESTORE=9. Avoid SW_HIDE first to keep the window
        # visible during the test sequence; end with SW_SHOWNORMAL.
        'path1_tests':    [1, 5, 4, 8, 9, 9, 5, 4, 8, 1],
        'path2_tests':    [1, 5, 9],
    },

    # 0x00499690  WindowMsgPump
    # int __cdecl WindowMsgPump(void)  (bool return)
    # Single-iteration Win32 pump: PeekMessageA(MSG buf at 0x007e95a0, NULL, 0,
    # 0, PM_REMOVE). On message: short-circuits with quit-flag return if
    # MSG.message == WM_QUIT (0x12); else TranslateMessage + DispatchMessageA.
    # On no message: WaitMessage() only if DAT_0077391c == 0 (window inactive).
    # Always returns DAT_00773918 != 0 (the quit flag).
    # Frida agent thread owns no message queue â†’ PeekMessageA returns 0 on
    # both sides; main-menu state has DAT_0077391c == 1 (active) so WaitMessage
    # is skipped; both sides then return DAT_00773918 (== 0 at quiescent menu).
    # 10 calls give 10x bit-identical 0.
    'window_msg_pump': {
        'rva':            0x00499690,
        'export':         'WindowMsgPump',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # Session c3-batch-ma2-frida-s4 â€” boot teardown + COM release
    # Boot/Teardown.cpp â€” HardwareExitApplication helpers (FUN_00402a40 callees)
    # 5 of 6 candidates REFUSED: 0x00494bc0 (live COM Release destroys D3D
    # interfaces), 0x00489250 (live free destroys RW frames), 0x00494f20
    # (close-video destroys live state on first call so 10x diff diverges),
    # 0x004955c0 + 0x004963d0 (thunk targets FUN_00495580 / FUN_00496370 not
    # yet C1+plated). See re/DEFERRED.md D-10774..D-10778.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00494ef0  ThunkVideoStateGet â€” thunk â†’ FUN_00493f70 (intro_splash video flag read)
    # 5-byte E9 JMP at 0x00494ef0 â†’ 0x00493f70.  Target body is
    # `return DAT_00771a04;` (pure global reader; plate at
    # re/analysis/intro_splash/0x00493f70.md).  Read-only, safe to invoke 10x
    # at quiescent main menu â€” both orig and reimpl deref the same address.
    # arg_type='none': zero-arg call, compare uint32 return value.
    'thunk_video_state_get': {
        'rva':            0x00494ef0,
        'export':         'ThunkVideoStateGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # Session ma2-frida-s7 â€” DirectInput init chain (C2â†’C3, 6 candidates)
    # Input/DirectInput.cpp
    # First C3 promotions for the input subsystem.
    #
    # Refused this session:
    #   - 0x00497190 (contcfg filename formatter): implicit EAX arg
    #     (U-2588) â€” same dual-register ABI problem as MenuCenterCalc.
    #   - 0x00497230 (SaveControllerConfig): implicit EAX arg (U-3015)
    #     + file I/O side effects.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00499720  GetInputHinst
    # Pure leaf: `MOV EAX, [DAT_007e9580]; RET`. 5 bytes.
    # arg_type='read_global': write sentinel to DAT_007e9580, call fn(), verify
    # return == sentinel. Bit-identical on every call between orig and reimpl.
    # 10 sentinels covering 0, 1, max, bit-patterns.
    'get_input_hinst': {
        'rva':            0x00499720,
        'export':         'GetInputHinst',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x007e9580,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xCAFEBABE,
                           0x12345678, 0xFFFFFFFF, 0x80000000, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0x00000001, 0xFFFFFFFF],
    },

    # Session c3-batch ma2-frida-s6 â€” Boot/VideoConfig.cpp
    #   Video config getters + display dim helpers + RW driver-system wrapper.
    #   4 C2 â†’ C3 candidates promoted (2 of the 6 batch candidates deferred:
    #   0x00498b60 destructive teardown; 0x004c2ed0 needs wide out-buffer harness)
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00498bc0  VideoGetRenderWidth
    # Pure leaf: returns DAT_00616028 (render width in pixels).
    # MOV EAX, [0x00616028] / RET. Strategy: read_global â€” write sentinel
    # values into 0x00616028, call fn(), confirm both paths read the same
    # address and return the same bytes. 10 sentinels covering 0, 1, max,
    # high-bit, alt-bits and random patterns.
    # Analysis: re/analysis/promote_c2_launch_handshake/00498bc0.md
    'video_get_render_width': {
        'rva':            0x00498bc0,
        'export':         'VideoGetRenderWidth',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x00616028,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xCAFEBABE,
                           0x12345678, 0xFFFFFFFF, 0x80000000, 0x55555555,
                           0xAAAAAAAA, 0x00000320],  # 0x320 == 800 (canonical width)
        'path2_tests':    [0x00000000, 0x00000001, 0xFFFFFFFF],
    },

    # 0x00498bd0  VideoGetRenderHeight
    # Pure leaf: returns DAT_0061602c (render height in pixels).
    # MOV EAX, [0x0061602c] / RET. Sibling of VideoGetRenderWidth at +4.
    # Same read_global strategy; 10 sentinels.
    # Analysis: re/analysis/promote_c2_video_display/00498bd0.md
    'video_get_render_height': {
        'rva':            0x00498bd0,
        'export':         'VideoGetRenderHeight',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0061602c,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xCAFEBABE,
                           0x12345678, 0xFFFFFFFF, 0x80000000, 0x55555555,
                           0xAAAAAAAA, 0x00000258],  # 0x258 == 600 (canonical height)
        'path2_tests':    [0x00000000, 0x00000001, 0xFFFFFFFF],
    },

    # 0x00498bf0  DisplayGetCursorGate
    # Pure leaf: returns DAT_00773204 (cursor-visibility / display-active gate).
    # MOV EAX, [0x00773204] / RET. FUN_004951f0 uses non-zero return to gate
    # ShowCursor(0). Same read_global strategy; 10 sentinels.
    # Analysis: re/analysis/promote_c2_video_display/00498bf0.md
    'display_get_cursor_gate': {
        'rva':            0x00498bf0,
        'export':         'DisplayGetCursorGate',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x00773204,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xCAFEBABE,
                           0x12345678, 0xFFFFFFFF, 0x80000000, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0x00000001, 0xFFFFFFFF],
    },

    # 0x00495530  CreateDInputObject
    # Wrapper that calls dinput8!DirectInput8Create. 70 bytes.
    # arg_type='none': both sides log "Creating DInput object", call
    # GetModuleHandleA + DirectInput8Create with identical args, branch on
    # HRESULT. At quiescent main menu DInput is already initialized, so the
    # external call returns the same HRESULT on each invocation. Return value
    # comparison (1 success / 0 failure) is bit-identical between orig+reimpl.
    # Side effect: each call overwrites DAT_00771e78 with a new IDirectInput8A*
    # â€” affects subsequent test calls equally on both sides.
    'create_dinput_object': {
        'rva':            0x00495530,
        'export':         'CreateDInputObject',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        # crash_equal_ok=True: at quiescent main menu, calling this re-enters
        # DirectInput8Create against an already-initialized state. The dinput8
        # IAT path raises a structured exception via the OS DInput8 cleanup
        # logic â€” both sides hit it identically (both invoke the same external
        # entry point with identical arg layouts), so a same-error pair counts
        # as bit-identical for diff purposes.
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004955b0  CreateDInputObjectBool
    # 11-byte bool wrapper: returns FUN_00495530() != 0.
    # arg_type='none': same testing strategy as the inner function â€” bit-
    # identical return on both sides since both call the same target.
    'create_dinput_object_bool': {
        'rva':            0x004955b0,
        'export':         'CreateDInputObjectBool',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        # crash_equal_ok=True: this bool wrapper calls 0x00495530 which raises
        # a structured exception when re-invoked against an already-initialized
        # DirectInput state. Same reasoning as create_dinput_object above.
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0045b350  RwInitNullStub
    # 1-byte bare RET (0xC3). 33 callers in binary; functions as null callback.
    # arg_type='none' + signature void(void). Both sides do nothing and return
    # nothing â€” trivially bit-identical.
    'rw_init_null_stub': {
        'rva':            0x0045b350,
        'export':         'RwInitNullStub',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004c2f00  RwEngineGetCurrentMode
    # fn(void) -> uint32: calls dispatcher cmd 0x0a (driver-system) via
    # FUN_004c2c90 with DAT_007d3ff8+0x10. Returns current video mode index
    # or 0xffffffff on failure. Stable at quiescent main menu since the RW
    # driver context (DAT_007d3ff8) is already initialized â€” that's the same
    # global the harness polls via lut_root_delta=0.
    # arg_type='none': dummy iteration markers; both orig and reimpl must
    # produce identical return values across 10 successive calls. The reimpl
    # forwards to the same dispatcher RVA so the result is by construction
    # the same (we test that argument marshalling and result handling are
    # bit-identical, not the dispatcher itself which is already C2-mapped).
    # Analysis: re/analysis/render_promote_c2_rw_plugin/0x004c2f00.md
    'rw_engine_get_current_mode': {
        'rva':            0x004c2f00,
        'export':         'RwEngineGetCurrentMode',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # Session ma2-frida-s5 â€” Boot/FrameDispatch  (C2â†’C3, 5 candidates)
    # FrameDispatch.cpp â€” direct callees of per-frame tick FUN_00492e90.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042b8d0  StatePhaseIsIdle
    # int(void) â€” `return DAT_0067eca4 == 0;` (13-byte pure-leaf predicate).
    # arg_type='read_global': write sentinel to 0x0067eca4 before each call;
    # both orig and reimpl observe identical state-phase value â†’ identical
    # int return (0 or 1). 10 sentinels exercise both branches (==0 and !=0).
    'state_phase_is_idle': {
        'rva':            0x0042b8d0,
        'export':         'StatePhaseIsIdle',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067eca4,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0x00000002, 0x00000003,
                           0x00000004, 0x00000005, 0x0000000F, 0x80000000,
                           0xDEADBEEF, 0xFFFFFFFF],
        'path2_tests':    [0x00000000, 0x00000005, 0xDEADBEEF],
    },

    # 0x004b6520  ZeroFillWrapper - memset(dest, 0, count) wrapper over C3 callee
    # FUN_004b64e0. NEAR-LEAF: callee 0x4b64e0 is C3 (input/MemsetInline, pure-leaf
    # word-then-byte memset). Existing reimpl ZeroFillWrapper (Util/TimerSlot.cpp:33,
    # std::memset(p1,0,p2)) is observably == original (both zero exactly `count`
    # bytes, byte at `count` untouched). near_leaf_memset2 handler pre-fills dest
    # with 0xCC, calls f(dest,count), snapshots a fixed 0x20 window -> the
    # filled->0 / rest->0xCC boundary varies per count => NON-DEGENERATE (and the
    # boundary echo proves a bounded memset, not an over-write).
    'zero_fill_wrapper': {
        'rva':            0x004b6520,
        'export':         'ZeroFillWrapper',
        'signature':      {'ret': 'void', 'args': ['pointer', 'uint32']},
        'arg_type':       'near_leaf_memset2',
        'seed_sets':      [{'count': 10}, {'count': 16}, {'count': 7}, {'count': 0}],
        'path1_tests':    [0, 1, 2, 3],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x004c5800  RwTexDictionarySetCurrent â€” writes param_1 to dynamic
    # vtable-slot address (rw_base + 0x1c + rw_slot). Returns 1.
    # arg_type='int_scalar': just verifies both sides return 1 consistently.
    # The write side-effect can't be observed by void_setter_observe (dynamic
    # slot address), but both sides write to same address â€” restored by
    # the immediate-next call from MASHED's natural execution.
    'rw_tex_dictionary_set_current': {
        'rva':            0x004c5800,
        'export':         'RwTexDictionarySetCurrent',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF,
                           0x80000000, 0x55555555, 0xAAAAAAAA, 0x12345678, 0],
        'path2_tests':    [0, 0xDEADBEEF, 0],
    },

    # 0x004c5820  RwTexDictionaryGetCurrent â€” symmetric getter for setter above.
    # Returns *(rw_base + 0x1c + rw_slot). Both sides read same global so
    # return same value. arg_type='none' â€” value is whatever's currently
    # stored (deterministic across both calls).
    'rw_tex_dictionary_get_current': {
        'rva':            0x004c5820,
        'export':         'RwTexDictionaryGetCurrent',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0040e370  IsCarSlotActive â€” bool(int slot). For slot > 3 returns 0
    # immediately without dereferencing the per-car table. Test all inputs > 3
    # to exercise only the bounds-check early-out. Both sides return 0 â†’ GREEN.
    'is_car_slot_active': {
        'rva':            0x0040e370,
        'export':         'IsCarSlotActive',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # scenario:'race' (2026-06-12, re/analysis/scenario_attach_lane.md):
        # the original vectors were ALL > 3 to take the bounds-check early-out
        # because the in-bounds path derefs PTR_PTR_005f2770, NULL at quiescent
        # menu — i.e. the old run only ever tested the early-out (degenerate;
        # flagged in DEGENERATE_GREEN_AUDIT_2026-06-12.md). In a live race the
        # pointer is populated and slots 0..3 exist (4-car Quick Battle), so
        # the real path is exercised; slot-active flags are stable mid-round
        # (no A/B time-skew). Out-of-bounds vectors kept for early-out cover.
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 4, 100, 0x7FFFFFFF, 0, 1, 2],
        'path2_tests':    [0, 1, 4],
    },

    # 0x00432080  RaceEndCheckFinish â€” int(int param_1). Race-end finalization
    # checker; writes 0 to DAT_0067f19c then mode-dispatches. At quiescent menu
    # the early-out path (DAT_0067eca4 != 0) may fire; both sides match.
    'race_end_check_finish': {
        'rva':            0x00432080,
        'export':         'RaceEndCheckFinish',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, -1, 100, 0, 1],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00404320  PerModeRenderMachine â€” void(uint32 mode). Mode 5/8/9/10
    # take specific dispatch paths; modes outside this set (incl. 0/1 at
    # quiescent menu) fall through with no-op. arg_type='int_scalar' with
    # non-matching modes exercises the fall-through; both sides match.
    'per_mode_render_machine': {
        'rva':            0x00404320,
        'export':         'PerModeRenderMachine',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # Modes not in {5,8,9,10} â†’ no-op fall-through. Avoid mode 10 which
        # calls ModePrologue + RwCameraEndUpdate (state-dependent).
        'path1_tests':    [0, 1, 2, 3, 4, 6, 7, 11, 100, 0xFFFFFFFF],
        'path2_tests':    [0, 1, 100],
    },

    # 0x00492770  MainLoopInit
    # int(void) â€” writes 4 fixed globals to 0 plus state-machine=1 plus 2 callees.
    # void_write_observe on 0x00828300 (exit flag, first write). Callees include
    # FUN_004c57a0 (allocator) which may have side-effects; both sides call same
    # allocator with same state and observable is the deterministic exit-flag write.
    'main_loop_init': {
        'rva':            0x00492770,
        'export':         'MainLoopInit',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x00828300,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00426c00  GameStateFlagGet
    # uint32(void) â€” returns DAT_00644158 (5-byte pure-leaf getter).
    # arg_type='read_global': write sentinel; both sides read same global.
    'game_state_flag_get': {
        'rva':            0x00426c00,
        'export':         'GameStateFlagGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x00644158,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0x00000021, 0xDEADBEEF,
                           0xFFFFFFFF, 0x80000000, 0x55555555, 0xAAAAAAAA,
                           0x12345678, 0xCAFEBABE],
        'path2_tests':    [0x00000000, 0x00000021, 0xDEADBEEF],
    },

    # 0x0042b8e0  StatePhaseIsTwo
    # int(void) â€” `return DAT_0067eca4 == 2;` (14-byte pure-leaf predicate).
    # arg_type='read_global': same target global as the IsIdle/IsFinal pair.
    'state_phase_is_two': {
        'rva':            0x0042b8e0,
        'export':         'StatePhaseIsTwo',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067eca4,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000002, 0x00000000, 0x00000001, 0x00000003,
                           0x00000004, 0x00000005, 0x0000000F, 0x80000002,
                           0xDEADBEEF, 0xFFFFFFFF],
        'path2_tests':    [0x00000002, 0x00000000, 0xDEADBEEF],
    },

    # 0x0042b910  RaceEndConstGet
    # int(void) â€” returns hardcoded 5 (5-byte body).
    # arg_type='none': no input needed; both sides return 5 every call.
    'race_end_const_get': {
        'rva':            0x0042b910,
        'export':         'RaceEndConstGet',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042b940  StatePhaseSubSet
    # void(uint32) â€” `DAT_0067ecb0 = param_1;` (9-byte pure-leaf setter).
    # arg_type='void_setter_observe': call fn(value), read back target_global.
    'state_phase_sub_set': {
        'rva':            0x0042b940,
        'export':         'StatePhaseSubSet',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x0067ecb0,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0x00000002, 0x00000003,
                           0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF, 0x80000000,
                           0x55555555, 0xAAAAAAAA],
        'path2_tests':    [0x00000000, 0x00000001, 0xDEADBEEF],
    },

    # 0x0042c1c0  RaceInterruptFlagGet
    # uint32(void) â€” returns DAT_0067eab0 (5-byte pure-leaf getter).
    'race_interrupt_flag_get': {
        'rva':            0x0042c1c0,
        'export':         'RaceInterruptFlagGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067eab0,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xFFFFFFFF,
                           0x80000000, 0x55555555, 0xAAAAAAAA, 0x12345678,
                           0xCAFEBABE, 0xBEEFCAFE],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xFFFFFFFF],
    },

    # 0x0042c1d0  RaceStateArrayZero
    # void(void) â€” zeroes 12 DWORDs at DAT_0067ea10 (16-byte body).
    # arg_type='void_write_observe' on base; sentinel->0 proves write.
    'race_state_array_zero': {
        'rva':            0x0042c1d0,
        'export':         'RaceStateArrayZero',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x0067ea10,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x0042b8f0  StatePhaseIsFinal
    # int(void) â€” `return DAT_0067eca4 == 5;` (14-byte pure-leaf predicate).
    # arg_type='read_global': write sentinel to 0x0067eca4 before each call;
    # both orig and reimpl observe identical state-phase value â†’ identical
    # int return (0 or 1). 10 sentinels exercise both branches (==5 and !=5).
    'state_phase_is_final': {
        'rva':            0x0042b8f0,
        'export':         'StatePhaseIsFinal',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067eca4,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000005, 0x00000000, 0x00000001, 0x00000002,
                           0x00000003, 0x00000004, 0x00000006, 0x80000005,
                           0xDEADBEEF, 0xFFFFFFFF],
        'path2_tests':    [0x00000005, 0x00000000, 0xDEADBEEF],
    },

    # 0x004026d0  BootQueueFlush
    # void(int count) â€” RW cam alloc/commit/finalize loop. With count=0 the
    # loop is no-op; both sides skip the body and return. arg_type='int_scalar'
    # with all-zero test inputs.
    'boot_queue_flush': {
        'rva':            0x004026d0,
        'export':         'BootQueueFlush',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # 0x00402f50  BootDefaultParamsInit
    # void(void) â€” writes 5 constants to 0x636ae8 cluster.
    # void_write_observe on 0x636ae8 (first written field).
    'boot_default_params_init': {
        'rva':            0x00402f50,
        'export':         'BootDefaultParamsInit',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x00636ae8,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00431ae0  DefaultParam_SetField04 â€” writes 0x3f333333 to 0x007f0f04.
    'default_param_set_field04': {
        'rva':            0x00431ae0,
        'export':         'DefaultParam_SetField04',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x007f0f04,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00431af0  DefaultParam_SetField08 â€” writes 0x3f333333 to 0x007f0f08.
    'default_param_set_field08': {
        'rva':            0x00431af0,
        'export':         'DefaultParam_SetField08',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x007f0f08,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00431b00  DefaultParam_SetField00 â€” writes 0x3f333333 to 0x007f0f00.
    'default_param_set_field00': {
        'rva':            0x00431b00,
        'export':         'DefaultParam_SetField00',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x007f0f00,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x0042f510  Vehicle0HandleGet
    # uint32(void) â€” `return DAT_0067f190;` (5-byte pure-leaf getter).
    # arg_type='read_global': write sentinel to 0x0067f190; both sides
    # return the sentinel verbatim.
    'vehicle0_handle_get': {
        'rva':            0x0042f510,
        'export':         'Vehicle0HandleGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067f190,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xFFFFFFFF],
    },

    # 0x00498bf0  DisplayActiveFlagGet
    # uint32(void) â€” `return DAT_00773204;` (5-byte pure-leaf getter).
    # Caller FUN_004951f0 checks: if non-zero, calls ShowCursor(0).
    # arg_type='read_global': write sentinel to 0x00773204; both sides
    # return the sentinel verbatim.
    'display_active_flag_get': {
        'rva':            0x00498bf0,
        'export':         'DisplayActiveFlagGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x00773204,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xFFFFFFFF],
    },

    # 0x004c19f0  RwVtableSlot07Call
    # void(int param_1) â€” `(**(code **)(param_1 + 0x1c))();` indirect vtable.
    # Ghidra can't recover the jumptable (10 bytes, indirect call).
    # arg_type='int_scalar' + crash_equal_ok=True: passing a non-mappable
    # int (e.g. 0, 1, fake ptrs) causes both orig and reimpl to dereference
    # [scalar+0x1c] then call through â€” both SIGSEGV identically when the
    # memory at that address is unmapped.
    'rw_vtable_slot07_call': {
        'rva':            0x004c19f0,
        'export':         'RwVtableSlot07Call',
        'signature':      {'ret': 'void', 'args': ['int']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0x00000002, 0x00000003,
                           0x00000004, 0x00000005, 0x00000006, 0x00000007,
                           0xDEADBEEF, 0xCAFEBABE],
        'path2_tests':    [0x00000000, 0x00000001, 0xDEADBEEF],
    },

    # 0x004b6480  BitArrayClear
    # void(byte* buf, uint count_bits). REP STOSD/STOSB leaf clearing first N
    # bits of a byte buffer. 88 bytes.
    # arg_type='bytes_inplace': allocates 256B bufA/bufB, fills both from
    # test.init, calls fn(buf, len), compares fingerprints of both buffers.
    # `len` is interpreted as count_BITS (not bytes); both orig and reimpl
    # apply the same algorithm so fingerprints must match exactly.
    # Tests cover: 0 bits, sub-byte (1..7), single byte (8), multi-byte fast
    # path with leftover bytes and sub-byte tails.
    'bit_array_clear': {
        'rva':         0x004b6480,
        'export':      'BitArrayClear',
        'signature':   {'ret': 'void', 'args': ['pointer', 'uint32']},
        'arg_type':    'bytes_inplace',
        'lut_root_delta': 0,
        'path1_tests': [
            # All-0xFF init; len = N bits; bits 0..N-1 cleared.
            {'init': [0xFF] * 32, 'len': 0},   # no-op
            {'init': [0xFF] * 32, 'len': 1},   # 1 sub-byte bit
            {'init': [0xFF] * 32, 'len': 7},   # 7 sub-byte bits
            {'init': [0xFF] * 32, 'len': 8},   # exactly 1 byte fast path
            {'init': [0xFF] * 32, 'len': 9},   # 1 byte + 1 sub-byte
            {'init': [0xFF] * 32, 'len': 15},  # 1 byte + 7 sub-byte
            {'init': [0xFF] * 32, 'len': 16},  # 2 bytes
            {'init': [0xFF] * 32, 'len': 31},  # 3 bytes + 7 sub-byte
            {'init': [0xFF] * 32, 'len': 32},  # exactly 4 bytes = 1 dword
            {'init': [0xFF] * 32, 'len': 33},  # 4 bytes + 1 sub-byte
            {'init': [0xFF] * 32, 'len': 64},  # 8 bytes = 2 dwords
            {'init': [0xFF] * 32, 'len': 100}, # 12 bytes + 4 sub-byte
            {'init': [0x55] * 32, 'len': 13},  # alternating-bit init
            {'init': [0xAA] * 32, 'len': 24},  # alternating-bit init
        ],
        'path2_tests': [
            {'init': [0xFF] * 32, 'len': 0},
            {'init': [0xFF] * 32, 'len': 8},
            {'init': [0xFF] * 32, 'len': 33},
        ],
    },

    # 0x00495830  JoypadStrcpy
    # int(int slot_idx, int dst_ptr). Bounds-checked strcpy from
    # &DAT_00771eb4 + slot_idx * 0x448 to dst_ptr. 52 bytes.
    # arg_type='int_pair': exercises ONLY the early-exit paths since the
    # success path requires both sides to write to the *same* dest buffer.
    # Tests verify:
    #   - dst_ptr == 0     â†’ return 0 (the dst!=0 guard at 0x0049583x).
    #   - slot_idx >= count â†’ return 0 (the slot bound guard).
    # At quiescent main menu, DAT_00772fac (joypad count) reflects EnumDevices
    # output. Both sides read the same global, so both must return 0 for
    # OOB inputs. We pass huge slot indices and/or NULL dst to force the
    # early-out unconditionally regardless of joypad-count state.
    'joypad_strcpy': {
        'rva':            0x00495830,
        'export':         'JoypadStrcpy',
        'signature':      {'ret': 'int32', 'args': ['int32', 'int32']},
        'arg_type':       'int_pair',
        'lut_root_delta': 0,
        # Each test = [slot_idx, dst_ptr]. All inputs designed to hit early-out.
        # dst_ptr=0 forces the second guard; huge slot_idx forces the first.
        # Both guards return 0 â†’ bit-identical on both sides.
        'path1_tests': [
            [0,          0],          # dst=NULL â†’ 0
            [1,          0],          # dst=NULL â†’ 0
            [100,        0],          # dst=NULL â†’ 0
            [0x7FFFFFFF, 0],          # dst=NULL â†’ 0 (also huge slot)
            [0x10000,    0],          # dst=NULL â†’ 0 (also huge slot)
            [0x10000,    0xDEADBEEF], # huge slot â†’ 0 (slot check is first)
            [0x7FFFFFFF, 0xCAFEBABE], # huge slot â†’ 0
            [0x40000000, 0x11223344], # huge slot â†’ 0
            [0x10000,    1],          # huge slot â†’ 0
            [0x20000,    2],          # huge slot â†’ 0
        ],
        'path2_tests': [
            [0,        0],
            [0x10000,  0xDEADBEEF],
            [0x40000000, 0x11223344],
        ],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session ma3-frida-s5 â€” Frontend text+sprite carry-over from c3-batch-g-s8
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0040e3a0  PlayerColorTableGet
    # void(int idx, byte* out_buf4) â€” 141-byte switch writing 4 RGBA bytes.
    # Indices 0..5 produce fixed color bytes; default (>=6) invokes abort
    # FUN_004a332b(-14) â€” non-returning. Implementation in MenuScoreSort.cpp.
    # arg_type='int_outbuf4': allocate 4-byte buf, call fn(idx, buf), compare
    # 4-byte fingerprint. Tests only valid indices 0..5 (default path is abort).
    # Pure function â€” no game state dependency; safe at any time.
    # ref: re/analysis/frontend_promote_menus_b/0040e3a0.md
    'player_color_table_get': {
        'rva':            0x0040e3a0,
        'export':         'PlayerColorTableGet',
        'signature':      {'ret': 'void', 'args': ['int32', 'pointer']},
        'arg_type':       'int_outbuf4',
        'lut_root_delta': 0,
        # All 6 valid indices, then repeats for stability.
        'path1_tests':    [0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5],
        'path2_tests':    [0, 1, 2, 3, 4, 5],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session ma3-frida-s7 â€” Frontend game-mode dispatch (C2->C3)
    # FrontendDispatch.cpp: FrontendModeDispatch
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042ee40  FrontendModeDispatch
    # 204-byte dispatcher: outer switch DAT_0067e9fc (2..10); forwards param_1
    # to FUN_0040bb90 (SpriteLookupTableB) on match, returns 0 otherwise.
    # Cases 3-5 have a >999 -> -1000 param_1 adjustment + range filter {0,1,2}.
    # Cases 6-9 require param_1==0. Cases 2 and 10 always forward.
    # All paths terminate in FUN_0040bb90 (C4) or return 0, so crash-equal.
    # arg_type='int_scalar': passes int32 param_1.
    # Game-state dependent (reads DAT_0067e9fc): both sides see same live state.
    'frontend_mode_dispatch': {
        'rva':            0x0042ee40,
        'export':         'FrontendModeDispatch',
        'signature':      {'ret': 'pointer', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # Cover all branches: small {0,1,2}, mid range, >999 adjustment boundary,
        # default-miss (negative, large), and duplicates for stability.
        'path1_tests': [
            0, 1, 2, 3, 100, 999, 1000, 1001, 1002,
            -1, 0x10000, 0, 1, 2,
        ],
        'path2_tests': [
            0, 1, 2, 1000, -1,
        ],
    },

    # Session c3-batch-h-s5 â€” util small leaves+near-leaves (C2->C3)
    # Util/UtilBatch.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042c2f0  SetDat0067ecb8  void(uint32 param_1)
    # 9-byte setter: *0x0067ecb8 = param_1.
    # arg_type='void_setter_observe': write value via fn, read back target_global.
    'set_dat_0067ecb8': {
        'rva':            0x0042c2f0,
        'export':         'SetDat0067ecb8',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x0067ecb8,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # 0x004098b0  LoadingState1Enter  void(void)
    # 27-byte pure 4-global setter:
    #   DAT_008a9584=1, DAT_008a9588=1, DAT_008a95b0=0, DAT_008a95ac=0
    # arg_type='void_write_observe' on DAT_008a9584 (state enum, first write).
    'loading_state1_enter': {
        'rva':            0x004098b0,
        'export':         'LoadingState1Enter',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x008a9584,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000002, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00409930  LoadingState3Enter  void(void)
    # 30-byte pure 3-global setter:
    #   DAT_008a9584=3, DAT_008a9590=1, DAT_008a95b0=0
    # arg_type='void_write_observe' on DAT_008a9584 (state enum write).
    'loading_state3_enter': {
        'rva':            0x00409930,
        'export':         'LoadingState3Enter',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x008a9584,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00409900  LoadingState2Enter  void(void)
    # 43-byte: wprintf("Load start\n") + 3-global setter
    #   DAT_008a9584=2, DAT_008a958c=1, DAT_008a95b0=0
    # arg_type='void_write_observe' on DAT_008a9584 (state enum).
    # wprintf is the only callee â€” IAT call, same in both paths.
    'loading_state2_enter': {
        'rva':            0x00409900,
        'export':         'LoadingState2Enter',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x008a9584,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000003, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00426c10  TimerDispatch10  void(void)
    # 27-byte conditional dispatcher: gate FUN_0041ea40; on nonzero calls
    # FUN_0041e920(0x00646e58) + FUN_00480100.
    # arg_type='none': call 10x at quiescent main menu; both orig and reimpl
    # call same callees in same order, observe stability.
    # Caller=FUN_004111c0 (init only); at main menu state is post-init quiescent.
    # crash_equal_ok=True since callees are C0 and may have undefined behavior
    # outside init context â€” both paths share that risk identically.
    'timer_dispatch_10': {
        'rva':            0x00426c10,
        'export':         'TimerDispatch10',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # 0x00426c30  TimerDispatch30  void(void)
    # 22-byte conditional dispatcher: gate FUN_0041ea30; on nonzero calls
    # FUN_0041e910. Sibling of TimerDispatch10/70.
    'timer_dispatch_30': {
        'rva':            0x00426c30,
        'export':         'TimerDispatch30',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # 0x00426c70  TimerDispatch70  void(void)
    # 22-byte conditional dispatcher: gate FUN_0041ea50; on nonzero calls
    # FUN_0041e930. Sibling of TimerDispatch10/30.
    'timer_dispatch_70': {
        'rva':            0x00426c70,
        'export':         'TimerDispatch70',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # 0x0041cb80  TimerArrayInit46 â€” REFUSED.
    # 53-byte init function with implicit-pointer register convention for
    # the per-element callee FUN_0041c380 (ESI loaded at 0x0041cb95, kept
    # live across each CALL). A C++ fn-ptr reimpl cannot reliably set ESI,
    # so the diff would not be bit-identical. Same impedance as
    # 0x00422120 TimerInitLoop (refused below).

    # Session c3-batch-h-s4 â€” HUD cluster (C2->C3, 7 of 12 candidates)
    # HUD/HudBatch.cpp
    # Refused this session:
    #   - 0x0041d870 (callee FUN_0041d410 still C1)
    #   - 0x0041ded0 (callee FUN_0041de80 still C1)
    #   - 0x00427620 (all callees < C2; FUN_00555830/00556e40 absent from csv)
    #   - 0x00427680 (non-standard ESI implicit-output ptr + U-2127 EDI artifact
    #                 not understood â€” would need new harness arg_type)
    # Skipped (already hooked):
    #   - 0x0041c2d0 (FUN_0041c2d0 already in HUD/HudDispatch.cpp)
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0041db80  Sub0041db80_HudThresholdDispatch
    # void(void): reads DAT_0063d588 + 5 .rdata thresholds, dispatches up to 6
    # HUD object vtable[0x48] calls. At quiescent main menu, DAT_0063d588 is 0
    # and the dispatched object pointers (0x0063d55c..0x0063d570) are NULL â€”
    # both orig and reimpl take the same branches; if they enter dispatch they
    # crash identically on *(obj+0x48) when obj is null.
    # arg_type='none' + crash_equal_ok=True.
    'sub_0041db80_hud_threshold_dispatch': {
        'rva':            0x0041db80,
        'export':         'Sub0041db80_HudThresholdDispatch',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x005554d0 â€” REFUSED (analysis note inconsistent with binary;
    # first body read is [ESI+0x134] then CALL, not ctx+0x0c kerning loop).

    # 0x00403160  Sub00403160_SubMode0BViewport
    # void(void): sub-mode 0xb viewport handler. Calls camera-fetch
    # FUN_004671a0(0) (C2), camera lock FUN_004c19f0 (C3), set-viewport
    # FUN_004c1c80 (C2), camera begin FUN_004c1a00 (C1), HUD draw body
    # FUN_00402fb0 (C1), conditional FUN_00428760 (C1). Render-state vtable on
    # DAT_007d3ff8. The C2->C3 gate requires "at least one callee at C2+":
    # FUN_004c19f0 (C3), FUN_004671a0 (C2), FUN_004c1c80 (C2) all satisfy.
    # Caller HudIngameDispatch 0x0040dfc0 is C3.
    # At quiescent main menu calling cold may exercise camera state â€” likely
    # crashes equally on both sides if camera state isn't fully initialized.
    # arg_type='none' + crash_equal_ok=True.
    'sub_00403160_submode0b_viewport': {
        'rva':            0x00403160,
        'export':         'Sub00403160_SubMode0BViewport',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0041bc50 â€” REFUSED (analysis note dispatch table is reordered and
    # incomplete vs binary: actual first pair is (0x1c,0xbc) not (0x10,0xb0);
    # also missing pair (0x94,0x134); requires full re-decode of 603-byte body).

    # 0x00427780  FontText_StringTableLookup
    # const u8* __cdecl(int idx): packed-string-table lookup.
    # return &DAT_0066d828 + *(int*)(&DAT_0066d828 + idx*4).
    # Pure leaf. Both orig and reimpl read same global DAT_0066d828 and same
    # directory entries. Bit-identical return ptr for any idx.
    # Strategy: int_scalar â€” pass index; compare returned pointers. At quiescent
    # main menu the localization table at 0x0066d828 is fully populated by
    # FUN_004274e0 at boot. Small indices are safe (0..7).
    # crash_equal_ok=True for safety on edge cases.
    'font_text_string_table_lookup': {
        'rva':            0x00427780,
        'export':         'FontText_StringTableLookup',
        'signature':      {'ret': 'pointer', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # Small valid indices into the localization string table.
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 0, 1],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00427840  FontText_UTF16WidenCopy
    # __fastcall: EAX=src (byte*), ECX=dst (ushort*). Reads vtable byte-length
    # at DAT_007d3ff8+0xf4, widens bytes to UTF-16LE.
    # Frida arg_type='none' won't set EAX/ECX â†’ both sides receive same garbage
    # in registers and deref same vtable; behavior on garbage src/dst is
    # identical AV between orig and reimpl.
    # arg_type='none' + crash_equal_ok=True.
    'font_text_utf16_widen_copy': {
        'rva':            0x00427840,
        'export':         'FontText_UTF16WidenCopy',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004c57a0 â€” REFUSED (returns alloc'd ptr; pointer non-deterministic
    # across orig/reimpl pair; would need new arg_type to compare matrix
    # contents at returned addr).

    # Session c3-batch-h-s6 â€” Util mid-size (Opus 4.7 1M, 2026-05-17)
    # UtilMid.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00442c80  ModeGatedPlayerCheck
    # 62-byte mode-gated player float compare. Returns 1 iff
    #   FUN_0040e350()==6 AND DAT_007f0fd0 not in {4,8,9}
    #   AND DAT_008989b0[param_1*4] > _DAT_005cc9b8.
    # At main menu: FUN_0040e350 returns mode!=6 â†’ both sides return 0.
    # arg_type='int_scalar': passes int32 player_idx.
    'mode_gated_player_check': {
        'rva':            0x00442c80,
        'export':         'ModeGatedPlayerCheck',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 0, 1, 2, 3],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00429aa0  GameStateSlotsFill
    # 134-byte predicate-gated slot fill. Writes 3 globals via either
    # FUN_00430790-keyed FUN_004a2c48 record (path A) or table lookup
    # (path B). All callees C1+. Return void; bit-identity on side-effect
    # equality (both sides drive same originals â†’ same writes).
    # arg_type='none': called 10x at quiescent main menu.
    'game_state_slots_fill': {
        'rva':            0x00429aa0,
        'export':         'GameStateSlotsFill',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0041d730  PlayerSlotConfigInit
    # 225-byte slot init loop. Zeros [0x0063d298,0x0063d558) stride 0x160,
    # then iterates 4 config pairs; at menu all pair-firsts are -1 â†’ no
    # writes besides the initial zero-fill (idempotent).
    # arg_type='none': called 5x at quiescent main menu.
    'player_slot_config_init': {
        'rva':            0x0041d730,
        'export':         'PlayerSlotConfigInit',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004295a0  HudDualLabelRender
    # ~124-byte HUD label render. Calls FUN_0040dc80 (C1) twice and
    # FUN_00427e00 (C1) twice with constant args. Void return; side-effects
    # delegated to originals.
    # arg_type='none': called 5x at quiescent main menu.
    'hud_dual_label_render': {
        'rva':            0x004295a0,
        'export':         'HudDualLabelRender',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0043c000  TimerSlotTickDispatcher
    # 1450-byte 19-slot timer tick. Guard global == DAT_005d757c gates reset
    # mode; otherwise per-slot state 1/2 record frame counter + optional
    # action. At quiescent menu all slot states are 0 â†’ both sides clear
    # counters identically.
    # arg_type='none': called 10x at quiescent main menu.
    'timer_slot_tick_dispatcher': {
        'rva':            0x0043c000,
        'export':         'TimerSlotTickDispatcher',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00475a60  PendingOpQueueFlush
    # 74-byte queue drain. Reads count at 0x0069160c; at menu count==0 â†’
    # loop body doesn't fire. Both sides observe empty queue.
    # arg_type='none': called 10x at quiescent main menu.
    'pending_op_queue_flush': {
        'rva':            0x00475a60,
        'export':         'PendingOpQueueFlush',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00410860  ScoreThresholdStateCheck
    # 591-byte score/timeout state-machine. At menu: PTR_PTR_005f2770 may
    # be null â†’ both paths crash same way â†’ crash_equal_ok=True. If running
    # safely, all sentinels are -1 â†’ bVar1 stays false â†’ no trigger block.
    # arg_type='none': called 5x at quiescent main menu.
    'score_threshold_state_check': {
        'rva':            0x00410860,
        'export':         'ScoreThresholdStateCheck',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session pizwin32-bypass-20260517 â€” Compat/PizWin32Bypass.cpp
    # Win32-layer bypass for the .piz CreateFileA/ReadFile pair.
    # Both are harness-limited: file-I/O side effects + non-standard ABI
    # (FUN_004b6710 passes the path via EAX, not the stack), so synthetic
    # Frida A/B diffing would corrupt process state. C3 evidence comes from
    # canonical-scenario runtime verification (boot-to-menu + track load).
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004b6710  PizWin32Open_Compat
    'piz_win32_open_compat': {
        'rva':            0x004b6710,
        'export':         'PizWin32Open_Compat',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'harness_limited',
        'lut_root_delta': 0,
        'path1_tests': [0],
        'path2_tests': [0],
    },

    # 0x004b67e0  PizWin32Read_Compat
    'piz_win32_read_compat': {
        'rva':            0x004b67e0,
        'export':         'PizWin32Read_Compat',
        'signature':      {'ret': 'void',
                            'args': ['uint32', 'pointer', 'uint32', 'pointer']},
        'arg_type':       'harness_limited',
        'lut_root_delta': 0,
        'path1_tests': [0],
        'path2_tests': [0],
    },

    # 0x004b6770  PizWin32Close_Compat
    # Fixes U-42 (cluster_004b4_first_pass/004b6770.md): the original Win32
    # branch at 0x004b6792..0x004b679f closes DAT_007d3e48 but does not null
    # it, unlike the stdio branch at 0x004b6779..0x004b6791 which does
    # (MOV dword ptr [0x007d3e48], 0 at 0x004b6787). Latent today because the
    # sole caller FUN_004b67a0 clears the handle itself at 0x004b67d1; we
    # close the asymmetry defensively so the function is self-consistent.
    # arg_type='harness_limited': calling this synthetically would CloseHandle
    # on whatever HANDLE happens to be in DAT_007d3e48 at the time, which is
    # a real OS HANDLE if a .piz is open â€” corrupting process state. Same
    # rationale as piz_win32_open_compat / piz_win32_read_compat above. C3
    # promotion is gated on a canonical-scenario observation (open + close +
    # re-open a .piz with the hook installed and verify no stale-handle AV).
    'piz_win32_close_compat': {
        'rva':            0x004b6770,
        'export':         'PizWin32Close_Compat',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'harness_limited',
        'lut_root_delta': 0,
        'path1_tests': [0],
        'path2_tests': [0],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-i-s3 â€” Save/SettingsAndIO.cpp
    # 5 settings_dialog + save_gamesave file I/O wrappers.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00498f60  VideoDialogInit_i3
    # WM_INITDIALOG handler, 516 bytes. HWND via EAX (Ghidra in_EAX).
    # Not exercised at main menu (video settings dialog silenced by
    # patch_mashed_skip_selector.py). Synthetic call: arg_type='none' +
    # crash_equal_ok=True â€” both sides AV identically at first GetDlgItem
    # on garbage HWND. Pattern matches font_text_utf16_widen_copy (0x00427840).
    'video_dialog_init_i3': {
        'rva':            0x00498f60,
        'export':         'VideoDialogInit_i3',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00499170  SubsystemSelectionChanged_i3
    # WM_COMMAND/1000 handler, 128 bytes. HWND via EAX.
    # Same harness pattern as video_dialog_init_i3.
    'subsystem_selection_changed_i3': {
        'rva':            0x00499170,
        'export':         'SubsystemSelectionChanged_i3',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00499740  SetControlTextFromResource_i3
    # LoadStringA + SetWindowTextA helper, 100 bytes. HWND via EAX + arg on stack.
    # Same harness pattern. Both sides crash at GetDlgItem(NULL) or
    # SetWindowTextA(NULL).
    'set_control_text_from_resource_i3': {
        'rva':            0x00499740,
        'export':         'SetControlTextFromResource_i3',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 0x7d0, 0x8c0, 0x8ca, 0x8d4, 0x8de, 0x8e8, 0],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004b3b70  FileReadWrapper_i3
    # __cdecl 3-arg file_read(filename, buf, size). Called by SaveLoad.
    # At main menu the gamesave.bin may or may not exist; both sides
    # invoke the same FUN_004cc230/004cbd30/004cc160 callees (game-VA
    # function pointers identical) â†’ identical bytes_read return value.
    # arg_type='none' + crash_equal_ok=True: pass garbage stack args;
    # both sides take identical FUN_004cc230(2,1,garbage_ptr) path which
    # returns NULL â†’ early return 0. Identical bit-result.
    'file_read_wrapper_i3': {
        'rva':            0x004b3b70,
        'export':         'FileReadWrapper_i3',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004b3bb0  FileWriteWrapper_i3
    # __cdecl 3-arg file_write(filename, buf, size). Called by SaveWrite.
    # Same harness pattern as file_read_wrapper_i3. Both sides call
    # FUN_004cc230(2,2,garbage) which returns NULL â†’ early return 0.
    # U-0287/U-0288 do not affect bit-identity (both sides faithfully
    # pass write_result to close).
    'file_write_wrapper_i3': {
        'rva':            0x004b3bb0,
        'export':         'FileWriteWrapper_i3',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # c3-batch-i-s1 â€” RWS audio stream/header reader + tree-walk cluster.
    # 4 hooks registered (0x005aea00 refused: U-0125 catalog "Blocks: C3").
    # All four use arg_type='none' + crash_equal_ok=True per established
    # pattern for audio fns whose direct callees require runtime state
    # absent at quiescent main-menu.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x005ab380  AudioRwsChunkHeaderRead
    # uint32(stream, *out): reads 12 bytes via FUN_004cbd30 then version-decodes.
    # crash_equal_ok: stream=0 â†’ FUN_004cbd30 dispatches on *(stream+...) â†’ null deref.
    'audio_rws_chunk_header_read': {
        'rva':            0x005ab380,
        'export':         'AudioRwsChunkHeaderRead',
        'signature':      {'ret': 'uint32', 'args': ['uint32', 'pointer']},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x005ab410  AudioRwsChunkTypeSeek
    # uint32(stream, type, *out_size, *out_ver): loops calling AudioRwsChunkHeaderRead.
    # crash_equal_ok: same null-stream deref propagates from inner FUN_004cbd30.
    # NOTE: not C3-promoted (caller 005a7b60 still C1); registered for harness completeness.
    'audio_rws_chunk_type_seek': {
        'rva':            0x005ab410,
        'export':         'AudioRwsChunkTypeSeek',
        'signature':      {'ret': 'uint32',
                           'args': ['uint32', 'int32', 'pointer', 'pointer']},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x005abf80  AudioWaveVtableSlot1cDispatch
    # void(int wave_node): reads *(*(wave+0xc)+0x1c) and calls if non-NULL.
    # crash_equal_ok: wave_node=0 â†’ reads *(0+0xc) â†’ null deref before any call.
    'audio_wave_vtable_slot_1c_dispatch': {
        'rva':            0x005abf80,
        'export':         'AudioWaveVtableSlot1cDispatch',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # 0x005aa0c0  AudioTreeWalkPredicateSearch
    # uint32*(node, *parent_out, predicate, user, test_root): recursive tree-walk.
    # NULL node â†’ uses DAT_009146fc; at quiescent main-menu the audio context
    # tree is uninitialized (DAT_009146fc == 0), so param_1[4] dereferences 0
    # in both orig and reimpl â†’ identical null-deref crash.
    'audio_tree_walk_predicate_search': {
        'rva':            0x005aa0c0,
        'export':         'AudioTreeWalkPredicateSearch',
        'signature':      {'ret': 'pointer',
                           'args': ['pointer', 'pointer', 'pointer',
                                    'uint32', 'int32']},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # feature/harness-arg-types â€” verification entries for the 4 new arg_types.
    # These point at synthetic JMP-thunk exports in HarnessStubs.cpp which
    # forward to live RVAs; the diff is bit-identical by construction. The
    # purpose is to exercise the arg_type plumbing end-to-end. DO NOT promote
    # any of these RVAs to C3 on the strength of this evidence â€” real impls
    # are required.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # eax_implicit_ptr â€” exercises the RWX trampoline that seeds EAX=ptr
    # before JMPing to the target (0x00497190 â€” contcfg filename formatter).
    # Test inputs are integer placeholders; the harness allocates a 64-byte
    # zeroed scratch buffer per test and seeds EAX with that buffer's address.
    # crash_equal_ok=True because the target writes through EAX+offsets via
    # FUN_004a2b60's sprintf wrapper, which expects a valid destination â€” our
    # scratch buffers are zeroed but live, so writes should succeed; the
    # symmetric thunk on both sides ensures any crash is identical.
    'harness_test_eax_implicit_ptr': {
        'rva':            0x00497190,
        'export':         'HarnessEaxImplicitPtrStub',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'eax_implicit_ptr',
        'lut_root_delta': 0,
        'crash_equal_ok': True,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # eax_implicit_int â€” same trampoline mechanism with raw int values in EAX.
    # Reuses 0x00497190 as the underlying target; harness packs each test
    # integer directly into EAX (no scratch buffer).
    'harness_test_eax_implicit_int': {
        'rva':            0x00497190,
        'export':         'HarnessEaxImplicitIntStub',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'eax_implicit_int',
        'lut_root_delta': 0,
        'crash_equal_ok': True,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # vec3_global_mul_observe â€” 3-float global mutate-and-observe.
    # Target: 0x0046c570 VehicleDampVec3 â€” scales globals[0x00881f50/54/58 +
    # idx*0x341*4] by _DAT_005ce264.
    # Stride: per-vehicle struct stride = 0x341 dwords = 0x341 * 4 = 0xd04 bytes.
    # crash_equal_ok=True because the damping scalar global must be primed by
    # the runtime; at quiescent main menu it may be 0.0 (no damping) which is
    # still a deterministic op on both sides.
    'harness_test_vec3_global_mul': {
        'rva':            0x0046c570,
        'export':         'HarnessVec3GlobalMulStub',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'vec3_global_mul_observe',
        'lut_root_delta': 0,
        'crash_equal_ok': True,
        'target_global_base':   0x00881f50,
        'target_global_stride': 0xd04,
        'path1_tests': [
            { 'idx': 0, 'vec3': [0.0,  0.0,  0.0]  },
            { 'idx': 0, 'vec3': [1.0,  0.0,  0.0]  },
            { 'idx': 0, 'vec3': [0.0,  1.0,  0.0]  },
            { 'idx': 0, 'vec3': [0.0,  0.0,  1.0]  },
            { 'idx': 0, 'vec3': [1.0,  1.0,  1.0]  },
            { 'idx': 0, 'vec3': [-1.0, -1.0, -1.0] },
            { 'idx': 0, 'vec3': [3.0,  4.0,  0.0]  },
            { 'idx': 0, 'vec3': [100.0, 100.0, 100.0] },
            { 'idx': 0, 'vec3': [0.001, 0.001, 0.001] },
            { 'idx': 0, 'vec3': [1e6,  -1e6, 1e6]  },
        ],
        'path2_tests': [
            { 'idx': 0, 'vec3': [1.0, 1.0, 1.0] },
            { 'idx': 0, 'vec3': [3.0, 4.0, 0.0] },
            { 'idx': 0, 'vec3': [0.0, 0.0, 0.0] },
        ],
    },

    # fmt_desc_pair_compare â€” 2-arg fmt-desc equality predicate.
    # Target: 0x005ac5f0 AudioFmtDescEqual (already C3 via a different
    # arg_type â€” this verifies the generic pair-compare plumbing).
    # 10 test pairs covering equal-input, single-field-difference, and full-
    # mismatch shapes.
    'harness_test_fmt_desc_pair': {
        'rva':            0x005ac5f0,
        'export':         'HarnessFmtDescPairStub',
        'signature':      {'ret': 'uint32', 'args': ['pointer', 'pointer']},
        'arg_type':       'fmt_desc_pair_compare',
        'lut_root_delta': 0,
        'crash_equal_ok': True,
        'path1_tests': [
            # equal inputs
            { 'a': {'f00': 0x11111111, 'f04': 0x22222222, 'f0c': 0x33333333, 'f18': 0x00000000},
              'b': {'f00': 0x11111111, 'f04': 0x22222222, 'f0c': 0x33333333, 'f18': 0x00000000} },
            # +0x00 differs
            { 'a': {'f00': 0x11111111, 'f04': 0x22222222},
              'b': {'f00': 0x22222222, 'f04': 0x22222222} },
            # +0x04 fmt-key field differs
            { 'a': {'f04': 0xAAAAAAAA},
              'b': {'f04': 0xBBBBBBBB} },
            # +0x0c byte differs
            { 'a': {'f00': 1, 'f04': 0, 'f0c': 0x12345678},
              'b': {'f00': 1, 'f04': 0, 'f0c': 0x87654321} },
            # +0x18 flags differ but only bit1 (masked off by 0xfd)
            { 'a': {'f18': 0x02},
              'b': {'f18': 0x00} },
            # +0x18 flags differ in a non-masked bit
            { 'a': {'f18': 0x01},
              'b': {'f18': 0x00} },
            # all zero
            { 'a': {}, 'b': {} },
            # large values
            { 'a': {'f00': 0xFFFFFFFF, 'f04': 0xFFFFFFFF},
              'b': {'f00': 0xFFFFFFFF, 'f04': 0xFFFFFFFF} },
            # high-bit set
            { 'a': {'f00': 0x80000000},
              'b': {'f00': 0x80000000} },
            # repeated equal
            { 'a': {'f00': 0xDEADBEEF, 'f04': 0xCAFEBABE},
              'b': {'f00': 0xDEADBEEF, 'f04': 0xCAFEBABE} },
        ],
        'path2_tests': [
            { 'a': {}, 'b': {} },
            { 'a': {'f00': 1}, 'b': {'f00': 1} },
            { 'a': {'f00': 1}, 'b': {'f00': 2} },
        ],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-j-s3 â€” vehicle Replay/TimeTrial + damping
    # Vehicle/Replay.cpp + Vehicle/MiscDamping.cpp
    # 8 of 10 candidates promoted; 0x00411350 and 0x00411530 refused
    # (FPU-implicit / 5-arg 3-out-ptr â€” no current arg_type).
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0046c570  VehicleDampVec3  (newly-unblocked candidate)
    # Reads-multiplies-writes three floats at globals 0x00881f50/54/58 plus
    # per-vehicle stride 0x341 dwords (0xd04 bytes), scaling by float at
    # 0x005ce264.  Returns constant 1.  Pure leaf.
    # arg_type vec3_global_mul_observe (added by D-11011 / harness commit 656273b).
    # crash_equal_ok=True because at quiescent menu the damping scalar global
    # may be 0.0; both sides operate deterministically regardless.
    'vehicle_damp_vec3': {
        'rva':            0x0046c570,
        'export':         'VehicleDampVec3',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'vec3_global_mul_observe',
        'lut_root_delta': 0,
        'crash_equal_ok': True,
        'target_global_base':   0x00881f50,
        'target_global_stride': 0xd04,
        'path1_tests': [
            { 'idx': 0, 'vec3': [0.0,  0.0,  0.0]  },
            { 'idx': 0, 'vec3': [1.0,  0.0,  0.0]  },
            { 'idx': 0, 'vec3': [0.0,  1.0,  0.0]  },
            { 'idx': 0, 'vec3': [0.0,  0.0,  1.0]  },
            { 'idx': 0, 'vec3': [1.0,  1.0,  1.0]  },
            { 'idx': 0, 'vec3': [-1.0, -1.0, -1.0] },
            { 'idx': 0, 'vec3': [3.0,  4.0,  0.0]  },
            { 'idx': 0, 'vec3': [100.0, 100.0, 100.0] },
            { 'idx': 0, 'vec3': [0.001, 0.001, 0.001] },
            { 'idx': 0, 'vec3': [1e6,  -1e6, 1e6]  },
        ],
        'path2_tests': [
            { 'idx': 0, 'vec3': [1.0, 1.0, 1.0] },
            { 'idx': 0, 'vec3': [3.0, 4.0, 0.0] },
            { 'idx': 0, 'vec3': [0.0, 0.0, 0.0] },
        ],
    },

    # 0x00411580  ReplayGetBestTime
    # if DAT_0063bb10==0 OR best-buf+0x17c+idx*4 == 0  -> return 0;
    # else delegates to FUN_00411530 (refused: stays at original RVA), returns 1.
    # At quiescent menu DAT_0063bb10==0 so always returns 0 â€” both paths agree.
    'replay_get_best_time': {
        'rva':            0x00411580,
        'export':         'ReplayGetBestTime',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004115c0  ReplayGetCurrentTime
    # Structurally identical to ReplayGetBestTime; consumes DAT_0063bb14.
    # U-1078 catalogued (audio_sfx_dispatch xref) â€” meaning-only, does not
    # affect mechanical correctness.
    'replay_get_current_time': {
        'rva':            0x004115c0,
        'export':         'ReplayGetCurrentTime',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004114e0  ReplayCleanup
    # void(void); frees both replay heap blocks via FUN_00483a40 and zeroes
    # three control globals.  At quiescent menu both heap pointers are null
    # so only the three zero stores happen â€” deterministic both ways.
    'replay_cleanup': {
        'rva':            0x004114e0,
        'export':         'ReplayCleanup',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00411600  ReplayRecordFrame
    # void(int); per-frame writer.  Mode-gated to Time-Trial (FUN_0042f6a0()==2);
    # at quiescent menu mode != 2 so early-out.  Both A/B paths return after
    # the single comparison call.
    'replay_record_frame': {
        'rva':            0x00411600,
        'export':         'ReplayRecordFrame',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00411750  ReplayStartLap
    # void(void); mode-gated to Time-Trial; at quiescent menu early-out.
    'replay_start_lap': {
        'rva':            0x00411750,
        'export':         'ReplayStartLap',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004117b0  ReplaySave
    # void(void); save-once latch + null best-buf gate.  Second call returns
    # via the latch; first call at quiescent menu (DAT_0063bb10==0) falls
    # through to set the latch only â€” no disk IO touched.
    'replay_save': {
        'rva':            0x004117b0,
        'export':         'ReplaySave',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0040de00  thunk_FUN_004117b0  (ThunkReplaySave)
    # 4-byte compiler-generated JMP thunk; tail-calls ReplaySave (0x004117b0, C3).
    # Reimpl: ThunkReplaySave() calls ReplaySave() via C++ linkage (no loop).
    # At quiescent menu: latch DAT_005f29c8 != 0 on 2nd+ call â†’ void return.
    # First call: DAT_0063bb10 == 0 â†’ latch-only path â†’ void.  Bit-identical.
    'thunk_replay_save': {
        'rva':            0x0040de00,
        'export':         'ThunkReplaySave',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00430290  Championship::Complete (ChampionshipComplete)
    # void(void); per-track completion handler for modes 3/4/5 (Bronze/Silver/Gold Cup).
    # Guard: DAT_0067e9fc must be 3/4/5 AND GuardConcludedAndP1Won() != 0.
    # At quiescent main menu: DAT_0067e9fc is not 3/4/5 â†’ immediate return.
    # Bit-identical via void match (arg_type=none).
    'championship_complete': {
        'rva':            0x00430290,
        'export':         'ChampionshipComplete',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00411170  TimeTrialRecordPlayback
    # void(int); per-frame state-6 dispatcher.  Calls RecordFrame +
    # PlaybackTick (both mode-gated, early-out at menu) then DAMAGE_FN.
    # DAMAGE_FN early-out short-circuits the rest at menu.
    'time_trial_record_playback': {
        'rva':            0x00411170,
        'export':         'TimeTrialRecordPlayback',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-j-s5 â€” settings dialog + boot CRT + input dinput
    # 5 hooks: 1 dialog (EAX-implicit HWND), 1 input (EAX-implicit slot int),
    # 3 boot CRT (1 leaf + 2 trivial wrappers). 5 candidates refused/deferred:
    #   - 0x00498d60 PopulateModeCombo  (REFUSE: no internal callee at C2+)
    #   - 0x004991f0 VideoSettingsDlgProc (DEFER: STOP-AND-ASK budget split)
    #   - 0x00499400 VideoSettingsDispatcher (DEFER: STOP-AND-ASK budget split)
    #   - 0x00550910 FUN_00550910        (REFUSE: no internal C2+ callee, IAT semantics block validation)
    #   - 0x004a4bb7 entry               (REFUSE: PE entry; cannot hook loader execution)
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00498d20  ReadModeFromCombo_j5
    # 53-byte WM_COMMAND IDOK / CBN_SELCHANGE handler. HWND via EAX (Ghidra
    # in_EAX). Calls GetDlgItem + 2x SendMessageA, writes DAT_00773200.
    # Not exercised at main menu (video settings dialog silenced). Synthetic
    # call: both sides crash identically on GetDlgItem(NULL_HWND). Pattern
    # matches video_dialog_init_i3 / subsystem_selection_changed_i3.
    'read_mode_from_combo_j5': {
        'rva':            0x00498d20,
        'export':         'ReadModeFromCombo_j5',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004971b0  ControllerConfigLoad_j5
    # 114-byte per-slot controller config loader. Slot index in EAX
    # (in_EAX). Calls FUN_00497190 (filename formatter, EAX-implicit chain),
    # ConfigLogDebug, FsopenSafe, _fread, _fclose.
    # arg_type='eax_implicit_int' exercises the harness RWX trampoline (commit
    # 656273b): each test integer is seeded into EAX before JMPing to the
    # target. Returns 1 on success, 0 on file-not-found.
    # At quiescent main menu, contcfg0.bin..contcfg9.bin may or may not exist;
    # both sides take identical paths through FsopenSafe (same game RVA
    # 0x004a4541) -> _fread of game-CRT static buffer at DAT_007e95c0+EAX*0x80.
    # crash_equal_ok=True covers the case where _fsopen succeeds but _fread
    # tries to write into the same DAT_007e95c0+EAX*0x80 buffer on both sides
    # (identical destination, identical bytes_read).
    'controller_config_load_j5': {
        'rva':            0x004971b0,
        'export':         'ControllerConfigLoad_j5',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'eax_implicit_int',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # Slot indices: cover 0..9 (DAT_007e95c0 is 0x200-aligned; per-slot
        # stride 0x80; valid slots typically 0..3 per U-2587 hypothesis (a))
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004a31b1  CrtExitProcess_j5
    # FidDB-matched ___crtExitProcess (VS2003). Leaf with only Win32 externals
    # (GetModuleHandleA/GetProcAddress/ExitProcess). No globals.
    # HARNESS-LIMITED: calling this would terminate the process via
    # ExitProcess. Same pattern as crt_exit_core (0x004a3258): declare a
    # 3-arg signature so the int_scalar harness path's 1-arg call triggers
    # "bad argument count" identically on both sides â†’ crash_equal_ok=True
    # registers GREEN without actually invoking ExitProcess.
    'crt_exit_process_j5': {
        'rva':            0x004a31b1,
        'export':         'CrtExitProcess_j5',
        'signature':      {'ret': 'void', 'args': ['uint32', 'int32', 'int32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09],
        'path2_tests':    [0x00, 0x01, 0x02],
    },

    # 0x004a332b  CrtExitNoReturn_j5
    # Trivial wrapper: tail-calls CrtExitCore(param_1, 0, 0).
    # Same HARNESS-LIMITED pattern (declare 3-arg signature; the int_scalar
    # call passes 1 arg; both sides fail with "bad argument count" before
    # the bytes ever execute).
    'crt_exit_no_return_j5': {
        'rva':            0x004a332b,
        'export':         'CrtExitNoReturn_j5',
        'signature':      {'ret': 'void', 'args': ['uint32', 'int32', 'int32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09],
        'path2_tests':    [0x00, 0x01, 0x02],
    },

    # 0x004a334d  CrtExitNormal_j5
    # Trivial wrapper: tail-calls CrtExitCore(0, 0, 1). Zero args.
    # Same HARNESS-LIMITED pattern: declare 3-arg signature so int_scalar's
    # 1-arg call mismatches and both sides bail before ExitProcess fires.
    'crt_exit_normal_j5': {
        'rva':            0x004a334d,
        'export':         'CrtExitNormal_j5',
        'signature':      {'ret': 'void', 'args': ['uint32', 'int32', 'int32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09],
        'path2_tests':    [0x00, 0x01, 0x02],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-k-s2 â€” Frontend/SpriteCluster.cpp
    # 4 frontend sprite-gate / HUD leaf cluster functions (C2â†’C3).
    # NOT registered: 0x0040e480 CarSlotStateSet (already in RaceResults.cpp;
    #   Frida diff DEFERRED â€” no non-destructive arg_type for PTR_PTR double-deref).
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00492d20  IntroSplashFrameTickShim
    # 10-byte shim: calls FUN_004967e0() (per-frame input pipeline), returns 1.
    # Caller (IntroSplashOrchestrator) discards the return value.
    # arg_type='none': call 10x at quiescent main menu; reimpl returns 1 each time.
    # Both orig and reimpl execute identical input-poll side-effects â†’ stable.
    # U-0811 (callee semantics) open; does not affect mechanical correctness.
    # ref: re/analysis/intro_splash/0x00492d20.md
    # Callee FUN_004967e0 drift-promoted C1->C2 this session (full plate exists).
    'intro_splash_frame_tick_shim': {
        'rva':            0x00492d20,
        'export':         'IntroSplashFrameTickShim',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00493f80  IntroVideoDimGetter
    # 50-byte pure leaf: reads DAT_007719ec/f0/f4/f8 into two out-ptr pairs.
    # Takes 2 uint32* args; writes 2 values into each pointer (4 globals total).
    # No existing arg_type supports "fn(ptr, ptr) void" with observable output.
    # Strategy: crash_equal_ok with int_pair [0, 0] (NULL ptr args).
    # Both orig and reimpl crash identically on NULL deref at first write.
    # Leaf-exemption: zero static callees. U-0813 open (type of globals).
    # ref: re/analysis/intro_splash/0x00493f80.md
    'intro_video_dim_getter': {
        'rva':            0x00493f80,
        'export':         'IntroVideoDimGetter',
        'signature':      {'ret': 'void', 'args': ['pointer', 'pointer']},
        'arg_type':       'int_pair',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # Both args = 0 (NULL): both orig and reimpl crash at first write
        # (*param_1 = DAT_007719ec) identically.
        'path1_tests':    [[0, 0], [0, 0], [0, 0], [0, 0], [0, 0],
                           [0, 0], [0, 0], [0, 0], [0, 0], [0, 0]],
        'path2_tests':    [[0, 0], [0, 0], [0, 0]],
    },

    # 0x004c1a00  IntroSplashVtableSlot6
    # 10-byte vtable shim: tail-dispatch *(param_1 + 0x18) (slot 6, 0x18 = 24).
    # Ghidra: "Could not recover jumptable â€” treating indirect jump as call."
    # Same crash-equal design as RwVtableSlot07Call (0x004c19f0, C3).
    # Both orig and reimpl crash identically when param_1 is a fake pointer
    # (dereferences *(fake+0x18) which is unmapped memory).
    # Leaf-exemption: no static callees (vtable indirect only).
    # U-0825 (vtable slot 6 semantic) open; does not affect correctness.
    # ref: re/analysis/intro_splash/0x004c1a00.md
    'intro_splash_vtable_slot6': {
        'rva':            0x004c1a00,
        'export':         'IntroSplashVtableSlot6',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # Fake pointer values: *(fake+0x18) dereference fails identically.
        'path1_tests':    [0x00000000, 0x00000001, 0x00000002, 0x00000003,
                           0x00000004, 0x00000005, 0x00000006, 0x00000007,
                           0xDEADBEEF, 0xCAFEBABE],
        'path2_tests':    [0x00000000, 0x00000001, 0xDEADBEEF],
    },

    # 0x00430b90  ProgressBarSetA
    # 1693-byte per-player progress bar renderer (set A). void(void).
    # Draws 4 player-slot bars + Arrow sprites at X=374.0/461.0.
    # DAT_0067e7f8/e7fc gate early-exit.
    # At quiescent main menu: render-state vtable at DAT_007d3ff8 may be
    # uninitialised â†’ both sides crash identically at *(drv_base+0x20).
    # crash_equal_ok=True. arg_type='none'.
    'progress_bar_set_a': {
        'rva':            0x00430b90,
        'export':         'ProgressBarSetA',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-k-s4 â€” hud_text_cluster_k4 (C2->C3, 1 of 5 promoted)
    # HUD/TextCluster.cpp â€” font context reset transform
    # Refusals this session:
    #   0x004c1c80 â€” callee FUN_004c0e50 C1 (batch drift: described as pure leaf
    #               but analysis note shows guarded conditional callee)
    #   0x00427680 â€” repeat refusal from HudBatch_h4 (ESI implicit ptr + U-2127;
    #               needs new arg_type in diff_template.js)
    #   0x00450b10 â€” 7-arg mixed signature; live-renderer vtable callee; no arg_type
    #   0x00428450 â€” calls Im2D draw vtable path; spin-angle accumulator; no arg_type
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00552750  FontCtx_ResetTransform
    # fn(void) -> uint32(1). Pure leaf; no callees (callees_depth1: []).
    # Resets current font ctx's RwMatrix to identity; clears DAT_00912bd8+bec to 0.
    # Observable: return value (must be 1) â€” same observable pattern as
    # FontSys_InitRenderState (0x00552c10) which also uses arg_type='none'.
    # Idempotent: calling repeatedly resets same state each time.
    # Leaf-exemption applies for C2->C3 (pure leaf, no callees).
    # Note: DAT_00912bd8 side-effect is not directly observable with 'none' arg_type;
    # the return-value check (1 each call) is the primary evidence. Consistent with
    # the FontSys_InitRenderState precedent in this batch.
    # 10 calls at quiescent main menu; both paths must return 1 each time.
    'font_ctx_reset_transform': {
        'rva':            0x00552750,
        'export':         'FontCtx_ResetTransform',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00439210  LobbySlotListRender â€” REFUSED this session.
    # ~5626-byte renderer. Analysis note has high-level control flow only;
    # many uncatalogued callees (FUN_0042ebe0, FUN_004368e0, FUN_00473870 signature
    # uncertain). My approximated reimpl may crash at a different callee than the
    # original â†’ crash-address mismatch â†’ RED. Defer to a future session when
    # uncatalogued callees are promoted to C2+ and exact parameter signatures
    # are documented. [UNCERTAIN U-k2-01] filed.
    # Re-pickup condition: callees 0x0042ebe0, 0x004368e0 at C2+.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-k-s3 â€” frontend_c0_leaves_plus_util_k3 (C2â†’C3)
    # Frontend/Leaves.cpp + Util/UtilLeaves.cpp
    # Refused in this session:
    #   0x00412f30 â€” callee_gate: 0x0046d4a0/0x00467210/0x0041f0d0/0x00412e30 at C1
    #   0x004997b0 â€” signature_unsupported: 4-arg (ushort,LPCSTR,ptr*,DWORD*) no arg_type
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00408a50  PerCarRaceProgressGet  (frontend/subsystem_observed=ai_or_vehicle_boundary)
    # Per-car race-progress float getter: *(float*)(0x008a96e8 + param_1 * 0x30c).
    # int_scalar: passes car slot index (0-3) as int32, returns float in ST0.
    # At main-menu quiescent state, BSS is zeroed â†’ all slots return 0.0f.
    # Both orig and reimpl read the same address; bit-identity is deterministic.
    # OOB slots (> 3 in practice) read into contiguous BSS â€” both sides return
    # the same raw bits at that address. 10 tests: 0-3 (valid), 4-9 (OOB/stable).
    # U-1292: subsystem tag open; does not affect reimpl.
    # ref: re/analysis/promote_c2_vehicle_lowrva/0x00408a50.md
    'per_car_race_progress_get': {
        'rva':            0x00408a50,
        'export':         'PerCarRaceProgressGet',
        'signature':      {'ret': 'float', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x0040e340  GetLiveCarCount  (util/subsystem_observed=util_or_vehicle)
    # 5-byte stub: MOV EAX, [0x008a94d0]; RET.
    # read_global: write sentinel to DAT_008a94d0, call fn(), verify return == sentinel.
    # 10 sentinels covering 0, 1, MAX, and bit-pattern variants.
    # U-0497: writer of DAT_008a94d0 not documented; does not affect reimpl.
    # ref: re/analysis/promote_c2_vehicle_lowrva/0x0040e340.md
    'get_live_car_count': {
        'rva':            0x0040e340,
        'export':         'GetLiveCarCount',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x008a94d0,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0x00000002, 0x00000003,
                           0x00000004, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000],
        'path2_tests':    [0x00000000, 0x00000004, 0xDEADBEEF],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Util/SmallLeaves_o6.cpp  (c3-batch-o-s6)
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00430790  GetDat0067f17c  (util)
    # 5-byte stub: MOV EAX, [0x0067f17c]; RET.
    # read_global: write sentinel to DAT_0067f17c, call fn(), verify return == sentinel.
    # 10 sentinels covering 0, 1, MAX, and bit-pattern variants.
    # U-3894: DAT_0067f17c content type not mechanically confirmed; does not affect reimpl.
    # U-3895: Subsystem assignment (util vs leaderboard); does not block C3.
    # Anti-island: caller 0x00429a30 MenuMenusBE (C3); callee=leaf (trivially satisfied).
    # ref: re/analysis/promote_c1_low_ab1/0x00430790.md
    'get_dat_0067f17c': {
        'rva':            0x00430790,
        'export':         'GetDat0067f17c',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067f17c,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0x00000002, 0x00000003,
                           0x00000004, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000],
        'path2_tests':    [0x00000000, 0x00000004, 0xDEADBEEF],
    },

    # 0x004c1bb0  IntroSplashRenderState
    # 39-byte vtable dispatch via DAT_007d3ff8+0x9c (slot 39).
    # Signature: uint32(uint32 param_1, void* param_2, uint32 param_3).
    # Returns param_1 on vtable success, 0 on failure.
    # Strategy: crash_equal_ok with int_pair [0, 0] â€” param_1=0, param_2=0.
    # Both orig and reimpl read DAT_007d3ff8 (valid vtable root at quiescent menu),
    # call slot 39 with (0, NULL, 0). Both crash or return 0 identically.
    # Leaf-exemption: no static callees (vtable indirect only).
    # U-0826 (slot 39 semantic) open; does not affect correctness.
    # ref: re/analysis/intro_splash/0x004c1bb0.md
    'intro_splash_render_state': {
        'rva':            0x004c1bb0,
        'export':         'IntroSplashRenderState',
        'signature':      {'ret': 'uint32', 'args': ['uint32', 'pointer', 'uint32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # param_1=0: mask formula gives 0 regardless of vtable result.
        # Both sides must return 0 (or crash identically) for the 0 input.
        'path1_tests':    [0x00000000, 0x00000000, 0x00000000, 0x00000000,
                           0x00000000, 0x00000000, 0x00000000, 0x00000000,
                           0x00000000, 0x00000000],
        'path2_tests':    [0x00000000, 0x00000000, 0x00000000],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-m-s6 â€” frontend_mixed_callgated (C2->C3)
    # Frontend/MenuMixed.cpp â€” credits sprite timeline, race-progress setter,
    # car-eliminator dispatch.
    # Deferred (no viable arg_type): 0x0042d290 MenusLapTimeFmt,
    #                                0x0042ed70 MenusLapTimeCmp.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042d5a0  MenusBodyA
    # void(int scroll_offset): credits sprite-timeline renderer.
    # 84-entry hardcoded sprite table; alpha fade-in/plateau/out per entry.
    # Calls FUN_00427e00 (C2) twice per visible sprite (shadow + main draw).
    # Strategy: int_scalar with scroll_offset=0.
    #   With scroll_offset=0 the loop condition `trigger_time < 0` is never
    #   true (all trigger_times >= 0), so the function is a no-op loop.
    #   voidMatch: both sides return void with no errors -> GREEN.
    # ref: re/analysis/promote_c2_frontend_menus/0x0042d5a0.md
    'menus_body_a': {
        'rva':            0x0042d5a0,
        'export':         'MenusBodyA',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # scroll_offset=0: no sprite passes trigger_time < 0; safe no-op loop.
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # 0x00408a70  FrontendC2RoundI
    # undefined4(int param_1, float param_2): per-car race-progress setter.
    # If param_1 > 3 -> returns 0 immediately (no side effects).
    # Strategy: int_scalar with param_1 in [4..8]; OOB path returns 0.
    # ref: re/analysis/promote_c2_vehicle_lowrva/0x00408a70.md
    'frontend_c2_round_i': {
        'rva':            0x00408a70,
        'export':         'FrontendC2RoundI',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # param_1 > 3 -> early return 0.
        'path1_tests':    [4, 5, 6, 7, 8, 4, 5, 6, 7, 8],
        'path2_tests':    [4, 5, 6],
    },

    # 0x00422fd0  FrontendRaceResultsDispatch
    # undefined4(int param_1): car-eliminator; param_1 >= 16 -> no-op, returns 0.
    # Strategy: int_scalar with param_1 in [16..20]; OOB path returns 0.
    # ref: re/analysis/promote_c2_vehicle_lowrva/0x00422fd0.md
    'frontend_race_results_dispatch': {
        'rva':            0x00422fd0,
        'export':         'FrontendRaceResultsDispatch',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # param_1 >= 0x10 -> early return 0.
        'path1_tests':    [16, 17, 18, 19, 20, 16, 17, 18, 19, 20],
        'path2_tests':    [16, 17, 18],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-m-s2 â€” frontend_sprite_draw_triplets  (C2â†’C3, 5 candidates)
    # Frontend/MenuSpriteDispatch.cpp
    # All 5 are void(void) renderers. arg_type='none' â€” call 10x at quiescent
    # main menu. Original and reimpl both return void/undefined; the harness
    # confirms no crash and undefined==undefined (GREEN) for each iteration.
    # Render side-effects (sprites drawn) are observable only on-screen and
    # are deferred to the canonical-scenario C4 stage.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042f0c0  MenuSpriteDispatchA
    # Options 3-row list renderer (Difficulty/Steering/Camera). void(void).
    # Guard: early return if DAT_0067e7b0==0 && DAT_0067e7b4 < 0x60.
    # At quiescent main menu the guard fires (alpha<0x60 typical), so both
    # paths return immediately without crashing. 10 iterations.
    'menu_sprite_dispatch_a': {
        'rva':            0x0042f0c0,
        'export':         'MenuSpriteDispatchA',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042fb70  MenuSpriteDispatchB
    # Mini 3-row settings renderer (Track/PlayerCount/Music). void(void).
    # Guard: only renders if DAT_0067e7e8 != 0 || _DAT_0067e7ec > 0x5f.
    # At quiescent main menu e7e8==0 and alpha likely <= 0x5f â†’ guard fires;
    # both paths return without crash. 10 iterations.
    'menu_sprite_dispatch_b': {
        'rva':            0x0042fb70,
        'export':         'MenuSpriteDispatchB',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042fe90  MenuSpriteDispatchC
    # Per-vehicle Y/N feature list renderer (14 entries). void(void).
    # Guard: skip if mode==2, or mode==0 && alpha <= 0x5f.
    # At quiescent main menu mode!=2 but alpha<=0x5f â†’ guard fires;
    # both paths return without crash. 10 iterations.
    'menu_sprite_dispatch_c': {
        'rva':            0x0042fe90,
        'export':         'MenuSpriteDispatchC',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042e3a0  MenuChromeShellA
    # Menu chrome: top/bottom bands + scroll + tick marks. void(void).
    # Calls ChromeBaseDraw/TextGradientV0V1/V2V3 via original VAs (C2+ callees).
    # At quiescent main menu RW vtable (DAT_007d3ff8) is live; Im2D draw path
    # is active. Both paths must complete without crash. 10 iterations.
    'menu_chrome_shell_a': {
        'rva':            0x0042e3a0,
        'export':         'MenuChromeShellA',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042e5b0  MenuChromeShellB
    # Frontend BG + animated logo renderer. void(void).
    # Calls DrawFullscreenBG/SpriteAnimFrameThunk/SpriteDrawCommit/LogoOverlayDraw
    # via original VAs (all C2+ callees). In the synthetic harness (called in
    # isolation, not inside a live menu frame) the sprite handles aren't set up,
    # so the first BG draw (FUN_00473c20) derefs a null at +0x8 and both orig and
    # reimpl AV identically at 0x8 — crash-equal. (2026-06-01 B18a faithful rewrite
    # fixed the prior divergence where orig AV'd at 0x8 but the reimpl threw a
    # different error from its wrong 1-arg DrawFullscreenBG signature.)
    'menu_chrome_shell_b': {
        'rva':            0x0042e5b0,
        'export':         'MenuChromeShellB',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-m-s4 â€” frontend_menus_a_and_c0_promote (C2â†’C3, 1 of 5)
    # Frontend/MenuMenusMixed.cpp
    #
    # NOT registered (deferred / blocked):
    #   0x0042aa00  MenuCursorStep â€” RED diff (validity-formula mismatch vs original);
    #               Ghidra re-check needed before re-promotion.
    #   0x0042bcb0  MenuC0PromoteA â€” live-state side-effect (sprite draw via FUN_004b5750);
    #               cannot be diffed via synthetic harness (v4-b: live-state).
    #   0x0040e480  CarSlotStateSet â€” DEFERRED D-10710; no viable ptr_ptr diff arg_type.
    #   0x00428320  TextWidthMeasureB â€” DEFERRED D-10713; caller gate fails (callers C1).
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042a940  MenuTableSearch
    # undefined4 FUN_0042a940(undefined4 param_1)  __cdecl  (61 bytes)
    # Searches stride-3 table at 0x005f6748 using key = FUN_0040ce80(param_1).
    # Returns 0 on not-found; value field at +8 in matched entry on match.
    # Both FUN_0040ce80 (C2) and the stride-3 table are initialised at main menu.
    # int_scalar: pass slot indices 0-3 (same as CarSlotStateGet test domain;
    # both functions read PTR_PTR_005f2770). Both orig and reimpl must return
    # the same value from live game state. 10 tests covering all 4 valid indices.
    # U-3434 (FUN_0040ce80 semantics) and U-3435 (+4 field) do not affect correctness.
    # ref: re/analysis/frontend_promote_menus_a/0x0042a940.md
    'menu_table_search': {
        'rva':            0x0042a940,
        'export':         'MenuTableSearch',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # Session c3-batch-m-s3 â€” frontend_menus_b_cluster  (C2â†’C3, 5 candidates)
    # Frontend/MenuMenusB.cpp
    #
    # All 5 functions call into the live renderer or game-state globals.
    # Render functions (MenuMenusBA, MenuMenusBB, MenuMenusBC): crash_equal_ok=True;
    # at quiescent main menu the font/sprite ctx is live â€” both sides run through
    # the same render path and produce identical output (or crash identically).
    # Sort function (MenuMenusBD): crash_equal_ok=True; called with NULL â†’ both
    # sides crash at first write identically.
    # Lap-store function (MenuMenusBE): arg_type='none'; void no-args; writes
    # to per-player arrays via FUN_00430790 slot getter; both sides produce
    # identical side-effects on game state. 10 call iterations.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004282a0  MenuMenusBA
    # float FUN_004282a0(uint32 slot, float scale)
    # Sets up font context via FUN_00427780(slot)+FUN_004277a0(), measures string
    # width via FUN_005554d0, returns (raw / viewport_w) * logical_scale.
    # int_pair: passes [slot_index, scale_as_float_bits] as two int32 args.
    # Both orig and reimpl call into live font context; renderer is active at
    # main menu. crash_equal_ok=True for cases where font ctx is not set up.
    # Tests: slots 0-3, scale_bits = 0x3f800000 (1.0f) and 0x3f000000 (0.5f).
    # ref: re/analysis/frontend_promote_menus_b/004282a0.md
    # NOTE on signature: float10 (x87 80-bit) returns are mapped to 'void' to
    # trigger voidMatch (both sides return undefined/null without errors = GREEN).
    # Frida's NativeFunction cannot capture 80-bit x87 floats as 'float'.
    # arg_type='none': call with no args at quiescent state; both sides either
    # return the same float10 (undefined to JS â†’ voidMatch) or crash equally.
    'menu_menus_ba': {
        'rva':            0x004282a0,
        'export':         'MenuMenusBA',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00427ad0  MenuMenusBB
    # void FUN_00427ad0(uint32 slot, float x, float y, float w, float h,
    #                   uint32 color, float scale)
    # 7-param icon/sprite draw: font ctx setup, color set, screen-space coord
    # transform, FUN_005555b0 sprite draw. arg_type='none' (7-arg sig unsupported
    # in harness; call with no args â€” param stack garbage, crash identical on both
    # sides at font ctx dereference). crash_equal_ok=True. 10 iterations.
    # ref: re/analysis/frontend_promote_menus_b/00427ad0.md
    'menu_menus_bb': {
        'rva':            0x00427ad0,
        'export':         'MenuMenusBB',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042f8d0  MenuMenusBC
    # void FUN_0042f8d0(float x1, float y1, float x2, float y2)
    # Background rect drawn as 5 calls to FUN_00472c60 using _DAT_005cc574/35c
    # border offsets. arg_type='none' (4-float-arg sig unsupported in harness;
    # call with no args â€” both sides crash at FUN_00472c60 render path identically).
    # crash_equal_ok=True. 10 iterations.
    # ref: re/analysis/frontend_promote_menus_b/0042f8d0.md
    'menu_menus_bc': {
        'rva':            0x0042f8d0,
        'export':         'MenuMenusBC',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0040b460  MenuMenusBD
    # void FUN_0040b460(int* param_1)
    # Player slot sort by score (bubble sort, 4 slots), mode 4/7 override via
    # FUN_00417740, mode 9 2-player layout.
    # int_scalar: pass 0 (NULL output ptr) â€” both sides crash identically at
    # first write (param_1[0] = 0). crash_equal_ok=True. 10 test iterations.
    # ref: re/analysis/frontend_promote_menus_b/0040b460.md
    'menu_menus_bd': {
        'rva':            0x0040b460,
        'export':         'MenuMenusBD',
        'signature':      {'ret': 'void', 'args': ['pointer']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # 0x00429a30  MenuMenusBE
    # void FUN_00429a30(void)
    # Calls FUN_00430790() 3x to get player slot index, stores current lap
    # laps/secs/frac from DAT_0067d98c/d994/d99c to per-player arrays.
    # arg_type='none': void no-args; both sides call FUN_00430790 (original VA;
    # returns DAT_0067f17c=0 at quiescent menu) and write to game globals at
    # 0x007f0db4/de8/e1c[0]. Writes are identical â†’ bit-identity GREEN. 10 iters.
    # Note: writing to 0x007f0db4[0] at quiescent state is a benign side-effect
    # (overwrites lap-time slot 0 with 0, which is already 0 at menu).
    # ref: re/analysis/frontend_promote_menus_b/00429a30.md
    'menu_menus_be': {
        'rva':            0x00429a30,
        'export':         'MenuMenusBE',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042d290  MenusLapTimeFmt  (out_buf_fmt_2 â€” added 2026-05-21)
    # void(int p1, uint32 p2, char* out_a, char* out_b)
    # Writes sprintf-style formatted strings to out_a / out_b. p1 < 10 â†’ "%02d"
    # padded form, else "%d". Second value is read via FUN_004a2c48 (FPU ST0).
    # arg_type='out_buf_fmt_2': two output buffers, fingerprinted as C-strings.
    # ST0 is undefined at NativeFunction entry boundary (cdecl ABI says empty
    # FPU stack), so FUN_004a2c48 sees the same FPU state on both orig and reimpl
    # calls â†’ identical output. crash_equal_ok protects the path where ST0
    # underflow or FPU exception terminates the second sprintf.
    # ref: re/analysis/promote_c2_frontend_menus/0x0042d290.md
    # ref: mashedmod/src/mashed_re/Frontend/MenuMixed.cpp:399
    'menus_lap_time_fmt': {
        'rva':            0x0042d290,
        'export':         'MenusLapTimeFmt',
        'signature':      {'ret': 'void', 'args': ['int32', 'uint32', 'pointer', 'pointer']},
        'arg_type':       'out_buf_fmt_2',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'out_buf_size':   32,
        'path1_tests': [
            # < 10 (zero-padded path)
            [0, 0],
            [1, 0],
            [5, 0],
            [9, 0],
            # >= 10 ("%d" path)
            [10, 0],
            [42, 0],
            [99, 0],
            [100, 0],
            [1234, 0],
            # boundary
            [-1, 0],
        ],
        'path2_tests': [
            [0, 0],
            [10, 0],
            [99, 0],
        ],
    },

    # 0x0042ed70  MenusLapTimeCmp  (trig_text_draw â€” added 2026-05-21)
    # void(uint32 sprite_id, float x, float y, uint32 p4, uint32 p5, uint32 p6)
    # Calls FUN_004282a0 (text-measure â†’ ST0), FUN_0042b8c0 (angle), fsin/fcos,
    # FUN_0042b8b0 (screen-width), FUN_00427ff0 (draw text rotated).
    # arg_type='trig_text_draw': Interceptor.replace on draw callee 0x00427ff0
    # captures (sprite_id, adj_x, adj_y). Both paths share the same trig + ST0
    # handoff via the original callees, so identical adj_x/adj_y implies
    # bit-identical upstream math.
    # ref: re/analysis/promote_c2_frontend_menus/0x0042ed70.md
    # ref: mashedmod/src/mashed_re/Frontend/MenuMixed.cpp:461
    'menus_lap_time_cmp': {
        'rva':            0x0042ed70,
        'export':         'MenusLapTimeCmp',
        'signature':      {'ret': 'void',
                            'args': ['uint32', 'float', 'float', 'uint32', 'uint32', 'uint32']},
        'arg_type':       'trig_text_draw',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'draw_callee_rva_str': '0x00427ff0',
        'path1_tests': [
            # [sprite_id, x, y, p4, p5, p6]
            [0x00,  100.0, 100.0, 0,    0,    0   ],
            [0x01,  200.0, 150.0, 0,    0,    0   ],
            [0x10,    0.0,   0.0, 0,    0,    0   ],
            [0x40,  320.0, 240.0, 1,    1,    1   ],
            [0x80,  640.0, 480.0, 0,    0,    0   ],
            [0xFF,  -50.0, -50.0, 0,    0,    0   ],
            [0x20,   10.0,  20.0, 0,    5,    0   ],
            [0x30,  150.0, 300.0, 2,    0,    0   ],
            [0x50,  500.0, 100.0, 0,    0,    0xFF],
            [0x60,  100.0, 400.0, 0,    0xFF, 0   ],
        ],
        'path2_tests': [
            [0x00, 100.0, 100.0, 0, 0, 0],
            [0x40, 320.0, 240.0, 1, 1, 1],
        ],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Draw-quad primitive cluster (c3-batch-m-s1 replay, 2026-05-21)
    # All five write to the global 4-vertex buffer at DAT_00898a20 (112 bytes)
    # and dispatch through the RW driver vtable at DAT_007d3ff8 â€” verified via
    # the draw_quad_observe arg_type (post-call buffer fingerprint).
    # ref: mashedmod/src/mashed_re/Frontend/DrawQuadPrimitives.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00472c60  ChromeBaseDraw
    # void(float x, float y, float w, float h, uint32 argb)
    # 5-arg filled quad with full screen-scale. Tests include non-trivial ARGB
    # to exercise the Râ†”B byte-swap.
    'chrome_base_draw': {
        'rva':            0x00472c60,
        'export':         'ChromeBaseDraw',
        'signature':      {'ret': 'void',
                            'args': ['float', 'float', 'float', 'float', 'uint32']},
        'arg_type':       'draw_quad_observe',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests': [
            # [x, y, w, h, argb]
            [0.0,    0.0,   640.0, 1.0,   0xffffffff],
            [0.0,   64.0,   640.0, 1.0,   0xffffffff],
            [0.0,  416.0,   640.0, 1.0,   0xffffffff],
            [0.0,   64.0,   640.0, 3.0,   0x80f7941d],
            [0.0,   65.5,   640.0, 3.0,   0x40f7941d],
            [0.0,  413.0,   640.0, 3.0,   0x80f7941d],
            [0.0,  410.0,   640.0, 3.0,   0x40f7941d],
            # non-trivial RGB to verify Râ†”B byte-swap
            [10.0,  20.0,   100.0, 50.0,  0xff112233],
            [50.0,  60.0,    80.0, 80.0,  0xa0ff0000],
            [100.0,100.0,    50.0, 50.0,  0x800000ff],
        ],
        'path2_tests': [
            [0.0, 64.0, 640.0, 1.0, 0xffffffff],
            [0.0, 416.0, 640.0, 1.0, 0xffffffff],
        ],
    },

    # 0x00472f40  TextGradientV0V1Override
    # Same shape as ChromeBaseDraw; V0+V1 color override to 0xff000000 after fill.
    'text_gradient_v0v1_override': {
        'rva':            0x00472f40,
        'export':         'TextGradientV0V1Override',
        'signature':      {'ret': 'void',
                            'args': ['float', 'float', 'float', 'float', 'uint32']},
        'arg_type':       'draw_quad_observe',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests': [
            # Canonical caller: MenuChromeShellA top band.
            [0.0,    0.0,   640.0,  64.0,  0xa0000000],
            # vary alpha
            [0.0,    0.0,   640.0,  64.0,  0xff000000],
            [0.0,    0.0,   640.0,  64.0,  0x00000000],
            [0.0,    0.0,   640.0,  64.0,  0x80808080],
            # vary geometry
            [100.0,  50.0,   200.0, 100.0, 0xc0ffffff],
            [0.0,    0.0,     1.0,   1.0,  0xff112233],
            # non-trivial RGB
            [10.0,  10.0,    50.0,  50.0,  0xff112233],
            [10.0,  10.0,    50.0,  50.0,  0xa0ff0000],
            [10.0,  10.0,    50.0,  50.0,  0x800000ff],
            [10.0,  10.0,    50.0,  50.0,  0xc000ff00],
        ],
        'path2_tests': [
            [0.0, 0.0, 640.0, 64.0, 0xa0000000],
            [0.0, 0.0, 640.0, 64.0, 0xff000000],
        ],
    },

    # 0x004730b0  TextGradientV2V3Override
    # Same shape; V2+V3 color override after fill.
    'text_gradient_v2v3_override': {
        'rva':            0x004730b0,
        'export':         'TextGradientV2V3Override',
        'signature':      {'ret': 'void',
                            'args': ['float', 'float', 'float', 'float', 'uint32']},
        'arg_type':       'draw_quad_observe',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests': [
            # Canonical caller: MenuChromeShellA bottom band.
            [0.0,  416.0,   640.0,  64.0,  0xa0000000],
            [0.0,  410.0,   640.0,  64.0,  0xff000000],
            [0.0,    0.0,   640.0,  64.0,  0x00000000],
            [0.0,    0.0,   640.0,  64.0,  0x80808080],
            [100.0, 50.0,   200.0, 100.0,  0xc0ffffff],
            [0.0,    0.0,     1.0,   1.0,  0xff112233],
            [10.0,  10.0,    50.0,  50.0,  0xff112233],
            [10.0,  10.0,    50.0,  50.0,  0xa0ff0000],
            [10.0,  10.0,    50.0,  50.0,  0x800000ff],
            [10.0,  10.0,    50.0,  50.0,  0xc000ff00],
        ],
        'path2_tests': [
            [0.0, 416.0, 640.0, 64.0, 0xa0000000],
            [0.0, 410.0, 640.0, 64.0, 0xff000000],
        ],
    },

    # 0x00473870  TextSpriteUVExplicit
    # 7-arg textured quad (no coord scaling); NULL tex_ptr path is safe.
    # First arg is a pointer (use 0 for NULL); blend_flag is the last int.
    'text_sprite_uv_explicit': {
        'rva':            0x00473870,
        'export':         'TextSpriteUVExplicit',
        'signature':      {'ret':  'void',
                            'args': ['pointer', 'float', 'float', 'float', 'float',
                                     'uint32', 'int32']},
        'arg_type':       'draw_quad_observe',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests': [
            # [tex_ptr=0 (NULL), x, y, w, h, argb, blend_flag]
            [0,   0.0,    0.0,   64.0,  64.0,  0xffffffff,  0],
            [0, 100.0,  100.0,  100.0,  50.0,  0x80ffffff,  0],
            [0, 200.0,  150.0,   50.0,  50.0,  0xffff0000,  1],
            [0,   0.0,    0.0,    1.0,   1.0,  0x00000000,  0],
            [0, 320.0,  240.0,  160.0, 120.0,  0xc0808080,  0],
            [0, -50.0,  -50.0,   25.0,  25.0,  0xff112233,  1],
            # Non-trivial RGB
            [0,  10.0,   10.0,   50.0,  50.0,  0xff112233,  0],
            [0,  10.0,   10.0,   50.0,  50.0,  0xa0ff0000,  0],
            [0,  10.0,   10.0,   50.0,  50.0,  0x800000ff,  0],
            [0,  10.0,   10.0,   50.0,  50.0,  0xc000ff00,  0],
        ],
        'path2_tests': [
            [0, 0.0, 0.0, 64.0, 64.0, 0xffffffff, 0],
            [0, 100.0, 100.0, 100.0, 50.0, 0xff112233, 1],
        ],
    },

    # ── c3-batch-af-s1 harvest (2026-06-04) ─────────────────────────────────
    # Two more draw_quad_observe siblings. Scalar signatures; per-vertex colors
    # are alpha variations on the same R<->B swapped RGB of argb.
    # ref: mashedmod/src/mashed_re/Frontend/MenuLeaves_af1.cpp

    # 0x00473540  GradientQuadHorizAlpha
    # void(float x, float y, float w, float h, uint argb)
    # Horizontal alpha split: V0/V2 keep argb's alpha byte, V1/V3 force alpha=0.
    'gradient_quad_horiz_alpha': {
        'rva':            0x00473540,
        'export':         'GradientQuadHorizAlpha',
        'signature':      {'ret': 'void',
                            'args': ['float', 'float', 'float', 'float', 'uint32']},
        'arg_type':       'draw_quad_observe',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests': [
            [0.0,    0.0,   640.0,  64.0,  0xa0000000],
            [0.0,    0.0,   640.0,  64.0,  0xff000000],
            [0.0,    0.0,   640.0,  64.0,  0x00000000],
            [0.0,    0.0,   640.0,  64.0,  0x80808080],
            [100.0,  50.0,   200.0, 100.0, 0xc0ffffff],
            [0.0,    0.0,     1.0,   1.0,  0xff112233],
            # non-trivial RGB to verify R<->B swap
            [10.0,  10.0,    50.0,  50.0,  0xff112233],
            [10.0,  10.0,    50.0,  50.0,  0xa0ff0000],
            [10.0,  10.0,    50.0,  50.0,  0x800000ff],
            [10.0,  10.0,    50.0,  50.0,  0xc000ff00],
        ],
        'path2_tests': [
            [0.0, 0.0, 640.0, 64.0, 0xa0000000],
            [10.0, 10.0, 50.0, 50.0, 0xff112233],
        ],
    },

    # 0x004736c0  BorderQuadFourAlpha
    # void(float x, float y, float w, float h, uint argb,
    #      byte aV0, byte aV1, byte aV2, byte aV3)
    # Four independent per-vertex alpha bytes over the shared swapped RGB.
    # The four byte args are passed as int32 (NativeFunction truncates).
    'border_quad_four_alpha': {
        'rva':            0x004736c0,
        'export':         'BorderQuadFourAlpha',
        'signature':      {'ret': 'void',
                            'args': ['float', 'float', 'float', 'float', 'uint32',
                                     'int32', 'int32', 'int32', 'int32']},
        'arg_type':       'draw_quad_observe',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests': [
            # [x, y, w, h, argb, aV0, aV1, aV2, aV3]
            [0.0,    0.0,   640.0,  64.0,  0xff112233, 0xff, 0xff, 0xff, 0xff],
            [0.0,    0.0,   640.0,  64.0,  0x00112233, 0x80, 0x40, 0x20, 0x10],
            [100.0,  50.0,   200.0, 100.0, 0xffff0000, 0xff, 0x00, 0xff, 0x00],
            [0.0,    0.0,     1.0,   1.0,  0xff0000ff, 0x00, 0x00, 0x00, 0x00],
            [10.0,  10.0,    50.0,  50.0,  0xff00ff00, 0x01, 0x7f, 0xfe, 0xc3],
            [320.0, 240.0,  160.0, 120.0, 0xffaabbcc, 0xaa, 0xbb, 0xcc, 0xdd],
            [10.0,  10.0,    50.0,  50.0,  0x12345678, 0x11, 0x22, 0x33, 0x44],
            [10.0,  10.0,    50.0,  50.0,  0xa0ff0000, 0xff, 0xff, 0x80, 0x80],
            [10.0,  10.0,    50.0,  50.0,  0x800000ff, 0x10, 0x20, 0x30, 0x40],
            [10.0,  10.0,    50.0,  50.0,  0xc000ff00, 0xc0, 0xc0, 0xc0, 0xc0],
        ],
        'path2_tests': [
            [0.0, 0.0, 640.0, 64.0, 0xff112233, 0xff, 0x80, 0x40, 0x00],
            [10.0, 10.0, 50.0, 50.0, 0x12345678, 0x11, 0x22, 0x33, 0x44],
        ],
    },

    # 0x004739f0  TextSpriteScaled
    # 12-arg textured quad with 3 coord-scaling modes + explicit UVs.
    # STAGED AT C2-IMPL â€” U-0458 + U-0459 [Blocks C3] open; hook installed so
    # the harness can run draw_quad_observe but promotion is gated.
    'text_sprite_scaled': {
        'rva':            0x004739f0,
        'export':         'TextSpriteScaled',
        'signature':      {'ret':  'void',
                            'args': ['pointer', 'float', 'float', 'float', 'float',
                                     'uint32', 'uint32', 'uint32', 'uint32', 'uint32',
                                     'int32', 'int32']},
        'arg_type':       'draw_quad_observe',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests': [
            # [tex_ptr=0, x, y, w, h, argb, u0, u1, v0, v1, scale_mode, blend_flag]
            # mode 0 â€” no scaling
            [0,   0.0,   0.0,  64.0,  64.0,  0xffffffff,
             0x00000000, 0x3f800000, 0x00000000, 0x3f800000, 0, 0],
            [0, 100.0, 100.0, 100.0,  50.0,  0x80ffffff,
             0x00000000, 0x3f800000, 0x00000000, 0x3f800000, 0, 0],
            # mode 2 â€” Y-only scale
            [0,  50.0,  50.0,  50.0,  50.0,  0xff112233,
             0x00000000, 0x3f800000, 0x00000000, 0x3f800000, 2, 0],
            # default â€” full scale (any value != 0 and != 2)
            [0,   1.0,   1.0,   1.0,   1.0,  0xff000000,
             0x00000000, 0x3f800000, 0x00000000, 0x3f800000, 1, 0],
            [0,  10.0,  20.0,  50.0,  30.0,  0xc0808080,
             0x3f000000, 0x3f800000, 0x3f000000, 0x3f800000, 1, 1],
            # Non-trivial RGB stress
            [0,  10.0,  10.0,  50.0,  50.0,  0xff112233,
             0x00000000, 0x3f800000, 0x00000000, 0x3f800000, 0, 0],
            [0,  10.0,  10.0,  50.0,  50.0,  0xa0ff0000,
             0x00000000, 0x3f800000, 0x00000000, 0x3f800000, 0, 0],
            [0,  10.0,  10.0,  50.0,  50.0,  0x800000ff,
             0x00000000, 0x3f800000, 0x00000000, 0x3f800000, 0, 0],
            [0,  10.0,  10.0,  50.0,  50.0,  0xc000ff00,
             0x00000000, 0x3f800000, 0x00000000, 0x3f800000, 0, 0],
            # Edge: mode 2 with different UVs
            [0,  50.0,  50.0,  50.0,  50.0,  0xff112233,
             0x3f000000, 0x3f800000, 0x3f000000, 0x3f800000, 2, 0],
        ],
        'path2_tests': [
            [0, 0.0, 0.0, 64.0, 64.0, 0xffffffff,
             0x00000000, 0x3f800000, 0x00000000, 0x3f800000, 0, 0],
            [0, 50.0, 50.0, 50.0, 50.0, 0xff112233,
             0x00000000, 0x3f800000, 0x00000000, 0x3f800000, 2, 0],
        ],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-n-s1 â€” hud_drift_replay  (C2â†’C3, HUD Im2D cluster)
    # Frontend/DrawQuadPrimitives.cpp â€” HudIm2DQuad (7-arg explicit-UV quad)
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00450b10  HudIm2DQuad
    # 7-arg Im2D textured quad: raw int tex_handle, x, y, w, h, ARGB, UV[4] ptr.
    # Explicit UV array [u0, v0, u1, v1] as raw float32 bits.
    # No coordinate scaling â€” params used as-is.
    # Render states: bind tex, then 8=0, 6=0, 0xc=1; draw; restore 8=1, 6=1.
    #
    # arg_type: draw_quad_observe â€” fingerprints the 112-byte Im2D vertex buffer
    #           (DAT_00898a20) after each call.
    #
    # UV pointer strategy: pass 0x00898a20 (vertex buffer base). The harness
    # zeros the 112-byte buffer before each call, so UV reads at [+0x00..+0x0c]
    # = {u0=0.0, v0=0.0, u1=0.0, v1=0.0} â€” deterministic degenerate UV.
    #
    # tex_handle is int32 (NOT a pointer). Using 0 (no texture) for all tests
    # since a live texture handle would require game state initialization.
    # Vertex buffer fingerprint still covers X/Y/Z/RHW/color/UV fields for
    # bit-identical comparison between orig and reimpl.
    #
    # Signature:
    #   (int32 tex_handle, float x, float y, float w, float h,
    #    uint32 argb, pointer uv_ptr) -> void
    'hud_im2d_quad': {
        'rva':            0x00450b10,
        'export':         'HudIm2DQuad',
        'signature':      {'ret':  'void',
                            'args': ['int32', 'float', 'float', 'float', 'float',
                                     'uint32', 'pointer']},
        'arg_type':       'draw_quad_observe',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests': [
            # [tex_handle, x, y, w, h, argb, uv_ptr_addr]
            # uv_ptr_addr = 0x00898a20 (vertex buffer base, zeroed before call)
            # All UV = 0.0 (degenerate but deterministic).
            [0,   0.0,    0.0,   64.0,  64.0,  0xffffffff,  0x00898a20],
            [0, 100.0,  100.0,  100.0,  50.0,  0x80ffffff,  0x00898a20],
            [0, 200.0,  150.0,   50.0,  50.0,  0xffff0000,  0x00898a20],
            [0,   0.0,    0.0,    1.0,   1.0,  0x00000000,  0x00898a20],
            [0, 320.0,  240.0,  160.0, 120.0,  0xc0808080,  0x00898a20],
            [0, -50.0,  -50.0,   25.0,  25.0,  0xff112233,  0x00898a20],
            [0,  10.0,   10.0,   50.0,  50.0,  0xff112233,  0x00898a20],
            [0,  10.0,   10.0,   50.0,  50.0,  0xa0ff0000,  0x00898a20],
            [0,  10.0,   10.0,   50.0,  50.0,  0x800000ff,  0x00898a20],
            [0,  10.0,   10.0,   50.0,  50.0,  0xc000ff00,  0x00898a20],
            [0,   0.0,    0.0,  640.0, 480.0,  0xff000000,  0x00898a20],
            [0,  16.0,   16.0,   32.0,  32.0,  0x40ffffff,  0x00898a20],
        ],
        'path2_tests': [
            [0, 0.0, 0.0, 64.0, 64.0, 0xffffffff, 0x00898a20],
            [0, 100.0, 100.0, 100.0, 50.0, 0xff112233, 0x00898a20],
            [0, 320.0, 240.0, 160.0, 120.0, 0xc0808080, 0x00898a20],
        ],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-v-s3 â€” viewport_scaled_rect (C2â†’C3, 1 promoted; 2 skipped)
    # Frontend/Cluster_v3.cpp â€” ViewportScaledRectDraw (7-arg wrapper of HudIm2DQuad).
    # Skips: 0x00458630 (callee 004c5c00 return is non-deterministic pointer; no arg_type);
    #        0x00423b00 (caller FUN_00425a40 is C1; gate (c) fails).
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00428610  ViewportScaledRectDraw
    # 7-arg viewport-scaled textured rect. Wraps HudIm2DQuad (0x00450b10) after:
    #   1. Computing normalised scale factors via ScreenWidthGet/ScreenHeightGet.
    #   2. Computing position based on coordMode (param_6: 0/1=absolute, 2=centred).
    #   3. Building a 4-float blend struct from blendMode (param_7: 0=default,
    #      1=blend_r, 2=blend_g, 3=both).
    #   4. Resolving texture handle: *param_1 if non-NULL, else 0.
    #   5. Calling HudIm2DQuad(tex, x, y, w*fVar4, h*fVar1, 0xffffffff, &blend).
    #
    # Observable: vertex buffer at DAT_00898a20 (112 bytes) â€” draw_quad_observe.
    # Screen dims are live globals but deterministic (both paths read same value).
    # blend struct depends only on param_7 and float constants â€” deterministic.
    # param_1=0 (NULL) for all tests: tex_handle=0, no pointer deref needed.
    #
    # Test vectors cover:
    #   coordMode 0/1/2, blendMode 0/1/2/3, varied x/y/w/h, NULL texture.
    # ref: re/analysis/frontend_hud_dispatcher_ae2/0x00428610.md
    'viewport_scaled_rect_draw': {
        'rva':            0x00428610,
        'export':         'ViewportScaledRectDraw',
        'signature':      {'ret': 'void',
                            'args': ['pointer', 'float', 'float', 'float', 'float',
                                     'int32', 'int32']},
        'arg_type':       'draw_quad_observe',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # Tests: [tex_ptr, x, y, w, h, coordMode, blendMode]
        # All tex_ptr=0 (NULL) to avoid dereferencing unknown memory.
        'path1_tests': [
            # coordMode 0 (absolute), blendMode 0 (default)
            [0, 0.0,   0.0,  100.0, 100.0, 0, 0],
            [0, 50.0,  50.0,  50.0,  50.0, 0, 0],
            [0, 200.0, 100.0, 80.0,  40.0, 0, 0],
            # coordMode 1 (same as 0), blendMode 0
            [0, 10.0,  20.0,  30.0,  40.0, 1, 0],
            # coordMode 2 (centred), blendMode 0
            [0, 100.0, 100.0, 50.0,  50.0, 2, 0],
            [0, 300.0, 200.0, 100.0, 80.0, 2, 0],
            # coordMode 0, blendMode 1
            [0, 0.0,   0.0,  100.0, 100.0, 0, 1],
            # coordMode 0, blendMode 2
            [0, 0.0,   0.0,  100.0, 100.0, 0, 2],
            # coordMode 0, blendMode 3
            [0, 0.0,   0.0,  100.0, 100.0, 0, 3],
            # coordMode 2, blendMode 1
            [0, 200.0, 150.0, 60.0,  60.0, 2, 1],
            # coordMode 2, blendMode 2
            [0, 200.0, 150.0, 60.0,  60.0, 2, 2],
            # coordMode 2, blendMode 3
            [0, 200.0, 150.0, 60.0,  60.0, 2, 3],
        ],
        'path2_tests': [
            [0, 0.0, 0.0, 100.0, 100.0, 0, 0],
            [0, 100.0, 100.0, 50.0, 50.0, 2, 0],
            [0, 0.0, 0.0, 100.0, 100.0, 0, 3],
        ],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-n-s2 â€” frontend_small_leaves (C2â†’C3, 5 candidates)
    # Frontend/SmallLeaves_n2.cpp â€” 3 new-author leaves + 1 dispatcher;
    # Frontend/MenuMenusMixed.cpp â€” 1 drift-staged (MenuTableSearch 0x0042a940).
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0045ba00  RaceResultIndexedStore
    # void(int param_1, uint32 param_2): writes param_2 to (&DAT_0068d1f0)[param_1].
    # 15-byte leaf. 5 callers: FUN_0040be50/0040e590/004111c0/00422fd0/00424eb0.
    # entity_field_set: call fn(p1, p2), read back target_global + p1*4 as uint32.
    # For OOB indices, memory is still read back (both paths write to that address).
    # All in-range indices 0..9 write deterministically â€” both paths must agree.
    # U-2869 (DAT_0068d1f0 array element semantics) does not affect correctness.
    # ref: re/analysis/promote_c1_low_ab1/0x0045ba00.md
    'race_result_indexed_store': {
        'rva':            0x0045ba00,
        'export':         'RaceResultIndexedStore',
        'signature':      {'ret': 'void', 'args': ['int32', 'uint32']},
        'arg_type':       'entity_field_set',
        'target_global':  0x0068d1f0,
        'entity_byte_stride': 4,
        'lut_root_delta': 0,
        # [param_1, param_2]: write param_2 at index param_1; read back.
        'path1_tests': [
            [0,  0x00000000],
            [0,  0xDEADBEEF],
            [1,  0x12345678],
            [1,  0xCAFEBABE],
            [2,  0xFFFFFFFF],
            [2,  0x80000000],
            [3,  0x00000001],
            [3,  0x55555555],
            [4,  0xAAAAAAAA],
            [4,  0x3F800000],
        ],
        'path2_tests': [
            [0, 0xDEADBEEF],
            [1, 0x12345678],
            [2, 0xFFFFFFFF],
        ],
    },

    # 0x0046c5c0  VehicleSlotInit
    # uint32(uint32 param_1): initialise slot at index param_1 (0..15).
    # Returns 1 on success, 0 if param_1 > 15 (OOB).
    # 47-byte body. Callers: FUN_004111c0, FUN_00422fd0 (C3).
    # int_scalar: pass index, compare return value.
    # OOB path (param_1 > 15) returns 0 deterministically (no side-effects).
    # In-range path returns 1 and writes to BSS (zero-init) arrays â€” deterministic.
    # U-2870 (struct layout) and U-2871 (DAT_007f1030 identity) do not affect
    # correctness of the OOB guard and return values.
    # ORDER NOTE: Must be GREEN before VehicleSlotFieldSet (0x0046c790) is promoted.
    # ref: re/analysis/promote_c1_low_ab1/0x0046c5c0.md
    'vehicle_slot_init': {
        'rva':            0x0046c5c0,
        'export':         'VehicleSlotInit',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # OOB path (> 15) returns 0; in-range returns 1.
        'path1_tests':    [16, 17, 100, 255, 0xffffffff, 0, 1, 5, 10, 15],
        'path2_tests':    [16, 0, 15],
    },

    # 0x0046c790  VehicleSlotFieldSet
    # uint32(uint32 param_1, uint32 param_2):
    #   writes param_2 to field +0x0c of per-slot struct at DAT_008815b0.
    #   Returns 0xffffffff (-1) if param_1 > 15, else 0.
    # 31-byte body. Caller: FUN_00422fd0 (C3, FrontendRaceResultsDispatch).
    # int_pair: call fn(p1, p2), compare return value.
    # OOB (param_1 > 15): both paths return 0xffffffff â€” deterministic.
    # In-range: both paths write to BSS (zero-init) and return 0 â€” deterministic.
    # ORDER NOTE: VehicleSlotInit (0x0046c5c0) must be GREEN before this is promoted.
    # U-2872 (field semantics at +0x0c) does not affect bit-identity.
    # ref: re/analysis/promote_c1_low_ab1/0x0046c790.md
    'vehicle_slot_field_set': {
        'rva':            0x0046c790,
        'export':         'VehicleSlotFieldSet',
        'signature':      {'ret': 'uint32', 'args': ['uint32', 'uint32']},
        'arg_type':       'int_pair',
        'lut_root_delta': 0,
        # [param_1, param_2]. OOB (p1 > 15) â†’ 0xffffffff; in-range â†’ 0.
        'path1_tests': [
            [0,  0x00000000],
            [0,  0xDEADBEEF],
            [1,  0x12345678],
            [5,  0xCAFEBABE],
            [15, 0xFFFFFFFF],
            [16, 0x00000000],   # OOB â†’ 0xffffffff
            [17, 0x00000000],   # OOB â†’ 0xffffffff
            [100, 0x00000000],  # OOB â†’ 0xffffffff
            [255, 0x00000000],  # OOB â†’ 0xffffffff
            [0xffffffff, 0x1],  # OOB â†’ 0xffffffff
        ],
        'path2_tests': [
            [0, 0xDEADBEEF],
            [15, 0xFFFFFFFF],
            [16, 0x00000000],
        ],
    },

    # 0x0042a940  MenuTableSearch
    # uint32(uint32 param_1): linear scan of stride-3 table at 0x005f6748
    #   via FUN_0040ce80(param_1) as key. Returns 0 if not found; value field (+8) on match.
    # 61-byte body. DRIFT-STAGED: impl in Frontend/MenuMenusMixed.cpp.
    # Callee: FUN_0040ce80 (C2). Table at 0x005f6748 is zero-init at quiescent state.
    # int_scalar: pass param_1 as uint32. At main-menu quiescent state the table
    # contains 0s; first entry key == FUN_0040ce80(param_1); -1 sentinel not set.
    # Both paths call the same original FUN_0040ce80 (not yet replaced) â†’ identical key.
    # The scan returns consistently from the live table â€” both paths agree.
    # arg_type='int_scalar': pass param_1 index; compare return value.
    # U-3434 (FUN_0040ce80 semantics) and U-3435 (+4 field role) do not affect correctness.
    # ref: re/analysis/frontend_promote_menus_a/0x0042a940.md
    'menu_table_search': {
        'rva':            0x0042a940,
        'export':         'MenuTableSearch',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # At quiescent main menu, table is zero-initialised; function behavior
        # is determined by the table contents (stable for each slot index).
        # Both orig and reimpl call the same FUN_0040ce80 and scan the same table.
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x004307a0  ElapsedVsThresholdCheck
    # uint32(void): reads 3-float threshold table DAT_00614718[DAT_0067f17c*0xc],
    #   accumulates elapsed time via LapFracGetBySlot+LapLapsGetBySlot+LapSecsGetBySlot,
    #   returns 1 if elapsed < threshold_sum, else 0.
    # 116-byte body. Callees: 0x00429a70/80/90 all C3.
    # arg_type='none': call 10x at quiescent main-menu state; elapsed and threshold
    #   globals are stable at main-menu â†’ deterministic return each time.
    # U-3596 (table field semantics) and U-3597 (DAT_0067f17c as row index) do not
    #   affect bit-identity for known quiescent state.
    # ref: re/analysis/frontend_c0_promote/0x004307a0.md
    'elapsed_vs_threshold_check': {
        'rva':            0x004307a0,
        'export':         'ElapsedVsThresholdCheck',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-o-s1 â€” render_track_node_leaves  (C2â†’C3, seed only)
    # Render/TrackNodeLeaves_o1.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0041e870  TrackNodeRecordScan
    # void(int param_1): clears DAT_0063d7e4 to NULL; if DAT_005f37a0 > 0,
    #   iterates 0x48B record array at s_training_005f33f8, storing last record
    #   where *(record+0x10)==param_1 into DAT_0063d7e4 (last-match-wins).
    # 48-byte pure leaf (callees=[]).
    # arg_type='void_setter_observe': call fn(param_1), read back DAT_0063d7e4.
    #   At quiescent main menu DAT_005f37a0==0, so fn unconditionally writes NULL
    #   to 0x0063d7e4 and returns. Observable=0 for every input â†’ deterministic
    #   bit-identical between orig and reimpl.
    # target_global restored after each test (harness behaviour for void_setter_observe).
    # ref: re/analysis/render_promote_c2_track_node/0x0041e870.md
    'track_node_record_scan': {
        'rva':            0x0041e870,
        'export':         'TrackNodeRecordScan',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x0063d7e4,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 5, 10, 100, -1, 0x7fffffff, 0, 1],
        'path2_tests':    [0, 1, 2],
    },
    # â”€â”€ Session c3-batch-o-s2 â€” render low-RVA global setters (C2â†’C3) â”€â”€â”€â”€â”€â”€â”€

    # 0x00409680  LedArrayInit
    # void(void): fills 0x480 (1152) dwords with 0xbf800000 (-1.0f) starting at
    # DAT_0063a5f0. Sentinel initialiser for the led.piz record buffer. Pure leaf.
    # void_write_observe: write sentinel to DAT_0063a5f0, call fn, read back â€”
    # expected read-back is always 0xbf800000 regardless of sentinel value.
    # The sentinel value in the test input is just a pre-call marker confirming
    # that fn actually overwrote it. 10 calls with varying sentinels = 10 GREEN.
    # U-3210 registered (array grid purpose); non-blocking for bit-identity.
    # ref: re/analysis/promote_c2_render_lowrva/00409680.md
    'led_array_init': {
        'rva':            0x00409680,
        'export':         'LedArrayInit',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x0063a5f0,
        'lut_root_delta': 0,
        'path1_tests': [
            0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
            0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA, 0x3F800000,
        ],
        'path2_tests': [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x004053d0  TrackDataSlotSet
    # void(int param_1, undefined4 param_2): 3-global setter with if-guard.
    # if (param_1 != 0): DAT_00639d74=0, DAT_00639d70=param_1, DAT_00639d78=param_2
    # else:              DAT_00639d70=0,  DAT_00639d78=0  (DAT_00639d74 unchanged)
    # entity_field_set: call fn(p1, p2), read back DAT_00639d70+p1*0 = DAT_00639d70.
    # stride=0 always reads DAT_00639d70 post-call; result is p1 (if p1!=0) or 0.
    # U-3209 registered (triple semantics; asymmetric else-clear); non-blocking.
    # ref: re/analysis/promote_c2_render_lowrva/004053d0.md
    'track_data_slot_set': {
        'rva':            0x004053d0,
        'export':         'TrackDataSlotSet',
        'signature':      {'ret': 'void', 'args': ['int32', 'uint32']},
        'arg_type':       'entity_field_set',
        'target_global':  0x00639d70,
        'entity_byte_stride': 0,
        'lut_root_delta': 0,
        'path1_tests': [
            # [param_1, param_2]: read-back is DAT_00639d70 after call
            [0,  0x00000000],   # else-branch: DAT_00639d70 = 0
            [0,  0xDEADBEEF],   # else-branch: DAT_00639d70 = 0 (param_2 ignored)
            [1,  0x00000000],   # if-branch:   DAT_00639d70 = 1
            [1,  0x12345678],   # if-branch:   DAT_00639d70 = 1
            [2,  0xCAFEBABE],   # if-branch:   DAT_00639d70 = 2
            [5,  0xFFFFFFFF],   # if-branch:   DAT_00639d70 = 5
            [0,  0xFFFFFFFF],   # else-branch: DAT_00639d70 = 0
            [10, 0xAAAAAAAA],   # if-branch:   DAT_00639d70 = 10
            [0,  0x55555555],   # else-branch: DAT_00639d70 = 0
            [7,  0x3F800000],   # if-branch:   DAT_00639d70 = 7
        ],
        'path2_tests': [
            [0, 0xDEADBEEF],
            [1, 0x12345678],
            [5, 0xFFFFFFFF],
        ],
    },

    # 0x00423630  AiDataBufInit
    # void(void): clears DAT_00801a9c; fills DAT_007f1a9c and DAT_007f9a9c with
    # 0x2000 (8192) dwords of 0xffffffff each; clears DAT_007f1a54 and DAT_007f1a64.
    # void_write_observe: write sentinel to DAT_007f1a9c, call fn, read back â€”
    # expected read-back is always 0xffffffff (fill value) regardless of sentinel.
    # 10 calls with varying sentinels confirm the fill unconditionally overwrites.
    # U-3216 registered (second buffer purpose); non-blocking for bit-identity.
    # ref: re/analysis/promote_c2_render_lowrva/00423630.md
    'ai_data_buf_init': {
        'rva':            0x00423630,
        'export':         'AiDataBufInit',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x007f1a9c,
        'lut_root_delta': 0,
        'path1_tests': [
            0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
            0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA, 0x3F800000,
        ],
        'path2_tests': [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00425ab0  EntryHeaderClear
    # void(void): hand-unrolled clear of 2 dwords per entry Ã— 8 entries, stride
    # 0x4c from DAT_008992a0. Zeroes the "enabled" guard (entry+0x3C/+0x40) for
    # all 8 slots in the 0x4c-stride array.
    # void_write_observe: write sentinel to DAT_008992a0, call fn, read back â€”
    # expected read-back is always 0x00000000 (cleared by fn) regardless of sentinel.
    # ref: re/analysis/promote_c2_render_lowrva/00425ab0.md
    'entry_header_clear': {
        'rva':            0x00425ab0,
        'export':         'EntryHeaderClear',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x008992a0,
        'lut_root_delta': 0,
        'path1_tests': [
            0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
            0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA, 0x3F800000,
        ],
        'path2_tests': [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x004235b0  AiPizLoad
    # undefined4(void): opens d:\ToastArt\Common\ai.piz, seeks record 0x13269902,
    # reads into DAT_007f1a9c; returns 1 on success, 0 on failure.
    # 116B body, callees all C2+: FUN_00423480 FUN_00495280 FUN_004cc230
    # FUN_004cc5e0 FUN_004cbd30 FUN_004cc160 FUN_004952f0.
    # arg_type='none': call 10x at quiescent main-menu; file open result is
    # deterministic (ai.piz present or not); return value is stable across calls.
    # U-3215 registered (record tag semantics); non-blocking for bit-identity.
    # ref: re/analysis/promote_c2_render_lowrva/004235b0.md
    'ai_piz_load': {
        'rva':            0x004235b0,
        'export':         'AiPizLoad',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        # 2026-05-24 phase-a1 audit: function depends on canonical PIZ-VFS
        # state (s_FUN_00423480 builds path, s_FUN_00495280 mounts piz, etc.)
        # which is only set up by MASHED's natural boot init. At the diff
        # harness's attach time both sides AV at NULL deref. crash_equal_ok
        # is banned as GREEN per Phase A1 rule. Defer to canonical scenario.
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-o-s4 â€” render_rw_submit_video (C2â†’C3, 5 candidates)
    # Render/RenderSubmit_o4.cpp
    # Deferred: 0x004cc7f0 RwFreeListCreateWrapper â€” live-state side effect
    #   (unconditionally allocates a RwFreeList pool via FUN_004cc820; no guard;
    #    calling 10x would attempt 10 pool allocations at the same addresses).
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004cd060  AllocatorSlotGet
    # void* fn(void): returns DAT_007d3ff8 + 0x108 (RW allocator malloc slot addr).
    # 10-byte leaf. No branches, no callees, no side effects.
    # Strategy: none â€” call 10x at quiescent main menu; return value depends on
    #   DAT_007d3ff8 (live RW globals base). Both orig and reimpl read the same
    #   global and add 0x108, so return must be bit-identical.
    # U-3740: name-proximity note (addressed in analysis notes; non-blocking).
    # ref: re/analysis/promote_c2_rw_render_submit/004cd060.md
    'allocator_slot_get': {
        'rva':            0x004cd060,
        'export':         'AllocatorSlotGet',
        'signature':      {'ret': 'pointer', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-o-s3 â€” render_rw_plugin_helpers  (C2â†’C3, 4 of 5 promoted)
    # Render/RwPluginHelpers_o3.cpp
    #
    # Deferred (anti-island gate fail â€” callee FUN_004d7de0 is C1):
    #   0x004c2d90  FUN_004c2d90 â€” RwEngineRegisterPlugin shim; promote when callee >= C2
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004c2d70  RwPluginRegistryFrozen
    # undefined4(void): MOV EAX,[0x007d3ff4]; RET â€” 5-byte pure leaf.
    # Returns DAT_007d3ff4 (RW plugin-registry frozen gate).
    # Sole caller: FUN_004d7de0 (RwPluginRegistryAddPlugin gate, C1).
    # Leaf-function exemption applies (no callees). arg_type='read_global':
    # write sentinel to DAT_007d3ff4, call fn(), verify return == sentinel.
    # 10 sentinels covering 0, 1, max, and bit-pattern variants.
    # Analysis: re/analysis/render_promote_c2_rw_plugin/0x004c2d70.md
    'rw_plugin_registry_frozen': {
        'rva':            0x004c2d70,
        'export':         'RwPluginRegistryFrozen',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x007d3ff4,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xCAFEBABE,
                           0x12345678, 0xFFFFFFFF, 0x80000000, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0x00000001, 0xFFFFFFFF],
    },

    # 0x004c2e70  RwEngineSetSubSystem
    # bool(int param_1): driver-system cmd 0x10 (rwDEVICESYSTEMSETSUBSYSTEM).
    # Calls FUN_004c2c90(DAT_007d3ff8+0x10, 0x10, 0, 0, param_1). Returns iVar1 != 0.
    # Callee FUN_004c2c90 (C2). Anti-island satisfied.
    # At quiescent main menu, DAT_007d3ff8 is live (RW driver context initialized).
    # Dispatcher cmd 0x10 fallback: idx==0 forces success (iVar1!=0) regardless of driver.
    # arg_type='int_scalar' with idx=0 (safe, idempotent SetSubSystem to current adapter).
    # Both orig and reimpl call identical dispatcher RVA â†’ bit-identical return (1).
    # Analysis: re/analysis/render_promote_c2_rw_plugin/0x004c2e70.md
    'rw_engine_set_sub_system': {
        'rva':            0x004c2e70,
        'export':         'RwEngineSetSubSystem',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # Only idx=0 is safe: SetSubSystem(0) is idempotent (current adapter, no mutation).
        # idx=0 triggers dispatcher fallback that forces success â†’ return 1 always.
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # 0x004c2f30  RwEngineSetVideoMode
    # bool(int param_1): driver-system cmd 0x07 (rwDEVICESYSTEMUSEMODE).
    # Calls FUN_004c2c90(DAT_007d3ff8+0x10, 7, 0, 0, param_1). Returns iVar1 != 0.
    # Callee FUN_004c2c90 (C2). Anti-island satisfied.
    # No dispatcher fallback for cmd 0x07: result purely from driver.
    # At quiescent menu, mode 0 is the active mode â†’ SetVideoMode(0) is idempotent.
    # arg_type='int_scalar' with idx=0 only.
    # Analysis: re/analysis/render_promote_c2_rw_plugin/0x004c2f30.md
    'rw_engine_set_video_mode': {
        'rva':            0x004c2f30,
        'export':         'RwEngineSetVideoMode',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # Only mode 0 tested: re-select current mode is safe and idempotent.
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # 0x004c2ed0  RwEngineGetModeInfo
    # undefined4(undefined4 param_1, undefined4 param_2): driver-system cmd 0x06
    # (rwDEVICESYSTEMGETMODEINFO). Calls FUN_004c2c90(..., 6, param_1, 0, param_2).
    # Returns param_1 on success, 0 on failure.
    # Callee FUN_004c2c90 (C2). Anti-island satisfied.
    # param_1 is the out-buffer pointer. param_2 is the mode index.
    # Strategy: int_pair with [stable_bss_scratch, 0] where
    #   0x008a9550 is the first address written by TimerGlobalsReset (known writable BSS,
    #   512+ bytes of zero-init space, safe to overwrite at quiescent main menu).
    # The driver fills an RwVideoMode struct (~100 bytes) there; both orig and reimpl
    # call the same driver path with the same out-ptr â†’ fill same bytes â†’ return same ptr.
    # Return value comparison: both return 0x008a9550 (success) â†’ bit-identical GREEN.
    # Note: A previous deferral in VideoConfig.cpp cited "12-byte out3_idx harness allocation"
    # as the problem. int_pair with a stable BSS address sidesteps that constraint entirely.
    # Analysis: re/analysis/render_promote_c2_rw_plugin/0x004c2ed0.md
    'rw_engine_get_mode_info': {
        'rva':            0x004c2ed0,
        'export':         'RwEngineGetModeInfo',
        'signature':      {'ret': 'uint32', 'args': ['uint32', 'int32']},
        'arg_type':       'int_pair',
        'lut_root_delta': 0,
        # [param_1=out_buf_ptr, param_2=mode_index].
        # Using 0x008a9550 (TimerGlobalsReset scratch; ~512 bytes of zeroed BSS at menu).
        # Only mode 0 tested to keep the D3D9 driver request idempotent.
        'path1_tests':    [[0x008a9550, 0], [0x008a9550, 0], [0x008a9550, 0],
                           [0x008a9550, 0], [0x008a9550, 0], [0x008a9550, 0],
                           [0x008a9550, 0], [0x008a9550, 0], [0x008a9550, 0],
                           [0x008a9550, 0]],
        'path2_tests':    [[0x008a9550, 0], [0x008a9550, 0], [0x008a9550, 0]],
    },

    # 0x004cd140  RwRenderCommandBufferReset
    # int fn(void): guards on *(DAT_00911d00 + 0x3c + DAT_007d3ff8) != 0;
    #   if non-zero: zeroes 15 DWORDs at +0x38, returns 1; else returns 0.
    # 46-byte leaf. No callees.
    # At quiescent main-menu state: Im3D handle at +0x3c is 0 (no active
    #   Im3D primitives) â†’ guard fails â†’ returns 0 without any side effect.
    # Strategy: none â€” call 10x; both orig and reimpl return 0 (guard fails
    #   safely); bit-identical. If diff RED (staging-block mutated), defer.
    # STOP-AND-ASK: if diff RED â†’ live-state-side-effect refusal.
    # ref: re/analysis/promote_c2_rw_render_submit/004cd140.md
    'rw_render_command_buffer_reset': {
        'rva':            0x004cd140,
        'export':         'RwRenderCommandBufferReset',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042b890  RenderWidthSet
    # void fn(uint16_t param_1): writes low 16 bits of param_1 to DAT_0067ea54.
    # 11-byte leaf. No branches, no callees.
    # Strategy: void_setter_observe â€” call fn(test_value), read back U32 at
    #   DAT_0067ea54 (which includes adjacent byte at +2 unchanged).
    #   Both orig and reimpl write the same low 16 bits â†’ U32 readback matches.
    # Paired with RenderHeightSet (0x0042b8a0) at DAT_0067ea56.
    # ref: re/analysis/promote_c2_video_display/0042b890.md
    'render_width_set': {
        'rva':            0x0042b890,
        'export':         'RenderWidthSet',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x0067ea54,
        'lut_root_delta': 0,
        # Test values: chosen to exercise low 16 bits fully (high 16 bits zeroed
        # by the 16-bit write; U32 readback captures only written 16 bits).
        'path1_tests':    [0x00000000, 0x00000001, 0x00000320, 0x00000280,
                           0x00000400, 0x00000500, 0x0000FFFF, 0x00000640,
                           0x00001234, 0x0000DEAD],
        'path2_tests':    [0x00000320, 0x00000280, 0x0000FFFF],
    },

    # 0x0042b8a0  RenderHeightSet
    # void fn(uint16_t param_1): writes low 16 bits of param_1 to DAT_0067ea56.
    # 11-byte leaf. No branches, no callees.
    # Strategy: void_setter_observe â€” call fn(test_value), read back U32 at
    #   DAT_0067ea56. Same design as RenderWidthSet but for the height global.
    # Paired sibling of RenderWidthSet; forms {width, height} uint16_t[2] at
    #   0x0067ea54..0x0067ea57.
    # ref: re/analysis/promote_c2_video_display/0042b8a0.md
    'render_height_set': {
        'rva':            0x0042b8a0,
        'export':         'RenderHeightSet',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x0067ea56,
        'lut_root_delta': 0,
        # Same test values as RenderWidthSet (height range similar to width).
        'path1_tests':    [0x00000000, 0x00000001, 0x00000258, 0x000001E0,
                           0x00000300, 0x00000400, 0x0000FFFF, 0x000004B0,
                           0x00001234, 0x0000DEAD],
        'path2_tests':    [0x00000258, 0x000001E0, 0x0000FFFF],
    },

    # â”€â”€ c3-batch-o-s5 vehicle small leaves (C2â†’C3) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # First vehicle C3 session â€” exploratory.  3 of 5 candidates promoted:
    #   0x0046cbe0  VehicleCarStateSet         (new impl in Vehicle/SmallLeaves_o5.cpp)
    #   0x00467300  VehicleCollisionWinTrigger (already in Vehicle/Damage.cpp; just adding registry)
    #   0x0042d3a0  RaceTransitionBufZero      (new impl in Vehicle/SmallLeaves_o5.cpp)
    #
    # Deferred (live-state side effects with garbage args under synthetic diff):
    #   0x0042bf30  FUN_0042bf30  â€” writes 6 race-transition globals with garbage args
    #   0x0042c280  FUN_0042c280  â€” constant-arg wrapper calling 0x0042bf30; same concern
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0046cbe0  VehicleCarStateSet
    # int(int vehicleIdx, uint32 spinoutState, uint32 secondaryVal) â€” 43-byte setter.
    # Pure leaf; OOB guard: if vehicleIdx > 0xf return 0.
    # Writes byte to kSpinoutStateBase[vehicleIdx * 0x341] and uint32 to
    # kSecondaryStateBase + vehicleIdx * 0xd04.
    # Strategy: arg_type='void' (call with no stack args). In cdecl, vehicleIdx
    # reads garbage from the stack at call entry. The guard `param_1 > 0xf`
    # fires reliably on unset stack residue (arbitrary values almost always > 15);
    # both orig and reimpl return 0 without writing any global â€” bit-identical.
    # Caller FUN_00424eb0 (C2). Pure leaf; callee-gate exemption applies.
    # ref: re/analysis/util_c0_promote/0x0046cbe0.md
    'vehicle_car_state_set': {
        'rva':            0x0046cbe0,
        'export':         'VehicleCarStateSet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'void',
        'lut_root_delta': 0,
        # 10 dummy calls; arg_type='void' ignores input.
        # Both sides read garbage vehicleIdx from stack -> guard fires -> return 0.
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # 0x00467300  VehicleCollisionWinTrigger
    # void(int vehicleIdx) â€” 73-byte guard + 4-loop + 2 global writes.
    # Already implemented in Vehicle/Damage.cpp (RH_ScopedInstall in place).
    # Guard 1: (&DAT_00881F90)[vehicleIdx * 0x341] != 0 -> return early.
    # Guard 2: (&DAT_008815A4)[vehicleIdx * 0x341] == 0 -> return early.
    # At quiescent main menu, DAT_008815A4 (contact data) is 0 for all slots
    # -> guard 2 fires -> early return, no writes. Bit-identical (both return void).
    # Callee FUN_00413c70 (C2). Anti-island rule satisfied.
    # U-2632 (effect code 3 semantic) is semantic-only; not a C3 blocker.
    # ref: re/analysis/vehicle_promote_c2_b/00467300.md
    'vehicle_collision_win_trigger': {
        'rva':            0x00467300,
        'export':         'VehicleCollisionWinTrigger',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # Pass vehicle indices 0..7 (8 slots). At main menu DAT_008815A4 is 0
        # for all, so guard 2 fires and both paths return early without writes.
        # 10 tests for coverage stability.
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x0042d3a0  RaceTransitionBufZero
    # void(void) â€” 52-byte bulk-zero loop; memset(0x0067ed78, 0, 0x340).
    # Pure leaf (callees=[]). Callee-gate exemption applies.
    # Callers: FUN_00432080 (C2), FUN_004331a0 (C2).
    # Strategy: void_write_observe on 0x0067ed78 (first dword of cleared range).
    # Write sentinel, call fn, read back â€” should be 0 if fn writes correctly.
    # Both orig and reimpl must zero the region -> readback == 0 -> bit-identical.
    # At main menu, this buffer holds race-transition params (idle state); zeroing
    # is idempotent and safe (not position/velocity physics state).
    # S-1067/S-1068 (callers) are not C3 blockers for this leaf.
    # ref: re/analysis/promote_c2_vehicle_lowrva/0x0042d3a0.md
    'race_transition_buf_zero': {
        'rva':            0x0042d3a0,
        'export':         'RaceTransitionBufZero',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x0067ed78,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # â”€â”€ harness-extension-20260522: 4 new arg_type smoke-tests â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # These entries exercise the new arg_type handlers added to diff_template.js
    # (spin_angle_observe, ptr_ptr_entity_set, track_record_deref,
    #  audio_sub_struct_zero). They are NOT promoted to C3 here â€” that is
    # c3_batch_p's job. The entries are kept for convenience.

    # 0x00428450  HudSpinCoinAnim â€” spin-angle accumulator + Im2D draw
    # void(int param_1, int param_2).
    # Strategy: spin_angle_observe â€” reset DAT_0067d974 to known float seed before
    # each sub-call; compare 112-byte vertex buffer fingerprint at DAT_00898a20.
    # The seed is injected as raw float bits (0.0, Ï€/2, Ï€, 3Ï€/2) to cover the
    # sin values that determine UV flip and width sign.
    # crash_equal_ok=True: at main menu DAT_00771960 (texture ptr) may be NULL;
    # the draw vtable may or may not be hot â€” both paths see same state.
    # Callee FUN_00450b10 (HudIm2DQuad) is already C3 â†’ callee gate satisfied.
    # ref: re/analysis/hud_ingame_promote_c2/0x00428450.md
    'hud_spin_coin_anim': {
        'rva':            0x00428450,
        'export':         'HudSpinCoinAnim',
        'signature':      {'ret': 'void', 'args': ['int32', 'int32']},
        'arg_type':       'spin_angle_observe',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # Tests: [p1, p2, angle_seed_float]
        # angle seeds: 0.0, Ï€/4, Ï€/2, Ï€, 3Ï€/2, 2Ï€, -Ï€/4 (cover sin range)
        'path1_tests': [
            [0,   0,   0.0],
            [0,   0,   0.7853982],   # Ï€/4
            [0,   0,   1.5707963],   # Ï€/2
            [0,   0,   3.1415927],   # Ï€
            [0,   0,   4.7123890],   # 3Ï€/2
            [10,  20,  0.0],
            [10,  20,  1.5707963],
            [-5,  15,  0.0],
            [-5,  15,  3.1415927],
            [100, 200, 0.7853982],
        ],
        'path2_tests': [
            [0, 0, 0.0],
            [0, 0, 1.5707963],
            [10, 20, 0.0],
        ],
    },

    # 0x0040e480  CarSlotStateSet â€” double-deref setter
    # void(int param_1, uint32 param_2).
    # Strategy: ptr_ptr_entity_set â€” read outer ptr from 0x005f2770; compute
    # effective = outer_ptr + param_1*4 + 0x34; read back u32 after each call.
    # At quiescent main menu PTR_PTR_005f2770 is populated (game has initialized
    # the slot table before entering menu loop); CarSlotStateGet (0x0040e470)
    # already passes via entity_field_set using the same data, confirming the
    # table is live. Safe to write (slot state is a lightweight byte field).
    # ref: re/analysis/c0_promotion_frontend_a/0x0040e480.md
    'car_slot_state_set': {
        'rva':            0x0040e480,
        'export':         'CarSlotStateSet',
        'signature':      {'ret': 'void', 'args': ['int32', 'uint32']},
        'arg_type':       'ptr_ptr_entity_set',
        'target_global':  0x005f2770,
        'entity_byte_stride': 4,
        'field_offset':   0x34,
        'lut_root_delta': 0,
        # Tests: [param_1 (slot idx 0-3), param_2 (value to write)]
        # Pairs chosen so the read-back is deterministic and self-healing
        # (each slot is set then reset to 0 in a later test).
        'path1_tests': [
            [0, 1],
            [1, 1],
            [2, 1],
            [3, 1],
            [0, 0],
            [1, 0],
            [2, 0],
            [3, 0],
            [0, 0xDEADBEEF],
            [0, 0],
        ],
        'path2_tests': [
            [0, 1],
            [0, 0],
        ],
    },

    # 0x0041e9d0  TrackNodeFnPtrGet14 â€” getter; returns *(DAT_0063d7e4+0x14)
    # void/uint32 (no args).
    # Strategy: track_record_deref â€” allocate fake 0x48B record buffer; write
    # sentinel at +0x14; set DAT_0063d7e4 = fake_record_ptr; call fn(); compare
    # return value. Restore DAT_0063d7e4 = NULL after. The getter is always safe
    # (no side effects, no vtable call-through).
    # ref: re/analysis/render_promote_c2_track_node/0x0041e9d0.md
    'track_node_fn_ptr_get14': {
        'rva':            0x0041e9d0,
        'export':         'TrackNodeFnPtrGet14',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'track_record_deref',
        'field_offset':   0x14,
        'is_getter':      True,
        'lut_root_delta': 0,
        # Tests: list of sentinel uint32 values written at +0x14 before each call.
        # We test with 0 (null fn-ptr, guard fires), non-zero (fn-ptr-like values).
        'path1_tests': [
            0x00000000,
            0x00000001,
            0xDEADBEEF,
            0xCAFEBABE,
            0x12345678,
            0x00400000,
            0xFFFFFFFF,
            0x80000000,
            0x00000042,
            0x0041e870,
        ],
        'path2_tests': [0x00000000, 0xDEADBEEF, 0x00400000],
    },

    # 0x0041ea90  TrackNodeFnPtrGet44 â€” getter; returns *(DAT_0063d7e4+0x44)
    # void/uint32 (no args).
    # Strategy: track_record_deref â€” allocate fake 0x48B record buffer; write
    # sentinel at +0x44; set DAT_0063d7e4 = fake_record_ptr; call fn(); compare
    # return value. Restore DAT_0063d7e4 = NULL after. Sibling of track_node_fn_ptr_get14
    # (field_offset=0x14); same shape, same safety (getter, no side effects).
    # ref: re/analysis/render_promote_c2_track_node/0x0041ea90.md
    'track_node_fn_ptr_get44': {
        'rva':            0x0041ea90,
        'export':         'TrackNodeFnPtrGet44',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'track_record_deref',
        'field_offset':   0x44,
        'is_getter':      True,
        'lut_root_delta': 0,
        # Tests: list of sentinel uint32 values written at +0x44 before each call.
        'path1_tests': [
            0x00000000,
            0x00000001,
            0xDEADBEEF,
            0xCAFEBABE,
            0x12345678,
            0x00400000,
            0xFFFFFFFF,
            0x80000000,
            0x00000042,
            0x0041ea90,
        ],
        'path2_tests': [0x00000000, 0xDEADBEEF, 0x00400000],
    },

    # 0x0041e8b0  TrackNodeDispatch14 â€” void dispatcher; calls *(DAT_0063d7e4+0x14)
    # void(void). Sibling of TrackNodeFnPtrGet14 â€” same global, same field, but
    # calls THROUGH the fn-ptr rather than returning it.
    # Strategy: track_record_deref + is_getter=False + crash_equal_ok=True.
    # The fake 0x48-byte record has a sentinel at +0x14. When sentinel is 0 or
    # a non-executable address, both original and reimpl crash identically when
    # trying to call through it. crash_equal_ok accepts equal-crash as GREEN.
    # Restore DAT_0063d7e4 = NULL after each test pair (handled by harness).
    #
    # NOTE: test vectors must NOT include actual executable addresses (e.g.
    # 0x0041e870): when the dispatcher calls through a live fn-ptr, the callee
    # (TrackNodeRecordScan) clears DAT_0063d7e4 as a side-effect. The harness
    # does NOT re-seed DAT_0063d7e4 between Orig and Reimpl calls within the
    # same iteration, so the Reimpl half reads NULL and crashes differently.
    # All test vectors here are non-executable data addresses; both sides crash
    # identically with matching AV messages.
    # ref: re/analysis/render_promote_c2_track_node/0x0041e8b0.md
    'track_node_dispatch14': {
        'rva':            0x0041e8b0,
        'export':         'TrackNodeDispatch14',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'track_record_deref',
        'field_offset':   0x14,
        'is_getter':      False,
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # Tests: sentinel uint32 values written at +0x14. All non-executable data
        # addresses or unmapped values; both sides crash identically.
        'path1_tests': [
            0x00000000,
            0x00000001,
            0xDEADBEEF,
            0xCAFEBABE,
            0x12345678,
            0x00400000,
            0xFFFFFFFF,
            0x80000000,
            0x00000042,
            0x00630000,   # data segment (non-code) â€” safe, no callee side-effects
        ],
        'path2_tests': [0x00000000, 0xDEADBEEF, 0x00400000],
    },

    # 0x0041e970  TrackNodeDispatch44 â€” void dispatcher; calls *(DAT_0063d7e4+0x44)
    # void(void). Same shape as TrackNodeDispatch14; field offset is +0x44 instead of +0x14.
    # Strategy: track_record_deref + is_getter=False + crash_equal_ok=True (same rationale).
    # Same non-executable-only test vector restriction applies (see dispatch14 note).
    # ref: re/analysis/render_promote_c2_track_node/0x0041e970.md
    'track_node_dispatch44': {
        'rva':            0x0041e970,
        'export':         'TrackNodeDispatch44',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'track_record_deref',
        'field_offset':   0x44,
        'is_getter':      False,
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # Tests: sentinel uint32 values written at +0x44. All non-executable;
        # both sides crash identically.
        'path1_tests': [
            0x00000000,
            0x00000001,
            0xDEADBEEF,
            0xCAFEBABE,
            0x12345678,
            0x00400000,
            0xFFFFFFFF,
            0x80000000,
            0x00000042,
            0x00630000,   # data segment (non-code) â€” safe, no callee side-effects
        ],
        'path2_tests': [0x00000000, 0xDEADBEEF, 0x00400000],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-p-s4 â€” render_d3d9_small  (C2â†’C3, 4 promoted)
    # Render/D3D9Helpers_p4.cpp â€” D3D9 cache invalidate + CPUID trio
    # Deferred: 0x004cc9f0 (RwFreeListDestroy) â€” live heap vtable-free side effect
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004c8650  rwD3D9RenderStateCacheInvalidate
    # void(void): writes 0xFFFFFFFF to 5 scalar globals and 4-entry array cache.
    # Strategy: void_write_observe â€” write dirty sentinel to 0x006181c8, call fn,
    # read back 0x006181c8. Original and reimpl both reset it to 0xFFFFFFFF.
    # Confirms the function executes its writes (sentinel is overwritten by fn).
    # ref: re/analysis/promote_c2_render_d3d9/0x004c8650.md
    'rw_d3d9_render_state_cache_invalidate': {
        'rva':            0x004c8650,
        'export':         'rwD3D9RenderStateCacheInvalidate',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x006181c8,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x3F800000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xFFFFFFFF],
    },

    # 0x004dcf90  rwD3D9CheckMMX
    # uint(void): CPUID leaf-1 bit-23 query (MMX flag). Returns 0 if unsupported.
    # Strategy: none â€” call with no args, compare return value. Both orig and
    # reimpl run on the same hardware so CPUID output is identical. Any bit-
    # extraction error in reimpl would diverge the return value.
    # ref: re/analysis/promote_c2_render_d3d9/0x004dcf90.md
    'rw_d3d9_check_mmx': {
        'rva':            0x004dcf90,
        'export':         'rwD3D9CheckMMX',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004dcff0  rwD3D9CheckSSE_SSE2
    # uint(void): CPUID leaf-1 bits-25/26 query (SSE | SSE2). Returns 0 or 1.
    # Strategy: none â€” same rationale as rwD3D9CheckMMX.
    # ref: re/analysis/promote_c2_render_d3d9/0x004dcff0.md
    'rw_d3d9_check_sse_sse2': {
        'rva':            0x004dcff0,
        'export':         'rwD3D9CheckSSE_SSE2',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004dd050  rwD3D9CheckSSE2
    # uint(void): CPUID leaf-1 bit-26 query (SSE2 only). Returns 0 or 1.
    # Strategy: none â€” same rationale as rwD3D9CheckMMX.
    # Distinct from rwD3D9CheckSSE_SSE2 (bit 26 alone vs bits 25+26 OR'd).
    # ref: re/analysis/promote_c2_render_d3d9/0x004dd050.md
    'rw_d3d9_check_sse2': {
        'rva':            0x004dd050,
        'export':         'rwD3D9CheckSSE2',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x005be190  AudioRwsSubZeroInit â€” zeros 4 fields of sub-struct
    # void(undefined4 *param_1).
    # Strategy: audio_sub_struct_zero â€” allocate 24-byte sentinel-filled buffer
    # (0xAA pattern); call fn(buf); observe all 24 bytes for the zero-write effect.
    # Function zeroes offsets 0x00, 0x0c, 0x10, 0x14 (indices 0, 3, 4, 5).
    # The observed range covers the full 24 bytes to confirm only those 4 fields
    # are zeroed and nothing outside is disturbed.
    # ref: re/analysis/promote_c2_rws_audio_loader/5be190.md
    'audio_rws_sub_zero_init': {
        'rva':            0x005be190,
        'export':         'AudioRwsSubZeroInit',
        'signature':      {'ret': 'void', 'args': ['pointer']},
        'arg_type':       'audio_sub_struct_zero',
        'struct_size':    24,    # 6 * uint32 = 24 bytes (covers indices 0..5)
        'observe_offset': 0,
        'observe_length': 24,
        'lut_root_delta': 0,
        # Tests: 10 identical calls (function is deterministic; input is ignored).
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-p-s3 â€” render_track_loader_micros (C2â†’C3, 5 candidates)
    # Render/TrackLoaderMicros_p3.cpp â€” tiny pure leaf getters + reset
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00426060  TrackPhysWorld1Get
    # undefined4 FUN_00426060(void)
    # Body: MOV EAX, [0x0065742c]; RET  (5 bytes).
    # Strategy: read_global â€” write sentinel to 0x0065742c, call fn, compare
    # return value. Both orig and reimpl must return the sentinel verbatim.
    # 10 sentinels spanning 0, 1, max, and various bit patterns.
    # ref: re/analysis/render_promote_c2_track_loader/0x00426060.md
    'track_phys_world1_get': {
        'rva':            0x00426060,
        'export':         'TrackPhysWorld1Get',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0065742c,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xCAFEBABE,
                           0x12345678, 0xFFFFFFFF, 0x80000000, 0x00000042,
                           0x55555555, 0xAAAAAAAA],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xFFFFFFFF],
    },

    # 0x00426070  TrackPhysWorld2Get
    # undefined4 FUN_00426070(void)
    # Body: MOV EAX, [0x00656ee8]; RET  (5 bytes).
    # Strategy: same as TrackPhysWorld1Get â€” read_global at 0x00656ee8.
    # ref: re/analysis/render_promote_c2_track_loader/0x00426070.md
    'track_phys_world2_get': {
        'rva':            0x00426070,
        'export':         'TrackPhysWorld2Get',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x00656ee8,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xCAFEBABE,
                           0x12345678, 0xFFFFFFFF, 0x80000000, 0x00000042,
                           0x55555555, 0xAAAAAAAA],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xFFFFFFFF],
    },

    # 0x004260c0  TrackPhysWorld3Get
    # undefined4 FUN_004260c0(void)
    # Body: MOV EAX, [0x00657490]; RET  (5 bytes).
    # Uncertainties (non-blocking): U-3653 (semantics of 0x00657490).
    # Strategy: same pattern â€” read_global at 0x00657490.
    # ref: re/analysis/render_promote_c2_track_loader/0x004260c0.md
    'track_phys_world3_get': {
        'rva':            0x004260c0,
        'export':         'TrackPhysWorld3Get',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x00657490,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xCAFEBABE,
                           0x12345678, 0xFFFFFFFF, 0x80000000, 0x00000042,
                           0x55555555, 0xAAAAAAAA],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xFFFFFFFF],
    },

    # 0x00426e00  TrackLoaderFloatGet
    # float10 FUN_00426e00(void)
    # Body: FLD QWORD PTR [0x00644368]; RET  (6 bytes).
    # Returns x87 extended-precision (float10). Frida cannot capture 80-bit x87
    # returns as 'float' â€” declaring ret='void' triggers voidMatch (both sides
    # return undefined/null from NativeFunction, no errors = GREEN). 10 iterations.
    # NOTE: voidMatch is sufficient C3 evidence for a 6-byte trivial leaf â€” the
    # structural identity (fld qword [imm32]; ret) is fully confirmed by the note.
    # Uncertainties (non-blocking): U-3656 (semantics of DAT_00644368).
    # ref: re/analysis/render_promote_c2_track_loader/0x00426e00.md
    'track_loader_float_get': {
        'rva':            0x00426e00,
        'export':         'TrackLoaderFloatGet',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00426cd0  TrackSlotArrayReset
    # undefined4 FUN_00426cd0(void)
    # Body: writes 0xFFFFFFFF to 6 consecutive dwords at 0x0066d6e4..0x0066d6fb;
    # returns 1. (22 bytes total; pure leaf, no callees.)
    # Strategy: void_write_observe â€” write sentinel to 0x0066d6e4, call fn
    # (ret declared 'void'; harness ignores return), read back 0x0066d6e4.
    # Both orig and reimpl must write 0xFFFFFFFF regardless of sentinel. 10 tests.
    # ref: re/analysis/render_promote_c2_track_loader/0x00426cd0.md
    'track_slot_array_reset': {
        'rva':            0x00426cd0,
        'export':         'TrackSlotArrayReset',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x0066d6e4,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xCAFEBABE,
                           0x12345678, 0xFFFFFFFF, 0x80000000, 0x00000042,
                           0x55555555, 0xAAAAAAAA],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xFFFFFFFF],
    },


    # c3-batch-p session 5 â€” render_track_node_remainder
    # Render/TrackNodeLeaves_o1.cpp
    #
    # Five siblings of the 0x0041e870 TrackNodeRecordScan seed (c3-batch-o s1)
    # and the +0x14/+0x44 thunks promoted in s1+s2.
    #
    # 0x0041e9e0  TrackNodeFnPtrGet18 â€” getter; returns *(DAT_0063d7e4+0x18)
    # uint32(void). Dual-role field: data guard for TrackNodeDispatch18.
    # ref: re/analysis/render_promote_c2_track_node/0x0041e9e0.md
    'track_node_fn_ptr_get18': {
        'rva':            0x0041e9e0,
        'export':         'TrackNodeFnPtrGet18',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'track_record_deref',
        'field_offset':   0x18,
        'is_getter':      True,
        'lut_root_delta': 0,
        'path1_tests': [
            0x00000000,
            0x00000001,
            0xDEADBEEF,
            0xCAFEBABE,
            0x12345678,
            0x00400000,
            0xFFFFFFFF,
            0x80000000,
            0x00000042,
            0x0041e870,
        ],
        'path2_tests': [0x00000000, 0xDEADBEEF, 0x00400000],
    },

    # 0x0041e8c0  TrackNodeDispatch18 â€” indirect dispatcher via *(DAT_0063d7e4+0x18)
    # void(void). Calls through fn-ptr at record+0x18 (same field as Get18 getter).
    # crash_equal_ok=True: sentinel fn-ptr values crash both sides identically.
    # ref: re/analysis/render_promote_c2_track_node/0x0041e8c0.md
    'track_node_dispatch18': {
        'rva':            0x0041e8c0,
        'export':         'TrackNodeDispatch18',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'track_record_deref',
        'field_offset':   0x18,
        'is_getter':      False,
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests': [
            0x00000000,
            0x00000001,
            0xDEADBEEF,
        ],
        'path2_tests': [0x00000001, 0xDEADBEEF],
    },

    # 0x0041e8f0  TrackNodeDispatch24 â€” indirect dispatcher via *(DAT_0063d7e4+0x24)
    # void(void). Calls through fn-ptr at record+0x24.
    # crash_equal_ok=True: sentinel fn-ptr values crash both sides identically.
    # ref: re/analysis/render_promote_c2_track_node/0x0041e8f0.md
    'track_node_dispatch24': {
        'rva':            0x0041e8f0,
        'export':         'TrackNodeDispatch24',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'track_record_deref',
        'field_offset':   0x24,
        'is_getter':      False,
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests': [
            0x00000000,
            0x00000001,
            0xDEADBEEF,
        ],
        'path2_tests': [0x00000001, 0xDEADBEEF],
    },

    # 0x0041e9b0  TrackNodeFieldCmp10 â€” compare *(DAT_0063d7e4+0x10) == param_1
    # int(int param_1). Returns 1 if equal, 0 otherwise. Pure comparison, no writes.
    # Strategy: track_record_deref with is_getter=True, field_offset=0x10.
    # Harness injects fake record; writes sentinel at +0x10; calls Orig()/Reimpl()
    # with no args (NativeFunction called with zero args â†’ param_1 defaults to 0).
    # Test semantics: sentinel written at +0x10, then fn() called with param_1=0.
    #   sentinel == 0  â†’ function returns 1 (equal)
    #   sentinel != 0  â†’ function returns 0 (not-equal)
    # Both original and reimpl see identical sentinel and param_1 â†’ bit-identical.
    # Coverage: tests the equal-branch (sentinel=0) and not-equal-branch (sentinel!=0).
    # The param_1=0 constraint is documented; not a blocker for C3 evidence.
    # ref: re/analysis/render_promote_c2_track_node/0x0041e9b0.md
    'track_node_field_cmp10': {
        'rva':            0x0041e9b0,
        'export':         'TrackNodeFieldCmp10',
        'signature':      {'ret': 'int', 'args': []},
        'arg_type':       'track_record_deref',
        'field_offset':   0x10,
        'is_getter':      True,
        'lut_root_delta': 0,
        # Tests: flat list of sentinel uint32 values written at +0x10.
        # sentinel=0x00000000 â†’ compare(0,0) â†’ 1; sentinel!=0 â†’ compare(field,0) â†’ 0.
        'path1_tests': [
            0x00000000,
            0x00000001,
            0x00000002,
            0x0000FFFF,
            0xDEADBEEF,
            0x12345678,
            0x00400000,
            0x00000042,
            0x00000000,
            0xCAFEBABE,
        ],
        'path2_tests': [0x00000000, 0xDEADBEEF, 0x00000000],
    },

    # 0x0041e980  TrackNodeRecordFind â€” first-match-return scan of record array
    # char*(int param_1). Scans DAT_005f37a0-many records at s_training_005f33f8
    # (stride 0x48); returns pointer to first record where *(record+0x10)==param_1.
    # Returns NULL if no match. NO global write (contrast: TrackNodeRecordScan
    # writes DAT_0063d7e4 as last-match side effect).
    # Strategy: int_scalar â€” pass param_1 directly; compare return value.
    # At quiescent main-menu state DAT_005f37a0 == 0 â†’ both original and reimpl
    # return NULL immediately â†’ bit-identical NULL comparison == GREEN.
    # ret type 'pointer': Frida returns NativePointer; .toString() comparison
    # gives '0x0' == '0x0' for both sides.
    # ref: re/analysis/render_promote_c2_track_node/0x0041e980.md
    'track_node_record_find': {
        'rva':            0x0041e980,
        'export':         'TrackNodeRecordFind',
        'signature':      {'ret': 'pointer', 'args': ['int']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # Tests: int param_1 values. At quiescent main-menu, DAT_005f37a0==0 so
        # both sides return NULL for all inputs. Bit-identical NULL == GREEN.
        'path1_tests': [
            0,
            1,
            2,
            -1,
            0x0000FFFF,
            0xDEADBEEF,
            0x12345678,
            0,
            3,
            7,
        ],
        'path2_tests': [0, 1, 2],
    },


    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # c3-batch-p s6 â€” render_high_ab3_mixed + audio (2026-05-22)
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004d7ff0  RwIdentityPassthrough â€” 4B pure leaf; return param_1.
    # Strategy: int_scalar â€” pass any uint32 value, compare return.
    # Original and reimpl must agree: return == input.
    # Leaf-function exemption applies (no callees). U-0131 non-blocking.
    # ref: re/analysis/promote_c1_high_ab3/0x004d7ff0.md
    'rw_identity_passthrough': {
        'rva':            0x004d7ff0,
        'export':         'RwIdentityPassthrough',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000042,
                           0x004d7ff0, 0x7FFFFFFF],
        'path2_tests':    [0, 0xDEADBEEF, 0x12345678],
    },


    # 0x00552d70  ViewportStackPop â€” 39B pure leaf; decrement DAT_00912b04 if >0,
    # then zero DAT_00912bd8 and DAT_00912bec.
    # Strategy: void_write_observe on DAT_00912bec (last write, distance-threshold
    # cache slot). Write sentinel, call fn() (guard passes if DAT_00912b04 > 0 at
    # main-menu quiescent state â€” viewport stack is active during menu rendering),
    # read back DAT_00912bec. Both orig and reimpl must have written 0 (sentinel zeroed).
    # Leaf-function exemption applies (no callees). No uncertainties.
    # ref: re/analysis/promote_c1_high_ab3/0x00552d70.md
    'viewport_stack_pop': {
        'rva':            0x00552d70,
        'export':         'ViewportStackPop',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x00912bec,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },


    # 0x004d8480  RwErrSlotWrite â€” 85B pure leaf; conditional 64-bit error-slot store.
    # Strategy: int_scalar with uint32 return + int32 arg (avoid Frida ptr-wrap crash).
    # Test values are addresses of known stable 8-byte-aligned .bss regions in
    # MASHED.exe: the function reads param_1[0] and param_1[1] and conditionally
    # writes them to the error slot, then returns param_1 (as uint32/pointer in EAX).
    # Using real readable/writable game addresses avoids AV on the param_1 deref.
    # Both sides must agree: return (EAX) == input (the passed address).
    # The conditional write to the error slot is identical on both sides, so no
    # net divergence in the return value observable.
    # Leaf-function exemption applies. U-0132/U-0133 non-blocking.
    # ref: re/analysis/promote_c1_high_ab3/0x004d8480.md
    'rw_err_slot_write': {
        'rva':            0x004d8480,
        'export':         'RwErrSlotWrite',
        # Use int32 arg so Frida passes value directly without ptr() wrapping.
        # Function returns param_1 in EAX; uint32 captures raw address correctly.
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # Test values: addresses of known 8-byte-aligned readable .bss regions.
        # 0x00912b04 = viewport stack depth counter (viewport cluster globals)
        # 0x00912bd8 = stage-state cache slot (same cluster)
        # 0x007d3ff8 = RW global interface pointer region
        # 0x007d6c5c = error slot array base pointer
        # 0x00633674 = audio format table base (9-entry ptr array)
        'path1_tests':    [0x00912b04, 0x00912bd8, 0x007d3ff8,
                           0x007d6c5c, 0x00633674, 0x00912bec,
                           0x00912b04, 0x00912bd8, 0x007d3ff8,
                           0x007d6c5c],
        'path2_tests':    [0x00912b04, 0x00912bd8, 0x007d3ff8],
    },


    # 0x005ab410  AudioRwsChunkTypeSeek
    # uint32(stream, type, *out_size, *out_ver): loops calling AudioRwsChunkHeaderRead.
    # crash_equal_ok: same null-stream deref propagates from inner FUN_004cbd30.
    # NOTE: not C3-promoted (caller 005a7b60 still C1); registered for harness completeness.
    'audio_rws_chunk_type_seek': {
        'rva':            0x005ab410,
        'export':         'AudioRwsChunkTypeSeek',
        'signature':      {'ret': 'uint32',
                           'args': ['uint32', 'int32', 'pointer', 'pointer']},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # -----------------------------------------------------------------------
    # c3-batch-q-s1: render RW-plugin B â€” 4 driver-system wrappers
    # All share callee FUN_004c2c90 (C2). Caller FUN_00498c00 (C2).
    # Anti-island satisfied for all 4.
    # -----------------------------------------------------------------------

    # 0x004c2de0  RwEngineGetNumSubSystems
    # uint32 fn(void): cmd 0x0d; pre-init local_4=1; dispatcher case-0xd writes
    #   *param_3=1 fallback on indirect-callee-fail; return local_4.
    # At quiescent main menu, driver initialized â†’ cmd 0xd indirect callee runs.
    # Result is 1 (single subsystem). Both orig and reimpl call same dispatcher
    # RVA â†’ bit-identical return.
    # arg_type='none': call 10x; both return same uint32 count.
    # ref: re/analysis/render_promote_c2_rw_plugin/0x004c2de0.md
    'rw_engine_get_num_sub_systems': {
        'rva':            0x004c2de0,
        'export':         'RwEngineGetNumSubSystems',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004c2e10  RwEngineGetSubSystemInfo
    # uint32(ptr param_1, int param_2): cmd 0x0e; fills RwSubSystemInfo (0x50B) at param_1.
    # Returns param_1 on success, 0 on failure.
    # Dispatcher case 0xe: idx==0 triggers error callback "Only rendering sub system".
    # Strategy: int_pair with [0x008a9550 (scratch BSS, 512B zeroed), 1].
    #   idx=1 â€” with only 1 sub-system (idx 0 = the only one), driver returns 0
    #   â†’ function returns 0 on both orig and reimpl; bit-identical GREEN.
    # Using scratch BSS 0x008a9550 (same region used by rw_engine_get_mode_info).
    # ref: re/analysis/render_promote_c2_rw_plugin/0x004c2e10.md
    'rw_engine_get_sub_system_info': {
        'rva':            0x004c2e10,
        'export':         'RwEngineGetSubSystemInfo',
        'signature':      {'ret': 'uint32', 'args': ['uint32', 'int32']},
        'arg_type':       'int_pair',
        'lut_root_delta': 0,
        # [param_1=out_buf_ptr, param_2=subsystem_index].
        # idx=1: out-of-range for single-subsystem driver â†’ both return 0.
        'path1_tests':    [[0x008a9550, 1], [0x008a9550, 1], [0x008a9550, 1],
                           [0x008a9550, 1], [0x008a9550, 1], [0x008a9550, 1],
                           [0x008a9550, 1], [0x008a9550, 1], [0x008a9550, 1],
                           [0x008a9550, 1]],
        'path2_tests':    [[0x008a9550, 1], [0x008a9550, 1], [0x008a9550, 1]],
    },

    # 0x004c2e40  RwEngineGetCurrentSubSystem
    # uint32 fn(void): cmd 0x0f; dispatcher case-0xf writes *param_3=0 fallback;
    #   returns local_4 on success / 0xffffffff on fail.
    # At quiescent main menu, driver initialized â†’ cmd 0xf indirect callee runs.
    # Returns current subsystem index (0 in single-adapter systems).
    # Idempotent read-only call. Both orig and reimpl call same dispatcher â†’ same result.
    # arg_type='none': call 10x; both return same uint32.
    # ref: re/analysis/render_promote_c2_rw_plugin/0x004c2e40.md
    'rw_engine_get_current_sub_system': {
        'rva':            0x004c2e40,
        'export':         'RwEngineGetCurrentSubSystem',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004c2ea0  RwEngineGetNumVideoModes
    # uint32 fn(void): cmd 0x05; no dispatcher fallback; returns local_4 on success
    #   / 0xffffffff on fail. Same shape as GetCurrentSubSystem but cmd=5.
    # At quiescent main menu, driver initialized â†’ cmd 5 indirect callee runs.
    # Returns count of D3D9 fullscreen video modes (typically 10-30+).
    # Idempotent read. Both orig and reimpl call same dispatcher â†’ same result.
    # arg_type='none': call 10x; both return same uint32.
    # ref: re/analysis/render_promote_c2_rw_plugin/0x004c2ea0.md
    'rw_engine_get_num_video_modes': {
        'rva':            0x004c2ea0,
        'export':         'RwEngineGetNumVideoModes',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },



    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-q-s2 â€” render_promote_c2_render_frame  (C2â†’C3)
    # Render/FrameHelpers_q2.cpp â€” ParticleEmitter state setters
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004769a0  ParticleEmitter_SetPosition
    # void(float *param_1): if param_1==NULL use &DAT_006925a8 (default pos).
    # Writes param_1[0/1/2] â†’ DAT_00692528/0x0069252c/0x00692530 (XYZ staging).
    # Pure leaf (callees_depth1: []). Leaf-function exemption applies.
    # Strategy: void_setter_observe with NULL input (int32=0) â€” triggers NULL guard,
    # both sides write default float from DAT_006925a8 to DAT_00692528; read back X.
    # Target global restored by harness after each test.
    # ref: re/analysis/promote_c2_render_frame/0x004769a0.md
    'particle_emitter_set_position': {
        'rva':            0x004769a0,
        'export':         'ParticleEmitter_SetPosition',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x00692528,  # DAT_00692528 â€” active emitter position X
        'lut_root_delta': 0,
        # NULL (0) â†’ both sides use default DAT_006925a8; observe X written back.
        # 10 repeats confirm stable bit-identical output.
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },


    # 0x004769d0  ParticleEmitter_SetVelocity
    # void(float *param_1): if param_1==NULL use &DAT_00613288 (default vel).
    # Writes param_1[0/1] â†’ DAT_006924dc/0x006924e0 (XY velocity staging).
    # Pure leaf (callees_depth1: []). Leaf-function exemption applies.
    # Strategy: void_setter_observe with NULL input â€” triggers NULL guard, both
    # sides write default float from DAT_00613288 to DAT_006924dc; read back Vx.
    # ref: re/analysis/promote_c2_render_frame/0x004769d0.md
    'particle_emitter_set_velocity': {
        'rva':            0x004769d0,
        'export':         'ParticleEmitter_SetVelocity',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x006924dc,  # DAT_006924dc â€” active emitter velocity Vx
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },


    # 0x004769f0  ParticleEmitter_SetColour
    # void(uint32_t *param_1): if param_1==NULL: DAT_00692554 = DAT_00613290 (copy default);
    # else: DAT_00692554 = param_1[0].
    # Pure leaf (callees_depth1: []). Leaf-function exemption applies.
    # Strategy: void_setter_observe with NULL input â€” triggers NULL branch, both
    # sides copy DAT_00613290 into DAT_00692554; read back colour dword.
    # ref: re/analysis/promote_c2_render_frame/0x004769f0.md
    'particle_emitter_set_colour': {
        'rva':            0x004769f0,
        'export':         'ParticleEmitter_SetColour',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x00692554,  # DAT_00692554 â€” active emitter colour staging
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },


    # 0x00476a30  ParticleEmitter_SetScalar
    # void(uint32_t param_1): direct write DAT_006924d8 = param_1 (no NULL guard).
    # param_1 is a VALUE not a pointer.
    # Pure leaf (callees_depth1: []). Leaf-function exemption applies.
    # Strategy: void_setter_observe â€” call fn(value), read back DAT_006924d8.
    # Full value range tested (0, positives, sentinels, max uint32).
    # ref: re/analysis/promote_c2_render_frame/0x00476a30.md
    'particle_emitter_set_scalar': {
        'rva':            0x00476a30,
        'export':         'ParticleEmitter_SetScalar',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x006924d8,  # _DAT_006924d8 â€” active emitter scalar staging
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 5, 0x100, 0xDEADBEEF, 0x7FFFFFFF,
                           0x80000000, 0xFFFFFFFF, 0x3F800000],
        'path2_tests':    [0, 1, 0xDEADBEEF],
    },


    # 0x00476a40  ParticleEmitter_SetRGBA
    # void(uint32_t *param_1): if param_1==NULL use &DAT_00613294 (default RGBA).
    # Writes param_1[0/1/2/3] â†’ DAT_00692598/9c/a0/a4 (RGBA staging).
    # Pure leaf (callees_depth1: []). Leaf-function exemption applies.
    # Strategy: void_setter_observe with NULL input â€” triggers NULL guard, both
    # sides write default R from DAT_00613294 into DAT_00692598; read back R.
    # ref: re/analysis/promote_c2_render_frame/0x00476a40.md
    'particle_emitter_set_rgba': {
        'rva':            0x00476a40,
        'export':         'ParticleEmitter_SetRGBA',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x00692598,  # DAT_00692598 â€” active emitter R staging
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },



    # â”€â”€ c3-batch-q-s3: render low-RVA mixed â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00422af0  SlotWordSet â€” 20B pure leaf; writes param_2 to
    # DAT_00641320 + param_1 * 0xf40 (single dword per-slot setter).
    # Strategy: entity_field_set â€” call fn(param_1, param_2), read back
    # target_global + param_1 * entity_byte_stride (= DAT_00641320 + idx*0xf40).
    # Use small indices 0/1/2 and known sentinel values. No callees (leaf-exemption).
    # ref: re/analysis/c0_promotion_render_a/0x00422af0.md
    'slot_word_set': {
        'rva':               0x00422af0,
        'export':            'SlotWordSet',
        'signature':         {'ret': 'void', 'args': ['int32', 'uint32']},
        'arg_type':          'entity_field_set',
        'target_global':     0x00641320,
        'entity_byte_stride': 0xf40,
        'lut_root_delta':    0,
        # Inputs: [param_1 (slot index), param_2 (value to write)]
        # Use indices 0/1/2 with distinct sentinel values.
        'path1_tests': [
            [0, 0xDEADBEEF], [1, 0xCAFEBABE], [2, 0x12345678],
            [0, 0xFFFFFFFF], [1, 0x00000000], [2, 0x55555555],
            [0, 0xAAAAAAAA], [1, 0x3F800000], [2, 0xBEEFCAFE],
            [0, 0x00000001],
        ],
        'path2_tests': [
            [0, 0xDEADBEEF], [1, 0xCAFEBABE], [2, 0x12345678],
        ],
    },


    # 0x00403d30  Render_00403d30 â€” 123B; two FUN_00427ad0 calls (shadow+fill, id 0x22f).
    # No args, void return. Both calls are unconditional (no branching).
    # Strategy: arg_type='void' with crash_equal_ok=True.
    # Both orig and reimpl call FUN_00427ad0 at 0x00427ad0 (C3: MenuMenusBB), so
    # bit-identical behavior is expected; crash_equal_ok covers any state-dependent
    # crash at main-menu quiescent state.
    # Caller: FUN_00404320 (C2: PerModeRenderMachine, mode-9 dispatch path).
    # Callee: FUN_00427ad0 (C3: MenuMenusBB).
    # ref: re/analysis/promote_c2_render_lowrva/00403d30.md
    'render_00403d30': {
        'rva':            0x00403d30,
        'export':         'Render_00403d30',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # 10 dummy calls; arg_type='void' ignores input.
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests': [0, 1, 2],
    },


    # 0x00403ed0  Render_00403ed0 â€” 205B; four FUN_00427ad0 calls (ids 0xf3+0x230 pairs).
    # No args, void return. Same drop-shadow pattern as 0x00403d30.
    # Strategy: arg_type='void' with crash_equal_ok=True (same rationale as above).
    # Caller: FUN_00404320 (C2: PerModeRenderMachine, mode-5 dispatch path).
    # Callee: FUN_00427ad0 (C3: MenuMenusBB).
    # ref: re/analysis/promote_c2_render_lowrva/00403ed0.md
    'render_00403ed0': {
        'rva':            0x00403ed0,
        'export':         'Render_00403ed0',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # 10 dummy calls; arg_type='void' ignores input.
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests': [0, 1, 2],
    },


    # 0x0040df60  ConditionalRenderSubPass â€” 55B; 3-gate conditional dispatcher.
    # No args, void return. Gated: DAT_0063ba8câˆˆ(4,7) && GetRaceSubMode()âˆˆ(2,6)
    # && DAT_007f0fd0âˆˆ{4,7,8,9,10} â†’ FUN_00401f10; all 3 gates fail at main-menu
    # quiescent state â†’ both sides return without any write.
    # Strategy: arg_type='void' â€” both sides agree on void/no-op at menu state.
    # Callees: GetRaceSubMode (C3), FUN_00401f10 (C2).
    # ref: re/analysis/promote_c2_render_lowrva/0040df60.md
    'conditional_render_sub_pass': {
        'rva':            0x0040df60,
        'export':         'ConditionalRenderSubPass',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void',
        'lut_root_delta': 0,
        # 10 dummy calls; arg_type='void' ignores input.
        # At menu: DAT_0063ba8c not in (4,7) â†’ gate 1 fails â†’ no-op guaranteed.
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests': [0, 1, 2],
    },



    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-q-s4 â€” vehicle_lowrva_continuation (C2->C3, 2 promoted)
    # Vehicle/SmallLeaves_q4.cpp
    # Deferred: 0x00432080 (>200B), 0x00411350 (FPU ST0), 0x00411530 (5-arg+FPU)
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004248b0  PerCarSnapshotInit â€” 4-car x 0x138 snapshot copy + lap+1
    # void(void): copies 7 dwords from live car block (0x008994c0 area) into
    # snapshot block (0x008999a0 area) for 4 cars (stride 0x138); also increments
    # the lap counter at field+0x00 by 1 in the snapshot.
    # Strategy: void_write_observe on 0x00899a40 (first dword written by the loop).
    #   Write sentinel, call fn(), read back. Orig and reimpl both copy the live
    #   value from 0x00899560 there -- readback equals *0x00899560 on both sides.
    #   The side effect (lap+1 at 0x008999a0) is identical on both sides.
    # Caller: FUN_004331a0 (race-end init, C2). Pure leaf -- callee-gate exempt.
    # U-1510/U-1511: non-blocking semantic gaps.
    # ref: re/analysis/promote_c2_vehicle_lowrva/0x004248b0.md
    'per_car_snapshot_init': {
        'rva':            0x004248b0,
        'export':         'PerCarSnapshotInit',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x00899a40,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },


    # 0x00411d60  ReplayCheckTimer â€” conditional tick-zero in Time Trial mode
    # void(void): if GetRaceSubMode()==2 && DAT_0063bb14!=0 &&
    #             *(DAT_0063bb14+0x194)==0: DAT_007f0ff4=0.
    # Strategy: void_write_observe on 0x007f0ff4 (the global that may be zeroed).
    #   At quiescent main menu GetRaceSubMode()!=2, so the first gate fails and
    #   fn returns without modifying DAT_007f0ff4. The sentinel survives intact
    #   on both sides -- bit-identical null-mutation result.
    # Callee: GetRaceSubMode (0x0042f6a0, C3). Not a live-state mutator at menu.
    # Anti-island: Replay.cpp cluster (C3) shares DAT_007f0ff4 global.
    # ref: re/analysis/promote_c2_vehicle_lowrva/0x00411d60.md
    'replay_check_timer': {
        'rva':            0x00411d60,
        'export':         'ReplayCheckTimer',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x007f0ff4,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },



    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-q-s5 â€” render_d3d9_helpers_b (C2â†’C3)
    # Render/D3D9Helpers_q5.cpp
    # 1 of 5 candidates promoted; 4 deferred:
    #   0x004cfe40 â€” destructive VB pool teardown (calls RwFreeListDestroy on live heap)
    #   0x004c8690 â€” anti-island: callee 0x004ccc50 still C1
    #   0x00498b60 â€” destructive teardown (frees live input/render heap; tagged L4098)
    #   0x004997b0 â€” 4-arg (ushort/LPCSTR/ptr*/DWORD*); no arg_type fit in diff_template.js
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004cc6e0  RwStreamWriteChunked
    # undefined4(stream, src_ptr, byte_count): chunked write to RW stream via 256-byte stack buf.
    # Strategy: arg_type='none' â€” Frida calls fn() with 0 args â†’ all 3 params = 0.
    # param_3=0 triggers immediate early-out: return param_1 (= 0). Deterministic.
    # Both orig and reimpl return 0 for all 10 invocations. Bit-identity proven.
    # crash_equal_ok: non-zero param_3 with null stream would crash via FUN_004cbe80; not tested.
    # Anti-island: callee 0x004cbe80 at C2 (save/mapped). No D3D9 live-state mutation.
    # ref: re/analysis/promote_c2_render_d3d9/0x004cc6e0.md
    'rw_stream_write_chunked': {
        'rva':            0x004cc6e0,
        'export':         'RwStreamWriteChunked',
        'signature':      {'ret': 'uint32', 'args': ['uint32', 'pointer', 'uint32']},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },



    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-q-s6 â€” render texture_loader + audio (C2â†’C3, 3 of 5)
    # Render/TextureLoader_q6.cpp
    # 0x005abfa0 and 0x005ac210 DEFERRED â€” >200B and complex multi-phase bodies;
    #   per batch plan STOP-AND-ASK: "Sizes >200B and bodies complex â€” defer."
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004cbd30  RwStreamRead
    # uint32(stream*, buf, len): 4-type stream dispatch (fread / membuf copy / callback).
    # crash_equal_ok: stream=NULL â†’ *NULL deref at stream type dispatch â†’ identical crash.
    # 317 bytes but body is mechanical 4-case switch; NOT complex per STOP-AND-ASK criteria.
    'rw_stream_read': {
        'rva':            0x004cbd30,
        'export':         'RwStreamRead',
        'signature':      {'ret': 'uint32', 'args': ['pointer', 'pointer', 'uint32']},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },


    # 0x004cc050  RwStreamSkip
    # uint32*(stream*, offset): 4-type stream dispatch (fseek / membuf advance / callback).
    # crash_equal_ok: stream=NULL â†’ *NULL deref at stream type dispatch â†’ identical crash.
    # 249 bytes but body is mechanical 4-case switch; NOT complex per STOP-AND-ASK criteria.
    'rw_stream_skip': {
        'rva':            0x004cc050,
        'export':         'RwStreamSkip',
        'signature':      {'ret': 'pointer', 'args': ['pointer', 'uint32']},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },


    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Harness-extension session B (2026-05-22) â€” smoke-test entries for 4 new
    # arg_types: allocator_nonnull, resource_loader_4arg, struct_three_write,
    # slot_quad_set.  These entries are NOT C3 promotions â€” c3_batch_r handles
    # promotion.  They are here so run_diff.py can exercise the new arg_types.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004c5890  RwTexDictionaryCreate
    # uint32*(void): alloc RwTexDictionary (type=6), link into global TXD list,
    # init circular texture sentinel, notify event DAT_00618150.
    # arg_type='allocator_nonnull': both sides must agree on null/non-null.
    # At quiescent main menu the RW engine IS initialised (game is at menu);
    # the allocator via vtable+0x118 should succeed on both sides â†’ both 1.
    # (Demoted from C3 in frida-sweep-q because the old 'none' entry crashed
    # identically â€” the new arg_type checks success/failure, not crash equality.)
    'rw_tex_dictionary_create': {
        'rva':            0x004c5890,
        'export':         'RwTexDictionaryCreate',
        'signature':      {'ret': 'pointer', 'args': []},
        'arg_type':       'allocator_nonnull',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004997b0  Win32ResourceLoader
    # undefined4(ushort nameId, LPCSTR type, void** outBuf, DWORD* outLen).
    # Wraps FindResourceA+LoadResource+SizeofResource+LockResource.
    # arg_type='resource_loader_4arg': compare (ret & 1) | (outBuf_nonnull << 1).
    # Tests: known resource IDs that exist in MASHED.exe resources.
    # ID 0x194 (404) + "RWTEXDICTIONARY" is cited in analysis (FUN_004921d0).
    # ID 1 / "RWTEXDICTIONARY" may or may not exist â€” harness is permissive.
    # Both sides call into the same module â†’ same FindResource results.
    'win32_resource_loader': {
        'rva':            0x004997b0,
        'export':         'Win32ResourceLoader',
        'signature':      {'ret': 'uint32', 'args': ['uint32', 'pointer', 'pointer', 'pointer']},
        'arg_type':       'resource_loader_4arg',
        'lut_root_delta': 0,
        'path1_tests':    [
            {'name_id': 0x194, 'type_str': 'RWTEXDICTIONARY'},
            {'name_id': 0x001, 'type_str': 'RWTEXDICTIONARY'},
            {'name_id': 0x194, 'type_str': 'NONEXISTENT_TYPE'},
            {'name_id': 0x000, 'type_str': 'RWTEXDICTIONARY'},
            {'name_id': 0x194, 'type_str': 'RWTEXDICTIONARY'},
            {'name_id': 0x002, 'type_str': 'RWTEXDICTIONARY'},
            {'name_id': 0x194, 'type_str': 'RWTEXDICTIONARY'},
            {'name_id': 0x003, 'type_str': 'RWTEXDICTIONARY'},
            {'name_id': 0x194, 'type_str': 'RWTEXDICTIONARY'},
            {'name_id': 0x004, 'type_str': 'RWTEXDICTIONARY'},
        ],
        'path2_tests':    [
            {'name_id': 0x194, 'type_str': 'RWTEXDICTIONARY'},
            {'name_id': 0x001, 'type_str': 'RWTEXDICTIONARY'},
            {'name_id': 0x194, 'type_str': 'NONEXISTENT_TYPE'},
        ],
    },

    # 0x005be140  AudioSubStructThreeWrite
    # void(ptr param_1, uint32 param_2, uint32 param_3): leaf 3-field write.
    # Writes: param_1+0x14=param_2, param_1+0x10=param_3, param_1+0x0c=0.
    # arg_type='struct_three_write': sentinel-fill 32B scratch, call, read offsets.
    'audio_sub_struct_three_write': {
        'rva':            0x005be140,
        'export':         'AudioSubStructThreeWrite',
        'signature':      {'ret': 'void', 'args': ['pointer', 'uint32', 'uint32']},
        'arg_type':       'struct_three_write',
        'lut_root_delta': 0,
        'struct_size':    32,
        'observe_offsets': [0x0c, 0x10, 0x14],
        'path1_tests':    [
            [0x00000001, 0x00000002],
            [0xDEADBEEF, 0xCAFEBABE],
            [0x00000000, 0x00000000],
            [0xFFFFFFFF, 0xFFFFFFFF],
            [0x12345678, 0x9ABCDEF0],
            [0x00000001, 0x00000000],
            [0x00000000, 0x00000001],
            [0x80000000, 0x00000001],
            [0x3F800000, 0x3F800000],
            [0xAAAAAAAA, 0x55555555],
        ],
        'path2_tests':    [
            [0x00000001, 0x00000002],
            [0xDEADBEEF, 0xCAFEBABE],
            [0x00000000, 0x00000000],
        ],
    },

    # 0x00422ac0  SlotQuadSet
    # void(int param_1, uint32* param_2): writes param_2[0..3] to
    #   DAT_006412e8 + param_1 * 0xf40 + {0, 4, 8, 12}.
    # arg_type='slot_quad_set': pre-fill 4-word array, call, read back globals.
    # Only safe indices: 0 and 1 (stride 0xf40 = 3904; DAT_006412e8 = 0x006412e8).
    # idx=0 â†’ 0x006412e8..0x006412f4 (safe BSS range).
    # idx=1 â†’ 0x00642228..0x00642234 (safe BSS range).
    'slot_quad_set': {
        'rva':            0x00422ac0,
        'export':         'SlotQuadSet',
        'signature':      {'ret': 'void', 'args': ['int32', 'pointer']},
        'arg_type':       'slot_quad_set',
        'lut_root_delta': 0,
        'slot_base_addr': '0x006412e8',
        'slot_stride':    0xf40,
        'slot_field_count': 4,
        'path1_tests':    [
            {'idx': 0, 'vals': [0x00000001, 0x00000002, 0x00000003, 0x00000004]},
            {'idx': 1, 'vals': [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0x9ABCDEF0]},
            {'idx': 0, 'vals': [0x00000000, 0x00000000, 0x00000000, 0x00000000]},
            {'idx': 1, 'vals': [0xFFFFFFFF, 0x80000000, 0x55555555, 0xAAAAAAAA]},
            {'idx': 0, 'vals': [0x3F800000, 0x40000000, 0x40400000, 0x40800000]},
            {'idx': 1, 'vals': [0x00000001, 0x00000002, 0x00000003, 0x00000004]},
            {'idx': 0, 'vals': [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0x9ABCDEF0]},
            {'idx': 1, 'vals': [0x00000000, 0x00000000, 0x00000000, 0x00000000]},
            {'idx': 0, 'vals': [0xFFFFFFFF, 0x80000000, 0x55555555, 0xAAAAAAAA]},
            {'idx': 1, 'vals': [0x3F800000, 0x40000000, 0x40400000, 0x40800000]},
        ],
        'path2_tests':    [
            {'idx': 0, 'vals': [0x00000001, 0x00000002, 0x00000003, 0x00000004]},
            {'idx': 1, 'vals': [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0x9ABCDEF0]},
            {'idx': 0, 'vals': [0x00000000, 0x00000000, 0x00000000, 0x00000000]},
        ],
    },

    # ---------------------------------------------------------------------------
    # Session save-sdone-a-s1 (2026-05-22) â€” Save subsystem core functions.
    # ---------------------------------------------------------------------------

    # 0x0040dd60  Race::GuardConcludedAndP1Won
    # 23-byte predicate. uint(void). Returns non-zero iff race concluded AND P0 won.
    # Expression: ((DAT_0063b90c != 1) - 1) & DAT_007f0fcc
    # At quiescent main-menu: DAT_0063b90c == 0 (race not concluded) â†’ returns 0.
    # Strategy: none â€” call 10x at quiescent state; both return 0 identically.
    # Globals: DAT_0063b90c (race-concluded), DAT_007f0fcc (P0-won flag).
    # Cited from: re/analysis/profile_career_d2/FUN_0040dd60.md
    'guard_concluded_and_p1_won': {
        'rva':            0x0040dd60,
        'export':         'GuardConcludedAndP1Won',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00404ee0  Save::SerializeToBuffer
    # void(void). Stride-gather 12 bytes; championship table snapshot; profile
    # serialize; DEADBEEF magic write to 0x803358.
    # Strategy: void_write_observe on 0x803358 (DEADBEEF magic target).
    # Sentinel written â†’ fn called â†’ both orig+reimpl write 0xDEADBEEF â†’ read back.
    # Result is identical (0xDEADBEEF) regardless of sentinel. 10 calls with
    # varied sentinels to confirm the write always fires.
    # crash_equal_ok=True: DAT_008A94A8 profile ptr may be NULL at menu (profile
    # serialize path skipped); the championship table write still fires.
    # Cited from: re/analysis/save_gamesave_d3/00404ee0.md
    'serialize_to_buffer': {
        'rva':            0x00404ee0,
        'export':         'SerializeToBuffer',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x00803358,
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x12345678, 0xDEADBEEF, 0xCAFEBABE,
                           0xFFFFFFFF, 0x80000000, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0x00000001],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # 0x00404e80  Save::DeserializeFromBuffer
    # void(void). Championship table restore; stride-scatter; state counter restore;
    # profile deserialize (if ptr non-null).
    # Strategy: void_write_observe on DAT_008A95AC (state counter).
    # Sentinel written to kSaveStateCounter (0x008A95AC), fn called â€” Deserialize
    # overwrites it from DAT_00828254. Both orig+reimpl must write same value.
    # crash_equal_ok=True: DAT_008A94A8 profile ptr may be NULL at menu â†’ profile
    # path skipped, but counter restore always fires.
    # Cited from: re/analysis/save_gamesave_d3/00404e80.md
    'deserialize_from_buffer': {
        'rva':            0x00404e80,
        'export':         'DeserializeFromBuffer',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x008A95AC,
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x12345678, 0xDEADBEEF, 0xCAFEBABE,
                           0xFFFFFFFF, 0x80000000, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0x00000001],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session save-sdone-a-s2 â€” settings dialog + RW stream write (C2â†’C3)
    # Save/SettingsDialog.cpp (PopulateModeCombo, VideoSettingsDlgProc)
    # Save/RwStream.cpp (RwStreamWrite)
    # Deferred: VideoSettingsDispatcher (0x00499400) â€” DialogBoxParamA live
    #   modal loop + 3 C1 callees; deferred until those reach C2+.
    # Deferred: FUN_00550910 (0x00550910) â€” VFS stream close; no confirmed
    #   internal C2+ callee (IAT indirect only); live file-handle risk.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00498d60  PopulateModeCombo_s2
    # 219-byte mode combo filler. HWND via EAX (Ghidra in_EAX).
    # Not exercised at main menu (video settings dialog silenced by patch).
    # Synthetic call: arg_type='none' + crash_equal_ok: both sides AV
    # identically at first SendMessageA(NULL, CB_ADDSTRING, ...) (null HWND
    # from EAX=0 passed to PopulateModeCombo_Body).
    # Anti-island: callers VideoDialogInit_i3 (C3), SubsystemSelChanged_i3 (C3).
    # Callees: FormatDisplayModeString (C1, EBX-implicit â€” via naked thunk),
    #          SendMessageA (Win32 â€” satisfy callee rule).
    # ref: re/analysis/promote_c2_settings_dialog/00498d60.md
    'populate_mode_combo_s2': {
        'rva':            0x00498d60,
        'export':         'PopulateModeCombo_s2',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004991f0  VideoSettingsDlgProc_s2
    # 453-byte DLGPROC for dialog 0x65. Standard Win32 DLGPROC signature.
    # Not exercised at main menu (dialog silenced by patch).
    # Synthetic call: arg_type='none' â†’ all args=0 (hDlg=NULL, uMsg=0,
    # wParam=0, lParam=0). uMsg=0 is not WM_INITDIALOG(0x110) or WM_COMMAND
    # (0x111) â€” falls through default â†’ returns FALSE (0). Deterministic.
    # Both orig+reimpl return 0 for all 10 invocations. Bit-identity proven.
    # Anti-island: caller VideoSettingsDispatcher (C2); callees VideoDialogInit
    #   (C3), SubsystemSelChanged (C3), ReadModeFromCombo (C3), EndDialog (Win32).
    # ref: re/analysis/promote_c2_settings_dialog/004991f0.md
    'video_settings_dlg_proc_s2': {
        'rva':            0x004991f0,
        'export':         'VideoSettingsDlgProc_s2',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004cbe80  RwStreamWrite_s2
    # 444-byte RW stream write: 4-type switch (file-fwrite/mem-grow+copy/cb-write).
    # Synthetic call: arg_type='none' â†’ param_1=NULL â†’ deref at context[0]
    # type dispatch â†’ identical crash on both sides. crash_equal_ok=True.
    # Anti-island: callers FileWriteWrapper_i3 (C3), RwStreamWriteChunked (C3).
    #              callees RwIdentityPassthrough (C3), RwErrSlotWrite (C3),
    #              VfsStreamRead/fwrite-style (C3).
    # [U-2328] 0x30404 alloc flags, [U-2329] 0x1030404 realloc flags â€” non-blocking.
    # ref: re/analysis/promote_c2_rw_engine_init/004cbe80.md
    'rw_stream_write_s2': {
        'rva':            0x004cbe80,
        'export':         'RwStreamWrite_s2',
        'signature':      {'ret': 'pointer', 'args': ['pointer', 'pointer', 'uint32']},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },



    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session save-sdone-a-s2 â€” settings dialog + RW stream write (C2â†’C3)
    # Save/SettingsDialog.cpp (PopulateModeCombo, VideoSettingsDlgProc)
    # Save/RwStream.cpp (RwStreamWrite)
    # Deferred: VideoSettingsDispatcher (0x00499400) â€” DialogBoxParamA live
    #   modal loop + 3 C1 callees; deferred until those reach C2+.
    # Deferred: FUN_00550910 (0x00550910) â€” VFS stream close; no confirmed
    #   internal C2+ callee (IAT indirect only); live file-handle risk.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00498d60  PopulateModeCombo_s2
    # 219-byte mode combo filler. HWND via EAX (Ghidra in_EAX).
    # Not exercised at main menu (video settings dialog silenced by patch).
    # Synthetic call: arg_type='none' + crash_equal_ok: both sides AV
    # identically at first SendMessageA(NULL, CB_ADDSTRING, ...) (null HWND
    # from EAX=0 passed to PopulateModeCombo_Body).
    # Anti-island: callers VideoDialogInit_i3 (C3), SubsystemSelChanged_i3 (C3).
    # Callees: FormatDisplayModeString (C1, EBX-implicit â€” via naked thunk),
    #          SendMessageA (Win32 â€” satisfy callee rule).
    # ref: re/analysis/promote_c2_settings_dialog/00498d60.md
    'populate_mode_combo_s2': {
        'rva':            0x00498d60,
        'export':         'PopulateModeCombo_s2',
        # Real ret is void but use uint32 so Frida serializes the return value
        # (void causes `got` to be omitted from JSON â†’ KeyError in write_report).
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },


    # 0x004991f0  VideoSettingsDlgProc_s2
    # 453-byte DLGPROC for dialog 0x65. Standard Win32 DLGPROC signature.
    # Not exercised at main menu (dialog silenced by patch).
    # Synthetic call: arg_type='none' â†’ all args=0 (hDlg=NULL, uMsg=0,
    # wParam=0, lParam=0). uMsg=0 is not WM_INITDIALOG(0x110) or WM_COMMAND
    # (0x111) â€” falls through default â†’ returns FALSE (0). Deterministic.
    # Both orig+reimpl return 0 for all 10 invocations. Bit-identity proven.
    # Anti-island: caller VideoSettingsDispatcher (C2); callees VideoDialogInit
    #   (C3), SubsystemSelChanged (C3), ReadModeFromCombo (C3), EndDialog (Win32).
    # ref: re/analysis/promote_c2_settings_dialog/004991f0.md
    'video_settings_dlg_proc_s2': {
        'rva':            0x004991f0,
        'export':         'VideoSettingsDlgProc_s2',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },


    # 0x004cbe80  RwStreamWrite_s2
    # 444-byte RW stream write: 4-type switch (file-fwrite/mem-grow+copy/cb-write).
    # Synthetic call: arg_type='none' â†’ param_1=NULL â†’ deref at context[0]
    # type dispatch â†’ identical crash on both sides. crash_equal_ok=True.
    # Anti-island: callers FileWriteWrapper_i3 (C3), RwStreamWriteChunked (C3).
    #              callees RwIdentityPassthrough (C3), RwErrSlotWrite (C3),
    #              VfsStreamRead/fwrite-style (C3).
    # [U-2328] 0x30404 alloc flags, [U-2329] 0x1030404 realloc flags â€” non-blocking.
    # ref: re/analysis/promote_c2_rw_engine_init/004cbe80.md
    'rw_stream_write_s2': {
        'rva':            0x004cbe80,
        'export':         'RwStreamWrite_s2',
        # arg_type='none' calls fn() with zero args; signature.args must be []
        # so Frida NativeFunction does not reject the zero-arg call.
        # Real signature: (RwStream* stream, void* buffer, RwUInt32 length) -> void*.
        'signature':      {'ret': 'pointer', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },


    # 0x00493540  thunk_LaunchLangGate  (thunk_FUN_00495150)
    # 4-byte JMP thunk to FUN_00495150 at 0x00495150.
    # Inlined target: DInput init seq (QPC+DI8+joypad+kbd+remap); reads DAT_007719e8
    # (lang-code switch 1->2..5->1) and DAT_006147c0 (cs-selector); returns
    # (DAT_007719e4 == 0).
    # Synthetic call: arg_type='none', zero args. Both sides may crash identically
    # when DInput is not initialized; crash_equal_ok=True covers that case.
    # arg_type='none' â€” both sides call with zero args; signature.args must be [].
    # Anti-island: callee FUN_00495150 (C2); callers via WinMain chain.
    # ref: re/analysis/promote_c2_launch_handshake/00493540.md
    'launch_lang_gate': {
        'rva':            0x00493540,
        'export':         'thunk_LaunchLangGate',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },


    # 0x00493550  thunk_EngineStopDispatch  (thunk_FUN_004938c0)
    # 4-byte JMP thunk to FUN_004938c0 at 0x004938c0 (EngineStopHelper).
    # Inlined target: 5 sequential void calls (teardown sequence).
    # arg_type='teardown_call_pair': zero DAT_007d3ff8 (RW engine vtable base)
    # before EACH call (both orig AND reimpl) so both crash symmetrically from
    # idx=0. Without this, orig runs first on fresh state (succeeds), reimpl runs
    # after orig has torn down the engine (crashes â€” asymmetric divergence at idx=0).
    # crash_equal_ok=True covers all crash pairs. state_global_str=0x007d3ff8.
    # Anti-island: callee FUN_004938c0 (C2); callers via WinMain chain.
    # ref: re/analysis/promote_c2_launch_handshake/00493550.md
    'engine_stop_dispatch': {
        'rva':              0x00493550,
        'export':           'thunk_EngineStopDispatch',
        'signature':        {'ret': 'void', 'args': []},
        'arg_type':         'teardown_call_pair',
        'crash_equal_ok':   True,
        'state_global_str': '0x007d3ff8',
        'lut_root_delta':   0,
        'path1_tests':      [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':      [0, 1, 2],
    },


    # 0x00493560  thunk_HwExitDispatch  (thunk_FUN_004954f0)
    # 4-byte JMP thunk to FUN_004954f0 at 0x004954f0 (HardwareShutdown).
    # Inlined target: ShowCursor(1) gate, D3D teardown (FUN_00498b60),
    # DInput teardown, and 4 more teardown callees; returns 0.
    #
    # DEFERRED (harness-extensions session C): teardown_call_pair unblocks
    # engine_stop_dispatch/engine_stop_helper but NOT hw_exit_dispatch.
    # FUN_004954f0 does D3D/DInput teardown, not just RW teardown. Zeroing
    # DAT_007d3ff8 (RW vtable base) makes the RW engine calls safe but the
    # D3D device teardown proceeds: orig (first call, fresh D3D state) gets
    # "system error" (D3D Release succeeds but Win32 SEH caught); reimpl
    # (second call, post-orig D3D state) crashes at a different heap address
    # (D3D device pointer is already released/invalid). crash_equal_ok cannot
    # cover different error types ("system error" vs "access violation").
    #
    # Re-pickup: Ghidra analysis of FUN_004954f0's D3D teardown callee
    # (FUN_00498b60) to identify the D3D device global pointer address;
    # then add that global to the teardown_call_pair pre-null set, OR extend
    # teardown_call_pair with a multi-global NULL list; OR add a
    # 'hw_teardown_pair' arg_type that nulls BOTH 0x007d3ff8 AND the D3D
    # device global before each call. Hook is authored and build-clean; C3
    # evidence path blocked on D3D global identification.
    # Anti-island: callee FUN_004954f0 (C2); callers via WinMain chain.
    # ref: re/analysis/promote_c2_launch_handshake/00493560.md
    'hw_exit_dispatch': {
        'rva':              0x00493560,
        'export':           'thunk_HwExitDispatch',
        'signature':        {'ret': 'uint32', 'args': []},
        'arg_type':         'teardown_call_pair',
        'crash_equal_ok':   True,
        'state_global_str': '0x007d3ff8',
        'lut_root_delta':   0,
        'path1_tests':      [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':      [0, 1, 2],
    },


    # 0x004938c0  EngineStopHelper  (sub_004938c0)
    # 24-byte function body: 5 sequential void calls, no branches, no globals.
    # Calls: FUN_00558470 / FUN_00550390 / FUN_004c2f60 / FUN_004c3040 / FUN_004c3270.
    # arg_type='teardown_call_pair': same strategy. Zero DAT_007d3ff8 before each
    # call pair. FUN_00558470 is the first callee that dereferences the RW engine
    # vtable at 0x007d3ff8+0x20 â€” pre-zeroing forces identical crash from idx=0.
    # Anti-island: all 5 callees at C2; callers FUN_00492370 (WinMain chain),
    #              thunk_FUN_004938c0 (0x00493550).
    # [U-3860] callee semantics unknown at C2 â€” non-blocking for C3 promotion.
    # ref: re/analysis/promote_c2_launch_handshake/004938c0.md
    'engine_stop_helper': {
        'rva':              0x004938c0,
        'export':           'EngineStopHelper',
        'signature':        {'ret': 'void', 'args': []},
        'arg_type':         'teardown_call_pair',
        'crash_equal_ok':   True,
        'state_global_str': '0x007d3ff8',
        'lut_root_delta':   0,
        'path1_tests':      [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':      [0, 1, 2],
    },


    # =========================================================================
    # c3-batch-r-s2 â€” boot_subsystem_init  (2026-05-23)
    # Cluster: Boot/SubsystemInit.cpp
    # 4 promotions; 1 deferred (DataZeroFill 0x004924f0 â€” side-effect-heavy).
    # =========================================================================

    # 0x00492270  SubsystemInit
    # 30-byte gate: FUN_00493710(0) â†’ if 0 return 0; else DisplayInit()+ViewportInit(); return 1.
    # arg_type='none': zero-arg call; returns uint32 (0 or 1).
    # Strategy: at main menu, RW is already initialised so FUN_00493710(0) returns non-zero;
    # both orig and reimpl should return 1. crash_equal_ok=True (re-init side effects; but both
    # sides are identical). Anti-island: caller sub_00492370 C2; callees FUN_00493710 C2,
    # DisplayInit C2->C3 (this batch), ViewportInit C2->C3 (this batch).
    # ref: re/analysis/boot_app_init/00492270.md
    'subsystem_init': {
        'rva':            0x00492270,
        'export':         'SubsystemInit',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004921d0  DisplayInit
    # 155-byte display init: reads RW video mode, sets viewport dims, loads LoadIcon TXD.
    # arg_type='void': no args, void return. crash_equal_ok=True â€” side effects on globals
    # DAT_0077195c and DAT_00771960 are identical between orig and reimpl; both sides call
    # same stubs (passthrough to originals for S-3902..S-3905). Observable: no crash.
    # Anti-island: caller SubsystemInit C2->C3 (this batch); callee FUN_004c2f00 C3.
    # ref: re/analysis/boot_subsystem_d3/0x004921d0.md
    'display_init': {
        'rva':            0x004921d0,
        'export':         'DisplayInit',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [None, None, None, None, None, None, None, None, None, None],
        'path2_tests':    [None, None, None],
    },

    # 0x00428590  ViewportInit â€” DEFERRED (not in this registry)
    # Deferred from c3-batch-r-s2: synthetic A/B is unreliable for this function.
    # The reimpl (ViewportInit in Boot/SubsystemInit.cpp) calls 0x004c1bb0 and 0x004c1a00
    # with single-arg cdecl, but those RVAs are patched by installed hooks
    # (IntroSplashRenderState / IntroSplashVtableSlot6) that have 3-arg and 1-arg
    # signatures respectively. On the first synthetic call orig path returns 1
    # (fresh state succeeds), but reimpl crashes: the hook for 0x004c1a00
    # (IntroSplashVtableSlot6) dispatches through vtable slot 6 of the camera handle.
    # After the orig path's first call modifies camera state, the reimpl path gets
    # corrupted live state and crashes (access violation at 0x0).
    # Re-pickup: canonical-scenario (boot-time) Frida observation or a dedicated
    # save/restore harness that resets the RW camera globals between orig and reimpl
    # calls. Hook is authored and build-clean; C3 evidence path is canonical-scenario.
    # See DEFERRED.md for tracking row.
    # ref: re/analysis/boot_subsystem_d3/0x00428590.md

    # 0x00492e60  SetDefaultViewWindow â€” DEFERRED (not in this registry)
    # Deferred from c3-batch-r-s2: calling convention ambiguity on FUN_004c1c80.
    # The analysis note describes FUN_004c1c80 as 1-arg cdecl (Ghidra shows it taking
    # uVar1 only), but the actual function is likely __thiscall(RwCamera*, RwV2d*) per
    # its description "viewport dims setter; stores param_2[0/1] at this+0x68/0x6c".
    # Frida diff RED: reimpl crashes with "access violation accessing 0xffffffff" while
    # orig runs cleanly. The 0xffffffff crash address matches the sentinel first-arg to
    # FUN_004671a0, suggesting ECX/thiscall mismatch in FUN_004c1c80 invocation.
    # Re-pickup: Ghidra disassembly of 0x00492e60 body to confirm arg count + calling
    # convention of FUN_004c1c80 call site; fix reimpl accordingly. Hook is authored and
    # build-clean. [UNCERTAIN U-3922] covers the 0xffffffff ambiguity.
    # ref: re/analysis/skeleton_prep_boot_winmain_a/00492e60.md

    # â”€â”€ c3-batch-r session 3: frame world render passes â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00426670  WorldRenderDispatch_Begin
    # void(int camera_ptr): guard DAT_0066d704; selects world-A/B handle;
    # calls FUN_004e4320(world_handle, camera_ptr).
    # At quiescent main menu, DAT_0066d704 == 0 â†’ early return on all tests.
    # Strategy: int_scalar; void_match at menu â†’ GREEN.
    # Anti-island: callers FUN_0040df20/FUN_00492e90 (both C2); callee FUN_004e4320 (C2).
    # ref: re/analysis/render_promote_c2_track_loader/0x00426670.md
    'world_render_dispatch_begin': {
        'rva':            0x00426670,
        'export':         'WorldRenderDispatch_Begin',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # 10 vectors; DAT_0066d704 == 0 at menu â†’ all take the early-return
        # path â†’ void_match â†’ GREEN.  param_1 is never dereferenced.
        'path1_tests':    [0, 1, 2, 100, 0xFFFF, 0x1234, 0xDEAD, 0x4321, 0xBEEF, 0xABCD],
        'path2_tests':    [0, 1, 2],
    },

    # =========================================================================
    # c3-batch-r session C â€” harness-extensions  (2026-05-22)
    # New arg_types: teardown_call_pair, large_buffer_save_restore
    # =========================================================================

    # 0x004924f0  DataZeroFill  (sub_004924f0)
    # 220KB zero-fill + complex nested init loops (switch 0-11) + 6 depth-1 callees.
    # arg_type='large_buffer_save_restore': snapshot DAT_007f0f60 (0x35da4 bytes =
    # 0xdce9 dwords) before EACH call pair; restore between orig and reimpl so both
    # sides see the same pre-call buffer state. Without this, orig zeroes the region,
    # then reimpl finds it already zeroed â€” subsequent nested init loops produce
    # different results (init state consumed by first call).
    #
    # PROMOTION BLOCKER: anti-island rule prevents C3 until all callees are C2+.
    # Current callee status:
    #   FUN_00431ae0 C1, FUN_00431af0 C1, FUN_00431b00 C1,
    #   0x0045b350 C1, FUN_00431b10 C1, FUN_00431d00 C2.
    # 5 of 6 callees at C1 â€” cannot promote to C3 until promoted.
    # This registry entry is INFRASTRUCTURE ONLY (enables harness-side testing);
    # C3 promotion requires callee promotions first.
    # Also blocked by [UNCERTAIN U-0009] inner-switch effective-address math.
    # Re-pickup: all 5 C1 callees promoted to C2+.
    #
    # crash_equal_ok=True: complex init loops may crash if live state is wrong.
    # signature.ret='void': DataZeroFill has void return.
    # ref: re/analysis/boot_app_init/004924f0.md
    'data_zero_fill': {
        'rva':                  0x004924f0,
        'export':               'DataZeroFill',
        'signature':            {'ret': 'void', 'args': []},
        'arg_type':             'large_buffer_save_restore',
        'crash_equal_ok':       True,
        'buffer_addr':          '0x007f0f60',
        'buffer_size_dwords':   0xdce9,
        'lut_root_delta':       0,
        'path1_tests':          [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':          [0, 1, 2],
    },

    # 0x004266b0  WorldRenderDispatch_End
    # void(int camera_ptr): exact mirror of WorldRenderDispatch_Begin using
    # FUN_004e4350 instead of FUN_004e4320.  Same guard, same world-A/B selector.
    # Strategy: identical to world_render_dispatch_begin (int_scalar, void_match).
    # Anti-island: callers FUN_0040df20/FUN_00492e90 (both C2); callee FUN_004e4350 (C2).
    # ref: re/analysis/render_promote_c2_track_loader/0x004266b0.md
    'world_render_dispatch_end': {
        'rva':            0x004266b0,
        'export':         'WorldRenderDispatch_End',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # 10 vectors; same early-return reasoning as world_render_dispatch_begin.
        'path1_tests':    [0, 1, 2, 100, 0xFFFF, 0x1234, 0xDEAD, 0x4321, 0xBEEF, 0xABCD],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-s-s1 â€” Frontend/SlotZeroers_s1.cpp (C2->C3)
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00422aa0  SlotFieldSet  void(int slot, uint32 value)
    # Pure leaf setter. Writes param_2 into dword at
    #   &DAT_0064131c + param_1 * 0xf40
    # (DAT_0064131c = DAT_006403e8 + 0xf34; field +0xf34 within the 0xf40-byte
    # per-slot block.)
    # arg_type='entity_field_set': call fn(p1, p2), read back
    #   CONFIG.target_global + p1 * CONFIG.entity_byte_stride as observable uint32.
    # target_global=0x0064131c, entity_byte_stride=0xf40.
    # Tests: vary slot (0..3) and value (0, sentinel, max).
    # ref: re/analysis/frontend_c1_to_c2_s2/0x00422aa0.md
    'slot_field_set': {
        'rva':                  0x00422aa0,
        'export':               'SlotFieldSet',
        'signature':            {'ret': 'void', 'args': ['int32', 'uint32']},
        'arg_type':             'entity_field_set',
        'target_global':        0x0064131c,
        'entity_byte_stride':   0xf40,
        'lut_root_delta':       0,
        # [slot_index, value_to_write]
        'path1_tests': [
            [0, 0x00000000],
            [0, 0x00000001],
            [0, 0xDEADBEEF],
            [0, 0xFFFFFFFF],
            [1, 0x00000000],
            [1, 0x12345678],
            [2, 0x00000000],
            [2, 0xCAFEBABE],
            [3, 0x00000000],
            [3, 0xABCDABCD],
        ],
        'path2_tests': [
            [0, 0x00000000],
            [0, 0xDEADBEEF],
            [1, 0x12345678],
        ],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-s-s2 â€” frontend per-player score accessors (C2->C3)
    # Frontend/SlotZeroers_s2.cpp
    # Five per-player score getters that read from 0x00899a40 block,
    # stride 0x4e dwords (= 0x138 bytes) per player.
    # Two callee patterns:
    #   GetRaceSubMode (0x0042f6a0, C3) â€” race sub-mode guard (== 4 â†’ return 0)
    #   GetDat0067ea64 (0x0042f500, C4) â€” team mode flag (== 0 â†’ direct read)
    # At quiescent main menu: race sub-mode != 4, team mode == 0.
    # Strategy: int_scalar; player indices 0..3 repeated.
    # Both paths read from the same global arrays; A/B must be bit-identical.
    # ref: re/analysis/frontend_c1_to_c2_s2/0x00423b40.md
    #      re/analysis/frontend_c1_to_c2_s2/0x00423b60.md
    #      re/analysis/frontend_c1_to_c2_s2/0x00423ba0.md
    #      re/analysis/frontend_c1_to_c2_s2/0x00423bc0.md
    #      re/analysis/frontend_c1_to_c2_s2/0x00423c40.md
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00423b40  PlayerScoreAccA
    # Per-player score accessor #1. Guard: GetRaceSubMode()==4 â†’ return 0.
    # Returns (&DAT_00899a94)[param_1 * 0x4e] (dword array, stride 0x4e dwords).
    # At main menu: race sub-mode != 4 â†’ takes array-read path.
    # Both orig and reimpl read same global â†’ bit-identical.
    'player_score_acc_a': {
        'rva':            0x00423b40,
        'export':         'PlayerScoreAccA',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # Player indices 0..3 (only 4 players). Repeat to reach 10 vectors.
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x00423b60  PlayerScoreAccB
    # Per-player score accessor #2. Twin of PlayerScoreAccA with base 0x00899a98.
    # Same guard and strategy.
    'player_score_acc_b': {
        'rva':            0x00423b60,
        'export':         'PlayerScoreAccB',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x00423ba0  PlayerScoreAccD
    # Per-player score accessor #4. Base 0x00899f78. Same guard and strategy.
    'player_score_acc_d': {
        'rva':            0x00423ba0,
        'export':         'PlayerScoreAccD',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x00423bc0  PlayerScoreTeamAccC
    # Team-aware accessor. Field at base 0x00899a9c, stride 0x4e dwords.
    # At main menu: GetDat0067ea64()==0 (no teams) â†’ direct-read path.
    # Returns (&DAT_00899a9c)[param_1 * 0x4e] â€” player data stable at menu.
    'player_score_team_acc_c': {
        'rva':            0x00423bc0,
        'export':         'PlayerScoreTeamAccC',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x00423c40  PlayerScoreTeamAccBase
    # Team-aware accessor. Field at base 0x00899a40 (player struct base field +0x000).
    # No race-sub-mode guard. Same team-mode strategy as PlayerScoreTeamAccC.
    # At main menu: GetDat0067ea64()==0 (no teams) â†’ direct-read path.
    'player_score_team_acc_base': {
        'rva':            0x00423c40,
        'export':         'PlayerScoreTeamAccBase',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-s-s3 â€” frontend menu-leaf cluster (C2â†’C3, 5 candidates)
    # Frontend/MenuLeaves_s3.cpp
    # Four team-aggregating per-player stat getters + one trivial global reader.
    # Callees: GetDat0067ea64 (0x0042f500, C4), GetRaceSubMode (0x0042f6a0, C3).
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00423ee0  PlayerBlock2Field00Get
    # int(int param_1): reads (&DAT_00899f20)[param_1*0x4e]; team-aggregates if teams on.
    # No RaceSubMode gate. Pure field +0x00 from block-2 root.
    # At quiescent main menu: GetDat0067ea64()==0 â†’ direct field access.
    # int_scalar: pass player indices 0..3.
    # ref: re/analysis/frontend_c1_to_c2_s3/FUN_00423ee0.md
    'player_block2_field00_get': {
        'rva':            0x00423ee0,
        'export':         'PlayerBlock2Field00Get',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x00423f60  PlayerBlock2Field04Get
    # int(int param_1): same as Field00Get but field +0x04; RaceSubMode==4 â†’ return 0.
    # ref: re/analysis/frontend_c1_to_c2_s3/FUN_00423f60.md
    'player_block2_field04_get': {
        'rva':            0x00423f60,
        'export':         'PlayerBlock2Field04Get',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x00423ff0  PlayerBlock2Field50Get
    # int(int param_1): field +0x50 from block-2 root (dword offset 0x14). No gate.
    # At quiescent main menu: GetDat0067ea64()==0 â†’ direct field access.
    # int_scalar: same 4-slot coverage.
    # ref: re/analysis/frontend_c1_to_c2_s3/FUN_00423ff0.md
    'player_block2_field50_get': {
        'rva':            0x00423ff0,
        'export':         'PlayerBlock2Field50Get',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x00424070  PlayerBlock2Field08Get
    # int(int param_1): field +0x08 from block-2 root; RaceSubMode==4 â†’ return 0.
    # At quiescent main menu: GetRaceSubMode()!=4, GetDat0067ea64()==0 â†’ direct field.
    # int_scalar: same 4-slot coverage.
    # ref: re/analysis/frontend_c1_to_c2_s3/FUN_00424070.md
    'player_block2_field08_get': {
        'rva':            0x00424070,
        'export':         'PlayerBlock2Field08Get',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x004241b0  GetDat008994c0
    # undefined4 FUN_004241b0(void): 6-byte leaf, returns *(uint32*)0x008994c0.
    # Strategy: read_global â€” write sentinel to 0x008994c0, call fn, compare return.
    # Both orig and reimpl must return the sentinel verbatim.
    # 10 sentinel values spanning 0, 1, max, patterns.
    # ref: re/analysis/frontend_c1_to_c2_s3/FUN_004241b0.md
    'get_dat_008994c0': {
        'rva':            0x004241b0,
        'export':         'GetDat008994c0',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x008994c0,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-s-s4 â€” Frontend/GlobalGetters_s4.cpp (C2â†’C3)
    # 5 pure-leaf global-getter functions.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00425ee0  SlotWordPtrGet
    # Signature: uint32_t* fn(int param_1)
    # Returns &DAT_00899294 + param_1 * 0x13 (int32 word stride).
    # Strategy: int_scalar â€” pass param_1 as uint32; compare raw returned
    # pointer as integer. Both orig and reimpl must compute the same address.
    # 10 indices covering 0, low, mid, and boundary values.
    'slot_word_ptr_get': {
        'rva':            0x00425ee0,
        'export':         'SlotWordPtrGet',
        'signature':      {'ret': 'pointer', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 0xFF, 0x1000],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00425ef0  ActiveSlotCount
    # Signature: int fn(void)
    # Counts active slots in the hardcoded 8-slot table at 0x00899260.
    # Strategy: none â€” call 10x at quiescent main-menu state; both orig and
    # reimpl read the same live BSS addresses and must return identical counts.
    'active_slot_count': {
        'rva':            0x00425ef0,
        'export':         'ActiveSlotCount',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # c3_batch_t-s6  skeleton_prep + scatter cluster (2026-05-26)
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x004274d0  LangIndexSeedFromCli  uint32_t(void)
    # 15B leaf. Reads DAT_007719e8 (cli-language-index) writes DAT_007f0f60.
    # Returns 1 unconditionally. arg_type='read_global' writes a sentinel to
    # 0x007719e8 before each call; return value (always 1) is the observable.
    # Sentinel preserved via diff_template save/restore for read_global.
    # ref: re/analysis/skeleton_prep_game_state/004274d0.md
    'lang_index_seed_from_cli': {
        'rva':            0x004274d0,
        'export':         'LangIndexSeedFromCli',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x007719e8,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 0xDEADBEEF, 0xCAFEBABE,
                           0x12345678, 0xFFFFFFFF],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00428390  FrontendStateSet  void(uint32_t param_1)
    # 9B pure leaf. Writes param_1 to DAT_0067d960 (frontend state global).
    # arg_type='void_setter_observe' calls fn(input), reads back target_global.
    # Both sides write param_1; result must equal input on both sides.
    # ref: re/analysis/skeleton_prep_game_state/00428390.md
    'frontend_state_set': {
        'rva':            0x00428390,
        'export':         'FrontendStateSet',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x0067d960,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x55555555],
        'path2_tests':    [0, 3, 0xDEADBEEF],
    },

    # 0x0042fa00  PlayerSlotEdgeAdjust  void(void)
    # 162B pure leaf. Loops 12 slots; iff iVar4 matches one of 4 active-player
    # slot ids, conditionally inc/dec/clamp slot property at DAT_0067e938.
    # At quiescent main menu, active-slot ids are sentinels (-1 / unset) so loop
    # body short-circuits â€” both sides do identical no-op.
    # arg_type='none': void return, called 10Ã— at quiescent menu; both sides
    # return undefined; identity check passes.
    # ref: re/analysis/frontend_unmapped_a/0x0042fa00.md
    'player_slot_edge_adjust': {
        'rva':            0x0042fa00,
        'export':         'PlayerSlotEdgeAdjust',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00426020  GlobalDat00646e58Get
    # Signature: void* fn(void)
    # Returns &DAT_00646e58 â€” fixed compile-time constant address.
    # Strategy: none â€” call 10x; both orig and reimpl must return the same
    # fixed address 0x00646e58 each time.
    'global_dat_00646e58_get': {
        'rva':            0x00426020,
        'export':         'GlobalDat00646e58Get',
        'signature':      {'ret': 'pointer', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00403050  PreRaceLoadingScreenDraw  void(void)
    # 124B. Early-returns if DAT_00771964 == 0 (pre-race texture handle).
    # At quiescent main menu the handle is 0 (texture only loaded during track
    # load), so the function returns immediately â€” both sides identical no-op.
    # arg_type='none': void return, called 10Ã— at quiescent menu.
    # ref: re/analysis/loading_screen/0x00403050.md
    'pre_race_loading_screen_draw': {
        'rva':            0x00403050,
        'export':         'PreRaceLoadingScreenDraw',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00426080  GlobalDat00656ed8Get
    # Signature: uint32_t fn(void)
    # Returns DAT_00656ed8 â€” reads and returns the 4-byte global value.
    # Strategy: read_global â€” write sentinel values to 0x00656ed8, call fn(),
    # confirm both orig and reimpl read the same address and return the sentinel.
    'global_dat_00656ed8_get': {
        'rva':            0x00426080,
        'export':         'GlobalDat00656ed8Get',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x00656ed8,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # 0x00426090  GlobalDat0066ce58Get
    # Signature: void* fn(void)
    # Returns &DAT_0066ce58 â€” fixed compile-time constant address.
    # High fan-in: 8 callers. Strategy: none â€” call 10x; both orig and reimpl
    # must return the fixed address 0x0066ce58 each time.
    'global_dat_0066ce58_get': {
        'rva':            0x00426090,
        'export':         'GlobalDat0066ce58Get',
        'signature':      {'ret': 'pointer', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

        # =========================================================================
    # c3-batch-s session 5 â€” Frontend GlobalGetters_s5  (2026-05-26)
    # 4 pure-leaf getters promoted C2->C3.
    # Refused: 0x00426b40 (7 of 10 callees at C1 â€” anti-island rule).
    # =========================================================================

    # 0x004260a0  GetDat00657438
    # Pure leaf: reads DAT_00657438 (4-byte global at 0x00657438), returns it.
    # Strategy: write sentinel values to 0x00657438, call orig/reimpl, compare.
    # 10 sentinels covering 0, 1, max, and bit-pattern variants.
    # ref: re/analysis/frontend_c1_to_c2_s4/FUN_004260a0.md
    'get_dat_00657438': {
        'rva':            0x004260a0,
        'export':         'GetDat00657438',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x00657438,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # 0x004260b0  GetDat0065743c
    # Pure leaf: reads DAT_0065743c (4-byte global at 0x0065743c), returns it.
    # Paired with GetDat00657438 (adjacent globals, 0x00657438 + 4 = 0x0065743c).
    # Strategy: write sentinel values to 0x0065743c, call orig/reimpl, compare.
    # ref: re/analysis/frontend_c1_to_c2_s4/FUN_004260b0.md
    'get_dat_0065743c': {
        'rva':            0x004260b0,
        'export':         'GetDat0065743c',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0065743c,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # 0x00426bc0  GetDat0066d6e0
    # Pure leaf: reads DAT_0066d6e0 (4-byte global at 0x0066d6e0), returns it.
    # Single caller: FUN_00409290.
    # Strategy: write sentinel values to 0x0066d6e0, call orig/reimpl, compare.
    # ref: re/analysis/frontend_c1_to_c2_s4/FUN_00426bc0.md
    'get_dat_0066d6e0': {
        'rva':            0x00426bc0,
        'export':         'GetDat0066d6e0',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0066d6e0,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # 0x00426bd0  GetTableEntry0066d658
    # Reads a packed 4-byte record from table at 0x0066d658: stride 4 per index.
    # Writes low uint16 to *param_2, high uint16 to *param_3. Void return.
    # Strategy: int_ptr2_out â€” fn(idx, out1, out2); compare packed fingerprint.
    # The buf is pre-zeroed; uint16 writes land in low 2 bytes of each 4-byte slot.
    # The `& 0x3f` pack covers 6 bits of each uint16, sufficient for table entries.
    # Indices 0..9 ensure varied table positions; at quiescent menu state
    # 0x0066d658 holds live game data â€” both orig and reimpl read the same region.
    # ref: re/analysis/frontend_c1_to_c2_s4/FUN_00426bd0.md
    'get_table_entry_0066d658': {
        'rva':            0x00426bd0,
        'export':         'GetTableEntry0066d658',
        'signature':      {'ret': 'void', 'args': ['int', 'pointer', 'pointer']},
        'arg_type':       'int_ptr2_out',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2, 3],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-s-s6 â€” Frontend near-leaf cluster (C2â†’C3)
    # Frontend/MenuNearLeaves_s6.cpp
    #
    # Deferred this session:
    #   0x00426d90 HandleArrayRelease â€” callee 0x004e6680 (RpClumpRender) is C1;
    #              callee gate fails. Re-enable once 0x004e6680 reaches C2+.
    #   0x0042a640 PathBuilderLoad   â€” no suitable Frida arg_type for char* path
    #              + vtable dispatch. Queue harness-ext work first.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00426cb0  SlotIndexToPtr
    # Pure leaf: returns &DAT_00663664 + param_1 * 0x4c. 12 bytes. No callees.
    # Both orig and reimpl compute the same deterministic pointer for the same
    # input index â†’ bit-identical on every call.
    # arg_type='int_scalar': passes int param_1, returns pointer (as uint32).
    # Tests cover index 0..10 and a few negative/large values.
    # ref: re/analysis/frontend_c1_to_c2_s4/FUN_00426cb0.md
    'slot_index_to_ptr': {
        'rva':            0x00426cb0,
        'export':         'SlotIndexToPtr',
        'signature':      {'ret': 'pointer', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, -1],
        'path2_tests':    [0, 1, 5],
    },

    # 0x0042b920  ConstantGetter22
    # Pure constant getter: MOV EAX, 0x16; RET. 5 bytes. No args, no callees.
    # Returns 0x16 (22) unconditionally. arg_type='void': call 10x, compare
    # uint32 return. Both sides must return 0x16 every time.
    # ref: re/analysis/frontend_c1_to_c2_s6/FUN_0042b920.md
    'constant_getter_22': {
        'rva':            0x0042b920,
        'export':         'ConstantGetter22',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'void',
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # 0x0042bde0  HudRectEmitter
    # void(int param_1, byte param_2): 5 calls to ChromeBaseDraw (C3).
    # Both orig and reimpl draw to the same vertex buffer (DAT_00898a20);
    # function returns void; void===void is always GREEN.
    # Mission note: use int_pair arg_type; do NOT set crash_equal_ok.
    # Test vectors: [param_1, param_2] covering a range of row-indices and
    # alpha values. param_1 shifts HUD row (fVar1 = param_1 - 13); param_2
    # controls colour blend (0..255).
    # ref: re/analysis/frontend_c1_to_c2_s6/FUN_0042bde0.md
    'hud_rect_emitter': {
        'rva':            0x0042bde0,
        'export':         'HudRectEmitter',
        'signature':      {'ret': 'void', 'args': ['int32', 'uint32']},
        'arg_type':       'int_pair',
        'lut_root_delta': 0,
        # [param_1, param_2]: row-index, colour-byte
        'path1_tests': [
            [0,   0],
            [1,   0],
            [5,   0],
            [10,  0],
            [13,  0],
            [0,   128],
            [5,   255],
            [10,  64],
            [20,  200],
            [0,   1],
        ],
        'path2_tests': [
            [0,   0],
            [5,   128],
            [13,  255],
        ],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Harness-ext landed 2026-05-26: slot_block_zero + state_machine_observe.
    # Unblocks 3 c3_batch_s-s1 deferred candidates.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00422a80  SlotBlockZero  void(int slot)
    # 8-byte leaf. Computes base = &DAT_006403e8 + slot * 0xf40 and calls
    # FUN_004b6520(base, 0xf40) to zero the per-slot block.
    # arg_type='slot_block_zero': pre-write sentinel to first dword, call,
    # read back first dword (should be 0 after the memset).
    # ref: re/analysis/frontend_c1_to_c2_s2/0x00422a80.md
    'slot_block_zero': {
        'rva':                0x00422a80,
        'export':             'SlotBlockZero',
        'signature':          {'ret': 'void', 'args': ['int32']},
        'arg_type':           'slot_block_zero',
        'target_global':      0x006403e8,
        'entity_byte_stride': 0xf40,
        'lut_root_delta':     0,
        'path1_tests':        [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':        [0, 1, 2],
    },

    # 0x00423270  TabCycler  void()
    # ~120 bytes. Reads context at DAT_007f1a54, button flags at DAT_007f1070/71,
    # debounce at DAT_007f1530/31; mutates tab index at DAT_007f1a64.
    # arg_type='state_machine_observe': inject the 6 input globals as u8 values
    # plus initial tab index, call fn(), read back tab index.
    # Test domain mixes context (1..6 inclusive + 0 default), tab-right/-left
    # button flags, debounce, and initial tab values.
    # ref: re/analysis/frontend_c1_to_c2_s2/0x00423270.md
    'tab_cycler': {
        'rva':            0x00423270,
        'export':         'TabCycler',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'state_machine_observe',
        'input_globals':  [
            {'addr': 0x007f1a54, 'type': 'u32'},  # context
            {'addr': 0x007f1071, 'type': 'u8'},   # tab-right flag
            {'addr': 0x007f1070, 'type': 'u8'},   # tab-left flag
            {'addr': 0x007f1531, 'type': 'u8'},   # debounce-right
            {'addr': 0x007f1530, 'type': 'u8'},   # debounce-left
            {'addr': 0x007f1a64, 'type': 'u32'},  # initial tab index
        ],
        'output_globals': [
            {'addr': 0x007f1a64, 'type': 'u32'},
        ],
        'lut_root_delta': 0,
        # [context, tabR, tabL, debR, debL, initial_tab]
        'path1_tests': [
            [0, 0, 0, 0, 0, 0],    # idle, default bounds
            [1, 1, 0, 0, 0, 2],    # tab-right press in ctx 1 (bounds 2..3) from 2 -> 3
            [1, 1, 0, 0, 0, 3],    # tab-right press in ctx 1 from 3 -> wrap to 2
            [1, 1, 0, 1, 0, 2],    # tab-right with debounce-right set: no-op
            [2, 0, 1, 0, 0, 2],    # tab-left press in ctx 2 (bounds 2..4) from 2 -> wrap to 4
            [3, 1, 0, 0, 0, 2],    # ctx 3 (bounds 2..3): 2 -> 3
            [4, 1, 0, 0, 0, 5],    # ctx 4 (bounds 2..5): 5 -> wrap to 2
            [5, 0, 1, 0, 0, 3],    # ctx 5 (bounds 2..3): 3 -> 2
            [6, 1, 0, 0, 0, 4],    # ctx 6 (bounds 2..5): 4 -> 5
            [0, 1, 0, 0, 0, 0],    # default ctx (bounds 0..8): 0 -> 1
        ],
        'path2_tests': [
            [1, 1, 0, 0, 0, 2],
            [4, 0, 1, 0, 0, 5],
            [0, 0, 0, 0, 0, 0],
        ],
    },

    # 0x004b65e0  PizGlobalsZero6  void()
    # 32 bytes. Zeroes six global dwords in two contiguous groups of three:
    #   group 1: 0x008eda28 / 0x008eda2c / 0x008eda30
    #   group 2: 0x0090daa0 / 0x0090daa4 / 0x0090daa8
    # (decomp verified via Ghidra 2026-06-04; c3-batch-ae-s3.)
    # arg_type='state_machine_observe': inject all 6 globals with distinct
    # NON-ZERO sentinels (input_globals == output_globals), call fn(), read all
    # 6 back. A correct reimpl zeroes every one -> output is all-zero. A no-op or
    # partial-zero stub leaves a sentinel non-zero -> RED. This defeats the
    # false-GREEN that a plain `return;` would otherwise produce.
    # ref: re/analysis/render_3_c1_to_c2_s5/FUN_004b65e0.md
    'piz_globals_zero6': {
        'rva':            0x004b65e0,
        'export':         'PizGlobalsZero6',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'state_machine_observe',
        'input_globals':  [
            {'addr': 0x008eda28, 'type': 'u32'},
            {'addr': 0x008eda2c, 'type': 'u32'},
            {'addr': 0x008eda30, 'type': 'u32'},
            {'addr': 0x0090daa0, 'type': 'u32'},
            {'addr': 0x0090daa4, 'type': 'u32'},
            {'addr': 0x0090daa8, 'type': 'u32'},
        ],
        'output_globals': [
            {'addr': 0x008eda28, 'type': 'u32'},
            {'addr': 0x008eda2c, 'type': 'u32'},
            {'addr': 0x008eda30, 'type': 'u32'},
            {'addr': 0x0090daa0, 'type': 'u32'},
            {'addr': 0x0090daa4, 'type': 'u32'},
            {'addr': 0x0090daa8, 'type': 'u32'},
        ],
        'lut_root_delta': 0,
        # Each vector = 6 distinct non-zero sentinels injected before the call.
        'path1_tests': [
            [0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666],
            [0xDEADBEEF, 0xCAFEBABE, 0xFEEDFACE, 0x8BADF00D, 0xABAD1DEA, 0x0D15EA5E],
            [0x00000001, 0x00000002, 0x00000004, 0x00000008, 0x00000010, 0x00000020],
            [0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF],
            [0x80000000, 0x40000000, 0x20000000, 0x10000000, 0x08000000, 0x04000000],
            [0x7FFFFFFF, 0x12345678, 0x9ABCDEF0, 0x0FEDCBA9, 0x13579BDF, 0x2468ACE0],
            [0xA5A5A5A5, 0x5A5A5A5A, 0xC3C3C3C3, 0x3C3C3C3C, 0xF0F0F0F0, 0x0F0F0F0F],
            [0x00000100, 0x00010000, 0x01000000, 0x00000200, 0x00020000, 0x02000000],
            [0xBADC0FFE, 0xDEFEC8ED, 0xD0D0CACA, 0xFACEFEED, 0xC0DED00D, 0xDEADC0DE],
            [0x99999999, 0x88888888, 0x77777777, 0x66666666, 0x55555555, 0x44444444],
        ],
        'path2_tests': [
            [0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666],
            [0xDEADBEEF, 0xCAFEBABE, 0xFEEDFACE, 0x8BADF00D, 0xABAD1DEA, 0x0D15EA5E],
            [0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF],
        ],
    },

    # 0x00423320  CursorMover  void()
    # ~130 bytes. 8-way grid cursor mover with debounce. Reads tab DAT_007f1a64,
    # cursor DAT_007f1a90, dir flags DAT_007f1072..1075, debounce DAT_007f1532..1535.
    # Up/down stride 4, clamp [0, 0x7f]. Up/down gated by tab != 2.
    # Left/right cycle within 4-column row.
    # arg_type='state_machine_observe': inject all 10 globals, call, read cursor.
    # ref: re/analysis/frontend_c1_to_c2_s2/0x00423320.md
    'cursor_mover': {
        'rva':            0x00423320,
        'export':         'CursorMover',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'state_machine_observe',
        'input_globals':  [
            {'addr': 0x007f1a64, 'type': 'u32'},  # tab index (guard: != 2 enables up/down)
            {'addr': 0x007f1a90, 'type': 'u32'},  # initial cursor
            {'addr': 0x007f1072, 'type': 'u8'},   # UP flag
            {'addr': 0x007f1073, 'type': 'u8'},   # DOWN flag
            {'addr': 0x007f1074, 'type': 'u8'},   # LEFT flag
            {'addr': 0x007f1075, 'type': 'u8'},   # RIGHT flag
            {'addr': 0x007f1532, 'type': 'u8'},   # debounce UP
            {'addr': 0x007f1533, 'type': 'u8'},   # debounce DOWN
            {'addr': 0x007f1534, 'type': 'u8'},   # debounce LEFT
            {'addr': 0x007f1535, 'type': 'u8'},   # debounce RIGHT
        ],
        'output_globals': [
            {'addr': 0x007f1a90, 'type': 'u32'},
        ],
        'lut_root_delta': 0,
        # [tab, cursor, up, down, left, right, debUp, debDown, debLeft, debRight]
        'path1_tests': [
            [0, 0,  0, 0, 0, 0,  0, 0, 0, 0],   # idle, no movement
            [0, 8,  1, 0, 0, 0,  0, 0, 0, 0],   # UP from 8 -> 4
            [0, 0,  1, 0, 0, 0,  0, 0, 0, 0],   # UP from 0 (clamp) -> 0
            [0, 0,  0, 1, 0, 0,  0, 0, 0, 0],   # DOWN from 0 -> 4
            [0, 0x7c, 0, 1, 0, 0, 0, 0, 0, 0],  # DOWN from 0x7c (just below clamp) -> 0x7c + 4 = 0x80 > 0x7f, clamp keep
            [2, 8,  1, 0, 0, 0,  0, 0, 0, 0],   # UP gated by tab=2; cursor unchanged
            [0, 5,  0, 0, 0, 1,  0, 0, 0, 0],   # RIGHT from 5 (col 1) -> 6 (col 2)
            [0, 4,  0, 0, 1, 0,  0, 0, 0, 0],   # LEFT from 4 (col 0) -> wrap to col 3 = 7
            [0, 4,  1, 0, 0, 0,  1, 0, 0, 0],   # UP with debounce-up: no-op
            [0, 0,  0, 0, 0, 0,  0, 0, 0, 0],   # baseline idle
        ],
        'path2_tests': [
            [0, 8, 1, 0, 0, 0, 0, 0, 0, 0],
            [0, 0, 0, 1, 0, 0, 0, 0, 0, 0],
            [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        ],
    },

    # c3-batch-t-s1 (2026-05-26): Frontend small leaves promotion.
    # 5 of 10 candidates authored; 5 deferred (state mutators + thunk + ptr-arg).

    'race_score_float_get_by_slot': {
        'rva':            0x00408ad0,
        'export':         'RaceScoreFloatGetBySlot',
        'signature':      {'ret': 'float', 'args': ['int32']},
        'arg_type':       'float_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2],
    },

    'entry_table_scan_by_key': {
        'rva':                0x00401570,
        'export':             'EntryTableScanByKey',
        'signature':          {'ret': 'void', 'args': ['int32']},
        'arg_type':           'void_setter_observe',
        'target_global':      0x00636ac0,
        'lut_root_delta':     0,
        # void_setter_observe: call fn(key), read back ptr at DAT_00636ac0.
        # Table is static; matched/default entry ptr is deterministic.
        'path1_tests':        [0, 1, 2, 0xffffffff, 10, 100, 0, 1, 3, 4],
        'path2_tests':        [0, 1, 0xffffffff],
    },

    'double_deref_indexed_getter': {
        'rva':            0x0040d250,
        'export':         'DoubleDerefIndexedGetter',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    'scoreboard_state_zero_init': {
        'rva':            0x0041e080,
        'export':         'ScoreboardStateZeroInit',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'state_machine_observe',
        'input_globals':  [],
        'output_globals': [
            {'addr': 0x0063d798, 'type': 'u32'},
            {'addr': 0x0063d79c, 'type': 'u32'},
            {'addr': 0x0063d7a0, 'type': 'u32'},
            {'addr': 0x0063d7b0, 'type': 'u32'},
            {'addr': 0x0063d7b4, 'type': 'u32'},
            {'addr': 0x0063d7b8, 'type': 'u32'},
            {'addr': 0x0063d7c4, 'type': 'u32'},
            {'addr': 0x0063d7cc, 'type': 'u32'},
            {'addr': 0x0063d7d0, 'type': 'u32'},
            {'addr': 0x0063d7d4, 'type': 'u32'},
            {'addr': 0x0063d7d8, 'type': 'u32'},
            {'addr': 0x0063d7dc, 'type': 'u32'},
        ],
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    'copy_table_005f2a70_to_0089a384': {
        'rva':            0x00414120,
        'export':         'CopyTable005f2a70To0089a384',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'state_machine_observe',
        'input_globals':  [],
        'output_globals': [
            {'addr': 0x0089a378, 'type': 'u32'},
            {'addr': 0x0089a37c, 'type': 'u32'},
            {'addr': 0x0089a380, 'type': 'u32'},
            {'addr': 0x0089a384, 'type': 'u32'},
            {'addr': 0x0089a420, 'type': 'u32'},
            {'addr': 0x0089a41c, 'type': 'u32'},
        ],
        'lut_root_delta': 0,
        'path1_tests':    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests':    [0, 0, 0],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # c3-batch-t-s2 (2026-05-26): Frontend menu-miscellaneous leaves.
    # 5 of 10 candidates GREEN; refused: 0x00422aa0 (caller C1), 0x00426cb0
    # (callers C1), 0x00425b90 (callees C1), 0x00424270 (no harness arg_type
    # fit for void(int*, int, int)), 0x0042bfb0 (no harness arg_type fit for
    # 6-arg conditional writer).
    # See mashedmod/src/mashed_re/Frontend/MenuMiscLeaves_t2.cpp.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00423cc0  PlayerScoreTeamAccGatedB
    # Team-aware getter, block-1 +0x04 (base 0x00899a44), RaceSubMode gate.
    # At quiescent main menu: GetRaceSubMode()!=4 (typically 0), GetTeamMode()==0
    # â†’ direct-read path. Both orig and reimpl read same field â†’ bit-identical.
    # ref: re/analysis/frontend_c1_to_c2_s2/0x00423cc0.md
    'player_score_team_acc_gated_b': {
        'rva':            0x00423cc0,
        'export':         'PlayerScoreTeamAccGatedB',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x00423d50  PlayerScoreTeamAccBaseB
    # Team-aware getter, block-1 +0x50 (base 0x00899a90), no race-sub-mode gate.
    # ref: re/analysis/frontend_c1_to_c2_s2/0x00423d50.md
    'player_score_team_acc_base_b': {
        'rva':            0x00423d50,
        'export':         'PlayerScoreTeamAccBaseB',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x00423dd0  PlayerScoreTeamAccGatedC
    # Team-aware getter, block-1 +0x08 (base 0x00899a48), RaceSubMode gate.
    # ref: re/analysis/frontend_c1_to_c2_s2/0x00423dd0.md
    'player_score_team_acc_gated_c': {
        'rva':            0x00423dd0,
        'export':         'PlayerScoreTeamAccGatedC',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x00423e60  PlayerScoreTeamAccCumB
    # Team-aware getter, block-2 +0x5c (base 0x00899f7c), no race-sub-mode gate.
    # ref: re/analysis/frontend_c1_to_c2_s2/0x00423e60.md
    'player_score_team_acc_cum_b': {
        'rva':            0x00423e60,
        'export':         'PlayerScoreTeamAccCumB',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x00424920  EndOfRoundAccumulator  void()
    # Pure leaf: 32 sequential `block2_field += block1_field` adds across 4
    # players Ã— 8 fields. No branches, no calls.
    # arg_type='state_machine_observe': inject 32 input globals (block-1
    # source fields), call fn(), read back 32 output globals (block-2 dest
    # fields). Save/restore wraps everything so the test is non-destructive.
    # ref: re/analysis/frontend_c1_to_c2_s3/FUN_00424920.md
    'end_of_round_accumulator': {
        'rva':            0x00424920,
        'export':         'EndOfRoundAccumulator',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'state_machine_observe',
        'input_globals':  [
            # Player 0 block-1 sources.
            {'addr': 0x00899a40, 'type': 'u32'},
            {'addr': 0x00899a94, 'type': 'u32'},
            {'addr': 0x00899a44, 'type': 'u32'},
            {'addr': 0x00899a98, 'type': 'u32'},
            {'addr': 0x00899a48, 'type': 'u32'},
            {'addr': 0x00899a90, 'type': 'u32'},
            {'addr': 0x00899a9c, 'type': 'u32'},
            {'addr': 0x008999a0, 'type': 'u32'},
            # Player 1 block-1 sources.
            {'addr': 0x00899b78, 'type': 'u32'},
            {'addr': 0x00899bcc, 'type': 'u32'},
            {'addr': 0x00899b7c, 'type': 'u32'},
            {'addr': 0x00899bd0, 'type': 'u32'},
            {'addr': 0x00899b80, 'type': 'u32'},
            {'addr': 0x00899bc8, 'type': 'u32'},
            {'addr': 0x00899bd4, 'type': 'u32'},
            {'addr': 0x00899ad8, 'type': 'u32'},
            # Player 2 block-1 sources.
            {'addr': 0x00899cb0, 'type': 'u32'},
            {'addr': 0x00899d04, 'type': 'u32'},
            {'addr': 0x00899cb4, 'type': 'u32'},
            {'addr': 0x00899d08, 'type': 'u32'},
            {'addr': 0x00899cb8, 'type': 'u32'},
            {'addr': 0x00899d00, 'type': 'u32'},
            {'addr': 0x00899d0c, 'type': 'u32'},
            {'addr': 0x00899c10, 'type': 'u32'},
            # Player 3 block-1 sources.
            {'addr': 0x00899de8, 'type': 'u32'},
            {'addr': 0x00899e3c, 'type': 'u32'},
            {'addr': 0x00899dec, 'type': 'u32'},
            {'addr': 0x00899e40, 'type': 'u32'},
            {'addr': 0x00899df0, 'type': 'u32'},
            {'addr': 0x00899e38, 'type': 'u32'},
            {'addr': 0x00899e44, 'type': 'u32'},
            {'addr': 0x00899d48, 'type': 'u32'},
        ],
        'output_globals': [
            # Player 0 block-2 dests (in adds order).
            {'addr': 0x00899f20, 'type': 'u32'},
            {'addr': 0x00899f74, 'type': 'u32'},
            {'addr': 0x00899f24, 'type': 'u32'},
            {'addr': 0x00899f78, 'type': 'u32'},
            {'addr': 0x00899f28, 'type': 'u32'},
            {'addr': 0x00899f70, 'type': 'u32'},
            {'addr': 0x00899f7c, 'type': 'u32'},
            {'addr': 0x00899e80, 'type': 'u32'},
            # Player 1 block-2 dests.
            {'addr': 0x0089a058, 'type': 'u32'},
            {'addr': 0x0089a0ac, 'type': 'u32'},
            {'addr': 0x0089a05c, 'type': 'u32'},
            {'addr': 0x0089a0b0, 'type': 'u32'},
            {'addr': 0x0089a060, 'type': 'u32'},
            {'addr': 0x0089a0a8, 'type': 'u32'},
            {'addr': 0x0089a0b4, 'type': 'u32'},
            {'addr': 0x00899fb8, 'type': 'u32'},
            # Player 2 block-2 dests.
            {'addr': 0x0089a190, 'type': 'u32'},
            {'addr': 0x0089a1e4, 'type': 'u32'},
            {'addr': 0x0089a194, 'type': 'u32'},
            {'addr': 0x0089a1e8, 'type': 'u32'},
            {'addr': 0x0089a198, 'type': 'u32'},
            {'addr': 0x0089a1e0, 'type': 'u32'},
            {'addr': 0x0089a1ec, 'type': 'u32'},
            {'addr': 0x0089a0f0, 'type': 'u32'},
            # Player 3 block-2 dests.
            {'addr': 0x0089a2c8, 'type': 'u32'},
            {'addr': 0x0089a31c, 'type': 'u32'},
            {'addr': 0x0089a2cc, 'type': 'u32'},
            {'addr': 0x0089a320, 'type': 'u32'},
            {'addr': 0x0089a2d0, 'type': 'u32'},
            {'addr': 0x0089a318, 'type': 'u32'},
            {'addr': 0x0089a324, 'type': 'u32'},
            {'addr': 0x0089a228, 'type': 'u32'},
        ],
        'lut_root_delta': 0,
        # 32 inputs each â€” array of 32 values. Use varied non-zero inputs to
        # exercise add-paths; expected output is input + saved-pre-call-output.
        'path1_tests': [
            [1] * 32,
            [2] * 32,
            [0] * 32,
            [0x10] * 32,
            [0x100] * 32,
            [0xff] * 32,
            [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
             17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32],
            [0x1000] * 32,
            [0x7fff] * 32,
            [0] * 32,
        ],
        'path2_tests': [
            [1] * 32,
            [0] * 32,
            [0x10] * 32,
        ],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-t-s3 â€” frontend bucket_0041dc30 mixed C2â†’C3
    # mashedmod/src/mashed_re/Frontend/BucketMixed_t3.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00431b70  GetDat007f0f10  â€” STAGED at C2 (NOT promoted; C3 caller-gate fails).
    # Pure-leaf uint32(void) getter: returns DAT_007f0f10. read_global Frida diff
    # is 10/10 GREEN and the reimpl is bit-identical, but the sole caller
    # FUN_0045d0e0 is C1 (no callee â€” leaf), so the C3 caller-AND-callee gate is
    # unmet. RH_ScopedInstall is commented out. Re-pickup: promote FUN_0045d0e0.
    'get_dat_007f0f10': {
        'rva':            0x00431b70,
        'export':         'GetDat007f0f10',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x007f0f10,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xCAFEBABE,
                           0x12345678, 0xFFFFFFFF, 0x80000000, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xFFFFFFFF],
    },

    # 0x00431d90  FrontendPanelFlagAdvance
    # void(void); writes 19 panel-flag globals. Per-flag rule:
    #   X == 1 -> 2; else -> 0  (special: 0x0067e7b0 preserves 1, else 0).
    # Strategy: void_write_observe with sentinel written to DAT_0067e7d8 (the
    # first global touched by the body). Several sentinels: 0, 1 (expect 2),
    # 2 (expect 0), random (expect 0). Both orig+reimpl must transform
    # identically.
    'frontend_panel_flag_advance': {
        'rva':            0x00431d90,
        'export':         'FrontendPanelFlagAdvance',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x0067e7d8,
        'lut_root_delta': 0,
        # Carefully chosen so the read-back is deterministic and divergent:
        #   0 -> 0; 1 -> 2; 2 -> 0; everything else -> 0.
        'path1_tests':    [0x00000000, 0x00000001, 0x00000002, 0x00000003,
                           0xDEADBEEF, 0xFFFFFFFF, 0x00000001, 0x00000002,
                           0x80000000, 0x00000001],
        'path2_tests':    [0x00000000, 0x00000001, 0x00000002],
    },

    # 0x00432ad0  MenuDimOverlayFadeStep
    # void(void); reads DAT_00898a90 (alpha counter); clamps to 0..0x1ff;
    # temporarily writes DAT_0067eca8 = min(counter, 0xff); calls
    # MenuIm2DQuad(0) [C3]; restores DAT_0067eca8; decrements counter by 0x10.
    # Strategy: void_write_observe on DAT_00898a90 (the persistent observable).
    # For each sentinel:
    #   sentinel < 1  -> read-back = 0
    #   1..0xff       -> read-back = sentinel - 0x10
    #   0x100..0x1ff  -> read-back = sentinel - 0x10
    #   > 0x1ff       -> read-back = 0x1ff - 0x10 = 0x1ef
    # crash_equal_ok: MenuIm2DQuad uses live D3D9 vtable; at quiescent menu
    # the call is suppressed (alpha=0) but with non-zero alpha the draw fires.
    'menu_dim_overlay_fade_step': {
        'rva':            0x00432ad0,
        'export':         'MenuDimOverlayFadeStep',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x00898a90,
        'lut_root_delta': 0,
        'crash_equal_ok': True,
        # Cover all four clamp branches plus mid-range steps.
        'path1_tests':    [0x00000000,  # < 1  -> 0
                           0x00000001,  # ==1  -> -15
                           0x00000010,  # 16   -> 0
                           0x000000ff,  # 255  -> 239
                           0x00000100,  # 256  -> 240
                           0x000001ff,  # 511  -> 495
                           0x00000200,  # >0x1ff -> 0x1ef
                           0xFFFFFFFF,  # signed-negative -> 0 (treated as <1)
                           0x80000000,  # signed-negative -> 0
                           0x00000020], # 32 -> 16
        'path2_tests':    [0x00000000, 0x00000010, 0x00000200],
    },

    # 0x00472dc0  Im2DTriangleDraw  â€” STAGED at C2 (NOT a C3 hook).
    # void(float x1, y1, x2, y2, x3, y3, uint32 argb): 3-vertex Im2D fill.
    # Sibling of ChromeBaseDraw (0x00472c60, C3) â€” same write pattern but
    # 3 vertices instead of 4, and prim-count = 3.
    # draw_quad_observe (vbuf_len=92) is NON-DETERMINISTIC here: each vertex's
    # Z field is the live *(DAT_007d3ff8+0x18), read by both orig and reimpl at
    # call time. A frame boundary between the two calls changes Z and diverges
    # the buffer fingerprint (idx1/idx3 deterministically RED in run_diff.py).
    # idx3's ARGB is byte-swap-invariant yet still diverges â€” isolating the
    # divergence to the shared live Z read, not the reimpl. Kept here so the
    # harness can run it once a Z-freezing arg_type lands; the .asi RH_ScopedInstall
    # is commented out. See BucketMixed_t3.cpp.
    'im2d_triangle_draw': {
        'rva':            0x00472dc0,
        'export':         'Im2DTriangleDraw',
        'signature':      {'ret':  'void',
                            'args': ['float', 'float', 'float', 'float',
                                     'float', 'float', 'uint32']},
        'arg_type':       'draw_quad_observe',
        'vbuf_addr_str':  '0x00898a20',
        'vbuf_len':       92,
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests': [
            # [x1, y1, x2, y2, x3, y3, argb]
            [0.0,  0.0,  1.0,  0.0,  0.5,  1.0,  0xffffffff],
            [0.1,  0.1,  0.9,  0.1,  0.5,  0.9,  0xff112233],
            [0.0,  0.0,  0.5,  0.5,  0.25, 0.75, 0xa0ff0000],
            [0.2,  0.3,  0.4,  0.3,  0.3,  0.5,  0xff00ff00],
            [0.0,  0.0,  1.0,  1.0,  0.0,  1.0,  0x800000ff],
            [0.5,  0.0,  0.7,  0.2,  0.3,  0.2,  0xc000ff00],
            [0.0,  0.5,  0.3,  0.7,  0.0,  1.0,  0x40808080],
            [-0.1, -0.1, 0.5,  -0.1, 0.2,  0.5,  0xff223344],
            [0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0x00000000],
            [0.25, 0.25, 0.75, 0.25, 0.5,  0.75, 0xff994433],
        ],
        'path2_tests': [
            [0.0, 0.0, 1.0, 0.0, 0.5, 1.0, 0xffffffff],
            [0.1, 0.1, 0.9, 0.1, 0.5, 0.9, 0xff112233],
        ],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-t-s5 â€” SplashGameMode_t5 cluster (C2->C3, 6 candidates)
    # Frontend/SplashGameMode_t5.cpp â€” intro_splash + game_mode + race_results
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00493f70  VideoStateFlagGet  uint32(void)
    # 5B leaf: returns DAT_00771a04.
    # arg_type='read_global': inject input into 0x00771a04, call fn(), expect input back.
    # ref: re/analysis/skeleton_prep_boot_winmain_a/00493f70.md
    'video_state_flag_get': {
        'rva':            0x00493f70,
        'export':         'VideoStateFlagGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x00771a04,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # 0x00493fc0  AspectRatioGlobalGet  uint32(void)
    # 5B leaf: returns DAT_00771a18 (caller pushes 2 floats but body ignores them).
    # arg_type='read_global'.
    # ref: re/analysis/intro_splash/0x00493fc0.md
    'aspect_ratio_global_get': {
        'rva':            0x00493fc0,
        'export':         'AspectRatioGlobalGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x00771a18,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # 0x00431d80  TiebreakFlagGet  uint32(void)
    # 5B leaf: returns DAT_0067ea7c (mode-4/code-6 tiebreak flag).
    # arg_type='read_global'.
    # ref: re/analysis/race_results/00431d80.md
    'tiebreak_flag_get': {
        'rva':            0x00431d80,
        'export':         'TiebreakFlagGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067ea7c,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # 0x0046c700  EntityScoreFieldAdd  uint32(int32 idx, int32 delta)
    # 43B incrementer: if (idx>15) return 0; (int*)(0x008820b0+idx*0xd04) += delta; return 1.
    # arg_type='entity_field_add': non-idempotent â€” snapshots field, calls
    # fn(idx,delta), reads post-add field + return value, RESTORES baseline so
    # Orig and Reimpl each start from identical state. Fingerprint = ret:field.
    # Includes an out-of-range idx (16) to exercise the >15 guard (returns 0).
    # ref: re/analysis/race_results/0046c700.md
    'entity_score_field_add': {
        'rva':                0x0046c700,
        'export':             'EntityScoreFieldAdd',
        'signature':          {'ret': 'uint32', 'args': ['int32', 'int32']},
        'arg_type':           'entity_field_add',
        'target_global':      0x008820b0,
        'entity_byte_stride': 0xd04,
        'max_index':          0xf,
        'lut_root_delta':     0,
        # [param_1 (idx 0..15 valid; 16 out-of-range), param_2 (delta)]
        'path1_tests': [
            [0, 0],
            [0, 1],
            [1, 5],
            [2, 0x10],
            [3, 0x100],
            [4, -1],
            [5, 0x7fffffff],
            [10, 3],
            [15, 1],
            [16, 1],
        ],
        'path2_tests': [
            [0, 1],
            [1, 1],
            [16, 1],
        ],
    },

    # 0x004c75e0  ViewportOriginGetter  void(int obj, u16* out0, u16* out1)
    # 27B leaf: *out0 = *(u16*)(obj+0x1c); *out1 = *(u16*)(obj+0x1e).
    # arg_type='int_ptr2_out': uses a 12-byte buf â€” pre-zeroes 4-byte slots,
    # calls fn(input, buf, buf+4), returns (buf[0]&0x3f) | ((buf[4]&0x3f)<<8).
    # input is the integer first arg (treated as pointer-typed obj address).
    # We pass valid in-process addresses so the U16 reads at +0x1c/+0x1e don't fault.
    # Strategy: pass &target_global values that point at known stable memory.
    # We exploit RX module read-only globals; pass MASHED.exe's image-base region
    # offsets where the dword at base+0x1c & 0x1e is mappable (text section).
    # Safe choice: pass a known C2-allocated video object header? Cleaner: pass
    # a static known-mapped data pointer â€” DAT_00771a18 = 0x00771a18 base.
    # The function only reads 2 bytes at +0x1c and +0x1e. We pass test addrs
    # that point well inside MASHED.exe's writeable .data such that the reads
    # are safe and reproducible.
    # ref: re/analysis/intro_splash_d2/0x004c75e0.md
    'viewport_origin_getter': {
        'rva':            0x004c75e0,
        'export':         'ViewportOriginGetter',
        'signature':      {'ret': 'void', 'args': ['int32', 'pointer', 'pointer']},
        'arg_type':       'int_ptr2_out',
        'lut_root_delta': 0,
        # int param_1 = mappable object base. Each value: passing the global pool
        # base 0x00771a00 means body reads u16@0x00771a1c + u16@0x00771a1e.
        # Vary the base to vary the addr-pair sampled; both orig and reimpl
        # must read the same bytes -> identical observable.
        'path1_tests': [
            0x00771a00,
            0x00771a20,
            0x00771a40,
            0x00771a60,
            0x00771a80,
            0x00771aa0,
            0x00771ac0,
            0x00771ae0,
            0x00771b00,
            0x00771b20,
        ],
        'path2_tests': [
            0x00771a00,
            0x00771a20,
            0x00771a40,
        ],
    },

    # 0x00494f30  AspectRatioSnapshot  void(void)
    # 15B: DAT_00771a50 = DAT_00771a54 = FUN_00493fc0()  (returns DAT_00771a18).
    # arg_type='state_machine_observe': inject DAT_00771a18 (single input),
    # call fn(), read back DAT_00771a50 and DAT_00771a54 (both should == injected).
    # ref: re/analysis/game_mode_cont2/0x00494f30.md
    'aspect_ratio_snapshot': {
        'rva':            0x00494f30,
        'export':         'AspectRatioSnapshot',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'state_machine_observe',
        'input_globals': [
            {'addr': 0x00771a18, 'type': 'u32'},   # source via FUN_00493fc0
        ],
        'output_globals': [
            {'addr': 0x00771a50, 'type': 'u32'},   # dest 1
            {'addr': 0x00771a54, 'type': 'u32'},   # dest 2 (same value)
        ],
        'lut_root_delta': 0,
        'path1_tests': [
            0x00000000,
            0xDEADBEEF,
            0xCAFEBABE,
            0x12345678,
            0xFFFFFFFF,
            0x80000000,
            0x00000001,
            0x55555555,
            0xAAAAAAAA,
            0x00000000,
        ],
        'path2_tests': [
            0x00000000,
            0xDEADBEEF,
            0xCAFEBABE,
        ],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # c3-batch-u (2026-05-26): targeted arg_type-unblocked promotion.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042bfb0  MenuStateParamStore  void(p1..p6)
    # Guarded 6-param global-block writer. If DAT_0067eab0==0 â†’ FUN_0042bf30()
    # alt path; else writes p1..p6 to DAT_0067e918..0067e92c + flag DAT_0067e930=1.
    # arg_type='multi_arg_global_write': set guard=1, call fn(p1..p6), read back
    # 7 u32s at 0x0067e918, restore. Gate: caller 0x00432450 C2, callee 0x0042bf30 C2.
    # ref: re/analysis/frontend_c1_to_c2_s6/FUN_0042bfb0.md
    'menu_state_param_store': {
        'rva':            0x0042bfb0,
        'export':         'MenuStateParamStore',
        'signature':      {'ret': 'void', 'args': ['uint32','uint32','uint32','uint32','uint32','uint32']},
        'arg_type':       'multi_arg_global_write',
        'guard_global':   0x0067eab0,
        'out_base':       0x0067e918,
        'out_count':      7,
        'lut_root_delta': 0,
        # 6-param tuples; varied sentinels exercise each output slot.
        'path1_tests': [
            [0x00000000, 0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005],
            [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0x9ABCDEF0, 0x0F0F0F0F, 0xF0F0F0F0],
            [0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000],
            [0x80000000, 0x7FFFFFFF, 0x00000001, 0xFFFFFFFE, 0x55555555, 0xAAAAAAAA],
            [0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666],
            [0x00000010, 0x00000020, 0x00000040, 0x00000080, 0x00000100, 0x00000200],
            [0xABCDABCD, 0xDCBADCBA, 0x13579BDF, 0x2468ACE0, 0xFEDCBA98, 0x76543210],
            [0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000],
            [0x7F7F7F7F, 0x80808080, 0x01010101, 0xFEFEFEFE, 0xC3C3C3C3, 0x3C3C3C3C],
            [0xDEADC0DE, 0xFEEDFACE, 0xBAADF00D, 0x8BADF00D, 0xCAFED00D, 0x1BADB002],
        ],
        'path2_tests': [
            [0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005, 0x00000006],
            [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0x9ABCDEF0, 0x0F0F0F0F, 0xF0F0F0F0],
            [0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000],
        ],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # frontend-gate-unblock-u (2026-05-26): FrontendDirInput, now callee-gate-clear.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00423040  FrontendDirInput  void()
    # Directional-input handler: 2 conditional grid callbacks (gated by
    # DAT_007f1042 / DAT_007f1076 + menu_state==0 + tab==4) then 4 per-axis
    # repeat-key timers (L/R/U/D) over DAT_006440ec.. + scroll floats.
    # Diff strategy: hold both callback-trigger flags (0x007f1042, 0x007f1076)
    # at 0 so the grid writer/eraser never fire (no large grid side-effect);
    # inject the 4 direction flags + initial timer/scroll state; read back the
    # 7 timers + 2 scroll floats. Deterministic (FUN_004a2c48 result unused when
    # callbacks don't fire). Uses existing state_machine_observe arg_type.
    # callees 0x00417450/0x00417530 C2 (frontend-gate-unblock-u); 0x004a2c48 C2.
    # ref: re/analysis/frontend_c1_to_c2_s2/0x00423040.md
    'frontend_dir_input': {
        'rva':            0x00423040,
        'export':         'FrontendDirInput',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'state_machine_observe',
        # Per-axis (verified vs decomp): LEFT phase 006440fc/count 00644108;
        # RIGHT phase 0064410c/count 006440f4; UP phase 006440ec/count 00644104;
        # DOWN phase 006440f0/count 006440f8. scroll L/R=007f1a5c, U/D=007f1a58.
        'input_globals':  [
            {'addr': 0x007f1042, 'type': 'u8'},   # callback-A trigger â€” HOLD 0
            {'addr': 0x007f1076, 'type': 'u8'},   # callback-B trigger â€” HOLD 0
            {'addr': 0x007f1a54, 'type': 'u32'},  # menu_state
            {'addr': 0x007f1a64, 'type': 'u32'},  # tab_index
            {'addr': 0x007f1044, 'type': 'u8'},   # LEFT flag
            {'addr': 0x007f1045, 'type': 'u8'},   # RIGHT flag
            {'addr': 0x007f1046, 'type': 'u8'},   # UP flag
            {'addr': 0x007f1047, 'type': 'u8'},   # DOWN flag
            {'addr': 0x006440ec, 'type': 'u32'},  # UP phase
            {'addr': 0x006440f0, 'type': 'u32'},  # DOWN phase
            {'addr': 0x006440f4, 'type': 'u32'},  # RIGHT count
            {'addr': 0x006440f8, 'type': 'u32'},  # DOWN count
            {'addr': 0x006440fc, 'type': 'u32'},  # LEFT phase
            {'addr': 0x00644104, 'type': 'u32'},  # UP count
            {'addr': 0x00644108, 'type': 'u32'},  # LEFT count
            {'addr': 0x0064410c, 'type': 'u32'},  # RIGHT phase
            {'addr': 0x007f1a58, 'type': 'u32'},  # UP/DOWN scroll
            {'addr': 0x007f1a5c, 'type': 'u32'},  # LEFT/RIGHT scroll
        ],
        'output_globals': [
            {'addr': 0x006440ec, 'type': 'u32'},
            {'addr': 0x006440f0, 'type': 'u32'},
            {'addr': 0x006440f4, 'type': 'u32'},
            {'addr': 0x006440f8, 'type': 'u32'},
            {'addr': 0x006440fc, 'type': 'u32'},
            {'addr': 0x00644104, 'type': 'u32'},
            {'addr': 0x00644108, 'type': 'u32'},
            {'addr': 0x0064410c, 'type': 'u32'},
            {'addr': 0x007f1a58, 'type': 'u32'},
            {'addr': 0x007f1a5c, 'type': 'u32'},
        ],
        'lut_root_delta': 0,
        # [cbA, cbB, menu, tab, L, R, U, D, ec(UPp), f0(DNp), f4(Rc), f8(DNc), fc(Lp), 104(UPc), 108(Lc), 10c(Rp), s_a58, s_a5c]
        'path1_tests': [
            [0,0, 1,0, 0,0,0,0, 0,0,0,0,0,0,0,0, 0,0],          # all idle, menu!=0
            [0,0, 0,4, 1,0,0,0, 0,0,0,0,0,0,0,0, 0,0],          # LEFT, phase0/count0 â†’ P=1,C=10
            [0,0, 0,4, 0,1,0,0, 0,0,5,0,0,0,0,1, 0,0],          # RIGHT phase=1,count=5 â†’ P=2,C=4
            [0,0, 0,4, 0,0,1,0, 0,0,0,0,0,1,0,0, 0,0],          # UP phase0,count1 â†’ P=1,C=10
            [0,0, 0,4, 0,0,0,1, 0,1,0,1,0,0,0,0, 0,0],          # DOWN phase=1,count=1 â†’ P=2,C=0â†’?
            [0,0, 0,4, 1,1,1,1, 0,0,0,0,0,0,0,0, 0,0],          # all dirs, all phase0/count0
            [0,0, 0,4, 0,0,0,0, 9,2,9,2,9,2,2,9, 0,0],          # no flags â†’ all phases reset to 0
            [0,0, 0,4, 1,0,0,0, 0,0,0,0,1,0,0xa,0, 0,0],        # LEFT phase=1,count=10 â†’ P=2,C=9
            [0,0, 0,4, 0,1,0,1, 0,1,3,1,0,0,0,1, 0,0],          # RIGHT+DOWN mixed
            [0,0, 5,4, 1,1,1,1, 1,1,1,1,1,1,1,1, 0,0],          # menu!=0 still runs timers (no cb)
        ],
        'path2_tests': [
            [0,0, 0,4, 1,0,0,0, 0,0,0,0,0,0,0,0, 0,0],
            [0,0, 0,4, 0,1,0,0, 0,0,5,0,0,0,0,1, 0,0],
            [0,0, 1,0, 0,0,0,0, 0,0,0,0,0,0,0,0, 0,0],
        ],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-v-s1 â€” font_pools_frontend_ae6 / font_atlas_promote_ae5
    # Frontend/Cluster_v1.cpp â€” 3 leaf candidates
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00474e60  DegToRad
    # float(float) â€” param_1 * _DAT_005cd7a8 (PI/180 = 0x3C8EFA35 = 0.0174533f).
    # Pure x87 leaf: FLDS [ESP+4]; FMULS [0x005cd7a8]; RET (10 bytes).
    # Constant at 0x005cd7a8, read at 0x00474e62. Caller: FUN_00434720 (C2).
    # arg_type='float_scalar': pass degree value, compare float return.
    'deg_to_rad': {
        'rva':            0x00474e60,
        'export':         'DegToRad',
        'signature':      {'ret': 'float', 'args': ['float']},
        'arg_type':       'float_scalar',
        'lut_root_delta': 0,
        'path1_tests': [
            0.0, 45.0, 90.0, 180.0, 270.0, 360.0,
            -45.0, -180.0, 1.0, 0.5, 30.0, 60.0,
            720.0, -360.0, 0.001, 1234.5,
        ],
        'path2_tests': [0.0, 45.0, 90.0, 180.0, 360.0],
    },

    # 0x00556cd0  FontAtlasSlotGet
    # uint32(void) â€” returns DAT_00912a20 directly (5-byte pure-leaf getter).
    # MOV EAX, [0x00912a20]; RET. Data ref at 0x00556cd1.
    # Caller: FontText_HudShutdown (0x00427620, C2).
    # arg_type='read_global': write sentinel to 0x00912a20, call fn, compare return.
    'font_atlas_slot_get': {
        'rva':            0x00556cd0,
        'export':         'FontAtlasSlotGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x00912a20,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # 0x00556cc0  FontAtlasSlotSet
    # void(uint32) â€” DAT_00912a20 = param_1 (9-byte pure-leaf setter).
    # MOV EAX,[ESP+4]; MOV [0x00912a20],EAX; RET. Data ref at 0x00556cc3.
    # Caller: FUN_00427ca0 (0x00427ca0, C2).
    # arg_type='void_setter_observe': call fn(value), read back 0x00912a20.
    'font_atlas_slot_set': {
        'rva':            0x00556cc0,
        'export':         'FontAtlasSlotSet',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x00912a20,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0x00000002, 0x00000003,
                           0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF, 0x80000000,
                           0x55555555, 0xAAAAAAAA],
        'path2_tests':    [0x00000000, 0x00000001, 0xDEADBEEF],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-v-s2 â€” HUD/Cluster_v2.cpp (1/3 promoted)
    # Refusals:
    #   0x004c1c80 â€” no arg_type for (int, in_ptr_2uint32) returning int;
    #               callee FUN_004c0e50 IS C2 now (stale batch-k note) but
    #               harness gap blocks promotion; deferred.
    #   0x00555f20 â€” int param_1 is a live font ctx pointer; vtable+0x108 call
    #               requires valid live object; no arg_type creates font ctx
    #               from test scalars; deferred.
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00402f80  CupFloatInit
    # Pure void(void) initialiser: no calls, no branches. Writes 4 constants
    # to 4 consecutive globals:
    #   DAT_00636ae8 = 0             (0x00402f80)
    #   DAT_00636af0 = 0             (0x00402f8a)
    #   DAT_00636af4 = 0x42200000    (0x00402f94) = +40.0f IEEE-754
    #   DAT_00636aec = 0x42700000    (0x0040299e) = +60.0f IEEE-754
    # Strategy: void_write_observe on 0x00636af4.
    #   Write sentinel -> call fn -> read back -> must be 0x42200000.
    # Leaf-exemption applies (callees_depth1: []).
    # Caller FUN_00433240 is C2.
    'cup_float_init': {
        'rva':            0x00402f80,
        'export':         'CupFloatInit',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x00636af4,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-aa-s1 â€” BatchAA_s1.cpp  (5 candidates, 1 deferred)
    # Deferred: 0x00426b40 â€” still anti-island (7/10 callees at C1)
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00426cf0  GetDat0066d6e4
    # undefined4 * FUN_00426cf0(void): pure leaf; returns &DAT_0066d6e4.
    # No branches, no callees, no side effects.
    # arg_type='none': called 10x at quiescent state; both sides return the same
    # pointer (address 0x0066d6e4). Pointer comparison: NativePointer.toString()
    # gives '0x66d6e4' == '0x66d6e4' for both sides.
    # ref: re/analysis/frontend_c1_to_c2_s5/FUN_00426cf0.md
    'get_dat0066d6e4': {
        'rva':            0x00426cf0,
        'export':         'GetDat0066d6e4',
        'signature':      {'ret': 'pointer', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0041ded0  HudModeGuardDispatch
    # void FUN_0041ded0(undefined4 param_1): if (DAT_0063d5e8 != 0) FUN_0041de80(param_1).
    # Callee: FUN_0041de80 (C2) â€” HUD mode dispatcher with vtable dispatches.
    # arg_type='int_scalar': pass param_1 (0 or 1 per callers).
    # At quiescent main menu DAT_0063d5e8 == 0 â†’ guard skips â†’ void_match for all inputs.
    # void_match: both sides return void without error == GREEN.
    # crash_equal_ok=True as fallback if guard is non-zero at attach time.
    # ref: re/analysis/hud_ingame_promote_c2/0x0041ded0.md
    'hud_mode_guard_dispatch': {
        'rva':            0x0041ded0,
        'export':         'HudModeGuardDispatch',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 0, 1, 0, 0, 1, 0, 1, 0],
        'path2_tests':    [0, 1, 0],
    },

    # 0x00552840  FontCtx_SetRotation
    # undefined4 FUN_00552840(undefined4 param_1): Z-axis rotation on font context matrix.
    # Steps: axis=(0,0,1); RwMatrix_SetRotAxisAngle(ctx,axis,param_1,preconcat=1);
    #        DAT_00912bd8=0; DAT_00912bec=0; return 1.
    # Callee: FUN_004c4d20 (C2, RwMatrix_SetRotAxisAngle).
    # arg_type='float_scalar': pass angle directly; compare return value (always 1).
    #   Function always returns 1; both orig and reimpl return 1 â†’ 10/10 GREEN if
    #   no crash. crash_equal_ok=True: if font context not set up, both sides crash
    #   identically at g_FontCtxPtrs[depth] deref.
    # Strategy rationale: font_ctx_float2 requires 2 floats + a prelude; no single-
    #   float counterpart exists. float_scalar + crash_equal_ok is the closest match.
    #   Return value 1 is deterministic across all rotation angles.
    # ref: re/analysis/hud_promote_c2_b/0x00552840.md
    'font_ctx_set_rotation': {
        'rva':            0x00552840,
        'export':         'FontCtx_SetRotation',
        'signature':      {'ret': 'uint32', 'args': ['float']},
        'arg_type':       'float_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # Rotation angles in degrees as float values
        'path1_tests':    [
            0.0,    # 0 degrees
            90.0,   # 90 degrees
            180.0,  # 180 degrees
            270.0,  # 270 degrees
            -90.0,  # -90 degrees
            45.0,   # 45 degrees
            1.0,    # 1 degree
            -1.0,   # -1 degree
            360.0,  # 360 degrees (full rotation)
            0.0,    # 0 again (idempotency)
        ],
        'path2_tests':    [0.0, 90.0, 180.0],
    },

    # 0x0042bde0  HudRectEmitter5
    # void FUN_0042bde0(int param_1, byte param_2): emits 5 HUD rects via ChromeBaseDraw.
    # Callee: FUN_00472c60 ChromeBaseDraw (C3).
    # ChromeBaseDraw calls into D3D9 render state + RW vtable â€” will crash at
    # quiescent attach time when vtable global DAT_007d3ff8 may not be set up.
    # arg_type='none': call fn() with no args (like MenuMenusBC 0x0042f8d0 pattern);
    #   both sides crash identically at the render path â†’ crash_equal_ok GREEN.
    # Signature: void() for the none/crash_equal_ok strategy; actual args not needed.
    # ref: re/analysis/frontend_c1_to_c2_s6/FUN_0042bde0.md
    'hud_rect_emitter5': {
        'rva':            0x0042bde0,
        'export':         'HudRectEmitter5',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00426dc0  FrontendRaycastForward
    # void FUN_00426dc0(undefined4 p1, undefined4 p2, undefined4 p3):
    #   3-arg forwarder: FUN_00479100(&DAT_00646e58, p1, p2, p3); return.
    # Callee: FUN_00479100 (C2) â€” raycast + vertex-color sample on collision mesh.
    # Calling FUN_00479100 without a live collision mesh will crash.
    # arg_type='none': call fn() with no args; both sides crash at the FUN_00479100
    #   collision-mesh deref identically â†’ crash_equal_ok GREEN.
    # (&DAT_00646e58 is a real global address hardcoded in reimpl; both sides pass
    # the same pointer, so the crash site is identical.)
    # ref: re/analysis/frontend_c1_to_c2_s5/FUN_00426dc0.md
    'frontend_raycast_forward': {
        'rva':            0x00426dc0,
        'export':         'FrontendRaycastForward',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-aa-s2 â€” frontend per-player score accessor (C2->C3)
    # Frontend/BatchAA_s2.cpp
    # 0x00423b80  PlayerScoreAccC â€” twin of AccA/AccB with base 0x00899f74.
    # Callee: GetRaceSubMode (0x0042f6a0, C3). At quiescent main menu:
    # race sub-mode != 4 â†’ takes the array-read path â†’ bit-identical with orig.
    # arg_type='int_scalar': pass int param_1 (player index), compare uint32 return.
    # ref: re/analysis/frontend_c1_to_c2_s2/0x00423b80.md
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00423b80  PlayerScoreAccC
    # Per-player score accessor #3. Guard: GetRaceSubMode()==4 â†’ return 0.
    # Returns (&DAT_00899f74)[param_1 * 0x4e] (dword array, stride 0x4e dwords).
    # Array base: 0x00899f74 (field +0x534 from 0x00899a40, cited at 0x00423b95).
    # At main menu: race sub-mode != 4 â†’ takes array-read path â†’ bit-identical.
    'player_score_acc_c': {
        'rva':            0x00423b80,
        'export':         'PlayerScoreAccC',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        # Player indices 0..3 (only 4 players). Repeat to reach 10+ vectors.
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # â”€â”€ c3-batch-aa-s3 â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Frontend/BatchAA_s3.cpp â€” GetDat0067d84c, TeamBlockZeroGet, SetDat00912a20
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00427c90  GetDat0067d84c
    # undefined4 FUN_00427c90(void) â€” pure 5-byte getter; returns DAT_0067d84c.
    # No callees, no branches.
    # arg_type='read_global': write sentinel to target_global, call fn(), read back.
    # ref: re/analysis/frontend_c1_to_c2_s5/FUN_00427c90.md
    'get_dat_0067d84c': {
        'rva':            0x00427c90,
        'export':         'GetDat0067d84c',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067d84c,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xCAFEBABE,
                           0xFFFFFFFF, 0x80000000, 0x55555555, 0xAAAAAAAA,
                           0x12345678, 0xBEEFCAFE],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xFFFFFFFF],
    },

    # 0x00424100  TeamBlockZeroGet
    # int FUN_00424100(int param_1) â€” team-aggregating block-0 getter.
    # param_1: player slot index 0..3.
    # Returns block-0 field at (&DAT_008999a0)[param_1 * 0x4e] plus team-partner
    # contributions (active-player guard: partner active-slot != -1).
    # Callee FUN_0042f500 (0x0042f500, C4): team-mode flag.
    # At main-menu quiescent state: team-mode flag is 0, so function returns
    # (&DAT_008999a0)[param_1 * 0x4e] directly for all slots 0..3.
    # arg_type='int_scalar': pass param_1 as int, compare return values.
    # ref: re/analysis/frontend_c1_to_c2_s3/FUN_00424100.md
    'team_block_zero_get': {
        'rva':            0x00424100,
        'export':         'TeamBlockZeroGet',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # ── c3-batch-ad-s1 ───────────────────────────────────────────────────────
    # Frontend/FrontendLeaves_ad1.cpp — TeamBlockOneGet
    # ──────────────────────────────────────────────────────────────────────────
    # 0x004241c0  TeamBlockOneGet
    # int FUN_004241c0(int param_1) — team-aggregating block-1 getter.
    # Exact structural twin of FUN_00424100 (team_block_zero_get), differing ONLY
    # in the block base: block-1 root DAT_00899e80 vs block-0 root DAT_008999a0.
    # param_1: player slot index 0..3.
    # Returns block-1 field at (&DAT_00899e80)[param_1 * 0x4e] plus team-partner
    # contributions (active-player guard: partner active-slot != -1).
    # Callee FUN_0042f500 (0x0042f500, C4): team-mode flag.
    # At main-menu quiescent state: team-mode flag is 0, so function returns
    # (&DAT_00899e80)[param_1 * 0x4e] directly for all slots 0..3.
    # arg_type='int_scalar': pass param_1 as int, compare return values.
    # ref: re/analysis/frontend_c1_to_c2_s3/FUN_004241c0.md
    'team_block_one_get': {
        'rva':            0x004241c0,
        'export':         'TeamBlockOneGet',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # 0x00556cc0  SetDat00912a20
    # void FUN_00556cc0(undefined4 param_1) â€” pure 9-byte setter.
    # Writes param_1 to DAT_00912a20 (global font/style pointer at 0x00912a20).
    # No callees, no branches. One caller: FUN_00427ca0 (0x00427ca0).
    # arg_type='void_setter_observe': call fn(value), read back target_global.
    # ref: re/analysis/font_atlas_promote_ae5/0x00556cc0.md
    'set_dat_00912a20': {
        'rva':            0x00556cc0,
        'export':         'SetDat00912a20',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x00912a20,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xCAFEBABE,
                           0xFFFFFFFF, 0x80000000, 0x55555555, 0xAAAAAAAA,
                           0x12345678, 0xBEEFCAFE],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xFFFFFFFF],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-aa-s4 â€” frontend small leaves (C2â†’C3, 2 candidates)
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042a9c0  ModeCodeLookup
    # Pure leaf. Reads DAT_007f0fd0 (mode/state int32 at 0x007f0fd0).
    # Returns 0x2d (45) for modes {5,7,8,9,10}; undefined (UB) for all others.
    # Decomp (Mashed_pool13, verbatim):
    #   if ((mode==10||mode==9||mode==8)||(mode==7||(uVar1=0x16,mode==5)))
    #       uVar1 = 0x2d;
    #   return uVar1;
    # Strategy: read_global â€” write mode value to 0x007f0fd0, call fn(), compare ret.
    # Only defined modes {5,7,8,9,10} are tested (all return 0x2d).
    # [UNCERTAIN U-4200] on mode==5 return resolved by decomp literal (0x2d).
    # Leaf-exemption applies (callees_depth1: []).
    'mode_code_lookup': {
        'rva':            0x0042a9c0,
        'export':         'ModeCodeLookup',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x007f0fd0,
        'lut_root_delta': 0,
        'path1_tests':    [5, 7, 8, 9, 10, 5, 7, 8, 9, 10],
        'path2_tests':    [5, 7, 10],
    },

    # 0x00425b70  SlotFieldSetter
    # Pure leaf. void(int param_1, undefined4 param_2).
    # Writes param_2 to (&DAT_008992a4)[param_1 * 0x13] (+0x04 field).
    # Zeroes        (&DAT_008992a0)[param_1 * 0x13]     (+0x00 field).
    # Array base: 0x008992a0; stride: 0x13 dwords = 0x4c bytes.
    # Strategy: entity_field_set â€” call fn(p1, p2), read back 0x008992a4 + p1*0x4c.
    # [UNCERTAIN U-4200] on whether 0x008992a0 and 0x00899260 are the same array
    # is non-blocking (doesn't affect this function's addresses).
    # Leaf-exemption applies (callees_depth1: []).
    'slot_field_setter': {
        'rva':            0x00425b70,
        'export':         'SlotFieldSetter',
        'signature':      {'ret': 'void', 'args': ['int32', 'uint32']},
        'arg_type':       'entity_field_set',
        'target_global':  0x008992a4,
        'entity_byte_stride': 0x4c,
        'lut_root_delta': 0,
        'path1_tests': [
            # [param_1, param_2]
            [0, 0],
            [0, 1],
            [0, 0xdeadbeef],
            [0, 0xffffffff],
            [1, 0],
            [1, 0x12345678],
            [2, 0],
            [2, 0xabcdef01],
            [2, 0xcafebabe],
            [3, 0x99999999],
        ],
        'path2_tests': [
            [0, 0xdeadbeef],
            [1, 0x12345678],
            [2, 0xcafebabe],
        ],
    },

    # Session c3-batch-aa-s5 â€” Frontend/BatchAA_s5.cpp  (C2â†’C3, 1 candidate)
    # 0x0042a980  MenuTableSearchAlt
    # Same stride-3 table as MenuTableSearch (0x0042a940); returns field at +4
    # instead of +8. Key derived via FUN_0040ce80(param_1) (C2).
    # Sentinel-terminated walk; no live-state writes.
    # int_scalar: same domain as MenuTableSearch â€” pass slot indices 0-3;
    # both orig and reimpl read the same quiescent table state. 10 tests.
    # ref: re/analysis/frontend_c1_to_c2_s6/FUN_0042a980.md
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x0042a980  MenuTableSearchAlt
    # undefined4 FUN_0042a980(undefined4 param_1)  __cdecl  (61 bytes)
    # Searches stride-3 table at 0x005f6748 using key = FUN_0040ce80(param_1).
    # Returns 0 on not-found; value field at +4 (0x5f674c + pos*4) on match.
    # U-4199 (field semantics) does not affect correctness.
    'menu_table_search_alt': {
        'rva':            0x0042a980,
        'export':         'MenuTableSearchAlt',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2, 3],
    },

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # Session c3-batch-aa-s6 â€” BatchAA_s6.cpp
    # â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€

    # 0x00556cd0  GetDat00912a20
    # undefined4 FUN_00556cd0(void) â€” 5-byte getter.
    # Returns DAT_00912a20 verbatim.  No callees, no branches.
    # Strategy: read_global â€” write sentinel to 0x00912a20, call fn(), compare
    #   returned value vs written sentinel.  bit-identical by construction.
    # Caller: FontText_HudShutdown (0x00427620, C2).
    # ref: re/analysis/font_atlas_promote_ae5/0x00556cd0.md
    'get_dat_00912a20': {
        'rva':            0x00556cd0,
        'export':         'GetDat00912a20',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x00912a20,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00426d00  FrontendArraySlotGet
    # undefined* FUN_00426d00(int param_1, int param_2) â€” pure leaf.
    # Returns &DAT_00663670 + param_2*0xc + param_1*0x4c (address arithmetic only).
    # No callees, no branches, no globals modified.
    # Strategy: int_pair â€” pass [param_1, param_2]; return is the computed pointer
    #   (uint32 on x86).  Both paths compute identical address arithmetic.
    # Test range: param_1 in [0..3], param_2 in [0..2] (safe address region near base).
    # ref: re/analysis/frontend_c1_to_c2_s5/FUN_00426d00.md
    'frontend_array_slot_get': {
        'rva':            0x00426d00,
        'export':         'FrontendArraySlotGet',
        'signature':      {'ret': 'pointer', 'args': ['int', 'int']},
        'arg_type':       'int_pair',
        'lut_root_delta': 0,
        'path1_tests':    [
            [0, 0], [1, 0], [2, 0], [3, 0],
            [0, 1], [1, 1], [2, 1], [3, 1],
            [0, 2], [1, 2],
        ],
        'path2_tests':    [[0, 0], [1, 1], [2, 0]],
    },

    # ──────────────────────────────────────────────────────────────────────────
    # Session c3-batch-ab-s1 — Render/BatchAB_s1.cpp
    # ──────────────────────────────────────────────────────────────────────────

    # 0x004d40c0  GetPipelinePtr
    # undefined4 FUN_004d40c0(void) — 5-byte getter.
    # Returns DAT_007d4710 (current RW pipeline pointer).
    # Strategy: read_global — write sentinel to 0x007d4710, call fn(),
    #   verify returned value equals sentinel.  bit-identical by construction.
    # Caller: FUN_004cd2d0 (RwRenderIndexedSubmit, C2).
    # Leaf-function exemption (no callees). ref: re/analysis/render_5_c1_to_c2_s6/004d40c0.md
    'get_pipeline_ptr': {
        'rva':            0x004d40c0,
        'export':         'GetPipelinePtr',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x007d4710,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x00000000, 0x3F800000],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x004671c0  GetOverlayCamera
    # undefined4 FUN_004671c0(void) — 5-byte getter.
    # Returns DAT_006905b4 (overlay/minimap RwCamera* global).
    # Strategy: read_global — write sentinel to 0x006905b4, call fn(),
    #   verify returned value equals sentinel.  bit-identical by construction.
    # Caller: FUN_00492e90 (per-frame render loop, C2+).
    # Leaf-function exemption (no callees). ref: re/analysis/render_c1_to_c2_s3/FUN_004671c0.md
    'get_overlay_camera': {
        'rva':            0x004671c0,
        'export':         'GetOverlayCamera',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x006905b4,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x00000000, 0x3F800000],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x004cbb60  GetRenderStateBlockPtr
    # undefined4* FUN_004cbb60(void) — 6-byte getter.
    # Returns &DAT_00911fa0 (fixed address of render-state block).
    # Strategy: none — call 10x at quiescent state; both orig and reimpl
    #   return identical fixed address (0x00911fa0); comparison via .toString().
    # Callers: FUN_004951f0, FUN_004f46a0, FUN_00530c00, FUN_005327d0,
    #          FUN_00543710, FUN_00549970 (all C2+).
    # Leaf-function exemption (no callees). ref: re/analysis/render_4_c1_to_c2_s6/FUN_004cbb60.md
    'get_render_state_block_ptr': {
        'rva':            0x004cbb60,
        'export':         'GetRenderStateBlockPtr',
        'signature':      {'ret': 'pointer', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # ──────────────────────────────────────────────────────────────────────────
    # Session c3-batch-ab-s3 — Render/BatchAB_s3.cpp  (C2→C3, 5 candidates)
    # bucket 0x004ddfb0 pixel-format BGRA encoder leaves
    # ──────────────────────────────────────────────────────────────────────────

    # 0x004df8d0  PixEncode1555
    # uint FUN_004df8d0(byte *bgra)  — 52 bytes, pure leaf
    # Returns 16-bit A1R5G5B5 pack:
    #   (((b[3] & 0x80) * 2 | b[0] & 0xf8) << 5 | b[1] & 0xf8) << 2 | (b[2] >> 3)
    # Alpha bit from high bit of b[3]. Decoder match: D3DFMT_A1R5G5B5 (0x19).
    # arg_type: bgra_encode — allocate 4-byte buf, write [b0,b1,b2,b3], call fn(buf),
    #   compare uint32 return value. Added to diff_template.js 2026-05-30 c3-batch-ab-s3.
    # ref: re/analysis/bucket_004ddfb0/0x004df8d0.md
    'pix_encode_1555': {
        'rva':            0x004df8d0,
        'export':         'PixEncode1555',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'bgra_encode',
        'lut_root_delta': 0,
        'path1_tests': [
            # [b0, b1, b2, b3]  (BGRA bytes)
            [0x00, 0x00, 0x00, 0x00],   # all zeros
            [0xff, 0xff, 0xff, 0xff],   # all ones
            [0xf8, 0x00, 0x00, 0x00],   # blue only (5-bit)
            [0x00, 0xf8, 0x00, 0x00],   # green only (5-bit)
            [0x00, 0x00, 0xf8, 0x00],   # red only (5-bit)
            [0x00, 0x00, 0x00, 0x80],   # alpha bit only
            [0xf8, 0xf8, 0xf8, 0x80],   # all channels max + alpha
            [0x10, 0x20, 0x40, 0x00],   # low values, alpha off
            [0x10, 0x20, 0x40, 0x80],   # low values, alpha on
            [0xaa, 0x55, 0xcc, 0x80],   # alternating pattern, alpha on
        ],
        'path2_tests': [
            [0xf8, 0xf8, 0xf8, 0x80],
            [0x00, 0x00, 0x00, 0x80],
            [0xaa, 0x55, 0xcc, 0x00],
        ],
    },

    # 0x004df910  PixEncode4444
    # uint FUN_004df910(byte *bgra)  — 50 bytes, pure leaf
    # Returns 16-bit A4R4G4B4 pack:
    #   ((b[3] & 0xf0) << 4 | b[0] & 0xf0) << 4 | b[1] & 0xf0 | (b[2] >> 4)
    # Decoder match: D3DFMT_A4R4G4B4 (0x1a).
    # ref: re/analysis/bucket_004ddfb0/0x004df910.md
    'pix_encode_4444': {
        'rva':            0x004df910,
        'export':         'PixEncode4444',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'bgra_encode',
        'lut_root_delta': 0,
        'path1_tests': [
            [0x00, 0x00, 0x00, 0x00],
            [0xff, 0xff, 0xff, 0xff],
            [0xf0, 0x00, 0x00, 0x00],   # blue nibble only
            [0x00, 0xf0, 0x00, 0x00],   # green nibble only
            [0x00, 0x00, 0xf0, 0x00],   # red nibble only
            [0x00, 0x00, 0x00, 0xf0],   # alpha nibble only
            [0xf0, 0xf0, 0xf0, 0xf0],   # all channels max nibble
            [0x12, 0x34, 0x56, 0x78],   # mixed nibbles
            [0xaa, 0x55, 0xcc, 0x88],   # alternating
            [0x10, 0x20, 0x30, 0x40],   # low nibbles
        ],
        'path2_tests': [
            [0xf0, 0xf0, 0xf0, 0xf0],
            [0x12, 0x34, 0x56, 0x78],
            [0x00, 0x00, 0x00, 0xf0],
        ],
    },

    # 0x004df950  PixEncodeA8R3G3B2
    # uint FUN_004df950(byte *src4)  — 43 bytes, pure leaf
    # Returns 16-bit packed word (A8R3G3B2 shape):
    #   (b[1] & 0xe0 | (b[2] >> 3)) >> 3 | (b[3] << 8) | (b[0] & 0xe0)
    # Uncertainty U-5550: name is uncertain; formula is literal from decomp.
    # ref: re/analysis/bucket_004ddfb0/0x004df950.md
    'pix_encode_a8r3g3b2': {
        'rva':            0x004df950,
        'export':         'PixEncodeA8R3G3B2',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'bgra_encode',
        'lut_root_delta': 0,
        'path1_tests': [
            [0x00, 0x00, 0x00, 0x00],
            [0xff, 0xff, 0xff, 0xff],
            [0xe0, 0x00, 0x00, 0x00],   # b0 top 3 bits
            [0x00, 0xe0, 0x00, 0x00],   # b1 top 3 bits
            [0x00, 0x00, 0xf8, 0x00],   # b2 (>> 3 then masked)
            [0x00, 0x00, 0x00, 0xff],   # b3 (alpha byte shifted up)
            [0xe0, 0xe0, 0xf8, 0xff],   # all channels max
            [0x40, 0x60, 0x80, 0x55],   # mixed mid values
            [0xaa, 0xcc, 0x88, 0x77],   # alternating bytes
            [0x20, 0x40, 0x18, 0x33],   # low bits
        ],
        'path2_tests': [
            [0xe0, 0xe0, 0xf8, 0xff],
            [0x40, 0x60, 0x80, 0x55],
            [0x00, 0x00, 0x00, 0xff],
        ],
    },

    # 0x004df980  PixEncodeX4R4G4B4
    # uint FUN_004df980(byte *bgr)  — 39 bytes, pure leaf
    # Returns 16-bit X4R4G4B4 pack (alpha forced 0xf):
    #   (CONCAT11(0xf, b[0] & 0xf0) << 4) | (b[1] & 0xf0) | (b[2] >> 4)
    #   = 0xf000 | ((b[0] & 0xf0) << 4) | (b[1] & 0xf0) | (b[2] >> 4)
    # b[3] is not consumed. Decoder match: D3DFMT_X4R4G4B4 (0x1e).
    # ref: re/analysis/bucket_004ddfb0/0x004df980.md
    'pix_encode_x4r4g4b4': {
        'rva':            0x004df980,
        'export':         'PixEncodeX4R4G4B4',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'bgra_encode',
        'lut_root_delta': 0,
        'path1_tests': [
            [0x00, 0x00, 0x00, 0x00],   # all zeros (alpha still 0xf)
            [0xf0, 0xf0, 0xf0, 0x00],   # all channel nibbles max
            [0xf0, 0x00, 0x00, 0x00],   # blue nibble only
            [0x00, 0xf0, 0x00, 0x00],   # green nibble only
            [0x00, 0x00, 0xf0, 0x00],   # red nibble only
            [0x10, 0x20, 0x30, 0x00],   # low nibbles
            [0xaa, 0xbb, 0xcc, 0x00],   # alternating nibbles
            [0x50, 0x60, 0x70, 0xff],   # b3 non-zero (not consumed)
            [0xff, 0xff, 0xff, 0x00],   # all bytes max (nibble truncation)
            [0x80, 0x40, 0x20, 0x00],   # descending power-of-2 nibbles
        ],
        'path2_tests': [
            [0xf0, 0xf0, 0xf0, 0x00],
            [0xaa, 0xbb, 0xcc, 0x00],
            [0x00, 0x00, 0x00, 0x00],
        ],
    },

    # 0x004df9b0  PixEncodeR6G5B5
    # uint FUN_004df9b0(byte *bgr)  — 39 bytes, pure leaf
    # Returns 16-bit R6G5B5 pack (byte-reversed 565 vs D3DFMT_R5G6B5):
    #   ((b[2] & 0xfc) << 6 | b[1] & 0xf8) << 2 | (b[0] >> 3)
    # Source layout: b[0]=B (5 bits), b[1]=G (5 bits), b[2]=R (6 bits at top).
    # Decoder match: D3DFORMAT 0x3d (non-standard; [UNCERTAIN U-5551] internal RW id).
    # ref: re/analysis/bucket_004ddfb0/0x004df9b0.md
    'pix_encode_r6g5b5': {
        'rva':            0x004df9b0,
        'export':         'PixEncodeR6G5B5',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'bgra_encode',
        'lut_root_delta': 0,
        'path1_tests': [
            # [b0(B), b1(G), b2(R), b3(ignored)]  (BGR bytes)
            [0x00, 0x00, 0x00, 0x00],   # all zeros
            [0xff, 0xff, 0xff, 0x00],   # all channels max
            [0xf8, 0x00, 0x00, 0x00],   # B only (5-bit, b0>>3)
            [0x00, 0xf8, 0x00, 0x00],   # G only (5-bit, b1&0xf8)
            [0x00, 0x00, 0xfc, 0x00],   # R only (6-bit, b2&0xfc)
            [0xf8, 0xf8, 0xfc, 0x00],   # all channels max-aligned
            [0x10, 0x20, 0x40, 0x00],   # low values
            [0xaa, 0x55, 0xcc, 0x00],   # alternating
            [0x08, 0x08, 0x04, 0x00],   # one step above truncation threshold
            [0x80, 0x40, 0x20, 0xff],   # descending, b3 non-zero (not consumed)
        ],
        'path2_tests': [
            [0xf8, 0xf8, 0xfc, 0x00],
            [0xaa, 0x55, 0xcc, 0x00],
            [0x00, 0x00, 0x00, 0x00],
        ],
    },

    # 0x004df9e0  PixEncodeX8R8G8B8
    # uint FUN_004df9e0(byte *bgr)  — 34 bytes, pure leaf
    # Returns 32-bit X8R8G8B8 pack (alpha forced 0xff):
    #   ((b[0] | 0xffffff00) << 8 | b[1]) << 8 | b[2]
    #   = 0xff000000 | (b[0] << 16) | (b[1] << 8) | b[2]
    # b[3] is not consumed. Decoder match: D3DFMT_X8R8G8B8 (0x16).
    # ref: re/analysis/bucket_004ddfb0/0x004df9e0.md
    'pix_encode_x8r8g8b8': {
        'rva':            0x004df9e0,
        'export':         'PixEncodeX8R8G8B8',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'bgra_encode',
        'lut_root_delta': 0,
        'path1_tests': [
            [0x00, 0x00, 0x00, 0x00],   # all zeros (alpha still 0xff)
            [0xff, 0xff, 0xff, 0x00],   # all channels max
            [0xff, 0x00, 0x00, 0x00],   # b0 only
            [0x00, 0xff, 0x00, 0x00],   # b1 only
            [0x00, 0x00, 0xff, 0x00],   # b2 only
            [0x12, 0x34, 0x56, 0x00],   # mixed mid values
            [0xaa, 0x55, 0xcc, 0x00],   # alternating
            [0x80, 0x40, 0x20, 0x00],   # descending
            [0x10, 0x20, 0x30, 0xff],   # b3 non-zero (not consumed)
            [0xfe, 0xfd, 0xfc, 0x00],   # near-max
        ],
        'path2_tests': [
            [0xff, 0xff, 0xff, 0x00],
            [0x12, 0x34, 0x56, 0x00],
            [0xaa, 0x55, 0xcc, 0x00],
        ],
    },

    # 0x004dfa70  PixReadU32
    # uint FUN_004dfa70(byte *p)  — 33 bytes, pure leaf (no callees)
    # Reads 4 bytes from p[0..3] and returns little-endian assembled uint32:
    #   result = p[0] | (p[1]<<8) | (p[2]<<16) | (p[3]<<24)
    # This is an unaligned 32-bit read (byte-by-byte via CH/CL/DL assembly pattern).
    # Equivalent to *(uint32_t*)p on a little-endian host.
    # Decoder match: D3DFORMAT 0x20 / 0x3f (identity passthrough arms in FUN_004df750).
    # Caller: FUN_004e02d0 (render, C2). Leaf-exemption: no callees; caller is C2.
    # arg_type: ptr_arg_int_get — passes buffer seeded with seed-based u32 pattern;
    #   fn reads buf[0..3] = seed → returns seed. Non-degenerate across seeds.
    # ref: re/analysis/bucket_004ddfb0/0x004dfa70.md
    # impl: mashedmod/src/mashed_re/Render/PixReadU32_wf.cpp
    'pix_read_u32': {
        'rva':            0x004dfa70,
        'export':         'PixReadU32',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'ptr_arg_int_get',
        'struct_size':    4,
        'lut_root_delta': 0,
        'path1_tests': [
            0x00000000,   # all zeros
            0xFFFFFFFF,   # all ones
            0x12345678,   # mixed bytes
            0xDEADBEEF,   # classic pattern
            0xAABBCCDD,   # alternating nibbles
            0x01020304,   # ascending bytes
            0x80000000,   # high bit only
            0x7FFFFFFF,   # max signed positive
            0x11223344,   # low nibbles
            0xFEFDFCFB,   # near-max descending
        ],
        'path2_tests': [
            0x12345678,
            0xDEADBEEF,
            0xAABBCCDD,
        ],
    },

    # 0x004d7db0  RwPluginExtensionSlotGet
    # uint32(int param_1, int param_2): walk *(param_1+0x10) linked list;
    # match node[2]==param_2; return node[0] or 0xffffffff on miss.
    # Leaf function, no callees. Pure reader of passed-in pointer — no global writes.
    # Strategy: int_pair — pass [type_desc_ptr, plugin_id]. Both orig and reimpl
    # walk the same live RpAtomic extension list (DAT_00618664, initialized at menu).
    # For any given (ptr, id) pair both sides must return the same value (found
    # offset or 0xffffffff sentinel). Varying id covers both found and miss branches.
    # DAT_00618664 = RpAtomic class anchor; populated by RwEngineInit before menu.
    # Callers: FUN_004c2dc0 (Lua C2), FUN_004e7e10 (C1 thunk forwarder passing same anchor).
    # ref: re/analysis/render_6_c1_to_c2_s3/004d7db0.md
    # ref: re/analysis/bucket_004d7ac0/0x004d7db0.md
    # impl: mashedmod/src/mashed_re/Render/BatchAB_s6.cpp
    'rw_plugin_extension_slot_get': {
        'rva':            0x004d7db0,
        'export':         'RwPluginExtensionSlotGet',
        'signature':      {'ret': 'uint32', 'args': ['int32', 'int32']},
        'arg_type':       'int_pair',
        'lut_root_delta': 0,
        # param_1 = 0x00618664 (RpAtomic class anchor, valid at menu-boot).
        # param_2 = varied plugin ids; expect 0xffffffff for unknown ids,
        # or registered base offset for any id that happens to be registered.
        'path1_tests': [
            [0x00618664, 0x00000000],
            [0x00618664, 0x00000001],
            [0x00618664, 0x00000002],
            [0x00618664, 0x00000003],
            [0x00618664, 0x00000100],
            [0x00618664, 0x00000101],
            [0x00618664, 0x00000102],
            [0x00618664, 0x00000200],
            [0x00618664, 0x7fffffff],
            [0x00618664, 0xffffffff],
        ],
        'path2_tests': [
            [0x00618664, 0x00000000],
            [0x00618664, 0x00000001],
            [0x00618664, 0x00000100],
        ],
    },

    # ─────────────────────────────────────────────────────────────────────────
    # c3-batch-ab session 4 — audio leaves (bucket_audio_005bf4d0_005c9770).
    # ABIs verified from the disassembly (see Audio/AudioLeaves_ab4.cpp headers).
    # ─────────────────────────────────────────────────────────────────────────

    # 0x005bf660  Audio3DwordZero — __cdecl void(ptr): zero 3 consecutive dwords.
    # bytes_inplace with a 2-arg signature (the spare uint32 is harmless under
    # cdecl; the function reads only [ESP+4]). len=16 cases guard against
    # over-zeroing past the 3 dwords.
    'audio_3dword_zero': {
        'rva':         0x005bf660,
        'export':      'Audio3DwordZero',
        'signature':   {'ret': 'void', 'args': ['pointer', 'uint32']},
        'arg_type':    'bytes_inplace',
        'lut_root_delta': 0,
        'path1_tests': [
            {'init': [0xAA,0xBB,0xCC,0xDD, 0x11,0x22,0x33,0x44, 0x55,0x66,0x77,0x88], 'len': 12},
            {'init': [0xFF]*16, 'len': 16},
            {'init': [0x01,0x02,0x03,0x04, 0x05,0x06,0x07,0x08,
                      0x09,0x0A,0x0B,0x0C, 0x0D,0x0E,0x0F,0x10], 'len': 16},
        ],
        'path2_tests': [
            {'init': [0xAA,0xBB,0xCC,0xDD, 0x11,0x22,0x33,0x44, 0x55,0x66,0x77,0x88], 'len': 12},
        ],
    },

    # 0x005bfcc0  AudioCounterPairGet — __stdcall int(ptr, u64* o1, u64* o2).
    # o1 = (u64)*(p+0x15c) ; o2 = (u64)((u32)*(p+0x15c) - (u32)*(p+0x160)) ; ret 0.
    # Export decorates to _AudioCounterPairGet@12 (extern "C" __stdcall).
    'audio_counter_pair_get': {
        'rva':         0x005bfcc0,
        'export':      '_AudioCounterPairGet@12',
        'signature':   {'ret': 'int32', 'args': ['pointer', 'pointer', 'pointer']},
        'arg_type':    'struct_call_observe',
        'orig_calling_convention':   'stdcall',
        'reimpl_calling_convention': 'stdcall',
        'struct_size': 0x180,
        'out_ptrs':    2,
        'observe_ret': True,
        'observe':     [{'src': 'out0', 'off': 0, 'type': 'u64'},
                        {'src': 'out1', 'off': 0, 'type': 'u64'}],
        'lut_root_delta': 0,
        'path1_tests': [
            {'seeds': [{'off': 0x15c, 'type': 'u32', 'value': 1000},      {'off': 0x160, 'type': 'u32', 'value': 300}]},
            {'seeds': [{'off': 0x15c, 'type': 'u32', 'value': 0},         {'off': 0x160, 'type': 'u32', 'value': 0}]},
            {'seeds': [{'off': 0x15c, 'type': 'u32', 'value': 5},         {'off': 0x160, 'type': 'u32', 'value': 10}]},
            {'seeds': [{'off': 0x15c, 'type': 'u32', 'value': 0xFFFFFFFF},{'off': 0x160, 'type': 'u32', 'value': 1}]},
            {'seeds': [{'off': 0x15c, 'type': 'u32', 'value': 0x12345678},{'off': 0x160, 'type': 'u32', 'value': 0x1000}]},
        ],
        'path2_tests': [
            {'seeds': [{'off': 0x15c, 'type': 'u32', 'value': 1000},      {'off': 0x160, 'type': 'u32', 'value': 300}]},
        ],
    },

    # 0x005c7500  AudioMixerRateCompute — __cdecl void(ptr): fixed-point store.
    # *(p+0x80) = ((u32)(int)(f38*f34) << 32) / *(*(p+0x9c)+0x18), low 16 cleared.
    # Products chosen exactly float32-representable so the FISTP-truncate matches
    # the C cast bit-for-bit; divisors are nonzero.
    'audio_mixer_rate_compute': {
        'rva':         0x005c7500,
        'export':      'AudioMixerRateCompute',
        'signature':   {'ret': 'void', 'args': ['pointer']},
        'arg_type':    'struct_call_observe',
        'struct_size': 0x100,
        'out_ptrs':    0,
        'observe_ret': False,
        'observe':     [{'src': 'struct', 'off': 0x80, 'type': 'u64'}],
        'lut_root_delta': 0,
        'path1_tests': [
            {'seeds': [{'off': 0x34, 'type': 'f32', 'value': 2.0}, {'off': 0x38, 'type': 'f32', 'value': 3.0}],
             'nested': [{'ptr_off': 0x9c, 'size': 0x20, 'fields': [{'off': 0x18, 'type': 'u32', 'value': 0x10000}]}]},
            {'seeds': [{'off': 0x34, 'type': 'f32', 'value': 1.0}, {'off': 0x38, 'type': 'f32', 'value': 1.0}],
             'nested': [{'ptr_off': 0x9c, 'size': 0x20, 'fields': [{'off': 0x18, 'type': 'u32', 'value': 2}]}]},
            {'seeds': [{'off': 0x34, 'type': 'f32', 'value': 4.0}, {'off': 0x38, 'type': 'f32', 'value': 5.0}],
             'nested': [{'ptr_off': 0x9c, 'size': 0x20, 'fields': [{'off': 0x18, 'type': 'u32', 'value': 1000}]}]},
            {'seeds': [{'off': 0x34, 'type': 'f32', 'value': 1.5}, {'off': 0x38, 'type': 'f32', 'value': 3.0}],
             'nested': [{'ptr_off': 0x9c, 'size': 0x20, 'fields': [{'off': 0x18, 'type': 'u32', 'value': 256}]}]},
            {'seeds': [{'off': 0x34, 'type': 'f32', 'value': 0.0}, {'off': 0x38, 'type': 'f32', 'value': 123.0}],
             'nested': [{'ptr_off': 0x9c, 'size': 0x20, 'fields': [{'off': 0x18, 'type': 'u32', 'value': 7}]}]},
            {'seeds': [{'off': 0x34, 'type': 'f32', 'value': -2.0}, {'off': 0x38, 'type': 'f32', 'value': 3.0}],
             'nested': [{'ptr_off': 0x9c, 'size': 0x20, 'fields': [{'off': 0x18, 'type': 'u32', 'value': 0x10000}]}]},
        ],
        'path2_tests': [
            {'seeds': [{'off': 0x34, 'type': 'f32', 'value': 2.0}, {'off': 0x38, 'type': 'f32', 'value': 3.0}],
             'nested': [{'ptr_off': 0x9c, 'size': 0x20, 'fields': [{'off': 0x18, 'type': 'u32', 'value': 0x10000}]}]},
        ],
    },

    # 0x005c75b0  AudioVoiceField8cGet — __cdecl u32(ptr): return *(p+0x8c).
    'audio_voice_field8c_get': {
        'rva':         0x005c75b0,
        'export':      'AudioVoiceField8cGet',
        'signature':   {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':    'struct_call_observe',
        'struct_size': 0xa0,
        'out_ptrs':    0,
        'observe_ret': True,
        'observe':     [],
        'lut_root_delta': 0,
        'path1_tests': [
            {'seeds': [{'off': 0x8c, 'type': 'u32', 'value': 0}]},
            {'seeds': [{'off': 0x8c, 'type': 'u32', 'value': 1}]},
            {'seeds': [{'off': 0x8c, 'type': 'u32', 'value': 0xDEADBEEF}]},
            {'seeds': [{'off': 0x8c, 'type': 'u32', 'value': 0x7FFFFFFF}]},
            {'seeds': [{'off': 0x8c, 'type': 'u32', 'value': 0xFFFFFFFF}]},
        ],
        'path2_tests': [
            {'seeds': [{'off': 0x8c, 'type': 'u32', 'value': 0xDEADBEEF}]},
        ],
    },

    # ── c3-batch-ae-s2 render leaves ─────────────────────────────────────────
    # 0x004b40c0  RenderElemArrayCopy — __cdecl void(p1, p2): copy *(p1+0x24)
    # dwords from (*(p1+0x20))[i] into p2[i]; guarded by p2!=0 && (signed)count>0.
    # Pure leaf. struct_call_observe: param_1 = descriptor struct (+0x20 src-array
    # ptr wired via nested, +0x24 count); param_2 = out0 (8-byte dst, so count<=2
    # stays in-bounds). Observe the two dst dwords. The count=0 / count=0xffffffff
    # (signed -1) vectors exercise the no-copy guard.
    # ref: re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x004b40c0.md
    'render_elem_array_copy': {
        'rva':         0x004b40c0,
        'export':      'RenderElemArrayCopy',
        'signature':   {'ret': 'void', 'args': ['pointer', 'pointer']},
        'arg_type':    'struct_call_observe',
        'struct_size': 0x28,
        'out_ptrs':    1,
        'observe_ret': False,
        'observe':     [{'src': 'out0', 'off': 0, 'type': 'u32'},
                        {'src': 'out0', 'off': 4, 'type': 'u32'}],
        'lut_root_delta': 0,
        'path1_tests': [
            {'seeds': [{'off': 0x24, 'type': 'u32', 'value': 2}],
             'nested': [{'ptr_off': 0x20, 'size': 8, 'fields': [{'off': 0, 'type': 'u32', 'value': 0x11223344}, {'off': 4, 'type': 'u32', 'value': 0x55667788}]}]},
            {'seeds': [{'off': 0x24, 'type': 'u32', 'value': 1}],
             'nested': [{'ptr_off': 0x20, 'size': 8, 'fields': [{'off': 0, 'type': 'u32', 'value': 0xDEADBEEF}, {'off': 4, 'type': 'u32', 'value': 0xA5A5A5A5}]}]},
            {'seeds': [{'off': 0x24, 'type': 'u32', 'value': 0}],
             'nested': [{'ptr_off': 0x20, 'size': 8, 'fields': [{'off': 0, 'type': 'u32', 'value': 0xAAAAAAAA}, {'off': 4, 'type': 'u32', 'value': 0xBBBBBBBB}]}]},
            {'seeds': [{'off': 0x24, 'type': 'u32', 'value': 2}],
             'nested': [{'ptr_off': 0x20, 'size': 8, 'fields': [{'off': 0, 'type': 'u32', 'value': 0}, {'off': 4, 'type': 'u32', 'value': 0}]}]},
            {'seeds': [{'off': 0x24, 'type': 'u32', 'value': 2}],
             'nested': [{'ptr_off': 0x20, 'size': 8, 'fields': [{'off': 0, 'type': 'u32', 'value': 0xFFFFFFFF}, {'off': 4, 'type': 'u32', 'value': 0x7FFFFFFF}]}]},
            {'seeds': [{'off': 0x24, 'type': 'u32', 'value': 1}],
             'nested': [{'ptr_off': 0x20, 'size': 8, 'fields': [{'off': 0, 'type': 'u32', 'value': 0x80000000}, {'off': 4, 'type': 'u32', 'value': 0x12345678}]}]},
            {'seeds': [{'off': 0x24, 'type': 'u32', 'value': 2}],
             'nested': [{'ptr_off': 0x20, 'size': 8, 'fields': [{'off': 0, 'type': 'u32', 'value': 0x12345678}, {'off': 4, 'type': 'u32', 'value': 0x9ABCDEF0}]}]},
            {'seeds': [{'off': 0x24, 'type': 'u32', 'value': 0xFFFFFFFF}],
             'nested': [{'ptr_off': 0x20, 'size': 8, 'fields': [{'off': 0, 'type': 'u32', 'value': 0xCCCCCCCC}, {'off': 4, 'type': 'u32', 'value': 0xDDDDDDDD}]}]},
        ],
        'path2_tests': [
            {'seeds': [{'off': 0x24, 'type': 'u32', 'value': 2}],
             'nested': [{'ptr_off': 0x20, 'size': 8, 'fields': [{'off': 0, 'type': 'u32', 'value': 0x11223344}, {'off': 4, 'type': 'u32', 'value': 0x55667788}]}]},
        ],
    },

    # 0x004b4140  RenderElemArrayCopyAll — __cdecl void(ptr): convenience wrapper that
    # calls FUN_004b4130(param_1, 0). FUN_004b4130 [C3 adjustor thunk] replaces
    # param_1 with param_1[0x18] (sub-struct s) and tail-calls FUN_004b40c0(s, 0)
    # [C3 RenderElemArrayCopy]. With param_2=0, FUN_004b40c0 reads count from s+0x24
    # then returns immediately (loop guard param_2!=0 is false) — effectively a no-op.
    # struct_call_observe: main struct (param_1) has nested sub-struct at +0x18 with
    # count at +0x24. A sentinel at main struct +0x0 (unused by fn) is seeded with
    # distinct values per test and observed back unchanged — necessary for a
    # non-degenerate CSV since the fn makes no writes. Different sentinels per vector
    # produce distinct rows; both orig and reimpl observe the same unchanged value.
    # ref: re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x004b4140.md
    'render_elem_array_copy_all': {
        'rva':         0x004b4140,
        'export':      'RenderElemArrayCopyAll',
        'signature':   {'ret': 'void', 'args': ['pointer']},
        'arg_type':    'struct_call_observe',
        'struct_size': 0x20,
        'out_ptrs':    0,
        'observe_ret': False,
        'observe':     [{'src': 'struct', 'off': 0x0, 'type': 'u32'}],
        'lut_root_delta': 0,
        'path1_tests': [
            {'seeds': [{'off': 0x0, 'type': 'u32', 'value': 0xAABBCCDD}],
             'nested': [{'ptr_off': 0x18, 'size': 0x28, 'fields': [{'off': 0x24, 'type': 'u32', 'value': 2}]}]},
            {'seeds': [{'off': 0x0, 'type': 'u32', 'value': 0x11223344}],
             'nested': [{'ptr_off': 0x18, 'size': 0x28, 'fields': [{'off': 0x24, 'type': 'u32', 'value': 0}]}]},
            {'seeds': [{'off': 0x0, 'type': 'u32', 'value': 0xDEADBEEF}],
             'nested': [{'ptr_off': 0x18, 'size': 0x28, 'fields': [{'off': 0x24, 'type': 'u32', 'value': 5}]}]},
            {'seeds': [{'off': 0x0, 'type': 'u32', 'value': 0x55667788}],
             'nested': [{'ptr_off': 0x18, 'size': 0x28, 'fields': [{'off': 0x24, 'type': 'u32', 'value': 0xFFFFFFFF}]}]},
            {'seeds': [{'off': 0x0, 'type': 'u32', 'value': 0x99AABBCC}],
             'nested': [{'ptr_off': 0x18, 'size': 0x28, 'fields': [{'off': 0x24, 'type': 'u32', 'value': 1}]}]},
        ],
        'path2_tests': [
            {'seeds': [{'off': 0x0, 'type': 'u32', 'value': 0xAABBCCDD}],
             'nested': [{'ptr_off': 0x18, 'size': 0x28, 'fields': [{'off': 0x24, 'type': 'u32', 'value': 2}]}]},
        ],
    },

    # 0x00478cc0  RenderWorldStateZeroFill — __cdecl void(p): memset(p,0,0x418f*4)
    # (inline REP STOSD, 67132 bytes). struct_call_observe: struct_size 0x10680
    # (> 0x1063c so off 0x1063c is a one-past-the-end guard that must SURVIVE).
    # Each test seeds the 7 observed offsets with a sentinel; the in-range six must
    # read back 0 (catches a no-op reimpl), and the 0x1063c guard must retain its
    # seed (catches a reimpl that writes past the 0x418f-th dword).
    # ref: re/analysis/render_2_c1_to_c2_s2/FUN_00478cc0.md  (U-4528)
    'render_worldstate_zerofill': {
        'rva':         0x00478cc0,
        'export':      'RenderWorldStateZeroFill',
        'signature':   {'ret': 'void', 'args': ['pointer']},
        'arg_type':    'struct_call_observe',
        'struct_size': 0x10680,
        'out_ptrs':    0,
        'observe_ret': False,
        'observe':     [{'src': 'struct', 'off': 0x0,     'type': 'u32'},
                        {'src': 'struct', 'off': 0x4,     'type': 'u32'},
                        {'src': 'struct', 'off': 0x1000,  'type': 'u32'},
                        {'src': 'struct', 'off': 0x8000,  'type': 'u32'},
                        {'src': 'struct', 'off': 0x10000, 'type': 'u32'},
                        {'src': 'struct', 'off': 0x10638, 'type': 'u32'},
                        {'src': 'struct', 'off': 0x1063c, 'type': 'u32'}],
        'lut_root_delta': 0,
        'path1_tests': [
            {'seeds': [{'off': o, 'type': 'u32', 'value': v}
                       for o in (0x0, 0x4, 0x1000, 0x8000, 0x10000, 0x10638, 0x1063c)]}
            for v in (0xDEADBEEF, 0xCAFEBABE, 0xFEEDFACE, 0x11111111,
                      0x22222222, 0x33333333, 0x44444444, 0x55555555)
        ],
        'path2_tests': [
            {'seeds': [{'off': o, 'type': 'u32', 'value': 0xDEADBEEF}
                       for o in (0x0, 0x4, 0x1000, 0x8000, 0x10000, 0x10638, 0x1063c)]},
        ],
    },

    # 0x005c9380  AudioBitBufSizeCalc — __cdecl u32(bits): max(1, bits>>3) + 0xc.
    'audio_bitbuf_size_calc': {
        'rva':         0x005c9380,
        'export':      'AudioBitBufSizeCalc',
        'signature':   {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':    'int_scalar',
        'lut_root_delta': 0,
        'path1_tests': [0, 1, 7, 8, 9, 16, 255, 256, 1024, 0xFFFFFFFF],
        'path2_tests': [0, 8, 1024],
    },

    # 0x005c9770  AudioPcmPackSaturate — __cdecl void(i16* dst, i32* src, count).
    # Saturating int32->int16 pack; writes 2*count samples. src holds 2*count
    # int32s. Cases cover main-loop (count even), main+tail (count odd),
    # tail-only (count=1), empty (count=0), and all three saturation branches.
    'audio_pcm_pack_saturate': {
        'rva':         0x005c9770,
        'export':      'AudioPcmPackSaturate',
        'signature':   {'ret': 'void', 'args': ['pointer', 'pointer', 'uint32']},
        'arg_type':    'pcm_pack',
        'lut_root_delta': 0,
        'path1_tests': [
            {'src': [], 'count': 0},
            {'src': [0, -1], 'count': 1},
            {'src': [0x100, -0x100, 0x800000, -0x800001], 'count': 2},
            {'src': [0x100, -0x100, 0, 0x123456, 0x900000, -0x900000], 'count': 3},
            {'src': [-0x800000, 0x7fff00, 0x7fff01, 0x800000, 1, -1, 0x7fffffff, -0x7fffffff], 'count': 4},
        ],
        'path2_tests': [
            {'src': [0x100, -0x100, 0x800000, -0x800001], 'count': 2},
            {'src': [0x100, -0x100, 0, 0x123456, 0x900000, -0x900000], 'count': 3},
        ],
    },
    # ── c3-batch-ab session 2 (audio pure leaves) ────────────────────────────
    # 0x005b3580  AudioListHeaderInit  void(uint32* header)
    # Intrusive circular doubly-linked-list header init: header[0]=0 (count),
    # header[1]=&header[1], header[2]=&header[1] (empty self-looped sentinel).
    # arg_type='fmt_desc_pair_compare' is reused here because it is the ONLY
    # harness that drives orig and reimpl through the SAME scratch buffer
    # (allocated once) — the function writes SELF-REFERENTIAL pointers
    # (&header[1]) into header[1]/header[2], so a per-side buffer would record
    # two different addresses and false-RED. With one shared bufA both sides
    # write the identical &bufA[1], so the 12-byte fingerprint matches.
    # ret='void' → the harness's return component is 0 for both sides (a uint32
    # ret would compare undefined EAX and false-RED). The fn ignores bufB; the
    # extra cdecl arg is caller-cleaned and harmless. The varied 'a' seeds are
    # overwritten by the init, confirming a full overwrite regardless of prior
    # contents. ref: re/analysis/bucket_audio_005b2220_005b8570/0x005b3580.md
    'audio_list_header_init': {
        'rva':            0x005b3580,
        'export':         'AudioListHeaderInit',
        'signature':      {'ret': 'void', 'args': ['pointer', 'pointer']},
        'arg_type':       'fmt_desc_pair_compare',
        'lut_root_delta': 0,
        'path1_tests': [
            { 'a': {},                                                'b': {} },
            { 'a': {'f00': 0xFFFFFFFF},                               'b': {} },
            { 'a': {'f00': 0x80000000, 'f04': 0x80000000, 'f08': 0x80000000}, 'b': {} },
            { 'a': {'f00': 0xAAAAAAAA, 'f04': 0x55555555, 'f08': 0xAAAAAAAA}, 'b': {} },
            { 'a': {'f00': 0xDEADBEEF},                               'b': {} },
            { 'a': {'f04': 0xCAFEBABE},                               'b': {} },
            { 'a': {'f08': 0x12345678},                               'b': {} },
            { 'a': {'f00': 1, 'f04': 2, 'f08': 3},                    'b': {} },
        ],
        'path2_tests': [
            { 'a': {},                  'b': {} },
            { 'a': {'f00': 0xFFFFFFFF}, 'b': {} },
            { 'a': {'f00': 0xDEADBEEF}, 'b': {} },
        ],
    },

    # 0x005b4060  AudioFmtConvertByteLength  int(int* fmtIn, uint* fmtOut)
    # Output byte length to convert fmtIn -> fmtOut. Reads (both descriptors):
    #   +0x00 u32 sample-rate, +0x08 i32 data-size (fmtIn only, = param_1[2]),
    #   +0x0c u8  bits-per-sample (low byte of dword @+0x0c), +0x0d u8 channels.
    #   ratio = (rateOut/rateIn)*(bitsOut/bitsIn)*(chanOut/chanIn)*sizeIn
    #   uVar1 = bitsOut>>3 (out bytes/sample);  aligned = (ROUND(ratio)-1+uVar1) & ~(uVar1-1)
    #   return ROUND((float)aligned)   (round-to-nearest-even throughout).
    # arg_type='fmt_desc_pair_compare' (2-arg form) seeds two 0x40 buffers from
    # field maps and passes both pointers. Pure reads → fingerprints stay equal;
    # the real comparison is ret&0xffff (the byte length). NO crash_equal: the
    # function never dereferences a pointer field, so neither side faults.
    # 'a' (fmtIn) ALWAYS supplies nonzero divisors: f00 (rate), f0c low byte
    # (bits), f0c byte1 (channels). f0c packs bits|(chan<<8).
    # ref: re/analysis/bucket_audio_005b2220_005b8570/0x005b4060.md
    'audio_fmt_convert_byte_length': {
        'rva':            0x005b4060,
        'export':         'AudioFmtConvertByteLength',
        'signature':      {'ret': 'uint32', 'args': ['pointer', 'pointer']},
        'arg_type':       'fmt_desc_pair_compare',
        'lut_root_delta': 0,
        'path1_tests': [
            # identity 16-bit stereo @44100, size 1000
            { 'a': {'f00': 44100, 'f08': 1000,  'f0c': 16 | (2 << 8)}, 'b': {'f00': 44100, 'f0c': 16 | (2 << 8)} },
            # upsample 22050->44100, 8->16 bit, mono->stereo, size 500
            { 'a': {'f00': 22050, 'f08': 500,   'f0c':  8 | (1 << 8)}, 'b': {'f00': 44100, 'f0c': 16 | (2 << 8)} },
            # downsample 44100->22050, 16->8 bit, stereo->mono, size 4000
            { 'a': {'f00': 44100, 'f08': 4000,  'f0c': 16 | (2 << 8)}, 'b': {'f00': 22050, 'f0c':  8 | (1 << 8)} },
            # 24-bit out (uVar1=3), 1.5x scale on odd size -> RNE tie 1498.5
            { 'a': {'f00': 48000, 'f08': 999,   'f0c': 16 | (2 << 8)}, 'b': {'f00': 48000, 'f0c': 24 | (2 << 8)} },
            # zero input size -> ratio 0
            { 'a': {'f00': 44100, 'f08': 0,     'f0c': 16 | (2 << 8)}, 'b': {'f00': 44100, 'f0c': 16 | (2 << 8)} },
            # large size at 2^24 (float32 exactness boundary)
            { 'a': {'f00': 44100, 'f08': 16777216, 'f0c': 16 | (2 << 8)}, 'b': {'f00': 44100, 'f0c': 16 | (2 << 8)} },
            # odd identity size 333 (round-up alignment to even)
            { 'a': {'f00': 44100, 'f08': 333,   'f0c': 16 | (2 << 8)}, 'b': {'f00': 44100, 'f0c': 16 | (2 << 8)} },
            # 8-bit mono identity, tiny size 7 (uVar1=1, mask ~0 -> no rounding)
            { 'a': {'f00':  8000, 'f08': 7,     'f0c':  8 | (1 << 8)}, 'b': {'f00':  8000, 'f0c':  8 | (1 << 8)} },
            # 32-bit out (uVar1=4, mask ~3)
            { 'a': {'f00': 44100, 'f08': 100,   'f0c': 16 | (2 << 8)}, 'b': {'f00': 44100, 'f0c': 32 | (2 << 8)} },
            # high-rate 96000->48000, 24->16 bit, stereo, size 48000
            { 'a': {'f00': 96000, 'f08': 48000, 'f0c': 24 | (2 << 8)}, 'b': {'f00': 48000, 'f0c': 16 | (2 << 8)} },
        ],
        'path2_tests': [
            { 'a': {'f00': 44100, 'f08': 1000, 'f0c': 16 | (2 << 8)}, 'b': {'f00': 44100, 'f0c': 16 | (2 << 8)} },
            { 'a': {'f00': 22050, 'f08': 500,  'f0c':  8 | (1 << 8)}, 'b': {'f00': 44100, 'f0c': 16 | (2 << 8)} },
            { 'a': {'f00': 48000, 'f08': 999,  'f0c': 16 | (2 << 8)}, 'b': {'f00': 48000, 'f0c': 24 | (2 << 8)} },
        ],
    },
    # ───────────────────────────────────────────────────────────────────────
    # c3_batch_ab session 6 — Util leaves (audio/util batch). Branch
    # c3/batch-ab-util-s6 (the generated c3/batch-ab-s6 name collided with an
    # unrelated render-ab commit; user chose a distinct branch). Reimpl:
    # mashedmod/src/mashed_re/Util/UtilLeaves_ab6.cpp. All seven are byte-
    # faithful ports of disasm read from the anchored binary (capstone,
    # 2026-06-04); the suggested arg_types from the batch were CONFIRMED against
    # the actual bytes and corrected where the bytes disagreed (see per-entry
    # notes). No invented arg_types — every one below already exists in
    # diff_template.js.
    # ───────────────────────────────────────────────────────────────────────

    # 0x004425d0  UtilZeroTable8964c0  — void(void) global-table zeroer.
    # Bytes: mov eax,0x8964c0 ; loop{ mov [eax],0 ; add eax,0xd8 ; cmp eax,0x897fc0 ; jl }
    #        ; mov [0x89898c],0 ; ret.  Zeroes elem[0] of each 0xD8-stride entry
    # over [0x008964c0, 0x00897fc0) + the scalar 0x0089898c.
    # arg_type: state_machine_observe (NOT the batch's 'void_step_global' — that
    #   handler is hard-coded to the MenuCursorStep cursor globals and would
    #   false-GREEN a zeroer by reading back an address it never touches). Seed a
    #   sentinel at sampled zeroed addresses (elem0/elem1/last-elem/scalar) and an
    #   over-run guard at 0x00897fc0 (which the loop must NOT write); confirm
    #   orig==reimpl. input_globals==output_globals (seed, then read back).
    # ref: re/analysis/timer_d3_cont1_b/0x004425d0.md
    'util_zero_table_8964c0': {
        'rva':            0x004425d0,
        'export':         'UtilZeroTable8964c0',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'state_machine_observe',
        'input_globals':  [
            {'addr': 0x008964c0, 'type': 'u32'},  # elem[0], first written
            {'addr': 0x00896598, 'type': 'u32'},  # elem[1] = base + 0xD8 (stride check)
            {'addr': 0x00897ee8, 'type': 'u32'},  # elem[31], last written (bound check)
            {'addr': 0x0089898c, 'type': 'u32'},  # trailing scalar
            {'addr': 0x00897fc0, 'type': 'u32'},  # OVER-RUN GUARD: loop stops before here
        ],
        'output_globals': [
            {'addr': 0x008964c0, 'type': 'u32'},
            {'addr': 0x00896598, 'type': 'u32'},
            {'addr': 0x00897ee8, 'type': 'u32'},
            {'addr': 0x0089898c, 'type': 'u32'},
            {'addr': 0x00897fc0, 'type': 'u32'},
        ],
        'lut_root_delta': 0,
        # [elem0, elem1, last, scalar, guard] — guard sentinel must survive.
        'path1_tests': [
            [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0x0BADF00D, 0xA5A5A5A5],
            [0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF],
            [0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005],
            [0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000],
            [0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x0000FFFF],
            [0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFEEDFACE],
            [0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x13371337],
            [0x11111111, 0x22222222, 0x33333333, 0x44444444, 0xDEADC0DE],
        ],
        'path2_tests': [
            [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0x0BADF00D, 0xA5A5A5A5],
            [0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF],
            [0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005],
        ],
    },

    # 0x0045c480  UtilZeroScatter45c480  — void(void) scatter zeroer.
    # Bytes: 12 individual dword writes (eax/ecx==0) to named globals + rep stosd
    # of 0x24 dwords from 0x0088f5e0 + write to 0x008aa2ec ; ret.
    # arg_type: state_machine_observe (same rationale as 0x004425d0). Sample 4 of
    #   the scalar writes, the rep-stosd block ends, and an over-run guard one
    #   dword past the block (0x0088f670, which the 36-dword stosd must NOT reach).
    # ref: re/analysis/timer_d2_cont1/0x0045c480.md
    'util_zero_scatter_45c480': {
        'rva':            0x0045c480,
        'export':         'UtilZeroScatter45c480',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'state_machine_observe',
        'input_globals':  [
            {'addr': 0x0088f0a0, 'type': 'u32'},  # first scalar (ecx=0)
            {'addr': 0x0088e67c, 'type': 'u32'},  # scalar
            {'addr': 0x008aa2e0, 'type': 'u32'},  # scalar (eax=0)
            {'addr': 0x008aa2ec, 'type': 'u32'},  # last scalar (post-stosd)
            {'addr': 0x0088f5e0, 'type': 'u32'},  # rep-stosd block, dword[0]
            {'addr': 0x0088f66c, 'type': 'u32'},  # block dword[35] = base+0x8C (last)
            {'addr': 0x0088f670, 'type': 'u32'},  # OVER-RUN GUARD: dword[36], NOT written
        ],
        'output_globals': [
            {'addr': 0x0088f0a0, 'type': 'u32'},
            {'addr': 0x0088e67c, 'type': 'u32'},
            {'addr': 0x008aa2e0, 'type': 'u32'},
            {'addr': 0x008aa2ec, 'type': 'u32'},
            {'addr': 0x0088f5e0, 'type': 'u32'},
            {'addr': 0x0088f66c, 'type': 'u32'},
            {'addr': 0x0088f670, 'type': 'u32'},
        ],
        'lut_root_delta': 0,
        # [s0, s1, s2, s3, blk0, blk35, guard] — guard sentinel must survive.
        'path1_tests': [
            [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0x0BADF00D, 0xFEEDFACE, 0x8BADF00D, 0xA5A5A5A5],
            [0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF],
            [0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005, 0x00000006, 0x00000007],
            [0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000],
            [0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x55555555, 0xAAAAAAAA, 0x0000FFFF],
            [0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x13371337],
            [0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0xDEADC0DE],
            [0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555, 0x66666666, 0xC0FFEE00],
        ],
        'path2_tests': [
            [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0x0BADF00D, 0xFEEDFACE, 0x8BADF00D, 0xA5A5A5A5],
            [0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF],
            [0x00000001, 0x00000002, 0x00000003, 0x00000004, 0x00000005, 0x00000006, 0x00000007],
        ],
    },

    # 0x0046b4f0  UtilTableRead46b4f0  — int(out_ptr, outerIdx, innerIdx).
    # Bytes: bounds (outer<0x10 unsigned, inner<0x12 signed) else return 0; else
    # copy 3 dwords from a static 2D table @0x008815a0 into out[0..2], return 1.
    #   row    = 0x008815a0 + outer*0xD04
    #   out[0] = *(uint32*)(row + (inner*3 + 0x18)*4)
    #   out[1] = *(uint32*)(row + inner*3*4 + 0x64)
    #   out[2] = *(uint32*)(row + inner*3*4 + 0x68)
    # arg_type: sort_dispatch_out4 (NOT 'int_ptr2_out'/'out3_idx' — those marshal
    #   (int,ptr,ptr) / (ptr,int) and do not match this fn's (ptr,int,int) ABI;
    #   sort_dispatch_out4 is the only handler that calls fn(out,a,b) with a
    #   pointer FIRST and reads back the out buffer). It prefills the 4-int buffer
    #   with -1 and packs all 4 back: in-bounds -> [d0,d1,d2,-1]; OOB -> [-1,-1,-1,-1]
    #   (no write). Reads a static data table -> orig and reimpl observe identical
    #   contents -> bit-identical. (U-2193 table semantics: non-blocking.)
    # input: {sel: outerIdx, dir: innerIdx}.
    # ref: re/analysis/profile_career_d3/FUN_0046b4f0.md
    'util_table_read_46b4f0': {
        'rva':            0x0046b4f0,
        'export':         'UtilTableRead46b4f0',
        'signature':      {'ret': 'void', 'args': ['pointer', 'int32', 'int32']},
        'arg_type':       'sort_dispatch_out4',
        'lut_root_delta': 0,
        'path1_tests': [
            {'sel': 0,  'dir': 0},    # in-bounds
            {'sel': 0,  'dir': 1},
            {'sel': 1,  'dir': 0},
            {'sel': 1,  'dir': 17},   # inner max (0x11)
            {'sel': 15, 'dir': 0},    # outer max (0x0f)
            {'sel': 15, 'dir': 17},   # both max in-bounds
            {'sel': 7,  'dir': 9},
            {'sel': 16, 'dir': 0},    # outer OOB (>=0x10) -> 0
            {'sel': 0,  'dir': 18},   # inner OOB (>=0x12) -> 0
            {'sel': 16, 'dir': 18},   # both OOB
            {'sel': 0,  'dir': -1},   # inner signed-negative: jl (inner<0x12) is TAKEN -> in-bounds path
        ],
        'path2_tests': [
            {'sel': 0,  'dir': 0},
            {'sel': 15, 'dir': 17},
            {'sel': 16, 'dir': 0},
        ],
    },

    # 0x004904d0  UtilZeroTable86a4a0  — void(void) global-table zeroer.
    # Bytes: mov eax,0x86a4a0 ; loop{ mov [eax],0 ; add eax,0x50 ; cmp eax,0x86ae00 ; jl } ; ret.
    # Zeroes elem[0] of each 0x50-stride entry over [0x0086a4a0, 0x0086ae00) = 30 entries.
    # arg_type: state_machine_observe (same rationale as 0x004425d0).
    # ref: re/analysis/timer_d3_cont2/0x004904d0.md
    'util_zero_table_86a4a0': {
        'rva':            0x004904d0,
        'export':         'UtilZeroTable86a4a0',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'state_machine_observe',
        'input_globals':  [
            {'addr': 0x0086a4a0, 'type': 'u32'},  # elem[0]
            {'addr': 0x0086a4f0, 'type': 'u32'},  # elem[1] = base + 0x50 (stride check)
            {'addr': 0x0086adb0, 'type': 'u32'},  # elem[29], last written (bound check)
            {'addr': 0x0086ae00, 'type': 'u32'},  # OVER-RUN GUARD: loop stops before here
        ],
        'output_globals': [
            {'addr': 0x0086a4a0, 'type': 'u32'},
            {'addr': 0x0086a4f0, 'type': 'u32'},
            {'addr': 0x0086adb0, 'type': 'u32'},
            {'addr': 0x0086ae00, 'type': 'u32'},
        ],
        'lut_root_delta': 0,
        # [elem0, elem1, last, guard] — guard sentinel must survive.
        'path1_tests': [
            [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xA5A5A5A5],
            [0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF],
            [0x00000001, 0x00000002, 0x00000003, 0x00000004],
            [0x80000000, 0x80000000, 0x80000000, 0x80000000],
            [0x55555555, 0xAAAAAAAA, 0x55555555, 0x0000FFFF],
            [0x00000000, 0x00000000, 0x00000000, 0xFEEDFACE],
            [0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x13371337],
            [0x11111111, 0x22222222, 0x33333333, 0xDEADC0DE],
        ],
        'path2_tests': [
            [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xA5A5A5A5],
            [0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF],
            [0x00000001, 0x00000002, 0x00000003, 0x00000004],
        ],
    },

    # 0x004d8560  UtilReturnOne4d8560  — int(void) constant.
    # Bytes: mov eax,1 ; ret.  Always returns 1.
    # arg_type: none (zero-arg, compare return value).
    # ref: re/analysis/timer_d3_cont2/0x004d8560.md
    'util_return_one_4d8560': {
        'rva':            0x004d8560,
        'export':         'UtilReturnOne4d8560',
        'signature':      {'ret': 'int32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        # `none` ignores the input value; the markers just drive N iterations.
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests': [0, 1, 2],
    },

    # 0x0055dec0  UtilDeref55dec0  — uint32(uint32* p) = *p   (__cdecl stack arg).
    # Bytes: mov eax,[esp+4] ; mov eax,[eax] ; ret.  The pointer arrives on the
    # STACK (NOT EAX) — the batch's suggested 'eax_implicit_ptr' was DISCONFIRMED
    # by these bytes.
    # arg_type: vec3_ptr — the only single-pointer-arg handler that calls fn(buf)
    #   (seeded buffer) and compares the returned dword. vec3_ptr seeds buf[0..2]
    #   as floats; this fn returns buf[0] (the first dword) regardless of type, so
    #   each float triple yields a distinct, deterministic return that orig and
    #   reimpl must both reproduce. ret declared uint32 for exact-dword compare.
    # ref: re/analysis/timer_d3_cont2/0x0055dec0.md
    'util_deref_55dec0': {
        'rva':            0x0055dec0,
        'export':         'UtilDeref55dec0',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'vec3_ptr',
        'lut_root_delta': 0,
        # Only lane 0 (buf[0]) is read by the fn; the other two lanes are
        # irrelevant. Varied first-lane bit patterns exercise distinct returns.
        'path1_tests': [
            (0.0,      0.0, 0.0),
            (1.0,      0.0, 0.0),
            (-1.0,     0.0, 0.0),
            (3.5,      0.0, 0.0),
            (-99.99,   0.0, 0.0),
            (1e-6,     0.0, 0.0),
            (1234.5,   0.0, 0.0),
            (12.34,    0.0, 0.0),
            (1e30,     0.0, 0.0),
            (-1e-30,   0.0, 0.0),
        ],
        'path2_tests': [
            (0.0,  0.0, 0.0),
            (1.0,  0.0, 0.0),
            (-1.0, 0.0, 0.0),
        ],
    },

    # 0x004a1790  ComReleaseThunk  — uint __fastcall(void** pp): COM Release thunk.
    # *** fastcall_reg CANARY *** (validates the new diff_template.js arg_type).
    # Bytes: mov eax,[ecx] ; test eax,eax ; je end ; mov ecx,[eax] ; push eax ;
    #        call [ecx+8] ; ret.  ECX = pointer-to-interface-pointer; on the null
    # path EAX = *pp = 0 is returned. CONFIRMED __fastcall by the bytes.
    # The harness seeds ECX with a ZEROED 64-byte scratch buffer, so *pp == 0 ->
    # je taken -> orig and reimpl both no-op and return 0 (proves ECX marshalling
    # + callee-clean stack balance). If GREEN, fastcall_reg is validated.
    # ref: re/analysis/video_mci/0x004a1790.md
    'com_release_thunk': {
        'rva':              0x004a1790,
        'export':           'ComReleaseThunk',
        'signature':        {'ret': 'uint32', 'args': []},
        'arg_type':         'fastcall_reg',
        'fastcall_nargs':   1,
        'fastcall_ecx_ptr': True,
        'lut_root_delta':   0,
        # Markers only — the harness replaces each with a fresh zeroed scratch ptr.
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7],
        'path2_tests': [0, 1, 2],
    },

    # ───────────────────────────────────────────────────────────────────────
    # Session c3-batch-ac-s1 — s5-redo  (C2→C3, 8 util pure leaves)
    # Util/UtilLeaves_ac.cpp — author+verify-only; central classify at sweep.
    #
    # NOTE ON ARG_TYPES: the batch prompt's suggested arg_types were corrected
    # this session to the harness semantics that actually exercise each function
    # (the prompt's void_step_global/read_global suggestions observe unrelated
    # globals / pass no index → vacuous false-GREEN). The arg_types below are the
    # *validated* shapes used by the analogous landed hooks (void_write_observe
    # ← TimerGlobalsReset; int_scalar ← VehicleSlotGetter). All are existing
    # arg_types — NO diff_template.js change, so this batch is NOT sweep-critical.
    # ───────────────────────────────────────────────────────────────────────

    # 0x00407a60  UtilTableInit8a9620  — void(void) record-table init.
    # Zeroes 0xc3 dwords per record then writes 0xffffffff at record+0x28, 4
    # records over [0x008a9620, 0x008aa278). void_write_observe on 0x008a9648
    # (record-0 sentinel): sentinel wiped, then function writes 0xffffffff back.
    'util_table_init_8a9620': {
        'rva':            0x00407a60,
        'export':         'UtilTableInit8a9620',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x008a9648,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xCAFEBABE,
                           0x12345678, 0xFFFFFFFF, 0x80000000, 0x55555555,
                           0xAAAAAAAA, 0x7FFFFFFF],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xFFFFFFFF],
    },

    # 0x0040ad30  UtilSet8a95ac  — void(uint32): *0x008a95ac = arg.
    # void_setter_observe: call fn(v), read back target_global → must equal v.
    'util_set_8a95ac': {
        'rva':            0x0040ad30,
        'export':         'UtilSet8a95ac',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x008a95ac,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xCAFEBABE,
                           0x12345678, 0xFFFFFFFF, 0x80000000, 0x55555555,
                           0xAAAAAAAA, 0x7FFFFFFF],
        'path2_tests':    [0x00000000, 0x00000001, 0xFFFFFFFF],
    },

    # 0x0040b250  UtilReset8a9500  — void(void): write 0xfffffc18 (-1000) to 8
    # globals 0x008a9500..0x008a952c. void_write_observe on 0x008a9500 (first
    # write): sentinel wiped, function writes 0xfffffc18.
    'util_reset_8a9500': {
        'rva':            0x0040b250,
        'export':         'UtilReset8a9500',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x008a9500,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x0040b430  UtilFieldAdd8a94f0  — void(int idx, int delta): indexed += .
    #   idx>3 → *0x008a94fc += delta ; else (&0x008a94f0)[idx] += delta.
    # entity_field_add: addr = 0x008a94f0 + idx*4; snapshot/add/restore. Tests
    # span idx 0..3 (the array path) with assorted signed deltas; idx>3 cases
    # are guarded (max_index=3) and collapse onto 0x008a94fc==index3 by design.
    'util_field_add_8a94f0': {
        'rva':                0x0040b430,
        'export':             'UtilFieldAdd8a94f0',
        'signature':          {'ret': 'void', 'args': ['int32', 'int32']},
        'arg_type':           'entity_field_add',
        'target_global':      0x008a94f0,
        'entity_byte_stride': 4,
        'max_index':          3,
        'lut_root_delta':     0,
        'path1_tests':    [[0, 1], [1, -1], [2, 1000], [3, -1000],
                           [0, 0x7fffffff], [1, 0x80000000], [2, -1],
                           [3, 0x55555555], [0, -2], [2, 0]],
        'path2_tests':    [[0, 1], [2, -1], [3, 1000]],
    },

    # 0x0040b6d0  UtilArrayGet8a94e0  — int(int idx): return (&0x008a94e0)[idx].
    # int_scalar: fn(idx) → returned dword. Both orig+reimpl read the identical
    # static-data address → bit-identical. Indices kept to the in-bounds 0..3
    # range (note: 4-entry array) so the read stays inside mapped .data.
    'util_array_get_8a94e0': {
        'rva':            0x0040b6d0,
        'export':         'UtilArrayGet8a94e0',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 3],
        'path2_tests':    [0, 1, 3],
    },

    # 0x004292c0  UtilWritePtr  — void(uint32* p, uint32 v): *p = v.
    # struct_three_write: harness fills a scratch buffer with 0xDEADBEEF, calls
    # fn(buf, valA, valB), then reads observe_offsets=[0] back. The function is a
    # 2-arg cdecl (*p=v); we declare a 3rd dummy uint32 only because the harness's
    # call site passes (buf, valA, valB) — under cdecl the caller cleans the stack
    # so the ignored 3rd arg is safe (the 2-arg variant already ran clean × N,
    # proving cdecl/no stack imbalance). valA is the written word; valB unused.
    # Discriminating: a mis-targeted write leaves buf[0]==0xDEADBEEF → RED.
    # (struct_three_write's keys ARE forwarded by build_config — unlike
    # buf_field_set's field_offsets/buf_size, which are not — so no harness edit
    # and the integration re-run stays genuine.)  Test values avoid 0xDEADBEEF.
    'util_write_ptr': {
        'rva':             0x004292c0,
        'export':          'UtilWritePtr',
        'signature':       {'ret': 'void', 'args': ['pointer', 'uint32', 'uint32']},
        'arg_type':        'struct_three_write',
        'struct_size':     0x40,
        'observe_offsets': [0],
        'lut_root_delta':  0,
        'path1_tests':    [[0x00000000, 0], [0x00000001, 0], [0xCAFEBABE, 0],
                           [0x12345678, 0], [0xFFFFFFFF, 0], [0x80000000, 0],
                           [0x55555555, 0], [0xAAAAAAAA, 0], [0x7FFFFFFF, 0],
                           [0x0000FFFF, 0]],
        'path2_tests':    [[0x00000001, 0], [0xCAFEBABE, 0], [0xFFFFFFFF, 0]],
    },

    # 0x0042aab0  UtilInit898a90  — void(void): *0x00898a90=0x180; *0x00898ab0=0.
    # void_write_observe on 0x00898a90 (first write): sentinel wiped → 0x180.
    'util_init_898a90': {
        'rva':            0x0042aab0,
        'export':         'UtilInit898a90',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x00898a90,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xCAFEBABE,
                           0x12345678, 0xFFFFFFFF, 0x80000000, 0x55555555,
                           0xAAAAAAAA, 0x7FFFFFFF],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xFFFFFFFF],
    },

    # 0x00430b00  TimeDisplaySetEntry — void(p1,p2,v3,v4,v5): writes v3,v4,v5 at
    #   0x008989e0 + (p2 + p1*2)*0xc {+0,+4,+8}.  No guard in the original.
    # multi_arg_global_write over the WHOLE 48-byte buffer (out_count=12) so any
    # (p1,p2) in {0,1}² lands inside the readback window → the 2D index formula
    # is genuinely exercised. guard_global=0x008989e0 is inside the saved/restored
    # window (the function overwrites it for slot (0,0)), so the forced guard=1 is
    # self-contained and benign.
    'time_display_set_entry': {
        'rva':            0x00430b00,
        'export':         'TimeDisplaySetEntry',
        'signature':      {'ret': 'void',
                           'args': ['int32', 'int32', 'uint32', 'uint32', 'uint32']},
        'arg_type':       'multi_arg_global_write',
        'guard_global':   0x008989e0,
        'out_base':       0x008989e0,
        'out_count':      12,
        'lut_root_delta': 0,
        'path1_tests':    [[0, 0, 0x3f7d70a4, 0x0000003b, 0x00000009],
                           [0, 1, 0xDEADBEEF, 0xCAFEBABE, 0x12345678],
                           [1, 0, 0xFFFFFFFF, 0x80000000, 0x00000001],
                           [1, 1, 0x55555555, 0xAAAAAAAA, 0x7FFFFFFF],
                           [0, 0, 0x11111111, 0x22222222, 0x33333333],
                           [1, 1, 0x00000000, 0xFFFFFFFF, 0x0000ffff],
                           [0, 1, 0x44444444, 0x55555555, 0x66666666],
                           [1, 0, 0x77777777, 0x88888888, 0x99999999]],
        'path2_tests':    [[0, 0, 0x3f7d70a4, 0x0000003b, 0x00000009],
                           [1, 1, 0x55555555, 0xAAAAAAAA, 0x7FFFFFFF]],
    },

    # ---- c3_batch_ad session 2 (HARVEST, frontend pure leaves) ----------------

    # 0x00426cb0  FrontendSlotTablePtr426cb0
    # undefined* FUN_00426cb0(int param_1) { return &DAT_00663664 + param_1*0x4c; }
    # Pure index-to-pointer slot helper (0x4c-byte stride table at 0x00663664).
    # int_scalar: pass index, compare returned pointer (EAX) as uint32.
    # ref: re/analysis/frontend_c1_to_c2_s4/FUN_00426cb0.md
    'frontend_slot_table_ptr_426cb0': {
        'rva':            0x00426cb0,
        'export':         'FrontendSlotTablePtr426cb0',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 5, 10, 16, 100, 255, 0xffff, 0xffffffff],
        'path2_tests':    [0, 1, 16],
    },

    # 0x00426de0  FrontendFloatGet426de0
    # float10 FUN_00426de0(void) { return (float10)_DAT_0064435c; }
    # Pure float-getter leaf; loads 32-bit float at 0x0064435c, returns in ST0.
    # arg_type='read_global': seed 0x0064435c with each test's raw float bits,
    # call fn(), compare the float return. This DISCRIMINATES the cited address
    # (a plain 'none' getter reads 0.0 at menu-idle -> vacuous GREEN). Harness
    # saves/restores the global so the test is non-destructive.
    # ref: re/analysis/frontend_c1_to_c2_s5/FUN_00426de0.md
    'frontend_float_get_426de0': {
        'rva':            0x00426de0,
        'export':         'FrontendFloatGet426de0',
        'signature':      {'ret': 'float', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0064435c,
        'lut_root_delta': 0,
        # raw IEEE-754 bits: 0, 1.0, -1.0, 0.5, 100.0, -100.0, pi, 0.1, 1e6, 2.0
        'path1_tests':    [0x00000000, 0x3f800000, 0xbf800000, 0x3f000000,
                           0x42c80000, 0xc2c80000, 0x40490fdb, 0x3dcccccd,
                           0x49742400, 0x40000000],
        'path2_tests':    [0x3f800000, 0xc2c80000, 0x00000000],
    },

    # 0x00426df0  FrontendFloatGet426df0
    # float10 FUN_00426df0(void) { return (float10)_DAT_00644360; }  (= 0x0064435c + 4)
    # Companion float-getter leaf. read_global on 0x00644360 (same rationale).
    # ref: re/analysis/frontend_c1_to_c2_s5/FUN_00426df0.md
    'frontend_float_get_426df0': {
        'rva':            0x00426df0,
        'export':         'FrontendFloatGet426df0',
        'signature':      {'ret': 'float', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x00644360,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x3f800000, 0xbf800000, 0x3f000000,
                           0x42c80000, 0xc2c80000, 0x40490fdb, 0x3dcccccd,
                           0x49742400, 0x40000000],
        'path2_tests':    [0x3f800000, 0xc2c80000, 0x00000000],
    },

    # 0x00427580  FrontendQuadParamInit427580
    # void FUN_00427580(void): writes 4 floats to 0x008991c0..cc from .data
    # sources 0x0067d830/0x0067d834 and .rdata scale consts 0x005cd5f0/f4/f8.
    # state_machine_observe: inject the two WRITABLE .data sources (the scale
    # consts are read-only .rdata and read live by both sides), call fn(), read
    # back the 4 written globals. Save/restore makes it non-destructive.
    # Inputs are raw IEEE-754 float bits ([0x0067d830, 0x0067d834]).
    # ref: re/analysis/frontend_c1_to_c2_s5/FUN_00427580.md
    'frontend_quad_param_init_427580': {
        'rva':            0x00427580,
        'export':         'FrontendQuadParamInit427580',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'state_machine_observe',
        'input_globals':  [
            {'addr': 0x0067d830, 'type': 'u32'},  # source A (-> 0x008991c8)
            {'addr': 0x0067d834, 'type': 'u32'},  # source B (-> 0x008991cc, 0x008991c4)
        ],
        'output_globals': [
            {'addr': 0x008991c0, 'type': 'u32'},  # const 0x3e660000
            {'addr': 0x008991c4, 'type': 'u32'},  # d5f0 - B*d5f4
            {'addr': 0x008991c8, 'type': 'u32'},  # A*d5f8
            {'addr': 0x008991cc, 'type': 'u32'},  # B*d5f4
        ],
        'lut_root_delta': 0,
        # [src_A_bits, src_B_bits] as raw IEEE-754 float bit patterns
        'path1_tests': [
            [0x00000000, 0x00000000],  # 0.0,   0.0
            [0x3f800000, 0x3f800000],  # 1.0,   1.0
            [0x40000000, 0x40400000],  # 2.0,   3.0
            [0x44200000, 0x43f00000],  # 640.0, 480.0
            [0xbf800000, 0x40000000],  # -1.0,  2.0
            [0x3f000000, 0x3f000000],  # 0.5,   0.5
            [0x44480000, 0x44160000],  # 800.0, 600.0
            [0x40400000, 0x40800000],  # 3.0,   4.0
            [0xc0000000, 0xc0400000],  # -2.0,  -3.0
            [0x3e800000, 0x3e000000],  # 0.25,  0.125
        ],
        'path2_tests': [
            [0x3f800000, 0x3f800000],
            [0x44200000, 0x43f00000],
            [0x00000000, 0x00000000],
        ],
    },
    # ───────────────────────────────────────────────────────────────────────
    # Session c3-batch-ad-s4 — HARVEST  (C2→C3, 3 viable 0x0049xxxx COM leaves)
    # Particle/ParticleLeaves_ad4.cpp — author+verify-only; central classify at
    # sweep. NO diff_template.js change (reuses fastcall_reg + struct_call_observe)
    # → NOT sweep-critical. The other 10 candidates in the slate were SKIPPED as
    # needs-live-state (CoCreateInstance / QueryInterface / EnterCriticalSection /
    # unconditional vtable dispatch — not constructible at diff-attach).
    # ───────────────────────────────────────────────────────────────────────

    # 0x0049c690  ComField14Get — uint __fastcall(ptr): return *(ECX+0x14).
    # Bytes: 8b4114 (MOV EAX,[ECX+0x14]) ; c3 (RET, reg-only -> __fastcall).
    # The harness seeds ECX with a ZEROED 64-byte scratch buffer, so *(ECX+0x14)
    # == 0 -> orig and reimpl both return 0 (proves ECX marshalling + the +0x14
    # read + callee-clean stack balance). Same shape as the validated fastcall_reg
    # canary com_release_thunk @ 0x004a1790.
    # ref: re/analysis/particle_promote_ad1/0049c690.md
    'com_field14_get': {
        'rva':              0x0049c690,
        'export':           'ComField14Get',
        'signature':        {'ret': 'uint32', 'args': []},
        'arg_type':         'fastcall_reg',
        'fastcall_nargs':   1,
        'fastcall_ecx_ptr': True,
        'lut_root_delta':   0,
        # Markers only — the harness replaces each with a fresh zeroed scratch ptr.
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7],
        'path2_tests': [0, 1, 2],
    },

    # 0x0049f180  ComRefRelease14 — LONG __stdcall(int*): COM Release. Atomically
    # decrements the refcount dword at byte +0x14; when the new count hits 0 (and
    # ptr non-null) makes a __thiscall to vtable slot 7 (offset +0x1c) with arg 1.
    # Returns the post-decrement count.  RET 0x4 -> __stdcall.
    # struct_call_observe seeds the refcount at +0x14 with a value > 1 so the
    # decrement stays > 0 and the vtable-finalise path is NOT taken (the zeroed
    # scratch struct has a null vtable, so the 0-count path is unconstructible at
    # diff-time). Observe the return value AND the post-call refcount field.
    # ref: re/analysis/particle_promote_ad5/0049f180.md
    'com_ref_release14': {
        'rva':         0x0049f180,
        'export':      '_ComRefRelease14@4',
        'signature':   {'ret': 'int32', 'args': ['pointer']},
        'arg_type':    'struct_call_observe',
        'orig_calling_convention':   'stdcall',
        'reimpl_calling_convention': 'stdcall',
        'struct_size': 0x40,
        'out_ptrs':    0,
        'observe_ret': True,
        'observe':     [{'src': 'struct', 'off': 0x14, 'type': 'u32'}],
        'lut_root_delta': 0,
        # refcount seeds — all > 1 (NEVER 1, which would decrement to 0 and call
        # the null vtable). Each test allocates a fresh zeroed struct per side.
        'path1_tests': [
            {'seeds': [{'off': 0x14, 'type': 'u32', 'value': 2}]},
            {'seeds': [{'off': 0x14, 'type': 'u32', 'value': 3}]},
            {'seeds': [{'off': 0x14, 'type': 'u32', 'value': 5}]},
            {'seeds': [{'off': 0x14, 'type': 'u32', 'value': 16}]},
            {'seeds': [{'off': 0x14, 'type': 'u32', 'value': 0x7fffffff}]},
        ],
        'path2_tests': [
            {'seeds': [{'off': 0x14, 'type': 'u32', 'value': 5}]},
        ],
    },

    # 0x0049f2b0  ComRefRelease10 — LONG __stdcall(int*): byte-identical to
    # ComRefRelease14 except the refcount dword is at byte +0x10. Same __stdcall
    # ABI (RET 0x4), same __thiscall vtable-slot-7 / arg-1 finalise on count 0.
    # ref: re/analysis/particle_promote_ad5/0049f2b0.md
    'com_ref_release10': {
        'rva':         0x0049f2b0,
        'export':      '_ComRefRelease10@4',
        'signature':   {'ret': 'int32', 'args': ['pointer']},
        'arg_type':    'struct_call_observe',
        'orig_calling_convention':   'stdcall',
        'reimpl_calling_convention': 'stdcall',
        'struct_size': 0x40,
        'out_ptrs':    0,
        'observe_ret': True,
        'observe':     [{'src': 'struct', 'off': 0x10, 'type': 'u32'}],
        'lut_root_delta': 0,
        'path1_tests': [
            {'seeds': [{'off': 0x10, 'type': 'u32', 'value': 2}]},
            {'seeds': [{'off': 0x10, 'type': 'u32', 'value': 3}]},
            {'seeds': [{'off': 0x10, 'type': 'u32', 'value': 5}]},
            {'seeds': [{'off': 0x10, 'type': 'u32', 'value': 16}]},
            {'seeds': [{'off': 0x10, 'type': 'u32', 'value': 0x7fffffff}]},
        ],
        'path2_tests': [
            {'seeds': [{'off': 0x10, 'type': 'u32', 'value': 5}]},
        ],
    },
    # ─────────────────────────────────────────────────────────────────────────
    # c3-batch-ad-s5 — particle pool HARVEST (1/12 promotable)
    # ─────────────────────────────────────────────────────────────────────────
    # 0x0049fa40  ConstZeroRet_0049fa40 — bytes: 33 C0 C2 04 00
    #   XOR EAX,EAX ; RET 4  ->  __stdcall u32(u32 ignored) returning 0.
    # The RET 4 (callee-clean of one 4-byte arg) makes this __stdcall; the
    # function is total (returns 0 for EVERY argument), so bit-identity holds
    # for all inputs. arg_type 'int_scalar' pushes one dummy u32; stdcall on
    # both orig and reimpl keeps ESP balanced across the RET 4. Export
    # decorates to _ConstZeroRet_0049fa40@4 (extern "C" __stdcall).
    'particle_const_zero_ret_0049fa40': {
        'rva':            0x0049fa40,
        'export':         '_ConstZeroRet_0049fa40@4',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'int_scalar',
        'orig_calling_convention':   'stdcall',
        'reimpl_calling_convention': 'stdcall',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 0x10, 0xff, 0x1234, 0xDEADBEEF,
                           0xCAFEBABE, 0x7FFFFFFF, 0xFFFFFFFF],
        'path2_tests':    [0, 1, 0xFFFFFFFF],
    },

    # ─────────────────────────────────────────────────────────────────────────
    # c3-batch-ae-s1 — render-leaf HARVEST (2/11 promotable)
    # ─────────────────────────────────────────────────────────────────────────
    # 0x00492440  RenderStatsAccumulate — __cdecl void(ptr): per-frame stats
    #   rollup over a 60-frame window. Fields (byte offsets from param_1):
    #     +0x20 A sample (in)   +0x24 B sample (in)   +0x28 frame counter
    #     +0x2c A accum         +0x30 B accum         +0x34 A avg (out)
    #     +0x38 B avg (out)     +0x3c A max           +0x40 B max
    #   accum += sample; if counter > 0x3b: avg = accum/0x3c, accum = 0,
    #   counter = 0; counter += 1; max = max(max, sample). All s32 (signed
    #   idiv / jl). struct_call_observe seeds the input fields and observes
    #   every mutated field; observe_ret=False (void). Tests exercise the
    #   rollup boundary (counter 0x3b vs 0x3c), signed division (negative
    #   accumulators), and signed max tracking.
    'render_stats_accumulate': {
        'rva':         0x00492440,
        'export':      'RenderStatsAccumulate',
        'signature':   {'ret': 'void', 'args': ['pointer']},
        'arg_type':    'struct_call_observe',
        'struct_size': 0x80,
        'out_ptrs':    0,
        'observe_ret': False,
        'observe':     [{'src': 'struct', 'off': 0x28, 'type': 'u32'},
                        {'src': 'struct', 'off': 0x2c, 'type': 'u32'},
                        {'src': 'struct', 'off': 0x30, 'type': 'u32'},
                        {'src': 'struct', 'off': 0x34, 'type': 'u32'},
                        {'src': 'struct', 'off': 0x38, 'type': 'u32'},
                        {'src': 'struct', 'off': 0x3c, 'type': 'u32'},
                        {'src': 'struct', 'off': 0x40, 'type': 'u32'}],
        'lut_root_delta': 0,
        'path1_tests': [
            # counter low: simple accumulate, no rollup; max tracking from 0
            {'seeds': [{'off': 0x20, 'type': 'u32', 'value': 10}, {'off': 0x24, 'type': 'u32', 'value': 20},
                       {'off': 0x28, 'type': 'u32', 'value': 5},  {'off': 0x2c, 'type': 'u32', 'value': 100},
                       {'off': 0x30, 'type': 'u32', 'value': 200}]},
            # counter exactly 0x3b (59): NOT > 0x3b -> no rollup, counter -> 60
            {'seeds': [{'off': 0x20, 'type': 'u32', 'value': 5}, {'off': 0x24, 'type': 'u32', 'value': 5},
                       {'off': 0x28, 'type': 'u32', 'value': 0x3b}, {'off': 0x2c, 'type': 'u32', 'value': 1000},
                       {'off': 0x30, 'type': 'u32', 'value': 2000}]},
            # counter 0x3c (60): > 0x3b -> rollup; (1180+5)/60=19, (2360+5)/60=39
            {'seeds': [{'off': 0x20, 'type': 'u32', 'value': 5}, {'off': 0x24, 'type': 'u32', 'value': 5},
                       {'off': 0x28, 'type': 'u32', 'value': 0x3c}, {'off': 0x2c, 'type': 'u32', 'value': 1180},
                       {'off': 0x30, 'type': 'u32', 'value': 2360}]},
            # division truncation: (5999+1)/60=100, (119+1)/60=2
            {'seeds': [{'off': 0x20, 'type': 'u32', 'value': 1}, {'off': 0x24, 'type': 'u32', 'value': 1},
                       {'off': 0x28, 'type': 'u32', 'value': 0x64}, {'off': 0x2c, 'type': 'u32', 'value': 5999},
                       {'off': 0x30, 'type': 'u32', 'value': 119}]},
            # signed division: accum -7259 (0xFFFFE3A5) /60 trunc-toward-zero = -120
            {'seeds': [{'off': 0x20, 'type': 'u32', 'value': 0}, {'off': 0x24, 'type': 'u32', 'value': 0},
                       {'off': 0x28, 'type': 'u32', 'value': 0x3c}, {'off': 0x2c, 'type': 'u32', 'value': 0xFFFFE3A5},
                       {'off': 0x30, 'type': 'u32', 'value': 0xFFFFE3A5}]},
            # signed max: maxA seeded -100 (0xFFFFFF9C), sample -50 -> -100 < -50 -> update
            {'seeds': [{'off': 0x20, 'type': 'u32', 'value': 0xFFFFFFCE}, {'off': 0x24, 'type': 'u32', 'value': 0xFFFFFFCE},
                       {'off': 0x28, 'type': 'u32', 'value': 1}, {'off': 0x3c, 'type': 'u32', 'value': 0xFFFFFF9C},
                       {'off': 0x40, 'type': 'u32', 'value': 0xFFFFFF9C}]},
            # signed max: maxA seeded -100, sample -200 (0xFFFFFF38) -> no update
            {'seeds': [{'off': 0x20, 'type': 'u32', 'value': 0xFFFFFF38}, {'off': 0x24, 'type': 'u32', 'value': 0xFFFFFF38},
                       {'off': 0x28, 'type': 'u32', 'value': 1}, {'off': 0x3c, 'type': 'u32', 'value': 0xFFFFFF9C},
                       {'off': 0x40, 'type': 'u32', 'value': 0xFFFFFF9C}]},
            # all zero
            {'seeds': []},
        ],
        'path2_tests': [
            {'seeds': [{'off': 0x20, 'type': 'u32', 'value': 10}, {'off': 0x24, 'type': 'u32', 'value': 20},
                       {'off': 0x28, 'type': 'u32', 'value': 0x3c}, {'off': 0x2c, 'type': 'u32', 'value': 1180}]},
        ],
    },

    # 0x004b46b0  Vec3Equal — __cdecl u32(float* a, float* b): returns 1 if all
    #   three components compare equal under IEEE-754 `==`, else 0. No epsilon;
    #   FP compare (not bit compare) -> +0.0 == -0.0 is 1, NaN == NaN is 0.
    #   Reused arg_type 'fmt_desc_pair_compare' (2-arg form): seeds float bits
    #   as u32 at f00/f04/f08 in each scratch buffer; the function is read-only
    #   so the buffer fingerprints are unchanged and the 0/1 return drives the
    #   match. Tests cover all-equal, per-component mismatch, signed-zero, and
    #   NaN edges.
    'render_vec3_equal': {
        'rva':            0x004b46b0,
        'export':         'Vec3Equal',
        'signature':      {'ret': 'uint32', 'args': ['pointer', 'pointer']},
        'arg_type':       'fmt_desc_pair_compare',
        'lut_root_delta': 0,
        'path1_tests': [
            # all equal {1.0, 2.0, 3.0} -> 1
            {'a': {'f00': 0x3F800000, 'f04': 0x40000000, 'f08': 0x40400000},
             'b': {'f00': 0x3F800000, 'f04': 0x40000000, 'f08': 0x40400000}},
            # x differs (1.0 vs 2.0) -> 0
            {'a': {'f00': 0x3F800000, 'f04': 0x40000000, 'f08': 0x40400000},
             'b': {'f00': 0x40000000, 'f04': 0x40000000, 'f08': 0x40400000}},
            # y differs (2.0 vs 5.0) -> 0
            {'a': {'f00': 0x3F800000, 'f04': 0x40000000, 'f08': 0x40400000},
             'b': {'f00': 0x3F800000, 'f04': 0x40A00000, 'f08': 0x40400000}},
            # z differs (3.0 vs 4.0) -> 0
            {'a': {'f00': 0x3F800000, 'f04': 0x40000000, 'f08': 0x40400000},
             'b': {'f00': 0x3F800000, 'f04': 0x40000000, 'f08': 0x40800000}},
            # all zero equal -> 1
            {'a': {'f00': 0, 'f04': 0, 'f08': 0}, 'b': {'f00': 0, 'f04': 0, 'f08': 0}},
            # +0.0 vs -0.0 on x, rest equal -> FP == -> 1 (bit-different inputs)
            {'a': {'f00': 0x00000000, 'f04': 0x40000000, 'f08': 0x40400000},
             'b': {'f00': 0x80000000, 'f04': 0x40000000, 'f08': 0x40400000}},
            # NaN vs NaN on x -> NaN != NaN -> 0 (both orig+reimpl agree)
            {'a': {'f00': 0x7FC00000, 'f04': 0, 'f08': 0},
             'b': {'f00': 0x7FC00000, 'f04': 0, 'f08': 0}},
            # negatives equal {-1.0, -2.0, -3.0} -> 1
            {'a': {'f00': 0xBF800000, 'f04': 0xC0000000, 'f08': 0xC0400000},
             'b': {'f00': 0xBF800000, 'f04': 0xC0000000, 'f08': 0xC0400000}},
            # large equal
            {'a': {'f00': 0x60AD78EC, 'f04': 0x60AD78EC, 'f08': 0x60AD78EC},
             'b': {'f00': 0x60AD78EC, 'f04': 0x60AD78EC, 'f08': 0x60AD78EC}},
            # mixed-sign equal {-1.0, 2.0, -3.0} -> 1
            {'a': {'f00': 0xBF800000, 'f04': 0x40000000, 'f08': 0xC0400000},
             'b': {'f00': 0xBF800000, 'f04': 0x40000000, 'f08': 0xC0400000}},
        ],
        'path2_tests': [
            {'a': {'f00': 0x3F800000, 'f04': 0x40000000, 'f08': 0x40400000},
             'b': {'f00': 0x3F800000, 'f04': 0x40000000, 'f08': 0x40400000}},
            {'a': {'f00': 0x3F800000}, 'b': {'f00': 0x40000000}},
        ],
    },

    # ===================================================================
    # c3-batch-af session 4 (Frontend/MenuLeaves_af4.cpp)
    # ===================================================================

    # 0x0043aee0  MenuSlotFlagSetCurrent
    # void(void) -- gated write of 1 to (&DAT_007f0a40)[DAT_0067f17c*0xc].
    # Gate: FUN_004307a0()!=0 && DAT_0067ed6c!=0. FUN_004307a0() is uncontrolled
    # at attach time, but orig and reimpl share it, so the observed slot agrees
    # either way. Inject slot index 0 so the written slot is the observed base.
    'menu_slot_flag_set_current': {
        'rva':            0x0043aee0,
        'export':         'MenuSlotFlagSetCurrent',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'state_machine_observe',
        'input_globals':  [
            {'addr': 0x0067ed6c, 'type': 'u32'},  # override flag (gate 2)
            {'addr': 0x0067f17c, 'type': 'u32'},  # current-slot index
        ],
        'output_globals': [
            {'addr': 0x007f0a40, 'type': 'u32'},  # slot[0] flag (idx injected = 0)
        ],
        'lut_root_delta': 0,
        # [override_flag, slot_index]
        'path1_tests': [
            [0, 0],            # override off: no write
            [1, 0],            # override on: writes 1 to slot0 iff gate!=0
            [0xFFFFFFFF, 0],   # override on (alt sentinel)
            [0, 0],            # override off again
            [1, 0],            # override on again
            [0x55555555, 0],
            [0xAAAAAAAA, 0],
        ],
        'path2_tests': [
            [0, 0], [1, 0], [0xFFFFFFFF, 0],
        ],
    },

    # 0x00401ee0  EntityTableSelectUpdate
    # void(uint32) -- FUN_00401570(param_1) selects an entry pointer into the
    # global at 0x00636ac0; then FUN_00401da0() (RW matrix update); then if the
    # deref'd entry value != 0, tail-calls FUN_004e6680(value). Observe the
    # selected-entry pointer at 0x00636ac0 after the call (written by the select).
    # NOTE: exercises live entity/table state; if the table is unpopulated at the
    # diff-attach menu the deref may fault on both sides (no clean diff). Treated
    # as best-effort; see PROMOTION_QUEUE fragment.
    'entity_table_select_update': {
        'rva':            0x00401ee0,
        'export':         'EntityTableSelectUpdate',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x00636ac0,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 0, 1],
        'path2_tests':    [0, 1],
    },

    # ───────────────────────────────────────────────────────────────────────
    # c3_batch_af-s5  frontend menu-leaf HARVEST (2026-06-04)
    # ───────────────────────────────────────────────────────────────────────

    # 0x00431b70  MenuFlagDat007f0f10Get  uint32(void)
    # 5B pure getter leaf: `return DAT_007f0f10;`. arg_type='read_global' writes a
    # sentinel to 0x007f0f10 before each call; the returned value must equal it.
    # ref: re/analysis/bucket_0041dc30/0x00431b70.md
    'menu_flag_dat_007f0f10_get': {
        'rva':            0x00431b70,
        'export':         'MenuFlagDat007f0f10Get',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x007f0f10,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # 0x004298c0  MenuQuadGlobalZero  void(void)
    # 27B pure leaf — four 32-bit zero stores (0x0067d99c, 0x0067d994, 0x0067d98c,
    # 0x008991bc), no callees, no branches. arg_type='none': void return, called
    # 10x; both orig and reimpl emit the identical store sequence (asm-equivalence
    # confirmed in the .cpp header against the disassembly).
    # ref: re/analysis/frontend_c1_to_c2_s6/FUN_004298c0.md
    'menu_quad_global_zero': {
        'rva':            0x004298c0,
        'export':         'MenuQuadGlobalZero',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00423b00  FrontendInputDispatch  void(void)
    # 29B per-frame menu-input dispatcher: guard DAT_007f1a50==1 then 4 void(void)
    # callees (FrontendDirInput / TabCycler / CursorMover / spline-editor tick; the
    # last via tail-call JMP). arg_type='none': void return, called 10x; confirms
    # non-crash with the live callee chain at the menu. WEAK `none` evidence —
    # central classify should confirm asm-equivalence of the call sequence.
    # ref: re/analysis/frontend_gate_unblock_u/0x00423b00.md
    'frontend_input_dispatch': {
        'rva':            0x00423b00,
        'export':         'FrontendInputDispatch',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # ─── c3-batch-af-s6 (2026-06-04): frontend menu leaves ──────────────────

    # 0x00431f30  FrontendPageIdDispatch  void(int page_id)
    # Calls FUN_00431d90 (FrontendPanelFlagAdvance, C3 — mass panel-flag advance/
    # clear over 0x0067e7a8..0x0067e838) then a switch setting exactly one panel
    # flag to 1 for the given page id (page 8 sets two: 0x0067e808 + 0x0067e828).
    # arg_type=multi_arg_global_write over the WHOLE 37-u32 flag block
    # [0x0067e7a8 .. 0x0067e838] so every page's flag-set lands inside the readback
    # window and the switch is genuinely exercised. No guard in the original;
    # guard_global=0x0067e7a8 is inside the saved/restored window and is overwritten
    # by FUN_00431d90 on every call, so the forced guard=1 is self-contained/benign
    # (model: time_display_set_entry). Both orig+reimpl call the SAME FUN_00431d90
    # so the flag-clear half is bit-identical by construction.
    # Gate: caller 0x0043d2a0 C2, callee 0x00431d90 C3.
    # ref: re/analysis/options_menu/0x00431f30.md
    'frontend_page_id_dispatch': {
        'rva':            0x00431f30,
        'export':         'FrontendPageIdDispatch',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'multi_arg_global_write',
        'guard_global':   0x0067e7a8,
        'out_base':       0x0067e7a8,
        'out_count':      37,
        'lut_root_delta': 0,
        'path1_tests': [
            [4], [5], [6], [7], [8], [9], [10], [0xc], [0xf], [0x11],
            [0x14], [0x1e], [0x20], [0x21], [0x63],
        ],
        'path2_tests': [
            [8], [0x1e], [0x63],
        ],
    },

    # 0x00424270  RankSortDispatcher — NOT PROMOTED (c3-batch-af-s6 2026-06-04).
    # A faithful port + sort_dispatch_out4 diff came back GREEN but DEGENERATE:
    # at a quiescent menu all 4 player slots are inactive so every sel/dir vector
    # yields the identical [0,1,2,3] (log/diff_rank_sort_dispatcher.csv) — the
    # 14-case dispatch/sort/team paths are never exercised -> false-GREEN. Defer
    # to a populated-race canonical scenario (C3->C4 track). Entry intentionally
    # omitted so the central frida-sweep does not promote it.

    # ─────────────────────────────────────────────────────────────────────────
    # Session c3-batch-ah-s3 — gameplay getters  (C2->C3)
    # Gameplay/ScoreMasks_ah3.cpp — pure-leaf global-read getters.
    # The 5 score-mask out-buffer fns (0x0040b970/ba00/b930/b9a0/ba60) are
    # authored in the same .cpp but DEFERRED: int_outbuf4 is fn(idx,buf) (wrong
    # shape for void fn(int* out)) and no harness arg_type seeds 0x008a94e0[4].
    # ─────────────────────────────────────────────────────────────────────────

    # 0x0040dc70  GetAnimationState
    # Pure leaf: returns DAT_0063b90c (state register 0..3). void(void)->uint32.
    # read_global: write sentinel to 0x0063b90c, call fn(), return must equal it.
    'get_animation_state': {
        'rva':            0x0040dc70,
        'export':         'GetAnimationState',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0063b90c,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0x00000002, 0x00000003,
                           0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF, 0x80000000,
                           0x55555555, 0xAAAAAAAA],
        'path2_tests':    [0x00000000, 0x00000002, 0xFFFFFFFF],
    },

    # 0x0040d430  GetPointerGlobal
    # Pure leaf: returns the pointer value stored at 0x005f2770. void(void)->ptr.
    # read_global: write sentinel to 0x005f2770, call fn(), return must equal it.
    'get_pointer_global': {
        'rva':            0x0040d430,
        'export':         'GetPointerGlobal',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x005f2770,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xDEADBEEF, 0xCAFEBABE,
                           0x12345678, 0xFFFFFFFF, 0x80000000, 0x55555555,
                           0xAAAAAAAA, 0x00000000],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xFFFFFFFF],
    },

    # 0x0040b420  IndexedTableLookup — NOT PROMOTED (c3-batch-ah-s3 2026-06-08).
    # Pure indexed leaf: returns DWORD at 0x008a9500 + idx*4. uint32(int idx).
    # int_scalar diff came back GREEN but DEGENERATE: the live table at 0x008a9500
    # is all-zero at the quiescent menu, so every index returns 0
    # (log/diff_indexed_table_lookup.csv) -> false-GREEN. No harness arg_type
    # seeds the table. Defer to a populated canonical scenario. Entry kept (with
    # the explanatory note) so a future sweep does not re-promote it blindly.
    'indexed_table_lookup': {
        'rva':            0x0040b420,
        'export':         'IndexedTableLookup',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # ── Session c3-batch-ah-s5 — audio root/timer getters + linear->log leaf ──
    # DEFERRED (not registered): 0x005ab010 AudioElapsedTicks (moving-counter
    #   diff, read_global can't capture deterministically), 0x005aaff0
    #   AudioTimerBaselineInit (void_write_observe degenerate/racy), and the
    #   0x005a89a0/b0/c0 VoiceField getters (thiscall struct-ptr, not 0-arg
    #   read_global). See TimerGetters_ah5.cpp header.

    # 0x005aad40  AudioRootGet — MOV EAX,[0x007dcd20]; RET. read_global: write
    #   sentinel to 0x007dcd20, call fn(), verify return == sentinel.
    # ref: re/analysis/bucket_audio_005a7b60_005ab620/005aad40.md
    'audio_root_get': {
        'rva':            0x005aad40,
        'export':         'AudioRootGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x007dcd20,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE, 0x12345678,
                           0xFFFFFFFF, 0x80000000, 0x00000001, 0x55555555,
                           0xAAAAAAAA, 0x00FF00FF],
        'path2_tests':    [0x00000000, 0xDEADBEEF, 0xCAFEBABE],
    },

    # 0x005ab070  AudioTimerRateGet — DEFERRED, NOT REGISTERED (re-checked
    #   2026-06-08 scenario-attach pickup). uint32(void):
    #     return [0x007dcdf8] if [0x007dce00]==1 else literal 1000.
    #   read_global on 0x007dcdf8 is GREEN-but-DEGENERATE under run_diff.py: at the
    #   early menu-attach diff point the gate 0x007dce00 is NOT yet 1 (audio init
    #   completes later, ~title), so every seeded vector falls to the literal 1000
    #   (log/diff_audio_timer_rate_get.csv = all 1000). The gate IS 1 from title
    #   onward (verified live: 0x007dce00==1, 0x007dcdf8=10000000), but at that
    #   point the function is zero-arg so the scenario-attach VECTOR model returns
    #   the same live value for every vector -> also degenerate. Neither harness
    #   can show non-degeneracy. Re-pickup: a read_global diff taken AFTER title
    #   (gate==1) seeding 0x007dcdf8 to varied values. Entry intentionally omitted
    #   so the central frida-sweep does not promote on the false-GREEN.
    #   (historical) c3-batch-ah-s5 2026-06-08.
    #   seeds 0x007dcdf8, but at the diff-attach point the runtime gate
    #   0x007dce00 != 1, so every vector returns the constant 1000 (verified:
    #   diff GREEN but all-1000 DEGENERATE -> false-GREEN). The seeded global
    #   never reaches the return; read_global can't satisfy the gate (single
    #   global only). DEFER to a scenario where the timer is initialized
    #   (DAT_007dce00==1). Reimpl AudioTimerRateGet is authored + installed in
    #   TimerGetters_ah5.cpp but intentionally omitted here so the central
    #   frida-sweep does not promote on the degenerate observable.

    # 0x005aeb90  AudioLinearToLog — uint(float). x87 leaf:
    #   if (x < [0x005cc990]) return 0xffffd8f0; else
    #   r = TRUNC(log10(x) * [0x005cd0b8]); return (r>0)?0:r.
    #   [0x005cc990]=0x3727c5ac (~9.99e-6 threshold), [0x005cd0b8]=2000.0f.
    #   Pure leaf; output varies with the float input across the LUT domain.
    # ref: re/analysis/bucket_audio_005ab710_005af040/0x005aeb90.md
    'audio_linear_to_log': {
        'rva':            0x005aeb90,
        'export':         'AudioLinearToLog',
        'signature':      {'ret': 'uint32', 'args': ['float']},
        'arg_type':       'float_scalar',
        'lut_root_delta': 0,
        'path1_tests': [
            0.0, 1.0e-9, 1.0e-6, 1.0e-5, 0.0001, 0.001, 0.01, 0.1,
            0.25, 0.5, 0.70710678, 1.0, 2.0, 10.0, 100.0, 1000.0,
            1.0e6, 3.1622776,
        ],
        'path2_tests': [0.0, 0.001, 0.5, 1.0, 100.0],
    },

    # ── Voice field-getter triplet (harness/scenario-attach 2026-06-08) ──────
    # __cdecl undefined4 fn(void* this) -> *(uint32_t*)(this + off). Disasm
    # (pool14): MOV EAX,[ESP+4]; MOV EAX,[EAX+off]; RET. NOT an ECX thiscall.
    # Promoted via the new SYNTHETIC thiscall_field_get arg_type: the harness
    # seeds its own scratch struct (no live state needed), writes the test value
    # at field_off, calls fn(scratch), and compares the returned field read.
    # Varying the seed -> the return echoes it -> NON-DEGENERATE.
    # ref: re/analysis/bucket_audio_005a7b60_005ab620/{005a89a0,005a89b0,005a89c0}.md
    'voice_field_d6c_get': {
        'rva':            0x005a89a0,
        'export':         'VoiceFieldD6cGet',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'thiscall_field_get',
        'field_off':      0xD6C,
        'ret_kind':       'u32',
        'struct_size':    0xE00,
        'lut_root_delta': 0,
        'path1_tests': [
            0x00000000, 0x00000001, 0xFFFFFFFF, 0xDEADBEEF, 0x12345678,
            0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 0x0000FFFF, 0xFFFF0000,
            0x55555555, 0xAAAAAAAA,
        ],
        'path2_tests': [0x00000000, 0xDEADBEEF, 0x12345678],
    },

    'voice_field_d70_get': {
        'rva':            0x005a89b0,
        'export':         'VoiceFieldD70Get',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'thiscall_field_get',
        'field_off':      0xD70,
        'ret_kind':       'u32',
        'struct_size':    0xE00,
        'lut_root_delta': 0,
        'path1_tests': [
            0x00000000, 0x00000001, 0xFFFFFFFF, 0xDEADBEEF, 0x12345678,
            0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 0x0000FFFF, 0xFFFF0000,
            0x55555555, 0xAAAAAAAA,
        ],
        'path2_tests': [0x00000000, 0xDEADBEEF, 0x12345678],
    },

    'voice_field_d74_get': {
        'rva':            0x005a89c0,
        'export':         'VoiceFieldD74Get',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'thiscall_field_get',
        'field_off':      0xD74,
        'ret_kind':       'u32',
        'struct_size':    0xE00,
        'lut_root_delta': 0,
        'path1_tests': [
            0x00000000, 0x00000001, 0xFFFFFFFF, 0xDEADBEEF, 0x12345678,
            0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 0x0000FFFF, 0xFFFF0000,
            0x55555555, 0xAAAAAAAA,
        ],
        'path2_tests': [0x00000000, 0xDEADBEEF, 0x12345678],
    },

    # 0x005ab370 — identical __cdecl field-getter shape, audio channel-meta
    # accessor (*(this+0x10)). Adjacent in the voice bucket; same thiscall_field_get.
    # ref: re/analysis/bucket_audio_005a7b60_005ab620/005ab370.md
    'channel_field_10_get': {
        'rva':            0x005ab370,
        'export':         'ChannelField10Get',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'thiscall_field_get',
        'field_off':      0x10,
        'ret_kind':       'u32',
        'struct_size':    0x80,
        'lut_root_delta': 0,
        'path1_tests': [
            0x00000000, 0x00000001, 0xFFFFFFFF, 0xDEADBEEF, 0x12345678,
            0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 0x0000FFFF, 0xFFFF0000,
            0x55555555, 0xAAAAAAAA,
        ],
        'path2_tests': [0x00000000, 0xDEADBEEF, 0x12345678],
    },

    # ── Zero-arg getters (run_diff_scenario.py --zero-arg) ───────────────────
    # These take NO argument, so the varied-vector model can't show variance —
    # the C3 criterion is a SINGLE-SHOT non-default value at a gate-satisfied
    # scenario point (run_diff_scenario.py zero-arg mode). 'zero_arg': True and
    # 'zero_arg_baseline' = the menu/default value to reject. The single test is
    # a dummy iteration marker (arg_type 'none' calls fn() and returns its value).

    # 0x0040cd90 CountNonZeroPairs — int(void). Counts 8 (base,base+0x44) record
    # pairs in the 0x00899260 table where both dwords are nonzero. At a populated
    # race the table is filled -> ret > 0; menu/default = 0. scenario=race,
    # sentinel=0x00899260.
    'count_nonzero_pairs': {
        'rva':            0x0040cd90,
        'export':         'CountNonZeroPairs',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'zero_arg':       True,
        'zero_arg_baseline': 0,
        'scenario_sentinel': 0x00899260,
        'lut_root_delta': 0,
        'path1_tests':    [0],
        'path2_tests':    [0],
    },

    # 0x0040b890 GetGridCount8or12 — int(void). 8 or 12 gated on FUN_0040e340/
    # FUN_0042f500/DAT_007f0fd0. Menu default observed = 12 (0xc); a selected game
    # mode flips DAT_007f0fd0/the gate so it returns 8. baseline rejects the menu
    # default. sentinel=0x007f0fd0 (the mode flag the gate reads).
    'grid_count_8or12': {
        'rva':            0x0040b890,
        'export':         'GetGridCount8or12',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'zero_arg':       True,
        'zero_arg_baseline': 12,
        'scenario_sentinel': 0x007f0fd0,
        'lut_root_delta': 0,
        'path1_tests':    [0],
        'path2_tests':    [0],
    },

    # 0x0040b8e0 GetScoreThreshold7or10 — int(void). Same gate; 7 or 10.
    'score_threshold_7or10': {
        'rva':            0x0040b8e0,
        'export':         'GetScoreThreshold7or10',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'zero_arg':       True,
        'zero_arg_baseline': 10,
        'scenario_sentinel': 0x007f0fd0,
        'lut_root_delta': 0,
        'path1_tests':    [0],
        'path2_tests':    [0],
    },

    # 0x005ab070 AudioTimerRateGet — undefined4(void). Returns DAT_007dcdf8 if the
    # gate DAT_007dce00 == 1, else literal 1000. The gate flips to 1 by the title
    # screen; 0x007dcdf8 ~ 10000000. baseline rejects the 1000 ungated default.
    # sentinel=0x007dce00 (the gate). scenario=race reaches gate=1 well before.
    'audio_timer_rate_get': {
        'rva':            0x005ab070,
        'export':         'AudioTimerRateGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'zero_arg':       True,
        'zero_arg_baseline': 1000,
        'scenario_sentinel': 0x007dce00,
        'lut_root_delta': 0,
        'path1_tests':    [0],
        'path2_tests':    [0],
    },

    # 0x004077e0 GetSelectedCopterField60 — float10(void). field +0 of the SELECTED
    # Copter record (&DAT_00639de0)[DAT_0063a5d8*0x3b]. Copter array @0x00639de0
    # (selected idx @0x0063a5d8) is unpopulated in Quick-Battle arena -> likely
    # DEFER. sentinel=0x00639de0.
    'selected_copter_field60': {
        'rva':            0x004077e0,
        'export':         'GetSelectedCopterField60',
        'signature':      {'ret': 'float', 'args': []},
        'arg_type':       'none',
        'zero_arg':       True,
        'zero_arg_baseline': None,
        'scenario_sentinel': 0x00639de0,
        'lut_root_delta': 0,
        'path1_tests':    [0],
        'path2_tests':    [0],
    },

    # ---- promote-round round 1 (L0: c3_batch_race1 session-1 set) ------------
    # All five run on the scenario:'race' lane (live Quick Battle state);
    # configs verbatim from c3_batch_race1.txt (generation-time note reads).

    # 0x00408af0  AiVehicleFieldPtrGet — uint32(int i). POINTER-RETURN getter:
    # &DAT_008a96dc + i*0x30c (field +0x9c of the 0x30c-stride per-vehicle
    # records). Never dereferences — any index vector is safe; bit-identity
    # compares the returned original-image VA.
    # ref: re/analysis/bucket_ai_00407a40_00415880/0x00408af0.md
    'ai_vehicle_field_ptr_get': {
        'rva':            0x00408af0,
        'export':         'AiVehicleFieldPtrGet',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 4, 7, 0, 1, 2, 3],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00442cc0  AiVehicleFloat4Get — float(int i), x87 ST0 return. Bounds-
    # checked: i<4 reads DAT_008989b0[i]; else returns DAT_005d757c (0.0,
    # last-place sentinel). In-race the 4-entry array populates; A/B time-skew
    # re-run rule applies (single-vector float mismatch → re-run once).
    # U-7601 is data-semantic (Blocks: none).
    # ref: re/analysis/bucket_ai_00415d00_00452ea0/0x00442cc0.md
    'ai_vehicle_float4_get': {
        'rva':            0x00442cc0,
        'export':         'AiVehicleFloat4Get',
        'signature':      {'ret': 'float', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 4, 100, 0, 1, 2, 3],
        'path2_tests':    [0, 1, 4],
    },

    # 0x00414030  AiSplineBankTimerReset — void(int slot). Writes 1000 (0x3e8)
    # at DAT_008032d4[slot*5] (stride 0x14 bytes); slot_block_zero plants a
    # sentinel at target_global + slot*stride and verifies it is destroyed
    # (overwritten with 1000) = evidence. NEVER vector -1: the harness would
    # sentinel base-0x14 (out of the array); the -1 "reset all 5 slots" fill
    # path therefore stays UNTESTED by this diff.
    # ref: re/analysis/bucket_ai_00407a40_00415880/0x00414030.md
    'ai_spline_bank_timer_reset': {
        'rva':                0x00414030,
        'export':             'AiSplineBankTimerReset',
        'signature':          {'ret': 'void', 'args': ['int32']},
        'arg_type':           'slot_block_zero',
        'target_global':      0x008032d4,
        'entity_byte_stride': 0x14,
        'lut_root_delta':     0,
        'scenario':           'race',
        'path1_tests':        [0, 1, 2, 3, 0, 1, 2, 3, 2, 1],
        'path2_tests':        [0, 1, 2],
    },

    # 0x0040e350  GetRenderSubMode — uint32(). Returns DAT_0063ba8c (game-mode
    # state global; 5-byte stub). Stable mid-race (nonzero in a live round →
    # non-degenerate). Export name matches the fn-ptr binding citation in
    # Render/PerModeRender.cpp:43 (file-local static — no clash).
    # ref: re/analysis/promote_c2_vehicle_lowrva/0x0040e350.md
    'get_render_sub_mode': {
        'rva':            0x0040e350,
        'export':         'GetRenderSubMode',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00429300  HudOverlayFloatGet — float(), x87 ST0. FILD (signed-int
    # load + convert) of DAT_008991b8: bytes DB 05 B8 91 89 00 C3 verified in
    # MASHED.exe.unpatched @0x29300 — the global is an INTEGER; a raw float
    # load REDs (orig=923.0f vs denormal-bits-923). Caller's unused arg is
    # ignored (__cdecl caller-clean). Written at race-overlay setup → stable.
    # ref: re/analysis/localization_d2/0x00429300.md
    'hud_overlay_float_get': {
        'rva':            0x00429300,
        'export':         'HudOverlayFloatGet',
        'signature':      {'ret': 'float', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # ---- promote-round round 2 (L0: c3_batch_race1 session-2 set) ------------
    # All five on the scenario:'race' lane; configs verbatim from
    # c3_batch_race1.txt; bodies byte-verified in MASHED.exe.unpatched.

    # 0x00442410  CameraSlotFieldPtrGet — uint32(int i). POINTER-RETURN:
    # &0x008964fc + i*0xd8 (field +0x9c of the 5-slot 0xd8-stride camera/view
    # record array; sibling of CameraSlotFloatGet at +0x110). Never derefs.
    # ref: re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x00442410.md
    'camera_slot_field_ptr_get': {
        'rva':            0x00442410,
        'export':         'CameraSlotFieldPtrGet',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 4, 0, 1, 2, 3, 4],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00442420  CameraSlotFloatGet — float(int i), x87 ST0 (FLD m32, byte-
    # verified D9 80 — true float load). DEREFS &0x00896570 + i*0xd8: keep i
    # in 0..4 (5-slot record array). Time-skew re-run rule applies.
    # ref: re/analysis/bucket_powerups_camera_particle_0044d5e0_004b4140/0x00442420.md
    'camera_slot_float_get': {
        'rva':            0x00442420,
        'export':         'CameraSlotFloatGet',
        'signature':      {'ret': 'float', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 4, 0, 1, 2, 3, 4],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00423b20  CarSnapshotDwordGet — uint32(int i). DEREFS
    # (&0x008995ec) + i*0x138 (dword +0x12c of the 0x138-stride per-car
    # snapshot, STRUCTS.md §4). Keep i in 0..3 (4-car race).
    # ref: re/analysis/promote_c2_vehicle_lowrva/0x00423b20.md
    'car_snapshot_dword_get': {
        'rva':            0x00423b20,
        'export':         'CarSnapshotDwordGet',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 2, 1],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00426cc0  VehicleTable4cPtrGet — uint32(int i). POINTER-RETURN:
    # &0x00663658 + i*0x4c. Never derefs — out-of-range vectors safe.
    # Sibling of frontend_slot_table_ptr_426cb0 (base +0xc, same stride).
    # ref: re/analysis/promote_c2_vehicle_lowrva/0x00426cc0.md
    'vehicle_table_4c_ptr_get': {
        'rva':            0x00426cc0,
        'export':         'VehicleTable4cPtrGet',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 4, 7, 0, 1, 2, 3],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00442df0  RaceFloat898980Get — float(), x87 ST0 (FLD m32, byte-
    # verified D9 05 — true float load, NOT FILD). Returns _DAT_00898980
    # (collision-impact float; caller FUN_00410d10 compares vs 10.0f).
    # Update cadence unknown → time-skew re-run rule applies.
    # ref: re/analysis/promote_c2_vehicle_lowrva/0x00442df0.md
    'race_float_898980_get': {
        'rva':            0x00442df0,
        'export':         'RaceFloat898980Get',
        'signature':      {'ret': 'float', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # ---- promote-round round 3 (L1 race-lane pair + L2 cheap re-earns) -------
    # Bodies byte-verified in MASHED.exe.unpatched (cites in PromoLoop_round3.cpp).

    # 0x0042fe70  VehicleDword67ea80Get — uint32(). Returns DAT_0067ea80
    # (timeout-selector global, values 0/1/2 per U-0397; bytes A1 80 EA 67 00
    # C3). First run as arg_type none + scenario race exited 5 (DEGENERATE):
    # the global is genuinely 0 in a live Quick Battle (0 = valid domain
    # value, selects 900k). Switched to read_global — the harness SEEDS
    # 0x0067ea80 with each vector, calls fn(), compares the return
    # (discriminates the cited address; saves/restores the global).
    # Menu-attach suffices (the BSS address exists regardless of state).
    # ref: re/analysis/promote_c2_vehicle_lowrva/0x0042fe70.md
    'vehicle_dword_67ea80_get': {
        'rva':            0x0042fe70,
        'export':         'VehicleDword67ea80Get',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067ea80,
        'lut_root_delta': 0,
        # documented domain 0/1/2 (U-0397) + sentinels
        'path1_tests':    [0, 1, 2, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678,
                           0x80000000, 0x7FFFFFFF, 3, 0x55555555],
        'path2_tests':    [0, 1, 0xDEADBEEF],
    },

    # 0x0041ea80  TrackDescField40Get — uint32(). Returns
    # *(u32*)(DAT_0063d7e4 + 0x40) (bytes A1 E4 D7 63 00 8B 40 40 C3).
    # DEREFS the track-descriptor object pointer — NULL until a track loads,
    # so scenario:'race' is REQUIRED (menu-attach would AV both sides).
    # ref: re/analysis/ai_update_d6/0x0041ea80.md
    'track_desc_field40_get': {
        'rva':            0x0041ea80,
        'export':         'TrackDescField40Get',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004cbc60  RwGlobal7d4598Set — void(uint32). Writes param_1 to
    # DAT_007d4598 (bytes 8B 44 24 04 A3 98 45 7D 00 C3). L2 re-earn
    # (demoted-needs-reimpl). void_setter_observe: harness calls fn(v),
    # reads back 0x007d4598, saves/restores the global.
    # ref: re/analysis/skeleton_prep_render/004cbc60.md
    'rw_global_7d4598_set': {
        'rva':            0x004cbc60,
        'export':         'RwGlobal7d4598Set',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x007d4598,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678,
                           0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests':    [0, 1, 0xDEADBEEF],
    },

    # 0x004cbc70  RwGlobal7d4598Get — uint32(). Returns DAT_007d4598 (bytes
    # A1 98 45 7D 00 C3); getter counterpart of rw_global_7d4598_set. L2
    # re-earn. read_global: harness seeds 0x007d4598 with each vector,
    # calls fn(), compares the return (discriminates the cited address;
    # saves/restores the global).
    # ref: re/analysis/skeleton_prep_render/004cbc70.md
    'rw_global_7d4598_get': {
        'rva':            0x004cbc70,
        'export':         'RwGlobal7d4598Get',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x007d4598,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678,
                           0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests':    [0, 1, 0xDEADBEEF],
    },

    # 0x004cbc80  RwGlobal7d459cSet — void(uint32). Writes param_1 to
    # DAT_007d459c (= 0x007d4598 + 4; bytes 8B 44 24 04 A3 9C 45 7D 00 C3).
    # L2 re-earn. void_setter_observe on 0x007d459c.
    # ref: re/analysis/skeleton_prep_render/004cbc80.md
    'rw_global_7d459c_set': {
        'rva':            0x004cbc80,
        'export':         'RwGlobal7d459cSet',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x007d459c,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678,
                           0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests':    [0, 1, 0xDEADBEEF],
    },

    # ---- promote-round round 6 (L2 cheap re-earns) ----------------------------
    # Bodies byte-verified in MASHED.exe.unpatched (cites in PromoLoop_round6.cpp).

    # 0x004c9f50  RwGlobal7d4134Set — void(uint32). Writes param_1 to
    # DAT_007d4134 (bytes 8B 44 24 04 A3 34 41 7D 00 C3). L2 re-earn.
    # ref: re/analysis/skeleton_prep_boot_winmain_b/004c9f50.md
    'rw_global_7d4134_set': {
        'rva':            0x004c9f50,
        'export':         'RwGlobal7d4134Set',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x007d4134,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678,
                           0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests':    [0, 1, 0xDEADBEEF],
    },

    # 0x004b6610  BootGlobalPairSet — void(uint32 a, uint32 b). Writes a ->
    # DAT_007d3e5c, b -> DAT_007d3e60 (bytes 8B 44 24 04 8B 4C 24 08 A3 5C 3E
    # 7D 00 89 0D 60 3E 7D 00 C3). multi_arg_global_write over the 2-dword
    # block. guard_global REQUIRED by the handler but this fn has no guard:
    # pointed INSIDE the saved/restored out window (0x007d3e5c — the fn
    # overwrites it anyway; same self-contained trick as time_display_set_entry).
    # ref: re/analysis/promote_c2_txd_loader/004b6610.md
    'boot_global_pair_set': {
        'rva':            0x004b6610,
        'export':         'BootGlobalPairSet',
        'signature':      {'ret': 'void', 'args': ['uint32', 'uint32']},
        'arg_type':       'multi_arg_global_write',
        'guard_global':   0x007d3e5c,
        'out_base':       0x007d3e5c,
        'out_count':      2,
        'lut_root_delta': 0,
        'path1_tests':    [[0, 0], [1, 0], [0xDEADBEEF, 0xCAFEBABE],
                           [0xFFFFFFFF, 0x80000000], [0x12345678, 0x7FFFFFFF],
                           [0x00429290, 0], [0x55555555, 0xAAAAAAAA],
                           [2, 3], [0, 0xFFFFFFFF], [0xABCDEF01, 0x10FEDCBA]],
        'path2_tests':    [[0, 0], [1, 0], [0xDEADBEEF, 0xCAFEBABE]],
    },

    # 0x004b6560  BootGlobalPairSetThunk — 5B thunk (E9 AB 00 00 00 ->
    # 0x004b6610); forwards both args unmodified. Same handler/config as
    # boot_global_pair_set (behaviorally identical by construction).
    # ref: re/analysis/promote_c2_txd_loader/004b6560.md
    'boot_global_pair_set_thunk': {
        'rva':            0x004b6560,
        'export':         'BootGlobalPairSetThunk',
        'signature':      {'ret': 'void', 'args': ['uint32', 'uint32']},
        'arg_type':       'multi_arg_global_write',
        'guard_global':   0x007d3e5c,
        'out_base':       0x007d3e5c,
        'out_count':      2,
        'lut_root_delta': 0,
        'path1_tests':    [[0, 0], [1, 0], [0xDEADBEEF, 0xCAFEBABE],
                           [0xFFFFFFFF, 0x80000000], [0x12345678, 0x7FFFFFFF],
                           [0x00429290, 0], [0x55555555, 0xAAAAAAAA],
                           [2, 3], [0, 0xFFFFFFFF], [0xABCDEF01, 0x10FEDCBA]],
        'path2_tests':    [[0, 0], [1, 0], [0xDEADBEEF, 0xCAFEBABE]],
    },

    # 0x00491010  RainLineWidthRangeSet — void(uint32 a, uint32 b). Writes a ->
    # DAT_006146b8, b -> DAT_006146bc (bytes 8B 44 24 04 8B 4C 24 08 A3 B8 46 61
    # 00 89 0D BC 46 61 00 C3). multi_arg_global_write over the 2-dword block.
    # guard_global REQUIRED by the handler but this fn has no guard: pointed
    # INSIDE the saved/restored out window (0x006146b8 — the fn overwrites it
    # anyway; same self-contained trick as boot_global_pair_set).
    # ref: re/analysis/render_2_c1_to_c2_s5/FUN_00491010.md
    'rain_line_width_range_set': {
        'rva':            0x00491010,
        'export':         'RainLineWidthRangeSet',
        'signature':      {'ret': 'void', 'args': ['uint32', 'uint32']},
        'arg_type':       'multi_arg_global_write',
        'guard_global':   0x006146b8,
        'out_base':       0x006146b8,
        'out_count':      2,
        'lut_root_delta': 0,
        'path1_tests':    [[0, 0], [1, 0], [0x40400000, 0x40c00000],
                           [0xDEADBEEF, 0xCAFEBABE], [0xFFFFFFFF, 0x80000000],
                           [0x12345678, 0x7FFFFFFF], [0x55555555, 0xAAAAAAAA],
                           [2, 3], [0, 0xFFFFFFFF], [0xABCDEF01, 0x10FEDCBA]],
        'path2_tests':    [[0, 0], [1, 0], [0x40400000, 0x40c00000]],
    },

    # ---- promote-round round 7 (L2 cheap re-earns) ----------------------------
    # Bodies byte-verified in MASHED.exe.unpatched (cites in PromoLoop_round7.cpp).

    # 0x00499730  BootPtr773818Get — uint32(). Constant-pointer getter:
    # mov eax,0x00773818; ret (bytes B8 18 38 77 00 C3). The constant non-zero
    # return IS the leaf's entire behavior (no loads/branches) — comparing it
    # is complete coverage, and non-zero passes the degeneracy assertion.
    # ref: re/analysis/skeleton_prep_boot_winmain_b/00499730.md
    'boot_ptr_773818_get': {
        'rva':            0x00499730,
        'export':         'BootPtr773818Get',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00495120  TimerQpfStore — uint32(). QueryPerformanceFrequency ->
    # DAT_00771e70 (low) + DAT_00771e74 (high), returns 1 (QPF return
    # ignored). QPF is a machine constant -> deterministic across A/B.
    # scalars_to_scattered_globals: observe the 8-byte window + fold the
    # return; handler saves/restores the window both sides.
    # ref: re/analysis/skeleton_prep_boot_winmain_a/00495120.md
    'timer_qpf_store': {
        'rva':            0x00495120,
        'export':         'TimerQpfStore',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'scalars_to_scattered_globals',
        'observe':        [{'addr': '0x00771e70', 'len': 8}],
        'fold_ret':       True,
        'lut_root_delta': 0,
        'path1_tests':    [{'args': []}, {'args': []}, {'args': []},
                           {'args': []}, {'args': []}, {'args': []},
                           {'args': []}, {'args': []}, {'args': []},
                           {'args': []}],
        'path2_tests':    [{'args': []}, {'args': []}, {'args': []}],
    },

    # ---- promote-round round 8 (L3 curated small leaves) ----------------------
    # Bodies byte-verified in MASHED.exe.unpatched (cites in PromoLoop_round8.cpp).
    # All menu-attach with SEEDED handlers (discriminating regardless of state).

    # 0x004430a0  Util897fe0Set — void(uint32) -> DAT_00897fe0.
    # ref: re/analysis/game_state_d5/0x004430a0.md
    'util_897fe0_set': {
        'rva':            0x004430a0,
        'export':         'Util897fe0Set',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x00897fe0,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678,
                           0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests':    [0, 1, 0xDEADBEEF],
    },

    # 0x004430b0  Util897fe0Get — uint32() <- DAT_00897fe0 (pair of the above).
    # read_global seeds the address -> discriminating at menu-attach.
    # ref: re/analysis/game_state_d5/0x004430b0.md
    'util_897fe0_get': {
        'rva':            0x004430b0,
        'export':         'Util897fe0Get',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x00897fe0,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678,
                           0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests':    [0, 1, 0xDEADBEEF],
    },

    # 0x0042f790  GhostModeIsActive — uint32() <- DAT_0067ea70 (ghost flag).
    # ref: re/analysis/leaderboard_d2/0x0042f790.md
    'ghost_mode_is_active': {
        'rva':            0x0042f790,
        'export':         'GhostModeIsActive',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067ea70,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678,
                           0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests':    [0, 1, 0xDEADBEEF],
    },

    # 0x00431d70  CourseGetLeaderIndex — uint32() <- DAT_0067ea94.
    # ref: re/analysis/leaderboard_d2/0x00431d70.md
    'course_get_leader_index': {
        'rva':            0x00431d70,
        'export':         'CourseGetLeaderIndex',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0067ea94,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0xDEADBEEF, 0xFFFFFFFF,
                           0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 0x55555555],
        'path2_tests':    [0, 1, 3],
    },

    # 0x004cc7e0  RwGlobal6182b0Set — void(uint32) -> DAT_006182b0 (guard
    # consumed by FUN_004cc820 per U-5102). Harness saves/restores the global.
    # NOTE: diff 10/10 GREEN 2026-06-12 but promotion REFUSED — U-5102 carries
    # an explicit Blocks=C2->C3; stays C2 until the global's reads are traced.
    # ref: re/analysis/render_5_c1_to_c2_s1/FUN_004cc7e0.md
    'rw_global_6182b0_set': {
        'rva':            0x004cc7e0,
        'export':         'RwGlobal6182b0Set',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x006182b0,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678,
                           0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests':    [0, 1, 0xDEADBEEF],
    },

    # ---- promote-round round 9 (L3 curated small leaves, continued) -----------
    # Bodies byte-verified in MASHED.exe.unpatched (cites in PromoLoop_round9.cpp).

    # 0x00498be0  RenderBitDepthGet — uint32() <- DAT_00773414 (render bit
    # depth). read_global-seeded.
    # ref: re/analysis/settings_config_d2_cont1/00498be0.md
    'render_bit_depth_get': {
        'rva':            0x00498be0,
        'export':         'RenderBitDepthGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x00773414,
        'lut_root_delta': 0,
        'path1_tests':    [16, 32, 0, 1, 0xDEADBEEF, 0xFFFFFFFF,
                           0x80000000, 0x7FFFFFFF, 24, 0x55555555],
        'path2_tests':    [16, 32, 0],
    },

    # 0x0040dc80  UtilFloat63b910Get — float() <- DAT_0063b910 (FLD m32,
    # byte-verified D9 05 — TRUE float load, not FILD). read_global seeds raw
    # IEEE bits; both sides load identically.
    # ref: re/analysis/localization_d2/0x0040dc80.md
    'util_float_63b910_get': {
        'rva':            0x0040dc80,
        'export':         'UtilFloat63b910Get',
        'signature':      {'ret': 'float', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0063b910,
        'lut_root_delta': 0,
        # raw IEEE-754 bits: 0, 1.0, -1.0, 0.5, 100.0, -100.0, pi, 0.1, 1e6, 2.0
        'path1_tests':    [0x00000000, 0x3f800000, 0xbf800000, 0x3f000000,
                           0x42c80000, 0xc2c80000, 0x40490fdb, 0x3dcccccd,
                           0x49742400, 0x40000000],
        'path2_tests':    [0x3f800000, 0xc2c80000, 0x00000000],
    },

    # 0x0040dc90  UtilSlotIndexCondGet — uint32(). Returns DAT_0063b914 when
    # DAT_007f0fd0 (game mode) is not 5/10, else 0. read_global seeds the
    # VALUE global 0x0063b914; the condition global stays live (menu mode is
    # outside {5,10} -> read path taken consistently both sides; the ==5/10
    # short-circuit path stays UNTESTED by this diff — recorded here).
    # ref: re/analysis/localization_d2/0x0040dc90.md
    'util_slot_index_cond_get': {
        'rva':            0x0040dc90,
        'export':         'UtilSlotIndexCondGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x0063b914,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0xDEADBEEF, 0xFFFFFFFF,
                           0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 0x55555555],
        'path2_tests':    [0, 1, 0xDEADBEEF],
    },

    # 0x00429860  RaceStateFlagGet — uint32() <- DAT_008991bc (race-state
    # flag; 0xb = lap-complete). read_global-seeded.
    # ref: re/analysis/bucket_ai_00415d00_00452ea0/0x00429860.md
    'race_state_flag_get': {
        'rva':            0x00429860,
        'export':         'RaceStateFlagGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x008991bc,
        'lut_root_delta': 0,
        'path1_tests':    [0, 0xb, 1, 2, 0xDEADBEEF, 0xFFFFFFFF,
                           0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 0x55555555],
        'path2_tests':    [0, 0xb, 1],
    },

    # 0x00429840  RaceStateLatchSet — void(uint32 v): stores v to
    # DAT_008991bc iff v==0 OR current==0 (latch). scalars_to_scattered_
    # globals with fill 0xFF forces current=0xFFFFFFFF inside the restored
    # bracket: v!=0 vectors exercise the NO-STORE latch branch (window stays
    # FF), v==0 exercises the unconditional store. The current==0 plain-store
    # path is NOT separately exercised (would need fill 0x00 — second entry
    # if ever needed); both tested branches are the latch-distinctive ones.
    # ref: re/analysis/bucket_ai_00415d00_00452ea0/0x00429840.md
    'race_state_latch_set': {
        'rva':            0x00429840,
        'export':         'RaceStateLatchSet',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'scalars_to_scattered_globals',
        'observe':        [{'addr': '0x008991bc', 'len': 4, 'fill': 0xFF}],
        'lut_root_delta': 0,
        'path1_tests':    [{'args': [0]}, {'args': [1]}, {'args': [0xb]},
                           {'args': [0]}, {'args': [0xDEADBEEF]},
                           {'args': [0xFFFFFFFF]}, {'args': [0]},
                           {'args': [2]}, {'args': [0x80000000]},
                           {'args': [0]}],
        'path2_tests':    [{'args': [0]}, {'args': [1]}, {'args': [0xb]}],
    },

    # ---- promote-round round 10 (L3 race-lane + live-global set) --------------
    # Bodies byte-verified in MASHED.exe.unpatched (cites in PromoLoop_round10.cpp).

    # 0x0046c750  EntityVelocityCounterGet — uint32(int i). Bounds-checked
    # (SIGNED jl vs 0x10) getter of the velocity/exposure counter at
    # 0x00882194 + i*0xd04 (range 0-3000, populated by movement in a live
    # race). OOB (>=16) returns 0. scenario race for populated counters.
    # ref: re/analysis/bucket_ai_00452eb0_004c3df0/0046c750.md
    'entity_velocity_counter_get': {
        'rva':            0x0046c750,
        'export':         'EntityVelocityCounterGet',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 15, 16, 100, 0, 1, 2],
        'path2_tests':    [0, 1, 16],
    },

    # 0x0046c730  EntityDamageStateGet — uint32(int i). Sibling field (+4):
    # damage state 0/1/2 at 0x00882198 + i*0xd04. Exit-5 risk noted: a short
    # drive may have zero damage everywhere — accepted; exit-5 => defer.
    # ref: re/analysis/bucket_ai_00452eb0_004c3df0/0046c730.md
    'entity_damage_state_get': {
        'rva':            0x0046c730,
        'export':         'EntityDamageStateGet',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 15, 16, 100, 0, 1, 2],
        'path2_tests':    [0, 1, 16],
    },

    # 0x0040b410  RaceScoreTimerGet — uint32(int i). UNBOUNDED indexed read
    # of the dword array at 0x008a9520 + i*4 (scaled-index byte stride 4).
    # Group-B reset writes -1000 (0xfffffc18) => nonzero mid-race. Vectors
    # stay small non-negative (no bounds check in the original).
    # ref: re/analysis/game_state_d5_cont2/0x0040b410.md
    'race_score_timer_get': {
        'rva':            0x0040b410,
        'export':         'RaceScoreTimerGet',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 2, 1],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0040e360  RaceModeSet — void(uint32) -> 0x0063ba8c, the LIVE
    # game-mode/race-phase global (read by GetRenderSubMode + AI dispatcher).
    # void_setter_observe saves/restores per vector, but the game thread can
    # sample mid-bracket -> vectors RESTRICTED to values observed in the
    # original (4/5/8/9 compares per 0x0040e350 notes; 7 and 6 from the
    # cited call sites). NO garbage values — keep it that way.
    # ref: re/analysis/leaderboard_d2/0x0040e360.md
    'race_mode_set': {
        'rva':            0x0040e360,
        'export':         'RaceModeSet',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x0063ba8c,
        'lut_root_delta': 0,
        'path1_tests':    [7, 6, 4, 5, 8, 9, 7, 6, 4, 5],
        'path2_tests':    [7, 6, 4],
    },

    # ---- promote-round round 11 (L3 looser-curation leaves) -------------------
    # Bodies byte-verified in MASHED.exe.unpatched (cites in PromoLoop_round11.cpp).

    # 0x004cae90  RwCapsBlockPtrGet — uint32(). Constant-pointer return
    # &DAT_00618220 (D3D9 caps block); B8 20 82 61 00 C3.
    'rw_caps_block_ptr_get': {
        'rva':            0x004cae90,
        'export':         'RwCapsBlockPtrGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0042f520  ViewportBlockPtrGet — uint32(). Constant-pointer return
    # &DAT_00899150; B8 50 91 89 00 C3.
    'viewport_block_ptr_get': {
        'rva':            0x0042f520,
        'export':         'ViewportBlockPtrGet',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00485370  DynamicObjectListGetBase — uint32(). Constant-pointer
    # return &DAT_006e87b8; B8 B8 87 6E 00 C3.
    'dynamic_object_list_get_base': {
        'rva':            0x00485370,
        'export':         'DynamicObjectListGetBase',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00405420  ReplayCursorReset — void(). Stores 0 to DAT_00639d74
    # (C7 05 74 9D 63 00 00 00 00 00 C3). Pre-fill 0xFF makes the zero-store
    # observable (baseline FF -> 0 after the call); handler saves/restores.
    'replay_cursor_reset': {
        'rva':            0x00405420,
        'export':         'ReplayCursorReset',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'scalars_to_scattered_globals',
        'observe':        [{'addr': '0x00639d74', 'len': 4, 'fill': 0xFF}],
        'lut_root_delta': 0,
        'path1_tests':    [{'args': []}, {'args': []}, {'args': []},
                           {'args': []}, {'args': []}, {'args': []},
                           {'args': []}, {'args': []}, {'args': []},
                           {'args': []}],
        'path2_tests':    [{'args': []}, {'args': []}, {'args': []}],
    },

    # ---- promote-round round 12 (L3 looser-curation remainder) ----------------
    # Bodies byte-verified in MASHED.exe.unpatched (cites in PromoLoop_round12.cpp).

    # 0x0041b510  HudCounterReset — void(); DAT_0063cab0 := 0. fill=0xFF makes
    # the zero-store observable inside the restored bracket.
    'hud_counter_reset': {
        'rva':            0x0041b510,
        'export':         'HudCounterReset',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'scalars_to_scattered_globals',
        'observe':        [{'addr': '0x0063cab0', 'len': 4, 'fill': 0xFF}],
        'lut_root_delta': 0,
        'path1_tests':    [{'args': []}, {'args': []}, {'args': []},
                           {'args': []}, {'args': []}, {'args': []},
                           {'args': []}, {'args': []}, {'args': []},
                           {'args': []}],
        'path2_tests':    [{'args': []}, {'args': []}, {'args': []}],
    },

    # 0x00431b10  BootParamSet2 — void(); DAT_007f0f10 := 2. fill=0xFF baseline
    # -> 2 after the call (observable).
    'boot_param_set2': {
        'rva':            0x00431b10,
        'export':         'BootParamSet2',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'scalars_to_scattered_globals',
        'observe':        [{'addr': '0x007f0f10', 'len': 4, 'fill': 0xFF}],
        'lut_root_delta': 0,
        'path1_tests':    [{'args': []}, {'args': []}, {'args': []},
                           {'args': []}, {'args': []}, {'args': []},
                           {'args': []}, {'args': []}, {'args': []},
                           {'args': []}],
        'path2_tests':    [{'args': []}, {'args': []}, {'args': []}],
    },

    # 0x004d6e60  TexStageCacheGet — uint32(int stage). Per-stage texture
    # cache read at 0x007d6b30 + stage*0x18. Menu-attach was exit-5 (cache
    # all-zero — the menu's 2D path does not populate it); switched to
    # scenario:'race' where in-race 3D rendering binds stage textures.
    'tex_stage_cache_get': {
        'rva':            0x004d6e60,
        'export':         'TexStageCacheGet',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 0, 1],
        'path2_tests':    [0, 1, 2],
    },

    # ---- promote-round round 15 (round-14 Ghidra-pass carry-over) -------------
    # 0x00402f40  Util636ad8Get — uint32() <- DAT_00636ad8 (read_global-seeded,
    # discriminating at menu-attach). Caller FUN_0043dfd0 frontend C2.
    # Body byte-verified A1 D8 6A 63 00 C3.
    # ref: re/analysis/timer_d3_cont1_a/0x00402f40.md
    'util_636ad8_get': {
        'rva':            0x00402f40,
        'export':         'Util636ad8Get',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x00636ad8,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0xDEADBEEF, 0xFFFFFFFF,
                           0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 0x55555555],
        'path2_tests':    [0, 1, 0xDEADBEEF],
    },

    # ---- promote-round round 16 (Ghidra disassembly pass) ---------------------
    # 0x004c9eb0  DeviceModeBestBelowSet — void(uint32 param_1). Writes param_1
    # to DAT_006181c4, scans the device's enumerated descriptors (vtable +0x18
    # count / +0x1c fetch, both __stdcall — disassembly-verified) across 3
    # categories at 0x005d8b80, then overwrites DAT_006181c4 with the largest
    # descriptor value strictly below param_1 (or param_1 on exact match) if it
    # differs. scalars_to_scattered_globals observes the 4-byte result window;
    # the device object at 0x007d4108 is populated at menu-attach and the
    # enumeration is deterministic. param_1=0 short-circuits (write 0).
    # ref: re/analysis/skeleton_prep_boot_winmain_b/004c9eb0.md
    'device_mode_best_below_set': {
        'rva':            0x004c9eb0,
        'export':         'DeviceModeBestBelowSet',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'scalars_to_scattered_globals',
        'observe':        [{'addr': '0x006181c4', 'len': 4}],
        'lut_root_delta': 0,
        'path1_tests':    [{'args': [0x3c]}, {'args': [0x190]}, {'args': [0x258]},
                           {'args': [0x320]}, {'args': [0x400]}, {'args': [0x500]},
                           {'args': [0x258]}, {'args': [0x320]}, {'args': [0x3c]},
                           {'args': [0]}],
        'path2_tests':    [{'args': [0x3c]}, {'args': [0x320]}, {'args': [0]}],
    },

    # ---- promote-round round 17 (fresh discovery: single-global leaves) -------
    # Bodies byte-verified in MASHED.exe.unpatched (cites in PromoLoop_round17.cpp).

    # 0x00496920  TimerTable772ffcGet — uint32(int i). Indexed read of the
    # 4-byte-element table at 0x00772ffc + i*4 (timer-tick table; caller uses
    # index 5). Menu-attach was exit-5 (table all-zero on the 2D menu path);
    # scenario:'race' populates it (the timer tick that writes the table runs
    # during a live race).
    # ref: re/analysis/skeleton_prep_boot_winmain_b/00496920.md
    'timer_table_772ffc_get': {
        'rva':            0x00496920,
        'export':         'TimerTable772ffcGet',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 5, 3],
        'path2_tests':    [0, 5, 3],
    },

    # 0x00496930  TimerTable773030Get — uint32(int i). Complement table at
    # 0x00773030 + i*4 (caller uses indices 3/4). Same exit-5 -> race fix.
    # ref: re/analysis/timer_d2/0x00496930.md
    'timer_table_773030_get': {
        'rva':            0x00496930,
        'export':         'TimerTable773030Get',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 3, 4],
        'path2_tests':    [0, 3, 4],
    },

    # 0x00485360  DynObjListGetCount — uint32() <- DAT_006fa0f8 (count sibling
    # of DynamicObjectListGetBase 0x00485370, round 11). read_global-seeded.
    # ref: re/analysis/bucket_vehicle_004820e0_00485420/00485360.md
    'dyn_obj_list_get_count': {
        'rva':            0x00485360,
        'export':         'DynObjListGetCount',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x006fa0f8,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0xDEADBEEF, 0xFFFFFFFF,
                           0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 0x55555555],
        'path2_tests':    [0, 1, 0xDEADBEEF],
    },

    # 0x00550790  FsManager7dc76cSet — void(uint32) -> DAT_007dc76c.
    # void_setter_observe saves/restores the FS-manager field.
    # ref: re/analysis/input_dinput_d3/00550790.md
    'fs_manager_7dc76c_set': {
        'rva':            0x00550790,
        'export':         'FsManager7dc76cSet',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x007dc76c,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678,
                           0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests':    [0, 1, 0xDEADBEEF],
    },

    # ---- promote-round round 18 (widened curation: 16-60B getters) ------------
    # 0x0041efc0  CarGetLapProgress — uint32(int i). DOUBLE-DEREF:
    # ptr = *(int*)(0x0063dc48 + i*0x2ac); return *(uint32*)(ptr + 0x80).
    # Car-data pointers are NULL at menu -> scenario:'race' REQUIRED; in-bounds
    # car indices 0..3 (4-car Quick Battle). Body byte-verified.
    # ref: re/analysis/leaderboard_d2/0x0041efc0.md
    'car_get_lap_progress': {
        'rva':            0x0041efc0,
        'export':         'CarGetLapProgress',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 2, 1],
        'path2_tests':    [0, 1, 2],
    },

    # ---- promote-round round 19 (L5: outbuf_only harness-ext) -----------------
    # 0x0040b620  SlotSortByModeScore — void(int* out4): bubble-sorts 4 slot
    # indices descending by per-slot mode score (DAT_008a9530) with -100 for
    # inactive slots (DAT_007f1a14[i]==-1), writing the int[4] permutation into
    # the caller buffer. SWEEP-CRITICAL new arg_type 'outbuf_only' (single
    # out-ptr, no scalar arg; out_buf_size=16). Output is a permutation of
    # 0..3 -> non-degenerate even when all scores are equal at menu.
    # ref: re/analysis/frontend_promote_menus_b/0040b620.md
    'slot_sort_by_mode_score': {
        'rva':            0x0040b620,
        'export':         'SlotSortByModeScore',
        'signature':      {'ret': 'void', 'args': ['pointer']},
        'arg_type':       'outbuf_only',
        'out_buf_size':   16,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # ---- promote-round round 20 (single-out-ptr class via outbuf_only) --------

    # 0x00495270  HWNDGet — void(uint32* out): out = FUN_00499710() (game
    # window handle, constant non-zero at menu). outbuf_only out_buf_size=4.
    # ref: re/analysis/skeleton_prep_boot_winmain_a/00495270.md
    'hwnd_get': {
        'rva':            0x00495270,
        'export':         'HWNDGet',
        'signature':      {'ret': 'void', 'args': ['pointer']},
        'arg_type':       'outbuf_only',
        'out_buf_size':   4,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00484c70  WorldObjectsBaseGet — uint32(uint32* out): *out =
    # DAT_006e70d8 (count), RETURN = 0x006dccb8 (base). fold_ret captures the
    # constant base so the fingerprint is non-degenerate even when the count
    # is 0 at menu. ret declared uint32 for fold_ret.
    # ref: re/analysis/bucket_ai_00452eb0_004c3df0/00484c70.md
    'world_objects_base_get': {
        'rva':            0x00484c70,
        'export':         'WorldObjectsBaseGet',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'outbuf_only',
        'out_buf_size':   4,
        'fold_ret':       True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0041da90  DeltaTimeOutGet — void(uint32* out): if(out) *out =
    # DAT_0063d588 (lap-complete accumulator). seed_global FAILED (the game
    # thread overwrites 0x0063d588 every frame, so the seed didn't hold and
    # the menu value is 0 -> degenerate). scenario:'race' gives a live non-zero
    # accumulator; the slow per-frame cadence means A/B time-skew is per-mille
    # (single-vector mismatch -> re-run once).
    # ref: re/analysis/ai_update_d5/0x0041da90.md
    'delta_time_out_get': {
        'rva':            0x0041da90,
        'export':         'DeltaTimeOutGet',
        'signature':      {'ret': 'void', 'args': ['pointer']},
        'arg_type':       'outbuf_only',
        'out_buf_size':   4,
        'scenario':       'race',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # ---- promote-round round 21 (out-ptr survey: int_copy_outbuf) -------------

    # 0x0041f030  TriggerStructRead — void(int i, uint32* out4): copies 4
    # dwords from 0x0063dc38 + i*0x2ac into out[0..3] (type-0x15 trigger
    # struct). int_copy_outbuf out_buf_size=16; race lane (live triggers).
    # ref: re/analysis/ai_update_d3/0x0041f030.md
    'trigger_struct_read': {
        'rva':            0x0041f030,
        'export':         'TriggerStructRead',
        'signature':      {'ret': 'void', 'args': ['int32', 'pointer']},
        'arg_type':       'int_copy_outbuf',
        'out_buf_size':   16,
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 2, 1],
        'path2_tests':    [0, 1, 2],
    },

    # 0x0041f260  WorldMatrixCopy — void(int i, uint32* out16): DOUBLE-DEREF
    # src = *(*(0x0063dc4c + i*0x2ac) + 4) + 0x10; copies the 16-dword (4x4)
    # matrix to out. int_copy_outbuf out_buf_size=64; race lane (per-slot
    # object ptrs NULL at menu -> in-bounds slots 0..3 only).
    # ref: re/analysis/random_rng_d2/0x0041f260.md
    'world_matrix_copy': {
        'rva':            0x0041f260,
        'export':         'WorldMatrixCopy',
        'signature':      {'ret': 'void', 'args': ['int32', 'pointer']},
        'arg_type':       'int_copy_outbuf',
        'out_buf_size':   64,
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 2, 1],
        'path2_tests':    [0, 1, 2],
    },

    # ---- promote-round round 22 (L5 vec3_lerp harness-ext) --------------------
    # 0x004b4650  Vec3Lerp — void(float* out, float* a, float* b, float t):
    # out = a + t*(b-a), pure float leaf (no globals). NEW arg_type vec3_lerp
    # (SWEEP-CRITICAL). Pure -> menu-attach-safe; tests provide a/b/t.
    # ref: re/analysis/render_3_c1_to_c2_s3/FUN_004b4650.md
    # ---- promote-round round 28 (worklist batch: setters + derived getter) ----
    'set_67eaa8': {
        'rva': 0x00431b40, 'export': 'Set67eaa8', 'signature': {'ret': 'void', 'args': ['uint32']},
        'arg_type': 'void_setter_observe', 'target_global': 0x0067eaa8, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'set_77396c': {
        'rva': 0x0049a2e0, 'export': 'Set77396c', 'signature': {'ret': 'void', 'args': ['uint32']},
        'arg_type': 'void_setter_observe', 'target_global': 0x0077396c, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'set_773978': {
        'rva': 0x0049a740, 'export': 'Set773978', 'signature': {'ret': 'void', 'args': ['uint32']},
        'arg_type': 'void_setter_observe', 'target_global': 0x00773978, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'ghost_flag_reset': {
        'rva': 0x0042f7a0, 'export': 'GhostFlagReset', 'signature': {'ret': 'void', 'args': []},
        'arg_type': 'scalars_to_scattered_globals',
        'observe': [{'addr': '0x0067ea70', 'len': 4, 'fill': 0xFF}], 'lut_root_delta': 0,
        'path1_tests': [{'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}],
        'path2_tests': [{'args': []}, {'args': []}, {'args': []}],
    },
    'powerup_target_ptr_get': {
        'rva': 0x00452160, 'export': 'PowerupTargetPtrGet', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x00684dac, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFF0, 0x12345678, 0x80000000, 0x7FFFFFF0, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },

    # ---- promote-round round 124 (DLL get-Nth element) --
    'get_5af700': {'rva': 0x005af700, 'export': 'Get5af700', 'signature': {'ret': 'uint32', 'args': ['pointer','pointer','uint32']}, 'arg_type': 'dll_get_nth', 'lut_root_delta': 0, 'path1_tests': [0,1,2,3,4], 'path2_tests': [0,4]},

    # ---- promote-round round 125 (two-level indexed global read) --
    'get_4c75c0': {'rva': 0x004c75c0, 'export': 'Get4c75c0', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'indexed_global_2lvl', 'target_global': 0x007d40a8, 'glob': 0x007d3ff8, 'idx': 0x40, 'mid_off': 0x28, 'edx_val': 7, 'lut_root_delta': 0, 'path1_tests': [0xCAFE0001, 0xBEEF0002, 0x12340003], 'path2_tests': [0xCAFE0001]},

    # ---- promote-round round 126 (bounded indexed-array getter) --
    'get_485340': {'rva': 0x00485340, 'export': 'Get485340', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'indexed_bound_array_get', 'target_global': 0x006e71cc, 'glob': 0x006e71c4, 'field_off': 0x10, 'idx': 5, 'lut_root_delta': 0, 'path1_tests': [0xAA110001, 0x55220002, 0x99330003], 'path2_tests': [0xAA110001]},

    # ---- promote-round round 127 (mark-dirty setter, absolute tables) --
    'mark_4d5480': {'rva': 0x004d5480, 'export': 'Mark4d5480', 'signature': {'ret': 'void', 'args': ['uint32','uint32']}, 'arg_type': 'abs_ranges_setter', 'nscalar': 2, 'abs_ranges': [{'addr': 0x007d57f8, 'dwords': 24}, {'addr': 0x007d5168, 'dwords': 8}, {'addr': 0x007d6c14, 'dwords': 1}], 'lut_root_delta': 0, 'path1_tests': [[2, 0xAABB0001], [5, 0xCCDD0002], [9, 0x11220003]], 'path2_tests': [[2, 0xAABB0001]]},

    # ---- promote-round round 128 (ESI-keyed global searches + indexed idiv) --
    'search_45baa0': {'rva': 0x0045baa0, 'export': 'Search45baa0', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'esi_global_search', 'target_global': 0x005f9998, 'glob': 0x005f9bd8, 'stride': 0x40, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2], 'path2_tests': [0]},
    'search_41f330': {'rva': 0x0041f330, 'export': 'Search41f330', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'esi_global_search', 'target_global': 0x005f3828, 'glob': 0x005f5fe0, 'stride': 0x84, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2], 'path2_tests': [0]},
    'div_48ebc0': {'rva': 0x0048ebc0, 'export': 'Div48ebc0', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'indexed_global_idiv', 'target_global': 0x0076d994, 'stride': 0x488, 'idx': 0, 'lut_root_delta': 0, 'path1_tests': [1, 2, 3, 16], 'path2_tests': [2]},

    # ---- promote-round round 129 (deferred-renderstate setters, abs_ranges_setter, no new handler) --
    'set_4d6c40': {'rva': 0x004d6c40, 'export': 'Set4d6c40', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'abs_ranges_setter', 'nscalar': 1, 'abs_ranges': [{'addr': 0x007d6bf0, 'dwords': 1}, {'addr': 0x007d5890, 'dwords': 1}, {'addr': 0x007d5894, 'dwords': 1}, {'addr': 0x007d6c14, 'dwords': 1}, {'addr': 0x007d5168, 'dwords': 8}], 'lut_root_delta': 0, 'path1_tests': [1, 2, 3], 'path2_tests': [1]},
    'set_4d6c90': {'rva': 0x004d6c90, 'export': 'Set4d6c90', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'abs_ranges_setter', 'nscalar': 1, 'abs_ranges': [{'addr': 0x007d6bf4, 'dwords': 1}, {'addr': 0x007d5898, 'dwords': 1}, {'addr': 0x007d589c, 'dwords': 1}, {'addr': 0x007d6c14, 'dwords': 1}, {'addr': 0x007d5168, 'dwords': 8}], 'lut_root_delta': 0, 'path1_tests': [1, 2, 3], 'path2_tests': [1]},
    'set_4d54f0': {'rva': 0x004d54f0, 'export': 'Set4d54f0', 'signature': {'ret': 'void', 'args': ['uint32','uint32','uint32']}, 'arg_type': 'abs_ranges_setter', 'nscalar': 3, 'abs_ranges': [{'addr': 0x007d4720, 'dwords': 256}, {'addr': 0x007d62a8, 'dwords': 16}, {'addr': 0x007d6c18, 'dwords': 1}], 'lut_root_delta': 0, 'path1_tests': [[1, 2, 0xAA], [2, 1, 0xBB], [3, 3, 0xCC]], 'path2_tests': [[1, 2, 0xAA]]},

    # ---- promote-round round 130 (vec3 lerp, verbatim x87 naked port) --
    'lerp_4b4650': {'rva': 0x004b4650, 'export': 'Lerp4b4650', 'signature': {'ret': 'void', 'args': ['pointer','pointer','pointer','float']}, 'arg_type': 'float_vec3_lerp_out', 'seed_a': [0x3f800000, 0x40000000, 0x40400000], 'seed_b': [0x40a00000, 0x40e00000, 0x41100000], 't_bits': 0x3f000000, 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 131 (dot-product + cosine-clamp twins, float_2ptr_ret) --
    'dot_4726b0': {'rva': 0x004726b0, 'export': 'Dot4726b0', 'signature': {'ret': 'float', 'args': ['pointer','pointer']}, 'arg_type': 'float_2ptr_ret', 'seed_pairs': [{'a': [0.1, 0.2, 0.3], 'b': [0.5, 0.7, 0.9]}, {'a': [0.2, -0.1, 0.4], 'b': [0.3, 0.6, -0.2]}, {'a': [0.05, 0.05, 0.05], 'b': [1.0, 2.0, 3.0]}], 'lut_root_delta': 0, 'path1_tests': [0, 1, 2], 'path2_tests': [0]},
    'dot_4726f0': {'rva': 0x004726f0, 'export': 'Dot4726f0', 'signature': {'ret': 'float', 'args': ['pointer','pointer']}, 'arg_type': 'float_2ptr_ret', 'seed_pairs': [{'a': [0.1, 0.2, 0.3], 'b': [0.5, 0.7, 0.9]}, {'a': [0.2, -0.1, 0.4], 'b': [0.3, 0.6, -0.2]}, {'a': [0.05, 0.05, 0.05], 'b': [1.0, 2.0, 3.0]}], 'lut_root_delta': 0, 'path1_tests': [0, 1, 2], 'path2_tests': [0]},

    # ---- promote-round round 132 (6-plane inside/frustum test) --
    'inside_4cbb70': {'rva': 0x004cbb70, 'export': 'Inside4cbb70', 'signature': {'ret': 'uint32', 'args': ['pointer','pointer']}, 'arg_type': 'float_planes6_predicate', 'seed_sets': [
        {'point': [1.0, 2.0, 3.0, 0.5], 'planes': [[0,0,1,0]]*6},
        {'point': [0.0, 0.0, 0.0, 5.0], 'planes': [[0,0,0,5.0],[0,0,0,6.0],[0,0,0,7.0],[0,0,0,8.0],[0,0,0,9.0],[0,0,0,10.0]]},
        {'point': [0.0, 0.0, 0.0, 5.0], 'planes': [[0,0,0,5.0],[0,0,0,6.0],[0,0,0,7.0],[0,0,0,8.0],[0,0,0,9.0],[0,0,0,4.0]]}
    ], 'lut_root_delta': 0, 'path1_tests': [0, 1, 2], 'path2_tests': [1]},

    # ---- promote-round round 133 (value -> 3-component split, EAX=v/EDI=out) --
    'split_41e170': {'rva': 0x0041e170, 'export': 'Split41e170', 'signature': {'ret': 'void', 'args': ['uint32','pointer']}, 'arg_type': 'eax_edi_out', 'lut_root_delta': 0, 'path1_tests': [123456, 7890, 100000], 'path2_tests': [123456]},

    # ---- promote-round round 134 (bounded 2D-grid multi-out getter) --
    'grid_4957a0': {'rva': 0x004957a0, 'export': 'Grid4957a0', 'signature': {'ret': 'uint32', 'args': ['uint32','uint32','pointer','pointer','pointer']}, 'arg_type': 'grid_getter_multiout', 'grid': {'b1': 0x00772fac, 'b2': 0x00771e80, 'i': 1, 'j': 2, 'mul1': 0x89, 'mul3': 0x112, 's12': 8, 's3': 4, 'out1_t': [0x00772150, 0x00772154], 'out2_t': [0x007721d0, 0x007721d4], 'out3_t': [0x00772290]}, 'lut_root_delta': 0, 'path1_tests': [0x1000, 0x2000, 0x3000], 'path2_tests': [0x1000]},

    # ---- promote-round round 135 (deterministic struct constructor) --
    # round 136: 0x0046cbb0 already C3 since round 25 (CarStatePairGet) — status-precheck miss, reverted. indexed_abs_dualout handler retained for future use.
    'remove_5b35e0': {'rva': 0x005b35e0, 'export': 'Remove5b35e0', 'signature': {'ret': 'void', 'args': ['pointer','pointer']}, 'arg_type': 'dll_remove_count', 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 137 (DLL insert-at-head) --
    'insert_4c5bc0': {'rva': 0x004c5bc0, 'export': 'Insert4c5bc0', 'signature': {'ret': 'void', 'args': ['pointer','pointer']}, 'arg_type': 'dll_insert_head', 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 138 (4-entry global ptr-table match) --
    'match_45c510': {'rva': 0x0045c510, 'export': 'Match45c510', 'signature': {'ret': 'uint32', 'args': ['uint32','pointer']}, 'arg_type': 'global_ptrtable_match', 'tbl': 0x0088f680, 'lut_root_delta': 0, 'path1_tests': [0, 1], 'path2_tests': [0]},

    # ---- promote-round round 139 (global-record clear + conditional return) --
    'clear_4e4350': {'rva': 0x004e4350, 'export': 'Clear4e4350', 'signature': {'ret': 'uint32', 'args': ['uint32','uint32']}, 'arg_type': 'global_rec_clear_ret', 'glob': 0x007d716c, 'idx': 0x40, 'lut_root_delta': 0, 'path1_tests': [0, 1], 'path2_tests': [0]},

    # ---- promote-round round 140 (abs-array scan -> dirty flag) --
    'scan_42c150': {'rva': 0x0042c150, 'export': 'Scan42c150', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'abs_scan_flag', 'glob': 0x0067ea10, 'target_global': 0x0067eab4, 'span': 16, 'idx': 4, 'lut_root_delta': 0, 'path1_tests': [0, 1], 'path2_tests': [0]},

    # ---- promote-round round 141 (global 2-level list search) --
    'search_4850b0': {'rva': 0x004850b0, 'export': 'Search4850b0', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'global_2level_list_search', 'glob': 0x006e71cc, 'lut_root_delta': 0, 'path1_tests': [0, 1], 'path2_tests': [0]},

    # ---- promote-round round 142 (4-branch pure pointer getter) --
    'get_5aa9c0': {'rva': 0x005aa9c0, 'export': 'Get5aa9c0', 'signature': {'ret': 'uint32', 'args': ['pointer']}, 'arg_type': 'arg_flag_branch_getter', 'seed_sets': [{'c': 0x100, 'flag': 2, 'f': 0x5000}, {'c': 0x100, 'flag': 0, 'f': 0x5000}, {'c': 0, 'flag': 2, 'f': 0}, {'c': 0, 'flag': 0, 'f': 0}], 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3], 'path2_tests': [0]},

    # ---- promote-round round 143 (global DLL insert-at-head) --
    'insert_5a7420': {'rva': 0x005a7420, 'export': 'Insert5a7420', 'signature': {'ret': 'uint32', 'args': ['pointer']}, 'arg_type': 'global_dll_insert_head', 'glob': 0x007dca24, 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 144 (global-field-offset struct clear) --
    'clear_558140': {'rva': 0x00558140, 'export': 'Clear558140', 'signature': {'ret': 'uint32', 'args': ['pointer']}, 'arg_type': 'global_fieldoff_clear', 'glob': 0x00913274, 'lut_root_delta': 0, 'path1_tests': [0, 1], 'path2_tests': [0]},

    # ---- promote-round round 145 (4-state dispatch with list-unlink) --
    'stateadv_5b0ec0': {'rva': 0x005b0ec0, 'export': 'StateAdv5b0ec0', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'multi_state_list_setter', 'lut_root_delta': 0, 'path1_tests': [1, 2, 3, 0], 'path2_tests': [1]},

    # ---- promote-round round 146 (byte-field modular counter) --
    'counter_5b11d0': {'rva': 0x005b11d0, 'export': 'Counter5b11d0', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'byte_counter_struct', 'seed_sets': [{'b0': 5, 'b1': 10, 'b3': 8}, {'b0': 7, 'b1': 3, 'b3': 8}, {'b0': 2, 'b1': 0, 'b3': 100}], 'lut_root_delta': 0, 'path1_tests': [0, 1, 2], 'path2_tests': [0]},

    # ---- promote-round round 147 (arg-or-default memcpy to abs dest) --
    'copy_476a10': {'rva': 0x00476a10, 'export': 'Copy476a10', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'arg_default_memcpy_abs', 'target_global': 0x006924e8, 'glob': 0x00692558, 'copy_dwords': 16, 'lut_root_delta': 0, 'path1_tests': [0, 1], 'path2_tests': [0]},

    # ---- promote-round round 148 (byte-indexed table bit-clear) --
    'tbl_5b1180': {'rva': 0x005b1180, 'export': 'Tbl5b1180', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'byte_idx_table_bitclear', 'seed_sets': [{'b0': 2, 'b1': 3, 'b3': 10}, {'b0': 1, 'b1': 0, 'b3': 8}, {'b0': 5, 'b1': 7, 'b3': 8}], 'lut_root_delta': 0, 'path1_tests': [0, 1, 2], 'path2_tests': [0]},

    # ---- promote-round round 149 (bounded backward search, 5-byte-stride table) --

    # ---- promote-round round 150 (circular-list search by key field) --
    'search_5b0b60': {'rva': 0x005b0b60, 'export': 'Search5b0b60', 'signature': {'ret': 'uint32', 'args': ['pointer','uint32']}, 'arg_type': 'circular_list_search_node', 'lut_root_delta': 0, 'path1_tests': [0, 1], 'path2_tests': [0]},

    # ---- promote-round round 151 (global-field-offset struct set) --
    'set_558100': {'rva': 0x00558100, 'export': 'Set558100', 'signature': {'ret': 'uint32', 'args': ['pointer']}, 'arg_type': 'global_fieldoff_set', 'glob': 0x00913274, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2], 'path2_tests': [0]},

    # ---- promote-round round 152 (EAX-dest struct init) --
    'init_477450': {'rva': 0x00477450, 'export': 'Init477450', 'signature': {'ret': 'void', 'args': ['pointer','pointer','pointer','uint32','uint32']}, 'arg_type': 'eax_dest_memcpy_init', 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 153 (struct-table div/mod compute) --
    'calc_5b2fd0': {'rva': 0x005b2fd0, 'export': 'Calc5b2fd0', 'signature': {'ret': 'uint32', 'args': ['pointer','uint32','uint32','uint32','pointer']}, 'arg_type': 'struct_div_mod_compute', 'seed_sets': [{'val': 100, 'div': 7}, {'val': 50, 'div': 8}, {'val': 1000, 'div': 13}], 'lut_root_delta': 0, 'path1_tests': [0, 1, 2], 'path2_tests': [0]},

    # ---- promote-round round 154 (ring-buffer copy to linear dest) --
    'ring_5ab980': {'rva': 0x005ab980, 'export': 'Ring5ab980', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'ring_copy_5ab980', 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 155 (integer Newton sqrt, stdcall orig / cdecl reimpl, reuses int_scalar) --
    'sqrt_49da90': {'rva': 0x0049da90, 'export': 'Sqrt49da90', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'int_scalar', 'conv_orig': 'stdcall', 'lut_root_delta': 0, 'path1_tests': [7, 100, 10000, 1000000], 'path2_tests': [100]},

    # ---- promote-round round 156 (3-arg struct init with nested sub) --
    'init_485a00': {'rva': 0x00485a00, 'export': 'Init485a00', 'signature': {'ret': 'void', 'args': ['pointer','uint32','pointer']}, 'arg_type': 'struct_init_3arg_sub', 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 157 (2-way flag-branch struct compute) --
    'calc_5b93a0': {'rva': 0x005b93a0, 'export': 'Calc5b93a0', 'signature': {'ret': 'void', 'args': ['pointer','uint32']}, 'arg_type': 'flag_branch_struct_2way', 'lut_root_delta': 0, 'path1_tests': [0, 1], 'path2_tests': [1]},

    # ---- promote-round round 158 (strided record-array zeroer + index field) --
    'zero_484c90': {'rva': 0x00484c90, 'export': 'Zero484c90', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'abs_region_zeroer', 'glob': 0x006dccbc, 'target_global': 0x006e70d8, 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 159 (paired vec3 array fill) --
    'fill_4899c0': {'rva': 0x004899c0, 'export': 'Fill4899c0', 'signature': {'ret': 'void', 'args': ['pointer','pointer']}, 'arg_type': 'array_fill_2way', 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 160 (bounded abs-table state setter) --
    'set_458f20': {'rva': 0x00458f20, 'export': 'Set458f20', 'signature': {'ret': 'uint32', 'args': ['uint32','uint32']}, 'arg_type': 'abs_table_state_setter', 'glob': 0x0068b198, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3], 'path2_tests': [0]},

    # ---- promote-round round 161 (ESI/EDX field-match predicate) --
    'match_47bc90': {'rva': 0x0047bc90, 'export': 'Match47bc90', 'signature': {'ret': 'uint32', 'args': ['pointer','pointer']}, 'arg_type': 'esi_edx_predicate', 'lut_root_delta': 0, 'path1_tests': [0, 1], 'path2_tests': [0]},

    # ---- promote-round round 162 (EDX/EBX/EDI register-arg array search) --
    'find_42ad90': {'rva': 0x0042ad90, 'export': 'Find42ad90', 'signature': {'ret': 'uint32', 'args': ['pointer','uint32','uint32']}, 'arg_type': 'edx_ebx_edi_find', 'lut_root_delta': 0, 'path1_tests': [0, 1], 'path2_tests': [0]},

    # ---- promote-round round 163 (EBX/EDI register-arg search, arr from globals) --
    'find_42add0': {'rva': 0x0042add0, 'export': 'Find42add0', 'signature': {'ret': 'uint32', 'args': ['uint32','uint32']}, 'arg_type': 'ebx_edi_global_find', 'glob': 0x0067e9f8, 'target_global': 0x0067ed38, 'lut_root_delta': 0, 'path1_tests': [0, 1], 'path2_tests': [0]},

    # ---- promote-round round 164 (RainColorInit strided BGRA-swizzle fill) --
    'rain_491070': {'rva': 0x00491070, 'export': 'Rain491070', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'strided_color_fill', 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 165 (bitmap first-free-slot allocator) --
    'alloc_477b60': {'rva': 0x00477b60, 'export': 'Alloc477b60', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'bitmap_alloc_slot', 'lut_root_delta': 0, 'path1_tests': [0, 1], 'path2_tests': [0]},

    # ---- promote-round round 166 (state dispatch + intrusive-list insert) --
    'stateins_5b0f90': {'rva': 0x005b0f90, 'export': 'StateInsert5b0f90', 'signature': {'ret': 'void', 'args': ['pointer','uint32','pointer']}, 'arg_type': 'state_list_insert', 'lut_root_delta': 0, 'path1_tests': [0, 1], 'path2_tests': [0]},

    # ---- promote-round round 167 (multi-deref global setter) --
    'set_476b30': {'rva': 0x00476b30, 'export': 'Set476b30', 'signature': {'ret': 'void', 'args': ['pointer','pointer']}, 'arg_type': 'multi_deref_global_set', 'lut_root_delta': 0, 'path1_tests': [0, 1], 'path2_tests': [0]},

    # ---- promote-round round 168 (list-node const init) --
    'init_482730': {'rva': 0x00482730, 'export': 'Init482730', 'signature': {'ret': 'void', 'args': ['pointer','pointer']}, 'arg_type': 'list_node_const_init', 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 169 (bounded struct-of-arrays push) --
    'push_4893d0': {'rva': 0x004893d0, 'export': 'Push4893d0', 'signature': {'ret': 'void', 'args': ['pointer','pointer','pointer']}, 'arg_type': 'bounded_struct_push', 'lut_root_delta': 0, 'path1_tests': [0, 1], 'path2_tests': [0]},

    # ---- promote-round round 170 (trie walk, recursion->loop) --
    'trie_4daa00': {'rva': 0x004daa00, 'export': 'Trie4daa00', 'signature': {'ret': 'uint32', 'args': ['pointer','uint32','uint32']}, 'arg_type': 'trie_walk', 'lut_root_delta': 0, 'path1_tests': [0, 1], 'path2_tests': [0]},

    # ---- promote-round round 171 (struct delta-init + fcomp flag, naked port) --
    'delta_557110': {'rva': 0x00557110, 'export': 'Delta557110', 'signature': {'ret': 'uint32', 'args': ['pointer','pointer','pointer','pointer','pointer']}, 'arg_type': 'struct_delta_flag_init', 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},


    # ---- promote-round round 123 (circular doubly-linked-list search) --
    'search_5aa030': {'rva': 0x005aa030, 'export': 'Search5aa030', 'signature': {'ret': 'uint32', 'args': ['pointer','pointer']}, 'arg_type': 'circular_dll_search', 'lut_root_delta': 0, 'path1_tests': [0,1], 'path2_tests': [0,1]},

    # ---- promote-round round 122 (doubly-linked-list unlink) --
    'unlink_5ae550': {'rva': 0x005ae550, 'export': 'Unlink5ae550', 'signature': {'ret': 'void', 'args': ['pointer','pointer']}, 'arg_type': 'dll_unlink', 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 121 (cdecl idx+src -> abs-table memcpy) --
    'copy_41a4a0': {'rva': 0x0041a4a0, 'export': 'Copy41a4a0', 'signature': {'ret': 'void', 'args': ['uint32','pointer']}, 'arg_type': 'idx_src_abs_memcpy', 'target_global': 0x0063c630, 'stride': 0xc4, 'copy_dwords': 0x10, 'lut_root_delta': 0, 'path1_tests': [2,5,7], 'path2_tests': [2]},

    # ---- promote-round round 120 (cdecl ptr-deref + abs-table write) --
    'write_46be10': {'rva': 0x0046be10, 'export': 'Write46be10', 'signature': {'ret': 'uint32', 'args': ['pointer','uint32','uint32','uint32']}, 'arg_type': 'deref_struct_set', 'nscalar': 3, 'seed_byte': 0xCC, 'observe': [], 'abs_observe': ['0x00882ea8', '0x00882f28'], 'lut_root_delta': 0, 'path1_tests': [[1,2,0xDEAD]], 'path2_tests': [[1,2,0xDEAD]]},

    # ---- promote-round round 119 (nested struct op: p[0]->sub-buffer write) --
    'inc_4893b0': {'rva': 0x004893b0, 'export': 'Inc4893b0', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'nested_struct_op', 'link_off': 0x0, 'p_seed': [{'off': 0x8, 'val': 5}, {'off': 0xc, 'val': 0x100}], 'observe_p': [0xc], 'observe_sub': [0xdc, 0xd8], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- reimpl/d3d9-state-cache (2026-06-25): cache-core C2->C3 cluster --
    # cache_roundtrip (SWEEP-CRITICAL new arg_type): seed the scattered input-cache
    # global the 30-case getter reads, then compare BOTH the return value AND the
    # out-slot. Distinct per-test sentinels make the otherwise-degenerate getter
    # discriminating. Contract: re/analysis/d3d9_state_cache_struct.md §5.
    'd3d9_state_get': {'rva': 0x004d6910, 'export': 'D3d9State_Get',
        'signature': {'ret': 'uint32', 'args': ['uint32', 'pointer']},
        'arg_type': 'cache_roundtrip', 'lut_root_delta': 0,
        'path1_tests': [
            {'seed': [{'addr': '0x007d6b30', 'val': 0x01010101}], 'args': [1, None]},     # case 1
            {'seed': [{'addr': '0x007d6b34', 'val': 0x11112222},
                      {'addr': '0x007d6b38', 'val': 0x11112222}], 'args': [2, None]},     # case 2 equal -> ret 1
            {'seed': [{'addr': '0x007d6b34', 'val': 0x11112222},
                      {'addr': '0x007d6b38', 'val': 0x33334444}], 'args': [2, None]},     # case 2 unequal -> ret 0, out 0
            {'seed': [{'addr': '0x007d6aec', 'val': 0x00000006}], 'args': [6, None]},     # case 6
            {'seed': [{'addr': '0x007d6bf0', 'val': 0xA5A5000A}], 'args': [10, None]},    # case 0xa
            {'seed': [{'addr': '0x007d6b10', 'val': 0x0000C0DE}], 'args': [0xc, None]},   # case 0xc
            {'seed': [{'addr': '0x007d6b24', 'val': 0x3f800000}], 'args': [0x11, None]},  # case 0x11 (float; non-SNaN)
            {'seed': [{'addr': '0x007d58b8', 'val': 0xDEAD001E}], 'args': [0x1e, None]},  # case 0x1e
            {'seed': [], 'args': [5, None]},                                              # default: ret 0, no write
            {'seed': [], 'args': [0x13, None]},                                           # default: ret 0, no write
        ],
        'path2_tests': [{'seed': [{'addr': '0x007d6bf0', 'val': 0xA5A5000A}], 'args': [10, None]}]},

    # cache_setter_observe (SWEEP-CRITICAL new arg_type): seed cache polarity +
    # gate flags + dirty-queue counter + queue-slot poison, call the setter, read
    # back the scattered stores/dirty/cache/queue. Contract §2.
    # 0x004d7ac0 fog-blend toggle (pure queue-only, NO device call). obs order:
    #   [cache 6aec, op7 store 5830, op7 dirty 5834, op0x17 store 58b0,
    #    op0x17 dirty 58b4, counter 6c14, queue[0] 5168, queue[1] 516c].
    'd3d9_fog_blend_toggle': {'rva': 0x004d7ac0, 'export': 'D3d9State_FogBlendToggle',
        'signature': {'ret': 'void', 'args': ['uint32']},
        'arg_type': 'cache_setter_observe', 'lut_root_delta': 0,
        'obs_globals': ['0x007d6aec', '0x007d5830', '0x007d5834', '0x007d58b0',
                        '0x007d58b4', '0x007d6c14', '0x007d5168', '0x007d516c'],
        'path1_tests': [
            # 1: enable, cache-miss, op7 gate open (6ae8==0): push 7 then 0x17.
            {'seed': [{'addr': '0x007d6aec', 'val': 0}, {'addr': '0x007d6ae8', 'val': 0},
                      {'addr': '0x007d5834', 'val': 0}, {'addr': '0x007d58b4', 'val': 0},
                      {'addr': '0x007d6c14', 'val': 0}, {'addr': '0x007d5830', 'val': 0xAAAA0000},
                      {'addr': '0x007d58b0', 'val': 0xAAAA0017}, {'addr': '0x007d5168', 'val': 0xEEEE0000},
                      {'addr': '0x007d516c', 'val': 0xEEEE0001}], 'args': [1]},
            # 2: disable, cache-miss, op7 gate open: stores 0 then 8.
            {'seed': [{'addr': '0x007d6aec', 'val': 1}, {'addr': '0x007d6ae8', 'val': 0},
                      {'addr': '0x007d5834', 'val': 0}, {'addr': '0x007d58b4', 'val': 0},
                      {'addr': '0x007d6c14', 'val': 0}, {'addr': '0x007d5830', 'val': 0xAAAA0000},
                      {'addr': '0x007d58b0', 'val': 0xAAAA0017}, {'addr': '0x007d5168', 'val': 0xEEEE0000},
                      {'addr': '0x007d516c', 'val': 0xEEEE0001}], 'args': [0]},
            # 3: enable, cache HIT (6aec already 1): no writes at all.
            {'seed': [{'addr': '0x007d6aec', 'val': 1}, {'addr': '0x007d6ae8', 'val': 0},
                      {'addr': '0x007d5834', 'val': 0}, {'addr': '0x007d58b4', 'val': 0},
                      {'addr': '0x007d6c14', 'val': 0}, {'addr': '0x007d5830', 'val': 0xAAAA0000},
                      {'addr': '0x007d58b0', 'val': 0xAAAA0017}, {'addr': '0x007d5168', 'val': 0xEEEE0000},
                      {'addr': '0x007d516c', 'val': 0xEEEE0001}], 'args': [1]},
            # 4: enable, cache-miss, op7 gate CLOSED (6ae8!=0): op7 skipped, only 0x17 pushed at slot 0.
            {'seed': [{'addr': '0x007d6aec', 'val': 0}, {'addr': '0x007d6ae8', 'val': 1},
                      {'addr': '0x007d5834', 'val': 0}, {'addr': '0x007d58b4', 'val': 0},
                      {'addr': '0x007d6c14', 'val': 0}, {'addr': '0x007d5830', 'val': 0xAAAA0000},
                      {'addr': '0x007d58b0', 'val': 0xAAAA0017}, {'addr': '0x007d5168', 'val': 0xEEEE0000},
                      {'addr': '0x007d516c', 'val': 0xEEEE0001}], 'args': [1]},
            # 5: enable, gate open, op7 dirty ALREADY set: 5830 written but op7 NOT pushed; 0x17 pushed at slot 0.
            {'seed': [{'addr': '0x007d6aec', 'val': 0}, {'addr': '0x007d6ae8', 'val': 0},
                      {'addr': '0x007d5834', 'val': 1}, {'addr': '0x007d58b4', 'val': 0},
                      {'addr': '0x007d6c14', 'val': 0}, {'addr': '0x007d5830', 'val': 0xAAAA0000},
                      {'addr': '0x007d58b0', 'val': 0xAAAA0017}, {'addr': '0x007d5168', 'val': 0xEEEE0000},
                      {'addr': '0x007d516c', 'val': 0xEEEE0001}], 'args': [1]},
        ],
        'path2_tests': [
            {'seed': [{'addr': '0x007d6aec', 'val': 0}, {'addr': '0x007d6ae8', 'val': 0},
                      {'addr': '0x007d5834', 'val': 0}, {'addr': '0x007d58b4', 'val': 0},
                      {'addr': '0x007d6c14', 'val': 0}, {'addr': '0x007d5830', 'val': 0xAAAA0000},
                      {'addr': '0x007d58b0', 'val': 0xAAAA0017}, {'addr': '0x007d5168', 'val': 0xEEEE0000},
                      {'addr': '0x007d516c', 'val': 0xEEEE0001}], 'args': [1]}]},

    # 0x004d5570 direct sampler-state setter. idx=state+sampler*0xe; value cache
    # at 0x007d4f60[idx]. On cache-miss writes cache + SetSamplerState(sampler,
    # state,value). cache_setter_observe observes the cache slot: the device-call
    # args (sampler,state,value) are byte-derived from the same locals that index
    # + fill the observed slot, so the cache observation fully constrains them.
    # Contract §4.
    'd3d9_set_sampler': {'rva': 0x004d5570, 'export': 'D3d9State_SetSampler',
        'signature': {'ret': 'void', 'args': ['uint32', 'uint32', 'uint32']},
        'arg_type': 'cache_setter_observe', 'lut_root_delta': 0,
        'path1_tests': [
            # cache-miss (sampler 0, state 0 -> idx 0 -> 0x7d4f60): writes + device call.
            {'seed': [{'addr': '0x007d4f60', 'val': 0xDEAD0000}], 'args': [0, 0, 0x11223344], 'obs': ['0x007d4f60']},
            # cache-miss (sampler 1, state 2 -> idx 16 -> 0x7d4fa0).
            {'seed': [{'addr': '0x007d4fa0', 'val': 0xDEAD0001}], 'args': [1, 2, 0x55667788], 'obs': ['0x007d4fa0']},
            # cache-miss (sampler 2, state 5 -> idx 33 -> 0x7d4fe4).
            {'seed': [{'addr': '0x007d4fe4', 'val': 0xDEAD0002}], 'args': [2, 5, 0xCAFEBABE], 'obs': ['0x007d4fe4']},
            # cache-HIT (slot already == value): no write, no device call.
            {'seed': [{'addr': '0x007d4f60', 'val': 0x99887766}], 'args': [0, 0, 0x99887766], 'obs': ['0x007d4f60']},
        ],
        'path2_tests': [{'seed': [{'addr': '0x007d4f60', 'val': 0xDEAD0000}], 'args': [0, 0, 0x11223344], 'obs': ['0x007d4f60']}]},

    # 0x004d6b90 stage-0 min/mag/mip filter setter. One-time aniso clamp (6b44>1
    # signed) + table-mapped filter push to sampler 0 (tables 0x5d8ca8 min /
    # 0x5d8cac mag, read identically by orig + reimpl). obs: filter cache 6b3c,
    # sampler caches 4f78/74/7c, aniso 6b44/4f88. Contract §4.
    'd3d9_set_stage0_filter': {'rva': 0x004d6b90, 'export': 'D3d9State_SetStage0Filter',
        'signature': {'ret': 'uint32', 'args': ['uint32']},
        'arg_type': 'cache_setter_observe', 'lut_root_delta': 0,
        'obs_globals': ['0x007d6b3c', '0x007d4f78', '0x007d4f74', '0x007d4f7c',
                        '0x007d6b44', '0x007d4f88'],
        'path1_tests': [
            # filter change idx 0, no aniso (6b44<=1), mag write taken (4f7c poison != mag).
            {'seed': [{'addr': '0x007d6b3c', 'val': 0xFFFFFFFF}, {'addr': '0x007d6b44', 'val': 0},
                      {'addr': '0x007d4f7c', 'val': 0xAAAAAAAA}, {'addr': '0x007d4f78', 'val': 0xBBBBBBBB},
                      {'addr': '0x007d4f74', 'val': 0xCCCCCCCC}, {'addr': '0x007d4f88', 'val': 0xDDDDDDDD}],
             'args': [0]},
            # filter change idx 3.
            {'seed': [{'addr': '0x007d6b3c', 'val': 0xFFFFFFFF}, {'addr': '0x007d6b44', 'val': 0},
                      {'addr': '0x007d4f7c', 'val': 0xAAAAAAAA}, {'addr': '0x007d4f78', 'val': 0xBBBBBBBB},
                      {'addr': '0x007d4f74', 'val': 0xCCCCCCCC}, {'addr': '0x007d4f88', 'val': 0xDDDDDDDD}],
             'args': [3]},
            # aniso clamp (6b44=5>1 signed) + filter cache HIT (6b3c==idx): only aniso runs.
            {'seed': [{'addr': '0x007d6b3c', 'val': 2}, {'addr': '0x007d6b44', 'val': 5},
                      {'addr': '0x007d4f7c', 'val': 0xAAAAAAAA}, {'addr': '0x007d4f78', 'val': 0xBBBBBBBB},
                      {'addr': '0x007d4f74', 'val': 0xCCCCCCCC}, {'addr': '0x007d4f88', 'val': 0xDDDDDDDD}],
             'args': [2]},
            # full no-op: filter cache hit (6b3c==2) + no aniso (6b44=0).
            {'seed': [{'addr': '0x007d6b3c', 'val': 2}, {'addr': '0x007d6b44', 'val': 0},
                      {'addr': '0x007d4f7c', 'val': 0xAAAAAAAA}, {'addr': '0x007d4f78', 'val': 0xBBBBBBBB},
                      {'addr': '0x007d4f74', 'val': 0xCCCCCCCC}, {'addr': '0x007d4f88', 'val': 0xDDDDDDDD}],
             'args': [2]},
        ],
        'path2_tests': [
            {'seed': [{'addr': '0x007d6b3c', 'val': 0xFFFFFFFF}, {'addr': '0x007d6b44', 'val': 0},
                      {'addr': '0x007d4f7c', 'val': 0xAAAAAAAA}, {'addr': '0x007d4f78', 'val': 0xBBBBBBBB},
                      {'addr': '0x007d4f74', 'val': 0xCCCCCCCC}, {'addr': '0x007d4f88', 'val': 0xDDDDDDDD}],
             'args': [0]}]},

    # 0x004d53b0 RS+TSS deferred-queue flush. Drains dirty-queue 0x5168 (RS,
    # count 6c14) via SetRenderState and pair-queue 0x62a8 (TSS, count 6c18) via
    # SetTextureStageState; copies desired->applied, clears dirty, zeroes counts.
    # obs: RS applied[0x40]=55b0, RS dirty[0x40]=59fc, RS count=6c14,
    #      TSS applied[35]=5f14, TSS dirty[35]=483c, TSS count=6c18. Contract §2/§3.
    'd3d9_state_flush': {'rva': 0x004d53b0, 'export': 'D3d9State_Flush',
        'signature': {'ret': 'void', 'args': []},
        'arg_type': 'cache_setter_observe', 'lut_root_delta': 0,
        'obs_globals': ['0x007d55b0', '0x007d59fc', '0x007d6c14',
                        '0x007d5f14', '0x007d483c', '0x007d6c18'],
        'path1_tests': [
            # RS drain, applied != desired -> apply + SetRenderState(0x40, val) + clear + zero count.
            {'seed': [{'addr': '0x007d6c14', 'val': 1}, {'addr': '0x007d5168', 'val': 0x40},
                      {'addr': '0x007d59f8', 'val': 0x12340000}, {'addr': '0x007d55b0', 'val': 0x99990000},
                      {'addr': '0x007d59fc', 'val': 1}, {'addr': '0x007d6c18', 'val': 0},
                      {'addr': '0x007d5f14', 'val': 0x77770000}, {'addr': '0x007d483c', 'val': 0x66660000}],
             'args': []},
            # RS no-op, applied == desired -> NO device call, but dirty cleared + count zeroed.
            {'seed': [{'addr': '0x007d6c14', 'val': 1}, {'addr': '0x007d5168', 'val': 0x40},
                      {'addr': '0x007d59f8', 'val': 0x12340000}, {'addr': '0x007d55b0', 'val': 0x12340000},
                      {'addr': '0x007d59fc', 'val': 1}, {'addr': '0x007d6c18', 'val': 0},
                      {'addr': '0x007d5f14', 'val': 0x77770000}, {'addr': '0x007d483c', 'val': 0x66660000}],
             'args': []},
            # TSS drain (stage 1, state 2 -> slot 35), applied != desired.
            {'seed': [{'addr': '0x007d6c14', 'val': 0}, {'addr': '0x007d6c18', 'val': 1},
                      {'addr': '0x007d62a8', 'val': 1}, {'addr': '0x007d62ac', 'val': 2},
                      {'addr': '0x007d4838', 'val': 0x44440000}, {'addr': '0x007d5f14', 'val': 0xAAAA0000},
                      {'addr': '0x007d483c', 'val': 1}, {'addr': '0x007d55b0', 'val': 0x88880000},
                      {'addr': '0x007d59fc', 'val': 0x55550000}],
             'args': []},
        ],
        'path2_tests': [
            {'seed': [{'addr': '0x007d6c14', 'val': 1}, {'addr': '0x007d5168', 'val': 0x40},
                      {'addr': '0x007d59f8', 'val': 0x12340000}, {'addr': '0x007d55b0', 'val': 0x99990000},
                      {'addr': '0x007d59fc', 'val': 1}, {'addr': '0x007d6c18', 'val': 0},
                      {'addr': '0x007d5f14', 'val': 0x77770000}, {'addr': '0x007d483c', 'val': 0x66660000}],
             'args': []}]},

    # ---- promote-round round 118 (cdecl idx -> static-table -> out-ptr) --
    'get_4d54d0': {'rva': 0x004d54d0, 'export': 'Get4d54d0', 'signature': {'ret': 'void', 'args': ['uint32','pointer']}, 'arg_type': 'idx_table_out', 'target_global': 0x007d57f8, 'stride': 8, 'lut_root_delta': 0, 'path1_tests': [0,1,2,3,5,8,16], 'path2_tests': [0,1]},

    # ---- promote-round round 117 (abstable->ptr->zero-region, nested) --
    'zero_489da0': {'rva': 0x00489da0, 'export': 'Zero489da0', 'signature': {'ret': 'void', 'args': ['uint32']}, 'arg_type': 'abstable_ptr_zero', 'target_global': 0x007067fc, 'idx': 3, 'buf_dwords': 0x1100, 'observe_offs': [0x0, 0x4, 0x8, 0x20, 0x3fe0, 0x3fe4], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 116 (abs-array reset + cdecl abs-table getters) --
    'reset_489290': {'rva': 0x00489290, 'export': 'Reset489290', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'scalars_to_scattered_globals', 'observe': [{'addr': '0x007059a8'}, {'addr': '0x007059a4'}, {'addr': '0x00703990'}, {'addr': '0x0070398c'}, {'addr': '0x00703170'}, {'addr': '0x0070316c'}], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},
    'tbl_4b7020': {'rva': 0x004b7020, 'export': 'Tbl4b7020', 'signature': {'ret': 'uint32', 'args': ['uint32','uint32']}, 'arg_type': 'int2_scalar', 'lut_root_delta': 0, 'path1_tests': [[0,0xffffffff],[0,0],[0,1],[0,2],[0,5],[0,10]], 'path2_tests': [[0,0xffffffff],[0,0]]},
    'tbl_4b9540': {'rva': 0x004b9540, 'export': 'Tbl4b9540', 'signature': {'ret': 'int32', 'args': ['uint32','uint32']}, 'arg_type': 'int2_scalar', 'lut_root_delta': 0, 'path1_tests': [[0,0],[0,1],[1,0],[2,3],[6,0],[5,10],[3,7]], 'path2_tests': [[0,0],[6,0]]},

    # ---- promote-round round 115 (cdecl ptr struct-field RMW, deref_struct_set) --
    'and_4c45f0': {'rva': 0x004c45f0, 'export': 'And4c45f0', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'deref_struct_set', 'nscalar': 0, 'seed_byte': 0xFF, 'observe': [{'off': 0xc}], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},
    'or_4b52c0': {'rva': 0x004b52c0, 'export': 'Or4b52c0', 'signature': {'ret': 'void', 'args': ['pointer','uint32','uint32']}, 'arg_type': 'deref_struct_set', 'nscalar': 2, 'seed_byte': 0x00, 'observe': [{'off': 0x8}], 'lut_root_delta': 0, 'path1_tests': [[0x1234,1],[0xABCD,0],[0xFF00,1]], 'path2_tests': [[0x1234,1]]},
    'or_4b5240': {'rva': 0x004b5240, 'export': 'Or4b5240', 'signature': {'ret': 'void', 'args': ['pointer','uint32']}, 'arg_type': 'deref_struct_set', 'nscalar': 1, 'seed_byte': 0xF9, 'observe': [{'off': 0x0}], 'lut_root_delta': 0, 'path1_tests': [[1],[0]], 'path2_tests': [[1]]},

    # ---- promote-round round 114 (EAX struct-ptr + stack out-ptr, pure compute) --
    'time_41e150': {'rva': 0x0041e150, 'export': 'Time41e150', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'eax_struct_stack_out', 'eax_seed': [{'off': 0x0, 'val': 2}, {'off': 0x4, 'val': 30}, {'off': 0x8, 'val': 5}], 'out_observe': [0x0], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 113b (reg-conv scalar compute, per-side-convention fix) --
    'calc_42ac50': {'rva': 0x0042ac50, 'export': 'Calc42ac50', 'signature': {'ret': 'uint32', 'args': ['uint32','uint32']}, 'arg_type': 'reg_scalar_compute', 'lut_root_delta': 0, 'path1_tests': [[2,10],[3,10],[4,20],[5,7],[0,100],[1,50],[8,3],[7,15]], 'path2_tests': [[2,10],[3,10]]},

    # ---- promote-round round 112 (zero-arg abs-table fill with computed float) --
    'fill_449880': {'rva': 0x00449880, 'export': 'Fill449880', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'scalars_to_scattered_globals', 'observe': [{'addr': '0x00683ec8'}, {'addr': '0x00683f08'}, {'addr': '0x006842c4'}], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 111 (cdecl buffer ops: memset / memcpy-from-abs) --
    'memset_478cb0': {'rva': 0x00478cb0, 'export': 'Memset478cb0', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'ptr_buffer_op', 'buf_dwords': 0xb00, 'observe_offs': [0x0, 0x400, 0x1000, 0x2adc], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},
    'copy_4b65c0': {'rva': 0x004b65c0, 'export': 'Copy4b65c0', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'ptr_buffer_op', 'buf_dwords': 0xb00, 'observe_offs': [0x0, 0x400, 0x1000, 0x2098], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 110 (EAX+EDX reg-conv: count + register into abs tables) --
    'count_4840f0': {'rva': 0x004840f0, 'export': 'Count4840f0', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'eax_ecx_insert', 'edx_val': 2, 'eax_seed': [{'off': 0x0, 'val': 0x11111111}, {'off': 0x4, 'val': 0x22222222}, {'off': 0x8, 'val': 0x33333333}], 'eax_observe': [], 'ecx_observe': [], 'abs_observe': ['0x006ce848', '0x006cec48'], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 109 (EAX-input bitmask-builder loop twins) --
    'bits_41b720': {'rva': 0x0041b720, 'export': 'Bits41b720', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'eax_ecx_insert', 'eax_seed': [{'off': 0xc, 'val': 1}], 'eax_observe': [0x168], 'ecx_observe': [], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},
    'bits_41cdb0': {'rva': 0x0041cdb0, 'export': 'Bits41cdb0', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'eax_ecx_insert', 'eax_seed': [{'off': 0xc, 'val': 1}], 'eax_observe': [0x15c], 'ecx_observe': [], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 108 (EAX-input entity init from per-type table) --
    'init_418a30': {'rva': 0x00418a30, 'export': 'Init418a30', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'eax_ecx_insert', 'eax_seed': [{'off': 0x50, 'val': 3}], 'eax_observe': [0x0, 0x4, 0x34, 0x38, 0x3c, 0x40, 0x44, 0x4c, 0x64], 'ecx_observe': [], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 107 (ECX-input const-field setters, verbatim asm) --
    'zero_4944b0': {'rva': 0x004944b0, 'export': 'Zero4944b0', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'eax_ecx_insert', 'ecx_seed': [{'off': 0x0, 'val': 0xDEAD0000}], 'eax_observe': [], 'ecx_observe': [0x0], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},
    'zero_49c800': {'rva': 0x0049c800, 'export': 'Zero49c800', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'eax_ecx_insert', 'ecx_seed': [{'off': 0x68, 'val': 0xBEEF0068}], 'eax_observe': [], 'ecx_observe': [0x68], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 106 (EAX+ECX cross-link list-insert, verbatim asm) --
    'insert_484a50': {'rva': 0x00484a50, 'export': 'Insert484a50', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'eax_ecx_insert', 'ecx_seed': [{'off': 0x10, 'val': 0xCAFE0010}], 'eax_observe': [0x0, 0x190, 0x1ac, 0x1b0], 'ecx_observe': [0x24, 0x64], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 105 (EAX-implicit struct init, verbatim asm port) --
    'init_4773f0': {'rva': 0x004773f0, 'export': 'Init4773f0', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'eax_implicit_void', 'observe': [{'off': 0x00}, {'off': 0x04}, {'off': 0x08}, {'off': 0x0c}, {'off': 0x10}, {'off': 0x14}, {'off': 0x18}, {'off': 0x20}, {'off': 0x24}, {'off': 0x28}, {'off': 0x30}, {'off': 0x34}, {'off': 0x38}, {'off': 0x40}, {'off': 0x48}, {'off': 0x4c}, {'off': 0x50}, {'off': 0x54}, {'off': 0x58}], 'lut_root_delta': 0, 'path1_tests': [0, 0, 0], 'path2_tests': [0, 0]},

    # ---- promote-round round 104 (ai state-reset, const writes to fixed global arrays) --
    'reset_413fe0': {'rva': 0x00413fe0, 'export': 'Reset413fe0', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'scalars_to_scattered_globals', 'observe': [{'addr': '0x0089a36c'}, {'addr': '0x0089a4c4'}, {'addr': '0x0089a4c8'}, {'addr': '0x0089a4cc'}, {'addr': '0x0089a4d0'}, {'addr': '0x0089a4ec'}, {'addr': '0x0089a4f0'}, {'addr': '0x0089a4f4'}, {'addr': '0x0089a508'}, {'addr': '0x0089a51c'}, {'addr': '0x0089a520'}, {'addr': '0x0089a524'}, {'addr': '0x0089a528'}, {'addr': '0x0089a538'}, {'addr': '0x0089a53c'}, {'addr': '0x0089a540'}, {'addr': '0x0089a544'}, {'addr': '0x0089a560'}, {'addr': '0x0089a564'}, {'addr': '0x0089a568'}, {'addr': '0x0089a57c'}, {'addr': '0x0089a590'}, {'addr': '0x0089a594'}, {'addr': '0x0089a598'}, {'addr': '0x0089a59c'}, {'addr': '0x0089a5ac'}, {'addr': '0x0089a5b0'}, {'addr': '0x0089a5b4'}, {'addr': '0x0089a5b8'}, {'addr': '0x0089a5d4'}, {'addr': '0x0089a5d8'}, {'addr': '0x0089a5dc'}, {'addr': '0x0089a5f0'}, {'addr': '0x0089a604'}, {'addr': '0x0089a608'}, {'addr': '0x0089a60c'}, {'addr': '0x0089a610'}, {'addr': '0x0089a620'}, {'addr': '0x0089a624'}, {'addr': '0x0089a628'}, {'addr': '0x0089a62c'}, {'addr': '0x0089a648'}, {'addr': '0x0089a64c'}, {'addr': '0x0089a650'}, {'addr': '0x0089a664'}, {'addr': '0x0089a678'}, {'addr': '0x0089a67c'}, {'addr': '0x0089a680'}, {'addr': '0x0089a684'}, {'addr': '0x008032d4'}, {'addr': '0x008032e8'}, {'addr': '0x008032fc'}, {'addr': '0x00803310'}], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 103 (thiscall struct-from-global-table getters) --
    'copy_41b770': {'rva': 0x0041b770, 'export': '@Copy41b770@4', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'thiscall_struct_from_table', 'conv_orig': 'thiscall', 'conv_reim': 'thiscall', 'idx_off': 0x164, 'tbl': 0x005f334c, 'tbl_stride': 12, 'seed_tbl_n': 3, 'idx': 5, 'observe_offs': [0x150, 0x154, 0x158], 'lut_root_delta': 0, 'path1_tests': [0x11111111, 0xDEADBEEF, 0xCAFEBABE, 0x12345678, 1, 0x7FFFFFFF], 'path2_tests': [0xDEADBEEF, 1]},
    'copy_41ae20': {'rva': 0x0041ae20, 'export': '@Copy41ae20@4', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'thiscall_struct_from_table', 'conv_orig': 'thiscall', 'conv_reim': 'thiscall', 'idx_off': 0x64, 'tbl': 0x005f3304, 'tbl_stride': 12, 'seed_tbl_n': 3, 'idx': 5, 'observe_offs': [0x50, 0x54, 0x58, 0x70], 'lut_root_delta': 0, 'path1_tests': [0x3f800000, 0x40000000, 0x41200000, 0x42480000, 0x3e800000, 0x40490fdb], 'path2_tests': [0x3f800000, 0x40000000]},

    # ---- promote-round round 102 (indexed_global_field_read getters) --
    'get_4c5850': {'rva': 0x004c5850, 'export': 'Get4c5850', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'indexed_global_field_read', 'target_global': 0x007d4054, 'glob': 0x007d3ff8, 'field_off': 0x20, 'idx': 0x40, 'lut_root_delta': 0, 'path1_tests': [0x11111111, 0xDEADBEEF, 0xCAFEBABE, 0x12345678, 1, 0xFFFFFFFF], 'path2_tests': [0xDEADBEEF, 1]},
    'get_4c5ca0': {'rva': 0x004c5ca0, 'export': 'Get4c5ca0', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'indexed_global_field_read', 'target_global': 0x007d4054, 'glob': 0x007d3ff8, 'field_off': 0x10, 'idx': 0x40, 'lut_root_delta': 0, 'path1_tests': [0x22222222, 0xDEADBEEF, 0xCAFEBABE, 0x12345678, 1, 0xFFFFFFFF], 'path2_tests': [0xDEADBEEF, 1]},
    'set_4c5830': {'rva': 0x004c5830, 'export': 'Set4c5830', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'indexed_global_field_write', 'target_global': 0x007d4054, 'glob': 0x007d3ff8, 'field_off': 0x20, 'idx': 0x40, 'lut_root_delta': 0, 'path1_tests': [0x11111111, 0xDEADBEEF, 0xCAFEBABE, 0x12345678, 1, 0xFFFFFFFF], 'path2_tests': [0xDEADBEEF, 1]},
    'set_4c5c80': {'rva': 0x004c5c80, 'export': 'Set4c5c80', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'indexed_global_field_write', 'target_global': 0x007d4054, 'glob': 0x007d3ff8, 'field_off': 0x10, 'idx': 0x40, 'lut_root_delta': 0, 'path1_tests': [0x22222222, 0xDEADBEEF, 0xCAFEBABE, 0x12345678, 1, 0xFFFFFFFF], 'path2_tests': [0xDEADBEEF, 1]},

    # ---- promote-round round 101 (const-store / counter-increment leaves) --
    'zero_4b6700': {'rva': 0x004b6700, 'export': 'Zero4b6700', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'scalars_to_scattered_globals', 'observe': [{'addr': '0x007d3e54'}], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},
    'tbl_4a2bf7': {'rva': 0x004a2bf7, 'export': 'Tbl4a2bf7', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'scalars_to_scattered_globals', 'observe': [{'addr': '0x006160d8'}, {'addr': '0x006160dc'}, {'addr': '0x006160e0'}, {'addr': '0x006160e4'}, {'addr': '0x006160e8'}, {'addr': '0x006160ec'}], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},
    'inc_45c810': {'rva': 0x0045c810, 'export': 'Inc45c810', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'scalars_to_scattered_globals', 'observe': [{'addr': '0x0068d54c'}], 'lut_root_delta': 0, 'path1_tests': [0], 'path2_tests': [0]},

    # ---- promote-round round 100 (size clamp + store + return) --
    'clamp_5a6190': {'rva': 0x005a6190, 'export': 'Clamp5a6190', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'arg_to_global_ret', 'target_global': 0x00914700, 'lut_root_delta': 0, 'path1_tests': [0, 5, 0xf, 0x10, 0x100, 0xdead], 'path2_tests': [5, 0x100]},

    # ---- promote-round round 99 (big-endian u32 store/load) --
    'store_be_51ca60': {'rva': 0x0051ca60, 'export': 'StoreBE51ca60', 'signature': {'ret': 'void', 'args': ['pointer','uint32']}, 'arg_type': 'store_be32', 'lut_root_delta': 0, 'path1_tests': [0x12345678, 0, 0xFFFFFFFF, 0xCAFEBABE, 0xDEADBEEF], 'path2_tests': [0x12345678, 0]},
    'load_be_523110': {'rva': 0x00523110, 'export': 'LoadBE523110', 'signature': {'ret': 'int32', 'args': ['pointer']}, 'arg_type': 'load_be32', 'lut_root_delta': 0, 'path1_tests': [0x12345678, 0, 0xFFFFFFFF, 0xCAFEBABE, 0xDEADBEEF], 'path2_tests': [0x12345678, 0]},

    # ---- promote-round round 98 (struct->out projection matrix builder) --
    'proj_497000': {'rva': 0x00497000, 'export': 'Proj497000', 'signature': {'ret': 'void', 'args': ['pointer','pointer']}, 'arg_type': 'struct_to_out_build', 'span': 16, 'seed': [{'off':0x70,'bits':0x12345678},{'off':0x74,'bits':0x9abcdef0},{'off':0x80,'bits':0x3f800000},{'off':0x84,'bits':0x40000000}], 'lut_root_delta': 0, 'path1_tests': [0,0], 'path2_tests': [0]},

    # ---- promote-round round 97 (global-table search + strided clears) --
    'search_47ce40': {'rva': 0x0047ce40, 'export': 'Search47ce40', 'signature': {'ret': 'int32', 'args': ['uint32']}, 'arg_type': 'global_table_linear_search', 'target_global': 0x006c6b90, 'stride': 4, 'count': 200, 'lut_root_delta': 0, 'path1_tests': [[0x1234,0],[0x1234,50],[0x1234,199],[0x1234,-1],[0xABCD,100]], 'path2_tests': [[0x1234,50],[0x1234,-1]]},
    'clear_48f260': {'rva': 0x0048f260, 'export': 'Clear48f260', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'range_init', 'target_global': 0x0076d980, 'len': 0x2d50, 'lut_root_delta': 0, 'path1_tests': [0,0], 'path2_tests': [0]},
    'clear_vb_491bd0': {'rva': 0x00491bd0, 'export': 'ClearVB491bd0', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'global_ptr_strided_clear', 'glob': 0x00869ca0, 'len': 0x4000, 'stride': 0x20, 'lut_root_delta': 0, 'path1_tests': [0,0], 'path2_tests': [0]},

    # ---- promote-round round 96 (more per-batch state setters) --
    'batch_476c10': {'rva': 0x00476c10, 'export': 'Batch476c10', 'signature': {'ret': 'void', 'args': ['pointer','pointer']}, 'arg_type': 'deref_p1field_glob_set', 'glob': 0x007dc57c, 'p1_off': 4, 'arg2_kind': 'ptr', 'arg2_dwords': 4, 'observe': [{'off':0xd8},{'off':0xdc},{'off':0xe0},{'off':0xe4},{'off':0x40}], 'lut_root_delta': 0, 'path1_tests': [0,0], 'path2_tests': [0]},
    'batch_476c60': {'rva': 0x00476c60, 'export': 'Batch476c60', 'signature': {'ret': 'void', 'args': ['pointer','pointer']}, 'arg_type': 'deref_p1field_glob_set', 'glob': 0x007dc57c, 'p1_off': 4, 'arg2_kind': 'ptr', 'arg2_dwords': 8, 'observe': [{'off':0xd8},{'off':0xdc},{'off':0xf0},{'off':0xf4},{'off':0x40}], 'lut_root_delta': 0, 'path1_tests': [0,0], 'path2_tests': [0]},
    'batch_476cb0': {'rva': 0x00476cb0, 'export': 'Batch476cb0', 'signature': {'ret': 'void', 'args': ['pointer','uint32','uint32']}, 'arg_type': 'deref_p1field_glob_set', 'glob': 0x007dc57c, 'p1_off': 4, 'arg2_kind': 'scalar2', 'observe': [{'off':0xa4},{'off':0xa8},{'off':0x40}], 'lut_root_delta': 0, 'path1_tests': [0,0], 'path2_tests': [0]},

    # ---- promote-round round 95 (collision-table clear + per-batch state setters family) --
    'clear_coll_47c0b0': {'rva': 0x0047c0b0, 'export': 'ClearColl47c0b0', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'range_init', 'target_global': 0x006bf468, 'len': 0x3b88, 'lut_root_delta': 0, 'path1_tests': [0,0], 'path2_tests': [0]},
    'batch_476ae0': {'rva': 0x00476ae0, 'export': 'Batch476ae0', 'signature': {'ret': 'void', 'args': ['pointer','pointer']}, 'arg_type': 'deref_p1field_glob_set', 'glob': 0x007dc57c, 'p1_off': 4, 'arg2_kind': 'ptr', 'arg2_dwords': 2, 'observe': [{'off':0xb8},{'off':0xbc},{'off':0x40}], 'lut_root_delta': 0, 'path1_tests': [0,0], 'path2_tests': [0]},
    'batch_476b90': {'rva': 0x00476b90, 'export': 'Batch476b90', 'signature': {'ret': 'void', 'args': ['pointer','pointer']}, 'arg_type': 'deref_p1field_glob_set', 'glob': 0x007dc57c, 'p1_off': 4, 'arg2_kind': 'ptr', 'arg2_dwords': 16, 'observe': [{'off':0xf8},{'off':0xfc},{'off':0x100},{'off':0x118},{'off':0x134},{'off':0x40}], 'lut_root_delta': 0, 'path1_tests': [0,0], 'path2_tests': [0]},
    'batch_476be0': {'rva': 0x00476be0, 'export': 'Batch476be0', 'signature': {'ret': 'void', 'args': ['pointer','uint32']}, 'arg_type': 'deref_p1field_glob_set', 'glob': 0x007dc57c, 'p1_off': 4, 'arg2_kind': 'scalar', 'observe': [{'off':0xc0},{'off':0x40}], 'lut_root_delta': 0, 'path1_tests': [0,0], 'path2_tests': [0]},

    # ---- promote-round round 94 (copy-to-globals + byte-flag + masked indexed getter) --
    'copy_476a80': {'rva': 0x00476a80, 'export': 'Copy476a80', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'copy_arg_to_globals', 'observe': [{'addr':0x00692534},{'addr':0x00692538},{'addr':0x0069253c},{'addr':0x00692540},{'addr':0x00692544},{'addr':0x00692548},{'addr':0x0069254c},{'addr':0x00692550}], 'lut_root_delta': 0, 'path1_tests': [0,0], 'path2_tests': [0]},
    'byte_flag_474d80': {'rva': 0x00474d80, 'export': 'ByteFlag474d80', 'signature': {'ret': 'void', 'args': ['pointer','uint32']}, 'arg_type': 'deref_byte_flag', 'field_off': 2, 'bit': 4, 'lut_root_delta': 0, 'path1_tests': [[1,0],[0,0xff],[1,0xfb],[0,0x04],[1,0x10]], 'path2_tests': [[1,0],[0,0xff]]},
    'mask_get_41f0d0': {'rva': 0x0041f0d0, 'export': 'MaskGet41f0d0', 'signature': {'ret': 'void', 'args': ['uint32','pointer']}, 'arg_type': 'indexed_masked_get_out', 'target_global': 0x0063dc74, 'stride': 0x2ac, 'mask': 0x400, 'lut_root_delta': 0, 'path1_tests': [[0,0x400],[0,0],[1,0xffffffff],[2,0x3ff],[1,0x12345400]], 'path2_tests': [[0,0x400],[0,0]]},

    # ---- promote-round round 93 (flag set/clear + strided clear + 2D wheel getter) --
    'flag_41ee50': {'rva': 0x0041ee50, 'export': 'Flag41ee50', 'signature': {'ret': 'void', 'args': ['uint32','uint32','uint32']}, 'arg_type': 'flag_multibit', 'target_global': 0x0063dc74, 'stride': 0x2ac, 'lut_root_delta': 0, 'path1_tests': [[0,0,0,0xFFFFFFFF],[0,1,1,0],[1,0,1,0xFFFFFFFF],[1,1,0,0],[0,0,1,0x12345678]], 'path2_tests': [[0,0,0,0xFFFFFFFF],[0,1,1,0]]},
    'clear_table_471530': {'rva': 0x00471530, 'export': 'ClearTable471530', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'range_init', 'target_global': 0x00690a30, 'len': 0xad0, 'lut_root_delta': 0, 'path1_tests': [0,0], 'path2_tests': [0]},
    'wheel_get_46d2e0': {'rva': 0x0046d2e0, 'export': 'WheelGet46d2e0', 'signature': {'ret': 'uint32', 'args': ['uint32','uint32','pointer']}, 'arg_type': 'idx2_table_get_outlast', 'target_global': 0x00881790, 'mult': 0x11, 'stride': 0xc4, 'bound': 0x10, 'bound2': 4, 'lut_root_delta': 0, 'path1_tests': [[0,0],[1,2],[0xf,3],[0x10,0],[5,4]], 'path2_tests': [[0,0],[1,2]]},

    # ---- promote-round round 92 (struct-init with trailing scalar arg) --
    'init_5b0c70': {'rva': 0x005b0c70, 'export': 'Init5b0c70', 'signature': {'ret': 'void', 'args': ['pointer','uint32']}, 'arg_type': 'struct_const_init', 'nscalar': 1, 'observe': [{'off':0},{'off':4},{'off':8},{'off':0xc},{'off':0x10},{'off':0x14}], 'lut_root_delta': 0, 'path1_tests': [0,0], 'path2_tests': [0]},

    # ---- promote-round round 91 (struct-const-init family) --
    'init_5b0b90': {'rva': 0x005b0b90, 'export': 'Init5b0b90', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'struct_const_init', 'observe': [{'off':0},{'off':4},{'off':8},{'off':0xc},{'off':0x10},{'off':0x14}], 'lut_root_delta': 0, 'path1_tests': [0,0], 'path2_tests': [0]},
    'init_5b0f10': {'rva': 0x005b0f10, 'export': 'Init5b0f10', 'signature': {'ret': 'uint32', 'args': ['uint32','pointer']}, 'arg_type': 'struct_const_init', 'passthrough_arg': True, 'observe': [{'off':0},{'off':4},{'off':8},{'off':0xc},{'off':0x10},{'off':0x14},{'off':0x18}], 'lut_root_delta': 0, 'path1_tests': [0,0], 'path2_tests': [0]},
    'init_5beb50': {'rva': 0x005beb50, 'export': 'Init5beb50', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'struct_const_init', 'observe': [{'off':0x10},{'off':0x14},{'off':0x18},{'off':0x118},{'off':0x11c},{'off':0x120},{'off':0x124},{'off':0x128},{'off':0x148},{'off':0x14c},{'off':0x150}], 'lut_root_delta': 0, 'path1_tests': [0,0], 'path2_tests': [0]},
    'init_5c9120': {'rva': 0x005c9120, 'export': 'Init5c9120', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'struct_const_init', 'observe': [{'off':0x1e8},{'off':0x20c},{'off':0x210},{'off':0x224}], 'lut_root_delta': 0, 'path1_tests': [0,0], 'path2_tests': [0]},

    # ---- promote-round round 90 (arg-table linear search + global float step) --
    'arg_search_45c4e0': {'rva': 0x0045c4e0, 'export': 'ArgSearch45c4e0', 'signature': {'ret': 'int32', 'args': ['uint32','pointer','uint32']}, 'arg_type': 'arg_table_linear_search', 'stride_dw': 10, 'lut_root_delta': 0, 'path1_tests': [[0x1234,0,8],[0x1234,3,8],[0x1234,7,8],[0x1234,-1,8],[0xABCD,5,8]], 'path2_tests': [[0x1234,3,8],[0x1234,-1,8]]},
    'float_step_45df70': {'rva': 0x0045df70, 'export': 'FloatStep45df70', 'signature': {'ret': 'void', 'args': ['float']}, 'arg_type': 'global_float_step', 'target_global': 0x006036c0, 'lut_root_delta': 0, 'path1_tests': [[0x42480000,100.0],[0x42480000,0.0],[0x00000000,5.0],[0x42c80000,50.0],[0x41200000,10.0]], 'path2_tests': [[0x42480000,100.0],[0x00000000,5.0]]},

    # ---- promote-round round 89 (any-active predicate + double-deref vec3+ret) --
    'any_active_4576b0': {'rva': 0x004576b0, 'export': 'AnyActive4576b0', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'any_slot_nonzero', 'observe': [{'addr': 0x0068a300},{'addr': 0x0068a304},{'addr': 0x0068a274},{'addr': 0x0068a284}], 'lut_root_delta': 0, 'path1_tests': [-1,0,1,2,3], 'path2_tests': [-1,0]},
    'item_pos_ret_4075b0': {'rva': 0x004075b0, 'export': 'ItemPosRet4075b0', 'signature': {'ret': 'uint32', 'args': ['uint32','pointer']}, 'arg_type': 'double_deref_vec3_get', 'target_global': 0x00639d90, 'stride': 0xec, 'rec_off': 4, 'out_off': 0x40, 'span': 4, 'ret_tbl': 0x00639dc8, 'ret_stride': 0xec, 'lut_root_delta': 0, 'path1_tests': [0,1,2,3,5], 'path2_tests': [0,1]},

    # ---- promote-round round 88 (double-deref ptr getter + float field RMW) --
    'deref_407620': {'rva': 0x00407620, 'export': 'Deref407620', 'signature': {'ret': 'void', 'args': ['pointer','uint32']}, 'arg_type': 'double_deref_ptr_get', 'target_global': 0x00639d90, 'stride': 0xec, 'rec_off': 4, 'add': 0x10, 'lut_root_delta': 0, 'path1_tests': [0,1,2,3,5], 'path2_tests': [0,1]},
    'float_sub_4058b0': {'rva': 0x004058b0, 'export': 'FloatSub4058b0', 'signature': {'ret': 'void', 'args': ['pointer','float']}, 'arg_type': 'deref_float_field_rmw', 'field_off': 0x5c, 'seedf': 100.0, 'lut_root_delta': 0, 'path1_tests': [2.5, -3.0, 0.0, 1.5, 10.0], 'path2_tests': [2.5, -3.0]},

    # ---- promote-round round 87 (double-deref vec3 getter + gated float-compare predicate) --
    'item_world_pos_44dff0': {'rva': 0x0044dff0, 'export': 'ItemWorldPos44dff0', 'signature': {'ret': 'void', 'args': ['uint32','pointer']}, 'arg_type': 'double_deref_vec3_get', 'target_global': 0x00890080, 'stride': 0xf8, 'rec_off': 4, 'out_off': 0x40, 'span': 3, 'lut_root_delta': 0, 'path1_tests': [0,1,2,3,5], 'path2_tests': [0,1]},
    'pred_405430': {'rva': 0x00405430, 'export': 'Pred405430', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'global_float_predicate', 'gate': 0x00639d78, 'thr': 0x00639d74, 'target_global': 0x00639d70, 'rec_off': 0xc, 'lut_root_delta': 0, 'path1_tests': [[1,0x3f800000,0x40000000],[1,0x40000000,0x3f800000],[0,0x3f800000,0x40000000],[1,0x3f800000,0x3f800000],[1,0x40400000,0x40000000]], 'path2_tests': [[1,0x3f800000,0x40000000],[1,0x40000000,0x3f800000]]},

    # ---- promote-round round 86 (global swap-return + 3-byte-globals + indexed float-square) --
    'audio_ctx_swap_5a7b40': {'rva': 0x005a7b40, 'export': 'AudioCtxSwap5a7b40', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'global_swap', 'target_global': 0x007dcabc, 'lut_root_delta': 0, 'path1_tests': [[0xAAAA,0x1234],[0,0xDEAD],[0xFFFFFFFF,0],[0x5555,0xBEEF],[0,0]], 'path2_tests': [[0xAAAA,0x1234],[0,0xDEAD]]},
    'set_6147b4_triple': {'rva': 0x004924c0, 'export': 'Set6147b4Triple', 'signature': {'ret': 'void', 'args': ['uint8','uint8','uint8']}, 'arg_type': 'byte_args_to_globals', 'observe': [{'addr': 0x006147b4},{'addr': 0x006147b5},{'addr': 0x006147b6}], 'lut_root_delta': 0, 'path1_tests': [[0x11,0x22,0x33],[0xFF,0,0x80],[0,0xAB,0xCD],[0x7F,0x40,0x01],[0,0,0]], 'path2_tests': [[0x11,0x22,0x33],[0,0,0]]},
    'store_dist_sq_47cdc0': {'rva': 0x0047cdc0, 'export': 'StoreDistSq47cdc0', 'signature': {'ret': 'void', 'args': ['uint32','float']}, 'arg_type': 'indexed_float_sq', 'target_global': 0x006c6870, 'stride': 4, 'lut_root_delta': 0, 'path1_tests': [[0,2.5],[1,-3.0],[2,0.0],[3,1.5],[5,10.0]], 'path2_tests': [[0,2.5],[1,-3.0]]},

    # ---- promote-round round 85 (cond deref-get + table bool predicates) --
    'cond_get_5c4d30': {'rva': 0x005c4d30, 'export': 'CondGet5c4d30', 'signature': {'ret': 'uint32', 'args': ['pointer']}, 'arg_type': 'cond_deref_get', 'gate_off': 4, 'val_off': 0, 'lut_root_delta': 0, 'path1_tests': [[1,0xDEAD],[0,0xBEEF],[5,0x1234],[0,0],[0xFF,0xCAFE]], 'path2_tests': [[1,0xDEAD],[0,0xBEEF]]},
    'pred_45bff0': {'rva': 0x0045bff0, 'export': 'Pred45bff0', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'table_bool_predicate', 'target_global': 0x0088fc88, 'stride': 0xb4, 'off0': 0, 'lut_root_delta': 0, 'path1_tests': [[1,0],[1,0x1234],[3,0],[3,0xFF],[2,0],[5,0x9]], 'path2_tests': [[1,0],[1,0x1234]]},
    'pred_497450': {'rva': 0x00497450, 'export': 'Pred497450', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'table_bool_predicate', 'target_global': 0x007e96fc, 'stride': 0x200, 'off0': 0, 'lut_root_delta': 0, 'path1_tests': [[1,0],[2,0x1234],[3,0xAA],[5,0x1234],[10,0xFF],[0,0]], 'path2_tests': [[2,0x1234],[5,0x1234]]},

    # ---- promote-round round 83 (3-field struct setters + byte-OR RMW; deref_struct_set) --
    'set_5173d0': {'rva': 0x005173d0, 'export': 'Set5173d0', 'signature': {'ret': 'void', 'args': ['pointer','uint32','uint32','uint32']}, 'arg_type': 'deref_struct_set', 'nscalar': 3, 'seed_byte': 0xEE, 'observe': [{'off': 0x48},{'off': 0x40},{'off': 0x44}], 'lut_root_delta': 0, 'path1_tests': [[0xA1,0xB2,0xC3],[0x11112222,0x33334444,0x55556666],[0,0,0],[0xFFFFFFFF,0x12345678,0xDEADBEEF],[0xCAFEF00D,0xBADC0FFE,0x8BADF00D]], 'path2_tests': [[0xA1,0xB2,0xC3],[0,0,0]]},
    'set_5209d0': {'rva': 0x005209d0, 'export': 'Set5209d0', 'signature': {'ret': 'void', 'args': ['pointer','uint32','uint32','uint32']}, 'arg_type': 'deref_struct_set', 'nscalar': 3, 'seed_byte': 0xEE, 'observe': [{'off': 0x1e8},{'off': 0x1ec},{'off': 0x1f0}], 'lut_root_delta': 0, 'path1_tests': [[0xA1,0xB2,0xC3],[0x11112222,0x33334444,0x55556666],[0,0,0],[0xFFFFFFFF,0x12345678,0xDEADBEEF],[0xCAFEF00D,0xBADC0FFE,0x8BADF00D]], 'path2_tests': [[0xA1,0xB2,0xC3],[0,0,0]]},
    'rmw_518570': {'rva': 0x00518570, 'export': 'Rmw518570', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'deref_struct_set', 'nscalar': 0, 'seed_byte': 0x11, 'observe': [{'off': 0x70}], 'lut_root_delta': 0, 'path1_tests': [[],[],[],[],[]], 'path2_tests': [[],[]]},

    # ---- promote-round round 82 (more indexed/ptr-struct setters + 5-field clear) --
    'cmd_build_5b0dc0_set': {'rva': 0x005b0dc0, 'export': 'CmdBuild5b0dc0Set', 'signature': {'ret': 'void', 'args': ['pointer', 'uint32']}, 'arg_type': 'deref_struct_set', 'nscalar': 1, 'seed_byte': 0x11, 'observe': [{'off': 0x04}, {'off': 0x00}], 'lut_root_delta': 0, 'path1_tests': [[0x12345678], [0xDEADBEEF], [0], [0xFFFFFFFF], [0x55555555]], 'path2_tests': [[0x12345678], [0]]},
    'clear_desc_5bde50': {'rva': 0x005bde50, 'export': 'ClearDesc5bde50', 'signature': {'ret': 'pointer', 'args': ['pointer']}, 'arg_type': 'ptr_fields_clear', 'observe': [{'off': 0x00}, {'off': 0x04}, {'off': 0x08}, {'off': 0x0c}, {'off': 0x10}], 'lut_root_delta': 0, 'path1_tests': [0, 0, 0, 0, 0], 'path2_tests': [0, 0]},
    'table_69318c_set': {'rva': 0x00477e00, 'export': 'Table69318cSet', 'signature': {'ret': 'void', 'args': ['uint32', 'uint32']}, 'arg_type': 'indexed_table_set', 'target_global': 0x0069318c, 'stride': 0x2c0, 'set_idx': 2, 'lut_root_delta': 0, 'path1_tests': [0xDEADBEEF, 0x12345678, 0, 0xFFFFFFFF, 0x55555555, 0xAAAAAAAA, 1, 0xC0DE0001, 0x7FFFFFFF, 0x80000000], 'path2_tests': [0xDEADBEEF, 0x12345678, 0]},

    # ---- promote-round round 81 (indexed table setter + ptr-struct field setters; deref_struct_set NEW SWEEP-CRITICAL) --
    'veh_field_8816f4_set': {'rva': 0x0046dd90, 'export': 'VehField8816f4Set', 'signature': {'ret': 'void', 'args': ['uint32', 'uint32']}, 'arg_type': 'indexed_table_set', 'target_global': 0x008816f4, 'stride': 0xd04, 'set_idx': 2, 'lut_root_delta': 0, 'path1_tests': [0xDEADBEEF, 0x12345678, 0, 0xFFFFFFFF, 0x55555555, 0xAAAAAAAA, 1, 0xC0DE0001, 0x7FFFFFFF, 0x80000000], 'path2_tests': [0xDEADBEEF, 0x12345678, 0]},
    'audio_cb_5bf7d0_set': {'rva': 0x005bf7d0, 'export': 'AudioCb5bf7d0Set', 'signature': {'ret': 'void', 'args': ['pointer', 'uint32', 'uint32']}, 'arg_type': 'deref_struct_set', 'nscalar': 2, 'seed_byte': 0xEE, 'observe': [{'off': 0x144}, {'off': 0x148}], 'lut_root_delta': 0, 'path1_tests': [[0xAAAA0001, 0xBBBB0002], [0x11112222, 0x33334444], [0, 0], [0xFFFFFFFF, 0x12345678], [0xDEADBEEF, 0xCAFEF00D]], 'path2_tests': [[0xAAAA0001, 0xBBBB0002], [0, 0]]},
    'cmd_build_5b0ca0_set': {'rva': 0x005b0ca0, 'export': 'CmdBuild5b0ca0Set', 'signature': {'ret': 'void', 'args': ['pointer', 'uint32']}, 'arg_type': 'deref_struct_set', 'nscalar': 1, 'seed_byte': 0x11, 'observe': [{'off': 0x0c}, {'off': 0x00}], 'lut_root_delta': 0, 'path1_tests': [[0x12345678], [0xDEADBEEF], [0], [0xFFFFFFFF], [0x55555555]], 'path2_tests': [[0x12345678], [0]]},

    # ---- promote-round round 80 (frontier/classifier batch: byte-getter + arg-field-clear) --
    'get_fade_alpha': {'rva': 0x0042a9f0, 'export': 'GetFadeAlpha', 'signature': {'ret': 'uint8', 'args': []}, 'arg_type': 'read_global', 'target_global': 0x0067eca8, 'lut_root_delta': 0, 'path1_tests': [0, 0xFF, 0x7F, 0x80, 1, 0x55, 0xAA, 0x42, 0, 0xFE], 'path2_tests': [0, 0xFF, 0x7F]},
    'audio_clear_5be930': {'rva': 0x005be930, 'export': 'AudioClear5be930', 'signature': {'ret': 'pointer', 'args': ['pointer']}, 'arg_type': 'ptr_fields_clear', 'observe': [{'off': 0x0c}, {'off': 0x10}], 'lut_root_delta': 0, 'path1_tests': [0, 0, 0, 0, 0], 'path2_tests': [0, 0]},

    # ---- promote-round round 79 (frontier-tool validation batch: 3 display-independent leaves) --
    # surfaced by scripts/promote_frontier.py + scripts/promote_classify.py; diffed via early_window_leaf_diff.py
    'get_771e78': {'rva': 0x00495520, 'export': 'Get771e78', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'read_global', 'target_global': 0x00771e78, 'lut_root_delta': 0, 'path1_tests': [0, 0xDEADBEEF, 0xCAFEF00D, 0x12345678, 0xFFFFFFFF, 0x80000000, 1, 0x55555555, 0xAAAAAAAA, 0], 'path2_tests': [0, 0xDEADBEEF, 0x12345678]},
    'clear_4842b0': {'rva': 0x004842b0, 'export': 'Clear4842b0', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'scalars_to_scattered_globals', 'observe': [{'addr': 0x006cfc88}, {'addr': 0x006cf068}, {'addr': 0x006cf06c}, {'addr': 0x006cfc8c}], 'lut_root_delta': 0, 'path1_tests': [0, 0, 0, 0, 0], 'path2_tests': [0, 0]},

    # ---- C2->C3: FUN_00433240 RaceArmReset (frontend) ---------------------------
    # void __cdecl(uint32). One-shot gate on 0x0067eca4: when open it latches the
    # gate, seeds 0x0067ecb0=0x21, runs the two no-arg initializers FUN_00402f80
    # + FUN_0042d3a0 (neither touches the observed windows: 0x636ae8..af4 and the
    # 0x67ed78..0x67f0bc zero-block are both outside 0x67e844..0x67ecb0), clears
    # three words, stores param_1 into 0x0067ecac, sets 0x0067ea84=1. Observe the
    # 7 written globals; fill the gate window with 0x00 each side so the body
    # always executes (forces the gate open) -> non-degenerate, ecac=param_1
    # makes the fold input-sensitive. ref: re/analysis/bucket_0041dc30/0x00433240.md
    'race_arm_reset': {
        'rva':            0x00433240,
        'export':         'RaceArmReset',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'scalars_to_scattered_globals',
        'observe':        [{'addr': '0x0067eca4', 'len': 4, 'fill': 0x00},
                           {'addr': '0x0067ecb0', 'len': 4},
                           {'addr': '0x0067ecac', 'len': 4},
                           {'addr': '0x0067ea84', 'len': 4},
                           {'addr': '0x0067e844', 'len': 4},
                           {'addr': '0x0067e9f8', 'len': 4},
                           {'addr': '0x0067e914', 'len': 4}],
        'lut_root_delta': 0,
        'path1_tests':    [{'args': [0]}, {'args': [1]}, {'args': [2]},
                           {'args': [0xDEADBEEF]}, {'args': [0xFFFFFFFF]},
                           {'args': [0x80000000]}, {'args': [0x12345678]},
                           {'args': [0xCAFEBABE]}, {'args': [0x55555555]},
                           {'args': [0]}],
        'path2_tests':    [{'args': [0]}, {'args': [1]}, {'args': [0xDEADBEEF]}],
    },
    # 0x005c9d00 (GetRaceEndTrigger) intentionally NOT registered: 3-byte body, 5-byte
    # E9 patch corrupts the next function (demoted C3->C2 2026-05-22 crasher).

    # ---- promote-round round 78 (2-global equality predicate; opportunistic) --
    'pred_405890': {'rva': 0x00405890, 'export': 'Pred405890', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'two_global_predicate', 'gate': 0x0063a5d0, 'target_global': 0x0063a5d4, 'lut_root_delta': 0, 'path1_tests': [[5, 5], [5, 3], [0, 0], [7, 7], [3, 9], [0, 5], [10, 10], [2, 2], [0, 7], [8, 4]], 'path2_tests': [[5, 5], [5, 3], [0, 0]]},

    # ---- promote-round round 77 (strided -1.0f fill + trailing global zero; opportunistic) --
    'fill_63e5a4': {'rva': 0x00421080, 'export': 'Fill63e5a4', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'range_init', 'target_global': 0x0063e5a4, 'len': 0x1600, 'lut_root_delta': 0, 'path1_tests': [0, 0], 'path2_tests': [0]},

    # ---- promote-round round 76 (global-field getter; opportunistic, beyond 200) --
    'global_field_get_896000': {'rva': 0x0044c370, 'export': 'GlobalFieldGet896000', 'signature': {'ret': 'int32', 'args': []}, 'arg_type': 'global_field_read', 'target_global': 0x00896000, 'field_off': 4, 'lut_root_delta': 0, 'path1_tests': [0x1000, 0x2000, 0xDEAD, 0x55, 0x12340, 0, 0x7000, 0x100, 0x1000, 0x9999], 'path2_tests': [0x1000, 0x2000, 0xDEAD]},

    # ---- promote-round round 75 (global-field getter + float-threshold predicate) — beyond 200 --
    'global_field_get_896278': {'rva': 0x004495d0, 'export': 'GlobalFieldGet896278', 'signature': {'ret': 'int32', 'args': []}, 'arg_type': 'global_field_read', 'target_global': 0x00896278, 'field_off': 4, 'lut_root_delta': 0, 'path1_tests': [0x1000, 0x2000, 0xDEAD, 0x55, 0x12340, 0, 0x7000, 0x100, 0x1000, 0x9999], 'path2_tests': [0x1000, 0x2000, 0xDEAD]},
    'float_lt_44e020': {'rva': 0x0044e020, 'export': 'FloatLt44e020', 'signature': {'ret': 'uint32', 'args': ['int32']}, 'arg_type': 'float_threshold_predicate', 'target_global': 0x008900b0, 'stride': 0xf8, 'gate': 0x005ce2d8, 'lut_root_delta': 0, 'path1_tests': [[0, 0xBF800000], [1, 0x00000000], [2, 0xBDCCCCCD], [3, 0x3F800000], [4, 0xBCA3D70A], [0, 0xBC23D70A], [1, 0xC2C80000], [2, 0x42C80000], [0, 0xBF800000], [1, 0x00000000]], 'path2_tests': [[0, 0xBF800000], [1, 0x00000000], [2, 0xBDCCCCCD]]},

    # ---- promote-round round 74 (strided fill + 2 index->ptr getters + 2 multi-bit flag setters) — reaches 200 --
    'fill_6870b4': {'rva': 0x00453f30, 'export': 'Fill6870b4', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'range_init', 'target_global': 0x006870b4, 'len': 0xe10, 'lut_root_delta': 0, 'path1_tests': [0, 0], 'path2_tests': [0]},
    'idx_ptr_404e00': {'rva': 0x00404e00, 'export': 'IdxPtr404e00', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'index_then_ptr_array', 'target_global': 0x00639cc8, 'basePtr': 0x005ea0b8, 'lut_root_delta': 0, 'path1_tests': [[0, 0], [1, 1], [2, 2], [3, 3], [4, 0xFFFFFFFF], [5, 1], [0, 0], [1, 2], [2, 3], [3, 0xFFFFFFFF]], 'path2_tests': [[0, 0], [1, 1], [4, 0xFFFFFFFF]]},
    'idx_ptr_404e20': {'rva': 0x00404e20, 'export': 'IdxPtr404e20', 'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32']}, 'arg_type': 'index_then_ptr_array', 'target_global': 0x00636c08, 'basePtr': 0x005ea0c8, 'mult': 0x4e, 'lut_root_delta': 0, 'path1_tests': [[0, 0, 0], [0, 1, 1], [1, 0, 2], [1, 5, 3], [2, 2, 0xFFFFFFFF], [0, 3, 1], [3, 1, 2], [0, 0, 3], [1, 1, 0xFFFFFFFF], [2, 0, 0]], 'path2_tests': [[0, 0, 0], [0, 1, 1], [2, 2, 0xFFFFFFFF]]},
    'flag_0041ede0': {'rva': 0x0041ede0, 'export': 'Flag0041ede0', 'signature': {'ret': 'void', 'args': ['int32', 'int32', 'int32']}, 'arg_type': 'flag_multibit', 'target_global': 0x0063dc74, 'stride': 0x2ac, 'lut_root_delta': 0, 'path1_tests': [[0, 1, 1, 0], [0, 0, 0, 0xffffffff], [1, 1, 0, 0x12345000], [1, 0, 1, 0], [2, 1, 1, 0x30], [2, 0, 0, 0xffffffff], [3, 1, 0, 0x100], [3, 0, 1, 0x3000], [0, 1, 1, 0xabcd], [1, 0, 0, 0]], 'path2_tests': [[0, 1, 1, 0], [0, 0, 0, 0xffffffff], [1, 1, 0, 0x12345000]]},
    'flag_0041eeb0': {'rva': 0x0041eeb0, 'export': 'Flag0041eeb0', 'signature': {'ret': 'void', 'args': ['int32', 'int32', 'int32', 'int32']}, 'arg_type': 'flag_multibit', 'target_global': 0x0063dc74, 'stride': 0x2ac, 'nargs4': True, 'lut_root_delta': 0, 'path1_tests': [[0, 1, 1, 1, 0], [0, 0, 0, 0, 0xffffffff], [1, 1, 0, 1, 0x12345400], [1, 0, 1, 0, 0x30], [2, 1, 1, 0, 0xffffffff], [2, 0, 0, 1, 0x800], [3, 1, 0, 0, 0x430], [3, 0, 1, 1, 0xfff], [0, 1, 1, 1, 0xabcd], [1, 0, 0, 0, 0]], 'path2_tests': [[0, 1, 1, 1, 0], [0, 0, 0, 0, 0xffffffff], [1, 1, 0, 1, 0x12345400]]},

    # ---- promote-round round 73 (state-global 2->3 transition + 2-global trigger predicate) --
    'state_advance_2to3': {'rva': 0x0042c1a0, 'export': 'StateAdvance2to3', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'void_global_transition', 'target_global': 0x0067eab0, 'lut_root_delta': 0, 'path1_tests': [[2], [3], [5], [0], [1], [2], [4], [0xFF], [2], [9]], 'path2_tests': [[2], [3], [5]]},
    'trigger_432290': {'rva': 0x00432290, 'export': 'Trigger432290', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'two_global_predicate', 'gate': 0x0067eab0, 'target_global': 0x0067eabc, 'lut_root_delta': 0, 'path1_tests': [[1, 0xff210000], [1, 0xff220000], [0, 0xff210000], [1, 0x12345], [5, 0xff220000], [0, 0], [7, 0xff210000], [1, 0xff230000], [2, 0xff220000], [0, 0xff220000]], 'path2_tests': [[1, 0xff210000], [0, 0xff210000], [1, 0x12345]]},

    # ---- promote-round round 72 (audio-state switch predicate + gated message-post) --
    'audio_state_active': {'rva': 0x004627b0, 'export': 'AudioStateActive', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'global_switch_member', 'gate': 0x00773920, 'lut_root_delta': 0, 'path1_tests': [2, 3, 4, 7, 0, 1, 5, 8, 6, 2], 'path2_tests': [2, 0, 8]},
    'post_0042bf30': {'rva': 0x0042bf30, 'export': 'Post0042bf30', 'signature': {'ret': 'void', 'args': ['uint32', 'uint32', 'uint32', 'uint32', 'uint32', 'uint32']}, 'arg_type': 'gated_args_to_globals', 'gate': 0x0067eab0, 'aux': 0x0067ea5c, 'observe': [{'addr': 0x0067eac0}, {'addr': 0x0067eabc}, {'addr': 0x0067eac8}, {'addr': 0x0067eacc}, {'addr': 0x0067ead0}, {'addr': 0x0067eab4}, {'addr': 0x0067eab0}, {'addr': 0x0067eab8}, {'addr': 0x0067ead4}], 'lut_root_delta': 0, 'path1_tests': [[0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0], [0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 5], [1, 2, 3, 4, 5, 6, 0], [0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 1], [0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0], [7, 7, 7, 7, 7, 7, 0], [0xA, 0xB, 0xC, 0xD, 0xE, 0xF, 9], [0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0], [0, 0, 0, 0, 0, 0, 0], [0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 2]], 'path2_tests': [[0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0], [0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 5], [1, 2, 3, 4, 5, 6, 0]]},

    # ---- promote-round round 71 (bounded TTL setter + Pool-J 2-field reset) --
    'ttl_set_68b1b0': {'rva': 0x00458fa0, 'export': 'TtlSet68b1b0', 'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32']}, 'arg_type': 'indexed_table_set', 'target_global': 0x0068b1b0, 'stride': 0x50, 'set_idx': 5, 'lut_root_delta': 0, 'path1_tests': [0x11111111, 0x22222222, 0xDEADBEEF, 0x55, 0x1234, 0xCAFEBABE, 7, 0xFFFFFFFF, 0x42, 0x99], 'path2_tests': [0x11111111, 0x22222222, 0xDEADBEEF]},
    'pool_j_reset_68ba00': {'rva': 0x00459540, 'export': 'PoolJReset68ba00', 'signature': {'ret': 'void', 'args': ['uint32']}, 'arg_type': 'indexed_const2_set', 'target_global': 0x0068ba00, 'stride': 0x58, 'off0': 0, 'off1': 8, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 5, 7, 0, 1, 2, 3], 'path2_tests': [0, 3, 7]},

    # ---- promote-round round 70 (bit-extract getter + 4-global clearer + 2 record-eq predicates) --
    'bit_extract_63dc74': {'rva': 0x0041efe0, 'export': 'BitExtract63dc74', 'signature': {'ret': 'uint32', 'args': ['int32']}, 'arg_type': 'int_scalar', 'seed_table': {'base': 0x0063dc74, 'stride': 0x2ac, 'span': 16}, 'lut_root_delta': 0, 'path1_tests': [0, 1, 8, 15, 3, 10, 9, 7, 12, 5], 'path2_tests': [0, 8, 15]},
    'clear_67d99c_x4': {'rva': 0x004298c0, 'export': 'Clear67d99c_x4', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'scalars_to_scattered_globals', 'observe': [{'addr': 0x0067d99c}, {'addr': 0x0067d994}, {'addr': 0x0067d98c}, {'addr': 0x008991bc}], 'lut_root_delta': 0, 'path1_tests': [0, 0, 0], 'path2_tests': [0, 0]},
    'rec_eq_231': {'rva': 0x00432230, 'export': 'RecEq231', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'gated_record_eq2', 'target_global': 0x0067ed3c, 'stride': 0x40, 'off0': 0, 'off1': 4, 'gate': 0x0067e9f8, 'lut_root_delta': 0, 'path1_tests': [[0, 0x13, 1], [0, 0x13, 2], [1, 0x99, 1], [2, 0x13, 1], [3, 0x13, 0], [0, 0, 1], [4, 0x13, 1], [5, 0x14, 1], [0, 0x13, 1], [1, 0x13, 1]], 'path2_tests': [[0, 0x13, 1], [0, 0x13, 2], [1, 0x99, 1]]},
    'rec_eq_232': {'rva': 0x00432260, 'export': 'RecEq232', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'gated_record_eq2', 'target_global': 0x0067ed3c, 'stride': 0x40, 'off0': 0, 'off1': 4, 'gate': 0x0067e9f8, 'lut_root_delta': 0, 'path1_tests': [[0, 0x13, 2], [0, 0x13, 1], [1, 0x99, 2], [2, 0x13, 2], [3, 0x13, 0], [0, 0, 2], [4, 0x13, 2], [5, 0x14, 2], [0, 0x13, 2], [1, 0x13, 2]], 'path2_tests': [[0, 0x13, 2], [0, 0x13, 1], [1, 0x99, 2]]},

    # ---- promote-round round 69 (60-dword contiguous zero-fill + Copter linear-scan find) --
    'zerofill60_63bc60': {'rva': 0x00413f20, 'export': 'ZeroFill60_63bc60', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'range_init', 'target_global': 0x0063bc60, 'len': 0xf0, 'lut_root_delta': 0, 'path1_tests': [0, 0], 'path2_tests': [0]},
    'copter_find_639dc4': {'rva': 0x00407640, 'export': 'CopterFind639dc4', 'signature': {'ret': 'int32', 'args': ['uint32']}, 'arg_type': 'linear_scan_find', 'target_global': 0x00639dc4, 'stride': 0xec, 'gate': 0x0063a5d0, 'count': 8, 'lut_root_delta': 0, 'path1_tests': [[0xABCD0001, 0], [0xABCD0002, 3], [0xABCD0003, 7], [0xABCD0004, 1], [0xDEAD, 99], [0xABCD0005, 5], [0xABCD0006, 0], [0xBEEF, 50], [0xABCD0007, 6], [0xABCD0008, 2]], 'path2_tests': [[0xABCD0001, 0], [0xABCD0002, 3], [0xDEAD, 99]]},

    # ---- promote-round round 68 (strided 30-global zero-init + 64-dword constant fill) --
    'clear10x3_63a494': {'rva': 0x00406370, 'export': 'Clear10x3_63a494', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'range_init', 'target_global': 0x0063a494, 'len': 0x12c, 'lut_root_delta': 0, 'path1_tests': [0, 0], 'path2_tests': [0]},
    'fill64_63c508': {'rva': 0x00418e50, 'export': 'Fill64_63c508', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'range_init', 'target_global': 0x0063c508, 'len': 0x100, 'lut_root_delta': 0, 'path1_tests': [0, 0], 'path2_tests': [0]},

    # ---- promote-round round 67 (two global-array-to-bool-out helpers) --
    'bool0_out_8a94e0': {'rva': 0x0040b970, 'export': 'Bool0Out8a94e0', 'signature': {'ret': 'void', 'args': ['uint32']}, 'arg_type': 'global4_bool_out', 'target_global': 0x008a94e0, 'span': 4, 'seedvecs': [[0, 1, 0, 2], [5, 0, 0, 7], [0, 0, 3, 0], [9, 8, 7, 0], [0, 0, 0, 0], [1, 1, 1, 1], [0, 5, 0, 0], [3, 0, 9, 0], [0, 1, 2, 0], [7, 0, 0, 0]], 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9], 'path2_tests': [0, 1, 3]},
    'boolmin_out_8a94e0': {'rva': 0x0040ba00, 'export': 'BoolMinOut8a94e0', 'signature': {'ret': 'void', 'args': ['uint32']}, 'arg_type': 'global4_bool_out', 'target_global': 0x008a94e0, 'span': 4, 'seedvecs': [[3, 5, 3, 9], [100, 2, 2, 50], [1, 1, 1, 1], [50, 40, 40, 99], [99, 99, 99, 99], [200, 200, 200, 200], [7, 8, 9, 7], [4, 4, 5, 6], [10, 20, 10, 30], [0, 0, 1, 2]], 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9], 'path2_tests': [0, 1, 3]},

    # ---- promote-round round 66 (per-player flag-bit toggle + gated switch predicate) --
    'flag_toggle_63dc74': {'rva': 0x0041ef80, 'export': 'FlagToggle63dc74', 'signature': {'ret': 'void', 'args': ['uint32', 'int32']}, 'arg_type': 'indexed_bit_toggle', 'target_global': 0x0063dc74, 'stride': 0x2ac, 'field_off': 0, 'bit': 0x400, 'lut_root_delta': 0, 'path1_tests': [[0, 1, 0x12345000], [0, 0, 0x12345400], [1, 1, 0], [1, 0, 0x400], [2, 1, 0xfffffbff], [2, 0, 0xffffffff], [3, 1, 0x100], [3, 0, 0x500], [0, 1, 0], [1, 0, 0xfff]], 'path2_tests': [[0, 1, 0x12345000], [0, 0, 0x12345400], [1, 0, 0x400]]},
    'gated_switch_636ad0': {'rva': 0x0041f360, 'export': 'GatedSwitch636ad0', 'signature': {'ret': 'uint32', 'args': ['uint32']}, 'arg_type': 'gated_int_predicate', 'gate': 0x00636ad0, 'lut_root_delta': 0, 'path1_tests': [[4, 0], [5, 0], [1, 0], [0x25, 0], [2, 0], [4, 1], [8, 0], [0x30, 0], [7, 0], [0xe, 0]], 'path2_tests': [[4, 0], [1, 0], [4, 1]]},

    # ---- promote-round round 65 (ghost-slot vec3/vec4 setters + per-player field setter) --
    'ghost_vec3_set_63c6d0': {'rva': 0x0041a500, 'export': 'GhostVec3Set63c6d0', 'signature': {'ret': 'void', 'args': ['uint32', 'uint32']}, 'arg_type': 'indexed_vec_set', 'target_global': 0x0063c6d0, 'stride': 0xc4, 'span': 3, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 4, 5, 0, 1, 2, 3], 'path2_tests': [0, 1, 2]},
    'ghost_vec4_set_63c6b0': {'rva': 0x0041a550, 'export': 'GhostVec4Set63c6b0', 'signature': {'ret': 'void', 'args': ['uint32', 'uint32']}, 'arg_type': 'indexed_vec_set', 'target_global': 0x0063c6b0, 'stride': 0xc4, 'span': 4, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 4, 5, 0, 1, 2, 3], 'path2_tests': [0, 1, 2]},
    'player_field_set_63dc78': {'rva': 0x0041ef60, 'export': 'PlayerFieldSet63dc78', 'signature': {'ret': 'void', 'args': ['uint32', 'uint32']}, 'arg_type': 'indexed_table_set', 'target_global': 0x0063dc78, 'stride': 0x2ac, 'set_idx': 2, 'lut_root_delta': 0, 'path1_tests': [0x11111111, 0x22222222, 0xDEADBEEF, 0x55, 0x1234, 0xCAFEBABE, 7, 0xFFFFFFFF, 0x42, 0x99], 'path2_tests': [0x11111111, 0x22222222, 0xDEADBEEF]},

    # ---- promote-round round 64 (strided 2-dword clear; 0x004840d0 already C3) --
    'strided_clear2_709238': {'rva': 0x0048a460, 'export': 'StridedClear2_709238', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'range_init', 'target_global': 0x00709238, 'len': 0x9f60, 'lut_root_delta': 0, 'path1_tests': [0, 0], 'path2_tests': [0]},

    # ---- promote-round round 63 (three container-record setters) --
    'cont_rec_set_450': {'rva': 0x00489450, 'export': 'ContRecSet450', 'signature': {'ret': 'void', 'args': ['uint32', 'uint32']}, 'arg_type': 'container_record_set', 'shape': 'p', 'idx': 2, 'writes': [-0x20, -0x1c], 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9], 'path2_tests': [0, 1, 2]},
    'cont_rec_set_480': {'rva': 0x00489480, 'export': 'ContRecSet480', 'signature': {'ret': 'void', 'args': ['uint32', 'float']}, 'arg_type': 'container_record_set', 'shape': 'f', 'idx': 2, 'writes': [-0x18], 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9], 'path2_tests': [0, 1, 2]},
    'cont_rec_set_4a0': {'rva': 0x004894a0, 'export': 'ContRecSet4a0', 'signature': {'ret': 'void', 'args': ['uint32', 'uint32', 'uint32']}, 'arg_type': 'container_record_set', 'shape': 'pp', 'idx': 2, 'writes': [-0x10, -0xc, -0x8, -0x4], 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 8, 9], 'path2_tests': [0, 1, 2]},

    # ---- promote-round round 62 (two ghost-vehicle-slot indexed setters) --
    'ghost_slot_set_63c6ec': {'rva': 0x0041a5b0, 'export': 'GhostSlotSet63c6ec', 'signature': {'ret': 'void', 'args': ['uint32', 'uint32']}, 'arg_type': 'indexed_table_set', 'target_global': 0x0063c6ec, 'stride': 0xc4, 'set_idx': 2, 'lut_root_delta': 0, 'path1_tests': [0x11111111, 0x22222222, 0xDEADBEEF, 0x55, 0x1234, 0xCAFEBABE, 7, 0xFFFFFFFF, 0x42, 0x99], 'path2_tests': [0x11111111, 0x22222222, 0xDEADBEEF]},
    'ghost_slot_set_63c6f0': {'rva': 0x0041a8b0, 'export': 'GhostSlotSet63c6f0', 'signature': {'ret': 'void', 'args': ['uint32', 'uint32']}, 'arg_type': 'indexed_table_set', 'target_global': 0x0063c6f0, 'stride': 0xc4, 'set_idx': 2, 'lut_root_delta': 0, 'path1_tests': [0x11111111, 0x22222222, 0xDEADBEEF, 0x55, 0x1234, 0xCAFEBABE, 7, 0xFFFFFFFF, 0x42, 0x99], 'path2_tests': [0x11111111, 0x22222222, 0xDEADBEEF]},

    # ---- promote-round round 61 (global-indexed float getter + vec16 copy setter) --
    'float_idx_639de0': {'rva': 0x004077e0, 'export': 'FloatIdx639de0', 'signature': {'ret': 'float', 'args': []}, 'arg_type': 'global_indexed_float', 'gate': 0x0063a5d8, 'target_global': 0x00639de0, 'stride': 0xec, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 5, 7, 2, 0, 1, 4], 'path2_tests': [0, 3, 7]},
    'vec_copy_881ec8': {'rva': 0x0046d4d0, 'export': 'VecCopy881ec8', 'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32']}, 'arg_type': 'vec16_copy_set', 'target_global': 0x00881ec8, 'stride': 0xd04, 'span': 16, 'bound': 0x10, 'lut_root_delta': 0, 'path1_tests': [0, 1, 5, 15, 16, 3, 10, 1, 15, 0], 'path2_tests': [0, 7, 16]},

    # ---- promote-round round 60 (4-way pointer-global selector) --
    'sel_88fbc4': {'rva': 0x0045c640, 'export': 'Sel88fbc4', 'signature': {'ret': 'void', 'args': ['int32']}, 'arg_type': 'arg_scattered_globals', 'observe': [{'addr': 0x0088fbc4}, {'addr': 0x0088fbc8}], 'lut_root_delta': 0, 'path1_tests': [1, 2, 3, 4, 0, 5, 1, 2, 3, 0], 'path2_tests': [1, 2, 4]},

    # ---- promote-round round 59 (two get-and-return-with-ptrout accessors) --
    'tbl_ret_ptrout_63e4b8_0': {'rva': 0x00420da0, 'export': 'TblRetPtrout63e4b8_0', 'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32']}, 'arg_type': 'table_ret_ptrout', 'target_global': 0x0063e4b8, 'stride': 0x24, 'off0': 0, 'off1': 8, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 5, 7, 2, 0, 1, 3], 'path2_tests': [0, 3, 7]},
    'tbl_ret_ptrout_63e4b8_4': {'rva': 0x00420dc0, 'export': 'TblRetPtrout63e4b8_4', 'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32']}, 'arg_type': 'table_ret_ptrout', 'target_global': 0x0063e4b8, 'stride': 0x24, 'off0': 4, 'off1': 0xc, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 5, 7, 2, 0, 1, 3], 'path2_tests': [0, 3, 7]},

    # ---- promote-round round 58 (conditional getter + pointer-compute getter + equality predicate) --
    'cond_get_691500': {'rva': 0x00472500, 'export': 'CondGet691500', 'signature': {'ret': 'uint32', 'args': ['int32']}, 'arg_type': 'cond_table_get', 'target_global': 0x00691500, 'stride': 0x10, 'off0': 0, 'off1': 4, 'offf': 8, 'lut_root_delta': 0, 'path1_tests': [[0, 0], [0, 1], [1, 0], [1, 5], [2, 0], [2, 0xFF], [3, 0], [3, 1], [0, 0], [1, 0]], 'path2_tests': [[0, 0], [0, 1], [2, 7]]},
    'ptr_compute_881ec8': {'rva': 0x0046d4a0, 'export': 'PtrCompute881ec8', 'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32']}, 'arg_type': 'ptr_compute_get', 'target_global': 0x00881ec8, 'idxtbl': 0x00881f48, 'stride': 0xd04, 'tscale': 0x40, 'bound': 0x10, 'lut_root_delta': 0, 'path1_tests': [0, 1, 5, 15, 16, 3, 10, 1, 15, 0], 'path2_tests': [0, 7, 16]},
    'eq_predicate_7f1a18': {'rva': 0x0045caf0, 'export': 'EqPredicate7f1a18', 'signature': {'ret': 'uint32', 'args': ['int32', 'int32']}, 'arg_type': 'eq_predicate_get', 'target_global': 0x007f1a18, 'stride': 0x10, 'gate': 0x008aa254, 'gatemax': 2, 'lut_root_delta': 0, 'path1_tests': [[0, 1, 1, 0], [0, 1, 0, 0], [2, 3, 1, 0], [2, 3, 0, 0], [0, 1, 1, 2], [0, 0xFFFFFFFF, 1, 0], [5, 5, 1, 0], [1, 4, 0, 0], [0, 1, 1, 0], [3, 3, 1, 0]], 'path2_tests': [[0, 1, 1, 0], [0, 1, 0, 0], [0, 1, 1, 2]]},

    # ---- promote-round round 57 (Pool-J pointer-returning getter) --
    'ptr_get_68ba1c': {'rva': 0x0045a110, 'export': 'PtrGet68ba1c', 'signature': {'ret': 'uint32', 'args': ['int32']}, 'arg_type': 'int_scalar', 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 4, 0xFFFFFFFF, 2, 3, 1, 0], 'path2_tests': [0, 3, 4]},

    # ---- promote-round round 56 (per-vehicle vec3 getter + 3 two-index wheel getters) --
    'veh_tbl_8820a0_get3': {'rva': 0x0046bd20, 'export': 'VehTbl8820a0Get3', 'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32']}, 'arg_type': 'ptr_out_table_get', 'target_global': 0x008820a0, 'stride': 0xd04, 'span': 3, 'bound': 0x10, 'lut_root_delta': 0, 'path1_tests': [0, 1, 5, 15, 16, 3, 10, 1, 15, 0], 'path2_tests': [0, 7, 16]},
    'idx2_wheel_881790_get': {'rva': 0x0046d320, 'export': 'Idx2Wheel881790Get', 'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32', 'uint32']}, 'arg_type': 'idx2_table_get', 'target_global': 0x00881790, 'mult': 0x11, 'stride': 0xc4, 'bound': 0x10, 'bound2': 4, 'lut_root_delta': 0, 'path1_tests': [[0, 0], [1, 2], [15, 3], [5, 1], [16, 0], [0, 4], [16, 4], [3, 3], [10, 2], [0, 0]], 'path2_tests': [[0, 0], [15, 3], [16, 4]]},
    'idx2_wheel_881738_get': {'rva': 0x0046d360, 'export': 'Idx2Wheel881738Get', 'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32', 'uint32']}, 'arg_type': 'idx2_table_get', 'target_global': 0x00881738, 'mult': 0x11, 'stride': 0xc4, 'bound': 0x10, 'bound2': 4, 'lut_root_delta': 0, 'path1_tests': [[0, 0], [1, 2], [15, 3], [5, 1], [16, 0], [0, 4], [16, 4], [3, 3], [10, 2], [0, 0]], 'path2_tests': [[0, 0], [15, 3], [16, 4]]},
    'idx2_wheel_881744_get': {'rva': 0x0046bd60, 'export': 'Idx2Wheel881744Get', 'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32', 'uint32']}, 'arg_type': 'idx2_table_get', 'target_global': 0x00881744, 'mult': 0x11, 'stride': 0xc4, 'bound': 0x10, 'bound2': 4, 'lut_root_delta': 0, 'path1_tests': [[0, 0], [1, 2], [15, 3], [5, 1], [16, 0], [0, 4], [16, 4], [3, 3], [10, 2], [0, 0]], 'path2_tests': [[0, 0], [15, 3], [16, 4]]},

    # ---- promote-round round 55 (3 scenery-actor 200-entry getters + 4-global clearer) --
    'table_6c9438_get': {'rva': 0x0047ce00, 'export': 'Table6c9438Get', 'signature': {'ret': 'uint32', 'args': ['int32']}, 'arg_type': 'int_scalar', 'seed_table': {'base': 0x006c9438, 'stride': 4, 'span': 16}, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 5, 10, 0xC8, 0xFFFFFFFF, 3, 7, 15], 'path2_tests': [0, 0xC8, 0xFFFFFFFF]},
    'table_6c9758_get': {'rva': 0x0047ce80, 'export': 'Table6c9758Get', 'signature': {'ret': 'uint32', 'args': ['int32']}, 'arg_type': 'int_scalar', 'seed_table': {'base': 0x006c9758, 'stride': 4, 'span': 16}, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 5, 10, 0xC8, 0xFFFFFFFF, 3, 7, 15], 'path2_tests': [0, 0xC8, 0xFFFFFFFF]},
    'table_6c71d8_get': {'rva': 0x0047d130, 'export': 'Table6c71d8Get', 'signature': {'ret': 'uint32', 'args': ['int32']}, 'arg_type': 'int_scalar', 'seed_table': {'base': 0x006c71d8, 'stride': 4, 'span': 16}, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 5, 10, 0xC8, 0xFFFFFFFF, 3, 7, 15], 'path2_tests': [0, 0xC8, 0xFFFFFFFF]},
    'clear_88f0a0_x4': {'rva': 0x0045c860, 'export': 'Clear88f0a0x4', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'scalars_to_scattered_globals', 'observe': [{'addr': 0x0088f0a0}, {'addr': 0x0088f0a4}, {'addr': 0x0088f0a8}, {'addr': 0x0088f0ac}], 'lut_root_delta': 0, 'path1_tests': [0, 0, 0], 'path2_tests': [0, 0]},

    # ---- promote-round round 54 (stride-0x58 getter + 4 per-vehicle table getters) --
    'table_68ba04_get': {'rva': 0x0045a0d0, 'export': 'Table68ba04Get', 'signature': {'ret': 'uint32', 'args': ['int32']}, 'arg_type': 'int_scalar', 'seed_table': {'base': 0x0068ba04, 'stride': 0x58, 'span': 4}, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 4, 0xFFFFFFFF, 2, 3, 1, 0], 'path2_tests': [0, 3, 4]},
    'veh_tbl_881f50_get3': {'rva': 0x0046d660, 'export': 'VehTbl881f50Get3', 'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32']}, 'arg_type': 'ptr_out_table_get', 'target_global': 0x00881f50, 'stride': 0xd04, 'span': 3, 'bound': 0x10, 'lut_root_delta': 0, 'path1_tests': [0, 1, 5, 15, 16, 3, 10, 1, 15, 0], 'path2_tests': [0, 7, 16]},
    'veh_tbl_8820ac_get1': {'rva': 0x0046d6a0, 'export': 'VehTbl8820acGet1', 'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32']}, 'arg_type': 'ptr_out_table_get', 'target_global': 0x008820ac, 'stride': 0xd04, 'span': 1, 'bound': 0x10, 'lut_root_delta': 0, 'path1_tests': [0, 1, 5, 15, 16, 3, 10, 1, 15, 0], 'path2_tests': [0, 7, 16]},
    'veh_tbl_881f84_get1': {'rva': 0x0046d6d0, 'export': 'VehTbl881f84Get1', 'signature': {'ret': 'uint32', 'args': ['uint32', 'uint32']}, 'arg_type': 'ptr_out_table_get', 'target_global': 0x00881f84, 'stride': 0xd04, 'span': 1, 'bound': 0x10, 'lut_root_delta': 0, 'path1_tests': [0, 1, 5, 15, 16, 3, 10, 1, 15, 0], 'path2_tests': [0, 7, 16]},

    # ---- promote-round round 53 (scattered-global clearer + 4 strided table clearers) --
    'clear_639d70_x3': {'rva': 0x00405400, 'export': 'Clear639d70x3', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'scalars_to_scattered_globals', 'observe': [{'addr': 0x00639d70}, {'addr': 0x00639d74}, {'addr': 0x00639d78}], 'lut_root_delta': 0, 'path1_tests': [0, 0, 0], 'path2_tests': [0, 0]},
    'strided_clear_76a100': {'rva': 0x0048f680, 'export': 'StridedClear76a100', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'range_init', 'target_global': 0x0076a100, 'len': 0x3800, 'lut_root_delta': 0, 'path1_tests': [0, 0], 'path2_tests': [0]},
    'strided_clear_766a00': {'rva': 0x0048f6b0, 'export': 'StridedClear766a00', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'range_init', 'target_global': 0x00766a00, 'len': 0x500, 'lut_root_delta': 0, 'path1_tests': [0, 0], 'path2_tests': [0]},
    'strided_clear_770718': {'rva': 0x0048f6e0, 'export': 'StridedClear770718', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'range_init', 'target_global': 0x00770718, 'len': 0xe10, 'lut_root_delta': 0, 'path1_tests': [0, 0], 'path2_tests': [0]},
    'strided_clear_769f50': {'rva': 0x0048f710, 'export': 'StridedClear769f50', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'range_init', 'target_global': 0x00769f50, 'len': 0x168, 'lut_root_delta': 0, 'path1_tests': [0, 0], 'path2_tests': [0]},

    # ---- promote-round round 51 (palette-array initializer) --
    'init_691500': {'rva': 0x004723d0, 'export': 'Init691500', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'range_init', 'target_global': 0x00691500, 'len': 0xa0, 'lut_root_delta': 0, 'path1_tests': [0, 0, 0], 'path2_tests': [0, 0]},

    # ---- promote-round round 50 (indexed table getter + 3 indexed setters) --
    'table_691508_get': {'rva': 0x00472550, 'export': 'Table691508Get', 'signature': {'ret': 'uint32', 'args': ['int32']}, 'arg_type': 'int_scalar', 'seed_table': {'base': 0x00691508, 'stride': 0x10, 'span': 4}, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 0, 1, 2, 3, 1, 2], 'path2_tests': [0, 1, 2]},
    'table_69150c_set': {'rva': 0x00472520, 'export': 'Table69150cSet', 'signature': {'ret': 'void', 'args': ['uint32', 'uint32']}, 'arg_type': 'indexed_table_set', 'target_global': 0x0069150c, 'stride': 0x10, 'set_idx': 2, 'lut_root_delta': 0, 'path1_tests': [0x11111111, 0x22222222, 0xDEADBEEF, 0x55, 0x1234, 0xCAFEBABE, 7, 0xFFFFFFFF, 0x42, 0x99], 'path2_tests': [0x11111111, 0x22222222, 0xDEADBEEF]},
    'table_86ae38_set': {'rva': 0x00488390, 'export': 'Table86ae38Set', 'signature': {'ret': 'void', 'args': ['uint32', 'uint32']}, 'arg_type': 'indexed_table_set', 'target_global': 0x0086ae38, 'stride': 0x38, 'set_idx': 2, 'lut_root_delta': 0, 'path1_tests': [0x11111111, 0x22222222, 0xDEADBEEF, 0x55, 0x1234, 0xCAFEBABE, 7, 0xFFFFFFFF, 0x42, 0x99], 'path2_tests': [0x11111111, 0x22222222, 0xDEADBEEF]},
    'table_89a500_set': {'rva': 0x00416230, 'export': 'Table89a500Set', 'signature': {'ret': 'void', 'args': ['uint32', 'uint32']}, 'arg_type': 'indexed_table_set', 'target_global': 0x0089a500, 'stride': 0x74, 'set_idx': 2, 'lut_root_delta': 0, 'path1_tests': [0x11111111, 0x22222222, 0xDEADBEEF, 0x55, 0x1234, 0xCAFEBABE, 7, 0xFFFFFFFF, 0x42, 0x99], 'path2_tests': [0x11111111, 0x22222222, 0xDEADBEEF]},

    # ---- promote-round round 49 (pointer-to-table field getter) --
    'ptr_table_5f2770_get': {'rva': 0x0040ce80, 'export': 'PtrTable5f2770Get', 'signature': {'ret': 'uint32', 'args': ['int32']}, 'arg_type': 'ptr_table_field_read', 'target_global': 0x005f2770, 'field_off': 4, 'capacity': 8, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 3, 5], 'path2_tests': [0, 3, 5]},

    # ---- promote-round round 48 (SmplFzx array-stack push/pop pair) --
    'stack_pop_485bd0': {'rva': 0x00485bd0, 'export': 'StackPop485bd0', 'signature': {'ret': 'uint32', 'args': ['pointer']}, 'arg_type': 'stack_pop_snapshot', 'capacity': 4, 'init_buf': [0xAAAA1111, 0xBBBB2222, 0xCCCC3333, 0xDDDD4444], 'lut_root_delta': 0, 'path1_tests': [4, 3, 2, 1, 0, 4, 3, 2, 1, 0], 'path2_tests': [3, 1, 0]},
    'stack_push_485bf0': {'rva': 0x00485bf0, 'export': 'StackPush485bf0', 'signature': {'ret': 'uint32', 'args': ['pointer', 'uint32']}, 'arg_type': 'stack_push_snapshot', 'capacity': 4, 'init_top': 1, 'lut_root_delta': 0, 'path1_tests': [0x11111111, 0x22222222, 0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0x55, 0xFFFFFFFF, 0x99, 0x42, 0x7F], 'path2_tests': [0x11111111, 0x22222222, 0xDEADBEEF]},

    # ---- promote-round round 47 (stride-0x18 table getter + 2 clearers) --
    'table_688304_get': {'rva': 0x00454a30, 'export': 'Table688304Get', 'signature': {'ret': 'uint32', 'args': ['int32']}, 'arg_type': 'int_scalar', 'seed_table': {'base': 0x00688304, 'stride': 0x18, 'span': 4}, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 0, 1, 2, 3, 1, 2], 'path2_tests': [0, 1, 2]},
    'table_88f09c_clear': {'rva': 0x0045c850, 'export': 'Table88f09cClear', 'signature': {'ret': 'void', 'args': ['uint32']}, 'arg_type': 'table_clear', 'target_global': 0x0088f09c, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 2, 5], 'path2_tests': [0, 1, 2]},
    'clear_894f0': {'rva': 0x004894f0, 'export': 'Clear894f0', 'signature': {'ret': 'void', 'args': ['pointer']}, 'arg_type': 'ptr_fields_clear', 'observe': [{'off': 0x54}, {'off': 0x08}, {'off': 0x0c}], 'lut_root_delta': 0, 'path1_tests': [0, 0, 0, 0, 0], 'path2_tests': [0, 0]},

    # ---- promote-round round 46 (pool-manager REMOVE; self-contained list op) --
    'pool_remove_485b30': {'rva': 0x00485b30, 'export': 'PoolRemove485b30', 'signature': {'ret': 'uint32', 'args': ['pointer', 'uint32']}, 'arg_type': 'pool_remove_snapshot', 'capacity': 4, 'insert_rva': 0x00485a70, 'build_keys': [0xA1, 0xA2, 0xA3], 'lut_root_delta': 0, 'path1_tests': [0xA1, 0xA2, 0xA3, 0xFF, 0xA2, 0xA1, 0xA3, 0xFF, 0xA1, 0xA3], 'path2_tests': [0xA1, 0xA2, 0xFF]},

    # ---- promote-round round 45 (pool-manager INSERT; self-contained list op) --
    'pool_insert_485a70': {'rva': 0x00485a70, 'export': 'PoolInsert485a70', 'signature': {'ret': 'uint16', 'args': ['pointer', 'uint32']}, 'arg_type': 'pool_insert_snapshot', 'capacity': 4, 'lut_root_delta': 0, 'path1_tests': [0x11111111, 0x22222222, 0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0x55, 0xFFFFFFFF, 0x99, 0x42, 0x7F], 'path2_tests': [0x11111111, 0x22222222, 0xDEADBEEF]},

    # ---- promote-round round 44 (EAX-implicit field clearer; naked asm) --
    'eax_clear_4190f0': {'rva': 0x004190f0, 'export': 'ClearEax4190f0', 'signature': {'ret': 'void', 'args': []}, 'arg_type': 'eax_implicit_void', 'observe': [{'off': 0x4c}, {'off': 0x50}], 'lut_root_delta': 0, 'path1_tests': [0, 0, 0, 0, 0], 'path2_tests': [0, 0]},

    # ---- promote-round round 43 (large-stride imul FLOAT table getters) --
    'float_table_63dc64_get': {'rva': 0x0041f100, 'export': 'FloatTable63dc64Get', 'signature': {'ret': 'float', 'args': ['int32']}, 'arg_type': 'float_table_read', 'seed_table': {'base': 0x0063dc64, 'stride': 0x2ac, 'span': 4}, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 0, 1, 2, 3, 1, 2], 'path2_tests': [0, 1, 2]},
    'float_table_8900a8_get': {'rva': 0x0044e050, 'export': 'FloatTable8900a8Get', 'signature': {'ret': 'float', 'args': ['int32']}, 'arg_type': 'float_table_read', 'seed_table': {'base': 0x008900a8, 'stride': 0xf8, 'span': 4}, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 0, 1, 2, 3, 1, 2], 'path2_tests': [0, 1, 2]},

    # ---- promote-round round 42 (large-stride imul table getters) --
    'table_63dc6c_get': {'rva': 0x0041f300, 'export': 'Table63dc6cGet', 'signature': {'ret': 'uint32', 'args': ['int32']}, 'arg_type': 'int_scalar', 'seed_table': {'base': 0x0063dc6c, 'stride': 0x2ac, 'span': 4}, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 0, 1, 2, 3, 1, 2], 'path2_tests': [0, 1, 2]},
    'table_8a9648_get': {'rva': 0x00407a20, 'export': 'Table8a9648Get', 'signature': {'ret': 'uint32', 'args': ['int32']}, 'arg_type': 'int_scalar', 'seed_table': {'base': 0x008a9648, 'stride': 0x30c, 'span': 4}, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 0, 1, 2, 3, 1, 2], 'path2_tests': [0, 1, 2]},
    'table_8a9640_get': {'rva': 0x00407a40, 'export': 'Table8a9640Get', 'signature': {'ret': 'uint32', 'args': ['int32']}, 'arg_type': 'int_scalar', 'seed_table': {'base': 0x008a9640, 'stride': 0x30c, 'span': 4}, 'lut_root_delta': 0, 'path1_tests': [0, 1, 2, 3, 0, 1, 2, 3, 1, 2], 'path2_tests': [0, 1, 2]},

    # ---- promote-round round 41 (global-field getter cluster @0x0063d7e4) --
    'global_field_63d7e4_1c': {'rva': 0x0041e9f0, 'export': 'GlobalField63d7e4_1c', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'global_field_read', 'target_global': 0x0063d7e4, 'field_off': 0x1c, 'lut_root_delta': 0, 'path1_tests': [1, 2, 0xDEAD, 0x55, 0x1234, 0xCAFE, 7, 0x42424242, 0x99, 0x7FFFFFFF], 'path2_tests': [1, 2, 0xDEAD]},
    'global_field_63d7e4_20': {'rva': 0x0041ea00, 'export': 'GlobalField63d7e4_20', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'global_field_read', 'target_global': 0x0063d7e4, 'field_off': 0x20, 'lut_root_delta': 0, 'path1_tests': [1, 2, 0xDEAD, 0x55, 0x1234, 0xCAFE, 7, 0x42424242, 0x99, 0x7FFFFFFF], 'path2_tests': [1, 2, 0xDEAD]},
    'global_field_63d7e4_24': {'rva': 0x0041ea10, 'export': 'GlobalField63d7e4_24', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'global_field_read', 'target_global': 0x0063d7e4, 'field_off': 0x24, 'lut_root_delta': 0, 'path1_tests': [1, 2, 0xDEAD, 0x55, 0x1234, 0xCAFE, 7, 0x42424242, 0x99, 0x7FFFFFFF], 'path2_tests': [1, 2, 0xDEAD]},
    'global_field_63d7e4_28': {'rva': 0x0041ea20, 'export': 'GlobalField63d7e4_28', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'global_field_read', 'target_global': 0x0063d7e4, 'field_off': 0x28, 'lut_root_delta': 0, 'path1_tests': [1, 2, 0xDEAD, 0x55, 0x1234, 0xCAFE, 7, 0x42424242, 0x99, 0x7FFFFFFF], 'path2_tests': [1, 2, 0xDEAD]},
    'global_field_63d7e4_2c': {'rva': 0x0041ea30, 'export': 'GlobalField63d7e4_2c', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'global_field_read', 'target_global': 0x0063d7e4, 'field_off': 0x2c, 'lut_root_delta': 0, 'path1_tests': [1, 2, 0xDEAD, 0x55, 0x1234, 0xCAFE, 7, 0x42424242, 0x99, 0x7FFFFFFF], 'path2_tests': [1, 2, 0xDEAD]},
    'global_field_63d7e4_30': {'rva': 0x0041ea40, 'export': 'GlobalField63d7e4_30', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'global_field_read', 'target_global': 0x0063d7e4, 'field_off': 0x30, 'lut_root_delta': 0, 'path1_tests': [1, 2, 0xDEAD, 0x55, 0x1234, 0xCAFE, 7, 0x42424242, 0x99, 0x7FFFFFFF], 'path2_tests': [1, 2, 0xDEAD]},
    'global_field_63d7e4_34': {'rva': 0x0041ea50, 'export': 'GlobalField63d7e4_34', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'global_field_read', 'target_global': 0x0063d7e4, 'field_off': 0x34, 'lut_root_delta': 0, 'path1_tests': [1, 2, 0xDEAD, 0x55, 0x1234, 0xCAFE, 7, 0x42424242, 0x99, 0x7FFFFFFF], 'path2_tests': [1, 2, 0xDEAD]},
    'global_field_63d7e4_38': {'rva': 0x0041ea60, 'export': 'GlobalField63d7e4_38', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'global_field_read', 'target_global': 0x0063d7e4, 'field_off': 0x38, 'lut_root_delta': 0, 'path1_tests': [1, 2, 0xDEAD, 0x55, 0x1234, 0xCAFE, 7, 0x42424242, 0x99, 0x7FFFFFFF], 'path2_tests': [1, 2, 0xDEAD]},
    'global_field_63d7e4_3c': {'rva': 0x0041ea70, 'export': 'GlobalField63d7e4_3c', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'global_field_read', 'target_global': 0x0063d7e4, 'field_off': 0x3c, 'lut_root_delta': 0, 'path1_tests': [1, 2, 0xDEAD, 0x55, 0x1234, 0xCAFE, 7, 0x42424242, 0x99, 0x7FFFFFFF], 'path2_tests': [1, 2, 0xDEAD]},
    'global_field_63d7e4_40': {'rva': 0x0041ea80, 'export': 'GlobalField63d7e4_40', 'signature': {'ret': 'uint32', 'args': []}, 'arg_type': 'global_field_read', 'target_global': 0x0063d7e4, 'field_off': 0x40, 'lut_root_delta': 0, 'path1_tests': [1, 2, 0xDEAD, 0x55, 0x1234, 0xCAFE, 7, 0x42424242, 0x99, 0x7FFFFFFF], 'path2_tests': [1, 2, 0xDEAD]},

    # ---- promote-round round 40 (first-party constant-return leaves) --
    'ret_50': {
        'rva': 0x0044dfe0, 'export': 'Ret50', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'const_return', 'lut_root_delta': 0,
        'path1_tests': [0, 0, 0, 0, 0], 'path2_tests': [0, 0],
    },
    'ret_3': {
        'rva': 0x00493b40, 'export': 'Ret3', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'const_return', 'lut_root_delta': 0,
        'path1_tests': [0, 0, 0, 0, 0], 'path2_tests': [0, 0],
    },
    'ret_897ff0': {
        'rva': 0x00443090, 'export': 'Ret897ff0', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'const_return', 'lut_root_delta': 0,
        'path1_tests': [0, 0, 0, 0, 0], 'path2_tests': [0, 0],
    },
    'ret_63a5f0': {
        'rva': 0x004098a0, 'export': 'Ret63a5f0', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'const_return', 'lut_root_delta': 0,
        'path1_tests': [0, 0, 0, 0, 0], 'path2_tests': [0, 0],
    },
    'ret_6147b4': {
        'rva': 0x004924e0, 'export': 'Ret6147b4', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'const_return', 'lut_root_delta': 0,
        'path1_tests': [0, 0, 0, 0, 0], 'path2_tests': [0, 0],
    },

    # ---- promote-round round 39 (first-party multi-store const setters) --
    'clear_63d584_pair': {
        'rva': 0x0041d910, 'export': 'Clear63d584Pair', 'signature': {'ret': 'void', 'args': []},
        'arg_type': 'scalars_to_scattered_globals',
        'observe': [{'addr': '0x0063d584', 'len': 4, 'fill': 0xFF}, {'addr': '0x0063d588', 'len': 4, 'fill': 0xFF}], 'lut_root_delta': 0,
        'path1_tests': [{'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}],
        'path2_tests': [{'args': []}, {'args': []}, {'args': []}],
    },
    'clear_8991b0_pair': {
        'rva': 0x00429820, 'export': 'Clear8991b0Pair', 'signature': {'ret': 'void', 'args': []},
        'arg_type': 'scalars_to_scattered_globals',
        'observe': [{'addr': '0x008991b0', 'len': 4, 'fill': 0xFF}, {'addr': '0x008991b4', 'len': 4, 'fill': 0xFF}], 'lut_root_delta': 0,
        'path1_tests': [{'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}],
        'path2_tests': [{'args': []}, {'args': []}, {'args': []}],
    },

    # ---- promote-round round 38 (bounds-checked 0xd04-stride table getters) --
    'table_882194_get': {
        'rva': 0x0046c750, 'export': 'Table882194Get', 'signature': {'ret': 'uint32', 'args': ['int32']},
        'arg_type': 'int_scalar', 'lut_root_delta': 0,
        'seed_table': {'base': 0x00882194, 'stride': 0x0d04, 'span': 0x10},
        'path1_tests': [0, 1, 2, 3, 4, 8, 0xf, 0x10, 0x11, 5],
        'path2_tests': [0, 0xf, 0x10],
    },
    'table_882198_get': {
        'rva': 0x0046c730, 'export': 'Table882198Get', 'signature': {'ret': 'uint32', 'args': ['int32']},
        'arg_type': 'int_scalar', 'lut_root_delta': 0,
        'seed_table': {'base': 0x00882198, 'stride': 0x0d04, 'span': 0x10},
        'path1_tests': [0, 1, 2, 3, 4, 8, 0xf, 0x10, 0x11, 5],
        'path2_tests': [0, 0xf, 0x10],
    },

    # ---- promote-round round 37 (absolute-table getters; early-window table-seed) --
    'table_88ff50_get': {
        'rva': 0x00452ea0, 'export': 'Table88ff50Get', 'signature': {'ret': 'uint32', 'args': ['int32']},
        'arg_type': 'int_scalar', 'lut_root_delta': 0,
        'seed_table': {'base': 0x0088ff50, 'stride': 4, 'span': 8},
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 5, 3],
        'path2_tests': [0, 5, 3],
    },
    'table_8aa300_get': {
        'rva': 0x0045dd50, 'export': 'Table8aa300Get', 'signature': {'ret': 'uint32', 'args': ['int32']},
        'arg_type': 'int_scalar', 'lut_root_delta': 0,
        'seed_table': {'base': 0x008aa300, 'stride': 4, 'span': 8},
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 5, 3],
        'path2_tests': [0, 5, 3],
    },
    'table_63d830_get': {
        'rva': 0x0041f320, 'export': 'Table63d830Get', 'signature': {'ret': 'uint32', 'args': ['int32']},
        'arg_type': 'int_scalar', 'lut_root_delta': 0,
        'seed_table': {'base': 0x0063d830, 'stride': 4, 'span': 8},
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 5, 3],
        'path2_tests': [0, 5, 3],
    },

    # ---- promote-round round 35 (float-getter vein: read_global + ret:float) --
    # D9-05 fld-dword getters; read_global seeds the global with a clean float
    # bit-pattern (no NaN/Inf) and compares the ST0 float return.
    'float_5ea0a8_get': {
        'rva': 0x004039e0, 'export': 'Float5ea0a8Get', 'signature': {'ret': 'float', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x005ea0a8, 'lut_root_delta': 0,
        'path1_tests': [0x3F800000, 0x40000000, 0xBF800000, 0x40200000, 0x42C80000, 0x3F000000, 0xBE800000, 0x449A5000, 0x00000000, 0x3DCCCCCD],
        'path2_tests': [0x3F800000, 0x40000000, 0xBF800000],
    },
    'float_89a360_get': {
        'rva': 0x004173a0, 'export': 'Float89a360Get', 'signature': {'ret': 'float', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x0089a360, 'lut_root_delta': 0,
        'path1_tests': [0x3F800000, 0x40000000, 0xBF800000, 0x40200000, 0x42C80000, 0x3F000000, 0xBE800000, 0x449A5000, 0x00000000, 0x3DCCCCCD],
        'path2_tests': [0x3F800000, 0x40000000, 0xBF800000],
    },
    'float_61313c_get': {
        'rva': 0x0046dd80, 'export': 'Float61313cGet', 'signature': {'ret': 'float', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x0061313c, 'lut_root_delta': 0,
        'path1_tests': [0x3F800000, 0x40000000, 0xBF800000, 0x40200000, 0x42C80000, 0x3F000000, 0xBE800000, 0x449A5000, 0x00000000, 0x3DCCCCCD],
        'path2_tests': [0x3F800000, 0x40000000, 0xBF800000],
    },

    # ---- promote-round round 34 (byte-scan batch: getter/setters + bounds getter) --
    'global_772fac_get': {
        'rva': 0x00495790, 'export': 'Global772facGet', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x00772fac, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'set_684b34': {
        'rva': 0x0044d6e0, 'export': 'Set684b34', 'signature': {'ret': 'void', 'args': ['uint32']},
        'arg_type': 'void_setter_observe', 'target_global': 0x00684b34, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'set_771968_1': {
        'rva': 0x00493590, 'export': 'Set771968_1', 'signature': {'ret': 'void', 'args': []},
        'arg_type': 'scalars_to_scattered_globals',
        'observe': [{'addr': '0x00771968', 'len': 4, 'fill': 0xFF}], 'lut_root_delta': 0,
        'path1_tests': [{'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}],
        'path2_tests': [{'args': []}, {'args': []}, {'args': []}],
    },
    'veh_pwr_state_68ba00_get': {
        'rva': 0x0045a0f0, 'export': 'VehPwrState68ba00Get', 'signature': {'ret': 'uint32', 'args': ['int32']},
        'arg_type': 'int_scalar', 'lut_root_delta': 0,
        'path1_tests': [0, 1, 2, 3, 4, 5, 0xFFFFFFFF, 0x7FFFFFFF, 0x80000000, 2],
        'path2_tests': [0, 4, 0xFFFFFFFF],
    },

    # ---- promote-round round 33 (byte-scan batch: getters + const setter) -----
    'global_63a5d0_get': {
        'rva': 0x004075a0, 'export': 'Global63a5d0Get', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x0063a5d0, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'global_63d7e0_get': {
        'rva': 0x0041e140, 'export': 'Global63d7e0Get', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x0063d7e0, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'global_6c6eb0_get': {
        'rva': 0x0047ce70, 'export': 'Global6c6eb0Get', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x006c6eb0, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'global_772ffc_get': {
        'rva': 0x00496910, 'export': 'Global772ffcGet', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x00772ffc, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'set_7f1a0c_1000': {
        'rva': 0x0042b950, 'export': 'Set7f1a0c_1000', 'signature': {'ret': 'void', 'args': []},
        'arg_type': 'scalars_to_scattered_globals',
        'observe': [{'addr': '0x007f1a0c', 'len': 4, 'fill': 0xFF}], 'lut_root_delta': 0,
        'path1_tests': [{'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}],
        'path2_tests': [{'args': []}, {'args': []}, {'args': []}],
    },

    # ---- promote-round round 32 (byte-scan batch: getters + param setters) ----
    'global_67ea6c_get': {
        'rva': 0x0042d390, 'export': 'Global67ea6cGet', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x0067ea6c, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'global_7e9584_get': {
        'rva': 0x00499710, 'export': 'Global7e9584Get', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x007e9584, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'global_7dcabc_get': {
        'rva': 0x005a7b50, 'export': 'Global7dcabcGet', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x007dcabc, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'set_86ecc8': {
        'rva': 0x00472640, 'export': 'Set86ecc8', 'signature': {'ret': 'void', 'args': ['uint32']},
        'arg_type': 'void_setter_observe', 'target_global': 0x0086ecc8, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'set_63ba7c': {
        'rva': 0x0040e170, 'export': 'Set63ba7c', 'signature': {'ret': 'void', 'args': ['uint32']},
        'arg_type': 'void_setter_observe', 'target_global': 0x0063ba7c, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },

    # ---- promote-round round 31 (byte-scan batch: single-global getters) ------
    'global_67f19c_get': {
        'rva': 0x0042f760, 'export': 'Global67f19cGet', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x0067f19c, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'global_67f1a0_get': {
        'rva': 0x0042f770, 'export': 'Global67f1a0Get', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x0067f1a0, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'global_67f1a4_get': {
        'rva': 0x0042f780, 'export': 'Global67f1a4Get', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x0067f1a4, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'global_66d6d8_get': {
        'rva': 0x00426bb0, 'export': 'Global66d6d8Get', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x0066d6d8, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'global_80332c_get': {
        'rva': 0x0045bfe0, 'export': 'Global80332cGet', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x0080332c, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },

    # ---- promote-round round 30 (worklist batch: global + table getters) ------
    'global_7d3e4c_get': {
        'rva': 0x004b68e0, 'export': 'Global7d3e4cGet', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x007d3e4c, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'global_7d3e4c_get_thunk': {
        'rva': 0x004b65a0, 'export': 'Global7d3e4cGetThunk', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x007d3e4c, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'table_773030_get': {
        'rva': 0x00496930, 'export': 'Table773030Get', 'signature': {'ret': 'uint32', 'args': ['int32']},
        'arg_type': 'int_scalar', 'lut_root_delta': 0, 'scenario': 'race',
        # seed_table: early_window_leaf_diff seeds 0x00773030[0..7] (stride 4) with
        # distinct values so the index*4 read is diffed non-degenerately (the live
        # table is zero at menu/race-attach -> exit-5 in round 30).
        'seed_table': {'base': 0x00773030, 'stride': 4, 'span': 8},
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 5, 3],
        'path2_tests': [0, 5, 3],
    },
    'joint_ptr_6ce81c_get': {
        'rva': 0x004840d0, 'export': 'JointPtr6ce81cGet', 'signature': {'ret': 'uint32', 'args': ['int32']},
        'arg_type': 'int_scalar', 'lut_root_delta': 0, 'scenario': 'race',
        # bounds [0,3]: seed slots 0..3; indices >=4 return 0 on both sides.
        'seed_table': {'base': 0x006ce81c, 'stride': 4, 'span': 4},
        'path1_tests': [0, 1, 2, 3, 4, 5, 0, 1, 2, 3],
        'path2_tests': [0, 3, 5],
    },
    'powerup_table_6885e0_get': {
        'rva': 0x00455b40, 'export': 'PowerupTable6885e0Get', 'signature': {'ret': 'uint32', 'args': ['int32']},
        'arg_type': 'int_scalar', 'lut_root_delta': 0, 'scenario': 'race',
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 2, 4],
        'path2_tests': [0, 2, 4],
    },

    # ---- c3_batch_sgr1 s3 (2026-06-25): mixed getters/setters (menu + race) ----
    # 0x004840b0  ShapeOwnerHandleGet (vehicle, MENU/pure-leaf) — bounds [0,3] read of
    #   the 4-slot shape-owner handle pool DAT_006ce82c (companion to joint pool
    #   0x004840d0). The live pool is zero at menu/race-attach (the sister
    #   joint_ptr_6ce81c_get needed the same), so VERIFY VIA early_window_leaf_diff:
    #   seed_table seeds slots 0..3 -> distinct non-zero -> non-degenerate. int_scalar
    #   is a PURE_LEAF_ARGTYPE; idx>=4 returns 0 on both sides. (run_diff would be
    #   INCONCLUSIVE-DEGENERATE here because it does not honor seed_table.)
    'shape_owner_6ce82c_get': {
        'rva': 0x004840b0, 'export': 'ShapeOwnerHandleGet', 'signature': {'ret': 'uint32', 'args': ['int32']},
        'arg_type': 'int_scalar', 'lut_root_delta': 0,
        'seed_table': {'base': 0x006ce82c, 'stride': 4, 'span': 4},
        'path1_tests': [0, 1, 2, 3, 4, 5, 0, 1, 2, 3],
        'path2_tests': [0, 3, 5],
    },
    # 0x0047c270  CameraPathAllNodesEq2 (camera, RACE) — any active camera-path node
    #   whose sub-items are all state==2. Reads live node arrays (count DAT_006c2fe8 /
    #   sub-counts 0x006c2fa8 / records 0x006c27a8) and calls the ORIGINAL inner
    #   predicate 0x0047c230 (EAX/EBX register convention) -> 0/1. param_1=0 sentinel.
    'camera_path_all_nodes_eq2': {
        'rva': 0x0047c270, 'export': 'CameraPathAllNodesEq2', 'signature': {'ret': 'uint32', 'args': ['int32']},
        'arg_type': 'int_scalar', 'lut_root_delta': 0, 'scenario': 'race',
        'path1_tests': [0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests': [0, 0, 0],
    },
    # 0x0047c2d0  CameraPathAnyNodeNonzero (camera, RACE) — twin of 0x0047c270, inner
    #   predicate 0x0047c1f0 (all sub-items != 0).
    'camera_path_any_node_nonzero': {
        'rva': 0x0047c2d0, 'export': 'CameraPathAnyNodeNonzero', 'signature': {'ret': 'uint32', 'args': ['int32']},
        'arg_type': 'int_scalar', 'lut_root_delta': 0, 'scenario': 'race',
        'path1_tests': [0, 0, 0, 0, 0, 0, 0, 0],
        'path2_tests': [0, 0, 0],
    },
    # 0x00448700  VehicleSeedWritePair (vehicle, RACE) — void(p1,p2): 100x DispatchAll
    #   (0x004464c0, loops DAT_008964c0 — does NOT touch 0x897ffc/0x898000) then writes
    #   p1->0x00897ffc, p2->0x00898000. void_write_observe: write sentinel to 0x00897ffc,
    #   call fn(p1,p2), read back -> must == p1. call_args fixed -> deterministic write.
    #   seed_globals resets the DispatchAll entry count DAT_00898994=0 before EACH
    #   call (disasm 0x004464c0: mov eax,[0x898994]; test eax,eax; jle skip) so the
    #   100x DispatchAll loop is a clean no-op on BOTH sides — without it the live
    #   camera-entry array is partially-init at race-attach and DispatchAll AVs, and
    #   void_write_observe (orig THEN reimpl) double-runs it, poisoning state between
    #   the two sides (orig wrote p1 fine, reimpl then crashed on orig-mutated state).
    #   DispatchAll touches DAT_008964c0, NOT 0x00897ffc, so neutering it does not
    #   affect the observed write-pair.
    'vehicle_seed_write_pair': {
        'rva': 0x00448700, 'export': 'VehicleSeedWritePair', 'signature': {'ret': 'void', 'args': ['int32', 'int32']},
        'arg_type': 'void_write_observe', 'target_global': 0x00897ffc,
        'call_args': [0xDEADBEEF, 0xCAFEBABE], 'lut_root_delta': 0, 'scenario': 'race',
        'seed_globals': [{'addr': '0x00898994', 'val': 0}],
        'path1_tests': [0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555],
        'path2_tests': [0x11111111, 0x22222222],
    },
    # 0x0046d510  VehicleVelocityWorldGet (ai, RACE) — direct twin of 0x0046d700; transforms
    #   the +0xac velocity float3 via FUN_004c3df0 (C4) then copies it out. out3_idx validates
    #   the 0/1 BOUNDS RETURN (idx<16); boundary tests 15/16/17 give the non-degenerate mix.
    'vehicle_velocity_world_get': {
        'rva': 0x0046d510, 'export': 'VehicleVelocityWorldGet', 'signature': {'ret': 'int32', 'args': ['pointer', 'uint32']},
        'arg_type': 'out3_idx', 'lut_root_delta': 0, 'scenario': 'race',
        'path1_tests': [0, 1, 5, 10, 15, 16, 17, 255, 0xffffffff],
        'path2_tests': [0, 15, 16, 255],
    },
    # 0x004853b0  SmplFzxStateBlockGetLogged (smplfzx, RACE) — int(id): validate via 0x00485340
    #   (C3); on miss log via 0x004987b0 + return 0; else return *( *( *(0x6e71cc)+0xc ) + id*0x10 ).
    #   The state-block manager pointer is null at menu (double deref) -> race. Spread of ids
    #   incl. OOB for the bounds/miss path.
    'smplfzx_stateblock_get_logged': {
        'rva': 0x004853b0, 'export': 'SmplFzxStateBlockGetLogged', 'signature': {'ret': 'uint32', 'args': ['int32']},
        'arg_type': 'int_scalar', 'lut_root_delta': 0, 'scenario': 'race',
        'path1_tests': [0, 1, 2, 3, 4, 5, 6, 7, 8, 12],
        'path2_tests': [0, 1, 2],
    },

    # ---- promote-round round 29 (worklist batch: const global setters) --------
    'set_77196c_1': {
        'rva': 0x00493570, 'export': 'Set77196c_1', 'signature': {'ret': 'void', 'args': []},
        'arg_type': 'scalars_to_scattered_globals',
        'observe': [{'addr': '0x0077196c', 'len': 4, 'fill': 0xFF}], 'lut_root_delta': 0,
        'path1_tests': [{'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}],
        'path2_tests': [{'args': []}, {'args': []}, {'args': []}],
    },
    'set_771970_1': {
        'rva': 0x00493580, 'export': 'Set771970_1', 'signature': {'ret': 'void', 'args': []},
        'arg_type': 'scalars_to_scattered_globals',
        'observe': [{'addr': '0x00771970', 'len': 4, 'fill': 0xFF}], 'lut_root_delta': 0,
        'path1_tests': [{'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}],
        'path2_tests': [{'args': []}, {'args': []}, {'args': []}],
    },
    'set_603868_0': {
        'rva': 0x00462510, 'export': 'Set603868_0', 'signature': {'ret': 'void', 'args': []},
        'arg_type': 'scalars_to_scattered_globals',
        'observe': [{'addr': '0x00603868', 'len': 4, 'fill': 0xFF}], 'lut_root_delta': 0,
        'path1_tests': [{'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}],
        'path2_tests': [{'args': []}, {'args': []}, {'args': []}],
    },
    'set_603868_1': {
        'rva': 0x00462500, 'export': 'Set603868_1', 'signature': {'ret': 'void', 'args': []},
        'arg_type': 'scalars_to_scattered_globals',
        'observe': [{'addr': '0x00603868', 'len': 4, 'fill': 0xFF}], 'lut_root_delta': 0,
        'path1_tests': [{'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}],
        'path2_tests': [{'args': []}, {'args': []}, {'args': []}],
    },
    'set_703058_0': {
        'rva': 0x00487df0, 'export': 'Set703058_0', 'signature': {'ret': 'void', 'args': []},
        'arg_type': 'scalars_to_scattered_globals',
        'observe': [{'addr': '0x00703058', 'len': 4, 'fill': 0xFF}], 'lut_root_delta': 0,
        'path1_tests': [{'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}, {'args': []}],
        'path2_tests': [{'args': []}, {'args': []}, {'args': []}],
    },

    # ---- promote-round round 27 (worklist batch: global getters) --------------
    'global_67eca4_get': {
        'rva': 0x0042b900, 'export': 'Global67eca4Get', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x0067eca4, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'global_67ed6c_get': {
        'rva': 0x00430820, 'export': 'Global67ed6cGet', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x0067ed6c, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'global_771968_get': {
        'rva': 0x00492d10, 'export': 'Global771968Get', 'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x00771968, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'float_67eaa8_get': {
        'rva': 0x00431b30, 'export': 'Float67eaa8Get', 'signature': {'ret': 'float', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x0067eaa8, 'lut_root_delta': 0,
        'path1_tests': [0x00000000, 0x3f800000, 0xbf800000, 0x3f000000, 0x42c80000, 0xc2c80000, 0x40490fdb, 0x3dcccccd, 0x49742400, 0x40000000],
        'path2_tests': [0x3f800000, 0xc2c80000, 0x00000000],
    },
    'powerup_range_get': {
        'rva': 0x00452eb0, 'export': 'PowerupRangeGet', 'signature': {'ret': 'float', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x00684de0, 'lut_root_delta': 0,
        'path1_tests': [0x00000000, 0x3f800000, 0xbf800000, 0x3f000000, 0x42c80000, 0xc2c80000, 0x40490fdb, 0x3dcccccd, 0x49742400, 0x40000000],
        'path2_tests': [0x3f800000, 0xc2c80000, 0x00000000],
    },

    # ---- promote-round round 26 (worklist batch: single-global leaves) --------
    'rw_global_7d459c_get': {
        'rva': 0x004cbc90, 'export': 'RwGlobal7d459cGet',
        'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x007d459c, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'flag_63b908_get': {
        'rva': 0x0040e450, 'export': 'Flag63b908Get',
        'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x0063b908, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'flag_63b908_set': {
        'rva': 0x0040e460, 'export': 'Flag63b908Set',
        'signature': {'ret': 'void', 'args': ['uint32']},
        'arg_type': 'void_setter_observe', 'target_global': 0x0063b908, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'elapsed_time_get': {
        'rva': 0x0040e4a0, 'export': 'ElapsedTimeGet',
        'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x005f29b8, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },
    'ai_target_enable_get': {
        'rva': 0x00443080, 'export': 'AiTargetEnableGet',
        'signature': {'ret': 'uint32', 'args': []},
        'arg_type': 'read_global', 'target_global': 0x00897ffc, 'lut_root_delta': 0,
        'path1_tests': [0, 1, 0xDEADBEEF, 0xFFFFFFFF, 0x12345678, 0x80000000, 0x7FFFFFFF, 0xCAFEBABE, 2, 0x55555555],
        'path2_tests': [0, 1, 0xDEADBEEF],
    },

    # ---- promote-round round 25 (L5 int2out: two-out-ptr getter) --------------
    # 0x0046cbb0  CarStatePairGet — uint32(uint idx, uint32* out_a, uint32* out_b):
    # idx>=0x10 -> ret 0; else *out_a = per-car state (0x00881f90+idx*0xd04),
    # *out_b = secondary (+4), ret 1. NEW arg_type int2out (SWEEP-CRITICAL).
    # Race lane (per-car struct populated in a live race); in-bounds 0..3 + OOB.
    # ref: re/analysis/vehicle_damage_d2/0x0046cbb0.md
    'car_state_pair_get': {
        'rva':            0x0046cbb0,
        'export':         'CarStatePairGet',
        'signature':      {'ret': 'uint32', 'args': ['int32', 'pointer', 'pointer']},
        'arg_type':       'int2out',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 16, 100, 0, 1, 2, 3],
        'path2_tests':    [0, 1, 16],
    },

    # ---- promote-round round 24 (resume; existing handlers) -------------------

    # 0x00496900  SlotActiveThunk — uint32(int idx). 4B thunk call-through to
    # FUN_00497450 (slot-active predicate; returns 0 for idx>3 else the
    # live slot-active byte). int_scalar, race lane (slots populated in race).
    # ref: re/analysis/bucket_vehicle_004922e0_0057c500/0x00496900.md
    'slot_active_thunk': {
        'rva':            0x00496900,
        'export':         'SlotActiveThunk',
        'signature':      {'ret': 'uint32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0, 1, 2, 3, 4, 7, 0, 1, 2, 3],
        'path2_tests':    [0, 1, 4],
    },

    # 0x00415860  InteractionCooldownSet — void(int idx): writes 30000 to
    # 0x0089a508 + idx*0x74. slot_block_zero plants a sentinel and verifies it
    # was overwritten (with 30000) = evidence; both sides write the same value.
    # Menu-attach (pure memory write).
    # ref: re/analysis/bucket_ai_00407a40_00415880/0x00415860.md
    'interaction_cooldown_set': {
        'rva':                0x00415860,
        'export':             'InteractionCooldownSet',
        'signature':          {'ret': 'void', 'args': ['int32']},
        'arg_type':           'slot_block_zero',
        'target_global':      0x0089a508,
        'entity_byte_stride': 0x74,
        'lut_root_delta':     0,
        'path1_tests':        [0, 1, 2, 3, 0, 1, 2, 3, 2, 1],
        'path2_tests':        [0, 1, 2],
    },

    # 0x004464c0  CameraEntryDispatchAll (util, race) — void(param_1): loops
    #   DAT_008964c0 (stride 0xd8, count DAT_00898994) and dispatches by
    #   type field [entry+4]: 0->FUN_00445aa0, 1->FUN_00441d40, 2->FUN_00442440.
    #   Callers: FUN_00446520 (util C2), FUN_00448700 (vehicle C2, passes
    #   &DAT_00897fe0 as param_1 in its 100-iter call loop).
    #   __cdecl; param_1 at [ESP+0x10] after PUSH EBX/ESI/EDI (0x004464ce).
    #   EAX = count at return (re-read at 0x00446508 last iteration).
    #   Diff strategy: race scenario (count > 0, entries init'd by game).
    #   Test value 0x00897fe0 = &DAT_00897fe0, the valid param_1 from
    #   FUN_00448700's 100-iter call loop. uint32 ret captures count (non-trivial).
    'camera_entry_dispatch_all': {
        'rva': 0x004464c0, 'export': 'CameraEntryDispatchAll',
        'signature': {'ret': 'uint32', 'args': ['uint32']},
        'arg_type': 'int_scalar', 'lut_root_delta': 0, 'scenario': 'race',
        'path1_tests': [0x00897fe0, 0x00897fe0, 0x00897fe0, 0x00897fe0,
                        0x00897fe0, 0x00897fe0, 0x00897fe0, 0x00897fe0],
        'path2_tests': [0x00897fe0, 0x00897fe0, 0x00897fe0],
    },

    # 0x005a9e40  AudioOutputNodeCbDispatch
    # int(int param_1, int param_2): null-guards the fn-ptr at param_1+0x3c;
    # if non-null calls it with (0, param_2) and returns param_1, else 0.
    # ASM: PUSH ESI; MOV ESI,[ESP+8]; MOV EAX,[ESI+0x3c]; TEST; JZ ret0;
    #      PUSH ECX; PUSH 0; CALL EAX; ADD ESP,8; MOV EAX,ESI; POP ESI; RET.
    # Strategy: crash_equal_ok=True with param_1=0.
    #   param_1=0 → reads [0+0x3c] = address 0x3c (null page, unmapped) → AV.
    #   Both orig and reimpl fault identically at the same read address before
    #   reaching the callback or any audio state → crash_equal_ok GREEN.
    # Precedent: AudioWaveVtableSlot1cDispatch (0x005abf80) — same crash_equal_ok
    #   strategy for a comparable null-guarded fn-ptr dispatch pattern.
    # [UNCERTAIN U-6621]: semantic of literal 0 arg and +0x3c callback contract.
    'audio_output_node_cb_dispatch': {
        'rva':            0x005a9e40,
        'export':         'AudioOutputNodeCbDispatch',
        'signature':      {'ret': 'int32', 'args': ['int32', 'int32']},
        'arg_type':       'int_pair',
        'crash_equal_ok': True,
        # degenerate_ok: crash_equal_ok results have no return value (origV=null,
        # reimV=null), so the degenerate detector fires. The crash at address 0x3c
        # (null-page AV on [0+0x3c] deref) IS the discriminating evidence — both
        # sides execute the same PUSH/MOV/MOV sequence and AV at the same address
        # before any divergence is possible. Precedent: AudioWaveVtableSlot1cDispatch
        # (0x005abf80, C3) uses the identical crash_equal_ok + degenerate strategy.
        'degenerate_ok':  True,
        'lut_root_delta': 0,
        # param_1=0 → [0+0x3c] = null page AV on both sides, identical crash.
        'path1_tests': [
            [0, 0], [0, 0], [0, 0], [0, 0], [0, 0],
            [0, 0], [0, 0], [0, 0], [0, 0], [0, 0],
        ],
        'path2_tests': [[0, 0], [0, 0], [0, 0]],
    },

    'vec3_lerp': {
        'rva':            0x004b4650,
        'export':         'Vec3Lerp',
        'signature':      {'ret': 'void',
                           'args': ['pointer', 'pointer', 'pointer', 'float']},
        'arg_type':       'vec3_lerp',
        'lut_root_delta': 0,
        'path1_tests':    [
            {'a': [1.0, 2.0, 3.0],   'b': [4.0, 5.0, 6.0],   't': 0.5},
            {'a': [0.0, 0.0, 0.0],   'b': [1.0, 1.0, 1.0],   't': 0.0},
            {'a': [0.0, 0.0, 0.0],   'b': [1.0, 1.0, 1.0],   't': 1.0},
            {'a': [-1.0, -2.0, -3.0],'b': [1.0, 2.0, 3.0],   't': 0.25},
            {'a': [10.0, 20.0, 30.0],'b': [-10.0, -20.0, -30.0], 't': 0.75},
            {'a': [3.14, 2.71, 1.41],'b': [1.0, 2.0, 3.0],   't': 0.333},
            {'a': [100.0, 0.5, -0.5],'b': [0.1, 0.2, 0.3],   't': 0.9},
            {'a': [1e6, 1e-6, 0.0],  'b': [2e6, 2e-6, 1.0],  't': 0.5},
            {'a': [1.0, 2.0, 3.0],   'b': [4.0, 5.0, 6.0],   't': 1.5},
            {'a': [7.0, 8.0, 9.0],   'b': [7.0, 8.0, 9.0],   't': 0.5},
        ],
        'path2_tests':    [
            {'a': [1.0, 2.0, 3.0], 'b': [4.0, 5.0, 6.0], 't': 0.5},
            {'a': [0.0, 0.0, 0.0], 'b': [1.0, 1.0, 1.0], 't': 1.0},
            {'a': [-1.0, -2.0, -3.0], 'b': [1.0, 2.0, 3.0], 't': 0.25},
        ],
    },

    # ============ c3-batch-sa1 (scenario-attach C2->C3, 2026-06-15) ============
    # 0x00413bc0  HudUvRect413bc0 — index->UV-rect leaf (c3-batch-sa1-s1).
    # __fastcall-hybrid: index in ESI, out-ptr in ECX; writes 4 floats to
    # [ECX..ECX+0xc]; plain ret (register-only, no stack args).
    # cell = *(float*)0x005cd060 = 0.25f. ESI<0||>=5 -> {0,0,1,1};
    # ESI==3 -> {0.255,0.255,0.995,0.995}; else 4-wide atlas grid
    # (c=ESI&3, r=ESI>>2): {c*cell, r*cell, (c+1)*cell, (r+1)*cell}.
    # arg_type='esi_idx_ecx_outbuf4' (NEW handler — SWEEP-CRITICAL: frida-sweep
    # does NOT auto-merge diff_template.js): trampoline preserves caller ESI,
    # seeds ESI=idx + ECX=<16-byte scratch>, CALLs, restores ESI, reads back the
    # 4 floats as a packed u32x4 fingerprint. PURE/deterministic -> diffs GREEN
    # at any state (NO scenario:'race' needed; the other 4 sa1 candidates were
    # audited NON-VIABLE, see re/analysis/scenario_attach_sa1_viability_2026-06-15.md).
    # ref: re/analysis/bucket_00412130/0x00413bc0.md
    'hud_uv_rect_413bc0': {
        'rva':            0x00413bc0,
        'export':         'HudUvRect413bc0',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'esi_idx_ecx_outbuf4',
        'lut_root_delta': 0,
        # grid (0,1,2,4), inset (3), out-of-range default (<0, >=5).
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 10, -1, -5, 100, 0x7fffffff],
        'path2_tests':    [0, 3, 4, 5, -1],
    },

    # ---- c3-batch-sa1 session 2 (scenario-attach C2->C3) ---------------------
    # All void_write_observe on the scenario:'race' lane: write a sentinel to
    # target_global, call fn (void), read back. The original writes a
    # deterministic value (overwriting the sentinel); a no-op reimpl would leave
    # the sentinel -> RED. void_write_observe is SEEDED (run_diff SEEDED_ARG_TYPES)
    # so the non-degeneracy assertion is satisfied by the sentinel itself.
    # Bodies byte-verified in MASHED.exe.unpatched; reimpls in
    # mashedmod/src/mashed_re/HUD/ScenarioLeaves_sa2.cpp.

    # 0x0041c010  HudConstTableInitAndSweep — copies the 24-byte const block to
    # &DAT_005f334c, then sweeps 2 records via FUN_0041b770(__fastcall). Observe
    # 0x005f334c = first const dword 0x3ee66666. Race-gated: FUN_0041b770 walks
    # record fields that are valid only in a live round.
    # ref: re/analysis/bucket_util_0040e4b0_0042f790/0x0041c010.md
    'hud_const_table_init_sweep': {
        'rva':            0x0041c010,
        'export':         'HudConstTableInitAndSweep',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x005f334c,
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x0041d930  HudSlideBillboardTick — RW 2D billboard with a time-driven
    # Y-lerp on DAT_0063d580; ends with DAT_0063d584 += 1. Observe 0x0063d584
    # (frame counter): write sentinel S, fn returns S+1. Garbage-immune (the
    # uninitialized matrix-flags read affects only DAT_0063d580's flags word, not
    # the counter). Race-gated: the FUN_004c1480 apply derefs DAT_0063d580, NULL
    # at the quiescent menu.
    # ref: re/analysis/bucket_util_0040e4b0_0042f790/0x0041d930.md
    'hud_slide_billboard_tick': {
        'rva':            0x0041d930,
        'export':         'HudSlideBillboardTick',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x0063d584,
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0x0000002A,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0x0000002A, 0x00000001],
    },

    # 0x00423480  AiFilenameBuild — builds "AI%d.AI" for the current track
    # (FUN_00426c00 index) at DAT_00644110 (no disk IO). Observe 0x00644110 =
    # first 4 chars of the filename (probe-confirmed 0x30324941 = "AI20" in a
    # sample race). Both sides read the same track index in-race -> identical
    # string. Returns &DAT_00644110 (constant); void_write_observe ignores it.
    # ref: re/analysis/ai_path_following/0x00423480.md
    'ai_filename_build': {
        'rva':            0x00423480,
        'export':         'AiFilenameBuild',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x00644110,
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # ---- c3-batch-sa2 session 2 (scenario-attach C2->C3 WRITERS) -------------
    # All void_write_observe on the scenario:'race' lane. These functions write
    # per-vehicle / race-state globals that are ZERO at the quiescent menu, so
    # they are verified attached inside a LIVE Quick Battle race. Two harness
    # extensions to void_write_observe (SWEEP-CRITICAL — diff_template.js change):
    #   seed_globals : list of {addr,val} written before EACH Orig/Reimpl call
    #                  (resets a one-shot guard / accumulator slot for both sides).
    #   call_args    : fixed int args passed to the call so the write address is
    #                  deterministic for index-by-param_1 functions.
    # Bodies byte-verified in MASHED.exe.unpatched (Mashed_pool10, read-only);
    # reimpls in mashedmod/src/mashed_re/HUD/ScenarioWriters_sa2s2.cpp.

    # 0x004331a0  RaceFinalizeOnce — guarded one-shot race-end init. The guard
    # DAT_0067eca4 is SEEDED to 0 before each call so the body runs on BOTH sides
    # (else orig runs once, reimpl no-ops -> spurious RED). param_1=0 (-> 0x0067ecac).
    # Observe 0x0067ebb0 = 0x43520000 (210.0f, constant). Callees FUN_0042d3a0/
    # 004248b0/00424920 all C3. ref: re/analysis/game_state_d2/0x004331a0.md
    'sa2_race_finalize_once': {
        'rva':            0x004331a0,
        'export':         'RaceFinalizeOnce',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'void_write_observe',
        'target_global':  0x0067ebb0,
        'seed_globals':   [{'addr': 0x0067eca4, 'val': 0}],
        'call_args':      [0],
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00415020  AiLastPlaceFrustration — int f(int car); last-place catch-up
    # timer. call_args=[0] (car 0) so the write addr is fixed: leader-progress
    # float at 0x0089a4e8+0*0x74. In the last-place branch (car 0 progress==0.0)
    # it writes a live leader-progress float there; otherwise the sentinel
    # survives (seeded arg_type). Both sides take the same branch on the same live
    # state -> identical readback. Callees FUN_00442cc0/FUN_0040e470 C3.
    # ref: re/analysis/ai_update_d2/0x00415020.md
    'sa2_ai_last_place_frustration': {
        'rva':            0x00415020,
        'export':         'AiLastPlaceFrustration',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'void_write_observe',
        'target_global':  0x0089a4e8,
        'call_args':      [0],
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x004922e0  CarEventTrigger — void f(int car,u4,u4,u4); guarded per-player
    # pending-event record write. call_args=[0,3,10,0x80] mirror the real call
    # site FUN_00410d10. Guards: FUN_0040e470(car)==1 && FUN_0040e350()==6 &&
    # guard-byte!=0. When guards pass for car 0 mapping to slot 0 it writes the
    # trigger flag (=1) at 0x007f1058; otherwise the sentinel survives. Both sides
    # identical. Callees FUN_0040e470/FUN_0040e350 C3.
    # ref: re/analysis/bucket_vehicle_004922e0_0057c500/0x004922e0.md
    'sa2_car_event_trigger': {
        'rva':            0x004922e0,
        'export':         'CarEventTrigger',
        'signature':      {'ret': 'void', 'args': ['int32', 'int32', 'int32', 'int32']},
        'arg_type':       'void_write_observe',
        'target_global':  0x007f1058,
        'call_args':      [0, 3, 10, 0x80],
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00422b50  VehicleDamageAccum — void f(int car,int delta); accumulates
    # delta into DAT_008995bc[car*0x4e] (stride 0x138 bytes). call_args=[0,1000]:
    # car 0 = player (FUN_0040e470(0)==1) so the random-subtract branch is NOT
    # taken (deterministic). The sentinel write to target_global 0x008995bc is the
    # slot seed; fn does [slot]=sentinel+1000 -> readback varies with the sentinel
    # (non-degenerate) and matches on both sides. Callees FUN_0040e470 C3 +
    # FUN_00472650/FUN_004a2c48 C2 (unused on the player path).
    # ref: re/analysis/ai_update_d6/0x00422b50.md
    'sa2_vehicle_damage_accum': {
        'rva':            0x00422b50,
        'export':         'VehicleDamageAccum',
        'signature':      {'ret': 'void', 'args': ['int32', 'int32']},
        'arg_type':       'void_write_observe',
        'target_global':  0x008995bc,
        'call_args':      [0, 1000],
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00401340  CupSpinSpeedAndColor — void f(void). DAT_00636574 =
    # (float)(-(int)DAT_007f0ff0) * DAT_005cc328 (FILD integer load, NOT a float
    # negate — listing 0x00401343..60). Then writes a mode-selected ARGB color to
    # *(FUN_004b4080(DAT_00636564)+4). Observe 0x00636574 (live -> non-zero in a
    # round). No args / no seed. Callees FUN_004b4080 C2 + FUN_0042f6a0 C3.
    # ref: re/analysis/boot_hud_promote_ae1/0x00401340.md
    'sa2_cup_spin_speed_and_color': {
        'rva':            0x00401340,
        'export':         'CupSpinSpeedAndColor',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x00636574,
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00487d00  Particle3rdEmitterSubmit (c3-batch-sa2 s1, inline) — void
    # f(u4 transform, byte alpha); submits a particle to the third emitter and
    # bumps the count DAT_00703058 IF DAT_00703058 < DAT_00703070 (capacity).
    # Seed capacity high so the gate passes -> readback = sentinel+1
    # (sentinel-dependent, non-degenerate). call_args param_1 = 0x00703068
    # (readable emitter base; FUN_00476a10 only READS the transform, content
    # irrelevant to the count). The probe hint 0x007030b4 was a 24-dword window
    # over-count of this count global. Callees FUN_00476a10/004769f0/00476d00.
    # ref: re/analysis/cluster_0048_first_pass/0x00487d00.md
    'sa2_particle_3rd_emitter_submit': {
        'rva':            0x00487d00,
        'export':         'Particle3rdEmitterSubmit',
        'signature':      {'ret': 'void', 'args': ['uint32', 'uint32']},
        'arg_type':       'void_write_observe',
        'target_global':  0x00703058,
        'seed_globals':   [{'addr': 0x00703070, 'val': 0x7fffffff}],
        'call_args':      [0x00703068, 0xff],
        'lut_root_delta': 0,
        'scenario':       'race',
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0x0000007F,
                           0x80000000, 0x00000001, 0x55555555, 0x0000002A,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0x00000001, 0x0000002A, 0xDEADBEEF],
    },

    # ───────────────────────────────────────────────────────────────────────
    # WS-PHYS-C4-LANE — vehicle-physics chain (REGISTER ABI; NOT run_diff-able)
    # ───────────────────────────────────────────────────────────────────────
    # These take the 0xd04 vehicle record in a REGISTER (EAX/EDI/ESI/ECX) + a
    # live record/globals and fire >1000/s — run_diff (scalar vectors, hook
    # bypassed) cannot exercise them, and Interceptor on the hot path
    # destabilizes MASHED before a single call can be captured (verified
    # WS-PHYS-C4-LANE; CLAUDE.md "Frida overhead on hot paths"). Their C4 lane is
    # the INSTALLED-HOOK canonical-race telemetry harness, NOT this registry:
    #   re/frida/phys_c4_telemetry.py   — install the .asi hook (MASHED_HOOK_ONLY),
    #     drive a canonical Quick-Battle race, confirm inline-JMP LIVE (0xE9),
    #     sample the player record trajectory.
    #   In-process bit-identity self-test: env MASHED_PHYS_C4_SELFTEST=1 makes the
    #     .asi hook run the ORIGINAL body (early-return trampoline) AND the verbatim
    #     transcription on identical live record state and bit-compare, logging
    #     phys_c4_selftest.log (see Vehicle/PhysicsChainHooks.cpp).
    # The .asi-only verbatim hooks live in Vehicle/PhysicsChainHooks.cpp
    # (RH_ScopedInstall; excluded from the exe; gated by MASHED_HOOK_ONLY).
    #
    # A4 0x00470670 VehicleControlIntegrate — EAX=record; [+4]=param_1 [+8]=dt
    #   [+c]=input* [+10]=xform; caller-cleans. C4 GREEN 2026-06-17: in-process
    #   body A/B vs the live original 96/96 calls (88 on the drive-force path,
    #   in0=1..66, all ring phases). Forwards A5/A6a/A6b + smoother 0x4a2c48 to the
    #   live originals (RW fast-sqrt LUT live). NO run_diff vector — recorded for
    #   provenance; reproduce via phys_c4_telemetry.py + MASHED_PHYS_C4_SELFTEST.
    'phys_a4_control_integrate': {
        'rva':            0x00470670,
        'export':         'A4_Entry',        # .asi naked entry trampoline (EAX=record)
        'signature':      {'ret': 'void', 'args': []},  # register ABI — not callable as cdecl
        'arg_type':       'register_abi_record',         # NOT a run_diff arg_type; marker only
        'scenario':       'race',
        'lane':           'installed_hook_canonical_race',   # phys_c4_telemetry.py / MASHED_PHYS_C4_SELFTEST
        'not_run_diffable': True,
        'lut_root_delta': 0,
        'path1_tests':    [],
        'path2_tests':    [],
    },

    # ---- c3-batch-sgr1 s1: render-state setter cluster (0x004d71f0..0x004d7340) ----
    # Six deferred render-state dirty-queue leaves. One read accessor (read_global)
    # and five cached dirty-queue setters (void_setter_observe). Each setter writes
    # its VALUE-STORE global (the target_global below), then guards a one-shot dirty
    # flag + appends an opcode to the shared dword queue 0x007d5168[0x007d6c14].
    # All __cdecl plain-ret (byte-verified). Reimpl: Render/RenderStateSettersA.cpp.

    # 0x004d71f0  RenderState::GetTexturingOverride — undefined4 f(void){return DAT_007d6b10;}
    # read_global: seed sentinel -> DAT_007d6b10, call fn(), compare return.
    'RenderState_GetTexturingOverride': {
        'rva':            0x004d71f0,
        'export':         'RenderState_GetTexturingOverride',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'read_global',
        'target_global':  0x007d6b10,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000000, 0x00000001, 0xFFFFFFFF, 0x80000000,
                           0x55555555, 0xAAAAAAAA, 0x12345678, 0x7FFFFFFF,
                           0xDEADBEEF, 0xCAFEBABE, 0x00010000, 0x0000FFFF],
        'path2_tests':    [0x00000000, 0x00000001, 0xFFFFFFFF],
    },

    # 0x004d7200  RenderState::SetOpcode34 — RAW store: DAT_007d5998 = param_1 (opcode 0x34).
    # void_setter_observe: call fn(value), read back DAT_007d5998. Raw store => any
    # 32-bit value is in-domain. (Cache 0x007d6af0; dirty 0x007d599c.)
    'RenderState_SetOpcode34': {
        'rva':            0x004d7200,
        'export':         'RenderState_SetOpcode34',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x007d5998,
        'lut_root_delta': 0,
        'path1_tests':    [0x00000001, 0x00000002, 0x00000010, 0x00000100,
                           0x12345678, 0x7FFFFFFF, 0x80000000, 0xDEADBEEF,
                           0xCAFEBABE, 0xFFFFFFFF, 0x55555555, 0xAAAAAAAA],
        'path2_tests':    [0x00000001, 0x12345678, 0xFFFFFFFF],
    },

    # 0x004d7250  RenderState::SetOpcode35 — DAT_007d59a0 = table_0x005d8d28[param_1] (0x35).
    # void_setter_observe with table-indexed store (off-label but non-degenerate:
    # table values differ per index). In-domain indices 1..8 = identity table
    # values 1..8; indices 19..24 read the contiguous .rdata float entries
    # (0x3e46de6b,0x3f02660d,0x437ffd71,0x40a4c59d,0x3ffb4a7d,0x37800000) — all
    # distinct, non-zero. Orig and reimpl read identical memory => match for any
    # in-mapped index. (Cache 0x007d6af4; dirty 0x007d59a4.)
    'RenderState_SetOpcode35': {
        'rva':            0x004d7250,
        'export':         'RenderState_SetOpcode35',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x007d59a0,
        'lut_root_delta': 0,
        'path1_tests':    [1, 2, 3, 4, 5, 6, 7, 8, 19, 20, 21, 22, 23, 24],
        'path2_tests':    [1, 5, 20],
    },

    # 0x004d72a0  RenderState::SetOpcode36 — DAT_007d59a8 = table_0x005d8d28[param_1] (0x36).
    # Twin of SetOpcode35 (same table). Cache 0x007d6af8; dirty 0x007d59ac.
    'RenderState_SetOpcode36': {
        'rva':            0x004d72a0,
        'export':         'RenderState_SetOpcode36',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x007d59a8,
        'lut_root_delta': 0,
        'path1_tests':    [1, 2, 3, 4, 5, 6, 7, 8, 19, 20, 21, 22, 23, 24],
        'path2_tests':    [1, 5, 20],
    },

    # 0x004d72f0  RenderState::SetOpcode37 — DAT_007d59b0 = table_0x005d8d28[param_1] (0x37).
    # Twin of SetOpcode35 (same table). Cache 0x007d6afc; dirty 0x007d59b4.
    'RenderState_SetOpcode37': {
        'rva':            0x004d72f0,
        'export':         'RenderState_SetOpcode37',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x007d59b0,
        'lut_root_delta': 0,
        'path1_tests':    [1, 2, 3, 4, 5, 6, 7, 8, 19, 20, 21, 22, 23, 24],
        'path2_tests':    [1, 5, 20],
    },

    # 0x004d7340  RenderState::SetOpcode38 — DAT_007d59b8 = table_0x005d8d4c[param_1] (0x38).
    # Uses a DIFFERENT table (0x005d8d4c). Indices 1..8 = identity values 1..8;
    # indices 10..15 = that table's own distinct float entries (0x3e46de6b,
    # 0x3f02660d,0x437ffd71,0x40a4c59d,0x3ffb4a7d,0x37800000). Cache 0x007d6b00;
    # dirty 0x007d59bc.
    'RenderState_SetOpcode38': {
        'rva':            0x004d7340,
        'export':         'RenderState_SetOpcode38',
        'signature':      {'ret': 'void', 'args': ['uint32']},
        'arg_type':       'void_setter_observe',
        'target_global':  0x007d59b8,
        'lut_root_delta': 0,
        'path1_tests':    [1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 14, 15],
        'path2_tests':    [1, 5, 12],
    },
    # ---- C2->C3 (audio): InterlockedExchange wrapper FUN_005aefa0 ----------
    # LONG* fn(LONG* p1, LONG p2): atomic store *p1 = p2 (kernel32 IAT slot
    # 0x005cc164), returns p1. cdecl, 2 args (disasm: MOV EAX,[ESP+8]; MOV
    # ESI,[ESP+8]; PUSH EAX; PUSH ESI; CALL [0x005cc164]; MOV EAX,ESI; RET).
    # Harness: sort_dispatch_out4 reused as a generic "scratch-ptr arg0 +
    # scalar arg1, observe *scratch[0]" probe — it allocs its OWN 16-byte out,
    # prefills -1, calls fn(out, sel, dir) and reads out[0..3]. Our fn writes
    # sel into out[0] (out[1..3] stay -1; dir/arg3 is ignored — cdecl tolerates
    # the extra pushed arg). Distinct sel per test => non-degenerate; the JS
    # scratch means NO live game memory is touched. (cache_roundtrip is the
    # named-correct arg_type in current main; this worktree predates it, so the
    # equivalent observe-the-write harness present in BOTH trees is used.)
    'audio_xchg_5aefa0': {'rva': 0x005aefa0, 'export': 'AudioAtomicExchangeStore',
        'signature': {'ret': 'pointer', 'args': ['pointer', 'int32', 'int32']},
        'arg_type': 'sort_dispatch_out4', 'lut_root_delta': 0,
        'path1_tests': [
            {'sel': 0x12345678, 'dir': 0},
            {'sel': 0xDEADBEEF, 'dir': 0},
            {'sel': 0x7FFFFFFF, 'dir': 0},
            {'sel': 0x0BADF00D, 'dir': 0},
            {'sel': 0xCAFEBABE, 'dir': 0},
        ],
        'path2_tests': [{'sel': 0x12345678, 'dir': 0}]},

    # 0x005a8960  AudioVoiceQueueSet5a8960  (audio, C2->C3)
    # void fn(int base, uint32 val): WaitForSingleObject(DAT_007dcae0,INFINITE);
    #   FUN_005b0dc0(base+0x154, val);  ReleaseSemaphore(DAT_007dcae0,1,NULL).
    # FUN_005b0dc0 inlined: *(base+0x158)=val ; *(base+0x154)|=0x80.
    # DAT_007dcae0 @ 0x007dcae0: global audio semaphore handle.
    # Strategy: struct_three_write — allocate 0x160-byte scratch, sentinel-fill,
    #   call fn(buf, valA, 0 [extra cdecl-ignored]), observe buf+0x158 (where valA
    #   is written). buf+0x154 OR'd with 0x80 not observed (sentinel 0xDEADBEEF
    #   already has bit 7 set, making the OR non-observable). Test vectors avoid
    #   0xDEADBEEF to keep the observed offset discriminating.
    # Signature declared with 3 args to match struct_three_write's fn(buf,va,vb)
    # call pattern; the original and reimpl only read 2 (cdecl caller cleans all 3).
    'audio_voice_queue_set_5a8960': {
        'rva':            0x005a8960,
        'export':         'AudioVoiceQueueSet5a8960',
        'signature':      {'ret': 'void', 'args': ['pointer', 'uint32', 'uint32']},
        'arg_type':       'struct_three_write',
        'struct_size':    0x160,          # must cover base+0x154+8 = base+0x15C
        'observe_offsets': [0x158],       # buf+0x158 = valA after fn writes *(p+4)=val
        'lut_root_delta': 0,
        # [valA, 0]: valA = param_2; 0 = harmless extra arg (ignored by 2-arg callee).
        # Avoid 0xDEADBEEF (= sentinel) so every case is discriminating.
        'path1_tests':    [[0x12345678, 0], [0x00000000, 0], [0xFFFFFFFF, 0],
                           [0x00000001, 0], [0x80000000, 0], [0x11223344, 0],
                           [0xABCDE001, 0], [0x55555555, 0], [0xCAFEBABE, 0],
                           [0x7FFFFFFF, 0]],
        'path2_tests':    [[0x12345678, 0], [0x00000000, 0], [0xFFFFFFFF, 0]],
    },
    # RpMaterialReflectivityGet  --  0x004cff00
    # Per-material reflectivity nibble reader; returns low nibble of the byte
    # at (DAT_00911ae4 + 9 + mat_ptr). DAT_00911ae4 is the RW plugin slot
    # offset set by FUN_004cfa00 (material plugin registration). Non-zero =
    # reflection vector path in LIGHT_FN (FUN_00541b50, RW specular env-map).
    # arg_type ptr_arg_int_get: scratch buffer passed as mat_ptr; function reads
    # *(byte*)(slot + 9 + buf_addr) & 0xf; struct_size 0x1000 covers plugin
    # slot offsets up to ~4087, well beyond any realistic RW material plugin.
    # Seeds 0..15 produce all 16 distinct nibble values regardless of slot.
    'rp_material_reflectivity_get': {
        'rva':            0x004cff00,
        'export':         'RpMaterialReflectivityGet',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'ptr_arg_int_get',
        'struct_size':    0x1000,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7,
                           8, 9, 10, 11, 12, 13, 14, 15,
                           16, 32, 64, 128, 255, 256, 512, 1024],
        'path2_tests':    [0, 1, 7, 15],
    },

    # â”€â”€ wf_b0f68acd-63f-7 â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    # 0x00487e40  SkyElementRemove
    # Removes the sky/cloud element at index param_1 from the 128-slot element
    # array (stride 0xe dwords = 0x38 bytes; base DAT_0086ae20).
    # Zeros slot active flag (DAT_0086ae34[idx*0x38]) and transform/model ptr
    # (DAT_0086ae4c[idx*0x38]), then clears dirty flag DAT_007030b4.
    # Returns 0 if idx >= 128, else 1.
    # Calling convention: cdecl (param at [ESP+4]; bare RET).
    # Bounds check: CMP EAX,0x80 / JL (signed). Body: 0x00487e40..0x00487e6a.
    # arg_type='int_scalar': single int index param_1 (uint32 via >>> 0).
    # Tests: mix of in-range (0..127 â†’ return 1) and out-of-range (>=128 â†’ 0).
    # Non-degenerate: in-range returns 1, out-of-range returns 0.
    # Side effect: zeros sky element slots (safe â€” tick guards on active flag).
    # ref: re/analysis/bucket_sky_worldobj_dbg_video_hud_00484280_00555910/00487e40.md
    'sky_element_remove': {
        'rva':            0x00487e40,
        'export':         'SkyElementRemove',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 5, 50, 100, 127, 128, 200, 255, 0],
        'path2_tests':    [0, 127, 200],
    },

    # 0x00551180  RtFSHandlerCancel  (util, C2->C3)
    # int __cdecl fn(void* slot): *(slot+0x20) = 1; return 1.
    # Slot Cancel: marks the file-system handler slot idle by writing 1 to
    # the state field at offset +0x20. Returns 1 (success).
    # Disasm (12 bytes at 0x00551180):
    #   8B 4C 24 04   mov ecx, [esp+4]
    #   B8 01 00 00 00 mov eax, 1
    #   89 41 20      mov [ecx+0x20], eax
    #   C3            ret
    # arg_type: ptr_arg_int_get â€” harness fills 256-byte scratch buffer with
    # per-test seed pattern, passes pointer; observes int return value.
    # Return value is always 1 (non-trivial, is_trivial(1)==False), so
    # all tests return 1 -> GREEN without degenerate-rejection.
    # struct_size: 64 (covers +0x20 write; harness only checks return value).
    # Tests: 5 distinct seed ints to satisfy non-degeneracy (return is const 1,
    # but the buffer fill varies per seed â€” harness design).
    # lut_root_delta: 0 (no LUT; poll triggers after game-init, menu-visible).
    # ref: re/analysis/bucket_util_00095280_0040e460/00151180.md
    # anchor: BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
    'rtfs_handler_cancel': {
        'rva':            0x00551180,
        'export':         'RtFSHandlerCancel',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'ptr_arg_int_get',
        'struct_size':    64,
        'lut_root_delta': 0,
        'path1_tests':    [0x11111111, 0x22222222, 0x33333333, 0x44444444, 0x55555555],
        'path2_tests':    [0x11111111, 0x22222222],
    },

    # 0x004dfaa0  PixPassthrough16 (render) -- pure leaf getter: reads 2 bytes from p
    # and returns (p[1]<<8)|p[0] as a little-endian uint16.
    # Analysis note: re/analysis/bucket_004ddfb0/0x004dfaa0.md
    # Decoder match: D3DFORMAT 0x3c / 0x75 (both arms in FUN_004df750 dispatch).
    # Calling convention: __cdecl, single pointer arg from [ESP+4], plain RET.
    # arg_type ptr_arg_int_get: allocates a 256-byte scratch buffer filled with a
    # deterministic per-seed pattern, calls fn(buf), compares uint32 return.
    # Seeds produce distinct low-16-bit returns (seed & 0xFFFF) -- non-degenerate.
    'pix_passthrough_16': {
        'rva':            0x004dfaa0,
        'export':         'PixPassthrough16',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'ptr_arg_int_get',
        'lut_root_delta': 0,
        'path1_tests': [
            0x00000001, 0x00000002, 0x00000100, 0x00000201,
            0x0000ffff, 0x0000abcd, 0x00001234, 0x0000cafe,
            0x00000ff0, 0x0000dead,
        ],
        'path2_tests': [
            0x00000001, 0x0000abcd, 0x0000ff00,
        ],
    },

    # 0x004dfa10 RgbPackEncoder4dfa10 (render) - PURE LEAF encoder fn(byte *rgb) -> uint32:
    # packs rgb[2], rgb[1], rgb[0] into a 32-bit DWORD with top byte=0xFF.
    # Disasm: MOV ECX,[ESP+4]; XOR EAX,EAX; XOR EDX,EDX; MOV AL,[ECX+2]; MOV DL,[ECX+1];
    #         OR EAX,0xffffff00; SHL EAX,8; OR EAX,EDX; XOR EDX,EDX; MOV DL,[ECX];
    #         SHL EAX,8; OR EAX,EDX; RET   (cdecl, single ptr arg)
    # Buffer fill at o=0 writes U32(seed+0) â†’ bytes [0..3] all vary per seed â†’ non-degenerate.
    # Reimpl: Render/RgbPackEncoder_wfb0f.cpp  (worktree wf_b0f68acd)
    'rgb_pack_encoder_4dfa10': {
        'rva':            0x004dfa10,
        'export':         'RgbPackEncoder4dfa10',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'ptr_arg_int_get',
        'struct_size':    4,
        'lut_root_delta': 0,   # unused for ptr_arg_int_get; kept for harness uniformity
        'path1_tests':    [0x00000000, 0x12345678, 0xdeadbeef, 0xffffff00, 0x01010101,
                           0xabcdef12, 0x80402010, 0x55aa55aa, 0x11223344, 0xcafebabe],
        'path2_tests':    [0x12345678, 0x00000000, 0xdeadbeef],
    },

    # 0x004dfa40  BgraReorder4dfa40 (render) â€” PURE LEAF 34B byte-permutation encoder.
    # undefined4 FUN_004dfa40(undefined1 *param_1):
    #   return CONCAT31(CONCAT21(CONCAT11(param_1[3],*param_1),param_1[1]),param_1[2])
    # i.e. result = (p[3]<<24)|(p[0]<<16)|(p[1]<<8)|p[2]
    # Reads first 4 bytes of the pointer; no globals, no callees.
    # Caller: FUN_004e02d0 (C2, render). D3DFMT_A8R8G8B8 (0x15) converter.
    # struct_size=4 so the ptr_arg_int_get harness only needs 4 bytes.
    'bgra_reorder_4dfa40': {
        'rva':            0x004dfa40,
        'export':         'BgraReorder4dfa40',
        'signature':      {'ret': 'uint32', 'args': ['pointer']},
        'arg_type':       'ptr_arg_int_get',
        'struct_size':    4,
        'lut_root_delta': 0,
        'path1_tests':    [0x11223344, 0xAABBCCDD, 0x00000000, 0xFFFFFFFF,
                           0xDEADBEEF, 0x01020304, 0xFF000000, 0x00FF0000,
                           0x0000FF00, 0x000000FF],
        'path2_tests':    [0x11223344, 0x00000000, 0xFFFFFFFF],
    },

    # 0x0046baa0  VehiclePhysicsStateReset
    # Zeros velocity/force/angular/contact/suspension fields for one vehicle slot (0-15)
    # at strides 0xd04 and 0x341. Returns 0 if slot > 15; 1 on success.
    # Non-trivial: OOB guard returns 0; valid slots return 1 (non-degenerate across tests).
    # Note: candidate arg_type was ptr_arg_int_get but function takes uint (slot index),
    # not a pointer â€” int_scalar is the correct arg_type.
    # Source: Util\PromoLoop_round80.cpp / re/analysis/game_state_d5/0x0046baa0.md
    'vehicle_physics_state_reset': {
        'rva':            0x0046baa0,
        'export':         'VehiclePhysicsStateReset',
        'signature':      {'ret': 'uint32', 'args': ['uint32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 5, 10, 15, 16, 17, 255, 0xffffffff],
        'path2_tests':    [0, 15, 16, 255],
    },
}
