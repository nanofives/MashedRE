"""negctl re-earn fix, cluster fstate (2026-10-12). Loads last; overrides b03/b09 entries by hook key.

vehicle_car_state_set: b09 left idx = -1 out because the old port compared idx signed (> 0xf) and would have written
0xd04 bytes below 0x00881f90. The re-port uses the original's unsigned jb (0x0046cbe7), so idx = -1 and 0x80000000
go back in, and the "record -1" cells at 0x00881f90 - 0xd04 are observed (pre-filled) so a signed compare shows up
as a store there.

split_screen_track_assignment: adds an out-of-range mode 0xffffffff (0x00430835 add eax,-2 ; cmp eax,8 ; ja is
unsigned, so it returns 0) and a negative p (p*0x30 wraps, reads below the table base).

All other vectors are b03/b09's, unchanged.
"""
import importlib.util
import pathlib

_here = pathlib.Path(__file__).resolve().parent


def _load(stem):
    spec = importlib.util.spec_from_file_location('negctl_fixtures_zfs_' + stem, _here / (stem + '.py'))
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


_b09 = _load('b09')

FIXES = {}


# ── 0x0046cbe0 VehicleCarStateSet(idx, a, b) ──────────────────────────────────────────────────────────────────────────
def _cs(idx, a, b):
    return {'args': [idx & 0xFFFFFFFF, a, b], 'seed': []}


_vcs = dict(_b09.FIXES['vehicle_car_state_set'])
FIXES['vehicle_car_state_set'] = {
    'observe': list(_vcs['observe']) + [{'addr': 0x00881f90 - 0xd04, 'len': 8, 'fill': 0xA5}],
    'path1_tests': list(_vcs['path1_tests']) + [_cs(-1, 0x11111111, 0x22222222), _cs(0x80000000, 3, 4)],
}


# ── 0x00430830 SplitScreenTrackAssignment(p) ──────────────────────────────────────────────────────────────────────────
FIXES['split_screen_track_assignment'] = {
    'path1_tests': list(_b09.FIXES['split_screen_track_assignment']['path1_tests'])
                   + [_b09._ss(0xFFFFFFFF, 1), _b09._ss(2, -1), _b09._ss(9, 9)],
}


# ── 0x00430290 Championship::Complete ─────────────────────────────────────────────────────────────────────────────────
# b03's nine vectors never reach id 0x264 (n == 2), 0x267 (n == 9), 0x269 (gold n == 10 with [0x7f0c84] != 2), the
# 0x004303c0 `p < 0x7f0c54` bound of the mode-3 next-row walk, or the mode-4 [0x7f0c54] == 2 skip. Cell layout as in
# the re-port comment (Save/CareerEvents.cpp): 0x007f0a40 + row*0x30 + col.
_b03 = _load('b03')


def _col(col0, rows, val=2, extra=None):
    cells = {col0 + r * 0x30 for r in rows}
    extra = extra or {}
    return lambda a: extra.get(a, val if a in cells else 0)


FIXES['championship_complete'] = {
    'path1_tests': list(_b03.FIXES['championship_complete']['path1_tests']) + [
        _b03._chp(3, 1, _col(0x007f0a44, (0, 5))),                                   # n == 2 -> 0x264
        _b03._chp(3, 0, _col(0x007f0a44, range(1, 10))),                             # n == 9 -> 0x267, c == 10 walk
        _b03._chp(3, 0, _col(0x007f0a44, range(1, 12))),                             # c == 12, row 12 hits the bound
        _b03._chp(5, 0, _col(0x007f0a54, range(1, 10))),                             # 0x266, then n == 10 -> 0x269
        _b03._chp(4, 3, _col(0x007f0a48, range(0, 10), extra={0x007f0c54: 2})),      # n == 10, [0x7f0c54] == 2 skip
    ],
}
