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
}
