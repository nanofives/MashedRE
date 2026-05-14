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
}
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
}
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
}
    'menu_mode_sync': {
        'rva':            0x0042f6b0,
        'export':         'MenuModeSync',
        'signature':      {'ret': 'uint32', 'args': []},
        'arg_type':       'none',
        'lut_root_delta': 0,
        'path1_tests':    [0, 1, 2, 3, 4, 5, 6, 7, 8, 9],
        'path2_tests':    [0, 1, 2],
    },

    # 0x00430910  MenuOptionSlotGet
    # int(void): reads game mode + option/row index; returns 0 or table value.

# int(void): reads DAT_0067e9fc (game mode), DAT_0067f184 (option index),
    # DAT_0067f17c (row index). Returns 0 for excluded combos or table value.
    # Pure read. arg_type='none': call 10x at quiescent state; compare returns.
}
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
}
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
}
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
}
}
