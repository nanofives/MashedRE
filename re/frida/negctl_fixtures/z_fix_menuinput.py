"""negctl re-earn fix, cluster MENUINPUT (2026-10-12): 0x0042aff0 / 0x0042b180 / 0x0042b310 / 0x0042b540.

Loads last (sorted glob in hooks_registry._apply_negctl_fixtures) and overrides the b03 / b08 entries by hook key.
The b03 / b08 vectors are kept verbatim and extended with vectors that separate the defects the re-port fixed:

  * pad 7 held alone (0x0042b062 jne 0x0042b0ad / 0x0042b1f2): the original counts it as held;
  * screen type 0x13 with sub 1 (0x0042b3fe / 0x0042b62e je -> return 1);
  * the player-slot walk at 0x007f1a14 + i*0x10, i = 0..3 (0x0042b09c / 0x0042b3bf `add esi, 0x10`). The dwords
    between the four id dwords (0x007f1a18..0x007f1a50) are seeded with 0, so a 4-byte-stride walk reads them as
    pad id 0. FUN_0040e470(i) returns *(*(0x005f2770) + i*4 + 0x34) (disasm 0x0040e470): 0x005f2770 points at a
    scratch block whose +0x34.. dwords are seeded per vector (2 = skip, anything else = eligible).
"""
import importlib.util
import pathlib
import struct


def _load(stem):
    f = pathlib.Path(__file__).resolve().parent / (stem + '.py')
    spec = importlib.util.spec_from_file_location('negctl_fixtures_zfix_' + stem, f)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


_b03 = _load('b03')
_b08 = _load('b08')


