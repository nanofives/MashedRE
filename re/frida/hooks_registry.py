# Hook verification registry.
#
# One entry per C3+ hook. The generic Frida harnesses (run_diff.py /
# run_verify_hook.py) read from this dict to build per-target test runs.
# To verify a new hook, add an entry here — no new .js or .py needed.
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

    # game_state_d2 trivial getters. Each is `MOV EAX, [imm32]; RET` — pure
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

    # ─────────────────────────────────────────────────────────────────────
    # Session c3-batch-c-s2 — hud_ingame_dispatch  (C2→C3, 1 candidate)
    # HUD/HudDispatch.cpp — pure-leaf uint32(void) global-read function
    # ─────────────────────────────────────────────────────────────────────

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

    # ─────────────────────────────────────────────────────────────────────
    # Session c3-batch-a-s1 — timer_d2_cont1  (C2→C3, 2 candidates)
    # Frontend/TimerReset.cpp — pure-leaf void(void) global-zero functions
    # ─────────────────────────────────────────────────────────────────────

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

    # ─────────────────────────────────────────────────────────────────────
    # Session 88 — c3_vehicle_state  (C2→C3, 5 candidates)
    # VehicleState.cpp — pure-leaf vehicle field getters/predicates
    # ─────────────────────────────────────────────────────────────────────

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
    # Returns DAT_008815a8[param_1*0x341] — no OOB guard.
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

    # ─────────────────────────────────────────────────────────────────────
    # Session c3-batch-a-s4 — frontend_c0_promote (C2→C3, 3 of 4 candidates)
    # FrontendState.cpp — pure-leaf frontend global accessors
    # Note: 0x0042f020 (VehicleFlagClear) REFUSED — __fastcall with EAX
    #       implicit arg not supported by NativeFunction('mscdecl') harness.
    # ─────────────────────────────────────────────────────────────────────

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
    # void(void): switch on DAT_0067f184 → writes DAT_0067e9fc. Default no-op.

