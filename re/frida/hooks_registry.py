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
#                                ret:  'float' (only float supported today)
#                                args: ['float'] or ['pointer']  (vec3 ptr)
#   arg_type          str        how to pack a test vector into the call:
#                                'float_scalar' -> pass directly
#                                'vec3_ptr'     -> alloc 12B, write three
#                                                  floats, pass pointer
#   lut_root_delta    int        byte offset added to (rw_globals+rw_offset)
#                                before dereferencing the LUT root pointer
#                                (0 for sqrt family, 4 for inv-sqrt)
#   path1_tests       list       inputs for the A/B diff harness (any
#                                length). For float_scalar this is a list
#                                of floats; for vec3_ptr it's a list of
#                                3-float tuples.
#   path2_tests       list       inputs for the hook-installer harness.
#                                Same shape as path1_tests, typically
#                                smaller. Verify path doesn't compare
#                                against expected values — it just checks
#                                no-crash + interceptor count + non-null
#                                return (bit-identity is path1's job).
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
}
