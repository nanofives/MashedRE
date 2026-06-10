# Locate the per-node camera-angle data source: search every file inside
# every TRACKS/*.piz for the azimuth sequence observed live in the override
# table DAT_0063a5f0 (camera_probe 2026-06-10): elev 15.0 paired with
# azimuths 180,157,135,112,67,22 (f32), tolerant of interleaving stride.
#
# Usage: py -3.12 re/tools/find_cam_angles.py
import struct, sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
ROOT = Path(__file__).resolve().parent.parent.parent
TRACKS = ROOT / "original" / "TOASTART" / "TRACKS"

NEEDLES = [157.0, 135.0, 112.0, 67.0, 22.0]


ENCODINGS = [("f32", "<f", 4), ("f64", "<d", 8), ("u32", "<I", 4),
             ("u16", "<H", 2), ("u8", "<B", 1), ("i16", "<h", 2)]


def scan_blob(name, blob):
    found = False
    for ename, fmt, sz in ENCODINGS:
        try:
            pat = struct.pack(fmt, NEEDLES[0] if "f" in ename else int(NEEDLES[0]))
        except struct.error:
            continue
        i = blob.find(pat)
        while i != -1:
            for stride in range(sz, max(sz * 2, 36) + 1):
                ok = True
                for k, v in enumerate(NEEDLES[1:], start=1):
                    j = i + k * stride
                    want = v if "f" in ename else int(v)
                    if j + sz > len(blob):
                        ok = False
                        break
                    got = struct.unpack_from(fmt, blob, j)[0]
                    if got != want:
                        ok = False
                        break
                if ok:
                    print(f"  {name}: {ename} offset 0x{i:x} stride {stride}")
                    found = True
            i = blob.find(pat, i + 1)
    return found


def main():
    from piz_extract import iter_entries  # reuse parser if exposed
    return 0


if __name__ == "__main__":
    # piz_extract may not expose a library API; do raw extraction per archive
    import subprocess, tempfile, os
    found_any = False
    exe = ROOT / "original" / "MASHED.exe.unpatched"
    if scan_blob("MASHED.exe", exe.read_bytes()):
        found_any = True
    for piz in sorted(TRACKS.glob("*.piz")):
        with tempfile.TemporaryDirectory() as td:
            subprocess.run([sys.executable, str(ROOT / "re/tools/piz_extract.py"),
                            "extract", str(piz), "-o", td],
                           capture_output=True)
            for f in Path(td).rglob("*"):
                if f.is_file():
                    if scan_blob(f"{piz.stem}/{f.name}", f.read_bytes()):
                        found_any = True
    print("done", "FOUND" if found_any else "— sequence not found in any track file")
