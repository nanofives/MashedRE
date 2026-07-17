#!/usr/bin/env python3
# re/tools/veccap/veccap_registry.py
#
# Declarative per-function registry driving the veccap lane (capture ->
# offline replay -> unicorn differ). One entry per verified/portable function.
#
# Signature kinds (implemented in all three tools):
#   f_f       — float ret, one float arg                (FastSqrt, FastInvSqrt)
#   f_ptrN    — float ret, one const float* arg, n_in floats read
#   f_out_in  — float ret, (float* out, const float* in), n floats each way;
#               compare ret bits AND out-buffer bits
#   v_out_in  — VOID ret, (float* out, const float* in), n_in read / n_out written;
#               compare ONLY the out-buffer bits (no return value). Added 2026-07-17
#               to onboard the B5e solver-island pure AABB/vector leaves whose
#               original signature is void(out, in) (e.g. FUN_00566830 perp-vector).
#
# Flags semantics on packed vectors: bit0 = synthetic, bit1 = degenerate.
# Degenerate vectors (|v| <= threshold margin) are SKIPPED by the offline
# replayer and the unicorn differ for 'degenerate_stubs' functions: those call
# the RW error stubs (0x004d7ff0/0x004d8480, C1) on that path, which is not
# executable outside a live game. Counted + printed, never silently dropped.
#
# 'sources' are TUs under mashedmod/src/mashed_re/ linked UNCHANGED into the
# offline replayer (hook framework stubbed by replay_offline.cpp).
import struct

# static .rdata constants the functions read; packed from original/MASHED.exe
# ON DISK (file offsets via pefile), so no live capture is needed for them.
STATIC_READS = [
    {'addr': 0x005D757C, 'size': 4},   # magnitude epsilon/threshold (RwV2d + RwV3dNormalize)
]

# every entry shares the RW LUT region snapshot (sqrt + inv-sqrt tables)
FUNCS = {
    'vec3_magnitude': {
        'rva': 0x004C3AC0, 'export': 'Vec3Magnitude', 'kind': 'f_ptrN', 'n_in': 3,
        'sources': ['Math/Vec3.cpp'], 'live_capture': True, 'degenerate_stubs': False,
    },
    'fast_sqrt': {
        'rva': 0x004C3B30, 'export': 'FastSqrt', 'kind': 'f_f',
        'sources': ['Math/RwSqrt.cpp'], 'live_capture': True, 'degenerate_stubs': False,
    },
    'fast_inv_sqrt': {
        'rva': 0x004C3B90, 'export': 'FastInvSqrt', 'kind': 'f_f',
        'sources': ['Math/RwSqrt.cpp'], 'live_capture': True, 'degenerate_stubs': False,
    },
    'vec2_length': {
        'rva': 0x004C3BF0, 'export': 'Vec2Length', 'kind': 'f_ptrN', 'n_in': 2,
        'sources': ['Math/RwV2d.cpp'], 'live_capture': True, 'degenerate_stubs': False,
    },
    'vec2_normalize': {
        'rva': 0x004C3C60, 'export': 'Vec2Normalize', 'kind': 'f_out_in', 'n_in': 2, 'n_out': 2,
        'sources': ['Math/RwV2d.cpp'], 'live_capture': True, 'degenerate_stubs': True,
    },
    'rwv3d_normalize': {
        'rva': 0x004C39B0, 'export': 'RwV3dNormalize', 'kind': 'f_out_in', 'n_in': 3, 'n_out': 3,
        'sources': ['Math/RwV3dNormalize.cpp'], 'live_capture': True, 'degenerate_stubs': True,
    },
    # --- B5e solver-island pure leaves (source copied from r7/b5e-solver-island;
    #     C2 there / C1 on main — veccap = per-leaf bit-identity evidence toward C3,
    #     NOT the diff-original promotion gate). ---
    'rwp_perp_vector': {
        # 0x00566830 — builds a perpendicular vector to `in` into `out`; pure
        # (no globals, no rand, no float10): zeroes the largest-|axis|, swaps/negates
        # the other two. Physics-only path -> synthetic vectors (menu never calls it).
        'rva': 0x00566830, 'export': 'FUN_00566830', 'kind': 'v_out_in', 'n_in': 3, 'n_out': 3,
        'sources': ['Collision/RwpSolverLeaves1.cpp'], 'live_capture': False, 'degenerate_stubs': False,
    },
    'rwp_aabb_merge': {
        # 0x00565ef0 — per-axis min/max merge of two 8-float AABBs (min +0..+2,
        # max +4..+6) into `out`. Pure. Two input buffers (n_a/n_b), no scalar.
        # Writes out[0,1,2,4,5,6] — index 3/7 are AABB padding, left untouched
        # (compared against the 0xcc capture sentinel; see n_out gap handling).
        'rva': 0x00565EF0, 'export': 'FUN_00565ef0', 'kind': 'v_out_2in',
        'n_a': 7, 'n_in': 14, 'n_out': 7, 'scalar': False,
        'sources': ['Collision/RwpSolverLeaves1.cpp'], 'live_capture': False, 'degenerate_stubs': False,
    },
    'rwp_aabb_span': {
        # 0x00565fa0 — AABB spanning two points (param_2/param_3) inflated by the
        # scalar param_4, into `out`. Pure. Reads in indices 0..2 of each buffer;
        # scalar is the last packed float. Writes out[0,1,2,4,5,6].
        'rva': 0x00565FA0, 'export': 'FUN_00565fa0', 'kind': 'v_out_2in',
        'n_a': 3, 'n_in': 7, 'n_out': 7, 'scalar': True,
        'sources': ['Collision/RwpSolverLeaves1.cpp'], 'live_capture': False, 'degenerate_stubs': False,
    },
}

