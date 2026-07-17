#!/usr/bin/env python3
# re/tools/veccap/replay_offline.py — registry-driven driver for the offline
# replayer. Packs out/vectors.json into the .bin the exe reads (statics pulled
# from original/MASHED.exe ON DISK via pefile), builds the exe when stale,
# runs both modes, prints verdicts. Fully offline (no game).
import json
import struct
import subprocess
import sys
from pathlib import Path

import pefile

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]
sys.path.insert(0, str(HERE))
from veccap_registry import FUNCS, STATIC_READS, KIND_IDS, is_degenerate, n_a, has_scalar  # noqa: E402

VCVARS = r'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat'
MASHED_EXE = ROOT / 'original' / 'MASHED.exe'
IMAGE_BASE = 0x00400000


def read_statics():
    """Pull STATIC_READS bytes from the exe file on disk (they are .rdata)."""
    pe = pefile.PE(str(MASHED_EXE), fast_load=True)
    out = []
    for s in STATIC_READS:
        rva = s['addr'] - IMAGE_BASE
        out.append((s['addr'], pe.get_data(rva, s['size'])))
    return out


def pack(json_path: Path, bin_path: Path):
    d = json.loads(json_path.read_text())
    r = d['regions']
    statics = read_statics()
    threshold = struct.unpack('<f', statics[0][1])[0]

    lut = bytes.fromhex(r['lut_hex'])
    lut_inv = bytes.fromhex(r['lut_inv_hex'])
    assert len(lut) == len(lut_inv)

    counts = {}
    with bin_path.open('wb') as f:
        f.write(struct.pack('<8I', 0x32504356, len(d['funcs']),
                            r['rw_globals'], r['rw_offset'], r['slot_addr'],
                            r['lut_root'], r['lut_inv_root'], len(lut)))
        f.write(lut)
        f.write(lut_inv)
        f.write(struct.pack('<I', len(statics)))
        for addr, data in statics:
            f.write(struct.pack('<II', addr, len(data)))
            f.write(data)
        for name, fd in d['funcs'].items():
            cfg = FUNCS[name]
            vecs = fd['vectors']
            # record: name, kind, rva, n_in, n_out, n_a, sflag, n_vectors
            f.write(struct.pack('<32s7I', cfg['export'].encode(),
                                KIND_IDS[fd['kind']], fd['rva'],
                                fd['n_in'], fd['n_out'], n_a(cfg), has_scalar(cfg), len(vecs)))
            n_deg = 0
            for v in vecs:
                floats = [struct.unpack('<f', struct.pack('<I', b))[0] for b in v['v_bits']]
                flags = (1 if v['src'] == 'synth' else 0)
                if is_degenerate(cfg, floats, threshold):
                    flags |= 2
                    n_deg += 1
                f.write(struct.pack(f"<{fd['n_in']}I", *v['v_bits']))
                f.write(struct.pack('<I', v['ret_bits']))
                if fd['n_out']:
                    f.write(struct.pack(f"<{fd['n_out']}I", *v['out_bits']))
                f.write(struct.pack('<B', flags))
            counts[name] = (len(vecs), n_deg)
    return counts


def build(exe: Path):
    srcs = {src for cfg in FUNCS.values() for src in cfg['sources']}
    files = [str(HERE / 'replay_offline.cpp')] + \
            [str(ROOT / 'mashedmod' / 'src' / 'mashed_re' / s) for s in sorted(srcs)]
    cmd = (f'call "{VCVARS}" >nul && cl /nologo /O2 /EHsc /Fe:"{exe}" '
           + ' '.join(f'"{s}"' for s in files)
           + ' /link /BASE:0x10000 /FIXED /DYNAMICBASE:NO')
    r = subprocess.run(cmd, shell=True, capture_output=True, text=True, cwd=str(HERE / 'out'))
    if r.returncode != 0:
        sys.exit(f'BUILD FAILED:\n{r.stdout}\n{r.stderr}')
    print(f'built {exe.name}')


def main():
    json_path = HERE / 'out' / 'vectors.json'
    bin_path = HERE / 'out' / 'vectors.bin'
    exe = HERE / 'out' / 'replay_offline.exe'
    counts = pack(json_path, bin_path)
    for name, (n, deg) in counts.items():
        print(f'packed {name}: {n} vectors ({deg} degenerate-flagged)')
    deps = [HERE / 'replay_offline.cpp'] + \
           [ROOT / 'mashedmod' / 'src' / 'mashed_re' / s
            for cfg in FUNCS.values() for s in cfg['sources']]
    if not exe.exists() or exe.stat().st_mtime < max(p.stat().st_mtime for p in deps):
        build(exe)
    rc_total = 0
    for mode in ('relocated', 'faithful'):
        for attempt in range(8):
            r = subprocess.run([str(exe), str(bin_path), mode], capture_output=True, text=True)
            if r.returncode != 2:
                break
            print(f'  [{mode}] setup lottery lost (attempt {attempt + 1}), retrying...')
        print(r.stdout.rstrip())
        rc_total |= r.returncode
    return rc_total


if __name__ == '__main__':
    sys.exit(main())
