#!/usr/bin/env python3
"""
rws_inspect.py - Inventory RenderWare stream (.rws) chunk structure.

Usage: rws_inspect.py <path-to-rws-file>
Output: CSV rows to stdout, one per chunk:
        file,depth,section_id_hex,size,version,parent_section_id

Chunk header layout (little-endian):
  uint32 section_id
  uint32 size         (payload bytes, excludes this 12-byte header)
  uint32 version      (libraryID field)

Container detection strategy:
  1. IDs confirmed as complex containers in babinary.c
     (criterion-rwg37/core/src/plcore/babinary.c, ChunkIsComplex):
       0x00000005 rwID_CAMERA, 0x00000006 rwID_TEXTURE, 0x00000007 rwID_MATERIAL,
       0x00000008 rwID_MATLIST, 0x00000009 rwID_ATOMICSECT, 0x0000000A rwID_PLANESECT,
       0x0000000B rwID_WORLD, 0x0000000E rwID_FRAMELIST, 0x0000000F rwID_GEOMETRY,
       0x00000010 rwID_CLUMP, 0x00000012 rwID_LIGHT, 0x00000014 rwID_ATOMIC,
       0x0000001A rwID_GEOMETRYLIST.
     Also 0x00000003 rwID_EXTENSION (per session prompt container-type list).
  2. IDs confirmed as leaf (NOT recursed) in babinary.c:
       0x00000001 rwID_STRUCT, 0x00000002 rwID_STRING,
       0x0000000D rwID_MATRIX, 0x00000013 rwID_UNICODESTRING.
  3. Unknown IDs: validated heuristically — payload must parse as a clean,
     non-overlapping sequence of chunk headers (all sizes fit). Only then
     recurse. This discovers audio-specific container types without guessing.
"""

import struct
import sys
import os

HEADER_SIZE = 12

# Confirmed containers from babinary.c ChunkIsComplex() returning TRUE
# Source: criterion-rwg37/core/src/plcore/babinary.c lines 113-232
KNOWN_CONTAINERS = frozenset({
    0x00000003,  # rwID_EXTENSION  (session prompt list)
    0x00000005,  # rwID_CAMERA
    0x00000006,  # rwID_TEXTURE
    0x00000007,  # rwID_MATERIAL
    0x00000008,  # rwID_MATLIST
    0x00000009,  # rwID_ATOMICSECT
    0x0000000A,  # rwID_PLANESECT
    0x0000000B,  # rwID_WORLD
    0x0000000E,  # rwID_FRAMELIST
    0x0000000F,  # rwID_GEOMETRY
    0x00000010,  # rwID_CLUMP
    0x00000012,  # rwID_LIGHT
    0x00000014,  # rwID_ATOMIC
    0x0000001A,  # rwID_GEOMETRYLIST
})

# Confirmed leaves from babinary.c ChunkIsComplex() returning FALSE
# Source: criterion-rwg37/core/src/plcore/babinary.c lines 113-232
KNOWN_LEAVES = frozenset({
    0x00000001,  # rwID_STRUCT
    0x00000002,  # rwID_STRING
    0x0000000D,  # rwID_MATRIX
    0x00000013,  # rwID_UNICODESTRING
})


def _validate_as_container(data: bytes) -> bool:
    """
    Returns True if `data` parses as a clean sequence of RW chunk headers
    with no gaps, no overruns, and at least one sub-chunk present.
    This is the heuristic used for unknown section IDs.
    """
    if len(data) < HEADER_SIZE:
        return False
    offset = 0
    count = 0
    while offset + HEADER_SIZE <= len(data):
        sec_id, size, _ver = struct.unpack_from('<III', data, offset)
        if sec_id == 0:
            return False
        payload_end = offset + HEADER_SIZE + size
        if payload_end > len(data):
            return False
        offset = payload_end
        count += 1
    return count > 0 and offset == len(data)


def walk_chunks(data: bytes, file_label: str, depth: int,
                parent_id: int, rows: list) -> None:
    offset = 0
    while offset < len(data):
        remaining = len(data) - offset
        if remaining < HEADER_SIZE:
            rows.append({
                'file': file_label,
                'depth': depth,
                'section_id': 'ERROR',
                'size': remaining,
                'version': '',
                'parent_id': f'0x{parent_id:08X}' if isinstance(parent_id, int) else parent_id,
            })
            return

        sec_id, size, ver = struct.unpack_from('<III', data, offset)
        parent_hex = f'0x{parent_id:08X}' if isinstance(parent_id, int) else parent_id

        payload_end = offset + HEADER_SIZE + size
        if payload_end > len(data):
            rows.append({
                'file': file_label,
                'depth': depth,
                'section_id': 'ERROR',
                'size': len(data) - offset - HEADER_SIZE,
                'version': f'0x{ver:08X}',
                'parent_id': parent_hex,
            })
            return

        rows.append({
            'file': file_label,
            'depth': depth,
            'section_id': f'0x{sec_id:08X}',
            'size': size,
            'version': f'0x{ver:08X}',
            'parent_id': parent_hex,
        })

        payload = data[offset + HEADER_SIZE: payload_end]

        if sec_id in KNOWN_CONTAINERS:
            walk_chunks(payload, file_label, depth + 1, sec_id, rows)
        elif sec_id not in KNOWN_LEAVES and size >= HEADER_SIZE:
            if _validate_as_container(payload):
                walk_chunks(payload, file_label, depth + 1, sec_id, rows)

        offset = payload_end


def inspect_file(path: str) -> list:
    label = os.path.relpath(path).replace('\\', '/')
    try:
        with open(path, 'rb') as fh:
            data = fh.read()
    except OSError as exc:
        return [{'file': label, 'depth': 0, 'section_id': 'ERROR',
                 'size': 0, 'version': str(exc), 'parent_id': 'none'}]
    rows = []
    walk_chunks(data, label, 0, 'none', rows)
    return rows


def main() -> None:
    if len(sys.argv) != 2:
        print('usage: rws_inspect.py <path-to-rws-file>', file=sys.stderr)
        sys.exit(1)

    rows = inspect_file(sys.argv[1])

    print('file,depth,section_id_hex,size,version,parent_section_id')
    for r in rows:
        print(f"{r['file']},{r['depth']},{r['section_id']},{r['size']},{r['version']},{r['parent_id']}")


if __name__ == '__main__':
    main()
