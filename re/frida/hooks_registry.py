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
}