# v_out_2in split accessors: n_a = buffer-1 floats, n_b = buffer-2 floats,
# has_scalar consumes the last packed input float as a by-value cdecl arg.
def n_a(cfg):
    return cfg.get('n_a', 0)


def has_scalar(cfg):
    return 1 if cfg.get('scalar') else 0


def n_b(cfg):
    return n_in(cfg) - n_a(cfg) - has_scalar(cfg)


KIND_IDS = {'f_f': 1, 'f_ptrN': 2, 'f_out_in': 3, 'v_out_in': 4, 'v_out_2in': 5}

SYNTH_COUNT = 512


def n_in(cfg):
    return cfg.get('n_in', 1)


def n_out(cfg):
    return cfg.get('n_out', 0)


def synth_inputs(cfg):
    """Deterministic synthetic inputs spanning exponent range (no RNG)."""
    n = n_in(cfg)
    out = []
    for i in range(SYNTH_COUNT):
        base = struct.unpack('<f', struct.pack(
            '<I', (0x3D800000 + i * 0x00123457) & 0x7F7FFFFF))[0]
        if cfg['kind'] == 'f_f':
            # scalar domain: positive (both sqrt variants are positive-domain)
            out.append([abs(base) if base != 0.0 else 1.0])
        elif cfg['kind'] == 'v_out_2in':
            # multi-buffer kinds need n distinct floats (the 3-value template below
            # only fills n<=3); vary exponent+sign per (vector, position).
            v = []
            for j in range(n):
                b = struct.unpack('<f', struct.pack(
                    '<I', (0x3D800000 + (i * 7 + j) * 0x00123457) & 0x7F7FFFFF))[0]
                v.append(b if (i + j) % 2 else -b)
            out.append(v)
        else:
            v = [base, -base * 0.5, base * 2.0 if i % 3 else 0.0][:n]
            out.append(v)
    if not cfg['degenerate_stubs']:
        out.append([0.0] * n)   # zero vector only where the zero path is stub-free
    return out


def is_degenerate(cfg, v, threshold):
    """Conservative degenerate flag (margin 4x) for stub-calling functions."""
    if not cfg['degenerate_stubs']:
        return False
    mag2 = sum(x * x for x in v)
    return mag2 <= (threshold * 4.0) ** 2
