"""patch window.py: before every step re-hash the shared .asi; if it is not the one this window deployed (a foreign session's window overwrote it in a gap
between our MASHED.exe processes), log CONTAMINATED and re-deploy ours before running the step, and log the hash again after. Idempotent."""
import pathlib
p = pathlib.Path(__file__).resolve().parent / "window.py"
t = p.read_text(encoding="utf-8")
if "CONTAMINATED" in t:
    print("already patched")
else:
    old = "        for s in steps:\n            env = dict(os.environ)\n"
    new = ("        deployed_hash = sha(SHARED)\n"
           "        for s in steps:\n"
           "            cur = sha(SHARED)\n"
           "            if cur != deployed_hash:\n"
           "                say(f\"CONTAMINATED before {s['name']}: shared asi sha256 {cur[:16]} != deployed {deployed_hash[:16]}; re-deploying ours\")\n"
           "                try:\n"
           "                    shutil.copy2(build, SHARED)\n"
           "                except OSError as e:\n"
           "                    say(f\"   re-deploy failed: {e}\")\n"
           "            env = dict(os.environ)\n")
    assert t.count(old) == 1
    t = t.replace(old, new)
    old2 = "                say(f\"{s['name']:42s} rc={p.returncode} ({time.time()-t1:.0f}s)\")\n"
    new2 = ("                say(f\"{s['name']:42s} rc={p.returncode} ({time.time()-t1:.0f}s) shared_asi_after={sha(SHARED)[:16]}\")\n")
    assert t.count(old2) == 1
    t = t.replace(old2, new2)
    p.write_text(t, encoding="utf-8")
    print("patched")
