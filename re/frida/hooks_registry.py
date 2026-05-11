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
}