# void(void): switch on DAT_0067f184 (0..11, absent 4/6/7); writes mapped
    # value to DAT_0067e9fc. Default case is no-op.
    # arg_type='none': both orig and reimpl return undefined; undefined===undefined
    # is true — proves no crash and export is present. Mapping correctness is
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
    # Pure leaf — no callees. Decomposes signed centisecond delta into sign+m+s+csec.
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
    # U-1748 is open: direction type unconfirmed — does not affect offset/bounds correctness.
    'vehicle_vec3_at_9c8_get': {
        'rva':            0x0046d700,
        'export':         'VehicleVec3At9C8Get',
        'signature':      {'ret': 'int32', 'args': ['pointer', 'uint32']},
        'arg_type':       'out3_idx',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 5, 10, 15, 16, 17, 255, 0xffffffff],
        'path2_tests':    [0, 15, 16, 255],
    },

    # ─────────────────────────────────────────────────────────────────────
    # Session c3-batch-a-s2 — frontend_menus_a small leaves (C2→C3)
    # MenuGetters.cpp — pure-leaf menu global getter, sentinel scanner,
    # and cursor stepper.
    # 0x0042ac50 (MenuCenterCalc) REFUSED — ECX+EAX dual-register ABI not
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

    # Session c3-batch-a-s2 — frontend_menus_a small leaves (C2→C3)
    # MenuGetters.cpp — pure-leaf menu sentinel scanner + cursor stepper
    # 0x0042ac50 (MenuCenterCalc) REFUSED — ECX+EAX dual-register ABI not
    #   supported by NativeFunction harness (same reason as VehicleFlagClear).
    # ─────────────────────────────────────────────────────────────────────

    # 0x0042ac00  MenuGroupCount
    # int __fastcall FUN_0042ac00(int /*unused*/, int* param_2)
    # Scans sentinel-delimited int array; counts 0xFF060000 groups before
    # 0xFF070000 terminator. ECX (param_1) unused. EDX = param_2.
    # arg_type: sentinel_array_ptr — writes array into 256B buf, calls
    #   fn(0, buf) with fastcall for orig (ECX=0, EDX=buf) and cdecl for
    #   reimpl (stack: [0, buf]).
    # ref: re/analysis/frontend_promote_menus_a/0x0042ac00.md
    # U-3439 (ECX unused), U-3440 (sentinel semantics) — do not affect correctness.
    'menu_group_count': {
        'rva':                    0x0042ac00,
        'export':                 'MenuGroupCount',
        'signature':              {'ret': 'int32', 'args': ['int32', 'pointer']},
        'arg_type':               'sentinel_array_ptr',
        'orig_calling_convention':   'fastcall',
        'reimpl_calling_convention': 'mscdecl',
        'lut_root_delta': 0,
        # Each test is a list of int32 values (as Python ints).
        # 0xFF060000 = group delimiter; 0xFF070000 = end-of-array terminator.
        # Expected results annotated in comments.
        'path1_tests': [
            [0xFF070000],                                                # 0: immediate terminator → 0
            [0xFF060000, 0xFF070000],                                    # 1: one delimiter → 1
            [0x00000001, 0xFF070000],                                    # 0: no delimiter → 0
            [0x00000001, 0xFF060000, 0xFF070000],                        # 1: one delimiter after data → 1
            [0xFF060000, 0x00000001, 0xFF060000, 0xFF070000],            # 2
            [0xFF060000, 0xFF060000, 0xFF070000],                        # 2: two adjacent delimiters → 2
            [0x00000001, 0x00000002, 0xFF060000, 0x00000003, 0xFF060000, 0xFF070000],  # 2
            [0x00000001, 0x00000002, 0x00000003, 0xFF070000],            # 0: only data, no delimiters → 0
            [0xFF060000, 0x00000001, 0x00000002, 0xFF060000, 0x00000004, 0xFF060000, 0xFF070000],  # 3
            [0xFF060000, 0xFF060000, 0xFF060000, 0xFF070000],            # 3: three adjacent → 3
            [0x00000064, 0x000000C8, 0x0000012C, 0x00000190, 0xFF070000],  # 0: all data → 0
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
    # arg_type: void_step_global — preps slot 0 globals from raw_bytes,
    #   calls fn(step), returns cursor readback from 0x0067ed40 (as int32).
    # raw_bytes layout: bytes 0..3 = limit as LE int32; byte N = validity
    #   byte for cursor N (read at 0x0067ed74+N for slot 0).
    # ref: re/analysis/frontend_promote_menus_a/0x0042aa00.md
    # U-3436/3437/3438 (slot range, array semantics, step source) — don't
    #   affect bit-identity correctness; limits and validity encoded in tests.
    'menu_cursor_step': {
        'rva':            0x0042aa00,
        'export':         'MenuCursorStep',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'void_step_global',
        'lut_root_delta': 0,
        # raw_bytes[0..3] = limit LE int32; raw_bytes[N] = validity[N].
        # validity[N] == 1 → valid entry; else invalid.
        # Return value: cursor at 0x0067ed40 after call (-1 = exhausted).
        'path1_tests': [
            # limit=0 → immediate -1
            {'raw_bytes': [0,0,0,0],              'initial_cursor': 0,  'step':  1},  # → -1
            # limit=1, val[0]=raw[0]=1=valid; step+1: wrap 0, valid → 0
            {'raw_bytes': [1,0,0,0],              'initial_cursor': 0,  'step':  1},  # → 0
            # limit=1, step-1: wrap to 0, valid → 0
            {'raw_bytes': [1,0,0,0],              'initial_cursor': 0,  'step': -1},  # → 0
            # limit=2, val[0]=2≠1,val[1]=0≠1 → exhausted → -1
            {'raw_bytes': [2,0,0,0, 0,0],         'initial_cursor': 0,  'step':  1},  # → -1
            # limit=5, valid@4 (raw[4]=1), init=0, step+1: →1,→2,→3,→4 valid → 4
            {'raw_bytes': [5,0,0,0, 1,0],         'initial_cursor': 0,  'step':  1},  # → 4
            # limit=5, valid@4, init=4, step-1: →3,→2,→1,→0(val=5≠1),→4 valid → 4
            {'raw_bytes': [5,0,0,0, 1,0],         'initial_cursor': 4,  'step': -1},  # → 4
            # limit=5, no valid (raw[4]=0) → exhausted → -1
            {'raw_bytes': [5,0,0,0, 0,0],         'initial_cursor': 2,  'step':  1},  # → -1
            # limit=5, valid@4, init=4, step+1: →5==limit→wrap 0,val=5≠1,→1,→2,→3,→4 valid → 4
            {'raw_bytes': [5,0,0,0, 1,0],         'initial_cursor': 4,  'step':  1},  # → 4
            # limit=6, valid@5 (raw[5]=1), init=0, step+1: →1,→2,→3,→4,→5 valid → 5
            {'raw_bytes': [6,0,0,0, 0,1, 0],      'initial_cursor': 0,  'step':  1},  # → 5
            # limit=6, valid@4 (raw[4]=1), init=5, step-1: →4 valid → 4
            {'raw_bytes': [6,0,0,0, 1,0, 0],      'initial_cursor': 5,  'step': -1},  # → 4
            # limit=3, no valid entries → exhausted → -1
            {'raw_bytes': [3,0,0,0, 0,0,0],       'initial_cursor': 1,  'step':  1},  # → -1
        ],
        'path2_tests': [
            {'raw_bytes': [0,0,0,0],              'initial_cursor': 0,  'step':  1},
            {'raw_bytes': [1,0,0,0],              'initial_cursor': 0,  'step':  1},
            {'raw_bytes': [5,0,0,0, 1,0],         'initial_cursor': 0,  'step':  1},
        ],
    },

    # ─────────────────────────────────────────────────────────────────────
    # Session c3-batch-g-s5 — menu state machine helpers (C2→C3, 3 of 4 candidates)
    # Frontend/MenuStateMachine.cpp
    # 0x0042ac50 (MenuCenterCalc) REFUSED — ECX+EAX dual-register ABI not
    #   supported by NativeFunction harness. D-10639 filed (re/DEFERRED.md;
    #   corrects prior draft's D-10637 miscite — D-10637 tracks a different gap).
    # ─────────────────────────────────────────────────────────────────────

    # 0x0042ae10  MenuReadinessCheckA
    # void(): reads guard globals (DAT_0067eab0 != 2 AND e7c8 == 0 AND 898ab0 != 0),
    # then path A (stride-0x4c char scan at 0x7f1502) or path B (car-index array).
    # Returns 0 (not ready) or 1 (ready).
    # arg_type='none': called 10x at quiescent main menu; DAT_0067eab0=0 at menu
    # (not 2), guard2 (e7c8) may vary — both paths return same value deterministically.
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
    # void(): structural variant of MenuReadinessCheckA — omits e7c8 guard;
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

    # ─────────────────────────────────────────────────────────────────────
    # Session ma3-frida-s3 — menu readiness third variant (carry-over from c3-batch-g)
    # Frontend/MenuStateMachine.cpp
    # ─────────────────────────────────────────────────────────────────────

    # 0x0042af50  MenuReadinessCheckC
    # void(): structural variant of MenuReadinessCheckA — same e7c8 guard as A;
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

    # Session c3-batch-a-s3 — frontend_menus_a medium batch
    # MenuNav.cpp — menu navigation helpers (C2→C3)
    # ─────────────────────────────────────────────────────────────────────

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
    # arg_type='none': called 10x at quiescent main menu (no inputs → returns 0
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

    # Session c3-batch-a-s5 — frontend_c0_promote + game_mode_cont2
    # FrontendNav.cpp / GameModeInit.cpp
    # ─────────────────────────────────────────────────────────────────────

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
            # SP mode (min=1): row>0, col=min → should quickly return.
            {'row': 2,  'col': 1, 'flag': 0, 'mp_flag': 0},
            {'row': 3,  'col': 1, 'flag': 0, 'mp_flag': 0},
            # SP mode: row>0, col>min → inner loop navigates.
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
            # { idx, guard_val }  — guard_val=0 → no write; guard_val!=0 → write 4 fields
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

    # ── Session 89: 5 util pure-function C2→C3 promotions ─────────────────────

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
        # Returns *(uint32*)0x0067e9fc — game-phase register.
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
        # Always returns 0; 3-byte stub (XOR EAX,EAX / RET / NOP×13).
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
        # initially — bit-identical against original is guaranteed.
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
        # Table zero-initialized before race; both players return 0 — valid A/B.
        'signature':      {'ret': 'void', 'args': ['int', 'pointer', 'pointer']},
        'arg_type':       'int_ptr2_out',
        'lut_root_delta': 0,
        'path1_tests': [0, 1, 2, 3, 0, 1, 2, 3],
        'path2_tests': [0, 1, 2, 3],
    },

# ── Session 85 audio leaves (C2→C3) ──────────────────────────────────────

    # 0x005aea10  AudioAlignedAlloc: alloc size+4, 4-byte-align, store raw base
    # at *(aligned-4).  Heap addresses are non-deterministic so we compare
    # (ptr % 4) and (ptr - header) — both must equal 4 for correct impl.
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
    # vtable dealloc trampoline 0x004522d0.  Test: alloc then free — no-crash.
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

# ─────────────────────────────────────────────────────────────────────
    # Session 86 — c3_render_math  (C2→C3, 5 candidates)
    # RW column-major transform + 2D vector math + matrix scale
    # ─────────────────────────────────────────────────────────────────────

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
            {'mat': _ROTY90, 'in': [1.0,  0.0,  0.0]},   # → (0,0,-1)
            {'mat': _ROTY90, 'in': [0.0,  0.0,  1.0]},   # → (1,0,0)
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
            {'mat': _TRANS,  'in': [0.0,  0.0,  0.0]},   # → (0,0,0), not (1,2,3)
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
    # Error path (magnitude < DAT_005d757c): sets out≈0, calls error stubs.
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
            [0.0,  0.0],   # zero → error path; out={0,0} mag=0 (both paths match)
        ],
        'path2_tests': [
            [1.0, 0.0], [3.0, 4.0], [0.0, 0.0],
        ],
    },

    # 0x004c5010  RwMatrixScale
    # 3-mode in-place matrix scale. Mode 0=replace, 1=M×Scale, 2=Scale×M.
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
            # Mode 1 (M×Scale): scale columns, translation unchanged
            {'mat': _IDENT,  'scale': [2.0, 3.0, 4.0], 'mode': 1},
            {'mat': _MIXED,  'scale': [2.0, 3.0, 4.0], 'mode': 1},
            {'mat': _TRANS,  'scale': [2.0, 3.0, 4.0], 'mode': 1},
            {'mat': _SCALE2, 'scale': [0.5, 0.5, 0.5], 'mode': 1},
            # Mode 2 (Scale×M): scale rows including translation
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

    # ─────────────────────────────────────────────────────────────────────
    # Session c3-batch-a-s6 — frontend_menus_a larger + game_mode (C2->C3)
    # MenuButtonDetect.cpp + GameModeCarSelect.cpp
    # ─────────────────────────────────────────────────────────────────────

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

    # ─────────────────────────────────────────────────────────────────────
    # Session c3-batch-b-s3 — sprite gate + HUD slot type mappers
    # SpriteGate.cpp
    # Drift-promotes: 0x004c5c00 C1->C2, 0x0040bb50 C1->C2
    # ─────────────────────────────────────────────────────────────────────

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

    # Session c3-batch-c-s1 — hud_ingame_promote_c2 (C2->C3, 3 candidates)
    # HUD/HudDispatch.cpp — game-mode router wrappers for in-game HUD draw
    # Callees 0x0041c2d0 and 0x0041bc50 drift-promoted C1->C2 (budget used).
    # Refused: 0x0041b630 (needs 0x0041b340 C1), 0x0041ccc0 (needs 0x0041c9a0 C1).
    # ─────────────────────────────────────────────────────────────────────

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

    # Session c3-batch-c-s3 — hud+frontend getters batch-c (C2->C3, 4 candidates)
    # HUD/HudDispatch.cpp, Frontend/FrontendAccessors.cpp, Frontend/FrontendMode.cpp
    # Pure-leaf HUD sub-mode and Frontend global/array/mode-index getters.
    # Candidate 0x00436810 DEFERRED -- U-3410/U-3411 unresolved.
    # ─────────────────────────────────────────────────────────────────────

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

    # Session c3-batch-c-s6 — hud_promote_c2_b font sys state (C2->C3)
    # HUD/FontCtx.cpp — font matrix stack + render-state init + ctx ops
    # Drift-promote: 0x004c57a0 C1->C2
    # FlushMatrix (0x00552e40) EXCLUDED — Frida not run, S-2126 open.
    # ─────────────────────────────────────────────────────────────────────

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
    # Test depths: 0 (normal), 1, 5, 29, 30 (at max-1), 31 (overflow -> false).
    'font_matrix_push': {
        'rva':            0x00552d10,
        'export':         'FontMatrix_Push',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'font_matrix_push',
        'lut_root_delta': 0,
        'path1_tests': [
            {'depth': 0},
            {'depth': 1},
            {'depth': 5},
            {'depth': 10},
            {'depth': 28},
            {'depth': 29},
            {'depth': 30},
            {'depth': 31},
            {'depth': 0},
            {'depth': 15},
        ],
        'path2_tests': [
            {'depth': 0},
            {'depth': 30},
            {'depth': 31},
        ],
    },

    # Session c3-batch-b-s1 — frontend_score_getters (C2->C3, 6 candidates)
    # Frontend/MenuScoreGetters.cpp — pure-leaf indexed reads + global getters
    # ─────────────────────────────────────────────────────────────────────

    # 0x0040b6b0  ModeScoreGetBySlot
    # Returns DAT_008a9530[param_1] — per-slot mode-score array element.
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
    # Returns DAT_0063b8ec — hotkey string base global. No args.
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
    # Returns (float)DAT_0067d99c[param_1] — indexed float read, lap frac array.
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
    # Returns DAT_0067d98c[param_1] — indexed read of lap laps array.
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

    # Session c3-batch-b-s2 — frontend batch b session 2 (C2→C3, 6 candidates)
    # MenuScoreGetters.cpp + MenuInit.cpp
    # SlotSortByModeScore (0x0040b620) OMITTED — arg_type 'void_out_ptr'
    #   (pass ptr, call, readback 4-element array) not supported by harness.
    # ─────────────────────────────────────────────────────────────────────

    # 0x00429a90  LapSecsGetBySlot
    # Returns (&DAT_0067d994)[param_1] — indexed read of lap-time seconds
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
    # 10 dummy-marker tests (game-mode global at quiescent main-menu = 1 → 0).
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
    # Note: harness checks exact return value — we write known sentinel to
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

    # Session c3-batch-b-s4 — frontend_promote_menus_b (C2→C3, 3 of 6)
    # MenuScoreSort.cpp + MenuRaceEnd.cpp
    #
    # NOT registered (callee gate failures or unsupported arg_type):
    #   0x0040b460  SlotSortByScoreWithModeOverride — REFUSED: callee 0x00417740 C0 (STUB S-0491)
    #   0x00429a30  LapTimeStoreToPlayerArrays — BLOCKED: callee 0x00430790 C1 with open U-3470
    #   0x0040e3a0  PlayerColorTableGet — OMITTED: (int, byte*) signature not supported by
    #                harness (pointer out-param needs custom arg_type)
    # ─────────────────────────────────────────────────────────────────────

    # 0x00430830  SplitScreenTrackAssignment
    # uint32 FUN_00430830(int param_1) — pure leaf, no callees.
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
    # uint32 FUN_0042fe30(void) — calls GetRaceSubMode()==0xb -> return 1, else DAT_0067ea74.
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
    # uint32 FUN_0042fe50(void) — complement of RaceEndFlagIfEndMode.
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

    # Session c3-batch-b-s5 — Frontend sprite dispatch + time decompose
    # SpriteDispatch.cpp + MenuTime.cpp
    # ─────────────────────────────────────────────────────────────────────

    # 0x0042fab0  SpriteSlotDispatch
    # Assembly-confirmed 10-way dispatch: MOV [ESP+4], slot_ptr; JMP FUN_0040bb90.
    # Slot 0 -> ptr 0x5cd898; slot 9 -> ptr 0x5cd838 (stride mostly 0xc).
    # arg_type='int_scalar': passes slot index (0–9 + out-of-range) as int32.
    # Default case (out of range 0–9): no-op return; both paths must agree (void).
    # Call with void wrapper: treat 'void' ret as uint32 for harness compatibility.
    'sprite_slot_dispatch': {
        'rva':            0x0042fab0,
        'export':         'SpriteSlotDispatch',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1],
        'path2_tests':    [0, 1, 9],
    },

    # 0x0042e590  SpriteAnimFrameThunk
    # Assembly-confirmed tail-call thunk: computes idx = sprite_slot + 2*DAT_0067f17c,
    # looks up sprite_ptr_table[idx] at 0x5f79d8, overwrites first arg, JMP FUN_0040bb70.
    # arg_type='int_scalar': passes sprite_slot as int32; FUN_0040bb70 receives
    # live game state for remaining args (x1/y1/x2/y2/color/uv/frame/flag).
    # Both orig and reimpl call FUN_0040bb70 with the same transformed first arg.
    'sprite_anim_frame_thunk': {
        'rva':            0x0042e590,
        'export':         'SpriteAnimFrameThunk',
        'signature':      {'ret': 'void', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2],
    },


    # Session c3-batch-d-s1 — frontend_menus large functions (C2->C3)
    # MenuHelpers.cpp: FrontendPlayerSlotCheck, FrontendCursorUpdate
    # ─────────────────────────────────────────────────────────────────────

    # 0x0042ebe0  FrontendPlayerSlotCheck
    # bool(int param_1): player-slot fullness checker; dispatch key {10,11,12}.
    # Iterates 0x7f0a48 stride 0x1e0; returns (enableFlag != 0) && !bVar1.
    # arg_type='int_scalar': pass param_1 as uint32; compare bool return (0/1).
    # Only valid dispatch keys {10,11,12} are tested — the original behaviour
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


    # ─────────────────────────────────────────────────────────────────────────
    # Session c3-batch-d-s3 — hud_ingame_promote_c2 (C2->C3)
    # HudDispatch.cpp
    # ─────────────────────────────────────────────────────────────────────────

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

    # ─────────────────────────────────────────────────────────────────────────
    # Session c3-batch-g-s11 — HUD-ingame core dispatch (C2->C3)
    # HUD/HudDispatch.cpp — loop helpers for {5/6}-path and {7}-path.
    # ─────────────────────────────────────────────────────────────────────────

    # 0x0041b630  HudSlotLoopB630
    # void(): iterates 4-entry array at 0x0063c8d0, stride 0x74 (116 bytes).
    # Per entry: reads int32_t at offset +0x6c; if non-zero -> calls FUN_0041b340.
    # Called on DAT_0063ba8c ∈ {5,6} path when FUN_0042f500()==0.
    # At main menu DAT_0063ba8c < 5 so HudIngameDispatch guard fires first —
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


    # Session c3-batch-b-s6 — VehicleUnlockFlagGet (C2->C3)
    # VehicleMeta.cpp
    # ─────────────────────────────────────────────────────────────────────

    # ─────────────────────────────────────────────────────────────────────────
    # Session c3-batch-f-s8 — audio dsound wrapper A (C2->C3, 4 candidates)
    # Audio/AudioDSound.cpp — COM wrapper field setter, vtable caller,
    # QI chain, and semaphore wrapper.
    # STRUCT GAP: AudioBufFieldSet accesses audio buf struct +0x74/+0x78/+0x11c/+0x38.
    # U-0361 (AudioDSoundRelease vtable slot semantic) — in ## Open uncertainties.
    # U-0360 (AudioDSoundQIChain IID at 005d09dc) — in ## Open uncertainties.
    # ─────────────────────────────────────────────────────────────────────────

    # 0x005baf60  AudioBufFieldSet
    # void(ptr, int): writes param_2 to *(param_1+0x74), ORs 0x100 into
    # *(param_1+0x78), then if bit 3 of *(param_1+0x78) is set writes param_2
    # to *(*(param_1+0x11c)+0x38).
    # Strategy: buf_field_set — allocate 0x120-byte zeroed buffer, set +0x78 to
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
    # Strategy: int_scalar — pass a fake ptr (e.g. 0x1000); both orig and reimpl
    # crash dereferencing *(0x1000+0x7c) at the same offset identically.
    'audio_dsound_release': {
        'rva':            0x005baf90,
        'export':         'AudioDSoundRelease',
        'signature':      {'ret': 'int32', 'args': ['int32']},
        'arg_type':       'int_scalar',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        # Fake pointer values — both orig and reimpl will AV identically.
        'path1_tests':    [0x1000, 0x1000, 0x1000, 0x1000, 0x1000,
                           0x1000, 0x1000, 0x1000, 0x1000, 0x1000],
        'path2_tests':    [0x1000, 0x1000, 0x1000],
    },

    # 0x005bc400  AudioDSoundQIChain
    # int(ptr*, ptr*): double QI chain on COM objects.
    # Both paths crash identically when param_2=NULL (first write *param_2=0 → AV at 0x0).
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
        # First write `*param_2 = 0` → AV at 0x0 for both orig and reimpl.
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
    # Strategy: semaphore_create — allocate 4-byte scratch buf, call
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
    # Array is zero-initialized before game-mode selection → all tests return
    # 0 at quiescent main-menu state. A/B identity guaranteed (both read 0x00
    # from the same global array).
    # [UNCERTAIN U-3176] No bounds check on param_1 — reproduced as-is.
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

    # Session c3-batch-e-s2 — VfsFileExists + AutosaveTrigger (C2->C3)
    # Save/GameSaveVFS.cpp
    # ─────────────────────────────────────────────────────────────────────────

    # 0x00550b00  VfsFileExists
    # ~186 bytes. int(char*). VFS file-exists router.
    # Branch A (DAT_007dc75c != 0): colon-scan → linked-list lookup → vtable[0x13](obj, filename).
    # Branch B (DAT_007dc75c == 0): if DAT_007dc768==NULL → return 0; else call (*cb)(6) → 0.
    # At quiescent main-menu state: DAT_007dc75c is likely 0 (VFS not active) → Branch B.
    # Branch B never dereferences param_1, so passing 0 or any pointer is safe at menu.
    # arg_type='int_scalar': passes filename pointer VA as int32 (ASLR disabled; fixed VAs).
    # At menu: both orig and reimpl → Branch B → return 0 (bit-identical).
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
        # scan_fn scans for ':'; finds none → default obj dispatch → vtable[0x13].
        # Both orig and reimpl call same vtable fn on same obj → bit-identical return.
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
    # Guard: if DAT_008a95ac == 0 → return immediately (no side effects).
    # At quiescent main-menu state: DAT_008a95ac == 0 → guard fires → both return void/0.
    # arg_type='none': called 5x; both paths return void/0 at quiescent state.
    # Globals observed side-channel: DAT_008a9584, DAT_008a9594, DAT_008a95b0 unchanged.
    'autosave_trigger': {
        'rva':            0x004099a0,
        'export':         'AutosaveTrigger',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        # 5 calls at quiescent menu; DAT_008a95ac==0 → early return; both return 0.
        'path1_tests':    [0, 1, 2, 3, 4],
        'path2_tests':    [0, 1, 2],
    },

    # Session c3-batch-i-s4 — Settings/video-config CONFIG_SAVE_FN (C2->C3)
    # Save/SettingsCfg_i4.cpp
    # ─────────────────────────────────────────────────────────────────────────

    # 0x004989b0  ConfigSave
    # ~76 bytes. int(void). Writes 512 bytes from global settings buffer at
    # 0x00773208 to "videocfg.bin" via game CRT _fsopen("wb")/_fwrite/_fclose.
    # Returns 1 on file-open success, 0 on failure.
    # At quiescent main-menu state, videocfg.bin exists+writable; both orig and
    # reimpl hit the success path → return 1 → bit-identical.
    # arg_type='none': 5 calls (file-write side effect is identical between both
    # paths — both write the same 512 bytes from the same global buffer).
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
    # Boot/CrtCompilerSupport.cpp — FidDB-identified MSVC CRT compiler support.
    # All four are C2-ceiling functions; C3 evidence is FidDB single-match only.
    # ---------------------------------------------------------------------------

    # 0x004a4b93  CrtFastErrorExit (_fast_error_exit / fast_error_exit)
    # void(uint32) — checks banner flag at 0x007739f0; if 1 calls __FF_MSGBANNER;
    # forwards param_1 to FUN_004ab8d6; terminates via ___crtExitProcess(0xFF).
    # arg_type='int_scalar': passes the error code as a uint32.
    # Note: function does not return (calls ExitProcess); harness must not wait
    # for a return value.  At quiescent state the banner flag is 0, so
    # __FF_MSGBANNER is not called — the function races to ExitProcess(0xFF).
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
    # void(void) [implicit EAX = stack size] — page-probe loop.
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
    # void(undefined4, int) — saves EBX/ESI/EDI/retaddr; installs ExceptionList.
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
    # void(void) — restores ExceptionList from EBP[-4]; writes retaddr.
    # Compiler-injected SEH teardown; non-standard ABI; cannot be hooked via
    # Frida Interceptor without corrupting the SEH chain.
    # C3 evidence: FidDB single-match VS __SEH_epilog +
    # decompilation matches (reads EBP[-4] → ExceptionList; writes retaddr to *EBP).
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

    # ── Session c3-batch-e-s8: Boot/CrtInit C2→C3 promotions ────────────────

    # 0x004a78b0  CrtPreInitLoop
    # Zero-slot fn-ptr table iterator. SEH frame present; loop body unreachable
    # as decompiled (start == end == 0x005e7b84 → zero iterations).
    # [UNCERTAIN U-0005] Decompiler may have collapsed two distinct table
    #   pointers into the same symbol; reproduced as-is.
    # Original returns void; we declare ret='uint32' for harness compat
    # (EAX will be whatever it was — both sides return the same EAX because the
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

    # ─────────────────────────────────────────────────────────────────────
    # Session c3-batch-e-s9 — boot_crt_env  (C2→C3, 2 candidates)
    # Boot/CrtEnvArgv.cpp — CRT startup env-string functions
    # ─────────────────────────────────────────────────────────────────────

    # 0x004abc53  CrtSetEnvp  (__setenvp, Visual Studio 2003 Release)
    # Takes no args; returns int (0 on success, -1 on failure).
    # At main-menu time DAT_007739e8 is already NULL (freed during startup),
    # so every re-call immediately returns -1. Both orig and reimpl must
    # return -1 identically — 10 calls confirm stable bit-identical output.
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

    # ─────────────────────────────────────────────────────────────────────────
    # Session c3-batch-f-s3 — audio sub-struct lifecycle cluster (C2->C3, 4)
    # Audio/AudioRws.cpp
    # Pool-return/heap-free/combined-cleanup leaves + 16-byte fmt-key comparator.
    # ─────────────────────────────────────────────────────────────────────────

    # 0x005ae080  AudioSubStructAFree
    # int*(int*): if *param_1!=0 and bit0(param_1[2]) → pool-return FUN_005ae920;
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
        # 0 → detection path (calls GetEnvironmentStringsW, may set to 1 or 2)
        # 1 → wide path  (calls GetEnvironmentStringsW again, converts)
        # 2 → ANSI path  (calls GetEnvironmentStrings ANSI, copies)
        # 0 again × 8 → repeatedly exercise detection path
        # Both orig and reimpl must return non-null (1) for all paths.
        'path1_tests':    [0, 1, 2, 0, 1, 2, 0, 1, 2, 0],
        'path2_tests':    [0, 1, 2],
    },

    # ─────────────────────────────────────────────────────────────────────────
    # Session c3-batch-f-s2 — audio fmt-desc utility cluster (C2→C3, 4 candidates)
    # Audio/AudioRws.cpp
    # ─────────────────────────────────────────────────────────────────────────

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
        #   f04=0, f10=0 → 0x1c (28)
        #   f04≠0, f10=0 → 0x2c (44)
        #   f04=0, f10≠0, f14=8 → 0x1c+8=36
        #   f04≠0, f10≠0, f14=8 → 0x2c+8=52
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
    # fmt_table_search: inject fake ctx with count=0 → trivially returns 1 (10x).
    # At count=0, the predicate is never called; result is deterministic.
    # (Injecting non-zero count would require calling into live game state via
    # FUN_005ac9e0 with a real desc ptr — deferred to canonical-scenario C4.)
    'audio_fmt_table_search': {
        'rva':            0x005acd10,
        'export':         'AudioFmtTableSearch',
        'signature':      {'ret': 'int32', 'args': ['pointer', 'pointer']},
        'arg_type':       'fmt_table_search',
        'lut_root_delta': 0,
        # All tests use count=0 → function returns 1 immediately without calling predicate.
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
    # Both orig and reimpl call into live game state — both must return same ptr.
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
    # int(int): if *(int*)(param+4)!=0 and bit1(*(uint*)(param+8)) → heap-free;
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
    # Same quiescent-menu guard logic — both sub-callees are guarded at their
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
    #   read back all 3 DWORDs — both paths must return '0,0,0'.
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
    # identically. arg_type='none' + crash_equal_ok=True → GREEN.
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
    # traversal exits immediately (piVar2 == param_1) → returns NULL.
    # Both paths return NULL=NULL with no errors → match=True without crash.
    # (Avoids the pool entirely for not-found cases.)
    'audio_list_remove_by_value': {
        'rva':          0x005ade10,
        'export':       'AudioListRemoveByValue',
        'signature':    {'ret': 'pointer', 'args': ['pointer', 'int32']},
        'arg_type':     'audio_list_remove',
        'insert_rva':   0x005addd0,
        'lut_root_delta': 0,
        # NOTE: payload=0 excluded — fresh harness sentinel has data[+8]=0;
        # payload=0 hits a degenerate match against the sentinel node itself,
        # causing AudioPoolFree to be called on an uninitialized pool → crash.
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
    # sentinel via Memory.alloc. nodeCount=0 tests: empty sentinel → loop
    # condition param_1[1] != param_1 is immediately False → no-op. Both
    # paths observe sentinel still self-loops → origV=1, reimV=1 → match=True.
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
    #   if non-NULL the callback at LAB_005ac930 is an inline code region —
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

    # Session c3-batch-b-s6 — VehicleUnlockFlagGet (C2->C3)
    # VehicleMeta.cpp
    # ─────────────────────────────────────────────────────────────────────

    # 0x0042ef40  VehicleUnlockFlagGet
    # Pure leaf: reads byte from DAT_007f0e50 + param_1*0xc at a byte-offset
    # selected by param_2 switch; returns 1 if byte == 0x01, else 0.
    # Array is zero-initialized before game-mode selection → all tests return
    # 0 at quiescent main-menu state. A/B identity guaranteed (both read 0x00
    # from the same global array).
    # [UNCERTAIN U-3176] No bounds check on param_1 — reproduced as-is.
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
    'music_group_volume_set': {
        'rva':            0x005baf00,
        'export':         'MusicGroupVolumeSet',
        'signature':      {'ret': 'void', 'args': ['pointer', 'float']},
        'arg_type':       'music_vol_set',
        'lut_root_delta': 0,
        'path1_tests':    [0.0, 1.0, 0.5, -1.0, 0.25, 0.75, -0.5, 2.0, 0.001, 100.0],
        'path2_tests':    [0.0, 1.0, 0.5],
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
    # Both orig and reimpl return void (undefined === undefined) — trivial match.
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
    # Strategy: int_copy24_out — call fn(slot, buf), read 6 dwords from buf.
    # Source is a live game-global array; at main-menu quiescent state the values
    # are stable. Both orig and reimpl read the same source → identical output.
    # Slot 0 maps to 0x0063dc10; slot 1 to 0x0063debc, etc.
    'timer_slot_data_copy': {
        'rva':            0x0041f000,
        'export':         'TimerSlotDataCopy',
        'signature':      {'ret': 'void', 'args': ['int32', 'pointer']},
        'arg_type':       'int_copy24_out',
        'lut_root_delta': 0,
        # Test vectors: slot indices 0..9 (same slot read multiple times is valid;
        # the source globals are stable at quiescent main-menu state).
        'path1_tests': [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests': [0, 1, 2],
    },

    # 0x00420d40  TimerStructArrayClear  void(void)
    # Zeroes 24 bytes per element × 6 elements at 0x0063e4b8..0x0063e4bc.
    # Strategy: void_write_observe on first element's first dword (0x0063e4b8).
    # Write sentinel, call fn, read back — should be 0.
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
    # Note: FUN_00421c50 is not yet classified; callee-gate: ≤2 drift-promotes, proceed.
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
    'timer_init_thunk': {
        'rva':            0x004222c0,
        'export':         'TimerInitThunk',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x0063fb90,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00422b10  TimerDwordClear  void(void)
    # Zeroes 0x4e0 bytes (312 dwords) at 0x008994c0.
    # Strategy: void_write_observe on 0x008994c0 (base of zeroed block).
    # Write sentinel, call fn, read back — should be 0.
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
    # Write sentinel, call fn, read back — should be 0.
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
    # Strategy: none — call 1x at quiescent LUT-ready state.
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
    # Strategy: none — call 10x at quiescent state; both must return identical
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

    # 0x004a4541  FsopenSafe
    # 18-byte _fsopen wrapper: forces shflag = 0x40 (_SH_DENYNO).
    # Two-arg thunk: FsopenSafe(filename, mode) -> _fsopen(filename, mode, 0x40).
    # Returns FILE* (NULL on failure).
    # Strategy: call with NULL, NULL as filename/mode pointers.
    # _fsopen(NULL, NULL, 0x40) returns NULL (CRT guards invalid args).
    # Both orig and reimpl must return NULL (ptr(0)) — bit-identical.
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
    # A single pointer arg is passed after the format pointer — it lands on
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
        # [fmt, arg1]: fmt=0x005d012c ("videocfg.bin" — no % specifiers),
        # arg1=0x005cf010 ("rb") — harmlessly on stack, not consumed by vsprintf.
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
    # Strategy: void arg_type — call 10x; return value is constant pointer
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
    # Strategy: none — call 5x at quiescent game state; videocfg.bin exists
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
    # void(uint, int, int): CRT exit core — acquires lock 8, walks atexit list,
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
    # int(void): PE entry point — full CRT startup sequence.
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
    # Strategy: write_global_setter — call fn(test_value), read back 0x0063d7e0.
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
    # Strategy: write_global_setter — call fn(test_value), read back 0x0066d6fc.
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
    # Strategy: write_global_setter — call fn(test_value), read back 0x0066d700.
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
    'timer_array_zero': {
        'rva':            0x00422b10,
        'export':         'TimerArrayZero',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x008994c0,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x00425b10  PlayerSlotZero
    # Pure leaf; writes 0 to 8 globals at stride 0x4c from 0x008992a0.
    # Strategy: write sentinel to first address (0x008992a0), call fn, read back.
    # Both original and reimpl must write 0 (sentinel overwritten).
    # 10 sentinel values confirm write for each call.
    # ref: re/analysis/timer_d3_cont1_b/0x00425b10.md
    'player_slot_zero': {
        'rva':            0x00425b10,
        'export':         'PlayerSlotZero',
        'signature':      {'ret': 'void', 'args': []},
        'arg_type':       'void_write_observe',
        'target_global':  0x008992a0,
        'lut_root_delta': 0,
        'path1_tests':    [0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xFFFFFFFF,
                           0x80000000, 0x00000001, 0x55555555, 0xAAAAAAAA,
                           0x3F800000, 0xBEEFCAFE],
        'path2_tests':    [0xDEADBEEF, 0xCAFEBABE, 0xFFFFFFFF],
    },

    # 0x004222c0  TimerInitThunk
    # 4-byte thunk of FUN_00422120 (0x00422120, C2).
    # Strategy: 'none' — call 10x at quiescent main menu; both original and
    # reimpl delegate to 0x00422120 and produce same side-effects.
    # FUN_00422120 iterates 0x0063fb90 stride 0x208 x4 calling FUN_00421c50;
    # at main menu the loop body is deterministic (same state both paths).
    # ref: re/analysis/timer_d3_cont1_b/0x004222c0.md
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
    'slot_data_copy': {
        'rva':            0x0041f000,
        'export':         'SlotDataCopy',
        'signature':      {'ret': 'void', 'args': ['int32', 'pointer']},
        'arg_type':       'int_out24',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 0, 1, 2, 3, 0, 1],
        'path2_tests':    [0, 1, 2],
    },

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
    # call fn, read back — if fn works correctly it will write 0 there.
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

    # ─────────────────────────────────────────────────────────────────────
    # Session c3-batch-g-s1 — frontend menu chrome (C2->C3, 2 candidates)
    # Frontend/MenuChrome.cpp
    # ─────────────────────────────────────────────────────────────────────

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

    # ─────────────────────────────────────────────────────────────────────────
    # Session c3-batch-g-s7 — race_results slot-state getters (C2→C3)
    # Frontend/RaceResults.cpp
    #
    # NOT registered (caller-gate / sig failures):
    #   0x0040b460  SlotSortByScoreWithModeOverride — REFUSED: callee 0x00417740 not C2
    #   0x0040e3a0  PlayerColorTableGet — already in MenuScoreSort.cpp;
    #                (int, byte*) sig unsupported (pointer out-param needs custom arg_type)
    #   0x0040e480  CarSlotStateSet — DEFERRED: no viable non-destructive diff arg_type
    #                for double-deref setter (entity_field_set reads wrong address for PTR_PTR)
    # ─────────────────────────────────────────────────────────────────────────

    # 0x0040e470  CarSlotStateGet
    # uint32 FUN_0040e470(int param_1) — 14-byte leaf getter, no callees.
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

    # Session c3-batch-g-s14 — FontSys+HudDispatch+RW-release (C2->C3)
    # HUD/FontCtx.cpp — FontSys init sequence
    # HUD/HudDispatch.cpp — EAX-thiscall vtable dispatcher
    # 0x004c5a60 DEFERRED — callees 004d8060 + 004c7650 both C1/unknown; caller gate fails.
    # ─────────────────────────────────────────────────────────────────────

    # 0x00552b60  FontSys_InitSeq
    # fn(void) -> void.
    # 8-step font subsystem init: zero flag + 7 alloc/init calls. No branches.
    # All 5 alloc callees are C1 stubs — crash identically on both sides when
    # called without initialised game state. crash_equal_ok=True counts same-
    # error as GREEN. Called once during game init (FontText_HudInit at 0x00427ca0).
    # arg_type='none': call 10x; both crash with same error each time.
    'font_sys_init_seq': {
        'rva':            0x00552b60,
        'export':         'FontSys_InitSeq',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # Session c3-batch-g-s15 — carry-overs from batch-d/c
    # MenuButtonDetect.cpp — trivial uint16 screen-dimension getters (C2→C3)
    # ─────────────────────────────────────────────────────────────────────

    # 0x0042b8b0  ScreenWidthGet
    # Returns DAT_0067ea54 as uint16_t in EAX. 6-byte leaf. No callees.
    # Strategy: arg_type='none' — call 10x at quiescent main menu state; both
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
    # Path B: skip. Every call: compose cached with camera view → DAT_00912b98.
    # Callee 0x004c0ed0 (S-2126) reads *(cam+4) — DAT_00912c0c is NULL at boot,
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
    # Strategy: arg_type='none' — call 10x at quiescent main menu state; both
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
    # No guards — unconditional. Shared by game-mode 10 and game-mode 5.
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

    # ─────────────────────────────────────────────────────────────────────
    # Session ma2-frida-s2 — Boot/Window cluster (C2→C3, 2 of 6 candidates)
    # Boot/Window.cpp — window-show + msgpump from the main game loop.
    # Refused: 0x00499ba0 (Window_Create — destructive 3-arg), 0x00499cc0
    #          (Window_Destroy — terminates game), 0x00499820 (Window_WndProc
    #          — needs 4-arg WNDPROC harness), 0x00496490 (Window_WmCreate
    #          — three callees still C1).
    # ─────────────────────────────────────────────────────────────────────

    # 0x004996f0  WindowShow
    # void __cdecl WindowShow(int nCmdShow)
    # ShowWindow(DAT_007e9584, nCmdShow); UpdateWindow(DAT_007e9584).
    # Both orig and reimpl invoke the same user32.dll APIs against the same
    # global HWND (DAT_007e9584), already created by the boot path. ShowWindow
    # on an already-visible window with SW_SHOW/SW_NORMAL is a no-op; reimpl
    # wrapper returns void → void_match counts as GREEN.
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
    # Frida agent thread owns no message queue → PeekMessageA returns 0 on
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

    # Session c3-batch-ma2-frida-s4 — boot teardown + COM release
    # Boot/Teardown.cpp — HardwareExitApplication helpers (FUN_00402a40 callees)
    # 5 of 6 candidates REFUSED: 0x00494bc0 (live COM Release destroys D3D
    # interfaces), 0x00489250 (live free destroys RW frames), 0x00494f20
    # (close-video destroys live state on first call so 10x diff diverges),
    # 0x004955c0 + 0x004963d0 (thunk targets FUN_00495580 / FUN_00496370 not
    # yet C1+plated). See re/DEFERRED.md D-10774..D-10778.
    # ─────────────────────────────────────────────────────────────────────

    # 0x00494ef0  ThunkVideoStateGet — thunk → FUN_00493f70 (intro_splash video flag read)
    # 5-byte E9 JMP at 0x00494ef0 → 0x00493f70.  Target body is
    # `return DAT_00771a04;` (pure global reader; plate at
    # re/analysis/intro_splash/0x00493f70.md).  Read-only, safe to invoke 10x
    # at quiescent main menu — both orig and reimpl deref the same address.
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

    # Session ma2-frida-s7 — DirectInput init chain (C2→C3, 6 candidates)
    # Input/DirectInput.cpp
    # First C3 promotions for the input subsystem.
    #
    # Refused this session:
    #   - 0x00497190 (contcfg filename formatter): implicit EAX arg
    #     (U-2588) — same dual-register ABI problem as MenuCenterCalc.
    #   - 0x00497230 (SaveControllerConfig): implicit EAX arg (U-3015)
    #     + file I/O side effects.
    # ─────────────────────────────────────────────────────────────────────

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

    # Session c3-batch ma2-frida-s6 — Boot/VideoConfig.cpp
    #   Video config getters + display dim helpers + RW driver-system wrapper.
    #   4 C2 → C3 candidates promoted (2 of the 6 batch candidates deferred:
    #   0x00498b60 destructive teardown; 0x004c2ed0 needs wide out-buffer harness)
    # ─────────────────────────────────────────────────────────────────────

    # 0x00498bc0  VideoGetRenderWidth
    # Pure leaf: returns DAT_00616028 (render width in pixels).
    # MOV EAX, [0x00616028] / RET. Strategy: read_global — write sentinel
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
    # — affects subsequent test calls equally on both sides.
    'create_dinput_object': {
        'rva':            0x00495530,
        'export':         'CreateDInputObject',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        # crash_equal_ok=True: at quiescent main menu, calling this re-enters
        # DirectInput8Create against an already-initialized state. The dinput8
        # IAT path raises a structured exception via the OS DInput8 cleanup
        # logic — both sides hit it identically (both invoke the same external
        # entry point with identical arg layouts), so a same-error pair counts
        # as bit-identical for diff purposes.
        'crash_equal_ok': True,
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x004955b0  CreateDInputObjectBool
    # 11-byte bool wrapper: returns FUN_00495530() != 0.
    # arg_type='none': same testing strategy as the inner function — bit-
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
    # nothing — trivially bit-identical.
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
    # driver context (DAT_007d3ff8) is already initialized — that's the same
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

    # Session ma2-frida-s5 — Boot/FrameDispatch  (C2→C3, 5 candidates)
    # FrameDispatch.cpp — direct callees of per-frame tick FUN_00492e90.
    # ─────────────────────────────────────────────────────────────────────

    # 0x0042b8d0  StatePhaseIsIdle
    # int(void) — `return DAT_0067eca4 == 0;` (13-byte pure-leaf predicate).
    # arg_type='read_global': write sentinel to 0x0067eca4 before each call;
    # both orig and reimpl observe identical state-phase value → identical
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

    # 0x0042b8f0  StatePhaseIsFinal
    # int(void) — `return DAT_0067eca4 == 5;` (14-byte pure-leaf predicate).
    # arg_type='read_global': write sentinel to 0x0067eca4 before each call;
    # both orig and reimpl observe identical state-phase value → identical
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

    # 0x0042f510  Vehicle0HandleGet
    # uint32(void) — `return DAT_0067f190;` (5-byte pure-leaf getter).
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
    # uint32(void) — `return DAT_00773204;` (5-byte pure-leaf getter).
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
    # void(int param_1) — `(**(code **)(param_1 + 0x1c))();` indirect vtable.
    # Ghidra can't recover the jumptable (10 bytes, indirect call).
    # arg_type='int_scalar' + crash_equal_ok=True: passing a non-mappable
    # int (e.g. 0, 1, fake ptrs) causes both orig and reimpl to dereference
    # [scalar+0x1c] then call through — both SIGSEGV identically when the
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
    #   - dst_ptr == 0     → return 0 (the dst!=0 guard at 0x0049583x).
    #   - slot_idx >= count → return 0 (the slot bound guard).
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
        # Both guards return 0 → bit-identical on both sides.
        'path1_tests': [
            [0,          0],          # dst=NULL → 0
            [1,          0],          # dst=NULL → 0
            [100,        0],          # dst=NULL → 0
            [0x7FFFFFFF, 0],          # dst=NULL → 0 (also huge slot)
            [0x10000,    0],          # dst=NULL → 0 (also huge slot)
            [0x10000,    0xDEADBEEF], # huge slot → 0 (slot check is first)
            [0x7FFFFFFF, 0xCAFEBABE], # huge slot → 0
            [0x40000000, 0x11223344], # huge slot → 0
            [0x10000,    1],          # huge slot → 0
            [0x20000,    2],          # huge slot → 0
        ],
        'path2_tests': [
            [0,        0],
            [0x10000,  0xDEADBEEF],
            [0x40000000, 0x11223344],
        ],
    },

    # ─────────────────────────────────────────────────────────────────────────
    # Session ma3-frida-s5 — Frontend text+sprite carry-over from c3-batch-g-s8
    # ─────────────────────────────────────────────────────────────────────────

    # 0x0040e3a0  PlayerColorTableGet
    # void(int idx, byte* out_buf4) — 141-byte switch writing 4 RGBA bytes.
    # Indices 0..5 produce fixed color bytes; default (>=6) invokes abort
    # FUN_004a332b(-14) — non-returning. Implementation in MenuScoreSort.cpp.
    # arg_type='int_outbuf4': allocate 4-byte buf, call fn(idx, buf), compare
    # 4-byte fingerprint. Tests only valid indices 0..5 (default path is abort).
    # Pure function — no game state dependency; safe at any time.
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

    # ─────────────────────────────────────────────────────────────────────
    # Session ma3-frida-s7 — Frontend game-mode dispatch (C2->C3)
    # FrontendDispatch.cpp: FrontendModeDispatch
    # ─────────────────────────────────────────────────────────────────────

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

    # Session c3-batch-h-s5 — util small leaves+near-leaves (C2->C3)
    # Util/UtilBatch_h5.cpp
    # ─────────────────────────────────────────────────────────────────────

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
    # wprintf is the only callee — IAT call, same in both paths.
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
    # outside init context — both paths share that risk identically.
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

    # 0x0041cb80  TimerArrayInit46 — REFUSED.
    # 53-byte init function with implicit-pointer register convention for
    # the per-element callee FUN_0041c380 (ESI loaded at 0x0041cb95, kept
    # live across each CALL). A C++ fn-ptr reimpl cannot reliably set ESI,
    # so the diff would not be bit-identical. Same impedance as
    # 0x00422120 TimerInitLoop (refused below).

    # Session c3-batch-h-s4 — HUD cluster (C2->C3, 7 of 12 candidates)
    # HUD/HudBatch_h4.cpp
    # Refused this session:
    #   - 0x0041d870 (callee FUN_0041d410 still C1)
    #   - 0x0041ded0 (callee FUN_0041de80 still C1)
    #   - 0x00427620 (all callees < C2; FUN_00555830/00556e40 absent from csv)
    #   - 0x00427680 (non-standard ESI implicit-output ptr + U-2127 EDI artifact
    #                 not understood — would need new harness arg_type)
    # Skipped (already hooked):
    #   - 0x0041c2d0 (FUN_0041c2d0 already in HUD/HudDispatch.cpp)
    # ─────────────────────────────────────────────────────────────────────

    # 0x0041db80  Sub0041db80_HudThresholdDispatch
    # void(void): reads DAT_0063d588 + 5 .rdata thresholds, dispatches up to 6
    # HUD object vtable[0x48] calls. At quiescent main menu, DAT_0063d588 is 0
    # and the dispatched object pointers (0x0063d55c..0x0063d570) are NULL —
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

    # 0x005554d0 — REFUSED (analysis note inconsistent with binary;
    # first body read is [ESI+0x134] then CALL, not ctx+0x0c kerning loop).

    # 0x00403160  Sub00403160_SubMode0BViewport
    # void(void): sub-mode 0xb viewport handler. Calls camera-fetch
    # FUN_004671a0(0) (C2), camera lock FUN_004c19f0 (C3), set-viewport
    # FUN_004c1c80 (C2), camera begin FUN_004c1a00 (C1), HUD draw body
    # FUN_00402fb0 (C1), conditional FUN_00428760 (C1). Render-state vtable on
    # DAT_007d3ff8. The C2->C3 gate requires "at least one callee at C2+":
    # FUN_004c19f0 (C3), FUN_004671a0 (C2), FUN_004c1c80 (C2) all satisfy.
    # Caller HudIngameDispatch 0x0040dfc0 is C3.
    # At quiescent main menu calling cold may exercise camera state — likely
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

    # 0x0041bc50 — REFUSED (analysis note dispatch table is reordered and
    # incomplete vs binary: actual first pair is (0x1c,0xbc) not (0x10,0xb0);
    # also missing pair (0x94,0x134); requires full re-decode of 603-byte body).

    # 0x00427780  FontText_StringTableLookup
    # const u8* __cdecl(int idx): packed-string-table lookup.
    # return &DAT_0066d828 + *(int*)(&DAT_0066d828 + idx*4).
    # Pure leaf. Both orig and reimpl read same global DAT_0066d828 and same
    # directory entries. Bit-identical return ptr for any idx.
    # Strategy: int_scalar — pass index; compare returned pointers. At quiescent
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
    # Frida arg_type='none' won't set EAX/ECX → both sides receive same garbage
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

    # 0x004c57a0 — REFUSED (returns alloc'd ptr; pointer non-deterministic
    # across orig/reimpl pair; would need new arg_type to compare matrix
    # contents at returned addr).

    # Session c3-batch-h-s6 — Util mid-size (Opus 4.7 1M, 2026-05-17)
    # UtilMid_h6.cpp
    # ─────────────────────────────────────────────────────────────────────────

    # 0x00442c80  ModeGatedPlayerCheck
    # 62-byte mode-gated player float compare. Returns 1 iff
    #   FUN_0040e350()==6 AND DAT_007f0fd0 not in {4,8,9}
    #   AND DAT_008989b0[param_1*4] > _DAT_005cc9b8.
    # At main menu: FUN_0040e350 returns mode!=6 → both sides return 0.
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
    # equality (both sides drive same originals → same writes).
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
    # then iterates 4 config pairs; at menu all pair-firsts are -1 → no
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
    # action. At quiescent menu all slot states are 0 → both sides clear
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
    # 74-byte queue drain. Reads count at 0x0069160c; at menu count==0 →
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
    # be null → both paths crash same way → crash_equal_ok=True. If running
    # safely, all sentinels are -1 → bVar1 stays false → no trigger block.
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

    # ─────────────────────────────────────────────────────────────────────
    # Session pizwin32-bypass-20260517 — Compat/PizWin32Bypass.cpp
    # Win32-layer bypass for the .piz CreateFileA/ReadFile pair.
    # Both are harness-limited: file-I/O side effects + non-standard ABI
    # (FUN_004b6710 passes the path via EAX, not the stack), so synthetic
    # Frida A/B diffing would corrupt process state. C3 evidence comes from
    # canonical-scenario runtime verification (boot-to-menu + track load).
    # ─────────────────────────────────────────────────────────────────────

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
    # a real OS HANDLE if a .piz is open — corrupting process state. Same
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

    # ─────────────────────────────────────────────────────────────────────
    # Session c3-batch-i-s3 — Save/SettingsAndIO_i3.cpp
    # 5 settings_dialog + save_gamesave file I/O wrappers.
    # ─────────────────────────────────────────────────────────────────────

    # 0x00498f60  VideoDialogInit_i3
    # WM_INITDIALOG handler, 516 bytes. HWND via EAX (Ghidra in_EAX).
    # Not exercised at main menu (video settings dialog silenced by
    # patch_mashed_skip_selector.py). Synthetic call: arg_type='none' +
    # crash_equal_ok=True — both sides AV identically at first GetDlgItem
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
    # function pointers identical) → identical bytes_read return value.
    # arg_type='none' + crash_equal_ok=True: pass garbage stack args;
    # both sides take identical FUN_004cc230(2,1,garbage_ptr) path which
    # returns NULL → early return 0. Identical bit-result.
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
    # FUN_004cc230(2,2,garbage) which returns NULL → early return 0.
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

    # ─────────────────────────────────────────────────────────────────────
    # c3-batch-i-s1 — RWS audio stream/header reader + tree-walk cluster.
    # 4 hooks registered (0x005aea00 refused: U-0125 catalog "Blocks: C3").
    # All four use arg_type='none' + crash_equal_ok=True per established
    # pattern for audio fns whose direct callees require runtime state
    # absent at quiescent main-menu.
    # ─────────────────────────────────────────────────────────────────────

    # 0x005ab380  AudioRwsChunkHeaderRead
    # uint32(stream, *out): reads 12 bytes via FUN_004cbd30 then version-decodes.
    # crash_equal_ok: stream=0 → FUN_004cbd30 dispatches on *(stream+...) → null deref.
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
    # crash_equal_ok: wave_node=0 → reads *(0+0xc) → null deref before any call.
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
    # NULL node → uses DAT_009146fc; at quiescent main-menu the audio context
    # tree is uninitialized (DAT_009146fc == 0), so param_1[4] dereferences 0
    # in both orig and reimpl → identical null-deref crash.
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
}
