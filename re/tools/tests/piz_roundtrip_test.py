"""Round-trip test: extract every .piz under original/TOASTART/ and verify the
re-packed archive is byte-identical to the original.

Usage from repo root:

    py -3.12 re/tools/tests/piz_roundtrip_test.py
        # exits 0 if every archive round-trips byte-identical, 1 otherwise.

    py -3.12 re/tools/tests/piz_roundtrip_test.py --keep-temp
        # leaves the temporary extract/repack tree in re/tools/tests/.tmp/.

    py -3.12 re/tools/tests/piz_roundtrip_test.py path/to/archive.piz
        # round-trips just that archive.

The test imports piz_extract from a sibling directory (re/tools/), so it must
be run from a checkout that has both files in place.
"""
import argparse
import hashlib
import shutil
import sys
import tempfile
from pathlib import Path

THIS_DIR = Path(__file__).resolve().parent
TOOLS_DIR = THIS_DIR.parent
REPO_ROOT = TOOLS_DIR.parent.parent
sys.path.insert(0, str(TOOLS_DIR))

import piz_extract  # noqa: E402


def sha256(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def find_default_archives() -> list[Path]:
    """All `.piz` files under `original/TOASTART/`, recursively, sorted.

    If `original/` doesn't live under this worktree (it's gitignored), fall
    back to the top-level checkout at `../../../original/` relative to the
    worktree, or to `C:/Users/maria/Desktop/Proyectos/Mashed/original/` as a
    final hardcoded fallback (the documented project root).
    """
    candidates = [
        REPO_ROOT / "original" / "TOASTART",
        REPO_ROOT.parent.parent / "original" / "TOASTART",
        Path("C:/Users/maria/Desktop/Proyectos/Mashed/original/TOASTART"),
    ]
    for toastart in candidates:
        if toastart.exists():
            return sorted(p for p in toastart.rglob("*.piz") if p.is_file())
    return []


def round_trip_one(archive: Path, work_root: Path) -> tuple[bool, str]:
    """Returns (ok, diagnosis).  ok=True iff repacked archive byte-identical."""
    name = archive.stem
    extract_dir = work_root / name
    repack_path = work_root / f"{name}.repack.piz"

    if extract_dir.exists():
        shutil.rmtree(extract_dir)
    if repack_path.exists():
        repack_path.unlink()

    # 1. extract (writes _piz_manifest.json)
    rc = piz_extract.extract_archive(archive, extract_dir, filter_glob=None)
    if rc != 0:
        return False, f"extract returned {rc}"

    # 2. pack
    rc = piz_extract.pack_archive(extract_dir, repack_path)
    if rc != 0:
        return False, f"pack returned {rc}"

    # 3. byte compare
    orig_size = archive.stat().st_size
    new_size = repack_path.stat().st_size
    if orig_size != new_size:
        return False, f"size differs: orig={orig_size} repack={new_size}"

    orig_h = sha256(archive)
    new_h = sha256(repack_path)
    if orig_h != new_h:
        # Find first differing byte
        with archive.open("rb") as a, repack_path.open("rb") as b:
            pos = 0
            while True:
                ab = a.read(4096)
                bb = b.read(4096)
                if not ab and not bb:
                    break
                for i in range(min(len(ab), len(bb))):
                    if ab[i] != bb[i]:
                        return False, (
                            f"first-byte-diff at 0x{pos + i:x}: "
                            f"orig={ab[i]:02x} repack={bb[i]:02x} "
                            f"(orig_sha256={orig_h[:16]}, repack_sha256={new_h[:16]})"
                        )
                pos += len(ab)
        return False, f"size equal but sha256 differs ({orig_h} vs {new_h})"

    return True, f"OK ({orig_size} bytes, sha256={orig_h[:16]})"


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument(
        "archives",
        nargs="*",
        type=Path,
        help="explicit list of .piz files; defaults to original/TOASTART/**/*.piz",
    )
    p.add_argument(
        "--keep-temp",
        action="store_true",
        help="leave the temporary extract/repack tree in re/tools/tests/.tmp/",
    )
    args = p.parse_args(argv)

    archives = args.archives or find_default_archives()
    if not archives:
        print("no .piz archives found under original/TOASTART/", file=sys.stderr)
        return 2

    if args.keep_temp:
        work_root = THIS_DIR / ".tmp"
        work_root.mkdir(exist_ok=True)
        tmp_ctx = None
    else:
        tmp_ctx = tempfile.TemporaryDirectory()
        work_root = Path(tmp_ctx.name)

    failures = []
    try:
        for archive in archives:
            ok, diagnosis = round_trip_one(archive, work_root)
            status = "PASS" if ok else "FAIL"
            print(f"[{status}] {archive.name}: {diagnosis}")
            if not ok:
                failures.append((archive.name, diagnosis))
    finally:
        if tmp_ctx is not None:
            tmp_ctx.cleanup()

    print()
    print(f"{len(archives) - len(failures)}/{len(archives)} archives round-trip byte-identical")
    if failures:
        print("FAILURES:")
        for name, diag in failures:
            print(f"  {name}: {diag}")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
