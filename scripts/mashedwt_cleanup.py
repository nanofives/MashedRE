"""Row-close cleanup of private game roots under D:\\MashedWT (HB-MASHEDWT-ROOT-CLEANUP, 2026-11-27).

A step's private root is <base>\\<step>_root[<suffix>] (WORKER_RULES.md "Game runs"), a robocopy of
original\\ (0.43-0.47 GB). Nothing removed it: 473 copies were 220 GB. diag.py wt-remove (the one
row-close point the merge train uses, merge_train.ps1 Remove-Wt) now calls cleanup_for_step().

Rules (memory mashedwt-junction-roots-check-targets-before-delete):
  * only <root>\\original is removed; the rest of the root and <step>_data stay.
  * a root whose original\\ is a junction: the link alone is removed (cmd /c rmdir), the target is not touched.
  * REFUSE when any other root has a reparse point into this root (a kept root still resolves through it).
  * REFUSE fixed roots that tools default to, roots written within MIN_AGE_DAYS, and roots named in a live
    process command line.
  * never rmdir /s or rmtree through a junction: every reparse point inside original\\ is unlinked alone first.
prep_root() is the root-prep copy with original\\hook_dual_install.log left out (45 MB log, 447 copies = 19.6 GB).
"""
import os
import shutil
import stat
import subprocess
import sys
import time

BASE = os.environ.get("MASHEDWT_BASE", r"D:\MashedWT")
FIXED = ("kfdot", "mtrain", "PORT-BATCH-TOOL", "savelink", "D5-DEPS-LOCAL", "DDKOBJ-SPIKE")
EXCLUDE_FILES = ("hook_dual_install.log",)
MIN_AGE_DAYS = 7.0


def is_reparse(p):
    try:
        return bool(os.lstat(p).st_file_attributes & stat.FILE_ATTRIBUTE_REPARSE_POINT) or os.path.islink(p)
    except (OSError, AttributeError):
        return False


def _norm(p):
    return os.path.normcase(os.path.normpath(p))


def _unlink_link(p):
    """Remove one link only, never its target."""
    if os.path.isdir(p):
        r = subprocess.run(["cmd", "/c", "rmdir", p], capture_output=True, text=True)
        return r.returncode == 0
    os.unlink(p)
    return True


def _link_target(p):
    try:
        return _norm(os.path.realpath(p))
    except OSError:
        return None


def junction_targets(base, skip_root=None):
    """{normalized target: [link paths]} for every reparse point at depth 1 inside every root (except skip_root)."""
    out = {}
    skip = _norm(skip_root) if skip_root else None
    for d in os.listdir(base):
        rd = os.path.join(base, d)
        if not os.path.isdir(rd) or is_reparse(rd) or _norm(rd) == skip:
            continue
        try:
            names = os.listdir(rd)
        except OSError:
            continue
        for n in names:
            p = os.path.join(rd, n)
            if is_reparse(p):
                t = _link_target(p)
                if t:
                    out.setdefault(t, []).append(p)
    return out


def inbound_links(base, root):
    """Links from OTHER roots that point at root or anything inside it."""
    r = _norm(root)
    hits = []
    for t, links in junction_targets(base, skip_root=root).items():
        if t == r or t.startswith(r + os.sep):
            hits += links
    return hits


def _live_cmdlines():
    try:
        r = subprocess.run(["powershell", "-NoProfile", "-Command",
                            "Get-CimInstance Win32_Process | ForEach-Object { $_.CommandLine }"],
                           capture_output=True, text=True, timeout=60)
        return r.stdout.lower()
    except Exception:
        return None


def tree_bytes(p):
    n = 0
    for dp, dn, fn in os.walk(p):
        dn[:] = [d for d in dn if not is_reparse(os.path.join(dp, d))]
        for f in fn:
            fp = os.path.join(dp, f)
            try:
                if not is_reparse(fp):
                    n += os.lstat(fp).st_size
            except OSError:
                pass
    return n


def _rm_ro(func, path, exc):
    os.chmod(path, stat.S_IWRITE)
    func(path)


def remove_original(orig):
    """Delete orig (a real dir) after unlinking every reparse point inside it alone."""
    for dp, dn, fn in os.walk(orig):
        for d in list(dn):
            p = os.path.join(dp, d)
            if is_reparse(p):
                dn.remove(d)
                if not _unlink_link(p):
                    raise RuntimeError("cannot unlink link " + p)
        for f in fn:
            p = os.path.join(dp, f)
            if is_reparse(p):
                _unlink_link(p)
    shutil.rmtree(orig, onerror=_rm_ro)


def roots_for_step(base, step):
    if not os.path.isdir(base):
        return []
    pre = step + "_root"
    return [os.path.join(base, d) for d in sorted(os.listdir(base))
            if d.startswith(pre) and os.path.isdir(os.path.join(base, d))]


def cleanup_for_step(step, base=None, min_age_days=MIN_AGE_DAYS, dry_run=False, live_text=None):
    """Returns list of (root, status, bytes_reclaimed, note). A refusal is a result, never an exception."""
    base = base or BASE
    res = []
    if any(step == f or step.startswith(f + "_") for f in FIXED):
        return [(step, "refused", 0, "fixed tool root")]
    live = live_text if live_text is not None else _live_cmdlines()
    for root in roots_for_step(base, step):
        orig = os.path.join(root, "original")
        if not os.path.lexists(orig):
            res.append((root, "skip", 0, "no original\\"))
            continue
        inb = inbound_links(base, root)
        if inb:
            res.append((root, "refused", 0, f"{len(inb)} other root(s) link into it, e.g. {inb[0]}"))
            continue
        if live is None:
            res.append((root, "refused", 0, "live-process check unavailable"))
            continue
        if _norm(root) in live:
            res.append((root, "refused", 0, "named in a live process command line"))
            continue
        age = (time.time() - os.path.getmtime(root)) / 86400
        if age < min_age_days:
            res.append((root, "refused", 0, f"root written {age:.1f} d ago (< {min_age_days})"))
            continue
        if is_reparse(orig):
            tgt = _link_target(orig)
            if not dry_run:
                _unlink_link(orig)
            res.append((root, "unlinked", 0, f"original\\ was a link to {tgt}; link removed, target untouched"))
            continue
        nbytes = tree_bytes(orig)
        if not dry_run:
            remove_original(orig)
        res.append((root, "dry" if dry_run else "removed", nbytes, "original\\ deleted"))
    return res


def prep_root(src, dst):
    """Root-prep copy: robocopy src to dst\\original without hook_dual_install.log (no /MIR, nothing deleted)."""
    cmd = ["robocopy", src, os.path.join(dst, "original"), "/E", "/NFL", "/NDL", "/NJH", "/NJS", "/NP",
           "/XF", *EXCLUDE_FILES]
    return subprocess.run(cmd).returncode < 8


if __name__ == "__main__":
    a = sys.argv[1:]
    if a[:1] == ["prep"] and len(a) == 3:
        sys.exit(0 if prep_root(a[1], a[2]) else 1)
    if a[:1] == ["clean"] and len(a) >= 2:
        for r in cleanup_for_step(a[1], dry_run="--dry-run" in a):
            print(r)
        sys.exit(0)
    print("usage: mashedwt_cleanup.py prep <original_dir> <root_dir> | clean <step> [--dry-run]")
    sys.exit(2)
