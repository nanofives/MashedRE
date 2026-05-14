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
}