def _f(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


FIXES = {}
_SCAL = 'scalars_to_scattered_globals'


def _btn(col, held, prev):
    s = []
    for k in range(8):
        s.append({'addr': 0x007f1040 + col + k * 0x4c, 'val': 1 if (held >> k) & 1 else 0, 'size': 1})
        s.append({'addr': 0x007f1500 + col + k * 0x4c, 'val': 1 if (prev >> k) & 1 else 0, 'size': 1})
    return s


# Four slot records (id at +0, the three dwords after it zeroed) plus the FUN_0040e470 table in scratch.
def _slotrecs(ids, states=(1, 1, 1, 1)):
    s = []
    for i, v in enumerate(ids):
        base = 0x007f1a14 + i * 0x10
        s.append({'addr': base, 'val': v & 0xFFFFFFFF})
        for o in (4, 8, 0xc):
            if base + o < 0x007f1a54:
                s.append({'addr': base + o, 'val': 0})
    s.append({'addr': 0x005f2770, 'val': {'scratch': 0}})
    for i, st in enumerate(states):
        s.append({'addr': {'scratch': 0x34 + i * 4}, 'val': st})
    return s


# ── 0x0042aff0 / 0x0042b180 MenuButtonDetectA / B ─────────────────────────────────────────────────────────────────────
def _det(col, tmr, held, prev, stype, t0, dt, mode=0x1000, ids=None, states=(1, 1, 1, 1)):
    s = [{'addr': 0x007f1a0c, 'val': mode}, {'addr': 0x0067e9f8, 'val': 0}, {'addr': 0x0067ed3c, 'val': stype},
         {'addr': tmr, 'val': _f(t0)}, {'addr': 0x007f1004, 'val': _f(dt)}] + _btn(col, held, prev)
    if ids is not None:
        s += _slotrecs(ids, states)
    return {'args': [], 'seed': s}


for _k, _col, _tmr in (('menu_button_detect_a', 6, 0x0067f1b0), ('menu_button_detect_b', 7, 0x0067f1b4)):
    _old = _b03.FIXES[_k]
    FIXES[_k] = dict(_old)
    FIXES[_k]['scratch_size'] = 0x80
    FIXES[_k]['scratch_fill'] = 0xA5
    FIXES[_k]['path1_tests'] = list(_old['path1_tests']) + [
        _det(_col, _tmr, 0x80, 0x80, 7, 0.3, 0.2),                                   # pad 7 alone, rearm -> 1
        _det(_col, _tmr, 0x80, 0x80, 3, 0.5, 0.1),                                   # pad 7 alone, keep timer
        _det(_col, _tmr, 0x01, 0x00, 6, 0.5, 0.1, 0, (-1, -1, -1, -1)),              # all ids -1 -> 0, clear
        _det(_col, _tmr, 0x28, 0x08, 6, 0.1, 0.1, 0, (3, -1, 5, -1), (1, 1, 2, 1)),  # id 5 skipped by state 2
        _det(_col, _tmr, 0x28, 0x08, 3, 0.1, 0.1, 0, (3, -1, 5, -1), (1, 1, 1, 1)),  # id 5 edge -> 1
        _det(_col, _tmr, 0x02, 0x00, 7, 0.35, 0.1, 0, (-1, 1, -1, -1)),             # held via record 1 -> rearm
    ]


# ── 0x0042b310 / 0x0042b540 MenuButtonDetectC / D ─────────────────────────────────────────────────────────────────────
def _detg(col, clr, held, prev, stype, sub, t0=0x1234, mode=0x1000, ids=None, states=(1, 1, 1, 1)):
    s = [{'addr': 0x007f1a0c, 'val': mode}, {'addr': 0x0067e9f8, 'val': 0}, {'addr': 0x0067ed3c, 'val': stype},
         {'addr': 0x0067ed40, 'val': sub}, {'addr': clr, 'val': t0}] + _btn(col, held, prev)
    if ids is not None:
        s += _slotrecs(ids, states)
    return {'args': [], 'seed': s}


for _k, _col, _clr, _src in (('menu_button_detect_c', 4, 0x0067f1b8, _b03),
                             ('menu_button_detect_d', 5, 0x0067f1bc, _b08)):
    _old = _src.FIXES[_k]
    FIXES[_k] = dict(_old)
    FIXES[_k]['scratch_size'] = 0x80
    FIXES[_k]['scratch_fill'] = 0xA5
    FIXES[_k]['path1_tests'] = list(_old['path1_tests']) + [
        _detg(_col, _clr, 0x01, 0x01, 0x13, 1),                                       # sub 1 -> 1
        _detg(_col, _clr, 0x80, 0x80, 0x13, 0),                                       # pad 7 alone, sub 0 -> 1
        _detg(_col, _clr, 0x01, 0x00, 3, 0, mode=0, ids=(-1, -1, -1, -1)),            # all ids -1 -> 0, clear
        _detg(_col, _clr, 0x28, 0x08, 3, 0, mode=0, ids=(3, -1, 5, -1), states=(1, 1, 2, 1)),  # id 5 skipped
        _detg(_col, _clr, 0x28, 0x08, 3, 0, mode=0, ids=(3, -1, 5, -1)),              # id 5 edge -> 1
        _detg(_col, _clr, 0x02, 0x02, 0x13, 1, mode=0, ids=(-1, 1, -1, -1)),          # held via record 1, sub 1
    ]


# ── 0x0042b770 MenuButtonDetectE ──────────────────────────────────────────────────────────────────────────────────────
# col +9 edge detect; the slot walk steps 0x10 over 4 records (0x0042b877 add esi,0x10; bound 0x0042b87b). b03's single
# slot vector (0,-1,-1,-1) reads the same under a 4-byte walk; the four-record vectors below do not.
def _dete(held, prev, mode=0x1000, ids=None, states=(1, 1, 1, 1)):
    s = [{'addr': 0x007f1a0c, 'val': mode}] + _btn(9, held, prev)
    if ids is not None:
        s += _slotrecs(ids, states)
    return {'args': [], 'seed': s}


_old = _b03.FIXES['menu_button_detect_e']
FIXES['menu_button_detect_e'] = dict(_old)
FIXES['menu_button_detect_e']['scratch_size'] = 0x80
FIXES['menu_button_detect_e']['scratch_fill'] = 0xA5
FIXES['menu_button_detect_e']['path1_tests'] = list(_old['path1_tests']) + [
    _dete(0x01, 0x00, 0, (-1, -1, -1, -1)),                          # all ids -1 -> 0
    _dete(0x28, 0x08, 0, (3, -1, 5, -1), (1, 1, 2, 1)),              # id 5 skipped by state 2 -> 0
    _dete(0x28, 0x08, 0, (3, -1, 5, -1)),                            # id 5 edge in record 2 -> 1
    _dete(0x01, 0x00, 0, (0, -1, -1, -1)),                           # id 0 edge in record 0 -> 1
    _dete(0x80, 0x00, 0, (-1, -1, -1, 7)),                           # id 7 edge in record 3 -> 1
    _dete(0x80, 0x00, 0, (-1, -1, -1, 7), (1, 1, 1, 2)),             # record 3 skipped by state 2 -> 0
]
