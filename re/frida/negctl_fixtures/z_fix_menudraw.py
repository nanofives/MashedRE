"""negctl re-earn fix, cluster MENUDRAW (2026-10-12): 0x0040b460 MenuMenusBD, 0x004282a0 MenuMenusBA,
0x0042f0c0 / 0x0042fb70 MenuSpriteDispatchA/B, 0x0042e5b0 MenuChromeShellB.

Loads last (sorted glob in hooks_registry._apply_negctl_fixtures), so every key here overrides b05/b08/b09. The
batch helpers are re-used from those files rather than copied.
"""
import importlib.util
import pathlib
import struct

_HERE = pathlib.Path(__file__).resolve().parent


def _load(stem):
    spec = importlib.util.spec_from_file_location('negctl_fixtures_zfix_' + stem, _HERE / (stem + '.py'))
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


_b05, _b08, _b09 = _load('b05'), _load('b08'), _load('b09')


def _f(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


FIXES = {}

# ── 0x0040b460 MenuMenusBD ───────────────────────────────────────────────────────────────────────────────────────────
# b05's seven vectors plus: the DAT_007f0fd0 4/7 override (FUN_00417740 per slot, then the re-read @0x0040b503), and
# keys that only sort correctly under a SIGNED compare (cmp edx,eax; jge @0x0040b4b8) and a dword stride.
_bd = _b05._bd
FIXES['menu_menus_bd'] = {
    'path1_tests': [_bd([3, 9, 1, 5], (), 0, 0), _bd([10, 20, 30, 40], (1,), 0, 0), _bd([7, 7, 7, 7], (), 0, 0),
                    _bd([-5, 2, 8, 0], (0, 3), 0, 0), _bd([1, 2, 3, 4], (), 9, 0), _bd([1, 2, 3, 4], (), 9, 1),
                    _bd([4, 3, 2, 1], (2,), 1, 0),
                    _bd([1, 2, 3, 4], (), 4, 0), _bd([9, 1, 5, 3], (), 7, 1),
                    _bd([-0x80000000, 0x7fffffff, -1, 0], (), 0, 0), _bd([0x100, 0x10000, 0x1000000, 1], (3,), 2, 0)],
}

# ── 0x004282a0 MenuMenusBA ───────────────────────────────────────────────────────────────────────────────────────────
# b08's six vectors plus viewport widths that are not powers of two, where a float-rounded intermediate differs from
# the ST0 one (the demotion case was id 40, size 8.0, width 800.0).
_ba = _b08._ba
FIXES['menu_menus_ba'] = {
    'path1_tests': [_ba(1, 24.0), _ba(5, 12.0), _ba(20, 40.0), _ba(40, 8.0, 800.0), _ba(100, 1.0), _ba(3, 16.0, 320.0),
                    _ba(7, 13.0, 800.0), _ba(40, 8.0, 1024.0), _ba(12, 9.5, 600.0), _ba(2, 30.0, 333.0),
                    _ba(64, 11.0, 1280.0), _ba(9, 7.25, 720.0)],
}

# ── 0x0042f0c0 / 0x0042fb70 MenuSpriteDispatchA/B ────────────────────────────────────────────────────────────────────
# Same observe set as b09 (VBUF 0x00898a20 pre-filled, the vertex ring, the colour record in scratch), plus
# boot_settle_s=8.0: without it every drawing vector AVs on BOTH arms (0x8 / 0x138, menu sprite dictionaries and the
# font load after LUT-ready), exactly as b09 menu_sprite_dispatch_c.
_menu_common = _b09._menu_common
_MENU = dict(_b09._MENU, boot_settle_s=8.0)


def _ma(mode, alpha, ea8c, c4, c8, sel40=None, sel00=None, sel80=None, e844=0, scr=0):
    # Selected row: mode 0 -> dword 0x0067ed40 + scr*0x40 (0x0042f0ff); mode != 0 -> 0x0067ed80 + (scr - 2*(e844==0))*0x40
    # (0x0042f110/0x0042f116). sel00 seeds the biased slot (0x0067ed80 - 0x80 at scr 0), sel80 the unbiased one.
    s = [{'addr': 0x0067e7b0, 'val': mode & 0xFFFFFFFF}, {'addr': 0x0067e7b4, 'val': alpha & 0xFFFFFFFF},
         {'addr': 0x0067e844, 'val': e844}, {'addr': 0x0067ea8c, 'val': ea8c},
         {'addr': 0x005f76c4, 'val': c4 & 0xFFFFFFFF}, {'addr': 0x005f76c8, 'val': c8 & 0xFFFFFFFF},
         {'addr': 0x0067e9f8, 'val': scr}]
    if sel40 is not None:
        s.append({'addr': 0x0067ed40 + scr * 0x40, 'val': sel40 & 0xFFFFFFFF})
    if sel00 is not None:
        s.append({'addr': 0x0067ed80 + (scr - 2) * 0x40, 'val': sel00 & 0xFFFFFFFF})
    if sel80 is not None:
        s.append({'addr': 0x0067ed80 + scr * 0x40, 'val': sel80 & 0xFFFFFFFF})
    return _menu_common(s)


FIXES['menu_sprite_dispatch_a'] = dict(
    path1_tests=[_ma(0, 0x80, 2, 1, 0, sel40=1, sel00=9, sel80=9),
                 _ma(0, 0x10, 0, 0, 0, sel40=0),                          # guard: render states only
                 _ma(1, 0x40, 1, 0, 1, sel40=9, sel00=2, sel80=0),          # e844 == 0: biased slot -> row 2
                 _ma(0, 0xff, 3, 5, 1, sel40=2, sel00=0, sel80=0),          # steering 5 -> 0x61 (not 1)
                 _ma(1, 0xc0, 0, 1, 1, sel40=9, sel00=1, sel80=0, e844=1),  # e844 != 0: unbiased slot -> row 0
                 _ma(0, 0x160, 1, 1, 7, sel40=0, sel00=9, sel80=9),         # dword alpha 0x160, byte 0x60; camera 7
                 _ma(0, -5, 0, 0, 0, sel40=1),                              # signed guard: -5 < 0x60
                 _ma(3, 0x20, 2, 0, 1, sel40=9, sel00=9, sel80=5, e844=1),  # mode 3 skips the guard; no row selected
                 _ma(1, 0x90, 1, 1, 0, sel40=9, sel00=9, sel80=1, e844=1, scr=1)],
    path2_tests=[_ma(0, 0x80, 2, 1, 0, sel40=1, sel00=9, sel80=9)], **_MENU)


def _mb(mode, alpha, ea84, ea78, ea80, sel40=None, sel80=None, scr=0):
    # Descriptor: mode 0 -> 0x0067ed38 + scr*0x40 (0x0042fb91), else 0x0067ed78 + scr*0x40 (0x0042fba2). They point at
    # DIFFERENT FUN_0042ac00 lists here (2 groups -> y 225, 3 groups -> y 210 via FUN_0042ac50) so a swapped table
    # shows. Selected row: dword 0x0067ed40 (mode 0) / 0x0067ed80 (else) + scr*0x40, compared with i (0x0042fc05/15).
    s = [{'addr': 0x0067e7e8, 'val': mode & 0xFFFFFFFF}, {'addr': 0x0067e7ec, 'val': alpha & 0xFFFFFFFF},
         {'addr': 0x0067ea84, 'val': ea84}, {'addr': 0x0067ea78, 'val': ea78 & 0xFFFFFFFF},
         {'addr': 0x0067ea80, 'val': ea80 & 0xFFFFFFFF}, {'addr': 0x0067e9f8, 'val': scr},
         {'addr': 0x0067ed38 + scr * 0x40, 'val': {'scratch': 0x100}},
         {'addr': 0x0067ed78 + scr * 0x40, 'val': {'scratch': 0x140}}]
    s += [{'addr': {'scratch': 0x140 + i * 4}, 'val': v}
          for i, v in enumerate([1, 0xFF060000, 2, 0xFF060000, 3, 0xFF060000, 4, 0xFF070000])]
    if sel40 is not None:
        s.append({'addr': 0x0067ed40 + scr * 0x40, 'val': sel40 & 0xFFFFFFFF})
    if sel80 is not None:
        s.append({'addr': 0x0067ed80 + scr * 0x40, 'val': sel80 & 0xFFFFFFFF})
    return _menu_common(s)


# Track (row 0) stays in 0..3 in every vector: outside it the original draws esi = an unwritten stack slot
# ([esp+0x18] @0x0042fbe6), which no harness can make equal across the two arms.
FIXES['menu_sprite_dispatch_b'] = dict(
    path1_tests=[_mb(0, 0x80, 0, 0, 1, sel40=1, sel80=9),
                 _mb(0, 0x10, 0, 0, 0, sel40=0, sel80=0),                   # guard: returns before any render state
                 _mb(1, 0x20, 3, 1, 2, sel40=9, sel80=2),
                 _mb(0, 0xff, 2, 0, 0, sel40=0, sel80=9),
                 _mb(1, 0x70, 1, 7, 3, sel40=9, sel80=1),                   # music 3 keeps row 1's id; players 7 -> 0x5a
                 _mb(0, 0x1a0, 0, 0, 2, sel40=2, sel80=0),                  # byte alpha 0xa0
                 _mb(0, -5, 0, 0, 0, sel40=0),                              # signed guard
                 _mb(1, 0x50, 2, 1, 0, sel40=9, sel80=0, scr=1)],
    path2_tests=[_mb(0, 0x80, 0, 0, 1, sel40=1, sel80=9)], **_MENU)

# ── 0x0042e5b0 MenuChromeShellB ──────────────────────────────────────────────────────────────────────────────────────
# b08's six vectors (phase 0x1e0 / 0xe0 / 60 / 300 / 498 / 0) plus mid-ramp phases on every arm of the alpha table
# (0xe8, 0xf6, 0x1e5) and slides that only come out right with fild/fadd in one rounding (0x01000001 + 512.0) and a
# sign test on the subtraction (jns @0x0042e848: 0x0000000f -> -1 -> 0, 0x80000005 -> wraps positive -> 0x200).
_chb = _b08._chb
FIXES['menu_chrome_shell_b'] = {
    'path1_tests': [_chb(8.0, 4, 6, 0x100), _chb(224.5 / 60.0, 4, 6, 0x08), _chb(1.0, 0, 3, 0x300),
                    _chb(5.0, 10, 10, 0x40), _chb(8.3, 1, 2, 0x200), _chb(0.0, 0, 0, 0),
                    _chb((0xe8 + 0.5) / 60.0, 7, 9, 0x01000001), _chb((0xf6 + 0.25) / 60.0, 3, 12, 0x0000000f),
                    _chb((0x1e5 + 0.75) / 60.0, 14, 5, 0x80000005), _chb((0x200 + 0x1fa) / 60.0, 2, 1, 0x7fffffff)],
}
