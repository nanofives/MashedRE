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
}
