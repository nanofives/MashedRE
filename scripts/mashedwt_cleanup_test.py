"""Test for mashedwt_cleanup.py on a fake root under a temp dir (never the real D:\\MashedWT).
Run: py -3.12 scripts/mashedwt_cleanup_test.py [--real-root <step> --apply]   (prints one line per check)
Temp base is created on D: (TEMPBASE, default D:\\MashedWT_test_<pid>) so free-bytes before/after is the real volume."""
import os
import shutil
import subprocess
import sys
import tempfile
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mashedwt_cleanup as mc

FAILS = []


def check(ok, name):
    print(("PASS " if ok else "FAIL ") + name)
    if not ok:
        FAILS.append(name)


def mk_install(d, mb=8, with_log=True):
    os.makedirs(os.path.join(d, "TOASTART"), exist_ok=True)
    with open(os.path.join(d, "MASHED.exe"), "wb") as f:
        f.write(os.urandom(1 << 20))
    for i in range(mb):
        with open(os.path.join(d, "TOASTART", f"x{i}.piz"), "wb") as f:
            f.write(os.urandom(1 << 20))
    if with_log:
        with open(os.path.join(d, "hook_dual_install.log"), "wb") as f:
            f.write(os.urandom(2 << 20))


def junction(link, target):
    r = subprocess.run(["cmd", "/c", "mklink", "/J", link, target], capture_output=True, text=True)
    assert r.returncode == 0, r.stdout + r.stderr


def old(p, days=30):
    t = time.time() - days * 86400
    os.utime(p, (t, t))


def main():
    here = os.path.splitdrive(os.path.abspath(__file__))[0] + "\\"
    base = os.path.join(here, f"MashedWT_cleanup_test_{os.getpid()}")
    os.makedirs(base)
    try:
        src = os.path.join(base, "_src_original")
        mk_install(src)
        # 0. prep_root leaves the log out
        r1 = os.path.join(base, "STEPA_root")
        os.makedirs(r1)
        check(mc.prep_root(src, r1), "prep_root robocopy rc<8")
        check(os.path.exists(os.path.join(r1, "original", "MASHED.exe"))
              and not os.path.exists(os.path.join(r1, "original", "hook_dual_install.log")),
              "prep_root copies the install and leaves hook_dual_install.log out")
        # 1. plain row close: measured reclaim on the volume
        os.makedirs(os.path.join(base, "STEPA_data"))
        with open(os.path.join(base, "STEPA_data", "keep.txt"), "w") as f:
            f.write("x")
        with open(os.path.join(r1, "result.txt"), "w") as f:
            f.write("keep")
        old(r1)
        du = shutil.disk_usage(base).free
        tb = mc.tree_bytes(os.path.join(r1, "original"))
        res = mc.cleanup_for_step("STEPA", base=base, live_text="")
        after = shutil.disk_usage(base).free
        check(res and res[0][1] == "removed" and not os.path.exists(os.path.join(r1, "original")), "plain root: original\\ removed")
        check(os.path.exists(os.path.join(r1, "result.txt")) and os.path.exists(os.path.join(base, "STEPA_data", "keep.txt")),
              "root files and _data kept")
        print(f"MEASURED STEPA tree {tb/1e9:.4f} GB, free-bytes delta {(after-du)/1e9:.4f} GB (fake 9 MB install)")
        check(res[0][2] == tb and tb >= 9 << 20, "reported bytes == tree size")
        # 2. inbound junction: refuse; target and linker intact
        rt = os.path.join(base, "TGT_root")
        mk_install(os.path.join(rt, "original"), with_log=False)
        old(rt)
        rl = os.path.join(base, "LINK_root")
        os.makedirs(rl)
        junction(os.path.join(rl, "original"), os.path.join(rt, "original"))
        old(rl)
        res = mc.cleanup_for_step("TGT", base=base, live_text="")
        check(res[0][1] == "refused" and os.path.exists(os.path.join(rt, "original", "MASHED.exe")), "root with an inbound junction is refused, target intact")
        # 3. junctioned original: link removed, target untouched
        res = mc.cleanup_for_step("LINK", base=base, live_text="")
        check(res[0][1] == "unlinked" and not os.path.lexists(os.path.join(rl, "original"))
              and os.path.exists(os.path.join(rt, "original", "MASHED.exe")), "linked original\\: link removed alone, target intact")
        # 4. junction INSIDE original must not be followed
        outside = os.path.join(base, "OUTSIDE")
        os.makedirs(outside)
        with open(os.path.join(outside, "precious.txt"), "w") as f:
            f.write("p")
        ri = os.path.join(base, "INNER_root")
        mk_install(os.path.join(ri, "original"), with_log=False)
        junction(os.path.join(ri, "original", "lnk"), outside)
        old(ri)
        res = mc.cleanup_for_step("INNER", base=base, live_text="")
        check(res[0][1] == "removed" and os.path.exists(os.path.join(outside, "precious.txt")), "junction inside original\\ unlinked, not followed")
        # 5. young root, live process, fixed root
        ry = os.path.join(base, "YOUNG_root")
        mk_install(os.path.join(ry, "original"), with_log=False)
        check(mc.cleanup_for_step("YOUNG", base=base, live_text="")[0][1] == "refused", "root written < 7 d ago refused")
        rv = os.path.join(base, "LIVE_root")
        mk_install(os.path.join(rv, "original"), with_log=False)
        old(rv)
        check(mc.cleanup_for_step("LIVE", base=base, live_text=rv.lower() + "\\original\\mashed.exe")[0][1] == "refused", "root named in a live command line refused")
        rf = os.path.join(base, "kfdot_root")
        mk_install(os.path.join(rf, "original"), with_log=False)
        old(rf)
        check(mc.cleanup_for_step("kfdot", base=base, live_text="")[0][1] == "refused" and os.path.exists(os.path.join(rf, "original")), "fixed tool root refused")
        # 6. diag.py wt-remove wiring is present
        diag = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "diag.py"), encoding="utf-8").read()
        check(diag.count("_close_step_root(wt)") - 1 == 2, "diag.py wt-remove calls _close_step_root on both paths")
    finally:
        for dp, dn, fn in os.walk(base):
            for d in list(dn):
                p = os.path.join(dp, d)
                if mc.is_reparse(p):
                    dn.remove(d)
                    mc._unlink_link(p)
        shutil.rmtree(base, ignore_errors=True)
    print("RESULT", "FAIL" if FAILS else "PASS", len(FAILS))
    return 1 if FAILS else 0


if __name__ == "__main__":
    sys.exit(main())
