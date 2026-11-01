"""Run the build's lints on a clean `git archive` of a ref, the tree the LAN release builds from.

Why (HB-LINT-408A50, 2026-11-01): the merge train linted 0e3d71a75 in a full worktree and passed,
while scripts/lan_release/publish.sh, which builds from `git archive <ref> <paths>`, failed the same
commit with NEW DUP-IN-TARGET 0x00408a50. The archive left out re/match/, so the scanner could not
follow MatchS01.cpp's `#include "../../../../re/match/s01/00407a60.c"` and lost one of the three
bodies. The train only ever saw the full tree, so it could not catch a gap in the archive.

This check builds exactly the publish tree (paths from scripts/lan_release/archive_paths.txt AT THE
REF, the same file publish.sh reads) and runs, inside it:
  1. include-closure: every quoted `#include` under mashedmod/src that leaves mashedmod/ resolves
     inside the archive (the compiler would fail on it later, after the lint has already lied);
  2. scripts/lint_rva_bodies.py   (mashedmod/build.bat:35)
  3. scripts/lint_inline_asm_abs.py (mashedmod/build.bat:48)

    py -3.12 scripts/lan_release/archive_lint.py [--ref <ref>] [--keep]

Exit 0 = the clean-archive build will pass its lints. Default ref HEAD of the current checkout.
"""
from __future__ import annotations

import argparse
import io
import pathlib
import re
import shutil
import subprocess
import sys
import tarfile
import tempfile

HERE = pathlib.Path(__file__).resolve().parent
PATHS_REL = "scripts/lan_release/archive_paths.txt"
RE_QINC = re.compile(r'^\s*#\s*include\s+"([^"]+)"')
SRC_EXT = {".cpp", ".c", ".h", ".hpp", ".inc"}


def git(*args, cwd, binary=False):
    r = subprocess.run(["git", *args], cwd=cwd, capture_output=True)
    if r.returncode != 0:
        raise SystemExit(f"[archive-lint] git {' '.join(args)} failed: "
                         f"{r.stderr.decode(errors='replace').strip()}")
    return r.stdout if binary else r.stdout.decode("utf-8", errors="replace")


def parse_paths(text: str) -> list[str]:
    out = []
    for ln in text.splitlines():
        ln = ln.split("#", 1)[0].strip()
        if ln:
            out.append(ln)
    return out


def include_closure(tree: pathlib.Path) -> list[str]:
    """Quoted includes under mashedmod/src that leave mashedmod/ and do not exist in `tree`."""
    mm = (tree / "mashedmod").resolve()
    todo = [p for p in (tree / "mashedmod" / "src").rglob("*") if p.suffix.lower() in SRC_EXT]
    seen, bad = set(), []
    while todo:
        p = todo.pop()
        if p in seen:
            continue
        seen.add(p)
        for n, ln in enumerate(p.read_text(encoding="utf-8", errors="replace").splitlines(), 1):
            m = RE_QINC.match(ln)
            if not m:
                continue
            tgt = (p.parent / m.group(1)).resolve()
            if tgt.is_relative_to(mm):
                continue          # in-tree; resolution through -I paths is the compiler's job
            rel = p.resolve().relative_to(tree.resolve()).as_posix()
            if not tgt.is_file():
                bad.append(f'{rel}:{n}  "{m.group(1)}"')
            elif tgt.suffix.lower() in SRC_EXT:
                todo.append(tgt)
    return sorted(bad)


def run_lint(tree: pathlib.Path, script: str) -> tuple[bool, str]:
    r = subprocess.run([sys.executable, str(tree / "scripts" / script)], cwd=tree,
                       capture_output=True, text=True, errors="replace")
    out = (r.stdout + r.stderr).rstrip()
    for ln in out.splitlines():
        if "WARN (allowlisted)" not in ln:
            print(f"    {ln}")
    return r.returncode == 0, out


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--ref", default="HEAD")
    ap.add_argument("--keep", action="store_true", help="keep the extracted tree and print its path")
    ap.add_argument("--paths", help="comma list overriding archive_paths.txt (controls only)")
    a = ap.parse_args(argv)

    repo = pathlib.Path(git("rev-parse", "--show-toplevel", cwd=HERE).strip())
    sha = git("rev-parse", "--verify", f"{a.ref}^{{commit}}", cwd=repo).strip()
    if a.paths:
        paths, src = [x.strip() for x in a.paths.split(",") if x.strip()], "--paths"
    else:
        try:
            paths = parse_paths(git("show", f"{sha}:{PATHS_REL}", cwd=repo))
            src = f"{sha[:9]}:{PATHS_REL}"
        except SystemExit:
            paths = parse_paths((HERE / "archive_paths.txt").read_text(encoding="utf-8"))
            src = f"{HERE / 'archive_paths.txt'} (the ref has no {PATHS_REL})"
    print(f"[archive-lint] ref {a.ref} = {sha[:9]}  paths {paths}  from {src}")

    tmp = pathlib.Path(tempfile.mkdtemp(prefix="archive_lint_"))
    try:
        blob = git("archive", "--format=tar", sha, *paths, cwd=repo, binary=True)
        with tarfile.open(fileobj=io.BytesIO(blob)) as tf:
            tf.extractall(tmp, filter="data")

        steps = 3
        fails = 0
        bad = include_closure(tmp)
        for b in bad:
            print(f"    MISSING {b}")
        ok = not bad
        fails += not ok
        print(f"[1/{steps}] include-closure ({len(bad)} unresolved) {'PASS' if ok else 'FAIL'}")
        for i, script in enumerate(("lint_rva_bodies.py", "lint_inline_asm_abs.py"), 2):
            ok, _ = run_lint(tmp, script)
            fails += not ok
            print(f"[{i}/{steps}] {script} {'PASS' if ok else 'FAIL'}")
        verdict = "OK" if fails == 0 else f"FAILED ({fails} step(s))"
        print(f"[archive-lint] {verdict} -- clean git archive of {sha[:9]}")
        return 0 if fails == 0 else 1
    finally:
        if a.keep:
            print(f"[archive-lint] kept {tmp}")
        else:
            shutil.rmtree(tmp, ignore_errors=True)


if __name__ == "__main__":
    raise SystemExit(main())
